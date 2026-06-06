import {
  assignCallBack,
  assignCallBackCall,
  callFlowDetails,
  callFlowList,
  deleteCallFlowInfoDao,
  getActivityList,
  getCallBackList,
  getCallSessionHistory,
  getConversationList,
  getCustomerDndInfoDao,
  getRoleIdList,
  getSessionIdDao,
  roleInfoDao,
  savecallflow,
  updateCallBackSessionId,
  updateCallBackSessionIdDao,
  updateCallBackSocialmedia,
  updateChatBackSessionId,
  updateVoiceMailCallbackDao,

} from '../../dao/callflow.dao';

import _ from 'lodash';
import { domain } from 'process';
import { v4 as uuidv4 } from 'uuid';
import { ioredisChat } from '../../../src/plugins/db';
import { RESPONSE, ROLES } from '../../helpers/constants';
import { logger } from '../../plugins/winston';

/**
 * @param req
 * @param res
 * @param done
 */
export async function saveCallFlowData(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const reqData = req.body;
    const d = new Date();
    const datestring = `${d.getDate()}/${d.getMonth() + 1
      }/${d.getFullYear()} ${d.getHours()}:${d.getMinutes()}`;
    const version =
      req?.body?.status === 2
        ? `Version ${datestring} : Published`
        : `Version ${datestring} : Save`;
    const cfid =
      req.body.cfid === '' || req.body.cfid === undefined
        ? uuidv4()
        : req.body.cfid;
    reqData.version = version;
    reqData.type = req?.body?.type ?? 'Custom';
    reqData.createDate = Date.now();
    reqData.lastModifiedDate = Date.now();
    reqData.status = req.body.status;
    reqData.companyId = auth?.companyId ?? 0;
    reqData.domainId = auth.domainId;
    reqData.description = req.body.description;
    reqData.cfid = cfid;
    const publishedData: any = [];
    publishedData.push(reqData);
    reqData.publishedDataArr = JSON.stringify(publishedData);
    const nodeData: any = {};
    nodeData.node = JSON.stringify(reqData?.nodes);
    nodeData.edge = JSON.stringify(reqData?.edges);
    const saveResult = await savecallflow(reqData, nodeData);

    res.send({
      statusCode: 200,
      status: 'OK',
      uuid: reqData.cfid,
      callFlowId: saveResult[0][0]?.callFlowId,
      message: saveResult[0][0].errmsg,
      getSaveCallflowList: reqData ?? {},
    });
  } catch (err) {
    logger.error('saveCallFlowData', { data: req?.body, Err: err });
    res.send({ statusCode: 500, result: {}, message: 'internal server error' });
  }
}

export async function updateCallFlowDataForPublish(
  req: any,
  res: any,
  done: any
) {
  try {
    const auth = req.headers;
    const reqData = req.body;
    const d = new Date();
    const datestring = `${d.getDate()}/${d.getMonth() + 1
      }/${d.getFullYear()} ${d.getHours()}:${d.getMinutes()}`;
    const version =
      req?.body?.status === 2
        ? `Version ${datestring} : Published`
        : `Version ${datestring} : Save`;
    reqData.version = version;
    reqData.type = req?.body?.type ?? 'Custom';
    reqData.createDate = Date.now();
    reqData.lastModifiedDate = Date.now();
    reqData.status = req.body.status;
    reqData.companyId = auth?.companyId ?? 0;
    reqData.domainId = auth.domainId;
    reqData.description = req.body.description;
    const publishedData: any = [];
    publishedData.push(reqData);
    reqData.publishedDataArr = JSON.stringify(publishedData);
    const nodeData: any = {};
    nodeData.node = JSON.stringify(reqData?.nodes);
    nodeData.edge = JSON.stringify(reqData?.edges);
    const saveResult = await savecallflow(reqData, nodeData);
    res.send({
      statusCode: 200,
      status: 'OK',
      uuid: reqData.cfid,
      message: saveResult[0][0].errmsg,
      getSaveCallflowList: reqData ?? {},
    });
  } catch (err) {
    logger.error('updateCallFlowDataForPublish', { data: req?.body, Err: err });
    res.send({ statusCode: 500, result: {}, message: 'internal server error' });
  }
}

