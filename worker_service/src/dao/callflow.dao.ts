import { mysqlPoolConnection } from '../plugins/db';
import { logger } from '../plugins/log';
import { loggerError, loggerTrace } from '../plugins/logger';

/**
 * Get agent details
 * @param UserID
 * @param domainId
 * @returns
 */
export const getAgentDetailsById = (UserID: any, domainId: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_getAgentDetails(?,?)',
        [
          domainId,
          UserID
        ],
        (err: any, result: any) => {
          loggerTrace('ccaas_getAgentDetails', [domainId, UserID], result);
          if (err) {
            loggerError('ccaas_getAgentDetails', [domainId, UserID], err);
            reject(err);
          } else {
            resolve(result?.length > 0 ? result?.[0] : []);
          }
        }
      );
    } catch (error) {
      loggerError('ccaas_getAgentDetails', [domainId, UserID], error);
      reject(error);
    }
  });
};

/**
 * Update current session for particular user
 * @param data
 * @returns
 */
export const updateAgentSessionId = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      if (data?.ext?.toString()?.trim().length === 3) {
        console.log('updateAgentSessionId payload', [data?.domainId, data?.ext, data?.currentSessionId]);
        mysqlPoolConnection.query(
          'call ccaas_updateAgentStatus(?,?,?)',
          [
            data?.domainId,
            data?.ext,
            data?.currentSessionId
          ],
          (err: any, result: any) => {
            loggerTrace('ccaas_updateAgentStatus', [data?.domainId, data?.ext, data?.currentSessionId], result);
            if (err) {
              loggerError('ccaas_updateAgentStatus err', [data?.domainId, data?.ext, data?.currentSessionId], err);
              reject(err);
            } else {
              resolve(result?.length > 0 ? result?.[0] : []);
            }
          }
        );
      } else {
        resolve([]);
      }
    } catch (error) {
      loggerError('ccaas_updateAgentStatus error', [data?.domainId, data?.ext, data?.currentSessionId], error);
      reject(error);
    }
  });
};

/**
 *
 * @param ddi
 * @returns
 */
export const isAgentStatusActive = (ddi: string) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_check_did_number_status(?)',
        [
          ddi,
        ],
        (err: any, result: any) => {
          loggerTrace('ccaas_get_check_did_number_status', [ddi], result);
          if (err) {
            loggerError('ccaas_get_check_did_number_status', [ddi], err);
            reject(err);
          } else {
            resolve(result?.length > 0 ? result?.[0] : null);
          }
        }
      );
    } catch (err) {
      loggerError('ccaas_get_check_did_number_status', [ddi], err);
      reject(err);
    }
  });
};

/**
 * Get customer number
 * @param customerNumber
 * @returns
 */
export const getCustomerDetails = (customerNumber: string) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_customer_details_by_phoneNumber(?)',
        [
          customerNumber,
        ],
        (err: any, result: any) => {
          loggerTrace('ccaas_get_customer_details_by_phoneNumber', [customerNumber], result);
          if (err) {
            loggerError('ccaas_get_customer_details_by_phoneNumber', [customerNumber], err);
            reject(err);
          } else {
            resolve(result?.length > 0 ? result?.[0] : null);
          }
        }
      );
    } catch (err) {
      loggerError('ccaas_get_customer_details_by_phoneNumber', [customerNumber], err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateCallBlastCompleted = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_update_callBlast_complete_status(?,?,?)',
        [
          data?.blastId,
          data?.domain_id,
          1
        ],
        (err: any, result: any) => {
          loggerTrace('ccaas_update_callBlast_complete_status', [data?.blastId, data?.domain_id, 1], result);
          if (err) {
            loggerError('ccaas_update_callBlast_complete_status', [data?.blastId, data?.domain_id, 1], err);
            reject(err);
          } else {
            resolve(result?.length > 0 ? result?.[0] : null);
          }
        }
      );
    } catch (err) {
      loggerError('ccaas_update_callBlast_complete_status', [data?.blastId, data?.domain_id, 1], err);
      reject(err);
    }
  });
};

/**
 *
 * @param ext
 * @param domainId
 * @returns
 */
export const getFiFOQueue = (ext: any, domainId: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_FIFO_queue(?,?)',
        [
          ext,
          domainId
        ],
        (err: any, result: any) => {
          loggerTrace('ccaas_get_FIFO_queue', [ext, domainId], result);
          if (err) {
            loggerError('ccaas_get_FIFO_queue', [ext, domainId], err);
            reject(err);
          } else {
            if (result && Array.isArray(result) && result?.[0]?.length > 0) {
              result?.[0]?.map((queue: any) => {
                if (queue?.queueList?.length > 0) {
                  resolve([queue]);
                }
              });
            }
          }
        }
      );
    } catch (err) {
      loggerError('ccaas_get_FIFO_queue', [ext, domainId], err);
      reject(err);
    }
  });
};

