/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */
import {
  checkQueueMapping,
  customHoursStatus,
  deleteBusinessHolidaysDao,
  deleteCustomHours,
  get,
  getBusinessHolidaysDao,
  getCustomHoursDetail,
  getCustomHoursDetailById,
  getCustomHoursList,
  getQueueDetails,
  getSearchList,
  insert,
  insertCustomHours,
  insertUpdateBusinessHolidaysDao,
  isValidCustomHoursName,
  isValidQueueName,
  queueDelete,
  update,
  updateBusinessHolidaysStatusDao,
  updateCustomHours,
  updateCustomHoursById,
  updateQueue,
  updateStatus,
} from '../../dao/queue';
import { logger } from '../../plugins/log';

import { RESPONSE } from '../../helpers/constants';

/**
 *
 * @param req
 * @param res
 * @param done
 * ccs login api
 */
export async function insertQueueHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      name: req?.body?.name ?? null,
      description: req?.body?.description ?? null,
      domainId: auth.domainId,
      OutboundCallerId: req?.body?.OutboundCallerId ?? null,
      outBoundNumber: req?.body?.outBoundNumber ?? null,
      maxCallHandlingTimeMM: req?.body?.maxCallHandlingTimeMM ?? null,
      callWrapUpTime: req?.body?.callWrapUpTime ?? null,
      hours_of_operation: req?.body?.hours_of_operation ?? null,
      outBoundCallStatus: req?.body?.outBoundCallStatus === 0 ? false : true,
      settingAudio: req?.body?.settingAudio ?? null,
      announcement: req?.body?.announcement ?? null,
      maximumCallers: req?.body?.maximumCallers ?? null,
      maximumWaitTime: req?.body?.maximumWaitTime ?? null,
      hoursOfOperatioId: req?.body?.hoursOfOperatioId ?? null,
      outOfBusinessHours: req?.body?.outOfBusinessHours ?? null,
      status: req?.body?.status ?? 1,
      type: req?.body?.type ?? null
    };
    const insertQueue: any = await insert(data);
    logger.info('insert', insertQueue);
    if (insertQueue) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.queue_insert_success, insertQueueList: insertQueue ?? {} });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {

    logger.error('insertQueueHandler', req.headers, req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function getQueueHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const qid = req.params.qid;
    const data: any = {};
    data.domainId = auth.domainId;
    const getQueue: any = await get(qid, data);
    logger.info('get', getQueue);
    if (Array.isArray(getQueue) && getQueue.length > 0) {
      const getquedetails: any = getQueue;
      const qw: any = getQueue;
      getquedetails.map((_, index) => (getquedetails[index].outBoundCallStatus = qw[0].outBoundCallStatus === 0 ? false : true));
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getQueueList: getquedetails,
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getQueue: [] });
    }
  } catch (err) {

    logger.error('getQueueHandler', req.headers, req.params, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function updateQueueHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const qdata: any = req.params.qid;
    const data: any = {
      name: req?.body?.name ?? null,
      description: req?.body?.description ?? null,
      domainId: auth.domainId,
      OutboundCallerId: req?.body?.OutboundCallerId ?? null,
      outBoundNumber: req?.body?.outBoundNumber ?? null,
      maxCallHandlingTimeMM: req?.body?.maxCallHandlingTimeMM ?? null,
      callWrapUpTime: req?.body?.callWrapUpTime ?? null,
      hours_of_operation: req?.body?.hours_of_operation ?? null,
      outBoundCallStatus: req?.body?.outBoundCallStatus === 0 ? false : true,
      settingAudio: req?.body?.settingAudio ?? null,
      announcement: req?.body?.announcement ?? null,
      maximumCallers: req?.body?.maximumCallers ?? null,
      maximumWaitTime: req?.body?.maximumWaitTime ?? null,
      hoursOfOperatioId: req?.body?.hoursOfOperatioId ?? null,
      outOfBusinessHours: req?.body?.outOfBusinessHours ?? null,
      status: req?.body?.status ?? 1,
      type: req?.body?.type ?? null,

    };
    if (qdata.qid !== '') {
      const updatequeue: any = await update(data, qdata);
      logger.info('update', updatequeue);
      if (updatequeue.length > 0) {
        res.status(200).send({
          statusCode: 200,
          message: RESPONSE.Queue_update_successfully,
          updateQueueList: updatequeue?.[0] ?? {},
        });
      } else {
        res.status(200).send({ statusCode: 404, message: RESPONSE.not_update });
      }
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.please_provider });
    }
  } catch (err) {

    logger.error('updateQueueHandler', req.headers, req.params, req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function deleteQueueHandler(req: any, res: any, done: any) {
  try {
    const qdata: any = req.params.qid;
    const deleteQueue: any = await queueDelete(qdata);
    logger.info('queueDelete', deleteQueue);
    if (deleteQueue) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.Queue_delete_successfully });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {

    logger.error('deleteQueueHandler', req.params, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}
/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getQueueListHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      domainId: auth.domainId,
      name: req.query.name,
    };
    const getQueueList: any = await getSearchList(data);
    logger.info('getSearchList', getQueueList);
    if (Array.isArray(getQueueList) && getQueueList.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getQueueLists: getQueueList,
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getQueueLists: [] });
    }
  } catch (err) {

    logger.error('getQueueListHandler', req.headers, req.query, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function checkQueueMappingHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data = auth.domainId;
    const qid = req.params.qid;
    if (qid !== '') {
      const queueRes: any = await checkQueueMapping(qid, data);
      logger.info('checkQueueMapping', queueRes);
      if (queueRes?.length > 0) {
        res.status(200).send({ statusCode: 200, message: RESPONSE.Queue, flag: 1 });
      } else {
        res.status(200).send({ statusCode: 405, message: RESPONSE.queueMap, flag: 0 });
      }
    } else {
      res.status(200).send({ statusCode: 405, message: RESPONSE.qid });
    }
  } catch (err) {

    logger.error('checkQueueMappingHandler', req.headers, req.params, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function updateQueueStatusHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const qdata: any = req.params.qid;
    const data: any = {
      status: req.body.status ? req.body.status : null,
    };
    if (req.params.qid !== '' && req.body.status !== '') {
      const updatequeue: any = await updateQueue(data, qdata);
      logger.info('updateQueue', updatequeue);
      if (updatequeue.affectedRows === 1) {
        res.status(200).send({
          statusCode: 200,
          message: RESPONSE.Queue_update_successfully,
        });
      } else {
        res.status(200).send({ statusCode: 404, message: RESPONSE.not_update });
      }
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.please_provider });
    }
  } catch (err) {

    logger.error('updateQueueStatusHandler', req.headers, req.params, req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function updateStatusHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const qid: any = req.params.qid;
    const data: any = {
      status: req?.body?.status ?? 1,
      domainId: auth.domainId,
    };
    if (qid !== '') {
      const updatequeue: any = await updateStatus(data, qid);
      logger.info('updateStatus', updatequeue);
      if (updatequeue.affectedRows === 1) {
        res.status(200).send({
          statusCode: 200,
          message: RESPONSE.status_update_successfully,
        });
      } else {
        res.status(200).send({ statusCode: 404, message: RESPONSE.not_update });
      }
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.qid });
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
 * ccs custom hours api
 */
export async function insertCustomHoursHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      cid: req?.body?.cid ?? null,
      name: req?.body?.name ?? null,
      description: req?.body?.description ?? null,
      hours: req?.body?.hours ?? null,
      timeZone: req?.body?.timeZone ?? null,
      domainId: auth.domainId,
      default_hours: req?.body?.default_hours ?? null,
      timeZoneValue: req?.body?.timeZoneValue ?? null,
      type: req?.body?.type ?? null,
      holiday: req?.body?.holiday ?? null
    };
    const insertCustom: any = await insertCustomHours(data);
    logger.info('insertCustomHours', insertCustom);
    if (insertCustom) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.customHours_insert_success, insertCustomList: insertCustom ?? {} });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {

    logger.error('updateStatusHandler', req.headers, req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getCustomHoursListHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      domainId: auth.domainId,
    };
    const getCustomList: any = await getCustomHoursList(data);
    logger.info('getCustomHoursList', getCustomList);
    if (Array.isArray(getCustomList) && getCustomList.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getCustomHoursListList: getCustomList,
      });
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        getCustomHoursListList: [],
      });
    }
  } catch (err) {

    logger.error('getCustomHoursListHandler', req.headers, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function updateCustomHoursHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const qdata: any = req.params.cid;
    const data: any = {
      name: req?.body?.name ?? null,
      description: req?.body?.description ?? null,
      hours: req?.body?.hours ?? null,
      timeZone: req?.body?.timeZone ?? null,
      domainId: auth.domainId,
      timeZoneValue: req?.body?.timeZoneValue ?? null,
      default_hours: req?.body?.default_hours ?? null,
      type: req?.body?.type ?? null,

    };
    if (qdata.cid !== '') {
      const updateCustom: any = await updateCustomHours(data, qdata);
      logger.info('updateCustomHours', updateCustom);
      if (updateCustom.affectedRows === 1) {
        res.status(200).send({
          statusCode: 200,
          message: RESPONSE.customHours_update_successfully,
          updateCustomList: updateCustom?.result ?? {},
        });
      } else {
        res.status(200).send({ statusCode: 404, message: RESPONSE.not_update });
      }
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.please_provider });
    }
  } catch (err) {

    logger.error('updateCustomHoursHandler', req.headers, req.params, req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function updateCustomHoursByIdHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      cid: req?.params?.cid,
      hoursOfOperation: req?.body?.hours ?? null,
      domainId: auth?.domainId ?? 0,
    };
    if (data?.cid !== '') {
      const updateCustom: any = await updateCustomHoursById(data);
      logger.info('updateCustomHoursById', updateCustom);
      if (updateCustom && Array.isArray(updateCustom) && updateCustom[0]?.errcode === 0) {
        res.status(200).send({
          statusCode: 200,
          message: RESPONSE.customHours_update_successfully,
        });
      } else {
        res.status(200).send({ statusCode: 404, message: RESPONSE.not_update });
      }
    } else {
      res.status(200).send({ statusCode: 400, message: RESPONSE.cid });
    }
  } catch (err) {

    logger.error('updateCustomHoursByIdHandler', req.headers, req.params, req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}
/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function deleteCustomHoursHandler(req: any, res: any, done: any) {
  try {
    const cid: any = req.params.cid;
    if (cid !== '') {
      const deleteCustom: any = await deleteCustomHours(cid);
      logger.info('deleteCustomHours', deleteCustom);
      if (deleteCustom) {
        res.status(200).send({ statusCode: 200, message: RESPONSE.customHours_delete_successfully });
      } else {
        res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
      }
    } else {
      res.status(200).send({ statusCode: 405, message: RESPONSE.cid });
    }
  } catch (err) {

    logger.error('deleteCustomHoursHandler', req.params, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function customHoursStatusHandler(req: any, res: any, done: any) {
  try {
    const cid = req.params.cid;
    if (cid !== '') {
      const customRes: any = await customHoursStatus(cid);
      logger.info('customHoursStatus', customRes);
      const queueRes: any = await getQueueDetails(cid);
      logger.info('getQueueDetails', queueRes);
      if (customRes?.cid === queueRes?.hoursOfOperatioId) {
        res.status(200).send({ statusCode: 200, message: RESPONSE.Business_Hours, flag: 1 });
      } else {
        res.status(200).send({ statusCode: 405, message: RESPONSE.Business, flag: 0 });
      }
    } else {
      res.status(200).send({ statusCode: 405, message: RESPONSE.cid });
    }
  } catch (err) {

    logger.error('customHoursStatusHandler', req.params, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getCustomHoursDetailHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {};
    data.domainId = auth.domainId;
    const cid = req.params.cid;
    if (cid !== '') {
      const getCustomHours: any = await getCustomHoursDetail(cid, data);
      logger.info('getCustomHoursDetail', getCustomHours);
      if (Array.isArray(getCustomHours) && getCustomHours.length > 0) {
        res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getCustomHoursListRes: getCustomHours });
      } else {
        res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getRoleListRes: [] });
      }
    } else {
      res.status(200).send({ statusCode: 424, message: 'cid number should not be empty' });
    }
  } catch (err) {

    logger.error('getCustomHoursDetailHandler', req.headers, req.params, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getCustomHoursByIdHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {};
    data.domainId = auth.domainId;
    const cid = req.params.cid;
    if (cid !== '') {
      const getCustomHours: any = await getCustomHoursDetailById(cid, data.domainId);
      logger.info('getCustomHoursDetailById', getCustomHours);
      if (getCustomHours && Array.isArray(getCustomHours) && getCustomHours?.length > 0) {
        res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getCustomHoursListRes: getCustomHours });
      } else {
        res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getCustomHoursListRes: [] });
      }
    } else {
      res.status(200).send({ statusCode: 400, message: RESPONSE.cid });
    }
  } catch (err) {

    logger.error('getCustomHoursByIdHandler', req.headers, req.params, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function isValidCustomHoursNameHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {};
    data.domainId = auth.domainId;
    data.name = req.body.name;
    const hoursRes: any = await isValidCustomHoursName(data);
    logger.info('isValidCustomHoursName', hoursRes);
    if (hoursRes && hoursRes.length > 0 && hoursRes[0]?.name.length > 0) {
      res.status(200).send({ statusCode: 403, message: RESPONSE.name_already_exist, flag: 0 });
    } else {
      res.status(200).send({ statusCode: 200, message: RESPONSE.available, flag: 1 });
    }
  } catch (err) {

    logger.error('isValidCustomHoursNameHandler', req.headers, req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function isValidQueueNameHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {};
    data.domainId = auth.domainId;
    data.name = req?.body?.name ?? null;
    const queueRes: any = await isValidQueueName(data);
    logger.info('isValidQueueName', queueRes);
    if (queueRes && queueRes.length > 0 && queueRes[0]?.name.length > 0) {
      res.status(200).send({ statusCode: 403, message: RESPONSE.name_already_exist, flag: 0 });
    } else {
      res.status(200).send({ statusCode: 200, message: RESPONSE.available, flag: 1 });
    }
  } catch (err) {

    logger.error('isValidQueueNameHandler', req.headers, req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function insertUpdateBusinessHolidaysHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      holidayId: req.body?.holidayId ?? null,
      domainId: auth?.domainId ?? null,
      businessId: req.body?.businessId ?? null,
      holidayName: req.body?.holidayName ?? null,
      startDate: req.body?.startDate ?? null,
      endDate: req.body?.endDate ?? null,
      holidayDesc: req.body?.holidayDesc ?? null
    };
    const insertUpdateBusinessHolidaysRes: any = await insertUpdateBusinessHolidaysDao(data);
    logger.info('insertUpdateBusinessHolidaysDao', insertUpdateBusinessHolidaysRes);
    if (insertUpdateBusinessHolidaysRes && insertUpdateBusinessHolidaysRes.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.Business_Hours_Insert_successfully, result: insertUpdateBusinessHolidaysRes });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.Business_Hours_Insert_Error });
    }
  } catch (err) {

    logger.error('insertUpdateBusinessHolidaysHandler', req.headers, req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getBusinessHolidaysHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      holidayId: req.query?.holidayId ?? null,
      domainId: auth?.domainId ?? null,
      businessId: req.query?.businessId ?? null,
    };
    const getBusinessHolidaysRes: any = await getBusinessHolidaysDao(data);
    logger.info('getBusinessHolidaysDao', getBusinessHolidaysRes);
    if (getBusinessHolidaysRes && getBusinessHolidaysRes.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.Get_Business_Hours, result: getBusinessHolidaysRes });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {

    logger.error('getBusinessHolidaysHandler', req.headers, req.query, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function deleteBusinessHolidaysHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      holidayId: req.query?.holidayId ?? null,
      domainId: auth?.domainId ?? null,
      businessId: req.query?.businessId ?? null,
    };
    const deleteBusinessHolidaysRes: any = await deleteBusinessHolidaysDao(data);
    logger.info('deleteBusinessHolidaysDao', deleteBusinessHolidaysRes);
    if (deleteBusinessHolidaysRes && deleteBusinessHolidaysRes.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.Delete_Business_Hours, result: deleteBusinessHolidaysRes });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {

    logger.error('deleteBusinessHolidaysHandler', req.headers, req.query, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function updateBusinessHolidaysStatusHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      holidayId: req.body?.holidayId ?? null,
      status: req.body?.status ?? null,
      businessId: req.body?.businessId ?? null,
    };
    const updateBusinessHolidaysStatusRes: any = await updateBusinessHolidaysStatusDao(data);
    logger.info('updateBusinessHolidaysStatusDao', updateBusinessHolidaysStatusRes);
    if (updateBusinessHolidaysStatusRes && updateBusinessHolidaysStatusRes.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.Update_Business_Hours_Status, result: updateBusinessHolidaysStatusRes });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {

    logger.error('updateBusinessHolidaysStatusHandler', req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}
