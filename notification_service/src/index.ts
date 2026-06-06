import createServer from './server';
const PORT = process.env.PORT || '5004';
const server = createServer();

server.listen(+PORT, '0.0.0.0', (err, address) => {
  if (err) throw err;
  console.info(`Ccaas_call server started...${address}`);
});
module.exports = server;
