import _ from 'lodash';
import { ioredisChat, pubsubRedisClient, redisClient } from '../../src/plugins/db';
import { RedisConstAdapters } from '../callflow';
import { updateAgentSessionId } from '../dao/callflow.dao';
import { insertNotificationCallBlastDao, insertNotificationVoiceMailDao } from '../dao/notification.dao';
import { loggerError } from '../plugins/logger';

export class RedisAdapters {
  public static sessionInstances: any;
  private ioredis: any;
  private sub: any;
  private streamclient: any;
  constructor(dotenv?: any) {
    try {
      this.sub = pubsubRedisClient;
      this.ioredis = ioredisChat;
      const response = redisClient.duplicate();
      this.streamclient = redisClient.duplicate();
      RedisAdapters.sessionInstances = {} as any;

      /* tslint:disable */
      const self: any = this;

      response.subscribe(process.env.CALL_STREAM_NAME);
      response.on('message', (_: any, msg: any) => {
        console.log('SUBSCRIBE DATA::::::::', msg);
        self.handleSessionBasedInstances(msg);
      });

      this.streamclient.xtrim(process.env.CALL_STREAM_NAME, 'MAXLEN', 0)
        .then(() => {
          console.log(`Stream ${process.env.CALL_STREAM_NAME} has been deleted.`);
          self.createGroup();
        })
        .catch((err) => {
          console.error('Error deleting the stream:', err);
        })
        .finally(() => {
        });
    } catch (error) {
      loggerError('RedisAdapters', {}, error)
    }
  }

  public deleteGroup() {
    try {
      const self = this;
      this.streamclient.xgroup('DESTROY', process.env.CALL_STREAM_NAME, process.env.CALL_CONSUMER_GROUP)
        .then((response) => {
          console.log(`Stream "${process.env.CALL_STREAM_NAME}" and consumer group "${process.env.CALL_CONSUMER_GROUP}" destroyed.`);
          self.createGroup();

        })
        .catch((err) => {
          console.error('Error destroying stream and consumer group:', err);
          self.createGroup();
        });
    } catch (error) {
      loggerError('deleteGroup', {}, error)
    }
  }

  public deleteConsumer() {
    try {
      const self = this;
      this.streamclient.xgroup('DELCONSUMER', process.env.CALL_STREAM_NAME, process.env.CALL_CONSUMER_GROUP, process.env.CALL_CONSUMER_NAME)
        .then((response) => {
          if (response === 1) {
            console.log(`Consumer "${process.env.CALL_CONSUMER_NAME}" removed from group "${process.env.CALL_CONSUMER_GROUP}"`);

            self.deleteGroup();
          } else {
            console.log(`Consumer "${process.env.CALL_CONSUMER_NAME}" not found in group "${process.env.CALL_CONSUMER_GROUP}"`);
            self.deleteGroup();
          }
        })
        .catch((err) => {
          console.error('Error removing consumer:', err);
          self.deleteGroup();
        });
    } catch (error) {
      loggerError('deleteConsumer', {}, error)
    }
  }

  public acknowledgeMessage(stream: any, group: any, messageId: any, messageData: any) {
    try {
      this.streamclient.xack(stream, group, messageId, (err, result) => {
        if (err) {
          console.error('Error Androidacknowledging message:', err);
        } else if (result === 1) {

          console.log(`${stream}_${group}  Acknowledged message ${messageData.session_id}_${messageId}_ACK`);

        } else {
          console.log(`${stream}_${group}  Message ${messageData.session_id}_${messageId}_ACK not found in consumer group`);
        }
      });
    } catch (error) {
      loggerError('acknowledgeMessage', {}, error)
    }
  }

