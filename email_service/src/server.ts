import fastify_cors from 'fastify-cors';
import fastify_oas from 'fastify-oas';
import * as app from './config/app';
import { validateSession } from './middlewares/authendication';
import emailHandler from './modules/email/routes';
import { logger } from './plugins/log';

function createServer() {
  const server = require('fastify')({
    logger,
    bodyLimit: 50 * 1024 * 1024,
  });
  server.addHook('preHandler', (request: any, reply: any, done: any) => {
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
    .register(emailHandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`,
    });
  server.setErrorHandler((error, req, res) => {
    req.log.error(error.toString());
    res.send({ error, statusCode: 400, message: 'Bad request' });
  });
  return server;
}

export default createServer;
