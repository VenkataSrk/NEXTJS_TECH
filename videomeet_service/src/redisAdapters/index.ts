/**
 * @createdBy Selvakani
 * @createdOn 27th April 2021
 */

//import { createClient, createCluster } from 'redis';
import { RedisClient } from "redis";
import { CBRedisClient, configuration, NLPRedisClient } from "../config";
const redisAdapter = require('socket.io-redis');
//const Redis = require("ioredis");
import Redis from 'ioredis';
import { createAdapter, RedisAdapter } from "socket.io-redis";
import _, { any } from "underscore";
import { MeetingService } from "../meetingService";
//var redis = require("redis");
import { EXDEV } from "constants";
import { logger } from "../log";
import { Console } from "console";
import { handleHubspotEmail } from "../hubspot";
import { meetingFindOne_model } from "../dao/meetingdetails";
//import { createAdapter, RedisAdapter } from "@socket.io/redis-adapter";
export class redisAdapters {
  private adaptor: any;
  private pub: any;
  private sub: any;
  private CBclient: any
  private CBsendclient: any
  private ioInstance: any

  constructor(iq: any) {
    // let redisConfig: any = {
    //   host: configuration.redis_client.host,
    //   port: configuration.redis_client.port,
    // };
    // let sttRedisConfig: any = {
    //   host: configuration.sttRedis.host,
    //   port: configuration.sttRedis.port
    // }
    // redisConfig.no_ready_check = configuration.redis_client.no_ready_check;
    // redisConfig.auth_pass = configuration.redis_client.auth_pass;
    // sttRedisConfig.no_ready_check = configuration.sttRedis.no_ready_check;
    // sttRedisConfig.auth_pass = configuration.sttRedis.auth_pass;

    this.ioInstance = iq;
    const pubClient = CBRedisClient.duplicate();

    this.CBsendclient = CBRedisClient.duplicate();

    let subClient1 = CBRedisClient.duplicate();
    let subClient2 = CBRedisClient.duplicate();

    const subscriber1 = NLPRedisClient.duplicate();


    (function subscribeblpop() {
      subClient1.blpop('rocketmqresponse_ccaas_56', 60000, async function (err: any, data: any) {
        try {
          // console.log("receive data >>>>>>>>>>>", data);
          if (data != null || data != undefined) {
            let duplicateMessage = JSON.parse(data[1]);
            // console.log("duplicateMessage >>>>>>>>>>>>", duplicateMessage)
            if (duplicateMessage) {
              await iq.receiveConfBridgeMessage(data[1]);
            }

            subscribeblpop();
          } else {
            subscribeblpop();
          }

        } catch (error) {
          subscribeblpop();
          console.log("subscribeblpop error>>>>>>", error)
          logger.info("<<<<< subscribeblpop error >>>>", error);
        }
      });
    })();

    (async function readStream() {
      console.log("readStream>")
      subscriber1.xread('BLOCK', 0, 'STREAMS', 'worktual_cc1', '$', async function (err: any, streamdata: any) {
        console.log("data >>>>>>", streamdata);
        try {
          if (streamdata) {
            const streams = streamdata[0][1]; // Get the array of streams returned
            let data = streams[0][1][1];
            console.log("a >>>>>>>>", data)
            if (data != null || data != undefined) {
              let parsedData = JSON.parse(data);
              console.log("parsedData >>>>>>", parsedData);
              if (parsedData) {
                await iq.broadcastCC(data);
              }

            }

          }
          readStream();
        } catch (error) {
          readStream();
          console.log("Translate blopop error", error)
          logger.info("<<<<< Translate blopop error >>>>", error);
        }

      });

    })();


    let self = this;
    subClient2.xtrim(`${process.env.LIVE_CB_STREAM_NAME_RES}`, 'MAXLEN', 0)
      .then(() => {
        console.log(`Stream "${process.env.LIVE_CB_STREAM_NAME_RES}" has been deleted.`);

        self.cbCreateGroup();
      })
      .catch((err: any) => {
        console.error('Error deleting the stream:', err);
      })
      .finally(() => {
        // client.quit();
      });


    //subscriber.subscribe("rocketmqresponse_8");
    // subscriber1.subscribe("dont_publish");
    //subscriber2.subscribe("translate");
    pubClient.on("error", (err: any) => console.log("Error" + err));
    pubClient.on("message", (err: any) => console.log("message" + err));
    pubClient.on("message_buffer", (err: any) => console.log("message_buffer" + err));
    pubClient.on("pmessage", (err: any) => console.log("pmessage" + err));
    pubClient.on("subscribe", (err: any) => console.log("subscribe" + err));

    pubClient.on("psubscribe", (err: any) => console.log("psubscribe" + err));
    const subClient = pubClient.duplicate();
    this.sub = subClient;
    this.pub = pubClient;
    this.CBclient = subClient2;
    this.adaptor = createAdapter({ pubClient, subClient });

    // let that = this;
    // let i = 0;
    // setInterval(() => {

    //   that.sendCBBasedSteam(i)
    //   i++

    // }, 3000);

  }



