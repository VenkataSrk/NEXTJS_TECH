import {
  aiccasGetConfigDetailsDao,
  basicInsertPrompt,
  defaultGlobalShortcutKeys,
  deleteOnboardUser,
  deleteUserActiveStatus,
  getAddonLanguagesDao,
  getAgentActivityDao,
  getAgentSupervisior,
  getAllProfileInfo,
  getAllUserList,
  getCallBackOption,
  getCcaasRoleInfo,
  getexpiredplandetailsDao,
  getInteraction,
  getInteractionLogDetailsDao,
  getInteractionView,
  getLoginToken,
  getOverallCallbackInteractionDetailsDao,
  getParticularInteractionLogDetailsDao,
  getProfileDao,
  getRoutingProfile,
  getSessionDetails,
  getUserDeatils,
  getuserdetails,
  getUserDetailsEmail,
  getUserDetailsList,
  getUserIdUser,
  getUserListByRole,
  inserSkill,
  insertAgentStatus,
  insertBasicCallFlow,
  insertBasicChatFlow,
  insertBusinessHours,
  insertCustomerTag,
  insertDefaultShortcutKeysDao,
  insertDispositionStatus,
  insertOnboardDidNumber,
  insertOnboardUser,
  insertQueue,
  insertRouting,
  loginSessionHistory,
  otpGeneratorDao,
  otpValidatorDao,
  smtpConfig,
  UpdateChatFlagDao,
  updateOnboardActiveStatus,
  updateOnboardUser,
  updatePassword,
  updateProfileNameDao,
  updateProfilePicDao,
  updateSessionHistory,
  updateTicketingId,
  updateUser,
  userUpdate,
} from '../../dao/login';
import { RESPONSE } from '../../helpers/constants';
import {
  getAgentStatuses,
  getBasicSkill,
  getBusinessHours,
  getCallflowEdges,
  getCallflowNodes,
  getCustomerTag,
  getDefaultCallflowData,
  getDefaultChatflowData,
  getDispositionData,
  getKevinChatflowEdges,
  getKevinChatflowNodes,
  getPlayPrompt,
  getQueueData,
  getRoutingProfileData,
} from './helper';

/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */
import axios from 'axios';
import bcrypt from 'bcrypt';
import jwt from 'jsonwebtoken';
import jwt_decode from 'jwt-decode';
import moment from 'moment';
import nodemailer from 'nodemailer';
import shortid from 'shortid';
import { pipeline } from 'stream';
import util from 'util';
import { v4 as uuidv4 } from 'uuid';
import { insertBotImagesDao } from '../../dao/bot';
import { updateCallFlowMap1 } from '../../dao/callFlow';
import { getOverallCallbackGroupInteractionDetailsDao, getOverallCallbackGroupInteractionPerticularDetailsDao, getSocialMediaMessageByIdDao, getSocialMediHistoryDetailsHandlerDao } from '../../dao/user';
import { ioredisWhatsapp } from '../../plugins/db';
import { logger } from '../../plugins/winston';
import { sendEmailWithOtp } from '../../services/mailSmtp';
import { sendSmsMessageOutbound } from '../../services/smsOut';

const jwt_secret = 'jwtfanhere';
const jwt_expiration = 60 * 10;

const pump = util.promisify(pipeline);

/**
 *
 * @param req
 * @param res
 *
 * ccs forgot Password api need to do migration
 */
// currently used in front end but need to do migration
export async function forgotPasswordHandler(req: any, res: any, done: any) {
  try {
    const data: any = { emailId: req.body.emailId };
    const userInfo: any = await getuserdetails(data); // currently using old table
    logger.info('getuserdetailsUserInfo', userInfo);
    if (userInfo.statusCode === 404) {
      res
        .status(200)
        .send({ statusCode: 404, message: RESPONSE.invalid_email });
    } else {
      const userd = {
        emailId: userInfo.emailId,
        accountId: userInfo.accountId,
        domainId: userInfo.domainId,
      };
      const token = jwt.sign({ userd }, jwt_secret, {
        expiresIn: jwt_expiration,
      });
      const generatelink = `${process.env.API_LOCAL_URL}/resetPassword?accesstoken=${token}`;
      const content = `Hi ${userInfo.firstName},<br /><br />Please click on the link below to reset the password <br /> <a href=${generatelink}" target = "blank" >Change Here</a> <br/> <br/> THANK YOU ${userInfo.firstName} `;
      sendMaillink(content, userd); // currently using old table
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.recovery_link_successfully,
      });
    }
  } catch (error) {
    req.log.error(error);
    logger.error('forgotPasswordHandler - Unexpected Error:', req.body, error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.mail_not_sent });
  }
}

// need to do migration
export async function resetPassLink(req: any, res: any, done: any) {
  try {
    const decoded: any = jwt_decode(req.query.accesstoken);
    const changedUser: any = await getuserdetails(decoded.userd); // old db used
    logger.info('getuserdetailsChangedUser', changedUser);
    const changePass: any = req.body.updatePass;
    if (req.body.updatePass === '' || undefined) {
      res
        .status(200)
        .send({ statusCode: 404, message: RESPONSE.password_empty });
    } else {
      const salt = await bcrypt.genSalt(10);
      const encrpytPass = await bcrypt.hash(changePass, salt);
      const changedPassword: any = await updatePassword(
        encrpytPass,
        changedUser
      );
      res
        .status(200)
        .send({ statusCode: 200, message: RESPONSE.pass_changed_successfully });
    }
  } catch (error) {
    req.log.error(error);
    logger.error('resetPassLink - Unexpected Error:', req.body, error);
    res
      .status(500)
      .send({ statusCode: 500, message: RESPONSE.password_not_changed });
  }
}

/**
 * update user
 * @param req
 * @param res
 * @param done
 */
