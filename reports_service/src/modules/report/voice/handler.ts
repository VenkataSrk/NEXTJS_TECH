
/**
 * @createdBy <krishnakumar.s@worktual.com>
 * @createdOn <05/04/2023>
 */

import { logger } from '../../../../src/plugins/log';
import {
  editReportColumnDao,
  getAnsweredCallReport,
  getCallBackSummaryDao,
  getEditReportColumnDao,
  getEditReportColumnDao1,
  getFilterValuesDao,
  getFilterValuesForOutBoundDao,
  getTranferCallReport,
  inboundCallReport,
  interActionDao,
  missedCallListDao,
  outboundCallReport
} from '../../../dao/voiceReport.dao';
import {
  RESPONSE,
  Type
} from '../../../helpers/constants';

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getAnsweredCallReportHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTmpstmp: req?.body?.startTmpstmp ?? null,
      endTmpstmp: req?.body?.endTmpstmp ?? null,
      timeZone: JSON.stringify(req?.body?.timeZone) ?? null,
      domainId: req?.headers?.domainId,
      agentName: req?.body?.agentName ? req?.body?.agentName : null,
      queueName: req?.body?.queueName ? req?.body?.queueName : null,
      limit: req?.body?.limit,
      offset: req?.body?.offset
    };

    const response: any = await getAnsweredCallReport(data);
    const totalRecords: any = response[2][0]?.v_Total_Count;
    if (totalRecords > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        type: Type.callProfile,
        totalCount: totalRecords,
        reportList: response[3],
        agentList: response[1],
        queueList: response[0]
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, totalCount: 0, reportList: [] });
    }
  } catch (err) {
    req.log.error(err);
    console.log(err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('updateDispositionForCall - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function missedCallReportHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTmpstmp: req?.body?.startTmpstmp,
      endTmpstmp: req?.body?.endTmpstmp,
      // timeZone: req?.body?.timeZone ?? null,
      timeZone: null,
      domainId: req?.headers?.domainId,
      offset: req?.body?.offset ?? 0,
      limit: req?.body?.limit ?? 50,
      queueIDs: req?.body?.queueName ? req?.body?.queueName : null,
      agentIDs: req?.body?.agentName ? req?.body?.agentName : null,
    };

    const response: any = await missedCallListDao(data);
    const totalRecords: any = response[2][0]?.v_Total_Count;

    if (totalRecords > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        type: Type.callMissed,
        totalCount: totalRecords,
        queueList: response[0],
        agentList: response[1],
        reportList: response[3],
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, totalCount: 0, reportList: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('missedCallReport - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function interActionReportHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      startTmpstmp: req.body.startTmpstmp,
      endTmpstmp: req.body.endTmpstmp,
      timeZone: JSON.stringify(req.body.timeZone),
      domainId: req?.headers?.domainId,
      limit: req.body.limit,
      offset: req.body.offset,
      queueName: req.body?.queueName ? req.body?.queueName : null,
      agentName: req.body?.agentName ? req.body?.agentName : null,
      callType: req.body?.callType ? req.body?.callType : null,
      disposition: req.body?.disposition ? req.body?.disposition : null,
      direction: req.body?.direction ? req.body?.direction : null,
      dispositionIds: req.body?.dispositionIds ? req.body?.dispositionIds : null

    };
    const response: any = await interActionDao(data);
    console.log(response);
    const totalRecords: any = response[4][0]?.v_Total_Count;
    if (totalRecords > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        type: Type.callInteraction,
        totalCount: totalRecords,
        queueList: response[0],
        agentList: response[1],
        reportList: response[5],
        dispositionList: response[2],
        directionList: response[3],
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getInterAcion: 0, reportList: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('Interaction repoert - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function editReportColumn(req: any, res: any, done: any) {
  try {
    const auth: any = req?.headers;
    const editCRportColumn: any = {
      domainId: req?.body?.domainId, // for testing
      ext: req?.body?.ext, // for testing
      editColumn: (Array.isArray(req?.body?.editColumn) && req?.body?.editColumn[0] !== null) ? req?.body?.editColumn : [],
      reportType: req.body?.reportType ? req.body?.reportType : null,
    };

    const result: any = await editReportColumnDao(editCRportColumn);
    const editColumn: any = JSON.parse(result[0][0].editColumn);
    const x: any = {
      editColumn,
      reportId: result[0][0]?.reportId,
      domainId: result[0][0]?.reportId,
      ext: result[0][0]?.ext,
      reportType: result[0][0]?.reportType
    };
    if (result[0]?.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, data: x });
    } else {
      res.status(200).send({ statusCode: 400, message: RESPONSE.incorrect_input, getInterAcion: 0, reportList: [] });
    }
  } catch (err) {
    req.log.error(err);
    console.log(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('Editreportcolumn - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getEditReportColumn(req: any, res: any, done: any) {
  try {
    const data: any = {
      reportId: req?.body?.reportId,
      domainId: req?.headers?.domainId,
      ext: req.headers.ext,
      reportType:  req.body?.reportType,
    };
    const editColumnResult: any = await getEditReportColumnDao1(data);
    if (editColumnResult.length > 0) {
      editColumnResult[0].editColumn = JSON.parse(editColumnResult[0]?.editColumn);
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        result: editColumnResult
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('Editreportcolumnreport - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function outboundCallReportHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTmpstmp: req?.body?.startTmpstmp,
      endTmpstmp: req?.body?.endTmpstmp,
      // timeZone: JSON.stringify(req.body.timeZone),
      timeZone: '[]',
      domainId: req.headers.domainId,
      queueId : req?.body?.queueName  ? req?.body?.queueName  : null,
      agentId  : req?.body?.agentName   ? req?.body?.agentName   : null,
      dispositionId : req?.body?.disposition ?? null,
      direction   : req?.body?.direction  ? req?.body?.direction  : null,
      limit: req.body.limit,
      offset: req.body.offset,
    };

    const resposne: any = await outboundCallReport(data);
    console.log(resposne);

    const totalRecords = resposne[0][0]?.count;
    if (totalRecords > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        type: Type.callOutbound,
        queueList: resposne[1],
        agentList: resposne[2],
        dispositionList: resposne[3],
        totalCount: resposne[0],
        reportList: resposne[4],
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, totalCount: 0, reportList: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('Outboundcallreport - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getTranferCallReportHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      startTmpstmp: req?.body?.startTmpstmp,
      endTmpstmp: req?.body?.endTmpstmp,
      timeZone: JSON.stringify(req?.body?.timeZone),
      domainId: auth.domainId,
      agentName: req?.body?.agentName ? req.body?.agentName : null,
      disposition: req?.body?.disposition ? req.body?.disposition : null,
      queueName: req?.body?.queueName ? req.body?.queueName : null,
      TransferedId: req?.body?.queueName ? req.body?.TransferedId : null,
      limit: req?.body?.limit,
      offset: req?.body?.offset,
    };
    const response: any = await getTranferCallReport(data);
    console.log(response);
    const totalRecords: any = response[3][0]?.v_Total_Count;
    if (totalRecords > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        type: Type.callTransfer,
        queueList: response[0],
        agentList: response[1],
        totalCount: response[3][0]?.v_Total_Count,
        reportList: response[4],
        Transferredtype: response[2]
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getTranfer: 0, getTranferList: [] });
    }
  } catch (err) {
    console.log(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('Transfercallreport- Unexpected Error:', err);
  }
}

export async function getCallBackSummaryHandler(req: any, res: any) {
  try {
    const data: any = {
      startTmpstmp: req?.body?.startTmpstmp,
      endTmpstmp: req?.body?.endTmpstmp,
      // timeZone: JSON.stringify(req?.body?.timeZone),
      timeZone: null,
      domainId: req?.headers?.domainId,
      queueIds: req?.body?.queueName ?? null,
      agentIds: req?.body?.agentName ?? null,
      voiceMail: req?.body?.voiceMail ?? null,
      offset: req?.body?.offset,
      limit: req?.body?.limit,
    };

    const getCallBackSummaryDaoresponse: any = await getCallBackSummaryDao(data);

    if (getCallBackSummaryDaoresponse[2][0]?.TotalCount > 0) {
      const getAnswered: any = getCallBackSummaryDaoresponse[1];

      const uniqueQueueArray: any = [...new Set(getAnswered.map((item: any) => item.agentName))] ?? [];
      const uniqueAgentArray: any = [...new Set(getAnswered.map((item: any) => item.queueName))] ?? [];
      const uniqueCustomerArray: any = [...new Set(getAnswered.map((item: any) => item.customerName))] ?? [];
      const uniqueVoiceMailArray: any = [...new Set(getAnswered.map((item: any) => item.VoiceMail))] ?? [];

      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        type: Type.callBackSummary,
        queueList: getCallBackSummaryDaoresponse[0],
        agentList: getCallBackSummaryDaoresponse[1],
        // customerList: uniqueCustomerArray,
        // voiceMail: uniqueVoiceMailArray,
        totalCount: getCallBackSummaryDaoresponse[2][0]?.TotalCount,
        reportList: getCallBackSummaryDaoresponse[3],
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getcallBackSummaryCount: 0, getCallBackSummaryDaoresponse: [] });
    }
  } catch (error) {
    console.log('getCallBackSummaryHandler catch error: ', error);
    req.log.error(error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function inboundCallReportHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTmpstmp: req?.body?.startTmpstmp,
      endTmpstmp: req?.body?.endTmpstmp,
      // timeZone: JSON.stringify(req.body.timeZone),
      timeZone: '[]',
      domainId: req.headers.domainId,
      queueId : req?.body?.queueName  ? req?.body?.queueName  : null,
      agentId  : req?.body?.agentName   ? req?.body?.agentName   : null,
      dispositionId : req?.body?.disposition ?? null,
      direction   : req?.body?.direction  ? req?.body?.direction  : null,
      limit: req.body.limit,
      offset: req.body.offset,
    };

    const resposne: any = await inboundCallReport(data);
    console.log(resposne);

    const totalRecords = resposne[0][0]?.count;
    if (totalRecords > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        type: Type.callOutbound,
        queueList: resposne[1],
        agentList: resposne[2],
        dispositionList: resposne[3],
        totalCount: resposne[0],
        reportList: resposne[4],
      });

      // const getOutbound = resposne[1];
      // const uniqueQueueArray: any = [];
      // const uniqueAgentArray: any = [];
      // const uniqueDispositionArray: any = [];
      // const uniqueSkillArray: any = [];

      // getOutbound.forEach((x: any) => {
      //   if (!uniqueAgentArray.includes(x.agentName) && x.agentName !== null && x.agentName !== undefined && x.agentName !== '') {
      //     uniqueAgentArray.push(x.agentName);
      //   }
      //   if (!uniqueQueueArray.includes(x.queueName) && x.queueName !== null && x.queueName !== undefined && x.queueName !== '') {
      //     uniqueQueueArray.push(x.queueName);
      //   }
      //   if (!uniqueDispositionArray.includes(x.DispositionStatus) && x.DispositionStatus !== null && x.DispositionStatus !== undefined && x.DispositionStatus !== '') {
      //     uniqueDispositionArray.push(x.DispositionStatus);
      //   }
      // if (!uniqueSkillArray.includes(x.skillName) && x.skillName !== null && x.skillName !== undefined && x.skillName !== '') {
      //   uniqueSkillArray.push(x.skillName);
      // }
      // });

    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, totalCount: 0, reportList: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('Inboundcallreport - Unexpected Error:', err);
  }
}