  // constructor(iq: any) {
  //   let redisConfig: any = {
  //     host: configuration.redis_client.host,
  //     port: configuration.redis_client.port,
  //   };

  //   let nodes = configuration.redisCluster;

  //   console.log("nodes >>>>", nodes);
  //   logger.info("nodes >>>>", nodes);

  //   const pub = new Redis.Cluster(nodes, { slotsRefreshInterval: 1000 });
  //   const pub1 = new Redis.Cluster(nodes, { slotsRefreshInterval: 1000 });
  //   const sub = pub.duplicate();

  //   (function subscribeblpop() {
  //     console.log("subscribeblpop>")
  //     sub.blpop('rocketmqresponse_1', 60000, async function (err: any, data: any) {

  //       try {
  //         if (data != null || data != undefined) {
  //           let duplicateMessage = JSON.parse(data[1]);
  //           if (duplicateMessage) {
  //             await iq.receiveConfBridgeMessage(data[1]);
  //           }

  //           subscribeblpop();
  //         } else {
  //           subscribeblpop();
  //         }

  //       } catch (error) {
  //         subscribeblpop();
  //         console.log("subscribeblpop error>>>>>>", error)
  //         logger.info("<<<<< subscribeblpop error >>>>", error);
  //       }

  //     });
  //   })();

  //   /*     (function translateblpop() {
  //         console.log("translateblpop>")
  //         sub.blpop('video_cc133', 60000, async function (err: any, data: any) {
  //           console.log("broadcastCC>>>>>>>>>>>>>", data)

  //           try {
  //             if (data != null || data != undefined) {

  //               let parsedData = JSON.parse(data[1])
  //               if (parsedData) {
  //                 await iq.broadcastCC(data[1]);
  //               }

  //             }
  //             translateblpop();
  //           } catch (error) {
  //             translateblpop();
  //             console.log("Translate blopop error", error)
  //             logger.info("<<<<< Translate blopop error >>>>", error);
  //           }

  //         });
  //       })(); */


  //   (async function readStream() {
  //     console.log("readStream>")
  //     pub1.xread('BLOCK', 0, 'STREAMS', 'worktual_cc', '$', async function (err: any, streamdata: any) {
  //       // console.log("data >>>>>>", data);
  //       try {
  //         const streams = streamdata[0][1]; // Get the array of streams returned
  //         let data = streams[0][1][1];
  //         console.log("a >>>>>>>>", data)
  //         if (data != null || data != undefined) {
  //           let parsedData = JSON.parse(data);
  //           console.log("parsedData >>>>>>", parsedData);
  //           if (parsedData) {
  //             await iq.broadcastCC(data);
  //           }

  //         }
  //         readStream();
  //       } catch (error) {
  //         readStream();
  //         console.log("Translate blopop error", error)
  //         logger.info("<<<<< Translate blopop error >>>>", error);
  //       }

  //     });

  //   })();

  //   let that = this;

  //   that.pub = pub;
  //   that.sub = sub;
  //   that.adaptor = redisAdapter({
  //     pubClient: pub,
  //     subClient: sub
  //   })

  //   this.push('rocketmqresponse_1', JSON.stringify({ "msg": "Push message testing" }));
  //   //that.listenForMessage();

  //   /*   let i = 0;
  //     setInterval(() => {

  //       that.sendSteam(i)
  //       i++

  //     }, 3000); */
  // }


