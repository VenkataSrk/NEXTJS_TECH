const { createLogger, format, transports } = require('winston');
require('winston-daily-rotate-file');

const logFormat = format.combine(
    format.timestamp({ format: 'YYYY-MM-DD HH:mm:ss' }),
    format.printf(info => `[${info.timestamp}] ${info.level.toUpperCase()}: ${info.message}`)
);
const dir = process.platform === 'win32' ? '/tmp' : '/var/log';

const dailyRotateFileTransport = new transports.DailyRotateFile({
  filename: `${dir}/core_service-%DATE%.log`,
  datePattern: 'YYYY-MM-DD',
  zippedArchive: true, // compress old logs
  maxSize: '20m',      // max file size before splitting
  maxFiles: '14d',     // keep logs for 14 days
});

export const logger = createLogger({
  level: 'info',
  format: logFormat,
  transports: [
    dailyRotateFileTransport,
        // new transports.Console(), // also print in console
  ],
});
