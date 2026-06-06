/**
 * @createdBy Selvakani
 * @createdOn 27th April 2021
 */

import { RedisClient } from "redis";
import { configuration } from "../config";
import { createAdapter, RedisAdapter } from "socket.io-redis";
import _, { any } from "underscore";
import { MeetingService } from "../meetingService";
import { EXDEV } from "constants";
var redis = require("redis");
export class redisAdapters {
  private adaptor: RedisAdapter;
  private pub: RedisClient;
  private sub: RedisClient;


  constructor(iq: any) {
    let redisConfig: any = {
      host: '10.150.0.116',//configuration.redis_client.host,
      port: 6379//configuration.redis_client.port,
    };

    //if (process.env.NODE_ENV === "staging") {
    redisConfig.no_ready_check = true//configuration.redis_client.no_ready_check;
    redisConfig.auth_pass = 'venchat23423sdnj116'//configuration.redis_client.auth_pass;
    // }

    const pubClient = new RedisClient(redisConfig);

    const subscriber = redis.createClient(redisConfig);

    let subClient1 = subscriber.duplicate();

    subscriber.on("message", async (channel: any, message: any) => {

      console.log("message >>>>>>>", message);
      //await iq.rocketMQConnect(message);

    });

    // const subscriber1 = redis.createClient(redisConfig);

    // subscriber1.on("message", async (channel:any,message: any) => {
    //    console.log("messageee",message);
    //   //  await iq.broadcastClosecaption(message);

    // });

    const subscriber2 = redis.createClient(redisConfig);

    subscriber2.on("message", async (channel: any, message: any) => {
      console.log("messageee>>>", message);
      await iq.broadcastCC(message);
      //  await iq.broadcastTranscript
    });


    (function subscribeblpop() {
      subClient1.blpop('rocketmqresponse_1', 0, function (err: any, data: any) {
        console.log(data[1]);

        subscribeblpop();
      });
    })();

    /*     subClient1.blpop('rocketmqresponse_1', 0, function (err: any, data: any) {
          console.log(' blpop: ' + data[1]);
          //setTimeout(function() {
          //      addWorker();
          //}, 0);
        }); */
    subscriber.subscribe("rocketmqresponse_8");
    // subscriber1.subscribe("dont_publish");
    subscriber2.subscribe("translate");
    pubClient.on("error", (err: any) => console.log("Error" + err));
    pubClient.on("message", (err: any) => console.log("message" + err));
    pubClient.on("message_buffer", (err: any) => console.log("message_buffer" + err));
    pubClient.on("pmessage", (err: any) => console.log("pmessage" + err));
    pubClient.on("subscribe", (err: any) => console.log("subscribe" + err));

    pubClient.on("psubscribe", (err: any) => console.log("psubscribe" + err));
    const subClient = pubClient.duplicate();
    this.sub = subClient;
    this.pub = pubClient;
    this.adaptor = createAdapter({ pubClient, subClient });

    /*     let that = this;
        setInterval(() => {
          pubClient.publish("socketiorequest_1", JSON.stringify({ "name": "selvamano" }));
        }, 3000) */
  }

  public push(meetingId: any, msg: any): void {
    this.pub.rpush(meetingId, msg.toString());
  }

  public createSet(key: any, data: any) {
    this.pub.sadd(key, data);
  }

  public getValuefromSet(key: any) {
    return new Promise((resolve, reject) => {
      this.pub.smembers(key, (err, data) => {
        resolve(data);
      });
    });
  }

  public async setMeetingDetails(meetingId: any, meetingDetails: any) {
    this.pub.set(meetingId, JSON.stringify(meetingDetails), "EX", 86400);
  }

  public removeValuebyKey(key: any) {
    return new Promise((resv, rej) => {
      this.pub.del(key, (err, reply) => {
        resv(1);
      });
    });
  }
  public getMeetingDetails(meetingId: any, jwtToken: any) {
    return new Promise((resolve, reject) => {
      try {
        this.pub.get(`${meetingId}_socket`, async (err, data) => {
          if (err || data == null || data == "{}") {
            const meetingSetting: any = await MeetingService.getMeetingbyId(meetingId, jwtToken).then((x) => {
              if (x) {
                this.setMeetingDetails(meetingId, x);
                resolve(x);
              } else {
                reject("user3 not found");
              }
            });
          } else {
            if (data != null) {
              const retData = JSON.parse(data);
              resolve(retData);
            }
          }
        });
      } catch (e) {
        reject(e);
      }
    });
  }

