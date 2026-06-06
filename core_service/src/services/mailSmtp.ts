import nodemailer from 'nodemailer';
import { logger } from '../plugins/winston';

class SmtpConfigLeadConfig {
  public server: any;
  public imap: any;
  public simpleParser: any;

  constructor() {
    this.server = nodemailer.createTransport({
      host: process.env.MAIL_SMTP_HOST,
      port: Number(process.env.MAIL_SMTP_PORT),
      secure: false,
      requireTLS: true,
      auth: {
        user: process.env.MAIL_SMTP_USER,
        pass: process.env.MAIL_SMTP_PASS,
      },
      tls: {
        minVersion: 'TLSv1',
        rejectUnauthorized: false,
      },
    });
  }

  public async getServer() {
    return this.server;
  }
}

export const sendEmailWithOtp = async (data: any) => {
  const ob = new SmtpConfigLeadConfig();
  const smtp = await ob.getServer();
  try {
    let res: any = null;
    console.log('asdasdasdasd', {
      from: process.env.SUPPORT_EMAIL,
      to: data.toId,
      cc: data.cc ?? '',
      bcc: data.bcc ?? '',
      subject: data.emailSubject,
      html: data.htmlContent,
      attachments: data?.attachments ?? []
    });
    res = await new Promise((resolve, reject) => {
      smtp.sendMail(
        {
          from: process.env.SUPPORT_EMAIL,
          to: data.toId,
          cc: data.cc ?? '',
          bcc: data.bcc ?? '',
          subject: data.emailSubject,
          html: data.htmlContent,
          attachments: data?.attachments ?? []
        },
        (error: any, info: any) => {
          smtp.close();
          if (error) {
            reject(error);
          } else {
            resolve(info);
          }
        }
        );
    });
    return res;
  } catch (err) {
    smtp.close();
    logger.error('sendEmailWithOtp', err);
    return null;
  }
};