  public cbCreateGroup() {
    let self = this;
    this.CBclient.xgroup('CREATE', process.env.LIVE_CB_STREAM_NAME_RES, process.env.LIVE_CB_CONSUMER_GROUP, '0', 'MKSTREAM', (err: any) => {
      if (err && !err.message.includes('BUSYGROUP Consumer Group name already exists')) {
        console.error('Error creating consumer group:', err);

        self.cbDeleteConsumer();
      } else {
        console.log('Start read readMessage >>>>>>>>');
        self.cbReadMessages(process.env.LIVE_CB_STREAM_NAME_RES, process.env.LIVE_CB_CONSUMER_GROUP, process.env.LIVE_CB_CONSUMER_NAME);
      }
    });
  };

  public cbDeleteGroup() {
    let self = this;
    this.CBclient.xgroup('DESTROY', process.env.LIVE_CB_STREAM_NAME_RES, process.env.LIVE_CB_CONSUMER_GROUP)
      .then((response: any) => {
        console.log(`Stream "${process.env.LIVE_CB_STREAM_NAME_RES}" and consumer group "${process.env.LIVE_CB_CONSUMER_GROUP}" destroyed.`);
        self.cbCreateGroup();

      })
      .catch((err: any) => {
        console.error('Error destroying stream and consumer group:', err);
        self.cbCreateGroup();
      });
  };
  public cbDeleteConsumer() {
    let self = this;
    this.CBclient.xgroup('DELCONSUMER', process.env.LIVE_CB_STREAM_NAME_RES, process.env.LIVE_CB_CONSUMER_GROUP, process.env.LIVE_CB_CONSUMER_NAME)
      .then((response: any) => {
        if (response === 1) {
          console.log(`Consumer "${process.env.LIVE_CB_CONSUMER_NAME}" removed from group "${process.env.LIVE_CB_CONSUMER_GROUP}"`);

          self.cbDeleteGroup();
        } else {
          console.log(`Consumer "${process.env.LIVE_CB_CONSUMER_NAME}" not found in group "${process.env.LIVE_CB_CONSUMER_GROUP}"`);
          self.cbDeleteGroup();
        }
      })
      .catch((err: any) => {
        console.error('Error removing consumer:', err);
        self.cbDeleteGroup();
      });
  };


  public cbacknowledgeMessage(stream: any, group: any, messageId: any, messageData: any) {
    // Use XACK to acknowledge the message
    let self = this;
    this.CBclient.xack(stream, group, messageId, (err: any, result: any) => {
      if (err) {
        console.error('Error Androidacknowledging message:', err);
      } else if (result === 1) {

        console.log(`${stream}_${group}  Acknowledged message ${messageId}_ACK`);
        self.CBclient.xtrim(stream, 'MAXLEN', 15)

      } else {
        console.log(`${stream}_${group}  Message ${messageId}_ACK not found in consumer group`);
        const inputData = {
          stream,
          group,
          messageId,
          message: messageData,
          isAck: false
        };
      }
    });
  }
  public cbReadMessages(stream: any, group: any, consumer: any) {
    // Use XREADGROUP to read messages
    console.log(stream, group, consumer, "consumer")
    let self = this;
    this.CBclient.xreadgroup(
      'GROUP', group, consumer, 'BLOCK', 0, 'STREAMS', stream, '>', (err: any, streams: any) => {
        // 'GROUP', group, consumer, 'STREAMS', stream, '$', (err, streams) => {
        if (err) {
          console.error('Error reading messages:', err);
          // redis.quit();
        } else {
          // Process the messages
          if (streams) {
            streams.forEach((entry: any) => {
              const [streamName, messages] = entry;
              // console.log(`Received messages from ${streamName}:`);
              messages.forEach(([messageId, messageData]: any) => {
                //console.log(`readMessages >>>>> ${stream}_${group} Message ID: ${messageId}, Message Data: ${messageData}`);
                const rawMessage = `${messageData}`.split('message,');
                console.log("rule base rawMessage >>>>>>", rawMessage);

                if (rawMessage != null || rawMessage != undefined) {

                  // console.log("duplicateMessage >>>>>>>>>>>>", duplicateMessage)

                  self.ioInstance.receiveConfBridgeMessage(rawMessage[1]);


                }
                self.cbacknowledgeMessage(stream, group, messageId, JSON.parse(rawMessage[1]));
                // self.socketadapter.receiveMessageFromTemplateResponse(messageData[1]);

              });
            });
          }

          // Continue reading messages
          self.cbReadMessages(stream, group, consumer);
        }
      }
    );
  }

