
import createServer from './server';
import { bootstrapMicrosoftSubscriptions } from './service/microsoftSubscriptions';

const PORT = process.env.SERVER_PORT || '5007';
const server = createServer();
let isMsBootstrapRunning = false;

async function runMicrosoftBootstrapSafely() {
  if (isMsBootstrapRunning) {
    console.info('Skipping Microsoft subscription bootstrap: previous run still in progress');
    return;
  }
  isMsBootstrapRunning = true;
  try {
    await bootstrapMicrosoftSubscriptions();
  } catch (error: any) {
    console.error('Microsoft subscription bootstrap run failed', error?.message || error);
  } finally {
    isMsBootstrapRunning = false;
  }
}

server.listen(PORT, '0.0.0.0', async (err, address) => {
  if (err) throw err;
  console.info(`email server started...${address}`);
  await runMicrosoftBootstrapSafely();

  const intervalMinutes = Number(process.env.MS_SUBSCRIPTION_MIN_INTERVAL || 50);
  const intervalMs = intervalMinutes * 60 * 1000;
  const bootstrapInterval = setInterval(() => {
    void runMicrosoftBootstrapSafely();
  },                                    intervalMs);
  bootstrapInterval.unref?.();

});

module.exports = server;
