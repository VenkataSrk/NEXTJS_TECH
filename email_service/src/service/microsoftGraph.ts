import axios from 'axios';
import fs from 'fs';
import path from 'path';
import { configs } from '../config/app';
import { CHANNEL_TYPE } from '../helpers/constants';
import { cleanSubject, createClientAssertion, htmlToText, removeReplyAllContent, stripDisclaimer } from '../helpers/utils';
import { createEmailSessionGraph } from '../modules/email/handler';
import { logger } from '../plugins/log';

const tokenCache: Map<string, { token: string; expiresAt: number }> = new Map();
const tokenFetchLocks: Map<string, Promise<string>> = new Map();
const AXIOS_TIMEOUT = Number(process.env.MS_AXIOS_TIMEOUT_MS || 30000);
const subscriptionCache: Map<string, number> = new Map();
const processedMessages: Map<string, number> = new Map();
const MAX_PROCESSED_CACHE_SIZE = Number(process.env.MS_PROCESSED_CACHE_MAX || 50000);
const processingMessages: Set<string> = new Set();
const MESSAGE_CACHE_TTL = 10 * 60 * 1000;

// Per-tenant API queue to prevent rate limiting when multiple mailboxes share a tenant
const tenantApiQueues: Map<string, Promise<any>> = new Map();
async function withTenantQueue<T>(tenantId: string, fn: () => Promise<T>): Promise<T> {
  const prev = tenantApiQueues.get(tenantId) || Promise.resolve();
  const next = prev.then(fn, fn);
  tenantApiQueues.set(tenantId, next);
  try {
    return await next;
  } finally {
    if (tenantApiQueues.get(tenantId) === next) {
      tenantApiQueues.delete(tenantId);
    }
  }
}

function cleanupProcessedMessages() {
  const now = Date.now();
  for (const [msgId, timestamp] of processedMessages) {
    if (now - timestamp > MESSAGE_CACHE_TTL) {
      processedMessages.delete(msgId);
    }
  }
  if (processedMessages.size > MAX_PROCESSED_CACHE_SIZE) {
    const overflow = processedMessages.size - MAX_PROCESSED_CACHE_SIZE;
    let removed = 0;
    for (const key of processedMessages.keys()) {
      processedMessages.delete(key);
      removed += 1;
      if (removed >= overflow) break;
    }
  }
}

setInterval(cleanupProcessedMessages, 5 * 60 * 1000);

function getTokenCacheKey(msConfig: any): string {
  return `${msConfig.tenantId}:${msConfig.clientId}`;
}

function getMsAuthConfig(jsonData?: any) {
  return {
    clientId: jsonData?.clientId,
    tenantId: jsonData?.tenantId,
    baseUrl: process.env.MS_GRAPH_BASE_URL,
    privateKeyPath: jsonData?.privateKey,
    thumbprint: jsonData?.thumbprint,
  };
}

async function createMicrosoftSubscription(email: string, msConfig: any, token: string) {
  const expiration = new Date(Date.now() + 55 * 60 * 1000).toISOString();
  const notificationUrl = process.env.MS_GRAPH_WEBHOOK_URL;
  if (!notificationUrl) {
    logger.error('MS webhook URL not configured (MS_GRAPH_WEBHOOK_URL)');
    return;
  }
  const payload = {
    notificationUrl,
    changeType: 'created',
    resource: `/users/${email}/messages`,
    expirationDateTime: expiration,
    clientState: 'ccaas-secret',
  };

  await axios.post(`${msConfig.baseUrl}/subscriptions`, payload, {
    headers: {
      Authorization: `Bearer ${token}`,
      'Content-Type': 'application/json',
    },
  });
}

