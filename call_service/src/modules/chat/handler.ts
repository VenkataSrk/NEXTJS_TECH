import { logger } from '../../../src/plugins/winston';
import { deleteChatBotChannelType, getAgentStatusViewDao, getAllChatFlowIdDao, getCallSessionIdDao, getChatByNumberDao, getChatDurationdao, getChatflowByDomainDao, getChatFlowIdDao, getChatSessionIdDao, getDefaultChatFlowIdDao, getListenSessionIdDao, getNodeDtlDao, saveChatFlowDao, updateChatBotChannelType } from '../../dao/chat.dao';
import { RESPONSE } from '../../helpers/constants';

/**
 * get chatSession details
 * @param req
 * @param res
 * @param done
 */
export async function getChatSessionId(req: any, res: any, done: any) {
  try {
    const chatSessionId = req.params.chatSessionId;
    const DaoRes: any = await getChatSessionIdDao(chatSessionId);
    const DaoChatDurationRes: any = await getChatDurationdao(chatSessionId);

    if (DaoRes) {
      res.status(200).send({ statusCode: 200, message: 'Success', result: DaoRes, result1: DaoChatDurationRes });
    } else {
      res.status(424).send({ statusCode: 424, message: 'failed' });
    }
  } catch (err) {
    req.log.error('getChatSessionId catch', req?.params?.chatSessionId, 'error', err);
    res.status(500).send({ statusCode: 500, message: 'internal server error' });
    logger.error('getChatSessionId catch', { data: req?.params?.chatSessionId, Err: err });
  }
}

/**
 * get CallSession details
 * @param req
 * @param res
 * @param done
 */
export async function getCallSessionId(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      CallSessionId: req?.params?.chatSessionId,
      domainId: auth.domainId
    };
    console.log('getCallSessionId data', req?.params?.chatSessionId, 'auth.domainId', auth.domainId, 'Date.now(),', Date.now());
    logger.info('getCallSessionId data', req?.params?.chatSessionId, 'auth.domainId', auth.domainId, 'Date.now(),', Date.now());
    const DaoRes: any = await getCallSessionIdDao(data);
    console.log('getCallSessionId DaoRes', DaoRes, 'Date.now()', Date.now());
    logger.info('getCallSessionId DaoRes', DaoRes, 'Date.now()', Date.now());

    if (DaoRes) {
      res.status(200).send({ statusCode: 200, message: 'Success', result: DaoRes[0] });
    } else {
      res.status(424).send({ statusCode: 424, message: 'failed' });
    }
  } catch (err) {
    logger.error('getCallSessionId catch', { data: req?.params?.chatSessionId, Err: err });
    res.status(500).send({ statusCode: 500, message: 'internal server error' });
  }
}

/**
 * Get
 * @param req
 * @param res
 * @param done
 */
export async function getListenSessionId(req: any, res: any, done: any) {
  try {
    const data: any = {};
    data.domainId = req.headers.domainId;
    data.sessionId = req.params.callSessionId;
    if (data?.sessionId !== '' && data?.sessionId !== null) {
      const DaoRes: any = await getListenSessionIdDao(data);
      if (DaoRes) {
        res.status(200).send({ statusCode: 200, message: 'Success', agentDetails: DaoRes[0]?.length > 0 ? DaoRes[0] : [] });
      } else {
        res.status(424).send({ statusCode: 424, message: 'failed' });
      }
    } else {
      res.status(424).send({ statusCode: 424, message: 'Please provide the sessionId' });
    }
  } catch (err) {
    console.log('getListenSessionId catch', req?.params?.chatSessionId, 'error', err);
    logger.error('getListenSessionId catch', { data: req?.params?.chatSessionId, Err: err });
    res.status(500).send({ statusCode: 500, message: 'internal server error' });
  }
}

/**
 * Get
 * @param req
 * @param res
 * @param done
 */
export async function getAgentStatusView(req: any, res: any, done: any) {
  try {
    const data: any = {};
    data.domainId = parseInt(req.params.calldomainId, 10);
    if (data?.domainId !== '' && data?.domainId !== null) {
      const DaoRes: any = await getAgentStatusViewDao(data);
      if (DaoRes) {
        res.status(200).send({ statusCode: 200, message: 'Success', agentViewDetails: DaoRes[0]?.length > 0 ? DaoRes[0] : [] });
      } else {
        res.status(424).send({ statusCode: 424, message: 'failed' });
      }
    } else {
      res.status(424).send({ statusCode: 424, message: 'Please provide the domainId' });
    }
  } catch (err) {
    console.log('getAgentStatusView catch', req?.params?.calldomainId, 'error', err);
    logger.error('getAgentStatusView catch', { data: req?.params?.calldomainId, Err: err });
    res.status(500).send({ statusCode: 500, message: 'internal server error' });
  }
}

