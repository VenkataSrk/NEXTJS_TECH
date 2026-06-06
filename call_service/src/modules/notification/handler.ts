import { result } from 'lodash';
import { ioredisChat } from '../../../src/plugins/db';
import { deleteNotificationDao, getNotificationDao, insertNotificationDao, updateMarkAsReadAllNotificationDao, updateMarkAsReadNotificationDao } from '../../dao/notification.dao';
/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

import { logger } from '../../../src/plugins/winston';
import { RESPONSE } from '../../helpers/constants';

// insert notification
export async function insertNotification(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      uuid: req.body.uuid,
      toExt: `${req.body.toExt}`,
      domainId: auth?.domainId ?? req.body?.domainId ?? req.body?.socialMediaDomainId ?? 0,
      notificationMsg: req.body.notificationMsg,
      channelType: req.body.channelType,
      markAsRead: 0,
      type: req.body.type,
      timeStamp: new Date().getTime(),
      coid: req?.body?.coid ?? null,
      customerValue: req?.body?.customerValue ?? null,
    };
    const insertStatus: any = await insertNotificationDao(data);
    ioredisChat.to(`${data?.domainId}`).emit('refreshNotification');
    res.status(200).send({ statusCode: 200, message: RESPONSE.notification_success_status, result: insertStatus[0] });
  } catch (err) {
    logger.error('insertNotification catch', { data: req?.body, Err: err });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

// get notifications
export async function getNotification(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      ext: auth.ext,
      domainId: auth.domainId,
      timeStamp: req?.body?.timeStamp ?? 0,
      limit: req?.body?.limit,
      offset: req?.body?.offset ?? 0
    };
    const getStatus: any = await getNotificationDao(data);
    if (Array.isArray(getStatus) && getStatus.length > 0) {
      res.status(200).send({
        statusCode: 200, message: RESPONSE.success_message,
        read: getStatus[0],
        unRead: getStatus[1],
        unReadCount: getStatus[2],
        readCount: getStatus[3]
      });
    } else {
      res.send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {
    console.log('getNotification catch', req?.body, 'error', err);
    logger.error('getNotification catch', { data: req?.body, Err: err });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

// delete a notification
export async function deleteNotification(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      ext: auth.ext,
      domainId: auth.domainId,
      nId: req?.body?.nId,
    };
    const deleteNotifications: any = await deleteNotificationDao(data);
    if (Array.isArray(deleteNotifications) && deleteNotifications.length > 0) {
      res.status(200).send({
        statusCode: 200, message: RESPONSE.success_message, result: deleteNotifications[0]
      });
    } else {
      res.send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {
    console.log('deleteNotification catch', req?.body, 'error', err);
    logger.error('deleteNotification catch', { data: req?.body, Err: err });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

// update notification
export async function updateMarkAsReadNotification(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      domainId: auth.domainId,
      nId: req?.body?.nId,
    };
    const updateMarkAsRead: any = await updateMarkAsReadNotificationDao(data);
    if (Array.isArray(updateMarkAsRead) && updateMarkAsRead.length > 0) {
      res.status(200).send({
        statusCode: 200, message: RESPONSE.success_message, result: updateMarkAsRead[0]
      });
    } else {
      res.send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {
    console.log('updateMarkAsReadNotification catch', req?.body, 'error', err);
    logger.error('updateMarkAsReadNotification catch', { data: req?.body, Err: err });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

// update notification
export async function updateMarkAsReadAllNotification(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      domainId: auth?.domainId,
      toExt: auth?.ext
    };
    const updateMarkAsReadAll: any = await updateMarkAsReadAllNotificationDao(data);
    if (Array.isArray(updateMarkAsReadAll) && updateMarkAsReadAll.length > 0) {
      res.status(200).send({
        statusCode: 200, message: RESPONSE.success_message, result: updateMarkAsReadAll[0]
      });
    } else {
      res.send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {
    console.log('getNotification catch', req?.body, 'error', err);
    logger.error('getNotification catch', { data: req?.body, Err: err });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}
