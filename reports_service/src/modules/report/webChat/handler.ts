import {
  getACWChatReport,
  getagentChatReport,
  getAgentQueueChatReport,
  getChatACWAgentReportHandlerDao,
  getChatAgentReportHandlerDao,
  getChatInteractionReport,
  getChatProfileReportHandlerDao,
  getchatSessionReport
} from '../../../dao/reportChat.dao';
import { getFilterValuesChatDao, getFilterValuesDao } from '../../../dao/voiceReport.dao';
import { logger } from '../../../plugins/log';

import { RESPONSE, Type } from '../../../helpers/constants';

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getChatSessionReportHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      startTmpstmp: req.body.startTmpstmp,
      endTmpstmp: req.body.endTmpstmp,
      timeZone: JSON.stringify(req.body.timeZone),
      domainId: auth.domainId,
      p_limit: req.body.limit,
      p_offset: req.body.offset,
      queueName: req.body?.queueName ? req.body?.queueName : null,
      agentName: req.body?.agentName ? req.body?.agentName : null,
      dispositionName: req.body?.dispositionName ? req.body?.dispositionName : null,
    };
    res.status(200).send({
      statusCode: 200,
      message: RESPONSE.success_message,
      totalCount: 0,
      reportList: [],
      getchatSession: 0
    });
  } catch (err) {
    console.log(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getchatSessionReportHandler - Unexpected Error:', err);
  }
}

export async function getAgentChatReportHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      startTmpstmp: req.body.startTmpstmp,
      endTmpstmp: req.body.endTmpstmp,
      timeZone: JSON.stringify(req.body.timeZone),
      domainId: auth.domainId,
      limit: req.body.limit,
      offset: req.body.offset,
      queueName: req.body?.queueName ? req.body?.queueName : null,
      agentName: req.body?.agentName ? req.body?.agentName : null,
    };
    const response: any = await getagentChatReport(data);
    const totalRecords: any = response[0][0]?.v_Total_Count;
    if (totalRecords > 0) {
      const getAgentChatDetails: any = response[1];
      const uniqueQueueArray: any = [];
      const uniqueAgentArray: any = [];
      getAgentChatDetails.forEach((x: any) => {
        if (!uniqueAgentArray.includes(x.agentName) && x.agentName !== null && x.agentName !== undefined && x.agentName !== '') {
          uniqueAgentArray.push(x.agentName);
        }
        if (!uniqueQueueArray.includes(x.queueName) && x.queueName !== null && x.queueName !== undefined && x.queueName !== '') {
          uniqueQueueArray.push(x.queueName);
        }
      });

      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        queueList: uniqueQueueArray,
        agentList: uniqueAgentArray,
        totalCount: totalRecords,
        reportList: getAgentChatDetails,
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, totalCount: 0, reportList: [] });
    }
  } catch (err) {
    console.log(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getagentChatReportHandler - Unexpected Error:', err);
  }
}

export async function getChatInteractionReportHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      startTmpstmp: req.body?.startTmpstmp,
      endTmpstmp: req.body?.endTmpstmp,
      timeZone: JSON.stringify(req.body?.timeZone),
      domainId: auth.domainId,
      limit: req.body?.limit,
      offset: req.body?.offset,
      queueName: req.body?.queueName ? req.body?.queueName : null,
      agentName: req.body?.agentName ? req.body?.agentName : null,
      callType: req.body?.callType ? req.body?.callType : null,
      disposition: req.body?.disposition ? req.body?.disposition : null,
    };
    const response: any = await getChatInteractionReport(data);
    const getChatInteractionDetails: any = response[0];
    const totalRecords: any = response[0][0].CountOfData;
    if (totalRecords > 0) {
      const uniqueQueueArray: any = [];
      const uniqueAgentArray: any = [];
      const uniqueDirectionNameArray: any = [];
      const uniqueDispostionArray: any = [];

      getChatInteractionDetails.forEach((x: any) => {
        if (!uniqueAgentArray.includes(x.AgentName) && x.AgentName !== null && x.AgentName !== undefined && x.AgentName !== '') {
          uniqueAgentArray.push(x.AgentName);
        }
        if (!uniqueQueueArray.includes(x.QueueName) && x.QueueName !== null && x.QueueName !== undefined && x.QueueName !== '') {
          uniqueQueueArray.push(x.QueueName);
        }
        if (!uniqueDirectionNameArray.includes(x.Direction) && x.Direction !== null && x.Direction !== undefined && x.Direction !== '') {
          uniqueDirectionNameArray.push(x.Direction);
        }
        if (!uniqueDispostionArray.includes(x.DispositionStatus) && x.DispositionStatus !== null && x.DispositionStatus !== undefined && x.DispositionStatus !== '') {
          uniqueDispostionArray.push(x.DispositionStatus);
        }
      });
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        queueList: uniqueQueueArray,
        agentList: uniqueAgentArray,
        directionList: uniqueDirectionNameArray,
        dispostion: uniqueDispostionArray,
        totalCount: totalRecords,
        getChatInteractionReportRes: getChatInteractionDetails,
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, totalCount: 0, getChatInteractionReportRes: [] });
    }
  } catch (err) {
    console.log(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getChatInteractionReport - Unexpected Error:', err);
  }
}

