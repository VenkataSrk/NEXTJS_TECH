import { v4 as uuidv4 } from 'uuid';
import { mysqlPoolConnection } from '../plugins/db';
import { loggerError, loggerTrace } from '../plugins/logger';
import { getCustomerDetails, updateCallBlastCompleted } from './callflow.dao';

/*
 * create missed notification
 * @param data
 * @returns
 */
export const insertNotificationDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call ccaas_notification_details(?,?,?,?,?,?,?,?,?,?)', [
        data.uuid,
        data.domainId,
        data.toExt,
        data.notificationMsg,
        0,
        data.timeStamp,
        data.type,
        data.channelType,
        data.coid,
        data.customerValue,
      ],                        (err: any, result: any) => {
        loggerTrace('ccaas_notification_details', [data.uuid, data.domainId, data.toExt, data.notificationMsg, 0, data.timeStamp, data.type, data.channelType, data.coid, data.customerValue], result);
        if (err) {
          loggerError('ccaas_notification_details', [data.uuid, data.domainId, data.toExt, data.notificationMsg, 0, data.timeStamp, data.type, data.channelType, data.coid, data.customerValue], err);
          reject(err);
        }
        resolve(result);
      });
    } catch (err) {
      loggerError('ccaas_notification_details', data, err);
      reject(err);
    }
  });
};

/**
 * create voicemail notification
 * @param data
 * @returns
 */
export const insertNotificationVoiceMailDao = (channel: any, redis: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      const time = new Date().getTime();
      const customerNumber = channel?.call_blast === 1 ? channel?.ddi : channel?.cli;
      const customerDetails: any = await getCustomerDetails(customerNumber);
      const customerName = `${customerDetails?.[0]?.firstName || ''} ${customerDetails?.[0]?.lastName || ''}`.trim();
      const incomingCallData = {
        id: uuidv4(),
        type: 'voicemail',
        domainId: channel?.domain_id,
        message: `Voicemail call from ${customerDetails?.[0]?.firstName ? customerName : customerNumber}`,
      };

      mysqlPoolConnection.query('call ccaas_notification_details(?,?,?,?,?,?,?,?,?,?)', [
        incomingCallData.id,
        incomingCallData.domainId,
        null,
        incomingCallData.message,
        0,
        new Date().getTime(),
        incomingCallData.type,
        'Call',
        customerDetails?.[0]?.coid ?? null,
        customerNumber
      ],                        (err: any, result: any) => {
        loggerTrace('ccaas_notification_details', [incomingCallData.id, incomingCallData.domainId, null, incomingCallData.message, 0, time, incomingCallData.type, 'Call', customerDetails?.[0]?.coid ?? null, customerNumber], result);
        if (err) {
          loggerError('ccaas_notification_details', [incomingCallData.id, incomingCallData.domainId, null, incomingCallData.message, 0, time, incomingCallData.type, 'Call', customerDetails?.[0]?.coid ?? null, customerNumber], err);
          reject(err);
        }
        redis.to(`${channel?.domain_id}`).emit('notification', incomingCallData);
        resolve(result);
      });
    } catch (err) {
      loggerError('ccaas_notification_details', channel, err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @param redis
 * @returns
 */
export const insertNotificationCallBlastDao = (data: any, redis: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      const uuid = uuidv4();
      const time = new Date().getTime();
      await updateCallBlastCompleted(data);
      if (data?.campaignStatus !== 6) {
        await updateCallBlastCompleted(data);
      }
      const campaignStatus = data?.campaignStatus === 4 ? 'completed' : data?.campaignStatus === 3 ? 'force stop' : data?.campaignStatus === 6 ? 'completed for today’s schedule' : '';
      mysqlPoolConnection.query('call ccaas_notification_details(?,?,?,?,?,?,?,?,?,?)', [
        uuid,
        data?.domain_id,
        null,
        `Outbound campaign <b>${data?.campaignName}</b> has been ${campaignStatus} successfully.`,
        0,
        time,
        'Outbound campaign',
        'Call',
        null,
        null
      ],                        (err: any, result: any) => {
        loggerTrace('ccaas_notification_details', [uuid, data?.domain_id, null, `Outbound campaign <b>${data?.campaignName}</b> has been ${campaignStatus} successfully.`, 0, time, 'Outbound campaign', 'Call', null, null], result);
        if (err) {
          loggerError('ccaas_notification_details', [uuid, data?.domain_id, null, `Outbound campaign <b>${data?.campaignName}</b> has been ${campaignStatus} successfully.`, 0, time, 'Outbound campaign', 'Call', null, null], err);
          reject(err);
        }
        redis.to(`${data?.domain_id}`).emit('refreshNotification');
        resolve(result);
      });
    } catch (err) {
      loggerError('ccaas_notification_details', data, err);
      reject(err);
    }
  });
};
