
export const APP_ENV = import.meta.env.VITE_APP_ENV;
export const config = () => {
  let APPID;
  let AppSECRET;
  let BASE_URL;
  let SOCKET_URL;
  let LOGIN_API;
  const MYACCOUNTSERVICE_URL = 'https://mysqlapi.worktual.co.uk/myaccount-mysql/v1';
  let STT_NLP;

  let MEET_RESTAPI_URL;
  let CALL_SERVER_WSS_URL;
  let VIDEO_CALL_SERVER_WSS_URL;
  let CALL_SERVER_STUN_URL;
  let CALL_SERVER_STUN_URL2;
  let VIDEO_CALL_SERVER_STUN_URL;
  let VIDEO_CALL_SERVER_STUN_URL_TH;
  let VIDEO_CALL_SERVER_STUN_URL_DDCL;
  let VIDEO_CALL_SERVER_V6_STUN_URL;
  let VIDEO_CALL_SERVER_V6_STUN_URL_DDCL;
  let VIDEO_CALL_SERVER_V6_STUN_URL_TH;
  const VIDEO_CALL_SERVER_STUN_USERNAME = "admin";
  const VIDEO_CALL_SERVER_STUN_PASSWORD = "system123";
  let CORESERVICE_URL;
  let UPLOAD_MEETING_FILES;
  let CALLSERVICE_URL;
  let EMAILSERVICE_URL;
  let SMSSERVICE_URL;
  let REPORTSERVICE_URL;
  let LIVECHATSERVICEURL;
  const CALL_SERVER_USER_AGENT = "CCAAS:BROWSER";
  const VIDEO_SERVER_USER_AGENT = "URWEB:VIDEO";
  const CALL_SERVER_STUN_USERNAME = "admin";
  const CALL_SERVER_STUN_PASSWORD = "system123";
  const DOMAIN_NAME_PORT = "5060";
  const MYACCOUNT = "";
  const LOG_OUT_API = "";
  let TICKETSERVICE_URL;
  let PRICING_URL;
  const CONNECTORS_API_BASE_URL = "https://qaai-ccaasapi.worktual.co.uk/connector";
  const DEVELOPER_APP_BASE_URL = 'https://qaapi.worktual.co.uk/services/v1'
  const ZENDESK_SUPPORT_CLIENT_ID = "zdg-worktual_ucaas_for_zendesk_support"
  const ZENDESK_SUPPORT_SCOPES = "tickets:read write users:read write organizations:write read"
  const BOT_URL = "";
  const SMS_DID = ''
  const WHATSAPP_URL = ''
  const FACEBOOK_URL = ''
  const INSTAGRAM_URL = ''
  const CHATBOT_URL = ''
  let SOCIALMEDIA_URL;
  const SHOPIFY_CLIENT_ID = 'b527b305f85fcc1c8be953ca7ecc2c38'
  const CRM_URL = "";
  const CCAAS_WHATSAPP_URL = '';
  const MYACCOUNT_PROFILE = '';
  const WHATSAPP_CONF_URL = '';
  const BUSINESS_WHATSAPP_CONF_URL = '';
  const BOT_SCRIPT_URL = '';
  const FACEBOOK_CONFIG_ID = '1109736967317755';
  const FACEBOOK_APP_ID = "2792648964217350";
  const GET_ALL_TEMPLATES_URL = "";
  const GET_TEMPLATE_URL = "";
  const BOTH_TYPE_CONTACT_SYNC = ['salesforce', 'HubSpot', 'dynamics', 'zoho', 'zendesk sell', 'zendesk support', 'pipedrive', 'shopify', 'wix', 'bigcommerce', 'magento', 'xero']
  const DASHBOARD_AISUMMARY_AGENTDASH = "";
  const DASHBOARD_AISUMMARY_AGENTDASH_RECOM = "";
  const DASHBOARD_AISUMMARY_MAINDASH = "";
  const DASHBOARD_AISUMMARY_MAINDASH_RECOM = "";
  const DASHBOARD_AISUMMARY_CAMPAIGNDASH = "";
  const DASHBOARD_AISUMMARY_CAMPAIGNDASH_RECOM = "";
  let FACE_DETECTION;
  let FACE_UPLOAD_IMAGE;
  let FACE_RECOGNITION;
  let DOC_UPLOAD_EXTRACT;
  let DOC_VERIFICATION;
  let VERIFICATION_DELETE;
  let TTS = `https://nlp.worktual.co.uk/tts`;
  let PRIVIOUS_CHAT_SUMMARY = "https://nlpccaas.worktual.co.uk/ai_ccass_agent_assist_previous_chat_summary";
  let AGENT_SUMMARIZE_CONVO = "";
  let NLP_CHAT_SOCKET_URL = 'wss://nlp.worktual.co.uk'
  let NLP_INBOX_CHAT_SOCKET_URL = 'wss://nlp.worktual.co.uk'
  let NLP_CCAAS_SOCKET_URL = 'wss://nlpccaas.worktual.co.uk';
  let NLP_REPORTS_API = '';
  let NLP_AGENT_ASSIST_QUERY = "https://nlpccaas.worktual.co.uk/ai_ccass_agent_assist_query_sentiment";
  let NLP_DNS = "https://nlpccaas.worktual.co.uk";
  let NLP_DNS_URL = "https://nlp.worktual.co.uk";
  let NLP_MYPLAN_SOCKET_URL = 'wss://nlpccaas.worktual.co.uk';
  let AI_CCAAS_REPORTSERVICE_URL = '';
  let AI_CCAAS_CORESERVICE_URL = '';

  if (APP_ENV === "production") {
    APPID = "844239";
    AppSECRET = "kU9IPc0C65";
    BASE_URL = "https://crmapi.worktual.in";
    SOCKET_URL = "";
    LOGIN_API = "https://urapi.worktual.co.uk/urauth/v1/token/create_jwt_crm";
    STT_NLP = "https://aichat.worktual.co.uk/audra_voice";
    CORESERVICE_URL = "https://crmapi.worktual.in/core_service/v1" 
    // CORESERVICE_URL = "http://localhost:5002/core_service/v1" 
    NLP_CHAT_SOCKET_URL = 'wss://nlp.worktual.co.uk'
    NLP_CHAT_SOCKET_URL = 'wss://nlp.worktual.co.uk'
    CALL_SERVER_STUN_URL = "turns:audioturnv4.worktual.co.uk:443";
    CALL_SERVER_STUN_URL2 = "turn:audioturnv6.worktual.co.uk:3478";
    CALL_SERVER_WSS_URL = "wss://ociccaasaudiosgwai.worktual.co.uk:57344";
    NLP_MYPLAN_SOCKET_URL='wss://nlpccaas.worktual.co.uk';
    AI_CCAAS_REPORTSERVICE_URL = "https://ai-ccaasapi.worktual.co.uk/reports_service/v1";
    AI_CCAAS_CORESERVICE_URL = "https://ai-ccaasapi.worktual.co.uk/core_service/v1";


  } else {
    APPID = "844239";
    AppSECRET = "kU9IPc0C65";
    BASE_URL = "https://qacrmapi.worktual.in";
    SOCKET_URL = "";
    LOGIN_API = "https://qaurapi.worktual.co.uk/urauth/v1/token/create_jwt_crm";
    STT_NLP = "https://aichat.worktual.co.uk/audra_voice";
    CORESERVICE_URL = "https://qacrmapi.worktual.in/core_service/v1"
    NLP_CHAT_SOCKET_URL = 'wss://qanlp.worktual.co.uk'
    CALL_SERVER_STUN_URL = "turns:audioturnv4.worktual.co.uk:443";
    CALL_SERVER_STUN_URL2 = "turn:audioturnv6.worktual.co.uk:3478";
    CALL_SERVER_WSS_URL = "wss://qaccaasaudiosgw.worktual.co.uk:57344";
    NLP_MYPLAN_SOCKET_URL='wss://nlpccaas.worktual.co.uk';
    AI_CCAAS_REPORTSERVICE_URL = "https://qaai-ccaasapi.worktual.co.uk/reports_service/v1";
    AI_CCAAS_CORESERVICE_URL = "https://qaai-ccaasapi.worktual.co.uk/core_service/v1";

  }

  return {
    APPID,
    AppSECRET,
    BASE_URL,
    SOCKET_URL,
    LOGIN_API,
    MYACCOUNTSERVICE_URL,
    STT_NLP,
    CALL_SERVER_WSS_URL,
    VIDEO_CALL_SERVER_WSS_URL,
    CALL_SERVER_USER_AGENT,
    VIDEO_SERVER_USER_AGENT,
    CALL_SERVER_STUN_URL,
    CALL_SERVER_STUN_URL2,
    VIDEO_CALL_SERVER_STUN_URL,
    VIDEO_CALL_SERVER_STUN_URL_DDCL,
    VIDEO_CALL_SERVER_STUN_URL_TH,
    VIDEO_CALL_SERVER_V6_STUN_URL,
    VIDEO_CALL_SERVER_V6_STUN_URL_DDCL,
    VIDEO_CALL_SERVER_V6_STUN_URL_TH,
    VIDEO_CALL_SERVER_STUN_USERNAME,
    VIDEO_CALL_SERVER_STUN_PASSWORD,
    CALL_SERVER_STUN_USERNAME,
    CALL_SERVER_STUN_PASSWORD,
    CORESERVICE_URL,
    EMAILSERVICE_URL,
    SMSSERVICE_URL,
    CALLSERVICE_URL,
    REPORTSERVICE_URL,
    LIVECHATSERVICEURL,
    DOMAIN_NAME_PORT,
    MEET_RESTAPI_URL,
    MYACCOUNT,
    LOG_OUT_API,
    TICKETSERVICE_URL,
    BOT_URL,
    CONNECTORS_API_BASE_URL,
    DEVELOPER_APP_BASE_URL,
    ZENDESK_SUPPORT_CLIENT_ID,
    ZENDESK_SUPPORT_SCOPES,
    SMS_DID,
    WHATSAPP_URL,
    PRICING_URL,
    CHATBOT_URL,
    SOCIALMEDIA_URL,
    FACEBOOK_URL,
    INSTAGRAM_URL,
    SHOPIFY_CLIENT_ID,
    CRM_URL,
    UPLOAD_MEETING_FILES,
    CCAAS_WHATSAPP_URL,
    MYACCOUNT_PROFILE,
    WHATSAPP_CONF_URL,
    BUSINESS_WHATSAPP_CONF_URL,
    BOT_SCRIPT_URL,
    FACEBOOK_CONFIG_ID,
    FACEBOOK_APP_ID,
    GET_ALL_TEMPLATES_URL,
    GET_TEMPLATE_URL,
    BOTH_TYPE_CONTACT_SYNC,
    DASHBOARD_AISUMMARY_AGENTDASH,
    DASHBOARD_AISUMMARY_AGENTDASH_RECOM,
    DASHBOARD_AISUMMARY_MAINDASH,
    DASHBOARD_AISUMMARY_MAINDASH_RECOM,
    DASHBOARD_AISUMMARY_CAMPAIGNDASH,
    DASHBOARD_AISUMMARY_CAMPAIGNDASH_RECOM,
    FACE_DETECTION,
    FACE_UPLOAD_IMAGE,
    FACE_RECOGNITION,
    DOC_UPLOAD_EXTRACT,
    DOC_VERIFICATION,
    VERIFICATION_DELETE,
    TTS,
    PRIVIOUS_CHAT_SUMMARY,
    AGENT_SUMMARIZE_CONVO,
    NLP_CHAT_SOCKET_URL,
    NLP_CCAAS_SOCKET_URL,
    NLP_REPORTS_API,
    NLP_AGENT_ASSIST_QUERY,
    NLP_DNS,
    NLP_DNS_URL,
    NLP_INBOX_CHAT_SOCKET_URL,
    NLP_MYPLAN_SOCKET_URL,
    AI_CCAAS_REPORTSERVICE_URL,
    AI_CCAAS_CORESERVICE_URL
  };
};

