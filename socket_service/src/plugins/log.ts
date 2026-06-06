/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

import { createLogger, format, transports } from 'winston';
import 'winston-daily-rotate-file';

const { combine, timestamp, printf, errors } = format;

const dir = process.platform === 'win32' ? '/tmp' : '/var/log';

/* LOG FORMAT */
const logFormat = printf((info: any) => {
  return `[${info.timestamp}] ${info.level.toUpperCase()}: ${info.stack || info.message}`;
});

/* INFO LOG */
const infoTransport = new transports.DailyRotateFile({
  filename: `${dir}/qaaisocket_logger_service-info-%DATE%.log`,
  level: 'info',
  datePattern: 'YYYY-MM-DD',
  zippedArchive: true,
  maxSize: '20m',
  maxFiles: '30d',
});

/* WARN LOG */
const warnTransport = new transports.DailyRotateFile({
  filename: `${dir}/qaaisocket_logger_service-warn-%DATE%.log`,
  level: 'warn',
  datePattern: 'YYYY-MM-DD',
  zippedArchive: true,
  maxSize: '20m',
  maxFiles: '30d',
});

/* ERROR LOG */
const errorTransport = new transports.DailyRotateFile({
  filename: `${dir}/qaaisocket_logger_service-error-%DATE%.log`,
  level: 'error',
  datePattern: 'YYYY-MM-DD',
  zippedArchive: true,
  maxSize: '20m',
  maxFiles: '30d',
});

export const logger = createLogger({
  level: 'info',
  format: combine(
    timestamp({
      format: 'YYYY-MM-DD HH:mm:ss.SSS', // date + time + milliseconds
    }),
    errors({ stack: true }),
    logFormat
  ),
  transports: [
    infoTransport,
    warnTransport,
    errorTransport,
    new transports.Console(),
  ],
});
