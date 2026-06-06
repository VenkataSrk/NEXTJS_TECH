import generator from 'generate-password';
import * as jwt from 'jsonwebtoken';
import _ from 'lodash';
import { configs } from '../../config/app';
import { deleteSettingsApi, getMeetInfo, getSettingsApi, getUrls, insertRecordTranscriptSP, insertUpdateSettingsApi, insertUpdateVirtualBackgroundApi, insertVerificationAttachmentsSP, setGuestDetails, setMeetInfo } from '../../dao/video';
import { logger } from '../../plugins/log';

export async function createAgentMeeting(req: any, res: any, adaptor: any) {
  try {
    const request: any = req.body;
    const authorization = req.headers;
    if (!authorization?.callHost || !authorization?.videoHost) {
      res.send({ statusCode: 424, message: 'Failed to create token, please try again' });
      return;
    }
    let participants: any = [
      {
        sipid: authorization.sipLoginId,
        name: authorization.profileName || '',
        ext: authorization.ext,
        profile_img: authorization.profileImg || '',
        roletype: 1,
        email: authorization.username,
        user_type: 'company',
      },
    ];

    if (req?.body?.participants) {
      participants = req?.body?.participants;
    }
    const data = parseMeetingReq(req, req.headers, '');
    const getData: any = await getSettingsApi(request.agentDetails.domain_id);
    logger.info('getSettingsApi', getData);
    data.title = req.body.title;
    data.description = '';
    data.meeting_id = data.uuid;
    data.start_timestamp = new Date().getTime();
    data.end_timestamp = new Date().getTime() + (24 * 60 * 60 * 1000);
    data.duration_minutes = 60;
    data.timezone = '';
    data.joining_option_type = '';
    data.calender_type = '';
    data.rrule = '';
    data.is_host_myvideo = true;
    data.is_record_meeting = false;
    data.is_alternate_host = false;
    data.is_participant_video = true;
    data.is_participant_join_bef_host = true;
    data.is_participant_invite_guest = false;
    data.is_participant_waiting_room = false;
    data.is_participant_mute_entry = false;
    data.is_use_personal_meeting_id = false;
    data.personal_meeting_id = '';
    data.is_without_host_continue = true;

    data.is_custom_password = false;
    data.dial_in_countries = [];
    data.password = '';
    data.participants = participants;
    data.queueId = request.queueId;
    data.agentDetails = request.agentDetails;
    data.startTime = request.startTime;
    data.inviteUrl = `${process.env.APP_URL}/customervideochat/${data.uuid}`;
    data.domainId = request.domainId;
    data.is_attended = 1;
    data.password = '';
    data.save_template = false;
    data.hostName = authorization.profileName || '';
    data.call_host = authorization.callHost;
    data.video_host = authorization.videoHost;

    data.status = 'Start';
    if (getData?.length > 0) {
      if (!_.isEmpty(request.agentDetails.vm_settings)) {
        if (request.agentDetails.vm_settings.isLinkExpiry === 1) {
          if (request.agentDetails.vm_settings.expiryUnitType === 'Minutes') {
            data.end_timestamp = new Date(data.start_timestamp).setMinutes(new Date(data.start_timestamp).getMinutes() + request.agentDetails.vm_settings.expiryUnit);
            data.duration_minutes = (data.end_timestamp - data.start_timestamp) / 60000 >= 1 ? Math.round((data.end_timestamp - data.start_timestamp) / 60000) : (data.end_timestamp - data.start_timestamp) / 60000;
          }else {
            data.end_timestamp = new Date(data.start_timestamp).setHours(new Date(data.start_timestamp).getHours() + request.agentDetails.vm_settings.expiryUnit);
            data.duration_minutes = (data.end_timestamp - data.start_timestamp) / 60000 >= 1 ? Math.round((data.end_timestamp - data.start_timestamp) / 60000) : (data.end_timestamp - data.start_timestamp) / 60000;
          }
        }
      }
    }
    const uuid: any = data.uuid; const invite_url = `${process.env.APP_URL}/customervideochat/${uuid}`;
    if (data.uuid !== null) {
      const meet: any = {};
      meet.meetingDetails = data;
      await setMeetInfo(data.uuid, meet);
      res.status(200).send({
        uuid,
        statusCode: 200,
        message: 'successfully updated',
        invite_url: `${process.env.APP_URL}/customervideochat/${data.uuid}`
      });
    } else {
      res.status(200).send({ statusCode: 404, message: 'uuid is not available' });
    }

  } catch (err) {
    logger.error('createAgentMeeting - Unexpected Error:', req.headers, req.body, err);
  }
}

