import {
  getAgntAssiatanceDao,
  getArrivalDashbordRate,
  getCallLiveDashboard,
  getCallLiveDashboardPieChart,
  getChatBotLiveAgentPerformanceDao,
  getChatBotLiveDashboardDao,
  getChatLiveDashboard,
  getComapreDashboardResponse,
  getCompareDashbaordTrendChartResponse,
  getCustomerCountForYearDao,
  getCustomerSatisfactionHandlerDao,
  getDashboardAgentStatusHandlerDao,
  getDashboardAverageHandlerDao,
  getDashboardPerformanceComparisonHandlerDao,
  getDashboardPiechartAgentStatusDetailsDao,
  getDashboardResponseRating,
  getDashboardSocialMediaStatusHandlerDao,
  getDashboardUtilizationHandlerDao,
  getEmailLiveDashboardPieChart,
  getLiveDashBoardFeautres,
  getOverAllSlaAiDao,
  getSentimentalAnalysisDao,
  getSmsLiveDashboardPieChart,
  getSurveyResponseDao,
  getTrackerDataDao,
  getTrendChartForcustomerDao,
  getUsedNoOfSessionDao,
  getVideoChannelDashMetricsDao,
  getWebchatLiveDashborad,
  roleInfoDao
} from '../../../dao/liveDashboard.dao';
import { RESPONSE } from '../../../helpers/constants';
import { ioredisReport } from '../../../plugins/db';
import { logger } from '../../../plugins/log';

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getCallLiveDashboardHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      startTmpstmp: req.body.startTmpstmp,
      endTmpstmp: req.body.endTmpstmp,
      timeZone: req.body.timezone,
      domainId: auth.domainId,
    };
    res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getCallLiveDashboardRes: [] });
  } catch (err) {
    console.log(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getCallLiveDashboardHandler - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getChatLiveDashboardHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      startTmpstmp: req.body.startTmpstmp,
      endTmpstmp: req.body.endTmpstmp,
      timeZone: req.body.timezone,
      domainId: auth.domainId,
    };
    const getChatLiveDashboardDetails: any = await getChatLiveDashboard(data);
    if (getChatLiveDashboardDetails.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getChatLiveDashboardRes: getChatLiveDashboardDetails[0],
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getChatLiveDashboardRes: [] });
    }
  } catch (err) {
    console.log(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getChatLiveDashboardHandler - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getCallLiveDashboardPieChartHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const getUserRole: any = await roleInfoDao(auth);
    if (auth.roleId) {
      const ccas_role_info = getUserRole?.roleid.toString();
      let ext = 0;
      if (auth.roleId === 3 || auth.roleId === 1) {
        ext = 0;
      } else {
        ext = auth.ext;
      }
      const data: any = {
        startTmpstmp: req?.body?.startTmpstmp,
        endTmpstmp: req?.body?.endTmpstmp,
        timeZone: null,
        domainId: req?.headers?.domainId,
        extNo: req?.headers?.ext,
      };

      const getPieChat: any = await getCallLiveDashboardPieChart(data);

      if (Array.isArray(getPieChat) && getPieChat.length > 0) {
        const pieData = {
          datasets: [
            {
              backgroundColor: ['#FF793D', '#653AE0'],
              borderColor: ['#FF793D', '#653AE0'],
              borderWidth: 1,
              data: [getPieChat[0][0]?.InboundCalls, getPieChat[0][0]?.OutboundCalls],
              label: '# of Votes',
            },
          ],
          TotalCalls: getPieChat[0][0].TotalCalls,
          InboundCalls: getPieChat[0][0].InboundCalls,
          OutboundCalls: getPieChat[0][0].OutboundCalls,
          TotalCallback: getPieChat[0][0].TotalCallback,
          TotalVoicemail: getPieChat[0][0].TotalVoicemail,
          CostPerCall: getPieChat[0][0].CostPerCall,
          CallAbandonRate: getPieChat[0][0].CallAbandonRate,
          CallsTransferRate: getPieChat[0][0].CallsTransferRate,
          AverageWrapUpTime: getPieChat[0][0].AverageWrapUpTime,
          Averageholddurationtime: getPieChat[0][0].Averageholddurationtime,
          AverageCallsPerMinute: getPieChat[0][0].AverageCallsPerMinute,
          unresolved: getPieChat[0][0].unresolved,
          agent_answered_call: getPieChat[0][0].agent_answered_call,
          Resolved: getPieChat[0][0].Resolved,
          Missed: getPieChat[0][0].Missed,
          QueuedCustomers: getPieChat[0][0].QueuedCustomers,
          servicelevel: getPieChat[0][0].servicelevel,
          isOutboundCampaign: getPieChat[0][0].isOutboundCampaign,
        };
        ioredisReport.to(`${data?.domainId}_${data?.ext}`).emit('callLiveDashboard', pieData);
        res.status(200).send({
          statusCode: 200,
          message: RESPONSE.success_message,
          getCallLiveDashboardDetails: pieData,
        });
      } else {
        res.status(200).send({
          statusCode: 200,
          message: RESPONSE.not_found,
          getCallLiveDashboardDetails: [],
        });
      }
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        roleInfo: [],
      });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    console.log('getoutboundDashboardDetails', err);
    logger.error('getoutboundDashboardDetails - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getWebchatLiveDashboradHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      startTmpstmp: req?.body?.startTmpstmp ?? 0,
      endTmpstmp: req?.body?.endTmpstmp ?? 0,
      domainId: auth.domainId,
      ext: auth.ext
    };
    const getWebchatChat: any = await getWebchatLiveDashborad(data);
    const webchat = {
      TotalChat: getWebchatChat[0][0].Total,
      Inbound: getWebchatChat[0][0].Inbound,
      Resolved: getWebchatChat[0][0].Resolved,
      unresolved: getWebchatChat[0][0].Unresolved,
      ChatAbandonRate: getWebchatChat[0][0].Abandoned,
      ChatTransferRate: getWebchatChat[0][0].Transferred,
      servicelevel: getWebchatChat[0][0].servicelevel,
      Missed: getWebchatChat[0][0].Missed,
      QueuedCustomers: getWebchatChat[0][0].QueuedCustomers,
      Callback_requested: getWebchatChat[0][0].Callback_requested
    };
    if (Array.isArray(getWebchatChat) && getWebchatChat.length > 0) {
      ioredisReport.to(`${data?.domainId}_${data?.ext}`).emit('webchatLiveDashboard', webchat);
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getWebchatChatResponse: webchat
      });
    } else {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.not_found,
        getWebchatChatResponse: []
      });
    }
  } catch (err) {
    req.log.error(err);
    console.log(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getWebchatLiveDashboradHandler - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getLiveDashBoardFeautresHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    res.status(200).send({
      statusCode: 200,
      message: RESPONSE.not_found,
      getLiveDashbordFeautresRes: [],
    });

  } catch (err) {
    req.log.error(err);
    console.log(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getLiveDashBoardFeautresHandler - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getArrivalDashbordRateHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    res.status(200).send({
      statusCode: 200,
      message: RESPONSE.not_found,
      getArrivalDashbordRes: [],
    });
  } catch (err) {
    req.log.error(err);
    console.log(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getArrivalDashbordRateHandler - Unexpected Error:', err);
  }
}

/**
 * Api for live chat dashboard stand  alone bot
 */

export async function getChatBotLiveDashboard(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      startTmpstmp: req.body.startTmpstmp,
      endTmpstmp: req.body.endTmpstmp,
      domainId: auth.domainId,
    };
    const getChatBotLiveDashboardDetails: any = await getChatBotLiveDashboardDao(data);
    const resultArr: any = {};
    const sessionArr: any = [];
    const dateTimeArr: any = [];
    const TotalSessionsArr: any = [];
    const AgentTransferArr: any = [];
    const Abandoned: any = [];
    const BotResolved: any = [];
    let totalsessionCount = 0;
    let botResolvedCount = 0;
    let agentTransferCount = 0;
    let abandonedCount = 0;
    getChatBotLiveDashboardDetails[0].map((list: any) => {
      sessionArr.push(list.SessionInterval);
      dateTimeArr.push(list.StartDateTime);
      TotalSessionsArr.push(list.TotalSessions);
      BotResolved.push(list.BotResolved);
      AgentTransferArr.push(list.AgentTransfer);
      Abandoned.push(list.Abandoned);
      totalsessionCount = totalsessionCount + list.TotalSessions;
      botResolvedCount = botResolvedCount + list.BotResolved;
      agentTransferCount = agentTransferCount + list.AgentTransfer;
      abandonedCount = abandonedCount + list.Abandoned;
    });
    resultArr.sessionData = sessionArr;
    resultArr.dataTimeData = dateTimeArr;
    resultArr.totalSessionData = TotalSessionsArr;
    resultArr.botTransferData = BotResolved;
    resultArr.agentTransferData = AgentTransferArr;
    resultArr.abandoned = Abandoned;
    const totalCountData: any = [totalsessionCount, botResolvedCount, agentTransferCount, abandonedCount];

    if (getChatBotLiveDashboardDetails.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getChatBotLiveDashboardResTotalData: totalCountData,
        getChatBotLiveDashboardResGraph: resultArr,
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getChatBotLiveDashboardRes: [] });
    }
  } catch (err) {
    console.log(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getChatBotLiveDashboardDetails - Unexpected Error:', err);
  }
}

