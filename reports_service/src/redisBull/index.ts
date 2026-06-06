import { DateTime } from 'luxon';
import { RRule } from 'rrule';
import { v4 as uuidv4 } from 'uuid';
import { insertNotificationDao, remainderCronDao } from '../dao/remainder.dao';
import { ioredisReport } from '../plugins/db';
import { reportQueue } from '../redisBull/queue';

export async function scheduleReportJob(report: any) {
  if (report?.rrule) {
    const rule = RRule.fromString(report.rrule);
    const nowInTimeZone = DateTime.now().setZone(`${report?.timezone}`);
    const nextRun: Date | null = rule.after(nowInTimeZone.toJSDate());
    if (!nextRun) return;
    if (nextRun.getTime() <= Date.now()) {
      console.log('Skipping past occurrence:', nextRun.toISOString());
      return;
    }
    const delay = nextRun.getTime() - Date.now();
    await reportQueue.add(report, {
      delay,
      jobId: `${report?.reportName}_${nextRun.getTime()}`,
      attempts: 3,
      removeOnComplete: true,
      removeOnFail: true,
    });

  }
}

export async function remainderCronHandler() {
  try {
    const rows = await remainderCronDao();

    if (Array.isArray(rows) && rows?.length > 0) {
      const notificationData = {
        uuid: uuidv4(),
        domainId: rows[0]?.domainId ?? null,
        toExt: rows[0]?.ext ?? null,
        notificationMsg: rows[0]?.notes ?? '',
        markAsRead: 0,
        timeStamp: Date.now(),
        type: 'reminder',
        channelType: null,
        coid: null,
        customerValue: null,
      };
      ioredisReport.to(`${notificationData?.domainId}_${notificationData?.toExt}`).emit('remainderNotification', notificationData);
      await insertNotificationDao(notificationData);
    }
  }catch (error) {
    console.error('Error in remainderCronHandler:', error);
  }
}
