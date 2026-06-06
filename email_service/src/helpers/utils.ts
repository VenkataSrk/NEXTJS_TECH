import axios from 'axios';
const dns = require('dns').promises;
import crypto from 'crypto';
import fs from 'fs';
import jwt from 'jsonwebtoken';
import path from 'path';
import { logger } from '../plugins/log';
import email from '../service/email';
import { sendMail } from '../service/smtp';

// send nlp Email CAI bot
export async function getEmailBot(nlpResult: any, companyDetails: any, content: string, activeBot: any) {
  try {
    const extractedText = content?.replace(/<\/?p>/g, '');
    const body = {
      query: extractedText ?? null,
      domine_id: nlpResult?.DomainId,
      website_id: '',
      company_name: companyDetails?.companyName ?? '',
      industry_type: companyDetails?.industryType ?? '',
      customerId: nlpResult?.FromEmail ?? null,
      isBot: activeBot?.isBot ?? 0
    };
    logger.info('Nlp email payload send', JSON.stringify(body, null, 2));
    const response = await axios.post(process?.env?.emailBot, body);
    logger.info('Nlp email received', JSON.stringify(response.data, null, 2));
    return response?.data;
  } catch (error) {
    logger.error('Error sending message:', error);
  }
}

// senti mental Score
export async function sentimentalScore(data: any) {
  try {
    const body = {
      HistoryId: data?.HistoryId,
      ChatDurationHistoryId: data?.ChatDurationHistoryId ?? 0,
      channelType: data?.channelType,
      domainId: data?.domainId,
      ext: parseInt(data?.ext, 10) || 0
    };

    const response: any = await axios.post(
      process?.env?.Sentimental,
      body,
      {
        headers: { 'Content-Type': 'application/json' },
        timeout: 5000
      }
    );
    return response ?? {};
  } catch (error) {
    logger.error('sentimentalScore', data, error);
    return {};
  }
}

// Agent Assist all Channel
export async function agentAssistChannel(data: any, getWebsite: any) {
  try {
    const body = {
      domainId: data?.domainId,
      ext: data?.ext,
      channelType: data?.channelType,
      channelId: data?.channelId,
      sessionId: data?.sessionId,
      websiteId: getWebsite[0]?.websiteId ?? '',
      message: data?.message,
      type: data?.type ?? '',
      company_name: (getWebsite[0]?.baseName === null ? '' : getWebsite[0]?.companyName) ?? '',
      industry_type: (getWebsite[0]?.CompanyType === null ? '' : getWebsite[0]?.industryType) ?? '',
      customerId: data?.customerId?.toString() ?? '',
      messageId: data?.messageId,
    };
    logger.info('agentAssistChannel----', body);
    logger.info(body);
    const response = await axios.post(process.env.semiChannel, body);
    logger.error('agentAssistChannel--response----', response);
    logger.info('agentAssistChannel--response----', response);
    return response;
  } catch (error) {
    logger.error('Error sending message:', error);
    // You can handle the error here, e.g., log it or return a specific response
  }
}

// lookup spf record
export async function lookupSPFRecord(domainName: any) {
  try {
    const records = await dns.resolveTxt(domainName.trim());
    logger.info('records>>>>>>>>>', records);
    const spfRecords = records.map((record: string[]) => record.join('')).filter((record: string) => record.startsWith('v=spf1'));
    logger.info('spfRecords>>>>>>>>>', spfRecords);
    if (spfRecords.length > 0) {
      return true;
    }
    return false;

  } catch (err) {
    logger.error(`Error looking up SPF record for ${domainName}:`, err);
  }
}

//  lookup cname
export async function lookupCNAME(domain: any) {
  try {
    const addresses = await dns.resolveCname(domain);
    if (addresses.length === 0) {
      return true;
    }
    return true;

  } catch (err) {
    if (err.code === 'ENODATA') {
      return true;
    }
    return true;

  }
}