/**
 *
 * @param qid
 * @returns
 */
export const getHoursofOperationNew = (qid: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_business_hours_validation(?)',
        [
          qid ?? null
        ],
        (e: any, result: any) => {
          loggerTrace('ccaas_get_business_hours_validation', [qid], result);
          if (e) {
            loggerError('ccaas_get_business_hours_validation', [qid], e);
            reject(false);
          } else if (result && Array.isArray(result) && result?.[0]?.[0]?.isBusinessHours) {
            resolve(true);
          }
          resolve(false);
        }
      );
    } catch (err) {
      loggerError('ccaas_get_business_hours_validation', [qid], err);
      reject(false);
    }
  });
};

/**
 *
 * @param qid
 * @returns
 */
export const getHours = (qid: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_hours(?)',
        [
          qid
        ],
        (err: any, result: any) => {
          loggerTrace('ccaas_get_hours', [qid], result);
          if (err) {
            loggerError('ccaas_get_hours', [qid], err);
            reject(err);
          } else {
            if (result && Array.isArray(result) && result?.[0]?.length > 0) {
              const res = JSON.parse(result?.[0]?.[0]?.hours ?? 'null');
              if (res?.entry?.length > 0) {
                resolve({ startTimeStamp: res?.entry?.[0]?.startTimeStamp, endTimeStamp: res?.entry?.[0]?.endTimeStamp });
              } else {
                resolve({ startTimeStamp: 0, endTimeStamp: 0 });
              }
            }
          }
        }
      );
    } catch (err) {
      loggerError('ccaas_get_hours', [qid], err);
      reject(err);
    }
  });
};

/**
 * Get count for agent routing profile mapping
 * @param qid
 * @returns
 */
export const getAgentCount = (qid: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_agent_count(?)',
        [
          qid
        ],
        (err: any, result: any) => {
          loggerTrace('ccaas_get_agent_count', [qid], result);
          if (err) {
            loggerError('ccaas_get_agent_count', [qid], err);
            reject(err);
          } else {
            resolve(result?.length > 0 ? result?.[0]?.[0]?.userCount : 0);
          }
        }
      );
    } catch (err) {
      loggerError('ccaas_get_agent_count', [qid], err);
      reject(err);
    }
  });
};

/**
 * Get custome queue settings
 * @param qid
 * @returns
 */
export const getQueueSettings = (qid: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_queue_settings(?)',
        [
          qid
        ],
        (err: any, result: any) => {
          loggerTrace('ccaas_get_queue_settings', [qid], result);
          if (err) {
            loggerError('ccaas_get_queue_settings', [qid], err);
            reject(err);
          } else {
            resolve(result?.length > 0 ? result?.[0]?.[0] : null);
          }
        }
      );
    } catch (err) {
      loggerError('ccaas_get_queue_settings', [qid], err);
      reject(err);
    }
  });
};

/**
 * Validate business hours in call flow
 * @param cid
 * @returns
 */
export const getHoursofOperationByIdnew = (cid: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_businessHoursValidation(?)',
        [
          cid
        ],
        (err: any, result: any) => {
          loggerTrace('ccaas_get_businessHoursValidation', [cid], result);
          if (err) {
            loggerError('ccaas_get_businessHoursValidation', [cid], err);
            reject(false);
          } else if (result && result?.[0]?.[0]?.isBusinessHours) {
            resolve(true);
          }
          resolve(false);

        }
      );
    } catch (err) {
      loggerError('ccaas_get_businessHoursValidation', [cid], err);
      reject(err);
    }
  });
};

/**
 * Get routing profile
 * @param rpid
 * @param qid
 * @returns
 */
export const getRoutingProfile = (rpid: any, qid: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_profile_mapping_priority_by_qid(?,?)',
        [
          rpid,
          qid
        ],
        (err: any, result: any) => {
          loggerTrace('ccaas_get_profile_mapping_priority_by_qid', [rpid, qid], result);
          if (err) {
            loggerError('ccaas_get_profile_mapping_priority_by_qid', [rpid, qid], err);
            reject(err);
          } else {
            resolve(result?.length > 0 ? result?.[0] : null);
          }
        }
      );
    } catch (err) {
      loggerError('ccaas_get_profile_mapping_priority_by_qid', [rpid, qid], err);
      reject(err);
    }
  });
};
