import dotenv from 'dotenv';
import { RedisAdapters } from './redisAddapter';
import createServer from './server';

const PORT = process.env.SERVER_PORT || '5000';
const server = createServer();

server.listen(+PORT, '0.0.0.0', (err, address) => {
  if (err) throw err;
  console.info(`Worker server started...${address}`);
});

const redisApp = new RedisAdapters(dotenv);
export { redisApp };
