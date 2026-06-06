import { remainderCron, sessionHistoryCron } from './cronJob';
import { getScheduleReportDao } from './dao/scheduleReport.dao';
import { CronJob } from './helpers/utils';
import { scheduleReportJob } from './redisBull';
import { reportQueue } from './redisBull/queue';
import createServer from './server';

const PORT = process.env.PORT || '5002';
const server = createServer();

server.listen(+PORT, '0.0.0.0', (err, address) => {
  if (err) throw err;
  remainderCron();
  console.info(`reports server started...${address}`);
});
module.exports = server;

reportQueue.process(async (job: any) => {
  console.log('ReportQueue Process Completed!!!!!!');
  await CronJob(job.data);
  await scheduleReportJob(job.data);
});

export async function init() {
  const reportsResponse: any = await getScheduleReportDao(null);
  if (Array.isArray(reportsResponse) && reportsResponse[0]) {
    for (const report of reportsResponse) {
      await scheduleReportJob(report);
    }
  }
}

init();

sessionHistoryCron();
