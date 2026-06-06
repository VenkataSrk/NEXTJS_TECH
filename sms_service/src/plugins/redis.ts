/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */
import { Emitter } from '@socket.io/redis-emitter';
import { config } from 'dotenv';
import Redis from 'ioredis';
import { logger } from '../utils/logger';

interface IRedisConfig {
  host: string;
  port: number;
  password?: string;
}

function createRedisClient({ host, port, password }: IRedisConfig) {
  const createClient = new Redis({
    host,
    port,
    password,
    retryStrategy(times) {
      // Exponential backoff, max 5s
      const delay = Math.min(times * 200, 5000);
      return delay;
    },
    reconnectOnError(err) {
      if (err.message.includes('READONLY')) return true;
      if (err.message.includes('ECONNRESET')) return true;
      return false;
    },
  });

  createClient.on('connect', () => {
    console.log(`✅ Redis connected [${host}:${port}]`);
    logger.info(`✅ Redis connected [${host}:${port}]`);
  });

  createClient.on('reconnecting', (delay) => {
    console.log(`🔄 Redis reconnecting in ${delay}ms [${host}:${port}]`);
    logger.info(`🔄 Redis reconnecting in ${delay}ms [${host}:${port}]`);
  });

  createClient.on('error', (err) => {
    console.error(`❌ Redis error [${host}:${port}]:`, err.message);
    logger.error(`❌ Redis error [${host}:${port}]:`, err.message);
  });

  createClient.on('end', () => {
    console.warn(`⚠️ Redis connection closed [${host}:${port}]`);
    logger.warn(`⚠️ Redis connection closed [${host}:${port}]`);
  });

  return createClient;
}

// Redis Clients
export const redisClient = createRedisClient({
  host: process.env.BLPOP_REDIS_INSTANCE,
  port: 6379,
  password: process.env.BLPOP_REDIS_PASSWORD,
});

export const pubsubRedisClient = createRedisClient({
  host: process.env.PUBSUB_REDIS_INSTANCE,
  port: 6379,
  password: process.env.PUBSUB_REDIS_PASSWORD,
});

export const otherRedisClient = createRedisClient({
  host: process.env.SOCKET_REDIS_INSTANCE,
  port: 6379,
  password: process.env.SOCKET_REDIS_PASSWORD,
});

// Duplicate connections for pub/sub
export const client = redisClient.duplicate();
export const subscriberEmitter = otherRedisClient.duplicate();

// Socket.io emitter
export const ioredisChat = new Emitter(otherRedisClient);

// Debugging Redis envs on startup
logger.debug('Redis Instances:', {
  BLPOP: process.env.BLPOP_REDIS_INSTANCE,
  PUBSUB: process.env.PUBSUB_REDIS_INSTANCE,
  SOCKET: process.env.SOCKET_REDIS_INSTANCE,
});
