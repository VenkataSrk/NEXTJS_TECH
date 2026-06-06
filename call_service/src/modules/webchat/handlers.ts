import axios from 'axios';
import { v4 as uuidv4 } from 'uuid';
import { logger } from '../../../src/plugins/winston';
import { configs } from '../../config/app';
import {
  createChatSession,
  getAgentAvailability,
  getAIChatMessageDao,
  replaceWebchatDao,
  setTransferChatMessage,
  updateAgentDetails
} from '../../dao/webChat.dao';
import { RESPONSE } from '../../helpers/constants';
import { getLocationDetails } from '../../helpers/utils';

import {
  getCustomerChatHistory,
  getCustomerChatHistoryByUid
} from '../../dao/callflow.dao';

/**
 *
 * @param req
 * @param res
 * @param done
 * @description Web Chat
 */

export const webChatGetAvailableAgent = async (req: any, res: any) => {
  try {
    const data: any = {
      sessionId: req.body?.sessionId,
      ChatHistoryId: req.body?.ChatHistoryId ?? 0,
      sessionStartTime: Math.floor(Date.now()),
      chatFlowSourceId: req.body?.callFlowId,
      companyId: req.body?.companyId,
      domainId: req.body?.domainId,
      IsInBound: req.body?.IsInBound ?? 1,
      CustomerId: req.body?.CustomerId,
      Browser: req?.body?.browser?.split('/')[0] ?? null,
      IPAdress: req?.body?.ipAddress ?? '192.168.1.1',
      DeviceType: req?.body?.deviceType ?? null,
      call_state: 'init',
      response: 'success',
      type: 'playPrompt',
      widgetId: req?.body?.widgetId ?? null,
      dialDuration: req?.body?.dialDuration ?? null,
      IsMissed: req?.body?.IsMissed ?? null
    };
    try {
      const ipdata: any = await axios.get(`https://ipinfo.io/${data?.IPAdress}/json`);
      const city = ipdata?.data?.city;
      const region = ipdata?.data?.region;
      const country = ipdata?.data?.country;
      const location = `${city},${region},${country}`;
      data.location = location ?? null;
    } catch (error) {
      const insertChatSessionCatch = await createChatSession(data);
      res.status(200).send({ statusCode: 200, message: 'Insert successfully', result: insertChatSessionCatch[0] });
    }
    const insertChatSession = await createChatSession(data);
    res.status(200).send({ statusCode: 200, message: 'Insert successfully', result: insertChatSession[0] });
  } catch (err) {
    res.status(500).send({ statusCode: 500, message: 'Internal Sever Error' });
    console.log('webChatGetAvailableAgent err :::::', err);
    logger.error('webChatGetAvailableAgent catch', { data: req?.body, Err: err });
  }
};

export const webChatValidateCustomer = async (req: any, res: any) => {
  try {
    const data = req.params.chatDurationHistoryId.trim();
    const responseData: any = await getCustomerChatHistory(data);
    const messageArr: any = [];
    if (responseData[0].length > 0) {
      if (Array.isArray(JSON.parse(responseData[0][0]?.Message)) && JSON.parse(responseData[0][0]?.Message).length > 0) {
        JSON.parse(responseData[0][0]?.Message ?? 'null')?.map((msg: any) => {
          messageArr.push(msg);
        });
      } else {
        messageArr.push(JSON.parse(responseData[0][0]?.Message));
      }
    }

    if (responseData[0]?.length > 0) {
      res.status(200).send({ statusCode: 200, message: 'Customer history Data', customerValidateData: responseData, messageList: messageArr });
    } else {
      res.status(200).send({ statusCode: 404, message: 'No Data Found', customerValidateData: null });
    }
  } catch (err) {
    console.log('webChatGetAvailableAgent err :::::', err?.message);
    logger.error('webChatGetAvailableAgent catch', { data: req?.params, Err: err });
  }
};

export const webChatValidateCustomerByUid = async (req: any, res: any) => {
  try {
    const data = req.params.chatUniqIdentifier.trim();
    const responseData: any = await getCustomerChatHistoryByUid(data);
    const messageArr: any = [];
    if (responseData[0].length > 0) {
      if (Array.isArray(JSON.parse(responseData[0][0]?.Message)) && JSON.parse(responseData[0][0]?.Message).length > 0) {
        JSON.parse(responseData[0][0]?.Message ?? 'null')?.map((msg: any) => {
          messageArr.push(msg);
        });
      } else {
        messageArr.push(JSON.parse(responseData[0][0]?.Message));
      }
    }

    if (responseData[0]?.length > 0) {
      res.status(200).send({ statusCode: 200, message: 'Customer history Data', customerValidateData: responseData, messageList: messageArr });
    } else {
      res.status(200).send({ statusCode: 404, message: 'No Data Found', customerValidateData: null });
    }
  } catch (err) {
    console.log(err?.message);
    console.log('webChatValidateCustomerByUid err :::::', req?.params, 'error', err?.message);
    logger.error('webChatGetAvailableAgent catch', { data: req?.params, Err: err });
  }
};

