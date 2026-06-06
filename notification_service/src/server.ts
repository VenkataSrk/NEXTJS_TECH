import fastify_cors from 'fastify-cors';
import fastify_oas from 'fastify-oas';
import * as app from './config/app';
import { logger } from './plugin/log';
require('./plugin/db');
function createServer() {
  const server = require('fastify')(logger);
  server.addHook('preHandler', (request, reply, done) => {
    request.log.info(`On preHandler ${JSON.stringify(request.body)}`);
    done();
  });
  server.addHook('onSend', (_request, reply, payload, done) => {
    if (!payload) {
      Object.assign(reply.raw, { payload });
    }
    reply.log.info(`On send ${payload}`);
    done();
  });
  server.register(fastify_cors, app.CORS_OPTIONS);
  server.register(fastify_oas, app.SWAGGER_OPTS);
  server.register(require('fastify-multer').contentParser);

  server.setErrorHandler((error, req, res) => {
    req.log.error(error.toString());
    res.send({ error });
  });
  return server;
}

export default createServer;
