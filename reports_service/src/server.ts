import axios from 'axios';
import fastify_cors from 'fastify-cors';
import fastify_oas from 'fastify-oas';
import cron from 'node-cron';
import * as app from './config/app';
import { validateSession } from './middlewares/authendication';
import aiBotReportsHandler from './modules/AI&BotReports/routes';
import aiBotAnalyticsHandler from './modules/analytics/AI & Bot/routes';
import emailAnalyticsHandler from './modules/analytics/email/routes';
import facebookAnalyticsHandler from './modules/analytics/facebook/routes';
import instagramAnalyticsHandler from './modules/analytics/instagram/routes';
import smsAnalyticsHandler from './modules/analytics/sms/routes';
import analyticsHandler from './modules/analytics/voice/routes';
import chatAnalyticsHandler from './modules/analytics/webChat/routing';
import WhatsappAnalyticsHandler from './modules/analytics/whatsapp/routes';
import compareDashboardHandler from './modules/Dashboard/compareDashboard/routes';
import leaderboardRoute from './modules/Dashboard/leaderboard/routes';
import liveDashboardHandler from './modules/Dashboard/liveDashboard/routes';
import outboundcampaignDashboardinfoHandler from './modules/Dashboard/outboundcampaign dashboard/routes';
import matrixHandler from './modules/Metrix/routes';
import outboundcampaignReportsHandler from './modules/queueAnalytics/outboundCampaign/routes';
import voiceQueueAnalyticsHandler from './modules/queueAnalytics/voice/routes';
import webchatQueueAnalyticsHandler from './modules/queueAnalytics/webchat/routes';
import whatsappQueueAnalyticsHandler from './modules/queueAnalytics/whatsapp/routes';
import agentReportHandler from './modules/report/agentReport/routes';
import emailHandler from './modules/report/email/routes';
import smsHandler from './modules/report/sms/routes';
import socialMediaHandler from './modules/report/socialMedia/routes';
import standAloneReportHandler from './modules/report/standalone/routes';
import reportHandler from './modules/report/voice/routes';
import reportChatHandler from './modules/report/webChat/routes';
import scheduleReportsHandler from './modules/scheduleReport/routes';
import voiceCallReportHandler from './modules/voiceCallReport/routes';
import { logger } from './plugins/log';

function createServer() {
  const server = require('fastify')(logger);
  server.addHook('preHandler', (request, reply, done) => {
    logger.info(`${'On preHandler'}${JSON.stringify(request.body)}`);
    done();
  });
  server.addHook('onSend', (_request, reply, payload, done) => {
    if (!payload) {
      Object.assign(reply.raw, { payload });
    }
    logger.info(`${'On send'}${payload}`);
    done();
  });
  server.register(fastify_cors, app.CORS_OPTIONS);
  server.register(fastify_oas, app.SWAGGER_OPTS);
  server.register(require('fastify-multer').contentParser);
  server
    .decorate('validateSession', validateSession)
    .register(require('fastify-auth'))
    .register(smsHandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`
    })
    .register(socialMediaHandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`
    })
    .register(emailHandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`
    })
    .register(matrixHandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`
    })
    .register(voiceCallReportHandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`
    })
    .register(reportHandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`
    })
    .register(analyticsHandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`
    })
    .register(reportChatHandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`
    })
    .register(liveDashboardHandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`
    })
    .register(compareDashboardHandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`
    })
    .register(chatAnalyticsHandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`
    })
    .register(emailAnalyticsHandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`
    })
    .register(smsAnalyticsHandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`
    })
    .register(aiBotAnalyticsHandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`
    })
    .register(WhatsappAnalyticsHandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`
    })
    .register(facebookAnalyticsHandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`
    })
    .register(instagramAnalyticsHandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`
    })
    .register(leaderboardRoute, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`
    })
    .register(agentReportHandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`
    })
    .register(outboundcampaignDashboardinfoHandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`
    })
    .register(outboundcampaignReportsHandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`
    })
    .register(standAloneReportHandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`
    })
    .register(scheduleReportsHandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`
    })
    .register(aiBotReportsHandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`
    })
    .register(voiceQueueAnalyticsHandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`
    })
    .register(webchatQueueAnalyticsHandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`
    })
    .register(whatsappQueueAnalyticsHandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`
    });
  server.setErrorHandler((error, req, res) => {
    req.log.error(error.toString());
    res.send({ error, statusCode: 400, message: 'Bad request' });
  });
  return server;
}

cron.schedule('0 0 */1 * * * ', () => {
  axios.post('', {
    headers: {
      'Content-Type': 'application/json'
    }
  }).then((response) => {
    // console.log(response);
  }).catch((response) => {
    // console.log(response);
  });
});

export default createServer;
