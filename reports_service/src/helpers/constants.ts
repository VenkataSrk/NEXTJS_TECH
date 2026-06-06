/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

export const METRIX = {
  GET_REPORT_DROPDOWN: '/get_report_dropdown',
};

export const RESPONSE = {
  not_found: 'No data found',
  internal_error: 'Internal server error',
  success_message: 'success',
  empty_data: 'Empty data',
  incorrect_input: 'incorrect input data type',
  schedule_reports: 'delete schedule report successfully',
  uid: 'uid number should not be empty',
};

export const REPORTVOICE = {
  GET_ANSWERED_CALL_PROFILE_REPORT: '/get_answered_call_profile', // used in live
  MISSED_CALL_LIST_REPORT: '/get_missedCall_list', // used in live
  INTERACTION_LIST_REPORT: '/interaction_list', // used in live
  EDIT_COLUMN_FOR_REPORTS: '/edit_column_reports', // 1st
  GET_EDIT_COLUMN_LIST: '/get_edit_cloumn_list', // 2nd
  OUTBOUND_CALL_REPORT: '/outbound_call_report',  // used in live
  GET_TRANSFER_CALL_REPORT: '/transfer_call_report',  // used in live
  GET_CALL_BACK_SUMMARY_REPORT: '/get_call_back_summary' ,
  INBOUND_CALL_REPORT: '/inbound_call_report', // used in live
  // used in live
};

export const ANALYTICS = {
  EDIT_COLUMNS: '/edit_columns',
  GET_COLUMNS: 'get_columns',
  INBOUND_CHAT_ANALYSIS_METRICS: '/inbound_chat_analysis_metrics',
  CHAT_PERFORMANCE_METRICS: '/chat_performance_metrics',
  SERVICE_LEVEL_AGREEMENT_METRICS: '/service_level_agreement_metrics',
  INBOUND_CHAT_ANALYSIS_METRICS_FILTER: '/inbound_chat_analysis_metrics_filter',
  CHAT_PERFORMANCE_METRICS_FILTER: '/chat_performance_metrics_filter',
  CHAT_SERVICE_LEVEL_METRICS_FILTER: '/chat_service_level_metrics_filter',
  CHAT_PERFORMANCE_METRICS_TREND_CHAT: '/performance_metrics_trend_chat',
  INBOUND_TREND_CHAT: '/inbound_trend_chat',
  CHAT_SERVICE_LEVEL_TREND_CHAT: '/service_level_trend_chat',
  AGENT_INBOUND_CHAT_ANALYSIS: '/agent_inbound_chat_analysis',
  AGENT_SERVICE_LEVEL_METRICS: '/agent_service_level_metrics',
  GET_INBOUND_QUEUE_FIELD: '/get_inbound_queue_field',
  GET_INBOUND_AGENT_FIELD: '/get_inbound_agent_field',
  GET_INBOUND_PERFORMANCE_FIELD: '/get_agent_performance_field',
  GET_INBOUND_QUEUE_PERFORMANCE_FIELD: '/get_queue_performance_field',
  GET_INBOUND_QUEUE_SERVICE_LEVEL_FIELD: '/get_queue_service_level_field',
  GET_INBOUND_QUEUE_SERVICE_LEVEL_SLA_FIELD: '/get_queue_service_level_sla_field',
  GET_INBOUND_AGENT_SERVICE_LEVEL_FIELD: '/get_agent_level_field',
  GET_INBOUND_AGENT_SERVICE_LEVEL_SLA_FIELD: '/get_agent_level_sla_field',
  INBOUND_AGENT_TREND_CHAT: '/inbound_agent_trend_chat',
  AGENT_PERFORMANCES_TREND_CHAT: '/agent_performances_trend_chat',
  EDIT_COLUMN_QUEUE_AGENT_ANALYTICS: '/edit_column_queue_agent_analytics',
  GET_EDIT_COLUMN_QUEUE_AGENT_ANALYTICS: '/get_edit_column_queue_agent_analytics',
  QUEUE_INBOUND_CHAT_VOLUME: '/queue_inbound_chat_volume',
  AGENT_INBOUND_CHAT_VOLUME: '/agent_inbound_chat_volume',
  AGENT_INBOUND_CHAT_ANALYSIS_METRICS_FILTER: '/agent_inbound_chat_analysis_metrics_filter',
  AGENT_CHAT_PERFORMANCES_METRICS_FILTER: '/agent_chat_performances_metrics_filter',
  AGENT_CHAT_SERVICELEVEL_METRICS_FILTER: '/agent_chat_servicelevel_metrics_filter',
  AGENT_PERFORMANCE_CHAT_METRICS: '/agent_performance_chat_metrics',
  AGENT_SERVICES_LEVEL_AGREEMENT_METRIC: '/agent_services_level_agreement_metrics',
  CREATE_EDIT_COLUMN_AGENT_ANALYTICS: '/create_edit_column_agent_analytics',
  GET_COLUMN_AGENT_ANALYTICS: '/get_column_agent_analytics',
  INSERT_UPDATE_VISITOR_INSIGHT: '/insert_update_visitor_insight',
  GET_VISITOR_INSIGHT_DATA: '/get_visitor_insight_data'
};