  public sendSteam(ie: any) {

    console.log("send stream callig");
    const streamKey = 'worktual_cc';
    const jsonData = { foo: 'bar', baz: ie }; // JSON data to send

    const streamData = [['data', JSON.stringify(jsonData)]]; // Format the JSON data as a stream entry

    let that = this;

    this.pub.xadd(streamKey, "*", "data", JSON.stringify(jsonData))

  }

  public sendCBBasedSteam = (ie: any) => {

    const streamKey = process.env.LIVE_CB_STREAM_NAME_RES;
    const jsonData = { 'question': 'Hello', 'sessionId': `${new Date().getTime()}`, 'bot_id': ie, 'botType': 'convo', 'domainId': 10300, 'websiteId': '', 'ipaddress': '194.66.175.90', 'isClose': 0 }
    //{ foo: 'Selva', baz: ie, iuid: `274d7496f5f-ccda-123c-11b0-00505690f345_10.150.0.182_${new Date().getTime()}` }; // JSON data to send

    //const streamData = [['data', JSON.stringify(jsonData)]]; // Format the JSON data as a stream entry

    // const that = this;
    console.log('sendSteam >>>>>>', jsonData, streamKey);
    // this.CBsendclient.publish("socketiorequest_ccaas_23", JSON.stringify(jsonData));

    this.CBsendclient.xadd(streamKey, '*', 'message', JSON.stringify(jsonData), (error: any, messageId: any) => {
      if (error) {
        console.error('Error adding entry to stream:', error);
      } else {
        // console.log('Added entry with message ID:', messageId);
        // ack(messageId)
      }
    });

  };

  public processMessage(message: any) {
    console.log("Id: %s. Data: %O", message[0], message[1]);
  };
  public async listenForMessage(lastId = "$") {

    console.log("listenForMessage >>>>>>");
    // `results` is an array, each element of which corresponds to a key.
    // Because we only listen to one key (mystream) here, `results` only contains
    // a single element. See more: https://redis.io/commands/xread#return-value
    const results = await this.pub.xread("BLOCK", 0, "STREAMS", "myStream", lastId);
    console.log("results >>>>>>", results);
    const [key, messages] = results[0]; // `key` equals to "user-stream"

    messages.forEach(this.processMessage);

    // Pass the last id of the results to the next round.
    await this.listenForMessage(messages[messages.length - 1][0]);
  }
  public async readStream() {
    console.log("Recive stream callig")
    const streamKey = 'worktual';

    await this.pub.xread('BLOCK', 60000, 'STREAMS', streamKey, '$')
      .then((result: any) => {
        console.log("result >>>>>>", result);
        const streams = result[0][1]; // Get the array of streams returned

        for (const [stream, entries] of streams) {
          console.log('Stream:', stream);

          /*           for (const [id, fields] of entries) {
                      console.log('Entry ID:', id);
                      const jsonData = JSON.parse(fields[1][1]); // Parse the JSON data
                      console.log('JSON Data:', jsonData);
                    } */
        }
      })
      .catch((error: any) => {
        console.error('Error reading Redis stream:', error);
      });

  }
  public push(meetingId: any, msg: any): void {
    this.pub.rpush(meetingId, msg);
  }

  public ccdatapush(msg: any) {
    this.pub.rpush("live_transcription", msg.toString());
  }

  public createSet(key: any, data: any) {
    this.pub.sadd(key, data);
  }

  public getValuefromSet(key: any) {
    return new Promise((resolve, reject) => {
      this.pub.smembers(key, (err: any, data: any) => {
        resolve(data);
      });
    });
  }

