import { messageValue } from './helpers/constants';
import createServer from './server';
import { chatQueueHandler } from './services/facebookChat_queue';
import { chatQueueInstagramHandler } from './services/instagramChat_queue';

const PORT = process.env.SERVER_PORT;
const server = createServer();

server.listen(+ PORT, '0.0.0.0', (err:any, address:any) => {
  if (err) throw err;
  console.info(`${messageValue?.serverMessage} ${address}`);
  chatQueueHandler();
  chatQueueInstagramHandler();
});

module.exports = server;
