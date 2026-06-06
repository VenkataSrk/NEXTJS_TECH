/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

import { Emitter } from '@socket.io/redis-emitter';
import dotenv from 'dotenv';
import Redis, { RedisOptions } from 'ioredis';
import mysql from 'mysql';
import { configs } from '../config/app';
import { logger } from './log';

const env = process.env.NODE_ENV || 'qa';
console.log(`✅ Loaded environment: ${env}`);
dotenv.config({ path: `.env.${env}` });

/* =========================
   REDIS WITH AUTO-RETRY
   ========================= */
const REDIS_MAX_RETRIES = Number(process.env.REDIS_MAX_RETRIES || 50);
const REDIS_RETRY_BASE_MS = Number(process.env.REDIS_RETRY_BASE_MS || 1000);
const REDIS_RETRY_MAX_MS = Number(process.env.REDIS_RETRY_MAX_MS || 30000);

function createRedisClient(name: string, options: RedisOptions): Redis {
  const client = new Redis({
    ...options,
    retryStrategy(times: number) {
      if (times > REDIS_MAX_RETRIES) {
        logger.error(`Redis [${name}] max retries (${REDIS_MAX_RETRIES}) exceeded, giving up`);
        return null;
      }
      const delay = Math.min(REDIS_RETRY_BASE_MS * Math.pow(2, Math.min(times - 1, 10)), REDIS_RETRY_MAX_MS);
      logger.warn(`Redis [${name}] reconnecting in ${delay}ms (attempt ${times}/${REDIS_MAX_RETRIES})`);
      return delay;
    },
    maxRetriesPerRequest: null,
    enableOfflineQueue: true,
    lazyConnect: false,
  });

  client.on('connect', () => {
    logger.info(`Redis [${name}] connected to ${options.host}:${options.port}`);
  });

  client.on('ready', () => {
    logger.info(`Redis [${name}] ready`);
  });

  client.on('error', (err: any) => {
    logger.error(`Redis [${name}] error: ${err.message}`);
  });

  client.on('close', () => {
    logger.warn(`Redis [${name}] connection closed, will retry automatically`);
  });

  client.on('reconnecting', (delay: number) => {
    logger.info(`Redis [${name}] reconnecting in ${delay}ms`);
  });

  return client;
}

export const redisClient = createRedisClient('blpop', { host: process.env.BLPOP_REDIS_INSTANCE, port: Number(process.env.BLPOP_REDIS_PORT), password: process.env.BLPOP_REDIS_PASSWORD });
export const pubsubRedisClient = createRedisClient('pubsub', { host: process.env.PUBSUB_REDIS_INSTANCE, port: Number(process.env.BLPOP_REDIS_PORT), password: process.env.PUBSUB_REDIS_PASSWORD });
export const otherRedisClient = createRedisClient('socket', { host: process.env.SOCKET_REDIS_INSTANCE, port: Number(process.env.BLPOP_REDIS_PORT), password: process.env.SOCKET_REDIS_PASSWORD });

export const subscriber = redisClient.duplicate();
export const assistBlpop = redisClient.duplicate();
export const sentiment = redisClient.duplicate();
export const client2 = redisClient.duplicate();
export const subscriberEmitter = otherRedisClient.duplicate();
export const ioredisChat = new Emitter(subscriberEmitter);

/* =========================
   MYSQL WITH AUTO-RETRY
   ========================= */
const MYSQL_HEALTH_CHECK_INTERVAL = Number(process.env.MYSQL_HEALTH_CHECK_MS || 30000);
const MYSQL_RETRY_DELAY_MS = Number(process.env.MYSQL_RETRY_DELAY_MS || 5000);
const MYSQL_MAX_RETRIES = Number(process.env.MYSQL_MAX_RETRIES || 20);

export function createMySQLPool(configuration: any, poolName?: string) {
  const name = poolName || configuration?.database || 'unknown';
  console.log(`🔧 Initializing MySQL pool [${name}]...`);
  logger.info(`🔧 Initializing MySQL pool [${name}]...`);

  const pool = mysql.createPool({
    ...configuration,
    waitForConnections: true,
    connectionLimit: configuration?.connectionLimit || 10,
    queueLimit: 0,
    acquireTimeout: 30000,
  });

  /* =========================
     POOL-LEVEL ERROR HANDLER
     ========================= */
  pool.on('error', (err: any) => {
    console.error(`❌ MySQL [${name}] POOL error:`, err.code, err.message);
    logger.error(`❌ MySQL [${name}] POOL error`, err);
  });

  /* =========================
     CONNECTION EVENTS
     ========================= */
  pool.on('connection', (connection: any) => {
    logger.info(`✅ MySQL [${name}] connection established | Thread ID: ${connection.threadId}`);

    connection.on('error', (err: any) => {
      logger.error(`❌ MySQL [${name}] connection error | Thread: ${connection.threadId}`, err.code, err.message);
    });
  });

  /* =========================
     HEALTH CHECK WITH AUTO-RETRY
     Periodically pings the DB; if down, retries until reconnected
     ========================= */
  let consecutiveFailures = 0;

  function healthCheck() {
    pool.query('SELECT 1', (err: any) => {
      if (err) {
        consecutiveFailures += 1;
        logger.error(`❌ MySQL [${name}] health check failed (${consecutiveFailures}): ${err.code} ${err.message}`);

        if (consecutiveFailures <= MYSQL_MAX_RETRIES) {
          const retryDelay = Math.min(MYSQL_RETRY_DELAY_MS * Math.pow(2, Math.min(consecutiveFailures - 1, 5)), 60000);
          logger.warn(`MySQL [${name}] retrying health check in ${retryDelay}ms (attempt ${consecutiveFailures}/${MYSQL_MAX_RETRIES})`);
          setTimeout(healthCheck, retryDelay);
        } else {
          logger.error(`❌ MySQL [${name}] max retries exceeded, will keep trying at normal interval`);
          consecutiveFailures = 0;
        }
      } else {
        if (consecutiveFailures > 0) {
          logger.info(`✅ MySQL [${name}] reconnected after ${consecutiveFailures} failures`);
          consecutiveFailures = 0;
        }
      }
    });
  }

  // Initial connectivity check
  pool.getConnection((err: any, connection: any) => {
    if (err) {
      console.error(`❌ MySQL [${name}] initial connection FAILED: ${err.code} ${err.message}`);
      logger.error(`❌ MySQL [${name}] initial connection FAILED`, err);
      // Start retry immediately
      setTimeout(healthCheck, MYSQL_RETRY_DELAY_MS);
      return;
    }

    console.log(`✅ MySQL [${name}] CONNECTED and READY`);
    logger.info(`✅ MySQL [${name}] CONNECTED and READY`);
    connection.release();
  });

  // Periodic health check keeps pool alive and detects disconnects
  setInterval(healthCheck, MYSQL_HEALTH_CHECK_INTERVAL);

  return pool;
}

export const mysqlEmailConnection = createMySQLPool(configs.emailService, 'emailService');
export const mysqlPoolConnection = createMySQLPool(configs.sqlCcaasDB, 'ccaasDB');
export const mysqlMyaccPoolConnection = createMySQLPool(configs.sqlMyAcccaasDB, 'myAccDB');
