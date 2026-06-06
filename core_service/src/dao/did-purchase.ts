import { mysqlPoolConnection, notificationMessages } from '../plugins/db';

/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */
import axios from 'axios';
import moment from 'moment';
import { v4 as uuidv4 } from 'uuid';
import { ROLES } from '../helpers/constants';
import { logger } from '../plugins/winston';
import { insertNotificationDao } from './notification.dao';

// /**
//  *
//  * @param data
//  * @returns
//  */
// export const didPurchase = (data: any) => {
//   return new Promise((resolve, reject) => {
//     try {
//       const callFlow = JSON.stringify(data.callFlowId);
//       const currentDate = moment(new Date()).format('YYYY-MM-DD HH:mm:ss');
//       const didPruchase = `INSERT INTO did_purchase (user_id,type,city,your_number,createdAt,domainId,callFlowId,recording,queueName,isCall,isSms,aliasName,outboundSms,smsThreshold,status,supervisorName) VALUES (${data.user_id},'${data.type}', '${data.city}',${data.your_number},'${currentDate}',${data.domainId},'${callFlow}',${data.recording},'${data.queueName}',${data.isCall},${data.isSms},'${data.aliasName}',${data.outboundSms},${data.smsThreshold},${data.status},'${data.supervisorName}')`;
//       mysqlPoolConnection.query(didPruchase, (err, result) => {
//         if (err) {
//           reject(err);
//         } else {
//           resolve(result);
//         }
//       });
//       const adminData = `SELECT * from user where roleid = ${ROLES.ADMIN_ROLE_ID}`;
//       mysqlPoolConnection.query(adminData, (err1, result1) => {
//         if (err1) {
//           reject(err1);
//         } else {
//           for (const resultone of result1) {
//             const data1 = {
//               id: uuidv4(),
//               type: 'Admin purchased new number',
//               message: 'New Contact center number has been added in the application',
//             };
//             notificationMessages(data1, resultone?.ext);
//             insertNotificationDao({
//               uuid: data1.id,
//               type: data1.type,
//               domainId: data.domainId,
//               toExt: resultone?.ext,
//               notificationMsg: data1?.message,
//               channelType: 'settings',
//               markAsRead: 1,
//               timeStamp: new Date().getTime(),
//             });
//           }
//           resolve(result1);
//         }
//       });
//     } catch (error) {
//       reject(error);
//     }
//   });
// };

/**
 *
 * @param data
 * @returns
 */
export const getDid = (data :any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_get_did_purchase_by_pid(?,?)',
        [
          data?.domainId,
          data?.pid
        ],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('ccaas_get_did_purchase_by_pid - Unexpected Error:', err);
          } else {
            resolve(result[0]);
          }
        });
    } catch (error) {
      reject(error);
      logger.error('ccaas_get_did_purchase_by_pid - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @param datau
 * @returns
 */
export const updateDid = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_update_did_purchase(?,?,?,?)',
        [
          data?.your_number ?? null,
          data?.domainId,
          data?.callFlowId ?? null,
          data?.recording
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('up_update_did_purchase - Unexpected Error:', err);
          }
          resolve(result[0]);
        }
      );
    } catch (err) {
      reject(err);
      logger.error('up_update_did_purchase - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param qdata
 * @returns
 */
export const deleteDid = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      const didDelete = [data.pid];
      mysqlPoolConnection.query(
        'CALL ccaas_delete_did_purchase_by_id(?,?)',
        [
          data.domainId,
          didDelete
        ],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('ccaas_delete_did_purchase_by_id - Unexpected Error:', err);
          } else {
            resolve(result);
          }
        });
    } catch (error) {
      reject(error);
      logger.error('ccaas_delete_did_purchase_by_id - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @param datau
 * @returns
 */
export const getDidPurchaseDetails = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_getDidPurchaseByDomainID(?)',
        [
          data?.domainId
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccaas_getDidPurchaseByDomainID - Unexpected Error:', err);
          }
          resolve(result[0]);
        }
      );
    } catch (err) {
      reject(err);
      logger.error('ccaas_getDidPurchaseByDomainID - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getSearchList = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_get_did_purchase_list(?,?,?,?,?)',
        [
          data?.domainId,
          data.searchBy,
          data.searchByValue,
          data.offset,
          data.limit
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccaas_get_did_purchase_list - Unexpected Error:', err);
          } else {
            resolve(result);
          }
        });
    } catch (error) {
      reject(error);
      logger.error('ccaas_get_did_purchase_list - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const didNumberEdit = (data: any, pid: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_didNumberEdit(?,?,?,?,?,?,?,?,?,?)',
        [
          pid ?? 0,
          data?.domainId,
          data?.aliasName ?? null,
          data?.outboundSms ?? null,
          data?.queueName ?? null,
          data?.smsThreshold ?? null,
          data?.supervisorName ?? null,
          data?.knowledgeBasedId ?? null,
          data?.isBot ?? null,
          data?.autoAssignAgent ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccaas_didNumberEdit - Unexpected Error:', err);
          }
          resolve(result[0]);
        }
      );
    } catch (error) {
      reject(error);
      logger.error('ccaas_didNumberEdit - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @param qdata
 * @returns
 * update didnumber for sms
 */

export const didStatusEdit = (data: any, pid: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_update_did_purchase_by_id(?,?,?,?)',
        [
          data.domainId,
          pid,
          data.status,
          data.isSms
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccaas_update_did_purchase_by_id - Unexpected Error:', err);
          }else {
            resolve(result);
          }
        }
      );
    } catch (error) {
      reject(error);
      logger.error('ccaas_update_did_purchase_by_id - Unexpected Error:', error);
    }
  });
};

export const deleteSipTrunkNumDb = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_siptrunkMapNumberDelete(?,?)',
        [data?.domainId, data?.number],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccaas_siptrunkMapNumberDelete - Unexpected Error:', err);
          }
          resolve(result);
        }
      );
    } catch (error) {
      reject(error);
      logger.error('ccaas_siptrunkMapNumberDelete - Unexpected Error:', error);
    }
  });
};

export const sipTrunckNumCheckDB = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_getDidNumberbyCallFlowId(?,?)',
        [data?.domainId, data?.number],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccaas_getDidNumberbyCallFlowId - Unexpected Error:', err);
          }
          resolve(result);
        }
      );
    } catch (error) {
      reject(error);
      logger.error('ccaas_getDidNumberbyCallFlowId - Unexpected Error:', error);
    }
  });
};

export const indicateQAToNlpDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      await axios({
        method: 'post',
        url: process.env.NLP_QA_CREATE_INDICATE,
        // url: 'http://46.43.144.145:5568/template_qa',
        headers: {
          'Content-Type': 'application/json'
        },
        data: {
          domainId: data?.domainId,
          chatId: data?.chatId,
          ext: data?.ext
        }
      })
        .then((response) => {
          resolve(response);
        })
        .catch((error) => {
          reject(error);
          logger.error('indicateQAToNlpDao - Unexpected Error:', error);
        });
    } catch (err) {
      reject(err);
      logger.error('indicateQAToNlpDao - Unexpected Error:', err);
    }
  });
};
export const getDomainDidList = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_DID_List(?,?,?,?,?,?)',
        [
          data?.domainId,
          data?.search ?? null,
          data?.numberType ?? null,
          data?.capability ?? null,
          data?.offset ?? null,
          data?.limit ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccaas_get_DID_List - Unexpected Error:', err);
          }
          resolve(result);
        }
      );
    } catch (error) {
      reject(error);
      logger.error('ccaas_get_DID_List - Unexpected Error:', error);
    }
  });
};
