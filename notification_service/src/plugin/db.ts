import { Emitter } from '@socket.io/redis-emitter';
import Redis from 'ioredis';
import mysql from 'mysql';
import { configs } from '../config/app';
import { IOSANDROIDCALL, sendAndPushTest, sendIosPushTest } from '../notification';

// MYSQL CONNECTION
export const mysqlconnection = mysql.createPool(configs.sqlCcaasDB);

// REDIS CONNECTION
export const pubsubRedisClient = new Redis({ host: process.env.PUBSUB_REDIS_INSTANCE, port: 6379, password: process.env.PUBSUB_REDIS_PASSWORD });
export const store = pubsubRedisClient.duplicate();
const client = pubsubRedisClient.duplicate();
export const otherRedisClient = new Redis({ host: process.env.SOCKET_REDIS_INSTANCE, port: 6379, password: process.env.SOCKET_REDIS_PASSWORD });
export const ioredisChat = new Emitter(otherRedisClient);
export const tokenRedisStore = new Redis({ host: process.env.TOKEN_REDIS_STORE_IP, password: process.env.TOKEN_REDIS_STORE_PASSWORD, port: 6379 });

pubsubRedisClient.on('connect', (res: any) => {
  console.log('Connected to Redis');
  // sendIosPushTest('', '', 'connected');
  // sendAndPushTest()
});
pubsubRedisClient.on('error', (err: any) => console.error('Redis error:', err));

// (function CCAAS() {
//   client.blpop('CCAAS_PN_LIVE', 60000).then((data: any) => {
//     try {
//       console.log("response data:", data)
//       if (data !== null || data !== undefined) {
//         IOSANDROIDCALL(JSON.parse(data[1] ?? null));
//         CCAAS();
//       } else {
//         CCAAS();
//       }
//     } catch (error) {
//       console.log('error :', error);
//       CCAAS();
//     }
//   });
// })();

const fcmNotificationSubscriber = otherRedisClient.duplicate();

fcmNotificationSubscriber.subscribe('webChatConnected_mob_FCM', (err, count) => {
  if (err) {
    console.error('Failed to subscribe:', err);
  } else {
    console.log(`Subscribed successfully to ${count} channels.`);
  }
});

fcmNotificationSubscriber.on('message', (channel, message) => {
  console.log(`Received on ${channel}: ${message}`);
  IOSANDROIDCALL(JSON.parse(message));
});

// Error Handling
pubsubRedisClient.on('error', err => console.error('❌ Redis error:', err));
fcmNotificationSubscriber.on('error', err => console.error('Subscriber Redis error:', err));

client.xtrim(process.env.CALL_STREAM_NAME, 'MAXLEN', 0)
  .then(() => {
    console.log(`Stream ${process.env.CALL_STREAM_NAME} has been deleted.`);
    // deleteConsumer();
    createGroup();
  })
  .catch((err) => {
    console.error('Error deleting the stream:', err);
  })
  .finally(() => {
    // client.quit();
  });

function deleteGroup() {
  client.xgroup('DESTROY', process.env.CALL_STREAM_NAME, process.env.CALL_CONSUMER_GROUP)
    .then((response) => {
      console.log(`Stream "${process.env.CALL_STREAM_NAME}" and consumer group "${process.env.CALL_CONSUMER_GROUP}" destroyed.`);
      createGroup();

    })
    .catch((err) => {
      console.error('Error destroying stream and consumer group:', err);
      createGroup();
    });
}

function deleteConsumer() {
  client.xgroup('DELCONSUMER', process.env.CALL_STREAM_NAME, process.env.CALL_CONSUMER_GROUP, process.env.CALL_CONSUMER_NAME)
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
      deleteGroup();
    });
}

function acknowledgeMessage(stream: any, group: any, messageId: any, messageData: any) {
  client.xack(stream, group, messageId, (err, result) => {
    if (err) {
      console.error('Error Androidacknowledging message:', err);
    } else if (result === 1) {

      console.log(`${stream}_${group}  Acknowledged message ${messageId}_ACK`);

    } else {
      console.log(`${stream}_${group}  Message ${messageData?.iuid}_${messageId}_ACK not found in consumer group`);
    }
  });
}

function readMessages(stream: any, group: any, consumer: any) {
  client.xreadgroup(
    'GROUP', group, consumer, 'BLOCK', 0, 'STREAMS', stream, '>', (err, streams) => {
      if (err) {
        console.error('Error reading messages:', err);
        // redis.quit();
      } else {
        if (streams) {
          streams.forEach((entry: any) => {
            const [streamName, messages] = entry;
            messages.forEach(([messageId, messageData]) => {
              console.log(`READ MESSAGE::::: ${stream}_${group} Message ID: ${messageId}, Message Data: ${messageData}`);
              const rawMessage = `${messageData}`.split('message,');
              acknowledgeMessage(stream, group, messageId, JSON.parse(rawMessage?.[1] ?? 'null'));
              IOSANDROIDCALL(JSON.parse(rawMessage?.[1] ?? 'null'));
            });
          });
        }
        readMessages(stream, group, consumer);
      }
    }
  );
}

function createGroup() {
  client.xgroup('CREATE', process.env.CALL_STREAM_NAME, process.env.CALL_CONSUMER_GROUP, '0', 'MKSTREAM', (err) => {
    if (err && !err.message.includes('BUSYGROUP Consumer Group name already exists')) {
      console.error('Error creating consumer group:', err);
      deleteConsumer();
    } else {
      console.log('START READING MESSAGE::::::');
      readMessages(process.env.CALL_STREAM_NAME, process.env.CALL_CONSUMER_GROUP, process.env.CALL_CONSUMER_NAME);
    }
  });
}