export async function ensureMicrosoftSubscription(user: any, jsonData?: any) {
  // Skip webhook subscriptions if disabled (useful for local dev without valid webhook URL)
  if (process.env.MS_WEBHOOK_DISABLED === 'true') {
    logger.info('⏭ MS subscription skipped (MS_WEBHOOK_DISABLED=true, using polling only)');
    return;
  }

  const notificationUrl = process.env.MS_GRAPH_WEBHOOK_URL;
  if (!notificationUrl) {
    logger.warn('⏭ MS subscription skipped (MS_GRAPH_WEBHOOK_URL not configured)');
    return;
  }

  const msConfig = getMsAuthConfig(jsonData);
  const email = user?.emailAddress;
  if (!email) return;
  const key = `${msConfig.tenantId}:${email}`;
  const lastAt = subscriptionCache.get(key) || 0;
  const now = Date.now();
  const intervalMinutes = Number(
    process.env.MS_SUBSCRIPTION_MIN_INTERVAL || 50
  );
  const intervalMs = intervalMinutes * 60 * 1000;
  if (now - lastAt < intervalMs) {
    const nextAt = new Date(lastAt + intervalMs).toISOString();
    logger.info('⏭ MS subscription skipped (too early)', {
      email,
      intervalMinutes,
      nextEligibleAt: nextAt,
      remainingMinutes: Math.ceil((lastAt + intervalMs - now) / 60000),
    });
    return;
  }
  try {
    const token = await getMicrosoftAccessToken(msConfig);
    await createMicrosoftSubscription(email, msConfig, token);
    subscriptionCache.set(key, now);
    logger.info('✅ MS subscription ensured', {
      email,
      ensuredAt: new Date(now).toISOString(),
      nextRenewAfterMinutes: intervalMinutes,
    });
  } catch (err: any) {
    // Remove from cache to allow immediate retry on next attempt
    subscriptionCache.delete(key);
    logger.error('❌ MS subscription error', {
      email,
      status: err?.response?.status,
      code: err?.response?.data?.error?.code,
      message: err?.response?.data?.error?.message || err?.message,
    });
  }
}

export async function fetchMicrosoftInbox(user: any, jsonData?: any) {
  const msConfig = getMsAuthConfig(jsonData);
  const baseUrl = msConfig.baseUrl!;
  const email = user.emailAddress;
  const maxDays = Number(process.env.MS_SYNC_MAX_DAYS || 0);
  const maxTop = Number(process.env.MS_SYNC_TOP || 25);
  const maxPages = Number(process.env.MS_SYNC_MAX_PAGES || 1);
  const batchDelayMs = Number(process.env.MS_SYNC_BATCH_DELAY_MS || 0);
  const maxMessages = Number(process.env.MS_SYNC_MAX_MESSAGES || 0);
  const concurrency = Math.max(1, Number(process.env.MS_SYNC_CONCURRENCY || 1));
  const errorBackoffMs = Number(process.env.MS_SYNC_ERROR_BACKOFF_MS || 0);
  const cutoffIso =
    maxDays > 0 ? new Date(Date.now() - maxDays * 24 * 60 * 60 * 1000).toISOString() : '';
  const filterParts = ['isRead eq false'];
  if (cutoffIso) {
    filterParts.push(`receivedDateTime ge ${cutoffIso}`);
  }
  const inboxUrl = `${baseUrl}/users/${email}/messages` + `?$filter=${filterParts.join(' and ')}` + `&$top=${maxTop}` + '&$orderby=receivedDateTime desc';
  await fetchWithTokenRetry(msConfig, async (token) => {
    let nextUrl: string | null = inboxUrl;
    let page = 0;
    let processedCount = 0;
    while (nextUrl && page < maxPages) {
      // Use tenant queue for listing API call to avoid rate limiting across shared-tenant mailboxes
      const res = await withTenantQueue(msConfig.tenantId, () => axios.get(nextUrl!, {
        headers: { Authorization: `Bearer ${token}` },
        timeout: AXIOS_TIMEOUT,
      }));
      const messages = res?.data?.value || [];
      const queue = maxMessages > 0
        ? messages.slice(0, Math.max(0, maxMessages - processedCount))
        : messages;

      const tasks = queue.map((msg: any) => async () => {
        try {
          await processGraphMessage(user, msg, msConfig);
        } catch (err: any) {
          logger.error('processGraphMessage failed', {
            email,
            messageId: msg?.internetMessageId,
            status: err?.response?.status,
            message: err?.message,
          });
          if (errorBackoffMs > 0) {
            await new Promise(resolve => setTimeout(resolve, errorBackoffMs));
          }
        } finally {
          processedCount += 1;
        }
      });

      const workers: any = [];
      for (let i = 0; i < Math.min(concurrency, tasks.length); i += 1) {
        workers.push((async () => {
          for (let j = i; j < tasks.length; j += concurrency) {
            await tasks[j]();
            if (maxMessages > 0 && processedCount >= maxMessages) {
              return;
            }
          }
        })());
      }
      await Promise.all(workers);

      if (maxMessages > 0 && processedCount >= maxMessages) {
        logger.info('Reached MS_SYNC_MAX_MESSAGES cap', { email, processedCount, maxMessages });
        return;
      }

      nextUrl = res?.data?.['@odata.nextLink'] || null;
      page += 1;
      if (batchDelayMs > 0 && nextUrl) {
        await new Promise(resolve => setTimeout(resolve, batchDelayMs));
      }
    }
  });
}

