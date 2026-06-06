/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

import moment from 'moment';

export const USER = {
  UPDATE: '/user_update',
  USER_UPDATE: '/update_user',
  FORGOT_PASSWORD: '/forgot_password',
  RESET_PASSWORD: '/reset_acc_password',
  GET_USER_LIST_BY: '/get_user_list_by',
  GET_USER_DETAILS: '/get_user_details',
  GET_AGENT_ACTIVITY: '/get_agent_activity',
  GET_AGENT_SUPERVISIOR_ACTIVITY: '/get_agent_supervisor_activity',
  GET_ALL_USER_LIST: '/get_all_user_list',
  GET_ALL_PROFILE_INFO: '/get_all_profile_info',
  GET_USER_DETAILS_LIST: '/get_user_details_list',
  INSERT_ONBOARD_USER: '/insert_onboard_user',
  ONBOARD_DID_NUMBER: '/onboard_did_number',
  GET_USER_DETAILS_EMAIL: '/get_user_details_email',
  UPDATE_ONBOARD_USER: '/update_Onboard_user',
  GET_TOKEN_LOGIN_DETAILS: '/get_token_login_details',
  UPDATE_TICKETING_ID: '/update_ticketing_id',
  INSERT_DEFAULT_KEVIN_DETAILS: '/insert_default_kevin_details',
  GET_USER_ID: '/getUserId',
  GET_INTERACTION_DETAILS: '/get_interaction_details',
  SET_DEFAULT_COMPANY: '/set_default_company',
  ONBOARD_DELETE_USER: '/onboard_delete_user',
  ONBOARD_TEMPORARY_DELETE_USER: '/onboard_temporary_delete_user',
  UPDATE_ONBOARD_USER_ACTIVE_STATUS: '/update_onboard_user_active_status',
  GET_INTERACTION_DETAILS_VIEW: '/get_interaction_details_view',
  UPDATE_ONBOARD_USER_FLAG: '/update_onboard_user_flag',
  GET_CALLBACK_OPTIONS: '/get_callback_options',
  GET_API_UNIQUE_DETAILS: '/get_api_unique_details',
  UPDATE_PROFILE_PICTURE: '/update_profile_Picture',
  UPDATE_PROFILE_NAME: '/update_profile_name',
  GET_PROFILE_IMG: '/get_profile_img',
  GENERATE_OTP: '/generate_otp',
  VALIDATE_OTP: '/validate_otp',
  GET_ADDON_LANGIAGES: '/get_addon_languages',
  VERIFY_CAPTCHA: '/verify_captcha',
  GET_INTERACTION_LOG_DETAILS: '/get_interaction_log_details',
  GET_CUSTOMER_PARTICULAR_INTERACTION_LOG:'/get_customer_particular_interaction_log',
  GET_CUSTOMER_OVERALL_CALLBACK_INTERACTION:'/get_customer_overall_callback_interaction',
  GET_CUSTOMER_OVERALL_CALLBACK_INTERACTION_GROUP:'/get_customer_overall_callback_interaction_group', // this is get interaction
  GET_CUSTOMER_OVERALL_CALLBACK_INTERACTION_PERTICULAR_DETAILS:'/get_customer_overall_callback_interaction_perticular_details',

};
export const SESSIONHISTORY = {
  INSERT_LOGIN_SESSION_HISTORY: '/insert_login_session_history',
  UPDATE_LOGIN_SESSION_HISTORY: '/update_login_session_history',
  GET_CCAAS_ROLE_INFO: '/get_ccaas_role_info',
  GET_SESSION_DETAILS: '/get_session_details/:sessionId',
  GET_EXPIRED_PLAN_DETAILS: '/get_expired_plan_details',
  AICCAAS_GET_CHANNEL_CONFIG_DETAILS:'/aiccaas_get_channel_config_details',
  GET_SOCIALMEDIA_HISTORY_DETAILS: '/get_socialmedia_history_details',
  GET_SOCIALMEDIA_MESSAGE_BY_ID:'/get_socialmedia_message_by_id',
};
export const QUEUE = {
  INSERT: '/insert_queue',
  GET: '/get_queue',
  UPDATE: '/update_queue',
  DELETE: '/delete_queue',
  GET_LIST: '/get_queue_list',
  UPDATE_QUEUE: '/update_queue_status',
  INSERT_CUSTOM_HOURS: '/insert_custom_hours',
  CHECK_QUEUE_MAPPING: '/check_queue_mapping',
  GET_CUSTOM_HOURS_LIST: '/get_custom_hours_list',
  UPDATE_CUSTOM_HOURS: '/update_custom_hours',
  UPDATE_CUSTOM_HOURS_BY_ID: '/update_custom_hours_by_id',
  DELETE_CUSTOM_HOURS: '/delete_custom_hours',
  CUSTOM_HOURS_STATUS: '/custom_hours_status',
  GET_CUSTOM_HOURS_DETAIL: '/get_custom_hours_detail',
  GET_CUSTOM_HOURS: '/get_custom_hours',
  IS_VALID_CUSTOM_HOURS_NAME: '/is_valid_custom_hours_name',
  IS_VALID_QUEUE_NAME: '/is_valid_queue_name',
  UPDATE_STATUS: '/queue_update_status',
  INSERT_UPDATE_BUSINESS_HOLIDAYS: '/insert_update_business_holidays',
  GET_BUSINESS_HOLIDAYS: '/get_update_business_holidays',
  DELETE_BUSINESS_HOLIDAYS: '/delete_business_holidays',
  UPDATE_BUSINESS_HOLIDAYS_STATUS: '/update_business_holidays_status',
};
export const ROUTING = {
  INSERT_ROUTING: '/insert_routing',
  GET_ROUTING: '/get_routing',
  UPDATE_ROUTING: '/update_routing',
  DELETE_ROUTING: '/delete_routing',
  GET_ROUTING_LIST: '/get_routing_list',
  ROUTING_PROFILE_STATUS: '/routing_profile_status',
  UPDATE_STATUS: '/update_status',
  IS_VALID_ROUTING_NAME: '/is_valid_routing_name',
  IS_ENABLE_LIST: '/is_Routing_profile_enable_list',
};

export const DID = {
  // PURCHASE_INSERT: '/did_purchase_insert',
  PURCHASE_GET: '/get_did_purchase',
  PURCHASE_UPDATE: '/did_purchase_update',
  PURCHASE_DELETE: '/did_purchase_delete',
  PURCHASE_GET_lIST: '/did_purchase_list',
  GET_DOMAIN_DID_lIST: '/get_domain_did_purchase_list',
  GET_DID_PURCHASES: '/get_did_purchases',
  UPDATE_IS_SMS: '/update_is_sms',
  UPDATE_DID_NUMBER_EDIT: '/update_didnumber_sms',
  UPDATE_DID_STATUS: '/update_did_status',
  SIP_TRUNK_NUM_DELETE: '/sip_trunk_num_delete',
  SIP_TRUNK_NUM_CHECK: '/sip_trunk_num_check',
  INDICATE_QA_TO_NLP: '/indicate_qa_to_nlp'
};