export async function getCallFlowList(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {};
    data.domainId = auth.domainId;
    callFlowList(data)
      .then((callList: any) => {
        res.send({
          statusCode: 200,
          result: callList,
          status: 'Ok',
          message: '',
        });
      })
      .catch((err: any) => {
        res.send({
          statusCode: 200,
          result: [],
          status: 'Failure',
          message: 'Call Flow not found!',
        });
      });
  } catch (err) {
    logger.error('getCallFlowList', { data: req?.headers?.domainId, Err: err });
    res.send({ statusCode: 500, result: [], message: 'internal server error' });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getCallFlowDetail(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {};
    data.cfid = req?.params?.cfid;
    data.domainId = auth.domainId;
    callFlowDetails(data)
      .then((callDetails: any) => {
        res.send({
          statusCode: 200,
          result: callDetails,
          status: 'Ok',
          message: '',
        });
      })
      .catch((err: any) => {
        res.send({
          statusCode: 200,
          result: [],
          status: 'Failure',
          message: 'Call Flow not found!',
        });
      });
  } catch (err) {
    logger.error('getCallFlowDetail', { data: req?.headers?.domainId, cfid: req?.params?.cfid, Err: err });
    res.send({ statusCode: 500, result: [], message: 'internal server error' });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getSessionIdList(req: any, res: any, done: any) {
  try {
    const sessionId = req.params.sessionId;
    const auth = req.headers;
    const getSessionId: any = await getSessionIdDao(auth.domainId, sessionId);
    if (getSessionId) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getSessionIdListRes: getSessionId,
      });
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        getSessionIdListRes: [],
      });
    }
  } catch (err) {
    logger.error('getSessionIdList', { data: req?.headers?.domainId, sessionId: req?.params?.sessionId, Err: err });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getCallBackListHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {
      domainId: auth.domainId,
      ext: auth.ext,
      emailId: auth.username,
      channelType: req.body.channelType,
      search: req.body.search,
      fromDate: req.body.fromDate,
      toDate: req.body.toDate,
      offset: req.body.offset,
      limit: req.body.limit,
      queueId: req?.body?.queueId ?? null
    };
    const roleInfo: any = await roleInfoDao(data);
    data.ccas_role_info = roleInfo?.roleid?.toString();
    // if (data?.ccas_role_info?.split(',')[0] === ROLES.SUPERVISOR_ROLE_ID || data?.ccas_role_info?.split(',')[0] === ROLES.ADMIN_ROLE_ID) {
    data.userID = roleInfo?.UserID;
    const ext = [];
    const getRoleId: any = await getRoleIdList(data);
    for (const result of getRoleId) {
      ext.push(result?.ext);
    }
    data.resultExt = ext.toString();
    const getCallBack: any = await getCallBackList(data);
    if (Array.isArray(getCallBack) && getCallBack.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getCallBackListRes: getCallBack[0],
        getWaitingLine: getCallBack[1]
      });
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        getCallBackListRes: [],
        getWaitingLine: []
      });
    }
    // } else if (data?.ccas_role_info?.split(',')[0] === ROLES.AGENT_ROLE_ID) {
    //   data.userID = roleInfo?.UserID;
    //   data.assigned_ext = data.ext;
    //   const getCallBack: any = await getCallBackList(data);
    //   if (Array.isArray(getCallBack) && getCallBack.length > 0) {
    //     res.status(200).send({
    //       statusCode: 200,
    //       message: RESPONSE.success_message,
    //       getCallBackListRes: getCallBack,
    //     });
    //   } else {
    //     res.status(200).send({
    //       statusCode: 404,
    //       message: RESPONSE.not_found,
    //       getCallBackListRes: [],
    //     });
    //   }
    // }
  } catch (err) {
    logger.error('getCallBackListHandler', { data: req?.headers?.domainId, body: req?.body, Err: err });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}
