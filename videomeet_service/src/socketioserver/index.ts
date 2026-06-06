/**
 * @createdBy Selvakani
 * @createdOn 27th April 2021
 */

import { Server, Socket } from "socket.io";
import {
  storeWhiteBoardMessage,
  getWhiteBoardMessage,
  getTranscripts, createClosecaption, findCloseCaption, updateCloseCaption, filterCloseCaption, filterNameCloseCaption, addKeywordsMeeting, removeKeywordsMeeting, storeClosecaption,
  storeClosecaptionCcaas,
  getSettingVoiceVideoRecordingdao
} from "../dao";
import { RedisAdapter } from "socket.io-redis";
//import {RedisAdapter } from "@socket.io/redis-adapter";
import { configuration, ioredis } from "../config";
import * as jwt from "jsonwebtoken";
import { uuid } from "uuidv4";
import { v4 as uuidv4 } from 'uuid';
import _ from "underscore";
import { EVENTNAMES } from "../helpers/constants";
import { redisAdapters } from "../redisAdapters";
import { rocketMQConnection } from "../rocketmqMiddleware";
import { videoCCRedisConnection } from '../videoccredispublisher';
import { cronJob } from "../cronJob";
import { closedCaption } from "../closedCaption";
import { logger } from "../log";
import { handleHubspotEmail, handlePartMailTrigger, particpantEmailTemplate } from "../hubspot";
import { eventNames, openStdin } from "process";
import { getSettings } from "../dao/meetingdetails";
const base64id = require("base64id");

let clientCount = 1;

/* let isRedisResponseProcessing = false;
let queuesList: any = []; */
export class IOServer {
  private io: any;
  private redisadaptor: redisAdapters = new redisAdapters(this);
  private rocketMQMiddleware: any = new rocketMQConnection();
  private videoccpublisher: any = new videoCCRedisConnection();
  private speechmatics: any = new closedCaption.SpeechMaticsEngine(this);
  private cronJob = new cronJob(this);
  private isRedisResponseProcessing: any = false;
  private queuesList: any = []


  constructor(httpServer: any) {
    this.io = new Server(httpServer, {
      path: '/videomeet',
      cors: {
        origin: "*",
      },
      pingInterval: 60000,
      pingTimeout: 120000,

    });

    /*     this.io.engine.generateId = function (result : any) {
    
          const decoded: any = jwt.verify(result?.headers?.auth, configuration?.jwt_secret);
         
          const userSocketId = decoded?.sipLoginId+"_"+decoded?.companyId +"_"+decoded?.ext+"_"+result?._query?.deviceid;
          console.log("decoded >>>>>>>>", decoded);
          base64id.generateId =  function ()
          {  
              
              return userSocketId;
          };
            return base64id.generateId();
          };  */

    this.addAdaptor();
    this.connectionhandler();
    this.addAuthMiddleware();
  }

  private addAdaptor(): void {
    //this.redisadaptor = new redisAdapters();
    this.io.adapter(this.redisadaptor.getredisAdapter());
  }

