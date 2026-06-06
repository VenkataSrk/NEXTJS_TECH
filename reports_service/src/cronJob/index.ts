import cron from 'node-cron';
import { remainderCronHandler } from '../redisBull';
// import { insertReportVoiceCount } from '../dao/reportTime.dao';

export const sessionHistoryCron = () => {
  cron.schedule('0 0 */1 * * *', async () => {
    // await insertReportVoiceCount();
  });
};

export const remainderCron = () => {
  cron.schedule('* * * * *', async () => {
    console.log('remaindercron is running ');
    await remainderCronHandler();
  });
};
