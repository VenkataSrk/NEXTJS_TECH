import { logger } from '../../src/plugins/log';
import { mysqlPoolConnection } from '../plugins/db';

export const getSocialMediaProfileReportHandlerDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_socialmedia_profile_report(?,?,?,?,?,?,?,?,?)',
        [
          data?.startTmpstmp,
          data?.endTmpstmp,
          data?.timeZone,
          data?.domainId,
          data?.queueName,
          data?.agentName,
          data?.disposition,
          data?.offset,
          data?.limit,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getSocialMediaProfileReportHandlerDao - Unexpected Error:', err);
            console.log('up_ccaas_socialmedia_profile_report error: ', err);
          }
          resolve(result);
        }
      );
    } catch (error) {
      console.log('up_ccaas_socialmedia_profile_report catch error: ', error);
      reject(error);
      logger.error('getSocialMediaProfileReportHandlerDao - Unexpected Error:', error);
    }
  });
};

export const getSocialMediaProfileReportHandlerDaoForSchedule = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_socialmedia_profile_report(?,?,?,?,?,?,?,?,?)',
        [
          data?.startTmpstmp,
          data?.endTmpstmp,
          data?.timeZone,
          data?.domainId,
          data?.queueName,
          data?.agentName,
          data?.disposition,
          data?.offset,
          data?.limit,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getSocialMediaProfileReportHandlerDaoForSchedule - Unexpected Error:', err);
            console.log('up_ccaas_socialmedia_profile_report error: ', err);
          }
          resolve(result?.length > 0 ? result[3] : []);
        }
      );
    } catch (error) {
      console.log('up_ccaas_socialmedia_profile_report catch error: ', error);
      reject(error);
      logger.error('getSocialMediaProfileReportHandlerDaoForSchedule - Unexpected Error:', error);
    }
  });
};

export const getSocialMediaAgentReportHandlerDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_socialmedia_agent_report(?,?,?,?,?,?,?,?)',
        [
          data?.startTmpstmp,
          data?.endTmpstmp,
          data?.timeZone,
          data?.domainId,
          data?.queueName,
          data?.agentName,
          data?.offset,
          data?.limit,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getSocialMediaAgentReportHandlerDao - Unexpected Error:', err);
            console.log('up_ccaas_socialmedia_agent_report error: ', err);
          }
          resolve(result);
        }
      );
    } catch (error) {
      console.log('up_ccaas_socialmedia_agent_report catch error: ', error);
      reject(error);
      logger.error('getSocialMediaAgentReportHandlerDao - Unexpected Error:', error);
    }
  });
};

export const getSocialMediaAgentReportHandlerDaoForSchedule = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_socialmedia_agent_report(?,?,?,?,?,?,?,?)',
        [
          data?.startTmpstmp,
          data?.endTmpstmp,
          data?.timeZone,
          data?.domainId,
          data?.queueName,
          data?.agentName,
          data?.offset,
          data?.limit,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getSocialMediaAgentReportHandlerDaoForSchedule - Unexpected Error:', err);
            console.log('up_ccaas_socialmedia_agent_report error: ', err);
          }
          resolve(result?.length > 0 ? result?.[3] : []);
        }
      );
    } catch (error) {
      console.log('up_ccaas_socialmedia_agent_report catch error: ', error);
      reject(error);
      logger.error('getSocialMediaAgentReportHandlerDaoForSchedule - Unexpected Error:', error);
    }
  });
};

export const getWhatsappACWReportHandlerDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_whatsapp_agent_ACW_report(?,?,?,?,?,?,?,?)',
        [
          data?.startTmpstmp,
          data?.endTmpstmp,
          data?.timeZone,
          data?.domainId,
          data?.queueName,
          data?.agentName,
          data?.offset,
          data?.limit,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getWhatsappACWReportHandlerDao - Unexpected Error:', err);
            console.log('up_ccaas_whatsapp_agent_ACW_report error: ', err);
          }
          resolve(result);
        }
      );
    } catch (error) {
      console.log('up_ccaas_socialmedia_agent_report catch error: ', error);
      reject(error);
      logger.error('getWhatsappACWReportHandlerDao - Unexpected Error:', error);
    }
  });
};

export const getWhatsappACWReportHandlerDaoForSchedule = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_whatsapp_agent_ACW_report(?,?,?,?,?,?,?,?)',
        [
          data?.startTmpstmp,
          data?.endTmpstmp,
          data?.timeZone,
          data?.domainId,
          data?.queueName,
          data?.agentName,
          data?.offset,
          data?.limit,
        ],
        (err: any, result: any) => {
          if (err) {
            logger.error('getWhatsappACWReportHandlerDaoForSchedule - Unexpected Error:', err);
            reject(err);
            console.log('up_ccaas_whatsapp_agent_ACW_report error: ', err);
          }
          resolve(result?.length > 0 ? result[3] : []);
        }
      );
    } catch (error) {
      console.log('up_ccaas_socialmedia_agent_report catch error: ', error);
      reject(error);
      logger.error('getWhatsappACWReportHandlerDaoForSchedule - Unexpected Error:', error);
    }
  });
};

