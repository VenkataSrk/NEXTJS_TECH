/**
 * @createdBy Selvakani
 * @createdOn 27th April 2021
 */

import _ from "underscore";
import { uuid } from "uuidv4";
import { CBRedisClient, configuration } from "../config";
import { logger } from "../log";
//const Redis = require("ioredis");

export class rocketMQConnection {
  publisher: any;
  constructor() {
    let redisConfig: any = {
      host: configuration.redis_client.host,
      port: configuration.redis_client.port,
    };

    redisConfig.no_ready_check = configuration.redis_client.no_ready_check;
    redisConfig.auth_pass = configuration.redis_client.auth_pass;


    //let nodes = configuration.redisCluster;
    console.log("Conference Bridge Redis IP>>>>>", redisConfig);

    this.publisher = CBRedisClient.duplicate();
  }

  public async connectionRocket(meetingdetails: any, participant: any, event: any, socketid: any, sdp?: any, participantList?: any) {
    // console.log("connectionRocket >>>", event, meetingdetails);

    //logger.info("connectionRocket >>>", participant);
    try {
      // let part_details = participant && participant[0]
      const setting: any = [];
      _.forEach(participant, (item) => {
        //console.log("Each participant >>>>", item)

        // logger.info("Each participant  >>>", item);
        let user_type = "customer";
        if (item?.isGuest) {
          user_type = "customer";
        } else {
          user_type = item?.isHost == true ? "agent" : "customer";
        }

        if (item?.conId?.includes('_screenshare')) user_type = 'screenshare'

        setting.push({
          user_id: (user_type === "customer") ? item?.name : item?.email,
          sip_id: item?.sipId,
          connection_id: socketid,
          device_id: item?.deviceid,
          user_type: user_type,
          plan_type: meetingdetails.plan_type
        });
      });

      let data: any = {
        ur_conf_message: {
          ur_conf_event: {
            event_sequence: uuid(),
            event_name: event,
            event_time: new Date(),
          },
          ur_conf_meeting_info: {
            meeting_id: meetingdetails.meetingId,
            meeting_name: meetingdetails.meetingName,
            host_joined: meetingdetails.isHostjoined,
          },
          ur_user_info: setting,
          ur_ccaas_info: {
            queue_id: meetingdetails.queueId.toString(),
            customer_name:meetingdetails.meetingDetails.agentDetails.queueName ? meetingdetails.meetingDetails.agentDetails.queueName : "",
            customer_number:meetingdetails.meetingDetails.agentDetails.customerNumber ? meetingdetails.meetingDetails.agentDetails.customerNumber : "",
            domain_id: meetingdetails.meetingDetails.agentDetails.domain_id.toString(),
            cli:meetingdetails.meetingDetails.agentDetails.customerNumber ? meetingdetails.meetingDetails.agentDetails.customerNumber : "",
            ddi:meetingdetails.meetingDetails.agentDetails.agentNumber ? meetingdetails.meetingDetails.agentDetails.agentNumber : "",
            customer_id:meetingdetails.meetingDetails.agentDetails.customerDetails.coid ? meetingdetails.meetingDetails.agentDetails.customerDetails.coid.toString() : ""
          },
          ur_media_info: {
            /*  video_enabled: true,
             audio_enabled: false,
             recording_enabled: false, */
            sdp_str: ""
          },
          correlation_id: `${participant[0]?.deviceid}_${meetingdetails?.meetingId}_${participant[0]?.sipId}`,
        },
      };

      if (event === 'sdp_update_request' || event === 'screenshare_video_req') {

        console.log("sdp_update_request >>>>>", socketid);
        data.ur_conf_message.ur_participant_list = participantList || []
        logger.info("sending to rocketmq data >>>>>", JSON.stringify(data));
        data.ur_conf_message.ur_media_info.sdp_str = sdp;
        //data.sdp_count = counter;
        // counter++;
        // console.log("sending to rocketmq data >>>>>", JSON.stringify(data));

        //logger.info("sending to rocketmq data  >>>", JSON.stringify(data));

        //this.publisher.publish("socketiorequest_1", JSON.stringify(data))
      } else if (event === 'tile_change_request') {
        data.ur_conf_message.ur_participant_list = participantList || []
        console.log("sending to rocketmq data >>>>>", JSON.stringify(data));
        logger.info("sending to rocketmq data >>>>>", JSON.stringify(data));
      }
      else if (event === 'recording_start') {
        let file: any = "";
        if (participant[0].recordFilePath.includes("VideoRecording")) { file = participant[0].recordFilePath.substring(participant[0].recordFilePath.indexOf("VideoRecording/")); }
        else {
          file = participant[0].recordFilePath.substring(participant[0].recordFilePath.indexOf("uk"));
        }

        let filepath = file.substring(file.indexOf('/') + 1)
        data.ur_conf_message.ur_user_info[0].browser_type = participant[0]?.browser_type;
        data.ur_conf_message.ur_user_info[0].rec_file_path = `${process.env.REC_FILE_PATH}${filepath}`;
        data.ur_conf_message.ur_user_info[0].rec_url = participant[0].recordFilePath;
        data.ur_conf_message.ur_user_info[0].rec_id = meetingdetails.recordId;
        data.ur_conf_message.ur_ccaas_info.agent_id= meetingdetails.meetingDetails.agentDetails.agentId ? meetingdetails.meetingDetails.agentDetails.agentId.toString() : ""

        console.log("sending to rocketmq data >>>>>", JSON.stringify(data));
        logger.info("sending to rocketmq data >>>>>", JSON.stringify(data));
      }
      else if (event === 'recording_stop') {
        let participant_arr: any = [];
        let part_list: any = meetingdetails.participants;
        let participant_list: any = Object.keys(part_list);
        participant_list.forEach((elem: any) => {
          if (part_list[elem]) {

            let data: any = {};
            data.email = part_list[elem].email;
            data.profileImg = part_list[elem].profileImg;
            data.sipId = part_list[elem].sipId;
            data.name = part_list[elem].name;
            data.ext = part_list[elem].ext;
            data.roletype = part_list[elem].roleType;
            data.user_type = part_list[elem].isGuest ? "guest" : "company";

            participant_arr.push(data);
          }
        })
        let file: any = "";
        if (participant[0].recordFilePath.includes("VideoRecording")) { file = participant[0].recordFilePath.substring(participant[0].recordFilePath.indexOf("VideoRecording/")); }
        else {
          file = participant[0].recordFilePath.substring(participant[0].recordFilePath.indexOf("uk"));
        }
        let filepath = file.substring(file.indexOf('/') + 1)

        data.ur_conf_message.ur_user_info[0].rec_file_path = `${process.env.REC_FILE_PATH}${filepath}`
        data.ur_conf_message.ur_user_info[0].rec_url = participant[0].recordFilePath;
        data.ur_conf_message.urmeet_user_list = participant_arr;
        console.log(`sending to rocketmq data >>>>>${new Date().getTime()}`, JSON.stringify(data));
        logger.info(`sending to rocketmq data >>>>>${new Date().getTime()}`, JSON.stringify(data));
      }
      else if(event === "transfer_init"){
        console.log("meetingdetails>>>>>>",meetingdetails);
        data.ur_conf_message.ur_ccaas_info.agent_mail = meetingdetails.secondaryAgentDetails.emailId;
        data.ur_conf_message.ur_ccaas_info.agent_ext = typeof(meetingdetails.secondaryAgentDetails.ext) === "string" ? meetingdetails.secondaryAgentDetails.ext : meetingdetails.secondaryAgentDetails.ext.toString();
        console.log(`sending to rocketmq data >>>>>${new Date().getTime()}`, JSON.stringify(data));
        logger.info(`sending to rocketmq data >>>>>${new Date().getTime()}`, JSON.stringify(data));
      }
      else if(event === "agent_joining"){
        if(meetingdetails.transfer){
          data.ur_conf_message.ur_ccaas_info.call_type="transfer";
        }
        console.log(`sending to rocketmq data >>>>>${new Date().getTime()}`, JSON.stringify(data));
        logger.info(`sending to rocketmq data >>>>>${new Date().getTime()}`, JSON.stringify(data));
      }
      else {
        console.log(`sending to rocketmq data >>>>>${new Date().getTime()}`, JSON.stringify(data));
        logger.info(`sending to rocketmq data >>>>>${new Date().getTime()}`, JSON.stringify(data));
      }


      //console.log("sending to rocketmq data >>>>>", JSON.stringify(data));
      this.publisher.publish(`${process.env.LIVE_CB_CHANNEL_PUBLISH}`, JSON.stringify(data));
    } catch (error) {
      console.log("rocketmq middleware error", error);
    }
  }

  public async sendSDPtoConfBridge(data: any) {

    this.publisher.publish("socketiorequest", JSON.stringify(data));
  }
}