/* tslint:disable */
// dkim verification
export function verifyDkim(domain: string) {
  const selector = 's1';
  try {
    return new Promise((resolve, reject) => {
      const dns = require('dns');
      const dkimRecordName = `${selector}._domainkey.${domain}`;
      dns.resolveTxt(dkimRecordName, (err: any, records: any) => {
        if (err) {
          reject(err);
          return;
        }
        if (records[0]?.length > 0) {
          resolve(true);
        } else {
          resolve(false);
        }
      });
    });
  } catch (error) {
    logger.info(error);
  }
}

// Function to identify attachment parts
export const findAttachmentParts = (struct: any, attachments: any = []) => {
  for (let i = 0, len = struct?.length; i < len; i++) {
    if (Array.isArray(struct[i])) {
      findAttachmentParts(struct[i], attachments);
    } else {
      if (struct[i]?.disposition && ['INLINE', 'ATTACHMENT'].includes(struct[i]?.disposition?.type?.toUpperCase())) {
        attachments.push(struct[i]);
      }
    }
  }
  logger.info('Attachments found:', attachments);
  return attachments;
};

export const removeReplyAllContent = (html: any) => {
  const replyAllPattern1 = /On .* wrote:/g;
  const replyAllPattern2 = /<p>(?:<br\/>)?&gt;.*(?:<br\/>)*<\/p>/g;
  let cleanedHtml = html.replace(replyAllPattern1, '');
  cleanedHtml = cleanedHtml.replace(replyAllPattern2, '');
  cleanedHtml = cleanedHtml.replace(/\n/g, '');
  return cleanedHtml;
};

export const buildAttMessageFunction = (attachment: any) => {
  const filename = attachment?.params?.name;
  const encoding = attachment?.encoding;
  return (msg: any, seqno: any) => {
    msg.on('body', (stream: any) => {
      const saveDir = path.join(__dirname, 'src', 'uploads', String(seqno));
      if (!fs.existsSync(saveDir)) {
        fs.mkdirSync(saveDir, { recursive: true });
      }
      const filePath = path.join(saveDir, filename);
      const savePath = `/${seqno}/${filename}`;
      const writeStream = fs.createWriteStream(filePath);
      writeStream.on('finish', () => {
        attachment += `${savePath}##`;
      });

      if (encoding.toUpperCase() === 'BASE64') {
        const { Base64Decode } = require('base64-stream');
        stream.pipe(new Base64Decode()).pipe(writeStream);
      } else {
        stream.pipe(writeStream);
      }
    });
    msg.once('end', () => {
      logger.info('end');
    });
  };
};

