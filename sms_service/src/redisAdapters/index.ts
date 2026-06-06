import _ from 'lodash';
import { ioredisChat, otherRedisClient, pubsubRedisClient, redisClient, subscriberEmitter } from '../plugins/redis';

const RedisAdapter = require('socket.io-redis');

export class RedisAdapters {
  public static sessionInstances: any;
  private adaptor: any;
  private sub: any;
  private pub: any;
  private dub: any;
  private authpub: any;
  private ioredis: any;
  private socketadapter: any;

  constructor(socketadapter?: any) {
    this.pub = redisClient;
    this.sub = pubsubRedisClient;
    this.dub = otherRedisClient;
    this.ioredis = ioredisChat;

    const client = redisClient.duplicate();

    RedisAdapters.sessionInstances = {} as any;

    this.adaptor = RedisAdapter({ pubClient: this.dub, subClient: subscriberEmitter });
    this.socketadapter = socketadapter;
    // this.BLPOPPER();

    // this.BLPOP();
    /* tslint:disable */
  }

  public push(channel: any, msg: any): void {
    this.dub.rpush(channel, JSON.stringify({ msg }));
  }

  public presencePushToUmn(channel: any, msg: any): void {
    this.pub.rpush(channel, JSON.stringify(msg));
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
      this.sub.get(key).then((result) => {
        resolve(result);
      });
    });
  }

  public publishMessageForAgent(key: any) {
    console.log(key);

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

  public getValuefromredis(key: any) {
    return new Promise((resolve, reject) => {
      this.authpub.get(key, (err: any, data: any) => {
        resolve(data);
      });
    });
  }
}