// Agent Analytics Voice
export const AGENTANALYTICSVOICE = {
  AGENT_INBOUND_CALL_ANALYSIS_METRICS_FILTER: '/agent_inbound_call_analysis_metrics_filter',
  AGENT_OUTBOUND_CALL_ANALYSIS_METRICS_FILTER: '/agent_outbound_call_analysis_metrics_filter',
  AGENT_CALLBACK_ANALYSIS_METRICS_FILTER: '/agent_callback_analysis_metrics_filter',
  AGENT_VOICEMAIL_ANALYSIS_METRICS_FILTER: '/agent_voicemail_analysis_metrics_filter',
  AGENT_SERVICELEVEL_CALL_ANALYSIS_METRICS_FILTER: '/agent_servicelevel_call_analysis_metrics_filter',
  CREATE_EDIT_COLUMN_CALL_ANALYTICS: '/create_edit_column_call_analytics',
  GET_COLUMN_CALL_AGENT_ANALYTICS: '/get_column_call_agent_analytics',
  AGENT_CALL_INBOUND_TRENDCHART: '/agent_call_inbound_trendchart',
  AGENT_CALL_OUTBOUND_TRENDCHART: '/agent_call_outbound_trendchart',
  AGENT_CALL_SERVICELEVEL_TRENDCHART: '/agent_call_servicelevel_trendchart',
  AGENT_CALL_CALLBACK_TRENDCHART: '/agent_callback_trendchart',
  AGENT_CALL_VOICEMAIL_TRENDCHART: '/agent_voicemail_trendchart'
};

// Agent Analytics Email
export const AGENTANALYTICSEMAIL = {
  AGENT_EMAIL_INBOUND_ANALYSIS_METRICS_FILTER: '/agent_email_inbound_analysis_metrics_filter',
  AGENT_EMAIL_PERFORMANCES_ANALYSIS_METRICS_FILTER: '/agent_email_performances_analysis_metrics_filter',
  AGENT_EMAIL_SERVICELEVEL_ANALYSIS_METRICS_FILTER: '/agent_email_servicelevel_analysis_metrics_filter',
  CREATE_EDIT_COLUMN_EMAIL_AGENT_ANALYTICS: '/create_edit_column_email_agent_analytics',
  GET_COLUMN_EMAIL_AGENT_ANALYTICS: '/get_column_email_agent_analytics',
  AGENT_EMAIL_INBOUND_TRENDCHART: '/agent_email_inbound_trendchart',
  AGENT_EMAIL_PERFORMANCES_TRENDCHART: '/agent_email_performances_trendchart',
  AGENT_EMAIL_SERVICELEVEL_TRENDCHART: '/agent_email_servicelevel_trendchart'
};

// Agent Analytics SMS
export const AGENTANALYTICSMS = {
  AGENT_SMS_INBOUND_ANALYSIS_METRICS_FILTER: '/agent_sms_inbound_analysis_metrics_filter',
  AGENT_SMS_PERFORMANCES_ANALYSIS_METRICS_FILTER: '/agent_sms_performances_analysis_metrics_filter',
  AGENT_SMS_SERVICELVEL_ANALYSIS_METRICS_FILTER: '/agent_sms_servicelevel_analysis_metrics_filter',
  AGENT_SMS_INBOUND_TRENDCHART: '/agent_sms_inbound_trendchart',
  AGENT_SMS_SERVICELEVEL_TRENDCHART: '/agent_sms_servicelevel_trendchart',
  AGENT_SMS_PERFORMANCES_TRENDCHART: '/agent_sms_performances_trendchart',
  CREATE_EDIT_COLUMN_SMS_ANALYTICS: '/create_edit_column_sms_analytics',
  GET_COLUMN_SMS_AGENT_ANALYTICS: '/get_column_sms_agent_analytics',
};

