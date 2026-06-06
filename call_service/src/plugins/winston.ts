const { createLogger, format, transports } = require('winston');
require('winston-daily-rotate-file');

const logFormat = format.combine(
  format.timestamp({ format: 'YYYY-MM-DD HH:mm:ss' }),
  format.errors({ stack: true }), // capture error stack traces
  format.metadata({ fillExcept: ['timestamp', 'level', 'message'] }),
  format.printf((info) => {
    const meta = info?.metadata && Object.keys(info?.metadata ?? {})?.length
      ? info?.metadata ? JSON.stringify(info?.metadata) : ''
      : '';
    return `[${info.timestamp}] ${info.level.toUpperCase()}: ${info.message} ${meta}`;
  })
);
const dir = process.platform === 'win32' ? '/tmp' : '/var/log';

const dailyRotateFileTransport = new transports.DailyRotateFile({
  filename: `${dir}/call_service-%DATE%.log`,
  datePattern: 'YYYY-MM-DD',
  zippedArchive: true, // compress old logs
  maxSize: '100m',      // max file size before splitting
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