export const ROLES = {
  OWNER_ROLE_ID: "1",
  SUB_OWNER_ROLE_ID: "2",
  ADMIN_ROLE_ID: "3",
  MEMBER_ROLE_ID: "4",
  SUPERVISOR_ROLE_ID: "5",
  AGENT_ROLE_ID: "6",
};

export const PRIVILLEGED_ROLES = [ROLES.OWNER_ROLE_ID, ROLES.SUB_OWNER_ROLE_ID, ROLES.ADMIN_ROLE_ID];

export const CHAT_BOX_PLAN_ID = {
  XAIA_1_FREE: 54,
  XAIA_2_FREE: 107,
  XAIA_2_PAID: 103,
  XAIA_3_FREE: 55,
  XAIA_3_PAID: 56,
  XAIA_4_PAID: 57
}

export const ROLES_NAME_LIST: any = {
  1: "Owner",
  2: "Sub-Owner",
  3: "Admin",
  4: "Member",
  5: "Supervisor",
  6: "Agent",
  7: "Consultant",
  8: "Executive",
};

export const CONTACT_CENTRE_PLAN_ID = {
  "XAIA_1+": 32,
  "XAIA_2+": 141,
  "XAIA_3+": 33,
  "XAIA_4+": 42
}

export const PRODUCT_ID_AND_NAME: any = {
  0: "All", //For support address in email
  1: "AI Contact Center",
  15: "Chatbox",
  16: "Marketing Email",
  17: "Appointment Bookings"
}