export async function getChatBotLiveDashboardAgentPerformance(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      startTmpstmp: req.body.startTmpstmp,
      endTmpstmp: req.body.endTmpstmp,
      domainId: auth.domainId,
    };
    const getDashboardAgentPerformance: any = await getChatBotLiveAgentPerformanceDao(data);
    const resultArr: any = [
      getDashboardAgentPerformance[0][0].AverageChatDuration ?? 0,
      getDashboardAgentPerformance[0][0].ChatAbandonedRate ?? 0,
      getDashboardAgentPerformance[0][0].AverageWrapUpTime ?? 0,
      getDashboardAgentPerformance[0][0].ChatTransferredRate ?? 0
    ];

    if (getDashboardAgentPerformance.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getChatBotLiveDashboardRes: resultArr,
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getChatBotLiveDashboardRes: [] });
    }
  } catch (err) {
    console.log(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getChatBotLiveDashboardDetails - Unexpected Error:', err);
  }
}

export async function getCustomerChartRate(req: any, res: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      startTmpstmp: req.body.startTmpstmp,
      endTmpstmp: req.body.endTmpstmp,
      domainId: auth.domainId,
    };
    const dateTimeArr: any = [];
    const toatalCount: any = [];
    const getTrendChartForCustomer: any = await getTrendChartForcustomerDao(data);
    getTrendChartForCustomer[1].map((list: any) => {
      dateTimeArr.push(list.StartDateTime);
      toatalCount.push(list.TotalCalls);
    });
    const result: any = {
      dataTimeData: dateTimeArr,
      totalCustomers: toatalCount
    };
    if (getTrendChartForCustomer.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getBotCustomerChart: result,
        averageCallTime: getTrendChartForCustomer[0].AverageCallTime || 0
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getBotCustomerChart: [] });
    }
  } catch (error) {
    console.log(error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getCustomerChartRate - Unexpected Error:', error);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getSmsLiveDashboardPieChartHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const getUserRole: any = await roleInfoDao(auth);

    if (auth.roleId) {
      let ext = 0;
      if (auth.roleId === 3 || auth.roleId === 1) {
        ext = 0;
      } else {
        ext = auth.ext;
      }
      const data: any = {
        startTmpstmp: req.body.startTmpstmp,
        endTmpstmp: req.body.endTmpstmp,
        timeZone: null,
        domainId: auth.domainId,
        extNo: ext,
      };
      const getPieSms: any = await getSmsLiveDashboardPieChart(data);

      if (Array.isArray(getPieSms) && getPieSms.length > 0) {

        const pieData = {
          datasets: [
            {
              backgroundColor: ['#DE003D', '#EBC252'],
              borderColor: ['#DE003D', '#EBC252'],
              borderWidth: 1,
              data: [getPieSms[0][0]?.InBound, getPieSms[0][0]?.Outbound],
              label: '# of Votes',
            },
          ],
          Total: getPieSms[0][0].Total,
          InBound: getPieSms[0][0].InBound,
          Outbound: getPieSms[0][0].Outbound,
          ServiceLevel: getPieSms[0][0].ServiceLevel,
          UnAssigned: getPieSms[0][0].UnAssigned,
          Assigned: getPieSms[0][0].Assigned,
          Opened: getPieSms[0][0].Opened,
          Resolved: getPieSms[0][0].Resolved,
          UnResolved: getPieSms[0][0].UnResolved,
          Transferred: getPieSms[0][0].Transferred,
          OverDue: getPieSms[0][0].OverDue,
          Discarded: getPieSms[0][0].Dsicarded
        };

        res.status(200).send({
          statusCode: 200,
          message: RESPONSE.success_message,
          getChatLiveDashboardDetails: pieData,
        });
      } else {
        res.status(200).send({
          statusCode: 200,
          message: RESPONSE.not_found,
          getChatLiveDashboardDetails: [],
        });
      }
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        roleInfo: [],
      });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getSmsLiveDashboardPieChart - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getEmailLiveDashboardPieChartHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const getUserRole: any = await roleInfoDao(auth);

    if (auth.roleId) {
      const ccas_role_info = getUserRole?.roleid.toString();
      let ext = 0;
      if (auth.roleId === 3 || auth.roleId === 1) {
        ext = 0;
      } else {
        ext = auth.ext;
      }
      const data: any = {
        startTmpstmp: req.body.startTmpstmp,
        endTmpstmp: req.body.endTmpstmp,
        timeZone: null,
        domainId: auth.domainId,
        extNo: ext,
      };
      const getPieEmail: any = await getEmailLiveDashboardPieChart(data);

      if (Array.isArray(getPieEmail) && getPieEmail.length > 0) {

        const pieData: any = {
          datasets: [
            {
              backgroundColor: ['#00B885', '#3A9EE0'],
              borderColor: ['#00B885', '#3A9EE0'],
              borderWidth: 1,
              data: [getPieEmail[0][0]?.InBound, getPieEmail[0][0]?.Outbound],
              label: '# of Votes',
            },
          ],
          Total: getPieEmail[0][0].Total,
          InBound: getPieEmail[0][0].InBound,
          Outbound: getPieEmail[0][0].Outbound,
          ServiceLevel: getPieEmail[0][0].ServiceLevel,
          UnAssigned: getPieEmail[0][0].UnAssigned,
          Assigned: getPieEmail[0][0].Assigned,
          Resolved: getPieEmail[0][0].Resolved,
          UnResolved: getPieEmail[0][0].UnResolved,
          Transferred: getPieEmail[0][0].Transferred,
          OverDue: getPieEmail[0][0].OverDue,
          Discarded: getPieEmail[0][0].Dsicarded,
          Abandoned: getPieEmail[0][0].Abandoned,
          Opened: getPieEmail[0][0].Opened,
          Suspended: getPieEmail[0][0].Suspended,
        };

        res.status(200).send({
          statusCode: 200,
          message: RESPONSE.success_message,
          getChatLiveDashboardDetails: pieData,
        });
      } else {
        res.status(200).send({
          statusCode: 200,
          message: RESPONSE.not_found,
          getChatLiveDashboardDetails: [],
        });
      }
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        roleInfo: [],
      });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getEmailLiveDashboardPieChart - Unexpected Error:', err);
  }
}
export async function getDashboardPiechartAgentStatusDetails(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const requestion = req.body;
    if (auth.roleId) {
      let ext = 0;
      if (auth.roleId === 3 || auth.roleId === 1) {
        ext = 0;
      } else {
        ext = auth.ext;
      }
      const data: any = {
        startTmpstmp: requestion.startTmpstmp,
        endTmpstmp: requestion.endTmpstmp,
        timeZone: requestion.timeZone,
        domainId: auth.domainId,
        extNo: ext
      };
      const agentStatusDetails: any = await getDashboardPiechartAgentStatusDetailsDao(data);
      if (agentStatusDetails.length > 0) {
        const data1 = {
          Away: 0,
          Busy: 0,
          'Not Ready': 0,
          Ready: 0,
          Offline: 0,
          Restrict: 0,
          'Transfer Only': 0,
        };
        agentStatusDetails.forEach((x: any) => { data1[x.state] = x.agent_status; });
        const pieData = {
          data1,
          datasets: [
            {
              backgroundColor: ['#00B885', '#AAABAD', '#F22E57', '#EBC252'],
              borderColor: ['#00B885', '#AAABAD', '#F22E57', '#EBC252'],
              borderWidth: 1
            },
          ],
        };
        res.status(200).send({
          statusCode: 200,
          message: RESPONSE.success_message,
          dashboardPiechartAgentStatusDetails: pieData
        });
      } else {
        res.status(200).send({
          statusCode: 404,
          message: RESPONSE.not_found,
          dashboardPiechartAgentStatusDetails: [],
        });
      }
    } else {
      res.status(200).send({
        statusCode: 401,
        message: RESPONSE.incorrect_input,
        dashboardPiechartAgentStatusDetails: [],
      });
    }
  } catch (err) {
    console.log(err);
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getDashboardPiechartAgentStatusDetails - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 */
export async function getDashboardAgentStatusHandler(req: any, res: any) {
  try {
    const auth = req.headers;
    const agentId = Number(req?.params?.agentId);
    const data: any = {
      domainId: auth.domainId,
      ext: auth.ext,
      agentId: agentId || null
    };
    const getDashboardAgentStatusHandlerRes: any = await getDashboardAgentStatusHandlerDao(data);
    let notReady: any;
    let transferOnly: any;
    let offline: any;
    let ready: any;
    let busy: any;
    let away: any;
    let total: any;

    getDashboardAgentStatusHandlerRes[0].map((agentData: any) => {
      if (agentData.statusName === 'Not Ready') {
        notReady = agentData.noOfAgent;
        return;
      }
      if (agentData.statusName === 'Transfer Only') {
        transferOnly = agentData.noOfAgent;
        return;
      }
      if (agentData.statusName === 'Offline') {
        offline = agentData.noOfAgent;
        return;
      }
      if (agentData.statusName === 'Ready') {
        ready = agentData.noOfAgent;
        return;
      }
      if (agentData.statusName === 'Busy') {
        busy = agentData.noOfAgent;
        return;
      }
      if (agentData.statusName === 'Away') {
        away = agentData.noOfAgent;
        return;
      }
      if (agentData.statusName === 'Total') {
        total = agentData.noOfAgent;
        return;
      }
    });
    if (Array.isArray(getDashboardAgentStatusHandlerRes) && getDashboardAgentStatusHandlerRes.length > 0) {
      const output: any = getDashboardAgentStatusHandlerRes[0].map((item: any) => item.noOfAgent);
      const pieData: any = {
        datasets: [
          {
            backgroundColor: ['#d62f49', '#800080', '#667799', 'green', '#964b00', '#f7ae4c'],
            borderColor: ['#d62f49', '#800080', '#667799', 'green', '#964b00', '#f7ae4c'],
            borderWidth: 1,
            data: [notReady, transferOnly, offline, ready, busy, away],
            label: '# of Votes',
          },
        ],
        NotReady: notReady,
        TransferOnly: transferOnly,
        Offline: offline,
        Ready: ready,
        Busy: busy,
        Away: away,
        Total: total,
        AllData: getDashboardAgentStatusHandlerRes[0],
        allAgent : getDashboardAgentStatusHandlerRes[1],
      };
      ioredisReport.to(`${data?.domainId}`).emit('agentstatusDashboard', {
        datasets: pieData.datasets,
        NotReady: notReady,
        TransferOnly: transferOnly,
        Offline: offline,
        Ready: ready,
        Busy: busy,
        Away: away,
        Total: total,
        AllData: getDashboardAgentStatusHandlerRes[0],
        allAgent : getDashboardAgentStatusHandlerRes[1],
      });
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getChatLiveDashboardDetails: pieData,
      });

    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        roleInfo: [],
      });
    }

  } catch (error) {
    req.log.error(error);
    console.log(error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getDashboardPiechartAgentStatusDetails - Unexpected Error:', error);
  }
}

