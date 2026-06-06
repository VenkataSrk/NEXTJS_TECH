import { worktualMySQl, worktualMySQlCcaas } from "../config";
import { MEETING_STATUS } from "../helpers/constants";
const Meetings = require('../model/meetingdetail-model');

export const meetingFindOne_model = async (
    data: any
): Promise<any> => {
    return new Promise((resolve, reject) => {
        try {

            console.log("meetingFindOne_model Bfore>>>>>>", data.uuid)
            worktualMySQl.query('CALL worktual_video_get_by_meetingid_uuid (?)', [
                data.uuid
            ], (err: any, response: any) => {
                if (err) {
                    console.log("err----", err);
                } else {

                    console.log("Event data >>>>", response[0][0]);
                    console.log("meetingFindOne_model after>>>>>>", new Date().getTime())
                    if (Array.isArray(response) && response[0]?.length !== 0) {

                        let rawData = JSON.stringify(response[0][0]);
                        let getMeeting = JSON.parse(rawData);

                        const now = new Date().getTime();

                        if (getMeeting !== null) {
                            getMeeting.participants = JSON.parse(getMeeting.participants)
                            getMeeting.chatSetting = JSON.parse(getMeeting.chatSetting)
                            getMeeting.ReadFlag = JSON.parse(getMeeting.ReadFlag)
                            getMeeting.dial_in_countries = JSON.parse(getMeeting.dial_in_countries)
                            getMeeting.files = JSON.parse(getMeeting.files)
                            getMeeting.alternate_host = JSON.parse(getMeeting.alternate_host) === undefined || null || "" || Array.isArray(JSON.parse(getMeeting.alternate_host))? {} : JSON.parse(getMeeting.alternate_host)
                            getMeeting.is_record_meeting = getMeeting.is_record_meeting === 1 ? true : false,
                                getMeeting.is_host_myaudio = getMeeting.is_host_myaudio === 1 ? true : false,
                                getMeeting.is_host_myvideo = getMeeting.is_host_myvideo === 1 ? true : false,
                                getMeeting.is_participant_video = getMeeting.is_participant_video === 1 ? true : false,
                                getMeeting.is_use_personal_meeting_id = getMeeting.is_use_personal_meeting_id === 1 ? true : false,
                                getMeeting.is_participant_waiting_room = getMeeting.is_participant_waiting_room === 1 ? true : false,
                                getMeeting.is_alternate_host = getMeeting.is_alternate_host === 1 ? true : false,
                                getMeeting.is_participant_join_bef_host = getMeeting.is_participant_join_bef_host === 1 ? true : false,
                                getMeeting.is_participant_invite_guest = getMeeting.is_participant_invite_guest === 1 ? true : false,
                                getMeeting.is_participant_mute_entry = getMeeting.is_participant_mute_entry === 1 ? true : false,
                                getMeeting.is_without_host_continue = getMeeting.is_without_host_continue === 1 ? true : false,
                                getMeeting.is_custom_password = getMeeting.is_custom_password === 1 ? true : false,
                                getMeeting.save_template = getMeeting.save_template === 1 ? true : false,
                                getMeeting.onlyContacts = getMeeting.onlyContacts === 1 ? true : false,
                                getMeeting.is_attended=getMeeting.is_attended === 1 ? true : false,
                                getMeeting.is_cancelled=getMeeting.is_cancelled === 1 ? true : false,
                                getMeeting.isRecurr=getMeeting.isRecurr === null || 0 ? false : true
                            if (getMeeting.status && getMeeting.status == MEETING_STATUS.START) {
                                if (now > getMeeting.end_timestamp && getMeeting.is_cancelled == 0 && getMeeting.is_attended == 0) {
                                    // getMeeting.status = MEETING_STATUS.UNATTENDED;
                                    getMeeting.status = MEETING_STATUS.MISSED;
                                } else {
                                    const participants: any = getMeeting.participants || [];

                                    getMeeting.status = MEETING_STATUS.START;

                                    /*    const isalternatehost: any = checkAvailability(participants, req.authorization.sipLoginId);
                                       if (getMeeting.sip_id == req.authorization.sipLoginId) {
                                           getMeeting.status = MEETING_STATUS.START;
                                       } else {
                                           getMeeting.status = MEETING_STATUS.JOIN;
                                       } */
                                }
                            }
                            if (getMeeting.status == MEETING_STATUS.ONGOING && now > getMeeting.end_timestamp) {
                                getMeeting.status = MEETING_STATUS.COMPLETED;
                            }

                            if (getMeeting.status === "Ongoing") {
                                getMeeting.status = "Ongoing Meeting";
                            }

                            console.log("process.env.NODE_ENV", process.env.NODE_ENV);

                            getMeeting.invite_url =
                                getMeeting.invite_url &&
                                getMeeting.invite_url.replace(new RegExp("\\bstagemeet.unifiedring.co.uk\\b"), "meet.worktual.co.uk");
                            /*      if (process.env.NODE_ENV === "production") {
                                  
                                   getMeeting.invite_url = getMeeting.invite_url && getMeeting.invite_url.replace(new RegExp("\\bstagemeet.unifiedring.co.uk\\b"), "meet.unifiedring.co.uk");
                                 }
                                 else */
                            if (process.env.NODE_ENV === "staging") {
                                getMeeting.invite_url =
                                    getMeeting.invite_url &&
                                    getMeeting.invite_url.replace(new RegExp("\\bmeet.unifiedring.co.uk\\b"), "stagemeet.worktual.co.uk");
                            }

                            if (process.env.NODE_ENV === "development_mysql") {
                                getMeeting.invite_url =
                                    getMeeting.invite_url &&
                                    getMeeting.invite_url.replace(new RegExp("\\bmeet.unifiedring.co.uk\\b"), "stagemeet-mysql.worktual.co.uk");
                            }

                            if (process.env.NODE_ENV === "qastaging") {
                                getMeeting.invite_url =
                                    getMeeting.invite_url &&
                                    getMeeting.invite_url.replace(new RegExp("\\bqameet.worktual.co.uk\\b"), "qameet.qaworktual.co.uk");
                            }

                            // console.log("FInal Response >>>>", getMeeting)
                            resolve({ statusCode: 200, result: getMeeting });
                        } else {
                            resolve({ statusCode: 404, result: {} });
                        }
                    } else {
                        resolve({ statusCode: 204, result: {} });
                    }
                }
            })
        } catch (err) {
            //reject(err);

            console.log("Getmeetig <ysql Eror >>>>>>>", err)
            resolve({ statusCode: 404, result: {} });
        }
    });
};


export const getSettings = async (id: any) => {
    console.log("idid>>>>>>>>>",id);
    return new Promise(async (resolve, reject) => {
      try {
        worktualMySQlCcaas.query(
          'call up_getVideoCallConfiguration(?)',
          [id ?? null],
          (err: any, result: any) => {
            console.log('err', err);
            console.log('getSettingsgetSettingsresult', result);
            if (err) {
              reject(err);
              console.log(err);
            }
            if(Array.isArray(result) && result[0]?.length !== 0){resolve(result[0][0]);}
            else{resolve({})}
          }
        );
      } catch (err) {
        reject(err);
        console.log("get video config error",err);
      }
    });
  };
