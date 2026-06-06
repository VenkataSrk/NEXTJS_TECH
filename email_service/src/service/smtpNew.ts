import { logger } from '../../src/plugins/log';
import { getUnsubscribeUser } from '../dao/email.dao';
const nodemailer = require('nodemailer');
const CryptoJS = require('crypto-js');

// send smtp email notification
export async function sendMailSmtp(data: any, smtp: any, text:any) {
  const jsonData = JSON.parse(smtp?.smtp_json || '{}');
  const host = jsonData?.host;
  const port = parseInt(jsonData?.port, 10);
  const isSecure = port === Number(process.env.SEND_MAIL_SMTP_PORT);
  const transporter = nodemailer.createTransport({
    host,
    port,
    secure: isSecure,
    auth: {
      user: smtp?.emailAddress,
      pass: jsonData?.password
    },
    tls: {
      rejectUnauthorized: false
    }
  });

  // Check unsubscribe status first
  // const Unsubscribe: any = await getUnsubscribeUser(data);
  // if (Unsubscribe?.length) {
  //   logger.info('⚠️ User is unsubscribed. Email not sent.');
  //   return;
  // }

  // Build unsubscribe link
  // const user = {
  //   Cid: data?.FromEmail,
  //   Uid: data?.ToEmail
  // };
  // const stringifiedObj = JSON.stringify(user);
  // const encrypted = CryptoJS.AES.encrypt(stringifiedObj, 'un_00').toString();
  // const unsubscribeUrl = `${process.env.UNSUBSCRIBE_URL}/unsubscribe?Id=${encrypted}`;

  // Prepare email body with unsubscribe footer
  // const emailBody = `${data?.text ?? ''}<br><br><a href="${unsubscribeUrl}" target="_blank" style="color:#007BFF;">Unsubscribe</a>`;
  try {
    const info = await transporter.sendMail({
      from: data?.FromEmail ?? smtp?.emailAddress ?? '',
      to: data?.ToEmail ?? '',
      cc: data?.cc ?? '',
      bcc: data?.bcc ?? '',
      subject: data?.EmailSubject ?? 'No Subject',
      html: text ?? data?.text ?? '',
      attachments: data?.attachments ?? [],
      messageId: data?.messageId ?? null,
      inReplyTo: data?.messageId ?? null,
      references: data?.messageId ?? null,
      // headers: {
      //   'List-Unsubscribe': `<${unsubscribeUrl}>`
      // }
    });
    logger.info('✅ Email sent:', info.messageId);
    const previewUrl = nodemailer.getTestMessageUrl(info);
    if (previewUrl) {
      logger.info('🔍 Preview URL:', previewUrl);
    }
  } catch (error: any) {
    logger.error('❌ Failed to send email:', error.message);
  }
}