async function getMicrosoftAccessToken(msConfig: any) {
  const cacheKey = getTokenCacheKey(msConfig);
  const cached = tokenCache.get(cacheKey);
  if (cached && Date.now() < cached.expiresAt) {
    return cached.token;
  }
  const inFlight = tokenFetchLocks.get(cacheKey);
  if (inFlight) {
    return inFlight;
  }
  const tokenUrl = `https://login.microsoftonline.com/${msConfig.tenantId}/oauth2/v2.0/token`;
  const fetchPromise = (async () => {
    try {
      const params = new URLSearchParams();
      params.append('client_id', msConfig.clientId);
      params.append('scope', 'https://graph.microsoft.com/.default');
      params.append('grant_type', 'client_credentials');
      params.append('client_assertion_type', 'urn:ietf:params:oauth:client-assertion-type:jwt-bearer');
      params.append('client_assertion', createClientAssertion(msConfig));
      const res = await axios.post(tokenUrl, params, {
        headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
        timeout: AXIOS_TIMEOUT,
      });
      const expiresIn = Number(res?.data?.expires_in || 3600);
      tokenCache.set(cacheKey, {
        token: res.data.access_token,
        expiresAt: Date.now() + (expiresIn - 60) * 1000,
      });
      return res.data.access_token;
    } finally {
      tokenFetchLocks.delete(cacheKey);
    }
  })();
  tokenFetchLocks.set(cacheKey, fetchPromise);
  return fetchPromise;
}

async function processGraphMessage(user: any, msg: any, msConfig: any) {
  if (!msg?.id || !msg?.internetMessageId) return;
  const maxDays = Number(process.env.MS_SYNC_MAX_DAYS || 0);
  if (maxDays > 0 && msg?.receivedDateTime) {
    const receivedAt = new Date(msg.receivedDateTime).getTime();
    if (!Number.isNaN(receivedAt)) {
      const ageMs = Date.now() - receivedAt;
      if (ageMs > maxDays * 24 * 60 * 60 * 1000) {
        logger.info('Skipping old message beyond sync window', {
          maxDays,
          messageId: msg.internetMessageId,
          receivedDateTime: msg.receivedDateTime,
        });
        return;
      }
    }
  }
  const messageKey = `${user?.emailAddress}:${msg.internetMessageId}`;
  if (processedMessages.has(messageKey)) {
    logger.info('Skipping already processed message', { messageId: msg.internetMessageId });
    return;
  }
  if (processingMessages.has(messageKey)) {
    logger.info('Skipping message already in progress', { messageId: msg.internetMessageId });
    return;
  }
  processingMessages.add(messageKey);

  try {
    // Use tenant queue to prevent rate limiting when multiple mailboxes share same tenant
    await withTenantQueue(msConfig.tenantId, async () => {
      const attachments = msg?.hasAttachments ? await downloadGraphAttachmentsWithRetry(user?.emailAddress, msg?.id, msConfig) : [];
      const rawBody = msg.body?.contentType === 'html' ? removeReplyAllContent(msg?.body?.content) : removeReplyAllContent(msg?.body?.content || '');
      const bodyContent = stripDisclaimer(msg.body?.contentType === 'html' ? htmlToText(rawBody) : rawBody);
      const sessionData = {
        customerData: {
          attachments,
          fromData: msg?.from?.emailAddress?.address || '',
          toData: (msg?.toRecipients || []).map((r: any) => r?.emailAddress?.address).filter(Boolean).join(','),
          dateData: msg?.receivedDateTime,
          textData: bodyContent,
          subjectData: cleanSubject(msg?.subject),
          sessionStartTime: Date.now(),
          messageId: msg?.internetMessageId,
          conversationId: msg?.conversationId || null,
          conversationIndex: msg?.conversationIndex || null,
          graphMessageId: msg?.id || null,
        },
        channelType: CHANNEL_TYPE.EMAIL,
      };
      console.log('sessionData>>>>>>>', sessionData);
      await createEmailSessionGraph(sessionData);
      await markGraphReadWithRetry(user?.emailAddress, msg?.id, msConfig);
      processedMessages.set(messageKey, Date.now());
      logger.info('Message processed successfully', { messageId: msg.internetMessageId, email: user?.emailAddress });
    });
  } finally {
    processingMessages.delete(messageKey);
  }
}

async function downloadGraphAttachmentsWithRetry(userEmail: string, messageId: string, msConfig: any) {
  let result: any[] = [];
  await fetchWithTokenRetry(msConfig, async (token) => {
    result = await downloadGraphAttachments(userEmail, token, messageId, msConfig);
  });
  return result;
}