export const SKILLS = {
  SKILL_INSERT: '/skill_insert',
  SKILL_GET: '/skill_get',
  SKILL_UPDATE: '/skill_update',
  SKILL_DELETE: '/skill_delete',
  SKILL_GET_LIST: '/get_skill_list',
  SKILL_STATUS: '/skill_status',
  UPDATE_SKILL_STATUS: '/update_skill_status',
  IS_VALID_SKILL_NAME: '/is_valid_skill_name',
  IS_ENABLE_LIST: '/is_skill_enable_list',
};
export const SHORTCUTKEYS = {
  INSERT_SHORTCUT_KEYS: '/insert_shortcut_keys',
  UPDATE_SHORTCUT_KEYS: '/update_shortcut_keys',
  GET_SHORTCUT_KEYS: '/get_shortcut_keys',
  UPDATE_STATUS_SHORTCUT_KEYS: '/update_status_shortcutkeys',
  RESET_ALL_SHORTCUT_KEYS: '/reset_all_shortcut_keys',
  INSERT_GLOBAL_SHORTCUT_KEYS: '/insert_global_shortcut_keys',
  GET_GLOBAL_SHORTCUT_KEYS: '/get_global_shortcut_keys',
  RESET_GLOBAL_SHORTCUT_KEY_BYUUID: '/reset_global_shortcut_by_uuid',
};

export const OAUTHCLIENT = {
  GET_OAUTH_CLIENT_INFO: '/get_oauth_client_info',
  DELETE_OAUTH_CLIENT_INFO: 'delete_oauth_client_info',
  INSERT_UPDATE_OATH_CLIENT_INFO: '/insert_update_oath_client_info',
  GET_DOMAIN_DETAILS_API: '/get_domain_details_api',
  GET_API_END_POINTS_DETAILS: '/get_api_end_point_details',
  CREATE_INSERT_DOMAIN_DETAILS_API: '/create_insert_domain_details_api',
  UPDATE_DOMAIN_DETAILS_API: '/update_domain_details_api',
  INSERT_END_POINTS_DETAILS: '/insert_end_points_details',
  UPDATE_END_POINTS_DETAILS: '/update_end_points_details',
  DELETE_DOMAIN_API: '/delete_domain_api',
  DELETE_API_END_POINT_DETAILS: '/delete_api_end_point_details',
};

export const ACCOUNT = {
  FORGET_PASSWORD: '/forget_password',
  FORGET_PASSWORD_RESET: '/forget_password_reset'
};

export const COUNTRY = {
  GET_ROLE_LIST: '/get_country_list',
  GET_COUNTRY_CONFIG: '/get_country_config',
  GET_NODES_ENTRY_POINT: '/get_nodes_entry_point',
  GET_STATE_CONFIG: '/get_state_config',
  GET_CITY_CONFIG: '/get_city_config'
};

export const CHATBOT = {
  INSERT_UPDATE_INSIGHT_VISITORS: '/insert_update_insight_vistors'
};

export const FILE = {
  UPLOAD: '/upload',
  VIRTUAL_BACKGROUND_UPLOAD: '/virtual_background_upload',
};

export const KNOWLEDGEBASEBOT = {
  CREATE_CATEGORY_FOLDER: '/create_category_folder',
  GET_CATEGORY_FOLDER: '/get_category_folder',
  UPLOAD_ARTICLE: '/upload_article',
  INSERT_UPDATE_ARTICLE: '/insert_article_file',
  GET_ARTICLE_LIST: '/get_article_list',
  UPDATE_WEBSITE_SCRAP: '/update_website_scrap',
  GET_ARTICLE_CATEGORY_WEBID: '/get_article_category_webid',
  INSERT_UPDATE_LEAD_EMAIL_GENERATION: '/insert_update_lead_email_generation',
  DELETE_CATEGORY_ID: '/delete_category_id',
  UPDATE_MULTIPLE_ARTICLE: '/update_multiple_article',
  DELETE_KNOWLEDGE_WEBID: '/delete_knowledge_webid',
  GET_ARTICLE_LIST_CATID: '/get_article_list_catid',
  DELETE_ARTICLE_ID: '/delete_article_id'
};

export const LEADGENERATION = {
  INSERT_UPDATE_LEAD_GENERATION: '/insert_update_lead_generation',
  GET_LEAD_GENERATION_ID: '/get_lead_generation_id',
  GET_LEAD_GENERATION_DOMAINID: '/get_lead_generation_domainId',
  INSERT_UPDATE_LEAD_USER_DETAILS: '/insert_update_lead_user_details',
  GET_LEAD_USER_DETAILS_DOMAINID: '/get_lead_user_details_domainId',
  GET_LEAD_USER_DETAILS_ID: '/get_lead_user_details_id',
  INSERT_PAYMENT_INTERGATION: '/insert_payment_intergation',
  GET_PAYMENT_INTERGATION: '/get_payment_intergation',
  GET_PAYMENT_INTERGATION_NO_AUTH: '/get_payment_intergation_no_auth',
  DELETE_PAYMENT_INTERGATION: '/delete_payment_intergation',
  GET_IRRELEVANT_DETAILS_DOMAINID: '/get_irrelevant_details_domainId',
  GET_COMPANY_TYPE_WEBSITEID: '/get_company_type_website',
  GET_CHAT_SESSION_AVAILABILITY: '/get_chat_session_availability',
  UPDATE_CHAT_SESSION_COUNT: '/update_chat_session_count'
};

export const BOT = {
  CREATE_BOT_INTENT: '/create_bot_intent',
  GET_BOT_INTENT: '/get_bot_intent',
  INSERT_DEFAULT_CHAT_IMAGES: '/insert_default_chat_images',
  GET_CHAT_IMAGE_DETAILS: '/get_chat_image_details',
  UPDATE_BOT_TEMPLATE_LIST: '/update_bot_template_list',
  INSERT_FALLBACK_MESSAGE_LIST: '/insert_fallback_message_list',
  GET_FALLBACK_MESSAGE_LIST: '/get_fallback_message_list',
  GET_FALLBACK_MESSAGE_LIST_NO_AUTH: '/get_fallback_message_list_no_auth',
  DEFAULT_FALLBACK_MESSAGE_LIST: '/default_fallback_message_list'
};

