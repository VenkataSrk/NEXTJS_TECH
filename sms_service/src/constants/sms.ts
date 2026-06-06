/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

/** ----------------- 📱 SMS API Endpoints ----------------- */
export const SMS_ENDPOINTS = {
  ASSIGN_CALLBACK: '/assign_callback_sms',
  INSERT_INBOUND: '/insert_sms_in',
  UPDATE_BOT_STATUS: '/update_sms_bot_status',
  ONBOARD_UPDATE_BOT_STATUS: '/onbord_update_sms_bot_status',
  UPDATE_DISPOSITION: '/update_disposition_sms',
  CCAAS_OUT: '/ccaas_sms_out',
  SEND_OUTBOUND: '/send_sms_outbound',
  SEND_OUTBOUND_COMPANY: '/send_sms_outbound_company',
  UPDATE_REPLYVIA_DISPOSITION: '/update_replyvia_disposition_sms',
} as const;

export type SmsEndpointKey = keyof typeof SMS_ENDPOINTS;

/** ----------------- ✅ API Response Messages ----------------- */
export const RESPONSE = {
  SUCCESS: 'Operation completed successfully',
  ERROR: 'Internal server error',
  VALIDATION_ERROR: 'Invalid input',
  UNAUTHORIZED: 'Unauthorized access',
  NOT_FOUND: 'No data found',
  EMPTY: 'Empty data',
  DISPOSITION_UPDATED: 'Disposition updated successfully',
  DID_REQUIRED: 'DID number should not be empty',
  NAME_EXISTS: 'Name already exists',
  BOT_STATUS_UPDATED: 'Bot status updated successfully',
  UPDATED_SUCCESS: 'Updated successfully',
  UPDATE_FAILED: 'Update failed',
  CALLBACK_ASSIGNED: 'Assign callback updated successfully',
  SMS_SENT: 'SMS sent successfully',
  SMS_ERROR:'Customer number is blocked'
} as const;

export type ResponseKey = keyof typeof RESPONSE;

/** ----------------- 👤 User Roles ----------------- */
export const ROLES = {
  OWNER: '1',
  SUB_OWNER: '2',
  ADMIN: '3',
  MEMBER: '4',
  SUPERVISOR: '5',
  AGENT: '6',
} as const;

export type RoleKey = keyof typeof ROLES;

/** ----------------- 🔔 Socket Emitters ----------------- */
export const EMITTERS = {
  ASSIGN_ACTIVITY: 'assignActivityListener',
  SUPERVISOR_ACTIVITY: 'supervisiorActivity',
  ADMIN_ACTIVITY: 'adminActivity',
  SUPERVISOR_BOT_ACTIVITY: 'supervisiorActivityBot',
  ADMIN_BOT_ACTIVITY: 'adminActivityBot',
  RECEIVED_SMS: 'receivedSms',
  INTERACTIONCLOSE:'interActionClosed',
  SENTIMENTSCORE :'SentimentScoreAnalytics'
} as const;

export type EmitterKey = keyof typeof EMITTERS;

/** ----------------- Channels----------------- */
export const CHANNELS = {
  SMS: 'Sms',
} as const;

export type ChannelsKey = keyof typeof CHANNELS;