export const storeCustomerCallBackRequest = async (req: any, res: any) => {
  try {
    const data: any = {};
    data.ChatHistoryId = req.body?.ChatHistoryId;
    data.CallBackDatetime = req.body?.CallBackStatus;
    await createChatSession(data);
    res.status(200).send({ statusCode: 200, message: 'Customer callback request insert' });
  } catch (err) {
    res.status(500).send({ statusCode: 500, message: 'Internal Server Error' });
    console.log('storeCustomerCallBackRequest err :::::', req?.body, 'error', err?.message);
    logger.error('storeCustomerCallBackRequest catch', { data: req?.body, Err: err });
  }
};

export const chatCustomerFileUpload = async (req: any, res: any, done: any) => {
  try {
    const filesarry: any = req.files?.doc;
    const session_id = req.body?.sessionId;
    const uuid = uuidv4();
    let url: any = '';
    for (const chat of filesarry) {
      url = `${configs.chat_path.fileGetUrl}${chat.originalname}`;
      chat.uuid = uuid;
      chat.url = url;
      chat.sessionId = session_id;
      chat.name = `${chat.originalname}`;
      chat.message = chat?.message ?? null;
      delete chat.destination;
      delete chat.originalname;
    }
    res.send({ statusCode: 200, errCode: -1, message: 'success', fileList: filesarry });
  } catch (err) {
    res.send({ statusCode: 500, errCode: 1, message: 'internal server error' });
    console.log('chatCustomerFileUpload err :::::', req?.body, 'req.files', req.files, 'error', err);
    logger.error('chatCustomerFileUpload catch', { data: req?.body, files: req.files, Err: err });
  }
};

export const getAvailableAgentDetails = async (req: any, res: any) => {
  try {
    // const redisadaptor: RedisAdapters = new RedisAdapters();
    const data: any = {
      session_id: req.body.sessionId,
      domain_id: req.body.domainId,
      response: 'success',
      channelType: 'Chat',
      // call_state: 'init',
      callFlowId: req?.body?.callFlowId,
      btnValue: 'transferToQueue_4zxOpMz6W',
      call_type: 'inbound',
      transferCallCount: req.body?.transferCallCount,
      timestamp: req.body?.timestamp
    };
    if (data.transferCallCount === 1) {
      data.status = 'dialTimeout';
    }
  } catch (err) {
    console.log('getAvailableAgentDetails err :::::', req.body, 'Error', err?.message);
    logger.error('getAvailableAgentDetails catch', { data: req?.body, Err: err });
  }
};

/**
 * Get AI Response message
 */

export const getAIChatMessage = async (req: any, res: any) => {
  try {
    const data: any = {};
    data.chatFlowId = req.params?.chid;
    const getAIMesaage: any = await getAIChatMessageDao(data);
    if (getAIMesaage.length > 0) {
      res.status(200).send({ statusCode: 200, message: 'Success', result: getAIMesaage });
    } else {
      res.status(404).send({ statusCode: 404, message: 'No Data Found' });
    }
  } catch (err) {
    res.status(500).send({ statusCode: 500, message: 'Internal Server Error' });
    console.log('getAIChatMessage err :::::', req.params, 'error', err?.message);
    logger.error('getAIChatMessage catch', { data: req?.paarams, Err: err });
  }
};

/**
 * Update Disposition Name
 */

export const updateChatDisposition = async (req: any, res: any) => {
  try {
    const auth = req.headers;
    const data: any = {
      ChatHistoryId: req.body?.ChatHistoryId,
      ChatDurationHistoryId: req.body?.ChatDurationHistoryId,
      domainId: auth.domainId,
      dispositionId: req?.body?.dispositionId ?? 0,
      Summary: req?.body?.summary ?? null,
      afterChatWorkTime: req.body?.afterChatWorkTime ?? 0
    };
    await updateAgentDetails(data);
    res.status(200).send({ statusCode: 200, message: 'Success' });
  } catch (err) {
    res.status(500).send({ statusCode: 500, message: 'Internal Server Error' });
    console.log('updateChatDisposition err :::::', req.body, 'error', err?.message);
    logger.error('updateChatDisposition catch', { data: req?.body, Err: err });
  }
};