  /*   public async setMeetingDetails1(meetingId: any, meetingDetails: any, iq?: any) {
      //console.log("this.pub >>>>", this.pub);
  
      // console.log("Set meeting details", JSON.stringify(meetingDetails));
      //this.pub.set(meetingId, JSON.stringify(meetingDetails), "EX", 86400);
  
      let array = [{ "ur_conf_message": { "ur_conf_event": { "event_sequence": "005e4275-a980-4a97-8879-8133dcddad53", "event_name": "host_joined", "event_time": "2023-04-26T16:50:33Z" }, "ur_conf_meeting_info": { "meeting_id": "4ij-82-0kv-cw", "meeting_name": "02", "host_joined": 0 }, "ur_user_info": [{ "user_id": "srikrishna.t@vectone.com", "sip_id": "5367", "connection_id": "GRWcdGLUGwgOnyZPAABl", "device_id": "27fd9396-1417-4409-809b-b7f61a939f3b", "user_type": "host" }], "ur_media_info": { "sdp_str": "" } } },
      { "ur_conf_message": { "ur_conf_event": { "event_sequence": "2ca25906-c750-48a6-bd6d-68902269e703", "event_name": "participant_joined", "event_time": "2023-04-26T10:11:50.976Z" }, "ur_conf_meeting_info": { "meeting_id": "4ij-82-0kv-cw", "meeting_name": "02", "host_joined": true }, "ur_user_info": [{ "user_id": "UmaShankari.R@vectone.com", "sip_id": "5732", "connection_id": "gJmIe8F-HlC0uwvTAABr", "device_id": "616e026c-7ade-4da6-9005-a84aa04eeddd", "user_type": "participant", "plan_type": 3 }], "ur_media_info": { "sdp_str": "" }, "correlation_id": "616e026c-7ade-4da6-9005-a84aa04eeddd_4ij-82-0kv-cw_5732" } },
  
      { "ur_conf_message": { "ur_conf_event": { "event_sequence": "123005e4275-a980-4a97-8879-8133dcddad53", "event_name": "host_joined", "event_time": "2023-04-26T16:50:33Z" }, "ur_conf_meeting_info": { "meeting_id": "4ij-82-0kv-cw", "meeting_name": "02", "host_joined": 0 }, "ur_user_info": [{ "user_id": "srikrishna.t@vectone.com", "sip_id": "5367", "connection_id": "GRWcdGLUGwgOnyZPAABl", "device_id": "27fd9396-1417-4409-809b-b7f61a939f3b", "user_type": "host" }], "ur_media_info": { "sdp_str": "" } } },
      { "ur_conf_message": { "ur_conf_event": { "event_sequence": "4442ca25906-c750-48a6-bd6d-68902269e703", "event_name": "participant_joined", "event_time": "2023-04-26T10:11:50.976Z" }, "ur_conf_meeting_info": { "meeting_id": "4ij-82-0kv-cw", "meeting_name": "02", "host_joined": true }, "ur_user_info": [{ "user_id": "UmaShankari.R@vectone.com", "sip_id": "5732", "connection_id": "gJmIe8F-HlC0uwvTAABr", "device_id": "616e026c-7ade-4da6-9005-a84aa04eeddd", "user_type": "participant", "plan_type": 3 }], "ur_media_info": { "sdp_str": "" }, "correlation_id": "616e026c-7ade-4da6-9005-a84aa04eeddd_4ij-82-0kv-cw_5732" } }
      ]
  
      for (let index = 0; index < array.length; index++) {
        const element = array[index];
        console.log("index >>>>>", index);
        iq.receiveConfBridgeMessage(JSON.stringify(element));
      }
    } */

  public async setMeetingDetails(meetingId: any, meetingDetails: any) {
    //console.log("this.pub >>>>", this.pub);
    // console.log("Set meeting details", JSON.stringify(meetingDetails));
    this.pub.set(`${meetingId}_socket`, JSON.stringify(meetingDetails), "EX", 86400);


  }

