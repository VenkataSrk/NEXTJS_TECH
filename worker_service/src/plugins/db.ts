/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

import { Emitter } from '@socket.io/redis-emitter';
import { config } from 'dotenv';
import { Redis } from 'ioredis';
import mysql from 'mysql';
import { logger } from './log';

const env = process.env.NODE_ENV || 'qa';
console.log(`✅ Loaded environment: ${env}`);
config({ path: `.env.${env}` });

/* ------------------ REDIS CONNECTION FACTORY ------------------ */
function createRedisClient({ host, port, password }) {
  const client = new Redis({
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

  client.on('connect', () => {
    console.log(`✅ Redis connected [${host}:${port}]`);
    logger.info(`✅ Redis connected [${host}:${port}]`);
  });

  client.on('reconnecting', (delay) => {
    console.log(`🔄 Redis reconnecting in ${delay}ms [${host}:${port}]`);
    logger.info(`🔄 Redis reconnecting in ${delay}ms [${host}:${port}]`);
  });

  client.on('error', (err) => {
    console.error(`❌ Redis error [${host}:${port}]:`, err.message);
    logger.error(`❌ Redis error [${host}:${port}]:`, err.message);
  });

  client.on('end', () => {
    console.warn(`⚠️ Redis connection closed [${host}:${port}]`);
    logger.warn(`⚠️ Redis connection closed [${host}:${port}]`);
  });

  return client;
}

/* ------------------ REDIS CLIENTS ------------------ */
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

export const subscriberEmitter = otherRedisClient.duplicate();
export const ioredisChat = new Emitter(subscriberEmitter);
export const subscriberAuth = redisClient.duplicate();

/* ------------------ MYSQL RECONNECTION HANDLER ------------------ */
/**
 *
 * @param configuration
 * @returns
 */
export function createMySQLPool(configuration: any) {
  console.log('🔧 Initializing MySQL pool...');
  logger.info('🔧 Initializing MySQL pool...');

  const pool = mysql.createPool({
    ...configuration,
    waitForConnections: true,
    connectionLimit: configuration?.connectionLimit || 10,
    queueLimit: 0
  });

  /* =========================
     POOL-LEVEL ERROR HANDLER
     ========================= */
  pool.on('error', (err: any) => {
    console.error('❌ MySQL POOL error:', err.code, err.message);
    logger.error('❌ MySQL POOL error', err);

    if (err.code === 'PROTOCOL_CONNECTION_LOST' || err.code === 'ECONNRESET') {
      console.warn('⚠️ Pool detected lost connections');
      logger.warn('⚠️ Pool detected lost connections');
    }
  });

  /* =========================
     CONNECTION EVENTS
     ========================= */
  pool.on('connection', (connection: any) => {
    console.log('✅ MySQL connection established | Thread ID:', connection.threadId);
    logger.info('✅ MySQL connection established | Thread ID:', connection.threadId);

    connection.on('error', (err: any) => {
      console.error(
        '❌ MySQL connection error | Thread:',
        connection.threadId,
        err.code,
        err.message
      );
      logger.error('❌ MySQL connection error', err);
    });
  });

  pool.on('acquire', (connection: any) => {
    console.log('➡️ MySQL connection acquired | Thread:', connection.threadId);
    logger.info('➡️ MySQL connection acquired | Thread:', connection.threadId);
  });

  pool.on('release', (connection: any) => {
    console.log('⬅️ MySQL connection released | Thread:', connection.threadId);
    logger.info('⬅️ MySQL connection released | Thread:', connection.threadId);
  });

  /* =========================
     EXPLICIT CONNECTIVITY CHECK
     ========================= */
  pool.getConnection((err: any, connection: any) => {
    if (err) {
      console.error('❌ MySQL POOL connection FAILED');
      console.error('❌ Reason:', err.code, err.message);
      logger.error('❌ MySQL POOL connection FAILED', err);
      return;
    }

    console.log('✅ MySQL POOL is CONNECTED and READY');
    logger.info('✅ MySQL POOL is CONNECTED and READY');
    connection.release();
  });

  return pool;
}

export const mysqlPoolConnection = createMySQLPool({
  host: process.env.MYSQL_HOST,
  user: process.env.MYSQL_USER,
  password: process.env.MYSQL_PASSWORD,
  database: process.env.MYSQL_DB,
  port: process.env.MYSQL_PORT,
  insecureAuth: true,
  options: {
    encrypt: false,
  },
});
