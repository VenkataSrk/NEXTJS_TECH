"use strict";
/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */
Object.defineProperty(exports, "__esModule", { value: true });
exports.logger = void 0;
const winston_1 = require("winston");
require("winston-daily-rotate-file");
const { combine, timestamp, printf, errors } = winston_1.format;
const dir = process.platform === 'win32' ? '/tmp' : '/var/log';
/* LOG FORMAT */
const logFormat = printf((info) => {
    return `[${info.timestamp}] ${info.level.toUpperCase()}: ${info.stack || info.message}`;
});
/* INFO LOG */
const infoTransport = new winston_1.transports.DailyRotateFile({
    filename: `${dir}/qaaisocket_logger_service-info-%DATE%.log`,
    level: 'info',
    datePattern: 'YYYY-MM-DD',
    zippedArchive: true,
    maxSize: '20m',
    maxFiles: '30d',
});
/* WARN LOG */
const warnTransport = new winston_1.transports.DailyRotateFile({
    filename: `${dir}/qaaisocket_logger_service-warn-%DATE%.log`,
    level: 'warn',
    datePattern: 'YYYY-MM-DD',
    zippedArchive: true,
    maxSize: '20m',
    maxFiles: '30d',
});
/* ERROR LOG */
const errorTransport = new winston_1.transports.DailyRotateFile({
    filename: `${dir}/qaaisocket_logger_service-error-%DATE%.log`,
    level: 'error',
    datePattern: 'YYYY-MM-DD',
    zippedArchive: true,
    maxSize: '20m',
    maxFiles: '30d',
});
exports.logger = (0, winston_1.createLogger)({
    level: 'info',
    format: combine(timestamp({
        format: 'YYYY-MM-DD HH:mm:ss.SSS', // date + time + milliseconds
    }), errors({ stack: true }), logFormat),
    transports: [
        infoTransport,
        warnTransport,
        errorTransport,
        new winston_1.transports.Console(),
    ],
});
//# sourceMappingURL=log.js.map