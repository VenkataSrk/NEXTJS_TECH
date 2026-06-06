import { Emitter } from '@socket.io/redis-emitter';
import Redis from 'ioredis';
import { logger } from './log';

const REDIS_RECONNECT_BASE_DELAY_MS = 2000;
const REDIS_RECONNECT_MAX_DELAY_MS = 30000;

function getRedisRetryDelay(attempt: number) {
  return Math.min(REDIS_RECONNECT_BASE_DELAY_MS * Math.max(1, attempt), REDIS_RECONNECT_MAX_DELAY_MS);
}

function attachRedisEventHandlers(client: Redis, clientName: string) {
  client.on('connect', () => {
    console.log(`🔗 Redis connected: ${clientName}`);
    logger.info(`🔗 Redis connected: ${clientName}`);
  });

  client.on('ready', () => {
    console.log(`✅ Redis ready: ${clientName}`);
    logger.info(`✅ Redis ready: ${clientName}`);
  });

  client.on('reconnecting', (time: number) => {
    console.warn(`⚠️ Redis reconnecting in ${time}ms: ${clientName}`);
    logger.warn(`⚠️ Redis reconnecting in ${time}ms: ${clientName}`);
  });

  client.on('close', () => {
    console.warn(`⚠️ Redis connection closed: ${clientName}`);
    logger.warn(`⚠️ Redis connection closed: ${clientName}`);
  });

  client.on('end', () => {
    console.error(`❌ Redis connection ended: ${clientName}`);
    logger.error(`❌ Redis connection ended: ${clientName}`);
  });

  client.on('error', (err: any) => {
    console.error(`❌ Redis error (${clientName}):`, err?.message || err);
    logger.error(`❌ Redis error (${clientName})`, err);
  });
}

function createRedisClient(clientName: string, options: any) {
  const client = new Redis({
    ...options,
    maxRetriesPerRequest: null,
    retryStrategy: (attempt: number) => {
      const delay = getRedisRetryDelay(attempt);
      console.warn(`⚠️ Redis retry scheduled in ${delay}ms | ${clientName} | Attempt #${attempt}`);
      logger.warn(`⚠️ Redis retry scheduled in ${delay}ms | ${clientName} | Attempt #${attempt}`);
      return delay;
    },
    reconnectOnError: (err: any) => {
      const message = err?.message || '';
      const shouldReconnect =
        message.includes('READONLY') ||
        message.includes('ECONNRESET') ||
        message.includes('ETIMEDOUT');
      return shouldReconnect;
    }
  });

  attachRedisEventHandlers(client, clientName);
  return client;
}

export const coreRedisClient = createRedisClient('coreRedisClient', {
  host: process.env.SOCKET_REDIS_INSTANCE,
  port: 6379,
  password: process.env.SOCKET_REDIS_PASSWORD
});
export const subscriberEmitter = coreRedisClient.duplicate();
attachRedisEventHandlers(subscriberEmitter, 'subscriberEmitter');
export const ioredisWhatsapp = new Emitter(subscriberEmitter);
export const ioredisWhatsappQueue: any = coreRedisClient.duplicate();
attachRedisEventHandlers(ioredisWhatsappQueue, 'ioredisWhatsappQueue');
export const ioredisWhatsappPushPop = createRedisClient('ioredisWhatsappPushPop', {
  host: process.env.BLPOP_REDIS_INSTANCE,
  port: 6379,
  password: process.env.BLPOP_REDIS_PASSWORD
});
export const clientRpush = ioredisWhatsappPushPop.duplicate();
attachRedisEventHandlers(clientRpush, 'clientRpush');
