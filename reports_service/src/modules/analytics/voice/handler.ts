/**
 * @createdBy <krishnakumar.s@worktual.com>
 * @createdOn <05/04/2023>
 */

import {
  agentCallBackAnalysisDao,
  agentInboundCallAnalysisDao,
  agentOutboundCallAnalysisDao,
  agentServiceLevelCallAnalysisDao,
  agentVoiceMailAnalysisDao,
  callBackTrendChartDao,
  callInboundTrendChartDao,
  callOutboundTrendChartDao,
  callServicelevelTrendChartDao,
  createCallAnalyticsDao,
  editAnalyticsColumnDao,
  getAbandonedCallProfileAnalytic,
  getAnalyticsColumnDao,
  getCoulumnCallAnalyticsDao,
  voiceMailTrendChartDao
} from '../../../dao/voiceAnalytics.dao';
import { RESPONSE } from '../../../helpers/constants';
import { logger } from '../../../plugins/log';

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getMetricsQueueInboundCallHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      startTmpstmp: req.body.startTmpstmp,
      endTmpstmp: req.body.endTmpstmp,
      timeZone: req.body.timeZone,
      domainId: auth.domainId,
    };
    res.status(200).send({
      statusCode: 200,
      message: RESPONSE.not_found,
    });
  } catch (err) {
    req.log.error(err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getMetricsQueueInboundCallHandler - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getQueueDispositionHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {
      startTmpstmp: req.body.startTmpstmp,
      endTmpstmp: req.body.endTmpstmp,
      timeZone: req.body.timeZone,
      domainId: auth.domainId,
    };

    res.status(200).send({
      statusCode: 200,
      message: RESPONSE.not_found,
    });

  } catch (err) {
    req.log.error(err);
    console.log(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getQueueDispositionHandler - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getIntervalAnalyticsHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {
      startTmpstmp: req.body.startTmpstmp,
      endTmpstmp: req.body.endTmpstmp,
      timeZone: req.body.timeZone,
      domainId: auth.domainId,
      p_Limit: req.body.limit,
      p_offset: req.body.offset
    };

    res.status(200).send({
      statusCode: 200,
      message: RESPONSE.not_found,
    });
  } catch (err) {
    req.log.error(err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getIntervalAnalyticsHandler - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getAbandonedCallProfileHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {
      startTmpstmp: req.body.startTmpstmp,
      endTmpstmp: req.body.endTmpstmp,
      timeZone: req.body.timeZone,
      domainId: auth.domainId,
      p_Limit: req.body.limit,
      p_offset: req.body.offset
    };
    const getAbandonedCallRes: any = await getAbandonedCallProfileAnalytic(data);
    const labelArr: any = [];
    const abandonedCallsCountArr: any = [];

    if (Array.isArray(getAbandonedCallRes) && getAbandonedCallRes.length > 0) {
      getAbandonedCallRes.map((report: any) => {
        labelArr.push(report.queueName);
        abandonedCallsCountArr.push(report.AbandonedCalls);
      });
      const datasets = {
        labels: labelArr,
        datasets: [
          {
            label: '# of Votes',
            data: abandonedCallsCountArr,
            backgroundColor: [
              'rgba(255, 99, 132, 0.2)',
              'rgba(54, 162, 235, 0.2)',
              'rgba(255, 206, 86, 0.2)',
              'rgba(75, 192, 192, 0.2)',
              'rgba(153, 102, 255, 0.2)',
              'rgba(255, 159, 64, 0.2)',
            ],
            borderColor: [
              'rgba(255, 99, 132, 1)',
              'rgba(54, 162, 235, 1)',
              'rgba(255, 206, 86, 1)',
              'rgba(75, 192, 192, 1)',
              'rgba(153, 102, 255, 1)',
              'rgba(255, 159, 64, 1)',
            ],
            borderWidth: 1,
          },
        ],
      };

      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getAbandonedCallProfileRes: datasets,
      });
    } else {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.not_found,
      });
    }
  } catch (err) {
    req.log.error(err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getAbandonedCallProfileHandler - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function editAnalyticsColumn(req: any, res: any, done: any) {
  try {
    const auth: any = req?.headers;
    const editCRportColumn: any = {
      analyticsId: req?.body?.analyticsId,
      domainId: req?.body?.domainId, // for testing
      ext: req?.body?.ext, // for testing
      editColumn: (Array.isArray(req?.body?.editColumn) && req?.body?.editColumn[0] !== null) ? req?.body?.editColumn : [],
      reportType: req.body?.reportType ? req.body?.reportType : null,
    };

    const result: any = await editAnalyticsColumnDao(editCRportColumn);
    const editColumn: any = JSON.parse(result[0][0].editColumn);
    const x: any = {
      editColumn,
      analyticsId: result[0][0]?.analyticsId,
      domainId: result[0][0]?.reportId,
      ext: result[0][0]?.ext,
      analyticsType: result[0][0]?.analyticsType
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
    logger.error('Editanalysiscolumn - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getAnalyticsColumn(req: any, res: any, done: any) {
  try {
    const data: any = {
      alalyticsId: req?.body?.analyticsId
    };
    const editColumnResult: any = await getAnalyticsColumnDao(data);
    if (editColumnResult[0][0].length !== null) {
      editColumnResult[0][0].editColumn = JSON.parse(editColumnResult[0][0]?.editColumn);
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        result: editColumnResult[0]
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getAnalyticsColumn - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function agentInboundCallAnalysisMetricsFilterHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTimeStamp: req?.body?.startTimeStamp,
      endTimeStamp: req?.body?.endTimeStamp,
      domainId: req?.headers?.domainId,
      offset: req?.body?.offset,
      limit: req?.body?.limit,
      agentId: req?.body?.agentId.length > 0 ? req?.body?.agentId : null,
    };
    const agentInboundCall: any = await agentInboundCallAnalysisDao(data);
    if (agentInboundCall && agentInboundCall?.length > 0) {
      const AgentDetails = agentInboundCall[0] ?? [];
      const AllAgentInboundCall = agentInboundCall[1] ?? [];
      const AgentInboundCallChart = agentInboundCall[2] ?? [];
      res.status(200).send({
        AgentDetails,
        AllAgentInboundCall,
        AgentInboundCallChart,
        statusCode: 200,
        message: RESPONSE.success_message,
      });
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        result: [],
      });
    }
  } catch (err) {
    req.log.error(err);
    console.log(err);
    res.status(500).send({
      statusCode: 500,
      message: RESPONSE.internal_error,
    });
    logger.error('queueInboundChatVolume - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function agentOutboundCallAnalysisMetricsFilterHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTimeStamp: req?.body?.startTimeStamp,
      endTimeStamp: req?.body?.endTimeStamp,
      domainId: req?.headers?.domainId,
      offset: req?.body?.offset,
      limit: req?.body?.limit,
      agentId: req?.body?.agentId.length > 0 ? req?.body?.agentId : null,
    };
    const agentOutboundCall: any = await agentOutboundCallAnalysisDao(data);
    if (agentOutboundCall && agentOutboundCall?.length > 0) {
      const AgentDetails = agentOutboundCall[0] ?? [];
      const AllAgentOutboundCall = agentOutboundCall[1] ?? [];
      const AgentOutboundCallChart = agentOutboundCall[2] ?? [];
      res.status(200).send({
        AgentDetails,
        AllAgentOutboundCall,
        AgentOutboundCallChart,
        statusCode: 200,
        message: RESPONSE.success_message,
      });
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        result: [],
      });
    }
  } catch (err) {
    req.log.error(err);
    console.log(err);
    res.status(500).send({
      statusCode: 500,
      message: RESPONSE.internal_error,
    });
    logger.error('getAnalyticsColumn - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function agentVoiceMailAnalysisMetricsFilterHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTimeStamp: req?.body?.startTimeStamp,
      endTimeStamp: req?.body?.endTimeStamp,
      domainId: req?.headers?.domainId,
      offset: req?.body?.offset,
      limit: req?.body?.limit,
      agentId: req?.body?.agentId.length > 0 ? req?.body?.agentId?.join(',') : null,
    };
    const agentVoiceMail: any = await agentVoiceMailAnalysisDao(data);
    if (agentVoiceMail && agentVoiceMail?.length > 0) {
      const AgentDetails = agentVoiceMail[0] ?? [];
      const AllAgentVoiceMail = agentVoiceMail[1] ?? [];
      const AgentVoiceMailChart = agentVoiceMail[2] ?? [];
      res.status(200).send({
        AgentDetails,
        AllAgentVoiceMail,
        AgentVoiceMailChart,
        statusCode: 200,
        message: RESPONSE.success_message,
      });
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        result: [],
      });
    }
  } catch (err) {
    req.log.error(err);
    console.log(err);
    res.status(500).send({
      statusCode: 500,
      message: RESPONSE.internal_error,
    });
    logger.error('agentVoiceMail - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function agentCallBackAnalysisMetricsFilterHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTimeStamp: req?.body?.startTimeStamp,
      endTimeStamp: req?.body?.endTimeStamp,
      domainId: req?.headers?.domainId,
      offset: req?.body?.offset,
      limit: req?.body?.limit,
      agentId: req?.body?.agentId.length > 0 ? req?.body?.agentId : null,
    };
    const agentCallBack: any = await agentCallBackAnalysisDao(data);
    if (agentCallBack && agentCallBack?.length > 0) {
      const AgentDetails = agentCallBack[0] ?? [];
      const AllagentCallBack = agentCallBack[1] ?? [];
      const AgentCallBackChart = agentCallBack[2] ?? [];
      res.status(200).send({
        AgentDetails,
        AllagentCallBack,
        AgentCallBackChart,
        statusCode: 200,
        message: RESPONSE.success_message,
      });
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        result: [],
      });
    }
  } catch (err) {
    req.log.error(err);
    console.log(err);
    res.status(500).send({
      statusCode: 500,
      message: RESPONSE.internal_error,
    });
    logger.error('agentCallBack - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function agentServiceLevelCallAnalysisMetricsFilterHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTimeStamp: req?.body?.startTimeStamp,
      endTimeStamp: req?.body?.endTimeStamp,
      domainId: req?.headers?.domainId,
      offset: req?.body?.offset,
      limit: req?.body?.limit,
      agentId: req?.body?.agentId.length > 0 ? req?.body?.agentId?.join(',') : null,
    };
    const agentServiceLevelCall: any = await agentServiceLevelCallAnalysisDao(data);
    if (agentServiceLevelCall && agentServiceLevelCall?.length > 0) {
      const AgentDetails = agentServiceLevelCall[0] ?? [];
      const AllAgentServiceLevel = agentServiceLevelCall[1] ?? [];
      const AgentServiceLevelChart = agentServiceLevelCall[2] ?? [];
      res.status(200).send({
        AgentDetails,
        AllAgentServiceLevel,
        AgentServiceLevelChart,
        statusCode: 200,
        message: RESPONSE.success_message,
      });
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        result: [],
      });
    }
  } catch (err) {
    req.log.error(err);
    console.log(err);
    res.status(500).send({
      statusCode: 500,
      message: RESPONSE.internal_error,
    });
    logger.error('agentServiceLevelCall - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function createCallAnalyticsHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {
      domainId: auth.domainId,
      ext: auth.ext,
      editColumnNames: req?.body?.editColumnNames ?? [],
      id: req?.body?.id ?? null,
    };
    const VoiceAnalyticsResult: any = await createCallAnalyticsDao(data);
    res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, result: VoiceAnalyticsResult[0] });
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('createCallAnalytics - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getCoulumnCallAnalyticsHandler(req: any, res: any) {
  try {
    const auth = req.headers;
    const data: any = {
      id: req?.params?.id ?? 0,
      domainId: auth.domainId,
      ext: auth.ext,
    };
    const EmailAnalyticsResult: any = await getCoulumnCallAnalyticsDao(data);
    res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, result: EmailAnalyticsResult[0] });

  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getCoulumnCallAnalytics - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function agentCallInboundTrendChartHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      domainId: req?.headers?.domainId,
      datetype: req?.body?.datetype ?? null,
      startTmpstmp: req?.body?.startTimeStamp,
      endTmpstmp: req?.body?.endTimeStamp,
      columnName : req?.body?.columnName ?? null,
      agentId  : req?.body?.agentId ?? null,
    };
    const CallTrendChat: any = await callInboundTrendChartDao(data);
    if (CallTrendChat && CallTrendChat?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        ReceivedCurrentChatCount: CallTrendChat[0],
        ReceivedPastChatCount: CallTrendChat[1]
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('CallInboundTrendChart - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function agentCallOutboundTrendChartHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      domainId: req?.headers?.domainId,
      datetype: req?.body?.datetype ?? null,
      startTimeStamp: req?.body?.startTimeStamp,
      endTimeStamp: req?.body?.endTimeStamp,
      columnName : req?.body?.columnName ?? null,
      agentId  : req?.body?.agentId ?? null,
    };
    const CallOutboundTrendChat: any = await callOutboundTrendChartDao(data);
    if (CallOutboundTrendChat && CallOutboundTrendChat?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        ReceivedCurrentChatCount: CallOutboundTrendChat[0],
        ReceivedPastChatCount: CallOutboundTrendChat[1]
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('CallOutboundTrendChart - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function agentCallServicelevelTrendChartHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      domainId: req?.headers?.domainId,
      datetype: req?.body?.datetype ?? null,
      startTimeStamp: req?.body?.startTimeStamp,
      endTimeStamp: req?.body?.endTimeStamp,
      columnName : req?.body?.columnName ?? null,
      agentId  : req?.body?.agentId ?? null,
    };
    const CallServiceTrendChat: any = await callServicelevelTrendChartDao(data);
    if (CallServiceTrendChat && CallServiceTrendChat?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        ReceivedCurrentChatCount: CallServiceTrendChat[0],
        ReceivedPastChatCount: CallServiceTrendChat[1],
        ServicelevelTrendChart:CallServiceTrendChat[2]
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('CallServicelevelTrendChart - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function agentCallBackTrendChartHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      domainId: req?.headers?.domainId,
      datetype: req?.body?.datetype ?? null,
      startTimeStamp: req?.body?.startTimeStamp,
      endTimeStamp: req?.body?.endTimeStamp,
      columnName : req?.body?.columnName ?? null,
      agentId  : req?.body?.agentId ?? null,
    };
    const CallBackTrendChat: any = await callBackTrendChartDao(data);
    if (CallBackTrendChat && CallBackTrendChat?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        ReceivedCurrentChatCount: CallBackTrendChat[0],
        ReceivedPastChatCount: CallBackTrendChat[1],
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('CallBackTrendChart - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function agentVoiceMailTrendChartHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTimeStamp: req?.body?.startTimeStamp,
      endTimeStamp: req?.body?.endTimeStamp,
      domainId: req?.headers?.domainId,
      columnName: req?.body?.columnName ?? null,
    };
    const VoiceMailTrendChat: any = await voiceMailTrendChartDao(data);
    if (VoiceMailTrendChat && VoiceMailTrendChat?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        VoiceMailTrendChat: VoiceMailTrendChat[0],
        learnMoreAboutTrendChatRes: VoiceMailTrendChat[1]
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('VoiceMailTrendChart - Unexpected Error:', err);
  }
}
