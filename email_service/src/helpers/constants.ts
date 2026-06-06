/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

export const EMAIL_CONSTANT = {
  GET_EMAIL_CHANNEL: '/get_email_channel',
  POST_EMAIL_CHANNEL: '/post_email_channel',
  DELETE_EMAIL_LIST: '/delete_email_list',
};

export const EMAIL = {
  CREATE_EMAIL_CONGIF: '/create_email_config',
  GET_EMAIL_CONFIG: '/get_email_config',
  DELETE_EMAIL_ADDRESS: '/delete_email_address',
  UPDATE_PRIMARY_EMAIL: '/update_primary_email',
  SEND_EMAIL: '/send_email',
  SEND_EMAIL_OUTBOUND: '/send_email_outbound',
  SEND_EMAIL_OUTBOUND_COMPANY: '/send_email_outbound_company',
  SEND_EMAIL_BOT: '/send_email_bot',
  SEND_EMAIL_AUDRACARE: '/send_email_audracare',
  GET_SUPERVISIOR_LIST: '/get_supervisior_list',
  GET_EMAIL_HISTORY_DETAILS: '/get_email_history_details',
  FILE_UPLOAD: '/file_upload',
  UPDATE_DISPOSITION: '/update_disposition_email',
  ONBORD_CREATE_EMAIL: '/onbord_create_email',
  UPDATE_EMAIL_BOT: '/update_email_bot_status',
  ONBORD_UPDATE_EMAIL_BOT: '/onbord_update_email_bot_status',
  ONBORD_GET_EMAIL_CONFIG: '/onbord_get_email_config',
  GET_AGENT_ASSIST_INFO: '/get_agent_assist_info',
  ASSIGN_CALLBACK_EMAIL: '/assign_callback_email',
  SEND_MAIL_LEAD: '/send_mail_lead',
  SEND_EMAIL_TEMPLATE: '/send_email_template',
  SEND_EMAIL_PLAINTEXT: '/send_email_plaintext',
  GET_EMAIL_FLOW_HISTORY: '/get_email_flowHistory_list',
  SEND_MAIL__ROUTE_MOBILE_WHATSAPP: '/send_mail_route_whatsapp',
  CREATE_CRM_EMAIL_CONFIG: '/create_crm_email_config',
  INSERT_DOMAIN_CONFIGURATION: '/insert_domain_configuration',
  GET_DOMAIN_CONFIGURATION_LIST: '/get_domain_configuration',
  GET_DKIM_GENERATE_KEY: '/get_dkim_generate_key',
  DELETE_DOMAIN_DETAILS: '/delete_domain_details/:id',
  GET_DOMAIN_CONFIGURATION_DETAILS: '/get_domain_configuration_details/:id',
  GET_DOMAIN_ALREADY_EXIST: '/get_domain_already_exist/:domainName',
  UPDATE_DOMAIN_VERIFICATION: '/update_domain_verification/:id',
  GET_VERIFICATION_DOMAIN_LIST: '/get_verification_domain_list',
  CREATE_MAILBOX: '/create_mailbox',
  UPDATE_REPLYVIA_DISPOSITION_EMAIL: '/update_replyvia_disposition_email',
  INSERT_UNSUBSCRIBE_DETAILS: '/insert_unsubscribe_details',
  INSERT_TICKETING_EMAIL_CONFIG: '/insert_ticket_email_config',
  DELETE_TICKET_EMAIL_CONFIG: '/delete_ticket_email_config',
  SEND_EMAIL_SESSION_EXPIRY:'/send_email_session_expiry',
  FETCHING_INBOX_EMAIL_CCAAS: '/fetching_inbox_email_ccaas'
};

export const RESPONSE = {
  not_found: 'No data found',
  internal_error: 'Internal server error',
  success_message: 'success',
  empty_data: 'Empty data',
  create_insert_successfully: 'create email insert successfully',
  email_updated_successfully: 'email updated successfully',
  email_deleted_successfully: 'email deleted successfully',
  create_Email: 'create sms successfully',
  email_template_deleted_successfully: 'email Template deleted successfully',
  id_empty: 'id no should not be empty',
  email_insert: 'Email Data Inserted successfully',
  email_update: 'Email Data Update successfully',
  email_duplicating: 'Email Address is duplicating',
  primary_key_update: 'Primary key updated successfully',
  Primary_set_successfully: 'Primary set successfully',
  email_send: 'email Sended successfully',
  forwarding: 'Forwarding is not set up.',
  error_checking: 'Error checking forwarding configuration:',
  disposition_statuss: 'update disposition status successfully',
  set_primary_email_field: 'set primary email field',
  bot_status: 'bot status update successfully',
  rpush: 'send push successfully',
  update_asssign: 'update assign callback successfully',
  email_template: 'email template Send successfully',
  email: 'need to purchase addon emailaddress',
  insert_domain: 'insert email domain successfully',
  failed_domain: 'failed email domain',
  delete_domain: 'email domain deleted successfully',
  delete_domain_field: 'email domain deleted field',
  name_already_exist: 'domainName already exist',
  available_domain: 'available domain',
  update_domain: 'verification status updated successfully',
  status_domain: 'verification status failed',
  not_verify_domain: 'domain not verified',
  csat: 'Customer satisfaction channel not found',
  invalid_channel: 'Invalid channel type',
  integration_email_template: 'Whatsapp integration email template',
};

export const oAuth = {
  clientId: '294061743332-fs46o16ggp3dshe6efr4ka2qpn137923.apps.googleusercontent.com',
  clientSecret: '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',
  grant_type: 'authorization_code',
  token_url: 'https://oauth2.googleapis.com/token',
};

export const ROLES = {
  OWNER_ROLE_ID: '1',
  SUB_OWNER_ROLE_ID: '2',
  ADMIN_ROLE_ID: '3',
  MEMBER_ROLE_ID: '4',
  SUPERVISOR_ROLE_ID: '5',
  AGENT_ROLE_ID: '6',
};

export const USER_ROLE = {
  AGENT_ROLE_ID: 43,
  ADMIN_ROLE_ID: 3,
  SUPERVISOR_ROLE_ID: 12,
};

export const CHANNEL_TYPE = {
  EMAIL: 'Email',
  WHATSAPP: 'Whatsapp',
  FACEBOOK: 'Facebook',
  INSTAGRAM: 'Instagram',
  SMS: 'Sms',
  CHAT: 'Chat',
  CALL: 'Call'
};

export const emitters = {
  receivedEmail: 'receivedEmail',
  supervisiorActivity: 'supervisiorActivity',
  interActionClosed: 'interActionClosed',
  SentimentScoreAnalytics: 'SentimentScoreAnalytics',
  agentAssistOmniChannel: 'agentAssistOmniChannel',
  assignActivityListener: 'assignActivityListener',
  supervisiorActivityBot: 'supervisiorActivityBot',
  adminActivityBot: 'adminActivityBot',
};
