
import Queue from 'bull';

const redisConfig = { host: process.env.SOCKET_REDIS_INSTANCE, port: 6379, password: process.env.SOCKET_REDIS_PASSWORD };

export const reportQueue = new Queue('schedule-report-queue', { redis: redisConfig });
