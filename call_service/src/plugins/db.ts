/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

import { Emitter } from '@socket.io/redis-emitter';
import Redis from 'ioredis';
import mysql from 'mysql';
import { configs } from '../config/app';
import { logger } from './winston';

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
  password: process.env.BLPOP_REDIS_PASSWORD
});
export const otherRedisClient = createRedisClient({
  host: process.env.SOCKET_REDIS_INSTANCE,
  port: 6379,
  password: process.env.SOCKET_REDIS_PASSWORD
});
export const callClient = createRedisClient({
  host: process.env.PUBSUB_REDIS_INSTANCE,
  port: 6379,
  password: process.env.PUBSUB_REDIS_PASSWORD
});
export const campaignClient = createRedisClient({
  host: process.env.CAMPAIGN_REDIS_INSTANCE,
  port: 6379,
  password: process.env.CAMPAIGN_REDIS_PASSWORD
});
export const ioredisChat = new Emitter(otherRedisClient);
export const call = callClient.duplicate();
export const TokenClient = campaignClient.duplicate();

/* ------------------ MYSQL RECONNECTION HANDLER ------------------ */
/* ========================= MYSQL WITH AUTO-RETRY ========================= */

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

export const mysqlPoolConnection = createMySQLPool(configs.sqlCcaasDB, 'ccaasDB');