export async function getSessionHistoryDetails(req: any, res: any, done: any) {
  try {
    const data: any = {
      domainId: req?.headers?.domainId,
      ext: req?.headers?.ext,
      emailId: req?.headers?.username,
      channelType: req?.body?.channelType,
      search: req?.body?.search || null,
      fromDate: req?.body?.fromDate,
      toDate: req?.body?.toDate,
      offset: req?.body?.offset ?? 0,
      limit: req?.body?.limit ?? 500,
      disposition: req?.body?.disposition ? req.body?.disposition : null,
      dispositionId: req?.body?.dispositionId ?? null,
      isInbound: req?.body?.isInbound ?? null,
      queueId: req?.body?.queueId ?? null,
      sentimentalFrom: req?.body?.sentimentalFrom ?? null,
      sentimentalTo: req?.body?.sentimentalTo ?? null,
      isQualifiedUser: req?.body?.isQualifiedUser ?? null
    };
    const roleInfo: any = await roleInfoDao(data);
    data.ccas_role_info = roleInfo?.roleid?.toString();
    if (
      data.ccas_role_info === ROLES?.SUPERVISOR_ROLE_ID ||
      data.ccas_role_info === ROLES?.ADMIN_ROLE_ID
    ) {
      data.userID = null;
      const ext = [];
      const getRoleId: any = await getRoleIdList(data);
      for (const result of getRoleId) {
        ext.push(result?.ext);
      }
      data.resultExt = ext.toString();
      const getCallBack: any = await getCallSessionHistory(data);
      if (Array.isArray(getCallBack) && getCallBack?.length > 0) {
        res.status(200).send({
          statusCode: 200,
          message: RESPONSE.success_message,
          getCallBackListRes: getCallBack,
        });
      } else {
        res.status(200).send({
          statusCode: 404,
          message: RESPONSE.not_found,
          getCallBackListRes: [],
        });
      }
    } else if (data.ccas_role_info === ROLES.AGENT_ROLE_ID) {
      data.userID = roleInfo?.UserID;
      const getCallBack: any = await getCallSessionHistory(data);
      if (Array.isArray(getCallBack) && getCallBack?.length > 0) {
        res.status(200).send({
          statusCode: 200,
          message: RESPONSE.success_message,
          getCallBackListRes: getCallBack,
        });
      } else {
        res.status(200).send({
          statusCode: 404,
          message: RESPONSE.not_found,
          getCallBackListRes: [],
        });
      }
    }
  } catch (err) {
    logger.error('getSessionHistoryDetails', { data: req?.headers?.domainId, body: req?.body, Err: err });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 */
export async function updateVoiceMailCallback(req: any, res: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      sessionId: req?.body?.sessionId ?? null,
      domainId: auth.domainId,
    };
    const results: any = await updateVoiceMailCallbackDao(data);
    if (results && results?.length > 0) {
      const updateVoiceMail = results ?? [];
      res.status(200).send({ updateVoiceMail, statusCode: 200, message: RESPONSE.success_message, });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [], });
    }
  } catch (err) {
    logger.error('updateVoiceMailCallback', { data: req?.headers?.domainId, sessionId: req?.body?.sessionId, Err: err });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error, });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * ccs assign call back api
 */
