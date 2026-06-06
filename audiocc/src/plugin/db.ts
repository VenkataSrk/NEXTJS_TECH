/**
 * @createdBy <Jayaselva.M>
 * @createdOn
 */

import { Emitter } from '@socket.io/redis-emitter';
import { Redis } from 'ioredis';
import mysql from 'mysql';
import { agentAssistMsg, audiocc } from '../modules/audiocc';
import { audioMsg } from '../dao/audiocc';
import { logger } from './log';
import { config } from 'dotenv';

const env = process.env.NODE_ENV || 'qa';
console.log(`✅ Loaded environment: ${env}`);
config({ path: `.env.${env}` });

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

export const otherRedisClient = createRedisClient({
  host: process.env.SOCKET_REDIS_INSTANCE,
  port: 6379,
  password: process.env.SOCKET_REDIS_PASSWORD,
});

export const subscriberEmitter = otherRedisClient.duplicate();
export const ioredisChat = new Emitter(subscriberEmitter);
export const convoRedisClient = otherRedisClient.duplicate();
export const audioClientStream = redisClient.duplicate();
export const agentAssistStream = redisClient.duplicate();
const streamclient = redisClient.duplicate();
const audioClient = redisClient.duplicate();
const agentAssistClient = redisClient.duplicate();

/* ------------------ MYSQL RECONNECTION HANDLER ------------------ */

function createMySQLPool(configuration: any) {
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
    logger.error('❌ MySQL POOL error:', err);

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
  options: { encrypt: false },
});

streamclient.xtrim(process.env.CALL_STREAM_NAME, 'MAXLEN', 0)
  .then(() => {
    console.log(`Stream ${process.env.CALL_STREAM_NAME} has been deleted.`);
    createGroup();
  })
  .catch((err) => {
    console.error('Error deleting the stream:', err);
    logger.error('Error deleting the stream:', err);
  })
  .finally(() => {
    // client.quit();
  });

function deleteGroup() {
  streamclient.xgroup('DESTROY', process.env.CALL_STREAM_NAME, process.env.CALL_CONSUMER_GROUP)
    .then((response) => {
      console.log(`Stream "${process.env.CALL_STREAM_NAME}" and consumer group "${process.env.CALL_CONSUMER_GROUP}" destroyed.`);
      createGroup();

    })
    .catch((err) => {
      console.error('Error destroying stream and consumer group:', err);
      logger.error('Error destroying stream and consumer group:', err);
      createGroup();
    });
}

function deleteConsumer() {
  streamclient.xgroup('DELCONSUMER', process.env.CALL_STREAM_NAME, process.env.CALL_CONSUMER_GROUP, process.env.CALL_CONSUMER_NAME)
    .then((response) => {
      if (response === 1) {
        console.log(`Consumer "${process.env.CALL_CONSUMER_NAME}" removed from group "${process.env.CALL_CONSUMER_GROUP}"`);

        deleteGroup();
      } else {
        console.log(`Consumer "${process.env.CALL_CONSUMER_NAME}" not found in group "${process.env.CALL_CONSUMER_GROUP}"`);
        deleteGroup();
      }
    })
    .catch((err) => {
      console.error('Error removing consumer:', err);
      logger.error('Error removing consumer:', err);
      deleteGroup();
    });
}

function acknowledgeMessage(stream: any, group: any, messageId: any, messageData: any) {
  try {
    streamclient.xack(stream, group, messageId, (err, result) => {
      if (err) {
        console.error('Error Androidacknowledging message cc:', err);
        logger.error('Error Androidacknowledging message cc:', JSON.stringify({ stream, group, messageId, messageData }), err);
      } else if (result === 1) {

        console.log(`${stream}_${group}  Acknowledged message ${messageId}_ACK`);

      } else {
        console.log(`${stream}_${group}  Message ${messageData?.iuid}_${messageId}_ACK not found in consumer group`);
      }
    });
  } catch (error) {
    logger.error('acknowledgeMessage', JSON.stringify({ stream, group, messageId, messageData }), error);
  }
}