export const CALLFLOW = {
  CALLFLOW_MAP: '/callflow_map_insert',
  GET_CALLFLOW_MAP: '/callflow_map_get',
  UPDATE_CALLFLOW_MAP: '/callflow_map_update',
  DELETE_CALLFLOW_MAP: '/callflow_map_delete',
  CALLFLOW_MAP_GET_LIST: '/callflow_map_get_list',
  CALLFLOW_SETTING: '/callflow_setting',
  GET_CALLFLOW_SETTING: '/callflow_setting_get',
};

export const CHATS = {
  CREATE_CHAT_CUSTOMERS: '/create_chat_customers',
  GET_CHAT_CUSTOMERS: '/get_chat_customers',
  GET_WEBCHAT_WIDGET_LIST: '/get_webchat_widget_list',
  CREATE_WEBCHAT_WIDGET: '/create_webchat_widget',
  GET_CHAT_WIDGET_BY_ID: '/get_chat_widget_by_id',
  UPDATE_WEB_CHAT_WIDGET: '/update_web_chat_widget',
  UPDATE_WEB_CHAT_WIDGET_STATUS: '/update_web_chat_widget_status',
  IS_VALID_WEB_CHAT_WIDGET_NAME: '/is_valid_web_chat_widget_name',
  CREATE_GROUP_CO_CHATS: '/create_group_co_chat',
  GET_GROUP_CO_CHATS: '/get_group_co_chat',
  GET_GROUP_CO_CHATS_ID: '/get_group_co_chat_id/:uid',
  SEND_MESSAGE_TO_CO_CHATS: '/sent_message_to_co_chat/:uid/:channelId',
  CLIENT_CHAT_WIDGET_DETAILS: '/client_chat_widget_details',
  GET_BOT_NAMES: '/get_bot_details',
  GET_COCHAT_LIST: '/get_coChat_list',
  GET_COCHAT_LIST_BY_ID: '/get_coChat_list_id',
  STORE_COCHAT_MESSAGE: '/store_coChat_message',
  INSERT_QUICK_ACTIONS_LIST: '/insert_quick_action_list',
  GET_QUICK_ACTIONS_LIST: '/get_insert_actions_list',
  UPDATE_QUICK_ACTION_LIST: '/update_quick_action_list',
  GET_NODES_LABEL_LIST: '/get_nodes_label_list',
  INSERT_UPDATE_TRIGGER_MESSAGE: '/insert_update_trigger_message',
  GET_TRIGGER_MESSAGE: '/get_trigger_message',
  TOP_UP_GENERATOR: '/top_up_generator',
  GET_CUSTOMER_CONTACT_FIELD: '/get_customer_contact_field',
  DELETE_CHAT_WIDGET_CONFIGURATION: '/delete_chat_configuration',
  CLONE_WEB_CHAT_CONFIGURATION: '/clone_web_chat_configuration',
  INSERT_UPDATE_CONCURRENT_CHAT: '/insert_update_concurrent_chat',
  GET_CONCURRENT_CHAT: '/get_concurrent_chat',
  GET_AGENT_TRACKER: '/get_agent_tracker',
  UPDATE_IS_AGENT_CONCURRENT_CHAT: '/update_is_agent_current_chat',
  INSERT_UPDATE_EMAILFLOW_HISTORY: '/insert_update_emailFlowHistory',
  ASSIGN_CALLBACK_CHAT: '/assign_callback_chat',
  GET_CALLBACK_LIST_NOTIFICATION: '/get_callback_list_notification',
  INSERT_UPDATE_CHAT_VARIABLE: '/insert_update_chat_variable',
  GET_CHAT_VARIABLES: '/get_chat_variables',
  GET_ALL_SESSION_MESSAGE: '/get_all_session_message',
  GET_COMPANY_DOMAIN_URL: '/get_company_domain_url',
  INSERT_BULK_IRRELEVANT_QUESTIONS: '/insert_bulk_irrelevant_question',
  GET_CUSTOMER_LATEST_MSG: '/get_customer_latest_msg/:customerId',
  ADD_CHAT_FLOW_LANGUAGE: '/add_chat_flow_language',
  GET_CHAT_FLOW_LANGUAGE: '/get_chat_flow_language',
  SET_CHAT_FLOW_LANGUAGE: '/set_chat_flow_language'
};
export const BOTSETTINGS = {
  CREATE_QUESTION: '/create_question_answer',
  UPDATE_QUESTION: '/update_question',
  DELETE_QUESTION: '/delete_question_answer',
  GET_QUESTION_BY_ID: '/get_question_answer_by_id',
  GET_QUESTION_LIST: '/get_question_answer_list',
  SEARCH_QUESTION_ANSWER: '/search_question_answer',
  BULK_UPLOAD_QUESTION_LIST: '/upload_question_answer',
  CREATE_WEB_CRAWLER: '/create_web_crawler',
  UPDATE_CHAT_FEEDBACK_COUNT: '/update_chat_feedback_count',
  CREATE_SURVEY_FEEDBACK: '/create_survey_feedback_count',
  DELETE_SURVEY_FEEDBACK: '/delete_survey_feedback_count',
  GET_SURVEY_FEEDBACK_BY_ID: '/get_survey_feedback_by_id',
  GET_SURVEY_FEEDBACK_LIST: '/get_survey_feedback_list',
  GET_QUESTION_ANSWER_CFID: '/get_question_answer_by_cfid',
  GET_QA_WEBSITEID: '/get_qa_wesiteid',
  GET_QA_DOMAINID: '/get_qa_list_domainid/:type?',
  GET_QUESTION_LIST_AUTH: '/get_question_answer_list_auth',
  INSERT_UPDATE_IRRELEVANT_QA: '/insert_update_irrelevant_qa',
  GET_QA_IRRELAVANTQA: '/get_update_irrelevant_qa',
  GET_KBQUESTION_ANSWER_CFID: '/get_kbquestion_answer_by_cfid',
  GET_KBQA_IRRELAVANTQA: '/get_update_irrelevant_qakb',
  DELETE_IRRELEVANT_QA: '/delete_irrelevant_qa',
  SEND_CALLBACK_EMAIL_USER: '/send_callback_email_user',
  GET_QUESTION_NO_AUTH: '/get_question_no_auth',
  GET_QA_SUGGESTION_API: '/get_qa_suggestion_api'
};

export const SMS = {
  GET_SUPERVISIOR_LIST: '/get_supervisor_list',
};

export const AGENT = {
  INSERT_AGENT_STATUS: '/insert_agent_status',
  GET_AGENT_STATUS: '/get_agent_status',
  UPDATE_AGENT_STATUS: '/update_agent_status',
  DELETE_AGENT_STATUS: '/delete_agent_status',
  GET_AGENT_STATUS_DETAILS: '/get_agent_status_details',
  IS_VALID_AGENT_NAME: '/is_valid_agent_status_name',
  UPDATE_AGENT_STATUS_VALUE: '/update_agent_status_value',
  GET_AGENT_STATUS_DESC: '/get_agent_status_desc',
  GET_EMAIL_HISTORY_DETAILS: '/get_email_history_details',
};

