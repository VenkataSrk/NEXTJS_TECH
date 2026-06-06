import { logger } from '../../src/plugins/log';
import { mysqlPoolConnection } from '../plugins/db';

/**
 * @description get Answered Call Report
 * @param data
 * @returns
 */
export const getAnsweredCallReport = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_answered_call_profile_report(?,?,?,?,?,?,?,?)',
        [
          data.startTmpstmp,
          data.endTmpstmp,
          data.timeZone,
          data.domainId,
          data.queueName,
          data.agentName,
          data.limit,
          data.offset,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log(err);
            logger.error('getAnsweredCallReport - Unexpected Error:', err);
          }
          resolve(result);
        }
      );
    } catch (err) {
      reject(err);
      logger.error('getAnsweredCallReport - Unexpected Error:', err);
    }
  });
};

/**
 * @description get Answered Call Report
 * @param data
 * @returns
 */
export const getAnsweredCallReportForSchedule = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_answered_call_profile_report(?,?,?,?,?,?,?,?)',
        [
          data.startTmpstmp,
          data.endTmpstmp,
          data.timeZone,
          data.domainId,
          data.queueName,
          data.agentName,
          data.limit,
          data.offset,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log(err);
            logger.error('getAnsweredCallReportForSchedule - Unexpected Error:', err);
          }
          resolve(result?.length > 0 ? result?.[3] : []);
        }
      );
    } catch (err) {
      reject(err);
      logger.error('getAnsweredCallReportForSchedule - Unexpected Error:', err);
    }
  });
};

/**
 * get missed Call List
 * @param data
 * @returns
 */
export const missedCallListDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_missed_call_report(?,?,?,?,?,?,?,?)',
        [
          data?.startTmpstmp,
          data?.endTmpstmp,
          data?.timeZone,
          data?.domainId,
          data?.queueIDs,
          data?.agentIDs,
          data?.offset,
          data?.limit,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log('ccaas_missed_call_report err: ', err);
            logger.error('missedCallListDao - Unexpected Error:', err);
          }
          resolve(result);
        }
      );
    } catch (err) {
      reject(err);
      logger.error('missedCallListDao - Unexpected Error:', err);
    }
  });
};

/**
 * get missed Call List
 * @param data
 * @returns
 */
export const missedCallListDaoForSchedule = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_missed_call_report(?,?,?,?,?,?,?,?)',
        [
          data?.startTmpstmp,
          data?.endTmpstmp,
          data?.timeZone,
          data?.domainId,
          data?.queueIDs,
          data?.agentIDs,
          data?.offset,
          data?.limit,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log('ccaas_missed_call_report err: ', err);
            logger.error('missedCallListDaoForSchedule - Unexpected Error:', err);
          }
          resolve(result?.length > 0 ? result?.[3] : []);
        }
      );
    } catch (err) {
      reject(err);
      logger.error('missedCallListDaoForSchedule - Unexpected Error:', err);
    }
  });
};

/**
 * get interAction
 * @param data
 * @returns
 */
export const interActionDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_interaction_report(?,?,?,?,?,?,?,?,?,?)',
        [
          data.startTmpstmp,
          data.endTmpstmp,
          data.timeZone,
          data.domainId,
          data.queueName,
          data.agentName,
          data.direction,
          data.disposition,
          data.limit,
          data.offset,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log('ccaas_interaction_call_report reject err: ', err);
          }
          resolve(result);
        }
      );
    } catch (err) {
      console.log('ccaas_interaction_call_report catch block err: ', err);
      logger.error('interActionDao - Unexpected Error:', err);
      reject(err);
    }
  });
};

export const interActionDaoForSchedule = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_interaction_report(?,?,?,?,?,?,?,?,?,?)',
        [
          data.startTmpstmp,
          data.endTmpstmp,
          data.timeZone,
          data.domainId,
          data.queueName,
          data.agentName,
          data.direction,
          data.dispositionIds,
          data.limit,
          data.offset,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log('ccaas_interaction_call_report reject err: ', err);
          }
          resolve(result?.length > 0 ? result[5] : []);
        }
      );
    } catch (err) {
      console.log('ccaas_interaction_call_report catch block err: ', err);
      logger.error('interActionDaoForSchedule - Unexpected Error:', err);
      reject(err);
    }
  });
};