// Agent Ai and Bot Analytics
export const AGENTAIBOTANALYTIC = {
  AGENT_ANALYTIC_BOT_INTERACTIONS_OVERVIEW: '/agent_analysis_bot_interaction',
  AGENT_ANALYTIC_BOT_TREND_CHART: '/get_analytics_trend_chart_info',
  AGENT_BOT_INTERACTION_CHANNEL: '/agent_bot_interaction_channel',
  AGENT_ANALYTIC_PERFORMANCE_AGENT_BOT: '/get_agent_analytic_performance_agent_bot',
  AGENT_ANALYTIC_ACTIVE_HOURS: '/get_agent_analytic_active_hours',
  AGENT_ASSIST_BOT_ANALYTICS: '/agent_assist_bot_analysis',
  AGENT_ASSIST_ANALYTICS_TREND_CHART: '/agent_assist_analytics_trend_chart',
  AGENT_ASSIST_INTERACTION_CHANNEL: '/agent_assist_interaction_channel',
  AGENT_ASSIST_ENGAGEMENTRATE_TREND_CHART: '/agent_assist_engagementrate_trend_chart',
  AGENT_ASSIST_OVERRIDERATE_INFO: '/agent_assist_overriderate_info',
  KNOWLEDGEBASE_INTERACTION_CHANNEL: '/get_knowledgebase_interaction_channel',
  KNOWLEDGEBASE_AVERAGEBASE_RESPONSEPERDAY: '/get_knowledgebase_averagebase_responseperday',
  KNOWLEDGEBASE_UTILIZATION_TRENDCHART: 'get_knowledgebase_untilization_trendchart',
  AGENT_ANALYTIC_BOT_NLUPERFORMENCES: '/agent_analytic_bot_nlu_performences',
  BOT_NLU_PERFORMENCES_HELPFUL_MESSAGE: '/get_Bot_NLU_Performences_Helpful_Message',
  INSERT_UPDATE_CHAT_INTENT: '/insert_update_chat_intent',
  GET_CHAT_INTENT: '/get_chat_intent',
  NLU_PERFORMENCES_TOP_INTENT_DISTRIBUTION: '/get_NLUPerformences_Top_Intent_Distribution',
  GET_API_CALLS_INFO: '/get_api_calls'
};

// Agent Whatsapp Analytics
export const AGENTWHATSAPPANALYTIC = {
  AGENT_WHATSAPP_INBOUND_ANALYTICS: '/agent_whatsapp_inbound_analytics',
  AGENT_WHATSAPP_PERFORMANCES_ANALYTICS: '/agent_whatsapp_performances_analytics',
  AGENT_WHATSAPP_SERVICELEVEL_ANALYTICS: '/agent_whatsapp_servicelevel_analytics',
  CREATE_EDIT_COLUMN_WHATSAPP_ANALYTICS: '/create_edit_column_whatsapp_analytics',
  GET_COLUMN_WHATSAPP_AGENT_ANALYTICS: '/get_column_whatsapp_agent_analytics',
  AGENT_WHATSAPP_INBOUND_TRENDCHART: '/agent_whatsapp_inbound_trendchart',
  AGENT_WHATSAPP_PERFORMANCES_TRENDCHART: '/agent_whatsapp_performances_trendchart',
  AGENT_WHATSAPP_SERVICELEVEL_TRENDCHART: '/agent_whatsapp_servicelevel_trendchart',
};