export async function getAgentMeeting(req: any, res: any, adaptor: any) {
  try {
    const uuid: any = req.params.uuid;
    if (uuid !== null) {
      const response = await getMeetInfo(uuid);
      logger.info('getMeetInfo', response);
      const data: any = JSON.parse(response);
      res.status(200).send({
        data,
        statusCode: 200,
        message: 'fetched successfully ',
      });
    } else {
      res.status(200).send({ statusCode: 404, message: 'uuid is not available' });
    }
  } catch (err) {
    logger.error('getAgentMeeting - Unexpected Error:', req.params, err);
  }
}
function makeid(length: number) {
  let result = '';
  const characters = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789';
  for (const val of Array(length)) {
    result += characters.charAt(Math.floor(Math.random() * characters.length));
  }
  return result;
}

function parseMeetingReq(req: any, authorization: any, password: string) {
  const meeting_uuid: any = `${makeid(3)}-${makeid(2)}-${makeid(3)}-${makeid(2)}`;
  const data: any = req.body;
  data.meeting_id = meeting_uuid;
  data.server_host = authorization.hostAddress;
  data.hostName = authorization.profileName;
  data.is_cancelled = 0;
  data.is_attended = 0;
  data.uuid = meeting_uuid;
  data.sip_id = authorization.sipLoginId;
  data.password = password;
  data.invite_url = `${process.env.APP_URL}/customervideochat/${meeting_uuid}`;
  return data;
}

export async function insertVmRecordTranscript(req: any, res: any, done: any) {
  try {
    const data = req?.body;

    const insertData: any = await insertRecordTranscriptSP(data.domain_id, data.recordId, data.recordTranscript);
    logger.info('insertRecordTranscriptSP', insertData);
    if (insertData?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        data: insertData,
        message: 'Success'
      });
    } else {
      res.status(200).send({ statusCode: 200, message: 'Data empty', data: insertData });
    }
  } catch (err) {
    logger.error('insertVmRecordTranscript - Unexpected Error:', req.body, err);
    res.send({ statusCode: 500, message: 'internal server error' });
  }
}
export async function insertVmVerificationAttachments(req: any, res: any, done: any) {
  try {
    const data = req?.body;

    const insertData: any = await insertVerificationAttachmentsSP(data.domain_id, data.coid, data.attachments);
    logger.info('insertVerificationAttachmentsSP', insertData);
    if (insertData?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        data: insertData,
        message: 'Success'
      });
    } else {
      res.status(200).send({ statusCode: 200, message: 'Data empty', data: insertData });
    }
  } catch (err) {
    logger.error('insertVmVerificationAttachments - Unexpected Error:', req.body, err);
    res.send({ statusCode: 500, message: 'internal server error' });
  }
}
export async function createAuthToken(req: any, res: any, done: any) {
  try {
    const data: any = {};
    const uuid = req.body.meetingid;
    let getMeeting: any;
    getMeeting = await getMeetInfo(uuid);
    logger.info(getMeeting, 'getMeeting===');
    getMeeting = JSON.parse(getMeeting ?? 'null');

    if (getMeeting !== null) {
      const sipId = Math.floor(1000 + Math.random() * 900000).toString();
      const extn = Math.floor(100 + Math.random() * 90000).toString();
      if (getMeeting?.meetingDetails.domain_id === null || getMeeting?.meetingDetails.domain_id === undefined) {
        logger.info('domain id is null or undefined');

        res.send({ statusCode: 424, message: 'Failed to create token, please try again' });
        return;
      }
      const d_id = `${getMeeting?.meetingDetails.domain_id}`;
      const pwd = generator.generate({
        length: 8,
        numbers: true,
      });
      const userName = req.body.name && req.body.name.replace(/\s+/g, '');
      const ext = getMeeting?.meetingDetails?.agentDetails?.customerNumber;
      const sipid = getMeeting?.meetingDetails?.agentDetails?.customerNumber;
      data.sip_login_id = sipid;
      data.role_id = '';
      data.domain_id = d_id;
      data.ext = ext;
      data.company_id = '';
      data.enetepriseid = '';
      data.host_address = '';
      data.order_id = '';
      data.dir_user_id = '';
      data.login_user_name = req.body.name;
      data.caller_id = req.body.caller_id;
      data.login_password = pwd;
      data.login_device_id = 'guest';
      data.login_source = 'guest';
      data.login_ipaddress = '';
      data.meeting_pwd = getMeeting.meetingDetails.password;
      data.meetingId = req.body.meetingid;
      data.type = 'guest';
      data.call_host = getMeeting.meetingDetails.agentDetails.domain_name;
      data.video_host = getMeeting.meetingDetails.agentDetails.video_meet_domain;
      const sipDetails = {
        user_id: `${ext}`,
        domain_id: `${d_id}`,
        password: `${pwd}`,
        callerName: `${userName}`,
      };
      await setGuestDetails(sipDetails, data);
      const token = await createSession(data);
      const meetData = getMeeting;
      meetData.guestDetails = data;

      await setMeetInfo(uuid, meetData);
      res.send({
        statusCode: 200,
        user_details: {
          ext,
          domain_name: getMeeting?.meetingDetails.agentDetails.domain_name,
          dp_password: pwd,
          video_domain_name: getMeeting?.meetingDetails.agentDetails.video_meet_domain,
        },
        message: token,
      });
    } else {
      res.send({ statusCode: 424, message: 'Meeting not found' });
    }
  } catch (err) {
    logger.error('getChatFlowlanguageHandler - Unexpected Error:', req.body, err);
    res.send({ statusCode: 500, message: 'internal server error' });
  }
}

