import {
  deleteDid,
  deleteSipTrunkNumDb,
  didNumberEdit,
  didStatusEdit,
  getDid,
  getDidPurchaseDetails,
  getDomainDidList,
  getSearchList,
  indicateQAToNlpDao,
  sipTrunckNumCheckDB,
  updateDid
} from '../../dao/did-purchase';

/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */
import { updateCallFlowMap1 } from '../../dao/callFlow';
import { RESPONSE } from '../../helpers/constants';
import { logger } from '../../plugins/log';

// /**
//  *
//  * @param req
//  * @param res
//  * @param done
//  * ccs login api
//  */
// export async function didPurchaseHandler(req: any, res: any, done: any) {
//   try {
//     const auth: any = req.headers;
//     const datap: any = {
//       user_id: req.body.user_id ? req.body.user_id : null,
//       type: req.body.type ? req.body.type : null,
//       city: req.body.city ? req.body.city : null,
//       your_number: req.body.your_number ? req.body.your_number : null,
//       domainId: auth.domainId,
//       callFlowId: req?.body?.callFlowId ?? null,
//       recording: req?.body?.recording ?? 0,
//       queueName: 'Basic Queue',
//       isCall: req?.body?.isCall ?? 0,
//       isSms: req?.body?.isSms ?? 0,
//       aliasName: req?.body?.aliasName ?? null,
//       outboundSms: req?.body?.outboundSms ?? 0,
//       smsThreshold: req?.body?.smsThreshold ?? 0,
//       status: req?.body?.status ?? 0,
//       supervisorName: req?.body?.supervisorName ?? null
//     };
//     const insertdidNumber: any = await didPurchase(datap);
//     logger.info('didPurchase', insertdidNumber);
//     if (insertdidNumber) {
//       res.status(200).send({ statusCode: 200, message: RESPONSE.did_purchase_insert });
//     } else {
//       res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
//     }
//   } catch (err) {

//     logger.error('didPurchaseHandler', req.headers, req.body, err);
//     res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
//   }
// }

export async function getDidPurchaseHandler(req: any, res: any, done: any) {
  try {
    const pid = req.params.pid;
    const auth = req?.headers;
    if (pid !== '') {
      const data = {
        pid,
        domainId: auth?.domainId
      };
      const getDidNumber: any = await getDid(data);
      logger.info('getDid', getDidNumber);
      if (getDidNumber && getDidNumber.length > 0) {
        res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getDidNumberlist: getDidNumber });
      } else {
        res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getDidNumber: [] });
      }
    } else {
      res.status(200).send({ statusCode: 424, message: 'userId number should not be empty' });
    }
  } catch (err) {

    logger.error('getDidPurchaseHandler', req.params, err);
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
export async function updateDidPurchaseHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      your_number: req?.params?.phoneNumbar,
      domainId: auth?.domainId,
      callFlowId: req?.body?.callFlowId ?? 0,
      recording: req?.body?.recording ?? 0
    };
    const updateDidNumber: any = await updateDid(data);
    logger.info('updateDid', updateDidNumber);
    await updateCallFlowMap1(data);
    if (updateDidNumber[0]?.errcode === 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.did_updated_successfully });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_update });
    }
  } catch (err) {

    logger.error('updateDidPurchaseHandler', req.headers, req.params, req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function deleteDidPurchaseHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const pid: any = req.params.pid;
    const body = {
      pid,
      domainId: auth?.domainId,
    };
    const deleteDidNumber: any = await deleteDid(body);
    logger.info('deleteDid', deleteDidNumber);
    if (deleteDidNumber?.affectedRows === 1) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.did_delete_successfully });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {
    logger.error('deleteDidPurchaseHandler', req.params, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getDidListHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {};
    data.domainId = auth.domainId;
    data.limit = req.query.limit;
    data.offset = req.query.offset;
    data.searchBy = req.query.searchBy;
    data.searchByValue = req.query.searchByValue;
    const getDidNumber: any = await getSearchList(data);
    const mapData = {
      list: getDidNumber[0],
      total_count: getDidNumber[1][0].TotalCount,
    };
    logger.info('getSearchList', mapData);
    if (mapData && mapData.list.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getDidNumberlist: mapData });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getDidNumber: [] });
    }
  } catch (err) {

    logger.error('getDidListHandler', req.headers, req.query, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getDidPurchaseDetailsHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {};
    data.domainId = auth.domainId;
    const getDidData: any = await getDidPurchaseDetails(data);
    logger.info('getDidPurchaseDetails', getDidData);
    if (getDidData && getDidData.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getDidPurchaseDetails: getDidData });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getDidPurchaseDetails: [] });
    }
  } catch (err) {
    logger.error('getDidPurchaseDetailsHandler', req.headers, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * update outbound SMS
 */

export async function updateDIDforISSMS(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      domainId: auth.domainId,
      status: null,
      isSms: parseInt((req?.body?.isSms || 0), 10) ?? null,
    };
    const pid = req.params.pid;
    const outBoundSMS: any = await didStatusEdit(data, pid);
    logger.info('outBoundSMS', outBoundSMS);
    if (outBoundSMS.affectedRows === 1) {
      res.status(200).send({
        statusCode: 200,
        message: 'outBound SMS updated successfully',
      });
    } else {
      res.status(200).send({ statusCode: 404, message: 'outBound SMS not updated.' });
    }
  } catch (err) {
    logger.error('updateDIDforISSMS', req.headers, req.params, req.body, err);
    res.status(500).send({ statusCode: 500, message: 'Internal server error' });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * edit DID number
 */

export async function updateDIDNumberSMS(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const pid = req.params.pid;
    const data = {
      domainId: auth.domainId,
      aliasName: req.body.aliasName,
      outboundSms: req.body.outboundSms,
      queueName: req.body.queueName,
      smsThreshold: req.body.smsThreshold,
      supervisorName: req?.body?.supervisorName ?? null,
      knowledgeBasedId: req?.body?.knowledgeBasedId ?? null,
      isBot: req?.body?.isBot ?? null,
      autoAssignAgent: req?.body?.autoAssignAgent ?? null
    };
    const didUpdateNumber: any = await didNumberEdit(data, pid);
    logger.info('didNumberEdit', didUpdateNumber);
    if (didUpdateNumber[0]?.errmsg === 'Updated successfully') {
      res.status(200).send({
        statusCode: 200,
        message: 'DID number updated successfully',
      });
    } else {
      res.status(200).send({ statusCode: 404, message: 'DID number not updated.' });
    }
  } catch (err) {

    logger.error('updateDIDNumberSMS', req.headers, req.params, req.body, err);
    res.status(500).send({ statusCode: 500, message: 'Internal server error' });
  }
}

export async function updateDIDStatusHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data = {
      domainId: auth.domainId,
      status: req?.body?.status ?? 0,
      isSms: null
    };
    const pid = req.params.pid;
    const didUpdateStatus: any = await didStatusEdit(data, pid);
    logger.info('didStatusEdit', didUpdateStatus);
    if (didUpdateStatus.affectedRows === 1) {
      res.status(200).send({
        statusCode: 200,
        message: 'DID status updated successfully',
      });
    } else {
      res.status(200).send({ statusCode: 404, message: 'DID status not updated.' });
    }
  } catch (err) {

    logger.error('updateDIDStatusHandler', req.headers, req.params, req.body, err);
    res.status(500).send({ statusCode: 500, message: 'Internal server error' });
  }
}