export const CONTACT_CENTRE_PARENT_ID = {
  "STARTER": 32,
  "GROWTH": 141,
  "PRO": 33,
  "ENTERPRISE": 42
}

export const getDynamicLocation = () => {
  try {
    if (window.location.host?.includes('lola')) {
      return 'LOLAAI'
    } else if (window.location.host?.includes('luka')) {
      return 'LUKASAI'
    } else if (window.location.host?.includes('ccaas')) {
      return 'CRM'
    } else {
      return 'CRM'
    }
  } catch (error) {
    console.log(error)
    return `CCAAS`
  }
}

export const MENUS = {
  DEALS: "1",
  CONTACT: "2",
  MARKETING: "3",
  PRODUCTS: "4",
  PROJECTS: "5",
  ONBOARDING: "6",
  OPERATIONAL_HUB: "7",
  DEAL_PIPELINE: "12",
  DEAL_REPORTS: "13",
  CONTACT_GROUPS: "14",
  CONTACT_IMPORT: "15",
  CAMPAIGNS: "8",
  TEMPLATES: "9",
  MARKETING_LISTS: "10",
  MARKETING_ANALYTICS: "11",
  PROJECT_TASKS: "16",
  PROJECT_REPORTS: "17",
  TICKETS: "18",
  SLA_MANAGEMENT: "19",
  OPERATIONAL_ANALYTICS: "20",
  INBOX: "21",
  SETUP: "22",
};
const admin_1 = [
  { "moduleId": 1, "permissions": { "view": "all", "create": true, "edit": "all", "delete": true, "assign": true, "close": true, "merge": false, "export": true, "send": false, "analytics": true } },
  { "moduleId": 2, "permissions": { "view": "all", "create": true, "edit": "all", "delete": true, "assign": false, "close": false, "merge": true, "export": true, "send": false, "analytics": true } },
  { "moduleId": 3, "permissions": { "view": "all", "create": true, "edit": "all", "delete": true, "assign": false, "close": false, "merge": false, "export": true, "send": true, "analytics": true } },
  { "moduleId": 4, "permissions": { "view": "all", "create": true, "edit": "all", "delete": true, "assign": true, "close": false, "merge": false, "export": true, "send": false, "analytics": true } },
  { "moduleId": 5, "permissions": { "view": "all", "create": true, "edit": "all", "delete": true, "assign": true, "close": true, "merge": false, "export": true, "send": false, "analytics": true } },
  { "moduleId": 21, "permissions": { "view": "all", "create": true, "edit": "all", "delete": true, "assign": true, "close": true, "merge": true, "export": true, "send": true, "analytics": true } },
  { "moduleId": 22, "permissions": { "view": "all", "create": true, "edit": "all", "delete": true, "assign": true, "close": true, "merge": true, "export": true, "send": true, "analytics": true } }
];

