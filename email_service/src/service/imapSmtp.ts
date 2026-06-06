
import fs from 'fs';
import Imap from 'imap';
import path from 'path';
import { CHANNEL_TYPE } from '../../src/helpers/constants';
import { logger } from '../../src/plugins/log';
import { configs } from '../config/app';
import { getMultipleEmail } from '../dao/email.dao';
import { removeReplyAllContent } from '../helpers/utils';
import { createEmailSession } from '../modules/email/handler';
import { fetchMicrosoftInbox, sleep } from './microsoftGraph';
const { simpleParser } = require('mailparser');

export async function imapSmtpInboxHandler() {
  try {
    const users: any = await getMultipleEmail();
    const smtpEmails = users.filter((u: any) => u.is_smtp === 1);
    logger.info('Total SMTP Emails:', smtpEmails.length);
    const batchSize = 20;
    for (let i = 0; i < smtpEmails.length; i += batchSize) {
      const batch = smtpEmails.slice(i, i + batchSize);
      await Promise.all(
        batch.map(async (user: any) => {
          try {
            const jsonData = JSON.parse(user.smtp_json || '{}');
            if (jsonData.provider === 'MICROSOFT') {
              // Poll Microsoft Graph API as fallback for webhooks
              await fetchMicrosoftInbox(user, jsonData);
            } else {
              await connectToImap(user, jsonData);
            }
          } catch (err: any) {
            logger.error(`Inbox error for ${user.emailAddress}`, err);
          }
        })
      );
      await sleep(5000);
    }
  } catch (err) {
    logger.error('imapSmtpInboxHandler error', err);
  }
}

// Create and connect IMAP
async function connectToImap(config: any, jsonData: any) {
  return new Promise((resolve, reject) => {
    const emailDomain = (config?.emailAddress || '').split('@')[1]?.toLowerCase() || '';
    let imapHost = '';
    if (emailDomain.includes('gmail.com')) {
      imapHost = process.env.IMAP_SMTP_HOST_GMAIL;
    } else if (emailDomain.includes('yahoo.com')) {
      imapHost = process.env.IMAP_SMTP_HOST_YAHOO;
    }

    if (!imapHost) {
      logger.info(`Skipping IMAP connection for unsupported SMTP domain: ${config?.emailAddress}`);
      resolve(null);
      return;
    }

    let settled = false;
    let fetchInterval: NodeJS.Timeout | null = null;
    let disconnectTimeout: NodeJS.Timeout | null = null;

    const settleResolve = () => {
      if (settled) return;
      settled = true;
      if (fetchInterval) clearInterval(fetchInterval);
      if (disconnectTimeout) clearTimeout(disconnectTimeout);
      resolve(null);
    };

    const settleReject = (err: any) => {
      if (settled) return;
      settled = true;
      if (fetchInterval) clearInterval(fetchInterval);
      if (disconnectTimeout) clearTimeout(disconnectTimeout);
      reject(err);
    };

    const imap = new Imap({
      user: config?.emailAddress,
      password: jsonData?.password,
      host: imapHost,
      port: Number(process.env.IMAP_SMTP_HOST_PORT),
      tls: true,
      keepalive: true,
      authTimeout: 100000,
      connTimeout: 300000,
      tlsOptions: {
        rejectUnauthorized: false,
      },
    });

    imap.once('ready', () => {
      logger.info(`IMAP connected for: ${config?.emailAddress}`);
      openInbox(imap, async (err: any) => {
        if (err) {
          logger.error(`Inbox open error for ${config?.emailAddress}:`, err.message);
          imap.end();
          settleReject(err);
          return;
        }

        logger.info('Inbox opened for:', config?.emailAddress);
        fetchUnseenEmails(imap);
        fetchInterval = setInterval(() => fetchUnseenEmails(imap), 10000);
        disconnectTimeout = setTimeout(() => {
          imap.end();
        },                             600000);
      });
    });

    imap.once('error', (err: any) => {
      logger.error(`IMAP Error for ${config?.emailAddress}:`, err.message);
      settleReject(err);
    });

    imap.once('end', () => {
      logger.info(`IMAP disconnected for: ${config?.emailAddress}`);
      settleResolve();
    });

    try {
      imap.connect();
    } catch (err: any) {
      logger.error('IMAP Connect error:', err.message);
      settleReject(err);
    }
  });
}

