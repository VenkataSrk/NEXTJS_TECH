/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

export const CALL = {
  SAVE_CALL_FLOW: '/savecallflow',
  UPDATE_CALL_FLOW: '/update_callflow',
  GET_CALL_FLOW_LIST: '/callflowlist',
  GET_CALL_FLOW_DETAILS: '/callflowdetails/:cfid',
  GET_SESSIONID_LIST: '/get_sessionid_list',
  GET_CALLBACK_LIST: '/get_callback_list',
  ASSIGN_CALLBACK: '/assign_callback',
  ASSIGN_CALLBACK_CALL: '/assign_callback_call',
  UPDATE_CALLBACK_CALL: '/update_callback_call',
  UPDATE_CALLBACK_SESSIONID: '/update_callback_sessionid',
  UPDATE_CALLBACK_CHAT: '/update_callback_chat',
  UPDATE_CALLBACK_SOCIAL_MEDIA: '/update_callback_social_media',
  GET_ACTIVITY_LIST: '/get_activity_list',
  GET_CONVERSATION_LIST: '/get_conversation_list',
  GET_SESSION_HISTORY_DETAILS: '/get_session_history_details',
  UPDATE_VOICEMAIL_CALLBACK: '/update_voiceMail_callBack',
  DELETE_CALL_FLOW: '/delete_call_flow',
  GET_CUSTOMER_DND_INFO: '/get_customer_dnd_info'
};

export const CALL_CONFIG = {
  INSERT: '/update_voice',
  GET_CALL_CONFIG: '/call_config',
  GET_BY_NUMBER: '/call_by_num/:number',
  DELETE_BY_NUMBER: '/coc/:number',
  UPDATE_STATUS: '/update_status',
  INSERT_UPDATE_VOICE_VIDEO_RECORDING: '/insert_update_voice_video_recording',
  GET_SETTING_VOICE_VIDEO_RECORDING: '/get_setting_voice_video_recording'
};

export const CALL_NOTIFICATION = {
  CALL_DETAILS: '/register',
  REMOVE_CALL_DETAILS: '/remove_call_details',
  IN_CALL: '/incall',
  SERVER_TIME: '/time',
  GET_VOICEMAIL_OVERALL_DETAILS:'/get_voicemail_overall_details'
};

export const STATE = {
  CREATE_STATE: '/create_state',
  GET_SATATE_LIST_BY: '/get_state_list_by'
};

export const PROMPT = {
  FILEUPLOAD: '/chat_upload_file',
  UPLOADFILE: '/prompt_file_upload',
  INSERT_AUDIO_PROMPT: '/insert_prompt_audio',
  GET_PROMPT_LIBRAR: '/get_prompt_audio_library',
  UPDATE_PROMPT_AUDIO: '/update_prompt_audio_library',
  DELETE_PROMPT_AUDIO: '/delete_prompt_audio',
  IS_VALID_PROMPT_NAME: '/is_valid_prompt_name',
  GET_PREVIEW_AUDIO: '/get_preview_audio',
};

export const CHAT = {
  GET_SESSION_ID: '/get_chatSessionId',
  GET_SESSION_CALL: '/get_callSessionId',
  GET_CHAT_BY_NUMBER: '/get_chatByNumber',
  SAVE_CHATFLOW_HISTORY: '/save_chatflow_history',
  GET_CHATFLOW_BY_CFID: '/get_chatflow_by_cfid',
  GET_ALL_CHATFLOW_DETAILS: '/get_all_chatflow_details',
  GET_CHATFLOW_NODES_LIST: '/get_chatflow_nodes_list',
  GET_LISTEN_SESSION_ID: '/listen',
  UPDATE_CHATBOTCHANNEL_TYPE: '/update_chatBot_channel_type',
  DELETE_CHATBOTCHANNEL_TYPE: '/delete_chatBotChannel_type',
  GET_AGENT_STATUS_VIEW: '/get_agent_data',
  GET_CHATFLOW_LIST_DOMAINID: '/get_chatflow_list_domainId',
  GET_DEFAULT_CHATFLOW_ID: '/get_default_chatflow_id',
  GET_CHATFLOW_NODE_DTL: '/get_chatflow_node_dtl',
};

