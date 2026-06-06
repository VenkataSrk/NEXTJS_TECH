import { MYPLANS } from '../../../src/helpers/constants';
import {
  billingGetInvoiceSummaryController,
  companyUserOrderExtensionController,
  createmyaccountsyncupdetails,
  emailOtp,
  getAddonListController,
  getAllAddonListHandler,
  getAllMembersRole,
  getBillServicePlan,
  getCompanyBillingSubscription,
  getDataAndPrivacySet,
  getLiveInteraction,
  getManagement,
  getMyAccountsyncDetails,
  getMyplanRolePermission,
  getMyplanTeamLogActivity,
  getMyplanTeamPermission,
  getMyplanTeamsDetails,
  getMyplanUserActivityLogActivity,
  getMyplanUserPermission,
  getRolesInfo,
  getSecurityTwoStepVerification,
  getStorageDataHandler,
  getUserActivityLogHandler,
  getUserDataPrivacySetting,
  getUserDepartment,
  getUserDetailsByRole,
  getUserDevice,
  getUserDevicesHandler,
  getUserLoginLog,
  getUserOnboardSetting,
  getUserPasswordInfo,
  getuserroles,
  insertUpdateDataAndPrivacySet,
  insertUpdateDataPrivacySetting,
  insertUpdateMyplanRolePermission,
  insertUpdateMyplanTeamsPermission,
  insertUpdateTwoStepVerificationHandler,
  insertUserActivityLogHandler,
  insertUserDevicesHandler,
  passwordUpdate,
  resendOtp,
  updateMyplanUserPermission,
  userGetDetails,
  usersGetUserExtensionInfoController,
  verifyOtp,
} from './handler';
// import { billingGetInvoiceSummarySchema, companyUserOrderExtensionSchema, getAddonListSchema, getBillServicePlanSchema, usersGetUserExtensionInfoSchema } from './schema';

