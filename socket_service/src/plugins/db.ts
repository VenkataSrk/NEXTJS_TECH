/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

import { Emitter } from '@socket.io/redis-emitter';
// import Redis from 'ioredis';
import Redis, { RedisOptions } from 'ioredis';
import mysql from 'mysql';
import { configs } from '../config/app';
import { isJSON } from '../constants';
import { logger } from './log';

// keep track of every redis client we create
const redisClients: Redis[] = [];
let redisClientSequence = 0;

const REDIS_RECONNECT_BASE_DELAY_MS = 1000;
const REDIS_RECONNECT_MAX_DELAY_MS = 10000;

const MYSQL_RECONNECT_BASE_DELAY_MS = 2000;
const MYSQL_RECONNECT_MAX_DELAY_MS = 30000;

let mysqlReconnectTimer: NodeJS.Timeout | null = null;
let mysqlReconnectAttempts = 0;
let isMysqlShuttingDown = false;

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

function registerRedisClient<T extends Redis>(client: T, clientName?: string): T {
  redisClientSequence += 1;
  const resolvedName = clientName || `redisClient_${redisClientSequence}`;
  attachRedisEventHandlers(client, resolvedName);
  redisClients.push(client);
  return client;
}

function createRedisClient(options: RedisOptions): Redis {
  return new Redis({
    ...options,
    maxRetriesPerRequest: null,
    retryStrategy(times: number) {
      const delay = getRedisRetryDelay(times);
      console.warn(`⚠️ Redis retry in ${delay}ms [${options.host}:${options.port}] attempt #${times}`);
      logger.warn(`⚠️ Redis retry in ${delay}ms [${options.host}:${options.port}] attempt #${times}`);
      return delay;
    },
    reconnectOnError(err: Error) {
      const message = err?.message || '';
      return (
        message.includes('READONLY') ||
        message.includes('ECONNRESET') ||
        message.includes('ETIMEDOUT')
      );
    }
  });
}

// Redis clients

export const redisClientAuth = registerRedisClient(
  createRedisClient({
    host: process.env.AUTH_REDIS_IP,
    port: 6379,
    password: process.env.AUTH_REDIS_PASSWORD,
  }),
  'redisClientAuth'
);

export const redisClient = registerRedisClient(
  createRedisClient({
    host: process.env.BLPOP_REDIS_INSTANCE,
    port: 6379,
    password: process.env.BLPOP_REDIS_PASSWORD,
  }),
  'redisClient'
);

export const pubsubRedisClient = registerRedisClient(
  createRedisClient({
    host: process.env.PUBSUB_REDIS_INSTANCE,
    port: 6379,
    password: process.env.PUBSUB_REDIS_PASSWORD,
  }),
  'pubsubRedisClient'
);

export const otherRedisClient = registerRedisClient(
  createRedisClient({
    host: process.env.SOCKET_REDIS_INSTANCE,
    port: 6379,
    password: process.env.SOCKET_REDIS_PASSWORD,
  }),
  'otherRedisClient'
);

export const NLPRedisClient = registerRedisClient(
  createRedisClient({
    host: process.env.NLP_BLPOP_REDIS_INSTANCE,
    port: 6379,
    password: process.env.NLP_BLPOP_REDIS_PASSWORD,
  }),
  'NLPRedisClient'
);

// export const redisClientAuth = new Redis({ host: process.env.AUTH_REDIS_IP, port: 6379, password: process.env.AUTH_REDIS_PASSWORD });
// export const redisClient = new Redis({ host: process.env.BLPOP_REDIS_INSTANCE, port: 6379, password: process.env.BLPOP_REDIS_PASSWORD });
// export const pubsubRedisClient = new Redis({ host: process.env.PUBSUB_REDIS_INSTANCE, port: 6379, password: process.env.PUBSUB_REDIS_PASSWORD });
// export const otherRedisClient = new Redis({ host: process.env.SOCKET_REDIS_INSTANCE, port: 6379, password: process.env.SOCKET_REDIS_PASSWORD });
// export const NLPRedisClient = new Redis({ host: process.env.NLP_BLPOP_REDIS_INSTANCE, port: 6379, password: process.env.NLP_BLPOP_REDIS_PASSWORD });

// export const B2cRedisClient = new Redis({ host: process.env.B2C_REDIS_INSTANCE, port: 6379, password: process.env.B2C_REDIS_PASSWORD });

export const B2cRedisClient = registerRedisClient(
  createRedisClient({
    host: process.env.B2C_REDIS_INSTANCE,
    port: 6379,
    password: process.env.B2C_REDIS_PASSWORD,
  }),
  'B2cRedisClient'
);