const sales_manager = [
  { "moduleId": 1, "permissions": { "view": "team", "create": true, "edit": "team", "delete": false, "assign": true, "close": true, "merge": false, "export": true, "send": false, "analytics": true } },
  { "moduleId": 2, "permissions": { "view": "team", "create": true, "edit": "team", "delete": false, "assign": false, "close": false, "merge": false, "export": true, "send": false, "analytics": true } },
  { "moduleId": 3, "permissions": { "view": "all", "create": false, "edit": false, "delete": false, "assign": false, "close": false, "merge": false, "export": false, "send": false, "analytics": true } },
  { "moduleId": 4, "permissions": { "view": "all", "create": false, "edit": false, "delete": false, "assign": false, "close": false, "merge": false, "export": true, "send": false, "analytics": true } },
  { "moduleId": 5, "permissions": { "view": "team", "create": false, "edit": false, "delete": false, "assign": true, "close": false, "merge": false, "export": true, "send": false, "analytics": true } },
  { "moduleId": 21, "permissions": { "view": "team", "create": true, "edit": "team", "delete": false, "assign": true, "close": true, "merge": false, "export": true, "send": true, "analytics": true } }
];

const sales_rep = [
  { "moduleId": 1, "permissions": { "view": "own", "create": true, "edit": "own", "delete": false, "assign": false, "close": true, "merge": false, "export": false, "send": false, "analytics": true } },
  { "moduleId": 2, "permissions": { "view": "own", "create": true, "edit": "own", "delete": false, "assign": false, "close": false, "merge": false, "export": false, "send": false, "analytics": true } },
  { "moduleId": 3, "permissions": { "view": "none", "create": false, "edit": false, "delete": false, "assign": false, "close": false, "merge": false, "export": false, "send": false, "analytics": false } },
  { "moduleId": 4, "permissions": { "view": "all", "create": false, "edit": false, "delete": false, "assign": false, "close": false, "merge": false, "export": false, "send": false, "analytics": true } },
  { "moduleId": 5, "permissions": { "view": "own", "create": false, "edit": false, "delete": false, "assign": false, "close": false, "merge": false, "export": false, "send": false, "analytics": true } },
  { "moduleId": 21, "permissions": { "view": "own", "create": true, "edit": "own", "delete": false, "assign": false, "close": true, "merge": false, "export": false, "send": true, "analytics": true } }
];