export async function getDashboardSocialMediaStatusHandler(req: any, res: any) {
  try {

    const data: any = {
      startTmpstmp: req?.body?.startTmpstmp,
      endTmpstmp: req?.body?.endTmpstmp,
      domainId: req?.headers?.domainId,
      timezone: null,
      ext: req?.headers?.ext,
      channelType: req?.body?.channelType,
    };

    const getDashboardSocialMediaStatusHandlerRes: any = await getDashboardSocialMediaStatusHandlerDao(data);

    if (Array.isArray(getDashboardSocialMediaStatusHandlerRes) && getDashboardSocialMediaStatusHandlerRes.length > 0) {

      const pieData: any = {
        datasets: [
          {
            backgroundColor: ['#00B885', '#DE003D'],
            borderColor: ['#FF793D', '#653AE0'],
            borderWidth: 1,
            data: [
              getDashboardSocialMediaStatusHandlerRes[0][0]?.InBound,
              getDashboardSocialMediaStatusHandlerRes[0][0]?.Outbound,
            ],
            label: '# of Votes',
          },
        ],
        Total: getDashboardSocialMediaStatusHandlerRes[0][0].Total,
        InBound: getDashboardSocialMediaStatusHandlerRes[0][0].InBound,
        Outbound: getDashboardSocialMediaStatusHandlerRes[0][0].Outbound,
        ServiceLevel: getDashboardSocialMediaStatusHandlerRes[0][0].ServiceLevel,
        QueuedCustomers: getDashboardSocialMediaStatusHandlerRes[0][0].QueuedCustomers,
        Resolved: getDashboardSocialMediaStatusHandlerRes[0][0].Resolved,
        UnResolved: getDashboardSocialMediaStatusHandlerRes[0][0].UnResolved,
        Abandoned: getDashboardSocialMediaStatusHandlerRes[0][0].Abandoned,
        Transferred: getDashboardSocialMediaStatusHandlerRes[0][0].Transferred,
        Callback: getDashboardSocialMediaStatusHandlerRes[0][0].Callback,
        Missed: getDashboardSocialMediaStatusHandlerRes[0][0].Missed,
      };

      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getChatLiveDashboardDetails: pieData,
      });
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        roleInfo: [],
      });
    }

  } catch (error) {
    req.log.error(error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getDashboardSocialMediaStatusHandler - Unexpected Error:', error);
  }
}