  public getMeetingInfofromRedis(meetingId: any) {
    return new Promise((resolve, reject) => {
      try {
        this.pub.get(meetingId, async (err, data) => {
          if (err || data == null || data == "{}") {
            /*   const meetingSetting: any = await MeetingService.getMeetingbyId(
                meetingId,
                jwtToken
              ).then((x) => {
                if (x) {
                  this.setMeetingDetails(meetingId, x);
                  resolve(x);
                } else {
                  reject("user3 not found");
                }
              }); */
          } else {
            if (data != null) {
              const retData = JSON.parse(data);
              resolve(retData);
            }
          }
        });
      } catch (e) {
        reject(e);
      }
    });
  }

  public addParticipantstoMeeting(meetingId: any, jwtToken: any, participant: any) {
    return new Promise((resolve, reject) => {
      try {
        this.pub.get(meetingId, async (err, data) => {
          /* if(err || data == null || data == '{}' ) { */
          const meetingSetting: any = await MeetingService.addParticipant(meetingId, jwtToken, participant).then((x) => {
            if (x) {
              // this.setMeetingDetails(meetingId, x);
              resolve(x);
            } else {
              reject("user6 not found");
            }
          });
          /*  }
            else{
                if(data != null) { 
                   const retData = JSON.parse(data);
                   resolve(retData);
                }
            } */
        });
      } catch (e) {
        reject(e);
      }
    });
  }

  public storemeetNow(meetingDetails: any, jwtToken: any) {
    console.log("meetingDetails >>>>>>>>>", meetingDetails);

    return new Promise(async (resolve, reject) => {
      try {
        const meetingSetting: any = await MeetingService.store_meetNow(meetingDetails, jwtToken).then((x) => {
          if (x) {
            // this.setMeetingDetails(meetingId, x);
            resolve(x);
          } else {
            reject("user7 not found");
          }
        });
      } catch (e) {
        reject(e);
      }
    });
  }

  public endMeeting(meetingDetails: any, jwtToken: any) {
    console.log("meetingDetails >>>>>>>>>", meetingDetails);

    return new Promise(async (resolve, reject) => {
      try {
        const meetingSetting: any = await MeetingService.endMeeting(meetingDetails, jwtToken).then((x) => {
          if (x) {
            // this.setMeetingDetails(meetingId, x);
            resolve(x);
          } else {
            reject("user8 not found");
          }
        });
      } catch (e) {
        reject(e);
      }
    });
  }

  public joinMeeting(meetingDetails: any, jwtToken: any) {
    console.log("meetingDetails >>>>>>>>>", meetingDetails);

    return new Promise(async (resolve, reject) => {
      try {
        const meetingSetting: any = await MeetingService.joinMeeting(meetingDetails, jwtToken).then((x) => {
          if (x) {
            // this.setMeetingDetails(meetingId, x);
            resolve(x);
          } else {
            reject("user9 not found");
          }
        });
      } catch (e) {
        reject(e);
      }
    });
  }

  public getHistory(meetingId: string): any {
    const retPromise = new Promise((resolve, reject) => {
      this.pub.lrange(meetingId, 0, -1, (err, data) => {
        if (err) {
          reject(err);
        } else {
          console.log(data);
          resolve(_.values(data));
        }
      });
    });
    return retPromise;
  }
  public updateGeneralSetting(meetingId: any, jwtToken: any, data: any) {
    return new Promise(async (resolve, reject) => {
      try {
        const meetingSetting: any = await MeetingService.updateGeneralSetting(meetingId, jwtToken, data).then((x) => {
          if (x) {
            //this.setMeetingDetails(meetingId, x);
            resolve(x);
          } else {
            reject("user2 not found");
          }
        });
      } catch (e) {
        reject(e);
      }
    });
  }

