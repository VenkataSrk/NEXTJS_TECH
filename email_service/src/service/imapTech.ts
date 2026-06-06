import fs from 'fs';
import Imap from 'imap';
import path from 'path';
import { CHANNEL_TYPE } from '../../src/helpers/constants';
import { logger } from '../../src/plugins/log';
import { configs } from '../config/app';
import { getMultipleEmail } from '../dao/email.dao';
import { createEmailSession } from '../modules/email/handler';
const { simpleParser } = require('mailparser');

let messageData: any = { seqno: '', from: '', to: '', date: '', subject: '', text: '', textAsHtml: '', attachment: '', attachments: [], messageId: '', cc: [] };
const processedMessageIds = new Set();

// imap Inbox
export async function imapInboxDomianHandler() {
  try {
    const getDataValue: any = await getMultipleEmail();
    logger.info('EmailCountTech', getDataValue?.length);
    const batchSize = 20;
    const totalAccounts = getDataValue.length;
    for (let i = 0; i < totalAccounts; i += batchSize) {
      const batch = getDataValue.slice(i, i + batchSize);
      await Promise.allSettled(
        batch.map((userConfig: any) => receiveEmails({
          user: userConfig?.emailAddress,
          password: process.env.IMAP_PASSWORD,
          host: process.env.IMAP_HOST,
          port: process.env.IMAP_SMTP_HOST_PORT,
          tls: true,
          keepalive: true,
          authTimeout: 100000,
          connTimeout: 300000,
          tlsOptions: {
            rejectUnauthorized: false
          },
        }))
      );
      // Wait for some time before connecting to the next batch
      await new Promise(resolve => setTimeout(resolve, 10000)); // Wait for 5 seconds
    }
  } catch (err) {
    logger.error('imapInboxDomianHandler - Unexpected Error:', err);
  }
}

// Function to handle errors
function handleError(err: any) {
  if (err?.textCode === 'AUTHENTICATIONFAILED') {
    logger.error('IMAP login failed Invalid credentials.');
  } else {
    logger.error('IMAP error for', err);
  }
}

// Function to process individual message
function processMessage(msg: any) {
  const from = msg.headers.from;
  const to = msg.headers.to;
}
export const compare = (data: any, needle = '') => {
  if (data.length > 0) {
    for (const k in data) {
      if (typeof data[k].date !== 'undefined' && typeof data[k].date[0] !== 'undefined' && data[k].date[0].toString() === needle.toString()) {
        return [false, data[k].seqno];
      }
    }
  }
  return [true, -1];
};

export const getDate = (headerLines: any) => {
  if (headerLines.length > 0) {
    for (const k in headerLines) {
      if (headerLines[k].key.toString() === 'date') {
        return headerLines[k].line.substring(6);
      }
    }
  }
  return '';
};
const toUpper = (thing: any) => (thing && thing.toUpperCase ? thing.toUpperCase() : thing);
const findAttachmentParts = (struct: any, attachments = []) => {
  for (let i = 0, len = struct.length; i < len; i = i + 1) {
    if (Array.isArray(struct[i])) {
      findAttachmentParts(struct[i], attachments);
    } else {
      if (struct[i].disposition && ['INLINE', 'ATTACHMENT'].indexOf(toUpper(struct[i].disposition.type)) > -1) {
        attachments.push(struct[i]);
      }
    }
  }
  return attachments;
};

