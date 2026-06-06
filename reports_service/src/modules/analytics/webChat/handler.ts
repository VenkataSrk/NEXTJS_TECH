import axios from 'axios';
import { cloneDeep } from 'sequelize/types/utils';
import {
  agentChatPerformanceMetricFilterDao,
  agentChatServicelevelMetricFilterDao,
  agentInboundChatAnalysis,
  agentInboundChatAnalysisDao,
  agentInboundChatVolume,
  agentInboundTrendChat,
  agentPerfomanceTrendChat,
  agentPerformanceChatAnalysis,
  agentServiceLevelAgreementMetricsDao,
  agentServiceLevelAnalysis,
  chatPerformanceDao,
  chatPerformanceMetricsApi,
  chatPerformanceTrendChat,
  chatServiceLevel,
  createAgentAnalyticsDao,
  editQueueAgent,
  getAnalysisQueueChatReportDao,
  getAnalyticsChatSessionReport,
  getAnalyticsQueueChatReport,
  getAnalyticsSkillChatReport,
  getChatDispositionStatus,
  getChatDispositionViewDetailsDao,
  getCoulumnAgentAnalyticsDao,
  getEditColumnAnalytics,
  getInboundAgentField,
  getInboundAgentServiceLevel,
  getInboundAgentServiceLevelSla,
  getInboundPerformance,
  getInboundQueueField,
  getInboundQueuePerformance,
  getInboundQueueServiceLevel,
  getInboundQueueServiceLevelSla,
  getVisitorInsightDao,
  inboundChatAnalysisDao,
  inboundChatAnalysisMetricsApi,
  inboundTrendChat,
  insertUpdateVisitorDao,
  queueInboundChatVolume,
  serviceLevelAgreementMetricsApi,
  serviceLevelTrendChat
} from '../../../dao/chatAnalytics.dao';
import { RESPONSE } from '../../../helpers/constants';
import { logger } from '../../../plugins/log';

/**
 *
 * @param req
 * @param res
 * @param done
 */

export async function getChatDispositionStatusHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      startTmpstmp: req.body.startTmpstmp,
      endTmpstmp: req.body.endTmpstmp,
      timeZone: req.body.timezone,
      domainId: auth.domainId
    };
    const getChatDispositionStatusDetails: any = await getChatDispositionStatus(data);

    const uniqueQueueNames: any = [...new Set(getChatDispositionStatusDetails[0]?.map((item: any) => item.queueName))];
    const uniqueDispositionNames: any = [...new Set(getChatDispositionStatusDetails[0]?.map((item: any) => item.dispositionname))];
    const response: any = [];
    uniqueDispositionNames.forEach((dispositionName: any) => {
      const dispositionObj: any = {
        label: dispositionName,
        data: []
      };
      uniqueQueueNames.forEach((queueName: any) => {
        const matchingData: any = getChatDispositionStatusDetails[0]?.find((item: any) => item.queueName === queueName && item.dispositionname === dispositionName);
        dispositionObj.data.push(matchingData ? matchingData.NoOfCharts : 0);
      });
      response.push(dispositionObj);
    });

    await response?.map((report: any) => {
      switch (report?.label.toLowerCase()) {
        case 'resolved':
          report.borderColor = 'red';
          report.pointRadius = 0;
          report.borderWidth = 2;
          break;
        case 'unresolved':
          report.borderColor = 'blue';
          report.pointRadius = 0;
          report.borderWidth = 2;
          break;
        case 'transferred':
          report.borderColor = 'black';
          report.pointRadius = 0;
          report.borderWidth = 2;
          break;
        case 'follow-up':
          report.borderColor = 'green';
          report.pointRadius = 0;
          report.borderWidth = 2;
          break;
        case 'closed':
          report.borderColor = 'orange';
          report.pointRadius = 0;
          report.borderWidth = 2;
          break;
        case 'do not contact':
          report.borderColor = 'green';
          report.pointRadius = 0;
          report.borderWidth = 2;
          break;
        default:
          const colorArr = ['yellow', 'pink', 'brown', 'puple', 'gray', 'lime', 'cyan', 'indigo', 'gold', 'lavender'];
          const i = Math.round(Math.random() * 10);
          report.borderColor = colorArr[i];
          report.pointRadius = 0;
          report.borderWidth = 2;
          break;
      }
    });
    const dataResult = {
      labels: uniqueQueueNames,
      datasets: response
    };
    if (getChatDispositionStatusDetails.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getCallLiveDashboardRes: dataResult,
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getCallLiveDashboardRes: [] });
    }
  } catch (err) {
    console.log(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getChatDispositionStatusHandler - Unexpected Error:', err);
  }
}