export async function deleteSipTrunkNumHandler(req: any, res: any) {
  try {
    const data: any = {
      domainId: req?.headers?.domainId,
      number: req?.params?.pid
    };

    const deleteSipNumber: any = await deleteSipTrunkNumDb(data);
    logger.info('deleteSipTrunkNumDb', deleteSipNumber);
    if (deleteSipNumber) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.sip_delete_successfully });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (error) {
    req.log.error(error);
    logger.error('deleteSipTrunkNumHandler', req.headers, req.params, error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function sipTrunkNumCheckHandler(req: any, res: any) {
  try {
    const data: any = {
      domainId: req?.body?.domainId,
      number: req?.body?.number
    };
    const checkSipNumber: any = await sipTrunckNumCheckDB(data);
    logger.info('sipTrunckNumCheckDB', checkSipNumber);
    if (checkSipNumber) {
      res.status(200).send({ statusCode: 200, message: checkSipNumber[0][0].errmsg });
    }

  } catch (error) {
    req.log.error(error);
    logger.error('sipTrunkNumCheckHandler', req.body, error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function indicateQaToNlp(req: any, res: any) {
  try {
    const data: any = {
      domainId: req?.headers?.domainId,
      chatId: req.body?.chatId,
      ext: req.body?.ext
    };
    const indicateQAToNlpResult: any = await indicateQAToNlpDao(data);
    logger.info('indicateQAToNlpDao', indicateQAToNlpResult);
    if (indicateQAToNlpResult?.data?.message === 'success') {
      res.status(200).send({ statusCode: 200, message: RESPONSE?.success_message });
    } else {
      res.status(200).send({ statusCode: 404, message: 'failed' });
    }
  } catch (err) {
    logger.error('indicateQaToNlp', req.headers, req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function getDidDomainListHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {};
    data.domainId = auth.domainId;
    data.search = req?.body?.search;
    data.numberType = req?.body?.numberType;
    data.capability = req?.body?.capability;
    data.limit = req?.body?.limit;
    data.offset = req?.body?.offset;

    const getDidNumber: any = await getDomainDidList(data);
    logger.info('getDomainDidList', getDidNumber);
    if (getDidNumber) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getDidNumberlist: getDidNumber?.[0], count: getDidNumber?.[1], numberType: getDidNumber?.[2] });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getDidNumber: [], count: getDidNumber?.[1], numberType: [] });
    }
  } catch (err) {

    logger.error('getDidDomainListHandler', req.headers, req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}