export async function assignCallBackHandlers(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      assignedBy: req?.body?.assignedBy ?? null,
      assignedTo: req?.body?.assignedTo ?? null,
      assignedOn: Date.now(),
      EmailHistoryId: req?.body?.emailHistoryId,
      Istransferred: req?.body?.Istransferred ?? null,
    };
    const assignRes: any = await assignCallBack(data);
    if (assignRes) {
      res
        .status(200)
        .send({ statusCode: 200, message: RESPONSE.update_asssign });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {
    logger.error('assignCallBackHandlers', { data: req?.body, Err: err });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * ccs assign call back call api
 */
export async function assignCallBackCallHandlers(
  req: any,
  res: any,
  done: any
) {
  try {
    const data: any = {
      assignedBy: req?.body?.assignedBy ?? null,
      assignedTo: req?.body?.assignedTo ?? null,
      assignedOn: Date.now(),
      CallHistoryId: req.body.CallHistoryId ?? null,
    };
    const assignRes: any = await assignCallBackCall(data);
    if (assignRes && Array.isArray(assignRes) && assignRes.length > 0) {
      res
        .status(200)
        .send({ statusCode: 200, message: RESPONSE.update_asssign });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {
    logger.error('assignCallBackCallHandlers', { data: req?.body, Err: err });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * ccs assign call back call api
 */
export async function updateCallBackCallHandlers(
  req: any,
  res: any,
  done: any
) {
  try {
    const domainId = (req.headers.domainId);
    const data: any = {
      CallHistoryId: req?.body?.CallHistoryId ?? null,
      callBackSessionId: req?.body?.callBackSessionId ?? null,
    };
    const response: any = await updateCallBackSessionId(data);
    if (response && Array.isArray(response) && response[0]?.errCode === 0) {
      ioredisChat.to(`${domainId}`).emit('reAssignActivityListener');
      res
        .status(200)
        .send({ statusCode: 200, message: RESPONSE.updated_success });
      ioredisChat.to(`${domainId}`).emit('interActionClosed');
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.failed,
      });
    }
  } catch (err) {
    logger.error('updateCallBackCallHandlers', { data: req?.body, Err: err });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * ccs assign call back call api
 */
export async function updateCallBackCallNewHandlers(
  req: any,
  res: any,
  done: any
) {
  try {
    const domainId = (req.headers.domainId);
    const data: any = {
      ParentsessionId: req?.body?.ParentsessionId ?? null,
      CallBacksessionId: req?.body?.CallBacksessionId ?? null,
    };
    const response: any = await updateCallBackSessionIdDao(data);
    if (response && Array.isArray(response) && response?.[0]?.errCode === 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.callback_success });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.callback_fail });
    }
  } catch (err) {
    logger.error('updateCallBackCallNewHandlers', { data: req?.body, Err: err });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function updateCallBackChatHandlers(
  req: any,
  res: any,
  done: any
) {
  try {
    const domainId = (req.headers.domainId);
    const data: any = {
      ChatHistoryId: req?.body?.ChatHistoryId ?? null,
      callBackSessionId: req?.body?.callBackSessionId ?? null,
    };
    const response: any = await updateChatBackSessionId(data);
    if (response && Array.isArray(response) && response[0]?.errCode === 0) {
      ioredisChat.to(`${domainId}`).emit('reAssignActivityListener');
      res
        .status(200)
        .send({ statusCode: 200, message: RESPONSE.updated_success });
      ioredisChat.to(`${domainId}`).emit('interActionClosed');
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.failed,
      });
    }
  } catch (err) {
    logger.error('updateCallBackChatHandlers', { data: req?.body, domainId: req?.headers?.domainId, Err: err });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function updateCallBackSocialmediaHandlers(
  req: any,
  res: any,
  done: any
) {
  try {
    const domainId = (req.headers.domainId);
    const data: any = {
      callBackSocialmediaHistoryId: req?.body?.callBackSocialmediaHistoryId ?? null,
      callBackSessionId: req?.body?.callBackSessionId ?? null,
      channelType: req?.body?.channelType ?? null
    };
    const response: any = await updateCallBackSocialmedia(data);
    if (response && Array.isArray(response) && response[0]?.errCode === 0) {
      console.log('updateCallBackChatHandlers ', response);
      logger.info('updateCallBackChatHandlers', response);
      ioredisChat.to(`${domainId}`).emit('reAssignActivityListener');
      res
        .status(200)
        .send({ statusCode: 200, message: RESPONSE.updated_success });
      ioredisChat.to(`${domainId}`).emit('interActionClosed');
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.failed,
      });
    }
  } catch (err) {
    logger.error('updateCallBackSocialmediaHandlers', { data: req?.body, domainId: req?.headers?.domainId, Err: err });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getActivityListHandlers(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      domainId: auth.domainId,
      ext: auth.ext,
      emailId: auth.username,
      channelType: req.body.channelType,
      disposition: req.body.disposition,
      callType: req.body.callType,
      fromDate: req.body.fromDate,
      toDate: req.body.toDate,
      offset: req.body.offset,
      limit: req.body.limit,
      search: req.body.search,
    };
    const roleInfo: any = await roleInfoDao(data);
    data.ccas_role_info = roleInfo?.roleid?.toString();
    if (data?.ccas_role_info?.split(',')[0] === ROLES.SUPERVISOR_ROLE_ID) {
      const ext = [];
      const getRoleId: any = await getRoleIdList(data);
      for (const resu of getRoleId) {
        ext.push(resu.ext);
      }
      data.ext = ext;
      const getVoiceListRes: any = await getActivityList(data);
      if (Array.isArray(getVoiceListRes) && getVoiceListRes.length > 0) {
        res.status(200).send({
          statusCode: 200,
          message: RESPONSE.success_message,
          getVoiceCallListRes: getVoiceListRes,
        });
      } else {
        res.status(200).send({
          statusCode: 404,
          message: RESPONSE.not_found,
          getVoiceCallListRes: [],
        });
      }
    } else if (data.ccas_role_info?.split(',')[0] === ROLES.AGENT_ROLE_ID) {
      const getVoiceListRes: any = await getActivityList(data);
      // const getVoiceListCountRes: any = await getVoiceListCount(data);
      if (Array.isArray(getVoiceListRes) && getVoiceListRes.length > 0) {
        res.status(200).send({
          statusCode: 200,
          message: RESPONSE.success_message,
          getVoiceCallListRes: getVoiceListRes,
        });
      } else {
        res.status(200).send({
          statusCode: 404,
          message: RESPONSE.not_found,
          getVoiceCallListRes: [],
        });
      }
    }
  } catch (err) {
    logger.error('getActivityListHandlers', { data: req?.body, domainId: req?.headers?.domainId, Err: err });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getConversationListHandler(
  req: any,
  res: any,
  done: any
) {
  try {
    const auth = req.headers;
    const sessionId = req.params.sessionId;
    const conversationRes: any = await getConversationList(sessionId, auth.domainId);
    if (conversationRes && Array.isArray(conversationRes) && conversationRes?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getConversationResListRes: conversationRes,
      });
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        getSessionListRes: [],
      });
    }
  } catch (err) {
    logger.error('getConversationListHandler', { data: req.params.sessionId, domainId: req?.headers?.domainId, Err: err });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}
export async function deleteCallFlow(req: any, res: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      callFlowId: req?.body?.callFlowId ?? null,
      domainId: auth.domainId,
    };
    const results: any = await deleteCallFlowInfoDao(data);
    if (results && results?.length > 0) {
      const response = results ?? [];
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, result: response });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [], });
    }
  } catch (err) {
    logger.error('deleteCallFlow', { data: req.body, domainId: req?.headers?.domainId, Err: err });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error, });
  }
}

export async function getCustomerDndInfo(req: any, res: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      customerNumber: req?.body?.customerNumber ?? null,
      domainId: auth.domainId,
    };
    const results: any = await getCustomerDndInfoDao(data);
    if (results && results?.length > 0) {
      const doNotDisturb = results ?? [];
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, result: doNotDisturb });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [], });
    }
  } catch (err) {
    logger.error('getCustomerDndInfo', { data: req.body, domainId: req?.headers?.domainId, Err: err });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error, });
  }
}