export const WEBCHAT = {
  CREATE_WEBCHAT: '/create_chat_session',
  GET_CUSTOMER_HISTORY: '/get_customer_history',
  GET_CUSTOMER_HISTORY_BY_UID: '/get_customer_history_by_uid',
  STORE_CUSTOMER_CALL_BACK: '/store_customer_callback',
  CREATE_CUSTOMER_UPLOAD: '/create_customer_file_upload',
  GET_AGENT_DETAILS: '/get_agent_details',
  GET_SESSION_DETAILS_CHATID: '/get_session_details_chatId',
  GET_AICHAT_HISTORY: '/get_aichat_history',
  UPDATE_CHAT_DISPOSTION: '/update_chat_disposition',
  BOT_CONNECT_ID_GENERATE: '/bot_connect_id_generate',
  AGENT_CONNECT_ID_GENERATE: '/agent_connect_id_generate',
  GET_LOCATION_DETAILS: '/get_location_details',
  GET_AGENT_AVAILABILITY: '/get_agent_availability',
  REPLACE_WEBCHAT_MESSAGE: '/replace_webchat_message'
};

export const QUEUE = {
  GET_QUEUE_WAIT_TIME: '/get_queue_wait_time',
  GET_ALL_QUEUE_WAIT_TIME: '/get_all_queue_wait_time',
  INSERT_QUEUE_DETAILS: '/queue_details'
};

export const DISPOSITION = {
  INSERT_DISPOSITION_STATUS: '/insert_disposition_status',
  GET_DISPOSITION_STATUS: '/get_disposition_status',
  UPDATE_DISPOSITION_STATUS: '/update_disposition_status',
  DELETE_DISPOSITION_STATUS: '/delete_disposition_status',
  IS_VALID_DISPOSITION_NAME: '/is_valid_disposition_name',
  UPDATE_DISPOSITION: '/update_disposition',
  UPDATE_CALLED_PERSON_DETAILS: '/update_called_person_details',
  GET_DISPOSITION_FILTER_LIST: '/get_disposition_filter_list',
  UPDATE_DISPOSITION_SMS: '/update_disposition_sms',
  ONBOARD_COMPANY_PURCHASE_ADDON: '/onboard_company_purchase_addon',
  UPDATE_DISPOSITION_VIDEO: '/update_disposition_video',
  GET_VIDEOMEET_RECORD_INFO: '/get_video_Meet_record_info',
  GET_VOICE_RECORD_INFO: 'get_voice_user_record_info',
  GET_VIDEO_SESSION_DETAILS: 'get_video_session_details/:sessionid',
  GET_VOICE_SESSION_DETAILS: 'get_voice_session_details/:sessionid',
  // AUTO_ASSIGN: '/auto_assign',
  UPDATE_DISPOSITION_BEFOREUNLOAD: 'update_disposition_beforeunload',
  AI_SUMMARY_SUBJECT: '/ai_summary_subject',
  REGENERATE_DISPOSITION: '/regenerate_disposition',
  DELETE_VOICE_RECORDINGS: '/delete_voice_recordings',
};

export const NOTIFICATION = {
  INSERT_NOTIFICATION: '/insert_notificationmsg',
  INSERT_NOTIFICATION_CALLBACK: '/insert_notificationmsg_callback',
  GET_NOTIFICATION: '/get_notification',
  DELETE_NOTIFICATION: '/delete_notification',
  UPDATE_MARKAS_READ: '/update_markasread',
  UPDATE_MARKAS_READ_ALL: '/update_markasread_all',
};

