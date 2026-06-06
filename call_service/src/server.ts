import fastify_cors from 'fastify-cors';
import fastify_oas from 'fastify-oas';
import * as app from './config/app';
import { validateSession } from './middlewares/authendication';
import callNotificationHandler from './modules/call/routes';
import callBlastHandler from './modules/callBlast/routes';
import callConfighandler from './modules/callConfig/routes';
import callFlowhandler from './modules/callFlow/routes';
import queueWaitTimeHandler from './modules/callWait/routes';
import chatHandler from './modules/chat/routes';
import dispositionStatusHandler from './modules/dispositionStatus/routes';
import notificationhandler from './modules/notification/routes';
import prompthandler from './modules/prompt/routes';
import stateHandler from './modules/stateTimer/routes';
import voiceBotHandler from './modules/voicebot/routes';
import CustomerWebChat from './modules/webchat/routes';
import { logger } from './plugins/winston';

require('./helpers/cron');

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
    .register(callFlowhandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`,
    })
    .register(callConfighandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`,
    })
    .register(chatHandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`,
    })
    .register(prompthandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`,
    })
    .register(CustomerWebChat, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`,
    })
    .register(queueWaitTimeHandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`,
    })
    .register(stateHandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`,
    })
    .register(dispositionStatusHandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`,
    })
    .register(notificationhandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`,
    })
    .register(callBlastHandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`,
    })
    .register(voiceBotHandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`,
    })
    .register(callNotificationHandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`,
    });
  server.setErrorHandler((error, req, res) => {
    req.log.error(error.toString());
    res.send({ error, statusCode: 400, message: 'Bad request' });
  });
  return server;
}

export default createServer;
