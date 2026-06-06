import axios from 'axios';
import { logger } from '../plugins/log';

/**
 *
 * @param req
 * @param link
 * @param data
 * @param flowId
 * @returns
 */
export async function nlpResponse(req: any, comDetails: any, data: any, flowId: any, flow_data: any, inputType: any) {
  try {
    const body = {
      query: req?.text?.body,
      domine_id: data?.domainId,
      website_id: '',
      chatId: flowId?.InstagramFlowId?.toString(),
      company_name: comDetails?.companyName ?? '',
      industry_type: comDetails?.industryType ?? '',
      connector: flowId?.isConnector === 1 ? true : false,
      shop_name: flowId?.connectorName ?? '',
      historyId: data?.InstagramHistoryId,
      customerId: data?.customerInstagramId,
      flow_data: flow_data ?? [],
      flow_type: inputType ?? 0
    };
    logger.info('Sending request to bot:', JSON.stringify(body, null, 2));
    const response = await axios.post(process?.env?.flowBotInsta, body);
    logger.info('Response from nlp>>>>>>>>>>>', JSON.stringify(response.data, null, 2));
    return response;
  } catch (error) {
    logger.error('nlpResponse err:', error);
  }
}

// send nlp facebook CAI bot
export async function getFacebookBot(nlpResult: any, botDetails: any, history: any, comDetails: any, getCoid: any, mediaUrl: any) {
  try {
    const body = {
      query: nlpResult?.body ?? null,
      domine_id: history?.domainId,
      website_id: '',
      chatId: botDetails?.FacebookFlowId?.toString() ?? '',
      company_name: comDetails?.companyName ?? '',
      industry_type: comDetails?.industryType ?? '',
      connector: botDetails?.FacebookFlowId?.isConnector === 1 ? true : false,
      shop_name: botDetails?.FacebookFlowId?.connectorName ?? '',
      historyId: history?.FacebookHistoryId,
      customerId: history?.customerFacebookId,
      coId: getCoid?.[0]?.coid ?? null,
      flow_data: [],
      flow_type: 0,
      audio: mediaUrl.previewUrl ?? null
    };
    logger.info('Sending request to bot:', JSON.stringify(body, null, 2));
    const response = await axios.post(process?.env?.flowBot, body);
    logger.info('Response from nlp>>>>>>>>>>>', JSON.stringify(response.data, null, 2));
    return response;
  } catch (error) {
    logger.error('getFacebookBot err', error);
  }
}

// senti mental Score
export async function sentimentalScoreDataInsta(data: any) {
  try {
    const body = {
      HistoryId: data?.InstagramHistoryId,
      ChatDurationHistoryId: data?.ChatDurationHistoryId ?? 0,
      channelType: data?.channelType,
      domainId: data?.domainId,
      ext: data?.ext ?? 0
    };
    return await axios.post(process?.env?.Sentimental, body);
  } catch (error) {
    logger.error('sentimentalScoreDataInsta err:', error);
  }
}

// senti mental Score
export async function sentimentalScore(data: any) {
  try {
    const body = {
      HistoryId: data?.HistoryId,
      ChatDurationHistoryId: data?.ChatDurationHistoryId ?? 0,
      channelType: data?.channelType,
      domainId: data?.domainId,
      ext: parseInt(data?.ext, 10) || 0
    };

    const response: any = await axios.post(
      process?.env?.Sentimental,
      body,
      {
        headers: { 'Content-Type': 'application/json' },
        timeout: 5000
      }
    );
    return response ?? {};
  } catch (error) {
    logger.error('sentimentalScore', data, error);
    return {};
  }
}

// send nlp Instagram CAI bot
export async function getInstagramBot(nlpResult: any, InstagramFlowId: any, history: any, comDetails: any, getCoid: any, mediaUrl: any) {
  try {
    const body = {
      query: nlpResult?.body ?? null,
      domine_id: history?.domainId,
      website_id: '',
      chatId: InstagramFlowId?.InstagramFlowId?.toString() ?? '',
      company_name: comDetails?.companyName ?? '',
      industry_type: comDetails?.industryType ?? '',
      connector: InstagramFlowId?.isConnector === 1 ? true : false,
      shop_name: comDetails?.connectorName ?? '',
      historyId: history?.InstagramHistoryId,
      customerId: history?.customerInstagramId,
      coId: getCoid?.[0]?.coid ?? null,
      flow_data: [],
      flow_type: 0,
      audio: mediaUrl?.previewUrl ?? null
    };
    logger.info('Sending request to bot:', JSON.stringify(body, null, 2));
    const response = await axios.post(process?.env?.flowBotInsta, body);
    logger.info('Response from nlp>>>>>>>>>>>', JSON.stringify(response.data, null, 2));
    return response;
  } catch (error) {
    logger.error('Error sending message:', error);
  }
}

// send nlp data
export async function nlpResponseData(req: any, comDetails: any, data: any, flowId: any, flow_data: any, inputType: any) {
  try {
    const body = {
      query: req?.text?.body,
      domine_id: data?.domainId,
      website_id: '',
      chatId: flowId.FacebookFlowId?.toString() ?? '',
      company_name: comDetails?.companyName ?? '',
      industry_type: comDetails?.industryType ?? '',
      connector: flowId?.isConnector === 1 ? true : false,
      shop_name: flowId?.connectorName ?? '',
      historyId: data?.FacebookHistoryId,
      customerId: data?.customerFacebookId,
      flow_data: flow_data ?? [],
      flow_type: inputType ?? 0
    };
    logger.info('Sending request to bot:', JSON.stringify(body, null, 2));
    const response = await axios.post(process?.env?.flowBot, body);
    logger.info('Response from nlp>>>>>>>>>>>', JSON.stringify(response.data, null, 2));
    return response;
  } catch (error) {
    logger.error('nlpResponseData err', error);
  }
}

// Agent Assist
export async function AgentAssist(data: any, comDetails: any, ext: any, message: any, assistId: any) {
  try {
    const body = {
      ext,
      message,
      domainId: data?.domainId,
      channelType: data?.channelType,
      channelId: data?.channelType === 'Facebook' ? data?.agentFacebookId : data?.agentInstagramId,
      sessionId: data?.sessionId,
      websiteId: '',
      type: '',
      company_name: comDetails?.companyName ?? '',
      industry_type: comDetails?.industryType ?? '',
      customerId: data?.channelType === 'Facebook' ? data?.customerFacebookId?.toString() ?? '' : data?.customerInstagramId?.toString() ?? '',
      messageId: assistId,
    };
    logger.info('Sending request to assist for bot:', JSON.stringify(body, null, 2));
    const response = await axios.post(process.env.semiChannel, body);
    logger.info('Response from nlp>>>>>>>>>>>', JSON.stringify(response.data, null, 2));
    return response;
  } catch (error) {
    logger.error('AgentAssist err:', error);
  }
}
