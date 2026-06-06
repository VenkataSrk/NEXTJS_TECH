
import mysql from 'mysql';
import { configs } from '../config/app';
import { logger } from './log';

/**
 * Mysql pool connection
 */
const RECONNECT_BASE_DELAY_MS = 2000;
const RECONNECT_MAX_DELAY_MS = 30000;

let reconnectTimer: NodeJS.Timeout | null = null;
let reconnectAttempts = 0;

function isRetryableDbError(code?: string) {
  return [
    'PROTOCOL_CONNECTION_LOST',
    'ECONNRESET',
    'ECONNREFUSED',
    'ETIMEDOUT',
    'PROTOCOL_ENQUEUE_AFTER_FATAL_ERROR'
  ].includes(code || '');
}

function getReconnectDelay(attempt: number) {
  return Math.min(RECONNECT_BASE_DELAY_MS * Math.max(1, attempt), RECONNECT_MAX_DELAY_MS);
}

function scheduleReconnect(reason: string, errCode?: string) {
  if (reconnectTimer) {
    return;
  }

  reconnectAttempts += 1;
  const delay = getReconnectDelay(reconnectAttempts);

  console.warn(
    `⚠️ Scheduling MySQL reconnect attempt #${reconnectAttempts} in ${delay}ms | Reason: ${reason} | Code: ${
      errCode || 'NA'
    }`
  );
  logger.warn(
    `⚠️ Scheduling MySQL reconnect attempt #${reconnectAttempts} in ${delay}ms | Reason: ${reason} | Code: ${
      errCode || 'NA'
    }`
  );

  reconnectTimer = setTimeout(() => {
    reconnectTimer = null;
    reinitializePool();
  },                          delay);
}

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

    if (isRetryableDbError(err?.code)) {
      console.warn('⚠️ Pool detected lost connections');
      logger.warn('⚠️ Pool detected lost connections');
      scheduleReconnect('pool_error', err?.code);
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

      if (isRetryableDbError(err?.code)) {
        scheduleReconnect('connection_error', err?.code);
      }
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

      if (isRetryableDbError(err?.code)) {
        scheduleReconnect('startup_connectivity_check', err?.code);
      }
      return;
    }

    console.log('✅ MySQL POOL is CONNECTED and READY');
    logger.info('✅ MySQL POOL is CONNECTED and READY');
    reconnectAttempts = 0;
    connection.release();
  });

  return pool;
}

function reinitializePool() {
  const previousPool = mysqlPoolConnection;
  const nextPool = createMySQLPool(configs.sqlCcaasDB);
  mysqlPoolConnection = nextPool;

  if (!previousPool) {
    return;
  }

  previousPool.end((err?: mysql.MysqlError | null) => {
    if (err) {
      console.error('❌ Failed to close previous MySQL pool:', err.code, err.message);
      logger.error('❌ Failed to close previous MySQL pool', err);
      return;
    }

    console.log('✅ Previous MySQL pool closed');
    logger.info('✅ Previous MySQL pool closed');
  });
}

export let mysqlPoolConnection = createMySQLPool(configs.sqlCcaasDB);