export async function getDashboardAverageHandler(req: any, res: any) {
  try {
    const data: any = {
      startTmpstmp: req?.body?.startTmpstmp,
      endTmpstmp: req?.body?.endTmpstmp,
      timezone: null,
      domainId: req?.headers?.domainId,
      ext: req?.headers?.ext,
      roleId: req?.headers?.roleId,
      agentId: req?.body?.agentId ?? null,
      channelType: req?.body?.channelType ?? 'All',
    };

    const getDashboardAverageHandlerRes: any = await getDashboardAverageHandlerDao(data);

    if (Array.isArray(getDashboardAverageHandlerRes) && getDashboardAverageHandlerRes.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getChatLiveDashboardDetails: getDashboardAverageHandlerRes[0][0],
      });
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        roleInfo: [],
      });
    }
  } catch (error) {
    req.log.error(error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getDashboardAverageHandlerRes - Unexpected Error:', error);
  }
}

export async function getCustomerSatisfactionHandler(req: any, res: any) {
  try {
    const data: any = {
      startTmpstmp: req?.body?.startTmpstmp,
      endTmpstmp: req?.body?.endTmpstmp,
      timezone: null,
      domainId: req?.headers?.domainId,
      ext: req?.headers?.ext,
      roleId: req?.headers?.roleId,
    };

    const getCustomerSatisfactionHandlerRes: any = await getCustomerSatisfactionHandlerDao(data);

    if (Array.isArray(getCustomerSatisfactionHandlerRes) && getCustomerSatisfactionHandlerRes.length > 0) {
      const pieData: any = {
        negativeSentimentalScore: getCustomerSatisfactionHandlerRes[0][0]?.negativeSentimentalScore,
        neutralSentimentalScore: getCustomerSatisfactionHandlerRes[0][0]?.neutralSentimentalScore,
        postiveSentimentalScore: getCustomerSatisfactionHandlerRes[0][0]?.postiveSentimentalScore,
      };
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getChatLiveDashboardDetails: pieData,
      });
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        roleInfo: [],
      });
    }
  } catch (error) {
    req.log.error(error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getDashboardPiechartAgentStatusDetails - Unexpected Error:', error);
  }
}

