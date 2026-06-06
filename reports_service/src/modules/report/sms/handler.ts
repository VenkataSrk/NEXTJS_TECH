import {
  getSmsAgentReportHandlerDaO,
  getSmsInterationReportDao,
  getSmsProfileReportHandlerDao,
} from '../../../dao/smsReportdao';
import { RESPONSE, Type } from '../../../helpers/constants';
import { logger } from '../../../plugins/log';

export async function getSmsProfileReportHandler(req: any, res: any) {
  try {
    const data: any = {
      startTmpstmp: req?.body?.startTmpstmp,
      endTmpstmp: req?.body?.endTmpstmp,
      //   timeZone: JSON.stringify(req?.body.timeZone),
      timeZone: null, // currently added null
      domainId: req?.headers?.domainId,
      // domainId: 10300,
      agentName: req?.body?.agentName ? req?.body?.agentName : null,
      queueName: req?.body?.queueName ? req?.body?.queueName : null,
      disposition: req?.body?.disposition ? req?.body?.disposition : null,
      limit: req?.body?.limit,
      offset: req?.body?.offset,
    };

    const smsResponse: any = await getSmsProfileReportHandlerDao(data);

    if (smsResponse[2][0]?.totalCount > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        type: Type.smsProfile,
        totalCount: smsResponse[2][0]?.totalCount,
        agentList: smsResponse[0],
        dispositionList: smsResponse[1],
        reportList: smsResponse[3],
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
    console.log('getSmsReportHandler catch error: ', error);
    req.log.error(error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getSmsProfileReportHandler - Unexpected Error:', error);
  }
}

export async function getSmsAgentReportHandler(req: any, res: any) {
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

    const agentReportResponse: any = await getSmsAgentReportHandlerDaO(data);
    if (agentReportResponse[1][0]?.totalCount > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        type: Type.smsAgent,
        totalCount: agentReportResponse[1][0]?.totalCount,
        agentList: agentReportResponse[0],
        reportList: agentReportResponse[2],
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
    console.log('getSmsAgentReportHandler catch error: ', error);
    req.log.error(error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getSmsAgentReportHandler - Unexpected Error:', error);
  }
}

export async function getSmsInterationReport(req: any, res: any) {
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
      limit: req?.body?.limit
    };
    const smsInterationResponse: any = await getSmsInterationReportDao(data);
    if (smsInterationResponse[3][0]?.totalCount > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        type: Type.smsInteraction,
        totalCount: smsInterationResponse[3][0]?.totalCount,
        agentList: smsInterationResponse[0],
        dispositionList: smsInterationResponse[1],
        directionList: smsInterationResponse[2],
        reportList: smsInterationResponse[4],
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
    console.log('getSmsInterationReport catch error: ', error);
    req.log.error(error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getSmsInterationReport - Unexpected Error:', error);
  }
}
