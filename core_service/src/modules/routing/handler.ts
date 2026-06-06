import {
  deleteRouting,
  // getContactInfo,
  // getProductContacts,
  getRouting,
  getSearchList,
  isRoutingEnableList,
  isValidRoutingName,
  routingProfileStatus,
  routingProfileUser,
  routingqueue,
  updateRouting,
  updateStatus,
} from '../../dao/routing';
import { logger } from '../../plugins/log';

import { RESPONSE } from '../../helpers/constants';

/**
 *
 * @param req
 * @param res
 * @param done
 * ccs login api
 */
export async function routingQueueHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const mergeRoutingQueue = [];
    const availableData = {};
    for (const entry of req?.body?.routingqueue) {
      const key = `${entry?.qid}-${entry?.priority}-${entry?.delay_in_seconds}`;
      const currentData = availableData[key];
      if (currentData) {
        availableData[key] = {
          ...entry,
          voice: entry?.voice + currentData?.voice > 0 ? 1 : 0,
          chat: entry?.chat + currentData?.chat > 0 ? 1 : 0,
        };
      } else {
        availableData[key] = entry;
      }
    }
    mergeRoutingQueue.push(...Object.values(availableData));
    const data: any = {
      name: req.body.name ? req.body.name : null,
      description: req.body.description ? req.body.description : null,
      default_outbound_queue: req.body.default_outbound_queue ? req.body.default_outbound_queue : null,
      No_of_linked_queues: mergeRoutingQueue?.length ?? 0,
      status: req.body.status ? req.body.status : 0,
      domainId: auth.domainId,
      routingMethod: req.body.routingMethod ? req.body.routingMethod : null,
      concurrentChat: req.body.concurrentChat ? req.body.concurrentChat : null,
      routingqueue: mergeRoutingQueue ?? null,
    };
    const insertQueue: any = await routingqueue(data);
    logger.info('routingqueue', insertQueue);
    if (insertQueue) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.routing_insert_successfully,
        insertQueueList: insertQueue ?? {},
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {

    logger.error('routingQueueHandler', req.headers, req.body , err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getRoutingHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const domainId = auth.domainId;
    const rid = req.params.rid;
    if (rid !== '') {
      const getRoutingRes: any = await getRouting(rid, domainId);
      logger.info('getRouting', getRoutingRes);
      if (getRoutingRes) {
        res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getRoutingReslist: getRoutingRes });
      } else {
        res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getRoutingRes: {} });
      }
    } else {
      res.status(200).send({ statusCode: 424, message: 'rid number should not be empty' });
    }
  } catch (err) {
    logger.error('getRoutingHandler', req.headers, req.params , err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * ccs login api
 */
export async function updateRoutingHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const dataq = req.params.rid;
    const mergeRoutingQueue = [];
    const availableData = {};
    for (const entry of req?.body?.routingqueue) {
      const key = `${entry?.qid}-${entry?.priority}-${entry?.delay_in_seconds}`;
      const currentData = availableData[key];
      if (currentData) {
        availableData[key] = {
          ...entry,
          voice: entry?.voice + currentData?.voice > 0 ? 1 : 0,
          chat: entry?.chat + currentData?.chat > 0 ? 1 : 0,
        };
      } else {
        availableData[key] = entry;
      }
    }
    mergeRoutingQueue.push(...Object.values(availableData));
    const data: any = {
      name: req.body.name ? req.body.name : null,
      description: req.body.description ? req.body.description : null,
      default_outbound_queue: req.body.default_outbound_queue ? req.body.default_outbound_queue : null,
      No_of_linked_queues: mergeRoutingQueue?.length ?? 0,
      status: req.body.status ? req.body.status : 0,
      domainId: auth.domainId,
      routingMethod: req.body.routingMethod ? req.body.routingMethod : null,
      concurrentChat: req.body.concurrentChat ? req.body.concurrentChat : null,
      type: req.body.type ? req.body.type : null,

      routingqueue: mergeRoutingQueue ?? null,
    };
    const updateQueue: any = await updateRouting(data, dataq);
    logger.info('updateRouting', updateQueue);
    if (updateQueue) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.routing_updated_successfully,
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {

    logger.error('updateRoutingHandler', req.headers, req.params, req.body , err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function deleteRoutingHandler(req: any, res: any, done: any) {
  try {
    const dataq = req.params.rid;
    const deleteRoutingRes: any = await deleteRouting(dataq);
    logger.info('deleteRouting', deleteRoutingRes);
    if (deleteRoutingRes) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.routing_deleted_successfully });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {

    logger.error('deleteRoutingHandler', req.params, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}
/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getRoutingListHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {};
    data.domainId = auth.domainId;
    data.limit = req.query.limit;
    data.offset = req.query.offset;
    data.searchBy = req.query.searchBy;
    data.searchByValue = req.query.searchByValue;
    const getRoutingl: any = await getSearchList(data);
    logger.info('getSearchList', getRoutingl);
    if (getRoutingl && getRoutingl.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getRoutingListRes: getRoutingl });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getRoutingListRes: [] });
    }
  } catch (err) {

    logger.error('getRoutingListHandler', req.headers, req.query , err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function routingProfileStatusHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const rid = req.params.rid;
    const domainId = auth.domainId;
    if (rid !== '' || rid !== undefined || rid !== null) {
      const userRes: any = await routingProfileUser(rid, domainId);
      logger.info('routingProfileUser', userRes);
      const statusRes: any = await routingProfileStatus(rid);
      logger.info('routingProfileStatus', statusRes);
      if (userRes?.routing_profile === statusRes?.rid) {
        res.status(200).send({ statusCode: 200, message: RESPONSE.Disabled, status: 0 });
      } else {
        res.status(200).send({ statusCode: 200, message: RESPONSE.IdDisabled, status: 1 });
      }
    } else {
      res.status(200).send({ statusCode: 405, message: RESPONSE.cid });
    }
  } catch (err) {

    logger.error('routingProfileStatusHandler', req.headers, req.params , err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * update status api
 */
export async function updateStatusHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const rid = req.params.rid;
    const data: any = {
      status: req.body.status,
      domainId: auth.domainId,
    };
    const updateStatusRes: any = await updateStatus(data, rid);
    logger.info('updateStatus', updateStatusRes);
    if (updateStatusRes.affectedRows === 1) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.stataus_updated_successfully,
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_update });
    }
  } catch (err) {

    logger.error('updateStatusHandler', req.headers, req.params, req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function isValidRoutingNameHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {};
    data.domainId = auth.domainId;
    data.name = req?.body?.name ?? null;
    const routingRes: any = await isValidRoutingName(data);
    logger.info('isValidRoutingName', routingRes);
    if (routingRes && routingRes.length > 0 && routingRes[0]?.name.length > 0) {
      res.status(200).send({ statusCode: 403, message: RESPONSE.name_already_exist, flag: 0 });
    } else {
      res.status(200).send({ statusCode: 200, message: RESPONSE.available, flag: 1 });
    }
  } catch (err) {

    logger.error('isValidRoutingNameHandler', req.headers, req.body , err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function isRoutingEnableListHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {};
    data.domainId = auth.domainId;
    const getEnable: any = await isRoutingEnableList(data);
    logger.info('isRoutingEnableList', getEnable);
    if (Array.isArray(getEnable) && getEnable.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getRoutingEnableListRes: getEnable });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getRoutingEnableListRes: [] });
    }
  } catch (err) {

    logger.error('isRoutingEnableListHandler', req.headers, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}
