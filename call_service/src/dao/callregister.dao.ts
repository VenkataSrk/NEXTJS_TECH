import { logger } from '../../src/plugins/winston';
import { INcall, IRegisterDetails } from '../modules/call/interface';
import { mysqlPoolConnection } from '../plugins/db';

export const create = (data: IRegisterDetails) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_Call_register_details(?,?,?,?,?,?,?)', [
          data.ext,
          data.domainId,
          data.domainName,
          data.callerId,
          data.deviceType,
          data.deviceToken,
          data.authToken
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('create err', { data, Err: err });
          }
          resolve(result);
        }
      );
    } catch (error) {
      reject(error);
      logger.error('create catch', { data, Err: error });

    }
  });
};

export const callStatus = (data: INcall) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccasUpdateAgentAvailabilityForVoice(?,?,?)', [
          data.agentId,
          data.domainId,
          data.status
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('callStatus err', { data, Err: err });
          }
          resolve(result);
        }
      );
    } catch (error) {
      reject(error);
      logger.error('callStatus catch', { data, Err: error });

    }
  });
};

export const removeRegister = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_Delete_Call_Register_Dtl(?,?,?)', [
          data?.domainId,
          data?.ext,
          data?.deviceType,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('removeRegister err', { data, Err: err });
          }
          resolve(result);
        }
      );
    } catch (error) {
      reject(error);
      logger.error('removeRegister catch', { data, Err: error });
    }

  });
};

/**
 *
 * @param data
 * @returns
 */
export const getVoicemailDetails = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_ai_get_overall_voice_mail(?)', [
          data?.domainId
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('getVoicemailDetails err', { data, Err: err });
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            resolve([]);
          }
        }
      );
    } catch (error) {
      reject(error);
      logger.error('getVoicemailDetails catch', { data, Err: error });
    }

  });
};