/**
 * get chatSession details
 * @param req
 * @param res
 * @param done
 */
export async function getChatByNumber(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      domainId: auth?.domainId,
      number: req?.body?.number
    };
    console.log('getChatByNumber', data, req);
    logger.info('getChatByNumber ', data, req);
    const DaoRes: any = await getChatByNumberDao(data);
    if (DaoRes) {
      res.status(200).send({ statusCode: 200, message: 'Success', result: DaoRes });
    } else {
      res.status(424).send({ statusCode: 424, message: 'failed' });
    }
  } catch (err) {
    console.log('getChatByNumber catch', req?.body, 'error', err);
    logger.error('getChatByNumber catch', { data: req?.body, Err: err });
    res.status(500).send({ statusCode: 500, message: 'internal server error' });
  }
}

/**
 *
 * New Design chat flows api's
 */
export async function saveChatFlowHistory(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const reqData = req.body;
    const d = new Date();
    const datestring = `${d.getDate()}/${d.getMonth() + 1}/${d.getFullYear()} ${d.getHours()}:${d.getMinutes()}`;
    const version = req?.body?.status === 2 ? `Version ${datestring} : Published` : `Version ${datestring} : Save`;
    reqData.cfid = req.body.chatFlowId === '' || req.body.chatFlowId === undefined ? 0 : req.body.chatFlowId;
    reqData.version = version;
    reqData.type = req?.body?.type ?? 'Default';
    reqData.status = req.body.status;
    reqData.companyId = auth.companyId;
    reqData.domainId = auth.domainId;
    reqData.description = req.body.description;
    reqData.action = req.body.action;
    reqData.category = req.body.category;
    reqData.channelType = req.body.channelType ?? null;
    const publishedData: any = [];
    publishedData.push(reqData);
    reqData.publishedDataArr = JSON.stringify(publishedData);
    const nodeData: any = {};
    nodeData.node = JSON.stringify(reqData?.nodes);
    nodeData.edge = JSON.stringify(reqData?.edges);
    reqData.isLanguage = req.body?.isLanguage;
    if (reqData?.templateName !== '' && reqData?.templateName !== null && reqData?.templateName !== undefined && reqData?.templateName !== 'Default Callflow Name') {
      const response = await saveChatFlowDao(reqData, nodeData);
      reqData.chatFlowId = response[0][0]?.chatFlowId;
      reqData.cfid = response[0][0]?.chatFlowId;
      res.status(200).send({ statusCode: 200, message: 'ChatFlow saved successfully', result: reqData });
    } else {
      res.status(200).send({ statusCode: 401, message: 'ChatFlow Name is empty' });

    }
  } catch (err) {
    logger.error('saveChatFlowHistory catch', { data: req?.body, Err: err });
    res.status(500).send({ statusCode: 500, message: 'internal server error' });
  }
}
export async function getChatFlowDetailsByCfid(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {};
    data.chatFlowId = req.params.cfid;
    data.domainId = auth.domainId;
    if (data?.chatFlowId !== '' || data?.chatFlowId !== undefined || data?.chatFlowId !== null) {
      const getChatFlowResult: any = await getChatFlowIdDao(data);
      if (getChatFlowResult.length > 0) {
        res.status(200).send({ statusCode: 200, message: 'success', result: getChatFlowResult });
      } else {
        res.status(200).send({ statusCode: 404, message: 'No data Found' });
      }
    } else {
      res.status(424).send({ statusCode: 424, message: 'Provide valide call flow Id' });
    }
  } catch (err) {
    logger.error('getChatFlowDetailsByCfid catch', { data: req?.params?.cfid, Err: err });
    res.status(500).send({ statusCode: 500, message: 'internal server error' });
  }
}
export async function getAllChatFlowDetails(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const getAllChatFlowResult: any = await getAllChatFlowIdDao(auth);
    if (getAllChatFlowResult.length > 0) {
      res.status(200).send({ statusCode: 200, message: 'success', result: getAllChatFlowResult });
    } else {
      res.status(200).send({ statusCode: 404, message: 'No data Found' });
    }
  } catch (err) {
    logger.error('getAllChatFlowDetails catch', { data: req?.headers, Err: err });
    res.status(500).send({ statusCode: 500, message: 'internal server error' });
  }
}