export const CUSTOMER = {
  INSERT_CUSTOMER_TAG: '/insert_customer_tag',
  GET_CUSTOMER_TAG: '/get_customer_tag',
  GET_CUSTOMER_TAG_DETAILS: '/get_customer_tag_details',
  UPDATE_CUSTOMER_TAG: '/update_customer_tag',
  DELETE_CUSTOMER_TAG: '/delete_customer_tag',
  IS_VALID_CUSTOMER_TAG: '/is_valid_customer_tag',
  CUSTOMER_TAG_DISABLE: '/customer_tag_disable',
  GET_CUSTOMER_TAG_DISABLE: '/get_customer_tag_disable',
  GET_MOBILE_NUMBER_STATUS: '/get_mobile_number_status',
  CREATE_CUSTOM_CONTACT: '/create_custom_contact',
  UPDATE_CUSTOM_CONTACT: '/update_custom_contact',
  GET_CUSTOM_CONTACT: '/get_custom_contact',
  DELETE_CUSTOM_CONTACT: '/delete_customer_contact',
  GET_CUSTOM_CONTACT_NO_AUTH: '/get_custom_contact_no_auth',
  CREATE_CUSTOM_CONTACT_NO_AUTH: '/create_custom_contact_no_auth',
  GET_PDF_PAGE_FINDER: '/get_pdg_page_finder'
};

export const CUSTOMER_TYPE = {
  INSERT_UPDATE_CUSTOMER_TYPE: '/insert_update_customer_type',
  GET_CUSTOMER_TYPE_LIST: '/get_customer_type_list',
  DELETE_CUSTOMER_TYPE: '/delete_customer_type/:id'
};

export const CONTACT = {
  INSERT_CUSTOMER_CONTACT: '/insert_customer_conatct',
  INSERT_CUSTOMER_CONTACT_MYACC: '/insert_customer_conatct_myacc',
  GET_CUSTOMER_CONTACT: '/get_customer_conatct/:newCustomer',
  GET_CUSTOMER_CONTACT_DETAILS: '/get_customer_conatct_details',
  UPDATE_CUSTOMER_CONTACT: '/update_customer_contact',
  DELETE_CUSTOMER_CONTACT: '/delete_customer_contact',
  IS_VALID_CONTACT: '/is_valid_contact',
  GET_BLOCK_CUSTOMER_CONTACT: '/get_block_customer_contact',
  GET_DONOTDISTURB_CUSTOMER_CONTACT: '/get_donotdisturb_customer_contact',
  INSERT_MERGE_CUSTOMER_CONTACT: '/insert_merge_customer_contact',
  GET_BLOCK_CUSTOMER_CONTACT_REQUEST: '/get_block_customer_contact_request',
  GET_DONOTDISTURB_CUSTOMER_CONTACT_REQUEST: '/get_donotdisturb_customer_contact_request',
  UPDATE_BLOCK_CUSTOMER: '/update_block_customer',
  UPDATE_DONOTDISTURB_CONTACT: '/update_donotdisturb_customer',
  UPDATE_BLOCK_CUSTOMER_REQUEST: '/update_block_customer_request',
  UPDATE_DONOTDISTURB_CONTACT_REQUEST: '/update_donotdisturb_customer_request',
  INSERT_BULK_CUSTOMER_CONTACT: '/insert_bulk_customer_contact',
  GET_CUSTOMER_CONTACT_PHONENUMBER: '/get_customer_contact_phoneNumber/:phoneNumber',
  CREATE_EDIT_COLUMN_CONTACT: '/create_edit_column_contact',
  GET_EDIT_COLUMN_CONTACT: '/get_edit_column_contact',
  UPDATE_CUSTOMER_CONTACT_TAG: '/update_customer_contct_tag',
  INSERT_UPDATE_CUSTOMER_CONTACT: '/insert_update_customer_contact',
  INSERT_UPDATE_CUSTOMER_CONTACT_NOAUTH: '/insert_update_customer_contact_noauth',
  INSERT_UPDATE_CUSTOMER_CONTACT_CALLBACK: '/insert_update_customer_contact_callback',
  GET_CUSTOMER_CONTACT_BY_EMAIL: '/get_customer_contact_by_email',
  GET_CUSTOMER_CONTACT_BY_PHONENUMBER: '/get_customer_contact_by_phonenumber',
  GET_CUSTOMER_CONTACT_CHANNEL: '/get_customer_contact_channel',
  DELETE_MULTIPLE_CUSTOMER_CONTACT: '/delete_multiple_customer_contact',
  GET_CUSTOMER_CONTACT_FIELD_TYPE: '/get_customer_contact_field_type',
  GET_CUSTOMER_CONTACT_FIELD: '/get_customer_contact_field/:screenId/:isArchive',
  GET_CUSTOMER_CONTACT_BY_COID: '/get_customer_contact_by_coid/:coid',
  GET_CUSTOMER_CONTACT_PAGINATION: '/get_customer_conatct_pagination',
  GET_CUSTOMER_CONTACT_BY_DATE: '/get_customer_conatct_by_date',
  CRM_INSERT_CONTACT: '/crm_insert_contact',
  GET_AI_CONSERVATION:'/get_ai_coversation/:sessionId',
  GET_MOBILE_CONTACT_LOG: '/get_mobile_contact_log',
  GET_CUSTOMER_TICKET_LOG: '/get_customer_ticket_log',
  GET_CUSTOMER_APPOINMENT_BOOKING_LOG: '/get_customer_appoinment_booking_log',
};

export const VIDEO_MEET = {
  CREATE_AGENT_VIDEOMEET: '/create_agent_videomeet',
  GET_AGENT_MEETING: '/get_agent_meeting/:uuid',
  CREATE_GUEST_USER_AUTH: '/create_guest',
  VIDEO_MEET_SETTINGS: '/video_meet_settings',
  VIRTUAL_BACKGROUND_URLS: '/virtual_background_urls',
  INSERT_VIDEO_RECORD_TRANSCRIPT: '/insert_videomeet_record_transcript',
  INSERT_VM_VERIFICATION_ATTACHMENTS: '/insert_videomeet_verification_attachments'
};

export const CUSTOMER_SATISFACTION = {
  INSERT_CUSTOMER_SATISFICATION: '/insert_customer_satisfication',
  GET_CUSTOMER_SATISFICATION_RATING_DESIGN: '/get_customer_satisfication_rating',
  GET_CUSTOMER_SATISFICATION_CHANNEL_STATUS: '/get_customer_satisfication_channel_status',
  INSERT_UPDATE_CUSTOMER_FEEDBACK: '/insert_update_customer_feedback',
  GET_CUSTOMER_SATISFICATION_CHANNEL: '/get_customer_satisfication_channel',
  INSERT_UPDATE_INTERACTION_NOTES: '/insert_update_interaction_notes',
  GET_INTERACTIONOVERALL_NOTES: '/get_interactionOverallNotesInfo',
  GET_AGENT_INTERACTION_NOTES_INFO: '/get_agent_Interaction_Notes_Info'

};