// Agent Facebook Analytics
export const AGENTFACEBOOKANALYTIC = {
  AGENT_FACEBOOK_INBOUND_ANALYTICS: '/agent_facebook_inbound_analytics',
  AGENT_FACEBOOK_PERFORMANCES_ANALYTICS: '/agent_facebook_performances_analytics',
  AGENT_FACEBOOK_SERVICELEVEL_ANALYTICS: '/agent_facebook_servicelevel_analytics',
  CREATE_EDIT_COLUMN_FACEBOOK_ANALYTICS: '/create_edit_column_facebook_analytics',
  GET_COLUMN_FACEBOOK_AGENT_ANALYTICS: '/get_column_facebook_agent_analytics',
  AGENT_FACEBOOK_INBOUND_TRENDCHART: '/agent_facebook_inbound_trendchart',
  AGENT_FACEBOOK_PERFORMANCES_TRENDCHART: '/agent_facebook_performance_trendchart',
  AGENT_FACEBOOK_SERVICELEVEL_TRENDCHART: '/agent_facebook_servicelevel_trendchart',
};

// Agent Instagram Analytics
export const AGENTINSTAGRAMANALYTIC = {
  AGENT_INSTAGRAM_INBOUND_ANALYTICS: '/agent_instagram_inbound_analytics',
  AGENT_INSTAGRAM_PERFORMANCES_ANALYTICS: '/agent_instagram_performances_analytics',
  AGENT_INSTAGRAM_SERVICELEVEL_ANALYTICS: '/agent_instagram_servicelevel_analytics',
  CREATE_EDIT_COLUMN_INSTAGRAM_ANALYTICS: '/create_edit_column_instagram_analytics',
  GET_COLUMN_INSTAGRAM_AGENT_ANALYTICS: '/get_column_instagram_agent_analytics',
  AGENT_INSTAGRAM_INBOUND_TRENDCHART: '/agent_instagram_inbound_trendchart',
  AGENT_INSTAGRAM_PERFORMANCES_TRENDCHART: '/agent_instagram_performance_trendchart',
  AGENT_INSTAGRAM_SERVICELEVEL_TRENDCHART: '/agent_instagram_servicelevel_trendchart',
};

// Queue Analytics Voice
export const VOICEQUEUEANALYTICS = {
  QUEUE_INBOUNDCALL_ANALYSIS_METRICS_FILTER: '/get_queue_inboundcall_analysis_metrics_filter',
  QUEUE_OUTBOUNDCALL_ANALYSIS_METRICS_FILTER: '/get_queue_outboundcall_analysis_metrics_filter',
  QUEUE_CALLBACK_VOICE_ANALYSIS_METRICS_FILTER: '/get_callback_voice_analysis_metrics_filter',
  QUEUE_VOICEMAIL_ANALYSIS_METRICS_FILTER:'/get_queue_voicemail_analysis_metrics_filter',
  QUEUE_SERVICELEVEL_ANALYSIS_METRICS_FILTER:'/get_queue_servicelevel_analysis_metrics_filter',
  QUEUE_OUTBOUNDCALL_BESTTIME_CALL_METRICS_FILTER:'/get_queue_outboundcall_besttime_call_metrics_filter',
  CREATE_EDIT_COLUMN_QUEUE_VOICE_ANALYTICS: '/create_edit_column_queue_voice_analysis',
  GET_COLUMN_QUEUE_VOICE__ANALYTICS: '/get_column_queue_voice_analysis',
  QUEUE_VOICE_INBOUND_TRENDCHART:'/get_queue_voice_inbound_trendChart',
  QUEUE_VOICE_OUTBOUND_TRENDCHART:'/get_queue_voice_outbound_trendChart',
  QUEUE_VOICE_CALLBACK_TRENDCHART:'/get_queue_voice_callback_trendChart',
  QUEUE_VOICE_OUTBOUND_BEST_TIME_TRENDCHART:'/get_queue_voice_outbound_best_time_trendChart',
  QUEUE_VOICE_SERVICELEVEL_TRENDCHART: '/get_queue_voice_servicelevel_trendChart'
};

// Queue Analytics webchat
export const WEBCHATQUEUEANALYTICS = {
  QUEUE_INBOUNDCHAT_ANALYSIS_METRICS_FILTER: '/get_queue_inboundchat_analysis_metrics_filter',
  QUEUE_CHATPERFORMANCES_ANALYSIS_METRICS_FILTER: '/get_queue_chatperformances_analysis_metrics_filter',
  QUEUE_SERVICELEVEL_ANALYSIS_METRICS_FILTER: '/get_chat_queue_servicelevel_analysis_metrics_filter',
  CREATE_EDIT_COLUMN_QUEUE_WEBCHAT_ANALYTICS: '/create_edit_column_queue_webchat_analysis',
  GET_COLUMN_QUEUE_WEBCHAT__ANALYTICS: '/get_column_queue_webchat_analysis',
  QUEUE_WEBCHAT_INBOUND_TRENDCHART:'/get_queue_webchat_inbound_trendChart',
  QUEUE_WEBCHAT_PERFORMANCES_TRENDCHART:'/get_queue_webchat_performances_trendChart',
  QUEUE_WEBCHAT_SERVICELEVEL_TRENDCHART:'/get_queue_webchat_servicelevel_trendChart',
};

