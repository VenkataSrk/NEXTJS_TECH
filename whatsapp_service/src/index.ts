import { RESPONSE } from './helpers/constants';
import createServer from './server';
import { chatQueueHandler } from './services/whatsappChat_queue';

const PORT = process.env.SERVER_PORT;
const server = createServer();

server.listen(+PORT, '0.0.0.0', (err: any, address: any) => {
  if (err) {
    console.error(RESPONSE?.serverError, err);
    process.exit(1);
  }
  console.info(`${RESPONSE?.serverMessage} ${address}`);
  chatQueueHandler();
});

module.exports = server;
