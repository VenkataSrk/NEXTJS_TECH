import { mysqlPoolConnection } from '../plugins/db';
import { loggerError, loggerTrace } from '../plugins/logger';

/**
 * Get call flow map id
 * @param sid
 * @returns
 */
export const getchanneldata = (sid: number) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_channel_data(?)',
        [
          sid ?? null
        ],
        (e: any, result: any) => {
          loggerTrace('ccaas_get_channel_data', [sid], result);
          if (e) {
            loggerError('ccaas_get_channel_data', [sid], e);
            reject(e);
          } else {
            resolve(result?.length > 0 ? result?.[0] : null);
          }
        }
      );
    } catch (err) {
      loggerError('ccaas_get_channel_data', [sid], err);
      reject(err);
    }
  });
};

/**
 * Get call flow Details
 * @param sid
 * @returns
 */
export const getnodesdata = (callFlowId: number, domainId: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_nodes_data(?,?)',
        [
          domainId ?? null,
          callFlowId ?? null
        ],
        (e: any, result: any) => {
          loggerTrace('ccaas_get_nodes_data', [domainId, callFlowId], result);
          if (e) {
            loggerError('ccaas_get_nodes_data', [domainId, callFlowId], e);
            reject(e);
          } else {
            resolve(result?.length > 0 ? result?.[0]?.[0]?.nodes : null);
          }
        }
      );
    } catch (err) {
      loggerError('ccaas_get_nodes_data', [domainId, callFlowId], err);
      reject(err);
    }
  });
};

/**
 * Get call flow by id
 * @param callFlowId
 * @returns
 */
export const getnodesdataForNewCallFlow = (callFlowId: number) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_nodes_data_for_new_call_flow(?)',
        [
          callFlowId ?? null
        ],
        (e: any, result: any) => {
          loggerTrace('ccaas_get_nodes_data_for_new_call_flow', [callFlowId], result);
          if (e) {
            loggerError('ccaas_get_nodes_data_for_new_call_flow', [callFlowId], e);
            reject(e);
          } else {
            resolve(result?.length > 0 ? result?.[0]?.[0]?.nodes : null);
          }
        }
      );
    } catch (err) {
      loggerError('ccaas_get_nodes_data_for_new_call_flow', [callFlowId], err);
      reject(err);
    }
  });
};
