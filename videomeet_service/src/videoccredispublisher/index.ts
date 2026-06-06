/**
 * @createdBy Selvakani
 * @createdOn 27th April 2021
 */

import _ from "underscore";
import { configuration } from "../config";


//const Redis = require("ioredis");
const redis = require("redis");

export class videoCCRedisConnection {
  publisher: any;
  constructor() {
    let redisConfig: any = {
      host: configuration.redis_client.host,
      port: configuration.redis_client.port,
    };


    redisConfig.no_ready_check = configuration.redis_client.no_ready_check;
    redisConfig.auth_pass = configuration.redis_client.auth_pass;


    console.log("Video CC Redis IP>>>>>", redisConfig);
    this.publisher = redis.createClient(redisConfig);
  }

  public async videoccpublish(channel: any, data: any) {
    this.publisher.publish(channel, data);
  }
}
