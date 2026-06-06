import { getUnsubscribeUser } from '../dao/email.dao';
import { logger } from '../plugins/log';
const nodemailer = require('nodemailer');
const CryptoJS = require('crypto-js');

export const sendMailAudracare = async (data:any) => {
  const transporter = nodemailer.createTransport({
    host: process.env.AUDRACARE_HOST,
    port: process.env.SMTP_PORT,
    secure: false,
    auth: {
      user: process.env.AUDRACARE_USER,
      pass: process.env.AUDRACARE_PASSWORD,
    },
    tls: true
  });
  const user = {
    Cid: data?.FromEmail,
    Uid: data?.ToEmail
  };
  const stringifiedObj = JSON.stringify(user);
  const encrypted = CryptoJS.AES.encrypt(stringifiedObj, 'un_00').toString();
  const unsubscribeUrl = `${process.env.UNSUBSCRIBE_URL}/unsubscribe?Id=${encrypted}`;

  const emailBody = `${data.text}<br><br><a href="${unsubscribeUrl}" target="_blank" style="color:#007BFF;">Unsubscribe</a>`;
  const mailOptions = {
    from: data.FromEmail,
    to: data.ToEmail,
    cc: data.cc,
    bcc: data.bcc,
    subject: data.EmailSubject,
    html: emailBody,
    attachments: data?.attachments ?? [],
    headers: {
      'List-Unsubscribe': `<${unsubscribeUrl}>`,
    }
  };

  try {
    const Unsubscribe:any = await getUnsubscribeUser(data);
    if (Unsubscribe?.length !== 0) {
      logger.info('User is unsubscribed. Email not sent.');
      return;
    }

    const info = await transporter.sendMail(mailOptions);
    logger.info('✅ Email sent:', info.messageId);
  } catch (error) {
    logger.error('sendMailAudracare - Unexpected Error:', error);
  } finally {
    transporter.close();
  }
};
