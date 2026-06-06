import { Emitter } from '@socket.io/redis-emitter';
import Redis from 'ioredis';
import mysql from 'mysql';
import { configs } from '../config/app';
import { logger } from './winston';

const REDIS_RECONNECT_BASE_DELAY_MS = 2000;
const REDIS_RECONNECT_MAX_DELAY_MS = 30000;
const MYSQL_RECONNECT_BASE_DELAY_MS = 2000;
const MYSQL_RECONNECT_MAX_DELAY_MS = 30000;

let mysqlReconnectTimer: NodeJS.Timeout | null = null;
let mysqlReconnectAttempts = 0;
const isMysqlShuttingDown = false;

function getRedisRetryDelay(attempt: number) {
  return Math.min(
    REDIS_RECONNECT_BASE_DELAY_MS * Math.max(1, attempt),
    REDIS_RECONNECT_MAX_DELAY_MS
  );
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
      return (
        message.includes('READONLY') ||
        message.includes('ECONNRESET') ||
        message.includes('ETIMEDOUT')
      );
    }
  });

  attachRedisEventHandlers(client, clientName);
  return client;
}

/**
 * Mysql pool connection
 */

function isRetryableDbError(code?: string) {
  return [
    'PROTOCOL_CONNECTION_LOST',
    'ECONNRESET',
    'ECONNREFUSED',
    'ETIMEDOUT',
    'PROTOCOL_ENQUEUE_AFTER_FATAL_ERROR'
  ].includes(code || '');
}

function getMysqlReconnectDelay(attempt: number) {
  return Math.min(
    MYSQL_RECONNECT_BASE_DELAY_MS * Math.max(1, attempt),
    MYSQL_RECONNECT_MAX_DELAY_MS
  );
}

function scheduleMysqlReconnect(reason: string, errCode?: string, poolName?: string) {
  if (isMysqlShuttingDown || mysqlReconnectTimer) {
    return;
  }

  mysqlReconnectAttempts += 1;
  const delay = getMysqlReconnectDelay(mysqlReconnectAttempts);

  console.warn(
    `⚠️ Scheduling MySQL reconnect attempt #${mysqlReconnectAttempts} in ${delay}ms | Pool: ${poolName || 'NA'} | Reason: ${reason} | Code: ${
      errCode || 'NA'
    }`
  );
  logger.warn(
    `⚠️ Scheduling MySQL reconnect attempt #${mysqlReconnectAttempts} in ${delay}ms | Pool: ${poolName || 'NA'} | Reason: ${reason} | Code: ${
      errCode || 'NA'
    }`
  );

  mysqlReconnectTimer = setTimeout(() => {
    mysqlReconnectTimer = null;
    reinitializeMysqlPools();
  },                               delay);
}

/**
 *
 * @param configuration
 * @returns
 */
export function createMySQLPool(configuration: any, poolName = 'mysqlPool') {
  console.log(`🔧 Initializing MySQL pool: ${poolName}`);
  logger.info(`🔧 Initializing MySQL pool: ${poolName}`);

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
    console.error(`❌ MySQL POOL error (${poolName}):`, err.code, err.message);
    logger.error(`❌ MySQL POOL error (${poolName})`, err);

    if (isRetryableDbError(err?.code)) {
      console.warn(`⚠️ Pool detected lost connections (${poolName})`);
      logger.warn(`⚠️ Pool detected lost connections (${poolName})`);
      scheduleMysqlReconnect('pool_error', err?.code, poolName);
    }
  });

  /* =========================
     CONNECTION EVENTS
     ========================= */
  pool.on('connection', (connection: any) => {
    console.log(`✅ MySQL connection established (${poolName}) | Thread ID:`, connection.threadId);
    logger.info(`✅ MySQL connection established (${poolName}) | Thread ID: ${connection.threadId}`);

    connection.on('error', (err: any) => {
      console.error(
        `❌ MySQL connection error (${poolName}) | Thread:`,
        connection.threadId,
        err.code,
        err.message
      );
      logger.error(`❌ MySQL connection error (${poolName})`, err);

      if (isRetryableDbError(err?.code)) {
        scheduleMysqlReconnect('connection_error', err?.code, poolName);
      }
    });
  });

  pool.on('acquire', (connection: any) => {
    console.log(`➡️ MySQL connection acquired (${poolName}) | Thread:`, connection.threadId);
    logger.info(`➡️ MySQL connection acquired (${poolName}) | Thread: ${connection.threadId}`);
  });

  pool.on('release', (connection: any) => {
    console.log(`⬅️ MySQL connection released (${poolName}) | Thread:`, connection.threadId);
    logger.info(`⬅️ MySQL connection released (${poolName}) | Thread: ${connection.threadId}`);
  });

  /* =========================
     EXPLICIT CONNECTIVITY CHECK
     ========================= */
  pool.getConnection((err: any, connection: any) => {
    if (err) {
      console.error(`❌ MySQL POOL connection FAILED (${poolName})`);
      console.error('❌ Reason:', err.code, err.message);
      logger.error(`❌ MySQL POOL connection FAILED (${poolName})`, err);

      if (isRetryableDbError(err?.code)) {
        scheduleMysqlReconnect('startup_connectivity_check', err?.code, poolName);
      }
      return;
    }

    console.log(`✅ MySQL POOL is CONNECTED and READY (${poolName})`);
    logger.info(`✅ MySQL POOL is CONNECTED and READY (${poolName})`);
    mysqlReconnectAttempts = 0;
    connection.release();
  });

  return pool;
}

