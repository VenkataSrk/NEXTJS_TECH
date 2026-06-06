import { logger } from '../../src/plugins/log';

const nodemailer = require('nodemailer');
class EmailConfig {
  public server: any;
  public imap: any;
  public simpleParser: any;

  constructor() {
    logger.info('constructor');
    this.server = nodemailer.createTransport({
      host: process.env.SMTP_HOST,
      port: process.env.SMTP_PORT,
      secure: false,
      requireTLS: true,
      auth: {
        user: process.env.SMTP_USER,
        pass: process.env.SMTP_PASSWORD,
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

  public async testMail(config = { from: '', to: '', subject: '', content: '' }) {
    logger.info('email testmail comes');
    try {
      await this.server.sendMail({
        from: config.from,
        to: config.to,
        subject: config.subject,
        html: config.content,
      });
      logger.info('Email sent successfully');
    } catch (error) {
      logger.error('Error sending email:', error);
      // throw error;
    }
  }
}
export default EmailConfig;
