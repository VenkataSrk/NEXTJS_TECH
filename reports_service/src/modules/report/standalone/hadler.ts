import {
  getStandAloneACWReportDao,
  getStandAloneBotChatReportDao,
  getStandAloneChatReportDao
} from '../../../dao/standAloneReportDao';
import { RESPONSE } from '../../../helpers/constants';
import { logger } from '../../../plugins/log';

export async function getChatRportHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      startTmpstmp: req.body.startTmpstmp,
      endTmpstmp: req.body.endTmpstmp,
      timeZone: null,
      domainId: auth.domainId,
      agentName: req.body?.agentName ? req.body?.agentName : null,
      queueName: req.body?.queueName ? req.body?.queueName : null,
      disposition: req.body?.disposition ? req.body?.disposition : null,
      pageLimit: req.body.limit,
      pageOffset: req.body.offset
    };
    const response: any = await getStandAloneChatReportDao(data);
    // if (response.length > 0) {
    res.status(200).send({
      statusCode: 200,
      message: RESPONSE.success_message,
      reportList: response.length > 0 ? response : [],
      type: 'GET_CHAT_REPORT'
    });
    // }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getoutboundDashboardDetails - Unexpected Error:', err);
  }
}

export async function getAgentBotReport(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      startTmpstmp: req.body.startTmpstmp,
      endTmpstmp: req.body.endTmpstmp,
      timeZone: null,
      domainId: auth.domainId,
      agentName: req.body?.agentName ? req.body?.agentName : null,
      queueName: req.body?.queueName ? req.body?.queueName : null,
      disposition: req.body?.disposition ? req.body?.disposition : null,
      pageLimit: req.body.limit,
      pageOffset: req.body.offset
    };
    const response: any = await getStandAloneChatReportDao(data);
    // if (response.length > 0) {
    res.status(200).send({
      statusCode: 200,
      message: RESPONSE.success_message,
      reportList: response.length > 0 ? response : [],
      type: 'GET_AGENT_BOT_REPORT'
    });
    // }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getAgentBotReport - Unexpected Error:', err);
  }
}

export async function getAgentChatReport(req: any, res: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      startTmpstmp: req.body.startTmpstmp,
      endTmpstmp: req.body.endTmpstmp,
      timeZone: null,
      domainId: auth.domainId,
      agentName: req.body?.agentName ? req.body?.agentName : null,
      queueName: req.body?.queueName ? req.body?.queueName : null,
      pageLimit: req.body.limit,
      pageOffset: req.body.offset
    };
    const response: any = await getStandAloneBotChatReportDao(data);
    // if (response.length > 0) {
    res.status(200).send({
      statusCode: 200,
      message: RESPONSE.success_message,
      reportList: response.length > 0 ? response : [],
      type: 'GET_AGENT_CHAT_REPORT'
    });
    // }
  } catch (error) {
    console.log(error);
    logger.error('getAgentChatReport - Unexpected Error:', error);
  }
}

export async function getAgentACWReport(req: any, res: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      startTmpstmp: req.body.startTmpstmp,
      endTmpstmp: req.body.endTmpstmp,
      timeZone: null,
      domainId: auth.domainId,
      agentName: req.body?.agentName ? req.body?.agentName : null,
      queueName: req.body?.queueName ? req.body?.queueName : null,
      pageLimit: req.body.limit,
      pageOffset: req.body.offset
    };
    const response: any = await getStandAloneACWReportDao(data);
    // if (response.length > 0) {
    res.status(200).send({
      statusCode: 200,
      message: RESPONSE.success_message,
      reportList: response.length > 0 ? response : [],
      type: 'GET_AGENT_ACW_REPORT'
    });
    // }

  } catch (error) {
    console.log(error);
    logger.error('getAgentACWReport - Unexpected Error:', error);
  }
}