export const getInstagramReportHandlerDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_Instragram_Profile_Report(?,?,?,?,?,?,?,?)',
        [
          data?.startTmpstmp,
          data?.endTmpstmp,
          data?.timeZone,
          data?.domainId,
          data?.agentName,
          data?.disposition,
          data?.offset,
          data?.limit,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getInstagramReportHandlerDao - Unexpected Error:', err);
            console.log('up_ccaas_Instragram_Profile_Report error: ', err);
          }
          resolve(result);
        }
      );
    } catch (error) {
      console.log('up_ccaas_Instragram_Profile_Report catch error: ', error);
      reject(error);
      logger.error('getInstagramReportHandlerDao - Unexpected Error:', error);
    }
  });
};

export const getInstagramReportHandlerDaoForSchedule = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_Instragram_Profile_Report(?,?,?,?,?,?,?,?)',
        [
          data?.startTmpstmp,
          data?.endTmpstmp,
          data?.timeZone,
          data?.domainId,
          data?.agentName,
          data?.disposition,
          data?.offset,
          data?.limit,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getInstagramReportHandlerDaoForSchedule - Unexpected Error:', err);
            console.log('up_ccaas_Instragram_Profile_Report error: ', err);
          }
          resolve(result?.length > 0 ? result[3] : []);
        }
      );
    } catch (error) {
      console.log('up_ccaas_Instragram_Profile_Report catch error: ', error);
      reject(error);
      logger.error('getInstagramReportHandlerDaoForSchedule - Unexpected Error:', error);
    }
  });
};

export const getInstagramacwReportHandlerDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_Instragram_Agent_Chat_Report(?,?,?,?,?,?,?,?)',
        [
          data?.startTmpstmp,
          data?.endTmpstmp,
          data?.timeZone,
          data?.domainId,
          data?.agentName,
          data?.disposition,
          data?.offset,
          data?.limit,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getInstagramacwReportHandlerDao - Unexpected Error:', err);
            console.log('up_ccaas_Instragram_Agent_Chat_Report error: ', err);
          }
          resolve(result);
        }
      );
    } catch (error) {
      console.log('up_ccaas_Instragram_Agent_Chat_Report catch error: ', error);
      reject(error);
      logger.error('getInstagramacwReportHandlerDao - Unexpected Error:', error);
    }
  });
};

export const getInstagramacwReportHandlerDaoForSchedule = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_Instragram_Agent_Chat_Report(?,?,?,?,?,?,?,?)',
        [
          data?.startTmpstmp,
          data?.endTmpstmp,
          data?.timeZone,
          data?.domainId,
          data?.agentName,
          data?.disposition,
          data?.offset,
          data?.limit,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getInstagramacwReportHandlerDaoForSchedule - Unexpected Error:', err);
            console.log('up_ccaas_Instragram_Agent_Chat_Report error: ', err);
          }
          resolve(result?.length > 0 ? result[3] : []);
        }
      );
    } catch (error) {
      console.log('up_ccaas_Instragram_Agent_Chat_Report catch error: ', error);
      reject(error);
      logger.error('getInstagramacwReportHandlerDaoForSchedule - Unexpected Error:', error);
    }
  });
};

export const getInstagramAgentacwReportHandlerDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_Instragram_Agent_ACW_Report(?,?,?,?,?,?,?,?)',
        [
          data?.startTmpstmp,
          data?.endTmpstmp,
          data?.timeZone,
          data?.domainId,
          data?.agentName,
          data?.disposition,
          data?.offset,
          data?.limit,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getInstagramAgentacwReportHandlerDao - Unexpected Error:', err);
            console.log('up_ccaas_Instragram_Agent_ACW_Report error: ', err);
          }
          resolve(result);
        }
      );
    } catch (error) {
      console.log('up_ccaas_Instragram_Agent_ACW_Report catch error: ', error);
      reject(error);
      logger.error('getInstagramAgentacwReportHandlerDao - Unexpected Error:', error);
    }
  });
};

export const getInstagramAgentacwReportHandlerDaoForSchedule = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_Instragram_Agent_ACW_Report(?,?,?,?,?,?,?,?)',
        [
          data?.startTmpstmp,
          data?.endTmpstmp,
          data?.timeZone,
          data?.domainId,
          data?.agentName,
          data?.disposition,
          data?.offset,
          data?.limit,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getInstagramAgentacwReportHandlerDaoForSchedule - Unexpected Error:', err);
            console.log('up_ccaas_Instragram_Agent_ACW_Report error: ', err);
          }
          resolve(result?.length > 0 ? result?.[3] : []);
        }
      );
    } catch (error) {
      console.log('up_ccaas_Instragram_Agent_ACW_Report catch error: ', error);
      reject(error);
      logger.error('getInstagramAgentacwReportHandlerDaoForSchedule - Unexpected Error:', error);
    }
  });
};

