import dotenv from 'dotenv';
import createServer from './server';

// Load correct env file based on NODE_ENV
dotenv.config({
  path: process.env.NODE_ENV === 'production' ? '.env.production' : '.env.qa',
});

const PORT = process.env.SERVER_PORT || 5008;
const server = createServer();

server.listen(PORT, '0.0.0.0', (err: any, address: string) => {
  if (err) throw err;
  console.info(
    `${process.env.APPLICATION_NAME} server started on ${address} [${process.env.NODE_ENV}]`
  );
});

module.exports = server;