const marketing_manager = [
  { "moduleId": 1, "permissions": { "view": "all", "create": false, "edit": false, "delete": false, "assign": false, "close": false, "merge": false, "export": false, "send": false, "analytics": true } },
  { "moduleId": 2, "permissions": { "view": "all", "create": false, "edit": false, "delete": false, "assign": false, "close": false, "merge": false, "export": true, "send": false, "analytics": true } },
  { "moduleId": 3, "permissions": { "view": "all", "create": true, "edit": "all", "delete": true, "assign": false, "close": false, "merge": false, "export": true, "send": true, "analytics": true } },
  { "moduleId": 4, "permissions": { "view": "none", "create": false, "edit": false, "delete": false, "assign": false, "close": false, "merge": false, "export": true, "send": false, "analytics": true } },
  { "moduleId": 5, "permissions": { "view": "none", "create": false, "edit": false, "delete": false, "assign": false, "close": false, "merge": false, "export": true, "send": false, "analytics": true } },
  { "moduleId": 21, "permissions": { "view": "all", "create": true, "edit": "all", "delete": false, "assign": false, "close": true, "merge": false, "export": true, "send": true, "analytics": true } }
];

const project_manager = [
  { "moduleId": 1, "permissions": { "view": "none", "create": false, "edit": false, "delete": false, "assign": false, "close": false, "merge": false, "export": true, "send": false, "analytics": true } },
  { "moduleId": 2, "permissions": { "view": "none", "create": false, "edit": false, "delete": false, "assign": false, "close": false, "merge": false, "export": true, "send": false, "analytics": true } },
  { "moduleId": 3, "permissions": { "view": "none", "create": false, "edit": false, "delete": false, "assign": false, "close": false, "merge": false, "export": true, "send": false, "analytics": true } },
  { "moduleId": 4, "permissions": { "view": "all", "create": true, "edit": "all", "delete": true, "assign": true, "close": false, "merge": false, "export": true, "send": false, "analytics": true } },
  { "moduleId": 5, "permissions": { "view": "all", "create": false, "edit": false, "delete": false, "assign": false, "close": false, "merge": false, "export": true, "send": false, "analytics": true } },
  { "moduleId": 21, "permissions": { "view": "team", "create": true, "edit": "team", "delete": false, "assign": true, "close": true, "merge": false, "export": true, "send": true, "analytics": true } }
];

