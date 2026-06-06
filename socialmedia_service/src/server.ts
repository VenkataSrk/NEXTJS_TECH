import fastify_cors from 'fastify-cors';
import fastify_oas from 'fastify-oas';
import path from 'path';
import * as app from './config/app';
import { RESPONSE } from './helpers/constants';
import { validateSession } from './middlewares/authendication';
import facebookHandler from './modules/facebook/routes';
import instagremHandler from './modules/instagram/routes';
import { logger } from './plugins/log';

function createServer() {
  const server = require('fastify')(logger);
  server.addHook('preHandler', (request:any, reply:any, done:any) => {
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
    secret: 'my-secret',
    parseOptions: {},
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
    .register(require('fastify-auth'));
  server.setErrorHandler((error, req, res) => {
    req.log.error(error.toString());
    res.send({ error, statusCode: 400, message: RESPONSE?.bad_request });
  })
    .register(facebookHandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`,
    })
    .register(instagremHandler, {
      prefix: `${app.configs.servicepath}/${app.configs.appversion}/`,
    });
  return server;
}

export default createServer;
