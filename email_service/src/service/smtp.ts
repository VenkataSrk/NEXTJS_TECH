const nodemailer = require('nodemailer');
const CryptoJS = require('crypto-js');
import { logger } from '../../src/plugins/log';
import { getUnsubscribeUser } from '../dao/email.dao';
export class SmtpConfig {
  private server: any;

  constructor(user: string, password: string) {
    this.server = nodemailer.createTransport({
      host: process.env.SMTP_HOST,
      port: Number(process.env.SMTP_PORT),
      secure: false,
      auth: {
        user,
        pass: password,
      },
      pool: true,
      maxConnections: 5,
      maxMessages: 100,
      tls: {
        rejectUnauthorized: false,
      },
    });
  }
  public async getServer() {
    return this.server;
  }
}

/**
 *
 * @param data
 */
export const sendMail = async (data: any) => {
  logger.info('sendMail>>>>>>>>', data);
  const ob = new SmtpConfig(data.FromEmail, 'admin123');
  const smtp = await ob.getServer();
  try {
    const result = await smtp.sendMail({
      from: data?.FromEmail ?? null,
      to: data?.ToEmail ?? null,
      cc: data?.cc ?? null,
      bcc: data?.bcc ?? null,
      subject: data?.EmailSubject ?? null,
      html: data?.text,
      attachments: data?.attachments ?? [],
      inReplyTo: data?.messageId ?? null,
      references: data?.messageId ?? null
    });
    logger.info('Email sent successfully >>>>>', result?.messageId);
  } catch (err) {
    smtp.close();
    logger.error('sendMail - Unexpected Error:', err);
  }
};

/**
 * @param data
 */
export const sendMailTemplate = async (data: any, html: any) => {
  const ob = new SmtpConfig(data.FromEmail, 'welcome123');
  const smtp = await ob.getServer();
  try {
    const htmlContent = `
      <!DOCTYPE html>
<html>
  <head>
    <title>Email Template</title>
    <meta name="googlebot" content="noindex" />
    <meta content="width=device-width, initial-scale=1.0" name="viewport">
    <META NAME="ROBOTS" CONTENT="NOINDEX, NOFOLLOW" />
    <style>
          ${data?.text || ''}
    </style>
  </head>
     ${html || ''}
</html>`;
    const res = await smtp.sendMail({
      from: data?.FromEmail ?? null,
      to: data?.ToEmail ?? null,
      subject: data?.EmailSubject ?? null,
      html: htmlContent,
    });
    smtp.close();
    logger.info('Email sent successfully');
  } catch (err) {
    smtp.close();
    logger.error(err);
  }
};
