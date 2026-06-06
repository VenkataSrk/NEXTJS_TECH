import path from 'path';
import { logger } from './plugins/log';

function createServer() {
  const server = require('fastify')(logger);
  server.addHook('preHandler', (request, reply, done) => {
    done();
  });
  server.addHook('onSend', (_request, reply, payload, done) => {
    if (!payload) {
      Object.assign(reply.raw, { payload });
    }
    done();
  });
  server.register(require('@fastify/cookie'), {
    secret: 'my-secret', // for cookies signature
    parseOptions: {}, // options for parsing cookies
  });
  server.register(require('fastify-multer').contentParser);
  server.register(require('@fastify/static'), {
    root: path.join(process.cwd(), 'files'),
    prefix: '/public/',
    index: false,
    list: true,
  });
  server
    .register(require('fastify-auth'));
  server.setErrorHandler((error, req, res) => {
    req.log.error(error.toString());
    res.send({ error, statusCode: 400, message: 'Bad request' });
  });
  return server;
}
export default createServer;
