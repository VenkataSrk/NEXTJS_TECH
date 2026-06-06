import { logger } from './plugins/winston';
import createServer from './server';

const PORT = process.env.SERVER_PORT || '5007';
const server = createServer();

server.listen(PORT, '0.0.0.0', (err, address) => {
  if (err) throw err;
  console.info(`call server started...${address}`);
  logger.info(`call server started...${address}`);
});

module.exports = server;