export async function getChatDispositionViewDetails(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      startTmpstmp: req.body.startTmpstmp,
      endTmpstmp: req.body.endTmpstmp,
      timeZone: req.body.timeZone,
      domainId: auth.domainId,
      queueName: req.body?.queueName ? req.body?.queueName : null,
      dispositionName: req.body?.dispositionName ? req.body?.dispositionName : null,
    };
    const getChatDispositionDetails: any = await getChatDispositionViewDetailsDao(data);
    if (getChatDispositionDetails.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getCallLiveDashboardRes: getChatDispositionDetails[0],
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getCallLiveDashboardRes: [] });
    }
  } catch (err) {
    console.log(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getChatDispositionViewDetails - Unexpected Error:', err);
  }
}

export async function getAnalyticsQueueChatReportHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      startTmpstmp: req.body.startTmpstmp ? req.body.startTmpstmp : null,
      endTmpstmp: req.body.endTmpstmp ? req.body.endTmpstmp : null,
      timeZone: req.body.timezone,
      domainId: auth.domainId,
      queueName: req.body.queueName ? req.body.queueName : null,
      limit: req.body.limit,
      offset: req.body.offset
    };

    const response: any = await getAnalyticsQueueChatReport(data);
    const totalRowCount = response[0][0]?.v_Total_Count;
    const getQueueChatReportDetails = response[2];
    const chartDataPoints: any = response[1];

    if (totalRowCount > 0) {
      const uniqueAgentArray: any = [];
      const uniqueQueueArray: any = [];
      const queueChartReport: any = [];

      getQueueChatReportDetails.forEach((x) => {
        if (!uniqueAgentArray.includes(x.agentName)) {
          uniqueAgentArray.push(x.agentName);
        }
        if (!uniqueQueueArray.includes(x.queueName)) {
          uniqueQueueArray.push(x.queueName);
        }
      });

      Object.keys(chartDataPoints[0]).forEach((key: any) => {
        const skillData: any = { label: '', data: [] };
        console.log(key);
        skillData.label = key;
        chartDataPoints.forEach((element) => {
          skillData.data.push(element[key]);
        });
        queueChartReport.push(skillData);
        if (key === 'chatPresented') {
          skillData.borderColor = 'rgba(102,119,153,1)';
          skillData.pointRadius = 0;
          skillData.borderWidth = 1.5;
          skillData.backgroundColor = 'rgba(102, 119, 153,0.5)';
        }
        if (key === 'chatHandled') {
          skillData.borderColor = 'rbga(113,82,1,1)';
          skillData.pointRadius = 0;
          skillData.borderWidth = 1.5;
          skillData.backgroundColor = 'rgba(53, 162, 235, 0.5)';
        }
        if (key === 'chatAbondened') {
          skillData.borderColor = 'rgba(242,46,87,1)';
          skillData.pointRadius = 0;
          skillData.borderWidth = 1.5;
          skillData.backgroundColor = 'rgba(53, 162, 235, 0.5)';
        }
      });
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        agentList: uniqueAgentArray,
        queueList: uniqueQueueArray,
        datasets: queueChartReport,
        totalCount: totalRowCount,
        getQueueChatReportRes: getQueueChatReportDetails,
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getQueueChatReportRes: [] });
    }
  } catch (err) {
    console.log(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getAnalyticsQueueChatReportHandler - Unexpected Error:', err);
  }
}

export async function getQueueAnalysisChatReportHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      startTmpstmp: req.body.startTmpstmp,
      endTmpstmp: req.body.endTmpstmp,
      timeZone: req.body.timezone,
      domainId: auth.domainId,
    };
    const getQueueChatReportDetails: any = await getAnalysisQueueChatReportDao(data);
    if (getQueueChatReportDetails.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getQueueChatReportRes: getQueueChatReportDetails[0],
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getQueueChatReportRes: [] });
    }
  } catch (err) {
    console.log(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getQueueAnalysisChatReportHandler - Unexpected Error:', err);
  }
}
function toLowerCase() {
  throw new Error('Function not implemented.');
}

export async function getAnalyticsSkillChatReportHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      startTmpstmp: req.body.startTmpstmp ? req.body.startTmpstmp : null,
      endTmpstmp: req.body.endTmpstmp ? req.body.endTmpstmp : null,
      timeZone: req.body.timezone,
      domainId: auth.domainId,
      agentName: req.body.agentName ? req.body.agentName : null,
      queueName: req.body.queueName ? req.body.queueName : null,
      skillName: req.body.skillName ? req.body.skillName : null,
      source: req.body.source ? req.body.source : null,
      limit: req.body.limit,
      offset: req.body.offset
    };
    const response: any = await getAnalyticsSkillChatReport(data);
    const totalRowCount = response[0][0]?.v_Total_Count;
    const chartDataPoints: any = response[1];
    const getSkillChatReportDetails = response[2];

    if (totalRowCount > 0) {
      const uniqueQueueArray: any = [];
      const uniqueSkillArray: any = [];
      const skillChartReport: any = [];

      chartDataPoints.forEach((x) => {
        if (!uniqueQueueArray.includes(x.queueName)) {
          uniqueQueueArray.push(x.queueName);
        }
        if (!uniqueSkillArray.includes(x.skillName)) {
          uniqueSkillArray.push(x.skillName);
        }
      });
      chartDataPoints.forEach((element) => {
        skillChartReport.push(element);
        Object.keys(element).map((key) => {
          if (key === 'totalChat') {
            const totalChat: any = { data: '' };
            totalChat.data = element[key];
            totalChat.borderColor = 'white ';
            totalChat.pointRadius = 0;
            totalChat.borderWidth = 1.5;
            totalChat.backgroundColor = 'rgba(102, 119, 153,0.5)';
            element[key] = totalChat;
          }
        });
      });
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        queueList: uniqueQueueArray,
        skillList: uniqueSkillArray,
        datasets: chartDataPoints,
        totalCount: totalRowCount,
        getSkillChatReportRes: getSkillChatReportDetails,
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getSkillChatReportRes: [] });
    }
  } catch (err) {
    console.log(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getAnalyticsSkillChatReportHandler - Unexpected Error:', err);
  }
}

export async function getAnalyticsChatSessionReportHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      startTmpstmp: req.body.startTmpstmp ? req.body.startTmpstmp : null,
      endTmpstmp: req.body.endTmpstmp ? req.body.endTmpstmp : null,
      domainId: auth.domainId,
      limit: req.body.limit,
      offset: req.body.offset
    };
    const response: any = await getAnalyticsChatSessionReport(data);
    const totalRowCount = response[0][0]?.v_Total_Count;
    const chartDataPoints: any = response[1];
    const getChatSessionReportDetails = response[2];

    if (totalRowCount > 0) {
      const chatHours: any = [];
      chartDataPoints.forEach((x) => {
        if (x.chatHours !== null || x.chatHours !== '') {
          chatHours.push(x.chatHours);
        }
      });

      const totalChat: any = { label: '', data: [] };
      const chatAnswered: any = { label: '', data: [] };
      const chatMissed: any = { label: '', data: [] };
      const chatTransferred: any = { label: '', data: [] };
      const graphData: any = [];

      chartDataPoints.forEach((element) => {
        Object.keys(element).map((key) => {
          if (key === 'totalChat') {
            totalChat.data.push(element[key]);
            totalChat.label = 'totalNumberOfChat';
            totalChat.queueName = element.queueName;
            totalChat.borderColor = 'rgba(102,119,153,1)';
            totalChat.pointRadius = 0;
            totalChat.borderWidth = 1.5;
            totalChat.backgroundColor = 'rgba(102, 119, 153,0.5)';
          }
          if (key === 'chatAnswered') {
            chatAnswered.data.push(element[key]);
            chatAnswered.label = 'chatAnswered';
            chatAnswered.queueName = element.queueName;
            chatAnswered.borderColor = 'rgba(67,156,243,1)';
            chatAnswered.pointRadius = 0;
            chatAnswered.borderWidth = 1.5;
            chatAnswered.backgroundColor = 'rgba(62, 144, 255, 0.5)';
          }
          if (key === 'chatMissed') {
            chatMissed.data.push(element[key]);
            chatMissed.label = 'chatMissed';
            chatMissed.queueName = element.queueName;
            chatMissed.borderColor = 'rgba(242,46,87,1)';
            chatMissed.pointRadius = 0;
            chatMissed.borderWidth = 1.5;
            chatMissed.backgroundColor = 'rgba(0, 113, 82, 0.5)';
          }
          if (key === 'chatsTransferred') {
            chatTransferred.data.push(element[key]);
            chatTransferred.label = 'chatTransferred';
            chatTransferred.queueName = element.queueName;
            chatTransferred.borderColor = 'rbga(113,82,1,1)';
            chatTransferred.pointRadius = 0;
            chatTransferred.borderWidth = 1.5;
            chatTransferred.backgroundColor = 'rgba(53, 162, 235, 0.5)';
          }
        });
      });

      graphData.push(totalChat, chatAnswered, chatMissed, chatTransferred);

      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        chatHoursList: chatHours,
        datasets: graphData,
        totalCount: totalRowCount,
        getChatSessionReportRes: getChatSessionReportDetails,
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getSkillChatReportRes: [] });
    }
  } catch (err) {
    console.log(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getAnalyticsChatSessionReportHandler - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function inboundChatAnalysisMetrics(req: any, res: any, done: any) {
  try {
    const data: any = {
      domainId: req?.headers?.domainId,
      offset: req?.query?.offset,
      limit: req?.query?.limit
    };
    const inboundChatAnalysis: any = await inboundChatAnalysisMetricsApi(data);
    if (inboundChatAnalysis[0].length !== 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        result: inboundChatAnalysis[1]
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('inboundChatAnalysisMetrics - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function chatPerformanceMetrics(req: any, res: any, done: any) {
  try {
    const data: any = {
      domainId: req?.headers?.domainId,
      offset: req?.query?.offset,
      limit: req?.query?.limit
    };
    const chatPerformance: any = await chatPerformanceMetricsApi(data);
    if (chatPerformance[1].length !== 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        result: chatPerformance[1]
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('inboundChatAnalysisMetrics - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function serviceLevelAgreementMetrics(req: any, res: any, done: any) {
  try {
    const data: any = {
      domainId: req?.headers?.domainId,
      offset: req?.query?.offset,
      limit: req?.query?.limit
    };
    const serviceLevelAgreement: any = await serviceLevelAgreementMetricsApi(data);
    if (serviceLevelAgreement[1].length !== 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        result: serviceLevelAgreement[1]
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('serviceLevelAgreementMetrics - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function inboundChatAnalysisMetricsFilter(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTimeStamp: req?.body?.startTimeStamp,
      endTimeStamp: req?.body?.endTimeStamp,
      domainId: req?.headers?.domainId,
      queueId: req?.body?.queueId,
      offset: req?.body?.offset,
      limit: req?.body?.limit
    };
    const inboundChat: any = await inboundChatAnalysisDao(data);
    if (inboundChat?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        queueDetails: inboundChat[0],
        inboundChatRes: inboundChat[2]
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    console.log(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('inboundChatAnalysis - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function chatPerformanceMetricsFilter(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTimeStamp: req?.body?.startTimeStamp,
      endTimeStamp: req?.body?.endTimeStamp,
      domainId: req?.headers?.domainId,
      queueId: req?.body?.queueId,
      offset: req?.body?.offset,
      limit: req?.body?.limit
    };
    const chatPerformanceData: any = await chatPerformanceDao(data);
    if (chatPerformanceData?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        queueDetails: chatPerformanceData[1],
        chatPerformanceRes: chatPerformanceData[4]
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('chatPerformance - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function chatServiceLevelMetricsFilter(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTimeStamp: req?.body?.startTimeStamp,
      endTimeStamp: req?.body?.endTimeStamp,
      domainId: req?.headers?.domainId,
      queueId: req?.body?.queueId,
      offset: req?.body?.offset,
      limit: req?.body?.limit
    };
    const ServiceLevel: any = await chatServiceLevel(data);
    if (ServiceLevel?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        queueDetails: ServiceLevel[1],
        chatServiceLevelRes: ServiceLevel[4]
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('chatServiceLevelMetricsFilter - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function inboundTrendChatHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTimeStamp: req?.body?.startTimeStamp,
      endTimeStamp: req?.body?.endTimeStamp,
      domainId: req?.headers?.domainId,
      columnName: req?.headers?.columnName ?? null,
    };
    const TrendChat: any = await inboundTrendChat(data);
    if (TrendChat?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        TrendChat: TrendChat[0],
        learnMoreAboutTrendChatRes: TrendChat[1]
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('inboundTrendChatHandler - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function agentInboundTrendChatHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTimeStamp: req?.body?.startTimeStamp,
      endTimeStamp: req?.body?.endTimeStamp,
      domainId: req?.headers?.domainId,
      columnName: req?.body?.columnName ?? null,
      dateType: req?.body?.datetype,
      agentId: req?.body?.agentId?.length > 0 ? req?.body?.agentId : req?.body?.agentId,
    };
    const TrendChat: any = await agentInboundTrendChat(data);
    if (TrendChat?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        ReceivedCurrentChatCount: TrendChat[0],
        ReceivedPastChatCount: TrendChat[1]
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('agentInboundTrendChatHandler - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function agentPerfomanceTrendChatHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTimeStamp: req?.body?.startTimeStamp,
      endTimeStamp: req?.body?.endTimeStamp,
      domainId: req?.headers?.domainId,
      columnName: req?.body?.columnName ?? null,
      dateType: req?.body?.datetype,
      agentId: req?.body?.agentId?.length > 0 ? req?.body?.agentId : req?.body?.agentId,
    };
    const TrendChat: any = await agentPerfomanceTrendChat(data);
    if (TrendChat?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        ReceivedCurrentChatCount: TrendChat[0],
        ReceivedPastChatCount: TrendChat[1]
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('agentPerfomanceTrendChat - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function chatPerformanceMetricsTrendChat(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTimeStamp: req?.body?.startTimeStamp,
      endTimeStamp: req?.body?.endTimeStamp,
      domainId: req?.headers?.domainId,
      columnName: req?.headers?.columnName ?? null
    };
    const TrendChat: any = await chatPerformanceTrendChat(data);
    if (TrendChat?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        ReceivedCurrentChatCount: TrendChat[0],
        ReceivedPastChatCount: TrendChat[1]
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('chatPerformanceMetricsTrendChat - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function serviceLevelTrendChatHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTimeStamp: req?.body?.startTimeStamp,
      endTimeStamp: req?.body?.endTimeStamp,
      domainId: req?.headers?.domainId,
      columnName: req?.body?.columnName ?? null,
      dateType: req?.body?.datetype,
      agentId: req?.body?.agentId,
      timeZone: req?.body?.timeZone
    };
    const TrendChat: any = await serviceLevelTrendChat(data);
    if (TrendChat?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        ReceivedCurrentChatCount: TrendChat[0],
        ReceivedPastChatCount: TrendChat[1],
        WebChatServiceLevelTrendChart: TrendChat[2],
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('serviceLevelTrendChat - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function agentInboundChatAnalysisMetrics(req: any, res: any, done: any) {
  try {
    const data: any = {
      domainId: req?.headers?.domainId,
      offset: req?.query?.offset,
      limit: req?.query?.limit
    };
    const inboundChatAnalysis: any = await agentInboundChatAnalysis(data);
    if (inboundChatAnalysis.length !== 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        agentInboundChatRes: inboundChatAnalysis[2]
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('agentInboundChatAnalysis - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function agentPerformanceChatAnalysisMetrics(req: any, res: any, done: any) {
  try {
    const data: any = {
      domainId: req?.headers?.domainId,
      limit: parseInt(req?.query?.limit, 10),
      offset: parseInt(req?.query?.offset, 0),
    };
    const agentPerformance: any = await agentPerformanceChatAnalysis(data);
    if (agentPerformance.length !== 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        agentPerformanceRes: agentPerformance[1]
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('agentPerformanceChatAnalysis - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function agentServiceLevelAnalysisMetrics(req: any, res: any, done: any) {
  try {
    const data: any = {
      domainId: req?.headers?.domainId,
      offset: req?.query?.offset,
      limit: req?.query?.limit
    };
    const agentServiceLevel: any = await agentServiceLevelAnalysis(data);
    if (agentServiceLevel.length !== 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        agentPerformanceRes: agentServiceLevel[2]
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('agentServiceLevelAnalysis - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getInboundQueueFieldHandler(req: any, res: any, done: any) {
  try {
    const getInbound: any = await getInboundQueueField();
    const descriptions = getInbound.map((item: { queueinboundheadingdescription: any; }) => item.queueinboundheadingdescription);
    if (getInbound.length !== 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getInboundQueueRes: descriptions
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getInboundQueueField - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getInboundAgentFieldHandler(req: any, res: any, done: any) {
  try {
    const getInbound: any = await getInboundAgentField();
    const description = getInbound.map((iteam: { agentinboundheadingdescription: any; }) => iteam.agentinboundheadingdescription);
    if (getInbound.length !== 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getInboundAgentRes: description
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getInboundPerformanceFieldHandler(req: any, res: any, done: any) {
  try {
    const getInbound: any = await getInboundPerformance();
    const description = getInbound.map((iteam: { agentperformanceheadingdescription: any; }) => iteam.agentperformanceheadingdescription);
    if (getInbound.length !== 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getInboundPerformanceRes: description
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getInboundPerformance - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getInboundQueuePerformanceFieldHandler(req: any, res: any, done: any) {
  try {
    const getInbound: any = await getInboundQueuePerformance();
    const description = getInbound.map((iteam: { queueperformancedescription: any; }) => iteam.queueperformancedescription);
    if (getInbound.length !== 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getInboundQueuePerformanceRes: description
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getInboundQueuePerformance - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getInboundQueueServiceLevelFieldHandler(req: any, res: any, done: any) {
  try {
    const getInbound: any = await getInboundQueueServiceLevel();
    const description = getInbound.map((iteam: { queueservicedescription: any; }) => iteam.queueservicedescription);
    if (getInbound.length !== 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getInboundQueueServiceLevelRes: description
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getInboundQueueServiceLevel - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getInboundQueueServiceLevelSlaFieldHandler(req: any, res: any, done: any) {
  try {
    const getInbound: any = await getInboundQueueServiceLevelSla();
    const description = getInbound.map((iteam: { queueservicedescription: any; }) => iteam.queueservicedescription);
    if (getInbound.length !== 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getInboundQueueServiceLevelSlaRes: description
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getInboundQueueServiceLevelSla - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getInboundAgentServiceLevelFieldHandlers(req: any, res: any, done: any) {
  try {
    const getInbound: any = await getInboundAgentServiceLevel();
    const description = getInbound.map((iteam: { agentservicedescription: any; }) => iteam.agentservicedescription);
    if (getInbound.length !== 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getInboundAgentServiceLevelRes: description
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getInboundAgentServiceLevel - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getInboundAgentServiceLevelSlaFieldHandler(req: any, res: any, done: any) {
  try {
    const getInbound: any = await getInboundAgentServiceLevelSla();
    const description = getInbound.map((iteam: { agentservicelevelheadingdescription: any; }) => iteam.agentservicelevelheadingdescription);
    if (getInbound.length !== 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getInboundAgentServiceLevelSlaRes: description
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getInboundAgentServiceLevelSla - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */

export async function editQueueAgentHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {
      domainId: auth.domainId,
      ext: auth.ext,
      editColumnNames: req?.body?.editColumnNames ?? [],
    };
    await editQueueAgent(data);
    res.status(200).send({ statusCode: 200, message: RESPONSE.success_message });
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('editQueueAgent - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */

export async function getEditColumnAnalyticsHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {
      domainId: auth.domainId,
      ext: auth.ext,
    };
    const getEditColumnRes = await getEditColumnAnalytics(data);
    if (Array.isArray(getEditColumnRes) && getEditColumnRes.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getEditColumnAnalyticsRes: getEditColumnRes[0] });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getEditColumnAnalytics - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function queueInboundChatVolumeHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTimeStamp: req?.body?.startTimeStamp,
      endTimeStamp: req?.body?.endTimeStamp,
      domainId: req?.headers?.domainId,
      volumeName: req?.body?.volumeName ?? null
    };
    data.volumeName = req?.body?.volumeName === 'Chat Volumes by Hour' ? 'chatvolumebyhour' : req?.body?.volumeName === 'Chat Volumes by Day' ? 'chatvolumebyday' : null;
    const VolumeChat: any = await queueInboundChatVolume(data);
    if (VolumeChat?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        queueVolumeChatRes: VolumeChat
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('queueInboundChatVolume - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function agentInboundChatVolumeHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTimeStamp: req?.body?.startTimeStamp,
      endTimeStamp: req?.body?.endTimeStamp,
      domainId: req?.headers?.domainId,
      volumeName: req?.body?.volumeName ?? null
    };
    data.volumeName = req?.body?.volumeName === 'Chat Volumes by Hour' ? 'chatvolumebyhour' : req?.body?.volumeName === 'Chat Volumes by Day' ? 'chatvolumebyday' : null;
    const VolumeChat: any = await agentInboundChatVolume(data);
    if (VolumeChat?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        agentVolumeChatRes: VolumeChat
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('AgentInboundChatVolume - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function agentInboundChatAnalysisMetricsFilterHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTimeStamp: req?.body?.startTimeStamp,
      endTimeStamp: req?.body?.endTimeStamp,
      domainId: req?.headers?.domainId,
      offset: req?.body?.offset,
      limit: req?.body?.limit,
      agentId: req?.body?.agentId.length > 0 ? req?.body?.agentId : null,
    };
    const agentInboundChat: any = await agentInboundChatAnalysisDao(data);
    if (agentInboundChat && agentInboundChat?.length > 0) {
      const AgentDetails = agentInboundChat[0] ?? [];
      const AllAgentInboundChat = agentInboundChat[1] ?? [];
      const AgentInboundChatChart = agentInboundChat[2] ?? [];
      res.status(200).send({
        AgentDetails,
        AllAgentInboundChat,
        AgentInboundChatChart,
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
    logger.error('agentInboundChatAnalysisD - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function agentChatPerformanceMetricsFilterHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTimeStamp: req?.body?.startTimeStamp,
      endTimeStamp: req?.body?.endTimeStamp,
      domainId: req?.headers?.domainId,
      offset: req?.body?.offset,
      limit: req?.body?.limit,
      agentId: req?.body?.agentId.length > 0 ? req?.body?.agentId : null,
    };
    const agentChatPerformance: any = await agentChatPerformanceMetricFilterDao(data);
    if (agentChatPerformance && agentChatPerformance?.length > 0) {
      const AgentDetails = agentChatPerformance[0] ?? [];
      const AllAgentChatPerformance = agentChatPerformance[1] ?? [];
      const AgentChartPerformance = agentChatPerformance[2] ?? [];
      res.status(200).send({
        AgentDetails,
        AllAgentChatPerformance,
        AgentChartPerformance,
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
    logger.error('agentChatPerformanceMetricFilter - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function agentChatServicelevelMetricsFilterHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      startTimeStamp: req?.body?.startTimeStamp,
      endTimeStamp: req?.body?.endTimeStamp,
      domainId: req?.headers?.domainId,
      offset: req?.body?.offset,
      limit: req?.body?.limit,
      agentId: req?.body?.agentId.length > 0 ? req?.body?.agentId : null,
    };
    const agentChatServicelevel: any = await agentChatServicelevelMetricFilterDao(data);
    if (agentChatServicelevel && agentChatServicelevel?.length > 0) {
      const AgentDetails = agentChatServicelevel[0] ?? [];
      const AllagentChatServicelevel = agentChatServicelevel[1] ?? [];
      const ChatServicelevelChart = agentChatServicelevel[2] ?? [];
      res.status(200).send({
        AgentDetails,
        AllagentChatServicelevel,
        ChatServicelevelChart,
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
    logger.error('agentChatServicelevelMetricFilter - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function agentServiceLevelAgreementMetricsHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      domainId: req?.headers?.domainId,
      limit: parseInt(req?.query?.limit, 10),
      offset: parseInt(req?.query?.offset, 0),
    };
    const agentServiceLevel: any = await agentServiceLevelAgreementMetricsDao(data);
    if (agentServiceLevel.length !== 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        agentServiceLevelRes: agentServiceLevel[1]
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('agentServiceLevelAgreementMetrics - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function createAgentAnalyticsHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {
      domainId: auth.domainId,
      ext: auth.ext,
      editColumnNames: req?.body?.editColumnNames ?? [],
      id: req?.body?.id ?? null,
    };
    const AgentAnalyticsResult: any = await createAgentAnalyticsDao(data);
    res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, result: AgentAnalyticsResult[0] });
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('AgentAnalyticsResult - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getCoulumnAgentAnalyticsHandler(req: any, res: any) {
  try {
    const auth = req.headers;
    const data: any = {
      domainId: auth.domainId,
      ext: auth.ext,
      id: req?.params?.id ?? 0,
    };
    const AgentAnalyticsResult: any = await getCoulumnAgentAnalyticsDao(data);
    res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, result: AgentAnalyticsResult[0] });

  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getCoulumnAgentAnalytics - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function insertUpdateVisitorInsight(req: any, res: any) {
  try {
    const auth = req.headers;
    const data: any = {
      domainId: req.body?.domainId,
      visitorId: req.body?.visitorId,
      customerId: req.body?.customerId,
      sessionStartTime: req.body?.dateAndTime,
      sessionEndTime: req.body?.sessionEndTime,
      device: req.body?.deviceType,
      ipAddress: req.body?.ipAddress ?? '192.168.1.1',
      browser: req.body?.browserDetails,
      language: req.body?.language
    };

    try {
      const ipdata: any = await axios.get(`https://ipinfo.io/${data?.ipAddress}/json`);
      const city = ipdata?.data?.city;
      const region = ipdata?.data?.region;
      const country = ipdata?.data?.country;
      const location = `${city},${region},${country}`;
      data.location = location ?? null;
    } catch (error) {
      const insertChatSessionCatch: any = await insertUpdateVisitorDao(data);
      res.status(200).send({ statusCode: 200, message: 'Insert successfully', result: insertChatSessionCatch[0] });
    }
    const insertVistorSight: any = await insertUpdateVisitorDao(data);
    res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, result: insertVistorSight[0] });

  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('insertUpdateVisitor - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getVisitorInsightList(req: any, res: any) {
  try {
    const auth = req.headers;
    const data: any = {
      domainId: auth.domainId,
      limit : req.query?.limit,
      offset: req.query?.offset
    };
    const getVisitorInsightResult: any = await getVisitorInsightDao(data);
    res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, result: getVisitorInsightResult[0] });

  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getVisitorInsight - Unexpected Error:', err);
  }
}
