import axios from 'axios';
import { logger } from '../plugins/log';
import { channelType } from './constants';

// send nlp data
export async function nlpResponseData(req: any, comDetails: any, config: any, history: any, flow_data: any, inputType: any) {
  try {
    const body = {
      query: req?.text?.body,
      domine_id: parseInt(req?.companyId, 10),
      website_id: '',
      chatId: config?.whatsappFlowId?.toString(),
      company_name: comDetails?.companyName ?? '',
      industry_type: comDetails?.industryType ?? '',
      connector: config?.isConnector === 1 ? true : false,
      shop_name: config?.connectorName ?? '',
      historyId: history?.WhatsappHistoryId,
      customerId: history?.customerNumber,
      flow_data: flow_data ?? [],
      flow_type: inputType ?? 0
    };
    logger.info('Sending request to bot:', JSON.stringify(body, null, 2));
    const response = await axios.post(process?.env?.flowBot, body);
    logger.info('Response from nlp>>>>>>>>>>>', JSON.stringify(response.data, null, 2));
    return response;
  } catch (error) {
    logger.error('nlpResponseData err:', error);
  }
}

// send nlp whatsapp CAI bot
export async function getWhatsappBot(nlpResult: any, comDetails: any, FlowId: any, id: any, getCoid:any, mediaUrl:any, context:any) {
  try {
    const body = {
      query: nlpResult?.body ?? null,
      domine_id: id?.domainId,
      website_id: '',
      chatId: FlowId?.whatsappFlowId?.toString() ?? '',
      company_name: comDetails?.companyName ?? '',
      industry_type: comDetails?.industryType ?? '',
      connector: FlowId?.isConnector === 1 ? true : false,
      shop_name: comDetails?.connectorName ?? '',
      historyId: id?.WhatsappHistoryId,
      customerId: id?.customerNumber,
      coId: getCoid?.[0]?.coid ?? null,
      flow_data: [],
      flow_type: 0,
      audio: mediaUrl?.previewUrl ?? null,
      templateMessage: context?.id ?? null
    };
    logger.info('Sending request to bot:', JSON.stringify(body, null, 2));
    const response = await axios.post(process?.env?.flowBot, body);
    logger.info('Response from nlp>>>>>>>>>>>', JSON.stringify(response.data, null, 2));
    return response;
  } catch (error) {
    logger.error('getWhatsappBot err:', error);
  }
}

// Agent Assist for whatsapp
export async function AgentAssistWhatsapp(data: any, comDetails: any, ext: any, message: any, assistId:any) {
  try {
    const body = {
      ext,
      message,
      domainId: data?.domainId,
      channelType: channelType?.Whatsapp,
      channelId: data?.calledNumber.toString() ?? '',
      sessionId: data?.sessionId,
      websiteId: '',
      type: '',
      company_name: comDetails?.companyName ?? '',
      industry_type: comDetails?.industryType ?? '',
      customerId:data?.customerNumber?.toString() ?? '',
      messageId: assistId ?? ''
    };
    logger.info('Sending request to bot:', JSON.stringify(body, null, 2));
    const response = await axios.post(process.env.semiChannel, body);
    logger.info('Response from nlp>>>>>>>>>>>', JSON.stringify(response.data, null, 2));
    return response;
  } catch (error) {
    logger.error('AgentAssistWhatsapp err:', error);
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

// senti mental Score
export async function sentimentalScoreData(data: any) {
  try {
    const body = {
      HistoryId: data?.WhatsappHistoryId,
      ChatDurationHistoryId: data?.ChatDurationHistoryId ?? 0,
      channelType: data?.channelType,
      domainId: data?.domainId,
      ext: data?.ext ?? 0
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
    logger.error('sentimentalScore err:', error);
    return {};
  }
}
