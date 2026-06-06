import { logger } from './plugin/log';
import createServer from './server';
require('./plugin/db');

const PORT = process.env.SERVER_PORT || '5002';
const server = createServer();

server.listen(+PORT, '0.0.0.0', (err: any, address: any) => {
  if (err) throw err;
  console.info(`CCAAS audio cc server started...${address}`);
  logger.info(`CCAAS audio cc server started...${address}`);
});
module.exports = server;