export const RESPONSE = {
  update_initiate_feedback: 'update initiate successfully',
  not_found: 'No data found',
  internal_error: 'Internal server error',
  success_message: 'success',
  create_queue_waitTime: 'create queue waitTime successfully',
  update_queue_waitTime: 'update queue waitTime successfully',
  deleted_queue_waitTime_successfully: 'deleted_queue_waitTime_successfully',
  empty_data: 'Empty data',
  create_State: 'creating state successfully',
  User_update_state_success: 'user_update_state_success',
  update_asssign: 'update assign callback successfully',
  disposition_status: 'insert disposition status successfully',
  disposition_statuss: 'update disposition status successfully',
  dispositionn_statuss: 'delete disposition status successfully',
  did: 'did number should not be empty',
  available: 'available',
  name_already_exist: 'name already exist',
  notification_success_status: 'notification details insert successfully',
  notification_delete_status: 'Deleted successfully',
  update_markas_read: 'mark as read update successfully',
  valid_input: 'give the valid input',
  bot_status: 'bot status update successfully',
  updated_success: 'Update Successfuly',
  insert_success: 'Insert Successfuly',
  failed: 'Updated Failed',
  fail: 'Failed',
  chatFlowId: 'chatFlowId number should not be empty',
  not_update: 'not update',
  delete_contact: 'delete successfully',
  callback_success: 'Updated CallBackCallHistoryId Successfully',
  callback_fail: 'Update CallBackCallHistoryId failed'
};

export const EMAIL_CONSTANT = {
  GET_EMAIL_CHANNEL: '/get_email_channel',
  POST_EMAIL_CHANNEL: '/post_email_channel',
  DELETE_EMAIL_LIST: '/delete_email',
  UPDATE_EMAIL_PRIMARYKEY: '/update_primarykey',
  GET_INBOX_MAIL: '/get_inbox_mail',
  CREATE_EMAIL_SESSION: '/create_email_session',
};

export const ROLES = {
  OWNER_ROLE_ID: '1',
  SUB_OWNER_ROLE_ID: '2',
  ADMIN_ROLE_ID: '3',
  MEMBER_ROLE_ID: '4',
  SUPERVISOR_ROLE_ID: '5',
  AGENT_ROLE_ID: '6',
};

export const CALL_BLAST = {
  INSERT_UPDATE_CALLBLAST: '/insert_update_callBlast',
  INSERT_AUDIO_PROMPT_CALLBLAST: '/insert_audio_prompt_callBlast',
  GET_CUSTOMER_CONTACT_CALLBLAST: '/get_customer_contact_callblast',
  GET_CALLBLAST_LIST: '/get_callblast_list',
  UPDATE_CALLBLAST_STUTAS: '/update_callblast_stutas',
  DELETE_CALLBLAST: '/delete_callblast',
  HISTORY_CALLBLAST: '/get_callBlast_history',
  GET_CALLBLAST_OVERALL_CAMPAIGNS: '/get_callBlast_overAll_Campaigns',
  GET_CALLBLAST_PARTICULAR_HISTORY: '/get_callBlast_particular_history',
  GET_CALLBLAST_AUTO_ANSWER: '/get_callBlast_auto_answer',
  INSERT_UPDATE_CALLBLAST_AUTO_ANSWER: '/insert_update_callBlast_auto_answer',
  GET_CUSTOMER_CONTACT_CALLBLAST_FILTER_VALUE: '/get_customer_contact_callblast_filter_value',
  DELETE_OUTBOUND_CAMPAIGN_DRAFT: '/delete_outbound_campaign_draft',
  GET_CALLBLAST_OVERALL_SCHEDULED_CAMPAIGNS: '/get_callblast_overall_scheduled_campaigns',
  GET_CHILD_BLASTID_HISTORY_DTL: '/get_childblastId_history_dtl',
};

export const VOICEBOT = {
  CREATE_GUEST_USER: '/guest'
};
export const HUBSPOT_DOMAINID = [7707, 7509, 20487, 20488, 20489];