export const CONVERSATION_TRIGGERS = {
  INSERT_UPDATE_CONVERSATION_TRIGGERS: '/insert_update_conversation_triggers',
  GET_CONVERSATION_TRIGGERS: '/get_conversation_triggers/:triggerid',
  DELETE_CONVERSATION_TRIGGERS: '/delete_conversation_triggers/:triggerid',
  Frontend_Logger_Details: 'logger_error_details',
};

export const RESPONSE = {
  user_insert_success: 'User Inserted successfully',
  User_update_success: 'User update successfully',
  delete_user: 'Delete user successfully',
  contact_not_deleted: 'Contact Not Deleted successfully',
  not_deleted: 'User Not deleted',
  user_deleted_successfully: 'User delete successfully',
  invalid_password: 'Please enter the valid Password',
  invalid_email: 'Please enter the valid Email',
  not_found: 'No data found',
  bad_request: 'Bad Request',
  should_not_be_empty: 'EmailId should not be empty',
  internal_error: 'Internal server error',
  login_insert: 'Login successfully',
  please_provider: 'Please provide all the feild',
  queue_insert_success: 'Queue insert successfully',
  did_purchase_insert: 'did purchase insert successfully',
  did_purchase_get: 'did purchase get successfully',
  did_updated_successfully: 'did purchase update successfully',
  not_update: 'not update',
  update_recording: 'update recording successfully',
  did_delete_successfully: 'did purchase delete successfully',
  success_message: 'success',
  Queue_update_successfully: 'Queue update successfully',
  Queue_delete_successfully: 'Queue delete successfully',
  routing_insert_successfully: 'routing insert successfully',
  routing_updated_successfully: 'routing updated successfully',
  routing_deleted_successfully: 'routing deleted successfully',
  mail_sent_successfully: 'Mail sent successfully',
  mail_not_sent: 'Mail not sent',
  password_not_changed: 'Password Not updated',
  password_empty: 'Password can not empty',
  mail_not_empty: 'Email can not empty',
  pass_changed_successfully: 'Password Changed Successfully',
  skill_insert_successfully: 'skill insert successfully',
  skill_updated_successfully: 'skill updated successfully',
  skill_deleted_successfully: 'skill deleted successfully',
  campiagn_insert_successfully: 'campiagn insert successfully',
  campiagn_updated_successfully: 'campiagn updated successfully',
  campiagn_deleted_successfully: 'campiagn deleted successfully',
  channel_insert: 'account channel insert successfully',
  channel_updated_successfully: 'account channel update successfully',
  delete_successfully: 'account channel delete successfully',
  recovery_link_successfully: 'recovery link send successfully',
  portform_insert_success: 'portform insert successfully',
  User_already_exist: 'User already exist',
  failed: 'failed',
  create_bot: 'creating bot successfully',
  bot_updated_successfully: 'bot updated successfully',
  bot_deleted_successfully: 'bot deleted successfully',
  callflow_insert_successfully: 'callflow insert successfully',
  callflow_updates_successfully: 'callflow updated successfully',
  callflow_deleted_successfully: 'callflow deleted successfully',
  state_updated_successfully: 'state_updated_successfully',
  customHours_insert_success: 'customHours_insert_successfully',
  customHours_update_successfully: 'customHours_update_successfully',
  customHours_delete_successfully: 'customHours deleted successfully',
  available_number_insert: 'available_number_insert',
  create_sms: 'create sms successfully',
  sms_updated_successfully: 'smsTemplate updated successfully',
  sms_template_deleted_successfully: 'sms template deleted successfully',
  create_insert_successfully: 'create email insert successfully',
  updated_available_number: 'updated available number successfully',
  email_updated_successfully: 'email updated successfully',
  create_Email: 'create sms successfully',
  Email_updated_successfully: ' email Template updated successfully',
  email_template_deleted_successfully: 'email Template deleted successfully',
  email_deleted_successfully: 'email deleted successfully',
  create_chat_customer: 'create chat customers successfully',
  Business_Hours: 'The Business Hours is already Mapped to Queue and CallFlow Hence Cannot Be Deleted',
  Business: 'something',
  cid: 'cid number should not be empty',
  qid: 'qid number should not be empty',
  aid: 'aid number should not be empty',
  coid: 'coid number should not be empty',
  rid: 'rid number should not be empty',
  email: 'email number should not be empty',
  phoneNumber: 'phoneNumber number should not be empty',
  websiteId: 'websiteId number should not be empty',
  Disabled: 'ID cannot be disabled',
  IdDisabled: 'Disabled successfully',
  sid: 'sin number should not be empty',
  activate: 'Are you sure you want to deactivate the skill ?',
  deactivate: "The Skill is Already Mapped to the agent can't deactivate the skill",
  stataus_updated_successfully: 'stataus updated successfully',
  available: 'available',
  name_already_exist: 'name already exist',
  email_already_exist: 'email already exist',
  Queue: 'The queue is already Mapped',
  queueMap: 'The queue is not Mapped',
  status_update_successfully: 'status update successfully',
  agent_status: 'insert agent status successfully',
  agent_statuss: 'update agent status successfully',
  agentt_statuss: 'delete agent status successfully',
  create_customer_type: 'insert customer type successfully',
  update_customer_type: 'update customer type successfully',
  delete_customer_type: 'deleted customer type successfully',
  delete_customer_field: 'deleted field',
  exists_customer_type: 'customer type already exists',
  customer_tag: 'insert customer tag successfully',
  customerr_tag: 'update customer tag successfully',
  customerr_tagg: 'delete customer tag successfully',
  agent_status_updated: 'agent status updated successfully',
  customerD: 'insert customer tag',
  customerA: 'update customer tag',
  contact: 'insert customer contact successfully',
  merge_contact: 'merge customer contact successfully',
  not_merge: 'not merge customer contact',
  update_contact: 'update customer contact successfully',
  update_block: 'update customer block successfully',
  update_doNot: 'update customer donotdistrup successfully',
  delete_contact: 'delete customer contact successfully',
  insert_success: 'insert successfully',
  updated_success: 'updated successfully',
  create_webChat_widget: 'created webchat widget successfully',
  update_webChat_widget: 'update webchat widget successfully',
  create_group_co_chat: 'created group chat successfully',
  send_message: 'send message successfully',
  insert_customer: 'insert bulk customer conatct successfully',
  insert_customer_marketing: 'insert bulk customer conatct in marketing successfully',
  insert_shortcutkeys: 'insert shortcutkeys successfully',
  not_authorize_to_change: 'not_authorize_to_change',
  shortcut_keys_updated: 'shortcut_keys_updated',
  insert_session_history: 'insert session History successfully',
  update_session_history: 'updated session History successfully',
  delete_question_message: 'question answer deleted successfully',
  sms_send: 'sms send successfully',
  invalid_chatId: 'Please provide the chatflow id',
  ticket_success: 'ticket id updated successfully',
  update_asssign: 'update assign callback successfully',
  bot_status: 'bot status update successfully',
  contactExists: 'Phone number/ Email is already exists in DB.I',
  sip_delete_successfully: 'sip deleted successfully',
  please_select_articleId: 'Please select tthe article Id',
  please_provide_webId: 'Please provide webId',
  insertConcurrent: 'Insert concurrent successfully',
  updateConcurrent: 'Update concurrent successfully',
  insertEmail: 'Added email flow history successfully',
  updateEmail: 'Updated email flow history successfully',
  CustomerSatisfication: 'insert Update customer satisfication',
  Customerfeedback: 'insert Update customer feedback',
  ConversationTriggers: 'insert conversation triggers successfully',
  delete_conversation_trigger: 'conversation trigger deleted successfully',
  insert_flow_language: 'Chat flow language added successfully',
  updated_flow_language: 'Chat flow language updated successfully',
  failed_flow_language: 'Chat flow language not inserted / updated',
  otp_process_success: 'OTP sent successfully',
  otp_process_failed: 'OTP process failed / Check your input',
  otp_verification_failed: 'OTP expired/incorrect',
  Business_Hours_Insert_successfully: 'Business Hours Inserted successfully',
  Business_Hours_Insert_Error: 'Business Hours Inserted error',
  Get_Business_Hours: 'Get Business Hours successfully',
  Delete_Business_Hours: 'Delete Business Hours successfully',
  Update_Business_Hours_Status: 'Update Business Hours successfully',
  Delete_Failed: 'Something went wrong. Failed to delete. Please try again'
};

