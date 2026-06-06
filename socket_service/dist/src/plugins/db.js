"use strict";
/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */
var __awaiter = (this && this.__awaiter) || function (thisArg, _arguments, P, generator) {
    function adopt(value) { return value instanceof P ? value : new P(function (resolve) { resolve(value); }); }
    return new (P || (P = Promise))(function (resolve, reject) {
        function fulfilled(value) { try { step(generator.next(value)); } catch (e) { reject(e); } }
        function rejected(value) { try { step(generator["throw"](value)); } catch (e) { reject(e); } }
        function step(result) { result.done ? resolve(result.value) : adopt(result.value).then(fulfilled, rejected); }
        step((generator = generator.apply(thisArg, _arguments || [])).next());
    });
};
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.closeAllRedis = exports.closeMysqlPool = exports.notificationMessages = exports.mysqlPoolConnection = exports.createMySQLPool = exports.logOutSubInstance = exports.logOutPubInstance = exports.redisForInactivity = exports.redisForPrensence = exports.nlp_sub = exports.fcmNotificationSubscriber = exports.agentStatusQueueData = exports.queueGetter = exports.presenceStore = exports.subscriberAuth = exports.ioredisChat = exports.subscriberEmitter = exports.subscriber = exports.B2cRedisClient = exports.NLPRedisClient = exports.otherRedisClient = exports.pubsubRedisClient = exports.redisClient = exports.redisClientAuth = void 0;
const redis_emitter_1 = require("@socket.io/redis-emitter");
// import Redis from 'ioredis';
const ioredis_1 = __importDefault(require("ioredis"));
const mysql_1 = __importDefault(require("mysql"));
const app_1 = require("../config/app");
const constants_1 = require("../constants");
const log_1 = require("./log");
// keep track of every redis client we create
const redisClients = [];
let redisClientSequence = 0;
const REDIS_RECONNECT_BASE_DELAY_MS = 1000;
const REDIS_RECONNECT_MAX_DELAY_MS = 10000;
const MYSQL_RECONNECT_BASE_DELAY_MS = 2000;
const MYSQL_RECONNECT_MAX_DELAY_MS = 30000;
let mysqlReconnectTimer = null;
let mysqlReconnectAttempts = 0;
let isMysqlShuttingDown = false;
function getRedisRetryDelay(attempt) {
    return Math.min(REDIS_RECONNECT_BASE_DELAY_MS * Math.max(1, attempt), REDIS_RECONNECT_MAX_DELAY_MS);
}
function attachRedisEventHandlers(client, clientName) {
    client.on('connect', () => {
        console.log(`🔗 Redis connected: ${clientName}`);
        log_1.logger.info(`🔗 Redis connected: ${clientName}`);
    });
    client.on('ready', () => {
        console.log(`✅ Redis ready: ${clientName}`);
        log_1.logger.info(`✅ Redis ready: ${clientName}`);
    });
    client.on('reconnecting', (time) => {
        console.warn(`⚠️ Redis reconnecting in ${time}ms: ${clientName}`);
        log_1.logger.warn(`⚠️ Redis reconnecting in ${time}ms: ${clientName}`);
    });
    client.on('close', () => {
        console.warn(`⚠️ Redis connection closed: ${clientName}`);
        log_1.logger.warn(`⚠️ Redis connection closed: ${clientName}`);
    });
    client.on('end', () => {
        console.error(`❌ Redis connection ended: ${clientName}`);
        log_1.logger.error(`❌ Redis connection ended: ${clientName}`);
    });
    client.on('error', (err) => {
        console.error(`❌ Redis error (${clientName}):`, (err === null || err === void 0 ? void 0 : err.message) || err);
        log_1.logger.error(`❌ Redis error (${clientName})`, err);
    });
}
function registerRedisClient(client, clientName) {
    redisClientSequence += 1;
    const resolvedName = clientName || `redisClient_${redisClientSequence}`;
    attachRedisEventHandlers(client, resolvedName);
    redisClients.push(client);
    return client;
}
function createRedisClient(options) {
    return new ioredis_1.default(Object.assign(Object.assign({}, options), { maxRetriesPerRequest: null, retryStrategy(times) {
            const delay = getRedisRetryDelay(times);
            console.warn(`⚠️ Redis retry in ${delay}ms [${options.host}:${options.port}] attempt #${times}`);
            log_1.logger.warn(`⚠️ Redis retry in ${delay}ms [${options.host}:${options.port}] attempt #${times}`);
            return delay;
        },
        reconnectOnError(err) {
            const message = (err === null || err === void 0 ? void 0 : err.message) || '';
            return (message.includes('READONLY') ||
                message.includes('ECONNRESET') ||
                message.includes('ETIMEDOUT'));
        } }));
}
// Redis clients
exports.redisClientAuth = registerRedisClient(createRedisClient({
    host: process.env.AUTH_REDIS_IP,
    port: 6379,
    password: process.env.AUTH_REDIS_PASSWORD,
}), 'redisClientAuth');
exports.redisClient = registerRedisClient(createRedisClient({
    host: process.env.BLPOP_REDIS_INSTANCE,
    port: 6379,
    password: process.env.BLPOP_REDIS_PASSWORD,
}), 'redisClient');
exports.pubsubRedisClient = registerRedisClient(createRedisClient({
    host: process.env.PUBSUB_REDIS_INSTANCE,
    port: 6379,
    password: process.env.PUBSUB_REDIS_PASSWORD,
}), 'pubsubRedisClient');
exports.otherRedisClient = registerRedisClient(createRedisClient({
    host: process.env.SOCKET_REDIS_INSTANCE,
    port: 6379,
    password: process.env.SOCKET_REDIS_PASSWORD,
}), 'otherRedisClient');
exports.NLPRedisClient = registerRedisClient(createRedisClient({
    host: process.env.NLP_BLPOP_REDIS_INSTANCE,
    port: 6379,
    password: process.env.NLP_BLPOP_REDIS_PASSWORD,
}), 'NLPRedisClient');
// export const redisClientAuth = new Redis({ host: process.env.AUTH_REDIS_IP, port: 6379, password: process.env.AUTH_REDIS_PASSWORD });
// export const redisClient = new Redis({ host: process.env.BLPOP_REDIS_INSTANCE, port: 6379, password: process.env.BLPOP_REDIS_PASSWORD });
// export const pubsubRedisClient = new Redis({ host: process.env.PUBSUB_REDIS_INSTANCE, port: 6379, password: process.env.PUBSUB_REDIS_PASSWORD });
// export const otherRedisClient = new Redis({ host: process.env.SOCKET_REDIS_INSTANCE, port: 6379, password: process.env.SOCKET_REDIS_PASSWORD });
// export const NLPRedisClient = new Redis({ host: process.env.NLP_BLPOP_REDIS_INSTANCE, port: 6379, password: process.env.NLP_BLPOP_REDIS_PASSWORD });
// export const B2cRedisClient = new Redis({ host: process.env.B2C_REDIS_INSTANCE, port: 6379, password: process.env.B2C_REDIS_PASSWORD });
exports.B2cRedisClient = registerRedisClient(createRedisClient({
    host: process.env.B2C_REDIS_INSTANCE,
    port: 6379,
    password: process.env.B2C_REDIS_PASSWORD,
}), 'B2cRedisClient');
// Redis duplicates for various purposes
exports.subscriber = registerRedisClient(exports.redisClient.duplicate(), 'subscriber');
exports.subscriberEmitter = registerRedisClient(exports.otherRedisClient.duplicate(), 'subscriberEmitter');
exports.ioredisChat = new redis_emitter_1.Emitter(exports.otherRedisClient);
exports.subscriberAuth = registerRedisClient(exports.redisClientAuth.duplicate(), 'subscriberAuth');
exports.presenceStore = registerRedisClient(exports.redisClientAuth.duplicate(), 'presenceStore');
exports.queueGetter = registerRedisClient(exports.otherRedisClient.duplicate(), 'queueGetter');
exports.agentStatusQueueData = registerRedisClient(exports.otherRedisClient.duplicate(), 'agentStatusQueueData');
exports.fcmNotificationSubscriber = registerRedisClient(exports.otherRedisClient.duplicate(), 'fcmNotificationSubscriber');
exports.nlp_sub = registerRedisClient(exports.pubsubRedisClient.duplicate(), 'nlp_sub');
exports.redisForPrensence = registerRedisClient(exports.otherRedisClient.duplicate(), 'redisForPrensence');
exports.redisForInactivity = registerRedisClient(exports.pubsubRedisClient.duplicate(), 'redisForInactivity');
exports.logOutPubInstance = registerRedisClient(exports.otherRedisClient.duplicate(), 'logOutPubInstance');
// export const logOutPubInstance1 = otherRedisClient.duplicate();
exports.logOutSubInstance = registerRedisClient(exports.otherRedisClient.duplicate(), 'logOutSubInstance');
// export const logOutSubInstance1 = otherRedisClient.duplicate();
exports.fcmNotificationSubscriber.subscribe('webChatConnected_mob_FCM', (err, count) => {
    if (err) {
        console.error('Failed to subscribe:', err);
    }
    else {
        console.log(`Subscribed successfully to ${count} channels.`);
    }
});
exports.logOutSubInstance.subscribe('worktual-logout-channel_yoo', (err, count) => {
    if (err) {
        console.error('Failed to subscribe:', err);
    }
    else {
        console.log(`Subscribed successfully to ${count} channels.`);
    }
});
exports.logOutSubInstance.on('message', (channel, message) => {
    console.log(`Received on ${channel}: ${message}`);
    const data = (0, constants_1.isJSON)(message) ? JSON.parse(message) : message;
    if (data === null || data === void 0 ? void 0 : data.loginTime) {
        exports.ioredisChat.to(`${data.domainId}_${data.ext}`).emit('newLoginUpdate', data);
    }
    else {
        exports.ioredisChat.to(`${data.domainId}_${data.ext}`).emit('force_logout', data);
    }
});
exports.logOutSubInstance.on('error', (err) => {
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
function isRetryableDbError(code) {
    return [
        'PROTOCOL_CONNECTION_LOST',
        'ECONNRESET',
        'ECONNREFUSED',
        'ETIMEDOUT',
        'PROTOCOL_ENQUEUE_AFTER_FATAL_ERROR'
    ].includes(code || '');
}
function getMysqlReconnectDelay(attempt) {
    return Math.min(MYSQL_RECONNECT_BASE_DELAY_MS * Math.max(1, attempt), MYSQL_RECONNECT_MAX_DELAY_MS);
}
function scheduleMysqlReconnect(reason, errCode) {
    if (isMysqlShuttingDown || mysqlReconnectTimer) {
        return;
    }
    mysqlReconnectAttempts += 1;
    const delay = getMysqlReconnectDelay(mysqlReconnectAttempts);
    console.warn(`⚠️ Scheduling MySQL reconnect attempt #${mysqlReconnectAttempts} in ${delay}ms | Reason: ${reason} | Code: ${errCode || 'NA'}`);
    log_1.logger.warn(`⚠️ Scheduling MySQL reconnect attempt #${mysqlReconnectAttempts} in ${delay}ms | Reason: ${reason} | Code: ${errCode || 'NA'}`);
    mysqlReconnectTimer = setTimeout(() => {
        mysqlReconnectTimer = null;
        reinitializeMysqlPool();
    }, delay);
}
function createMySQLPool(configuration) {
    console.log('🔧 Initializing MySQL pool...');
    log_1.logger.info('🔧 Initializing MySQL pool...');
    const pool = mysql_1.default.createPool(Object.assign(Object.assign({}, configuration), { waitForConnections: true, connectionLimit: (configuration === null || configuration === void 0 ? void 0 : configuration.connectionLimit) || 10, queueLimit: 0 }));
    /* =========================
       POOL-LEVEL ERROR HANDLER
       ========================= */
    pool.on('error', (err) => {
        console.error('❌ MySQL POOL error:', err.code, err.message);
        log_1.logger.error('❌ MySQL POOL error', err);
        if (isRetryableDbError(err === null || err === void 0 ? void 0 : err.code)) {
            console.warn('⚠️ Pool detected lost connections');
            log_1.logger.warn('⚠️ Pool detected lost connections');
            scheduleMysqlReconnect('pool_error', err === null || err === void 0 ? void 0 : err.code);
        }
    });
    /* =========================
       CONNECTION EVENTS
       ========================= */
    pool.on('connection', (connection) => {
        console.log('✅ MySQL connection established | Thread ID:', connection.threadId);
        log_1.logger.info('✅ MySQL connection established | Thread ID:', connection.threadId);
        connection.on('error', (err) => {
            console.error('❌ MySQL connection error | Thread:', connection.threadId, err.code, err.message);
            log_1.logger.error('❌ MySQL connection error', err);
            if (isRetryableDbError(err === null || err === void 0 ? void 0 : err.code)) {
                scheduleMysqlReconnect('connection_error', err === null || err === void 0 ? void 0 : err.code);
            }
        });
    });
    pool.on('acquire', (connection) => {
        console.log('➡️ MySQL connection acquired | Thread:', connection.threadId);
        log_1.logger.info('➡️ MySQL connection acquired | Thread:', connection.threadId);
    });
    pool.on('release', (connection) => {
        console.log('⬅️ MySQL connection released | Thread:', connection.threadId);
        log_1.logger.info('⬅️ MySQL connection released | Thread:', connection.threadId);
    });
    /* =========================
       EXPLICIT CONNECTIVITY CHECK
       ========================= */
    pool.getConnection((err, connection) => {
        if (err) {
            console.error('❌ MySQL POOL connection FAILED');
            console.error('❌ Reason:', err.code, err.message);
            log_1.logger.error('❌ MySQL POOL connection FAILED', err);
            if (isRetryableDbError(err === null || err === void 0 ? void 0 : err.code)) {
                scheduleMysqlReconnect('startup_connectivity_check', err === null || err === void 0 ? void 0 : err.code);
            }
            return;
        }
        console.log('✅ MySQL POOL is CONNECTED and READY');
        log_1.logger.info('✅ MySQL POOL is CONNECTED and READY');
        mysqlReconnectAttempts = 0;
        connection.release();
    });
    return pool;
}
exports.createMySQLPool = createMySQLPool;
function reinitializeMysqlPool() {
    if (isMysqlShuttingDown) {
        return;
    }
    const previousPool = exports.mysqlPoolConnection;
    const nextPool = createMySQLPool(app_1.configs.sqlCcaasDB);
    exports.mysqlPoolConnection = nextPool;
    if (!previousPool) {
        return;
    }
    previousPool.end((err) => {
        if (err) {
            console.error('❌ Failed to close previous MySQL pool:', err.code, err.message);
            log_1.logger.error('❌ Failed to close previous MySQL pool', err);
            return;
        }
        console.log('✅ Previous MySQL pool closed');
        log_1.logger.info('✅ Previous MySQL pool closed');
    });
}
exports.mysqlPoolConnection = createMySQLPool(app_1.configs.sqlCcaasDB);
const notificationMessages = (data, ext) => {
    // ioredisChat?.to(ext?.toString()).emit('notification', data);
};
exports.notificationMessages = notificationMessages;
function closeMysqlPool() {
    return __awaiter(this, void 0, void 0, function* () {
        return new Promise((resolve) => {
            isMysqlShuttingDown = true;
            if (mysqlReconnectTimer) {
                clearTimeout(mysqlReconnectTimer);
                mysqlReconnectTimer = null;
            }
            if (exports.mysqlPoolConnection) {
                exports.mysqlPoolConnection.end((err) => {
                    if (err) {
                        console.error('Error closing MySQL pool:', err);
                    }
                    resolve();
                });
            }
            else {
                resolve();
            }
        });
    });
}
exports.closeMysqlPool = closeMysqlPool;
function closeAllRedis() {
    return __awaiter(this, void 0, void 0, function* () {
        for (const client of redisClients) {
            try {
                yield client.quit();
            }
            catch (err) {
                console.error('Error quitting redis client:', err);
            }
            try {
                client.disconnect();
            }
            catch (err) {
                console.error('Error disconnecting redis client:', err);
            }
        }
    });
}
exports.closeAllRedis = closeAllRedis;
//# sourceMappingURL=db.js.map