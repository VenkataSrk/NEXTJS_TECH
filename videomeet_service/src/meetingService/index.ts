/**
 * @createdBy Selvakani
 * @createdOn 27th April 2021
 */


import axios from "axios";
import _ from "underscore";
import { configuration } from "../config";
import { participants, setting, participant } from "../model";
import * as jwt from "jsonwebtoken";
const { Translate } = require('@google-cloud/translate').v2;
import { ENDPOINTS, baseURL, baseURLChat } from "../helpers/constants";
import { logger } from "../log";
import { meetingFindOne_model } from "../dao/meetingdetails";

export class MeetingService {
  public host: string = configuration.meeting.host;
  public port: number = configuration.meeting.port;

  public static async getMeetingbyId(
    meetingId: any,
    jwtToken: any
  ): Promise<setting | boolean> {
    let settings: setting = {
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
      guestDetails:{}
    };

    let res: any = {};
    try {
      const dataArr: any = {};
      dataArr["headers"] = { Authorization: jwtToken };
      let decoded: any = {};

      // decoded = jwt.verify(jwtToken, configuration.jwt_secret);

      const url = `${baseURL}${ENDPOINTS.getMeetingbyId}/${meetingId}`;


      //"https://urchat.unifiedring.co.uk:5003/api/get_meeting/" + meetingId;



      /*   res = await axios.get(url, dataArr).catch((error: any) => {
  
          logger.info("<<<<< ERROR : getMeetingbyId >>>>", error);
          //console.log("get Meeting by id error", error)
          throw error;
        }); */

      res = await meetingFindOne_model({ uuid: meetingId })

      if (Object.keys(res.result).length === 0 || res?.statusCode !== 200) {
        return false;
      }
      const data = res;
      if (data.statusCode === 404) {
        return false;
      }
    
      console.log("result >>>>>>>>>>>>", res.result)
      let result = res.result;

      /*       settings.participants[result.sip_id] = {
              Video: true,
              Mute: true,
              Pined: false,
              Name: "",
              Ishost: true,
              sipId: result.sip_id,
              isAlternatehost: false,
              IsRecordingAllowed: false,
              moveTowaitingroom: result.is_participant_waiting_room,
              profile_img: "",
              status: "invited", //"joined" / "inwaitingroom" / "removed";
              hand: false,
              ext: "",
              Remove_from_Meeting: false,
            }; */
      settings.waitingRoom = result.is_participant_waiting_room;
      settings.meetingPwd = result.password;
      settings.generalSetting.isWaitingroom = result.is_participant_waiting_room;
      settings.generalSetting.is_participant_mute_entry = result.is_participant_mute_entry;
      settings.hostDisconnect = true//result.is_without_host_continue;
      settings.isParticipantVideo = result.is_participant_video;
      settings.isHostMyVideo = result.is_host_myvideo;
      settings.isParticipantjoinbefhost = result.is_participant_join_bef_host || false;
      settings.is_participant_invite_guest = result.is_participant_invite_guest || false;
      settings.startTime = result.start_timestamp;
      settings.endTime = result.end_timestamp;
      settings.duration = result.duration_minutes;
      settings.inviteUrl = result.invite_url;
      settings.restrictScreenShare = result.restrictScreenShare || false;
      settings.authenticationJoin = result.authenticationJoin || "Anyone";
      settings.requireAuthentication = result.requireAuthentication || false;

      settings.meetingName = result.title;
      settings.is_host_myaudio = result.is_host_myaudio || false;

      settings.status = result.status


      let sipIdList: any = [];
      _.forEach(result.participants, (item) => {
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
          moveToWaitingRoom: result.is_participant_waiting_room,
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


    } catch (err) {

      logger.info("<<<<< ERROR : Catch getMeetingbyId >>>>", err);
      return false;
    }
    // console.log(settings);

    logger.info("<<<<< Response : getMeetingbyId >>>>", settings);
    return settings;
  }


  public static async addParticipant(
    meetingId: any,
    jwtToken: any,
    participant: any
  ): Promise<setting | boolean> {
    let res: any = {};
    try {
      const dataArr: any = {};

      dataArr["headers"] = { Authorization: jwtToken };
      const url = `${baseURL}${ENDPOINTS.addParticpants}` //"https://urchat.unifiedring.co.uk:5003/api/add_participants";
      let jsondata: any = {};
      jsondata.uuid = meetingId;
      jsondata.participants = participant;
      res = await axios.post(url, jsondata, dataArr).catch((error: any) => {
        console.log(error, "error axios");

        logger.info("<<<<< ERROR : addParticipant >>>>", error);
        throw error;
      });

      if (!res || res.status !== 200) {
        return false;
      }

      const data = res.data;
      if (data.statusCode === 404) {
        return false;
      }
    } catch (err) {

      logger.info("<<<<< ERROR : Catch addParticipant >>>>", err);
      return false;
    }
    console.log("add part result",res);
    
    return res;
  }

  public static async getMeetingbyIdNoToken(
    meetingId: any
  ){
    let res: any = {};
    try {
      res = await meetingFindOne_model({ uuid: meetingId })
      if (res?.statusCode === 200) {
        return res.result;
      }else{
        return null;
      }
    } catch (err) {
      logger.info("<<<<< ERROR : Catch getMeetingbyId >>>>", err);
      return false;
    }
  }

  public static async updateMeetingNoToken(
    meetingId: any,
    details: any
  ): Promise<setting | boolean> {
    let res: any = {};
    try {
      const url = `${baseURL}${ENDPOINTS.updateMeetingNoToken}` + meetingId;//"https://urchat.unifiedring.co.uk:5003/api/update_chatsetting/" +
      res = await axios.patch(url, details).catch((error: any) => {
        console.log("error axios", error);
        logger.info("<<<<< ERROR : updateMeeting>>>>", error);
        return false;
      });
    } catch (err) {
      logger.info("<<<<< ERROR : updateMeeting catch>>>>", err);
      return false;
    }
    return res;
  }


  public static async store_meetNow(
    meetingDetails: any,
    jwtToken: any
  ): Promise<setting | boolean> {
    let res: any = {};
    try {
      const dataArr: any = {};
      dataArr["headers"] = { Authorization: jwtToken };
      const url = `${baseURL}${ENDPOINTS.meetNow}`//"https://urchat.unifiedring.co.uk:5003/api/meet_now";

      res = await axios
        .post(url, meetingDetails, dataArr)
        .catch((error: any) => {
          logger.info("<<<<< ERROR : store_meetNow >>>>", error);
          throw error;
        });

      if (!res || res.status !== 200) {
        return false;
      }

      const data = res.data;
      if (data.statusCode === 404) {
        return false;
      }
    } catch (err) {

      logger.info("<<<<< ERROR : catch store_meetNow >>>>", err);
      return false;
    }
    console.log("storemeet now my sql",res);
    return res;
  }


  public static async endMeeting(
    meetingDetails: any,
    jwtToken: any
  ): Promise<setting | boolean> {
    let res: any = {};
    try {
      console.log("meetdetails >>>>", meetingDetails);
      const dataArr: any = {};
      dataArr["headers"] = { Authorization: jwtToken };
      const url = `${baseURL}${ENDPOINTS.endMeeting}`//"https://urchat.unifiedring.co.uk:5003/api/end_meeting";

      res = await axios
        .post(url, meetingDetails, dataArr)
        .catch((error: any) => {
          console.log("error axios");

          logger.info("<<<<< ERROR : endMeeting >>>>", error);
          throw error;
        });

      if (!res || res.status !== 200) {
        return false;
      }

      const data = res.data;
      if (data.statusCode === 404) {
        return false;
      }
    } catch (err) {

      logger.info("<<<<< ERROR : catch endMeeting >>>>", err);
      return false;
    }
    console.log("endmeeting my sql",res);
    return res;
  }


  public static async joinMeeting(
    meetingDetails: any,
    jwtToken: any,
  ): Promise<setting | boolean> {
    let res: any = {};
    try {
      const dataArr: any = {};
      dataArr["headers"] = { Authorization: jwtToken };
      const url = `${baseURL}${ENDPOINTS.meetNow}`//"https://urchat.unifiedring.co.uk:5003/api/end_meeting";

      res = await axios
        .post(url, meetingDetails, dataArr)
        .catch((error: any) => {
          console.log("error axios");

          logger.info("<<<<< ERROR : joinMeeting >>>>", error);
          return false;
          throw error;
        });

      if (!res || res.status !== 200) {
        return false;
      }

      const data = res.data;
      if (data.statusCode === 404) {
        return false;
      }
    } catch (err) {

      logger.info("<<<<< ERROR : catch joinMeeting >>>>", err);
      return false;
    }
    console.log("joinmeeting my sql",res);
    return res;
  }

  public static async updateGeneralSetting(
    meetingId: any,
    jwtToken: any,
    details: any
  ): Promise<setting | boolean> {
    let res: any = {};
    try {
      const dataArr: any = {};
      dataArr["headers"] = { Authorization: jwtToken };
      const url = `${baseURL}${ENDPOINTS.updateChatSetting}` + meetingId;//"https://urchat.unifiedring.co.uk:5003/api/update_chatsetting/" +

      res = await axios.patch(url, details, dataArr).catch((error: any) => {
        console.log("error axios", error);

        logger.info("<<<<< ERROR : updateGeneralSetting >>>>", error);
        throw error;
      });
      if (!res || res.status !== 200) {
        return false;
      }

      const data = res.data;
      if (data.statusCode === 404) {
        return false;
      }
    } catch (err) {

      logger.info("<<<<< ERROR : updateGeneralSetting catch >>>>", err);
      return false;
    }
    console.log("update general setting my sql",res);
    return res;
  }

  public static async getUserData(
    jwtToken: any
  ): Promise<setting | boolean> {
    let res: any = {};
    try {
      const dataArr: any = {};

      dataArr["headers"] = { Authorization: jwtToken };
      const url = `${baseURLChat}${ENDPOINTS.getUserData}` //"https://urchat.unifiedring.co.uk:5003/api/add_participants";
      res = await axios.get(url, dataArr).catch((error: any) => {
        console.log(error, "error axios");

        logger.info("<<<<< ERROR : getuser >>>>", error);
        return false;
        throw error;
      });

      if (!res || res.status !== 200) {
        return false;
      }

      const data = res.data;
      if (data.statusCode === 404) {
        return false;
      }
    } catch (err) {

      logger.info("<<<<< ERROR : Catch getuser >>>>", err);
      return false;
    }
    console.log("get user data my sql",res);
    return res;
  }



  public static async getMeeting(
    meetingDetails: any,
    jwtToken: any
  ): Promise<setting | boolean> {
    let res: any = {};
    try {
      const dataArr: any = {};
      dataArr["headers"] = { Authorization: jwtToken };
      const url = `${baseURL}${ENDPOINTS.getMeetingbyId}/${meetingDetails}`;


      res = await meetingFindOne_model({ uuid: meetingDetails })

      /*       res = await axios.get(url, dataArr).catch((error: any) => {
              console.log("Get meeting error axios", error);
      
              logger.info("<<<<< ERROR : getMeeting >>>>", error);
              // throw error;
              return false;
            }); */

      console.log("res.data >>>>>>>", res);

      //console.log("Find Get Meeting details >>>>>>>>", res);
      if (Object.keys(res.result).length === 0 || res?.statusCode !== 200) {
        return false;
      }

      if (res.statusCode === 404) {
        return false;
      }

      console.log("Get Neeting error hanlde calling")
      return res.result;

      /*       if (res.hasOwnProperty('result')) {
              console.log("IF Get Neeting error hanlde calling")
              return res.result;
            }
            else {
              console.log("else calloing")
              let dummydata: any = {
              }
              return dummydata;
            } */


    } catch (err) {

      logger.info("<<<<< ERROR : getMeeting catch>>>>", err);
      return false;
    }

  }

  public static async hostJoining(
    meetingId: any,
    jwtToken: any
  ): Promise<setting | boolean> {
    let res: any = {};
    try {
      const dataArr: any = {};
      dataArr["headers"] = { Authorization: jwtToken };
      const url = `${baseURL}${ENDPOINTS.hostStartmeeting}/${meetingId}`;


      res = await axios.get(url, dataArr).catch((error: any) => {
        console.log("hostJoining error axios", error);

        logger.info("<<<<< ERROR : hostJoining >>>>", error);
        return false;
        throw error;
      });

      if (!res || res.status !== 200) {
        return false;
      }

      //console.log("API RESPONSE >>>>>>>>", res);

      const data = res.data;
      if (data.statusCode === 404) {
        return false;
      }
    } catch (err) {

      logger.info("<<<<< ERROR : hostJoining catch>>>>", err);
      return false;
    }

    console.log("host joining res.data.result >>>>>>", res);
    return res;
  }


  public static async participantJoining(
    meetingId: any,
    jwtToken: any
  ): Promise<setting | boolean> {
    let res: any = {};
    try {
      const dataArr: any = {};
      dataArr["headers"] = { Authorization: jwtToken };
      const url = `${baseURL}${ENDPOINTS.participantJoining}/${meetingId}`;


      res = await axios.get(url, dataArr).catch((error: any) => {
        console.log("participantJoining error axios", error);

        logger.info("<<<<< ERROR : participantJoining >>>>", error);
        return false;
      });

      if (!res || res.status !== 200) {
        return false;
      }

      const data = res.data;
      if (data.statusCode === 404) {
        return false;
      }
    } catch (err) {

      logger.info("<<<<< ERROR : participantJoining catch>>>>", err);
      return false;
    }

    console.log("participantJoining res.data.result >>>>>>", res.data);
    return res.data;
  }


  public static async updateMeeting(
    meetingId: any,
    jwtToken: any,
    details: any
  ): Promise<setting | boolean> {
    let res: any = {};
    try {
      const dataArr: any = {};
      dataArr["headers"] = { Authorization: jwtToken };


      const url = `${baseURL}${ENDPOINTS.updateMeeting}` + meetingId;//"https://urchat.unifiedring.co.uk:5003/api/update_chatsetting/" +
      res = await axios.patch(url, details, dataArr).catch((error: any) => {
        console.log("error axios", error);

        logger.info("<<<<< ERROR : updateMeeting>>>>", error);
        return false;
      });
      /*       if (!res || res.status !== 200) {
              return false;
            }
      
            const data = res.data;
            if (data.statusCode === 404) {
              return false;
            } */
    } catch (err) {

      logger.info("<<<<< ERROR : updateMeeting catch>>>>", err);
      return false;
    }
    console.log("update meeting my sql",res);
    return res;
  }
  public static async updateWhiteBoard(
    meetingId: any,
    jwtToken: any,
    details: any
  ): Promise<setting | boolean> {
    let res: any = {};
    try {
      const dataArr: any = {};
      dataArr["headers"] = { Authorization: jwtToken };
      const url = `${baseURL}${ENDPOINTS.updateWhiteBoard}`;
      res = await axios.post(url, details, dataArr).catch((error: any) => {
        console.log("error axios", error);

        logger.info("<<<<< ERROR : updateWhiteBoard >>>>", error);
        return false;
      });
      if (!res || res.status !== 200) {
        return false;
      }

      const data = res.data;
      if (data.statusCode === 404) {
        return false;
      }
    } catch (err) {

      logger.info("<<<<< ERROR : updateWhiteBoard catch>>>>", err);
      return false;
    }
    console.log("update white my sql",res);
    
    return res;
  }
  public static async getWhiteBoard(
    meetingDetails: any,
    jwtToken: any
  ): Promise<setting | boolean> {
    let res: any = {};
    try {
      const dataArr: any = {};
      dataArr["headers"] = { Authorization: jwtToken };
      const url = `${baseURL}${ENDPOINTS.getWhiteBoard}/${meetingDetails}`;
      res = await axios.get(url, dataArr).catch((error: any) => {
        console.log("error axios", error);
        logger.info("<<<<< ERROR : getWhiteBoard >>>>", error);
        return false;
      });

      if (!res || res.status !== 200) {
        return false;
      }

      const data = res.data;
      if (data.statusCode === 404) {
        return false;
      }
    } catch (err) {

      logger.info("<<<<< ERROR : getWhiteBoard catc>>>>", err);
      return false;
    }
    console.log("whiteboard  my sql",res.data.result);
    return res.data.result;
  }


  public static async deleteUser(userid: any) {


    let res: any = {};

    try {

      const url = `${baseURL}${ENDPOINTS.deleteGuestUser}/${userid}`;
      res = await axios.delete(url).catch((error: any) => {
        console.log("error axios", error);

        logger.info("<<<<< ERROR : deleteUser catc>>>>", error);
        //throw error;
        return false;
      });

      if (!res || res.status !== 200) {
        return false;
      }

      console.log("del user my sql",res.result);
      
      return res;

    } catch (err) {
      logger.info("<<<<< ERROR : deleteUser catch >>>>", err);
    }



  }

  public static async googleTranslation(
    Text: any, target: any
  ): Promise<setting | boolean> {


    return false;
    let projectId = 'unifiedring-292608'
    // Instantiates a client
    const translate = new Translate({ projectId });

    //async function quickStart(Text: any) {
    // The text to translate
    const text = Text;



    console.log("text >>>>>>>>>>>", text);
    // Translates some text into Russian
    const [translation] = await translate.translate(text, target);
    // console.log(`Text: ${text}`);
    console.log(`<<<<<<<<<<Translation>>>>>>>>>>>>: ${translation}`);

    return translation;

  }

}
