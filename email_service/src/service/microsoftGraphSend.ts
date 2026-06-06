import axios from 'axios';
import crypto from 'crypto';
import fs from 'fs';
import jwt from 'jsonwebtoken';
import path from 'path';
import { configs } from '../config/app';
import { createClientAssertion, getAttachmentName, getTokenCacheKey, splitEmails } from '../helpers/utils';
import { logger } from '../plugins/log';

const tokenCache = new Map<string, { token: string; expiresAt: number }>();
const AXIOS_TIMEOUT = Number(process.env.MS_AXIOS_TIMEOUT_MS || 30000);

function resolveHrefToPath(href: string) {
  if (!href) return null;
  if (configs?.email_path?.Email_fileGetUrl && href.startsWith(configs.email_path.Email_fileGetUrl)) {
    const fileName = href.replace(configs.email_path.Email_fileGetUrl, '');
    return path.join(configs.email_path.Email_upload_destination, fileName);
  }
  return null;
}

function buildAttachmentFromContent(att: any, nameFallback: any) {
  let contentBytes = '';
  if (att?.contentBytes) {
    contentBytes = att.contentBytes;
  } else if (Buffer.isBuffer(att?.content)) {
    contentBytes = att.content.toString('base64');
  } else if (typeof att?.content === 'string') {
    if (att?.encoding === 'base64') {
      contentBytes = att.content;
    } else {
      contentBytes = Buffer.from(att.content).toString('base64');
    }
  }

  if (!contentBytes) return null;

  return {
    contentBytes,
    '@odata.type': '#microsoft.graph.fileAttachment',
    name: getAttachmentName(att, nameFallback),
    contentType: att?.contentType || att?.type || att?.fileType,
  };
}

function buildAttachmentFromPath(filePath: string, att: any, nameFallback: any) {
  if (!filePath || !fs.existsSync(filePath)) return null;
  const buffer = fs.readFileSync(filePath);
  return {
    '@odata.type': '#microsoft.graph.fileAttachment',
    name: getAttachmentName(att, nameFallback || path.basename(filePath)),
    contentType: att?.contentType || att?.type || att?.fileType,
    contentBytes: buffer.toString('base64'),
  };
}