// Queue Analytics whatsapp
export const WHATSAPPQUEUEANALYTICS = {
  QUEUE_WHATSAPP_INBOUND_ANALYSIS_METRICS: '/get_whatsapp_inbound_analysis_metrics',
  QUEUE_WHATSAPP_PERFORMANCE_ANALYSIS_METRICS: '/get_whatsapp_performance_analysis_metrics',
  QUEUE_WHATSAPP_SERVICELEVEL_ANALYSIS_METRICS: '/get_whatsapp_servicelevel_analysis_metrics',
  CREATE_EDIT_COLUMN_QUEUE_WHATSAPP_ANALYTICS: '/create_edit_column_queue_whatsapp_analysis',
  GET_COLUMN_QUEUE_WHATSAPP__ANALYTICS: '/get_column_queue_whatsapp_analysis',
  QUEUE_WHATSAPP_INBOUND_TRENDCHART:'/get_queue_whatsapp_inbound_trendChart',
  QUEUE_WHATSAPP_PERFORMANCE_TRENDCHART:'/get_queue_whatsapp_performance_trendChart',
  QUEUE_WHATSAPP_SERVICELEVEL_TRENDCHART:'/get_queue_whatsapp_servicelevel_trendChart',
};

// based on new figma design
export const SMS = {
  GET_SMS_PROFILE_REPORT: '/get_sms_profile_report', // 1st profile report // 10300
  GET_SMS_AGENT_REPORT: '/get_sms_agent_report', // 2nd agent report
  GET_SMS_INTERATION_REPORT: '/get_sms_interation_report' // 3rd interation report
};

// based on new figma design
export const SOCIALMEDIA = {
  GET_SOCIALMEDIA_PROFILE_REPORT: '/get_socialmedia_profile_report', // done
  GET_SOCIALMEDIA_AGENT_REPORT: '/get_socialmedia_agent_report', // done
  GET_WHATSAPP_ACW_REPORT: '/get_whatsapp_ACW_report', // done
  GET_INSTAGRAM_REPORT: '/get_instagram_profile_report',
  GET_INSTAGRAM_ACW_REPORT: '/get_instagram_ACW_report',
  GET_INSTAGRAM_AGENT_ACW_REPORT: '/get_instagram_Agent_ACW_report',
  GET_FACEBOOK_REPORT: '/get_facebook_report',
  GET_FACEBOOK_AGENT_REPORT: '/get_facebook_Agent_report',
  GET_FACEBOOK_AGENT_ACW_REPORT: '/get_facebook_Agent_ACW_report',
};

// based on new figma design
export const EMAIL = {
  GET_EMAIL_PROFILE_REPORT: '/get_email_profile_report', // tested 10300
  GET_EMAIL_AGENT_REPORT: '/get_email_agent_report', // tested 10300
  GET_EMAIL_INTERATION_REPORT: '/get_email_interation_report' // tested 10300
};

export const ANALYTICSVOICE = {
  GET_QUEUE_BASE_INBOUND_CALL: '/get_queue_base_inbound_call', // commenting as using old SP need to work on it
  GET_DISPOSITION_STATUS: '/get_disposition_status',
  GET_INTERVAL_REPORT: '/get_interval_report',
  ABANDONED_CALL_PROFILE: '/abandoned_call_profile', // not in live db
};

export const REPORTCHAT = {
  GET_CHAT_REPORTS_DETAILS: '/get_chat_report_details',
  GET_AGENT_CHAT_DETAILS: '/get_agent_chat_details',
  GET_CHAT_INTERACTION_DETAILS: '/get_chat_interaction_details',
  GET_ACW_CHAT_DETAILS: '/get_acw_chat_details',
  GET_AGENT_QUEUE_CHAT_DETAILS: '/get_agent_queue_chat_details',
  GET_CHAT_PROFILE_REPORT: '/get_chat_profile_report', // for reports 7709 domain working check
  GET_CHAT_AGENT_REPORT: '/get_chat_agent_report', // for reports 7709 domain working
  GET_CHAT_AGENT_ACW_REPORT: '/get_chat_agent_acw_report' // for reports 7709 domain working
};