  public getMeetingFind(meetingId: any, jwtToken: any) {
    return new Promise(async (resolve, reject) => {
      try {
        //this.pub.get(meetingId, async (err, data) => {
        const meetingSetting: any = await MeetingService.getMeeting(meetingId, jwtToken).then((x) => {
          if (x) {
            resolve(x);
          } else {
            reject("user3 not found");
          }
        });
        // });
      } catch (e) {
        reject(e);
      }
    });
  }

  public hostJoiningStatus(meetingId: any, jwtToken: any) {
    return new Promise(async (resolve, reject) => {
      try {

        const meetingSetting: any = await MeetingService.hostJoining(meetingId, jwtToken).then((x) => {
          if (x) {
            resolve(x);
          } else {
            reject("Reject hostJoiningStatus");
          }
        });
      } catch (e) {
        reject(e);
      }
    });
  }

  public participantJoiningStatus(meetingId: any, jwtToken: any) {
    return new Promise(async (resolve, reject) => {
      try {

        const meetingSetting: any = await MeetingService.participantJoining(meetingId, jwtToken).then((x) => {
          if (x) {
            resolve(x);
          } else {
            reject("Reject hostJoiningStatus");
          }
        });
      } catch (e) {
        reject(e);
      }
    });
  }

  public updateMeeting(meetingId: any, jwtToken: any, data: any) {
    return new Promise(async (resolve, reject) => {
      try {
        const meetingSetting: any = await MeetingService.updateMeeting(meetingId, jwtToken, data).then((x) => {
          if (x) {
            //this.setMeetingDetails(meetingId, x);
            resolve(x);
          } else {
            reject("user2 not found");
          }
        });
      } catch (e) {
        reject(e);
      }
    });
  }

  public updateWhiteBoard(meetingId: any, jwtToken: any, data: any) {
    return new Promise(async (resolve, reject) => {
      try {
        const meetingSetting: any = await MeetingService.updateWhiteBoard(meetingId, jwtToken, data).then((x) => {
          if (x) {
            //this.setMeetingDetails(meetingId, x);
            resolve(x);
          } else {
            reject("user not found");
          }
        });
      } catch (e) {
        reject(e);
      }
    });
  }
  public getWhiteBoardFind(meetingId: any, jwtToken: any) {
    return new Promise((resolve, reject) => {
      try {
        this.pub.get(meetingId, async (err, data) => {
          const meetingSetting: any = await MeetingService.getWhiteBoard(meetingId, jwtToken).then((x) => {
            if (x) {
              resolve(x);
            } else {
              reject("user not found");
            }
          });
        });
      } catch (e) {
        reject(e);
      }
    });
  }

  public getUserData(token: any) {
    return new Promise(async (resolve, reject) => {
      try {
        const meetingSetting: any = await MeetingService.getUserData(token).then((x) => {
          if (x) {
            resolve(x);
          } else {
            reject("getuserdata failed");
          }
        });
      } catch (e) {
        reject(e);
      }
    });
  }

  public deleteGuestUser(userid: any) {
    return new Promise(async (resolve, reject) => {
      try {
        const meetingSetting: any = await MeetingService.deleteUser(userid).then((x) => {
          if (x) {
            resolve(x);
          } else {
            reject("deleteGuestUser failed");
          }
        });
      } catch (e) {
        reject(e);
      }
    });
  }
  //public getHistory()
  //public Edit
  //public getMissed()

  public getredisAdapter(): RedisAdapter {
    return this.adaptor;
  }

  public translateLanguage(text: any, targetlanguage: any) {
    return new Promise(async (resolve, reject) => {
      try {
        console.log("translateLanguage >>>>>>>>>", text);
        console.log("targetlanguage >>>>>>>>>", targetlanguage);
        const meetingSetting: any = await MeetingService.googleTranslation(text, targetlanguage).then((x: any) => {
          if (x) {
            resolve(x);
          } else {
            console.log("Error", x);
            reject("user8 not found");
          }
        });
      } catch (e) {
        reject(e);
      }
    });
  }

  public isInTheFuture(date: any) {
    const today = new Date();
    today.setHours(23, 59, 59, 998);
    return date > today;
  }

  public ccdatapush(msg: any) {
    this.pub.rpush("live_transcription", msg.toString());
  }
}
