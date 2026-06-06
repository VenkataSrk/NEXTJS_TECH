import * as CryptoJS from 'crypto-js';
import fs from 'fs';
import { v4 as uuidv4 } from 'uuid';
import { logger } from '../../../src/plugins/log';
import { insertNotificationDao } from '../../dao/email.dao';
import { sendMail } from '../../service/smtp';
import { CHANNEL_TYPE } from '../constants';

export async function handleCustomerSatisfaction(data: any, datas: any, req: any, auth: any) {
  logger.info('COMESSS111');
  try {
    const authToken = req.headers.authorization;
    const sendCustomerSatisfactionAsync = async () => {
      await sendCustomerSatisfaction(data, datas, req, authToken);
      logger.info('Email sent successfully');
    };
    if (data[0]?.isEnable === 1) {
      await sendCustomerSatisfaction(data, datas, req, authToken);
    } else {
      await sendCustomerSatisfactionAsync();
    }
  } catch (error) {
    logger.error('Error sending customer satisfaction email:', error);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function sendCustomerSatisfaction(data: any, datas: any, req: any, auth: any) {
  logger.info('COMMESSS222');
  try {
    const authToken = req.headers.authorization;
    const htmlContent = fs.readFileSync('src/helpers/customerSatisfication/customerSatisficationEmail.html', 'utf-8');

    const emailMessages = data[0]?.emailMessage;

    const customerName = data[0]?.customerName ? data[0]?.customerName : 'Customer';

    const updatedEmailMessage = emailMessages?.replace(/Customer Name/g, customerName);

    let updatedHtmlContent: any;
    const csatBaseUrl = process.env.CSAT;

    function encrypt(text: string, secretKey: string = process.env.SECRET_KEY): string {
      return CryptoJS.AES.encrypt(text, secretKey).toString();
    }

    const encryptedDomainId = encrypt(datas?.domainId.toString());
    const encryptedChannelTypeId = encrypt(datas?.channelTypeId.toString());
    const encryptedHistoryId = encrypt(datas?.historyId.toString());

    if (Array.isArray(updatedEmailMessage)) {
      updatedHtmlContent = htmlContent
        .replace('{{content}}', updatedEmailMessage.join(' '))
        .replace(/{{csat_link}}/g, `${csatBaseUrl}?domainId=${encryptedDomainId}&channelTypeId=${encryptedChannelTypeId}&historyId=${encryptedHistoryId}`);
    } else {
      updatedHtmlContent = htmlContent
        .replace('{{content}}', updatedEmailMessage?.replace(/['"]+/g, ''))
        .replace(/{{csat_link}}/g, `${csatBaseUrl}?domainId=${encryptedDomainId}&channelTypeId=${encryptedChannelTypeId}&historyId=${encryptedHistoryId}`);
    }

    logger.info('updatedHtmlContent', updatedHtmlContent);
    const emailData = {
      FromEmail: data[0]?.domainContact,
      ToEmail: data[0]?.customerContact,
      EmailSubject: data[0]?.emailSubject,
      text: updatedHtmlContent
    };
    await sendMail(emailData);
    logger.info('Email sent successfully');

  } catch (err) {
    req.log.error('Error:', err);
    logger.info('Error:', err);
  }
}

export const createAndInsertNotification = async (res: any, coid: any) => {
  const sendAtTimestamp = res[0]?.CreatedAt;
  const sendDateTime = new Date(sendAtTimestamp).toLocaleString();
  const email = res[0]?.FromEmail;
  const CustomerName = 'CustomerName';

  const notificationData = {
    uuid: uuidv4(),
    domainId: res[0]?.DomainId,
    toExt: null,
    notificationMsg: `Email received from ${CustomerName} (${email}) on ${sendDateTime}`,
    markAsRead: 0,
    timeStamp: Date.now(),
    type: CHANNEL_TYPE?.EMAIL,
    channelType: res[0]?.channelType,
    coid: coid.coid,
    customerValue: res[0]?.FromEmail
  };
  logger.info('notificationData>>>>', notificationData);
  try {
    const result = await insertNotificationDao(notificationData);
    logger.info('Notification inserted:', result);
  } catch (err) {
    logger.error('Error inserting notification:', err);
  }
};
