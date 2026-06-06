import { AIBOTREPORTS } from '../../helpers/constants';
import { getAiBotPerformancesBotAgentReportHandler, getAiBotPerformancesReportHandler, getAiCallQualityReportHandler, getAiChatQualityReportHandler } from './handler';

export default function aiBotReportsHandler(server: any, options: any, next: any) {

  // TODO:get Ai call Quality Report
  server.post(
    AIBOTREPORTS.GET_AI_CALL_QUALITY_REPORT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get Ai call Quality Report',
        description: 'get Ai call Quality Report api',
        tags: ['Ai_Bot_Reports'],
        // body: CallQualityReport.body,
        // response: CallQualityReport.response,
      },
    },
    getAiCallQualityReportHandler
  );

  // TODO:get Ai chat Quality Report
  server.post(
    AIBOTREPORTS.GET_AI_CHAT_QUALITY_REPORT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get Ai call Quality Report',
        description: 'get Ai call Quality Report api',
        tags: ['Ai_Bot_Reports'],
        // body: ChatQualityReport.body,
        // response: ChatQualityReport.response,
      },
    },
    getAiChatQualityReportHandler
  );

  // TODO: get Ai Bot Performances Report
  server.post(
    AIBOTREPORTS.GET_AI_BOT_PERFORMANCES_REPORT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get Ai Bot Performances Report',
        description: 'get Ai Bot Performances Report api',
        tags: ['Ai_Bot_Reports'],
        // body: ChatQualityReport.body,
        // response: ChatQualityReport.response,
      },
    },
    getAiBotPerformancesReportHandler
  );

   // TODO: get Ai Bot Performances Bot Agent Report
  server.post(
    AIBOTREPORTS.GET_AI_BOT_PERFORMANCES_BOT_AGENT_REPORT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get Ai Bot Performances Bot Agent Report',
        description: 'get Ai Bot Performances Report  Bot Agent api',
        tags: ['Ai_Bot_Reports'],
        // body: ChatQualityReport.body,
        // response: ChatQualityReport.response,
      },
    },
    getAiBotPerformancesBotAgentReportHandler
  );

  next();
}
