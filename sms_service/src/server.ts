import fastify_cors from 'fastify-cors';
import fastify_oas from 'fastify-oas';
import { logger } from '../src/utils/logger';
import * as app from './config/app';
import { validateSession } from './middlewares/authendication';
import smsHandler from './modules/sms/routes';

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
    .register(require('fastify-auth'));
  server.register(smsHandler, {
    prefix: `${app.configs.servicepath}/${app.configs.appversion}`,
  });
  server.setErrorHandler((error, req, res) => {
    req.log.error(error.toString());
    res.send({ error, statusCode: 400, message: 'Bad request' });
  });
  return server;
}

export default createServer;