export async function getChatDetailsByCfidNoAuth(req: any, res: any, done: any) {
  try {

    const data: any = {};
    data.chatFlowId = req.body?.cfid;
    data.domainId = req.body?.domainId;

    if (data?.chatFlowId !== '' || data?.chatFlowId !== undefined || data?.chatFlowId !== null) {
      const getChatFlowResult: any = await getChatFlowIdDao(data);

      if (getChatFlowResult.length > 0) {
        res.status(200).send({ statusCode: 200, message: 'success', result: getChatFlowResult });
      } else {
        res.status(200).send({ statusCode: 404, message: 'No data Found' });
      }
    } else {
      res.status(424).send({ statusCode: 424, message: 'Provide valide call flow Id' });
    }
  } catch (err) {
    logger.error('getChatDetailsByCfidNoAuth catch', { data: req?.params?.cfid, Err: err });
    res.status(500).send({ statusCode: 500, message: 'internal server error' });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * update ChatBotChannelType api
 */
export async function updateChatBotChannelTypeHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      chatFlowId: req?.body?.chatFlowId ?? null,
      channelType: req?.body?.channelType ?? null,
      templateName: req?.body?.templateName ?? null,
      description: req?.body?.description ?? null,
      isLanguage: req?.body?.isLanguage ?? null,
      domainId: auth.domainId
    };
    if (data?.chatFlowId !== '' && data?.chatFlowId !== 0 && data?.chatFlowId !== null) {
      const updateBot: any = await updateChatBotChannelType(data);
      if (updateBot[0].errcode === 0) {
        res.status(200).send({ statusCode: 200, message: RESPONSE.updated_success });
      } else {
        res.status(200).send({ statusCode: 404, message: RESPONSE.not_update });
      }
    } else {
      res.status(200).send({ statusCode: 405, message: RESPONSE.chatFlowId });
    }
  } catch (err) {
    logger.error('updateChatBotChannelTypeHandler catch', { data: req?.body, Err: err });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function deleteChatBotChannelTypeHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data = {
      chatFlowId: req?.body?.chatFlowId ?? null,
      domainId: auth.domainId
    };
    if (data?.chatFlowId !== '' && data?.chatFlowId !== 0 && data?.chatFlowId !== null) {
      const deleteBot: any = await deleteChatBotChannelType(data);
      console.log('deleteBot=-=-=-=-=-=', deleteBot);
      logger.info('deleteChatBotChannelTypeHandler deleteBot', deleteBot);

      if (deleteBot[0].errcode === 0) {
        res.status(200).send({ statusCode: 200, message: RESPONSE.delete_contact });
      } else if (deleteBot[0].errcode === -2) {
        res.status(200).send({ statusCode: 401, message: deleteBot[0].errmsg });
      } else {
        res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
      }
    } else {
      res.status(200).send({ statusCode: 405, message: RESPONSE.chatFlowId });
    }
  } catch (err) {
    logger.error('deleteChatBotChannelTypeHandler catch', { data: req?.body, Err: err });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function getChatFlowDetailsByDomain(req: any, res: any, done: any) {
  try {

    const auth: any = req.headers;
    const getChatFlowResult: any = await getChatflowByDomainDao(auth);
    if (getChatFlowResult.length > 0) {
      res.status(200).send({ statusCode: 200, message: 'success', result: getChatFlowResult });
    } else {
      res.status(200).send({ statusCode: 404, message: 'No data Found' });
    }
  } catch (err) {
    logger.error('getChatFlowDetailsByDomain catch', { data: req?.headers, Err: err });
    res.status(500).send({ statusCode: 500, message: 'internal server error' });
  }
}

export async function getDefaultChatFlowId(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data = req.params?.flowId;
    const getChatFlowResult: any = await getDefaultChatFlowIdDao(data);
    if (getChatFlowResult.length > 0) {
      res.status(200).send({ statusCode: 200, message: 'success', result: getChatFlowResult });
    } else {
      res.status(200).send({ statusCode: 404, message: 'No data Found' });
    }
  } catch (err) {
    logger.error('getDefaultChatFlowId catch', { data: req.params?.flowId, Err: err });
    res.status(500).send({ statusCode: 500, message: 'internal server error' });
  }
}

export async function getNodeDtlHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data = {
      domainId: auth.domainId,
      chatFlowId: req.params?.flowId
    };
    console.log('getNodeDtlHandler>>>>', data);
    logger.info('getNodeDtlHandler catch', data);
    const getChatFlowResult: any = await getNodeDtlDao(data);
    if (getChatFlowResult.length > 0) {
      res.status(200).send({ statusCode: 200, message: 'success', result: getChatFlowResult });
    } else {
      res.status(200).send({ statusCode: 404, message: 'No data Found' });
    }
  } catch (err) {
    logger.error('getNodeDtlHandler catch', { data: req.params?.flowId, Err: err });
    res.status(500).send({ statusCode: 500, message: 'internal server error' });
  }
}
