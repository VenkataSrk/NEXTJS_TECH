import { ioredisChat, otherRedisClient, pubsubRedisClient, redisClient, subscriberEmitter } from '../plugins/db';
import { logger } from '../plugins/log';
const RedisAdapter = require('socket.io-redis');

export class RedisAdapters {
  public static sessionInstances: any;
  private adaptor: any;
  private sub: any;
  private pub: any;
  private dub: any;
  private ioredis: any;

  constructor() {

    this.pub = redisClient;
    this.sub = pubsubRedisClient;
    this.dub = otherRedisClient;
    this.ioredis = ioredisChat;

    RedisAdapters.sessionInstances = {} as any;

    this.adaptor = RedisAdapter({ pubClient: this.dub, subClient: subscriberEmitter });
    // this.BLPOPPER();
  }

  public push(channel: any, msg: any): void {
    this.dub.rpush(channel, JSON.stringify({ msg }));
  }

  public createSet(key: any, data: any) {
    this.dub.sadd(key, data);
  }

  public setVariableValue(name: any, keyName: any, keyValue: any) {
    this.dub.hset(name, keyName, keyValue);
  }

  public getapiData() {
    return new Promise((resolve, reject) => {
      try {
        this.dub.hgetall('apiValues', (err, results) => {
          if (results) {
            resolve(results);
          } else {
            reject(err);
          }
        });
      } catch (error) {
        reject(error);
      }
    });
  }

  public getValuefromSet(key: any) {
    return new Promise((resolve, reject) => {
      this.dub.smembers(key, (err, data) => {
        resolve(data);
      });
    });
  }

  public getFlow(key: any) {
    return new Promise((resolve, reject) => {
      this.sub.get(key).then((result: any) => {
        resolve(result);
      });
    });
  }

  public publishMessage(key: any) {
    logger.info(key);
    this.dub.publish(process.env.RESPONSECHANNEL, key);
    // this.BLPOP();
  }
  public publishMessageChat(key: any) {
    this.dub.rpush(process.env.REQUESTCHANNEL, key);
    // this.BLPOP();
  }
  public getredisAdapter(): any {
    return this.adaptor;
  }

  public removeValuebyKey(key: any) {
    return new Promise((resolve, reject) => {
      this.dub.del(key, (err: any, reply: any) => {
        resolve(1);
      });
    });
  }
}
