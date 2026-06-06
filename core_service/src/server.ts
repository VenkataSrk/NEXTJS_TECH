import axios from 'axios';
import fastify_cors from 'fastify-cors';
import fastify_oas from 'fastify-oas';
import cron from 'node-cron';
import path from 'path';
import * as app from './config/app';
import { isSelect } from './helpers/utils';
import { validateSession } from './middlewares/authendication';
import accountChannelsHandler from './modules/account_channels/routes';
import agentStatusHandler from './modules/agentStatus/routes';
import botHandler from './modules/bot/routes';
import boradCastHandler from './modules/broadcast/routes';
import callMapHandler from './modules/callMap/routes';
import ChatBotRouter from './modules/chatbot/routes';
import conversationTriggerHandler from './modules/conversation_trigger/routes';
import CountryHandler from './modules/country/routes';
import contactHandler from './modules/customerContact/routes';
import customerSatisfactionHandler from './modules/customerSatisfaction/routes';
import customerTagHandler from './modules/customerTag/routes';
import customerTypeHandler from './modules/customerType/routes';
import didPruchaseHandler from './modules/did-purchase/routes';
import fileHandler from './modules/fileUpload/routing';
import home_routes from './modules/home/routes';
import internalChatHandler from './modules/internalChat/routes';
import knowledgeBaseRouter from './modules/knowledgeBase/routes';
import knowledgeBaseHandler from './modules/knowledgebasemain/routes';
import MyPlans_routes from './modules/myPlans/routes';
// import  usersGetUserExtensionInfoController  from './modules/myPlans/routes';
import oauthClientHandler from './modules/oauth/routes';
import outboundcampaign_routes from './modules/outboundcampaign/routes';
import botSettingsHandler from './modules/questionAndAnswer/routes';
import queuehandler from './modules/queue/routes';
import routinghandler from './modules/routing/routes';
import shortcutKeyshandler from './modules/shortcutKeys/routes';
import skillhandler from './modules/skills/routes';
import smsHandler from './modules/sms/routes';
import surveyFeedbackHandler from './modules/survey_feedback/routes';
import userhandler from './modules/user/routes';
import videomeetservice from './modules/videomeet/routes';
import chatSettingsHandler from './modules/webChatSettings/routes';
import { logger } from './plugins/winston';

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
  server.register(require('@fastify/cookie'), {
    secret: 'my-secret', // for cookies signature
    parseOptions: {}, // options for parsing cookies
  });

  server.register(fastify_cors, app.CORS_OPTIONS);
  server.register(fastify_oas, app.SWAGGER_OPTS);
  server.register(require('fastify-multer').contentParser);
  server.register(require('@fastify/static'), {
    root: path.join(process.cwd(), 'files'),
    prefix: '/public/',
    index: false,
    list: true,
  });
  server
    .decorate('validateSession', validateSession)
    .register(require('fastify-auth'))
    .register(userhandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`,
    })
    .register(queuehandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`,
    })
    .register(routinghandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`,
    })
    .register(didPruchaseHandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`,
    })
    .register(oauthClientHandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`,
    })
    .register(skillhandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`,
    })
    .register(shortcutKeyshandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`,
    })
    .register(accountChannelsHandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`,
    })
    .register(CountryHandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`,
    })
    .register(ChatBotRouter, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`,
    })
    .register(fileHandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`,
    })
    .register(botHandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`,
    })
    .register(callMapHandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`,
    })
    .register(smsHandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`,
    })
    .register(agentStatusHandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`,
    })
    .register(chatSettingsHandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`,
    })
    .register(customerTagHandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`,
    })
    .register(knowledgeBaseRouter, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`,
    })
    .register(contactHandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`,
    })
    .register(botSettingsHandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`,
    })
    .register(surveyFeedbackHandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`,
    })
    .register(videomeetservice, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`,
    })
    .register(internalChatHandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`,
    })
    .register(boradCastHandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`,
    })
    .register(customerTypeHandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`,
    })
    .register(customerSatisfactionHandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`,
    })
    .register(conversationTriggerHandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`,
    })
    .register(MyPlans_routes, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`,
    })
    .register(knowledgeBaseHandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`,
    })
    .register(outboundcampaign_routes, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`,
    })
    .register(home_routes, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`,
    });
  server.setErrorHandler((error, req, res) => {
    req.log.error(error.toString());
    res.send({ error, statusCode: 400, message: 'Bad request' });
  });
  return server;
}

cron.schedule('*/2 * * * *', () => {
  axios
    .post('https://stageccaasapi.worktual.co.uk/core_service/v1/get_incoming_message_info', {
      headers: {
        'Content-Type': 'application/json',
      },
    })
    .then((response) => {
      logger.info('createServer', response);
    })
    .catch((error) => {
      logger.error('createServer', error);
    });
});

// Schedule the cron job to run every 4 hours
cron.schedule('0 */4 * * *', () => {
  console.log('Cron job started at', new Date().toLocaleString());
  isSelect();
});

export default createServer;
accountChannelsHandler;
