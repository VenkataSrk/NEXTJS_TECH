
import { logger } from '../../../src/plugins/winston';
import {
  getAllQueueWaitTime, getQueueWaitTime, insert
} from '../../dao/queueWaitTime.dao';

/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

import { RESPONSE } from '../../helpers/constants';

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getQueueWaitTimeHandlers(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      qid: req.params.qid,
      domainId: auth.domainId,
    };
    if (data.qid !== '') {
      const getQueuList: any = await getQueueWaitTime(data);
      if (getQueuList) {
        res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getQueueWaitTimeList: getQueuList });
      } else {
        res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getQueueWaitTimeList: [] });
      }
    } else {
      res.status(200).send({ statusCode: 424, message: 'qid no should not be empty' });
    }
  } catch (err) {
    logger.error('getQueueWaitTimeHandlers catch', { data: req.params.qid, domainId: req?.headers?.domainId, Err: err });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getAllQueueWaitTimeHandlers(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      domainId: auth.domainId,
    };
    const getAllQueue: any = await getAllQueueWaitTime(data);
    if (Array.isArray(getAllQueue) && getAllQueue[0].length > 0) {
      // const queues = getAllQueue[0];
      // queues.forEach((queue: any) => {
      //   const queueList = JSON.parse(queue.queueList);
      //   queueList.forEach((item: any) => {
      //     item.currentTime = Math.floor(Date.now() / 1000);
      //   });
      //   queue.queueList = JSON.stringify(queueList);
      // });
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getAllQueueList: getAllQueue[0],
      });
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        getAllQueueList: [],
      });
    }
  } catch (err) {
    logger.error('getQueueWaitTimeHandlers catch', 'error', { domainId: req?.headers?.domainId, Err: err });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function insertQueue(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const body: any = req.body;
    const data: any = {
      domainId: auth.domainId,
      callHistoryId: body?.callHistoryId,
      agentId: body?.agentId ?? null,
      queueId: body?.queueId
    };
    if (data?.callHistoryId) {
      const response: any = await insert(data);
      if (Array.isArray(response) && response?.length > 0 && response[0]?.errCode === 1) {
        res.status(200).send({
          statusCode: 200,
          message: RESPONSE.success_message,
        });
      } else {
        res.status(200).send({
          statusCode: 422,
          message: RESPONSE.failed,
        });
      }
    } else {
      res.status(200).send({
        statusCode: 424,
        message: 'CallHistoryId should not be empty',
      });
    }
  } catch (err) {
    logger.error('insertQueue catch', { data: req?.body, Err: err });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}