export default function MyPlans_routes(server: any, options: any, next: any) {
  // Urmyaccwebgetuserextensioninfo
  server.post(
    MYPLANS.MY_PLANS_PROFILE_INFO,
    {
      // preValidation: [server.validateSession],
      schema: {
        summary: 'My Plans Profile Info',
        description: 'My Plans Profile Info',
        tags: ['Users'],
        // body: usersGetUserExtensionInfoSchema.body,
        // response: usersGetUserExtensionInfoSchema.response
      },
    },
    usersGetUserExtensionInfoController,
  );

  // Urmyaccwebuserorderextension
  server.post(
    MYPLANS.USER_MANAGEMENT_LIST,
    {
      // preValidation: [server.validateSession,server.validateAccessToken],
      schema: {
        summary: 'Worktual MyAccount company user order extension api',
        description: 'Worktual MyAccount company user order extension',
        tags: ['Company'],
        // body: companyUserOrderExtensionSchema.body,
        // response: companyUserOrderExtensionSchema.response
      },
    },
    companyUserOrderExtensionController,
  );

  // Urbillgetcustomerinvoicesummary
  server.post(
    MYPLANS.ORDER_HIISTORY,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Worktual MyAccount get customer invoice summary api',
        description: 'Worktual MyAccount get customer invoice summary',
        tags: ['Billing'],
        // body: billingGetInvoiceSummarySchema.body,
        // response: billingGetInvoiceSummarySchema.response
      },
    },
    billingGetInvoiceSummaryController,
  );

  // wtms_bill_get_service_plan
  server.post(
    MYPLANS.SUBSCRIPTION_MANAGEMENT,
    {
      // preValidation: [server.validateSession],
      schema: {
        summary: 'get bill service plan',
        description: 'get bill service plan api',
        tags: ['Billing'],
        // body: getBillServicePlanSchema.body,
        // response: getBillServicePlanSchema.response
      },
    },
    getBillServicePlan,
  );

  server.post(
    MYPLANS.GET_ADDON_LIST,
    {
      // preValidation: [server.validateSession],
      schema: {
        summary: 'Fetch available add-on plans',
        description:
          'Retrieves the list of available add-ons for the user’s subscribed plan.',
        tags: ['MyAccount', 'Add-ons'],
        // body: getAddonListSchema.body,
      },
    },
    getAddonListController,
  );

  // anbu
  server.get(
    MYPLANS.GET_USER_PASSWORD_INFO, // --1
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get user password information',
        description:
          'Fetches password-related details such as last updated date or reset status for a specific user.',
        tags: ['User', 'Security'],
        // body: getBillServicePlanSchema.body,
        // response: getBillServicePlanSchema.response
      },
    },
    getUserPasswordInfo,
  );

  server.post(
    MYPLANS.GET_SYNC_UP_MYACCOUNT_DETAILS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Sync MyAccount details',
        description:
          'Retrieves the latest synced MyAccount data from the database for a specific user.',
        tags: ['MyAccount', 'Sync'],
        // body: getBillServicePlanSchema.body,
        // response: getBillServicePlanSchema.response
      },
    },
    getMyAccountsyncDetails,
  );

  server.post(
    MYPLANS.CREATE_MYACCOUNT_SYNCUP_DETAILS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Create or update MyAccount sync-up details',
        description:
          'Saves or updates user information in MyAccount sync records.',
        tags: ['MyAccount', 'Sync'],
        // body: getBillServicePlanSchema.body,
        // response: getBillServicePlanSchema.response
      },
    },
    createmyaccountsyncupdetails,
  );

  server.get(
    MYPLANS.GET_USER_DATA_PRIVACY_SETTING,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get user data privacy settings',
        description:
          'Retrieves the current privacy and data sharing settings configured by the user.',
        tags: ['User', 'Privacy'],
        // body: getBillServicePlanSchema.body,
        // response: getBillServicePlanSchema.response
      },
    },
    getUserDataPrivacySetting,
  );

  server.post(
    MYPLANS.INSERT_UPDATE_USERDATA_PRIVACY_SETTING,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Update user data privacy settings',
        description:
          'Inserts or updates user preferences related to data privacy and permissions.',
        tags: ['User', 'Privacy'],
        // body: getBillServicePlanSchema.body,
        // response: getBillServicePlanSchema.response
      },
    },
    insertUpdateDataPrivacySetting,
  );
  server.post(
    MYPLANS.GET_USER_DEVICES,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get registered user devices',
        description:
          'Fetches all devices currently registered or logged in under the user’s account.',
        tags: ['User', 'Devices'],
        // body: getBillServicePlanSchema.body,
        // response: getBillServicePlanSchema.response
      },
    },
    getUserDevice,
  );

  server.post(
    MYPLANS.INSERT_UPDATE_USER_DEVICES,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get user management list',
        description:
          'Retrieves a list of users or team members managed by the logged-in user.',
        tags: ['User', 'Management'],
        // body: getBillServicePlanSchema.body,
        // response: getBillServicePlanSchema.response
      },
    },
    getManagement,
  );

  server.post(
    MYPLANS.GET_USER_ROLES,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Fetch user roles',
        description:
          'Retrieves the list of roles and permissions assigned to the user.',
        tags: ['User', 'Roles'],
        // body: getBillServicePlanSchema.body,
        // response: getBillServicePlanSchema.response
      },
    },
    getuserroles,
  );

  server.get(
    MYPLANS.USER_GET_DETAILS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get user profile details',
        description:
          'Fetches detailed profile information for the given user ID, including contact and role info.',
        tags: ['User', 'Profile'],
        // body: getBillServicePlanSchema.body,
        // response: getBillServicePlanSchema.response
      },
    },
    userGetDetails,
  );

  server.get(
    MYPLANS.GET_USER_DEPARTMENT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get user department details',
        description:
          'Fetches the department or team structure associated with the current user.',
        tags: ['User', 'Organization'],
        // body: getBillServicePlanSchema.body,
        // response: getBillServicePlanSchema.response
      },
    },
    getUserDepartment,
  );

  server.get(
    MYPLANS.GET_USER_LOGIN_LOG,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Fetch user login activity log',
        description:
          'Retrieves all login and logout events for the user to track account access history.',
        tags: ['User', 'Security', 'Logs'],
        // body: getBillServicePlanSchema.body,
        // response: getBillServicePlanSchema.response
      },
    },
    getUserLoginLog,
  );

  server.get(
    MYPLANS.GET_ADDON_PURCHASE_LIST,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Fetch Add on list',
        description: 'Retrieves all add on list.',
        tags: ['addon'],
        // body: getBillServicePlanSchema.body,
        // response: getBillServicePlanSchema.response
      },
    },
    getAllAddonListHandler,
  );

  server.post(
    MYPLANS.GET_ROLES_INFO,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Fetch user login activity log',
        description:
          'Retrieves all login and logout events for the user to track account access history.',
        tags: ['User', 'Security', 'Logs'],
        // body: getBillServicePlanSchema.body,
        // response: getBillServicePlanSchema.response
      },
    },
    getRolesInfo,
  );

  server.post(
    MYPLANS.INSERT_UPDATE_TWO_STEP_VERIFICATION,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Insert or Update Two-Step Verification Settings',
        description:
          'Allows the user to enable, disable, or update their two-step verification preferences such as app-based, email-based, or phone-based authentication. Ensures enhanced account security by storing updated 2FA configurations for the user.',
        tags: ['User Security', 'Two-Step Verification', 'Account Settings'],
        // body: getBillServicePlanSchema.body,
        // response: getBillServicePlanSchema.response
      },
    },
    insertUpdateTwoStepVerificationHandler,
  );

  server.get(
    MYPLANS.GET_SECURITY_TWO_STEP_VERIFCATION,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Retrieve Two-Step Verification Settings',
        description:
          'Fetches the current two-step verification settings for the logged-in user, including app, email, and phone-based authentication statuses. Useful for displaying security preferences in the user profile or settings page.',
        tags: ['User Security', 'Two-Step Verification', 'Account Settings'],
        // body: getBillServicePlanSchema.body,
        // response: getBillServicePlanSchema.response
      },
    },
    getSecurityTwoStepVerification,
  );

  server.get(
    MYPLANS.GET_USER_ONBOARD_SETTING_COMPLETION,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Onboarding completion get api',
        description: 'Onboarding completion get api',
        tags: ['Onboarding'],
        // body: getBillServicePlanSchema.body,
        // response: getBillServicePlanSchema.response
      },
    },
    getUserOnboardSetting,
  );
  server.get(
    MYPLANS.CCAAS_GET_LIVE_INTERACTION,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get Live Interaction for Ai',
        description: 'get Live Interaction api',
        tags: ['Onboarding'],
        // body: getBillServicePlanSchema.body,
        // response: getBillServicePlanSchema.response
      },
    },
    getLiveInteraction,
  );
  server.post(
    MYPLANS.INSERT_UPDATE_DATA_AND_PRIVACY_SET,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Insert update data and privacy set api',
        description: 'Insert update data and privacy set api',
        tags: ['Onboarding'],
        // body: getBillServicePlanSchema.body,
        // response: getBillServicePlanSchema.response
      },
    },
    insertUpdateDataAndPrivacySet,
  );
  server.post(
    MYPLANS.GET_DATA_AND_PRIVACY_SET,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get data and privacy set api',
        description: 'Get data and privacy set api',
        tags: ['Onboarding'],
        // body: getBillServicePlanSchema.body,
        // response: getBillServicePlanSchema.response
      },
    },
    getDataAndPrivacySet,
  );
  server.post(
    MYPLANS.GET_ALL_MEMBERS_ROLE,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get all members role api',
        description: 'Get all members role api',
        tags: ['Onboarding'],
        // body: getBillServicePlanSchema.body,
        // response: getBillServicePlanSchema.response
      },
    },
    getAllMembersRole,
  );

  server.post(
    MYPLANS.GET_USER_DETAILS_BY_ROLE,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get user details by role',
        description: 'Get user details by role',
        tags: ['Onboarding'],
        // body: getBillServicePlanSchema.body,
        // response: getBillServicePlanSchema.response
      },
    },
    getUserDetailsByRole,
  );

  server.post(
    MYPLANS.INSERT_UPDATE_MYPLAN_ROLE_PERMISSION,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Insert update myplan role permission api',
        description: 'Insert update myplan role permission api',
        tags: ['Onboarding'],
        // body: getBillServicePlanSchema.body,
        // response: getBillServicePlanSchema.response
      },
    },
    insertUpdateMyplanRolePermission,
  );

  server.get(
    MYPLANS.GET_MYPLAN_ROLE_PERMISSION,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get myplan role permission api',
        description: 'Get myplan role permission api',
        tags: ['Onboarding'],
        // body: getBillServicePlanSchema.body,
        // response: getBillServicePlanSchema.response
      },
    },
    getMyplanRolePermission,
  );

  server.post(
    MYPLANS.GET_MYPLAN_TEAMS_DETAILS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get teams details api',
        description: 'Get get teams details api',
        tags: ['Onboarding'],
        // body: getBillServicePlanSchema.body,
        // response: getBillServicePlanSchema.response
      },
    },
    getMyplanTeamsDetails,
  );

  server.post(
    MYPLANS.INSERT_UPDATE_MYPLAN_TEAM_PERMISSION,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Insert update myplan team permission api',
        description: 'Insert update myplan team permission api',
        tags: ['Onboarding'],
        // body: getBillServicePlanSchema.body,
        // response: getBillServicePlanSchema.response
      },
    },
    insertUpdateMyplanTeamsPermission,
  );

  server.post(
    MYPLANS.GET_MYPLAN_TEAM_PERMISSION,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get myplan team permission api',
        description: 'Get myplan team permission api',
        tags: ['Onboarding'],
        // body: getBillServicePlanSchema.body,
        // response: getBillServicePlanSchema.response
      },
    },
    getMyplanTeamPermission,
  );

  server.post(
    MYPLANS.GET_MYPLAN_TEAM_LOG_ACTIVITY,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get myplan team permission api',
        description: 'Get myplan team permission api',
        tags: ['Onboarding'],
        // body: getBillServicePlanSchema.body,
        // response: getBillServicePlanSchema.response
      },
    },
    getMyplanTeamLogActivity,
  );

  server.post(
    MYPLANS.UPDATE_MYPLAN_USER_PERMISSION,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Update myplan user permission api',
        description: 'Update myplan user permission api',
        tags: ['Onboarding'],
        // body: getBillServicePlanSchema.body,
        // response: getBillServicePlanSchema.response
      },
    },
    updateMyplanUserPermission,
  );

  server.post(
    MYPLANS.GET_MYPLAN_USER_PERMISSION,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get myplan user permission api',
        description: 'Get myplan user permission api',
        tags: ['Onboarding'],
        // body: getBillServicePlanSchema.body,
        // response: getBillServicePlanSchema.response
      },
    },
    getMyplanUserPermission,
  );

  server.post(
    MYPLANS.GET_MYPLAN_PARTICULAR_USER_LOG_ACTIVITY,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get myplan user particular log activity api',
        description: 'Get myplan  user particular log activity  api',
        tags: ['Onboarding'],
        // body: getBillServicePlanSchema.body,
        // response: getBillServicePlanSchema.response
      },
    },
    getMyplanUserActivityLogActivity,
  );

  server.post(
    MYPLANS.INSERT_USER_ACTIVITY_LOG,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Insert user Activity Log',
        description: 'Insert user Activity Log Api',
        tags: ['User', 'Management'],
        // body: getBillServicePlanSchema.body,
        // response: getBillServicePlanSchema.response
      },
    },
    insertUserActivityLogHandler,
  );
  server.post(
    MYPLANS.GET_USER_ACTIVITY_LOG,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get user log activity api',
        description: 'Get user log activity  api',
        tags: ['Onboarding'],
        // body: getBillServicePlanSchema.body,
        // response: getBillServicePlanSchema.response
      },
    },
    getUserActivityLogHandler,
  );

  server.post(
    MYPLANS.INSERT_MPLAN_USER_DEVICES_DTL,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get user management list',
        description:
          'Retrieves a list of users or team members managed by the logged-in user.',
        tags: ['User', 'Management'],
        // body: getBillServicePlanSchema.body,
        // response: getBillServicePlanSchema.response
      },
    },
    insertUserDevicesHandler,
  );
  server.post(
    MYPLANS.GET_USER_DEVICES_DTL,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get user devices api',
        description: 'Get user devices api',
        tags: ['Onboarding'],
        // body: getBillServicePlanSchema.body,
        // response: getBillServicePlanSchema.response
      },
    },
    getUserDevicesHandler,
  );

  server.get(
    MYPLANS.COMPNAY_BILLIG_SUBSCRIPTION,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'company billing subscription',
        description: 'company billing subscription api',
        tags: ['Onboarding']
        // body: getBillServicePlanSchema.body,
        // response: getBillServicePlanSchema.response
      },
    },
    getCompanyBillingSubscription
  );

  server.get(
    MYPLANS.GET_STORAGE_DATA,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get storage data',
        description: 'get storage data api',
        tags: ['Onboarding']
        // body: getBillServicePlanSchema.body,
        // response: getBillServicePlanSchema.response
      },
    },
    getStorageDataHandler
  );

  // forgot password
  server.post(
    MYPLANS.EMAIL_OTP_VERIFICATION,
    {
      // preValidation: [server.validateSession], // not need
      schema: {
        summary: 'email otp verification',
        description: 'email otp verification api',
        tags: ['user'],
      },
    },
    emailOtp,
  );

  server.post(
    MYPLANS.CHECK_OTP_VALIDATION,
    {
      // preValidation: [server.validateSession], // not need
      schema: {
        summary: 'check otp validation',
        description: 'check otp validation  api',
        tags: ['user'],
      },
    },
    verifyOtp,
  );

  server.post(
    MYPLANS.UPDATE_PASSWORD,
    {
      // preValidation: [server.validateSession], // not need
      schema: {
        summary: 'update password',
        description: 'update password api',
        tags: ['user'],
      },
    },
    passwordUpdate,
  );
  server.post(
    MYPLANS.RESEND_OTP,
    {
      // preValidation: [server.validateSession], // not need
      schema: {
        summary: 'resend otp',
        description: 'resend otp api',
        tags: ['user'],
      },
    },
    resendOtp,
  );

  next();
}
