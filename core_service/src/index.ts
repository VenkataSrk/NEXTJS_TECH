import createServer from './server';

const PORT = process.env.SERVER_PORT || '3002';
const server = createServer();

server.listen(+ PORT, '0.0.0.0', (err, address) => {
  if (err) throw err;
  console.info(`core server started...${address}`);
});

module.exports = server;
