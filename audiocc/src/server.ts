import { logger } from './plugin/log';

function createServer() {
  const server = require('fastify')(logger);
  server.addHook('preHandler', (request: any, reply: any, done: any) => {
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

  server.setErrorHandler((error: any, req: any, res: any) => {
    req.log.error(error.toString());
    res.send({ error });
  });
  return server;
}
export default createServer;
