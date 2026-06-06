/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

import { SESSIONHISTORY, USER } from '../../helpers/constants';
import {
  aiccasGetConfigDetailsHandler,
  deleteOnboardUserHandler,
  deleteTemporaryOnboardUserHandler,
  forgotPasswordHandler,
  getAddonLanguagesHandler,
  getAgentActivity,
  getAgentSupervisiorActivity,
  getAllProfileInfoHandler,
  getAllUserListHandler,
  getApiUniqueHandler,
  getCallBackOptionHandler,
  getCcaasRoleInfoHandler,
  getexpiredplandetailsHandler,
  getInteractionHandler,
  getInteractionLogDetailsHandler,
  getInteractionViewHandler,
  getLoginTokenHandler,
  getOverallCallbackGroupInteractionDetailsHandler,
  getOverallCallbackGroupInteractionPerticularDetailsHandler,
  getOverallCallbackInteractionDetailsHandler,
  getParticularInteractionLogDetailsHandler,
  getProfileImgHandler,
  getSessionDetailsHandler,
  getSocialMediaMessageByIdHandler,
  getSocialMediHistoryDetailsHandler,
  getUserDeatilsHandler,
  getUserDetailsEmailHandler,
  getUserDetailsListHandler,
  getUserIdUserExtDomain,
  getUserListByRoleHandler,
  insertOnboardDidNumberHandler,
  insertOnboardUserHandler,
  loginSessionHistoryHandler,
  otpGeneratorHandler,
  otpValidatorHandler,
  resetPassLink,
  setDefaultCompanyHandler,
  setDefaultKevinChatBot,
  UpdateChatFlag,
  updateOnboardActiveStatusHandler,
  updateOnboardUserHandler,
  updateProfileNameHandler,
  updateProfilePicHandler,
  updateSessionHistoryHandler,
  updateTicketingIdHandler,
  updateUserHandler,
  userUpdateHandler,
  verifyCaptchaHandler,
} from './handler';
import {
  forgotPasswordSchema,
  generateOtpSchema,
  getCcasRoleInfoSchema,
  resetPasswordSchema,
  validateOtpSchema,
} from './schema';