const currentDate = moment(new Date()).format('YYYY-MM-DD HH:Added email flow history successfullymm:ss');
export const INPUT = {
  hours_of_operation: {
    businessHoursFile: {
      cid: 1,
      name: 'Default Hours',
      description: 'The times during which the center is open and able to take calls or handle customer inquiries',
      hours:
        '[{"day":"Monday","isHoliday":false,"entry":[{"startTimeStamp":"1670902210292","endTimeStamp":"1670934610292","differenceTime":"","id":1}]},{"day":"Tuesday","isHoliday":false,"entry":[{"startTimeStamp":"1670902210292","endTimeStamp":"1670934610292","differenceTime":"","id":1}]},{"day":"Wednesday","isHoliday":false,"entry":[{"startTimeStamp":"1670902210292","endTimeStamp":"1670934610292","differenceTime":"","id":1}]},{"day":"Thursday","isHoliday":false,"entry":[{"startTimeStamp":"1670902210292","endTimeStamp":"1670934610292","differenceTime":"","id":1}]},{"day":"Friday","isHoliday":false,"entry":[{"startTimeStamp":"1670902210292","endTimeStamp":"1670934610292","differenceTime":"","id":1}]},{"day":"Saturday","isHoliday":true,"entry":[{"startTimeStamp":"","endTimeStamp":"","differenceTime":"","id":1}]},{"day":"Sunday","isHoliday":true,"entry":[{"startTimeStamp":"","endTimeStamp":"","differenceTime":"","id":1}]}]',
      timeZone: 'Based on Purchase Region',
      default_hours: '9',
      createdAt: currentDate,
    },
  },
  announcement: {
    type: 0,
    textToSpeech: '',
    AnnouncementValue: 'No Announcement',
    placeHolder: '',
  },
  maximumCallers: {
    type: 'No Limit',
    limitResponseType: 0,
    queueLimit: 0,
    overFlowQueueName: '',
    overFlowQueue: 0,
  },
  maximumWaitTime: {
    type: 'Keep Waiting',
    maxWaitTime: 0,
    fallBackQueue: 0,
    file: {},
    timeElapsed: 0,
    timeElapsedValue: '',
    fallBackQueueName: '',
    fileName: '',
  },
  routingqueue: [
    {
      qid: 278,
      voice: 1,
      chat: 0,
      priority: 1,
      delay_in_seconds: 0,
    },
  ],
  nodes: [
    {
      id: 'entryNode_p3Lly3Rl4',
      type: 'entryPoint',
      data: {
        label: 'inputsdsd node',
      },
      position: {
        x: 250,
        y: 5,
      },
      width: 260,
      height: 121,
    },
    {
      id: 'playPrompt_ClmtTGxih',
      type: 'playPrompt',
      position: {
        x: 150,
        y: 255,
      },
      data: {
        type: 'textToSpeech',
        message: 'Welcome to Vectone Mobiles',
        chatMessage: '',
        isChat: false,
      },
      width: 260,
      height: 121,
      selected: false,
    },
    {
      id: 'transferToQueue_uy_IYn04q',
      type: 'transferToQueue',
      position: {
        x: 195,
        y: 465,
      },
      data: {
        type: 'Transfer to Queue',
        Queue: {
          qid: 335,
          name: 'stage testing queue 28V1',
          description: 'stage testing queue on 28/12/2022',
          domainId: 25694,
          createdAt: '2022-12-29T09:05:03.000Z',
          updatedAt: null,
          settingAudio:
            '{"connectingAudio":{"ConnectingAudioFile":{"fieldname":"doc","encoding":"7bit","mimetype":"audio/wav","filename":"ConnectingAudio.wav","path":"/volume/nfsshare/ccaas/prompt/25694/ConnectingAudio.wav","size":288044,"uuid":"abf71855-1e7a-45fd-b950-b11075dad780","type":"upload","promptType":"0","url":"https://ccaas-storage.worktual.co.uk/prompt/25694/ConnectingAudio.wav","name":"connecting audio","description":"connecting audio file","domainId":"25694","companyId":7763,"message":"","createdat":"2022-12-08T08:34:20.409Z","__v":0},"ConnectingAudioName":"connecting audio"},"greetingAudio":{"greetingAudioFile":{"fieldname":"doc","encoding":"7bit","mimetype":"audio/wav","filename":"Greeting tone.wav","path":"/volume/nfsshare/ccaas/prompt/25694/Greeting tone.wav","size":163660,"uuid":"974115f2-6130-429a-bf16-d1c897a3b852","type":"upload","promptType":"0","url":"https://ccaas-storage.worktual.co.uk/prompt/25694/Greeting tone.wav","name":"Greeting tone","description":"Greeting tone file","domainId":"25694","companyId":7763,"message":"","createdat":"2022-12-08T08:35:24.870Z","__v":0},"greetingAudioName":"Greeting tone"},"queueAudio":{"queueAudioFile":{"fieldname":"doc","encoding":"7bit","mimetype":"audio/wav","filename":"QueueAudio.wav","path":"/volume/nfsshare/ccaas/prompt/25694/QueueAudio.wav","size":911626,"uuid":"9cb83c28-9a63-4e46-a03d-089d6d5d2207","type":"upload","promptType":"0","url":"https://ccaas-storage.worktual.co.uk/prompt/25694/QueueAudio.wav","name":"Queue Audio","description":"Queue Audio file","domainId":"25694","companyId":7763,"message":"","createdat":"2022-12-08T08:36:52.387Z","__v":0},"queueAudioName":"Queue Audio"},"holdAudio":{"holdAudioFile":{"fieldname":"doc","encoding":"7bit","mimetype":"audio/wav","filename":"Classical.wav","path":"/volume/nfsshare/ccaas/prompt/25694/Classical.wav","size":311340,"uuid":"7e424bc7-1b26-480d-b161-77fcd810add3","type":"upload","promptType":"0","url":"https://ccaas-storage.worktual.co.uk/prompt/25694/Classical.wav","name":"Hold tone","description":"Hold tone file","domainId":"25694","companyId":7763,"message":"","createdat":"2022-12-08T08:36:16.715Z","__v":0},"holdAudioName":"Hold tone"}}',
          announcement: '{"type":0,"textToSpeech":"","AnnouncementValue":"No Announcement","placeHolder":""}',
          maximumCallers: '{"type":"No Limit","limitResponseType":0,"queueLimit":0,"overFlowQueueName":"","overFlowQueue":0}',
          maximumWaitTime:
            '{"type":"Keep Waiting","maxWaitTime":0,"fallBackQueue":0,"file":{},"timeElapsed":0,"timeElapsedValue":"","fallBackQueueName":"","fileName":""}',
          OutboundCallerId: 'vectone stage 28v1',
          outBoundNumber: 9876,
          maxCallHandlingTimeMM: 10,
          maxCallHandlingTimeSS: null,
          callWrapUpTime: 2,
          outBoundCallStatus: 1,
          hoursOfOperatioId: 83,
          hours_of_operation:
            '{"businessHoursFile":{"cid":83,"name":"Default hours","description":"dfghsfghsf","hours":"[{"day":"Monday","isHoliday":false,"entry":[{"startTimeStamp":"1670470224915","endTimeStamp":"1670502624915","differenceTime":"","id":1}]},{"day":"Tuesday","isHoliday":false,"entry":[{"startTimeStamp":"1670470224915","endTimeStamp":"1670502624915","differenceTime":"","id":1}]},{"day":"Wednesday","isHoliday":false,"entry":[{"startTimeStamp":"1670470224915","endTimeStamp":"1670502624915","differenceTime":"","id":1}]},{"day":"Thursday","isHoliday":false,"entry":[{"startTimeStamp":"1670470224915","endTimeStamp":"1670502624915","differenceTime":"","id":1}]},{"day":"Friday","isHoliday":false,"entry":[{"startTimeStamp":"1670470224915","endTimeStamp":"1670502624915","differenceTime":"","id":1}]},{"day":"Saturday","isHoliday":true,"entry":[{"startTimeStamp":"","endTimeStamp":"","differenceTime":"","id":1}]},{"day":"Sunday","isHoliday":true,"entry":[{"startTimeStamp":"","endTimeStamp":"","differenceTime":"","id":1}]}]","timeZone":"Africa/Addis Ababa (EAT) GMT +03:00","domainId":25694,"default_hours":"9","createdAt":"2022-12-08T07:06:37.000Z"}}',
          outOfBusinessHours:
            '{"busineHoursType":0,"busineHoursValue":"Announcement","fileName":"Greeting tone","file":{"fieldname":"doc","encoding":"7bit","mimetype":"audio/wav","filename":"Greeting tone.wav","path":"/volume/nfsshare/ccaas/prompt/25694/Greeting tone.wav","size":163660,"uuid":"974115f2-6130-429a-bf16-d1c897a3b852","type":"upload","promptType":"0","url":"https://ccaas-storage.worktual.co.uk/prompt/25694/Greeting tone.wav","name":"Greeting tone","description":"Greeting tone file","domainId":"25694","companyId":7763,"message":"","createdat":"2022-12-08T08:35:24.870Z","__v":0}}',
          status: 1,
        },
        skill: ['Solving skill'],
        externalNumber: '',
      },
      width: 260,
      height: 193,
      selected: true,
    },
    {
      id: 'disconnect_l4D1aXM8x',
      type: 'disconnect',
      position: {
        x: 510,
        y: 345,
      },
      data: {
        label: 'disconnect_node',
      },
      width: 260,
      height: 85,
    },
  ],
  edges: [
    {
      source: 'entryNode_p3Lly3Rl4',
      sourceHandle: 'entry_source_entryNode_p3Lly3Rl4',
      target: 'playPrompt_ClmtTGxih',
      targetHandle: 'playPrompt_ClmtTGxih',
      type: 'buttonedge',
      id: 'reactflow__edge-entryNode_p3Lly3Rl4entry_source_entryNode_p3Lly3Rl4-playPrompt_ClmtTGxihplayPrompt_ClmtTGxih',
    },
    {
      source: 'playPrompt_ClmtTGxih',
      sourceHandle: 'playPrompt_ClmtTGxih',
      target: 'transferToQueue_uy_IYn04q',
      targetHandle: 'transferToQueue_uy_IYn04q',
      type: 'buttonedge',
      id: 'reactflow__edge-playPrompt_ClmtTGxihplayPrompt_ClmtTGxih-transferToQueue_uy_IYn04qtransferToQueue_uy_IYn04q',
    },
    {
      source: 'transferToQueue_uy_IYn04q',
      sourceHandle: 'at capactiy',
      target: 'disconnect_l4D1aXM8x',
      targetHandle: 'b',
      type: 'buttonedge',
      id: 'reactflow__edge-transferToQueue_uy_IYn04qat capactiy-disconnect_l4D1aXM8xb',
    },
  ],
};