function readMessages(stream: any, group: any, consumer: any) {
  try {
    streamclient.xreadgroup(
      'GROUP', group, consumer, 'BLOCK', 0, 'STREAMS', stream, '>', (err, streams) => {
        if (err) {
          console.error('Error reading messages cc:', err);
          logger.error('Error reading messages cc:', JSON.stringify({ stream, group, consumer }), err);
        } else {
          if (streams) {
            streams.forEach((entry: any) => {
              const [streamName, messages] = entry;
              messages.forEach(([messageId, messageData]) => {
                console.log(`READ MESSAGE::::: ${stream}_${group} Message ID: ${messageId}, Message Data: ${messageData}`);
                const rawMessage = `${messageData}`.split('transcript_data,');
                acknowledgeMessage(stream, group, messageId, JSON.parse(rawMessage?.[1] ?? 'null'));
                audiocc(JSON.parse(rawMessage?.[1] ?? 'null'));
              });
            });
          }
          readMessages(stream, group, consumer);
        }
      }
    );
  } catch (error) {
    logger.error('readMessages', JSON.stringify({ stream, group, consumer }), error);
  }
}

function createGroup() {
  try {
    streamclient.xgroup('CREATE', process.env.CALL_STREAM_NAME, process.env.CALL_CONSUMER_GROUP, '0', 'MKSTREAM', (err) => {
      if (err && !err.message.includes('BUSYGROUP Consumer Group name already exists')) {
        console.error('Error creating consumer group cc:', err);
        logger.error('Error creating consumer group cc:', process.env.CALL_STREAM_NAME, process.env.CALL_CONSUMER_GROUP, err);
        deleteConsumer();
      } else {
        console.log('START READING MESSAGE::::::');
        readMessages(process.env.CALL_STREAM_NAME, process.env.CALL_CONSUMER_GROUP, process.env.CALL_CONSUMER_NAME);
      }
    });
  } catch (error) {
    logger.error('createGroup', process.env.CALL_STREAM_NAME, process.env.CALL_CONSUMER_GROUP, error);
  }
}

/***************************************************audio cc queue****************************************************************/
audioClient.xtrim(process.env.CALL_STREAM_MESSAGE_NAME, 'MAXLEN', 0)
  .then(() => {
    console.log(`Stream ${process.env.CALL_STREAM_MESSAGE_NAME} has been deleted.`);
    createAudioGroup();
  })
  .catch((err) => {
    console.error('Error deleting the stream audiocc queue:', err);
    logger.error('Error deleting the stream audiocc queue:', err);
  })
  .finally(() => {
    // client.quit();
  });

function deleteAudioGroup() {
  audioClient.xgroup('DESTROY', process.env.CALL_STREAM_MESSAGE_NAME, process.env.CALL_CONSUMER_MESSAGE_GROUP)
    .then((response) => {
      console.log(`Stream "${process.env.CALL_STREAM_MESSAGE_NAME}" and consumer group "${process.env.CALL_CONSUMER_MESSAGE_GROUP}" destroyed.`);
      createAudioGroup();

    })
    .catch((err) => {
      console.error('Error destroying stream and consumer group audiocc queue:', err);
      logger.error('Error destroying stream and consumer group audiocc queue:', err);
      createAudioGroup();
    });
}

function deleteAudioConsumer() {
  audioClient.xgroup('DELCONSUMER', process.env.CALL_STREAM_MESSAGE_NAME, process.env.CALL_CONSUMER_MESSAGE_GROUP, process.env.CALL_CONSUMER_MESSAGE_NAME)
    .then((response) => {
      if (response === 1) {
        console.log(`Consumer "${process.env.CALL_CONSUMER_MESSAGE_NAME}" removed from group "${process.env.CALL_CONSUMER_MESSAGE_GROUP}"`);

        deleteAudioGroup();
      } else {
        console.log(`Consumer "${process.env.CALL_CONSUMER_MESSAGE_NAME}" not found in group "${process.env.CALL_CONSUMER_MESSAGE_GROUP}"`);
        deleteAudioGroup();
      }
    })
    .catch((err) => {
      console.error('Error removing consumer audiocc queue:', err);
      logger.error('Error removing consumer audiocc queue:', err);
      deleteAudioGroup();
    });
}

function acknowledgeAudioMessage(stream: any, group: any, messageId: any, messageData: any) {
  try {
    audioClient.xack(stream, group, messageId, (err, result) => {
      if (err) {
        console.error('Error Androidacknowledging message audio queue:', err);
        logger.error('Error Androidacknowledging message audio queue:', JSON.stringify({ stream, group, messageId, messageData }), err);
      } else if (result === 1) {

        console.log(`${stream}_${group}  Acknowledged message ${messageId}_ACK`);

      } else {
        console.log(`${stream}_${group}  Message ${messageData?.iuid}_${messageId}_ACK not found in consumer group`);
      }
    });
  } catch (error) {
    logger.error('acknowledgeAudioMessage audiocc queue', JSON.stringify({ stream, group, messageId, messageData }), error);
  }
}

