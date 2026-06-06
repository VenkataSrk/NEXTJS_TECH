/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

import fs from 'fs';
import moment from 'moment';
import path from 'path';
import winston from 'winston';

const dir = process.platform === 'win32' ? '/tmp' : '/var/log';
if (!fs.existsSync(dir)) fs.mkdirSync(dir, { recursive: true });

const logFileName = path.join(dir, `${moment().format('YYYY-MM-DD')}-frontend_error.log`);

export const Errorlogger = winston.createLogger({
  level: 'info',
  format: winston.format.combine(
    winston.format.timestamp({ format: 'YYYY-MM-DD hh:mm A' }),
    winston.format.printf(({ timestamp, level, message }) => {
      return `[${timestamp}] [${level.toUpperCase()}]: ${message}`;
    })
  ),
  transports: [
    new winston.transports.File({ filename: logFileName }),
    new winston.transports.Console()
  ]
});