export const htmlContentData = (data: any, countData: any, getDatas: any) => {
  const startDate = getDatas?.planStartTime?.toISOString()?.split('T')[0];
  const endDate = getDatas?.planEndTime?.toISOString()?.split('T')[0];
  const count = data?.isEmailSent == '0' ? "50%" : data?.isEmailSent == '1' ? "80%" : "100";
  const fullName = [countData?.Firstname, countData?.Surname]?.filter(Boolean)?.join(' ') || 'Customer';
  const alert = data?.isEmailSent === '2' ? 'Heads Up: All Sessions Have Been Consumed! ' : data?.isEmailSent === '0' ? "Session Alert: You've Reached 50% Consumption!" : "Session Alert: You've Reached 80% Consumption!";
  const countHtml = `
    <!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8" />
  <title>Session Usage Alert</title>
</head>
<body style="margin:0; padding:0; background-color:#e5e5e5; font-family:Arial, Helvetica, sans-serif;">
  <table width="100%" cellpadding="0" cellspacing="0" style="background-color:#e5e5e5;">
    <tr>
      <td align="center" style="padding:20px 0;">
    <!-- Email Container -->
    <table width="600" cellpadding="0" cellspacing="0" style="background-color:#ffffff; border-radius:8px; overflow:hidden;">
     
      <!-- Header / Banner -->
      <tr>
        <td style="background-color:#000000; padding:30px 24px; text-align:center;border-radius: 0 24px;">
          <div style="color:#ffffff; font-size:18px; font-weight:bold; margin-bottom:12px;">
            <img src='https://worktual.co.uk/whitelogonew.png' alt='worktualogo'/>
          </div>

          <div style="font-size:28px; color:#f5c542; margin-bottom:10px;">
          <img src='https://ccaas-storage.worktual.co.uk/chat/13490/alertmsg.png' alt='alert'/>
        </div>

          <div style="color:#ffffff; font-size:22px; font-weight:bold; line-height:1.4;">
            ${alert ?? ''}
          </div>

          <!-- Progress Info -->
          <table width="100%" cellpadding="0" cellspacing="0" style="margin-top:24px;">
            <tr>
              <td style="color:#ffffff; font-size:12px;" align="left">
                ${getDatas?.TotalSessionUsed ?? '-'} Session completed
              </td>
              <td style="color:#ffffff; font-size:12px;" align="right">
                Total ${getDatas?.TotalSession ?? 0} session
              </td>
            </tr>
            <tr>
              <td colspan="2" style="padding-top:8px;">
                <div style="background-color:#444444; height:6px; border-radius:4px;">
                  <div style="width:${count}; background-color:#7c3aed; height:6px; border-radius:4px;"></div>
                </div>
              </td>
            </tr>
          </table>
        </td>
      </tr>

      <!-- Body Content -->
      <tr>
        <td style="padding:28px 24px; color:#333333; font-size:14px; line-height:1.6;background:#F8F8F8;padding:34px;">
          <p style="margin:0 0 16px 0;">
            Dear <strong>${fullName ?? ''}</strong>,
          </p>

          <p style="margin:0 0 16px 0;">
            We are writing to inform you that your organization has now utilized
            <strong>${count ?? '-'}</strong> of the session quota included in your current plan
            with Worktual Ltd.
          </p>

          <p style="margin:0 0 8px 0;"><strong>Summary:</strong></p>
          <ul style="margin:0 0 16px 20px; padding:0;">
            <li>Subscription Plan: <strong>${countData?.plan_name ?? ''}</strong></li>
            <li>Sessions Allocated: <strong>${getDatas?.TotalSession ?? '-'}</strong></li>
            <li>Sessions Used: <strong>${count ?? '-'}</strong></li>
            <li>Remaining Sessions: <strong>${getDatas?.remining_sessions ?? '-'}</strong></li>
            <li> Start to End Date: <strong>${(startDate && endDate && `${startDate} to ${endDate}`) ?? '-'}</strong></li>
            </ul>
          <p style="margin:0 0 16px 0;">
            You may continue using the session as usual. However, we recommend
            monitoring your remaining sessions regularly to avoid any unexpected
            service interruptions.
          </p>

          <p style="margin:0 0 16px 0;">
            Should you require additional sessions or wish to explore plan upgrade
            options, please don’t hesitate to contact us or access your admin dashboard.
          </p>

          <p style="margin:0 0 24px 0;">
            Thank you for being a valuable part of our community. Your success is our success!
          </p>

          <!-- CTA Button -->
          <table cellpadding="0" cellspacing="0" align="center">
            <tr>
              <td style="background-color:#000000; border-radius: 0 14px;">
                <p style="display:inline-block; padding:0 24px; color:#ffffff; text-decoration:none; font-size:14px; font-weight:bold;">
                  To get more sessions, please contact us at +918428224590.
                </p>
              </td>
            </tr>
          </table>

          <p style="margin-top:32px; font-size:13px;">
            Warm regards,<br />
            <strong>Client Success Team</strong>
          </p>
        </td>
      </tr>

      <!-- Footer -->
      <tr>
        <td style="background-color:#fff; padding:12px; text-align:center; font-size:12px; color:#1B191D;">
          www.worktual.co.uk
        </td>
      </tr>

    </table>
    <!-- End Container -->

  </td>
</tr>
  </table>
</body>
</html>`
  return countHtml;
}