function readAudioMessages(stream: any, group: any, consumer: any) {
  try {
    audioClient.xreadgroup(
      'GROUP', group, consumer, 'BLOCK', 0, 'STREAMS', stream, '>', (err, streams) => {
        if (err) {
          console.error('Error reading messages audio queue:', err);
          logger.error('Error reading messages audio queue:', JSON.stringify({ stream, group, consumer }), err);
        } else {
          if (streams) {
            streams.forEach((entry: any) => {
              const [streamName, messages] = entry;
              messages.forEach(async ([messageId, messageData]) => {
                console.log(`READ MESSAGE::::: ${stream}_${group} Message ID: ${messageId}, Message Data: ${messageData}`);
                const rawMessage = `${messageData}`.split('transcript_data,');
                acknowledgeAudioMessage(stream, group, messageId, JSON.parse(rawMessage?.[1] ?? 'null'));
                try {
                  await audioMsg(JSON.parse(rawMessage?.[1] ?? 'null'))
                    .catch((err2) => {
                      console.error('Error audioMsg SP:', err2);
                      logger.error('Error audioMsg SP:', JSON.stringify({ stream, group, consumer }), err2);
                    });
                } catch (error) {
                  console.error('Error audioMsg SP:', error);
                  logger.error('Error audioMsg SP:', JSON.stringify({ stream, group, consumer }), error);
                }
              });
            });
          }
          readAudioMessages(stream, group, consumer);
        }
      }
    );
  } catch (error) {
    console.log('readAudioMessages audiocc queue:', error);
    logger.error('readAudioMessages audiocc queue:', JSON.stringify({ stream, group, consumer }), error);
  }
}

function createAudioGroup() {
  try {
    audioClient.xgroup('CREATE', process.env.CALL_STREAM_MESSAGE_NAME, process.env.CALL_CONSUMER_MESSAGE_GROUP, '0', 'MKSTREAM', (err) => {
      if (err && !err.message.includes('BUSYGROUP Consumer Group name already exists')) {
        console.error('Error creating consumer group audio queue:', err);
        logger.error('Error creating consumer group audio queue:', process.env.CALL_STREAM_MESSAGE_NAME, process.env.CALL_CONSUMER_MESSAGE_GROUP, err);
        deleteAudioConsumer();
      } else {
        console.log('START READING AUDIO-CC MESSAGE::::::');
        readAudioMessages(process.env.CALL_STREAM_MESSAGE_NAME, process.env.CALL_CONSUMER_MESSAGE_GROUP, process.env.CALL_CONSUMER_MESSAGE_NAME);
      }
    });
  } catch (error) {
    logger.error('createAudioGroup audiocc queue', process.env.CALL_STREAM_MESSAGE_NAME, process.env.CALL_CONSUMER_MESSAGE_GROUP, error);
  }
}

/***************************************************agent assist stream************************************************************/
agentAssistClient.xtrim(process.env.AGENT_ASSIST_REDIS_CHANNEL_RES, 'MAXLEN', 0)
  .then(() => {
    console.log(`Stream ${process.env.AGENT_ASSIST_REDIS_CHANNEL_RES} has been deleted.`);
    createAgentAssistGroup();
  })
  .catch((err) => {
    console.error('Error deleting the stream agent assist:', err);
    logger.error('Error deleting the stream agent assist:', err);
  })
  .finally(() => {
    // client.quit();
  });

function deleteAgentAssistGroup() {
  agentAssistClient.xgroup('DESTROY', process.env.AGENT_ASSIST_REDIS_CHANNEL_RES, process.env.AGENT_ASSIST_CONSUMER_GROUP)
    .then((response) => {
      console.log(`Stream "${process.env.AGENT_ASSIST_REDIS_CHANNEL_RES}" and consumer group "${process.env.AGENT_ASSIST_CONSUMER_GROUP}" destroyed.`);
      createAgentAssistGroup();

    })
    .catch((err) => {
      console.error('Error destroying stream and consumer group agent assist:', err);
      logger.error('Error destroying stream and consumer group agent assist:', err);
      createAgentAssistGroup();
    });
}

