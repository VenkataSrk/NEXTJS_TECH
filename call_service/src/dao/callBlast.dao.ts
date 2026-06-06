import { logger } from '../../src/plugins/winston';
import { mysqlPoolConnection } from '../plugins/db';

export const updateCallBlastConfig = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_insert_update_callBlast(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?) ',
        [
          data.cid,
          data.campaignType,
          data.domainId,
          data.blastName,
          data.blastDescription,
          data.triggerType,
          JSON.stringify(data.triggerDailyFile) ?? null,
          JSON.stringify(data.triggerCustomFile) ?? null,
          data.timeZone,
          data.outboundNumber,
          data.callerId,
          data.campaignMessageType,
          JSON.stringify(data.campaignMessage) ?? null,
          JSON.stringify(data.textSpeechFile) ?? null,
          JSON.stringify(data.promptMessage) ?? null,
          data.createdBy,
          data.contactType,
          JSON.stringify(data.contact) ?? null,
          JSON.stringify(data.importContact) ?? null,
          JSON.stringify(data.importContactFile) ?? null,
          JSON.stringify(data.addContact) ?? null,
          data.companyName,
          JSON.stringify(data.connectingAudio),
          JSON.stringify(data.agentUnavailable),
          JSON.stringify(data.agentReject),
          data?.maxDuration

        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log('updateCallBlastConfig result', { data: JSON.stringify(data), Err: err });
            logger.error('updateCallBlastConfig result err ', { data: JSON.stringify(data), Err: err });
          }
          resolve(result?.length > 0 ? result?.[0] : []);
        }
      );
    } catch (error) {
      logger.error('updateCallBlastConfig catch err ', { data: JSON.stringify(data), Err: error });
    }
  });
};

export const getCustomerContactCallBlast = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_callBlast_cutomerContact_dtl(?,?,?,?,?,?,?,?,?) ',
        [
          data.domainId,
          data.doNotDisturb,
          data.customerType,
          data.search,
          data.customerTag,
          data.source,
          data.country,
          data.offset,
          data.limit
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getCustomerContactCallBlast result err ', { data, Err: err });
          }
          resolve(result?.length > 0 ? result : []);
        }
      );
    } catch (error) {
      reject(error);
      logger.error('getCustomerContactCallBlast catch err ', { data, Err: error });
    }
  });
};

export const getCallBlastList = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_callBlast_overAll_dtl(?,?,?,?,?,?,?,?,?) ',
        [
          data.blastId,
          data.domainId,
          data.campaignName,
          data.campaignType,
          data.createdBy,
          data.timeZone,
          data.offset,
          data.limit,
          data.statusName
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log('getCallBlastList err', { data, Err: err });
            logger.error('getCallBlastList catch err ', { data, Err: err });
          }
          resolve(result?.length > 0 ? result : []);
        }
      );
    } catch (error) {
      reject(error);
      logger.error('getCallBlastList catch err ', { data, Err: error });
      console.log('getCallBlastList catch', { data, Err: error });
    }
  });
};

export const getOverallCallBlastList = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_callBlast_overAll_Campaigns(?,?,?,?,?,?,?,?,?) ',
        [
          data.blastId,
          data.domainId,
          data.campaignName,
          data.campaignType,
          data.createdBy,
          data.timeZone,
          data.offset,
          data.limit,
          data.triggerStatus
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log('getOverallCallBlastList err', { data, Err: err });
            logger.error('getOverallCallBlastList catch err ', { data, Err: err });
          }
          resolve(result?.length > 0 ? result : []);
        }
      );
    } catch (error) {
      reject(error);
      console.log('getOverallCallBlastList catch', { data, Err: error });
      logger.error('getOverallCallBlastList catch err ', { data, Err: error });
    }
  });
};
export const updateCallBlastStutas = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_update_callBlast_status(?,?,?,?,?) ',
        [
          data.blastId,
          data.domainId,
          data.blastStatus,
          data.forceStop,
          data.pause
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('updateCallBlastStutas result err ', { data, Err: err });
          }
          resolve(result?.length > 0 ? result : []);
        }
      );
    } catch (error) {
      reject(error);
      logger.error('updateCallBlastStutas catch err ', { data, Err: error });
    }
  });
};

export const deleteCallblast = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_delete_callBlast_dtl(?,?) ',
        [
          data.blastId,
          data.domainId,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('deleteCallblast result err ', { data, Err: err });
          }
          resolve(result?.length > 0 ? result : []);
        }
      );
    } catch (error) {
      reject(error);
      logger.error('deleteCallblast catch err ', { data, Err: error });
    }
  });
};