export const htmlPlanActive = (countData: any, getDatas: any) => {
  const fullName = [countData?.Firstname, countData?.Surname]?.filter(Boolean)?.join(' ') || 'Customer';
  const endDate = getDatas?.planEndTime?.toISOString()?.split('T')[0];
  const inactivePlan =
    `<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8" />
  <title>Session Usage Alert</title>
   <style>
    /* Mobile Styles */
    @media only screen and (max-width: 600px) {
      .container {
        width: 100% !important;
      }
    }
  </style>
</head>
<body style="margin:0; padding:0; background-color:#e5e5e5; font-family:Arial, Helvetica, sans-serif;">
  <table width="100%" cellpadding="0" cellspacing="0" style="background-color:#e5e5e5;">
    <tr>
      <td align="center" style="padding:20px 0;">
    <!-- Email Container -->
    <table width="600" class="container" cellpadding="0" cellspacing="0" style="background-color:#ffffff; border-radius:8px; overflow:hidden;">
     
      <!-- Header / Banner -->
      <tr>
        <td style="background-color:#000000; padding:30px 24px; text-align:center;border-radius: 0 24px;">
          <div style="color:#ffffff; font-size:18px; font-weight:bold; margin-bottom:12px;">
            <img src='https://worktual.co.uk/whitelogonew.png' alt='worktualogo'/>
          </div>

          <div style="font-size:28px; color:#f5c542; margin-bottom:10px;">
          <img src='https://ccaas-storage.worktual.co.uk/chat/13490/alertmsg.png' alt='alert'/>
        </div>

          <div style="color:#ffffff; font-size:22px; font-weight:400; line-height:42px;">
            ${getDatas?.is_reminder_for_date === 1 ? 'Worktual Contract Expiring in 7 Days' : 'Your Subscription Plan Has Expired'}
          </div>
          <div style="color:#ffffff; font-size:34px; font-weight:bold; line-height:1.4;">${getDatas?.is_reminder_for_date === 1 ? '7 Days' : ''}</div>

        </td>
      </tr>

      <!-- Body Content -->
      <tr style="padding:16px;display:block;">
        <td style="padding:28px 24px; color:#333333; font-size:14px; line-height:1.6;background:#F8F8F8;padding:34px;border-radius:12px">
          <p style="margin:0 0 16px 0;font-weight:600">
            Dear ${fullName ?? ''},
          </p>

          <p style="margin:0 0 16px 0;">
            This is a reminder that your <strong>plan / contract with Worktual</strong> is set to expire on <strong>${endDate ?? '-'}</strong>.</p>

          <p style="margin:0 0 8px 0;">To avoid any interruption in services, we kindly request you to initiate the renewal or extension process at the earliest.</p>
          <p style="margin:0 0 16px 0;">If you require any assistance or clarification, please feel free to contact our team.</p>

          <p style="margin:0 0 16px 0;">We appreciate your association with Worktual and look forward to continuing our collaboration.</p>

          <p style="margin:0 0 24px 0;">
            Thank you for being a valuable part of our community. Your success is our success!
          </p>


          <p style="margin-top:32px; font-size:16px;font-weight:bold">
            Warm regards,<br />
            <strong style="font-size:14px;font-weight:500">Client Success Team</strong>
          </p>
        </td>
      </tr>

      <!-- Footer -->
      <tr>
        <td style="background-color:#fff; padding:12px; text-align:center; font-size:12px; color:#1B191D;">
          www.worktual.co.uk
        </td>
      </tr>

    </table>
    <!-- End Container -->

  </td>
</tr>
  </table>
</body>
</html>`
  return inactivePlan;
}

// call Google Sheet SyncAPI
export function callGoogleSheetSyncAPI(data: any) {
  axios.post(process.env.googlesheet, data)
    .catch((err) => {
      logger.error('GoogleSheet sync call failed:', err);
    });
}

export function formatPrivateKey(keyContent: string): string {
  if (keyContent.includes('\n')) {
    return keyContent;
  }
  // Fix escaped newlines
  if (keyContent.includes('\\n')) {
    return keyContent.replace(/\\n/g, '\n');
  }
  const header = '-----BEGIN PRIVATE KEY-----';
  const footer = '-----END PRIVATE KEY-----';
  const body = keyContent.replace(header, '').replace(footer, '').replace(/\s/g, '');
  const formatted = body.match(/.{1,64}/g)?.join('\n') || body;
  return `${header}\n${formatted}\n${footer}`;
}

