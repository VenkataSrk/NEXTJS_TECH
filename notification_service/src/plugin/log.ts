import { configure, getLogger } from 'log4js';
import moment from 'moment';

export const logger = getLogger();
logger.level = 'info';
const dir = process.platform === 'win32' ? '/tmp' : '/var/log';

configure({
  appenders: {
    log: {
      type: 'file',
      filename: `${dir}/${moment().format('DDMMYYYY')}-ccaas_call_notification.log`,
      maxLogSize: 10485760,
      backups: 15
    }
  },
  categories: { default: { appenders: ['log'], level: 'info' } }
});