function deleteAgentAssistConsumer() {
  agentAssistClient.xgroup('DELCONSUMER', process.env.AGENT_ASSIST_REDIS_CHANNEL_RES, process.env.AGENT_ASSIST_CONSUMER_GROUP, process.env.AGENT_ASSIST_CONSUMER_NAME)
    .then((response) => {
      if (response === 1) {
        console.log(`Consumer "${process.env.AGENT_ASSIST_CONSUMER_NAME}" removed from group "${process.env.AGENT_ASSIST_CONSUMER_GROUP}"`);

        deleteAgentAssistGroup();
      } else {
        console.log(`Consumer "${process.env.AGENT_ASSIST_CONSUMER_NAME}" not found in group "${process.env.AGENT_ASSIST_CONSUMER_GROUP}"`);
        deleteAgentAssistGroup();
      }
    })
    .catch((err) => {
      console.error('Error removing consumer agent assist:', err);
      logger.error('Error removing consumer agent assist:', err);
      deleteAgentAssistGroup();
    });
}

function acknowledgeAgentAssistMessage(stream: any, group: any, messageId: any, messageData: any) {
  try {
    agentAssistClient.xack(stream, group, messageId, (err, result) => {
      if (err) {
        console.error('Error Androidacknowledging message agent assist:', err);
        logger.error('Error Androidacknowledging message agent assist:', JSON.stringify({ stream, group, messageId, messageData }), err);
      } else if (result === 1) {

        console.log(`${stream}_${group}  Acknowledged message ${messageId}_ACK`);

      } else {
        console.log(`${stream}_${group}  Message ${messageData?.iuid}_${messageId}_ACK not found in consumer group`);
      }
    });
  } catch (error) {
    logger.error('acknowledgeAgentAssistMessage agent assist', JSON.stringify({ stream, group, messageId, messageData }), error);
  }
}

function readAgentAssistMessages(stream: any, group: any, consumer: any) {
  try {
    agentAssistClient.xreadgroup(
      'GROUP', group, consumer, 'BLOCK', 0, 'STREAMS', stream, '>', (err, streams) => {
        if (err) {
          console.error('Error reading messages agent assist:', err);
          logger.error('Error reading messages agent assist:', JSON.stringify({ stream, group, consumer }), err);
        } else {
          if (streams) {
            streams.forEach((entry: any) => {
              const [streamName, messages] = entry;
              messages.forEach(async ([messageId, messageData]) => {
                console.log(`AGENT ASSIST MESSAGE::::: ${stream}_${group} Message ID: ${messageId}, Message Data: ${messageData}`);
                const rawMessage = `${messageData}`.split('agent_assist_data,');
                acknowledgeAgentAssistMessage(stream, group, messageId, JSON.parse(rawMessage?.[1] ?? 'null'));
                agentAssistMsg(JSON.parse(rawMessage?.[1] ?? 'null'));
              });
            });
          }
          readAgentAssistMessages(stream, group, consumer);
        }
      }
    );
  } catch (error) {
    logger.error('readAgentAssistMessages agent assist', JSON.stringify({ stream, group, consumer }), error);
  }
}

function createAgentAssistGroup() {
  try {
    agentAssistClient.xgroup('CREATE', process.env.AGENT_ASSIST_REDIS_CHANNEL_RES, process.env.AGENT_ASSIST_CONSUMER_GROUP, '0', 'MKSTREAM', (err) => {
      if (err && !err.message.includes('BUSYGROUP Consumer Group name already exists')) {
        console.error('Error creating consumer group agent assist:', err);
        logger.error('Error creating consumer group agent assist:', process.env.AGENT_ASSIST_REDIS_CHANNEL_RES, process.env.AGENT_ASSIST_CONSUMER_GROUP, err);
        deleteAgentAssistConsumer();
      } else {
        console.log('START READING AGENT ASSIST MESSAGE::::::');
        readAgentAssistMessages(process.env.AGENT_ASSIST_REDIS_CHANNEL_RES, process.env.AGENT_ASSIST_CONSUMER_GROUP, process.env.AGENT_ASSIST_CONSUMER_NAME);
      }
    });
  } catch (error) {
    logger.error('createAgentAssistGroup agent assist', process.env.AGENT_ASSIST_REDIS_CHANNEL_RES, process.env.AGENT_ASSIST_CONSUMER_GROUP, error);
  }
}