  public transcriptBLPOP() {
    this.sub.blpop('video_cc1', 60000, async function (err: any, data: any) {
      console.log("broadcastCC>>>>>>>>>>>>>", data)

      /*  if (data != null || data != undefined) {
         await iq.broadcastCC(data[1]);
       } */

    });
  }
  public removeValuebyKey(key: any) {
    return new Promise((resv, rej) => {
      this.pub.del(key, (err: any, reply: any) => {
        resv(1);
      });
    });
  }
  public getMeetingDetails(meetingId: any, jwtToken: any) {
    console.log("getMeetingDetails cslling>>>>>>>>>")
    return new Promise((resolve, reject) => {
      try {
        this.pub.get(meetingId, async (err: any, data: any) => {

          // console.log("err >>>>>", err);
          // console.log("get data >>>>", data);
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
        console.log("EEEEEEE", e)
        reject(e);
      }
    });
  }

  public getMeetingInfofromRedis(meetingId: any) {
    return new Promise((resolve, reject) => {
      try {
        let settings: any = {
          lockMeeting: false, //on enabling this No user will be allowed to join the  ongoing meeting
          waitingRoom: false,
          isParticipantjoinbefhost: false,
          startTime: 0,
          endTime: 0,
          duration: 60,
          inviteUrl: "",
          isHostjoined: false,
          hostDisconnect: true,
          plan_type: 1,
          user_plan_type: "",
          isHostAudioMute: false,
          isWhiteBoard: false,
          isVoiceEngineStarted: false,
          isScreenshareStarted: false,
          isWhiteboardStarted: false,
          isParticipantVideo: true,
          isHostMyVideo: true,
          chatSetting: "Everyone Publicly and Privately",
          generalSetting: {
            isChat: true,
            isScreenShare: true,
            renameThemselves: true,
            lockMeeting: false,
            is_participant_unmute_themselves: false,
            shareOptions: {
              howManyPart: "one",
              whoShare: "host",
              startShare: "host",
            },
            isWaitingroom: false,
            is_participant_mute_entry: false
          },
          participants: {},
          joinedParticipants: [],
          conversations: [],
          meetingId: meetingId,
          meetingPwd: "",
          meetingName: "",
          activeConn: [],
          waitingroomConn: [],
          joinedConn: [],
          alternateHostDetails: {},
          hostDetails: {},
          devicesInfo: {},
          globalKeywords: [
            "Task", "Note", "Important", "Name", "Mark", "List", "Event"
          ],
          joinedParticipantlist: [],
          restrictScreenShare: false,
          authenticationJoin: "anyone",
          requireAuthentication: false,
          sipIdList: [],
          is_participant_invite_guest: false,
          is_host_myaudio: false,
          status: "",
          meetStartTime: 0,
          queueId:"",
          agentDetails:{},
          domain_id:"",
          guestDetails:{},
          meetingDetails:{},
          recordId:"",
          transfer:false,
          secondaryAgentDetails:{},
          transferStatus:""
        };
        this.pub.get(meetingId, async (err: any, data: any) => {
           console.log("datadatadatadatadatadatadata",data)
                    if (err || data == null || data == "{}") {
                     resolve(settings) 
                    } else {
                      if (data != null) {
              const retData = JSON.parse(data);
              // console.log("retDataretData",retData);
              settings.waitingRoom =  false;
              settings.meetingPwd = retData.meetingDetails.password || "";
              settings.generalSetting.isWaitingroom = false;
              settings.generalSetting.is_participant_mute_entry = retData.meetingDetails.is_participant_mute_entry;
              settings.hostDisconnect = true//re.ultis_without_host_continue;
              settings.isParticipantVideo = retData.meetingDetails.is_participant_video || true;
              settings.isHostMyVideo = retData.meetingDetails.is_host_myvideo || false;
              settings.isParticipantjoinbefhost = true;
              settings.is_participant_invite_guest = retData.meetingDetails.is_participant_invite_guest || false;
              settings.startTime = retData.meetingDetails.start_timestamp;
              settings.endTime = retData.meetingDetails.end_timestamp;
              settings.duration = retData.meetingDetails.duration_minutes;
              settings.inviteUrl = retData.meetingDetails.invite_url;
              settings.restrictScreenShare = retData.meetingDetails.restrictScreenShare || false;
              settings.authenticationJoin = retData.meetingDetails.authenticationJoin || "Anyone";
              settings.requireAuthentication = retData.meetingDetails.requireAuthentication || false;
              settings.queueId=retData.meetingDetails.queueId;
              settings.agentDetails=retData.meetingDetails.agentDetails;
              settings.domain_id=retData.meetingDetails.domain_id;
              settings.startTime=retData.meetingDetails.startTime;
              settings.meetingName = retData.meetingDetails.title;
              settings.is_host_myaudio = retData.meetingDetails.is_host_myaudio || false;
              settings.status = retData.meetingDetails.status;
              settings.guestDetails=retData.guestDetails;
              settings.meetingDetails=retData.meetingDetails;
        
              let part=JSON.stringify(retData.meetingDetails.participants)

              let sipIdList: any = [];
              _.forEach(retData.meetingDetails.participants, (item:any) => {
                const sipId = item.sipid;
        
        
                let ishost: boolean = false;
                let isAlternatehost: boolean = false;
        
                if (item.roletype == 1) ishost = true;
                if (item.roletype == 2) isAlternatehost = true;
        
                sipIdList.push(sipId);
                settings.participants[sipId] = {
                  video: true,
                  mute: true,
                  pined: false,
                  name: item.name || "",
                  isHost: ishost,
                  sipId: sipId,
                  isAlternateHost: isAlternatehost,
                  isRecordingAllowed: false,
                  moveToWaitingRoom: retData.meetingDetails.is_participant_waiting_room,
                  profileImg: item.profile_img || "",
                  status: "invited", //"joined" / "inwaitingroom" / "removed";
                  hand: false,
                  ext: item.ext,
                  isGuest: false,
                  roleType: item.roletype || 3,
                  removeFromMeeting: false,
                  email: item.email,
                  isHostAllowed: false,
                  waiting_retrycount: 0,
                  recordingStatus: "stop",
                  closedCaption: false,
                  isScreenshare: false,
                  isWhiteboard: false,
                  isMirror: true,
                  joinedTime: 0,
                  isVoiceengineStart: false,
                  isReconnectstatus: false,
                  recordFilePath: configuration.videoRecordFileURL,
                  reactions: "",
                  non_verbal: "",
                  cc_lang: "en",
                  speaker_lang: "en"
                };
              });
        
              settings.sipIdList = sipIdList;
              console.log("settings >>>>>>>>>", settings)
             resolve(settings);
            //  return settings;
            }
          }
        });
      } catch (e) {

        console.log("catch calling>>>>>>", e);
        reject(e);
      }
    });
  }

  public getMeetingRedis(meetingId: any) {
    return new Promise((resolve, reject) => {
      this.pub.get(`${meetingId}_socket`, async (err: any, data: any) => {

        console.log("Getting new data from new >>>>>>>>>", data)
        if (err || data == null || data == "{}") {
       
        await this.getMeetingInfofromRedis(meetingId).then((x) => {
console.log("After getting data from orignal", x)
          if (x) {
            this.setMeetingDetails(meetingId, x);
            resolve(x);
          } else {
            reject("user3 not found");
          }
        })
      
        }
        else{
        if(data !== null)
        {  const retData = await JSON.parse(data);
          resolve(retData);
        //  return retData;
        
        }
        }
      })
    })
  }

  public addParticipantstoMeeting(meetingId: any, jwtToken: any, participant: any) {
    return new Promise((resolve, reject) => {
      try {
        this.pub.get(meetingId, async (err: any, data: any) => {
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
      this.pub.lrange(meetingId, 0, -1, (err: any, data: any) => {
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
    console.log("getMeetingFind >>>>>>")
    return new Promise(async (resolve, reject) => {
      try {
        //this.pub.get(meetingId, async (err, data) => {
        const meetingSetting: any = await MeetingService.getMeeting(meetingId, jwtToken).then((x) => {
          if (x) {
            resolve(x);
          } else {
            // reject("user3 not found");
            resolve({});
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

          // console.log("TRESPONSE HOST JOININ >>>>", x)
          if (x) {
            resolve(x);
          } else {
            resolve({});
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

  public updateMeetingWithoutToken(meetingId: any, data: any) {
    return new Promise(async (resolve, reject) => {
      try {
        await MeetingService.updateMeetingNoToken(meetingId, data).then((x) => {
          if (x) {
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

  public getMeetingDetailsWithoutToken(meetingId: any) {
    console.log("getMeetingDetails cslling>>>>>>>>>")
    return new Promise((resolve, reject) => {
      try {
        this.pub.get(meetingId, async (err: any, data: any) => {
          // console.log("err >>>>>", err);
          // console.log("get data >>>>", data);
          await MeetingService.getMeetingbyIdNoToken(meetingId).then((x) => {
            if (x !== null) {
              // console.log(">>>>>>>>>>>>>get MeetingbyId NoToken>>>>>>>>>>>", JSON.stringify(x));
              resolve(x);
            } else {
              reject(null);
            }
          });
        });
      } catch (e) {
        console.log("EEEEEEE", e)
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
        this.pub.get(meetingId, async (err: any, data: any) => {
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
            reject({});
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
            reject({});
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

  public getredisAdapter() {
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
}
