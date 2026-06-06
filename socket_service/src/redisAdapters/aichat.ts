import { splitFirst } from '../helpers/constants';
import { NLPRedisClient } from '../plugins/db';

export class Aichat {
  private socketadapter: any;
  private streamclient: any;

  constructor(socketadapter?: any) {
    // const self: any = this;
    this.socketadapter = socketadapter;
    this.streamclient = NLPRedisClient.duplicate();
    console.log('TRIM STREAM NAME:', process.env.AI_CHAT_STREAM_NAME_RES);
    this.streamclient.xtrim(process.env.AI_CHAT_STREAM_NAME_RES, 'MAXLEN', 0)
      .then(() => {
        console.log(`Stream ${process.env.AI_CHAT_STREAM_NAME_RES} has been deleted.`);
        this.createGroup();
      })
      .catch((err) => {
        console.error('Error deleting the stream:', err);
      });
  }

  public deleteGroup() {
    // const self = this;
    this.streamclient.xgroup('DESTROY', process.env.AI_CHAT_STREAM_NAME_RES, process.env.AI_CHAT_CONSUMER_GROUP)
      .then((response) => {
        console.log(`Stream "${process.env.AI_CHAT_STREAM_NAME_RES}" and consumer group "${process.env.AI_CHAT_CONSUMER_GROUP}" destroyed.`);
        this.createGroup();

      })
      .catch((err) => {
        console.error('Error destroying stream and consumer group:', err);
        this.createGroup();
      });
  }

  public deleteConsumer() {
    // const self = this;
    this.streamclient.xgroup('DELCONSUMER', process.env.AI_CHAT_STREAM_NAME_RES, process.env.AI_CHAT_CONSUMER_GROUP, process.env.AI_CHAT_CONSUMER_NAME)
      .then((response) => {
        if (response === 1) {
          console.log(`Consumer "${process.env.AI_CHAT_CONSUMER_NAME}" removed from group "${process.env.AI_CHAT_CONSUMER_GROUP}"`);

          this.deleteGroup();
        } else {
          console.log(`Consumer "${process.env.AI_CHAT_CONSUMER_NAME}" not found in group "${process.env.AI_CHAT_CONSUMER_GROUP}"`);
          this.deleteGroup();
        }
      })
      .catch((err) => {
        console.error('Error removing consumer:', err);
        this.deleteGroup();
      });
  }

  public acknowledgeMessage(stream: any, group: any, messageId: any, messageData: any) {
    this.streamclient.xack(stream, group, messageId, (err, result) => {
      if (err) {
        console.error('Error Androidacknowledging message:', err);
      } else if (result === 1) {

        console.log(`${stream}_${group} Ai Chat Acknowledged message ${messageData?.session_id ?? ''}_${messageId}_ACK`);

      } else {
        console.log(`${stream}_${group} Ai Chat Message ${messageData?.session_id ?? ''}_${messageId}_ACK not found in consumer group`);
      }
    });
  }

  public readMessages(stream: any, group: any, consumer: any) {
    console.log(stream, group, consumer);
    // const self = this;
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
                if (messageData) {
                  // const rawMessage = `${messageData}`.split('message,');
                  const rawMessage = splitFirst(`${messageData}`, 'message,');
                  this.acknowledgeMessage(stream, group, messageId, JSON.parse(rawMessage[1]));
                  this.socketadapter.receiveMessageFromAIChatResponse(messageData[1]);
                } else {
                  console.log('Ai Chat NO MSG FOUND :::  Error  >>>>> >>>> ', messageId, messageData);
                }
              });
            });
          }
          this.readMessages(stream, group, consumer);
        }
      }
    );
  }

  public createGroup() {
    // const self = this;
    this.streamclient.xgroup('CREATE', process.env.AI_CHAT_STREAM_NAME_RES, process.env.AI_CHAT_CONSUMER_GROUP, '0', 'MKSTREAM', (err) => {
      if (err && !err.message.includes('BUSYGROUP Consumer Group name already exists')) {
        console.error('Error creating consumer group:', err);
        this.deleteConsumer();
      } else {
        console.log('START READING MESSAGE::::::');
        this.readMessages(process.env.AI_CHAT_STREAM_NAME_RES, process.env.AI_CHAT_CONSUMER_GROUP, process.env.AI_CHAT_CONSUMER_NAME);
      }
    });
  }
}