// Redis duplicates for various purposes
export const subscriber = registerRedisClient(redisClient.duplicate(), 'subscriber');
export const subscriberEmitter = registerRedisClient(otherRedisClient.duplicate(), 'subscriberEmitter');
export const ioredisChat = new Emitter(otherRedisClient);
export const subscriberAuth = registerRedisClient(redisClientAuth.duplicate(), 'subscriberAuth');
export const presenceStore = registerRedisClient(redisClientAuth.duplicate(), 'presenceStore');
export const queueGetter = registerRedisClient(otherRedisClient.duplicate(), 'queueGetter');
export const agentStatusQueueData = registerRedisClient(otherRedisClient.duplicate(), 'agentStatusQueueData');
export const fcmNotificationSubscriber = registerRedisClient(
  otherRedisClient.duplicate(),
  'fcmNotificationSubscriber'
);

export const nlp_sub = registerRedisClient(pubsubRedisClient.duplicate(), 'nlp_sub');
export const redisForPrensence = registerRedisClient(otherRedisClient.duplicate(), 'redisForPrensence');
export const redisForInactivity = registerRedisClient(pubsubRedisClient.duplicate(), 'redisForInactivity');
export const logOutPubInstance = registerRedisClient(otherRedisClient.duplicate(), 'logOutPubInstance');

// export const logOutPubInstance1 = otherRedisClient.duplicate();
export const logOutSubInstance = registerRedisClient(otherRedisClient.duplicate(), 'logOutSubInstance');
// export const logOutSubInstance1 = otherRedisClient.duplicate();

fcmNotificationSubscriber.subscribe('webChatConnected_mob_FCM', (err, count) => {
  if (err) {
    console.error('Failed to subscribe:', err);
  } else {
    console.log(`Subscribed successfully to ${count} channels.`);
  }
});

logOutSubInstance.subscribe('worktual-logout-channel_yoo', (err, count) => {
  if (err) {
    console.error('Failed to subscribe:', err);
  } else {
    console.log(`Subscribed successfully to ${count} channels.`);
  }
});

logOutSubInstance.on('message', (channel, message) => {
  console.log(`Received on ${channel}: ${message}`);
  const data = isJSON(message) ? JSON.parse(message) : message;
  if (data?.loginTime) {
    ioredisChat.to(`${data.domainId}_${data.ext}`).emit('newLoginUpdate', data);
  } else {
    ioredisChat.to(`${data.domainId}_${data.ext}`).emit('force_logout', data);
  }
});

logOutSubInstance.on('error', (err: any) => {
  console.error('logOutSubInstance Subscriber Redis error:', err);
});

/**
 * Mssql pool connection
 */

/**
 *
 * @param configuration
 * @returns
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

function scheduleMysqlReconnect(reason: string, errCode?: string) {
  if (isMysqlShuttingDown || mysqlReconnectTimer) {
    return;
  }

  mysqlReconnectAttempts += 1;
  const delay = getMysqlReconnectDelay(mysqlReconnectAttempts);

  console.warn(
    `⚠️ Scheduling MySQL reconnect attempt #${mysqlReconnectAttempts} in ${delay}ms | Reason: ${reason} | Code: ${
      errCode || 'NA'
    }`
  );
  logger.warn(
    `⚠️ Scheduling MySQL reconnect attempt #${mysqlReconnectAttempts} in ${delay}ms | Reason: ${reason} | Code: ${
      errCode || 'NA'
    }`
  );

  mysqlReconnectTimer = setTimeout(() => {
    mysqlReconnectTimer = null;
    reinitializeMysqlPool();
  },                               delay);
}

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
      scheduleMysqlReconnect('pool_error', err?.code);
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
        scheduleMysqlReconnect('connection_error', err?.code);
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
        scheduleMysqlReconnect('startup_connectivity_check', err?.code);
      }
      return;
    }

    console.log('✅ MySQL POOL is CONNECTED and READY');
    logger.info('✅ MySQL POOL is CONNECTED and READY');
    mysqlReconnectAttempts = 0;
    connection.release();
  });

  return pool;
}

function reinitializeMysqlPool() {
  if (isMysqlShuttingDown) {
    return;
  }

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

export const notificationMessages = (data: any, ext: any) => {
  // ioredisChat?.to(ext?.toString()).emit('notification', data);
};

export async function closeMysqlPool(): Promise<void> {
  return new Promise((resolve) => {
    isMysqlShuttingDown = true;
    if (mysqlReconnectTimer) {
      clearTimeout(mysqlReconnectTimer);
      mysqlReconnectTimer = null;
    }

    if (mysqlPoolConnection) {
      mysqlPoolConnection.end((err: any) => {
        if (err) {
          console.error('Error closing MySQL pool:', err);
        }
        resolve();
      });
    } else {
      resolve();
    }
  });
}

export async function closeAllRedis(): Promise<void> {
  for (const client of redisClients) {

    try {
      await client.quit();
    } catch (err) {
      console.error('Error quitting redis client:', err);
    }

    try {
      client.disconnect();
    } catch (err) {
      console.error('Error disconnecting redis client:', err);
    }
  }
}