function reinitializeMysqlPools() {
  if (isMysqlShuttingDown) {
    return;
  }

  const previousMainPool = mysqlPoolConnection;
  const previousRolePool = mysqlPoolConnectionRole;
  const previousBotRolePool = botMysqlPoolConnections;
  const previousWorktualPool = mysqlPoolConnectionWorktual;

  mysqlPoolConnection = createMySQLPool(configs.sqlCcaasDB, 'sqlCcaasDB');
  mysqlPoolConnectionRole = createMySQLPool(configs.sqlRoleDB, 'sqlRoleDB');
  botMysqlPoolConnections = createMySQLPool(configs.botSqlRoleDB, 'botSqlRoleDB');
  mysqlPoolConnectionWorktual = createMySQLPool(configs.sqlMyaccountDB, 'sqlMyaccountDB');

  const closeOldPool = (pool: any, poolName: string) => {
    if (!pool) {
      return;
    }

    pool.end((err?: mysql.MysqlError | null) => {
      if (err) {
        console.error(`❌ Failed to close previous MySQL pool (${poolName}):`, err.code, err.message);
        logger.error(`❌ Failed to close previous MySQL pool (${poolName})`, err);
        return;
      }

      console.log(`✅ Previous MySQL pool closed (${poolName})`);
      logger.info(`✅ Previous MySQL pool closed (${poolName})`);
    });
  };

  closeOldPool(previousMainPool, 'sqlCcaasDB');
  closeOldPool(previousRolePool, 'sqlRoleDB');
  closeOldPool(previousBotRolePool, 'botSqlRoleDB');
  closeOldPool(previousWorktualPool, 'sqlMyaccountDB');
}

export let mysqlPoolConnection = createMySQLPool(configs.sqlCcaasDB, 'sqlCcaasDB');
export let mysqlPoolConnectionRole = createMySQLPool(configs.sqlRoleDB, 'sqlRoleDB');
export let botMysqlPoolConnections = createMySQLPool(configs.botSqlRoleDB, 'botSqlRoleDB');
export let mysqlPoolConnectionWorktual = createMySQLPool(configs.sqlMyaccountDB, 'sqlMyaccountDB');

export const coreRedisClient = createRedisClient('coreRedisClient', {
  host: process.env.SOCKET_REDIS_INSTANCE,
  port: Number(process.env.SOCKET_REDIS_PORT),
  password: process.env.SOCKET_REDIS_PASSWORD
});

export const store = createRedisClient('store', {
  host: process.env.VIDEO_SOCKET_INSTANCE,
  port: Number(process.env.SOCKET_REDIS_PORT),
  password: process.env.VIDEO_SOCKET_PASSWORD
});

export const gueststore = createRedisClient('gueststore', {
  host: process.env.GUEST_VIDEO_SOCKET_INSTANCE,
  port: Number(process.env.SOCKET_REDIS_PORT),
  password: process.env.GUEST_VIDEO_SOCKET_PASSWORD
});

export const subscriberEmitter = coreRedisClient.duplicate();
attachRedisEventHandlers(subscriberEmitter, 'subscriberEmitter');

export const ioredisWhatsapp = new Emitter(subscriberEmitter);

export const notificationMessages = (data: any, ext: any) => {
  if (ext) {
    ioredisWhatsapp.to(ext).emit('notification', data);
  }
};

export const ioredisWhatsappPushPop = createRedisClient('ioredisWhatsappPushPop', {
  host: process.env.BLPOP_REDIS_INSTANCE,
  port: Number(process.env.SOCKET_REDIS_PORT),
  password: process.env.BLPOP_REDIS_PASSWORD
});

export const clientRpush = ioredisWhatsappPushPop.duplicate();
attachRedisEventHandlers(clientRpush, 'clientRpush');
