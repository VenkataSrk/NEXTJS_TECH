import nodemailer from 'nodemailer';

class EmailConfig {
  public server: any;
  public imap: any;
  public simpleParser: any;

  constructor(emailData: any) {
    this.server = nodemailer.createTransport({
      host: 'mail.worktual.tech',
      port: 587,
      secure: false,
      auth: {
        user: emailData,
        pass: 'admin123',
      },
      tls: {
        rejectUnauthorized: false,
      },
    });
  }

  public async getServer() {
    return this.server;
  }

  public async testMail(config = { from: '', to: '', subject: '', content: '', attachmentsData: [], attachmentsPdf: [], attachmentsXls: [], attachmentsXlsl: [] }) {
    try {

      const attachmentsAll = [
        config.attachmentsData && {
          filename: 'reportFile.csv',
          content: config.attachmentsData,
        },
        config.attachmentsPdf && {
          filename: 'reportFile.pdf',
          content: config.attachmentsPdf,
        },
        config.attachmentsXls && {
          filename: 'reportFile.xls',
          content: config.attachmentsXls,
        },
        config.attachmentsXlsl && {
          filename: 'reportFile.xlsx',
          content: config.attachmentsXlsl,
        },
      ].filter(Boolean);

      const hasInvalidAttachment = attachmentsAll.some(({ content }: any) =>
        Array.isArray(content) ||
        !(typeof content === 'string' || Buffer.isBuffer(content) || content instanceof Uint8Array || content instanceof DataView)
      );

      await this.server.sendMail({
        from: config.from,
        to: config.to,
        subject: config.subject,
        html: config.content,
        // attachments: attachmentsAll,
        ...(hasInvalidAttachment ? {} : { attachments: attachmentsAll }),
      });
      console.log('Email sent successfully');
    } catch (error) {
      console.error('Error sending email:', error);
      // throw error;
    }
  }
}
export default EmailConfig;
