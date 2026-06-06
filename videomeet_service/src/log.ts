import moment from 'moment';
import { configure, getLogger } from "log4js";

export const logger = getLogger();
logger.level = "info";
const dir = process.platform === 'win32' ? '/tmp' : '/var/log';

/* configure({
  appenders: {
    log: {
      type: "file",
      filename: dir + `/${moment().format('DDMMYYYY')}-socketio.log`,
      maxLogSize: 52428800,
      backups: 50
    }
  },
  categories: { default: { appenders: ["log"], level: "info" } }
}); */

configure({
  appenders: {
    everything: { type: "dateFile", filename: dir + `/videomeet-socketio.log` },
  },
  categories: {
    default: { appenders: ["everything"], level: "info" },
  },
});