export async function getACWChatReportHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      startTmpstmp: req.body.startTmpstmp ? req.body.startTmpstmp : null,
      endTmpstmp: req.body.endTmpstmp ? req.body.endTmpstmp : null,
      timeZone: JSON.stringify(req.body?.timeZone),
      domainId: auth.domainId,
      limit: req.body?.limit,
      offset: req.body?.offset,
      queueName: req.body?.queueName ? req.body?.queueName : null,
      agentName: req.body?.agentName ? req.body?.agentName : null,
    };
    const response: any = await getACWChatReport(data);
    const totalRecords = response[0][0]?.v_Total_Count;
    if (totalRecords > 0) {
      const getACWChatDetails = response[1];
      const uniqueAgentArray: any = [];
      const uniqueQueueArray: any = [];
      getACWChatDetails.forEach((x: any) => {
        if (!uniqueAgentArray.includes(x.agentName) && x.agentName !== null && x.agentName !== undefined && x.agentName !== '') {
          uniqueAgentArray.push(x.agentName);
        }
        if (!uniqueQueueArray.includes(x.queueName) && x.queueName !== null && x.queueName !== undefined && x.queueName !== '') {
          uniqueQueueArray.push(x.queueName);
        }
      });
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        queueList: uniqueQueueArray,
        agentList: uniqueAgentArray,
        totalCount: totalRecords,
        reportList: getACWChatDetails,
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, totalCount: 0, reportList: [] });
    }
  } catch (error) {
    console.log(error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getACWChatReportHandler - Unexpected Error:', error);
  }
}

export async function getAgentQueueChatReportHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      startTmpstmp: req.body.startTmpstmp ? req.body.startTmpstmp : null,
      endTmpstmp: req.body.endTmpstmp ? req.body.endTmpstmp : null,
      timeZone: JSON.stringify(req.body?.timeZone),
      domainId: auth.domainId,
      limit: req?.body?.limit,
      offset: req?.body?.offset,
      queueName: req.body?.queueName ? req.body?.queueName : null,
      agentName: req.body?.agentName ? req.body?.agentName : null,
    };
    const response: any = await getAgentQueueChatReport(data);
    const v_totalCount = response[0][0]?.v_Total_Count;
    const getAgentQueueChatDetails = response[1];
    if (v_totalCount > 0) {
      const uniqueAgentArray: any = [];
      const uniqueQueueArray: any = [];
      getAgentQueueChatDetails.forEach((x) => {
        if (!uniqueAgentArray.includes(x.agentName) && x.agentName !== null && x.agentName !== undefined && x.agentName !== '') {
          uniqueAgentArray.push(x.agentName);
        }
        if (!uniqueQueueArray.includes(x.queueName) && x.queueName !== null && x.queueName !== undefined && x.queueName !== '') {
          uniqueQueueArray.push(x.queueName);
        }
      });
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        queueList: uniqueQueueArray,
        agentList: uniqueAgentArray,
        totalCount: v_totalCount,
        getAgentQueueChatReport: getAgentQueueChatDetails,
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, totalCount: 0, getACWChatDetails: [] });
    }
  } catch (error) {
    console.log(error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getAgentQueueChatReport - Unexpected Error:', error);
  }
}

