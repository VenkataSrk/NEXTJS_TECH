import { constants } from 'buffer';
import {
  getEmailAgentReportHandlerDaO,
  getEmailInterationReportDao,
  getEmailReportHandlerDao,
} from '../../../dao/emailReportdao';
import { RESPONSE, Type } from '../../../helpers/constants';
import { logger } from '../../../plugins/log';

export async function getEmailReportHandler(req: any, res: any) {
  try {
    const data: any = {
      startTmpstmp: req?.body?.startTmpstmp,
      endTmpstmp: req?.body?.endTmpstmp,
      //   timeZone: JSON.stringify(req?.body.timeZone),
      timeZone: null, // currently added null
      domainId: req?.headers?.domainId,
      agentName: req?.body?.agentName ? req?.body?.agentName : null,
      queueName: req?.body?.queueName ? req?.body?.queueName : null,
      disposition: req?.body?.disposition ? req?.body?.disposition : null,
      limit: req?.body?.limit,
      offset: req?.body?.offset,
    };

    const emailResponse: any = await getEmailReportHandlerDao(data);
    if (emailResponse[2][0]?.TotalCount > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        type: Type.emailProfile,
        agentList: emailResponse[0],
        dispositionList: emailResponse[1],
        totalCount: emailResponse[2][0]?.TotalCount,
        reportList: emailResponse[3]
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
    console.log('getEmailReportHandler catch error: ', error);
    req.log.error(error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getEmailReportHandler - Unexpected Error:', error);
  }
}

export async function getEmailAgentReportHandler(req: any, res: any) {
  try {
    const data: any = {
      startTmpstmp: req?.body?.startTmpstmp,
      endTmpstmp: req?.body?.endTmpstmp,
      //   timeZone: JSON.stringify(req?.body.timeZone),
      timeZone: null, // currently added null
      domainId: req?.headers?.domainId,
      agentName: req?.body?.agentName ? req?.body?.agentName : null,
      queueName: req?.body?.queueName ? req?.body?.queueName : null,
      limit: req?.body?.limit,
      offset: req?.body?.offset,
    };

    const agentReportResponse: any = await getEmailAgentReportHandlerDaO(data);
    if (agentReportResponse[2][0]?.totalCount > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        type: Type.emailAgent,
        totalCount: agentReportResponse[2][0]?.totalCount,
        agentList: agentReportResponse[1],
        queueList: agentReportResponse[0],
        reportList: agentReportResponse[3],
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
    console.log('getEmailAgentReportHandler catch error: ', error);
    req.log.error(error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getEmailAgentReportHandler - Unexpected Error:', error);
  }
}

export async function getEmailInterationReport(req: any, res: any) {
  try {
    const data: any = {
      startTmpstmp: req?.body?.startTmpstmp,
      endTmpstmp: req?.body?.endTmpstmp,
      timeZone: null,
      domainId: req?.headers?.domainId,
      queueName: req?.body?.queueName ? req?.body?.queueName : null,
      agentName: req?.body?.agentName ? req?.body?.agentName : null,
      direction: req?.body?.direction ? req?.body?.direction : null,
      disposition: req?.body?.disposition ? req?.body?.disposition : null,
      offset: req?.body?.offset,
      limit: req?.body?.limit,
    };
    const emailInterationResponse: any = await getEmailInterationReportDao(data);
    if (emailInterationResponse[3][0]?.totalCount > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        type: Type.emailInteraction,
        totalCount: emailInterationResponse[3][0]?.totalCount,
        agentList: emailInterationResponse[0],
        dispositionList: emailInterationResponse[1],
        directionList: emailInterationResponse[2],
        reportList: emailInterationResponse[4],

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
    console.log('getEmailInterationReport catch error: ', error);
    req.log.error(error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getEmailInterationReport - Unexpected Error:', error);
  }
}