async function downloadGraphAttachments(userEmail: string, token: string, messageId: string, msConfig: any) {
  const baseUrl = msConfig.baseUrl!;
  const res = await axios.get(
    `${baseUrl}/users/${userEmail}/messages/${messageId}/attachments`,
    { headers: { Authorization: `Bearer ${token}` }, timeout: AXIOS_TIMEOUT }
  );

  const files: any[] = [];
  const maxAttachmentSize = Number(process.env.MS_MAX_ATTACHMENT_SIZE || 25 * 1024 * 1024);
  const uploadDir = configs.email_path.Email_upload_destination;

  // Verify upload directory exists or can be created
  try {
    if (!fs.existsSync(uploadDir)) {
      fs.mkdirSync(uploadDir, { recursive: true });
    }
  } catch (dirErr: any) {
    logger.error('Failed to create attachment upload directory', {
      uploadDir,
      messageId,
      userEmail,
      error: dirErr?.message,
    });
    // Return empty attachments array - don't block message processing
    return files;
  }

  for (const item of res.data?.value || []) {
    if (item['@odata.type'] !== '#microsoft.graph.fileAttachment') continue;
    if (item?.isInline) continue;

    const attachmentSize = Number(item.size || 0);
    if (attachmentSize > maxAttachmentSize) {
      logger.warn('Skipping attachment exceeding size limit', {
        maxAttachmentSize,
        messageId,
        userEmail,
        name: item.name,
        size: attachmentSize
      });
      continue;
    }

    try {
      const buffer = Buffer.from(item.contentBytes, 'base64');
      if (buffer.length > maxAttachmentSize) {
        logger.warn('Skipping attachment exceeding size limit after decode', {
          maxAttachmentSize,
          messageId,
          userEmail,
          name: item.name,
          size: buffer.length
        });
        continue;
      }
      const safeName = `${Date.now()}_${item.name}`;
      const filePath = path.join(uploadDir, safeName);
      fs.writeFileSync(filePath, buffer);
      files.push({
        href: `${configs.email_path.Email_fileGetUrl}${safeName}`,
        filename: safeName,
        size: buffer.length,
        type: item.contentType,
        fileType: item.contentType,
      });
    } catch (fileErr: any) {
      logger.error('Failed to save attachment', {
        messageId,
        userEmail,
        name: item.name,
        error: fileErr?.message,
      });
      // Continue with other attachments
    }
  }

  return files;
}

async function markGraphReadWithRetry(userEmail: string, messageId: string, msConfig: any) {
  await fetchWithTokenRetry(msConfig, async (token) => {
    await markGraphRead(userEmail, token, messageId, msConfig);
  });
}

async function markGraphRead(userEmail: string, token: string, messageId: string, msConfig: any) {
  const baseUrl = msConfig.baseUrl!;
  await axios.patch(
    `${baseUrl}/users/${userEmail}/messages/${messageId}`,
    { isRead: true },
    { headers: { Authorization: `Bearer ${token}` }, timeout: AXIOS_TIMEOUT }
  );
}

async function fetchWithTokenRetry(msConfig: any, fn: (token: any) => any, maxRetries = 3) {
  let lastError: any;
  for (let attempt = 0; attempt < maxRetries; attempt += 1) {
    try {
      const token = await getMicrosoftAccessToken(msConfig);
      await fn(token);
      return;
    } catch (err: any) {
      lastError = err;
      const status = err?.response?.status;

      // Handle 401 - invalid/expired token
      if (status === 401) {
        const cacheKey = getTokenCacheKey(msConfig);
        tokenCache.delete(cacheKey);
        // Retry with fresh token
        continue;
      }

      // Handle 429 - rate limited
      if (status === 429) {
        const retryAfter = parseInt(err?.response?.headers?.['retry-after'] || '5', 10);
        const waitMs = Math.min(retryAfter * 1000, 30000); // Max 30 seconds wait
        logger.warn('MS Graph rate limited, waiting before retry', {
          maxRetries,
          waitMs,
          attempt: attempt + 1,
          retryAfterSeconds: retryAfter,
        });
        await sleep(waitMs);
        continue;
      }

      // Handle 503/504 - service unavailable, retry with backoff
      if (status === 503 || status === 504) {
        const backoffMs = Math.min(1000 * Math.pow(2, attempt), 10000);
        logger.warn('MS Graph service unavailable, retrying with backoff', {
          maxRetries,
          backoffMs,
          status,
          attempt: attempt + 1,
        });
        await sleep(backoffMs);
        continue;
      }

      // For other errors, don't retry
      throw err;
    }
  }
  // All retries exhausted
  throw lastError;
}

export function sleep(ms: number) {
  return new Promise((resolve) => {
    setTimeout(resolve, ms);
  });
}