export const AGENTREPORT = {
  GET_AGENT_CALL_SUMMARY_REPORT: 'get_agent_call_summary_report', // working 10300
  GET_AGENT_PERFORMANCE: '/get_agent_performance', // ccaas_getAgentPerformanceReport 10300
  GET_LOGIN_AND_LOGOUT_REPORT: 'get_login_and_logout_report',
  GET_QUERY_HANDING_TIME_REPORT: 'get_query_handing_time_report',
  GET_AGENT_STATUS_REPORT: 'get_agent_status_report',
  GET_AGENT_FEEDBACK_REPORT: 'get_Agent_Feedback_Report', // old
  GET_CSAT_AGENT_FEEDBACK_REPORT:'/get_csat_agent_feedback_report', // new
  GET_PARTICULAR_AGENT_STATUS_REPORT:'/get_particular_AgentStatus_Report'
};
export const OUTBOUNDCAMPAIGNDASHBOARD = {
  GET_CALLBLAST_DASHBOARD_INFO: 'get_callBlast_dashboard_info',
  GET_CALLBLAST_CHART:'get_callBlast_dashboard_barChat',
  GET_CALLBLAST_LEADERBOARD_INFO:'get_callBlast_leaderBoard_info'
};
export const OUTBOUNDCAMPAIGNREPORTS = {
  GET_AGENT_OUTBOUNDCAMPAIGN_REPORT_METRICS: 'get_Agent_Outbound_Campaign_Report_Metrics',
  GET_SUMMARY_OUTBOUNDCAMPAIGN_REPORT_METRICS: 'get_Campaign_Summary_Report_Metrics',
};
export const STANDALONEAGENT = {
  GET_CHAT_REPORT: 'get_chat_report',
  GET_AGENT_BOT_REPORT: 'get_agentbot_report',
  GET_AGENT_CHAT_REPORT: 'get_agentchat_report',
  GET_AGENT_ACW_REPORT: 'get_agentacw_report'
};

export const DASHBOARD = {
  GET_CALL_LIVE_DASHBOARD_PIECHART: '/get_call_live_dashboard_piechart',
  GET_WEBCHAT_LIVE_DASHBOARD: '/get_webchat_live_dashboard',
  GET_SMS_LIVE_DASHBOARD_PIECHART: '/get_sms_live_dashboard_piechart',
  GET_EMAIL_LIVE_DASHBOARD_PIECHART: '/get_email_live_dashboard_piechart',
  GET_DASHBOARD_SOCIAL_MEDIA_STATUS: '/get_dashboard_social_media_status',
  GET_DASHBOARD_AGENT_STATUS: '/get_dashboard_agent_status/:agentId',
  GET_DASHBOARD_AVERAGE: '/get_dashboard_average',
  GET_CUSTOMER_SATISFACTION: '/get_customer_satisfaction',
  GET_CALL_LIVE_DASHBOARD: '/get_call_live_dashboard',
  GET_CHAT_LIVE_DASHBOARD: '/get_chat_live_dashboard',
  GET_LIVE_DASHBOARD_FEAUTRES: '/get_live_dashboard_feautres',
  GET_ARRIVAL_DASHBOARD_RATE: '/get_call_dashboard_rate',
  GET_COMPARE_DASHBOARD: '/get_compare_dashboard',
  GET_LEADERBOARD_DETAILS: '/get_leaderboard_details',
  GET_AGENT_ACTIVITY_LIST_DETAILS: '/get_agent_activity_list_details',
  GET_AGENT_ACTIVITY_DETAILS: '/get_agent_activity_details',
  GET_LIVE_CHATBOT_DASHBOARD: '/get_live_chatbot_dashboard',
  GET_AGENT_PERFORMANCE_DASHBOARD: '/get_agent_performance_dashboard',
  GET_CUSTOMER_CHART_RATE: 'get_customer_chart_rate',
  GET_DASHBOARD_AGENT_STATUS_DETAILS: '/getDashboardAgentStatusDetails',
  GET_DASHBOARD_UTILIZATION: '/get_dashboard_utilization',
  GET_DASHBOARD_PERFORMANCE_AGENT_BOT: '/get_dashborad_performance_agent_bot',
  GET_USED_NO_OF_SESSION: '/get_used_no_of_session',
  GET_LEADERBOARD_DASHBOARD_LIST: '/get_leaderboard_dashboard_list',
  GET_LIVE_DASHBOARD_RESPONSE_RATING: '/get_response_rating',
  GET_AGENTREPORT_DETAILS: '/ccaas_get_leaderBoard_agent_dtl',
  GET_LIVE_DASHBOARD_AGENT_ASSISTANCE:'/get_live_dashboard_agent_assistance',
  GET_LIVE_DASHBOARD_SURVEY_RESPONSE:'/get_live_dashboard_survey_response',
  GET_LIVE_DASHBOARD_SENTIMENTL_ANALYSIS :'/get_live_dashboard_customer_satisfaction',
  COMPARE_DASHBOARD_INFO:'get_compare_dashbaord',
  GET_COMPARE_DASHBOAARD_TRENDCHART:'/get_compare_dashbaord_trendchart',
  GET_AGENT_RECENT_ANSWERED_INFO:'/ccaas_get_leaderBoard_agent_recent_answered_info',
  GET_DISPOSITION_STATUS_CHANNEL: '/get_disposition_status_channel',
  GET_CUSTOMER_COUNT_FOR_YEAR: '/get_customer_count_for_year',
  GET_VIDEO_CHANNEL_DASH_METRICS: '/get_video_channel_dash_metrics',
  GET_TRACKER_DATA: '/get_tracker_data',
  GET_OVERALL_SLA_AI: '/get_overall_sla_ai',
};