/**
 * botConnectIdGenerate Name
 */

export const botConnectIdGenerate = async (req: any, res: any) => {
  try {
    const auth = req.headers;
    const data: any = {
      ChatHistoryId: req.body?.ChatHistoryId ?? 0,
      StartTime: Math.floor(Date.now()),
      IsBot: 1,
      BotId: req.body?.botId ?? 1,
      SourceChatDurationHistoryId: req.body?.sourceChatDurationHistoryId ?? 0
    };
    const updateResult: any = await updateAgentDetails(data);
    if (data?.SourceChatDurationHistoryId !== 0) {
      data.DestinationChatDurationHistoryId = updateResult[0][0]?.ChatDurationHistoryId;
      setTransferChatMessage(data);
    }
    res.status(200).send({ statusCode: 200, message: 'Success', result: updateResult });
  } catch (err) {
    res.status(500).send({ statusCode: 500, message: 'Internal Server Error' });
    console.log('botConnectIdGenerate err :::::', req.body, 'req.body', err?.message);
    logger.error('botConnectIdGenerate catch', { data: req?.body, Err: err });
  }
};

/**
 * agentConnectIdGenerate Name
 */

export const agentConnectIdGenerate = async (req: any, res: any) => {
  try {
    const auth = req.headers;
    const data: any = {
      ChatHistoryId: req.body?.ChatHistoryId ?? 0,
      StartTime: Math.floor(Date.now()),
      AgentId: req.body?.agentId,
      AgentSkillId: req.body?.agentSkillId,
      ChatDurationHistoryId: req.body?.ChatDurationHistoryId ?? null,
      IsBot: 0,
      QueueId: req.body?.queueId,
      SourceChatDurationHistoryId: req.body?.sourceChatDurationHistoryId ?? 0
    };
    const updateResult: any = await updateAgentDetails(data);
    if (data?.SourceChatDurationHistoryId !== 0) {
      data.DestinationChatDurationHistoryId = updateResult[0][0]?.ChatDurationHistoryId;
      setTransferChatMessage(data);
    }
    res.status(200).send({ statusCode: 200, message: 'Success', result: updateResult });
  } catch (err) {
    res.status(500).send({ statusCode: 500, message: 'Internal Server Error' });
    console.log('agentConnectIdGenerate err :::::', req.body, 'error', err?.message);
    logger.error('agentConnectIdGenerate catch', { data: req?.body, Err: err });
  }
};

/**
 *
 * @param req
 * @param res
 */
export const getLocationDetailsHandler = async (req: any, res: any) => {
  try {
    const data: any = {};
    data.IPAddress = req.params?.IPAddress;
    const getlocation: any = await getLocationDetails(data);
    if (Object.keys(getlocation)?.length) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getlocationResponse: getlocation });
    } else {
      res.status(404).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    console.log('getLocationDetailsHandler err :::::', req.params, 'error', err?.message);
    logger.error('getLocationDetailsHandler catch', { data: req?.params, Err: err });
  }
};

/**
 *
 * @param req
 * @param res
 */
export const getAgentAvailabilityHandler = async (req: any, res: any) => {
  try {
    const data: any = {};
    data.domainId = req.body?.domainId;
    data.roleId = '1,2,3,4,5,6';
    const getAgent: any = await getAgentAvailability(data);
    const readyAgents = getAgent.filter((agent: any) => agent?.statusName === 'Ready');
    const agentCount = readyAgents?.length ? 1 : 0;
    res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getAgentResponse: agentCount });
  } catch (err) {
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    console.log('getAgentAvailabilityHandler err :::::', req.body, 'error', err?.message);
    logger.error('getAgentAvailabilityHandler catch', { data: req?.body, Err: err });
  }
};

/**
 *
 * @param req
 * @param res
 */
export const replaceWebchatMessage = async (req: any, res: any) => {
  try {
    const data: any = {};
    data.message = req.body?.messageArr;
    data.chatHistoryId = req.body?.chatHistoryId;
    data.chatDurationHistoryId = req.body?.chatDurationHistoryId;

    const replaceWebchatresult: any = await replaceWebchatDao(data);
    if (replaceWebchatresult?.length) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, replaceWebchatResponse: replaceWebchatresult });
    } else {
      res.status(404).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    console.log('replaceWebchatMessage err :::::', req.body, 'error', err?.message);
    logger.error('replaceWebchatMessage catch', { data: req?.body, Err: err });
  }
};
