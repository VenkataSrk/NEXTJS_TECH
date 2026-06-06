import axios from 'axios';
import * as CryptoJS from 'crypto-js';
import { v4 as uuidv4 } from 'uuid';
import { insertNotificationDao } from '../../dao/smsDao';
import { loggerError, loggerTrace } from '../../utils/log';

export async function handleCustomerSatisfaction(
  data: any,
  datas: any,
  req: any,
  auth: any
) {
  try {
    const authToken = req.headers.authorization;

    // Send the customer satisfaction email
    const sendCustomerSatisfactionAsync = async () => {
      await sendCustomerSatisfaction(data, datas, authToken);
      loggerTrace('sendCustomerSatisfaction- Email Sent', { data, datas }, {});
      console.log('Email sent successfully');
    };

    // Check if isEnable is 1
    if (data[0]?.isEnable === 1) {
      // Call sendCustomerSatisfaction directly if isEnable is 1
      await sendCustomerSatisfaction(data, datas, authToken);
      loggerTrace('sendCustomerSatisfaction Email Sent', { data, datas }, {});
    } else {
      // Otherwise, proceed with the async function call
      await sendCustomerSatisfactionAsync();
    }
  } catch (error) {
    loggerError('sendCustomerSatisfaction - Error', error, { data, datas, auth });
    throw error;
  }
}

// send Message Customer
export async function sendCustomerSatisfaction(data: any, datas: any, auth: any) {

  // Function to encrypt a string using CryptoJS
  function encrypt(text: string, secretKey: string = process.env.SECRET_KEY): string {
    return CryptoJS.AES.encrypt(text, secretKey).toString();
  }

  try {
    const encryptedDomainId = encrypt(datas?.domainId.toString());
    const encryptedChannelTypeId = encrypt(datas?.channelTypeId.toString());
    const encryptedHistoryId = encrypt(datas?.historyId.toString());

    const link = `${process.env.CSATSMS}?domainId=${encryptedDomainId}&channelTypeId=${encryptedChannelTypeId}&historyId=${encryptedHistoryId}`;
    console.log('link', link);

    const customerName = data[0]?.customerName ? data[0].customerName : 'customer';
    const updatedMessage = data[0]?.smsMessage.replace('[Customer Name]', customerName).replace(/<\/?[^>]+(>|$)/g, '').replace(/"/g, '');
    const updatedHtmlContent = `${updatedMessage} ${link}`;
    const body = {
      source: data[0]?.domainContact,
      destination: data[0]?.customerContact,
      content: updatedHtmlContent
    };
    console.log('body:', body);

    const headers = {
      'Content-Type': 'application/json',
      Authorization: `${auth?.authorization || process?.env?.SMS_API_TOKEN}`, // Use the token from auth or environment variable
    };

    console.log('Using API Token:', headers.Authorization);
    await axios.post(process?.env?.smsOut, body, { headers });
    return body;
  } catch (error) {
    console.error('Error sending message:', error);
    return { error: 'Failed to send the SMS' };
  }
}

export const createAndInsertNotification = async (smsCcaasTable: any, coid: any) => {

  const sendAtTimestamp = smsCcaasTable?.createdAt;
  const sendDateTime = new Date(sendAtTimestamp).toLocaleString();
  const sms = smsCcaasTable?.customerNumber;
  const CustomerName = 'CustomerName';

  const notificationData = {
    uuid: uuidv4(),
    domainId: smsCcaasTable?.domainId,
    toExt: null,
    notificationMsg: `Sms received from ${CustomerName} (${sms}) on ${sendDateTime}`,
    markAsRead: 0,
    timeStamp: Date.now(),
    type: 'Sms',
    channelType: smsCcaasTable?.channelType,
    coid: coid.coid,
    customerValue: smsCcaasTable?.customerNumber
  };
  try {
    const result = await insertNotificationDao(notificationData);
    return result;
  } catch (err) {
    loggerError('Error inserting notification:', err, {});
    console.error('Error inserting notification:', err);
  }
};