export async function getChatProfileReportHandler(req: any, res: any) {
  try {
    const data: any = {
      startTmpstmp: req.body.startTmpstmp ? req.body.startTmpstmp : null,
      endTmpstmp: req.body.endTmpstmp ? req.body.endTmpstmp : null,
      // timeZone: JSON.stringify(req.body?.timeZone),
      timeZone: null,
      domainId: req?.headers?.domainId,
      queueName : req?.body?.queueName ? req.body?.queueName : null,
      agentName : req?.body?.agentName ? req.body?.agentName : null,
      disposition : req?.body?.disposition ? req?.body?.disposition : null,
      offset : req?.body?.offset,
      limit: req?.body?.limit,
    };

    const getChatProfileResponse: any = await getChatProfileReportHandlerDao(data);
    if (getChatProfileResponse[3][0]?.totalCount > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        type: Type.chatProfile,
        totalCount: getChatProfileResponse[3][0]?.totalCount,
        queueList: getChatProfileResponse[2],
        agentList: getChatProfileResponse[0],
        dispositionList: getChatProfileResponse[1],
        reportList: getChatProfileResponse[4],
      });
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        totalCount: 0,
        reportList: [],
      });
    }

  } catch (error) {
    console.log('getChatProfileReportHandler catch error: ', error);
    req.log.error(error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getAgentQueueCgetChatProfileReportHandlerDaohatReport - Unexpected Error:', error);
  }

}

export async function getChatAgentReportHandler(req: any, res: any) {
  try {
    const data: any = {
      startTmpstmp: req.body.startTmpstmp ? req.body.startTmpstmp : null,
      endTmpstmp: req.body.endTmpstmp ? req.body.endTmpstmp : null,
      // timeZone: JSON.stringify(req.body?.timeZone),
      timeZone: null,
      domainId: req?.headers?.domainId,
      queueName : req?.body?.queueName ? req.body?.queueName : null,
      agentName : req?.body?.agentName ? req.body?.agentName : null,
      offset : req?.body?.offset,
      limit: req?.body?.limit,
    };

    const getChatAgentResponse: any = await getChatAgentReportHandlerDao(data);
    if (getChatAgentResponse[3][0]?.totalCount > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        type: Type.chatAgent,
        totalCount: getChatAgentResponse[4][0]?.totalCount,
        queueList: getChatAgentResponse[0],
        agentList: getChatAgentResponse[1],
        dispositionList: getChatAgentResponse[2],
        reportList: getChatAgentResponse[4],
      });
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        totalCount: 0,
        reportList: [],
      });
    }
  } catch (err) {
    console.log('getChatAgentReportHandler catch error: ', err);
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getChatAgentReportHandler - Unexpected Error:', err);
  }
}

export async function getChatACWAgentReportHandler(req: any, res: any) {
  try {
    const data: any = {
      startTmpstmp: req.body.startTmpstmp ? req.body.startTmpstmp : null,
      endTmpstmp: req.body.endTmpstmp ? req.body.endTmpstmp : null,
      // timeZone: JSON.stringify(req.body?.timeZone),
      timeZone: null,
      domainId: req?.headers?.domainId,
      queueName : req?.body?.queueName ? req.body?.queueName : null,
      agentName : req?.body?.agentName ? req.body?.agentName : null,
      offset : req?.body?.offset,
      limit: req?.body?.limit,
    };

    const getChatACWAgentResponse : any = await getChatACWAgentReportHandlerDao(data);

    if (getChatACWAgentResponse[3][0]?.totalCount > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        type: Type.chatACW,
        totalCount: getChatACWAgentResponse[0][0]?.totalCount,
        queueList: getChatACWAgentResponse[0],
        agentList: getChatACWAgentResponse[1],
        dispositionList: getChatACWAgentResponse[2],
        reportList: getChatACWAgentResponse[4],
      });

    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        totalCount: 0,
        reportList: [],
      });
    }

  } catch (error) {
    console.log('getChatACWAgentReportHandler catch error: ', error);
    req.log.error(error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getAgentQueueChatReport - Unexpected Error:', error);
  }
}