export const getCallblastInfo = () => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_callBlast_info() ',
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log('getCallblastInfo err', { Err: err });
            logger.error('getCallblastInfo catch err ', { Err: err });
          }
          resolve(result?.length > 0 ? result?.[0] : []);
        }
      );
    } catch (error) {
      reject(error);
      console.log('getCallblastInfo', { Err: error });
      logger.error('getCallblastInfo catch err ', { Err: error });
    }
  });
};

export const getCallBlastHistoryList = (data: any) => {
  logger.info('getCallBlastHistoryList', data);
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_callBlast_overAll_history_dtl(?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          data.blastId,
          data.domainId,
          data.campaignName,
          data.campaignType,
          data.createdBy,
          data.timeZone,
          data.startDate,
          data.endDate,
          data.statusName,
          data.offset,
          data.limit,
          data.outboundNumber
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getCallBlastHistoryList result err ', { data, Err: err });
          }
          resolve(result?.length > 0 ? result : []);
        }
      );
    } catch (error) {
      reject(error);
      logger.error('getCallBlastHistoryList catch err ', { data, Err: error });
    }
  });
};

export const callBlastParticularHistory = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_callBlast_triggered_call_history_dtl(?,?,?,?,?,?,?,?,?)',
        [
          data.blastId,
          data.triggeredId,
          data.customerName,
          data.callStatus,
          data.agentName,
          data.phoneNumber,
          data.limit,
          data.offset,
          data.dispositionId
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('callBlastParticularHistory catch err ', { data, Err: err });
          }
          resolve(result?.length > 0 ? result : []);
        }
      );
    } catch (error) {
      reject(error);
      console.log('callBlastParticularHistory catch', { data, Err: error });
      logger.error('callBlastParticularHistory catch err ', { data, Err: error });
    }
  });
};

export const getCallBlastAutoAnswerDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_callBlast_campaignCallAutoAnswer(?)',
        [
          data.domainId,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getCallBlastAutoAnswerDao result err ', { data, Err: err });
          }
          resolve(result?.length > 0 ? result : []);
        }
      );
    } catch (error) {
      reject(error);
      logger.error('getCallBlastAutoAnswerDao catch err ', { data, Err: error });

    }
  });
};

export const insertUpdatCallBlastAutoAnswerHandlerDaos = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_inserUpdate_callBlast_campaignCallAutoAnswer(?,?)',
        [
          data.domainId,
          data.isAutoAnswer,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('insertUpdatCallBlastAutoAnswerHandlerDaos catch err ', { data, Err: err });
          }
          resolve(result?.length > 0 ? result?.[0] : []);
        }
      );
    } catch (error) {
      reject(error);
      logger.error('insertUpdatCallBlastAutoAnswerHandlerDaos catch err ', { data, Err: error });
    }
  });
};

export const getCustomerContactCallBlastFilterDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_callBlast_cutomerContact_dropDown_info(?) ',
        [
          data.domainId,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getCustomerContactCallBlastFilterDao result err ', { data, Err: err });
          }
          resolve(result?.length > 0 ? result : []);
        }
      );
    } catch (error) {
      reject(error);
      console.log('getCustomerContactCallBlastFilterDao catch', { data, Err: error });
      logger.error('getCustomerContactCallBlastFilterDao catch err ', { data, Err: error });
    }
  });
};

export const deleteOutboundCampaignDraftDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_delete_outbound_campaign_draft_chats(?,?) ',
        [
          data?.domainId,
          data?.campDraftId,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('deleteOutboundCampaignDraftDao result err ', { data, Err: err });
          }
          resolve(result?.length > 0 ? result : []);
        }
      );
    } catch (error) {
      reject(error);
      logger.error('deleteOutboundCampaignDraftDao catch err ', { data, Err: error });
    }
  });
};

export const getCallBlastOverallScheduledCampaignsDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {

      mysqlPoolConnection.query(
        'call ccaas_get_callBlast_overAll_scheduled_campaigns(?) ',
        [
          data?.domainId,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getCallBlastOverallScheduledCampaignsDao result err ', { data, Err: err });
          }

          console.log('resultresult', result);
          resolve(result?.length > 0 ? result[0] : []);
        }
      );
    } catch (error) {
      reject(error);
      logger.error('getCallBlastOverallScheduledCampaignsDao catch err ', { data, Err: error });
    }
  });
};

export const getCallBlastPerticularChildScheduledCampaignsDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {

      mysqlPoolConnection.query(
        'call ccaas_get_childblastId_history_dtl(?,?,?) ',
        [
          data?.blastId,
          data?.offset,
          data?.limit,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getCallBlastOverallScheduledCampaignsDao result err ', { data, Err: err });
          }

          console.log('resultresult', result);
          resolve(result?.length > 0 ? result : []);
        }
      );
    } catch (error) {
      reject(error);
      logger.error('getCallBlastOverallScheduledCampaignsDao catch err ', { data, Err: error });
    }
  });
};
