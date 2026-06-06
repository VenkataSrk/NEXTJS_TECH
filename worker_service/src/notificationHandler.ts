import { insertNotificationCallBlastDao, insertNotificationVoiceMailDao } from './dao/notification.dao';
import { loggerError } from './plugins/logger';

export const notificationHandler = async (channel: any, ioHandler: any) => {
  try {
    if (channel?.campaignStatus === 4 && channel?.domain_id) {
      await insertNotificationCallBlastDao(channel, ioHandler);
    }
    if (channel?.acd_event === 28) {
      await insertNotificationVoiceMailDao(channel, ioHandler);
    }
  } catch (error) {
    loggerError('notificationHandler', channel, error);
  }
};