const product_manager = [
  { "moduleId": 1, "permissions": { "view": "none", "create": false, "edit": false, "delete": false, "assign": false, "close": false, "merge": false, "export": true, "send": false, "analytics": true } },
  { "moduleId": 2, "permissions": { "view": "none", "create": false, "edit": false, "delete": false, "assign": false, "close": false, "merge": false, "export": true, "send": false, "analytics": true } },
  { "moduleId": 3, "permissions": { "view": "none", "create": false, "edit": false, "delete": false, "assign": false, "close": false, "merge": false, "export": true, "send": false, "analytics": true } },
  { "moduleId": 4, "permissions": { "view": "all", "create": false, "edit": false, "delete": false, "assign": false, "close": false, "merge": false, "export": true, "send": false, "analytics": true } },
  { "moduleId": 5, "permissions": { "view": "all", "create": true, "edit": "all", "delete": true, "assign": true, "close": true, "merge": false, "export": true, "send": false, "analytics": true } },
  { "moduleId": 21, "permissions": { "view": "own", "create": false, "edit": false, "delete": false, "assign": false, "close": false, "merge": false, "export": true, "send": false, "analytics": true } }
];

export const ACTIONS_ACCESS = {
  "VIEW": "view",
  "CREATE": "create",
  "EDIT": "edit",
  "DELETE": "delete",
  "ASSIGN": "assign",
  "CLOSE": "close",
  "MERGE": "merge",
  "EXPORT": "export",
  "SEND": "send",
  "ANALYTICS": "analytics"
};

/**
 * 
 * @param moduleId 
 * @param action 
 * @param permissionsData 
 * @param type 
 * @example type : all, team, own - type will be checked
 * @returns boolean
 */

export const levelOfAccess = (moduleId :any, action :string, permissionsData :any, type = '')  => {
  try {

    if (!permissionsData?.length) return false;

    
    const module = permissionsData?.find((p: any) => p.moduleId == moduleId);
    if (!module || !module.permissions) return false;
    const value = module.permissions?.[action];

    if (typeof value === "boolean") return value;

    if(type && value) return type === value;
    if (value !== "none") return true;

    // if (["all", 'team'].includes(value)) return true;

    return false;

  } catch (error) {
    console.error("levelOfAccess error:", error);
    return false;
  }
};

export const welCareDomainId = 20667;
export const pothysDomainId = 20692;