export const editReportColumnDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    const editColumnName = JSON.stringify(data?.editColumn);
    try {
      mysqlPoolConnection.query(
        'call up_InsertUpdatereport_config(?,?,?,?)',
        [
          data?.domainId,
          data?.ext,
          editColumnName,
          data?.reportType,
        ],
        (err: any, result: any) => {
          if (err) {
            console.log('up_InsertUpdatereport_config error: ', err);
            reject(err);
          }
          resolve(result);
        }
      );
    } catch (err) {
      reject(err);
      console.log('up_InsertUpdatereport_config catch error: ', err);
      logger.error('editReportColumnDao - Unexpected Error:', err);
    }
  });
};

/**
 * get Edit Report Column
 * @param data
 * @returns
 */
export const getEditReportColumnDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_edit_report_column_dao(?, ?, ?, ?)',
        [
          data?.domainId,
          data?.ext,
          data.reportType,
          data.reportId,
        ],
        (err: any, result: any) => {
          if (err) {
            console.log('getEditReportColumnDao error: ', err);
            reject(err);
            logger.error('getEditReportColumnDao - Unexpected Error:', err);
          }
          resolve(result);
        }
      );
    } catch (err) {
      reject(err);
      logger.error('getEditReportColumnDao - Unexpected Error:', err);
    }
  });
};

export const getEditReportColumnDao1 = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {

      mysqlPoolConnection.query(
        'call up_GetReportEditColumn(?,?,?,?)',
        [
          data?.reportId,
          data?.domainId,
          data?.ext,
          data?.reportType
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log('up_GetReportEditColumn err: ', err);
            logger.error('editReportColumnDao - Unexpected Error:', err);
          }
          resolve(result[0]);
        }
      );
    } catch (error) {
      console.log('getEditReportColumnDao1 error catch: ', error);
      reject(error);
      logger.error('missedCallListDao - Unexpected Error:', error);
    }
  });
};

export const outboundCallReport = (data: any) => {
  console.log(data);
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_outbound_call_summary_report(?,?,?,?,?,?,?,?,?,?)',
        [
          data?.startTmpstmp,
          data?.endTmpstmp,
          data?.timeZone,
          data?.domainId,
          data?.queueId,
          data?.agentId,
          data?.direction,
          data?.limit,
          data?.offset,
          data?.dispositionId,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log(err);
            logger.error('outboundCallReport - Unexpected Error:', err);
          }
          resolve(result);
        }
      );
    } catch (err) {
      reject(err);
      logger.error('outboundCallReport - Unexpected Error:', err);
    }
  });
};

export const outboundCallReportForSchedule = (data: any) => {
  console.log(data);
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_outbound_call_summary_report(?,?,?,?,?,?,?,?,?,?)',
        [
          data?.startTmpstmp,
          data?.endTmpstmp,
          data?.timeZone,
          data?.domainId,
          data?.queueId ?? null,
          data?.agentId ?? null,
          data?.direction ?? null,
          data?.limit ?? null,
          data?.offset ?? null,
          data?.dispositionIds ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('outboundCallReportForSchedule - Unexpected Error:', err);
            console.log(err);
          }
          resolve(result?.length > 0 ? result?.[4] : []);
        }
      );
    } catch (err) {
      reject(err);
      logger.error('outboundCallReportForSchedule - Unexpected Error:', err);
    }
  });
};

export const getFilterValuesDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_filter_values_dao(?,?,?)',
        [
          data?.domainId,
          data?.startTmpstmp,
          data.endTmpstmp,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getFilterValuesDao - Unexpected Error:', err);
            console.log(err);
          }
          resolve(result);
        }
      );
    }catch (err) {
      reject(err);
      logger.error('getFilterValuesDao - Unexpected Error:', err);
    }
  });
};

export const getFilterValuesForOutBoundDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_filter_values_for_outbound_dao(?,?,?)',
        [
          data?.domainId,
          data?.startTmpstmp,
          data.endTmpstmp,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getFilterValuesForOutBoundDao - Unexpected Error:', err);
            console.log(err);
          }
          resolve(result);
        }
      );
    } catch (err) {
      reject(err);
      logger.error('getFilterValuesForOutBoundDao - Unexpected Error:', err);
    }
  });
};