export async function createSession(data: any) {
  return generateToken(data);
}

function generateToken(data: any) {
  const username = data.login_user_name;
  const password = data.login_password;
  const deviceId = data.login_device_id;
  const sipLoginId = data.sip_login_id;
  const roleId = data.role_id;
  const domainId = data.domain_id;
  const ext = data.ext;
  const companyId = data.company_id;
  const enetepriseid = data.enetepriseid;
  const hostAddress = data.host_address;
  const orderId = data.order_id;
  const dirUserId = data.dir_user_id;
  const meetingId = data.meetingId;
  const meeting_pwd = data.meeting_pwd;
  const type = data.type;
  const caller_id = data.caller_id;
  const call_host = data.call_host;
  const video_host = data.video_host;

  const jwtPayload = {
    username,
    password,
    deviceId,
    sipLoginId,
    roleId,
    domainId,
    ext,
    companyId,
    enetepriseid,
    hostAddress,
    orderId,
    dirUserId,
    type,
    meetingId,
    meeting_pwd,
    caller_id,
    call_host,
    video_host
  };

  return jwt.sign(jwtPayload, configs.jwtsecret);
}

export async function getSettings(req: any, res: any, done: any) {
  try {
    const domainId = req?.query?.domainId;
    const getData: any = await getSettingsApi(domainId);
    logger.info('getSettingsApi', getData);
    if (getData?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        data: getData,
        message: 'Success'
      });
    } else {
      res.status(200).send({ statusCode: 200, message: 'Data empty', data: getData });
    }
  } catch (err) {
    logger.error('getSettings - Unexpected Error:', req.query, err);
    res.send({ statusCode: 500, message: 'internal server error' });
  }
}

export async function insertUpdateSettings(req: any, res: any, done: any) {
  try {
    const data = req?.body;
    const insertData: any = await insertUpdateSettingsApi(data);
    logger.info('insertUpdateSettingsApi', insertData);
    if (insertData?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        data: insertData,
        message: 'Success'
      });
    } else {
      res.status(200).send({ statusCode: 200, message: 'Data empty', data: insertData });
    }
  } catch (err) {
    logger.error('insertUpdateSettings - Unexpected Error:', req.body, err);
    res.send({ statusCode: 500, message: 'internal server error' });
  }
}

export async function deleteSettings(req: any, res: any, done: any) {
  try {
    const data = req?.query;
    const deleteData: any = await deleteSettingsApi(data);
    logger.info('deleteData', deleteSettingsApi);
    if (deleteData?.affectedRows) {
      res.status(200).send({
        statusCode: 200,
        message: 'Success'
      });
    } else {
      res.status(200).send({ statusCode: 200, message: 'Data not found' });
    }
  } catch (err) {
    logger.error('deleteSettings - Unexpected Error:', req.query, err);
    res.send({ statusCode: 500, message: 'Internal server error' });
  }
}

export async function getVirtualBackgroundUrls(req: any, res: any, done: any) {
  try {
    const domainId = req?.query?.domainId;
    const getData: any = await getUrls(domainId);
    logger.info('getUrls', getData);
    if (getData?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        data: getData,
        message: 'Success'
      });
    } else {
      res.status(200).send({ statusCode: 200, message: 'Data empty', data: getData });
    }

  } catch (err) {
    logger.error('getVirtualBackgroundUrls - Unexpected Error:', req.query, err);
    res.send({ statusCode: 500, message: 'internal server error' });
  }
}

export async function insertUpdateVirtualBackgroundUrls(req: any, res: any, done: any) {
  try {
    const data = req?.body;
    const insertData: any = await insertUpdateVirtualBackgroundApi(data);
    logger.info('insertUpdateVirtualBackgroundApi', insertData);
    if (insertData[0]?.errCode > 0) {
      res.status(200).send({
        statusCode: 200,
        data: insertData[0]?.errMsg,
        message: 'Success'
      });
    } else {
      res.status(200).send({ statusCode: 200, message: 'Failed', data: insertData[0]?.errMsg });
    }

  } catch (err) {
    logger.error('getVirtualBackgroundUrls - Unexpected Error:', req.body, err);
    res.send({ statusCode: 500, message: 'internal server error' });
  }
}