// Open Inbox Folder
function openInbox(imap: any, callback: any) {
  imap.openBox('INBOX', false, callback);
}

// Fetch unseen (new) emails
function fetchUnseenEmails(imap: any) {
  imap.search(['UNSEEN'], (err: any, results: any) => {
    if (err) {
      logger.error('Search error:', err.message);
      return;
    }
    if (!results.length) {
      logger.info('No new unseen emails.');
      return;
    }

    const f = imap.fetch(results, { bodies: '' });

    f.on('message', (msg: any) => {
      processEmail(imap, msg);

      msg.once('attributes', (attrs: any) => {
        imap.addFlags(attrs.uid, ['\\Seen'], (error: any) => {
          if (error) logger.error('Mark seen error:', error.message);
        });
      });
    });

    f.once('error', (error: any) => {
      logger.error('Fetch error:', error.message);
    });

    f.once('end', () => {
      logger.info('Done fetching unseen emails.');
    });
  });
}

// Process single email
async function processEmail(imap: any, msg: any) {
  let buffer = '';
  msg.on('body', (stream: any) => {
    stream.on('data', (chunk: any) => {
      buffer += chunk.toString('utf8');
    });
    stream.once('end', async () => {
      try {
        const mail = await simpleParser(buffer);
        const messageId = mail?.messageId;
        logger.info('Message-ID:', messageId);
        let attachments: any = [];
        if (mail?.attachments?.length) {
          attachments = await downloadAttachments(imap, mail?.attachments, path.join(__dirname, 'attachments'));
        }
        const cleanedBody = removeReplyAllContent(mail.text);

        const sessionData = {
          customerData: {
            fromData: mail?.from?.text ?? '',
            toData: mail?.to?.text ?? '',
            dateData: mail?.date ?? '',
            textData: cleanedBody ?? '',
            subjectData: mail.subject ? mail?.subject?.replace(/RE:|Re:|re:/, '').trim() : '',
            sessionStartTime: Date.now(),
            attachments: attachments ?? [],
            messageId: messageId ?? undefined
          },
          channelType: CHANNEL_TYPE?.EMAIL,
        };

        await createEmailSession(sessionData);

      } catch (err) {
        logger.error('Process email error:', err.message);
      }
    });
  });
}

/* tslint:disable */
// Download attachments
function downloadAttachments(imap: any, attachments: any, outputPath: any) {
  return new Promise((resolve, reject) => {
    const downloaded = [];

    if (!fs.existsSync(outputPath)) {
      fs.mkdirSync(outputPath, { recursive: true });
    }

    let remaining = attachments.length;
    if (remaining === 0) {
      return resolve(downloaded);
    }

    attachments.forEach((attachment: any) => {
      if (!attachment?.partId) {
        console.warn('Attachment missing partId:', attachment.filename);
        remaining--;
        if (remaining === 0) resolve(downloaded);
        return;
      }

      const fetch = imap.fetch(attachment?.partId, { bodies: [attachment.partId], struct: true });

      fetch.on('message', (msg: any) => {
        msg.on('body', (stream: any) => {
          const chunks: any = [];
          stream.on('data', (chunk: any) => {
            chunks.push(chunk);
          });
          stream.on('end', () => {
            const buffer = Buffer.concat(chunks);
            const uniqueFileName = attachment.filename;
            const tempPath = path.join(__dirname, uniqueFileName);

            fs.writeFileSync(tempPath, buffer, 'base64');

            const finalPath = path.join(configs.email_path.Email_upload_destination, uniqueFileName);
            if (!fs.existsSync(configs.email_path.Email_upload_destination)) {
              fs.mkdirSync(configs.email_path.Email_upload_destination, { recursive: true });
            }
            fs.copyFileSync(tempPath, finalPath);

            const fileUrl = `${configs.email_path.Email_fileGetUrl}${uniqueFileName}`;

            downloaded.push({
              href: fileUrl,
              type: attachment.contentType,
              size: fs.statSync(tempPath).size,
              filename: attachment.filename,
              fileType: attachment.contentType,
            });

            remaining--;
            if (remaining === 0) resolve(downloaded);
          });
        });
      });

      fetch.once('error', (err: any) => {
        logger.error('Attachment fetch error:', err.message);
        remaining--;
        if (remaining === 0) resolve(downloaded);
      });
    });
  });
}