export const ANALYTICSCHAT = {
  GET_CHAT_DISPOSTION_STATUS: '/get_chat_disposition_status',
  GET_CHAT_DISPOSITION_VIEW_DETAILS: '/get_disposition_view_details',
  GET_QUEUE_CHAT_REPORT: '/get_analytics_queue_chat_report',
  GET_QUEUE_ANALYSIS_REPORT: '/get_queue_analysis_report',
  GET_SKILL_CHAT_REPORT: '/get_analytics_skill_chat_report',
  GET_CHAT_SESSION_REPORT: '/get_analytics_chat_session_report',
};

export const SCHEDULEREPORT = {
  INSERT_SCHEDULE_REPORT_INFO: '/insert_schedule_report_info',
  GET_SCHEDULE_REPORT_INFO: '/get_schedule_report_info',
  DELETE_SCHEDULE_REPORT_INFO: '/delete_schedule_report_info'
};

export const AIBOTREPORTS = {
  GET_AI_CALL_QUALITY_REPORT: '/get_ai_call_quality_report',
  GET_AI_CHAT_QUALITY_REPORT: '/get_ai_chat_quality_report',
  GET_AI_BOT_PERFORMANCES_REPORT: '/get_ai_bot_performances_report',
  GET_AI_BOT_PERFORMANCES_BOT_AGENT_REPORT: '/get_ai_bot_performances_bot_agent_report'
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
  SUPERVISOR_ROLE_ID: 12
};

export const Type = {
  callInteraction : 'callInteraction',
  callProfile : 'callProfile',
  callMissed : 'callMissed',
  callOutbound : 'callOutbound',
  callTransfer :'callTransfer',
  callBackSummary : 'callBackSummary',
  emailProfile : 'emailProfile',
  emailInteraction : 'emailInteraction',
  emailAgent : 'emailAgent',
  smsProfile : 'smsProfile',
  smsAgent : 'smsAgent',
  smsInteraction : 'smsInteraction',
  socialMediaProfile : 'socialMediaProfile',
  socialMediaAgent : 'socialMediaAgent',
  socialMediaACW : 'socialMediaACW',
  instaMedia : 'instaMedia',
  instaMediaACW : 'instaMediaACW',
  instaMediaAgentACW : 'instaMediaAgentACW',
  facebookMediaACW : 'facebookMediaACW',
  chatProfile : 'chatProfile',
  chatAgent : 'chatAgent',
  chatACW : 'chatACW'
};