export function createClientAssertion(msConfig: any): string {
  if (!msConfig?.privateKey && !msConfig?.privateKeyPath) {
    throw new Error('Microsoft private key is missing');
  }
  const rawKey = msConfig.privateKey || msConfig.privateKeyPath;
  const privateKey = formatPrivateKey(rawKey);
  const now = Math.floor(Date.now() / 1000);

  const payload = {
    aud: `https://login.microsoftonline.com/${msConfig.tenantId}/oauth2/v2.0/token`,
    iss: msConfig.clientId,
    sub: msConfig.clientId,
    jti: crypto.randomBytes(16).toString('hex'),
    nbf: now,
    exp: now + 600,
  };

  // Base64url encode the thumbprint (remove colons if present)
  if (!msConfig?.thumbprint) {
    throw new Error('Microsoft certificate thumbprint is missing');
  }
  const thumbprintHex = msConfig.thumbprint.replace(/:/g, '');
  const x5t = Buffer.from(thumbprintHex, 'hex').toString('base64url');

  return jwt.sign(payload, privateKey, {
    algorithm: 'RS256',
    header: { x5t, alg: 'RS256', typ: 'JWT' },
  });
}

export function cleanSubject(subject: string = '') {
  return subject.replace(/^(RE:|FW:|FWD:)\s*/gi, '').trim();
}

export function stripDisclaimer(text: string = '') {
  if (!text) return '';
  const normalized = `${text}`.replace(/\r/g, '');
  const hasAiAssistant = /\bAI ASSISTANT\b/i.test(normalized);

  const disclaimerMarkers = [
    /NOTICE AND DISCLAIMER/i,
    /Thanks?\s*(?:and|&)\s*regards,\s*AI ASSISTANT/i,
  ];
  if (hasAiAssistant) {
    disclaimerMarkers.push(/Dear Customer[,\s:.-]/i);
    disclaimerMarkers.push(/Hi Customer[,\s:.-]/i);
  }

  let cutoffIndex = -1;
  for (const marker of disclaimerMarkers) {
    const match = marker.exec(normalized);
    if (!match || typeof match.index !== 'number') continue;
    if (cutoffIndex === -1 || match.index < cutoffIndex) {
      cutoffIndex = match.index;
    }
  }

  if (cutoffIndex > -1) {
    return normalized.slice(0, cutoffIndex).trim();
  }
  return normalized.trim();
}

export function htmlToText(html: string = '') {
  const withoutTags = html
    .replace(/<style[\s\S]*?<\/style>/gi, ' ')
    .replace(/<script[\s\S]*?<\/script>/gi, ' ')
    .replace(/<br\s*\/?>/gi, '\n')
    .replace(/<\/p>/gi, '\n')
    .replace(/<[^>]+>/g, ' ')
    .replace(/&nbsp;/gi, ' ')
    .replace(/&amp;/gi, '&')
    .replace(/&lt;/gi, '<')
    .replace(/&gt;/gi, '>')
    .replace(/&#39;/gi, "'")
    .replace(/&quot;/gi, '"');
  return withoutTags.replace(/\s+\n/g, '\n').replace(/[ \t]+/g, ' ').trim();
}

export function getTokenCacheKey(msConfig: any): string {
  return `${msConfig.tenantId}:${msConfig.clientId}`;
}

export function extractAddress(value: string) {
  const trimmed = value.trim();
  const match = trimmed.match(/<([^>]+)>/);
  return (match ? match[1] : trimmed).trim();
}

/* tslint:disable */
export function splitEmails(value: any): string[] {
  if (!value) return [];
  if (Array.isArray(value)) {
    return value
      .map((v) => `${v}`)
      .join(',')
      .split(/[;,]/g)
      .map((v) => extractAddress(v))
      .filter(Boolean);
  }
  return `${value}`
    .split(/[;,]/g)
    .map((v) => extractAddress(v))
    .filter(Boolean);
}

export function getAttachmentName(att: any, fallbackName: string) {
  return att?.filename || att?.name || fallbackName || `attachment_${Date.now()}`;
}