export const getFilterValuesChatDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_filter_values_chat_dao(?,?,?)',
        [
          data?.domainId,
          data?.startTmpstmp,
          data.endTmpstmp,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getFilterValuesChatDao - Unexpected Error:', err);
            console.log(err);
          }
          resolve(result);
        }
      );
    } catch (err) {
      reject(err);
      logger.error('getFilterValuesChatDao - Unexpected Error:', err);
    }
  });
};

export const getTranferCallReport = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_Transfer_Call_Report(?,?,?,?,?,?,?,?,?,?)',
        [
          data.startTmpstmp,
          data.endTmpstmp,
          data.timeZone,
          data.domainId,
          data.queueName,
          data.agentName,
          data.disposition,
          data.TransferedId ,
          data.limit,
          data.offset,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getTranferCallReport - Unexpected Error:', err);
          }
          resolve(result);
        }
      );
    } catch (err) {
      reject(err);
      logger.error('getTranferCallReport - Unexpected Error:', err);
    }
  });
};

export const getTranferCallReportForSchedule = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_Transfer_Call_Report(?,?,?,?,?,?,?,?,?,?)',
        [
          data.startTmpstmp,
          data.endTmpstmp,
          data.timeZone,
          data.domainId,
          data.queueName,
          data.agentName,
          data.disposition,
          data.TransferedId ,
          data.limit,
          data.offset,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getTranferCallReportForSchedule - Unexpected Error:', err);
          }
          resolve(result?.length > 0 ? result?.[4] : []);
        }
      );
    } catch (err) {
      reject(err);
      logger.error('getTranferCallReportForSchedule - Unexpected Error:', err);
    }
  });
};

export const getCallBackSummaryDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_CcaasCallBackSummaryReport (?,?,?,?,?,?,?,?,?)',
        [
          data?.startTmpstmp,
          data?.endTmpstmp,
          data?.timeZone,
          data?.domainId,
          data?.queueIds,
          data?.agentIds,
          data?.voiceMail,
          data?.offset,
          data?.limit,
        ],
        (err: any, result: any) => {
          console.log('getCallBackSummaryDao output: ');
          if (err) {
            console.log('getCallBackSummaryDao err: ', err);
            reject(err);
            logger.error('getCallBackSummaryDao - Unexpected Error:', err);
          }
          resolve(result);
        }
      );
    } catch (error) {
      console.log('getCallBackSummaryDao error catch: ', error);
      logger.error('Callbacksummary - Unexpected Error:', error);
    }
  });
};

export const getCallBackSummaryDaoForSchedule = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_CcaasCallBackSummaryReport (?,?,?,?,?,?,?,?,?)',
        [
          data?.startTmpstmp,
          data?.endTmpstmp,
          data?.timeZone,
          data?.domainId,
          data?.queueIds,
          data?.agentIds,
          data?.voiceMail,
          data?.offset,
          data?.limit,
        ],
        (err: any, result: any) => {
          console.log('getCallBackSummaryDao output: ');
          if (err) {
            console.log('getCallBackSummaryDao err: ', err);
            reject(err);
            logger.error('getCallBackSummaryDao - Unexpected Error:', err);
          }
          resolve(result?.length > 0 ? result?.[3] : []);
        }
      );
    } catch (error) {
      console.log('getCallBackSummaryDao error catch: ', error);
      logger.error('getCallBackSummaryDaoForSchedule - Unexpected Error:', error);
    }
  });
};

export const inboundCallReport = (data: any) => {
  console.log(data);
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_inbound_call_summary_report(?,?,?,?,?,?,?,?,?,?)',
        [
          data?.startTmpstmp,
          data?.endTmpstmp,
          data?.timeZone,
          data?.domainId,
          data?.queueId,
          data?.agentId,
          data?.direction,
          data?.limit,
          data?.offset,
          data?.dispositionId,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log(err);
            logger.error('inboundCallReport - Unexpected Error:', err);
          }
          resolve(result);
        }
      );
    } catch (err) {
      reject(err);
      logger.error('inboundCallReport - Unexpected Error:', err);
    }
  });
};