export const getFacebookReportHandlerDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_Facebook_Profile_Report(?,?,?,?,?,?,?,?)',
        [
          data?.startTmpstmp,
          data?.endTmpstmp,
          data?.timeZone,
          data?.domainId,
          data?.agentName,
          data?.disposition,
          data?.offset,
          data?.limit,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getInstagramAgentacwReportHandlerDaoForSchedule - Unexpected Error:', err);
            console.log('up_ccaas_Facebook_Profile_Report error: ', err);
          }
          resolve(result);
        }
      );
    } catch (error) {
      console.log('up_ccaas_Facebook_Profile_Report catch error: ', error);
      reject(error);
      logger.error('getFacebookReportHandlerDao - Unexpected Error:', error);
    }
  });
};

export const getFacebookReportHandlerDaoForSchedule = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_Facebook_Profile_Report(?,?,?,?,?,?,?,?)',
        [
          data?.startTmpstmp,
          data?.endTmpstmp,
          data?.timeZone,
          data?.domainId,
          data?.agentName,
          data?.disposition,
          data?.offset,
          data?.limit,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getFacebookReportHandlerDaoForSchedule - Unexpected Error:', err);
            console.log('up_ccaas_Facebook_Profile_Report error: ', err);
          }
          resolve(result?.length > 0 ? result[3] : []);
        }
      );
    } catch (error) {
      console.log('up_ccaas_Facebook_Profile_Report catch error: ', error);
      reject(error);
      logger.error('getfacebookReportHandlerDaoForSchedule - Unexpected Error:', error);
    }
  });
};

export const getFacebookAgentReportHandlerDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_Facebook_Agent_Chat_Report(?,?,?,?,?,?,?,?)',
        [
          data?.startTmpstmp,
          data?.endTmpstmp,
          data?.timeZone,
          data?.domainId,
          data?.agentName,
          data?.disposition,
          data?.offset,
          data?.limit,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getFacebookAgentReportHandlerDao - Unexpected Error:', err);
            console.log('up_ccaas_Facebook_Agent_Chat_Report error: ', err);
          }
          resolve(result);
        }
      );
    } catch (error) {
      console.log('up_ccaas_Facebook_Agent_Chat_Report catch error: ', error);
      reject(error);
      logger.error('getfacebookAgentReportHandlerDao - Unexpected Error:', error);
    }
  });
};

export const getfacebookAgentReportHandlerDaoForSchedule = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_Facebook_Agent_Chat_Report(?,?,?,?,?,?,?,?)',
        [
          data?.startTmpstmp,
          data?.endTmpstmp,
          data?.timeZone,
          data?.domainId,
          data?.agentName,
          data?.disposition,
          data?.offset,
          data?.limit,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getfacebookAgentReportHandlerDaoForSchedule - Unexpected Error:', err);
            console.log('up_ccaas_Facebook_Agent_Chat_Report error: ', err);
          }
          resolve(result?.length > 0 ? result[3] : []);
        }
      );
    } catch (error) {
      console.log('up_ccaas_Facebook_Agent_Chat_Report catch error: ', error);
      reject(error);
      logger.error('getfacebookAgentReportHandlerDaoForSchedule - Unexpected Error:', error);
    }
  });
};

export const getFacebookAgentACWReportHandlerDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_Facebook_agent_ACW_report(?,?,?,?,?,?,?,?)',
        [
          data?.startTmpstmp,
          data?.endTmpstmp,
          data?.timeZone,
          data?.domainId,
          data?.agentName,
          data?.disposition,
          data?.offset,
          data?.limit,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getFacebookAgentACWReportHandlerDao - Unexpected Error:', err);
            console.log('up_ccaas_Facebook_agent_ACW_report error: ', err);
          }
          resolve(result);
        }
      );
    } catch (error) {
      console.log('up_ccaas_Facebook_agent_ACW_report catch error: ', error);
      reject(error);
      logger.error('getfacebookAgentACWReportHandlerDao - Unexpected Error:', error);
    }
  });
};

export const getfacebookAgentACWReportHandlerDaoForSchedule = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_Facebook_agent_ACW_report(?,?,?,?,?,?,?,?)',
        [
          data?.startTmpstmp,
          data?.endTmpstmp,
          data?.timeZone,
          data?.domainId,
          data?.agentName,
          data?.disposition,
          data?.offset,
          data?.limit,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getfacebookAgentACWReportHandlerDaoForSchedule - Unexpected Error:', err);
            console.log('up_ccaas_Facebook_agent_ACW_report error: ', err);
          }
          resolve(result?.length > 0 ? result?.[3] : []);
        }
      );
    } catch (error) {
      console.log('up_ccaas_Facebook_agent_ACW_report catch error: ', error);
      reject(error);
      logger.error('getfacebookAgentACWReportHandlerDaoForSchedule - Unexpected Error:', error);
    }
  });
};