  public readMessages(stream: any, group: any, consumer: any) {
    try {
      const self = this;
      this.streamclient.xreadgroup(
        'GROUP', group, consumer, 'BLOCK', 0, 'STREAMS', stream, '>', (err, streams) => {
          if (err) {
            console.error('Error reading messages:', err);
          } else {
            if (streams) {
              streams.forEach((entry: any) => {
                const [streamName, messages] = entry;
                messages.forEach(([messageId, messageData]) => {
                  console.log(`READ MESSAGE::::: ${stream}_${group} Message ID: ${messageId}, Message Data: ${messageData}`);
                  const rawMessage = `${messageData}`.split('message,');
                  self.acknowledgeMessage(stream, group, messageId, JSON.parse(rawMessage?.[1] ?? "null"));
                  self.handleSessionBasedInstances(rawMessage?.[1] ?? "null");
                });
              });
            }
            self.readMessages(stream, group, consumer);
          }
        }
      );
    } catch (error) {
      loggerError('readMessages', {}, error)
    }
  }

  public createGroup() {
    try {
      const self = this;
      this.streamclient.xgroup('CREATE', process.env.CALL_STREAM_NAME, process.env.CALL_CONSUMER_GROUP, '0', 'MKSTREAM', (err) => {
        if (err && !err.message.includes('BUSYGROUP Consumer Group name already exists')) {
          console.error('Error creating consumer group:', err);
          self.deleteConsumer();
        } else {
          console.log('START READING MESSAGE::::::');
          self.readMessages(process.env.CALL_STREAM_NAME, process.env.CALL_CONSUMER_GROUP, process.env.CALL_CONSUMER_NAME);
        }
      });
    } catch (error) {
      loggerError('createGroup', {}, error);
    }
  }

  public handleSessionBasedInstances(data: any) {
    try {
      const parsedData = JSON.parse(data ?? 'null');
      console.log('PARSE DATA::::::::', parsedData);

      if (parsedData?.req_type === 'CALLBACK') {
        this.ioredis.to(`${parsedData.domain_id}_${parsedData.ext}`).emit('callback', parsedData);
        return;
      }

      if (parsedData?.req_type === 'CAMPAIGN_DIALOUT') {
        console.log('CAMPAIGN_DIALOUT', data);
        this.ioredis.to(`${parsedData.domain_id}_${parsedData.ext}`).emit('CAMPAIGN_DIALOUT', parsedData);
        return;
      }

      if (parsedData?.req_type === 'AGENT_REGISTER' && parsedData?.pd === 1) {
        this.ioredis.to(`${parsedData?.domain_id}_${parsedData?.ext}`).emit('AGENT_REGISTER', parsedData);
        return;
      }

      if (parsedData?.call_state === 'init') {
        const newObject = new RedisConstAdapters(this);
        RedisAdapters.sessionInstances[parsedData?.session_id] = newObject;
        const exit = newObject?.callflowhandler(data);
      } else if (
        parsedData?.disconnectedBy &&
        (
          parsedData?.status === 'disconnected' ||
          parsedData?.status === 'vms_disconnected' ||
          parsedData?.acd_event === 10 ||
          parsedData?.acd_event === 17
        )) {
        const exit = RedisAdapters.sessionInstances[parsedData?.session_id]?.callflowhandler(data);
        RedisAdapters.sessionInstances[parsedData?.session_id] = null;
        delete RedisAdapters.sessionInstances[parsedData?.session_id];
      } else {
        const exit = RedisAdapters.sessionInstances[parsedData?.session_id]?.callflowhandler(data);
      }
    } catch (error) {
      loggerError('handleSessionBasedInstances', data, error);
    }
  }

  public emitMessage(to: any, event: any, message: any) {
    try {
      console.log('EMIT MESSAGE::::::::', to, event, message);
      this.ioredis.to(`${to.toString()}`).emit(event, message);
    } catch (error) {
      loggerError('emitMessage', { to, event, message }, error);
    }
  }

  public async publishMessage(key: any) {
    try {
      console.log('PUBLISH MESSAGE ::::::::::', JSON.parse(key));
      this.sub.publish(process.env.RESPONSECHANNEL, key);
    } catch (error) {
      loggerError('publishMessage', key, error);
    }
  }
}
