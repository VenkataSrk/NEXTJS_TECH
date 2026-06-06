import { Emitter } from '@socket.io/redis-emitter';
import Redis, { RedisOptions } from 'ioredis';
import mysql from 'mysql';
import { configs } from '../config/app';
import { logger } from './log';

// ==================== RETRY CONFIGURATION ====================
const MAX_REDIS_RETRY_ATTEMPTS = 20;
const MAX_REDIS_RETRY_DELAY = 30000; // 30 seconds max delay
const MYSQL_RETRY_INTERVAL = 5000; // 5 seconds between MySQL retries
const MAX_MYSQL_RETRY_ATTEMPTS = 10;

// ==================== REDIS RETRY STRATEGY ====================
function createRedisRetryStrategy(clientName: string): RedisOptions['retryStrategy'] {
  return (times: number) => {
    if (times > MAX_REDIS_RETRY_ATTEMPTS) {
      console.error(`[${clientName}] Redis max retry attempts (${MAX_REDIS_RETRY_ATTEMPTS}) reached. Giving up.`);
      logger.error(`[${clientName}] Redis max retry attempts (${MAX_REDIS_RETRY_ATTEMPTS}) reached. Giving up.`);
      return null;
    }
    const delay = Math.min(times * 1000, MAX_REDIS_RETRY_DELAY);
    console.log(`[${clientName}] Redis reconnecting... attempt ${times}, next retry in ${delay}ms`);
    logger.warn(`[${clientName}] Redis reconnecting... attempt ${times}, next retry in ${delay}ms`);
    return delay;
  };
}

function createRedisClient(name: string, options: RedisOptions): Redis {
  const client = new Redis({
    ...options,
    retryStrategy: createRedisRetryStrategy(name),
    maxRetriesPerRequest: null,
    enableReadyCheck: true,
    reconnectOnError: (err) => {
      const targetErrors = ['READONLY', 'ECONNRESET', 'ETIMEDOUT'];
      return targetErrors.some(e => err.message.includes(e));
    },
  });

  client.on('connect', () => {
    console.log(`[${name}] Redis connected`);
    logger.info(`[${name}] Redis connected`);
  });
  client.on('ready', () => {
    console.log(`[${name}] Redis ready`);
    logger.info(`[${name}] Redis ready`);
  });
  client.on('error', (err) => {
    console.error(`[${name}] Redis error:`, err.message);
    logger.error(`[${name}] Redis error:`, err.message);
  });
  client.on('close', () => {
    console.warn(`[${name}] Redis connection closed`);
    logger.warn(`[${name}] Redis connection closed`);
  });
  client.on('reconnecting', (delay: number) => {
    console.log(`[${name}] Redis reconnecting in ${delay}ms...`);
    logger.info(`[${name}] Redis reconnecting in ${delay}ms...`);
  });

  return client;
}

// ==================== MYSQL CONNECTION WITH RETRY ====================
/**
 * @param configuration
 * @returns mysql.Pool
 */
export function createMySQLPool(configuration: any) {
  console.log('[MySQL] Initializing MySQL pool...');
  logger.info('[MySQL] Initializing MySQL pool...');

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
    console.error('[MySQL] POOL error:', err.code, err.message);
    logger.error('[MySQL] POOL error', err);

    if (err.code === 'PROTOCOL_CONNECTION_LOST' || err.code === 'ECONNRESET') {
      console.warn('[MySQL] Pool detected lost connections');
      logger.warn('[MySQL] Pool detected lost connections');
    }
  });

    /* =========================
       CONNECTION EVENTS
       ========================= */
  pool.on('connection', (connection: any) => {
    console.log('[MySQL] Connection established | Thread ID:', connection.threadId);
    logger.info('[MySQL] Connection established | Thread ID:', connection.threadId);

    connection.on('error', (err: any) => {
      console.error(
                '[MySQL] Connection error | Thread:',
                connection.threadId,
                err.code,
                err.message
            );
      logger.error('[MySQL] Connection error', err);
    });
  });

  pool.on('acquire', (connection: any) => {
    console.log('[MySQL] Connection acquired | Thread:', connection.threadId);
    logger.info('[MySQL] Connection acquired | Thread:', connection.threadId);
  });

  pool.on('release', (connection: any) => {
    console.log('[MySQL] Connection released | Thread:', connection.threadId);
    logger.info('[MySQL] Connection released | Thread:', connection.threadId);
  });

    /* =========================
       CONNECTIVITY CHECK WITH RETRY
       ========================= */
  let retryCount = 0;

  function testConnection() {
    pool.getConnection((err: any, connection: any) => {
      if (err) {
        retryCount += 1;
        console.error(`[MySQL] Connection FAILED (attempt ${retryCount}/${MAX_MYSQL_RETRY_ATTEMPTS}): ${err.code} ${err.message}`);
        logger.error(`[MySQL] Connection FAILED (attempt ${retryCount}/${MAX_MYSQL_RETRY_ATTEMPTS})`, err);
        if (retryCount < MAX_MYSQL_RETRY_ATTEMPTS) {
          console.log(`[MySQL] Retrying in ${MYSQL_RETRY_INTERVAL}ms...`);
          logger.info(`[MySQL] Retrying in ${MYSQL_RETRY_INTERVAL}ms...`);
          setTimeout(testConnection, MYSQL_RETRY_INTERVAL);
        } else {
          console.error(`[MySQL] Max retry attempts (${MAX_MYSQL_RETRY_ATTEMPTS}) reached. Pool created but connection could not be verified.`);
          logger.error(`[MySQL] Max retry attempts (${MAX_MYSQL_RETRY_ATTEMPTS}) reached.`);
        }
        return;
      }

      retryCount = 0;
      console.log('[MySQL] POOL is CONNECTED and READY');
      logger.info('[MySQL] POOL is CONNECTED and READY');
      connection.release();
    });
  }

  testConnection();
  return pool;
}

// ==================== REDIS CONNECTIONS ====================
export const reportRedisClient = createRedisClient('ReportRedis', { host: process.env.SOCKET_REDIS_INSTANCE, port: 6379, password: process.env.SOCKET_REDIS_PASSWORD });
export const subscriberEmitter = reportRedisClient.duplicate();
export const ioredisReport = new Emitter(subscriberEmitter);

// ==================== MYSQL CONNECTION ====================
export const mysqlPoolConnection = createMySQLPool(configs.sqlCcaasDB);