const buildAttMessageFunction = (attachment, _seqno) => {
  const filename = attachment.params.name;
  const encoding = attachment.encoding;

  return (msg: any, seqno: any) => {
    const prefix = `(#${seqno}) `;
    msg.on('body', (stream, info) => {
      const _dir = require('path').join(__dirname, '');
      const dir = _dir.split('src');
      dir[0] = `${dir[0]}src/uploads/${_seqno}`;
      if (!fs.existsSync(dir[0])) {
        fs.mkdirSync(dir[0], { recursive: true });
      }
      const filePath = require('path').join(`${dir[0]}/${filename}`);
      const savePath = require('path').join(`/${_seqno}/${filename}`);

      const writeStream = fs.createWriteStream(filePath);
      writeStream.on('finish', () => {
        messageData.attachment += `${savePath}##`;
      });

      if (toUpper(encoding) === 'BASE64') {
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

const escapeRegExp = (str2: string, attachment?: string): string => {
  if (!attachment || typeof attachment !== 'string') {
    return str2;
  }
  let str = str2;
  const _array = attachment.split('##');
  const _array2 = [..._array];
  for (const _img of _array) {
    const _img2 = _img.split('\\').pop();
    if (_img2 && _img2.includes('image0')) {
      const _image = `<img src="http://localhost:5007/public${_img.replace(/\\/g, '/')}">`;
      const regexObj = new RegExp(`\\[cid:${_img2}@(.+?)\\]`, 'g');
      if (regexObj.test(str)) {
        const index = _array.indexOf(_img);
        _array2[index] = '';
      }
      str = str.replace(regexObj, _image);
    }
  }
  for (const _img of _array2) {
    if (_img !== '') {
      const _img2 = _img.split('\\').pop();
      if (_img2 && _img2.includes('image0')) {
        const _image = `<img src="http://localhost:5007/public${_img.replace(/\\/g, '/')}">`;
        const regexObj2 = new RegExp('\\[Description: Description: cid:image0(.+?)]', 'g');
        str = str.replace(regexObj2, _image);
      }
    }
  }
  return str;
};

const removeReplyAllContent = (html: any) => {
  const replyAllPattern1 = /On .* wrote:/g;
  const replyAllPattern2 = /<p>(?:<br\/>)?&gt;.*(?:<br\/>)*<\/p>/g;
  let cleanedHtml = html.replace(replyAllPattern1, '');
  cleanedHtml = cleanedHtml.replace(replyAllPattern2, '');
  return cleanedHtml;
};

const downloadAttachments = async (imap: any, message: any, outputPath: any) => {
  const attachments = [];

  // await processPart(message);
  for (const data of message) {
    const base64Content = data.content.toString('base64');
    const uniqueFileName = `${data.filename}`;
    const filePath = path.join(__dirname, uniqueFileName);
    fs.writeFileSync(filePath, base64Content, 'base64');
    if (!fs.existsSync(configs.email_path.Email_upload_destination)) {
      fs.mkdirSync(configs.email_path.Email_upload_destination, { recursive: true });
    }
    fs.copyFileSync(filePath, `${configs.email_path.Email_upload_destination}${uniqueFileName}`);
    const url = `${configs.email_path.Email_fileGetUrl}${uniqueFileName}`;
    attachments.push({ href: url, type: data.contentType, size: data.size, filename: data.filename, fileType: data.contentType });
  }
  logger.info('attachment', attachments);
  return attachments;
};
function receiveEmails(userConfig: any) {
  const imap = new Imap(userConfig);
  imap.connect();
  imap.once('ready', () => {
    logger.info(`📥 Connected to IMAP server for Tech user: ${userConfig.user}`);
    imap.openBox('INBOX', false, (err: any, box: any) => {
      if (err) {
        handleError(err);
        return;
      }
      fetchUnseenEmails();
      const interval = setInterval(fetchUnseenEmails, 5000);
      imap.once('end', () => {
        logger.info(`🔌 Disconnected from IMAP server for Tech user: ${userConfig.user}`);
        clearInterval(interval);
      });
    });
    setTimeout(() => {
      if (imap.state === 'authenticated') {
        imap.end();
      }
    },         600000);

  });
  imap.once('error', handleError);

  function fetchUnseenEmails() {
    imap.search(['UNSEEN'], (err, results) => {
      if (err) {
        handleError(err);
        return;
      }
      if (results.length === 0) {
        return;
      }
      const fetch = imap.fetch(results, { bodies: '' });

      const f = imap.fetch(results, {
        bodies: ['HEADER.FIELDS (FROM TO SUBJECT DATE)', 'TEXT'],
        struct: true,
        markSeen: true,
      });
      fetch.on('message', (msg, seqno) => {
        logger.info('Message #%d', seqno);
        if (seqno) {
          messageData.seqno = seqno;
        }
        const prefix = `(#${seqno}) `;
        let buffer: any = '';
        msg.on('body', (stream, info) => {
          stream.on('data', (chunk) => {
            buffer += chunk.toString('utf8');
          });

          stream.once('end', async () => {
            const _buffer = Imap.parseHeader(buffer);
            if (_buffer && _buffer.from) {
              messageData.date = _buffer.date;
              messageData.from = _buffer.from;
              messageData.to = _buffer.to;
              messageData.subject = _buffer.subject;
              messageData.messageId = _buffer['message-id']?.[0];
            }
          });
          simpleParser(stream, async (e: any, parsed: any) => {
            if (!parsed) return;

            const { html, text, textAsHtml, attachments, messageId } = parsed;
            if (!messageId) {
              logger.info('❌ No messageId, skipping...');
              return;
            }
            if (processedMessageIds.has(messageId)) {
              logger.info('❌ Duplicate messageId skipped:', messageId);
              return;
            }

            processedMessageIds.add(messageId);
            messageData.messageId = messageId;

            let baseHtml = textAsHtml || text || html || '';

            const outlookBodyMatch = baseHtml.match(/<div[^>]+direction:\s*ltr[^>]*>([\s\S]*?)<\/div>/i);
            if (outlookBodyMatch) {
              baseHtml = outlookBodyMatch[1];
            }
            baseHtml = baseHtml
              .replace(/<p[^>]*>\s*(NOTICE AND DISCLAIMER)[\s\S]*?<\/p>[\s\S]*$/i, '')
              .replace(/<p>--(.+?)--<\/p>/gis, '')
              .replace(/<img[^>]+src=["']cid:[^"']+["'][^>]*>/gi, '')
              .replace(/<(p|div|span)[^>]*style="([^"]*)"/gi, (match: any, tag: any, style: any) => {
                const alignMatch = style.match(/text-align\s*:\s*(left|center|right)/i);
                return alignMatch ? `<${tag} style="text-align:${alignMatch[1].toLowerCase()}"` : `<${tag}`;
              });
            baseHtml = baseHtml.replace(
              /NOTICE AND DISCLAIMER[\s\S]*$/i,
              ''
            );
            const formattedHtml = baseHtml
              .replace(/<br\s*\/?>/gi, ' ')
              .replace(/<\/p>\s*<p>/gi, '</p><br/><p>')
              .replace(/\*(.*?)\*/gs, '<b>$1</b>')
              .replace(/_(.*?)_/gs, '<i>$1</i>')
              .replace(/~(.*?)~/gs, '<u>$1</u>')
              .replace(/\[color:(.*?)\](.*?)\[\/color\]/gs, '<span style="color:$1">$2</span>')
              .replace(/\[bgcolor:(.*?)\](.*?)\[\/bgcolor\]/gs, '<span style="background-color:$1">$2</span>')
              .replace(/^\d+\.\s+(.*)$/gm, '<ol><li>$1</li></ol>')
              .replace(/^- (.*)$/gm, '<ul><li>$1</li></ul>')
              .replace(/\[align:(left|center|right)\]([\s\S]*?)\[\/align\]/gi, '<div style="text-align:$1">$2</div>')
              .replace(/\[indent\](.*?)\[\/indent\]/gs, '<div style="margin-left:20px">$1</div>')
              .replace(/\[indent2\](.*?)\[\/indent2\]/gs, '<div style="margin-left:40px">$1</div>')
              .replace(/\[outdent\](.*?)\[\/outdent\]/gs, '<div style="margin-left:0px">$1</div>')
              .replace(/^>\s?(.*)$/gm, '<blockquote>$1</blockquote>')
              .replace(/<p[^>]*>\s*(NOTICE AND DISCLAIMER)[\s\S]*?<\/p>[\s\S]*$/i, '');

            const strippedText = formattedHtml.replace(/<\/?[^>]+(>|$)/g, '');
            messageData.textAsHtml = escapeRegExp(formattedHtml, messageData.attachment);
            messageData.text = escapeRegExp(strippedText, messageData.attachment);
            const bodyData = removeReplyAllContent(messageData.textAsHtml);
            if (!bodyData || bodyData.trim() === '') {
              logger.info('❌ text was empty or only reply:', bodyData);
              return;
            }
            if (attachments?.length > 0) {
              messageData.attachments = await downloadAttachments(imap, attachments, './attachments');
            }

            const _data = {
              customerData: {
                fromData: messageData?.from[0] ?? undefined,
                toData: messageData?.to[0] ?? undefined,
                dateData: messageData?.date[0] ?? undefined,
                textData: messageData?.textAsHtml ?? undefined,
                subjectData: messageData?.subject[0]?.replace(/^(RE:|Re:|re:|FW:|Fw:|fw:|FWD:|Fwd:|fwd:)\s*/g, '')?.trim() ?? undefined,
                sessionStartTime: Date.now(),
                attachments: messageData?.attachments ?? [],
                messageId: messageData?.messageId ?? 0,
              },
              channelType: CHANNEL_TYPE?.EMAIL,
            };
            logger.info('✅ initialData', _data);
            await createEmailSession(_data);
            messageData = [{
              seqno: '', from: '', to: '', date: '',
              subject: '', text: '', textAsHtml: '',
              attachment: '', attachments: []
            }];
          });

        });

        msg.once('attributes', (attrs: any) => {
          if (attrs && attrs.struct) {
            const attachments = findAttachmentParts(attrs.struct, []);
            for (let i = 0, len = attachments?.length; i < len; i = i + 1) {
              const attachment = attachments[i];
              const ff = imap.fetch(attrs?.uid, {
                bodies: [attachment.partID],
                struct: true,
              });
              ff.on('message', buildAttMessageFunction(attachment, seqno));
            }
          }
        });
        msg.once('end', async () => {
          logger.info(`${prefix}fineshed`);
        });
      });
      // Event: headers
      fetch.on('headers', (headers: any) => {
        const msg = {
          headers: Imap.parseHeader(headers)
        };
        // Process the message headers
        processMessage(msg);
      });
      // Event: end
      fetch.once('end', () => {
        logger.info(`Fetched ${results.length} unseen message(s) for Tech user: ${userConfig.user}`);
      });
    });
  }
}