export default function userhandler(server: any, options: any, next: any) {
  server.get(
    USER.GET_AGENT_ACTIVITY,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get agent activity ',
        description: 'Get agent activity ',
        tags: ['user'],
      },
    },
    getAgentActivity
  );

  server.get(
    USER.GET_AGENT_SUPERVISIOR_ACTIVITY,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get agent supervisior activity ',
        description: 'Get agent supervisior activity ',
        tags: ['user'],
      },
    },
    getAgentSupervisiorActivity
  );

  // update user
  server.put(
    USER.UPDATE,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update user',
        description: 'Get user api',
        tags: ['user'],
      },
    },
    updateUserHandler
  );

  // need to do migration
  server.post(
    USER.FORGOT_PASSWORD,
    {
      schema: {
        summary: 'Forgot Password E-mail',
        desscription: 'Forgot Password E-mail api',
        tags: ['user'],
        body: forgotPasswordSchema.body,
        response: forgotPasswordSchema.response,
      },
    },
    forgotPasswordHandler
  );

  server.post(
    USER.RESET_PASSWORD,
    {
      schema: {
        summary: 'Reset Password Link',
        desscription: 'Reset Password Link api',
        tags: ['user'],
        querystring: resetPasswordSchema.querystring,
        body: resetPasswordSchema.body,
        response: resetPasswordSchema.response,
      },
    },
    resetPassLink
  );

  // get users list by role
  server.get(
    `${USER.GET_USER_LIST_BY}/:roleId`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get user list by roleid ',
        description: 'Get user list by roleid',
        tags: ['user'],
      },
    },
    getUserListByRoleHandler
  );

  // get user details
  server.get(
    `${USER.GET_USER_DETAILS}/:report_to`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get user details',
        description: 'Get user details',
        tags: ['user'],
      },
    },
    getUserDeatilsHandler
  );

  // get all user list
  server.get(
    USER.GET_ALL_USER_LIST,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get all user user',
        description: 'Get all user user',
        tags: ['user'],
      },
    },
    getAllUserListHandler
  );

  // get Profile Information
  server.get(
    USER.GET_ALL_PROFILE_INFO,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get profile information',
        description: 'Get profile information',
        tags: ['user'],
      },
    },
    getAllProfileInfoHandler
  );

  // get user details
  server.get(
    `${USER.GET_USER_DETAILS_LIST}/:uid`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get user details list',
        description: 'Get user details list api',
        tags: ['user'],
      },
    },
    getUserDetailsListHandler
  );

  server.put(
    USER.USER_UPDATE,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update user',
        description: 'update user api',
        tags: ['user'],
      },
    },
    userUpdateHandler
  );

  // insert onboard user
  server.post(
    USER.INSERT_ONBOARD_USER,
    {
      preValidation: [],
      schema: {
        summary: 'insert onboard user',
        description: 'insert onboard user api',
        tags: ['user'],
      },
    },
    insertOnboardUserHandler
  );
  // update onboard user
  server.put(
    USER.UPDATE_ONBOARD_USER,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update onboard user',
        description: 'update onboard user api',
        tags: ['user'],
      },
    },
    updateOnboardUserHandler
  );

  // post userID with ext and domainId
  server.post(
    USER.GET_USER_ID,
    {
      // preValidation: [server.validateSession],
      schema: {
        summary: 'get userID',
        description: 'get userID',
        tags: ['user'],
      },
    },
    getUserIdUserExtDomain
  );

  // insert onboard user
  server.post(
    USER.ONBOARD_DID_NUMBER,
    {
      preValidation: [],
      schema: {
        summary: 'insert onboard did number',
        description: 'insert onboard did number api',
        tags: ['user'],
      },
    },
    insertOnboardDidNumberHandler
  );

  // insert logInSessionHistory
  server.post(
    SESSIONHISTORY.INSERT_LOGIN_SESSION_HISTORY,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Insert login session history ',
        description: 'Insert login session history api',
        tags: ['sessionHistory'],
      },
    },
    loginSessionHistoryHandler
  );
  // update logInSessionHistory
  server.put(
    SESSIONHISTORY.UPDATE_LOGIN_SESSION_HISTORY,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update login session history',
        description: 'update login session history',
        tags: ['sessionHistory'],
      },
    },
    updateSessionHistoryHandler
  );

  // get session history based on query parameters
  server.get(
    SESSIONHISTORY.GET_SESSION_DETAILS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get session details',
        description: 'get session details',
        tags: ['sessionHistory'],
      },
    },
    getSessionDetailsHandler
  );

  // get ccaas role info
  server.post(
    SESSIONHISTORY.GET_CCAAS_ROLE_INFO,
    {
      preValidation: [],
      schema: {
        summary: 'get ccaas role info',
        description: 'get ccaas role info api',
        tags: ['user'],
        body: getCcasRoleInfoSchema.body,
        response: getCcasRoleInfoSchema.response,
      },
    },
    getCcaasRoleInfoHandler
  );

  // get user details email
  server.get(
    USER.GET_USER_DETAILS_EMAIL,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get user details email',
        description: 'get user details email api',
        tags: ['user'],
      },
    },
    getUserDetailsEmailHandler
  );

  // token login
  server.post(
    USER.GET_TOKEN_LOGIN_DETAILS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'email based login',
        description: 'email based login api',
        tags: ['user'],
      },
    },
    getLoginTokenHandler
  );

  // ticketing api
  server.put(
    USER.UPDATE_TICKETING_ID,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update ticketing id',
        description: 'update ticketing id api',
        tags: ['user'],
      },
    },
    updateTicketingIdHandler
  );

  // Insert Default settings for kevin bot
  server.post(
    USER.INSERT_DEFAULT_KEVIN_DETAILS,
    {
      preValidation: [],
      schema: {
        summary: 'Insert default kevin details',
        description: 'Insert default kevin details api',
        tags: ['user'],
      },
    },
    setDefaultKevinChatBot
  );

  // get interaction for all channels
  server.post(
    USER.GET_INTERACTION_DETAILS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get interaction details',
        description: 'get interaction details api ',
        tags: ['user'],
      },
    },
    getInteractionHandler
  );

  // get interaction for all channels view
  server.post(
    USER.GET_INTERACTION_DETAILS_VIEW,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get interaction details',
        description: 'get interaction details api ',
        tags: ['user'],
      },
    },
    getInteractionViewHandler
  );

  // set default company
  server.post(
    USER.SET_DEFAULT_COMPANY,
    {
      preValidation: [],
      schema: {
        summary: 'set default basic queue and basic routing ',
        description: 'set default basic queue and basic routing  api',
        tags: ['user'],
      },
    },
    setDefaultCompanyHandler
  );

  // delete user for onboarding
  server.post(
    USER.ONBOARD_DELETE_USER,
    {
      preValidation: [],
      schema: {
        summary: 'delete onboarding user',
        description: 'delete onboarding user api',
        tags: ['user'],
      },
    },
    deleteOnboardUserHandler
  );

  // temporary delete user for onboarding
  server.post(
    USER.ONBOARD_TEMPORARY_DELETE_USER,
    {
      preValidation: [],
      schema: {
        summary: 'temporary delete onboarding user',
        description: 'temporary delete onboarding user api',
        tags: ['user'],
      },
    },
    deleteTemporaryOnboardUserHandler
  );

  // update active status
  server.put(
    USER.UPDATE_ONBOARD_USER_ACTIVE_STATUS,
    {
      preValidation: [],
      schema: {
        summary: 'update onboarding user status',
        description: 'update onboarding user status api',
        tags: ['user'],
      },
    },
    updateOnboardActiveStatusHandler
  );

  server.post(
    USER.UPDATE_ONBOARD_USER_FLAG,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update onboard user flag',
        description: 'update onboard user flag api',
        tags: ['user'],
      },
    },
    UpdateChatFlag
  );

  // get callback options
  server.get(
    USER.GET_CALLBACK_OPTIONS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get callback options',
        description: 'get callback options api',
        tags: ['user'],
      },
    },
    getCallBackOptionHandler
  );

  server.get(
    USER.GET_API_UNIQUE_DETAILS,
    {
      // preValidation: [server.validateSession],
      schema: {
        summary: 'get unique details',
        description: 'get unique details api',
        tags: ['user'],
      },
    },
    getApiUniqueHandler
  );

  // update profile picture
  server.post(
    USER.UPDATE_PROFILE_PICTURE,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update profile picture',
        description: 'update profile picture api',
        tags: ['user'],
      },
    },
    updateProfilePicHandler
  );

  // update profile name
  server.post(
    USER.UPDATE_PROFILE_NAME,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update profile name',
        description: 'update profile name api',
        tags: ['user'],
      },
    },
    updateProfileNameHandler
  );

  // get profile img
  server.get(
    USER.GET_PROFILE_IMG,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get profile status',
        description: 'get profile api',
        tags: ['user'],
      },
    },
    getProfileImgHandler
  );

  server.post(
    USER.GENERATE_OTP,
    {
      schema: {
        body: generateOtpSchema.body,
        response: generateOtpSchema.response,
        summary: 'generate otp',
        description: 'generate otp api',
        tags: [''],
      },
    },
    otpGeneratorHandler
  );

  server.post(
    USER.VALIDATE_OTP,
    {
      schema: {
        body: validateOtpSchema.body,
        response: validateOtpSchema.response,
        summary: 'generate otp',
        description: 'generate otp api',
        tags: [''],
      },
    },
    otpValidatorHandler
  );

  server.get(
    USER.GET_ADDON_LANGIAGES,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get addon languages',
        description: 'get addon languages api',
        tags: ['user'],
      },
    },
    getAddonLanguagesHandler
  );

  server.post(
    USER.VERIFY_CAPTCHA,
    {
      // preValidation: [server.validateSession],
      schema: {
        summary: 'verify captcha',
        description: 'verify captcha api',
        tags: ['user'],
      },
    },
    verifyCaptchaHandler
  );

  // get expired plan details
  server.get(
    SESSIONHISTORY.GET_EXPIRED_PLAN_DETAILS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get expired plan details',
        description: 'get expired plan details',
        tags: ['sessionHistory'],
      },
    },
    getexpiredplandetailsHandler
  );

  // get interaction for all channels view
  server.post(
    USER.GET_INTERACTION_LOG_DETAILS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get interaction log details',
        description: 'get interaction log details api ',
        tags: ['user'],
      },
    },
    getInteractionLogDetailsHandler
  );

  // get interaction for particular view
  server.post(
    USER.GET_CUSTOMER_PARTICULAR_INTERACTION_LOG,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get particular interaction log details',
        description: 'get particular interaction log details api ',
        tags: ['user'],
      },
    },
    getParticularInteractionLogDetailsHandler
  );

  // get interaction for particular view
  server.post(
    USER.GET_CUSTOMER_OVERALL_CALLBACK_INTERACTION,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get customer overall callback interaction details',
        description: 'get customer overall callback interaction details api ',
        tags: ['user'],
      },
    },
    getOverallCallbackInteractionDetailsHandler
  );

  // AiCcaas get channel config details
  server.get(
    SESSIONHISTORY.AICCAAS_GET_CHANNEL_CONFIG_DETAILS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get channel config details api',
        description: 'Get channel config details api',
        tags: ['sessionHistory'],
      },
    },
    aiccasGetConfigDetailsHandler
  );

  // get customer overall callback by group interaction details
  server.post(
    USER.GET_CUSTOMER_OVERALL_CALLBACK_INTERACTION_GROUP,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get customer overall callback by group interaction details',
        description: 'get customer overall callback by group  interaction details api ',
        tags: ['user'],
      },
    },
    getOverallCallbackGroupInteractionDetailsHandler
  );

  // get customer overall callback by group perticular interaction details
  server.post(
    USER.GET_CUSTOMER_OVERALL_CALLBACK_INTERACTION_PERTICULAR_DETAILS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get customer overall callback by group perticular interaction details',
        description: 'get customer overall callback by group perticular  interaction details api ',
        tags: ['user'],
      },
    },
    getOverallCallbackGroupInteractionPerticularDetailsHandler
  );

  // Get Socialmedia history details
  server.post(
    SESSIONHISTORY.GET_SOCIALMEDIA_HISTORY_DETAILS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get Socialmedia history details',
        description: 'Get Socialmedia history details api ',
        tags: ['sessionHistory'],
      },
    },
    getSocialMediHistoryDetailsHandler
  );

  // Get Socialmedia message by id
  server.post(
    SESSIONHISTORY.GET_SOCIALMEDIA_MESSAGE_BY_ID,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get Socialmedia message by id',
        description: 'Get Socialmedia message by id api ',
        tags: ['sessionHistory'],
      },
    },
    getSocialMediaMessageByIdHandler
  );

  next();
}