export const ROLES = {
  OWNER_ROLE_ID: '1',
  SUB_OWNER_ROLE_ID: '2',
  ADMIN_ROLE_ID: '3',
  MEMBER_ROLE_ID: '4',
  SUPERVISOR_ROLE_ID: '5',
  AGENT_ROLE_ID: '6',
};

export const channelTypeCode = {
  Call: 2003001,
  Chat: 2003002,
  Email: 2003003,
  SMS: 2003004,
  Whatsapp: 2003005,
  Facebook: 2003006,
  Instagram: 2003007
};

export const INTERNAL_CHAT = {
  CREATE: '/create_internal_chat',
  GET_CHAT: '/internal_chat',
  GET_PARTICULAR_CHAT: '/particular_internal_chat',
  EDIT_CHAT: '/edit_internal_chat',
  DELETE_CHAT: '/delete_internal_chat',
  MARK_ALL_AS_READ_CHAT: '/read_internal_chat',
  GET_CHAT_HISTORY_ID: '/internal_chat_history_id',
};

export const BROADCAST_CHAT = {
  INSERT_BROADCAST_GROUP: '/create_broadcast_group',
  GET_ALL_GROUP_DETAILS: '/get_all_group',
  GET_BROADCAST_AGENT_DETAILS: '/get_broadcast_agent_details',
  DELETE_BROADCAST_AGENT_DETAILS: '/delete_broadcast_agent_details',
  INSERT_BROADCAST_GROUP_MSG: '/insert_broadCast_groupmsg',
  GET_BROADCAST_GROUP_MSG: '/get_broadcast_groupmsg',
  DELETE_BROADCAST_GROUP_MSG: '/delete_broadcast_groupmsg'
};