  private addAuthMiddleware(): void {
    this.io.use(async (socket: any, next: any) => {

      console.log("Socket connection establish>>>")
      let decoded: any = {};
      socket.handshake.auth.token =
        socket.handshake.headers && socket.handshake.headers.auth;
      if (
        !socket.handshake.headers ||
        !socket.handshake.auth.token ||
        !socket.handshake.query.roomId ||
        !socket.handshake.query.deviceid
      ) {
        logger.info("<<<<<<<< Unauthorized Received request >>>>>>");
        console.log("<<<<<<<< Unauthorized Received request >>>>>>>")
        next(new Error("unauthorized"));
      } else {
        decoded = jwt.verify(socket.handshake.auth.token, configuration.jwt_secret);
        const meetingId = socket.handshake.query.roomId;
        console.log("decodeddecoded123444", decoded)
        console.log(` ${new Date().getTime()} <<<<<< Meeeting ID : ${meetingId} >>>>> <<<<<<<< INCOMING REQUEST  >>>>`, JSON.stringify(decoded))
        logger.info(` ${new Date().getTime()} <<<<<< Meeeting ID : ${meetingId} >>>>> <<<<<<<< INCOMING REQUEST  >>>>`, JSON.stringify(decoded))
        let sipid = decoded.sipLoginId;
        let deviceid = socket.handshake.query.deviceid;
        const findsip: any = await this.redisadaptor.getMeetingRedis(
          meetingId
        );
        // console.log("findsipfindsip",findsip);
        socket.handshake.query.sipLoginId = sipid;

        if (!findsip || Object.keys(findsip).length === 0) {

          logger.info("Invalid meeting id calling");
          console.log("Invalid meeting id calling");
          next(new Error("No Meeting available"));
        } else {
          let filterArray = _.findWhere(findsip.meetingDetails.participants, { sipid: sipid });
          // console.log("filterArrayfilterArrayfilterArray",filterArray);
          const response: any = await this.redisadaptor
            ?.getMeetingRedis(meetingId)
            .then(async (meetingdetails: any) => {

              console.log("meetingdetailsmeetingdetails////",meetingdetails)
              // this.redisadaptor.setMeetingDetails(meetingId,meetingdetails)
              let isAlternateHost: boolean = false;

              let deviceidDetails = _.findWhere(meetingdetails.devicesInfo, {
                deviceid: deviceid,
              });

              let customer_details:any=meetingdetails.joinedParticipantlist.filter((elem:any)=>{
                return elem.user_type !== "agent" && !elem.conId.includes("_screenshare");
              })

              if(customer_details.length > 0 && decoded.type === "guest"){
                next(new Error("Meeting Link is expired"));
              }

              console.log("new Date().getTimenew Date().getTime>>>",new Date().getTime(),"Date.now()",Date.now(),"meetingdetails.meetingDetails.end_timestamp>>>>",meetingdetails.meetingDetails.end_timestamp);

              if (meetingdetails.status === 'Completed' || Date.now() > meetingdetails.meetingDetails.end_timestamp) {
                next(new Error("Meeting Link is expired"));
              }
              else {

                if (
                  filterArray &&
                  meetingdetails.participants[sipid] == undefined &&
                  filterArray.roletype != 1
                ) {
                  if (filterArray.roletype == 2) isAlternateHost = true;
                  meetingdetails.participants[sipid] = {
                    video: true,
                    mute: true,
                    pined: false,
                    name: filterArray.name || "",
                    isHost: false,
                    sipId: decoded.sipLoginId,
                    isAlternateHost: false,
                    isRecordingAllowed: true,
                    moveToWaitingRoom: meetingdetails.waitingRoom,
                    profileImg: "",
                    status: "invited", //"joined" / "inwaitingroom" / "removed";
                    hand: false,
                    ext: decoded.ext,
                    roleType: 3,
                    isGuest: false,
                    removeFromMeeting: false,
                    email: decoded.username || "",
                    isHostAllowed: false,
                    waiting_retrycount: 0,
                    recordingStatus: "stop",
                    closedCaption: false,
                    isScreenshare: false,
                    isWhiteboard: false,
                    isMirror: true,
                    isVoiceengineStart: false,
                    recordFilePath: configuration.videoRecordFileURL,
                    speaker_lang: "en",
                    cc_lang: "en",
                    reactions: "",
                    non_verbal: ""
                  };

                  await this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);
                } else {

                  let existDeviceid_details: any = _.findWhere(
                    meetingdetails.devicesInfo,
                    { deviceid: deviceid }
                  );
                  if (
                    existDeviceid_details &&
                    existDeviceid_details.sipId == decoded.sipLoginId &&
                    (existDeviceid_details.removeFromMeeting ||
                      existDeviceid_details.status == "removed")
                  ) {
                    next(new Error("Host already removed you."));
                  } else {
                    if (decoded.type && decoded.type == "guest") {
                      if (
                        decoded.meeting_pwd == meetingdetails.meetingPwd &&
                        decoded.meetingId == meetingId
                      ) {

                        // console.log("<<<<<<<<<<<<<<<<<<<GUEST USER CALLNG>>>>>>>>>>>>>>>>")

                        meetingdetails.participants[sipid] = {
                          video: true,
                          mute: true,
                          pined: false,
                          name: decoded.username || "",
                          isHost: false,
                          sipId: decoded.sipLoginId,
                          isAlternateHost: false,
                          isRecordingAllowed: true,
                          moveToWaitingRoom: meetingdetails.waitingRoom,
                          profileImg: "",
                          status: "invited", //"joined" / "inwaitingroom" / "removed";
                          hand: false,
                          ext: decoded.ext,
                          roleType: 3,
                          isGuest: true,
                          removeFromMeeting: false,
                          email: "",
                          isHostAllowed: false,
                          waiting_retrycount: 0,
                          recordingStatus: "stop",
                          closedCaption: false,
                          isScreenshare: false,
                          isWhiteboard: false,
                          isMirror: true,
                          isVoiceengineStart: false,
                          recordFilePath: configuration.videoRecordFileURL,
                          cc_lang: "en",
                          speaker_lang: "en",
                          reactions: "",
                          non_verbal: ""
                        };

                        // console.log(meetingdetails,"<<<<<<<<<<<<<<<<<<<GUEST USER CALLNG [SIP ID DETAILS]>>>>>>>>>>>>>>>>", meetingdetails.participants[sipid])

                        logger.info("<<<<<<<<<<<<<<<<<<<GUEST USER CALLNG [SIP ID DETAILS]>>>>>>>>>>>>>>>>", meetingdetails.participants[sipid]);
                        await this.redisadaptor.setMeetingDetails(
                          meetingId,
                          meetingdetails
                        );
                        // console.log("nottt12333",filterArray)
                        if (!filterArray) {
                          console.log("nottt")
                          const guestDetails = [
                            {
                              sipid: decoded.sipLoginId,
                              name: decoded.username,
                              ext: decoded.ext,
                              profile_img: "",
                              roletype: 3,
                              user_type: decoded.type,
                              email: "",
                            },
                          ];

                          let part: any = meetingdetails.meetingDetails.participants.concat(guestDetails)
                          meetingdetails.meetingDetails.participants = part;

                          // console.log("meetingdetailsmeetingdetails000",meetingdetails)

                          this.redisadaptor.setMeetingDetails(meetingdetails.meetingDetails.uuid, meetingdetails);


                          // this.redisadaptor
                          //   ?.addParticipantstoMeeting(
                          //     meetingId,
                          //     socket.handshake.auth.token,
                          //     guestDetails
                          //   )
                          //   .then((response: any) => { })
                          //   .catch((e) => {
                          //     new Error("Internal server error");
                          //   });
                        }
                        next();
                      } else {
                        next(new Error("Not_valid_participants"));
                      }
                    } else if (
                      !meetingdetails ||
                      (!meetingdetails.participants[sipid] &&
                        !decoded.type &&
                        decoded.type !== "guest")
                    ) {


                      let existHost_details: any = _.findWhere(
                        meetingdetails.participants,
                        { isHost: true }
                      );

                      // existHost_details.isHost = false;

                      // meetingdetails.participants[existHost_details.sipId.toString()] = existHost_details
                      meetingdetails.participants[sipid] = {
                        video: true,
                        mute: true,
                        pined: false,
                        name: decoded.profileName || "",
                        isHost: true,
                        sipId: decoded.sipLoginId,
                        isAlternateHost: false,
                        isRecordingAllowed: false,
                        moveToWaitingRoom: meetingdetails.waitingRoom,
                        profileImg: decoded.profileImg,
                        status: "invited", //"joined" / "inwaitingroom" / "removed";
                        hand: false,
                        ext: decoded.ext,
                        roleType: 1,
                        isGuest: false,
                        removeFromMeeting: false,
                        email: decoded.username,
                        isHostAllowed: false,
                        waiting_retrycount: 0,
                        recordingStatus: "stop",
                        closedCaption: false,
                        isScreenshare: false,
                        isWhiteboard: false,
                        isMirror: true,
                        isVoiceengineStart: false,
                        reactions: "",
                        non_verbal: "",
                        cc_lang: "en",
                        speaker_lang: "en",

                      };

                      console.log("<<<<<<<<<<<<<<<<<<<COMPANY USER CALLNG [SIP ID DETAILS]>>>>>>>>>>>>>>>>", meetingdetails.participants[sipid])

                      logger.info("<<<<<<<<<<<<<<<<<<<COMPANY USER CALLNG [SIP ID DETAILS]>>>>>>>>>>>>>>>>", meetingdetails.participants[sipid]);
                      await this.redisadaptor.setMeetingDetails(
                        meetingId,
                        meetingdetails
                      );
                      if (!filterArray) {
                        const guestDetails = [
                          {
                            sipid: decoded.sipLoginId,
                            name: decoded.profileName,
                            ext: decoded.ext,
                            profile_img: decoded.profileImg,
                            roletype: 3,
                            user_type: 'company',
                            email: decoded.username,
                          },
                        ];

                        // this.redisadaptor
                        //   ?.addParticipantstoMeeting(
                        //     meetingId,
                        //     socket.handshake.auth.token,
                        //     guestDetails
                        //   )
                        //   .then((response: any) => { })
                        //   .catch((e) => {
                        //     new Error("Internal server error");
                        //   });
                      }
                      next();
                    } else {
                      next();
                    }
                  }

                }

              }



            })
            .catch((e) => {
              console.log("errorrrr000123", e)
              next(new Error("Not_valid_participants"));
            });
        }


      }
    });
  }

  private connectionhandler(): void {
    //Connection

    this.io.on(EVENTNAMES.CONNECTION, async (socket: Socket) => {

      let socketDetails = socket
      const query = socketDetails.handshake.query;
      const meetingId: any = query.roomId || "";
      const deviceid: any = query.deviceid || "";
      const userName = socketDetails.handshake.query.sipLoginId || "";
      const jwtToken = socketDetails.handshake.auth.token;
      const audio = socketDetails.handshake.query.isAudio == "true" ? true : false;
      const video = socketDetails.handshake.query.isVideo == "true" ? true : false;
      const transfer = socketDetails.handshake.query.transfer == "true" ? true : false;
      const decoded: any = jwt.verify(socketDetails.handshake.auth.token, configuration.jwt_secret);

      let isDisconneted = false;
      const meetInfo: any = {};
      meetInfo.jwtToken = jwtToken;
      meetInfo.sipLoginId = userName;
      meetInfo.meetingId = meetingId;
      meetInfo.sipInfo = decoded;
      meetInfo.socketId = socket.id //decoded?.sipLoginId+"_"+decoded?.companyId +"_"+decoded?.ext+"_"+deviceid;//socket.id;
      meetInfo.deviceid = deviceid;
      meetInfo.audio = audio;
      meetInfo.video = video;
      meetInfo.isTransfer=transfer;
      meetInfo.timer = null;

      const meetingdetails: any = await this.redisadaptor?.getMeetingRedis(
        meetingId
      );

      logger.info("<<<< Connection Init >>>", meetInfo, "meetingdetails#######", meetingdetails);
      console.log("meetInfo####",meetInfo);


      // console.log("IO redis calling");

      //let userArray = ['5369', '6439', '38248']
      // ioredis.to(userArray).emit("addParticipant_notification", "Hello from video meetinh");

      await this.connectionManager(meetingdetails, socketDetails, meetInfo);

      socket.on(EVENTNAMES.CC_LANG_UPDATE, async (req: any) => {
        console.log("req>>", req);
        this.updateCCLang(req, meetInfo);
      })

      socket.on(EVENTNAMES.REACTIONS, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.REACTIONS} >>>>>>`, req);
        await this.handleReactions(req, meetInfo);
      });
      socket.on(EVENTNAMES.NON_VERBAL, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.NON_VERBAL} >>>>>>`, req);
        await this.handleNonVerbal(req, meetInfo);
      });

      // socket.on(EVENTNAMES.HOST_REQUEST_UNHIDE, async (req: any) => {
      //           logger.info(`<<<< ${EVENTNAMES.HOST_REQUEST_UNHIDE} >>>>>>`, req);
      //           await this.hostRequesttoUnhide(req, meetInfo);
      //         });

      //Public Chat ////

      socket.on(EVENTNAMES.PUBLIC_CHAT, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.PUBLIC_CHAT} >>>>>>`, req);

        this.sendNewchatMessage(req, meetInfo);
      });

      //Private Chat //

      socket.on(EVENTNAMES.PRIVATE_CHAT, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.PRIVATE_CHAT} >>>>>>`, req);
        if (req.tosocketId) {
          this.sendPrivatechatMessage(req, meetInfo);
        } else {
          //Need to handle
        }
      });

      //Add participants //

      socket.on(EVENTNAMES.ADD_PARTICIPANT, (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.ADD_PARTICIPANT} >>>>>>`, req);
        this.saveAddParticipants(req, meetInfo);
      });

      socket.on(EVENTNAMES.HOST_ALLOW_SCREENSHARE, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.HOST_ALLOW_SCREENSHARE} >>>>>>`, req);
        await this.AllowScreenShare(req, meetInfo);
      });

      socket.on(EVENTNAMES.HOST_REJECT_SCREENSHARE_REQUEST, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.HOST_REJECT_SCREENSHARE_REQUEST} >>>>>>`, req);
        await this.hostRejectScreenShareRequest(req, meetInfo);
      })

      socket.on(EVENTNAMES.PARTICIPANT_REQUEST_SCREENSHARE, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.PARTICIPANT_REQUEST_SCREENSHARE} >>>>>>`, req);
        await this.participantRequesttoScreenShare(req, meetInfo);
      })

      //End meeting //

      socket.on(EVENTNAMES.END_MEETING, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.END_MEETING} >>>>>>`, req);
        this.endMeetingAll(req, meetInfo);
      });

      //Move to waiting room

      socket.on(EVENTNAMES.MOVE_TO_WAITING_ROOM, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.MOVE_TO_WAITING_ROOM} >>>>>>`, req);
        this.moveWaitingroom(req, meetInfo);
      });

      //Join meeting

      socket.on(EVENTNAMES.JOINMEETING, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.JOINMEETING} >>>>>>`, req);
        await this.joinMeetingroom(req, meetInfo);
      });

      //Remove meeting

      socket.on(EVENTNAMES.REMOVE_FROM_MEETING, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.REMOVE_FROM_MEETING} >>>>>>`, req);
        await this.removefromMeeting(req, meetInfo);
      });

      //Make as admin

      socket.on(EVENTNAMES.MAKE_AS_ADMIN, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.MAKE_AS_ADMIN} >>>>>>`, req);
        await this.makeasAdmin(req, meetInfo);
      });

      //Get chat list

      socket.on(EVENTNAMES.GET_CHAT_LIST, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.GET_CHAT_LIST} >>>>>>`, req);
        if (
          req.fromsocketId == "" &&
          req.tosocketId == "" &&
          req.chatType == ""
        ) {
          await this.emitEvent(
            EVENTNAMES.GET_CHAT_LIST,
            socket.id,
            "Invalid inputs"
          );
        } else {
          if (
            req.chatType &&
            req.chatType.toLowerCase() == "private" &&
            !req.fromsocketId &&
            !req.tosocketId
          ) {
            await this.emitEvent(
              EVENTNAMES.GET_CHAT_LIST,
              socket.id,
              "Invalid inputs"
            );
          } else {
            this.getChatsbyId(req, meetInfo);
          }
        }
      });

      //Rename change

      socket.on(EVENTNAMES.RENAME_CHANGED, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.RENAME_CHANGED} >>>>>>`, req);
        await this.setRenameChange(req, meetInfo);
      });

      //Multiple participants

      socket.on(EVENTNAMES.REMOVE_MULTIPLE_PARTICIPANTS, async (req: any) => {
        logger.info(
          `<<<< ${EVENTNAMES.REMOVE_MULTIPLE_PARTICIPANTS} >>>>>>`,
          req
        );

        this.multipleRemovefromMeeting(req, meetInfo);
      });

      //Chat setting

      socket.on(EVENTNAMES.CHAT_SETTING, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.CHAT_SETTING} >>>>>>`, req);

        await this.setChatSetting(req, meetInfo);
      });

      //General setting
      socket.on(EVENTNAMES.GENERAL_SETTING, (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.GENERAL_SETTING} >>>>>>`, req);

        this.setGeneralMetting(meetingId, jwtToken, req, deviceid);
      });

      socket.on(EVENTNAMES.UPDATE_MEETING_TITLE, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.UPDATE_MEETING_TITLE} >>>>>>`, req);

        await this.updateMeetingTitle(meetingId, jwtToken, req, deviceid);
      });

      socket.on(EVENTNAMES.CO_BROWSER_REQ, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.CO_BROWSER_REQ} >>>>>>`, req);
        await this.reqCoBrowse(req, meetInfo);
      })

      socket.on(EVENTNAMES.CO_BROWSER_ACCEPTED, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.CO_BROWSER_ACCEPTED} >>>>>>`, req);
        await this.acceptCoBrowse(req, meetInfo);
      })
      
      socket.on(EVENTNAMES.CO_BROWSER_DECLINED, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.CO_BROWSER_DECLINED} >>>>>>`, req);
        await this.declineCoBrowse(req, meetInfo);
      })
      socket.on(EVENTNAMES.UPDATE_CANVAS_SIZE, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.UPDATE_CANVAS_SIZE} >>>>>>`, req);
        await this.updateCanvaSize(req, meetInfo);
      })



      //Rename change
      socket.on(EVENTNAMES.SDP_RECEIVE_FROM_CLIENT, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.SDP_RECEIVE_FROM_CLIENT} >>>>>>`, req);
        await this.receiveSDPfromClient(req, meetInfo);
      });

      socket.on('disconnecting', async (reason: any) => {
        logger.info(`<<<< ${EVENTNAMES.SDP_RECEIVE_FROM_CLIENT} >>>>>>`, reason);

        console.log('disconnecting >>>>>', reason);

      });

      //Disconnect
      socket.on("disconnect", async (e: any) => {
        logger.info(`<<<< disconnect >>>>>>`, e);
        console.log('disconnect calling>>>>>>.')
        let { meetingId, jwtToken, sipLoginId, socketId, sipInfo } = meetInfo;
        //  await this.stopClosecaption({ socketId: `${socketId}` }, meetInfo);
        const adapter: RedisAdapter = this.io.of("/").adapter as RedisAdapter;
        const meetingdetails: any = await this.redisadaptor.getMeetingRedis(
          meetingId
        );

        console.log("disconnect >>>>>>>>", socketId, meetingId, e);
        logger.info("disconnect >>>>>>>>", socketId, meetingId, e);

        let Sipid_details: any = _.findWhere(meetingdetails.devicesInfo, {
          conId: socketId,
        });


        let delay = Sipid_details?.delaySec || 1;

        console.log("delay >>>>>>>>", socketId, `<<<<< delay is >>>>>${delay}`);
        logger.info("delay >>>>>>>>", socketId, `<<<<< delay is >>>>>${delay}`);

        if (e === "transport close") {

          console.log("transport close calling");
          logger.info(`${socketId} transport close calling`);


          console.log("setTimeout transport close calling");
          logger.info(`${socketId} setTimeout transport close calling`);
          let meet_info = meetInfo;

          let is_Disconneted = isDisconneted


          let participant: any = _.findWhere(meetingdetails.devicesInfo, {
            conId: socketId,
          });


          let screen_participant: any = _.findWhere(meetingdetails.devicesInfo, {
            conId: `${socketId}_screenshare`,
          });

          if (screen_participant && screen_participant.isScreenshare) {

            screen_participant.status = "transport_error";
            screen_participant.closedCaption = false;

            screen_participant.isScreenshare = false;

            screen_participant.isWhiteboard = false;

            await this.rocketMQMiddleware.connectionRocket(
              meetingdetails,
              [screen_participant],
              EVENTNAMES.SCREENSHARE_STOP,
              screen_participant.conId
            );

            await this.emitEvent(EVENTNAMES.SCREEN_SHARE, meetingId, {
              message: false,
              socketid: screen_participant.conId,
              meetingdetails: meetingdetails,
            });

            let sdeviceid: any = screen_participant.deviceid;
            if (meetingdetails.devicesInfo.hasOwnProperty(sdeviceid)) {
              meetingdetails.devicesInfo[sdeviceid] = screen_participant;
            }
          }


          if (participant) {

            participant.status = "transport_error";

            participant.closedCaption = false;

            participant.isScreenshare = false;

            participant.isWhiteboard = false;

            // Sipid_details.status = "disconnected";

            let deviceid: any = participant.deviceid;
            if (meetingdetails.devicesInfo.hasOwnProperty(deviceid)) {
              meetingdetails.devicesInfo[deviceid] = participant;
            }

          }
          if (is_Disconneted === false) {

            console.log("disconnect boolean value false  section calling")
            logger.info(`${socketId} disconnect boolean value false  section callin`)



            if (Sipid_details) Sipid_details.isReconnectstatus = true;
            let deviceid: any = Sipid_details?.deviceid || "";
            if (meetingdetails.devicesInfo.hasOwnProperty(deviceid)) {
              meetingdetails.devicesInfo[deviceid] = Sipid_details;
            }



            /*  await this.emitEvent(EVENTNAMES.POOR_NETWORK_EVENT, meetingId, {
               meetingdetails: meetingdetails,
               socketid: socketId,
             }); */

            let delay_sec = delay * 100;
            setTimeout(async () => {
              await this.transportErrorHandling(meet_info, delay_sec)
            }, delay * 200)
          } else {

            console.log("disconnect boolean value true section calling");
            logger.info(`${socketId} disconnect boolean value true  section callin`)
            isDisconneted = false
          }

          await this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);




        } else {
          console.log("Other disconnect error calling")
          logger.info(`${socketId} Other disconnect error calling`)

          if (e === "ping timeout") {

            if (Sipid_details) Sipid_details.isReconnectstatus = true;
            let deviceid: any = Sipid_details?.deviceid || "";
            if (meetingdetails.devicesInfo.hasOwnProperty(deviceid)) {
              meetingdetails.devicesInfo[deviceid] = Sipid_details;
            }


            await this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);

            await this.emitEvent(EVENTNAMES.POOR_NETWORK_EVENT, meetingId, {
              meetingdetails: meetingdetails,
              socketid: socketId,
            });
          }

          setTimeout(async () => {

            console.log("setTimeout Other disconnect error calling")
            logger.info(`${socketId} setTimeout Other disconnect error calling`)
            let meet_info = meetInfo;
            let error = e;
            let sipdetails = Sipid_details
            await this.handleDisconnectEvent(sipdetails, error, meet_info, delay)
          }, delay * 200)
        }



        // if (Sipid_details) {
        //   Sipid_details.closedCaption = false;

        //   Sipid_details.isScreenshare = false;

        //   Sipid_details.isWhiteboard = false;

        //   // Sipid_details.status = "disconnected";

        //   if (
        //     meetingdetails.devicesInfo.hasOwnProperty(Sipid_details.deviceid)
        //   ) {
        //     meetingdetails.devicesInfo[Sipid_details.deviceid] = Sipid_details;
        //   }
        // }

        // let screenstatus_filter = _.filter(meetingdetails.devicesInfo, {
        //   isScreenshare: true,
        // });

        // let whitestatus_filter = _.filter(meetingdetails.devicesInfo, {
        //   isWhiteboard: true,
        // });



        // if (screenstatus_filter.length === 0) {
        //   meetingdetails.isScreenshareStarted = false;
        //   //  this.speechmatics.stopClosedcaption();
        // }

        // if (whitestatus_filter.length === 0) {
        //   meetingdetails.isWhiteboardStarted = false;
        //   //  this.speechmatics.stopClosedcaption();
        // }

        // if (e === "ping timeout") {
        //   if (Sipid_details) {

        //     /*  await this.rocketMQMiddleware.connectionRocket(
        //        meetingdetails,
        //        [Sipid_details],
        //        EVENTNAMES.USER_TERMINATED,
        //        socketId
        //      );

        //      let filterList = _.without(meetingdetails.joinedParticipantlist, _.findWhere(meetingdetails.joinedParticipantlist, {
        //        conId: socketId
        //      }));

        //      meetingdetails.joinedParticipantlist = filterList;
        //      Sipid_details.status = "disconnected";
        //      //  await this.rocketMQMiddleware.connectionRocket(
        //      //   meetingdetails,
        //      //   [Sipid_details],
        //      //   EVENTNAMES.UPDATE_PARTICIPANTLIST,
        //      //   socketId
        //      // ); 
        //      await this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);
        //      await this.emitEvent(
        //        EVENTNAMES.CLIENT_UPDATED_PARTICIPANTLIST,
        //        meetingdetails.meetingId,
        //        {
        //          participantList: meetingdetails.joinedParticipantlist,
        //        }
        //      ); */
        //     //if (Sipid_details.isHost) {
        //     this.deleteConnection(meetInfo);
        //     //}

        //   }
        // } else if (e === "transport close") {

        //   if (isDisconneted === false) {

        //     console.log("disconnect boolean value false  section calling")
        //     await this.transportErrorHandling(meetInfo)
        //   } else {

        //     console.log("disconnect boolean value true section calling");
        //     isDisconneted = false
        //   }



        //   /*         let filterList = _.without(meetingdetails.joinedParticipantlist, _.findWhere(meetingdetails.joinedParticipantlist, {
        //             conId: socketId
        //           }));

        //           meetingdetails.joinedParticipantlist = filterList;


        //           await this.rocketMQMiddleware.connectionRocket(
        //             meetingdetails,
        //             [Sipid_details],
        //             EVENTNAMES.USER_TERMINATED,
        //             socketId
        //           );

        //           await this.emitEvent(
        //             EVENTNAMES.MEETING_STATUS,
        //             meetingdetails.meetingId,
        //             {
        //               message: `${Sipid_details.name} Disconneted from the meeting`,
        //               socketid: Sipid_details.conId,
        //               meetingdetails: meetingdetails,
        //             }
        //           );


        //           await this.emitEvent(
        //             EVENTNAMES.CLIENT_UPDATED_PARTICIPANTLIST,
        //             meetingdetails.meetingId,
        //             {
        //               participantList: meetingdetails.joinedParticipantlist,
        //             }
        //           );




        //           if (Sipid_details) {
        //             Sipid_details.status = "disconnected";

        //             if (
        //               meetingdetails.devicesInfo.hasOwnProperty(Sipid_details.deviceid)
        //             ) {
        //               meetingdetails.devicesInfo[Sipid_details.deviceid] =
        //                 Sipid_details;
        //             }
        //           }

        //           await this.redisadaptor.setMeetingDetails(meetingId, meetingdetails); */

        // } else if (e === "transport error") {




        //   let filterList = _.without(meetingdetails.joinedParticipantlist, _.findWhere(meetingdetails.joinedParticipantlist, {
        //     conId: socketId
        //   }));

        //   meetingdetails.joinedParticipantlist = filterList;

        //   /*  await this.rocketMQMiddleware.connectionRocket(
        //      meetingdetails,
        //      [Sipid_details],
        //      EVENTNAMES.UPDATE_PARTICIPANTLIST,
        //      socketId
        //    ); */

        //   //if (Sipid_details !== 'disconnected') {
        //   await this.rocketMQMiddleware.connectionRocket(
        //     meetingdetails,
        //     [Sipid_details],
        //     EVENTNAMES.USER_TERMINATED,
        //     socketId
        //   );

        //   await this.emitEvent(
        //     EVENTNAMES.MEETING_STATUS,
        //     meetingdetails.meetingId,
        //     {
        //       message: `${Sipid_details.name} Disconneted from the meeting`,
        //       socketid: Sipid_details.conId,
        //       meetingdetails: meetingdetails,
        //     }
        //   );


        //   await this.emitEvent(
        //     EVENTNAMES.CLIENT_UPDATED_PARTICIPANTLIST,
        //     meetingdetails.meetingId,
        //     {
        //       participantList: meetingdetails.joinedParticipantlist,
        //     }
        //   );


        //   //}

        //   if (Sipid_details) {
        //     Sipid_details.status = "disconnected";

        //     if (
        //       meetingdetails.devicesInfo.hasOwnProperty(Sipid_details.deviceid)
        //     ) {
        //       meetingdetails.devicesInfo[Sipid_details.deviceid] =
        //         Sipid_details;
        //     }
        //   }

        //   await this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);

        // } else {




        //   if (Sipid_details && e !== "server namespace disconnect") {
        //     Sipid_details.status = "disconnected";

        //     if (
        //       meetingdetails.devicesInfo.hasOwnProperty(Sipid_details.deviceid)
        //     ) {
        //       meetingdetails.devicesInfo[Sipid_details.deviceid] =
        //         Sipid_details;
        //     }

        //     console.log("<<<<<<<<<<DISCONNECT EVENT CALLOING>>>>>>>>>>")
        //     await this.rocketMQMiddleware.connectionRocket(
        //       meetingdetails,
        //       [Sipid_details],
        //       EVENTNAMES.USER_TERMINATED,
        //       socketId
        //     );

        //     let filterList = _.without(meetingdetails.joinedParticipantlist, _.findWhere(meetingdetails.joinedParticipantlist, {
        //       conId: socketId
        //     }));

        //     meetingdetails.joinedParticipantlist = filterList;
        //     /*  await this.rocketMQMiddleware.connectionRocket(
        //        meetingdetails,
        //        [Sipid_details],
        //        EVENTNAMES.UPDATE_PARTICIPANTLIST,
        //        socketId
        //      ); */
        //     await this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);



        //     //if(e !== "transport close"){

        //     await this.emitEvent(
        //       EVENTNAMES.MEETING_STATUS,
        //       meetingdetails.meetingId,
        //       {
        //         message: `${Sipid_details.name} Disconneted from the meeting`,
        //         socketid: Sipid_details.conId,
        //         meetingdetails: meetingdetails,
        //       }
        //     );

        //     await this.emitEvent(
        //       EVENTNAMES.CLIENT_UPDATED_PARTICIPANTLIST,
        //       meetingdetails.meetingId,
        //       {
        //         participantList: meetingdetails.joinedParticipantlist,
        //       }
        //     );

        //     //}

        //   }
        // }
      });
      //Active Speaker
      socket.on(EVENTNAMES.ACTIVE_SPEAKER, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.ACTIVE_SPEAKER} >>>>>>`, req);
        await this.activeSpeaker(req, meetInfo);
      });

      //Remove meeting

      socket.on(EVENTNAMES.REMOVE_FROM_MEETING, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.REMOVE_FROM_MEETING} >>>>>>`, req);
        await this.removefromMeeting(req, meetInfo);
      });

      //speaker language
      socket.on(EVENTNAMES.MIC_LANGUAGE, async (req: any) => {
        console.log("req>>", req);
        this.updateMicLang(req, meetInfo);
      })

      // Hand Raise
      socket.on(EVENTNAMES.HAND_RAISE, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.HAND_RAISE} >>>>>>`, req);
        await this.handRaiseMeeting(req, meetInfo);
      });
      /**
       * Description: This event used to all lower hand raised
       * Event Name: LOWER_ALL_HAND_RAISE
       */

      socket.on(EVENTNAMES.LOWER_ALL_HAND_RAISE, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.LOWER_ALL_HAND_RAISE} >>>>>>`, req);
        await this.lowerAllHandRaise(req, meetInfo);
      });
      /**
       * Description: This event used to enable single recording
       * Event Name: ENABLE_RECORDING
       */

      socket.on(EVENTNAMES.ENABLE_RECORDING, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.ENABLE_RECORDING} >>>>>>`, req);
        await this.enableRecordMeeting(req, meetInfo);
      });
      /**
       * Description: This event used to move to Enable Recording for all
       * Event Name: MULTIPLE_ENABLE_RECORDING
       */
      socket.on(EVENTNAMES.MULTIPLE_ENABLE_RECORDING, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.MULTIPLE_ENABLE_RECORDING} >>>>>>`, req);
        await this.allenableRecording(req, meetInfo);
      });
      /**
       * Description: This event used to move to WaitingRoom for all
       * Event Name: MULTIPLE_TO_WAITING_ROOM
       */
      socket.on(EVENTNAMES.MULTIPLE_TO_WAITING_ROOM, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.MULTIPLE_TO_WAITING_ROOM} >>>>>>`, req);
        await this.multipleWaitingRoom(req, meetInfo);
      });
      /**
       * Description: This event used to update whiteboard points
       * Event Name: MULTIPLE_TO_WAITING_ROOM
       */

      socket.on(EVENTNAMES.WHITE_BOARD, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.WHITE_BOARD} >>>>>>`, req);
        await this.sendwhiteBoard(req, meetInfo);
      });
      /**
       * Description: This event used to get whiteboard points
       * Event Name: MULTIPLE_TO_WAITING_ROOM
       */
      socket.on(EVENTNAMES.GET_WHITE_BOARD, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.GET_WHITE_BOARD} >>>>>>`, req);
        await this.getWhiteBoardlist(req, meetInfo);
      });
      /**
       * Description: This event used to audio mute & unmute
       * Event Name: AUDIO_MUTE_UNMUTE
       */
      socket.on(EVENTNAMES.AUDIO_MUTE_UNMUTE, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.AUDIO_MUTE_UNMUTE} >>>>>>`, req);
        await this.audioMuteUnmute(req,meetInfo);
      });

      socket.on(EVENTNAMES.HIDE_ALL, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.HIDE_ALL} >>>>>>`, req);
        await this.videoHideAll(req,meetInfo);
      });
      /**
       * Description: This event used to video mute & unmute
       * Event Name: VIDEO_HIDE_SHOW
       */
      socket.on(EVENTNAMES.VIDEO_HIDE_SHOW, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.VIDEO_HIDE_SHOW} >>>>>>`, req);
        await this.videoHideShow(req, meetInfo);
      });
      /**
       * Description: This event used to all participants audio mute
       * Event Name: MUTE_ALL
       */
      socket.on(EVENTNAMES.MUTE_ALL, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.MUTE_ALL} >>>>>>`, req);
        await this.audioMuteAll(req, meetInfo);
      });

      socket.on(EVENTNAMES.WAITINGROOM_RETRY, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.WAITINGROOM_RETRY} >>>>>>`, req);
        await this.denyWaitingroom(req, meetInfo);
      });

      socket.on(EVENTNAMES.HOST_REQUEST_UNMUTE, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.HOST_REQUEST_UNMUTE} >>>>>>`, req);
        await this.hostRequesttoMute(req, meetInfo);
      });

      socket.on(EVENTNAMES.PARTICIPANT_REQUEST_UNMUTE, async (req: any) => {
        logger.info(
          `<<<< ${EVENTNAMES.PARTICIPANT_REQUEST_UNMUTE} >>>>>>`,
          req
        );
        await this.participantRequesttoUnMute(req, meetInfo);
      });

      socket.on(EVENTNAMES.HOST_REQUEST_UNMUTEALL, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.HOST_REQUEST_UNMUTEALL} >>>>>>`, req);
        await this.hostRequesttoMuteAll(req, meetInfo);
      });

      socket.on(EVENTNAMES.AGAIN_RETRY_WATINGROOM, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.AGAIN_RETRY_WATINGROOM} >>>>>>`, req);
        await this.retryWaitingroom(req, meetInfo);
      });

      socket.on(EVENTNAMES.ADMIT_ALL, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.ADMIT_ALL} >>>>>>`, req);

        await this.admitAll(req, meetInfo);
      });

      socket.on(EVENTNAMES.PARTICIPANT_REQUEST_RECORING, async (req: any) => {
        logger.info(
          `<<<< ${EVENTNAMES.PARTICIPANT_REQUEST_RECORING} >>>>>>`,
          req
        );
        await this.participantRequesttoRecording(req, meetInfo);
      });

      socket.on(EVENTNAMES.HOST_REJECT_RECORING_REQUEST, async (req: any) => {
        logger.info(
          `<<<< ${EVENTNAMES.HOST_REJECT_RECORING_REQUEST} >>>>>>`,
          req
        );
        await this.hostRejectRecordRequest(req, meetInfo);
      });

      socket.on(EVENTNAMES.RECORDING_ACTION, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.RECORDING_ACTION} >>>>>>`, req);
        await this.startRecording(req, meetInfo);
      });

      socket.on(EVENTNAMES.TRANSFER_INITIATE, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.TRANSFER_INITIATE} >>>>>>`, req);
        await this.initiateTransfer(req, meetInfo);
      });

      socket.on(EVENTNAMES.STOP_RECORDING, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.STOP_RECORDING} >>>>>>`, req);
        await this.stopRecording(req, meetInfo);
      });

      socket.on(EVENTNAMES.START_CLOSE_CAPTION, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.START_CLOSE_CAPTION} >>>>>>`, req);
        await this.startClosecaption(req, meetInfo);
        // this.speechmatics.startConnection(meetInfo);
      });

      /*       socket.on(EVENTNAMES.SEND_VOICE_DATA, async (req: any) => {
              logger.info(`<<<< ${EVENTNAMES.SEND_VOICE_DATA} >>>>>>`, req);
      
              this.speechmatics.receiveVoicedata(req.voicedata);
            }); */


      socket.on(EVENTNAMES.GET_TRANSCRIPTS, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.GET_TRANSCRIPTS} >>>>>>`, req);



        await this.getAllTranscripts(req, meetInfo);

      });

      socket.on(EVENTNAMES.TRANSLATE_TRANSCRIPT, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.TRANSLATE_TRANSCRIPT} >>>>>>`, req);

        await this.translateTrascript(req, meetInfo);

      });






      // socket.on(EVENTNAMES.SEND_VOICE_DATA, async (req: any) => {
      //   logger.info(`<<<< ${EVENTNAMES.SEND_VOICE_DATA} >>>>>>`, req);

      //   //await this.broadcastClosecaption(req, meetInfo);

      // });

      socket.on(EVENTNAMES.STORE_CLOSE_CAPTION, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.SEND_VOICE_DATA} >>>>>>`, req);

        // this.speechmatics.receiveVoicedata(req.voicedata);

        await this.storeFullClosecaption(req, meetInfo);

      });


      socket.on(EVENTNAMES.SEARCH_TRANSCRIPT, async (req: any) => {

        await this.searchTranscript(req, meetInfo)
        //this.speechmatics.receiveVoicedata(req.voicedata);
      });

      socket.on(EVENTNAMES.FILTER_TRANSCRIPT, async (req: any) => {

        await this.filterTranscript(req, meetInfo)
        //this.speechmatics.receiveVoicedata(req.voicedata);
      });


      socket.on(EVENTNAMES.UPDATE_TRANSCRIPT, async (req: any) => {

        await this.updateTranscript(req, meetInfo);
        //this.speechmatics.receiveVoicedata(req.voicedata);
      });


      socket.on(EVENTNAMES.END_CLOSE_CAPTION, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.END_CLOSE_CAPTION} >>>>>>`, req);
        await this.stopClosecaption(req, meetInfo);

        //this.speechmatics.stopClosedcaption();
      });

      socket.on(EVENTNAMES.SCREEN_SHARE, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.SCREEN_SHARE} >>>>>>`, req);
        await this.screenShareaction(req, meetInfo);

        //this.speechmatics.stopClosedcaption();
      });

      socket.on(EVENTNAMES.MIRROR_UPDATE, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.MIRROR_UPDATE} >>>>>>`, req);
        await this.updateMirror(req, meetInfo);

        //this.speechmatics.stopClosedcaption();
      });

      socket.on(EVENTNAMES.GET_KEYWORDS, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.GET_KEYWORDS} >>>>>>`, req);
        await this.getKeywords(req, meetInfo);

        //this.speechmatics.stopClosedcaption();
      });

      socket.on(EVENTNAMES.ADD_KEYWORD, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.ADD_KEYWORD} >>>>>>`, req);
        await this.addKeywords(req, meetInfo);

        //this.speechmatics.stopClosedcaption();
      });


      socket.on(EVENTNAMES.REMOVE_KEYWORD, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.REMOVE_KEYWORD} >>>>>>`, req);
        await this.removeKeyword(req, meetInfo);

        //this.speechmatics.stopClosedcaption();
      });


      socket.on(EVENTNAMES.AUTOMATEDMINUTES, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.AUTOMATEDMINUTES} >>>>>>`, req);
        await this.automatedMinutes(req, meetInfo);

        //this.speechmatics.stopClosedcaption();
      });


      socket.on(EVENTNAMES.MANUALLY_DISCONNECT, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.MANUALLY_DISCONNECT} >>>>>>`, req);

        isDisconneted = true;
        await this.handleManuallyDisconnect(req, meetInfo);

        //this.speechmatics.stopClosedcaption();
      });

      socket.on(EVENTNAMES.RECONNECT_SCREENSHARE, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.RECONNECT_SCREENSHARE} >>>>>>`, req);
        await this.handleScreenshareReconnect(req, meetInfo);

        //this.speechmatics.stopClosedcaption();
      });


      socket.on(EVENTNAMES.JOIN_SCREENSHARE, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.JOIN_SCREENSHARE} >>>>>>`, req);
        console.log(`<<<< ${EVENTNAMES.JOIN_SCREENSHARE} >>>>>>`, req);
        await this.joinScreenShare(req, meetInfo);

        //this.speechmatics.stopClosedcaption();
      });

      socket.on(EVENTNAMES.PINNED_USER, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.PINNED_USER} >>>>>>`, req);
        console.log(`<<<< ${EVENTNAMES.PINNED_USER} >>>>>>`, req);
        await this.handlePinnedUser(req, meetInfo);

        //this.speechmatics.stopClosedcaption();
      });

      socket.on(EVENTNAMES.TILE_CHANGE_REQUEST, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.TILE_CHANGE_REQUEST} >>>>>>`, req);
        console.log(`<<<< ${EVENTNAMES.TILE_CHANGE_REQUEST} >>>>>>`, req);
        await this.handleTilechange(req, meetInfo);

        //this.speechmatics.stopClosedcaption();
      });

      socket.on(EVENTNAMES.SCREENSHARE_VIDEO_REQ, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.SCREENSHARE_VIDEO_REQ} >>>>>>`, req);
        console.log(`<<<< ${EVENTNAMES.SCREENSHARE_VIDEO_REQ} >>>>>>`, req);
        await this.handleScreenshareVideo(req, meetInfo);
      });

      socket.on(EVENTNAMES.ACTIVE_SPEAKER_VIDEO_REQ, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.ACTIVE_SPEAKER_VIDEO_REQ} >>>>>>`, req);
        console.log(`<<<< ${EVENTNAMES.ACTIVE_SPEAKER_VIDEO_REQ} >>>>>>`, req);
        await this.handleActiveSpeakerVideo(req, meetInfo);
      });


      socket.on(EVENTNAMES.POOR_NETWORK_EVENT, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.POOR_NETWORK_EVENT} >>>>>>`, req);
        console.log(`<<<< ${EVENTNAMES.POOR_NETWORK_EVENT} >>>>>>`, req);
        await this.lowNetworkdetection(req, meetInfo);
      });

      socket.on(EVENTNAMES.VIDEO_RESOLUTION_REQ, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.VIDEO_RESOLUTION_REQ} >>>>>>`, req);
        console.log(`<<<< ${EVENTNAMES.VIDEO_RESOLUTION_REQ} >>>>>>`, req);
        await this.handleVideoresolution(req, meetInfo);
      });

      socket.on(EVENTNAMES.VERIFICATION_REQ, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.VERIFICATION_REQ} >>>>>>`, req);
        console.log(`<<<< ${EVENTNAMES.VERIFICATION_REQ} >>>>>>`, req);
        await this.handleVerificationReqRes(req, meetInfo);
      });

      socket.on(EVENTNAMES.VERIFICATION_DETAILS, async (req: any) => {
        logger.info(`<<<< ${EVENTNAMES.VERIFICATION_REQ} >>>>>>`, req);
        console.log(`<<<< ${EVENTNAMES.VERIFICATION_REQ} >>>>>>`, req);
        await this.handleVerificationDetails(req, meetInfo);
      });



    });
  }

  private async connectionManager(
    meetingdetails: any,
    socket: any,
    meetInfo: any
  ) {
    try {
      const {
        meetingId,
        jwtToken,
        sipLoginId,
        socketId,
        deviceid,
        audio,
        video,
        isTransfer
      } = meetInfo;

      // let active_connection: any = meetingdetails.activeConn || [];
      //let joined_connection: any = meetingdetails.joinedConn || [];
      //let waiting_connection: any = meetingdetails.waitingroomConn || [];

      //  active_connection.push(socketId);

      // meetingdetails.activeConn = active_connection;

      // console.log("<<<<<<<<<<<,Connection Manager calling >>>>>>>",meetingdetails);
      logger.info("<<<<<<<<<<<,Connection Manager calling >>>>>>>", meetInfo);

      let part: any = { ...meetingdetails.participants[sipLoginId.toString()] };
      let isWaitingroom = false;

      let existDeviceid_details: any = _.findWhere(meetingdetails.devicesInfo, {
        deviceid: deviceid,
      });
      let existSipid_details: any = _.findWhere(meetingdetails.devicesInfo, {
        sipId: sipLoginId,
      });

      // console.log("<<<<<<<<<<<,existDeviceid_details >>>>>>>",existDeviceid_details);
      // console.log("<<<<<<<<<<<,existSipid_details >>>>>>>",existSipid_details);
      if (part.isGuest) isWaitingroom = false;



      if (existDeviceid_details && existDeviceid_details.sipId == sipLoginId) {

        console.log("<<<<<<<<<<<,Match existDeviceid_details calling >>>>>>>");
        logger.info("<<<<<<<<<<<,Match existDeviceid_details calling >>>>>>>", meetInfo);
        isWaitingroom = existDeviceid_details.moveToWaitingRoom;

        await this.rocketMQMiddleware.connectionRocket(
          meetingdetails,
          [existSipid_details],
          EVENTNAMES.USER_TERMINATED,
          existDeviceid_details.conId
        );

        let filterList = _.without(meetingdetails.joinedParticipantlist, _.findWhere(meetingdetails.joinedParticipantlist, {
          deviceid: existDeviceid_details.deviceid
        }));

        let _filterList = _.without(filterList, _.findWhere(filterList, {
          deviceid: `${existDeviceid_details.deviceid}_screenshare`
        }));

        meetingdetails.joinedParticipantlist = _filterList;


        if (existDeviceid_details && existDeviceid_details?.deviceid == deviceid && (existDeviceid_details?.status == "transport_error" || existDeviceid_details?.status == "dtls_connection_failure" || existDeviceid_details?.status == "joined" || existDeviceid_details?.status == 'network failure')) {

          logger.info("<<<<<<<<<<<,existDeviceid_details >>>>>>>", existDeviceid_details);
          let dis_meetingdetails = meetingdetails;
          let duplicate_existDeviceid_details = existDeviceid_details;
          duplicate_existDeviceid_details.status = "disconnected";

          dis_meetingdetails.devicesInfo[deviceid] = duplicate_existDeviceid_details;
          await this.emitEvent(
            EVENTNAMES.MEETING_STATUS,
            meetingdetails.meetingId,
            {
              message: `${existDeviceid_details.name} Disconnected from the meeting`,
              socketid: existDeviceid_details.conId,
              meetingdetails: dis_meetingdetails,
            }
          );

          await this.emitEvent(
            EVENTNAMES.CLIENT_UPDATED_PARTICIPANTLIST,
            meetingdetails.meetingId,
            {
              participantList: meetingdetails.joinedParticipantlist,
            }
          );
        }


        let screen_participant: any = _.findWhere(meetingdetails.devicesInfo, {
          deviceid: `${deviceid}_screenshare`,
        });

        console.log("<<<<<<screen_participant >>>>>>>", screen_participant);
        if (screen_participant && screen_participant.isScreenshare) {

          logger.info("<<<<<<<<<<<,screen_participant details >>>>>>>", screen_participant);
          let cpy_existDeviceid_details = screen_participant;
          cpy_existDeviceid_details.closedCaption = false;

          cpy_existDeviceid_details.isScreenshare = false;

          cpy_existDeviceid_details.isWhiteboard = false;

          let sdeviceid: any = cpy_existDeviceid_details.deviceid;
          console.log("<<<<<<cpy_existDeviceid_details >>>>>>>", cpy_existDeviceid_details);
          if (meetingdetails.devicesInfo.hasOwnProperty(sdeviceid)) {
            meetingdetails.devicesInfo[sdeviceid] = cpy_existDeviceid_details;
          }

          console.log(" meetingdetails.devicesInfo[sdeviceid] >>>>>>>", meetingdetails.devicesInfo[sdeviceid]);
          let screenstatus_filter = _.filter(meetingdetails.devicesInfo, {
            isScreenshare: true,
          });

          let whitestatus_filter = _.filter(meetingdetails.devicesInfo, {
            isWhiteboard: true,
          });

          if (screenstatus_filter.length === 0) {
            meetingdetails.isScreenshareStarted = false;
            //  this.speechmatics.stopClosedcaption();
          }

          if (whitestatus_filter.length === 0) {
            meetingdetails.isWhiteboardStarted = false;
            //  this.speechmatics.stopClosedcaption();
          }

          await this.rocketMQMiddleware.connectionRocket(
            meetingdetails,
            [cpy_existDeviceid_details],
            EVENTNAMES.SCREENSHARE_STOP,
            cpy_existDeviceid_details.conId
          );

          console.log(" <<<<<<<meetingdetails.devicesInfo[sdeviceid] >>>>>>>", JSON.stringify(meetingdetails));
          await this.emitEvent(EVENTNAMES.SCREEN_SHARE, meetingId, {
            message: false,
            socketid: cpy_existDeviceid_details.conId,
            meetingdetails: meetingdetails,
          });
        }


        logger.info("<<<<<<<<<<<,existDeviceid_details set meeting details >>>>>>>", JSON.stringify(meetingdetails));
        await this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);
      }

      if (
        meetingdetails.hostDetails &&
        Object.keys(meetingdetails.hostDetails).length > 0 &&
        meetingdetails.hostDetails.deviceid !== deviceid &&
        meetingdetails.isHostjoined
      )
        // part.isHost = false; call is transferred and when  sec agent joins isHost turns false so commented

      part.video = video;
      part.mute = audio;
      part.isMirror = true;
      if (part.isHost || !isWaitingroom) {
        socket.join(meetingId);

        // console.log("meetingdetails.devicesInfo^^^^^^^^^^^^^^^",meetingdetails.devicesInfo)
        part.isHostAllowed = true;
        if (
          !meetingdetails.devicesInfo.hasOwnProperty(deviceid) &&
          !existSipid_details
        ) {
          // meetingdetails.devices_info[deviceid] = {};
          if (part.isHost) {
            // Inprogress

            const findsip: any = await this.redisadaptor.getMeetingRedis(
              meetingId,
            );

            console.log(`<<<<<,Bfore calling the update api>>>>> ${new Date().getTime()}`)
            findsip.status = "Ongoing";
            findsip.is_attended = 1;
            let sipid = Object.keys(meetingdetails.participants) || [];


            ioredis.to(sipid).emit("ongoing_notification", meetingdetails);

            console.log(`<<<<<,After calling the update api>>>>> ${new Date().getTime()}`)
            if (!meetingdetails.isHostMyVideo) {
              part.video = false;
            }


            part.isRecordingAllowed = true;
            let existHost_details: any = _.findWhere(
              meetingdetails.devicesInfo,
              { isHost: true }
            );

            if (existHost_details && existHost_details.deviceid != deviceid) {
              // existHost_details.isHost = false;//when transferred to same agent again isHost turns false so commented
              existHost_details.isRecordingAllowed = false;
              existHost_details.roleType = 3;

              if (
                meetingdetails.alternateHostDetails.deviceid ==
                existHost_details.deviceid
              )
                existHost_details.isAlternateHost = true;
              existHost_details.roleType = 2;

              meetingdetails.devicesInfo[existHost_details.deviceid] =
                existHost_details;
            }

            meetingdetails.isHostjoined = false;
            meetingdetails.hostDetails = part;
            meetingdetails.hostDetails.deviceid = deviceid;

            const { devicesInfo, generalSetting, isParticipantjoinbefhost, activeConn, alternateHostDetails, chatSetting, conversations, globalKeywords, joinedConn, joinedParticipantlist, joinedParticipants, participants, waitingroomConn,
              ...other } = meetingdetails;

            let sipidList: any = meetingdetails.sipIdList || [];

            console.log("sipidList >>>>>>", sipidList);
            console.log("<<<<<<<<<<<<<< AGENT JOINED >>>>>>>>>>>>>>>>>", sipidList);

            this.redisadaptor.hostJoiningStatus(meetingId, jwtToken);
            ioredis.to(sipidList).emit("hostjoined_notification", other);
            //   await this.connectionRocket(meetingdetails, meetInfo, EVENTNAMES.HOST_JOINING, socketId);
          } else if (part.isAlternateHost) {
            meetingdetails.alternateHostDetails = part;
            meetingdetails.alternateHostDetails.deviceid = deviceid;
          }

          if (!part.isHost) {

            // this.redisadaptor.participantJoiningStatus(meetingId, jwtToken);
            if (!meetingdetails.isParticipantVideo)
              part.video = false;

            if (meetingdetails.generalSetting.is_participant_mute_entry) {
              part.mute = true
            }
          }
        } else {
          if (
            meetingdetails.hostDetails &&
            meetingdetails.hostDetails.deviceid == deviceid &&
            meetingdetails.hostDetails.sipId == sipLoginId
          ) {
            part.isHost = true;
            part.isRecordingAllowed = true;

            let existHost_details: any = _.findWhere(
              meetingdetails.devicesInfo,
              { isHost: true }
            );
            if (existHost_details && existHost_details.deviceid != deviceid) {
              existHost_details.isHost = false;
              existHost_details.isRecordingAllowed = false;
              existHost_details.roleType = 3;

              if (
                meetingdetails.alternateHostDetails.deviceid ==
                existHost_details.deviceid
              )
                existHost_details.isAlternateHost = true;
              existHost_details.roleType = 2;

              meetingdetails.devicesInfo[existHost_details.deviceid] =
                existHost_details;
            }
          } else if (
            meetingdetails.alternateHostDetails &&
            meetingdetails.alternateHostDetails.deviceid == deviceid &&
            meetingdetails.alternateHostDetails.sipId == sipLoginId
          ) {
            part.isAlternateHost = true;
          } else {
            if (!existSipid_details && part.isAlternateHost) {
              meetingdetails.alternateHostDetails = part;
              meetingdetails.alternateHostDetails.deviceid = deviceid;
            } else {

              //host details need handle

              if (Object.keys(meetingdetails.hostDetails).length == 0 && part.isHost) {
                part.isHost = false;
                part.roleType = 1;
              } else {
                part.isHost = false;
                part.roleType = 3;
              }

              part.isAlternateHost = false;
            }
          }
        }

        if (
          existDeviceid_details &&
          existDeviceid_details.deviceid == deviceid &&
          existDeviceid_details.sipId == sipLoginId
        ) {
          existDeviceid_details.conId = socketId;
          existDeviceid_details.status = "waiting for umn server response";
          existDeviceid_details.video = video;
          existDeviceid_details.mute = audio;
          part = existDeviceid_details;

          if (
            sipLoginId == meetingdetails.hostDetails.sipId &&
            meetingdetails.hostDetails.deviceid == deviceid
          ) {
            part.isHost = true;
            part.isRecordingAllowed = true;
            meetingdetails.hostDetails = part;
          } else if (
            meetingdetails.alternateHostDetails &&
            meetingdetails.alternateHostDetails.deviceid == deviceid &&
            meetingdetails.alternateHostDetails.sipId == sipLoginId
          ) {
            meetingdetails.alternateHostDetails = part;
          }
        } else {
          part.status = "waiting for umn server response";
          part.removeFromMeeting = false;
          part.moveToWaitingRoom = false;
          part.conId = socketId;
          /* part.video = video;
          part.mute = audio; */
          part.pined = false;
          part.hand = false;
          part.removeFromMeeting = false;
          part.moveToWaitingRoom = false;
          part.deviceid = deviceid;
        }

        part.isMirror = true;
        //  meetingdetails.joined_participants[sipLoginId.toString()][socket.id] = part;

        let del = Object.keys(meetingdetails.devicesInfo).length || 0;

        if (!meetingdetails.devicesInfo.hasOwnProperty(deviceid))
          part.delaySec = (del + 1);


        /*   waiting_connection = waiting_connection.filter(
            (item: any) => item !== socket.id
          );
   */
        /*   joined_connection.push(socket.id);
          meetingdetails.joinedConn = joined_connection; */
        // meetingdetails.waitingroomConn = waiting_connection;


        meetingdetails.devicesInfo[deviceid] = part;


        if (meetingdetails.isParticipantjoinbefhost || meetingdetails.isHostjoined || part.isHost) {

          logger.info(`<<<<<<<<<<< JOIN_INITIALIZED SEND INVITE  >>>>>>> ${new Date().getTime()}`, JSON.stringify(part));
          console.log(`<<<<<<<<<<< JOIN_INITIALIZED SEND INVITE  >>>>>>> ${new Date().getTime()}`, JSON.stringify(part));
          await this.emitEvent(EVENTNAMES.JOIN_INITIALIZED, socketId, {
            message: "send_invite",
            user_details: part,
            socketid: socketId,
            meetingdetails: meetingdetails,
          });

        } else {

          part.status = "host_not_joined"
          meetingdetails.devicesInfo[deviceid] = part;
          await this.emitEvent(EVENTNAMES.CONNECTION_STATUS, socketId, {
            meetingdetails: meetingdetails,
            isConnected: "host_not_joined",
            socketid: socketId,
          });

        }
        console.log("isTransferisTransfer>>",isTransfer);
        if(isTransfer) {meetingdetails.transfer = true;};
        /*       await this.emitEvent(EVENTNAMES.MEETING_STATUS, meetingId, { message: `${part.name} Joined the meeting`, socketid: socketId, meetingdetails: meetingdetails }); */
        meetingdetails.devicesInfo[deviceid] = part;
        await this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);
        if (part.isHost) {
          await this.rocketMQMiddleware.connectionRocket(
            meetingdetails,
            [part],
            EVENTNAMES.AGENT_JOINING,
            socketId,
          );
        } else {
          if (meetingdetails.isParticipantjoinbefhost || meetingdetails.isHostjoined) {
            await this.rocketMQMiddleware.connectionRocket(
              meetingdetails,
              [part],
              EVENTNAMES.CUSTOMER_JOINING,
              socketId
            );
          }
        }
      } else {


        part.status = "inWaitingroom";
        part.conId = socket.id;
        part.video = video;
        part.mute = audio;
        part.pined = false;
        part.hand = false;
        part.moveToWaitingRoom = true;
        part.removeFromMeeting = false;
        part.deviceid = deviceid;


        //let existSipid_details: any = _.where(meetingdetails.devices_info, { sipId: sipLoginId });
        if (
          !meetingdetails.devicesInfo.hasOwnProperty(deviceid) &&
          !existSipid_details
        ) {
          //  meetingdetails.joined_participants[sipLoginId.toString()] = {};

          if (!meetingdetails.isParticipantVideo)
            part.video = false;


          if (meetingdetails.generalSetting.is_participant_mute_entry) {
            part.mute = true
          }

          if (part.isAlternateHost) {
            meetingdetails.alternateHostDetails = part;
            meetingdetails.alternateHostDetails.deviceid = deviceid;
          }
        } else {
          if (!existSipid_details && part.isAlternateHost) {
            meetingdetails.alternateHostDetails = part;
            meetingdetails.alternateHostDetails.deviceid = deviceid;
          } else {
            part.isHost = false;
            part.roleType = 3;
            part.isAlternateHost = false;
          }
        }

        //meetingdetails.participants[userName.toString()] = part;
        /*   joined_connection = joined_connection.filter(
            (item: any) => item !== socket.id
          ); */

        //waiting_connection.push(socket.id);
        //meetingdetails.joinedConn = joined_connection;
        //meetingdetails.waitingroomConn = waiting_connection;

        if (
          existDeviceid_details &&
          existDeviceid_details.deviceid == deviceid &&
          existDeviceid_details.sipId == sipLoginId
        ) {
          existDeviceid_details.conId = socket.id;
          part = existDeviceid_details;
          part.status = "inWaitingroom";
        }

        let del = Object.keys(meetingdetails.devicesInfo).length || 0;

        if (!meetingdetails.devicesInfo.hasOwnProperty(deviceid))
          part.delaySec = (del + 1);

        meetingdetails.devicesInfo[deviceid] = part;

        part.isHostAllowed = false;
        // meetingdetails.joined_participants[sipLoginId.toString()][socket.id] = part;

        await this.rocketMQMiddleware.connectionRocket(
          meetingdetails,
          [part],
          EVENTNAMES.CUSTOMER_JOINING,
          socketId
        );

        await this.emitEvent(EVENTNAMES.CONNECTION_STATUS, socket.id, {
          meetingdetails: meetingdetails,
          isConnected: "waitingroom",
          socketid: socket.id,
        });

        let Host_details: any = _.findWhere(meetingdetails.devicesInfo, {
          isHost: true,
        });

        if (Host_details) {

          await this.emitEvent(EVENTNAMES.MEETING_STATUS, Host_details.conId, {
            message: `${part.name} in a waiting room`,
            socketid: socket.id,
            meetingdetails: meetingdetails,
          });
        }


        await this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);
      }
    } catch (e) {
      console.log("connection manager error", e);

      logger.info(`<<<< connection manager error >>>>>>`, e);
    }
  }

private async handleVerificationReqRes(req: any, meetInfo: any) {
    try {
      let { meetingId } = meetInfo;
      const meetingdetails: any =  await this.redisadaptor.getMeetingRedis(
        meetingId
      );
    let device_details: any = _.findWhere(meetingdetails.devicesInfo, {
            deviceid: req.deviceid,
          });
      console.log("handleVerification >>>>>", req,"device_details>>>>>>>>>",device_details)
     if(device_details){
      if(req.type==="req"){
        const { type, ...updated } = req;
       await this.emitEvent(EVENTNAMES.VERIFICATION_REQ,device_details.conId, {
        details:updated,
        socketid: device_details.conId,
        deviceid:device_details.deviceid,
        type:"req"
      });
    }
      else if(req.type==="res"){
  const { type, ...updated } = req;
         
          await this.emitEvent(EVENTNAMES.VERIFICATION_RES,meetingId, {
           details:updated,
           socketid: device_details.conId,
           deviceid:device_details.deviceid,
           type:"res"
         });
        }
        else if(req.type === "cancel" || req.type === "failure" || req.type === "complete"){
            const { type, ...updated } = req;
          await this.emitEvent(EVENTNAMES.VERIFICATION_RES,meetingId, {
             details:updated,
            socketid: device_details.conId,
            deviceid:device_details.deviceid,
            type:req.type
          });
        }
                  else if(req.type === "doccapture" || req.type ==="docdetails"){
                     const { type, ...updated } = req;
          await this.emitEvent(EVENTNAMES.VERIFICATION_RES,meetingId, {
            socketid: device_details.conId,
            deviceid:device_details.deviceid,
            type:req.type,
            details:updated
          });
        }
      }
      

    } catch (error) {
      console.log("error>>>>>>",error);
    }
  }

  private async handleVerificationDetails(req: any, meetInfo: any) {
    try {
     //imgCount till 5 //verificationFiles //faceverificationSucess
      let { meetingId} = meetInfo;
      const meetingdetails: any =  await this.redisadaptor.getMeetingRedis(
        meetingId
      );
    let device_details: any = _.findWhere(meetingdetails.devicesInfo, {
            deviceid: req.deviceid,
          });
     if(device_details){
      await this.emitEvent(EVENTNAMES.VERIFICATION_DETAILS,device_details.conId, {
        socketid: device_details.conId,
        deviceid:device_details.deviceid,
        details:req
      });
     }
      

    } catch (error) {
      console.log("error>>>>>>",error);
    }
  }

  private async updateMicLang(req: any, meetInfo: any) {
    let { meetingId, jwtToken, sipLoginId, socketId } = meetInfo;

    const meetingdetails: any = await this.redisadaptor.getMeetingRedis(
      meetingId
    );
    let Sipid_details: any = _.findWhere(meetingdetails.devicesInfo, {
      conId: req.socketId,
    });
    if (Sipid_details) {
      Sipid_details.speaker_lang = req.lang;
      if (meetingdetails.devicesInfo.hasOwnProperty(Sipid_details.deviceid)) {
        meetingdetails.devicesInfo[Sipid_details.deviceid] = Sipid_details;
      }

      this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);
      const participant = Sipid_details;

      let data: any = {

        connection_id: participant.conId,
        meeting_id: meetingId,
        cc_lang: participant.cc_lang,
        cc_state: true,
        speaker_name: participant.name,
        is_host: participant.isHost,
        device_id: participant.deviceid,
        image_url: participant.profileImg,
        join_status: "joined",
        speaker_lang: req.lang,
        status: 3
      }

      await this.videoccpublisher.videoccpublish("subchannel", JSON.stringify(data));
      // await this.redisadaptor.ccdatapush(data);
      await this.emitEvent(EVENTNAMES.MIC_LANGUAGE, req.socketId, {
        message: `Speaker language changed to ${req.lang === 'fr' ? 'french' : req.lang === 'pt' ? 'portueguese' : req.lang === "es" ? "Spanish" : req.lang === 'ja' ? "japanese" : req.lang === 'ko' ? "Korean" : req.lang === 'ta' ? "Tamil" : req.lang === 'te' ? "Telugu" : req.lang === 'hi' ? "Hindi" : req.lang === 'ar' ? "Arabic" : req.lang === "bn" ? "Bengali" : req.lang === "pl" ? "Polish" : req.lang === "ro" ? "Romanian" : req.lang === "ru" ? "Russian" : req.lang === "ur" ? "Urdu" : "english"}`,
        socketid: req.socketId,
        userName: "",
        lang: req.lang,
        deviceid: participant.deviceid
      });
    }
  }

  private async moveWaitingroom(req: any, meetInfo: any) {
    try {
      const adapter: RedisAdapter = this.io.of("/").adapter as RedisAdapter;
      let { meetingId, jwtToken, sipLoginId, socketId } = meetInfo;

      const meetingdetails: any = await this.redisadaptor.getMeetingRedis(
        meetingId
      );

      let Sipid_details: any = _.findWhere(meetingdetails.devicesInfo, {
        conId: req.socketId,
      });
      const participant = Sipid_details; //meetingdetails.devices_info[req.sipId];
      //  let participant_Keys = Object.keys(participant)

      if (participant) {
        /*  let joined_connection: any = meetingdetails.joinedConn || [];
         let waiting_connection: any = meetingdetails.waitingroomConn || [];
 
         joined_connection = joined_connection.filter(
           (item: any) => item !== req.socketId
         );
 
         waiting_connection.push(req.socketId);
         meetingdetails.joinedConn = joined_connection; */
        // meetingdetails.waitingroomConn = waiting_connection;

        let deviceid = participant.deviceid;
        adapter
          .remoteLeave(req.socketId, meetingId.toString())
          .then(async (x) => {
            participant.status = "inWaitingroom";

            participant.moveToWaitingRoom = true;
            //meetingdetails.joined_participants[req.sipId] = participant;

            if (meetingdetails.devicesInfo.hasOwnProperty(deviceid)) {
              meetingdetails.devicesInfo[deviceid] = participant;
            }

            this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);
            // await this.connectionRocket(meetingdetails, [participant], EVENTNAMES.PARTICIPANT_REMOVE, req.socketId)
            await this.emitEvent(EVENTNAMES.MOVE_TO_WAITING_ROOM, socketId, {
              meetingdetails: meetingdetails,
              message: "Moved to waiting room",
              socketid: socketId,
            });
            await this.emitEvent(EVENTNAMES.CONNECTION_STATUS, req.socketId, {
              meetingdetails: meetingdetails,
              isConnected: "waitingroom",
              socketid: req.socketId,
            });

            /* this.io.of("/").to(meetingId).emit(EVENTNAMES.BROADCAST_MESSAGE, {
               body: "moved to waiting room",
               socketId: socketId,
               userName: req.userName,
             });*/
          })
          .catch((x) => console.log(x));
      } else {
        //Need to handle
      }
    } catch (e) {
      console.log("Move to waiting room Error", e);

      logger.info(`<<<< Move to waiting room Error >>>>>>`, e);
    }
  }

  private async joinMeetingroom(req: any, meetInfo: any) {
    try {
      const adapter: RedisAdapter = this.io.of("/").adapter as RedisAdapter;
      let { meetingId, jwtToken, sipLoginId, socketId } = meetInfo;

      const meetingdetails: any = await this.redisadaptor.getMeetingRedis(
        meetingId
      );

      let Sipid_details: any = _.findWhere(meetingdetails.devicesInfo, {
        conId: req.socketId,
      });
      const participant = Sipid_details; //meetingdetails.devices_info[req.sipId]; //meetingdetails.participants[m.sipId];

      if (participant) {
        //  let joined_connection: any = meetingdetails.joinedConn || [];
        // let waiting_connection: any = meetingdetails.waitingroomConn || [];
        let Name = participant.name;
        let deviceid = participant.deviceid;
        /*  waiting_connection = waiting_connection.filter(
           (item: any) => item !== req.socketId
         ); */

        /*  joined_connection.push(req.socketId);
         meetingdetails.joinedConn = joined_connection; */
        // meetingdetails.waitingroomConn = waiting_connection;
        adapter
          .remoteJoin(req.socketId, meetingId.toString())
          .then(async (x: any) => {
            participant.status = "waiting room initialized";
            //participant.moveToWaitingRoom = false;
            //participant.removeFromMeeting = false;
            // meetingdetails.joined_participants[req.sipId] = participant;

            if (meetingdetails.devicesInfo.hasOwnProperty(deviceid)) {
              meetingdetails.devicesInfo[deviceid] = participant;
            }

            await this.rocketMQMiddleware.connectionRocket(
              meetingdetails,
              [participant],
              EVENTNAMES.CUSTOMER_JOINING,
              req.socketId
            );

            if (!participant.isHostAllowed) {
              participant.isHostAllowed = true;
              await this.emitEvent(EVENTNAMES.JOIN_INITIALIZED, req.socketId, {
                message: "send_invite",
                user_details: participant,
                socketid: req.socketId,
                meetingdetails: meetingdetails,
              });
            }

            await this.emitEvent(EVENTNAMES.MEETING_STATUS, socketId, {
              message: `${Name} Waiting room initialized`,
              socketid: req.socketId,
              meetingdetails: meetingdetails,
            });

            this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);

            /*await this.emitEvent(EVENTNAMES.CONNECTION_STATUS, req.socketId, {
              meetingdetails: meetingdetails,
              isConnected: "connected",
              socketid: req.socketId
            }); */
          })
          .catch((x: any) => console.log(x));
      }
    } catch (e) {
      console.log("Join romm handle", e);

      logger.info(`<<<< Join romm handle Error >>>>>>`, e);
    }
  }

  private async removefromMeeting(req: any, meetInfo: any) {
    try {
      const adapter: RedisAdapter = this.io.of("/").adapter as RedisAdapter;
      let { meetingId, jwtToken, sipLoginId, socketId, sipInfo } = meetInfo;

      const meetingdetails: any = await this.redisadaptor.getMeetingRedis(
        meetingId
      );

      let Sipid_details: any = _.findWhere(meetingdetails.devicesInfo, {
        conId: req.socketId,
      });
      const rec_data = { ...Sipid_details };
      const participant = Sipid_details; //meetingdetails.participants[m.sipId];


      if (participant) {
        let deviceid = participant.deviceid;
        participant.cc_lang = "en";
        participant.speaker_lang = "en";
        participant.recordingStatus = "stop";
        let data: any = {

          connection_id: participant.conId,
          meeting_id: meetingId,
          cc_lang: "en",
          cc_state: participant.closedCaption,
          speaker_name: participant.name,
          is_host: participant.isHost,
          device_id: participant.deviceid,
          image_url: participant.profileImg,
          join_status: "left",
          speaker_lang: "en",
          status: 4
        }
        await this.videoccpublisher.videoccpublish("subchannel", JSON.stringify(data));


        await this.emitEvent(EVENTNAMES.REMOVE_MEETING_NOTIFY, req.socketId, {
          message: "Removed the Meeting",
          type: "leave",
          meetingdetails: meetingdetails,
        });
        if (rec_data.recordingStatus === "start" || rec_data.recordingStatus === "resume" || rec_data.recordingStatus === "pause") {
          await this.emitEvent(EVENTNAMES.RECORDING_ACTION, meetingId, {
            message: `${rec_data.name} stopped the recording.`,
            meetingdetails: meetingdetails,
            participantdetails: participant,
          });
        }
        adapter
          .remoteDisconnect(req.socketId, meetingId)
          .then(async (x: any) => {
            participant.status = "removed";
            participant.moveToWaitingRoom = false;
            participant.removeFromMeeting = true;
            let Name = participant.name;

            /*    meetingdetails.activeConn = _.without(
                 meetingdetails.activeConn,
                 req.socketId
               ); */
            /*   meetingdetails.joinedConn = _.without(
                meetingdetails.joinedConn,
                req.socketId
              ); */
            /*   meetingdetails.waitingroomConn = _.without(
                meetingdetails.waitingroomConn,
                req.socketId
              ); */

            if (meetingdetails.devicesInfo.hasOwnProperty(deviceid)) {
              meetingdetails.devicesInfo[deviceid] = participant;
            }
            this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);
            await this.rocketMQMiddleware.connectionRocket(
              meetingdetails,
              [participant],
              EVENTNAMES.PARTICIPANT_REMOVE,
              req.socketId
            );

            await this.emitEvent(EVENTNAMES.REMOVE_FROM_MEETING, meetingId, {
              meetingdetails: meetingdetails,
              message: `${Name} removed from the meeting`,
              removed_socketId: req.socketId,
            });
          })
          .catch((x: any) => console.log(x));
      } else {
        //need to handle
      }
    } catch (error) {
      logger.info(`<<<<remove from meeting Error >>>>>>`, error);
    }
  }

  private async multipleRemovefromMeeting(req: any, meetInfo: any) {
    try {
      let { meetingId, jwtToken, sipLoginId, socketId, sipInfo } = meetInfo;

      const adapter: RedisAdapter = this.io.of("/").adapter as RedisAdapter;
      const meetingdetails: any = await this.redisadaptor.getMeetingRedis(
        meetingId
      );

      let i = 0;
      let namess: any = [];
      while (i < req.socket_ids.length) {
        const participant = _.findWhere(meetingdetails.devicesInfo, {
          conId: req.socket_ids[i],
        }); //meetingdetails.participants[m.sipId];

        await this.emitEvent(
          EVENTNAMES.REMOVE_MEETING_NOTIFY,
          req.socket_ids[i],
          {
            message: "Removed the Meeting",
            type: "leave",
            meetingdetails: meetingdetails,
          }
        );

        await adapter
          .remoteDisconnect(req.socket_ids[i], meetingId)
          .then(async (x: any) => {
            let deviceid = participant.deviceid;
            participant.status = "removed";
            participant.moveToWaitingRoom = false;
            participant.removeFromMeeting = true;

            let Name = participant.name;

            /*  meetingdetails.activeConn = _.without(
               meetingdetails.activeConn,
               req.socket_ids[i]
             ); */
            /*  meetingdetails.joinedConn = _.without(
               meetingdetails.joinedConn,
               req.socket_ids[i]
             ); */
            /*    meetingdetails.waitingroomConn = _.without(
                 meetingdetails.waitingroomConn,
                 req.socket_ids[i]
               ); */

            if (meetingdetails.devicesInfo.hasOwnProperty(deviceid)) {
              meetingdetails.devicesInfo[deviceid] = participant;
            }

            await this.rocketMQMiddleware.connectionRocket(
              meetingdetails,
              [participant],
              EVENTNAMES.PARTICIPANT_REMOVE,
              req.socket_ids[i]
            );

            this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);

            namess.push(Name);
            /* await this.emitEvent(EVENTNAMES.REMOVE_FROM_MEETING, meetingId, {
              message: `${Name} Removed from meeting`,
              socketid: req.socket_ids[i],
              meetingdetails: meetingdetails,
            }); */
          })
          .catch((x: any) => console.log(x));

        i++;
      }

      await this.emitEvent(EVENTNAMES.REMOVE_FROM_MEETING, meetingId, {
        message: `${namess} Removed from meeting`,
        socketid: req.socket_ids[i],
        meetingdetails: meetingdetails,
      });
    } catch (e) {
      console.log("<<<<<<<<<< multipleRemovefromMeeting >>>>>>>", e);

      logger.info(`<<<<multipleRemovefromMeeting Error >>>>>>`, e);
    }
  }

  private async makeasAdmin(req: any, meetInfo: any) {
    try {
      let { meetingId, jwtToken, sipLoginId, socketId, sipInfo } = meetInfo;

      const adapter: RedisAdapter = this.io.of("/").adapter as RedisAdapter;
      const meetingdetails: any = await this.redisadaptor.getMeetingRedis(
        meetingId
      );
      let currentHost_details: any = _.findWhere(meetingdetails.devicesInfo, {
        conId: socketId,
      });
      const participant = _.findWhere(meetingdetails.devicesInfo, {
        conId: req.socketId,
      }); //meetingdetails.devices_info[req.sipId]; //meetingdetails.participants[m.sipId];
      console.log("currentHost_details", currentHost_details);

      if (participant) {
        let Name = participant.name;
        let part_Sipid = participant.sipId;
        let deviceid = participant.deviceid;
        let host_deviceid = currentHost_details.deviceid;

        participant.isHost = true;
        participant.isAlternateHost = false;
        participant.isRecordingAllowed = true;
        participant.roleType = 1;

        currentHost_details.isHost = false;
        currentHost_details.isAlternateHost = false;
        currentHost_details.roleType = 3;

        if (meetingdetails.devicesInfo.hasOwnProperty(deviceid)) {
          if (
            meetingdetails.alternateHostDetails &&
            Object.keys(meetingdetails.alternateHostDetails).length > 0 &&
            meetingdetails.alternateHostDetails.deviceid == deviceid &&
            meetingdetails.alternateHostDetails.sipId == part_Sipid
          ) {
            meetingdetails.alternateHostDetails = {};
          }
          meetingdetails.devicesInfo[deviceid] = participant;
          meetingdetails.hostDetails = participant;
        }

        if (meetingdetails.devicesInfo.hasOwnProperty(host_deviceid)) {
          meetingdetails.devicesInfo[host_deviceid] = currentHost_details;
        }

        this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);
        await this.emitEvent(EVENTNAMES.HOST_CHANGED, meetingId, {
          message: `${Name} is host now`,
          hostDetails: participant,
          meetingdetails: meetingdetails,
        });
      } else {
        //need to handle
      }
    } catch (e) {
      console.log(">>>>>>>>>>>>>>> makeasAdmin <<<<<<<<<<<<<<<<<", e);

      logger.info(`<<<<makeasAdmin Error >>>>>>`, e);
    }
  }

  private async endMeetingAll(req: any, meetInfo: any) {


    let { meetingId, jwtToken, sipLoginId, socketId, deviceid } = meetInfo;
    const meetingdetails: any = await this.redisadaptor.getMeetingRedis(
      meetingId
    );
    let connectionDetails: any = _.findWhere(meetingdetails.devicesInfo, {
      deviceid: deviceid,
    });

    if(req.type ==="endall"){

    await this.rocketMQMiddleware.connectionRocket(
      meetingdetails,
      [connectionDetails],
      EVENTNAMES.END_MEETING_ALL,
      socketId
    );


    await this.emitEvent(EVENTNAMES.END_MEETING, meetingId, {
      message: "Meeting was ended",
      type: req.type,
      meetingdetails: meetingdetails,
    });

    const { devicesInfo, generalSetting, isParticipantjoinbefhost, activeConn, alternateHostDetails, chatSetting, conversations, globalKeywords, joinedConn, joinedParticipantlist, joinedParticipants, waitingroomConn,
      ...other } = meetingdetails;
    let sipid = Object.keys(meetingdetails.participants) || [];
    // ioredis.to(sipid).emit("completed_notification", other);
    meetingdetails.joinedParticipantlist = [];
    this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);

    let devices: any = Object.keys(meetingdetails.devicesInfo);
    let recordUsers: any = [];
    devices.forEach((elem: any) => {
      if (meetingdetails.devicesInfo[elem]) {
        if (meetingdetails.devicesInfo[elem].recordingStatus !== "stop")
          recordUsers.push(meetingdetails.devicesInfo[elem])
      }
    })

    console.log("recordUsers >>>>>>>>>>>>", recordUsers);
    for (let rindex = 0; rindex < recordUsers.length; rindex++) {
      const relement = recordUsers[rindex];
      relement.recordingStatus = 'stop'

      meetingdetails.devicesInfo[relement.deviceid] =
        relement;
      await this.emitEvent(EVENTNAMES.RECORDING_ACTION, meetingId, {
        message: `${relement.name} stopped the recording.`,
        meetingdetails: meetingdetails,
        participantdetails: relement,
      });

    }


    for (var i in meetingdetails.devicesInfo) {
      let device_details = meetingdetails.devicesInfo[i];
      const adapter: RedisAdapter = this.io.of("/").adapter as RedisAdapter;
      await adapter
        .remoteDisconnect(device_details.conId, meetingId)
        .then((x: any) => { })
        .catch((x: any) => {
          console.log(x);
        });
      meetingdetails.devicesInfo[i].status = "end";
    }


    meetingdetails.joinedParticipantlist = [];
    meetingdetails.status = "Completed"
    this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);
  }
  else{
    let Sipid_details: any = _.findWhere(meetingdetails.devicesInfo, {
      deviceid: deviceid,
    });

    if (Sipid_details) {
    await this.rocketMQMiddleware.connectionRocket(
      meetingdetails,
      [Sipid_details],
      EVENTNAMES.USER_DISCONNECTED,
      Sipid_details.conId
    );
    let filterList = _.without(meetingdetails.joinedParticipantlist, _.findWhere(meetingdetails.joinedParticipantlist, {
      deviceid: deviceid,
    }));
    console.log("filterList>>>***************",filterList);
    meetingdetails.joinedParticipantlist = filterList;

    this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);

    await this.emitEvent(
      EVENTNAMES.CLIENT_UPDATED_PARTICIPANTLIST,
      meetingId,
      {
        participantList: meetingdetails.joinedParticipantlist,
        deviceInfo: meetingdetails.devicesInfo
      }
    );
    connectionDetails.status = "left";
    connectionDetails.reactions = "";
    connectionDetails.recordingStatus = "stop";
    meetingdetails.devicesInfo[connectionDetails.deviceid] =
      connectionDetails;
    this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);
    // joinedParticipantlist
    
    for (var i in meetingdetails.devicesInfo) {

      meetingdetails.devicesInfo[i].video = true;
      meetingdetails.devicesInfo[i].mute = false;
    
  }
  this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);
 for(let i =0;i < meetingdetails.joinedParticipantlist.length;i++){
   await this.emitEvent(EVENTNAMES.HIDE_ALL, meetingdetails.joinedParticipantlist[i].conId, {
    meetingid: meetingId,
    meetingdetails: meetingdetails,
    video:true,
    audio:false
  });}
    await this.emitEvent(EVENTNAMES.END_MEETING, socketId, {
      message: "Left the meeting",
      type: req.type,
      socketid: socketId,
      meetingdetails: meetingdetails,
    });


    const adapter: RedisAdapter = this.io.of("/").adapter as RedisAdapter;
    console.log("Remote disconnecr callijkg>>>>>>");
    await adapter
      .remoteDisconnect(socketId, meetingId)
      .then(async (x: any) => {
        console.log("Leave success calling", socketId, x);
        //await this.connectionRocket(meetingdetails, [connectionDetails], EVENTNAMES.USER_DISCONNECTED, socketId);
      })
      .catch((x: any) => console.log(x));

    await this.emitEvent(EVENTNAMES.MEETING_STATUS, meetingId, {
      message: `${connectionDetails.name} Left the meeting`,
      socketid: socketId,
      meetingdetails: meetingdetails,
    });
  }
}

  }
  private async endMeeting(req: any, meetInfo: any) {
    try {
      let { meetingId, jwtToken, sipLoginId, socketId, deviceid } = meetInfo;

      const meetingdetails: any = await this.redisadaptor.getMeetingRedis(
        meetingId
      );

      if (!meetingdetails) {
        await this.emitEvent(EVENTNAMES.END_MEETING, meetingId, {
          errmsg: "No meeting available",
        });
      } else {
        let Sipid_details: any = _.findWhere(meetingdetails.devicesInfo, {
          deviceid: deviceid,
        });

        if (Sipid_details) {
          const rec_data = { ...Sipid_details };
          let screen_participant: any = _.findWhere(meetingdetails.devicesInfo, {
            deviceid: `${deviceid}_screenshare`,
          });

          if (screen_participant) {


            screen_participant.status = "left";
            screen_participant.closedCaption = false;

            screen_participant.isScreenshare = false;

            screen_participant.isWhiteboard = false;

            await this.rocketMQMiddleware.connectionRocket(
              meetingdetails,
              [screen_participant],
              EVENTNAMES.SCREENSHARE_STOP,
              screen_participant.conId
            );

            await this.emitEvent(EVENTNAMES.SCREEN_SHARE, meetingId, {
              message: false,
              socketid: screen_participant.conId,
              meetingdetails: meetingdetails,
            });

            let sdeviceid: any = screen_participant.deviceid;
            if (meetingdetails.devicesInfo.hasOwnProperty(sdeviceid)) {
              meetingdetails.devicesInfo[sdeviceid] = screen_participant;
            }
          }
          console.log("rec_data_end", rec_data);

          let screenShare_filter = _.filter(meetingdetails.devicesInfo, {
            isScreenshare: true,
          });
          if (screenShare_filter.length === 0) {
            meetingdetails.isScreenshareStarted = false;
          }

          await this.rocketMQMiddleware.connectionRocket(
            meetingdetails,
            [Sipid_details],
            EVENTNAMES.USER_DISCONNECTED,
            req.socketId
          );

          let filterList = _.without(meetingdetails.joinedParticipantlist, _.findWhere(meetingdetails.joinedParticipantlist, {
            deviceid: deviceid,
          }));

          let _filterList = _.without(filterList, _.findWhere(filterList, {
            deviceid: `${deviceid}_screenshare`
          }));

          let waitingstatus_filter = _.filter(meetingdetails.devicesInfo, {
            status: "inWaitingroom",
          });





          meetingdetails.joinedParticipantlist = _filterList;

          this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);

          await this.emitEvent(
            EVENTNAMES.CLIENT_UPDATED_PARTICIPANTLIST,
            meetingId,
            {
              participantList: meetingdetails.joinedParticipantlist,
            }
          );

          let connectionDetails = Sipid_details; //meetingdetails.joined_participants[sipLoginId][socketId];

          const adapter: RedisAdapter = this.io.of("/").adapter as RedisAdapter;
          if (req.type == "leave") {
            let status_filter = _.filter(meetingdetails.devicesInfo, {
              status: "joined",
            });

            if (Sipid_details) {

              Sipid_details.closedCaption = false;

              if (
                meetingdetails.devicesInfo.hasOwnProperty(
                  Sipid_details.deviceid
                )
              ) {
                meetingdetails.devicesInfo[Sipid_details.deviceid] =
                  Sipid_details;
              }
            }

            let cc_status_filter = _.filter(meetingdetails.devicesInfo, {
              closedCaption: true,
            });


            let ccOwner = _.findWhere(meetingdetails.devicesInfo, {
              isVoiceengineStart: true,
            });

            console.log("ccOwner >>>", ccOwner);

            if (cc_status_filter.length === 0) {
              meetingdetails.isVoiceEngineStarted = false;
            }

            if (status_filter.length === 1) {
              await this.rocketMQMiddleware.connectionRocket(
                meetingdetails,
                [connectionDetails],
                EVENTNAMES.LAST_PERSON_LEFT,
                req.socketId
              );
            }

            /*         await this.rocketMQMiddleware.connectionRocket(
                      meetingdetails,
                      [connectionDetails],
                      EVENTNAMES.USER_DISCONNECTED,
                      socketId
                    );
        
                    let filterList = _.without(meetingdetails.joinedParticipantlist, _.findWhere(meetingdetails.joinedParticipantlist, {
                      conId: socketId
                    }));
        
                    meetingdetails.joinedParticipantlist = filterList;
                    this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);
        
                    await this.emitEvent(
                      EVENTNAMES.CLIENT_UPDATED_PARTICIPANTLIST,
                      meetingId,
                      {
                        participantList: meetingdetails.joinedParticipantlist,
                      }
                    ); */
            /* await this.rocketMQMiddleware.connectionRocket(
              meetingdetails,
              [connectionDetails],
              EVENTNAMES.UPDATE_PARTICIPANTLIST,
              socketId
            ); */


            if (connectionDetails.isHost) {
              let alternate_details: any = _.findWhere(
                meetingdetails.devicesInfo,
                { isAlternateHost: true }
              );

              if (
                !alternate_details &&
                (!req.alternate_host ||
                  Object.keys(req.alternate_host).length == 0)
              ) {
                await this.emitEvent(EVENTNAMES.BROADCAST_MESSAGE, req.socketId, {
                  message: `Alternate host details must`,
                  socketid: req.socketId,
                  userName: connectionDetails.name,
                });
              } else {
                let req_alternate_details: any = _.findWhere(
                  meetingdetails.devicesInfo,
                  { conId: req.alternate_host.socketId }
                );

                if (req_alternate_details) {
                  req_alternate_details.isHost = true;
                  req_alternate_details.isAlternateHost = false;
                  req_alternate_details.roleType = 1;
                  req_alternate_details.isRecordingAllowed = true;

                  meetingdetails.devicesInfo[req_alternate_details.deviceid] =
                    req_alternate_details;

                  connectionDetails.status = "left";
                  connectionDetails.isHost = false;
                  connectionDetails.reactions = "";
                  connectionDetails.cc_lang = "en";
                  connectionDetails.speaker_lang = "en";
                  connectionDetails.recordingStatus = "stop";
                  meetingdetails.devicesInfo[connectionDetails.deviceid] =
                    connectionDetails;

                  this.redisadaptor.setMeetingDetails(
                    meetingId,
                    meetingdetails
                  );
                  let data: any = {

                    connection_id: Sipid_details.conId,
                    meeting_id: meetingId,
                    cc_lang: "en",
                    cc_state: Sipid_details.closedCaption,
                    speaker_name: Sipid_details.name,
                    is_host: Sipid_details.isHost,
                    device_id: Sipid_details.deviceid,
                    image_url: Sipid_details.profileImg,
                    join_status: "left",
                    speaker_lang: "en",
                    status: 4
                  }
                  await this.videoccpublisher.videoccpublish("subchannel", JSON.stringify(data));

                  if (rec_data.recordingStatus === "start" || rec_data.recordingStatus === "resume" || rec_data.recordingStatus === "pause") {
                    await this.emitEvent(EVENTNAMES.RECORDING_ACTION, meetingId, {
                      message: `${rec_data.name} stopped the recording.`,
                      meetingdetails: meetingdetails,
                      participantdetails: Sipid_details,
                    });
                  }

                  await this.emitEvent(EVENTNAMES.END_MEETING, req.socketId, {
                    message: "Left the meeting",
                    type: "leave",
                    meetingdetails: meetingdetails,
                  });


                  // await adapter.remoteDisconnect(socketId, meetingId);

                  await adapter
                    .remoteDisconnect(req.socketId, meetingId)
                    .then(async (x: any) => {
                      //await this.connectionRocket(meetingdetails, [connectionDetails], EVENTNAMES.USER_DISCONNECTED, socketId);
                    })
                    .catch((x: any) => console.log(x));

                  await this.emitEvent(EVENTNAMES.MEETING_STATUS, meetingId, {
                    message: `${connectionDetails.name} Left the meeting`,
                    socketid: req.socketId,
                    meetingdetails: meetingdetails,
                  });

                  await this.emitEvent(EVENTNAMES.HOST_CHANGED, meetingId, {
                    message: `${req_alternate_details.name} is host now`,
                    hostDetails: req_alternate_details,
                    socketid: req.socketId,
                    meetingdetails: meetingdetails,
                  });
                }
              }
            } else {

              console.log("Participant Leave meeting callig")
              connectionDetails.status = "left";
              connectionDetails.reactions = "";
              connectionDetails.recordingStatus = "stop";
              meetingdetails.devicesInfo[connectionDetails.deviceid] =
                connectionDetails;
              this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);
              //adapter.remoteDisconnect(socketId, meetingId);
              console.log("Before rec data calling", rec_data);
              if (rec_data.recordingStatus === "start" || rec_data.recordingStatus === "resume" || rec_data.recordingStatus === "pause") {

                console.log("Inside rec data calling", connectionDetails);

                await this.emitEvent(EVENTNAMES.RECORDING_ACTION, meetingId, {
                  message: `${rec_data.name} stopped the recording.`,
                  meetingdetails: meetingdetails,
                  participantdetails: connectionDetails,
                });
              }

              await this.emitEvent(EVENTNAMES.END_MEETING, socketId, {
                message: "Left the meeting",
                type: req.type,
                socketid: socketId,
                meetingdetails: meetingdetails,
              });


              console.log("Remote disconnecr callijkg>>>>>>");
              await adapter
                .remoteDisconnect(socketId, meetingId)
                .then(async (x: any) => {
                  console.log("Leave success calling", socketId, x);
                  //await this.connectionRocket(meetingdetails, [connectionDetails], EVENTNAMES.USER_DISCONNECTED, socketId);
                })
                .catch((x: any) => console.log(x));

              await this.emitEvent(EVENTNAMES.MEETING_STATUS, meetingId, {
                message: `${connectionDetails.name} Left the meeting`,
                socketid: socketId,
                meetingdetails: meetingdetails,
              });
            }







          } else {
            let { meetingId, jwtToken, sipLoginId, socketId } = meetInfo;


            if (connectionDetails.isHost) {
              await this.rocketMQMiddleware.connectionRocket(
                meetingdetails,
                [connectionDetails],
                EVENTNAMES.END_MEETING_ALL,
                socketId
              );
              const { devicesInfo, generalSetting, isParticipantjoinbefhost, activeConn, alternateHostDetails, chatSetting, conversations, globalKeywords, joinedConn, joinedParticipantlist, joinedParticipants, waitingroomConn,
                ...other } = meetingdetails;
              let sipid = Object.keys(meetingdetails.participants) || [];
              ioredis.to(sipid).emit("completed_notification", other);
              meetingdetails.joinedParticipantlist = [];
              this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);
              /*  await this.rocketMQMiddleware.connectionRocket(
                 meetingdetails,
                 [connectionDetails],
                 EVENTNAMES.UPDATE_PARTICIPANTLIST,
                 socketId
               ); */

              await this.emitEvent(
                EVENTNAMES.CLIENT_UPDATED_PARTICIPANTLIST,
                meetingId,
                {
                  participantList: meetingdetails.joinedParticipantlist,
                }
              );
              //completed status
              const findsip: any = await this.redisadaptor.getMeetingFind(
                meetingId,
                jwtToken
              );
              let end_time = new Date(Date.now())
              findsip.end_timestamp = end_time.setMinutes(end_time.getMinutes());
              let dur: any = (findsip.end_timestamp - findsip.start_timestamp) / 60000 >= 1 ? Math.round((findsip.end_timestamp - findsip.start_timestamp) / 60000) : (findsip.end_timestamp - findsip.start_timestamp) / 60000
              findsip.duration_minutes = dur;
              findsip.is_attended = 1;
              findsip.status = "Completed";
              this.redisadaptor
                ?.updateMeeting(meetingId, jwtToken, findsip)
                .then((response: any) => { })
                .catch((e) => {
                  new Error("Internal server error");
                });

              let devices: any = Object.keys(meetingdetails.devicesInfo);
              let recordUsers: any = [];
              devices.forEach((elem: any) => {
                if (meetingdetails.devicesInfo[elem]) {
                  if (meetingdetails.devicesInfo[elem].recordingStatus !== "stop")
                    recordUsers.push(meetingdetails.devicesInfo[elem])
                }
              })

              console.log("recordUsers >>>>>>>>>>>>", recordUsers);
              for (let rindex = 0; rindex < recordUsers.length; rindex++) {
                const relement = recordUsers[rindex];
                relement.recordingStatus = 'stop'

                meetingdetails.devicesInfo[relement.deviceid] =
                  relement;
                await this.emitEvent(EVENTNAMES.RECORDING_ACTION, meetingId, {
                  message: `${relement.name} stopped the recording.`,
                  meetingdetails: meetingdetails,
                  participantdetails: relement,
                });

              }


              await this.emitEvent(EVENTNAMES.END_MEETING, meetingId, {
                message: "Meeting was ended",
                type: req.type,
                meetingdetails: meetingdetails,
              });

              for (let sindex = 0; sindex < waitingstatus_filter.length; sindex++) {
                const element = waitingstatus_filter[sindex];
                console.log("waitingstatus_filter >>>>>>", element);
                let w_meetingdetails = meetingdetails
                await this.emitEvent(EVENTNAMES.END_MEETING, element.conId, {
                  message: "Meeting was ended",
                  meetingdetails: w_meetingdetails,
                });
              }

              for (var i in meetingdetails.devicesInfo) {
                let device_details = meetingdetails.devicesInfo[i];
                if (device_details.isGuest)
                  this.redisadaptor?.deleteGuestUser(device_details.ext);

                adapter
                  .remoteDisconnect(device_details.conId, meetingId)
                  .then((x: any) => { })
                  .catch((x) => {
                    console.log(x);
                  });
                meetingdetails.devicesInfo[i].status = "end";
              }


              meetingdetails.joinedParticipantlist = [];
              meetingdetails.status = "Completed"
              /*  meetingdetails.activeConn = []; */
              //  meetingdetails.waitingroomConn = [];
              //meetingdetails.joinedConn = [];
              this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);
            } else {
              await this.emitEvent(EVENTNAMES.END_MEETING, socketId, {
                message: "Your not the host, host can only end the meeting",
                socketid: socketId,
                userName: connectionDetails.name,
              });
            }
          }
        } else {
          await this.emitEvent(EVENTNAMES.END_MEETING, req.socketid, {
            message: "User not found",
            socketid: socketId,
            userName: "",
          });
        }
      }

    } catch (e) {
      console.log("end meeting error", e);

      logger.info(`<<<<end meeting Error >>>>>>`, e);
    }
  }

  private async sendNewchatMessage(msg: any, meetInfo: any) {
    try {
      let { meetingId, jwtToken, sipLoginId, socketId, sipInfo } = meetInfo;
      const meetingdetails: any = await this.redisadaptor.getMeetingRedis(
        meetingId
      );
      const msg_id = uuid();
      const fromdetails: any = _.findWhere(meetingdetails.devicesInfo, {
        conId: socketId,
      });

      msg.from = fromdetails && fromdetails.sipId;

      msg.fromJid =
        fromdetails && `${fromdetails.sipId}@${sipInfo.hostAddress}`;
      msg.fromProfileUrl = fromdetails && fromdetails.profileImg;
      msg.fromName = (fromdetails && fromdetails.name) || "";
      msg.fromsocketId = socketId;
      msg.to = meetingId;
      msg.hostAddress = fromdetails && sipInfo.hostAddress;
      msg.timeStamp = new Date().toISOString();
      msg.type = msg.type || "TEXT";
      msg.isPublic = true;
      msg.msg_Id = msg_id;
      msg.from_deviceId = fromdetails && fromdetails.deviceid;
      msg.to_deviceId = "";

      let msgArray = meetingdetails.conversations || [];
      msgArray.push(msg);
      meetingdetails.conversations = msgArray;

      this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);

      // delete msg.from_deviceId;
      // delete msg.to_deviceId;
      await this.emitEvent(EVENTNAMES.PUBLIC_CHAT, meetingId, msg);
    } catch (e) {
      console.log("sendNewchatMessage >>>>>>", e);

      logger.info(`<<<<sendNewchatMessage Error >>>>>>`, e);
    }
  }

  private async sendPrivatechatMessage(msg: any, meetInfo: any) {
    try {
      let { meetingId, jwtToken, sipLoginId, socketId, sipInfo } = meetInfo;

      const meetingdetails: any = await this.redisadaptor.getMeetingRedis(
        meetingId
      );

      const fromdetails: any = _.findWhere(meetingdetails.devicesInfo, {
        conId: socketId,
      });

      const todetails: any = _.findWhere(meetingdetails.devicesInfo, {
        conId: msg.tosocketId,
      });
      const msg_id = uuid();
      msg.from = fromdetails && fromdetails.sipId;
      msg.fromJid =
        fromdetails && `${fromdetails.sipId}@${sipInfo.hostAddress}`;
      msg.fromProfileUrl = (fromdetails && fromdetails.profileImg) || "";
      msg.fromName = (fromdetails && fromdetails.name) || "";
      msg.fromsocketId = socketId;
      msg.to = todetails && todetails.sipId;
      msg.toJid = todetails && `${todetails.sipId}@${sipInfo.hostAddress}`;
      msg.toProfileUrl = (todetails && todetails.profileImg) || "";
      msg.toName = (todetails && todetails.name) || "";
      msg.hostAddress = sipInfo.hostAddress;
      msg.timeStamp = new Date().toISOString();
      msg.type = msg.type || "TEXT";
      msg.isPublic = false;
      msg.msg_Id = msg_id;
      msg.from_deviceId = fromdetails && fromdetails.deviceid;
      msg.to_deviceId = todetails && todetails.deviceid;

      let msgArray = meetingdetails.conversations || [];
      msgArray.push(msg);
      meetingdetails.conversations = msgArray;

      this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);

      // delete msg.from_deviceId
      //delete msg.to_deviceId
      await this.emitEvent(EVENTNAMES.PRIVATE_CHAT, socketId, msg);
      await this.emitEvent(EVENTNAMES.PRIVATE_CHAT, msg.tosocketId, msg);
    } catch (e) {
      console.log("sendPrivatechatMessage >>>>>>", e);

      logger.info(`<<<<sendPrivatechatMessage Error >>>>>>`, e);
    }
  }

  private async getChatsbyId(inputdetails: any, meetInfo: any) {
    try {
      let { meetingId, jwtToken, sipLoginId, socketId, sipInfo } = meetInfo;

      const meetingdetails: any = await this.redisadaptor.getMeetingRedis(
        meetingId
      );

      let chatArray = meetingdetails.conversations;
      const fromdetails: any = _.findWhere(meetingdetails.devicesInfo, {
        conId: inputdetails.fromsocketId,
      });
      const todetails: any = _.findWhere(meetingdetails.devicesInfo, {
        conId: inputdetails.tosocketId,
      });

      if (inputdetails.chatType.toLowerCase() == "private") {
        let filterArray = chatArray.filter(
          (item: any, index: any) =>
            ((item.from_deviceId === fromdetails.deviceid &&
              item.to_deviceId === todetails.deviceid) ||
              (item.to_deviceId === fromdetails.deviceid &&
                item.from_deviceId === todetails.deviceid)) &&
            item.isPublic === false
        );

        this.io
          .of("/")
          .to(socketId)
          .emit(EVENTNAMES.GET_CHAT_LIST, filterArray);
      } else {
        let filterArray = chatArray.filter(
          (item: any, index: any) => item.isPublic == true
        );
        this.io
          .of("/")
          .to(socketId)
          .emit(EVENTNAMES.GET_CHAT_LIST, filterArray);
      }
    } catch (error) {
      logger.info(`<<<<getChatsbyId Error >>>>>>`, error);
    }
  }
  private sendBroadcastMessage(to: any, msg: any) {
    this.io.of("/").to(to).emit(EVENTNAMES.BROADCAST_MESSAGE, msg);
  }

  private async participantRequesttoScreenShare(
    inputdetails: any,
    meetInfo: any
  ) {
    try {
      let { meetingId, jwtToken, sipLoginId, socketId, sipInfo } = meetInfo;
      const meetingdetails: any = await this.redisadaptor.getMeetingRedis(
        meetingId
      );
      let part: any = _.findWhere(meetingdetails.devicesInfo, {
        conId: inputdetails.socketId,
      });

      let Host_details: any = _.findWhere(meetingdetails.devicesInfo, {
        isHost: true,
      });

      if (Host_details && part) {
        await this.emitEvent(
          EVENTNAMES.PARTICIPANT_REQUEST_SCREENSHARE,
          Host_details.conId,
          {
            message: `${part.name} request to screen share`,
            meetingdetails: meetingdetails,
            participantdetails: part,
          }
        );
      }
    } catch (e) {
      console.log("participantRequesttoScreenshare error", e);

      logger.info(`<<<< ERROR :::::  participantRequesttoScreenshare >>>>>> `, e);
    }
  }

  private async AllowScreenShare(inputdetails: any, meetInfo: any) {
    try {
      let { meetingId, jwtToken, sipLoginId, socketId, sipInfo } = meetInfo;
      const meetingdetails: any =  await this.redisadaptor.getMeetingRedis(
        meetingId
      );
      let part: any = _.findWhere(meetingdetails.devicesInfo, {
        conId: inputdetails.socketId,
      });

      //let Host_details: any = _.findWhere(meetingdetails.devicesInfo, { isHost: true });

      if (part) {
        part.isScreenShare = inputdetails.type;
        meetingdetails.restrictScreenShare = false;
        if (meetingdetails.devicesInfo.hasOwnProperty(part.deviceid)) {
          meetingdetails.devicesInfo[part.deviceid] = part;
        }

        this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);
        const findsip: any = await this.redisadaptor.getMeetingFind(
          meetingId,
          jwtToken
        );
        findsip.restrictScreenShare = false;
        this.redisadaptor
          ?.updateMeeting(meetingId, jwtToken, findsip)
          .then((response: any) => { })
          .catch((e) => {
            new Error("Internal server error");
          });

        await this.emitEvent(EVENTNAMES.HOST_ALLOW_SCREENSHARE, inputdetails.socketId, {
          message: `Host has allowed screen share`,
          meetingdetails: meetingdetails,
          participantdetails: part,
        });

      }
    }
    catch (e) {
      console.log("screen share error", e);
    }
  }

  private async hostRejectScreenShareRequest(inputdetails: any, meetInfo: any) {
    try {
      let { meetingId, jwtToken, sipLoginId, socketId, sipInfo } = meetInfo;
      const meetingdetails: any =  await this.redisadaptor.getMeetingRedis(
        meetingId
      );
      let part: any = _.findWhere(meetingdetails.devicesInfo, {
        conId: inputdetails.socketId,
      });

      if (part) {
        await this.emitEvent(
          EVENTNAMES.HOST_REJECT_SCREENSHARE_REQUEST,
          inputdetails.socketId,
          {
            message: `Host has reject your request`,
            meetingdetails: meetingdetails,
            participantdetails: part,
          }
        );
      }
    } catch (e) {
      console.log("hostRejectScreenshareRequest error", e);
      logger.info(`<<<< ERROR :::::  hostRejectScreenshareRequest >>>>>> `, e);
    }
  }

  private async saveAddParticipants(req: any, meetInfo: any) {
    try {
      let { meetingId, jwtToken, sipLoginId, socketId, sipInfo } = meetInfo;
      const meetingdetails: any =  await this.redisadaptor.getMeetingRedis(
        meetingId
      );

      console.log("req >>>>>>>", req);
      let { details } = req;
      let i = 0;
      const sipLoginIddetails: any = _.findWhere(meetingdetails.devicesInfo, {
        conId: socketId,
      });
      let sipArray: any = Object.keys(meetingdetails.participants) || [];
      let sipidList: any = [];
      if (!meetingdetails) {
        await this.emitEvent(EVENTNAMES.MEETING_STATUS, meetingId, {
          errmsg: "No meeting available",
        });
        return;
      } else {


        while (i < details.length) {
          const part = _.findWhere(meetingdetails.devicesInfo, {
            sipId: details[i].sipid,
          });

          sipidList.push(`${details[i].sipid}`);
          if (!part && (sipLoginIddetails.isHost || meetingdetails.is_participant_invite_guest)) {
            meetingdetails.participants[details[i].sipid] = {
              video: true,
              mute: true,
              pined: false,
              name: details[i].name || "",
              isHost: false,
              sipId: details[i].sipid,
              isAlternateHost: false,
              isRecordingAllowed: false,
              recordingStatus: "stop",
              moveToWaitingRoom: meetingdetails.waitingRoom,
              profileImg: details[i].profile_img || "",
              status: "invited", //"joined" / "inwaitingroom" / "removed";
              hand: false,
              ext: details[i].ext,
              roleType: details[i].roletype || 3,
              removeFromMeeting: false,
              email: details[i].email || "",
              closedCaption: false,
              isScreenshare: false,
              isWhiteboard: false,
              isVoiceengineStart: false,
              joinedTime: 0,
              isReconnectstatus: false,
              cc_lang: "en",
              speaker_lang: "en",
              reactions: "",
              non_verbal: ""
            };

            this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);
          } else {
            await this.emitEvent(EVENTNAMES.ADD_PARTICIPANT, socketId, {
              meetingid: meetingId,
              participant:
                "Already participant in the meeting.please try differnt user",
            });
          }
          i++;
        }
      }

      let duplicate_sipid: any;
      if (details) {
        duplicate_sipid = details.filter((user: any) => { return !sipArray.includes(user.sipid) });
        if (duplicate_sipid) {

          await this.redisadaptor
            ?.addParticipantstoMeeting(meetingId, jwtToken, details)
            .then((response: any) => { })
            .catch((e) => {
              new Error("Internal server error");
            });
          // handlePartMailTrigger(details, sipLoginIddetails, meetingdetails)
          // handleHubspotEmail(details, sipLoginIddetails, meetingdetails)
        }
      }

      const { devicesInfo, generalSetting, isParticipantjoinbefhost, activeConn, alternateHostDetails, chatSetting, conversations, globalKeywords, joinedConn, joinedParticipantlist, joinedParticipants, participants, waitingroomConn,
        ...other } = meetingdetails;

      let Host_details: any = _.findWhere(meetingdetails.devicesInfo, {
        isHost: true,
      });

      other.hostDetails = Host_details
      console.log("sipidList >>>>>>", sipidList);
      console.log("other >>>>>>", other);

      ioredis.to(sipidList).emit("addParticipant_notification", other);

      ////
      await this.emitEvent(EVENTNAMES.ADD_PARTICIPANT, meetingId, {
        meetingid: meetingId,
        participant: details,
        meetingdetails: meetingdetails,
      });
    } catch (e) {
      console.log("error");
    }
  }

  private async setGeneralMetting(
    meetingId: any,
    jwtToken: any,
    details: any,
    deviceid: any
  ) {
    try {
      const meetingdetails: any =  await this.redisadaptor.getMeetingRedis(
        meetingId
      );

      let part: any = _.findWhere(meetingdetails.devicesInfo, {
        deviceid: deviceid,
      });



      if (part.isHost) {


        meetingdetails.waitingRoom = details.isWaitingroom;

        meetingdetails.lockMeeting = details.lockMeeting;
        // this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);
        const findsip: any = await this.redisadaptor.getMeetingFind(
          meetingId,
          jwtToken
        );
        findsip.is_participant_waiting_room = details.isWaitingroom;

        findsip.is_participant_mute_entry = details.is_participant_mute_entry;
        findsip.chatSetting = details;
        this.redisadaptor
          ?.updateMeeting(meetingId, jwtToken, findsip)
          .then((response: any) => { })
          .catch((e) => {
            new Error("Internal server error");
          });



        meetingdetails.generalSetting = details;
        this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);
        await this.emitEvent(EVENTNAMES.GENERAL_SETTING, meetingId, {
          meetingid: meetingId,
          meetingdetails: meetingdetails,
        });
      }
    } catch (e) {
      console.log("deletion error", e);
    }
  }

  private async reqCoBrowse(
 req:any,meetInfo:any
  ) {
    let { meetingId,socketId } = meetInfo;
    try {
      const meetingdetails: any =  await this.redisadaptor.getMeetingRedis(
        meetingId
      );
      await this.emitEvent(EVENTNAMES.CO_BROWSER_REQ,req.conId, {
       conId:req.conId,deviceid:req.deviceid,
       msg:"request"
      });
      

    } catch (e) {
      console.log("req co browser error", e);
    }
  }

  private async acceptCoBrowse(
    req:any,meetInfo:any
     ) {
       let { meetingId,socketId } = meetInfo;
       try {
         const meetingdetails: any =  await this.redisadaptor.getMeetingRedis(
           meetingId
         );

         await this.emitEvent(EVENTNAMES.CO_BROWSER_ACCEPTED,meetingId, {
          conId:req.conId,deviceid:req.deviceid,
          msg:"accept"
         });
   
       } catch (e) {
         console.log("accept co browser error", e);
       }
     }

     

     private async declineCoBrowse(
      req: any,
      meetInfo: any
    ) {
      try {
        let { meetingId, jwtToken, socketId } = meetInfo;
        const meetingdetails: any = await this.redisadaptor.getMeetingRedis(
          meetingId
        );    
          await this.emitEvent(
            EVENTNAMES.CO_BROWSER_DECLINED,
            meetingId,
            {
            msg:"decline",
            conId: req.conId,deviceid:req.deviceid}
          );
      } catch (e) {
        console.log("decline co browser error", e);
  
        logger.info(`<<<< ERROR :::::  decline co browser >>>>>> `, e);
      }
    }

    
  private async updateCanvaSize(
    req:any,
    meetInfo: any
  ) {
    try {
      let { meetingId, jwtToken, sipLoginId, socketId, sipInfo } = meetInfo;
      const meetingdetails: any = await this.redisadaptor.getMeetingRedis(
        meetingId
      );
       await this.emitEvent(
          EVENTNAMES.UPDATE_CANVAS_SIZE,
          req.conId,
          {
          msg:{ width:req.width,
           height:req.height}
          }
        );
      
    } catch (e) {
      console.log("update canvas error", e);

      logger.info(`<<<< ERROR :::::  update canvas error >>>>>> `, e);
    }
  }

  private async updateMeetingTitle(
    meetingId: any,
    jwtToken: any,
    details: any,
    deviceid: any
  ) {
    try {
      const meetingdetails: any =  await this.redisadaptor.getMeetingRedis(
        meetingId
      );
      let part: any = _.findWhere(meetingdetails.devicesInfo, {
        deviceid: deviceid,
      });

      if (part.isHost) {
        const findsip: any = await this.redisadaptor.getMeetingFind(
          meetingId,
          jwtToken
        );
        findsip.title = details.title;

        this.redisadaptor
          ?.updateMeeting(meetingId, jwtToken, findsip)
          .then((response: any) => { })
          .catch((e) => {
            new Error("Internal server error");
          });

        meetingdetails.meetingName = details.title;

        let sipidList = Object.keys(meetingdetails.participants) || [];
        console.log("message sipidList >>>>", sipidList);

        const { devicesInfo, hostDetails, generalSetting, isParticipantjoinbefhost, activeConn, alternateHostDetails, chatSetting, conversations, globalKeywords, joinedConn, joinedParticipantlist, joinedParticipants, participants, waitingroomConn,
          ...other } = meetingdetails;

        ioredis.to(sipidList).emit("meetingName_notification", other);
        this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);
        await this.emitEvent(EVENTNAMES.UPDATE_MEETING_TITLE, meetingId, {
          meetingid: meetingId,
          meetingdetails: other,
        });
      }
    } catch (e) {
      console.log("deletion error", e);
    }
  }

  private async hostDisconnected(meetingdetails: any, hostDetails: any) {
    console.log("hostDisconnected", hostDetails);
    let alternate_details: any = _.findWhere(meetingdetails.devicesInfo, {
      isAlternateHost: true,
    });

    const adapter: RedisAdapter = this.io.of("/")
      .adapter as RedisAdapter;

    if (alternate_details) {
      alternate_details.isHost = true;
      alternate_details.isAlternateHost = false;
      alternate_details.roleType = 1;
      meetingdetails.devicesInfo[alternate_details.deviceid] =
        alternate_details;
      hostDetails.status = "network failure";
      hostDetails.isHost = false;
      hostDetails.isRecordingAllowed = false;
      hostDetails.reactions = "";
      hostDetails.cc_lang = "en";
      hostDetails.speaker_lang = "en";
      hostDetails.recordingStatus = "stop";
      meetingdetails.devicesInfo[hostDetails.deviceid] = hostDetails;
      //delete meetingdetails.devices_info[hostDetails.deviceid];
      /*  this.redisadaptor.setMeetingDetails(
         meetingdetails.meetingId,
         meetingdetails
       ); */
      await this.emitEvent(EVENTNAMES.HOST_CHANGED, meetingdetails.meetingId, {
        message: `${alternate_details.name} is host now`,
        hostDetails: alternate_details,
        meetingdetails: meetingdetails,
      });
    }


    hostDetails.status = "network failure";
    hostDetails.isReconnectstatus = false;
    if (
      meetingdetails.devicesInfo.hasOwnProperty(hostDetails.deviceid)
    ) {
      meetingdetails.devicesInfo[hostDetails.deviceid] =
        hostDetails;
    }

    await this.rocketMQMiddleware.connectionRocket(
      meetingdetails,
      [hostDetails],
      EVENTNAMES.USER_TERMINATED,
      hostDetails.conId
    );


    let filterList = _.without(meetingdetails.joinedParticipantlist, _.findWhere(meetingdetails.joinedParticipantlist, {
      deviceid: hostDetails.deviceid
    }));


    let _filterList = _.without(filterList, _.findWhere(filterList, {
      deviceid: `${hostDetails.deviceid}_screenshare`
    }));

    meetingdetails.joinedParticipantlist = _filterList;
    await this.redisadaptor.setMeetingDetails(meetingdetails.meetingId, meetingdetails);

    /*  await this.rocketMQMiddleware.connectionRocket(
       meetingdetails,
       [hostDetails],
       EVENTNAMES.UPDATE_PARTICIPANTLIST,
       hostDetails.conId
     ); */


    adapter
      .remoteDisconnect(hostDetails.conId, meetingdetails.meetingId)
      .then((x: any) => { })
      .catch((x: any) => {
        console.log(x);
      });
    let data: any = {

      connection_id: hostDetails.conId,
      meeting_id: meetingdetails.meetingId,
      cc_lang: "en",
      cc_state: hostDetails.closedCaption,
      speaker_name: hostDetails.name,
      is_host: hostDetails.isHost,
      device_id: hostDetails.deviceid,
      image_url: hostDetails.profileImg,
      join_status: "left",
      speaker_lang: "en",
      status: 4
    }
    await this.videoccpublisher.videoccpublish("subchannel", JSON.stringify(data));

    await this.emitEvent(EVENTNAMES.MEETING_STATUS, meetingdetails.meetingId, {
      message: `${hostDetails.name} Disconnected from the meeting`,
      socketid: hostDetails.conId,
      meetingdetails: meetingdetails,
    });

    await this.emitEvent(
      EVENTNAMES.CLIENT_UPDATED_PARTICIPANTLIST,
      meetingdetails.meetingId,
      {
        participantList: meetingdetails.joinedParticipantlist,
      }
    );

    //Timer = null;
  }

  private async deleteConnection(meetInfo: any) {
    try {
      let { meetingId, jwtToken, sipLoginId, socketId, sipInfo } = meetInfo;

      const meetingdetails: any =  await this.redisadaptor.getMeetingRedis(
        meetingId
      );

      let participant: any = _.findWhere(meetingdetails.devicesInfo, {
        conId: socketId,
      });
      if (participant) {
        let Name = participant.name;

        // delete meetingdetails.joined_participants[user][conId];

        if (participant.isHost) {
          if (meetingdetails.hostDisconnect) {
            /* Timer = setTimeout(() => {
              this.hostDisconnected(meetingdetails, participant)
            }, 60 * 1000) */

            /*             participant.status = "network failure";
            
                        let deviceid: any = participant.deviceid;
                        if (meetingdetails.devicesInfo.hasOwnProperty(deviceid)) {
                          meetingdetails.devicesInfo[deviceid] = participant;
                        } */

            // this.cronJob.startCron(meetingdetails, participant);

            // this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);

            this.hostDisconnected(meetingdetails, participant);


          } else {

            participant.status = "network failure";
            //this.cronJob.startHostLeaveCron(meetingdetails, participant);
            //this.cronJob.startCron(meetingdetails, participant);

            this.cronExecutionHostLeave(meetingdetails, participant)

          }
        } else {

          participant.status = "network failure";
          this.cronExecutionHostLeave(meetingdetails, participant)


          //this.cronJob.startHostLeaveCron(meetingdetails, participant);
        }




        this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);

      }
    } catch (e) {
      console.log("deletion error", e);
    }
  }

  private async emitEvent(eventName: any, to: any, message: any) {

    logger.info(
      `<<<< emitEvent ::::: ${eventName} :::: ${to} >>>>>> `,
      message
    );

    /* console.log(
      `<<<< emitEvent ::::: ${eventName} :::: ${to} >>>>>> `,
      message
    ); */
    this.io.of("/").to(to).emit(eventName, message);
  }
  private async handRaiseMeeting(inputdetails: any, meetInfo: any) {
    try {
      let { meetingId, jwtToken, sipLoginId, socketId, sipInfo } = meetInfo;
      const meetingdetails: any =  await this.redisadaptor.getMeetingRedis(
        meetingId
      );
      let part: any = _.findWhere(meetingdetails.devicesInfo, {
        conId: inputdetails.socketId,
      });
      part.hand = inputdetails.handRaise;
      meetingdetails.devicesInfo[part.deviceid] = part;
      this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);
      await this.emitEvent(EVENTNAMES.HAND_RAISE, meetingId, {
        meetingid: meetingId,
        meetingdetails: meetingdetails,
        socketid: inputdetails.socketId,
      });
    } catch (e) {
      console.log("handraise error", e);

      logger.info(`<<<< ERROR :::::  handraise error >>>>>> `, e);
    }
  }

  private async lowerAllHandRaise(inputdetails: any, meetInfo: any) {
    try {
      let { meetingId, jwtToken, sipLoginId, socketId, deviceid } = meetInfo;
      const meetingdetails: any =  await this.redisadaptor.getMeetingRedis(
        meetingId
      );
      let existDeviceid_details: any = _.findWhere(meetingdetails.devicesInfo, {
        deviceid: deviceid,
      });

      if (existDeviceid_details.isHost) {
        for (var i in meetingdetails.devicesInfo) {
          meetingdetails.devicesInfo[i].hand = inputdetails.handRaise;
        }
        this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);
        await this.emitEvent(EVENTNAMES.LOWER_ALL_HAND_RAISE, meetingId, {
          meetingid: meetingId,
          meetingdetails: meetingdetails,
        });
      }
    } catch (e) {
      console.log("lower all handraise error", e);
      logger.info(`<<<< ERROR :::::  lower all handraise error >>>>>> `, e);
    }
  }
  private async enableRecordMeeting(inputdetails: any, meetInfo: any) {
    try {
      let { meetingId, jwtToken, sipLoginId, socketId, sipInfo } = meetInfo;
      const meetingdetails: any =  await this.redisadaptor.getMeetingRedis(
        meetingId
      );
      await this.emitEvent(EVENTNAMES.ENABLE_RECORDING, inputdetails.conId, {
        meetingid: meetingId,
        meetingdetails: meetingdetails,
        socketid: inputdetails.conId,
      });
    } catch (e) {
      console.log("enable recording error", e);
      logger.info(`<<<< ERROR :::::  enable recording error >>>>>> `, e);
    }
  }

  private async allenableRecording(req: any, meetInfo: any) {
    try {
      let { meetingId, jwtToken, sipLoginId, socketId, deviceid } = meetInfo;
      const meetingdetails: any =  await this.redisadaptor.getMeetingRedis(
        meetingId
      );
      let existDeviceid_details = meetingdetails.devicesInfo[deviceid];
      if (existDeviceid_details.isHost) {
        let i = 0;
        while (i < req.socket_ids.length) {
          const participant = _.findWhere(meetingdetails.devicesInfo, {
            conId: req.socket_ids[i],
          }); //meetingdetails.participants[m.sipId];
          participant.isRecordingAllowed = req.isRecordingAllowed;
          meetingdetails.devicesInfo[participant.deviceid] = participant;
          this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);

          await this.emitEvent(EVENTNAMES.ENABLE_RECORDING, req.socket_ids[i], {
            meetingid: meetingId,
            meetingdetails: meetingdetails,
            socketid: req.socket_ids[i],
          });
          i++;
        }
      }
    } catch (e) {
      console.log("allenableRecording", e);

      logger.info(`<<<< ERROR :::::  allenableRecording >>>>>> `, e);
    }
  }

  private async multipleWaitingRoom(req: any, meetInfo: any) {
    try {
      let { meetingId, jwtToken, sipLoginId, socketId, deviceid } = meetInfo;
      const adapter: RedisAdapter = this.io.of("/").adapter as RedisAdapter;
      const meetingdetails: any =  await this.redisadaptor.getMeetingRedis(
        meetingId
      );
      let i = 0;
      while (i < req.socket_ids.length) {
        const participant = _.findWhere(meetingdetails.devicesInfo, {
          conId: req.socket_ids[i],
        }); //meetingdetails.participants[m.sipId];
        adapter
          .remoteLeave(participant.conId, meetingId.toString())
          .then(async (x: any) => {
            participant.status = "inWaitingroom";
            participant.moveToWaitingRoom = true;
            meetingdetails.devicesInfo[participant.deviceid] = participant;
            this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);
            await this.emitEvent(EVENTNAMES.MOVE_TO_WAITING_ROOM, socketId, {
              socketid: req.socket_ids[i],
              meetingdetails: meetingdetails,
            });
            await this.emitEvent(
              EVENTNAMES.CONNECTION_STATUS,
              participant.conId,
              {
                meetingdetails: meetingdetails,
                isConnected: "waitingroom",
                socketid: req.socket_ids[i],
              }
            );
          })
          .catch((x: any) => console.log(x));
        i++;
      }
    } catch (e) {
      console.log("waiting room error", e);

      logger.info(`<<<< ERROR :::::  multiwaiting room error >>>>>> `, e);
    }
  }
  private async setChatSetting(inputdetails: any, meetInfo: any) {
    try {
      let { meetingId, jwtToken, sipLoginId, socketId, deviceid } = meetInfo;
      const meetingdetails: any =  await this.redisadaptor.getMeetingRedis(
        meetingId
      );
      let part: any = _.findWhere(meetingdetails.devicesInfo, {
        deviceid: deviceid,
      });
      if (part.isHost) {
        meetingdetails.chatSetting = inputdetails;
        this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);
        await this.emitEvent(EVENTNAMES.CHAT_SETTING, meetingId, {
          meetingid: meetingId,
          meetingdetails: meetingdetails,
        });
      }
    } catch (e) {
      console.log("chat setting error", e);

      logger.info(`<<<< ERROR :::::  chat setting error >>>>>> `, e);
    }
  }

  private async setRenameChange(inputdetails: any, meetInfo: any) {
    try {
      let { meetingId, jwtToken, sipLoginId, socketId, deviceid } = meetInfo;
      const meetingdetails: any =  await this.redisadaptor.getMeetingRedis(
        meetingId
      );
      let existDeviceid_details: any = _.findWhere(meetingdetails.devicesInfo, {
        conId: inputdetails.socketId,
      });
      if (!meetingdetails || !existDeviceid_details) {
        new Error("Not_valid_participants");
      } else if (
        inputdetails.name.trim() == "" ||
        inputdetails.name == undefined
      ) {
        new Error("Empty Name Not Allowed");
      } else {
        let i = 0;
        while (i < meetingdetails.conversations.length) {
          if (
            meetingdetails.conversations[i].from_deviceId ===
            existDeviceid_details.deviceid
          ) {
            meetingdetails.conversations[i].fromName = inputdetails.name;
          } else if (
            meetingdetails.conversations[i].to_deviceId ===
            existDeviceid_details.deviceid
          ) {
            meetingdetails.conversations[i].toName = inputdetails.name;
          }
          i++;
        }
        existDeviceid_details.name = inputdetails.name;
        meetingdetails.devicesInfo[existDeviceid_details.deviceid] =
          existDeviceid_details;
        this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);
        await this.emitEvent(EVENTNAMES.RENAME_CHANGED, meetingId, {
          message: "Name Changed " + inputdetails.name,
          changed_name: inputdetails.name,
          socketid: inputdetails.socketId,
          meetingdetails: meetingdetails,
        });
      }
    } catch (e) {
      console.log("rename change event error", e);
      logger.info(`<<<< ERROR :::::  rename change event erro >>>>>> `, e);
    }
  }

  private async activeSpeaker(req: any, meetInfo: any) {
    try {
      let { meetingId, jwtToken, sipLoginId, socketId, deviceid } = meetInfo;
      // let meetingdetails: any = await this.redisadaptor?.getMeetingDetails(
      //   meetingId,
      //   jwtToken
      // );

      // let existDeviceid_details: any = _.findWhere(meetingdetails.devicesInfo, { conId: req.socketId });
      // if (!req?.isScreenshareStarted && req?.userdetails && req?.meetingdetails) {
      //   await this.rocketMQMiddleware.connectionRocket(
      //     req.meetingdetails,
      //     [req.userdetails],
      //     EVENTNAMES.ACTIVE_SPEAKER_VIDEO_REQ,
      //     socketId,
      //   );
      // }


      await this.emitEvent(EVENTNAMES.ACTIVE_SPEAKER, meetingId, {
        message: req.message,
        audioPitch: req?.audioPitch || "",
        speaker_socketId: req.socketId,
      });
      // if (existDeviceid_details) {
      //   await this.rocketMQMiddleware.connectionRocket(
      //     meetingdetails,
      //     [existDeviceid_details],
      //     EVENTNAMES.ACTIVE_SPEAKER_INFO,
      //     req.socketId,
      //   );
      // }
    } catch (error) {
      logger.info(`<<<< ERROR :::::  activeSpeaker >>>>>> `, error);
    }
  }

  private async receiveSDPfromClient(req: any, meetInfo: any) {
    try {

      // console.log("receiveSDPfromClient >>>>>>", meetInfo);
      // console.log("receiveSDPfromClient Socket ID>>>>>>", req?.socketId);

      logger.info("receiveSDPfromClient >>>>>>", meetInfo);
      logger.info("receiveSDPfromClient Socket ID>>>>>>", req?.socketId);
      //clientCount++;

      let { meetingId, jwtToken, sipLoginId, socketId, deviceid } = meetInfo;
      const meetingdetails: any = await this.redisadaptor.getMeetingRedis(
        meetingId
      );

      let conId = req?.socketId || socketId;
      let connectionDetails: any = _.findWhere(meetingdetails.devicesInfo, {
        conId: conId,
      });

      //console.log("req?.participantList >>>>>", socketId,req?.participantList);
      await this.rocketMQMiddleware.connectionRocket(
        meetingdetails,
        [connectionDetails],
        EVENTNAMES.SDP_UPDATE_REQUEST,
        conId,
        req?.sdp,
        req?.participantList
      );

      /*  await this.rocketMQMiddleware.connectionRocket(
         meetingdetails,
         [connectionDetails],
         EVENTNAMES.UPDATE_PARTICIPANTLIST,
         socketId
       );
  */

      //this.rocketMQMiddleware.sendSDPtoConfBridge(req.sdp)
    } catch (error) {
      logger.info(`<<<< ERROR :::::  activeSpeaker >>>>>> `, error);
    }
  }


  private async sendwhiteBoard(req: any, meetInfo: any) {
    try {
      let { meetingId, jwtToken, sipLoginId, socketId, sipInfo } = meetInfo;
      const meetingdetails: any =  await this.redisadaptor.getMeetingRedis(
        meetingId
      );   
              await this.emitEvent(EVENTNAMES.WHITE_BOARD, meetingId, {
                conId: req.conId,
                message: req.message,
                meetingdetails:meetingdetails
              });



    } catch (error) {
      logger.info(`<<<< ERROR :::::  sendwhiteBoard >>>>>> `, error);
    }
  }

  private async getWhiteBoardlist(req: any, meetInfo: any) {
    try {
      let { meetingId, jwtToken, sipLoginId, socketId, deviceid } = meetInfo;
      /* const whiteboarddetails: any = await this.redisadaptor.getWhiteBoardFind(
      meetingId,
      jwtToken
    ); */

      let whiteboarddetails = await getWhiteBoardMessage(meetingId);
      await this.emitEvent(EVENTNAMES.GET_WHITE_BOARD, socketId, {
        whiteboarddetails: whiteboarddetails,
        sharer_socketId: socketId,
      });
    } catch (error) {
      logger.info(`<<<< ERROR :::::  getWhiteBoardlist >>>>>> `, error);
    }
  }

  private async audioMuteUnmute(inputdetails:any,meetInfo: any) {
    try {


      let { meetingId, jwtToken, sipLoginId, socketId, sipInfo } = meetInfo;
      const meetingdetails: any =  await this.redisadaptor.getMeetingRedis(
        meetingId
      );

      console.log("meetingdetailsmeetingdetails>>>>>>>>>>>>",meetingdetails)
      let part: any = _.findWhere(meetingdetails.devicesInfo, {
        conId: inputdetails.socketId,
      });

      console.log("part audio>>>>>>>>>>>>",part)
      // let mute_device_details:any=[]; let unmute_device_details:any=[];
      if (part && part.isHost == true) {
        meetingdetails.hostDetails.mute = inputdetails.mute;
      } else if (part.isAlternateHost == true) {
        meetingdetails.alternateHostDetails.mute = inputdetails.mute;
      }
      part.mute = inputdetails.mute;
      meetingdetails.devicesInfo[part.deviceid] = part;
      this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);

      logger.info("<<<<< AUDIO_MUTE_UNMUTE >>>>", part)
      await this.emitEvent(EVENTNAMES.AUDIO_MUTE_UNMUTE, meetingId, {
        meetingid: meetingId,
        meetingdetails: meetingdetails,
        socketid: inputdetails.socketId,
      });
    } catch (e) {
      console.log("audio mute error", e);

      logger.info(`<<<< ERROR :::::  audio mute >>>>>> `, e);
    }
  }
  private async videoHideShow(inputdetails: any, meetInfo: any) {
    try {
      let { meetingId, jwtToken, sipLoginId, socketId, sipInfo } = meetInfo;
      const meetingdetails: any = await this.redisadaptor.getMeetingRedis(
        meetingId
      );
      let part: any = _.findWhere(meetingdetails.devicesInfo, {
        conId: inputdetails.socketId,
      });
      if (part.isHost == true) {
        meetingdetails.hostDetails.video = inputdetails.video;
      } else if (part.isAlternateHost == true) {
        meetingdetails.alternateHostDetails.video = inputdetails.video;
      }
      part.video = inputdetails.video;
      meetingdetails.devicesInfo[part.deviceid] = part;
      this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);

      await this.emitEvent(EVENTNAMES.VIDEO_HIDE_SHOW, meetingId, {
        meetingid: meetingId,
        meetingdetails: meetingdetails,
        socketid: inputdetails.socketId,
      });
    } catch (e) {
      console.log("vedio mute error", e);
      logger.info(`<<<< ERROR :::::  videoHideShow >>>>>> `, e);
    }
  }

  private async audioMuteAll(inputdetails: any, meetInfo: any) {
    try {
      console.log("inputdetails>>>>>>",inputdetails);
      let { meetingId, jwtToken, sipLoginId, socketId, deviceid } = meetInfo;
      const meetingdetails: any = await this.redisadaptor.getMeetingRedis(
        meetingId
      );


        for (var i in meetingdetails.devicesInfo) {

            meetingdetails.devicesInfo[i].mute = inputdetails.mute;
            meetingdetails.devicesInfo[i].video=inputdetails.video;
          
        }
        this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);
        await this.emitEvent(EVENTNAMES.MUTE_ALL, meetingId, {
          meetingid: meetingId,
          meetingdetails: meetingdetails,
        });
      
    } catch (e) {
      console.log("mute audio all error", e);
      logger.info(`<<<< ERROR :::::  audioMuteAll >>>>>> `, e);
    }
  }

  private async videoHideAll( inputdetails:any,meetInfo: any) {
    try {
      console.log("inputdetails>>>>>>",inputdetails);
      let { meetingId, jwtToken, sipLoginId, socketId, deviceid } = meetInfo;
      const meetingdetails: any = await this.redisadaptor.getMeetingRedis(
        meetingId
      );


        for (var i in meetingdetails.devicesInfo) {

            meetingdetails.devicesInfo[i].video = inputdetails.video;
            meetingdetails.devicesInfo[i].mute = inputdetails.mute;
          
        }
        this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);
        await this.emitEvent(EVENTNAMES.HIDE_ALL, meetingId, {
          meetingid: meetingId,
          meetingdetails: meetingdetails,
        });
      
    } catch (e) {
      console.log("video all hide error", e);
      logger.info(`<<<< ERROR :::::  videoHideAll >>>>>> `, e);
    }
  }

  private async denyWaitingroom(req: any, meetInfo: any) {
    try {
      let { meetingId, jwtToken, sipLoginId, socketId, deviceid } = meetInfo;
      const meetingdetails: any =  await this.redisadaptor.getMeetingRedis(
        meetingId
      );

      let i = 0;
      while (i < req.socket_ids.length) {
        let Sipid_details: any = _.findWhere(meetingdetails.devicesInfo, {
          conId: req.socket_ids[i],
        });
        let host_details: any = _.findWhere(meetingdetails.devicesInfo, {
          deviceid: deviceid,
        });

        if (host_details && host_details.isHost && Sipid_details) {
          let participant_deviceid = Sipid_details.deviceid;
          let retryCount = Sipid_details.waiting_retrycount || 0;

          if (retryCount >= 3) {
            const adapter: RedisAdapter = this.io.of("/")
              .adapter as RedisAdapter;

            // await this.emitEvent(EVENTNAMES.END_MEETING, req.socket_ids[i], {
            //   message: "Left the meeting",
            //   type: "leave",
            //   meetingdetails: meetingdetails,
            // });

            await this.emitEvent(EVENTNAMES.FINAL_DENY_JOIN, req.socket_ids[i], {
              message: "host has denied your entry",
              meetingdetails: meetingdetails,
            });

            adapter
              .remoteDisconnect(req.socket_ids[i], meetingId)
              .then(async (x: any) => {
                Sipid_details.status = "removed";
                Sipid_details.moveToWaitingRoom = false;
                Sipid_details.removeFromMeeting = true;
                let Name = Sipid_details.name;

                /*   meetingdetails.activeConn = _.without(
                    meetingdetails.activeConn,
                    req.socket_ids[i]
                  ); */
                /*                 meetingdetails.joinedConn = _.without(
                                  meetingdetails.joinedConn,
                                  req.socket_ids[i]
                                ); */
                /*   meetingdetails.waitingroomConn = _.without(
                    meetingdetails.waitingroomConn,
                    req.socket_ids[i]
                  ); */

                if (
                  meetingdetails.devicesInfo.hasOwnProperty(
                    participant_deviceid
                  )
                ) {
                  meetingdetails.devicesInfo[participant_deviceid] =
                    Sipid_details;
                }

                this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);

                await this.emitEvent(EVENTNAMES.REMOVE_FROM_MEETING, socketId, {
                  meetingdetails: meetingdetails,
                  message: `${Name} removed from the meeting`,
                  removed_socketId: req.socket_ids[i],
                });
                /*  this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);
                   await this.rocketMQMiddleware.connectionRocket(meetingdetails, [Sipid_details], EVENTNAMES.PARTICIPANT_REMOVE, req.socketId)
                   await this.emitEvent(EVENTNAMES.REMOVE_FROM_MEETING, meetingId, {
                     meetingdetails: meetingdetails,
                     message: `${Name} removed from the meeting`,
                     removed_socketId: req.socketId,
                   }); */
              })
              .catch((x: any) => console.log(x));
          } else {
            let total_count = retryCount + 1;
            Sipid_details.waiting_retrycount = total_count;

            Sipid_details.status = "watiningroom_denied";
            if (
              meetingdetails.devicesInfo.hasOwnProperty(participant_deviceid)
            ) {
              meetingdetails.devicesInfo[participant_deviceid] = Sipid_details;
            }

            await this.emitEvent(
              EVENTNAMES.WAITINGROOM_RETRY,
              req.socket_ids[i],
              {
                meetingid: meetingId,
                retryCount: total_count,
                meetingdetails: meetingdetails,
              }
            );

            await this.emitEvent(EVENTNAMES.MEETING_STATUS, socketId, {
              message: `${Sipid_details.name} denied the meeting`,
              socketid: req.socket_ids[i],
              meetingdetails: meetingdetails,
            });

            this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);
          }
        }

        i++;
      }
    } catch (e) {
      console.log("denyWaitingroom", e);

      logger.info(`<<<< ERROR :::::  denyWaitingroom >>>>>> `, e);
    }
  }
  private async hostRequesttoMute(inputdetails: any, meetInfo: any) {
    try {
      let { meetingId, jwtToken, sipLoginId, socketId, sipInfo } = meetInfo;
      const meetingdetails: any =  await this.redisadaptor.getMeetingRedis(
        meetingId
      );
      let part: any = _.findWhere(meetingdetails.devicesInfo, {
        conId: socketId,
      });
      /* if (part.isHost == true) {
         meetingdetails.hostDetails.mute = inputdetails.mute;
       } else if (part.isAlternateHost == true) {
         meetingdetails.alternateHostDetails.mute = inputdetails.mute;
       }
       part.mute = inputdetails.mute;
       meetingdetails.devicesInfo[part.deviceid] = part;
       this.redisadaptor.setMeetingDetails(meetingId, meetingdetails); */
      await this.emitEvent(
        EVENTNAMES.HOST_REQUEST_UNMUTE,
        inputdetails.socketId,
        {
          message: "The host requests you to unmute",
          meetingdetails: meetingdetails,
          hostdetails: part,
        }
      );
    } catch (e) {
      console.log("hostRequesttoMute", e);

      logger.info(`<<<< ERROR :::::  hostRequesttoMute >>>>>> `, e);
    }
  }

  private async participantRequesttoUnMute(inputdetails: any, meetInfo: any) {
    try {
      let { meetingId, jwtToken, sipLoginId, socketId, sipInfo } = meetInfo;
      const meetingdetails: any =  await this.redisadaptor.getMeetingRedis(
        meetingId
      );
      let part: any = _.findWhere(meetingdetails.devicesInfo, {
        conId: inputdetails.socketId,
      });

      let Host_details: any = _.findWhere(meetingdetails.devicesInfo, {
        isHost: true,
      });

      if (Host_details && part) {
        await this.emitEvent(
          EVENTNAMES.PARTICIPANT_REQUEST_UNMUTE,
          Host_details.conId,
          {
            message: `${part.name} requests to unmute themselves`,
            meetingdetails: meetingdetails,
            participantdetails: part,
          }
        );
      }
    } catch (e) {
      console.log("participantRequesttoMute error", e);

      logger.info(`<<<< ERROR :::::  participantRequesttoMute >>>>>> `, e);
    }
  }

  private async hostRequesttoMuteAll(inputdetails: any, meetInfo: any) {
    try {
      let { meetingId, jwtToken, sipLoginId, socketId, sipInfo } = meetInfo;
      const meetingdetails: any =  await this.redisadaptor.getMeetingRedis(
        meetingId
      );
      let part: any = _.findWhere(meetingdetails.devicesInfo, {
        conId: socketId,
      });

      if (part.isHost) {
        meetingdetails.isHostAudioMute = false;
        this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);
      }

      /* if (part.isHost == true) {
         meetingdetails.hostDetails.mute = inputdetails.mute;
       } else if (part.isAlternateHost == true) {
         meetingdetails.alternateHostDetails.mute = inputdetails.mute;
       }
       part.mute = inputdetails.mute;
       meetingdetails.devicesInfo[part.deviceid] = part;
       this.redisadaptor.setMeetingDetails(meetingId, meetingdetails); */
      await this.emitEvent(EVENTNAMES.HOST_REQUEST_UNMUTEALL, meetingId, {
        message: "The host requests you to unmute",
        meetingdetails: meetingdetails,
        hostdetails: part,
      });
    } catch (e) {
      console.log("hostRequesttoMuteAll", e);

      logger.info(`<<<< ERROR :::::  hostRequesttoMuteAll >>>>>> `, e);
    }
  }

  private async retryWaitingroom(req: any, meetInfo: any) {
    try {
      let { meetingId, jwtToken, sipLoginId, socketId, deviceid } = meetInfo;
      const meetingdetails: any = await this.redisadaptor.getMeetingRedis(
        meetingId
      );

      let Sipid_details: any = _.findWhere(meetingdetails.devicesInfo, {
        conId: socketId,
      });

      let Host_details: any = _.findWhere(meetingdetails.devicesInfo, {
        isHost: true,
      });

      if (Host_details && Sipid_details) {
        let participant_deviceid = Sipid_details.deviceid;
        Sipid_details.status = "inWaitingroom";

        if (meetingdetails.devicesInfo.hasOwnProperty(participant_deviceid)) {
          meetingdetails.devicesInfo[participant_deviceid] = Sipid_details;
        }

        this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);

        await this.emitEvent(EVENTNAMES.MEETING_STATUS, Host_details.conId, {
          message: `${Sipid_details.name} Joined the meeting`,
          socketid: socketId,
          meetingdetails: meetingdetails,
        });
      }
    } catch (e) {
      console.log("retryWaitingroom", e);

      logger.info(`<<<< ERROR :::::  retryWaitingroom >>>>>> `, e);
    }
  }

  private async admitAll(req: any, meetInfo: any) {
    try {
      let { meetingId, jwtToken, sipLoginId, socketId, deviceid } = meetInfo;
      const adapter: RedisAdapter = this.io.of("/").adapter as RedisAdapter;
      const meetingdetails: any =  await this.redisadaptor.getMeetingRedis(
        meetingId
      );

      let i = 0;
      while (i < req.socket_ids.length) {
        const participant = _.findWhere(meetingdetails.devicesInfo, {
          conId: req.socket_ids[i],
        }); //meetingdetails.participants[m.sipId];

        logger.info(`<<<<< ADMIT ALL >>>>> ${req.socket_ids[i]}`, participant);
        await adapter
          .remoteJoin(req.socket_ids[i], meetingId)
          .then(async (x: any) => {
            let part_deviceid = participant.deviceid;

            let Name = participant.name;
            participant.status = "waiting room initialized";

            if (meetingdetails.devicesInfo.hasOwnProperty(part_deviceid)) {
              meetingdetails.devicesInfo[part_deviceid] = participant;
            }

            await this.rocketMQMiddleware.connectionRocket(
              meetingdetails,
              [participant],
              EVENTNAMES.PARTICIPANT_JOINIG,
              req.socket_ids[i]
            );

            if (!participant.isHostAllowed) {
              participant.isHostAllowed = true;
              await this.emitEvent(
                EVENTNAMES.JOIN_INITIALIZED,
                req.socket_ids[i],
                {
                  message: "send_invite",
                  user_details: participant,
                  socketid: req.socket_ids[i],
                  meetingdetails: meetingdetails,
                }
              );
            }

            await this.emitEvent(EVENTNAMES.MEETING_STATUS, socketId, {
              message: `${Name} Waiting room initialized`,
              socketid: req.socket_ids[i],
              meetingdetails: meetingdetails,
            });

            this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);
          })
          .catch((x: any) => console.log(x));

        i++;
      }
    } catch (e) {
      console.log("waiting room error", e);

      logger.info(`<<<< ERROR :::::  waiting room erro >>>>>> `, e);
    }
  }

  private async updateCCLang(req: any, meetInfo: any) {
    let { meetingId, jwtToken, sipLoginId, socketId } = meetInfo;

    const meetingdetails: any =  await this.redisadaptor.getMeetingRedis(
      meetingId
    );
    let Sipid_details: any = _.findWhere(meetingdetails.devicesInfo, {
      conId: req.socketId,
    });
    if (Sipid_details) {
      Sipid_details.cc_lang = req.lang;
      if (meetingdetails.devicesInfo.hasOwnProperty(Sipid_details.deviceid)) {
        meetingdetails.devicesInfo[Sipid_details.deviceid] = Sipid_details;
      }

      this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);
      const participant = Sipid_details;
      let data: any = {

        connection_id: participant.conId,
        meeting_id: meetingId,
        cc_lang: req.lang,
        cc_state: true,
        speaker_name: participant.name,
        is_host: participant.isHost,
        device_id: participant.deviceid,
        image_url: participant.profileImg,
        join_status: "joined",
        speaker_lang: participant.speaker_lang,
        status: 2
      }

      await this.videoccpublisher.videoccpublish("subchannel", JSON.stringify(data));
      //await this.redisadaptor.ccdatapush(data);
      await this.emitEvent(EVENTNAMES.CC_LANG_UPDATE, req.socketId, {
        message: `Caption language changed to ${req.lang === 'fr' ? 'french' : req.lang === 'pt' ? 'portueguese' : req.lang === "es" ? "Spanish" : req.lang === 'ja' ? "japanese" : req.lang === 'ko' ? "Korean" : req.lang === 'ta' ? "Tamil" : req.lang === 'te' ? "Telugu" : req.lang === 'hi' ? "Hindi" : req.lang === 'ar' ? "Arabic" : req.lang === "bn" ? "Bengali" : req.lang === "bn" ? "Bengali" : req.lang === "pl" ? "Polish" : req.lang === "ru" ? "Russian" : req.lang === "ro" ? "Romanian" : req.lang === "ur" ? "Urdu" : "english"}`,
        socketid: req.socketId,
        userName: "",
        lang: req.lang,
        deviceid: participant.deviceid
      });
    }
  }

  private async participantRequesttoRecording(
    inputdetails: any,
    meetInfo: any
  ) {
    try {
      let { meetingId, jwtToken, sipLoginId, socketId, sipInfo } = meetInfo;
      const meetingdetails: any =  await this.redisadaptor.getMeetingRedis(
        meetingId
      );

        await this.emitEvent(
          EVENTNAMES.PARTICIPANT_REQUEST_RECORING,
          inputdetails.socketId,
          {
            message: `Agent request to record`,
            meetingdetails: meetingdetails,
          }
        );
      
    } catch (e) {
      console.log("participantRequesttoRecording error", e);

      logger.info(`<<<< ERROR :::::  participantRequesttoRecording >>>>>> `, e);
    }
  }

  private async hostRejectRecordRequest(inputdetails: any, meetInfo: any) {
    try {
      let { meetingId, jwtToken, sipLoginId, socketId, sipInfo } = meetInfo;
      const meetingdetails: any =  await this.redisadaptor.getMeetingRedis(
        meetingId
      );
      let part: any = _.findWhere(meetingdetails.devicesInfo, {
        conId: inputdetails.socketId,
      });

      if (part) {
        await this.emitEvent(
          EVENTNAMES.HOST_REJECT_RECORING_REQUEST,
          inputdetails.socketId,
          {
            message: `Host has reject your request`,
            meetingdetails: meetingdetails,
            participantdetails: part,
          }
        );
      }
    } catch (e) {
      console.log("hostRejectRecordRequest error", e);
      logger.info(`<<<< ERROR :::::  hostRejectRecordRequest >>>>>> `, e);
    }
  }

  private async startRecording(inputdetails: any, meetInfo: any) {
    try {
      let { meetingId, jwtToken, sipLoginId, socketId, sipInfo } = meetInfo;
      const meetingdetails: any =  await this.redisadaptor.getMeetingRedis(
        meetingId
      );
      let part: any = _.findWhere(meetingdetails.devicesInfo, {
        conId: inputdetails.socketId,
      });

      //let Host_details: any = _.findWhere(meetingdetails.devicesInfo, { isHost: true });

      if (part) {
        part.recordingStatus = inputdetails.type;

        if (inputdetails.type === "start") { part.recordFilePath = `${configuration.videoRecordFileURL}${meetingdetails.meetingId}/${part.ext}/${part.conId}_${new Date().getTime()}.mp4` }
        if (meetingdetails.devicesInfo.hasOwnProperty(part.deviceid)) {
          meetingdetails.devicesInfo[part.deviceid] = part;
        }

        this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);
        if (inputdetails.type == "start") {
          if (inputdetails.hasOwnProperty("browserType")) part.browser_type = inputdetails?.browserType;
          meetingdetails.recordId=uuidv4();
          await this.redisadaptor.setMeetingDetails(meetingId,meetingdetails);
          await this.rocketMQMiddleware.connectionRocket(
            meetingdetails,
            [part],
            EVENTNAMES.RECORDING_START,
            part.conId
          );
          let rec_settings:any= await getSettingVoiceVideoRecordingdao(meetingdetails.meetingDetails.agentDetails.domain_id);
          console.log("rec_settingsrec_settings>>>",rec_settings);

          await this.emitEvent(EVENTNAMES.RECORDING_ACTION, meetingId, {
            message: `${part.name} started the recording.`,
            meetingdetails: meetingdetails,
            participantdetails: part,
            recSettings:rec_settings?.length > 0 ? rec_settings : []
          });
        } else if (inputdetails.type == "stop") {

          await this.rocketMQMiddleware.connectionRocket(
            meetingdetails,
            [part],
            EVENTNAMES.RECORDING_STOP,
            part.conId
          );

          await this.emitEvent(EVENTNAMES.RECORDING_ACTION, meetingId, {
            message: `${part.name} stopped the recording.`,
            meetingdetails: meetingdetails,
            participantdetails: part
          });
          const { devicesInfo, generalSetting, isParticipantjoinbefhost, activeConn, alternateHostDetails, chatSetting, conversations, globalKeywords, joinedConn, joinedParticipantlist, joinedParticipants, waitingroomConn,
            ...other } = meetingdetails;
          ioredis.to([part.sipId]).emit("recording_stop", other);
        } else if (inputdetails.type == "pause") {
          await this.rocketMQMiddleware.connectionRocket(
            meetingdetails,
            [part],
            EVENTNAMES.RECORDING_PAUSE,
            part.conId
          );
          await this.emitEvent(EVENTNAMES.RECORDING_ACTION, meetingId, {
            message: `${part.name} paused the recording.`,
            meetingdetails: meetingdetails,
            participantdetails: part,
          });
        } else if (inputdetails.type == "resume") {
          await this.rocketMQMiddleware.connectionRocket(
            meetingdetails,
            [part],
            EVENTNAMES.RECORDING_RESUME,
            part.conId
          );
          await this.emitEvent(EVENTNAMES.RECORDING_ACTION, meetingId, {
            message: `${part.name} resume the recording.`,
            meetingdetails: meetingdetails,
            participantdetails: part,
          });
        } else {
          await this.emitEvent(
            EVENTNAMES.RECORDING_ACTION,
            inputdetails.socketId,
            {
              message: `Invalid type`,
              meetingdetails: meetingdetails,
              participantdetails: part,
            }
          );
        }

        /*         if(part.isHost){
          await this.emitEvent(EVENTNAMES.START_RECORDING, meetingId, {
            message: `${part.name} started the recording.`,
            meetingdetails: meetingdetails,
            participantdetails: part
          });
        }else{

          await this.emitEvent(EVENTNAMES.START_RECORDING, Host_details.conId, {
            message: `${part.name} started the recording.`,
            meetingdetails: meetingdetails,
            participantdetails: part
          });
        }
        */
      }
    } catch (e) {
      console.log("startRecording error", e);

      logger.info(`<<<< ERROR :::::  startRecording >>>>>> `, e);
    }
  }

  // initiateTransfer

  private async initiateTransfer(inputdetails: any, meetInfo: any) {
    try {
      let { meetingId, jwtToken, sipLoginId, socketId, sipInfo } = meetInfo;
      const meetingdetails: any =  await this.redisadaptor.getMeetingRedis(
        meetingId
      );
      let part: any = _.findWhere(meetingdetails.devicesInfo, {
        conId: socketId,
      });
      meetingdetails.secondaryAgentDetails= inputdetails;
      await this.rocketMQMiddleware.connectionRocket(
        meetingdetails,
        [part],
        EVENTNAMES.TRANSFER_INIT_CONF,
        part.conId
      );
      
      for (var i in meetingdetails.devicesInfo) {

        meetingdetails.devicesInfo[i].video = false;
        meetingdetails.devicesInfo[i].mute = true;
      
    }
    await this.emitEvent(EVENTNAMES.TRANSFER_INITIATED, meetingId, {
      meetingdetails: meetingdetails,
      type:"transfer_initiated",
      details:inputdetails

    });
    await this.emitEvent(EVENTNAMES.HIDE_ALL, meetingId, {
      meetingid: meetingId,
      meetingdetails: meetingdetails,
      video:false,
      audio:true
    });
    this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);



    } catch (e) {
      console.log("transfer initiate error", e);

      logger.info(`<<<< ERROR :::::  transfer initiate error >>>>>> `, e);
    }
  }

  private async stopRecording(inputdetails: any, meetInfo: any) {
    try {
      let { meetingId, jwtToken, sipLoginId, socketId, sipInfo } = meetInfo;
      const meetingdetails: any =  await this.redisadaptor.getMeetingRedis(
        meetingId
      );
      let part: any = _.findWhere(meetingdetails.devicesInfo, {
        conId: inputdetails.socketId,
      });

      let Host_details: any = _.findWhere(meetingdetails.devicesInfo, {
        isHost: true,
      });

      if (part) {
        await this.emitEvent(EVENTNAMES.STOP_RECORDING, meetingId, {
          message: `${part.name} stopped the recording.`,
          meetingdetails: meetingdetails,
          participantdetails: part,
        });
      }
    } catch (e) {
      console.log("stopRecording error", e);

      logger.info(`<<<< ERROR :::::  stopRecording >>>>>> `, e);
    }
  }

  private async startClosecaption(req: any, meetInfo: any) {

    try {
      let { meetingId, jwtToken, sipLoginId, socketId, sipInfo, deviceid } =
        meetInfo;

      const meetingdetails: any =  await this.redisadaptor.getMeetingRedis(
        meetingId
      );

      let part: any = _.findWhere(meetingdetails.devicesInfo, {
        conId: socketId,
      });

      /*  if (!meetingdetails.isVoiceEngineStarted) {
         meetingdetails.isVoiceEngineStarted = true;
         this.speechmatics.startConnection(meetInfo);
       } */

      if (part) {
        part.closedCaption = true;
        let data: any = {

          connection_id: part.conId,
          meeting_id: meetingId,
          cc_lang: part.cc_lang,
          cc_state: true,
          speaker_name: part.name,
          is_host: part.isHost,
          device_id: part.deviceid,
          image_url: part.profileImg,
          join_status: "joined",
          speaker_lang: part.speaker_lang,
          status: 2
        }
        await this.videoccpublisher.videoccpublish("subchannel", JSON.stringify(data));
        if (meetingdetails.devicesInfo.hasOwnProperty(part.deviceid)) {
          meetingdetails.devicesInfo[part.deviceid] = part;
        }
      }

      this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);

      // await createClosecaption(meetingId,  {});

      await this.emitEvent(
        EVENTNAMES.CLOSE_CAPTION_STARTED,
        socketId,
        "CloseCaptionStarted"
      );
    } catch (error) {
      logger.info(`<<<< ERROR :::::  startClosecaption >>>>>> `, error);
    }
  }

  private async stopClosecaption(req: any, meetInfo: any) {

    try {
      let { meetingId, jwtToken, sipLoginId, socketId, deviceid } = meetInfo;

      const meetingdetails: any =  await this.redisadaptor.getMeetingRedis(
        meetingId
      );

      let part: any = _.findWhere(meetingdetails.devicesInfo, {
        conId: socketId,
      });

      if (part) {
        let data: any = {

          connection_id: part.conId,
          meeting_id: meetingId,
          cc_lang: part.cc_lang,
          cc_state: false,
          speaker_name: part.name,
          is_host: part.isHost,
          device_id: part.deviceid,
          image_url: part.profileImg,
          join_status: "joined",
          speaker_lang: part.speaker_lang,
          status: 2
        }
        await this.videoccpublisher.videoccpublish("subchannel", JSON.stringify(data));
        part.closedCaption = false;

        if (meetingdetails.devicesInfo.hasOwnProperty(part.deviceid)) {
          meetingdetails.devicesInfo[part.deviceid] = part;
        }
      }
      /* let status_filter = _.filter(meetingdetails.devicesInfo, {
        closedCaption: true,
      });

      let ccOwner = _.findWhere(meetingdetails.devicesInfo, {
        isVoiceengineStart: true,
      }); */


      /*       if (status_filter.length === 0) {
              meetingdetails.isVoiceEngineStarted = false;
      
              this.speechmatics.stopClosedcaption();
            } else {
              if (ccOwner) {
                this.speechmatics.stopClosedcaption();
              }
      
              let ccSocketid = status_filter[0].conId;
              await this.emitEvent(EVENTNAMES.CLOSE_CAPTION_ON_REQUEST, ccSocketid, {
                message: `close caption initizialized`,
                meetingdetails: meetingdetails,
              });
            } */



      this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);

      await this.emitEvent(
        EVENTNAMES.CLOSE_CAPTION_ENDED,
        socketId,
        "ClosecaptionEnded"
      );
    } catch (error) {
      logger.info(`<<<< ERROR :::::  stopClosecaption >>>>>> `, error);
    }
  }





  private async storeFullClosecaption(req: any, meetInfo: any) {
    try {
      let { meetingId, jwtToken, sipLoginId, socketId, deviceid } = meetInfo;



      let meetingdetails: any = await this.redisadaptor.getMeetingRedis(
        meetingId
      );


      let speakerDetails: any = _.findWhere(meetingdetails.devicesInfo, {
        conId: socketId,
      });


      //if(req.type == "AddTranscript"){
      let transciptdetails: any = {};
      transciptdetails.tid = uuid();
      transciptdetails.text = req.voiceData;
      transciptdetails.timeStamp = new Date().toISOString();

      transciptdetails.imageURL = req.imageURL;
      transciptdetails.lang = req.lang;

      transciptdetails.deviceid = req.deviceid
      transciptdetails.convertedText = ""

      transciptdetails.speakerName = speakerDetails ? speakerDetails.name : "";
      transciptdetails.isHost = speakerDetails ? speakerDetails.isHost : false;


      // await storeClosecaption(meetingId, transciptdetails);

      await this.emitEvent(EVENTNAMES.STORE_CLOSE_CAPTION, meetingId, transciptdetails);

      //}


    } catch (error) {
      logger.info("<<<<< ERROR: broadcastTranscript >>>>", error);
    }
  }

  private async searchTranscript(req: any, meetInfo: any) {
    try {
      let { meetingId, jwtToken, sipLoginId, socketId, deviceid } = meetInfo;



      const meetingdetails: any =  await this.redisadaptor.getMeetingRedis(
        meetingId
      );


      let data: any = {};
      data.uuid = meetingId;
      data.searchTerm = req.searchTerm;
      data.offset = req.offset;
      data.limit = req.limit;

      let searchResult: any = await findCloseCaption(data);

      await this.emitEvent(
        EVENTNAMES.SEARCH_TRANSCRIPT,
        socketId,
        { result: searchResult }
      );

    } catch (error) {
      logger.info("<<<<< ERROR: searchTranscript >>>>", error);
    }
  }


  private async filterTranscript(req: any, meetInfo: any) {
    try {
      let { meetingId, jwtToken, sipLoginId, socketId, deviceid } = meetInfo;



      const meetingdetails: any =  await this.redisadaptor.getMeetingRedis(
        meetingId
      );




      let data: any = {};
      data.uuid = meetingId;
      data.names = req.names;
      data.filterType = req.filterType;
      data.filterKeys = req.filterKeys;
      data.offset = req.offset;
      data.limit = req.limit;

      if (req.filterType == "name") {

        let searchResult: any = await filterNameCloseCaption(data);

        await this.emitEvent(
          EVENTNAMES.FILTER_TRANSCRIPT,
          socketId,
          { result: searchResult }
        );

      } else {
        let searchResult: any = await filterCloseCaption(data);

        await this.emitEvent(
          EVENTNAMES.FILTER_TRANSCRIPT,
          socketId,
          { result: searchResult }
        );
      }



    } catch (error) {
      logger.info("<<<<< ERROR: filterTranscript >>>>", error);
    }
  }



  private async updateTranscript(req: any, meetInfo: any) {
    try {
      let { meetingId, jwtToken, sipLoginId, socketId, deviceid } = meetInfo;

      const meetingdetails: any = await this.redisadaptor.getMeetingRedis(
        meetingId
      );

      let updateResult = await updateCloseCaption(meetingId, req.tid, req.updateText);


      if (updateResult && updateResult.modifiedCount && updateResult.modifiedCount != 0) {
        await this.emitEvent(
          EVENTNAMES.UPDATE_TRANSCRIPT,
          socketId,
          "Updated successfully"
        );

      } else {
        await this.emitEvent(
          EVENTNAMES.UPDATE_TRANSCRIPT,
          socketId,
          "Updation failed"
        );
      }

    } catch (error) {
      logger.info("<<<<< ERROR: updateTranscript >>>>", error);
    }
  }
  // private async broadcastClosecaption(meetInfo: any) {
  //   console.log("meetInfo>>", meetInfo);

  //   try {

  //     console.log("broadcastClosecaption >>>>>>>>>")
  //     let { meetingId, jwtToken, sipLoginId, socketId, deviceid } = meetInfo;
  //     let data: any = JSON.parse(meetInfo)


  //     let meetingdetails: any = await this.redisadaptor.getMeetingInfofromRedis(
  //       data.uuid
  //     );

  //     let closeCaptionUser = _.filter(meetingdetails.devicesInfo, {
  //       closedCaption: true,
  //     });

  //     console.log("closeCaptionUser", closeCaptionUser);

  //     // let speakerDetails: any = _.findWhere(meetingdetails.devicesInfo, {
  //     //   conId: socketId,
  //     // });



  //     /* if(req.type == "AddTranscript"){
  //       let transciptdetails: any = {};
  //       transciptdetails.text = req.voiceData;
  //       transciptdetails.timeStamp = new Date().toISOString();
  //       transciptdetails.speakerName = speakerDetails ? speakerDetails.name : "";
  //       transciptdetails.isHost = speakerDetails ? speakerDetails.isHost : false;


  //       await storeClosecaption(meetingId, transciptdetails);

  //     } */


  //     for (let index = 0; index < closeCaptionUser.length; index++) {
  //       const element = closeCaptionUser[index];

  //       //if(!element.mute) 
  //       await this.emitEvent(EVENTNAMES.CLOSE_CAPTION, element.conId, data);

  //     }

  //   } catch (error) {
  //     logger.info("<<<<< ERROR: broadcastTranscript >>>>", error);
  //   }
  // }

  // broadcastCC = async (meetInfo: any) => {
  //   //console.log("broadcastCC>>",meetInfo);

  //   try {

  //     let data: any = JSON.parse(meetInfo);

  //     // console.log("data >>>>>>>", data);
  //     if (data.Meeting_Id) {
  //       let meetingIdIdx:any=data.Meeting_Id.indexOf("_")
  //       let uuid:any=data.Meeting_Id;
  //       if(meetingIdIdx > -1){uuid=data.Meeting_Id.slice(0,meetingIdIdx);}
  //       let meetingdetails: any = await this.redisadaptor.getMeetingRedis(
  //         uuid
  //       );


  //         let translated_data = {};
  //         let device_details: any = _.findWhere(meetingdetails.devicesInfo, {
  //           conId: data.Connection_Id,
  //         });
  //         console.log("device_detailsdevice_details cc>>>>>>>>>",device_details);
  //         let profile = "";
  //         let translation = data?.translation || {}
  //         if (device_details && device_details.profileImg) { profile = device_details.profileImg; }
  //         console.log(device_details,"device_details");

  //           if(device_details.isHost){
  //             translated_data={
  //               "id": uuidv4(),
  //               "from": `${meetingdetails.meetingDetails.agentDetails.domain_id}_${meetingdetails.meetingDetails.agentDetails.ext}`,
  //               "to":`${meetingdetails.meetingDetails.agentDetails.sessionId}`,
  //               "sessionId": `${meetingdetails.meetingDetails.agentDetails.sessionId}`,
  //               "message": `<p>${data.text}</p>`,
  //               "sender": {
  //                   "uuid": meetingdetails.meetingDetails.agentDetails.agentId.toString(),
  //                   "Name": meetingdetails.meetingDetails.agentDetails.username,
  //                   "Email":  meetingdetails.meetingDetails.agentDetails.Email,
  //               },
  //               "isBot": false,
  //               "isCustomer": false,
  //               "messageType": "Text",
  //               "sendAt": Math.floor(Date.now() / 1000),
  //               "chatIconLogo": "",
  //               "timestamp": Math.floor(Date.now() / 1000)
  //           }
            
  //           }
  //           else{
  //             translated_data=
  //               {
  //                 "messageID": uuidv4(),
  //                 "from": `${meetingdetails.meetingDetails.agentDetails.sessionId}`,
  //                 "to": `${meetingdetails.meetingDetails.agentDetails.domain_id}_${meetingdetails.meetingDetails.agentDetails.ext}`,
  //                 "sessionId": `${meetingdetails.meetingDetails.agentDetails.sessionId}`,
  //                 "message": `<p>${data.text}</p>`,
  //                 "sendAt": Math.floor(Date.now() / 1000),
  //                 "messageType": "Text",
  //                 "pdfurl": null,
  //                 "isBot": false,
  //                 "isCustomer": true,
  //                 "sender": {
  //                     "Full Name": `${meetingdetails.meetingDetails.agentDetails.customerDetails.firstName} ${meetingdetails.meetingDetails.agentDetails.customerDetails.lastName}`,
  //                     "Email": Array.isArray(meetingdetails.meetingDetails.agentDetails.customerDetails.email) && meetingdetails.meetingDetails.agentDetails.customerDetails.email?.length > 0 ? meetingdetails.meetingDetails.agentDetails.customerDetails.email[0] : "",
  //                     "Phone Number": (Array.isArray(meetingdetails.meetingDetails.agentDetails.customerDetails.phoneNumber) && meetingdetails.meetingDetails.agentDetails.customerDetails.phoneNumber?.length) > 0 ?  meetingdetails.meetingDetails.agentDetails.customerDetails.phoneNumber[0] : ""
  //                 }
  //             }
              
              
  //           }
           

  //         // console.log("translated_data>>>", translated_data, "device_details.conId", device_details.conId, "deviceid_arr", deviceid_arr)
  //         if(device_details){await this.emitEvent(EVENTNAMES.CLOSE_CAPTION, device_details.conId, translated_data);}

  

  //       await storeClosecaptionCcaas(meetingdetails, data);
  //     }

  //     // }

  //   } catch (error) {

  //     console.log("broadcastCC >>>>>>>", error);
  //     logger.info("<<<<< ERROR: broadcastTranscript >>>>", error);
  //   }
  // }

  broadcastCC = async (meetInfo: any) => {
    //console.log("broadcastCC>>",meetInfo);

    try {

      //console.log("broadcastCC1 >>>>>>>>>")
      //let { meetingId, jwtToken, sipLoginId, socketId, deviceid } = meetInfo;
      let data: any = JSON.parse(meetInfo);

      // console.log("data >>>>>>>", data);
      if (data.Meeting_Id) {
        let meetingIdIdx:any=data.Meeting_Id.indexOf("_")
        let uuid:any=data.Meeting_Id;
        if(meetingIdIdx > -1){uuid=data.Meeting_Id.slice(0,meetingIdIdx);}
        let meetingdetails: any = await this.redisadaptor.getMeetingRedis(
          uuid
        );

        console.log("meetingdetails >>>>>>>>", meetingdetails);
        //console.log("data>>***", data);




        // let closeCaptionUser = _.filter(meetingdetails.devicesInfo, {
        //   closedCaption: true,
        // });

        //console.log("meetingdetails.devicesInfo",meetingdetails.devicesInfo);


        //console.log("closeCaptionUser",closeCaptionUser);


        // for (let index = 0; index < closeCaptionUser.length; index++) {
        //   const element = closeCaptionUser[index];
        let deviceid_arr = Object.keys(data.Language);
        deviceid_arr?.forEach(async (elem: any, i: any) => {
          let translated_data = {};
          let device_details: any = _.findWhere(meetingdetails.devicesInfo, {
            deviceid: elem,
          });
          let profile = "";
          let translation = data?.translation || {}
          if (device_details && device_details.profileImg) { profile = device_details.profileImg; }
          console.log(device_details,"device_details");
          let meetingIdIdx:any=data.Meeting_Id.indexOf("_")
          let id:any=data.Meeting_Id;
          if(meetingIdIdx > -1){id=data.Meeting_Id.slice(0,meetingIdIdx);}
          if (translation[elem]) {
            translated_data = {
              uuid: uuid,
              Connection_Id: data.Connection_Id,
              text: data.text,
              imageURL: profile,
              lang: data.lang,
              deviceid: data.deviceid,
              speakerName: data.speakerName,
              isHost: data.isHost,
              Cc_State: data.Cc_State,
              convertedText: data.translation[elem],
              isFinal: data.isFinal,
              speakerLang: data.speaker_lang ? data.speaker_lang : "en"
            }
          } else {
            translated_data = {
              uuid: uuid,
              Connection_Id: data.Connection_Id,
              text: data.text,
              imageURL: profile,
              lang: data.lang,
              deviceid: data.deviceid,
              speakerName: data.speakerName,
              isHost: data.isHost,
              Cc_State: data.Cc_State,
              convertedText: data.text,
              isFinal: data.isFinal,
              speakerLang: data.speaker_lang ? data.speaker_lang : "en"
            }
          }

          if(device_details){await this.emitEvent(EVENTNAMES.CLOSE_CAPTION, device_details.conId, translated_data);
            let hostDetails: any = _.findWhere(meetingdetails.devicesInfo, {
              isHost: true,
            });
           if(hostDetails && hostDetails.recordingStatus !== "stop" && hostDetails.recordingStatus !=="pause") {await this.emitEvent(EVENTNAMES.INSERT_REC_CC,hostDetails.conId,data);}
          }

        })

        let transcript_data:any={};
        let dd: any = _.findWhere(meetingdetails.devicesInfo, {
          conId: data.Connection_Id,
        });
        console.log("dddddddddddddd==============",dd);
                    if(dd.isHost){
              transcript_data={
                "id": uuidv4(),
                "from": `${meetingdetails.meetingDetails.agentDetails.domain_id}_${meetingdetails.meetingDetails.agentDetails.ext}`,
                "to":`${meetingdetails.meetingDetails.agentDetails.sessionId}`,
                "sessionId": `${meetingdetails.meetingDetails.agentDetails.sessionId}`,
                "message": `<p>${data.text}</p>`,
                "sender": {
                    "uuid": meetingdetails.meetingDetails.agentDetails.agentId.toString(),
                    "Name": meetingdetails.meetingDetails.agentDetails.username,
                    "Email":  meetingdetails.meetingDetails.agentDetails.Email,
                },
                "isBot": false,
                "isCustomer": false,
                "messageType": "Text",
                "sendAt": Math.floor(Date.now() / 1000),
                "chatIconLogo": "",
                "timestamp": Math.floor(Date.now() / 1000)
            }
            
            }
            else{
              transcript_data=
                {
                  "messageID": uuidv4(),
                  "from": `${meetingdetails.meetingDetails.agentDetails.sessionId}`,
                  "to": `${meetingdetails.meetingDetails.agentDetails.domain_id}_${meetingdetails.meetingDetails.agentDetails.ext}`,
                  "sessionId": `${meetingdetails.meetingDetails.agentDetails.sessionId}`,
                  "message": `<p>${data.text}</p>`,
                  "sendAt": Math.floor(Date.now() / 1000),
                  "messageType": "Text",
                  "pdfurl": null,
                  "isBot": false,
                  "isCustomer": true,
                  "sender": {
                      "Full Name": `${meetingdetails.meetingDetails.agentDetails.customerDetails.firstName} ${meetingdetails.meetingDetails.agentDetails.customerDetails.lastName}`,
                      "Email": Array.isArray(meetingdetails.meetingDetails.agentDetails.customerDetails.email) && meetingdetails.meetingDetails.agentDetails.customerDetails.email?.length > 0 ? meetingdetails.meetingDetails.agentDetails.customerDetails.email[0] : "",
                      "Phone Number": (Array.isArray(meetingdetails.meetingDetails.agentDetails.customerDetails.phoneNumber) && meetingdetails.meetingDetails.agentDetails.customerDetails.phoneNumber?.length) > 0 ?  meetingdetails.meetingDetails.agentDetails.customerDetails.phoneNumber[0] : ""
                  }
              }
            }

        await storeClosecaptionCcaas(meetingdetails, transcript_data);
      }

      // }

    } catch (error) {

      console.log("broadcastCC >>>>>>>", error);
      logger.info("<<<<< ERROR: broadcastTranscript >>>>", error);
    }
  }
  private async getAllTranscripts(req: any, meetInfo: any) {
    try {
      let { meetingId, jwtToken, sipLoginId, socketId, deviceid } = meetInfo;
      /* const whiteboarddetails: any = await this.redisadaptor.getWhiteBoardFind(
      meetingId,
      jwtToken
    ); */

      let transciptdetails = await getTranscripts(meetingId);
      await this.emitEvent(EVENTNAMES.GET_TRANSCRIPTS, socketId, {
        transcripts: transciptdetails,

      });
    } catch (error) {
      logger.info(`<<<< ERROR :::::  getAllTranscripts >>>>>> `, error);
    }
  }



  private async translateTrascript(req: any, meetInfo: any) {
    try {
      let { meetingId, jwtToken, sipLoginId, socketId, deviceid } = meetInfo;

      return true;

      let transcript_data: any = await getTranscripts(meetingId);

      console.log("transcript_data >>>>>>", transcript_data.transcripts);

      console.log("req >>>>>>>>", req);

      let { targetLang } = req;


      console.log("targetLang >>>>>>>>", targetLang);
      let FinalString = "";
      let converted_text = "";

      let original_String = "";

      for (let index = 0; index < transcript_data.transcripts.length; index++) {

        const element = transcript_data.transcripts[index].text;

        if (FinalString == "") {
          FinalString = element
          original_String = element;
        }
        else {

          let bck_string = FinalString;
          FinalString += "######" + element

          if (FinalString.length > 90000 && FinalString.length < 100000)
            original_String = FinalString
          else {
            if (FinalString.length > 90000) {
              original_String = bck_string
            }
          }

        }

        if (FinalString.length > 90000 && FinalString.length < 100000) {


          let translatell = FinalString;

          FinalString = ""


          let french: any = await this.redisadaptor.translateLanguage(translatell, targetLang);

          if (converted_text == "")
            converted_text = french
          else {
            converted_text += "######" + french
          }

        } else {


          if (index != 0 && FinalString.length > 90000) {

            if (index == (transcript_data.transcripts.length - 1)) {
              original_String = FinalString
            }

            let french: any = await this.redisadaptor.translateLanguage(original_String, targetLang);
            FinalString = "";
            FinalString = element;
            original_String = element;
            if (converted_text == "")
              converted_text = french
            else {
              converted_text += "######" + french
            }


          } else {

            if (index == (transcript_data.transcripts.length - 1)) {

              console.log("Final >>>>>>", FinalString);

              let french: any = await this.redisadaptor.translateLanguage(FinalString, targetLang);

              if (converted_text == "")
                converted_text = french
              else {
                converted_text += "######" + french
              }
            } else {


            }

          }

        }

      }


      console.log("converted_text >>>>>>", converted_text);
      let converted_array = converted_text.split("######");


      console.log("converted_array >>>>>>>>>>>>", converted_array);


      for (let kk = 0; kk < transcript_data.transcripts.length; kk++) {
        transcript_data.transcripts[kk].convertedText = converted_array[kk];
        transcript_data.transcripts[kk].lang = targetLang

      }


      this.sleep(2000);
      console.log("transcript_data >>>>>>", transcript_data);

      await this.emitEvent(EVENTNAMES.TRANSLATE_TRANSCRIPT, socketId, {
        transcripts: transcript_data,

      });
    } catch (error) {
      logger.info(`<<<< ERROR :::::  translateTrascript >>>>>> `, error);
    }
  }

  private async screenShareaction(req: any, meetInfo: any) {
    try {
      let { meetingId, jwtToken, sipLoginId, socketId, sipInfo } = meetInfo;
      const meetingdetails: any =  await this.redisadaptor.getMeetingRedis(
        meetingId
      );
      let connectionDetails: any = _.findWhere(meetingdetails.devicesInfo, {
        conId: socketId,
      });

      let settings = meetingdetails.generalSetting;

      if (settings) {
        if (settings.isScreenShare) {
          if ((req.type == "start" && !meetingdetails.isScreenshareStarted) || (req.type == "start" && connectionDetails.isScreenshare)) {
            if (connectionDetails) {
              connectionDetails.isScreenshare = true;

              meetingdetails.isScreenshareStarted = true;

              if (
                meetingdetails.devicesInfo.hasOwnProperty(
                  connectionDetails.deviceid
                )
              ) {
                meetingdetails.devicesInfo[connectionDetails.deviceid] =
                  connectionDetails;
              }
            }
          } else {
            if (connectionDetails) {
              connectionDetails.isScreenshare = false;

              if (
                meetingdetails.devicesInfo.hasOwnProperty(
                  connectionDetails.deviceid
                )
              ) {
                console.log("has own property calling", connectionDetails);
                meetingdetails.devicesInfo[connectionDetails.deviceid] =
                  connectionDetails;
              }
            }

            let screenShare_filter = _.filter(meetingdetails.devicesInfo, {
              isScreenshare: true,
            });
            if (screenShare_filter.length === 0) {
              meetingdetails.isScreenshareStarted = false;
            }
          }

          await this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);

          console.log("meetingdetails >>>>", meetingdetails);
          await this.emitEvent(EVENTNAMES.SCREEN_SHARE, meetingId, {
            message: req.message,
            socketid: socketId,
            meetingdetails: meetingdetails,
          });


        } else {

          if (meetingdetails.isScreenshareStarted) {

            if (connectionDetails) {
              connectionDetails.isScreenshare = false;

              if (
                meetingdetails.devicesInfo.hasOwnProperty(
                  connectionDetails.deviceid
                )
              ) {
                console.log("has own property calling", connectionDetails);
                meetingdetails.devicesInfo[connectionDetails.deviceid] =
                  connectionDetails;
              }
            }

            let screenShare_filter = _.filter(meetingdetails.devicesInfo, {
              isScreenshare: true,
            });
            if (screenShare_filter.length === 0) {
              meetingdetails.isScreenshareStarted = false;
            }

            await this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);

            console.log("meetingdetails >>>>", meetingdetails);
            await this.emitEvent(EVENTNAMES.SCREEN_SHARE, meetingId, {
              message: req.message,
              socketid: socketId,
              meetingdetails: meetingdetails,
            });

          } else {

            await this.emitEvent(EVENTNAMES.SCREEN_SHARE, socketId, {
              message: "Screenshare not enabled for this meeting.",
              socketid: socketId,
            });
          }



        }
      }
    } catch (error) {
      // logger.info(`<<<< ERROR :::::  sendwhiteBoard >>>>>> `, error);
    }
  }


  private async joinScreenShare(req: any, meetInfo: any) {
    try {
      let { meetingId, jwtToken, sipLoginId, socketId, sipInfo } = meetInfo;
      const meetingdetails: any =  await this.redisadaptor.getMeetingRedis(
        meetingId
      );
      const connectionDetails: any = _.findWhere(meetingdetails.devicesInfo, {
        conId: socketId,
      });

      let settings = meetingdetails.generalSetting;

      if (settings) {
        if (settings.isScreenShare) {
          if ((req.type == "start" && !meetingdetails.isScreenshareStarted) || (req.type == "start" && connectionDetails.isScreenshare)) {
            if (connectionDetails) {

              console.log("<<<<< SCREEN SHARE SEC CALLING", JSON.stringify(meetingdetails));

              const screenShareDetails = { ...connectionDetails };
              let new_deviceId = `${connectionDetails.deviceid}_screenshare`
              let newConnectionId = `${socketId}_screenshare`
              screenShareDetails.conId = newConnectionId;
              screenShareDetails.status = 'screensharejoining'
              screenShareDetails.isScreenshareUser = true
              screenShareDetails.deviceid = new_deviceId;
              screenShareDetails.isHost = false;
              screenShareDetails.isScreenshare = false;
              screenShareDetails.isAlternateHost = false;
              screenShareDetails.sipId = `${connectionDetails.sipId}_screenshare`



              meetingdetails.devicesInfo[new_deviceId] =
                screenShareDetails;


              console.log("<<<<< AFTRE SCREEN SHARE SEC CALLING", JSON.stringify(meetingdetails));
              await this.rocketMQMiddleware.connectionRocket(
                meetingdetails,
                [screenShareDetails],
                EVENTNAMES.START_SCREENSHARE,
                newConnectionId
              );



            }
          } else {

            const screenShareconnectionDetails: any = _.findWhere(meetingdetails.devicesInfo, {
              conId: `${socketId}_screenshare`,
            });
            if (screenShareconnectionDetails) {
              screenShareconnectionDetails.isScreenshare = false;
              screenShareconnectionDetails.status = 'disconnected';

              if (
                meetingdetails.devicesInfo.hasOwnProperty(
                  screenShareconnectionDetails.deviceid
                )
              ) {
                console.log("has own property calling", screenShareconnectionDetails);
                meetingdetails.devicesInfo[screenShareconnectionDetails.deviceid] =
                  screenShareconnectionDetails;
              }
            }

            let screenShare_filter = _.filter(meetingdetails.devicesInfo, {
              isScreenshare: true,
            });
            if (screenShare_filter.length === 0) {
              meetingdetails.isScreenshareStarted = false;
            }

            if (screenShareconnectionDetails) {
              await this.rocketMQMiddleware.connectionRocket(
                meetingdetails,
                [screenShareconnectionDetails],
                EVENTNAMES.SCREENSHARE_STOP,
                screenShareconnectionDetails.conId
              );
            }


            let filterList = _.without(meetingdetails.joinedParticipantlist, _.findWhere(meetingdetails.joinedParticipantlist, {
              deviceid: screenShareconnectionDetails.deviceid
            }));
            console.log("socketId >>>>>>", socketId)
            console.log("filterList >>>>>>", filterList)
            meetingdetails.joinedParticipantlist = filterList;
            //await this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);

            /*  await this.rocketMQMiddleware.connectionRocket(
               meetingdetails,
               [screenShareconnectionDetails],
               EVENTNAMES.UPDATE_PARTICIPANTLIST,
               socketId
             ); */


            await this.emitEvent(EVENTNAMES.SCREEN_SHARE, meetingId, {
              message: false,
              socketid: socketId,
              meetingdetails: meetingdetails,
            });

            await this.emitEvent(
              EVENTNAMES.CLIENT_UPDATED_PARTICIPANTLIST,
              meetingdetails.meetingId,
              {
                participantList: meetingdetails.joinedParticipantlist,
              }
            );

          }

          await this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);


        } else {

          if (meetingdetails.isScreenshareStarted) {


            const screenShareconnectionDetails: any = _.findWhere(meetingdetails.devicesInfo, {
              conId: `${socketId}_screenshare`,
            });

            if (screenShareconnectionDetails) {
              screenShareconnectionDetails.isScreenshare = false;
              screenShareconnectionDetails.status = 'disconnected';

              if (
                meetingdetails.devicesInfo.hasOwnProperty(
                  screenShareconnectionDetails.deviceid
                )
              ) {
                console.log("has own property calling", screenShareconnectionDetails);
                meetingdetails.devicesInfo[screenShareconnectionDetails.deviceid] =
                  screenShareconnectionDetails;
              }
            }

            let screenShare_filter = _.filter(meetingdetails.devicesInfo, {
              isScreenshare: true,
            });
            if (screenShare_filter.length === 0) {
              meetingdetails.isScreenshareStarted = false;
            }

            //await this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);

            await this.rocketMQMiddleware.connectionRocket(
              meetingdetails,
              [screenShareconnectionDetails],
              EVENTNAMES.SCREENSHARE_STOP,
              screenShareconnectionDetails.conId
            );


            let filterList = _.without(meetingdetails.joinedParticipantlist, _.findWhere(meetingdetails.joinedParticipantlist, {
              deviceid: screenShareconnectionDetails.deviceid
            }));

            console.log("filterList >>>>>>", filterList)
            meetingdetails.joinedParticipantlist = filterList;
            await this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);

            /*  await this.rocketMQMiddleware.connectionRocket(
               meetingdetails,
               [screenShareconnectionDetails],
               EVENTNAMES.UPDATE_PARTICIPANTLIST,
               socketId
             ); */


            console.log("meetingdetails >>>>", meetingdetails);
            await this.emitEvent(EVENTNAMES.SCREEN_SHARE, meetingId, {
              message: req.message,
              socketid: socketId,
              meetingdetails: meetingdetails,
            });

            await this.emitEvent(
              EVENTNAMES.CLIENT_UPDATED_PARTICIPANTLIST,
              meetingdetails.meetingId,
              {
                participantList: meetingdetails.joinedParticipantlist,
              }
            );

          } else {

            await this.emitEvent(EVENTNAMES.SCREEN_SHARE, socketId, {
              message: "Screenshare not enabled for this meeting.",
              socketid: socketId,
            });
          }



        }
      }
    } catch (error) {
      // logger.info(`<<<< ERROR :::::  sendwhiteBoard >>>>>> `, error);
    }
  }

  private async handlePinnedUser(req: any, meetInfo: any) {
    try {
      let { meetingId, jwtToken, sipLoginId, socketId, sipInfo } = meetInfo;
      /*  const meetingdetails: any = await this.redisadaptor.getMeetingDetails(
         meetingId,
         jwtToken
       ); */
      await this.emitEvent(EVENTNAMES.PINNED_USER, req.socketId, {
        message: req.message,
        socketid: socketId,
      });

    } catch (error) {
      // logger.info(`<<<< ERROR :::::  sendwhiteBoard >>>>>> `, error);
    }
  }

  private async lowNetworkdetection(req: any, meetInfo: any) {
    try {
      let { meetingId, jwtToken, sipLoginId, socketId, sipInfo } = meetInfo;
      /*  const meetingdetails: any = await this.redisadaptor.getMeetingDetails(
         meetingId,
         jwtToken
       ); */

      console.log("lowNetworkdetection >>>>>", req)
      await this.emitEvent(EVENTNAMES.POOR_NETWORK_EVENT, meetingId, {
        message: req,
        socketid: socketId,
      });

    } catch (error) {
      // logger.info(`<<<< ERROR :::::  sendwhiteBoard >>>>>> `, error);
    }
  }

  private async handleVideoresolution(req: any, meetInfo: any) {
    try {
      let { meetingId, jwtToken, sipLoginId, socketId, sipInfo } = meetInfo;
      /*  const meetingdetails: any = await this.redisadaptor.getMeetingDetails(
         meetingId,
         jwtToken
       ); */

      console.log("handleVideoresolution >>>>>", req)
      await this.emitEvent(EVENTNAMES.VIDEO_RESOLUTION_REQ, req.socketId, {
        message: "Video resolution req",
        socketid: socketId,
      });

    } catch (error) {
      // logger.info(`<<<< ERROR :::::  sendwhiteBoard >>>>>> `, error);
    }
  }




  private async handleTilechange(req: any, meetInfo: any) {
    try {
      let { meetingId, jwtToken, sipLoginId, socketId, sipInfo } = meetInfo;
      const meetingdetails: any =  await this.redisadaptor.getMeetingRedis(
        meetingId
      );

      let Sipid_details: any = _.findWhere(meetingdetails.devicesInfo, {
        conId: socketId,
      });

      let partList = req?.participantList || []
      await this.rocketMQMiddleware.connectionRocket(
        meetingdetails,
        [Sipid_details],
        EVENTNAMES.TILE_CHANGE_REQUEST,
        socketId,
        "",
        partList
      );

    } catch (error) {
      // logger.info(`<<<< ERROR :::::  sendwhiteBoard >>>>>> `, error);
    }
  }

  private async handleScreenshareVideo(req: any, meetInfo: any) {
    try {
      let { meetingId, jwtToken, sipLoginId, socketId, sipInfo } = meetInfo;
      const meetingdetails: any =  await this.redisadaptor.getMeetingRedis(
        meetingId
      );

      let Sipid_details: any = _.findWhere(meetingdetails.devicesInfo, {
        conId: socketId,
      });

      console.log("handleScreenshareVideo Sipid_details >>>>>>", Sipid_details);

      let joinedDetails = {
        conId: req.socketId,
        user_id: "",
        user_type: ""
      }
      await this.rocketMQMiddleware.connectionRocket(
        meetingdetails,
        [Sipid_details],
        EVENTNAMES.SCREENSHARE_VIDEO_REQ,
        socketId,
        "",
        [joinedDetails]
      );

    } catch (error) {
      // logger.info(`<<<< ERROR :::::  sendwhiteBoard >>>>>> `, error);
    }
  }

  private async handleActiveSpeakerVideo(req: any, meetInfo: any) {
    try {
      let { meetingId, jwtToken, sipLoginId, socketId, sipInfo } = meetInfo;
      const meetingdetails: any =  await this.redisadaptor.getMeetingRedis(
        meetingId
      );

      let Sipid_details: any = _.findWhere(meetingdetails.devicesInfo, {
        conId: socketId,
      });

      await this.rocketMQMiddleware.connectionRocket(
        meetingdetails,
        [Sipid_details],
        EVENTNAMES.ACTIVE_SPEAKER_VIDEO_REQ,
        socketId,
      );

    } catch (error) {
      // logger.info(`<<<< ERROR :::::  sendwhiteBoard >>>>>> `, error);
    }
  }

  private async updateMirror(req: any, meetInfo: any) {
    try {
      console.log("updateMirror >>>>.", req);
      let { meetingId, jwtToken, sipLoginId, socketId, sipInfo } = meetInfo;
      const meetingdetails: any =  await this.redisadaptor.getMeetingRedis(
        meetingId
      );
      let connectionDetails: any = _.findWhere(meetingdetails.devicesInfo, {
        conId: socketId,
      });

      console.log("connectionDetails >>>>.", connectionDetails);
      if (connectionDetails) {
        connectionDetails.isMirror = req.mirror;

        if (
          meetingdetails.devicesInfo.hasOwnProperty(connectionDetails.deviceid)
        ) {
          console.log("inside hasown propert");
          meetingdetails.devicesInfo[connectionDetails.deviceid] =
            connectionDetails;
        }

        console.log(
          "meetingdetails.devicesInfo[connectionDetails.deviceid] ",
          meetingdetails.devicesInfo[connectionDetails.deviceid]
        );
        await this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);

        await this.emitEvent(EVENTNAMES.MIRROR_UPDATE, socketId, {
          message: "Updated successfully",
          socketid: socketId,
          meetingdetails: meetingdetails,
        });
      } else {
        await this.emitEvent(EVENTNAMES.MIRROR_UPDATE, socketId, {
          message: "Updation failed",
          socketid: socketId,
          meetingdetails: meetingdetails,
        });
      }
    } catch (error) {
      logger.info(`<<<< ERROR :::::  updateMirror >>>>>> `, error);
    }
  }

  // private async addHighlights(req:any,meetInfo:any){
  //   try{
  //     let { meetingId, jwtToken, sipLoginId, socketId, sipInfo } = meetInfo;
  //     const meetingdetails: any = await this.redisadaptor.getMeetingDetails(
  //       meetingId,
  //       jwtToken
  //     );
  //     let connectionDetails: any = _.findWhere(meetingdetails.devicesInfo, {
  //       conId: socketId,
  //     });

  //   }
  //   catch{

  //   }
  // }

  private async getKeywords(req: any, meetInfo: any) {
    try {
      console.log("getKeywords >>>>.", req);
      let { meetingId, jwtToken, sipLoginId, socketId, sipInfo } = meetInfo;
      const meetingdetails: any =  await this.redisadaptor.getMeetingRedis(
        meetingId
      );
      let connectionDetails: any = _.findWhere(meetingdetails.devicesInfo, {
        conId: socketId,
      });

      let keywords = meetingdetails.globalKeywords || ["Task", "Note", "Important", "Name", "Mark", "List", "Event"];

      console.log("connectionDetails >>>>.", connectionDetails);
      //if (connectionDetails) {

      await this.emitEvent(EVENTNAMES.GET_KEYWORDS, socketId, {
        keywords
      });
      //} 
    } catch (error) {
      logger.info(`<<<< ERROR :::::  getKeywords >>>>>> `, error);
    }
  }


  private async addKeywords(req: any, meetInfo: any) {
    try {
      console.log("addKeywords >>>>.", req);
      let { meetingId, jwtToken, sipLoginId, socketId, sipInfo } = meetInfo;
      const meetingdetails: any =  await this.redisadaptor.getMeetingRedis(
        meetingId
      );
      let connectionDetails: any = _.findWhere(meetingdetails.devicesInfo, {
        conId: socketId,
      });
      let keywords: any = meetingdetails.globalKeywords || ["Task", "Note", "Important", "Name", "Mark", "List", "Event"];
      console.log("connectionDetails >>>>.", connectionDetails);
      if (connectionDetails.isHost) {

        keywords.push(req.keyword)

        meetingdetails.globalKeywords = keywords;

        await this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);
        addKeywordsMeeting(meetingId, keywords)

        await this.emitEvent(EVENTNAMES.ADD_KEYWORD, socketId, {
          message: "Updated successfully",
          socketid: socketId,
          keywords: meetingdetails.globalKeywords,
        });
      } else {
        await this.emitEvent(EVENTNAMES.ADD_KEYWORD, socketId, {
          message: "Host only can add the keyword",
          socketid: socketId,
          meetingdetails: meetingdetails.globalKeywords,
        });
      }
    } catch (error) {
      logger.info(`<<<< ERROR :::::  addKeywords >>>>>> `, error);
    }
  }


  private async removeKeyword(req: any, meetInfo: any) {
    try {
      console.log("removeKeyword >>>>.", req);
      let { meetingId, jwtToken, sipLoginId, socketId, sipInfo } = meetInfo;
      const meetingdetails: any =  await this.redisadaptor.getMeetingRedis(
        meetingId
      );
      let connectionDetails: any = _.findWhere(meetingdetails.devicesInfo, {
        conId: socketId,
      });
      let keywords: any = meetingdetails.globalKeywords || ["Task", "Note", "Important", "Name", "Mark", "List", "Event"];

      if (connectionDetails.isHost) {

        // keywords.push(req.keyword)

        let remove_keywords = req.keyword.join() || ""

        let removed_key = _.without(
          keywords,
          remove_keywords
        );

        meetingdetails.globalKeywords = removed_key;



        console.log("meetingdetails.globalKeywords >>>>>>>>", meetingdetails.globalKeywords);
        removeKeywordsMeeting(meetingId, removed_key)
        await this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);

        await this.emitEvent(EVENTNAMES.REMOVE_KEYWORD, socketId, {
          message: "Updated successfully",
          socketid: socketId,
          keywords: meetingdetails.globalKeywords,
        });
      } else {
        await this.emitEvent(EVENTNAMES.REMOVE_KEYWORD, socketId, {
          message: "Host only can remove the keyword",
          socketid: socketId,
          meetingdetails: meetingdetails.globalKeywords,
        });
      }
    } catch (error) {
      logger.info(`<<<< ERROR :::::  addKeywords >>>>>> `, error);
    }
  }


  private async automatedMinutes(req: any, meetInfo: any) {
    try {
      let { meetingId, jwtToken, sipLoginId, socketId, deviceid } = meetInfo;



      const meetingdetails: any =  await this.redisadaptor.getMeetingRedis(
        meetingId
      );


      let data: any = {};
      data.uuid = meetingId;
      data.filter = meetingdetails.globalKeywords || ["Task", "Note", "Important", "Name", "Mark", "List", "Event"];


      let searchResult: any = await filterCloseCaption(data);

      await this.emitEvent(
        EVENTNAMES.AUTOMATEDMINUTES,
        socketId,
        { result: searchResult }
      );




    } catch (error) {
      logger.info("<<<<< ERROR: automatedMinutes >>>>", error);
    }
  }



  private async handleScreenshareReconnect(inputdetails: any, meetInfo: any) {
    try {


      console.log("handleManuallyDisconnect >>>>>", meetInfo);
      let { meetingId, jwtToken, sipLoginId, socketId, sipInfo } = meetInfo;
      const meetingdetails: any =  await this.redisadaptor.getMeetingRedis(
        meetingId
      );

      await this.emitEvent(
        EVENTNAMES.RECONNECT_SCREENSHARE,
        meetingId,
        {

          socketid: socketId,
          meetingdetails: meetingdetails,
        }
      );
    } catch (e) {
      console.log("handraise error", e);

      logger.info(`<<<< ERROR :::::  handraise error >>>>>> `, e);
    }
  }
  private async handleManuallyDisconnect(inputdetails: any, meetInfo: any) {
    try {


      //console.log("handleManuallyDisconnect >>>>>", meetInfo);

      logger.info("handleManuallyDisconnect >>>>>", meetInfo);
      let { meetingId, jwtToken, sipLoginId, socketId, sipInfo } = meetInfo;
      const adapter: RedisAdapter = this.io.of("/").adapter as RedisAdapter;
      const meetingdetails: any =  await this.redisadaptor.getMeetingRedis(
        meetingId
      );

      let Sipid_details: any = _.findWhere(meetingdetails.devicesInfo, {
        conId: socketId,
      });
      const rec_data = { ...Sipid_details };
      if (Sipid_details) {
        Sipid_details.closedCaption = false;

        Sipid_details.isScreenshare = false;

        Sipid_details.isWhiteboard = false;
        Sipid_details.cc_lang = "en";
        Sipid_details.speaker_lang = "en";
        Sipid_details.recordingStatus = "stop";
        //Sipid_details.status = "disconnected";


      }

      if (meetingdetails.isScreenshareStarted) {
        const screenshare_sipid_details: any = _.findWhere(meetingdetails.devicesInfo, {
          conId: `${socketId}_screenshare`,
        });
        if (screenshare_sipid_details && screenshare_sipid_details?.isScreenshare) {
          screenshare_sipid_details.isScreenshare = false;
          screenshare_sipid_details.status = "disconnected";

          if (
            meetingdetails.devicesInfo.hasOwnProperty(screenshare_sipid_details.deviceid)
          ) {
            meetingdetails.devicesInfo[screenshare_sipid_details.deviceid] = screenshare_sipid_details;
          }


          let filterList = _.without(meetingdetails.joinedParticipantlist, _.findWhere(meetingdetails.joinedParticipantlist, {
            deviceid: `${Sipid_details.deviceid}_screenshare`
          }));
          console.log("socketId >>>>>>", socketId)
          console.log("filterList >>>>>>", filterList)
          meetingdetails.joinedParticipantlist = filterList;

          await this.rocketMQMiddleware.connectionRocket(
            meetingdetails,
            [screenshare_sipid_details],
            EVENTNAMES.SCREENSHARE_STOP,
            screenshare_sipid_details.conId
          );

          await this.emitEvent(EVENTNAMES.SCREEN_SHARE, meetingId, {
            message: false,
            socketid: socketId,
            meetingdetails: meetingdetails,
          });
        }

      }

      let screenstatus_filter = _.filter(meetingdetails.devicesInfo, {
        isScreenshare: true,
      });

      let whitestatus_filter = _.filter(meetingdetails.devicesInfo, {
        isWhiteboard: true,
      });

      if (screenstatus_filter.length === 0) {
        meetingdetails.isScreenshareStarted = false;

      }

      if (whitestatus_filter.length === 0) {
        meetingdetails.isWhiteboardStarted = false;

      }

      let filterList = _.without(meetingdetails.joinedParticipantlist, _.findWhere(meetingdetails.joinedParticipantlist, {
        deviceid: Sipid_details.deviceid
      }));
      console.log("socketId >>>>>>", socketId)
      console.log("filterList >>>>>>", filterList)
      meetingdetails.joinedParticipantlist = filterList;

      /* await this.rocketMQMiddleware.connectionRocket(
        meetingdetails,
        [Sipid_details],
        EVENTNAMES.UPDATE_PARTICIPANTLIST,
        socketId
      ); */



      if (Sipid_details.status !== "disconnected") {

        if (Sipid_details) Sipid_details.status = "disconnected";
        Sipid_details.cc_lang = "en";
        Sipid_details.speaker_lang = "en";
        Sipid_details.recordingStatus = "stop";

        if (
          meetingdetails.devicesInfo.hasOwnProperty(Sipid_details.deviceid)
        ) {
          meetingdetails.devicesInfo[Sipid_details.deviceid] =
            Sipid_details;
        }


        await this.rocketMQMiddleware.connectionRocket(
          meetingdetails,
          [Sipid_details],
          EVENTNAMES.USER_TERMINATED,
          socketId
        );

        let data: any = {

          connection_id: Sipid_details.conId,
          meeting_id: meetingId,
          cc_lang: "en",
          cc_state: Sipid_details.closedCaption,
          speaker_name: Sipid_details.name,
          is_host: Sipid_details.isHost,
          device_id: Sipid_details.deviceid,
          image_url: Sipid_details.profileImg,
          join_status: "left",
          speaker_lang: "en",
          status: 4
        }
        await this.videoccpublisher.videoccpublish("subchannel", JSON.stringify(data));
       if(Sipid_details.recordingStatus !== "stop"){ await this.emitEvent(EVENTNAMES.RECORDING_ACTION, meetingId, {
          message: `${rec_data.name} stopped the recording.`,
          meetingdetails: meetingdetails,
          participantdetails: Sipid_details,
        });
        await this.rocketMQMiddleware.connectionRocket(
          meetingdetails,
          [Sipid_details],
          EVENTNAMES.RECORDING_STOP,
          Sipid_details.conId
        );
      }
        await this.emitEvent(
          EVENTNAMES.MEETING_STATUS,
          meetingdetails.meetingId,
          {
            message: `${Sipid_details.name} Disconnected from the meeting`,
            socketid: Sipid_details.conId,
            meetingdetails: meetingdetails,
          }
        );

        await this.emitEvent(
          EVENTNAMES.CLIENT_UPDATED_PARTICIPANTLIST,
          meetingdetails.meetingId,
          {
            participantList: meetingdetails.joinedParticipantlist,
          }
        );

      }

      if (Sipid_details) Sipid_details.status = "disconnected";
      Sipid_details.cc_lang = "en";
      Sipid_details.speaker_lang = "en";
      Sipid_details.recordingStatus = "stop";

      if (
        meetingdetails.devicesInfo.hasOwnProperty(Sipid_details.deviceid)
      ) {
        meetingdetails.devicesInfo[Sipid_details.deviceid] =
          Sipid_details;
      }

      await this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);


    } catch (e) {
      console.log("handraise error", e);

      logger.info(`<<<< ERROR :::::  handraise error >>>>>> `, e);
    }
  }

  private async handleReactions(req: any, meetInfo: any) {

    let { meetingId, jwtToken, sipLoginId, socketId, sipInfo } = meetInfo;
    const meetingdetails: any =  await this.redisadaptor.getMeetingRedis(
      meetingId
    );
    let part: any = _.findWhere(meetingdetails.devicesInfo, {
      conId: req.conId,
    });
    if (part) {
      if (req.action === "add") {
        meetingdetails.devicesInfo[part.deviceid].reactions = req.emoji;
        await this.emitEvent(EVENTNAMES.REACTION_REMOVE, req.conId, {
          emoji: req.emoji,
          message: `${part.name} has reacted ${req.emoji}`,
          socketid: req.conId,
          meetingdetails: meetingdetails,
          deviceid: part.deviceid,
          action: req.action
        });
      }
      else {
        if (req.action === "remove") {
          meetingdetails.devicesInfo[part.deviceid].reactions = "";
        }
      }

      if (meetingdetails.devicesInfo.hasOwnProperty(part.deviceid)) {
        meetingdetails.devicesInfo[part.deviceid] = part;
      }

      this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);

      await this.emitEvent(EVENTNAMES.REACTIONS, meetingId, {
        emoji: req.emoji,
        message: (req.action === "add") ? `${part.name} has reacted ${req.emoji}` : `${part.name} has removed ${req.emoji}`,
        socketid: req.conId,
        meetingdetails: meetingdetails,
        deviceid: part.deviceid,
        action: req.action
      });
    }
  }

  private async handleNonVerbal(req: any, meetInfo: any) {
    let { meetingId, jwtToken, sipLoginId, socketId, sipInfo } = meetInfo;
    const meetingdetails: any =  await this.redisadaptor.getMeetingRedis(
      meetingId
    );
    let part: any = _.findWhere(meetingdetails.devicesInfo, {
      conId: req.conId,
    });
    if (part) {
      if (req.action === "add") {
        meetingdetails.devicesInfo[part.deviceid].non_verbal = req.emoji;
      }
      else {
        meetingdetails.devicesInfo[part.deviceid].non_verbal = "";
      }
      if (meetingdetails.devicesInfo.hasOwnProperty(part.deviceid)) {
        meetingdetails.devicesInfo[part.deviceid] = part;
      }

      this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);

      await this.emitEvent(EVENTNAMES.NON_VERBAL, meetingId, {
        emoji: req.emoji,
        message: (req.action === "add") ? `${part.name} has reacted ${req.emoji}` : `${part.name} has removed ${req.emoji}`,
        socketid: req.conId,
        meetingdetails: meetingdetails,
        deviceid: part.deviceid,
        action: req.action
      });

    }
  }

  // private async hostRequesttoUnhide(inputdetails: any, meetInfo: any) {
  //      try {
  //         let { meetingId, jwtToken, sipLoginId, socketId, sipInfo } = meetInfo;
  //         const meetingdetails: any = await this.redisadaptor.getMeetingDetails(
  //           meetingId,
  //           jwtToken
  //         );
  //         let part: any = _.findWhere(meetingdetails.devicesInfo, {
  //           conId: socketId,
  //         });
  //         await this.emitEvent(
  //           EVENTNAMES.HOST_REQUEST_UNHIDE,
  //           inputdetails.socketId,
  //           {
  //             message: "The host requests you to unhide your video",
  //             meetingdetails: meetingdetails,
  //             hostdetails: part,
  //           }
  //         );
  //       } catch (e) {
  //         console.log("hostRequesttoUnhide", e);

  //         logger.info(`<<<< ERROR :::::  hostRequesttoUnhide >>>>>> `, e);
  //       }
  //     }



  cronExecution = async (meetingdetails: any, participant: any) => {
    try {
      let meeting_id: any = meetingdetails.meetingId;
      meetingdetails = await this.redisadaptor.getMeetingRedis(
        meeting_id
      );

      let existHost_details: any = _.findWhere(meetingdetails.devicesInfo, {
        isHost: true,
      });

      logger.info(
        "<<<< cron execution calling - meeting details>>>>>.",
        JSON.stringify(meetingdetails)
      );

      logger.info(
        "<<<<<< cron execution calling - host details >>>>>>",
        existHost_details
      );

      if (existHost_details.status == "network failure") {
        this.hostDisconnected(meetingdetails, participant);
      }
    } catch (error) {
      logger.info(`<<<< ERROR :::::  cronExecution >>>>>> `, error);
    }
  };

  private async transportErrorHandling(meetInfo: any, delay_sec?: any) {
    try {

      console.log("transportErrorHandling calling", meetInfo);
      logger.info("transportErrorHandling calling", meetInfo);
      let { meetingId, jwtToken, sipLoginId, socketId, sipInfo } = meetInfo;

      const meetingdetails: any =  await this.redisadaptor.getMeetingRedis(
        meetingId
      );

      let participant: any = _.findWhere(meetingdetails.devicesInfo, {
        conId: socketId,
      });


      let screen_participant: any = _.findWhere(meetingdetails.devicesInfo, {
        conId: `${socketId}_screenshare`,
      });

      if (screen_participant && screen_participant.isScreenshare) {

        screen_participant.status = "transport_error";
        screen_participant.closedCaption = false;

        screen_participant.isScreenshare = false;

        screen_participant.isWhiteboard = false;

        await this.rocketMQMiddleware.connectionRocket(
          meetingdetails,
          [screen_participant],
          EVENTNAMES.SCREENSHARE_STOP,
          screen_participant.conId
        );

        await this.emitEvent(EVENTNAMES.SCREEN_SHARE, meetingId, {
          message: false,
          socketid: screen_participant.conId,
          meetingdetails: meetingdetails,
        });

        let sdeviceid: any = screen_participant.deviceid;
        if (meetingdetails.devicesInfo.hasOwnProperty(sdeviceid)) {
          meetingdetails.devicesInfo[sdeviceid] = screen_participant;
        }
      }


      if (participant) {

        participant.status = "transport_error";

        participant.closedCaption = false;

        participant.isScreenshare = false;

        participant.isWhiteboard = false;

        // Sipid_details.status = "disconnected";

        let deviceid: any = participant.deviceid;
        if (meetingdetails.devicesInfo.hasOwnProperty(deviceid)) {
          meetingdetails.devicesInfo[deviceid] = participant;
        }
        let delay = (20000 - delay_sec)
        if (delay < 500) delay = 20000 + delay_sec;
        this.cronJob.transportErrorCron(meetingdetails, participant, delay);
      }


      let screenstatus_filter = _.filter(meetingdetails.devicesInfo, {
        isScreenshare: true,
      });

      let whitestatus_filter = _.filter(meetingdetails.devicesInfo, {
        isWhiteboard: true,
      });

      if (screenstatus_filter.length === 0) {
        meetingdetails.isScreenshareStarted = false;
        //  this.speechmatics.stopClosedcaption();
      }

      if (whitestatus_filter.length === 0) {
        meetingdetails.isWhiteboardStarted = false;
        //  this.speechmatics.stopClosedcaption();
      }

      this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);

    } catch (e) {
      console.log("deletion error", e);
    }
  }


  private async handleDisconnectEvent(devicedetails: any, e: any, meetInfo: any, delay?: any) {
    try {

      console.log("handleDisconnectEvent calling", e)

      let { meetingId, jwtToken, sipLoginId, socketId, sipInfo } = meetInfo;
      logger.info(`${socketId} handleDisconnectEvent calling ${e}`)
      const meetingdetails: any = await this.redisadaptor.getMeetingRedis(
        meetingId
      );

      let Sipid_details: any = _.findWhere(meetingdetails.devicesInfo, {
        conId: socketId,
      });

      if (Sipid_details) {

        Sipid_details.closedCaption = false;

        Sipid_details.isScreenshare = false;

        Sipid_details.isWhiteboard = false;
        Sipid_details.isReconnectstatus = false;

        // Sipid_details.status = "disconnected";

        if (
          meetingdetails.devicesInfo.hasOwnProperty(Sipid_details.deviceid)
        ) {
          meetingdetails.devicesInfo[Sipid_details.deviceid] = Sipid_details;
        }
      }



      let screen_participant: any = _.findWhere(meetingdetails.devicesInfo, {
        conId: `${socketId}_screenshare`,
      });

      if (screen_participant && screen_participant.isScreenshare) {

        screen_participant.status = "transport_error";
        screen_participant.closedCaption = false;

        screen_participant.isScreenshare = false;

        screen_participant.isWhiteboard = false;

        await this.rocketMQMiddleware.connectionRocket(
          meetingdetails,
          [screen_participant],
          EVENTNAMES.SCREENSHARE_STOP,
          screen_participant.conId
        );

        await this.emitEvent(EVENTNAMES.SCREEN_SHARE, meetingId, {
          message: false,
          socketid: screen_participant.conId,
          meetingdetails: meetingdetails,
        });

        let sdeviceid: any = screen_participant.deviceid;
        if (meetingdetails.devicesInfo.hasOwnProperty(sdeviceid)) {
          meetingdetails.devicesInfo[sdeviceid] = screen_participant;
        }
      }

      let screenstatus_filter = _.filter(meetingdetails.devicesInfo, {
        isScreenshare: true,
      });

      let whitestatus_filter = _.filter(meetingdetails.devicesInfo, {
        isWhiteboard: true,
      });



      if (screenstatus_filter.length === 0) {
        meetingdetails.isScreenshareStarted = false;
        //  this.speechmatics.stopClosedcaption();
      }

      if (whitestatus_filter.length === 0) {
        meetingdetails.isWhiteboardStarted = false;
        //  this.speechmatics.stopClosedcaption();
      }


      await this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);

      if (e === "ping timeout") {
        if (Sipid_details) {

          let actualDelay = (30000 - delay);

          setTimeout(async () => {

            this.deleteConnection(meetInfo);
          }, actualDelay)


        }
      } else if (e === "transport close") {

        /*         if (isDisconneted === false) {
        
                  console.log("disconnect boolean value false  section calling")
                  await this.transportErrorHandling(meetInfo)
                } else {
        
                  console.log("disconnect boolean value true section calling");
                  isDisconneted = false
                } */
      } else if (e === "transport error") {

        let filterList = _.without(meetingdetails.joinedParticipantlist, _.findWhere(meetingdetails.joinedParticipantlist, {
          deviceid: Sipid_details.deviceid
        }));


        let _filterList = _.without(filterList, _.findWhere(filterList, {
          deviceid: `${Sipid_details.deviceid}_screenshare`
        }));
        console.log("socketId >>>>>>", socketId)
        console.log("filterList >>>>>>", filterList)

        meetingdetails.joinedParticipantlist = _filterList;

        if (Sipid_details) {
          Sipid_details.status = "disconnected";
          Sipid_details.isReconnectstatus = false;
          Sipid_details.cc_lang = "en";
          Sipid_details.speaker_lang = "en";
          Sipid_details.recordingStatus = "stop";

          if (
            meetingdetails.devicesInfo.hasOwnProperty(Sipid_details.deviceid)
          ) {
            meetingdetails.devicesInfo[Sipid_details.deviceid] =
              Sipid_details;
          }
        }

        await this.rocketMQMiddleware.connectionRocket(
          meetingdetails,
          [Sipid_details],
          EVENTNAMES.USER_TERMINATED,
          socketId
        );
        let data: any = {

          connection_id: Sipid_details.conId,
          meeting_id: meetingId,
          cc_lang: "en",
          cc_state: Sipid_details.closedCaption,
          speaker_name: Sipid_details.name,
          is_host: Sipid_details.isHost,
          device_id: Sipid_details.deviceid,
          image_url: Sipid_details.profileImg,
          join_status: "left",
          speaker_lang: "en",
          status: 4
        }
        await this.videoccpublisher.videoccpublish("subchannel", JSON.stringify(data));
        await this.emitEvent(
          EVENTNAMES.MEETING_STATUS,
          meetingdetails.meetingId,
          {
            message: `${Sipid_details.name} Disconnected from the meeting`,
            socketid: Sipid_details.conId,
            meetingdetails: meetingdetails,
          }
        );
        await this.emitEvent(
          EVENTNAMES.CLIENT_UPDATED_PARTICIPANTLIST,
          meetingdetails.meetingId,
          {
            participantList: meetingdetails.joinedParticipantlist,
          }
        );
        //}

        await this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);

      } else {
        if (Sipid_details && e !== "server namespace disconnect") {
          Sipid_details.status = "disconnected";
          Sipid_details.isReconnectstatus = false;
          if (
            meetingdetails.devicesInfo.hasOwnProperty(Sipid_details.deviceid)
          ) {
            meetingdetails.devicesInfo[Sipid_details.deviceid] =
              Sipid_details;
          }
          console.log("<<<<<<<<<<DISCONNECT EVENT CALLOING>>>>>>>>>>")
          await this.rocketMQMiddleware.connectionRocket(
            meetingdetails,
            [Sipid_details],
            EVENTNAMES.USER_TERMINATED,
            socketId
          );


          let filterList = _.without(meetingdetails.joinedParticipantlist, _.findWhere(meetingdetails.joinedParticipantlist, {
            deviceid: Sipid_details.deviceid
          }));

          let _filterList = _.without(filterList, _.findWhere(filterList, {
            deviceid: `${Sipid_details.deviceid}_screenshare`
          }));
          meetingdetails.joinedParticipantlist = _filterList;

          await this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);



          //if(e !== "transport close"){

          await this.emitEvent(
            EVENTNAMES.MEETING_STATUS,
            meetingdetails.meetingId,
            {
              message: `${Sipid_details.name} Disconnected from the meeting`,
              socketid: Sipid_details.conId,
              meetingdetails: meetingdetails,
            }
          );

          await this.emitEvent(
            EVENTNAMES.CLIENT_UPDATED_PARTICIPANTLIST,
            meetingdetails.meetingId,
            {
              participantList: meetingdetails.joinedParticipantlist,
            }
          );

          //}

        }
      }

    } catch (e) {
      console.log("deletion error", e);
    }
  }


  transportErrorCronExecution = async (meetingdetails: any, participant: any) => {

    // console.log("transportErrorCronExecution >>>", JSON.stringify(meetingdetails));

    console.log("transportErrorCronExecution participant >>>", JSON.stringify(participant));
    logger.info("transportErrorCronExecution participant >>>", JSON.stringify(participant));

    try {

      let meetingId: any = meetingdetails.meetingId;
      const adapter: RedisAdapter = this.io.of("/")
        .adapter as RedisAdapter;


      meetingdetails = await this.redisadaptor.getMeetingRedis(
        meetingId
      );


      let existdevice_details: any = _.findWhere(meetingdetails.devicesInfo, {
        deviceid: participant.deviceid,
      });

      console.log("transportErrorCronExecution existHost_details >>>", JSON.stringify(existdevice_details));
      //if (existdevice_details && existdevice_details?.sipId == participant?.sipId && (existdevice_details?.status == "transport_error" || existdevice_details?.status == "dtls_connection_failure")) {


      await this.rocketMQMiddleware.connectionRocket(
        meetingdetails,
        [participant],
        EVENTNAMES.USER_TERMINATED,
        participant.conId
      );

      let filterList = _.without(meetingdetails.joinedParticipantlist, _.findWhere(meetingdetails.joinedParticipantlist, {
        deviceid: participant.deviceid
      }));

      let _filterList = _.without(filterList, _.findWhere(filterList, {
        deviceid: `${participant.deviceid}_screenshare`
      }));

      let existingDeviceDetailsStatus: any = "";
      if (existdevice_details) {

        existingDeviceDetailsStatus = existdevice_details.status
        if (existdevice_details.isScreenshare) {
          await this.rocketMQMiddleware.connectionRocket(
            meetingdetails,
            [existdevice_details],
            EVENTNAMES.SCREENSHARE_STOP,
            existdevice_details.conId
          );
        }
        existdevice_details.closedCaption = false;

        existdevice_details.isScreenshare = false;

        existdevice_details.isWhiteboard = false;
        existdevice_details.isReconnectstatus = false;
        existdevice_details.cc_lang = "en";
        existdevice_details.speaker_lang = "en";
        existdevice_details.recordingStatus = "stop";
        existdevice_details.status = "disconnected";

        if (
          meetingdetails.devicesInfo.hasOwnProperty(existdevice_details.deviceid)
        ) {
          meetingdetails.devicesInfo[existdevice_details.deviceid] = existdevice_details;
        }
      }

      let screenstatus_filter = _.filter(meetingdetails.devicesInfo, {
        isScreenshare: true,
      });

      let whitestatus_filter = _.filter(meetingdetails.devicesInfo, {
        isWhiteboard: true,
      });



      if (screenstatus_filter.length === 0) {
        meetingdetails.isScreenshareStarted = false;
        //  this.speechmatics.stopClosedcaption();
      }

      if (whitestatus_filter.length === 0) {
        meetingdetails.isWhiteboardStarted = false;
        //  this.speechmatics.stopClosedcaption();
      }

      meetingdetails.joinedParticipantlist = _filterList;
      await this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);

      /*  await this.rocketMQMiddleware.connectionRocket(
         meetingdetails,
         [participant],
         EVENTNAMES.UPDATE_PARTICIPANTLIST,
         participant.conId
       ); */

      console.log("existdevice_details?.deviceid >>>>>>>", existdevice_details?.deviceid);
      console.log("participant.deviceid >>>>>>>", participant.deviceid);
      console.log("existingDeviceDetailsStatus >>>>>>>", existingDeviceDetailsStatus);

      if (existdevice_details && existdevice_details?.deviceid == participant.deviceid && (existingDeviceDetailsStatus == "transport_error" || existingDeviceDetailsStatus == "dtls_connection_failure")) {

        console.log("Transport close exe >>>>>>", meetingdetails.meetingId)
        let data: any = {

          Connection_Id: participant.conId,
          meeting_id: meetingId,
          cc_lang: "en",
          cc_state: participant.closedCaption,
          speaker_name: participant.name,
          is_host: participant.isHost,
          device_id: participant.deviceid,
          image_url: participant.profileImg,
          join_status: "left",
          speaker_lang: "en",
          status: 4
        }
        await this.videoccpublisher.videoccpublish("subchannel", JSON.stringify(data));
        console.log("EMITTINHTransport close exe >>>>>>", meetingdetails.meetingId)
        await this.emitEvent(
          EVENTNAMES.MEETING_STATUS,
          meetingdetails.meetingId,
          {
            message: `${participant.name} Disconnected from the meeting`,
            socketid: participant.conId,
            meetingdetails: meetingdetails,
          }
        );
        await this.emitEvent(
          EVENTNAMES.CLIENT_UPDATED_PARTICIPANTLIST,
          meetingdetails.meetingId,
          {
            participantList: meetingdetails.joinedParticipantlist,
          }
        );
      }

      adapter
        .remoteDisconnect(participant.conId, meetingId)
        .then((x: any) => { })
        .catch((x) => {
          console.log(x);
        });
      //}

    } catch (error) {
      console.log("cronExecutionHostLeave >>>", error)
    }


  }

  cronExecutionHostLeave = async (meetingdetails: any, participant: any) => {

    try {

      let meetingId: any = meetingdetails.meetingId;
      const adapter: RedisAdapter = this.io.of("/")
        .adapter as RedisAdapter;
      if (participant.isHost) {
        /*  let existHost_details: any = _.findWhere(meetingdetails.devicesInfo, {
           isHost: true,
         });
  */

        // if (existHost_details.status == "network failure") {


        for (var i in meetingdetails.devicesInfo) {
          if (meetingdetails.devicesInfo[i]) {
            adapter
              .remoteDisconnect(meetingdetails.devicesInfo[i].conId, meetingId)
              .then((x: any) => { })
              .catch((x) => {
                console.log(x);
              });
          }
        }

        meetingdetails.joinedParticipantlist = [];
        this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);

        /* await this.rocketMQMiddleware.connectionRocket(
          meetingdetails,
          [participant],
          EVENTNAMES.UPDATE_PARTICIPANTLIST,
          participant.conId
        ); */

        await this.emitEvent(
          EVENTNAMES.CLIENT_UPDATED_PARTICIPANTLIST,
          meetingdetails.meetingId,
          {
            participantList: meetingdetails.joinedParticipantlist,
          }
        );

        this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);
        // }
      } else {

        /*  let existHost_details: any = _.findWhere(meetingdetails.devicesInfo, {
           conId: participant.conId,
         }); */

        //if (existHost_details.status == "network failure") {


        await this.rocketMQMiddleware.connectionRocket(
          meetingdetails,
          [participant],
          EVENTNAMES.USER_TERMINATED,
          participant.conId
        );

        let filterList = _.without(meetingdetails.joinedParticipantlist, _.findWhere(meetingdetails.joinedParticipantlist, {
          deviceid: participant.deviceid
        }));

        let _filterList = _.without(filterList, _.findWhere(filterList, {
          deviceid: `${participant.deviceid}_screenshare`
        }));


        participant.status = "network failure";
        participant.isReconnectstatus = false;
        participant.cc_lang = "en";
        participant.speaker_lang = "en";
        participant.recordingStatus = "stop";

        if (
          meetingdetails.devicesInfo.hasOwnProperty(participant.deviceid)
        ) {
          meetingdetails.devicesInfo[participant.deviceid] =
            participant;
        }

        meetingdetails.joinedParticipantlist = _filterList;
        await this.redisadaptor.setMeetingDetails(meetingId, meetingdetails);

        /*  await this.rocketMQMiddleware.connectionRocket(
           meetingdetails,
           [participant],
           EVENTNAMES.UPDATE_PARTICIPANTLIST,
           participant.conId
         ); */

        let data: any = {

          connection_id: participant.conId,
          meeting_id: meetingId,
          cc_lang: "en",
          cc_state: participant.closedCaption,
          speaker_name: participant.name,
          is_host: participant.isHost,
          device_id: participant.deviceid,
          image_url: participant.profileImg,
          join_status: "left",
          speaker_lang: "en",
          status: 4
        }
        await this.videoccpublisher.videoccpublish("subchannel", JSON.stringify(data));
        await this.emitEvent(
          EVENTNAMES.MEETING_STATUS,
          meetingdetails.meetingId,
          {
            message: `${participant.name} Disconnected from the meeting`,
            socketid: participant.conId,
            meetingdetails: meetingdetails,
          }
        );
        await this.emitEvent(
          EVENTNAMES.CLIENT_UPDATED_PARTICIPANTLIST,
          meetingdetails.meetingId,
          {
            participantList: meetingdetails.joinedParticipantlist,
          }
        );
        adapter
          .remoteDisconnect(participant.conId, meetingId)
          .then((x: any) => { })
          .catch((x) => {
            console.log(x);
          });
        //}




      }




    } catch (error) {
      console.log("cronExecutionHostLeave >>>", error)
    }


  }

  private async getJoinedList(joinedstatus_filter: any) {

    joinedstatus_filter.sort(function compare(a: any, b: any) {
      let dateA: any = new Date(a.joinedTime);
      let dateB: any = new Date(b.joinedTime);
      return dateA - dateB;
    });


    let filterd_joinedList = [];
    for (let joinindex = 0; joinindex < joinedstatus_filter.length; joinindex++) {
      let joinedParticipantDetails = {
        conId: joinedstatus_filter[joinindex]?.connection_id,
        user_id: joinedstatus_filter[joinindex]?.user_id,
        user_type: joinedstatus_filter[joinindex]?.user_type
      }

      filterd_joinedList.push(joinedParticipantDetails)

    }

    return filterd_joinedList;
  }


  receiveConfBridgeMessage = async (responsemessage: any) => {

    console.log(`<<<<< receiveConfBridgeMessage >>>>  ${new Date().getTime()}`, responsemessage)
    // logger.info(`<<<<< receiveConfBridgeMessage >>>>  ${new Date().getTime()}`, responsemessage);
    try {

      if (responsemessage) {


        let duplicateMessage = JSON.parse(responsemessage);
        if (duplicateMessage.ur_conf_message) {
          let deuplicate_event_sequence = duplicateMessage.ur_conf_message.ur_conf_event.event_sequence;
          this.queuesList.push({ id: deuplicate_event_sequence, value: responsemessage })

          // console.log("Pushing.....", queuesList);

          console.log("<<<<<< this.queuesList >>>>>", this.queuesList.length, this.isRedisResponseProcessing);
          logger.info("<<<<<< this.queuesList >>>>>", this.queuesList.length, this.isRedisResponseProcessing);
          // return new Promise(async (resv, rej) => {
          if (!this.isRedisResponseProcessing) {
            await this.rocketMQConnect(responsemessage, false)
          }


        }
        // });

      }
    } catch (error) {
      console.log("receiveConfBridgeMessage >>>>>>>", error)
    }


  }

  queueHandling = async (eventSeq: any) => {

    //console.log("queueHandling >>>>>>>>>>>>>>>>>>>>>>", eventSeq);
    // console.log("Before this.queuesList >>>> >>>>>>>>>>>>>>>>>>>>>>", this.queuesList)
    setTimeout(() => {
      let q_arr = _.without(this.queuesList, _.findWhere(this.queuesList, {
        id: eventSeq,
      }));
      this.queuesList = q_arr

      console.log("this.queuesList >>>>", this.queuesList);
      if (this.queuesList.length > 0) {
        this.rocketMQConnect(q_arr[0].value, true);
      } else {
        this.isRedisResponseProcessing = false
      }
    }, 100)

  }

  rocketMQConnect = async (responsemessage: any, next?: boolean) => {
    console.log("<<<<< receiving data from rocketmq >>>>", responsemessage);

    //logger.info("<<<<< receiving data from rocketmq >>>>", responsemessage);
    if (responsemessage) {
      try {

        if (this.isRedisResponseProcessing && !next) {
          console.log("returning >>>>")
          return;
        }

        const message = JSON.parse(responsemessage);

        if (message.ur_conf_message) {

          const { event_name, event_time, event_sequence } =
            message.ur_conf_message.ur_conf_event;

          const eventSeq = message.ur_conf_message.ur_conf_event.event_sequence

          // console.log("eventSeq >>>>>", eventSeq);

          this.isRedisResponseProcessing = true
          if (event_name !== EVENTNAMES.SDP_UPDATE_RESPONSE) {
            // console.log("<<<<< receiving data from rocketmq >>>>", responsemessage);
            logger.info("<<<<< receiving data from rocketmq >>>>", responsemessage);
          }


          const { video_enabled, audio_enabled, recording_enabled } =
            message.ur_conf_message.ur_media_info;

          /*           let meetingdetails: any =
                      await this.redisadaptor.getMeetingInfofromRedis(meeting_id); */

          //console.log("event_name >>>>>>>", event_name);
          if (event_name == "agent_joined") {
            console.log("agent joined>>>>>>>>>>>");
            const { ur_user_info } = message.ur_conf_message;
            const user_info_message = message;

            const { meeting_id, meeting_name, host_joined } =
              user_info_message.ur_conf_message.ur_conf_meeting_info;


            const meetingdetails: any =
              await this.redisadaptor.getMeetingRedis(meeting_id);

            let joinedList = meetingdetails.joinedParticipantlist || [];

            console.log("joinedList >>>>.", joinedList);

            const user_info_details = ur_user_info[0]

            //console.log("host_joined >>>>>>>", user_info_details);

            logger.info("<<<<< host_joined >>>>", user_info_details);
            const socketId = user_info_details?.connection_id;
            const device_id = user_info_details?.device_id;
            const sip_id = user_info_details?.sip_id;

            let joinedDetails = {
              conId: user_info_details?.connection_id,
              user_id: user_info_details?.user_id,
              user_type: user_info_details?.user_type,
              deviceid: device_id
            }

            let f_arr = _.without(joinedList, _.findWhere(joinedList, {
              deviceid: device_id,
            }));

            f_arr.push(joinedDetails);

            // console.log("f_arr >>>>>>>", f_arr);
            let devicedetails: any = _.findWhere(meetingdetails.devicesInfo, {
              deviceid: device_id, sipId: sip_id
            });

            let join_list = _.uniq(f_arr, 'conId')
            logger.info("joinedList >>>>>>>", join_list);
            meetingdetails.joinedParticipantlist = join_list;
            //console.log("devicedetails >>>>>>>", devicedetails);
            logger.info("devicedetails >>>>>>>", devicedetails);

            //console.log("meetingdetails.joinedParticipantlist >>>>>", meetingdetails.joinedParticipantlist);


            if (devicedetails) {
              devicedetails.status = "joined";
              devicedetails.joinedTime = new Date().getTime();
              devicedetails.isHostAllowed = true;
              devicedetails.isReconnectstatus = false;
              devicedetails.conId = user_info_details?.connection_id;
              devicedetails.moveToWaitingRoom = false;
              devicedetails.removeFromMeeting = false;
              if(meetingdetails.transferStatus ==="success"){  let agentDet:any=meetingdetails.meetingDetails.agentDetails;

                agentDet={...agentDet,ext:meetingdetails.secondaryAgentDetails.ext.toString(),sip_login_id:user_info_details?.sip_id,Email:meetingdetails.secondaryAgentDetails.emailId,username:meetingdetails.secondaryAgentDetails.UserName,role_id:meetingdetails.secondaryAgentDetails.role_id }
                meetingdetails.meetingDetails.agentDetails=agentDet;
              }
            } else {
              devicedetails = {
                video: true,
                mute: false,
                pined: false,
                name: user_info_details?.user_id || "",
                isHost: true,
                sipId: user_info_details?.sip_id,
                isAlternateHost: false,
                isRecordingAllowed: false,
                moveToWaitingRoom: false,
                profileImg: "",
                status: "joined", //"joined" / "inwaitingroom" / "removed";
                hand: false,
                ext: "",
                isGuest: false,
                roleType: 1,
                removeFromMeeting: false,
                email: "",
                isHostAllowed: true,
                waiting_retrycount: 0,
                recordingStatus: "stop",
                closedCaption: false,
                isScreenshare: false,
                isWhiteboard: false,
                isMirror: true,
                joinedTime: new Date().getTime(),
                isVoiceengineStart: false,
                isReconnectstatus: false,
                recordFilePath: configuration.videoRecordFileURL,
                reactions: "",
                non_verbal: "",
                cc_lang: "en",
                speaker_lang: "en"
              };
              devicedetails.status = "joined";
              devicedetails.isHostAllowed = true;
              devicedetails.isReconnectstatus = false;
              devicedetails.conId = user_info_details?.connection_id
              devicedetails.deviceid = user_info_details?.device_id;
              devicedetails.name = user_info_details?.user_id;
              if(meetingdetails.isSecondaryAgent){  let agentDet:any=meetingdetails.meetingDetails.agentDetails;

                agentDet={...agentDet,ext:meetingdetails.secondaryAgentDetails.ext.toString(),sip_login_id:user_info_details?.sip_id,Email:meetingdetails.secondaryAgentDetails.emailId,username:meetingdetails.secondaryAgentDetails.UserName,role_id:meetingdetails.secondaryAgentDetails.role_id }
                meetingdetails.meetingDetails.agentDetails=agentDet;}
            }


            if (devicedetails) {
              //console.log("Insside participant >>>>>>", devicedetails);
              /*   devicedetails.status = "joined";
                devicedetails.joinedTime = new Date().getTime();
                devicedetails.isHostAllowed = true;
                devicedetails.isReconnectstatus = false;
                devicedetails.conId = user_info_details?.connection_id */
              let ccdata: any = {

                connection_id: user_info_details?.connection_id,
                meeting_id: meeting_id,
                cc_lang: "en",
                cc_state: devicedetails.closedCaption,
                speaker_name: devicedetails.name,
                is_host: devicedetails.isHost,
                device_id: devicedetails.deviceid,
                image_url: devicedetails.profileImg,
                join_status: "joining",
                speaker_lang: "en",
                status: 1
              }
              await this.videoccpublisher.videoccpublish("subchannel", JSON.stringify(ccdata));

              /*  if (
                 meetingdetails.devicesInfo.hasOwnProperty(
                   user_info_details.device_id
                 )
               ) { */
              meetingdetails.devicesInfo[user_info_details?.device_id] =
                devicedetails;
              //}



              /*  await this.rocketMQMiddleware.connectionRocket(
                 meetingdetails,
                 [devicedetails],
                 EVENTNAMES.UPDATE_PARTICIPANTLIST,
                 socketId
               ); */

              let waitingstatus_filter = _.filter(meetingdetails.devicesInfo, {
                status: "inWaitingroom",
              });

              /* let joinedstatus_filter = _.filter(meetingdetails.devicesInfo, {
                status: "joined",
              });
 */
              //console.log("joinedstatus_filter Array>>>>>>", joinedstatus_filter);
              // console.log("before meetingdetails >>>>>>", meetingdetails)




              // for (let jindex = 0; jindex < joinedstatus_filter.length; jindex++) {
              //const joinelement = joinedstatus_filter[jindex];

              const getData: any = await getSettings(meetingdetails.meetingDetails.agentDetails.domain_id);
              if (!_.isEmpty(getData)) {
              console.log('getData agent joined >>>>>>>>>>>>', getData);
                await this.emitEvent(EVENTNAMES.VM_SETTINGS, socketId, {
                  meetingdetails: meetingdetails,
                  settings: getData,
                  socketid: socketId,
                });
              }

              await this.emitEvent(EVENTNAMES.CONNECTION_STATUS, socketId, {
                meetingdetails: meetingdetails,
                isConnected: "connected",
                socketid: socketId,
              });

              await this.emitEvent(EVENTNAMES.MEETING_STATUS, meeting_id, {
                message: `${devicedetails.name} Joined the meeting`,
                socketid: socketId,
                meetingdetails: meetingdetails,
              });


              if (meetingdetails.joinedParticipantlist.length > 1) {
                await this.emitEvent(
                  EVENTNAMES.CLIENT_UPDATED_PARTICIPANTLIST,
                  meeting_id,
                  {
                    participantList: meetingdetails.joinedParticipantlist,
                  }
                );
              }

              //}

              // console.log("meetingdetails.isHostjoined >>>>>", meetingdetails.isHostjoined);
              //console.log("meetingdetails.isParticipantjoinbefhost >>>>>", meetingdetails.isParticipantjoinbefhost);
              if ((!meetingdetails.isHostjoined)) {

                let host_not_joined_filter = _.filter(meetingdetails.devicesInfo, {
                  status: "host_not_joined",
                });

                console.log("host_not_joined_filter >>>>", host_not_joined_filter);
                logger.info("host_not_joined_filter >>>>", host_not_joined_filter);

                // console.log("meetingdetailsmeetingdetails",meetingdetails);
                meetingdetails.isHostjoined = true;
                await this.emitEvent(EVENTNAMES.HOST_STATUS, meeting_id, {
                  message: "Host joined",
                  meetingdetails: meetingdetails,
                });

                let that = this;
                for (let hindex = 0; hindex < host_not_joined_filter.length; hindex++) {
                  (function (hindex) {
                    setTimeout(async function () {


                      const helement = host_not_joined_filter[hindex];

                      helement.status = "waiting room initialized";

                      // console.log("helement >>>>>>>", helement);

                      if (meetingdetails.devicesInfo.hasOwnProperty(helement.deviceid)) {
                        meetingdetails.devicesInfo[helement.deviceid] = helement;
                      }
                      await that.emitEvent(EVENTNAMES.JOIN_INITIALIZED, helement.conId, {
                        message: "send_invite",
                        user_details: helement,
                        socketid: helement.conId,
                        meetingdetails: meetingdetails,
                      });

                      await that.rocketMQMiddleware.connectionRocket(
                        meetingdetails,
                        [helement],
                        EVENTNAMES.PARTICIPANT_JOINIG,
                        helement.conId
                      );
                      await that.redisadaptor.setMeetingDetails(meeting_id, meetingdetails);

                    }, 500 * (hindex + 1));
                  })(hindex);
                }

              }

              meetingdetails.isHostjoined = true;

              // console.log("waitingstatus_filter Array>>>>>>", waitingstatus_filter);
              //console.log("joinedstatus_filter Array>>>>>>", joinedstatus_filter);
              for (let sindex = 0; sindex < waitingstatus_filter.length; sindex++) {
                const element = waitingstatus_filter[sindex];

                //if(!element.mute) 

                //console.log("waitingstatus_filter >>>>>>", element);

                let w_meetingdetails = meetingdetails
                w_meetingdetails.isHostjoined = true;
                await this.emitEvent(EVENTNAMES.HOST_STATUS, element.conId, {
                  message: "Host joined",
                  meetingdetails: w_meetingdetails,
                });

              }



              // console.log("meetingdetails >>>>>", meetingdetails);
              await this.redisadaptor.setMeetingDetails(meeting_id, meetingdetails);

              logger.info("Host connect status", devicedetails)


              console.log("End of host joined>>>>>>>>")

              await this.queueHandling(eventSeq);


            }

          } else if (event_name == "customer_joined") {


            const { ur_user_info } = message.ur_conf_message;
            console.log("inside participant joined section>>>>>>");
            //logger.info("inside participant joined section>>>>>>", ur_user_info);
            const part_info_message = message

            const { meeting_id, meeting_name, host_joined } =
              part_info_message.ur_conf_message.ur_conf_meeting_info;


            const meetingdetails: any =
              await this.redisadaptor.getMeetingRedis(meeting_id);

            if (meetingdetails.meetStartTime === 0) {
              // meetingdetails.meetStartTime = new Date().getTime();
              let currentTime:any=new Date(Date.now());
              meetingdetails.meetStartTime=currentTime.setMinutes(currentTime.getMinutes() - 2);
              await this.redisadaptor.setMeetingDetails(meeting_id, meetingdetails);
            }
            //     if(meet_details !== null && meet_details.meetings_type === 2){
            //       meet_details.start_timestamp=new Date().getTime();
            //       meet_details.end_meeting= new Date(meet_details.start_timestamp).setHours(new Date(meet_details.start_timestamp).getHours() + 1);
            //       let dur:any= (meet_details.end_timestamp - meet_details.start_timestamp)/60000 >= 1 ? Math.round((meet_details.end_timestamp - meet_details.start_timestamp)/60000) : (meet_details.end_timestamp - meet_details.start_timestamp)/60000 
            //       meet_details.duration_minutes=dur;
            //       meetingdetails.duration=dur;
            //       meetingdetails.start_timestamp = new Date().getTime();
            //       meetingdetails.end_meeting=new Date(meetingdetails.start_timestamp).setHours(new Date(meetingdetails.start_timestamp).getHours() + 1);
            //       console.log("meetingdetailsendtime",meetingdetails.end_meeting,"meetingdetails.duration",meetingdetails.duration);

            //       this.redisadaptor
            //       ?.updateMeetingWithoutToken(meeting_id, meet_details)
            //       .then((response: any) => { })
            //       .catch((e:any) => {
            //         new Error("Internal server error");
            //       });
            //     }
            //     let sipid = Object.keys(meetingdetails.participants) || [];
            //     console.log("updateTimeee >>>>>**",sipid);
            //     console.log("ongoing.participants >>>>>**",meetingdetails.participants);
            //     ioredis.to(sipid).emit("ongoing_notification", meetingdetails);
            // }
            // console.log("meetingdetails >>>>>>", meetingdetails);

            logger.info("inside participant joined section>>>>>>", meetingdetails);
            // console.log("inside participant joined section>>>>>>", meetingdetails);
            let joinedList = meetingdetails.joinedParticipantlist || [];

            console.log("joinedList >>>>.", joinedList);

            const part_info_details = ur_user_info[0];
            const device_id = part_info_details?.device_id;
            const sip_id = part_info_details?.sip_id;
            logger.info("<<<<< participant_joined >>>>", part_info_details);

            let joinedDetails = {
              conId: part_info_details?.connection_id,
              user_id: part_info_details?.user_id,
              user_type: part_info_details?.user_type,
              deviceid: device_id
            }

            let f_arr = _.without(joinedList, _.findWhere(joinedList, {
              deviceid: device_id,
            }));

            f_arr.push(joinedDetails);

            // console.log("f_arr >>>>>>>", f_arr);
            let join_list = _.uniq(f_arr, 'conId')

            // console.log("joinedList >>>>>>>", join_list);
            logger.info("joinedList >>>>>>>", join_list);
            meetingdetails.joinedParticipantlist = join_list;

            const socketId = part_info_details.connection_id;




            let participant: any = _.findWhere(meetingdetails.devicesInfo, {
              deviceid: device_id, sipId: sip_id
            });

            if (participant) {
              participant.status = "joined";
              participant.joinedTime = new Date().getTime();
              participant.isHostAllowed = true;
              participant.isReconnectstatus = false;
              participant.conId = part_info_details?.connection_id;
              participant.moveToWaitingRoom = false;
              participant.removeFromMeeting = false;
            } else {
              participant = {
                video: true,
                mute: false,
                pined: false,
                name: part_info_details?.user_id || "",
                isHost: false,
                sipId: part_info_details?.sip_id,
                isAlternateHost: false,
                isRecordingAllowed: false,
                moveToWaitingRoom: false,
                profileImg: "",
                status: "joined", //"joined" / "inwaitingroom" / "removed";
                hand: false,
                ext: "",
                isGuest: false,
                roleType: 3,
                removeFromMeeting: false,
                email: "",
                isHostAllowed: true,
                waiting_retrycount: 0,
                recordingStatus: "stop",
                closedCaption: false,
                isScreenshare: false,
                isWhiteboard: false,
                isMirror: true,
                joinedTime: new Date().getTime(),
                isVoiceengineStart: false,
                isReconnectstatus: false,
                recordFilePath: configuration.videoRecordFileURL,
                reactions: "",
                non_verbal: "",
                cc_lang: "en",
                speaker_lang: "en"
              };
              participant.status = "joined";
              participant.isHostAllowed = true;
              participant.isReconnectstatus = false;
              participant.conId = part_info_details?.connection_id
              participant.deviceid = part_info_details?.device_id;
              participant.name = part_info_details?.user_id;

            }

            //console.log("devicedetails >>>>>>>", participant);
            logger.info("devicedetails >>>>>>>", participant);
            //console.log("meetingdetails.joinedParticipantlist >>>>>", meetingdetails.joinedParticipantlist);
            if (participant) {

              //console.log("Insside participant >>>>>>", participant)
              let Name = participant.name;
              let deviceid = participant.deviceid;

              /*  participant.conId = part_info_details?.connection_id
               participant.status = "joined";
               participant.moveToWaitingRoom = false;
               participant.joinedTime = new Date().getTime();
               participant.removeFromMeeting = false;
               participant.isHostAllowed = true;
               participant.isReconnectstatus = false; */
              let data: any = {

                connection_id: participant.conId,
                meeting_id: meeting_id,
                cc_lang: "en",
                cc_state: participant.closedCaption,
                speaker_name: participant.name,
                is_host: participant.isHost,
                device_id: participant.deviceid,
                image_url: participant.profileImg,
                join_status: "joining",
                speaker_lang: "en",
                status: 1
              }
              await this.videoccpublisher.videoccpublish("subchannel", JSON.stringify(data));
              // if (meetingdetails.devicesInfo.hasOwnProperty(deviceid)) {
              meetingdetails.devicesInfo[deviceid] = participant;
              //}
              meetingdetails.meetingDetails.agentDetails.sessionId=part_info_details.call_id;
              await this.redisadaptor.setMeetingDetails(meeting_id, meetingdetails);

              // console.log("meetingdetails >>>>>", meetingdetails);
              //console.log("meetign _id", meeting_id);

              /*  await this.rocketMQMiddleware.connectionRocket(
                 meetingdetails,
                 [participant],
                 EVENTNAMES.UPDATE_PARTICIPANTLIST,
                 socketId
               ); */
              let joinedstatus_filter = _.filter(meetingdetails.devicesInfo, {
                status: "joined",
              });

              console.log("joinedstatus_filter Array>>>>>>", joinedstatus_filter);

              //for (let jindex = 0; jindex < joinedstatus_filter.length; jindex++) {
              // const joinelement = joinedstatus_filter[jindex];

              // const getData: any = await getSettings(meetingdetails.meetingDetails.agentDetails.domain_id);
              //   console.log('getData**********', getData);
              //   if (!_.isEmpty(getData)) {
              //     await this.emitEvent(EVENTNAMES.VM_SETTINGS, socketId, {
              //       meetingdetails: meetingdetails,
              //       settings: getData,
              //       socketid: socketId,
              //     });
              //   } 

              await this.emitEvent(EVENTNAMES.CONNECTION_STATUS, socketId, {
                meetingdetails: meetingdetails,
                isConnected: "connected",
                socketid: socketId,
              });

              await this.emitEvent(EVENTNAMES.MEETING_STATUS, meeting_id, {
                message: `${Name} Joined the meeting`,
                socketid: socketId,
                meetingdetails: meetingdetails,
              });

              if (meetingdetails.joinedParticipantlist.length > 1) {
                await this.emitEvent(
                  EVENTNAMES.CLIENT_UPDATED_PARTICIPANTLIST,
                  meeting_id,
                  {
                    participantList: meetingdetails.joinedParticipantlist,
                  }
                );
              }


              //}

              /*              await this.emitEvent(EVENTNAMES.JOINMEETING, meeting_id, {
                             message: `${Name} Joined the meeting`,
                             socketid: socketId,
                             meetingdetails: meetingdetails,
                           });
              */

              logger.info("Particioant connect status", participant)


              await this.queueHandling(eventSeq);
            }
            else {
              await this.queueHandling(eventSeq);
            }
          }
          else if (event_name == "queue_annc_text") {
            const { ur_user_info, ur_ccaas_info } = message.ur_conf_message;
            const user_info_message = message

            const { meeting_id, meeting_name, host_joined } =
              user_info_message.ur_conf_message.ur_conf_meeting_info;


            const meetingdetails: any =
              await this.redisadaptor.getMeetingRedis(meeting_id);

            const part_info_details = ur_user_info[0];
            const socketID = part_info_details.connection_id;

            let Sipid_details: any = _.findWhere(meetingdetails.devicesInfo, {
              conId: socketID,
            });
            const participant = Sipid_details;
            await this.emitEvent(EVENTNAMES.VIDEO_ANNOUNCEMENT, socketID, {
              meetingdetails: meetingdetails,
              message: ur_ccaas_info.annc_text,
              socketid: socketID,
              isConnected: "queuewaiting"
            });
            await this.queueHandling(eventSeq);
          }
          else if(event_name === "transferee_declined"){
            const { ur_user_info, ur_ccaas_info } = message.ur_conf_message;
            const user_info_message = message

            const { meeting_id} =
              user_info_message.ur_conf_message.ur_conf_meeting_info;

            const meetingdetails: any =
              await this.redisadaptor.getMeetingRedis(meeting_id);

      
              for (var i in meetingdetails.devicesInfo) {

                meetingdetails.devicesInfo[i].video = true;
                meetingdetails.devicesInfo[i].mute = false;
              
            }
            meetingdetails.transferStatus="";
            await this.emitEvent(EVENTNAMES.TRANSFER_INITIATED, meeting_id, {
              meetingdetails: meetingdetails,
              type:"transfer_declined",
              details:{}
            });
            await this.emitEvent(EVENTNAMES.HIDE_ALL, meeting_id, {
                meetingid: meeting_id,
                meetingdetails: meetingdetails,
                video:true,
                audio:false
              });
              this.redisadaptor.setMeetingDetails(meeting_id, meetingdetails);
            await this.queueHandling(eventSeq); 
          }
          else if(event_name ==="transfer_agent_connected"){
            const { ur_user_info, ur_ccaas_info } = message.ur_conf_message;
            const user_info_message = message

            const { meeting_id, meeting_name, host_joined } =
              user_info_message.ur_conf_message.ur_conf_meeting_info;


            const meetingdetails: any =
              await this.redisadaptor.getMeetingRedis(meeting_id);
              for (var i in meetingdetails.devicesInfo) {

                meetingdetails.devicesInfo[i].video = true;
                meetingdetails.devicesInfo[i].mute = false;
              
            }
              console.log("meetingdetails.joinedParticipantlist>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>",meetingdetails.joinedParticipantlist);
              let filterList = _.without(meetingdetails.joinedParticipantlist, _.findWhere(meetingdetails.joinedParticipantlist, {
                deviceid: ur_user_info[0].device_id,
              }));
              meetingdetails.transferStatus="success";
              console.log("without sec agent ===================================================",filterList);
                //2nd agent
                  let joinedList = meetingdetails.joinedParticipantlist || [];

                  console.log("joinedList >>>>.", joinedList);
      
                  const user_info_details = ur_user_info[0]
      
                  //console.log("host_joined >>>>>>>", user_info_details);
      
                  logger.info("<<<<< host_joined >>>>", user_info_details);
                  const socketId = user_info_details?.connection_id;
                  const device_id = user_info_details?.device_id;
                  const sip_id = user_info_details?.sip_id;
      
                  let joinedDetails = {
                    conId: user_info_details?.connection_id,
                    user_id: user_info_details?.user_id,
                    user_type: user_info_details?.user_type,
                    deviceid: device_id
                  }
      
                  let f_arr = _.without(joinedList, _.findWhere(joinedList, {
                    conId: ur_ccaas_info.primary_agent,
                  }));
      
                  f_arr.push(joinedDetails);
      
                  // console.log("f_arr >>>>>>>", f_arr);
                  let devicedetails: any = _.findWhere(meetingdetails.devicesInfo, {
                    deviceid: device_id, sipId: sip_id
                  });
      
                  let join_list = _.uniq(f_arr, 'conId')
                  logger.info("joinedList >>>>>>>", join_list);
                  meetingdetails.joinedParticipantlist = join_list;
                  //console.log("devicedetails >>>>>>>", devicedetails);
                  logger.info("devicedetails >>>>>>>", devicedetails);
      
                  //console.log("meetingdetails.joinedParticipantlist >>>>>", meetingdetails.joinedParticipantlist);
                  for (let partlist = 0; partlist < filterList.length; partlist++) {
                    await this.emitEvent(EVENTNAMES.TRANSFER_INITIATED, filterList[partlist].conId, {
                      meetingdetails: meetingdetails,
                      type:filterList[partlist].user_type === "customer" ? "transfer_connected_continue" : "transfer_connected_end"
                    });
                  }
                    
                  await this.emitEvent(EVENTNAMES.TRANSFER_INITIATED, ur_user_info[0].connection_id, {
                    meetingdetails: meetingdetails,
                    type:"transfer_connected_continue"
                    });
      
                  if (devicedetails) {
                    devicedetails.status = "joined";
                    devicedetails.joinedTime = new Date().getTime();
                    devicedetails.isHostAllowed = true;
                    devicedetails.isReconnectstatus = false;
                    devicedetails.conId = user_info_details?.connection_id;
                    devicedetails.moveToWaitingRoom = false;
                    devicedetails.removeFromMeeting = false;
                    if(meetingdetails.transferStatus ==="success"){  let agentDet:any=meetingdetails.meetingDetails.agentDetails;
      
                      agentDet={...agentDet,ext:meetingdetails.secondaryAgentDetails.ext.toString(),sip_login_id:user_info_details?.sip_id,Email:meetingdetails.secondaryAgentDetails.emailId,username:meetingdetails.secondaryAgentDetails.UserName,role_id:meetingdetails.secondaryAgentDetails.role_id }
                      meetingdetails.meetingDetails.agentDetails=agentDet;
                    }
                  } else {
                    devicedetails = {
                      video: true,
                      mute: false,
                      pined: false,
                      name: user_info_details?.user_id || "",
                      isHost: true,
                      sipId: user_info_details?.sip_id,
                      isAlternateHost: false,
                      isRecordingAllowed: false,
                      moveToWaitingRoom: false,
                      profileImg: "",
                      status: "joined", //"joined" / "inwaitingroom" / "removed";
                      hand: false,
                      ext: "",
                      isGuest: false,
                      roleType: 1,
                      removeFromMeeting: false,
                      email: "",
                      isHostAllowed: true,
                      waiting_retrycount: 0,
                      recordingStatus: "stop",
                      closedCaption: false,
                      isScreenshare: false,
                      isWhiteboard: false,
                      isMirror: true,
                      joinedTime: new Date().getTime(),
                      isVoiceengineStart: false,
                      isReconnectstatus: false,
                      recordFilePath: configuration.videoRecordFileURL,
                      reactions: "",
                      non_verbal: "",
                      cc_lang: "en",
                      speaker_lang: "en"
                    };
                    devicedetails.status = "joined";
                    devicedetails.isHostAllowed = true;
                    devicedetails.isReconnectstatus = false;
                    devicedetails.conId = user_info_details?.connection_id
                    devicedetails.deviceid = user_info_details?.device_id;
                    devicedetails.name = user_info_details?.user_id;
                    if(meetingdetails.isSecondaryAgent){  let agentDet:any=meetingdetails.meetingDetails.agentDetails;
      
                      agentDet={...agentDet,ext:meetingdetails.secondaryAgentDetails.ext.toString(),sip_login_id:user_info_details?.sip_id,Email:meetingdetails.secondaryAgentDetails.emailId,username:meetingdetails.secondaryAgentDetails.UserName,role_id:meetingdetails.secondaryAgentDetails.role_id }
                      meetingdetails.meetingDetails.agentDetails=agentDet;}
                  }
      
      
                  if (devicedetails) {
                    //console.log("Insside participant >>>>>>", devicedetails);
                    /*   devicedetails.status = "joined";
                      devicedetails.joinedTime = new Date().getTime();
                      devicedetails.isHostAllowed = true;
                      devicedetails.isReconnectstatus = false;
                      devicedetails.conId = user_info_details?.connection_id */
      
                    /*  if (
                       meetingdetails.devicesInfo.hasOwnProperty(
                         user_info_details.device_id
                       )
                     ) { */
                    meetingdetails.devicesInfo[user_info_details?.device_id] =
                      devicedetails;
                    //}
      
      
      
                    /*  await this.rocketMQMiddleware.connectionRocket(
                       meetingdetails,
                       [devicedetails],
                       EVENTNAMES.UPDATE_PARTICIPANTLIST,
                       socketId
                     ); */
      
                    let waitingstatus_filter = _.filter(meetingdetails.devicesInfo, {
                      status: "inWaitingroom",
                    });
      
                    /* let joinedstatus_filter = _.filter(meetingdetails.devicesInfo, {
                      status: "joined",
                    });
       */
                    //console.log("joinedstatus_filter Array>>>>>>", joinedstatus_filter);
                    // console.log("before meetingdetails >>>>>>", meetingdetails)
      
      
      
      
                    // for (let jindex = 0; jindex < joinedstatus_filter.length; jindex++) {
                    //const joinelement = joinedstatus_filter[jindex];
      
                    const getData: any = await getSettings(meetingdetails.meetingDetails.agentDetails.domain_id);
                    if (!_.isEmpty(getData)) {
                    console.log('getData agent joined >>>>>>>>>>>>', getData);
                      await this.emitEvent(EVENTNAMES.VM_SETTINGS, socketId, {
                        meetingdetails: meetingdetails,
                        settings: getData,
                        socketid: socketId,
                      });
                    }
      
                    await this.emitEvent(EVENTNAMES.CONNECTION_STATUS, socketId, {
                      meetingdetails: meetingdetails,
                      isConnected: "connected",
                      socketid: socketId,
                    });
      
                    await this.emitEvent(EVENTNAMES.MEETING_STATUS, meeting_id, {
                      message: `${devicedetails.name} Joined the meeting`,
                      socketid: socketId,
                      meetingdetails: meetingdetails,
                    });
      
      
                    if (meetingdetails.joinedParticipantlist.length > 1) {
                      await this.emitEvent(
                        EVENTNAMES.CLIENT_UPDATED_PARTICIPANTLIST,
                        meeting_id,
                        {
                          participantList: meetingdetails.joinedParticipantlist,
                        }
                      );
                    }
      
                    //}
      
                    // console.log("meetingdetails.isHostjoined >>>>>", meetingdetails.isHostjoined);
                    //console.log("meetingdetails.isParticipantjoinbefhost >>>>>", meetingdetails.isParticipantjoinbefhost);
                    if ((!meetingdetails.isHostjoined)) {
      
                      let host_not_joined_filter = _.filter(meetingdetails.devicesInfo, {
                        status: "host_not_joined",
                      });
      
                      console.log("host_not_joined_filter >>>>", host_not_joined_filter);
                      logger.info("host_not_joined_filter >>>>", host_not_joined_filter);
      
                      // console.log("meetingdetailsmeetingdetails",meetingdetails);
                      meetingdetails.isHostjoined = true;
                      await this.emitEvent(EVENTNAMES.HOST_STATUS, meeting_id, {
                        message: "Host joined",
                        meetingdetails: meetingdetails,
                      });
      
                      let that = this;
                      for (let hindex = 0; hindex < host_not_joined_filter.length; hindex++) {
                        (function (hindex) {
                          setTimeout(async function () {
      
      
                            const helement = host_not_joined_filter[hindex];
      
                            helement.status = "waiting room initialized";
      
                            // console.log("helement >>>>>>>", helement);
      
                            if (meetingdetails.devicesInfo.hasOwnProperty(helement.deviceid)) {
                              meetingdetails.devicesInfo[helement.deviceid] = helement;
                            }
                            await that.emitEvent(EVENTNAMES.JOIN_INITIALIZED, helement.conId, {
                              message: "send_invite",
                              user_details: helement,
                              socketid: helement.conId,
                              meetingdetails: meetingdetails,
                            });
      
                            await that.rocketMQMiddleware.connectionRocket(
                              meetingdetails,
                              [helement],
                              EVENTNAMES.PARTICIPANT_JOINIG,
                              helement.conId
                            );
                            await that.redisadaptor.setMeetingDetails(meeting_id, meetingdetails);
      
                          }, 500 * (hindex + 1));
                        })(hindex);
                      }
      
                    }
      
                    meetingdetails.isHostjoined = true;
      
                    // console.log("waitingstatus_filter Array>>>>>>", waitingstatus_filter);
                    //console.log("joinedstatus_filter Array>>>>>>", joinedstatus_filter);
                    for (let sindex = 0; sindex < waitingstatus_filter.length; sindex++) {
                      const element = waitingstatus_filter[sindex];
      
                      //if(!element.mute) 
      
                      //console.log("waitingstatus_filter >>>>>>", element);
      
                      let w_meetingdetails = meetingdetails
                      w_meetingdetails.isHostjoined = true;
                      await this.emitEvent(EVENTNAMES.HOST_STATUS, element.conId, {
                        message: "Host joined",
                        meetingdetails: w_meetingdetails,
                      });
      
                    }
      
                  for(let i=0;i<meetingdetails.joinedParticipantlist.length;i++){await this.emitEvent(EVENTNAMES.HIDE_ALL, meetingdetails.joinedParticipantlist[i].conId, {
                    meetingid: meeting_id,
                    meetingdetails: meetingdetails,
                    video:true,
                    audio:false
                  });}
                  this.redisadaptor.setMeetingDetails(meeting_id, meetingdetails);
            }
          
              
              await this.queueHandling(eventSeq);
          }
          else if (event_name == "participant_moved") {

            const { ur_user_info } = message.ur_conf_message;
            const user_info_message = message

            const { meeting_id, meeting_name, host_joined } =
              user_info_message.ur_conf_message.ur_conf_meeting_info;


            const meetingdetails: any =
              await this.redisadaptor.getMeetingRedis(meeting_id);

            const part_info_details = ur_user_info[0];
            const socketID = part_info_details.connection_id;

            console.log("participant_moved >>>>>>>>>", part_info_details);

            logger.info("<<<<< participant_moved >>>>", part_info_details);
            let Sipid_details: any = _.findWhere(meetingdetails.devicesInfo, {
              conId: socketID,
            });
            const participant = Sipid_details; //meetingdetails.devices_info[req.sipId];
            //  let participant_Keys = Object.keys(participant)

            if (participant) {
              /* let joined_connection: any = meetingdetails.joinedConn || [];
              let waiting_connection: any = meetingdetails.waitingroomConn || [];
      
      
              joined_connection = joined_connection.filter((item: any) => item !== socketID)
      
              waiting_connection.push(socketID);
              meetingdetails.joinedConn = joined_connection;
              meetingdetails.waitingroomConn = waiting_connection; */

              let deviceid = participant.deviceid;

              participant.status = "inWaitingroom";

              participant.moveToWaitingRoom = true;
              //meetingdetails.joined_participants[req.sipId] = participant;

              if (meetingdetails.devicesInfo.hasOwnProperty(deviceid)) {
                meetingdetails.devicesInfo[deviceid] = participant;
              }

              // this.redisadaptor.setMeetingDetails(meeting_id, meetingdetails);

              await this.emitEvent(
                EVENTNAMES.MOVE_TO_WAITING_ROOM,
                meeting_id,
                {
                  meetingdetails: meetingdetails,
                  message: "Moved to waiting room",
                  socketid: socketID,
                }
              );
              await this.emitEvent(EVENTNAMES.CONNECTION_STATUS, socketID, {
                meetingdetails: meetingdetails,
                isConnected: "waitingroom",
                socketid: socketID,
              });
            } else {
              //Need to handle
            }
          } else if (event_name == "participant_removed") {



            const { ur_user_info } = message.ur_conf_message;
            logger.info(
              "<<<<< participant_removed section calling >>>>",
              ur_user_info
            );
            const user_error_info_message = message



            const { meeting_id, meeting_name, host_joined } =
              user_error_info_message.ur_conf_message.ur_conf_meeting_info;




            const meetingdetails: any =
              await this.redisadaptor.getMeetingRedis(meeting_id);

            const remove_info_details = ur_user_info[0];
            console.log("remove_info_details >>>>>>>", remove_info_details);

            logger.info(
              "<<<<< remove_info_details >>>>",
              remove_info_details
            );

            const socketId = remove_info_details.connection_id;

            const device_id = remove_info_details?.device_id;
            const sip_id = remove_info_details?.sip_id;
            let Sipid_details: any = {}

            if (device_id) {
              Sipid_details = _.findWhere(meetingdetails.devicesInfo, {
                deviceid: device_id, sipId: sip_id
              });
            } else {
              Sipid_details = _.findWhere(meetingdetails.devicesInfo, {
                conId: socketId
              });
            }



            console.log("participant_removed >>>>>>>>>", remove_info_details);

            logger.info("<<<<< participant_removed >>>>", remove_info_details);

            const participant = Sipid_details || {};

            console.log("participant >>>>>>>", participant);

            if (Object.keys(participant).length > 0) {


              console.log("Inside remove participant >>>>>>>", participant);

              let filterList = _.without(meetingdetails.joinedParticipantlist, _.findWhere(meetingdetails.joinedParticipantlist, {
                deviceid: device_id
              }));

              console.log("removed filterList >>>>", filterList);
              meetingdetails.joinedParticipantlist = filterList;
              await this.redisadaptor.setMeetingDetails(meeting_id, meetingdetails);

              /*  await this.rocketMQMiddleware.connectionRocket(
                 meetingdetails,
                 [participant],
                 EVENTNAMES.UPDATE_PARTICIPANTLIST,
                 participant.conId
               ); */

              logger.info("<<<<< participant_removed updated participant list>>>>", remove_info_details);
              await this.emitEvent(
                EVENTNAMES.CLIENT_UPDATED_PARTICIPANTLIST,
                meeting_id,
                {
                  participantList: meetingdetails.joinedParticipantlist,
                }
              );

            }

            await this.queueHandling(eventSeq);

            /*             let socketID = ur_user_info[0].connection_id;
                        let Sipid_details: any = _.findWhere(meetingdetails.devicesInfo, { conId: socketID });
                        const participant = Sipid_details; //meetingdetails.participants[m.sipId];
            
                        if (participant) {
            
                          let deviceid = participant.deviceid
            
                          participant.status = "removed";
                          participant.moveToWaitingRoom = false;
                          participant.removeFromMeeting = true;
                          let Name = participant.name
            
                         
            
            
                          if (meetingdetails.devicesInfo.hasOwnProperty(deviceid)) {
                            meetingdetails.devicesInfo[deviceid] = participant;
                          }
                          this.redisadaptor.setMeetingDetails(meeting_id, meetingdetails);
            
                          await this.emitEvent(EVENTNAMES.REMOVE_FROM_MEETING, meeting_id, {
                            meetingdetails: meetingdetails,
                            message: `${Name} removed from the meeting`,
                            removed_socketId: socketID,
                          });
            
            
            
                        } else {
            
                          //need to handle
                        } */
          } else if (event_name == "user_disconnected") {
            const { ur_user_info } = message.ur_conf_message;
            logger.info("<<<<< user_disconnected >>>>", ur_user_info[0]);
            await this.queueHandling(eventSeq);

            /*             let socketId = ur_user_info[0].connection_id;
                        let connectionDetails: any = _.findWhere(meetingdetails.devicesInfo, { conId: socketId });
            
            
                        if (connectionDetails) {
            
                          const adapter: RedisAdapter = this.io.of("/").adapter as RedisAdapter;
                          connectionDetails.status = "left";
                          meetingdetails.devicesInfo[connectionDetails.deviceid] = connectionDetails;
                          //this.redisadaptor.setMeetingDetails(meeting_id, meetingdetails);
            
            
                          await this.emitEvent(EVENTNAMES.MEETING_STATUS, meeting_id,
                            { message: `${connectionDetails.name} Left the meeting`, socketid: socketId, meetingdetails: meetingdetails });
            
                        } */
          } else if (event_name == "end_meeting_all_done") {

            const { ur_user_info } = message.ur_conf_message;
            await this.queueHandling(eventSeq);
            // console.log("event_name >>>>>>>>>", meeting_id);

            logger.info("<<<<< end_meeting_all_done >>>>", ur_user_info[0]);
          } else if (event_name == "user_terminated") {
            const { ur_user_info } = message.ur_conf_message;
            logger.info(" <<<<<< user_terminated section >>>>>>>>", ur_user_info)
            console.log("user_terminated calling>>>>>>>>>");
            await this.queueHandling(eventSeq);
          } else if (event_name == "user_joining_status_error") {


            const { ur_user_info } = message.ur_conf_message;
            logger.info("user_joining_status_error ", ur_user_info);
            const user_error_info_message = message

            const { meeting_id, meeting_name, host_joined } =
              user_error_info_message.ur_conf_message.ur_conf_meeting_info;

            const meetingdetails: any =
              await this.redisadaptor.getMeetingRedis(meeting_id);

            console.log("meetingdetails >>>>>>>>", meetingdetails);

            const user_error_info_details = ur_user_info[0];
            console.log("user_joining_status_error >>>>>>>", user_error_info_details);

            logger.info(
              "<<<<< user_joining_status_error >>>>",
              user_error_info_details
            );

            const socketId = user_error_info_details.connection_id;

            const device_id = user_error_info_details?.device_id;
            const sip_id = user_error_info_details?.sip_id;
            let Sipid_details: any = {}

            if (device_id) {
              Sipid_details = _.findWhere(meetingdetails.devicesInfo, {
                deviceid: device_id, sipId: sip_id
              });
            } else {
              Sipid_details = _.findWhere(meetingdetails.devicesInfo, {
                conId: socketId
              });
            }

            await this.emitEvent(EVENTNAMES.SERVER_FAILURE, socketId, {
              message: "Something went wrong, please try again later.",
              socketid: socketId,
            });



            logger.info(
              "<<<<<  user_joining_status error_details >>>>",
              user_error_info_details
            );
            const participant = Sipid_details || {}; //meetingdetails.devices_info[req.sipId];
            const rec_data = Sipid_details || {};

            console.log("user_joining_status devicedetails >>>>>>>", participant);
            logger.info("user_joining_status devicedetails >>>>>>>", participant);

            if (Object.keys(participant).length > 0) {


              console.log("user_joining_status inside device details >>>>>>>", participant);
              logger.info("user_joining_status inside device details >>>>>>>", participant);
              await this.rocketMQMiddleware.connectionRocket(
                meetingdetails,
                [participant],
                EVENTNAMES.USER_TERMINATED,
                socketId
              );

              let filterList = _.without(meetingdetails.joinedParticipantlist, _.findWhere(meetingdetails.joinedParticipantlist, {
                deviceid: device_id
              }));
              meetingdetails.joinedParticipantlist = filterList;
              await this.redisadaptor.setMeetingDetails(meeting_id, meetingdetails);

              /*  await this.rocketMQMiddleware.connectionRocket(
                 meetingdetails,
                 [participant],
                 EVENTNAMES.UPDATE_PARTICIPANTLIST,
                 participant.conId
               );
  */
              let deviceid = participant.deviceid;

              participant.status = "umn_server_failure";
              participant.reactions = "";
              participant.cc_lang = "en";
              participant.speaker_lang = "en";
              participant.recordingStatus = "stop";
              if (meetingdetails.devicesInfo.hasOwnProperty(deviceid)) {
                meetingdetails.devicesInfo[deviceid] = participant;
              }
              this.redisadaptor.setMeetingDetails(meeting_id, meetingdetails);
              await this.emitEvent(EVENTNAMES.RECORDING_ACTION, meeting_id, {
                message: `${rec_data.name} started the recording.`,
                meetingdetails: meetingdetails,
                participantdetails: participant,
              });
              await this.emitEvent(EVENTNAMES.MEETING_STATUS, meeting_id, {
                message: `${Sipid_details?.name} left the meeting`,
                socketid: socketId,
                meetingdetails: meetingdetails,
              });
              let data: any = {

                connection_Id: participant.conId,
                meeting_id: meeting_id,
                cc_lang: "en",
                cc_state: participant.closedCaption,
                speaker_name: participant.name,
                is_host: participant.isHost,
                device_id: participant.deviceid,
                image_url: participant.profileImg,
                join_status: "left",
                speaker_lang: "en",
                status: 4
              }
              await this.videoccpublisher.videoccpublish("subchannel", JSON.stringify(data));
              await this.emitEvent(
                EVENTNAMES.CLIENT_UPDATED_PARTICIPANTLIST,
                meeting_id,
                {
                  participantList: meetingdetails.joinedParticipantlist,
                }
              );
            }

            await this.queueHandling(eventSeq);

          } else if (event_name == "dtls_connection_failure_error") {

            const { ur_user_info } = message.ur_conf_message;
            console.log("dtls_connection_failure_error >>>>>>>", ur_user_info[0]);
            logger.info("dtls_connection_failure_error >>>>>>>", ur_user_info[0])

            const dtls_error_info_message = message

            const { meeting_id, meeting_name, host_joined } =
              dtls_error_info_message.ur_conf_message.ur_conf_meeting_info;


            const meetingdetails: any =
              await this.redisadaptor.getMeetingRedis(meeting_id);

            const dtls_error_info_details = ur_user_info[0];
            logger.info(
              "<<<<< dtls_connection_failure_error user details>>>>",
              dtls_error_info_details
            );

            const socketId = dtls_error_info_details.connection_id;
            const device_id = dtls_error_info_details.device_id;
            const sip_id = dtls_error_info_details.sip_id;
            let Sipid_details: any = {}

            if (device_id) {
              Sipid_details = _.findWhere(meetingdetails.devicesInfo, {
                deviceid: device_id, sipId: sip_id
              });
            } else {
              Sipid_details = _.findWhere(meetingdetails.devicesInfo, {
                conId: socketId
              });
            }


            await this.emitEvent(EVENTNAMES.CONNECTION_FAILURE, socketId, {
              message: "Something went wrong, please try again later.",
              socketid: socketId,
            });

            logger.info(
              "<<<<<  dtls_connection_failure_error error_details >>>>",
              dtls_error_info_details
            );

            logger.info(
              "<<<<<  dtls_connection_failure_error Sipid_detailss >>>>",
              Sipid_details
            );
            const participant = Sipid_details || {}; //meetingdetails.devices_info[req.sipId];

            console.log("dtls_connection_failure_error devicedetails >>>>>>>", participant);
            logger.info("dtls_connection_failure_error devicedetails >>>>>>>", participant);

            let filterList = _.without(meetingdetails.joinedParticipantlist, _.findWhere(meetingdetails.joinedParticipantlist, {
              deviceid: device_id
            }));
            meetingdetails.joinedParticipantlist = filterList;

            logger.info("dtls_connection_failure_error meetingdetails.joinedParticipantlist >>>>>>>", meetingdetails.joinedParticipantlist);
            this.redisadaptor.setMeetingDetails(meeting_id, meetingdetails);

            /*  await this.rocketMQMiddleware.connectionRocket(
               meetingdetails,
               [participant],
               EVENTNAMES.UPDATE_PARTICIPANTLIST,
               participant.conId
             ); */

            if (Object.keys(participant).length > 0) {

              console.log("dtls inside device details >>>>>>>", participant);
              logger.info("dtls_connection_failure_error inside device details >>>>>>>", participant);
              /*  await this.rocketMQMiddleware.connectionRocket(
                 meetingdetails,
                 [participant],
                 EVENTNAMES.USER_TERMINATED,
                 socketId
               ); */

              await this.emitEvent(
                EVENTNAMES.CLIENT_UPDATED_PARTICIPANTLIST,
                meeting_id,
                {
                  participantList: meetingdetails.joinedParticipantlist,
                }
              );
              const deviceid = participant.deviceid;

              participant.status = "dtls_connection_failure";
              participant.reactions = "";

              if (meetingdetails.devicesInfo.hasOwnProperty(deviceid)) {
                meetingdetails.devicesInfo[deviceid] = participant;
              }
              this.redisadaptor.setMeetingDetails(meeting_id, meetingdetails);

              /*  await this.emitEvent(EVENTNAMES.MEETING_STATUS, meeting_id, {
                 message: `${Sipid_details?.name} left the meeting`,
                 socketid: socketId,
                 meetingdetails: meetingdetails,
               }); */

              await this.queueHandling(eventSeq);
            } else {
              await this.queueHandling(eventSeq);
            }

          } else if (event_name == "dtls_reconnection_success") {

            const { ur_user_info } = message.ur_conf_message;

            const part_info_message = message

            const { meeting_id, meeting_name, host_joined } =
              part_info_message.ur_conf_message.ur_conf_meeting_info;


            const meetingdetails: any =
              await this.redisadaptor.getMeetingRedis(meeting_id);

            const part_info_details = ur_user_info[0];
            console.log("dtls_reconnection_success >>>>>>>>>", part_info_details);

            logger.info("<<<<< dtls_reconnection_success >>>>", part_info_details);

            const socketId = part_info_details.connection_id;
            const device_id = part_info_details?.device_id;
            const sip_id = part_info_details?.sip_id;


            let Sipid_details: any = {}

            if (device_id) {
              Sipid_details = _.findWhere(meetingdetails.devicesInfo, {
                deviceid: device_id, sipId: sip_id
              });
            } else {
              Sipid_details = _.findWhere(meetingdetails.devicesInfo, {
                conId: socketId
              });
            }

            const participant = Sipid_details || {};
            console.log("devicedetails >>>>>>>", participant);
            logger.info("devicedetails >>>>>>>", participant);
            if (Object.keys(participant).length > 0) {

              let Name = participant.name;
              let deviceid = participant.deviceid;


              participant.status = "joined";
              participant.moveToWaitingRoom = false;
              participant.removeFromMeeting = false;
              participant.isHostAllowed = true;

              if (meetingdetails.devicesInfo.hasOwnProperty(deviceid)) {
                meetingdetails.devicesInfo[deviceid] = participant;
              }

              this.redisadaptor.setMeetingDetails(meeting_id, meetingdetails);

              console.log("meetign _id", meeting_id);

              /*  await this.emitEvent(EVENTNAMES.JOINMEETING, meeting_id, {
                 message: `${Name} Joined the meeting`,
                 socketid: socketId,
                 meetingdetails: meetingdetails,
               }); */

              await this.emitEvent(EVENTNAMES.MEETING_STATUS, meeting_id, {
                message: `${participant?.name} Joined the meeting`,
                socketid: socketId,
                meetingdetails: meetingdetails,
              });

              await this.queueHandling(eventSeq);

            } else {
              await this.queueHandling(eventSeq);
            }
          } else if (event_name == EVENTNAMES.SCREENSHARE_STARTED) {

            const { ur_user_info } = message.ur_conf_message;

            const part_info_message = message

            const { meeting_id, meeting_name, host_joined } =
              part_info_message.ur_conf_message.ur_conf_meeting_info;


            const meetingdetails: any =
              await this.redisadaptor.getMeetingRedis(meeting_id);

            const part_info_details = ur_user_info[0];
            const device_id = part_info_details?.device_id;

            let joinedList = meetingdetails.joinedParticipantlist || [];
            const user_info_details = ur_user_info[0]
            let joinedDetails = {
              conId: user_info_details?.connection_id,
              user_id: user_info_details?.user_id,
              user_type: user_info_details?.user_type,
              deviceid: device_id
            }

            joinedList.push(joinedDetails);

            meetingdetails.joinedParticipantlist = joinedList;
            console.log("<<<<<<< SCREENSHARE_STARTED >>>>>>>>>", part_info_details);

            logger.info("<<<<< SCREENSHARE_STARTED >>>>", part_info_details);

            const socketId = part_info_details.connection_id;


            const sip_id = part_info_details?.sip_id;
            const participant: any = _.findWhere(meetingdetails.devicesInfo, {
              deviceid: device_id, sipId: sip_id
            });


            /*  const origin_deviceId = device_id?.split("_screenshare")[0] */

            /*   const originparticipant: any = _.findWhere(meetingdetails.devicesInfo, {
                deviceid: origin_deviceId, sipId:sip_id?.split("_screenshare")[0]
              }); */
            console.log("SCREENSHARE_STARTED devicedetails >>>>>>>", participant);
            logger.info("SCREENSHARE_STARTED devicedetails >>>>>>>", participant);
            if (participant) {

              logger.info("INSIDE SCREENSHARE_STARTED devicedetails >>>>>>>", participant);
              let Name = participant.name;
              let deviceid = participant.deviceid;


              participant.status = "joined";
              participant.moveToWaitingRoom = false;
              participant.removeFromMeeting = false;
              participant.isHostAllowed = true;
              participant.isScreenshare = true;

              if (meetingdetails.devicesInfo.hasOwnProperty(deviceid)) {
                meetingdetails.devicesInfo[deviceid] = participant;
              }



              meetingdetails.isScreenshareStarted = true;

              this.redisadaptor.setMeetingDetails(meeting_id, meetingdetails);

              await this.emitEvent(EVENTNAMES.SCREEN_SHARE, meeting_id, {
                message: true,
                socketid: socketId,
                meetingdetails: meetingdetails,
              });

              /*  await this.rocketMQMiddleware.connectionRocket(
                 meetingdetails,
                 [participant],
                 EVENTNAMES.UPDATE_PARTICIPANTLIST,
                 socketId
               ); */

              await this.emitEvent(
                EVENTNAMES.CLIENT_UPDATED_PARTICIPANTLIST,
                meeting_id,
                {
                  participantList: meetingdetails.joinedParticipantlist,
                }
              );
              await this.queueHandling(eventSeq);
            } else {
              await this.queueHandling(eventSeq);
            }

          } else if (event_name == EVENTNAMES.SCRENNSHARE_STOPPED) {

            console.log("<<<< SCRENNSHARE_STOPPED devicedetails >>>>>>>", message.ur_conf_message);
            logger.info("<<<< SCRENNSHARE_STOPPED devicedetails >>>>>>>", message.ur_conf_message);
            /*  const { ur_user_info } = message.ur_conf_message;
         
             const part_info_message = message
         
             const { meeting_id, meeting_name, host_joined } =
             part_info_message.ur_conf_message.ur_conf_meeting_info;
         
         
             const meetingdetails: any =
             await this.redisadaptor.getMeetingInfofromRedis(meeting_id);
         
             const part_info_details = ur_user_info[0];
             console.log("<<<<< SCRENNSHARE_STOPPED >>>>>>>>>", part_info_details);
         
             logger.info("<<<<< SCRENNSHARE_STOPPED >>>>", part_info_details);
         
             const socketId = part_info_details.connection_id;
         
             const device_id = part_info_details?.device_id;
             const sip_id = part_info_details?.sip_id;
             const participant: any = _.findWhere(meetingdetails.devicesInfo, {
               deviceid: device_id, sipId:sip_id
             });
         
         
         
             console.log("<<<< SCRENNSHARE_STOPPED devicedetails >>>>>>>", participant);
             logger.info("<<<< SCRENNSHARE_STOPPED devicedetails >>>>>>>", participant);
             if (participant) {
               
               let Name = participant.name;
               let deviceid = participant.deviceid;
              
         
               participant.status = "disconnected";
               participant.moveToWaitingRoom = false;
               participant.removeFromMeeting = false;
               participant.isHostAllowed = true;
               participant.isScreenshare = false;
               
               if (meetingdetails.devicesInfo.hasOwnProperty(deviceid)) {
                 meetingdetails.devicesInfo[deviceid] = participant;
               }
         
               meetingdetails.isScreenshareStarted = false;
         
               this.redisadaptor.setMeetingDetails(meeting_id, meetingdetails);
         
               await this.emitEvent(EVENTNAMES.SCREEN_SHARE, meeting_id, {
                 message: false,
                 socketid: socketId,
                 meetingdetails: meetingdetails,
               });
         
             }
         
         
         
         
             */
            await this.queueHandling(eventSeq);

          } else if (event_name == EVENTNAMES.SCRENNSHARE_DTLS_ERROR) {

            const { ur_user_info } = message.ur_conf_message;
            console.log("<<<<< SCRENNSHARE_DTLS_ERROR >>>>>>>", ur_user_info[0]);

            const dtls_error_info_message = message

            const { meeting_id, meeting_name, host_joined } =
              dtls_error_info_message.ur_conf_message.ur_conf_meeting_info;


            const meetingdetails: any =
              await this.redisadaptor.getMeetingRedis(meeting_id);

            const dtls_error_info_details = ur_user_info[0];
            logger.info(
              "<<<<< SCRENNSHARE_DTLS_ERROR >>>>",
              dtls_error_info_details
            );

            const socketId = dtls_error_info_details.connection_id;
            const originSocketid = socketId?.split("_screenshare");
            const device_id = dtls_error_info_details.device_id;
            const sip_id = dtls_error_info_details.sip_id;
            let Sipid_details: any = {}

            if (device_id) {
              Sipid_details = _.findWhere(meetingdetails.devicesInfo, {
                deviceid: device_id, sipId: sip_id
              });
            } else {
              Sipid_details = _.findWhere(meetingdetails.devicesInfo, {
                conId: socketId
              });
            }


            await this.emitEvent(EVENTNAMES.SCRENNSHARE_DTLS_ERROR, originSocketid, {
              message: "Something went wrong, please try again later.",
              socketid: originSocketid,
            });

            logger.info(
              "<<<<<  dtls_connection_failure_error error_details >>>>",
              dtls_error_info_details
            );

            logger.info(
              "<<<<<  dtls_connection_failure_error Sipid_detailss >>>>",
              Sipid_details
            );
            const participant = Sipid_details || {}; //meetingdetails.devices_info[req.sipId];

            console.log("<<<<< SCRENNSHARE_DTLS_ERROR devicedetails >>>>>>>", participant);
            logger.info("<<<<<< SCRENNSHARE_DTLS_ERROR devicedetails >>>>>>>", participant);

            if (Object.keys(participant).length > 0) {

              console.log("<<<<<<  SCRENNSHARE_DTLS_ERROR inside device details >>>>>>>", participant);
              logger.info("<<<<<< SCRENNSHARE_DTLS_ERROR inside device details >>>>>>>", participant);


              const deviceid = participant.deviceid;

              participant.status = "dtls_connection_failure";

              if (meetingdetails.devicesInfo.hasOwnProperty(deviceid)) {
                meetingdetails.devicesInfo[deviceid] = participant;
              }
              this.redisadaptor.setMeetingDetails(meeting_id, meetingdetails);

              /*  await this.emitEvent(EVENTNAMES.MEETING_STATUS, meeting_id, {
                 message: `${Sipid_details?.name} left the meeting`,
                 socketid: socketId,
                 meetingdetails: meetingdetails,
               }); */
              await this.queueHandling(eventSeq);
            } else {
              await this.queueHandling(eventSeq);
            }

          } else if (event_name == EVENTNAMES.SCREENSHARE_USERJOINING_ERROR) {

            const { ur_user_info } = message.ur_conf_message;
            const user_error_info_message = message

            const { meeting_id, meeting_name, host_joined } =
              user_error_info_message.ur_conf_message.ur_conf_meeting_info;




            const meetingdetails: any =
              await this.redisadaptor.getMeetingRedis(meeting_id);

            const user_error_info_details = ur_user_info[0];
            console.log("<<<<<< SCREENSHARE_USERJOINING_ERROR >>>>>>>", user_error_info_details);

            logger.info(
              "<<<<< SCREENSHARE_USERJOINING_ERROR >>>>",
              user_error_info_details
            );

            const socketId = user_error_info_details.connection_id;
            const originSocketid = socketId?.split("_screenshare");
            const device_id = user_error_info_details?.device_id;
            const sip_id = user_error_info_details?.sip_id;
            let Sipid_details: any = {}

            if (device_id) {
              Sipid_details = _.findWhere(meetingdetails.devicesInfo, {
                deviceid: device_id, sipId: sip_id
              });
            } else {
              Sipid_details = _.findWhere(meetingdetails.devicesInfo, {
                conId: socketId
              });
            }

            await this.emitEvent(EVENTNAMES.SCREENSHARE_USERJOINING_ERROR, originSocketid, {
              message: "Something went wrong, please try again later.",
              socketid: originSocketid,
            });



            logger.info(
              "<<<<<  user_joining_status error_details >>>>",
              user_error_info_details
            );
            const participant = Sipid_details || {}; //meetingdetails.devices_info[req.sipId];


            console.log("devicedetails >>>>>>>", participant);
            logger.info("devicedetails >>>>>>>", participant);

            if (Object.keys(participant).length > 0) {


              console.log("inside device details >>>>>>>", participant);
              logger.info("inside device details >>>>>>>", participant);
              await this.rocketMQMiddleware.connectionRocket(
                meetingdetails,
                [participant],
                EVENTNAMES.SCREENSHARE_STOP,
                socketId
              );

              let deviceid = participant.deviceid;

              participant.status = "umn_server_failure";

              participant.isScreenshare = false;
              meetingdetails.isScreenshareStarted = false;


              if (meetingdetails.devicesInfo.hasOwnProperty(deviceid)) {
                meetingdetails.devicesInfo[deviceid] = participant;
              }
              this.redisadaptor.setMeetingDetails(meeting_id, meetingdetails);

              await this.emitEvent(EVENTNAMES.SCREEN_SHARE, meeting_id, {
                message: false,
                socketid: socketId,
                meetingdetails: meetingdetails,
              });

              await this.queueHandling(eventSeq);
            } else {
              await this.queueHandling(eventSeq);
            }

          } else if (event_name == EVENTNAMES.SCRENNSHARE_DTLS_SUCCESS) {

            const { ur_user_info } = message.ur_conf_message;

            const part_info_message = message

            const { meeting_id, meeting_name, host_joined } =
              part_info_message.ur_conf_message.ur_conf_meeting_info;


            const meetingdetails: any =
              await this.redisadaptor.getMeetingRedis(meeting_id);

            const part_info_details = ur_user_info[0];
            console.log("<<<<< SCRENNSHARE_DTLS_SUCCESS >>>>>>>>>", part_info_details);

            logger.info("<<<<< SCRENNSHARE_DTLS_SUCCESS >>>>", part_info_details);

            const socketId = part_info_details.connection_id;
            const device_id = part_info_details?.device_id;
            const sip_id = part_info_details?.sip_id;


            let Sipid_details: any = {}

            if (device_id) {
              Sipid_details = _.findWhere(meetingdetails.devicesInfo, {
                deviceid: device_id, sipId: sip_id
              });
            } else {
              Sipid_details = _.findWhere(meetingdetails.devicesInfo, {
                conId: socketId
              });
            }

            const participant = Sipid_details || {};
            console.log("<<<<< SCRENNSHARE_DTLS_SUCCESS devicedetails >>>>>>>", participant);
            logger.info("<<<<< SCRENNSHARE_DTLS_SUCCESS devicedetails >>>>>>>", participant);
            if (Object.keys(participant).length > 0) {

              let Name = participant.name;
              let deviceid = participant.deviceid;


              participant.status = "joined";
              participant.moveToWaitingRoom = false;
              participant.removeFromMeeting = false;
              participant.isHostAllowed = true;

              if (meetingdetails.devicesInfo.hasOwnProperty(deviceid)) {
                meetingdetails.devicesInfo[deviceid] = participant;
              }

              this.redisadaptor.setMeetingDetails(meeting_id, meetingdetails);


              await this.emitEvent(EVENTNAMES.SCRENNSHARE_DTLS_SUCCESS, meeting_id, {
                message: "start",
                socketid: socketId,
                meetingdetails: meetingdetails,
              });

              await this.queueHandling(eventSeq);

            } else {
              await this.queueHandling(eventSeq);
            }
          } else if (event_name == EVENTNAMES.UPDATE_PARTICIPANTLIST_SUCCESS) {

            console.log("Updated participant calling >>>>>")
            const { ur_user_info } = message.ur_conf_message;

            const part_info_message = message

            const { meeting_id, meeting_name, host_joined } =
              part_info_message.ur_conf_message.ur_conf_meeting_info;

            const meetingdetails: any =
              await this.redisadaptor.getMeetingRedis(meeting_id);

            let joinedList = meetingdetails.joinedParticipantlist || [];

            console.log("joinedList participant calling >>>>>", joinedList)
            await this.emitEvent(
              EVENTNAMES.CLIENT_UPDATED_PARTICIPANTLIST,
              meeting_id,
              {
                participantList: joinedList,
              }
            );

            await this.queueHandling(eventSeq);
          } else if (event_name === EVENTNAMES.SDP_UPDATE_RESPONSE) {



            const { ur_user_info } = message.ur_conf_message;

            logger.info("<<<<<< SDP_UPDATE_RESPONSE >>>>>>", ur_user_info)

            const part_info_message = message

            const { meeting_id, meeting_name, host_joined } =
              part_info_message.ur_conf_message.ur_conf_meeting_info;


            const sdp_info_details = ur_user_info[0];
            const socketId = sdp_info_details.connection_id;

            const { sdp_str } = part_info_message.ur_conf_message.ur_media_info;

            const meetingdetails: any =
              await this.redisadaptor.getMeetingRedis(meeting_id);

            console.log("SDP sent to client callig", socketId)
            logger.info("<<<<<< SDP_UPDATE_RESPONSE  sent to client callig>>>>>>", ur_user_info)
            await this.emitEvent(
              EVENTNAMES.SDP_SENT_TO_CLIENT,
              socketId,
              {
                sdp: sdp_str,
              }
            );
            await this.queueHandling(eventSeq);
          } else if (event_name === EVENTNAMES.TILE_CHANGE_RESPONSE) {
            const { ur_user_info, ur_participant_list } = message.ur_conf_message;
            const part_info_message = message;
            const { meeting_id, meeting_name, host_joined } =
              part_info_message.ur_conf_message.ur_conf_meeting_info;

            await this.emitEvent(
              EVENTNAMES.TILE_CHANGE_RESPONSE,
              meeting_id,
              {
                participantList: ur_participant_list,
              }
            );

            await this.queueHandling(eventSeq);
          } else {
            await this.queueHandling(eventSeq);
            //this.isRedisResponseProcessing = true
          }

        }
      } catch (e) {
        console.log("rocketmq error", e);

        logger.info("<<<<< ERROR: Receiving RocketMQ Message>>>>", e);
      }
    }
  };

  // broadcastTranscript = async (meetInfo: any, data: any) => {
  //   try {
  //     let { meetingId, jwtToken, sipLoginId, socketId, deviceid } = meetInfo;
  //     //console.log("broadcastTranscript >>>>>>", data);

  //     //logger.info("<<<<< broadcastTranscript >>>>", data);

  //     let meetingdetails: any = await this.redisadaptor.getMeetingInfofromRedis(
  //       meetingId
  //     );

  //     let closeCaptionUser = _.filter(meetingdetails.devicesInfo, {
  //       closedCaption: true,
  //     });

  //     let speakerDetails: any = _.findWhere(meetingdetails.devicesInfo, {
  //       conId: socketId,
  //     });

  //     data.speakerName = speakerDetails && speakerDetails.name;

  //     // console.log("closeCaptionUser", closeCaptionUser);

  //     //logger.info("<<<<< closeCaptionUser >>>>", closeCaptionUser);

  //     for (let index = 0; index < closeCaptionUser.length; index++) {
  //       const element = closeCaptionUser[index];

  //       //if(!element.mute) 
  //       await this.emitEvent(EVENTNAMES.CLOSE_CAPTION, element.conId, data);

  //     }
  //   } catch (error) {
  //     logger.info("<<<<< ERROR: broadcastTranscript >>>>", error);
  //   }
  // };

  reconizationStarted = async (meetInfo: any, data: any) => {
    try {
      let { meetingId, jwtToken, sipLoginId, socketId, deviceid } = meetInfo;

      console.log("reconizationStarted >>>>", meetInfo);

      logger.info("<<<<< reconizationStarted >>>>");

      await this.emitEvent(
        EVENTNAMES.CLOSE_CAPTION_STARTED,
        socketId,
        "CloseCaptionStarted"
      );
    } catch (error) {
      logger.info("<<<<< ERROR : reconizationStarted >>>>", error);
    }
  };



  endOfTranscriptSucces = async (meetInfo: any) => {
    try {
      let { meetingId, jwtToken, sipLoginId, socketId, deviceid } = meetInfo;

      console.log("endOfTranscriptSucces", meetInfo);

      await this.emitEvent(
        EVENTNAMES.CLOSE_CAPTION_ENDED,
        socketId,
        "ClosecaptionEnded"
      );
    } catch (error) {
      logger.info("<<<<< ERROR : endOfTranscriptSucces >>>>", error);
    }
  };

  private async sleep(ms: any) {
    return new Promise(
      resolve => setTimeout(resolve, ms)
    );
  }
}


