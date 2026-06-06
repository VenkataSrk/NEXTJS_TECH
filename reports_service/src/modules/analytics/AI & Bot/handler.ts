
import {
  agentAnalysisBotInteractionDao,
  agentAnalysisBotTrendChartDao,
  agentAnalyticActiveHoursDao,
  agentAnalyticBotNluPerformencesDao,
  agentAnalyticPerformanceComparisonHandlerDao,
  agentAssistAnalysisTrendChartDao,
  agentAssistBotAnalysisDao,
  agentAssistEngagementRateTrendChartDao,
  agentAssistInteractionChannelDao,
  agentAssistOverriderateInfoDao,
  agentBotInteractionChannelDao,
  botNluPerformencesMessageDao,
  getApiCallsDao,
  getChatIntentDao,
  getNluPerformencesTopIntentDistributionDao,
  insertUpdateChatIntentDao,
  knowledgbaseAveragebaseResponseDao,
  knowledgbaseInteractionChannelDao,
  knowledgebaseUntilizationChartDao,
} from '../../../dao/botAnalytics.dao';
import {
  RESPONSE
} from '../../../helpers/constants';
import { ioredisReport } from '../../../plugins/db';
import { logger } from '../../../plugins/log';

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function agentAnalysisBotInteractionHandler(req: any, res: any, done: any) {
  try {
    const auth = req?.headers;
    const data: any = {
      startTimeStamp: req?.body?.startTimeStamp,
      endTimeStamp: req?.body?.endTimeStamp,
      domainId: auth.domainId,
    };
    const AnalysisBotInteraction: any = await agentAnalysisBotInteractionDao(data);
    if (AnalysisBotInteraction && AnalysisBotInteraction?.length > 0) {
      const AIBotInteraction = AnalysisBotInteraction[0] ?? [];
      res.status(200).send({
        AIBotInteraction,
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
    logger.error('agentAnalysisBotInteraction - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function agentAnalysisBotTrendChartHandler(req: any, res: any, done: any) {
  try {
    const auth = req?.headers;
    const data: any = {
      startTimeStamp: req?.body?.startTimeStamp,
      endTimeStamp: req?.body?.endTimeStamp,
      domainId: auth.domainId,
    };
    const BotTrendChat: any = await agentAnalysisBotTrendChartDao(data);
    if (BotTrendChat && BotTrendChat?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        BotInteractionChat: BotTrendChat[0],
        SessionMetricsValue: BotTrendChat[1]
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('agentAnalysisBotTrendChart - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function agentBotInteractionChannelHandler(req: any, res: any, done: any) {
  try {
    const auth = req?.headers;
    const data: any = {
      startTimeStamp: req?.body?.startTimeStamp,
      endTimeStamp: req?.body?.endTimeStamp,
      domainId: auth.domainId,
    };
    const AIBotInteraction: any = await agentBotInteractionChannelDao(data);
    if (AIBotInteraction && AIBotInteraction?.length > 0) {
      const BotInteractionChannel = AIBotInteraction[0] ?? [];
      res.status(200).send({
        BotInteractionChannel,
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
    logger.error('agentBotInteractionChannelHandl - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getAgentAnalyticPerformanceComparisonHandler(req: any, res: any) {
  try {
    const data: any = {
      startTimeStamp: req?.body?.startTimeStamp,
      endTimeStamp: req?.body?.endTimeStamp,
      domainId: req?.headers?.domainId,
      channelType: null,
    };
    const getAgentAnalyticPerformanceComparisonRes: any = await agentAnalyticPerformanceComparisonHandlerDao(data);
    if (Array.isArray(getAgentAnalyticPerformanceComparisonRes) && getAgentAnalyticPerformanceComparisonRes.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getAgentAnalyticPerformance: getAgentAnalyticPerformanceComparisonRes[0][0],
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
    logger.error('getAgentAnalyticPerformanceComparison - Unexpected Error:', error);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getAgentAnalyticActiveHoursHandler(req: any, res: any) {
  try {
    const data: any = {
      startTimeStamp: req?.body?.startTimeStamp,
      endTimeStamp: req?.body?.endTimeStamp,
      domainId: req?.headers?.domainId,
    };
    const AgentAnalyticActiveHoursRes: any = await agentAnalyticActiveHoursDao(data);
    if (Array.isArray(AgentAnalyticActiveHoursRes) && AgentAnalyticActiveHoursRes.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        AgentAnalyticActiveHours: AgentAnalyticActiveHoursRes[0],
        agentEngagementRate: AgentAnalyticActiveHoursRes[1],
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
    logger.error('getAgentAnalyticActiveHours - Unexpected Error:', error);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function agentAssistBotAnalysisHandler(req: any, res: any, done: any) {
  try {
    const auth = req?.headers;
    const data: any = {
      startTimeStamp: req?.body?.startTimeStamp,
      endTimeStamp: req?.body?.endTimeStamp,
      domainId: auth.domainId,
    };
    const AgentAssistBotAnalysis: any = await agentAssistBotAnalysisDao(data);
    if (Array.isArray(AgentAssistBotAnalysis) && AgentAssistBotAnalysis.length > 0) {
      const AgentAssistAnalysis = AgentAssistBotAnalysis[0] ?? [];
      const AgentAssistAnalysisValue = AgentAssistBotAnalysis[1] ?? [];
      res.status(200).send({
        AgentAssistAnalysis,
        AgentAssistAnalysisValue,
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
    logger.error('agentAssistBotAnalysis - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function agentAssistAnalysisTrendChartHandler(req: any, res: any, done: any) {
  try {
    const auth = req?.headers;
    const data: any = {
      startTimeStamp: req?.body?.startTimeStamp,
      endTimeStamp: req?.body?.endTimeStamp,
      domainId: auth.domainId,
    };
    const AverageSpeedResponse: any = await agentAssistAnalysisTrendChartDao(data);
    if (AverageSpeedResponse && AverageSpeedResponse?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        SpeedOfResponse: AverageSpeedResponse[0],
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('AgentAssistAnalysisTrendChart - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function agentAssistInteractionChannelHandler(req: any, res: any, done: any) {
  try {
    const auth = req?.headers;
    const data: any = {
      startTimeStamp: req?.body?.startTimeStamp,
      endTimeStamp: req?.body?.endTimeStamp,
      domainId: auth.domainId,
    };
    const AgentAssistInteraction: any = await agentAssistInteractionChannelDao(data);
    if (AgentAssistInteraction && AgentAssistInteraction?.length > 0) {
      const AssistInteractionChannel = AgentAssistInteraction[0] ?? [];
      res.status(200).send({
        AssistInteractionChannel,
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
    logger.error('agentAssistInteractionChannel - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function agentAssistEngagementRateTrendChartHandler(req: any, res: any, done: any) {
  try {
    const auth = req?.headers;
    const data: any = {
      startTimeStamp: req?.body?.startTimeStamp,
      endTimeStamp: req?.body?.endTimeStamp,
      domainId: auth.domainId,
    };
    const AgentAssistEngagement: any = await agentAssistEngagementRateTrendChartDao(data);
    if (AgentAssistEngagement && AgentAssistEngagement?.length > 0) {
      const AgentAssistEngagementRate = AgentAssistEngagement[0] ?? [];
      res.status(200).send({
        AgentAssistEngagementRate,
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
    logger.error('agentAssistEngagementRateTrendChart - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function agentAssistOverriderateHandler(req: any, res: any, done: any) {
  try {
    const auth = req?.headers;
    const data: any = {
      startTimeStamp: req?.body?.startTimeStamp,
      endTimeStamp: req?.body?.endTimeStamp,
      domainId: auth.domainId,
    };
    const agentAssistOverriderate: any = await agentAssistOverriderateInfoDao(data);
    if (agentAssistOverriderate && agentAssistOverriderate?.length > 0) {
      const agentAssistOverrideRateInfo = agentAssistOverriderate[0] ?? [];
      res.status(200).send({
        agentAssistOverrideRateInfo,
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
    logger.error('agentAssistOverriderate - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function knowledgbaseInteractionChannelHandler(req: any, res: any, done: any) {
  try {
    const auth = req?.headers;
    const data: any = {
      startTimeStamp: req?.body?.startTimeStamp,
      endTimeStamp: req?.body?.endTimeStamp,
      domainId: auth.domainId,
    };
    const knowledgbaseInteraction: any = await knowledgbaseInteractionChannelDao(data);
    if (knowledgbaseInteraction && knowledgbaseInteraction?.length > 0) {
      const knowledgbaseInteractionChannel = knowledgbaseInteraction[0] ?? [];
      res.status(200).send({
        knowledgbaseInteractionChannel,
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
    logger.error('knowledgbaseInteractionChannel - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function knowledgbaseAveragebaseResponseperdayHandler(req: any, res: any, done: any) {
  try {
    const auth = req?.headers;
    const data: any = {
      startTimeStamp: req?.body?.startTimeStamp,
      endTimeStamp: req?.body?.endTimeStamp,
      domainId: auth.domainId,
    };
    const knowledgbaseAveragebase: any = await knowledgbaseAveragebaseResponseDao(data);
    if (knowledgbaseAveragebase && knowledgbaseAveragebase?.length > 0) {
      const knowledgbaseAveragebaseResponse = knowledgbaseAveragebase[0] ?? [];
      res.status(200).send({
        knowledgbaseAveragebaseResponse,
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
    logger.error('knowledgbaseAveragebaseResponse - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function knowledgebaseUntilizationTrendChartHandler(req: any, res: any, done: any) {
  try {
    const auth = req?.headers;
    const data: any = {
      startTimeStamp: req?.body?.startTimeStamp,
      endTimeStamp: req?.body?.endTimeStamp,
      domainId: auth.domainId,
    };
    const knowledgebaseUntilization: any = await knowledgebaseUntilizationChartDao(data);
    if (knowledgebaseUntilization && knowledgebaseUntilization?.length > 0) {
      const knowledgebaseUntilizationTrendChart = knowledgebaseUntilization[0] ?? [];
      res.status(200).send({
        knowledgebaseUntilizationTrendChart,
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
    logger.error('knowledgebaseUntilizationChart - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function agentAnalyticBotNluPerformencesHandler(req: any, res: any, done: any) {
  try {
    const auth = req?.headers;
    const data: any = {
      startTimeStamp: req?.body?.startTimeStamp,
      endTimeStamp: req?.body?.endTimeStamp,
      domainId: auth.domainId,
    };
    const agentAnalyticBotNluPerformences: any = await agentAnalyticBotNluPerformencesDao(data);
    if (agentAnalyticBotNluPerformences && agentAnalyticBotNluPerformences?.length > 0) {
      const agentAnalyticBotNluPerformencesInfo = agentAnalyticBotNluPerformences[0] ?? [];
      res.status(200).send({
        agentAnalyticBotNluPerformencesInfo,
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
    logger.error('agentAnalyticBotNluPerformences - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function botNluPerformencesMessageHandler(req: any, res: any, done: any) {
  try {
    const auth = req?.headers;
    const data: any = {
      startTimeStamp: req?.body?.startTimeStamp,
      endTimeStamp: req?.body?.endTimeStamp,
      domainId: auth.domainId,
    };
    const botNluPerformencesMessage: any = await botNluPerformencesMessageDao(data);
    if (botNluPerformencesMessage && botNluPerformencesMessage?.length > 0) {
      const botNluPerformencesMessageInfo = botNluPerformencesMessage[0] ?? [];
      res.status(200).send({
        botNluPerformencesMessageInfo,
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
    logger.error('botNluPerformencesMessage - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function insertUpdateChatIntentHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      IntentId: req?.body?.IntentId ?? null,
      IntentName: req?.body?.IntentName ?? null,
      IntentCount: req?.body?.IntentCount ?? null,
      domainId: auth?.domainId,
      ChatFlowId: req?.body?.ChatFlowId ?? null,
      ChatHistoryId: req?.body?.ChatHistoryId ?? null,
    };
    const insertUpdateChatIntent: any = await insertUpdateChatIntentDao(data);
    if (insertUpdateChatIntent && insertUpdateChatIntent?.length > 0) {
      res.status(200).send({ statusCode: 200, message: insertUpdateChatIntent[0] });
    } else {
      res.status(200).send({ statusCode: 200, message: insertUpdateChatIntent[0] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('insertUpdateChatIntent - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getChatIntentHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      domainId: auth.domainId
    };
    const getChatIntent: any = await getChatIntentDao(data);
    if (getChatIntent && getChatIntent?.length > 0) {
      res.status(200).send({ statusCode: 200, message: getChatIntent[0] });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getChatIntent - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getNluPerformencesTopIntentDistributionHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      domainId: auth?.domainId,
    };
    const getNluPerformencesTopIntent: any = await getNluPerformencesTopIntentDistributionDao(data);
    if (getNluPerformencesTopIntent && getNluPerformencesTopIntent?.length > 0) {
      res.status(200).send({ statusCode: 200, message: getNluPerformencesTopIntent });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getNluPerformencesTopIntentDistribution - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getApiCallsHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {};
    data.domainId = auth.domainId;
    const getApiCalls: any = await getApiCallsDao(data);
    if (Array.isArray(getApiCalls) && getApiCalls.length > 0) {
      const responseInfo = [...getApiCalls[0], ...getApiCalls[1]];
      res.status(200).send({
        statusCode: 200, message: RESPONSE.success_message, getApiCallsInfo: responseInfo,
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getApiCallsInfo: [] });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getApiCallsHandler - Unexpected Error:', err);
  }
}
