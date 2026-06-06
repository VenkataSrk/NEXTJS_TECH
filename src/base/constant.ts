export const config = () => {
    let APPID;
    let AppSECRET;
    let BASE_URL;
    let EC_BASE_URL;
    let SOCKET_URL;
    let LOGIN_API;
    let LOGIN_OUT;
    let LOGIN_All
    let WHATSAPP_CAMPAIGN_SERVICE;
    let BUSINESS_WHATSAPP_CONF_URL;
    const CORESERVICE_URL = "https://ccaasapi.worktual.co.uk/core_service/v1";
    const CCAAS_EMAILSERVICE_URL = 'https://ccaasapi.worktual.co.uk/emailService/v1'
    let NLP_CAMPAIGN_URL;
    let INAPP_BASE_URL;
    let MYACCOUNTSERVICE_URL = 'https://mysqlapi.worktual.co.uk/myaccount-mysql/v1';
    let MYACCOUNT_URL ;
    let AI_INSIGHTS_URL;
    let ANALYTICS_ACTION_PREVIEW;
    let WEEKLYCAMPAIGN_PERFORMANCE_REPORT;
    let WEEKLYCAMPAIGN_PERFORMANCE_METRIC;
    let WEEKLYCAMPAIGN_PERFORMANCE_SUMMARY;
    let STT_NLP;
    let NLP_MYPLAN_SOCKET_URL;
    let WTMS_URL = "https://mysqlapi.worktual.co.uk/urutms-mysql/v1"
    let AI_CORESERVICE_URL;
    let CRMCORESERVICE_URL;
    let AI_CCAAS_REPORTSERVICE_URL;
    let AI_CCAAS_CORESERVICE_URL;
    if (import.meta.env.VITE_APP_ENV === "development") {
        APPID = "844239";
        AppSECRET = "kU9IPc0C65";
        // BASE_URL = "http://localhost:5007/campaignService/v1";
        // EC_BASE_URL = "http://localhost:5007/emailcampaignService/v1";
        //   BASE_URL = "http://46.43.144.104:3001/whatsappcampaignService/v1";    
        BASE_URL = "https://qacampaignapi.worktual.co.uk/campaignService/v1";
        SOCKET_URL = "https://qacampaignapi.worktual.co.uk";
        // SOCKET_URL = "http://localhost:5000";
        LOGIN_API = "https://qaurapi.worktual.co.uk/urauth/v1/token/create_jwt";
        LOGIN_OUT= "https://qacampaignapi.worktual.co.uk/campaignService/v1/logoutall";
        LOGIN_All = "https://qacampaignapi.worktual.co.uk/campaignService/v1/loginall";
        WHATSAPP_CAMPAIGN_SERVICE = "https://qacampaignapi.worktual.co.uk/campaignService/v1";
        // BUSINESS_WHATSAPP_CONF_URL = "http://localhost:5007/campaignService/v1/insertUpdate_business_account";
        BUSINESS_WHATSAPP_CONF_URL = "https://qacampaignapi.worktual.co.uk/campaignService/v1/insertUpdate_business_account";
        NLP_CAMPAIGN_URL = "https://nlpcampaign.worktual.co.uk";
        MYACCOUNT_URL = "https://qa-myaccount.worktual.co.uk";
        // AI_INSIGHTS_URL="http://192.168.13.112:8133/get_analytics_insights"
        AI_INSIGHTS_URL="https://nlpcampaign.worktual.co.uk/get_analytics_insights_qa"       
        ANALYTICS_ACTION_PREVIEW="https://nlpcampaign.worktual.co.uk/proactivate_action_preview_qa";
        WEEKLYCAMPAIGN_PERFORMANCE_REPORT = 'https://nlpcampaign.worktual.co.uk/WeeklyCampaign_Performance_Report_qa';
        WEEKLYCAMPAIGN_PERFORMANCE_METRIC = 'https://nlpcampaign.worktual.co.uk/WeeklyCampaign_Performance_metric_qa';
        WEEKLYCAMPAIGN_PERFORMANCE_SUMMARY = 'https://nlpcampaign.worktual.co.uk/overall_WeeklyCampaign_Performance_summary_qa';
        STT_NLP = "https://aichat.worktual.co.uk/audra_voice";
        NLP_MYPLAN_SOCKET_URL="`https://nlpccaas.worktual.co.uk`";
        WTMS_URL = "https://qaurapi.worktual.co.uk/qaurutms-mysql/v1"
        AI_CORESERVICE_URL="https://ai-ccaasapi.worktual.co.uk/core_service/v1";
        CRMCORESERVICE_URL = "https://qacrmapi.worktual.in/core_service/v1";
        AI_CCAAS_REPORTSERVICE_URL = "https://qaai-ccaasapi.worktual.co.uk/reports_service/v1";
        AI_CCAAS_CORESERVICE_URL = "https://qaai-ccaasapi.worktual.co.uk/core_service/v1";
    } else if (import.meta.env.VITE_APP_ENV === "production") {
        APPID = "844239";
        AppSECRET = "kU9IPc0C65";
        // BASE_URL = "http://localhost:5007/campaignService/v1";
        BASE_URL = "https://campaignapi.worktual.co.uk/campaignService/v1";
        SOCKET_URL = "https://campaignapi.worktual.co.uk";
        EC_BASE_URL = "https://campaignapi.worktual.co.uk/emailcampaignService/v1";
        LOGIN_API = "https://urapi.worktual.co.uk/urauth/v1/token/create_jwt";
        LOGIN_OUT= "https://campaignapi.worktual.co.uk/campaignService/v1/logoutall";
        LOGIN_All = "https://campaignapi.worktual.co.uk/campaignService/v1/loginall";
        WHATSAPP_CAMPAIGN_SERVICE = "https://campaignapi.worktual.co.uk/campaignService/v1";
        BUSINESS_WHATSAPP_CONF_URL = "https://campaignapi.worktual.co.uk/campaignService/v1/insertUpdate_business_account";
        NLP_CAMPAIGN_URL = "https://nlpcampaign.worktual.co.uk";
        MYACCOUNT_URL = "https://myaccount.worktual.co.uk";
        // AI_INSIGHTS_URL="http://192.168.13.112:8133/get_analytics_insights"
        AI_INSIGHTS_URL="https://nlpcampaign.worktual.co.uk/get_analytics_insights"       
        ANALYTICS_ACTION_PREVIEW="https://nlpcampaign.worktual.co.uk/proactivate_action_preview"
        WEEKLYCAMPAIGN_PERFORMANCE_REPORT = 'https://nlpcampaign.worktual.co.uk/WeeklyCampaign_Performance_Report_live';
        WEEKLYCAMPAIGN_PERFORMANCE_METRIC = 'https://nlpcampaign.worktual.co.uk/WeeklyCampaign_Performance_metric_live';
        WEEKLYCAMPAIGN_PERFORMANCE_SUMMARY = ' https://nlpcampaign.worktual.co.uk/overall_WeeklyCampaign_Performance_summary_live';
        STT_NLP = "https://aichat.worktual.co.uk/audra_voice"
        NLP_MYPLAN_SOCKET_URL="`https://nlpccaas.worktual.co.uk`";
        WTMS_URL = "https://mysqlapi.worktual.co.uk/urutms-mysql/v1"
        AI_CORESERVICE_URL="https://ai-ccaasapi.worktual.co.uk/core_service/v1";
        CRMCORESERVICE_URL = "https://crmapi.worktual.in/core_service/v1" 
        AI_CCAAS_REPORTSERVICE_URL = "https://ai-ccaasapi.worktual.co.uk/reports_service/v1";
         AI_CCAAS_CORESERVICE_URL = "https://ai-ccaasapi.worktual.co.uk/core_service/v1";
    } else {
        APPID = "844239";
        AppSECRET = "kU9IPc0C65";
        BASE_URL = "https://qacampaignapi.worktual.co.uk/campaignService/v1";
        EC_BASE_URL = "https://qacampaignapi.worktual.co.uk/emailcampaignService/v1";
    }
    return {
        APPID,
        AppSECRET,
        BASE_URL,
        EC_BASE_URL,
        SOCKET_URL,
        WHATSAPP_CAMPAIGN_SERVICE,
        BUSINESS_WHATSAPP_CONF_URL,
        CORESERVICE_URL,
        LOGIN_API,
        LOGIN_OUT,
        LOGIN_All,
        NLP_CAMPAIGN_URL,
        INAPP_BASE_URL,
        MYACCOUNTSERVICE_URL,
        CCAAS_EMAILSERVICE_URL,
        MYACCOUNT_URL,
        AI_INSIGHTS_URL,
        ANALYTICS_ACTION_PREVIEW,
        WEEKLYCAMPAIGN_PERFORMANCE_REPORT,
        WEEKLYCAMPAIGN_PERFORMANCE_METRIC,
        WEEKLYCAMPAIGN_PERFORMANCE_SUMMARY,
        STT_NLP,
        NLP_MYPLAN_SOCKET_URL,
        WTMS_URL,
        AI_CORESERVICE_URL,
        CRMCORESERVICE_URL,
        AI_CCAAS_REPORTSERVICE_URL,
        AI_CCAAS_CORESERVICE_URL
    };
};