export const KNOWLEDGEBASE = {
  INSERT_KNOWLEDGE_BASE: '/create_knowledge_base',
  GET_KNOWLEDGEBASE_CATEGORY_ARTICLE_DTL: '/get_knowledge_base_category_article_dtl/:knowledgeBaseType?',
  GET_KNOWLEDGEBASE_ARTICLE_DTL: '/get_knowledge_base_article_dtl/:knowledgeBaseType/:categoryId',
  UPDATE_KNOWLEDEBASE_CATEGORY_DTL: '/update_knowledge_base_category_dtl',
  DELETE_KNOWLEDGEBASE_DTL: '/delete_knowledgeBase_dtl',
  UPDATE_MOVE_FILES_TO_FOLDER: '/update_move_files_to_folder'
};

export const CRM_DOMAINS = {
  LORIO: '19969'
};

export const MYPLANS = {
  MY_PLANS_PROFILE_INFO: '/get_my_plans_profile_info', // not
  USER_MANAGEMENT_LIST: '/get_my_plans_user_management_list', // not
  ORDER_HIISTORY: '/get_my_plans_order_history', // not
  SUBSCRIPTION_MANAGEMENT: '/get_my_plans_subscription_management', // not
  GET_ADDON_LIST: '/get_my_plans_addon_list',  // starts
  GET_USER_PASSWORD_INFO: '/get_user_password_info', // not
  GET_SYNC_UP_MYACCOUNT_DETAILS: '/get_syncup_myaccount_details', // not
  CREATE_MYACCOUNT_SYNCUP_DETAILS: '/create_myaccount_syncup_details', // not
  GET_USER_DATA_PRIVACY_SETTING: '/get_user_data_privacy_setting', // not
  INSERT_UPDATE_USERDATA_PRIVACY_SETTING: '/insert_update_data_privacy_setting', // not
  GET_USER_DEVICES: '/get_user_devices', // not
  INSERT_UPDATE_USER_DEVICES: '/insert_update_user_devices', // not
  GET_USER_ROLES: '/get_user_roles', // not
  USER_GET_DETAILS: '/get_user_details', // not
  GET_USER_DEPARTMENT: '/get_user_department', // not
  GET_USER_LOGIN_LOG: '/get_user_login_log', // not
  GET_ADDON_PURCHASE_LIST:'/get_addOn_purchase_list', // not
  GET_ROLES_INFO: '/get_roles_info', // not
  INSERT_UPDATE_TWO_STEP_VERIFICATION:'/insert_update_two_step_verification', // not
  GET_SECURITY_TWO_STEP_VERIFCATION : '/get_security_two_step_verification', // not
  GET_USER_ONBOARD_SETTING_COMPLETION : '/get_user_onboard_settings_completion', // cc
  CCAAS_GET_LIVE_INTERACTION : '/get_live_interaction_ai', // cc
  INSERT_UPDATE_DATA_AND_PRIVACY_SET: '/insert_update_data_and_privacy_set', // cc
  GET_DATA_AND_PRIVACY_SET: '/get_data_and_privacy_set', // cc
  GET_ALL_MEMBERS_ROLE : '/get_all_members_role',  // not
  GET_USER_DETAILS_BY_ROLE:'/get_user_details_by_role', // cc
  INSERT_UPDATE_MYPLAN_ROLE_PERMISSION: '/insert_update_myplan_role_permission', // cc
  GET_MYPLAN_ROLE_PERMISSION: '/get_myplan_role_permission', // cc
  GET_MYPLAN_TEAMS_DETAILS: '/get_myplan_teams_details', // cc
  INSERT_UPDATE_MYPLAN_TEAM_PERMISSION: '/insert_update_myplan_team_permission', // cc
  GET_MYPLAN_TEAM_PERMISSION: '/get_myplan_team_permission', // cc
  GET_MYPLAN_TEAM_LOG_ACTIVITY: '/get_myplan_team_log_activity', // cc
  UPDATE_MYPLAN_USER_PERMISSION: '/update_myplan_user_permission', // cc
  GET_MYPLAN_USER_PERMISSION: '/get_myplan_user_permission', // cc
  GET_MYPLAN_PARTICULAR_USER_LOG_ACTIVITY: '/get_myplan_particular_user_log_activity', // cc
  INSERT_USER_ACTIVITY_LOG: '/insert_user_activity_log', // cc
  GET_USER_ACTIVITY_LOG: '/get_user_activity_log', // cc
  INSERT_MPLAN_USER_DEVICES_DTL: '/insert_user_devices_dtl', // cc
  GET_USER_DEVICES_DTL: '/get_user_devices_dtl', // cc
  COMPNAY_BILLIG_SUBSCRIPTION:'/get_company_billing_subscription',
  GET_STORAGE_DATA: '/get_storage_data',

  // forgot password
  EMAIL_OTP_VERIFICATION: '/email_otp_verification',
  CHECK_OTP_VALIDATION: '/check_otp_validation',
  UPDATE_PASSWORD: '/update_password',
  RESEND_OTP: '/resend_otp',
};

export const OUTBPUNDCAMPAIGN = {
  INSERT_UPDATE_OUTBOUND_CAMPAIGN : '/insert_update_outbound_campaign',
  GET_OUTBOUND_CAMPAIGN_DRAFT_CHAT : '/get_outbound_campaign_draft_chat',
};

export const HOME = {
  INSERT_UPDATE_HOME_CHAT_HISTORY : '/insert_update_home_chat_history',
  GET_HOME_CHAT_HISTORY: '/get_home_chat_history',
  DELETE_HOME_CHAT_HISTORY: '/delete_home_chat_history',
};