export async function getDashboardUtilizationHandler(req: any, res: any) {
  try {
    const data: any = {
      startTmpstmp: req?.body?.startTmpstmp,
      endTmpstmp: req?.body?.endTmpstmp,
      domainId: req?.headers?.domainId,
      ext: req?.headers?.ext,
      channeltype: req?.body?.channelType,
    };

    const getDashboardUtilizationHandlerRes: any = await getDashboardUtilizationHandlerDao(data);

    if (Array.isArray(getDashboardUtilizationHandlerRes) && getDashboardUtilizationHandlerRes.length > 0) {
      const pieData: any = {
        occupancyRate: getDashboardUtilizationHandlerRes[0][0]?.occupancyRate,
        utilizationRate: getDashboardUtilizationHandlerRes[0][0]?.utilizationRate,
        blockedContacts: getDashboardUtilizationHandlerRes[0][0]?.blockedContacts,
      };
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getChatLiveDashboardDetails: pieData,
      });
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        roleInfo: [],
      });
    }
  } catch (error) {
    req.log.error(error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getDashboardPiechartAgentStatusDetails - Unexpected Error:', error);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getDashboardPerformanceComparisonHandler(req: any, res: any) {
  try {
    const data: any = {
      startTimeStamp: req?.body?.startTmpstmp,
      endTimeStamp: req?.body?.endTmpstmp,
      domainId: req?.headers?.domainId,
      channelType: req?.body?.channelType,
    };
    const getDashboardPerformanceComparisonHandlerRes: any = await getDashboardPerformanceComparisonHandlerDao(data);
    if (Array.isArray(getDashboardPerformanceComparisonHandlerRes) && getDashboardPerformanceComparisonHandlerRes.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getChatLiveDashboardDetails: getDashboardPerformanceComparisonHandlerRes[0][0],
      });
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        roleInfo: [],
      });
    }
  } catch (error) {
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
export async function getUsedNoOfSessionHandler(req: any, res: any, done: any) {
  try {
    const auth = req?.headers;
    const data: any = {
      domainId: auth.domainId,
      channelType: req?.body?.channelType ?? 'All',
    };
    const NoOfSession: any = await getUsedNoOfSessionDao(data);
    if (NoOfSession && NoOfSession?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        result: NoOfSession,
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function getResponseRatingHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      p_startTmpstmp: req.body.startTmpstmp,
      p_endTmpstmp: req.body.endTmpstmp,
      channelType: req.body.channelType ?? 'All',
      p_domainId: auth.domainId,
    };
    const response: any = await getDashboardResponseRating(data);
    if (response && Array.isArray(response) && response?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        responseRating: response[0],
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, responseRating: [] });
    }
  } catch (err) {
    console.log(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function getAgentAssistanceHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      p_startTmpstmp: req.body.startTmpstmp ?? null,
      p_endTmpstmp: req.body.endTmpstmp ?? null,
      p_domainId: auth.domainId,
      agentAssistChannelType :req.body.agentAssistChannelType ?? null,
      agentId :req.body.agentId ?? null,
    };
    const response: any = await getAgntAssiatanceDao(data);
    if (response && Array.isArray(response) && response?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        agentAssistance: response[0],
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, agentAssistance: [] });
    }
  } catch (err) {
    console.log(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function getSurveyResponseHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      p_startTmpstmp: req.body.startTmpstmp,
      p_endTmpstmp: req.body.endTmpstmp,
      p_domainId: auth.domainId,
    };
    const response: any = await getSurveyResponseDao(data);
    if (response && Array.isArray(response) && response?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        surveyResponse: response,
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, surveyResponse: [] });
    }
  } catch (err) {
    console.log(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function getSentimentalAnalysisHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      startTmpstmp: req.body.startTmpstmp,
      endTmpstmp: req.body.endTmpstmp,
      domainId: auth.domainId,
      SentimentalAnalysischannelType :req.body.SentimentalAnalysischannelType
    };
    const response: any = await getSentimentalAnalysisDao(data);
    if (response && Array.isArray(response) && response?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        SentimentalAnalysis: response,
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, SentimentalAnalysis: [] });
    }
  } catch (err) {
    console.log(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function getComapreDashboardHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      domainId: auth.domainId,
      startTmpstmp: req.body.startTmpstmp,
      endTmpstmp: req.body.endTmpstmp,
      productId: req.body.productId
    };
    const response: any = await getComapreDashboardResponse(data);
    if (response && Array.isArray(response) && response?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        result: response[0],
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    console.log(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function getCompareDashbaordTrendChartHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      domainId: auth.domainId,
      startTmpstmp:req.body.startTmpstmp,
      endTmpstmp: req.body.endTmpstmp,
      datetype: req.body.datetype,
      columnName : req.body.columnName,
      columnNameCompare : req.body.columnNameCompare,
      productId: req.body.productId

    };
    const response: any = await getCompareDashbaordTrendChartResponse(data);
    if (response && Array.isArray(response) && response?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        result: response[0],
        compareData1:response[1],
        compareData2:response[2],
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [], compareData1:[] , compareData2:[] });
    }
  } catch (err) {
    console.log(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function getCustomerCountForYearHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      domainId: auth.domainId,
    };
    const response: any = await getCustomerCountForYearDao(data);
    if (response && Array.isArray(response) && response?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        result: response[0],
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, SentimentalAnalysis: [] });
    }
  } catch (err) {
    console.log(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function getVideoChannelDashMetrics(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      startTmpstmp: req?.body?.startTmpstmp,
      endTmpstmp: req?.body?.endTmpstmp,
      domainId: auth?.domainId,
      timezone: null,
      ext: req?.headers?.ext,
    };
    const response: any = await getVideoChannelDashMetricsDao(data);
    if (response && Array.isArray(response) && response?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        result: response[0],
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, SentimentalAnalysis: [] });
    }
  } catch (err) {
    console.log(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function getTrackerData(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      domainId: auth?.domainId,
    };
    const response: any = await getTrackerDataDao(data);
    if (response && Array.isArray(response) && response?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        result: response[0],
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, SentimentalAnalysis: [] });
    }
  } catch (err) {
    console.log(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function getOverAllSlaAi(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      domainId: auth?.domainId,
      startTmpstmp: req?.body?.startTmpstmp,
      endTmpstmp: req?.body?.endTmpstmp,
    };
    const response: any = await getOverAllSlaAiDao(data);
    if (response && Array.isArray(response) && response?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        result: response,
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, SentimentalAnalysis: [] });
    }
  } catch (err) {
    console.log(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}