function buildGraphAttachments(attachments: any[] = []) {
  const graphAttachments: any[] = [];
  for (const att of attachments) {
    const nameFallback = att?.filename || att?.name || att?.fileName || '';
    const fromContent = buildAttachmentFromContent(att, nameFallback);
    if (fromContent) {
      graphAttachments.push(fromContent);
      continue;
    }

    const filePath = att?.path || att?.filePath || resolveHrefToPath(att?.href);
    const fromPath = buildAttachmentFromPath(filePath, att, nameFallback);
    if (fromPath) {
      graphAttachments.push(fromPath);
      continue;
    }

    if (att?.href) {
      logger.info(`Graph send: skipping attachment (cannot read local file): ${att.href}`);
    }
  }
  return graphAttachments;
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

async function getMicrosoftAccessToken(msConfig: any): Promise<string> {
  const cacheKey = getTokenCacheKey(msConfig);
  const cached = tokenCache.get(cacheKey);
  if (cached && Date.now() < cached.expiresAt) {
    return cached.token;
  }
  const tokenUrl = `https://login.microsoftonline.com/${msConfig.tenantId}/oauth2/v2.0/token`;
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

  const expiresIn = Number(res.data.expires_in || 3600);
  tokenCache.set(cacheKey, {
    token: res.data.access_token,
    expiresAt: Date.now() + (expiresIn - 60) * 1000,
  });

  return res.data.access_token;
}

function asRecipients(emails: string[]) {
  return emails.map(a => ({
    emailAddress: { address: a }
  }));
}

function uniqueLowercase(emails: string[]) {
  const seen = new Set<string>();
  const out: string[] = [];
  for (const email of emails) {
    const normalized = `${email || ''}`.trim().toLowerCase();
    if (!normalized || seen.has(normalized)) continue;
    seen.add(normalized);
    out.push(normalized);
  }
  return out;
}

function shouldFallbackToDirectSend(err: any) {
  const status = Number(err?.response?.status || 0);
  const code = `${err?.response?.data?.error?.code || ''}`.toLowerCase();
  const message = `${err?.response?.data?.error?.message || err?.message || ''}`.toLowerCase();

  // Resource not found or gone
  if (status === 404 || status === 410) return true;

  // Forbidden - might be permission issue on the specific message
  if (status === 403) return true;

  // Server errors - fallback to direct send as alternative
  if (status === 500 || status === 502 || status === 503) return true;

  // Bad request with specific error indicators
  if (status === 400 && (
    code.includes('invalid') ||
    code.includes('notfound') ||
    code.includes('itemnotfound') ||
    code.includes('erroritemnotfound') ||
    code.includes('mailboxnotfound') ||
    message.includes('id is malformed') ||
    message.includes('resource could not be discovered') ||
    message.includes('does not exist') ||
    message.includes('cannot be found')
  )) {
    return true;
  }
  return false;
}

function sleep(ms: number): Promise<void> {
  return new Promise(resolve => setTimeout(resolve, ms));
}

async function fetchWithTokenRetry(msConfig: any, fn: (token: any) => Promise<void>, maxRetries = 3) {
  let lastError: any;
  for (let attempt = 0; attempt < maxRetries; attempt += 1) {
    try {
      const token = await getMicrosoftAccessToken(msConfig);
      await fn(token);
      return;
    } catch (err: any) {
      lastError = err;
      const status = err?.response?.status;

      // Log detailed error information from Microsoft Graph API
      if (err?.response?.data) {
        logger.error('Graph API error response:', {
          status,
          maxRetries,
          error: err?.response?.data?.error,
          message: err?.response?.data?.error?.message,
          code: err?.response?.data?.error?.code,
          attempt: attempt + 1,
        });
      }

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
        logger.warn('MS Graph send rate limited, waiting before retry', {
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
        logger.warn('MS Graph send service unavailable, retrying with backoff', {
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

export async function sendMailGraph(data: any, smtp: any, text: any) {
  const jsonData = (() => {
    try {
      return JSON.parse(smtp?.smtp_json || '{}');
    } catch (error) {
      logger.error('Invalid smtp_json:', error);
      return {};
    }
  })();

  /* tslint:disable */
  const msConfig = getMsAuthConfig(jsonData);
  if (!msConfig.clientId || !msConfig.tenantId || !msConfig.baseUrl) {
    const missing = ['clientId','tenantId', 'baseUrl'].filter(
      (k) => !msConfig[k as keyof typeof msConfig]
    );
    logger.error('Graph send: missing MS config', { missing });
    throw new Error(`Microsoft Graph config missing: ${missing.join(', ')}`);
  }

  const baseUrl = msConfig.baseUrl;
  const smtpFrom = `${smtp?.emailAddress || ''}`.trim();
  const smtpLower = smtpFrom.toLowerCase();
  const fromList = uniqueLowercase(splitEmails(data?.FromEmail || ''));
  const toListRaw = uniqueLowercase(splitEmails(data?.ToEmail || ''));
  const ccListRaw = uniqueLowercase(splitEmails(data?.cc || ''));
  const bccListRaw = uniqueLowercase(splitEmails(data?.bcc || ''));

  const fromEmail = smtpFrom || fromList[0] || '';
  const toList = toListRaw.filter((email) => email !== smtpLower);
  const ccList = ccListRaw.filter((email) => email !== smtpLower);
  const bccList = bccListRaw.filter((email) => email !== smtpLower);

  // If inbound/outbound values are flipped, fallback to original sender.
  if (toList.length === 0 && fromList.length > 0) {
    const fallbackTo = fromList.find((email) => email !== smtpLower);
    if (fallbackTo) {
      toList.push(fallbackTo);
    }
  }

  if (!fromEmail || toList.length === 0) {
    logger.error('Graph send: missing from/to email', {
      fromEmail,
      toList,
      fromList,
      toListRaw,
      ccListRaw,
      bccListRaw,
    });
    throw new Error('Graph send: missing from/to email');
  }

  const message: any = {
    subject: data?.EmailSubject ?? 'No Subject',
    body: {
      contentType: 'HTML',
      content: text ?? data?.text ?? '',
    },
    toRecipients: asRecipients(toList),
    ccRecipients: asRecipients(ccList),
    bccRecipients: asRecipients(bccList),
  };

  const attachments = buildGraphAttachments(data?.attachments || []);
  if (attachments.length) {
    message.attachments = attachments;
  }
  const graphMessageId = data?.graphMessageId || data?.replyToGraphMessageId;
  const sentContent = `${text ?? data?.text ?? ''}`;
  const sentContentPreview = sentContent?.replace(/\s+/g, ' ')?.trim()?.slice(0, 500);

  if (graphMessageId) {
    logger.info('Graph send: using reply endpoint for threading', { graphMessageId });
    try {
      await fetchWithTokenRetry(msConfig, async (token) => {
        const replyResponse = await axios.post(`${baseUrl}/users/${encodeURIComponent(fromEmail)}/messages/${graphMessageId}/createReply`, {},
          {
            headers: {
              Authorization: `Bearer ${token}`,
              'Content-Type': 'application/json'
            },
            timeout: AXIOS_TIMEOUT,
          }
        );
        const draftId = replyResponse.data?.id;
        if (!draftId) {
          throw new Error('Failed to create reply draft - no ID returned');
        }
        try {
          await axios.patch(
            `${baseUrl}/users/${encodeURIComponent(fromEmail)}/messages/${draftId}`,
            {
              body: message.body,
              toRecipients: message.toRecipients,
              ccRecipients: message.ccRecipients,
              bccRecipients: message.bccRecipients,
              attachments: message.attachments,
            },
            {
              headers: {
                Authorization: `Bearer ${token}`,
                'Content-Type': 'application/json'
              },
              timeout: AXIOS_TIMEOUT,
            }
          );
          await axios.post(
            `${baseUrl}/users/${encodeURIComponent(fromEmail)}/messages/${draftId}/send`,
            {},
            {
              headers: { Authorization: `Bearer ${token}` },
              timeout: AXIOS_TIMEOUT,
            }
          );
        } catch (sendErr: any) {
          logger.error('Failed to send reply, attempting to delete orphaned draft', { draftId });
          try {
            await axios.delete(
              `${baseUrl}/users/${encodeURIComponent(fromEmail)}/messages/${draftId}`,
              { headers: { Authorization: `Bearer ${token}` }, timeout: AXIOS_TIMEOUT }
            );
            logger.info('Orphaned draft deleted successfully', { draftId });
          } catch (deleteErr) {
            logger.error('Failed to delete orphaned draft', { draftId });
          }
          throw sendErr;
        }
      });
    } catch (replyErr: any) {
      if (!shouldFallbackToDirectSend(replyErr)) {
        throw replyErr;
      }
      logger.warn('Graph reply threading failed; falling back to direct sendMail', {
        graphMessageId,
        status: replyErr?.response?.status,
        code: replyErr?.response?.data?.error?.code,
        message: replyErr?.response?.data?.error?.message || replyErr?.message,
      });
      await fetchWithTokenRetry(msConfig, async (token) => {
        await axios.post(
          `${baseUrl}/users/${encodeURIComponent(fromEmail)}/sendMail`,
          { message, saveToSentItems: true },
          { headers: { Authorization: `Bearer ${token}` }, timeout: AXIOS_TIMEOUT }
        );
      });
    }
  } else {
    logger.info('Graph send: using direct sendMail (no threading)');
    await fetchWithTokenRetry(msConfig, async (token) => {
      await axios.post(
        `${baseUrl}/users/${encodeURIComponent(fromEmail)}/sendMail`,
        { message, saveToSentItems: true },
        { headers: { Authorization: `Bearer ${token}` }, timeout: AXIOS_TIMEOUT }
      );
    });
  }

  logger.info('✅ Graph email sent', {
    messageId: data?.messageId || null,
    subject: data?.EmailSubject || null,
    graphMessageId: graphMessageId || null,
    fromEmail,
    to: toList,
    cc: ccList,
    bcc: bccList,
    sentContentPreview,
    sentContentLength: sentContent?.length,
    threaded: !!graphMessageId
  });
}
``