// used in the live code but have to check why we need to still using it
export async function updateUserHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const body: any = req.body;
    const data: any = {
      emailId: body.emailId,
      ext: auth.ext,
      companyId: auth.companyId,
      domainId: auth.domainId,
      directNumber: body.directNumber,
      address: body.address,
      routing_profile: body.routing_profile,
      report_to: body.report_to,
      phone_type: body.phone_type,
      phone_number: body.phone_number,
      desk_phone_number: body.desk_phone_number,
      after_call_work_timeout: body.after_call_work_timeout,
      initial_setup_menu: body.initial_setup_menu
        ? body.initial_setup_menu
        : null,
      initial_setup_flag: body.initial_setup_flag
        ? body.initial_setup_flag
        : null,
      add_skill: body.add_skill,
      initial_selection_flag: body.initial_selection_flag
        ? body.initial_selection_flag
        : null,
      country_code: body.country_code,
      state: body.state,
      stateTimer: body.stateTimer,
      callType: body.callType,
      customer: body.customer,
      name: body.name,
      localization: body.localization,
      routing_profile_name: body.routing_profile_name,
      active_status: body.active_status,
      outBound_call: body?.outBound_call ?? 0,
      report_to_name: body?.report_to_name ?? null,
      isVoice: body?.isVoice ?? null,
      isVoiceCallTransfer: body?.isVoiceCallTransfer ?? null,
      isChat: body?.isChat ?? null,
      isChatTransfer: body?.isChatTransfer ?? null,
      isEmail: body?.isEmail ?? null,
      isEmailTransfer: body?.isEmailTransfer ?? null,
    };
    if (data.emailId !== '') {
      const updatequeue: any = await updateUser(data);
      logger.info('updateUser', updatequeue);
      JSON.parse(JSON.stringify(updatequeue) ?? 'null');
      if (updatequeue.flag === 1) {
        res
          .status(200)
          .send({ statusCode: 200, message: RESPONSE.User_update_success });
      } else {
        res
          .status(200)
          .send({ statusCode: 200, message: RESPONSE.user_insert_success });
      }
    } else {
      res
        .status(200)
        .send({ statusCode: 404, message: RESPONSE.please_provider });
    }
  } catch (err) {
    logger.error('updateUserHandler - Unexpected Error:', req.headers, req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function getAgentActivity(req: any, res: any, done: any) {
  try {
    const data: any = {
      domainId: req?.headers?.domainId,
      emailId: req?.headers?.username,
    };
    const getList: any = await getAgentActivityDao(data);
    logger.info('getAgentActivityDao', getList);
    if (Array.isArray(getList) && getList.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getUserListNumber: getList,
      });
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        getUserListNumber: [],
      });
    }
  } catch (err) {
    logger.error('getAgentActivity - Unexpected Error:', req?.headers , err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getAgentSupervisiorActivity(
  req: any,
  res: any,
  done: any
) {
  try {
    const data: any = {
      ext: req?.headers?.ext,
      domainId: req?.headers?.domainId,
    };
    const getList: any = await getAgentSupervisior(data);
    logger.info('getAgentSupervisior', getList);
    if (Array.isArray(getList) && getList.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getAgentList: getList,
      });
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        getAgentList: [],
      });
    }
  } catch (err) {
    logger.error('getAgentSupervisiorActivity - Unexpected Error:', req?.headers, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

// used in forget password link need to check
export async function sendMaillink(content: any, userd: any) {
  try {
    const smtpSettingInfo: any = await smtpConfig(userd);
    logger.info('smtpConfig', smtpSettingInfo);
    const smtpArr: any = JSON.stringify(smtpSettingInfo[0]);
    const smtpArrInfo: any = JSON.parse(smtpArr ?? 'null');
    const transporter = nodemailer.createTransport({
      host: smtpArrInfo.smtp_host,
      port: smtpArrInfo.smtp_port,
      secure: false,
      auth: {
        user: smtpArrInfo.smtp_user,
        pass: smtpArrInfo.smtp_password,
      },
    });
    const mailMessage = {
      from: smtpArrInfo.smtp_from_mail_id,
      to: userd.emailId,
      subject: 'Recovery Password link',
      html: content,
    };
    return new Promise<void>((resolve, reject) => {
      const sendMail: any = transporter.sendMail(mailMessage, (err, info) => {
        // if (err) {
        //   resolve(err);
        // } else if (info.response === '250 OK') {
        //   ;
        //   resolve(info);
        // }
      });
      return sendMail;
    });
  } catch (error) {
    logger.error('sendMaillink - Unexpected Error:', error);
  }
}

export async function getUserListByRoleHandler(req: any, res: any, done: any) {
  try {
    const dataq: any = {
      domainId: req?.headers?.domainId,
    };
    const roleId = req?.params?.roleId;
    const getRoleId: any = await getUserListByRole(roleId, dataq);
    logger.info('getUserListByRole', getRoleId);
    if (Array.isArray(getRoleId) && getRoleId.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getUserListByRoleId: getRoleId,
      });
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        getRoleListRes: [],
      });
    }
  } catch (err) {
    logger.error('getUserListByRoleHandler - Unexpected Error:', req?.headers, req?.params, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getUserDeatilsHandler(req: any, res: any, done: any) {
  try {
    const domainId = req?.headers?.domainId;
    const report_to = req.params.report_to;
    const getList: any = await getUserDeatils(report_to, domainId);
    logger.info('getUserDeatils', getList);
    let arrRes: any = [];
    arrRes = getList.map((data: any) => data.ext);
    if (Array.isArray(getList) && getList[0]?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        agentExt: arrRes,
        getUserDetails: getList[0],
      });
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        agentExt: [],
        getUserDetails: [],
      });
    }
  } catch (err) {
    logger.error('getUserDeatilsHandler - Unexpected Error:', req?.headers, req?.params, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getAllUserListHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      domainId: req?.headers?.domainId,
      ext: req?.headers?.ext,
    };
    const getAllList: any = await getAllUserList(data);
    logger.info('getAllUserList', getAllList);
    if (Array.isArray(getAllList) && getAllList[0]?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getAllUserList: getAllList[0],
      });
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        getAllUserList: [],
      });
    }
  } catch (err) {
    logger.error('getAllUserListHandler - Unexpected Error:', req?.headers, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getAllProfileInfoHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      ext: req?.headers?.ext,
      domainId: req?.headers?.domainId,
    };
    const getAllUser: any = await getAllProfileInfo(data);
    logger.info('getAllProfileInfo', getAllUser);
    if (Array.isArray(getAllUser) && getAllUser?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getAllUserList: getAllUser,
      });
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        getAllUserList: [],
      });
    }
  } catch (err) {
    logger.error('getAllProfileInfoHandler - Unexpected Error:', req?.headers, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getUserDetailsListHandler(req: any, res: any, done: any) {
  try {
    const uid = req?.params?.uid;
    const data: any = {
      domainId: req?.headers?.domainId,
    };
    const getDetailsList: any = await getUserDetailsList(data, uid);
    logger.info('getUserDetailsList', getDetailsList);
    if (Array.isArray(getDetailsList) && getDetailsList.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getUserDetailsListRes: getDetailsList,
      });
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        getUserDetailsListRes: [],
      });
    }
  } catch (err) {
    logger.error('getUserDetailsListHandler - Unexpected Error:', req?.headers, req?.params, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 * update user
 * @param req
 * @param res
 * @param done
 */
export async function userUpdateHandler(req: any, res: any, done: any) {
  try {
    const body: any = req?.body;
    const data: any = {
      emailId: body.emailId,
      domainId: req?.headers?.domainId,
      routing_profile: body?.routing_profile ?? null,
      report_to: body.report_to ?? null,
      add_skill: body.add_skill ?? null,
      outBound_call: body?.outBound_call ?? 0,
      // routing_profile_name: body.routing_profile_name ?? null,
      // report_to_name: body?.report_to_name ?? null,
      voice: body?.voice ?? 0,
      chat: body?.chat ?? 0,
      email: body?.email ?? 0,
      sms: body?.sms ?? 0,
      socialMedia: body?.socialMedia ?? 0,
      chatLimit: body?.chatLimit ?? 0,
      pri_language: body?.primaryLanguage?.addonId ? JSON.stringify(body?.primaryLanguage) : null,
      sec_language: body?.secondaryLanguage?.length > 0 ? JSON.stringify(body?.secondaryLanguage) : null,
      inboundCall: body?.inboundCall ?? 0,
      outboundCampaignCall: body?.outboundCampaignCall ?? 0,
      is_outbound_campaign_specific_agent: body?.is_outbound_campaign_specific_agent ?? 0
    };
    if (data.emailId !== '') {
      const updatequeue: any = await userUpdate(data);
      logger.info('userUpdate', updatequeue);
      if (updatequeue[0]?.errcode === 0) {
        ioredisWhatsapp
          .to(`${data.domainId}`)
          .emit('updateUserManagement', data);
        res.status(200).send({
          statusCode: 200,
          message: RESPONSE.User_update_success,
          userUpdateRes: data,
        });
      } else {
        res.status(200).send({ statusCode: 404, message: RESPONSE.not_update });
      }
    } else {
      res
        .status(200)
        .send({ statusCode: 404, message: RESPONSE.please_provider });
    }
  } catch (err) {
    logger.error('userUpdateHandler - Unexpected Error:', req?.body, req?.headers, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 * set Default company
 * @param req
 * @param res
 * @param done
 */
// need to validate whether it is being used or not
export async function setDefaultCompanyHandler(req: any, res: any, done: any) {
  try {
    getAgentStatuses(req).forEach(async (list: any) => insertAgentStatus(list));
    getDispositionData(req).forEach(async (list: any) =>
      insertDispositionStatus(list)
    );
    getCustomerTag(req).forEach(async (list: any) => insertCustomerTag(list));
    getBasicSkill(req).forEach(async (list: any) => inserSkill(list));
    getPlayPrompt(req).forEach(async (list: any) => basicInsertPrompt(list));
    // basicInsertPrompt(getPlayPrompt(req));
    const businessHours = getBusinessHours(req);
    await insertBusinessHours(businessHours.outOfBusiness).then(
      (result1: any) => {
        const cid = result1?.[0]?.bid;
        const outOfBusinessHours = { ...businessHours.outOfBusiness, cid };
        const updateBusinessHours = {
          ...businessHours,
          outOfBusiness: outOfBusinessHours,
        };
        insertQueue(getQueueData(req, updateBusinessHours, cid)).then(
          async (result2: any) => {
            const qid = result2?.[0]?.qid;
            insertRouting(getRoutingProfileData(req, qid));
            if (req.body?.plan_id === 32) {
              const chatnodes = getKevinChatflowNodes(updateBusinessHours, qid);
              const chatedges = getKevinChatflowEdges();
              const chatflowData = getDefaultChatflowData(req);
              const chatnodeData: any = {
                nodes: JSON.stringify(chatnodes),
                edges: JSON.stringify(chatedges),
              };
              insertBasicChatFlow(chatflowData, chatnodeData);
            }
            const nodes = getCallflowNodes(updateBusinessHours, qid);
            const edges = getCallflowEdges();
            const callflowData = getDefaultCallflowData(req);
            const nodeData: any = {
              nodes: JSON.stringify(nodes),
              edges: JSON.stringify(edges),
            };
            insertBasicCallFlow(callflowData, nodeData).then((__: any) => {
              res
                .status(200)
                .send({ statusCode: 200, message: RESPONSE.insert_success });
            });
          }
        );
      }
    );
  } catch (err) {
    logger.error('setDefaultCompanyHandler - Unexpected Error:', req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 * insert user
 * @param req
 * @param res
 * @param done
 */
// need to validate whether the api is being consumed or not
// api not in live
// but the sp used for the db is migrated to latest DB
export async function insertOnboardUserHandler(req: any, res: any, done: any) {
  try {
    const getRouting: any = await getRoutingProfile(req?.body?.domainId);
    const data: any = {
      UserName: req.body?.UserName ?? null,
      emailId: req?.body?.emailId ?? null,
      directNumber: req?.body?.directNumber ?? 0,
      address: req?.body?.address ?? null,
      routing_profile_id: getRouting[0]?.rid ?? 0,
      report_to_UserID: req?.body?.report_to_UserID ?? 0,
      sipLoginId: req?.body?.sipLoginId ?? 0,
      companyId: req?.body?.companyId ?? 0,
      domainId: req?.body?.domainId ?? 0,
      ext: req?.body?.ext ?? 0,
      isActive: req?.body?.isActive ?? 0,
      callTypeId: req?.body?.callTypeId ?? 0,
      roleid: req?.body?.roleid ?? 0,
      localization: req?.body?.localization ?? null,
      currentSession: req?.body?.currentSession ?? null,
      isVoice: req?.body?.isVoice ?? 0,
      isVoiceCallTransfer: req?.body?.isVoiceCallTransfer ?? 0,
      isChat: req?.body?.isChat ?? 0,
      isChatTransfer: req?.body?.isChatTransfer ?? 0,
      isEmail: req?.body?.isEmail ?? 0,
      isEmailTransfer: req?.body?.isEmailTransfer ?? 0,
      StatusID: req?.body?.statusID ?? 0,
      isSms: req?.body?.isSms ?? 0,
      isSocialMedia: req?.body?.isSocialMedia ?? 0,
      addSkill: req?.body?.addSkill ?? [],
      voice: req?.body?.voice ?? 0,
      chat: req?.body?.chat ?? 0,
      email: req?.body?.email ?? 0,
      sms: req?.body?.sms ?? 0,
      socialMedia: req?.body?.socialMedia ?? 0,
    };
    if (data.emailId !== '') {
      const inserUser: any = await insertOnboardUser(data);
      logger.info('insertOnboardUser', inserUser);
      if (inserUser[0]?.errcode === -1) {
        res
          .status(200)
          .send({ statusCode: 521, message: RESPONSE.User_already_exist });
      } else {
        res
          .status(200)
          .send({ statusCode: 200, message: RESPONSE.user_insert_success });
      }
    } else {
      res
        .status(200)
        .send({ statusCode: 404, message: RESPONSE.should_not_be_empty });
    }
  } catch (err) {
    logger.error('insertOnboardUserHandler - Unexpected Error:', req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * did onboard number
 */
export async function insertOnboardDidNumberHandler(
  req: any,
  res: any,
  done: any
) {
  try {
    const data: any = {
      user_id: req?.body?.ext ?? 0,
      city: req?.body?.city ?? null,
      country: req?.body?.country ?? null,
      your_number: req?.body?.your_number ?? null,
      domainId: req?.body?.domainId ?? 0,
      callFlowId: process.env.DEFAULT_CALLFLOW_ID,
      recording: req?.body?.recording ?? 1,
      status: req?.body?.status ?? 1,
      queueName: req?.body?.queueName ?? 'Default Queue',
      isCall: req?.body?.isCall ?? 0,
      isSms: req?.body?.isSms ?? 0,
      aliasName: req?.body?.aliasName ?? null,
      outboundSms: req?.body?.outboundSms ?? 0,
      smsThreshold: req?.body?.smsThreshold ?? 24,
      callFlowName: req?.body?.callFlowName ?? 'Default Call Flow',
      defaultUpdated: req?.body?.defaultUpdated ?? 0,
      supervisorName: req?.body?.supervisorName ?? null,
      isCallIn: req?.body?.isCallIn ?? 0,
      isCallOut: req?.body?.isCallOut ?? 0,
      isSmsOut: req?.body?.isSmsOut ?? 0,
      isSmsIn: req?.body?.isSmsIn ?? 0,
      type: req?.body?.type ?? null,
    };
    const insertdidNumber: any = await insertOnboardDidNumber(data);
    logger.info('insertOnboardDidNumber', insertdidNumber);
    if (insertdidNumber[0]?.errcode === 0) {
      const callDetails = {
        your_number: data.your_number,
        domainId: data.domainId,
        callFlowId: process.env.DEFAULT_CALLFLOW_ID,
      };
      await updateCallFlowMap1(callDetails);
      res
        .status(200)
        .send({ statusCode: 200, message: RESPONSE.did_purchase_insert });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {
    logger.error('insertOnboardDidNumberHandler - Unexpected Error:', req?.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function loginSessionHistoryHandler(
  req: any,
  res: any,
  done: any
) {
  try {
    const dataS: any = {
      logInSessionId: req?.body?.logInSessionId,
      logInTimestamp: req?.body?.logInTimestamp,
      domainId: req?.headers?.domainId,
      sipLoginId: req?.headers?.sipLoginId,
      companyId: req?.headers?.companyId,
      ext: req?.headers?.ext,
    };
    const sessionHistoryRes: any = await loginSessionHistory(dataS);
    logger.info('sessionHistoryRes', sessionHistoryRes);
    if (sessionHistoryRes) {
      res
        .status(200)
        .send({ statusCode: 200, message: RESPONSE.insert_session_history });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {
    logger.error('loginSessionHistoryHandler - Unexpected Error:', req?.body, req?.headers, err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}
// getSessionDetailsHandler;

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getSessionDetailsHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {
      domainId: auth.domainId,
      sessionId: req.params.sessionId,
      // ext: auth.ext
    };
    const getSession: any = await getSessionDetails(data);
    logger.info('getSessionDetails', getSession);
    if (getSession && Array.isArray(getSession) && getSession[0]?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getSessionDetailsRes: getSession[0],
      });
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        getSessionDetailsRes: [],
      });
    }
  } catch (err) {
    logger.error('getSessionDetailsHandler - Unexpected Error:', req.headers, req.params, err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}
/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function updateSessionHistoryHandler(
  req: any,
  res: any,
  done: any
) {
  try {
    const data: any = {
      logInSessionId: req?.body?.logInSessionId,
      logOutTimestamp: req?.body?.logOutTimestamp,
      sipLoginId: req?.headers?.sipLoginId,
      domainId: req?.headers?.domainId,
      companyId: req?.headers?.companyId,
      ext: req?.headers?.ext,
    };
    const updateSession: any = await updateSessionHistory(data);
    logger.info('updateSessionHistory', updateSession);
    if (Array.isArray(updateSession) && updateSession[0].errcode === 0) {
      res
        .status(200)
        .send({ statusCode: 200, message: RESPONSE.update_session_history });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_update });
    }
  } catch (err) {
    logger.error('updateSessionHistoryHandler - Unexpected Error:', req.params, req?.headers, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getCcaasRoleInfoHandler(req: any, res: any, done: any) {
  try {
    const loginData: any = {};
    const reqBody = req.body;
    loginData.login_user_name = reqBody.username.toLowerCase();
    loginData.login_password = reqBody.password;
    loginData.login_device_id = reqBody.deviceId;
    loginData.login_source = reqBody.source;
    loginData.login_ipaddress = reqBody.ipAddress;
    loginData.is_force_login = reqBody.isForceLogin;
    const ccaasRoleId: any = await getCcaasRoleInfo(loginData);
    logger.info('getCcaasRoleInfo', ccaasRoleId);
    if (Array.isArray(ccaasRoleId) && ccaasRoleId.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getCCaasRoleIdInfoRes: ccaasRoleId,
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {
    logger.error('getCcaasRoleInfoHandler - Unexpected Error:', req.body, err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getUserDetailsEmailHandler(
  req: any,
  res: any,
  done: any
) {
  try {
    const data: any = {
      emailId: req?.headers?.username,
      domainId: req?.headers?.domainId,
    };
    const getList: any = await getUserDetailsEmail(data);
    logger.info('getUserDetailsEmail', getList);
    if (Array.isArray(getList) && getList[0]?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getUserDetailsRes: getList[0],
      });
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        getUserDetailsRes: [],
      });
    }
  } catch (err) {
    logger.error('getUserDetailsEmailHandler - Unexpected Error:', req?.headers, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function updateOnboardUserHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      domainId: req?.headers?.domainId,
      sipLoginId: req?.headers?.sipLoginId,
      companyId: req?.headers?.companyId,
      ...req.body,
    };
    const updateuser: any = await updateOnboardUser(data);
    logger.info('updateOnboardUser', updateuser);
    if (updateuser[0]?.errcode === 0) {
      res
        .status(200)
        .send({ statusCode: 200, message: RESPONSE.User_update_success });
    } else {
      res.status(200).send({ statusCode: 200, message: RESPONSE.not_update });
    }
  } catch (err) {
    logger.error('updateOnboardUserHandler - Unexpected Error:', req?.headers, req?.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function getUserIdUserExtDomain(req: any, res: any) {
  try {
    const data: any = {
      domain_id: req?.body?.domain_id,
      ext: req?.body?.ext,
    };
    const output: any = await getUserIdUser(data);
    logger.info('getUserIdUser', output);
    if (output) {
      res.status(200).send({ statusCode: 200, message: output });
    }
  } catch (err) {
    logger.error('getUserIdUserExtDomain - Unexpected Error:', req?.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getLoginTokenHandler(req: any, res: any, done: any) {
  try {
    const data: any = req.body;
    const getLoginDetails: any = await getLoginToken(data);
    logger.info('getLoginToken', getLoginDetails);
    res.send({
      status_code: 200,
      message: RESPONSE.success_message,
      getCCaasRoleIdInfoRes: getLoginDetails,
    });
  } catch (err) {
    logger.error('getLoginTokenHandler - Unexpected Error:', req.body, err);
    res.send({
      status_code: 500,
      err_code: -1,
      affected_rows: 0,
      message: RESPONSE.internal_error,
    });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function updateTicketingIdHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      domainId: req?.headers?.domainId,
      ticketId: req?.body?.ticketId,
      historyId: req?.body?.historyId,
      channelType: req?.body?.channelType,
      ext: req?.headers?.ext,
    };
    await updateTicketingId(data);
    res.status(200).send({ statusCode: 200, message: RESPONSE.ticket_success });
  } catch (err) {
    logger.error('updateTicketingIdHandler - Unexpected Error:', req?.headers, req?.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 * Insert Kevin ChatBot
 */

export async function setDefaultKevinChatBot(req: any, res: any, done: any) {
  try {
    getAgentStatuses(req).forEach(async (list: any) => insertAgentStatus(list));
    getDispositionData(req).forEach(async (list: any) =>
      insertDispositionStatus(list)
    );
    getCustomerTag(req).forEach(async (list: any) => insertCustomerTag(list));
    getBasicSkill(req).forEach(async (list: any) => inserSkill(list));
    getPlayPrompt(req).forEach(async (list: any) => basicInsertPrompt(list));
    // basicInsertPrompt(getPlayPrompt(req));
    const businessHours = getBusinessHours(req);
    await insertBusinessHours(businessHours.outOfBusiness).then(
      (result1: any) => {
        const cid = result1?.[0]?.bid;
        const outOfBusinessHours = { ...businessHours.outOfBusiness, cid };
        const updateBusinessHours = {
          ...businessHours,
          outOfBusiness: outOfBusinessHours,
        };
        insertQueue(getQueueData(req, updateBusinessHours, cid)).then(
          async (result2: any) => {
            const qid = result2?.[0]?.qid;
            insertRouting(getRoutingProfileData(req, qid));
            const imagesUrl: any = {};
            imagesUrl.imageUrl = [
              'https://ccaas-storage.worktual.co.uk/chat/botoneimages.svg',
              'https://ccaas-storage.worktual.co.uk/chat/6704/bot2.svg',
              'https://ccaas-storage.worktual.co.uk/chat/6704/bot3.svg',
              'https://ccaas-storage.worktual.co.uk/chat/6704/bot4.svg',
              'https://ccaas-storage.worktual.co.uk/chat/6704/bot5.svg',
              'https://ccaas-storage.worktual.co.uk/chat/6704/bot6.svg',
            ];
            imagesUrl.backgroundImage = [
              'https://ccaas-storage.worktual.co.uk/chat/10592/bgg1.png',
              'https://ccaas-storage.worktual.co.uk/chat/10592/bgg2.png',
              'https://ccaas-storage.worktual.co.uk/chat/10592/bgg3.png',
              'https://ccaas-storage.worktual.co.uk/chat/10592/bgg4.png',
              'https://ccaas-storage.worktual.co.uk/chat/10592/bgg5.png',
              'https://ccaas-storage.worktual.co.uk/chat/10592/bgg6.png',
              'https://ccaas-storage.worktual.co.uk/chat/10592/bgg7.png',
            ];
            const data: any = { domainId: req.body.domainId };
            await insertBotImagesDao(data, imagesUrl);
            const nodes = getKevinChatflowNodes(updateBusinessHours, qid);
            const edges = getKevinChatflowEdges();
            const callflowData = getDefaultChatflowData(req);
            const nodeData: any = {
              nodes: JSON.stringify(nodes),
              edges: JSON.stringify(edges),
            };
            insertBasicChatFlow(callflowData, nodeData).then((__: any) => {
              res
                .status(200)
                .send({ statusCode: 200, message: RESPONSE.insert_success });
            });
          }
        );
      }
    );
  } catch (err) {
    logger.error('setDefaultKevinChatBot - Unexpected Error:', req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param response
 */
// this has no api end points
export const insertDefaultGlobalShortcutKeys = async (response: any) => {
  try {
    const data: any = [
      {
        uuid: uuidv4(),
        function: 'Global Search',
        keyValues: {
          key1: 'Ctrl',
          key2: 'S',
        },
        status: 1,
      },
      {
        uuid: uuidv4(),
        function: 'Go To Agent Tracker',
        keyValues: {
          key1: 'Alt',
          key2: 'T',
        },
        status: 1,
      },
      {
        uuid: uuidv4(),
        function: 'Decline Incoming call',
        keyValues: {
          key1: 'Shift',
          key2: 'D',
        },
        status: 1,
      },
      {
        uuid: uuidv4(),
        function: 'Mute and Unmute the call',
        keyValues: {
          key1: 'Ctrl',
          key2: 'M',
        },
        status: 1,
      },
      {
        uuid: uuidv4(),
        function: 'Ready State',
        keyValues: {
          key1: 'Alt',
          key2: 'R',
        },
        status: 1,
      },
      {
        uuid: uuidv4(),
        function: 'Transfer Only',
        keyValues: {
          key1: 'Shift',
          key2: 'T',
        },
        status: 1,
      },
      {
        uuid: uuidv4(),
        function: 'Offline',
        keyValues: {
          key1: 'Alt',
          key2: 'O',
        },
        status: 1,
      },
      {
        uuid: uuidv4(),
        function: 'Create New Customer',
        keyValues: {
          key1: 'Alt',
          key2: 'C',
        },
        status: 1,
      },
      {
        uuid: uuidv4(),
        function: 'Answer Incoming call',
        keyValues: {
          key1: 'Shift',
          key2: 'A',
        },
        status: 1,
      },
      {
        uuid: uuidv4(),
        function: 'Hang-up Call',
        keyValues: {
          key1: 'Shift',
          key2: 'H',
        },
        status: 1,
      },
      {
        uuid: uuidv4(),
        function: 'Logout',
        keyValues: {
          key1: 'Alt',
          key2: 'Q',
        },
        status: 1,
      },
      {
        uuid: uuidv4(),
        function: 'Not Ready',
        keyValues: {
          key1: 'Alt',
          key2: 'N',
        },
        status: 1,
      },
      {
        uuid: uuidv4(),
        function: 'Away',
        keyValues: {
          key1: 'Alt',
          key2: 'A',
        },
        status: 1,
      },
    ];
    await defaultGlobalShortcutKeys(data, response);
  } catch (err) {
    logger.error('insertDefaultGlobalShortcutKeys', err);
  }
};

// this has no api end points
export async function insertDefaultShortcutKeys(response: any) {
  try {
    const data: any = [
      {
        uuid: uuidv4(),
        function: 'My Activity tab',
        shortcutKey: 'Alt + M + A',
        keyValues: {
          key1: 'Alt',
          key2: 'M',
          key3: 'A',
        },
        defaultKeys: {
          key1: 'Alt',
          key2: 'M',
          key3: 'A',
        },
        status: 1,
      },
      {
        uuid: uuidv4(),
        function: 'All Activity tab',
        shortcutKey: 'Alt + A+ A',
        keyValues: {
          key1: 'Alt',
          key2: 'A',
          key3: 'A',
        },
        defaultKeys: {
          key1: 'Alt',
          key2: 'A',
          key3: 'A',
        },
        status: 1,
      },
      {
        uuid: uuidv4(),
        function: 'Contacts',
        shortcutKey: 'Alt + A + C',
        keyValues: {
          key1: 'Alt',
          key2: 'A',
          key3: 'C',
        },
        defaultKeys: {
          key1: 'Alt',
          key2: 'A',
          key3: 'C',
        },
        status: 1,
      },
      {
        uuid: uuidv4(),
        function: 'Hold the call',
        shortcutKey: 'Alt + H',
        keyValues: {
          key1: 'Alt',
          key2: 'H',
        },
        defaultKeys: {
          key1: 'Alt',
          key2: 'H',
        },
        status: 1,
      },
      {
        uuid: uuidv4(),
        function: 'Transfer the call',
        shortcutKey: 'Shift + R',
        keyValues: {
          key1: 'Shift',
          key2: 'R',
        },
        defaultKeys: {
          key1: 'Shift',
          key2: 'R',
        },
        status: 1,
      },
      {
        uuid: uuidv4(),
        function: 'Callback',
        shortcutKey: 'Ctrl + B',
        keyValues: {
          key1: 'Ctrl',
          key2: 'B',
        },
        defaultKeys: {
          key1: 'Ctrl',
          key2: 'B',
        },
        status: 1,
      },
      {
        uuid: uuidv4(),
        function: 'Leave Conference Call',
        shortcutKey: 'Alt + L',
        keyValues: {
          key1: 'Alt',
          key2: 'L',
        },
        defaultKeys: {
          key1: 'Alt',
          key2: 'L',
        },
        status: 1,
      },
      {
        uuid: uuidv4(),
        function: 'Custom Queue',
        shortcutKey: 'Ctrl + Q',
        keyValues: {
          key1: 'Ctrl',
          key2: 'Q',
        },
        defaultKeys: {
          key1: 'Ctrl',
          key2: 'Q',
        },
        status: 1,
      },
      {
        uuid: uuidv4(),
        function: 'Create Custom Queue',
        shortcutKey: 'Ctrl + Q +C',
        keyValues: {
          key1: 'Ctrl',
          key2: 'Q',
          key3: 'C',
        },
        defaultKeys: {
          key1: 'Ctrl',
          key2: 'Q',
          key3: 'C',
        },
        status: 1,
      },
      {
        uuid: uuidv4(),
        function: 'Prompt Library',
        shortcutKey: 'Alt + P',
        keyValues: {
          key1: 'Alt',
          key2: 'P',
        },
        defaultKeys: {
          key1: 'Alt',
          key2: 'P',
        },
        status: 1,
      },
      {
        uuid: uuidv4(),
        function: 'Create Prompt Library',
        shortcutKey: 'Alt + P +C',
        keyValues: {
          key1: 'Alt',
          key2: 'P',
          key3: 'C',
        },
        defaultKeys: {
          key1: 'Alt',
          key2: 'P',
          key3: 'C',
        },
        status: 1,
      },
      {
        uuid: uuidv4(),
        function: 'Skill Management',
        shortcutKey: 'Alt + M',
        keyValues: {
          key1: 'Alt',
          key2: 'M',
        },
        defaultKeys: {
          key1: 'Alt',
          key2: 'M',
        },
        status: 1,
      },
      {
        uuid: uuidv4(),
        function: 'Create Skill Management',
        shortcutKey: 'Alt + M +C',
        keyValues: {
          key1: 'Alt',
          key2: 'M',
          key3: 'C',
        },
        defaultKeys: {
          key1: 'Alt',
          key2: 'M',
          key3: 'C',
        },
        status: 1,
      },
      {
        uuid: uuidv4(),
        function: 'Business Hours',
        shortcutKey: 'Alt + B',
        keyValues: {
          key1: 'Alt',
          key2: 'B',
        },
        defaultKeys: {
          key1: 'Alt',
          key2: 'B',
        },
        status: 1,
      },
      {
        uuid: uuidv4(),
        function: 'To Create Business Hours',
        shortcutKey: 'Alt + B + C',
        keyValues: {
          key1: 'Alt',
          key2: 'B',
          key3: 'C',
        },
        defaultKeys: {
          key1: 'Alt',
          key2: 'B',
          key3: 'C',
        },
        status: 1,
      },
      {
        uuid: uuidv4(),
        function: 'Routing Profile',
        shortcutKey: 'Alt + U',
        keyValues: {
          key1: 'Alt',
          key2: 'U',
        },
        defaultKeys: {
          key1: 'Alt',
          key2: 'U',
        },
        status: 1,
      },
      {
        uuid: uuidv4(),
        function: 'To Create Routing Profile',
        shortcutKey: 'Alt + U + C',
        keyValues: {
          key1: 'Alt',
          key2: 'U',
          key3: 'C',
        },
        defaultKeys: {
          key1: 'Alt',
          key2: 'U',
          key3: 'C',
        },
        status: 1,
      },
      {
        uuid: uuidv4(),
        function: 'Webchat Configuration',
        shortcutKey: 'Alt + W',
        keyValues: {
          key1: 'Alt',
          key2: 'W',
        },
        defaultKeys: {
          key1: 'Alt',
          key2: 'W',
        },
        status: 1,
      },
      {
        uuid: uuidv4(),
        function: 'To Create Chat widget',
        shortcutKey: 'Alt + W + C',
        keyValues: {
          key1: 'Alt',
          key2: 'W',
          key3: 'C',
        },
        defaultKeys: {
          key1: 'Alt',
          key2: 'W',
          key3: 'C',
        },
        status: 1,
      },
      {
        uuid: uuidv4(),
        function: 'Agent Status',
        shortcutKey: 'Shift + S',
        keyValues: {
          key1: 'Shift',
          key2: 'S',
        },
        defaultKeys: {
          key1: 'Shift',
          key2: 'S',
        },
        status: 1,
      },
      {
        uuid: uuidv4(),
        function: 'To Create Agent Status',
        shortcutKey: 'Shift + S + C',
        keyValues: {
          key1: 'Shift',
          key2: 'S',
          key3: 'C',
        },
        defaultKeys: {
          key1: 'Shift',
          key2: 'S',
          key3: 'C',
        },
        status: 1,
      },
      {
        uuid: uuidv4(),
        function: 'Disposition Status',
        shortcutKey: 'Shift + D',
        keyValues: {
          key1: 'Shift',
          key2: 'D',
        },
        defaultKeys: {
          key1: 'Shift',
          key2: 'D',
        },
        status: 1,
      },
      {
        uuid: uuidv4(),
        function: 'To Create Disposition Status',
        shortcutKey: 'Shift + D + C',
        keyValues: {
          key1: 'Shift',
          key2: 'D',
          key3: 'C',
        },
        defaultKeys: {
          key1: 'Shift',
          key2: 'D',
          key3: 'C',
        },
        status: 1,
      },
      {
        uuid: uuidv4(),
        function: 'Customer Type',
        shortcutKey: 'Shift + T + T',
        keyValues: {
          key1: 'Shift',
          key2: 'T',
          key3: 'T',
        },
        defaultKeys: {
          key1: 'Shift',
          key2: 'T',
          key3: 'T',
        },
        status: 1,
      },
      {
        uuid: uuidv4(),
        function: 'To Create Customer Type',
        shortcutKey: 'Shift + T + C',
        keyValues: {
          key1: 'Shift',
          key2: 'T',
          key3: 'C',
        },
        defaultKeys: {
          key1: 'Shift',
          key2: 'T',
          key3: 'C',
        },
        status: 1,
      },
      {
        uuid: uuidv4(),
        function: 'Analytics',
        shortcutKey: 'Shift  + Alt + A',
        keyValues: {
          key1: 'Shift',
          key2: 'Alt',
          key3: 'A',
        },
        defaultKeys: {
          key1: 'Shift',
          key2: 'Alt',
          key3: 'A',
        },
        status: 1,
      },
      {
        uuid: uuidv4(),
        function: 'Dashboard',
        shortcutKey: 'Shift  + Alt + D',
        keyValues: {
          key1: 'Shift',
          key2: 'Alt',
          key3: 'D',
        },
        defaultKeys: {
          key1: 'Shift',
          key2: 'Alt',
          key3: 'D',
        },
        status: 1,
      },
      {
        uuid: uuidv4(),
        function: 'Reports',
        shortcutKey: 'Shift  + Alt + R',
        keyValues: {
          key1: 'Shift',
          key2: 'Alt',
          key3: 'R',
        },
        defaultKeys: {
          key1: 'Shift',
          key2: 'Alt',
          key3: 'R',
        },
        status: 1,
      },
      {
        uuid: uuidv4(),
        function: 'Bell Notification',
        shortcutKey: 'Shift + B',
        keyValues: {
          key1: 'Shift',
          key2: 'B',
        },
        defaultKeys: {
          key1: 'Shift',
          key2: 'B',
        },
        status: 1,
      },
    ];

    await insertDefaultShortcutKeysDao(data, response);
  } catch (err) {
    logger.error('insertDefaultShortcutKeys - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getInteractionHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {
      domainId: auth.domainId,
      emailId: req?.body?.emailId ?? null,
      phoneNumber: req?.body?.phoneNumber ?? 0,
      customerId: req?.body?.customerId ?? null,
      customerFacebookId: req?.body?.customerFacebookId ?? null,
      customerInstagramId: req?.body?.customerInstagramId ?? null,
    };
    const getInteractionList: any = await getInteraction(data);
    logger.info('getInteractionList', getInteractionList);
    if (Array.isArray(getInteractionList) && getInteractionList.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getInteractionResponse: getInteractionList[0],
        getDispositionName: getInteractionList[1],

      });
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        getInteractionResponse: [],
      });
    }
  } catch (err) {
    logger.error('getInteractionHandler - Unexpected Error:', req?.body, req.headers, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getInteractionViewHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {
      domainId: auth.domainId,
      emailId: req?.body?.emailId ?? null,
      phoneNumber: req?.body?.phoneNumber ?? null,
      customerId: req?.body?.customerId ?? null,
      customerFacebookId: req?.body?.customerFacebookId ?? null,
      customerInstagramId: req?.body?.customerInstagramId ?? null,
      interactionId: req?.body?.interactionId ?? null,
      TicketId: req?.body?.TicketId ?? null
    };
    const getInteractionListView: any = await getInteractionView(data);
    logger.info('getInteractionView', getInteractionListView);
    if (
      Array.isArray(getInteractionListView) &&
      getInteractionListView.length > 0
    ) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getInteractionViewResponse: getInteractionListView[0],
        getInteractionViewTicketingResponse: getInteractionListView[1],

      });
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        getInteractionViewResponse: [],
      });
    }
  } catch (err) {
    logger.error('getInteractionViewHandler - Unexpected Error:', req?.body, req.headers, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function deleteOnboardUserHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      domainId: req?.body?.domainId,
      ext: req?.body?.ext,
    };
    const onboardUser: any = await deleteOnboardUser(data);
    logger.info('deleteOnboardUser', onboardUser);
    if (onboardUser[0].errcode === 0) {
      ioredisWhatsapp
        .to(`${data?.domainId}_${data?.ext}`)
        .emit('deleteUserLogout');
      res.status(200).send({ statusCode: 200, message: RESPONSE.delete_user });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_deleted });
    }
  } catch (err) {
    logger.error('deleteOnboardUserHandler - Unexpected Error:', req?.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function deleteTemporaryOnboardUserHandler(
  req: any,
  res: any,
  done: any
) {
  try {
    const data: any = {
      domainId: req?.body?.domainId,
      ext: req?.body?.ext,
      status: req?.body?.status
    };
    const values = data?.ext?.split(',');
    values.forEach(async (ext: any) => {
      await deleteUserActiveStatus(data, ext);
      ioredisWhatsapp.to(`${data?.domainId}_${ext}`).emit('deleteUserLogout');
    });
    res
      .status(200)
      .send({ statusCode: 200, message: RESPONSE.success_message });
  } catch (err) {
    logger.error('deleteTemporaryOnboardUserHandler - Unexpected Error:', req?.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function updateOnboardActiveStatusHandler(
  req: any,
  res: any,
  done: any
) {
  try {
    const data: any = {
      isActive: req?.body?.isActive,
      domainId: req?.body?.domainId,
      emailId: req?.body?.emailId ?? null,
      ext: req?.body?.ext,
    };
    const onboardUser: any = await updateOnboardActiveStatus(data);
    logger.info('updateOnboardActiveStatus', onboardUser);
    if (onboardUser[0].errcode === 0) {
      ioredisWhatsapp
        .to(`${data?.domainId}_${data?.ext}`)
        .emit('deleteUserLogout');
      res
        .status(200)
        .send({ statusCode: 200, message: onboardUser[0]?.errmsg });
    } else {
      res
        .status(200)
        .send({ statusCode: 404, message: onboardUser[0]?.errmsg });
    }
  } catch (err) {
    logger.error('updateOnboardActiveStatusHandler - Unexpected Error:', req?.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */

export async function UpdateChatFlag(req: any, res: any, done: any) {
  try {
    const data: any = req?.headers;
    await UpdateChatFlagDao(data);
    res.status(200).send({ statusCode: 200, message: 'Update successfully' });
  } catch (err) {
    logger.error('UpdateChatFlag - Unexpected Error:', req?.headers, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 */
export const getCallBackOptionHandler = async (req: any, res: any) => {
  try {
    const data: any = {
      domainId: req?.headers?.domainId,
    };
    const getCallBack: any = await getCallBackOption(data);
    logger.info('getCallBackOption', getCallBack);
    if (getCallBack && getCallBack.length > 0) {
      res
        .status(200)
        .send({
          statusCode: 200,
          message: RESPONSE.success_message,
          getCallBackOptionRes: getCallBack,
        });
    } else {
      res
        .status(200)
        .send({
          statusCode: 404,
          message: RESPONSE.not_found,
          getCallBackOptionRes: [],
        });
    }
  } catch (error) {
    logger.error('getCallBackOptionHandler - Unexpected Error:', req?.headers, error);
    req.log.error(error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
};

/**
 *
 * @param req
 * @param res
 */
export const getApiUniqueHandler = async (req: any, res: any) => {
  try {
    res
      .status(200)
      .send({ statusCode: 200, message: RESPONSE.success_message });
  } catch (error) {
    logger.error('getApiUniqueHandler - Unexpected Error:', error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
};

/**
 *
 * @param req
 * @param res
 * @param done
 * Update Profile api
 */
export async function updateProfilePicHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      domainId: auth.domainId,
      profileImage: req?.body?.profileImage ?? null,
      ext: auth.ext,
    };
    const insertResult: any = await updateProfilePicDao(data);
    logger.info('updateProfilePicDao', insertResult);
    if (insertResult && insertResult.length > 0) {
      res.status(200).send({ statusCode: 200, insertResult: insertResult[0] });
    } else {
      res.status(404).send({ statusCode: 404, result: [] });
    }
  } catch (err) {
    logger.error('getApiUniqueHandler - Unexpected Error:', req?.body, req.headers, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * Update Profile api
 */
export async function updateProfileNameHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      domainId: auth.domainId,
      firstName: req?.body?.firstName ?? null,
      lastName: req?.body?.lastName ?? null,
      ext: auth.ext,
    };
    const insertResult: any = await updateProfileNameDao(data);
    logger.info('updateProfileNameDao', insertResult);
    if (insertResult && insertResult.length > 0) {
      res.status(200).send({ statusCode: 200, insertResult: insertResult[0] });
    } else {
      res.status(404).send({ statusCode: 404, result: [] });
    }
  } catch (err) {
    logger.error('updateProfileNameHandler - Unexpected Error:', req.headers, req?.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * get profile api
 */
export async function getProfileImgHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {};
    data.domainId = auth.domainId;
    data.ext = auth.ext;
    const results: any = await getProfileDao(data);
    logger.info('getProfileDao', results);
    if (Array.isArray(results) && results.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, result: results[0] });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    logger.error('getProfileImgHandler - Unexpected Error:', req.headers, err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * generate otp api
 */
export async function otpGeneratorHandler(req: any, res: any, done: any) {
  try {
    let res1: any;
    const emailRegex = /^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$/;
    const isEmail = emailRegex.test(req.body?.toId);
    const payload = {
      domainId: req.body.domainId,
      toId: req.body.toId,
      expiryTime: req.body.expiryTime,
    };
    const response = await otpGeneratorDao(payload);
    logger.info('otpGeneratorDao>>>>>>>>>', response);
    if (response[0]?.errcode === 0 && Array.isArray(response) && response.length > 0) {
      const expiryTime = Math.floor(req.body.expiryTime / 60);
      const data = {
        emailSubject: `${req.body.businessName} - OTP generated.`,
        htmlContent: `Use ${response[0]?.verify_code} as one time password(OTP) for ${req.body.businessName} service. Do not share this OTP to anyone for security reasons. The OTP will be valid for ${expiryTime} ${expiryTime > 1 ? 'minutes' : 'minute'}.`,
        toId: req.body.toId,
        fromNum: '447441428907'
      };
      if (isEmail) {
        res1 = await sendEmailWithOtp(data);
      } else if (Number(req.body.toId)) {
        res1 = await sendSmsMessageOutbound(data);
      }
      if (res1) {
        res.status(200).send({ statusCode: 200, message: RESPONSE.otp_process_success, result: response[0] });
      } else {
        res.status(200).send({ statusCode: 400, message: RESPONSE.otp_process_failed, result: response[0] });
      }
    } else {
      res.status(200).send({ statusCode: 400, message: RESPONSE.otp_process_failed, result: response });
    }
  } catch (err) {
    logger.error('otpGeneratorHandler - Unexpected Error:', req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.otp_process_failed });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * validate otp api
 */
export async function otpValidatorHandler(req: any, res: any, done: any) {
  try {
    const payload = {
      domainId: req.body.domainId,
      toId: req.body.toId,
      verificationCode: req.body.verificationCode,
    };
    const otpValidationResponse = await otpValidatorDao(payload);
    logger.info('otpValidatorDao', otpValidationResponse);
    if (otpValidationResponse[0]?.errcode === 0 && Array.isArray(otpValidationResponse) && otpValidationResponse.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, result: otpValidationResponse[0] });
    } else {
      res.status(200).send({ statusCode: 400, message: RESPONSE.otp_verification_failed, result: otpValidationResponse[0] });
    }

  } catch (err) {
    logger.error('otpValidatorHandler - Unexpected Error:', req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.otp_process_failed });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * get profile api
 */
export async function getAddonLanguagesHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const payload = {
      domainId: auth.domainId,
    };
    const results: any = await getAddonLanguagesDao(payload);
    logger.info('getAddonLanguagesDao', results);
    if (Array.isArray(results) && results.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, result: results });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    logger.error('getAddonLanguagesHandler - Unexpected Error:', req.headers, err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * get profile api
 */
export async function getexpiredplandetailsHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const payload = {
      domainId: auth.domainId,
    };
    const results: any = await getexpiredplandetailsDao(payload);
    logger.info('getexpiredplandetailsDao', results);
    if (Array.isArray(results) && results.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, result: results });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    logger.error('getexpiredplandetailsHandler - Unexpected Error:', req.headers, err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function verifyCaptchaHandler(req: any, res: any) {
  try {
    const response = await axios.post(
      `https://www.google.com/recaptcha/api/siteverify?secret=${process.env.VERIFY_CAPTCHA_SECRETKEY}&response=${req.body.token}`
    );

    if (response.data) {
      res.status(200).send({ statusCode: 200, message: RESPONSE?.success_message, captchaResponse: response.data });
    } else {
      res.status(200).send({ statusCode: 404, message: 'failed' });
    }
  } catch (err) {
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getInteractionLogDetailsHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {
      domainId: auth.domainId,
      offset: req?.body?.offset ,
      limit: req?.body?.limit ,
      isQualifiedUser: req?.body?.isQualifiedUser,
      AgentId : req?.body?.userId ?? null,
      fromDate : req?.body?.fromDate ?? null,
      toDate : req?.body?.toDate ?? null,
      agentName : req?.body?.agentName ?? null,
      dispositionName : req?.body?.dispositionName ?? null,
      customerName : req?.body?.customerName ?? null,
      isBot : req?.body?.isBot ?? null,
      isInbound : req?.body?.isInbound ?? null,
      isMissed : req?.body?.isMissed ?? null,
      isAbonded : req?.body?.isAbonded ?? null,
      channelName : req?.body?.channelName ?? null,
    };
    console.log('getInteractionLogDetailsHandler Payload ::', data);

    logger.info('getInteractionLogDetailsHandler Payload ::', data);
    const getInteractionLogDetails: any = await getInteractionLogDetailsDao(data);
    if (
      Array.isArray(getInteractionLogDetails) &&
      getInteractionLogDetails.length > 0
    ) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getInteractionViewResponse: getInteractionLogDetails[0],

      });
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        getInteractionViewResponse: [],
      });
    }
  } catch (err) {
    logger.error('getInteractionViewHandler - Unexpected Error:', req?.body, req.headers, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getParticularInteractionLogDetailsHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {
      domainId: auth.domainId,
      coid:req?.body?.coid,
      offset: req?.body?.offset ,
      limit: req?.body?.limit ,
      userId : req?.body?.userId ?? null
    };
    const getInteractionLogDetails: any = await getParticularInteractionLogDetailsDao(data);
    logger.info('getInteractionView', getInteractionLogDetails);
    if (
      Array.isArray(getInteractionLogDetails) &&
      getInteractionLogDetails.length > 0
    ) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getInteractionViewResponse: getInteractionLogDetails[0],

      });
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        getInteractionViewResponse: [],
      });
    }
  } catch (err) {
    logger.error('getInteractionViewHandler - Unexpected Error:', req?.body, req.headers, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getOverallCallbackInteractionDetailsHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {
      domainId: auth.domainId,
      offset: req?.body?.offset ,
      limit: req?.body?.limit ,

    };
    const getInteractionLogDetails: any = await getOverallCallbackInteractionDetailsDao(data);
    console.log('getInteractionView--->>>>', getInteractionLogDetails);
    logger.info('getInteractionView', getInteractionLogDetails);
    if (
      Array.isArray(getInteractionLogDetails) &&
      getInteractionLogDetails.length > 0
    ) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getInteractionViewResponse: getInteractionLogDetails[0],

      });
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        getInteractionViewResponse: [],
      });
    }
  } catch (err) {
    logger.error('getInteractionViewHandler - Unexpected Error:', req?.body, req.headers, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * get profile api
 */
export async function aiccasGetConfigDetailsHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const payload = {
      domainId: auth.domainId,
    };
    const results: any = await aiccasGetConfigDetailsDao(payload);
    logger.info('getexpiredplandetailsDao', results);
    if (Array.isArray(results) && results.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, result: results });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    logger.error('aiccasGetConfigDetailsHandler - Unexpected Error:', req.headers, err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getOverallCallbackGroupInteractionDetailsHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {
      domainId: auth.domainId,
      isMissed:req?.body?.isMissed,
      isAbandoned:req?.body?.isAbandoned,
      offset: req?.body?.offset ,
      limit: req?.body?.limit ,
      channelName: req?.body?.channelName,
      customerName:req?.body?.customerName,
      QueueName:req?.body?.QueueName,
      fromDate:req?.body?.fromDate,
      toDate:req?.body?.toDate,
      agentName:req?.body?.agentName
    };
    console.log('object123', data);
    const result: any = await getOverallCallbackGroupInteractionDetailsDao(data);

    if (Array.isArray(result) && result[0]?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getInteractionViewResponse: result[0],
        totalCount: result[1]?.[0]?.TotalCount || 0,
        isMissed: result[1]?.[0]?.IsMissed || 0,
        isAbandoned: result[1]?.[0]?.IsAbandoned || 0,
      });
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        getInteractionViewResponse: [],
        totalCount: 0,
        isMissed: 0,
        isAbandoned: 0,
      });
    }
  } catch (err) {
    logger.error('getOverallCallbackGroupInteractionDetailsHandler - Unexpected Error:', req?.body, req.headers, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getOverallCallbackGroupInteractionPerticularDetailsHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {
      domainId: auth.domainId,
      coid: req?.body?.coid,
      isMissed: req?.body?.isMissed ?? null,
      isAbandoned: req?.body?.isAbandoned ?? null,
      offset: req?.body?.offset ,
      limit: req?.body?.limit ,

    };

    const getInteractionLogDetails: any = await getOverallCallbackGroupInteractionPerticularDetailsDao(data);
    if (Array.isArray(getInteractionLogDetails) && getInteractionLogDetails.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getInteractionViewResponse: getInteractionLogDetails[0],
      });
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        getInteractionViewResponse: [],
      });
    }
  } catch (err) {
    logger.error('getOverallCallbackGroupInteractionPerticularDetailsHandler - Unexpected Error:', req?.body, req.headers, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getSocialMediHistoryDetailsHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      historyId: req?.body?.historyId ?? null,
      channelType: req?.body?.channelType ?? null,
    };

    const getHistoryDetails: any = await getSocialMediHistoryDetailsHandlerDao(data);
    if (Array.isArray(getHistoryDetails) && getHistoryDetails.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        response: getHistoryDetails[0],
      });
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        response: [],
      });
    }
  } catch (err) {
    logger.error('getSocialMediHistoryDetailsHandler - Unexpected Error:', req?.body, req.headers, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getSocialMediaMessageByIdHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      historyId: req?.body?.historyId ?? null,
      channelType: req?.body?.channelType ?? null,
    };

    const getHistoryMessageDetails: any = await getSocialMediaMessageByIdDao(data);
    if (Array.isArray(getHistoryMessageDetails) && getHistoryMessageDetails.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        response: getHistoryMessageDetails[0],
      });
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        response: [],
      });
    }
  } catch (err) {
    logger.error('getSocialMediHistoryDetailsHandler - Unexpected Error:', req?.body, req.headers, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}
