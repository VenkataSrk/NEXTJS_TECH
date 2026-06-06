import { mysqlPoolConnection, notificationMessages } from '../plugins/db';

import moment from 'moment';
import { v4 as uuidv4 } from 'uuid';
import { logger } from '../plugins/winston';
import { insertNotificationDao } from './notification.dao';

/**
 *
 * @param data
 * @returns
 */
export const insertAgentStatus = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      const chennals = data.channels ? JSON.stringify(data.channels) : null;
      const currentDate = moment(new Date()).format('YYYY-MM-DD HH:mm:ss');
      mysqlPoolConnection.query(
        'call ccaas_insert_update_agent_status(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          data?.aid,
          data?.statusName,
          data?.type,
          data?.description,
          chennals,
          data?.status,
          data?.domainId,
          data?.isDeleted ?? 0,
          data?.isVoice,
          data?.isVoiceCallTransfer,
          data?.isChat,
          data?.isChatTransfer,
          data?.isEmail,
          data?.isEmailTransfer,
          data?.colorCode,
        ],
        (err, result) => {
          if (err) {
            logger.error('ccaas_insert_update_agent_status - Unexpected Error:', err);
            reject(err);
          } else {
            resolve({
              chennals,
              aid: result[0][0]?.aid,
              statusName: data.statusName,
              type: data.type,
              description: data.description,
              status: data.status,
              domainId: data.domainId,
              createdAt: currentDate,
              updatedAt: '',
              isVoice: data.isVoice,
              isVoiceCallTransfer: data.isVoiceCallTransfer,
              isChat: data.isChat,
              isChatCallTransfer: data.isChatCallTransfer,
              isEmail: data.isEmail,
              isEmailTransfer: data.isEmailTransfer,
              colorCode: data.colorCode,
            });
          }
        });
    } catch (error) {
      reject(error);
      logger.error('ccaas_insert_update_agent_status - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getAgentStatus = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_agent_status_dtl(?)',
        [
          data?.domainId,
        ],
        (err, result) => {
          if (err) {
            logger.error('ccaas_get_agent_status_dtl - Unexpected Error:', err);
            reject(err);
          } else {
            if (result && result[0]?.length > 0) {
              resolve(result[0]);
            } else {
              resolve([]);
            }
          }
        });
    } catch (error) {
      reject(error);
      logger.error('ccaas_get_agent_status_dtl - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getAgentStatusDesc = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_agent_status_dtl(?)',
        [
          data?.domainId,
        ],
        (err, result) => {
          if (err) {
            logger.error('ccaas_get_agent_status_dtl - Unexpected Error:', err);
            reject(err);
          } else {
            if (result && result[0]?.length > 0) {
              resolve(result[0]);
            } else {
              resolve([]);
            }
          }
        });
    } catch (error) {
      reject(error);
      logger.error('ccaas_get_agent_status_dtl - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @param aid
 * @returns
 */
export const updateAgentStatus = (data: any, aid: any) => {
  return new Promise((resolve, reject) => {
    try {
      const chennals = JSON.stringify(data.channels);
      mysqlPoolConnection.query(
        'call updateAgentsStatus(?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          aid,
          data?.domainId,
          data?.statusName,
          data?.type,
          data?.description,
          chennals,
          data?.status,
          data?.isVoice,
          data?.isVoiceCallTransfer,
          data?.isChat,
          data?.isChatTransfer,
          data?.isEmail,
          data?.isEmailTransfer,
          data?.colorCode,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('updateAgentsStatus - Unexpected Error:', err);
          }
          resolve(result.length > 0 ? result[0] : []);
        }
      );

    } catch (error) {
      reject(error);
      logger.error('updateAgentsStatus - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param aid
 * @param domainId
 * @returns
 */
export const deleteAgentStatus = (aid: any, domainId: any) => {
  return new Promise((resolve, reject) => {
    try {
      const username = `SELECT UserID, myAccUserId, UserName, emailId, directNumber, address, routing_profile_id, report_to_UserID, sipLoginId
, companyId, domainId, ext, isActive, callTypeId, inboundCall, outboundCampaignCall, roleid, localization
, currentSession, isVoice, isVoiceCallTransfer, isAvailableForVoice, isChat, isChatTransfer, isEmail, isEmailTransfer
, StatusID, isSms, isSocialMedia, companyName, addSkill, voice, chat, email, sms, socialMedia, chatLimit, createdAt
, updatedAt, stateTimer, AssignedTime, chatFlag, ProfileImage, emp_id
 FROM tb_User where StatusID = ${aid}`;
      mysqlPoolConnection.query(username, (error: any, result: any) => {
        if (error) {
          reject(error);
          logger.error('deleteAgentStatus - Unexpected Error:', error);
        } else {
          for (const i of result) {
            const data = {
              id: uuidv4(),
              delete_status: 1,
              type: 'Admin deleted agent status',
              message: 'Your status is Away now. Admin has Deleted the status. To change your status, go to your Profile menu',
            };
            notificationMessages(data, `${i?.domainId ?? ''}_${i?.ext ?? ''}`);
            insertNotificationDao({
              domainId,
              uuid: data.id,
              toExt: i?.ext,
              notificationMsg: data?.message,
              type: data.type,
              channelType: 'settings',
              markAsRead: 1,
              delete_status: 1,
              timeStamp: new Date().getTime(),
              coid: null,
              customerValue: null
            });
          }
          resolve(result);
        }
      });
      mysqlPoolConnection.query(
        'call ccaas_delete_agent_status(?,?)',
        [
          aid,
          domainId,
        ],
        (err, result) => {
          if (err) {
            logger.error('ccaas_delete_agent_status - Unexpected Error:', err);
            reject(err);
          } else {
            resolve(result);
          }
        });
    } catch (error) {
      reject(error);
      logger.error('ccaas_delete_agent_status - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getAgentStatusDetails = (data: any, aid: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_agent_status_by_aid(?,?)',
        [
          aid,
          data?.domainId,
        ],
        (err, result) => {
          if (err) {
            logger.error('ccaas_get_agent_status_by_aid - Unexpected Error:', err);
            reject(err);
          } else {
            resolve(result?.[0]);
          }
        });
    } catch (error) {
      reject(error);
      logger.error('ccaas_get_agent_status_by_aid - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const isValidAgentName = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_agent_status_statusName_availability_check(?,?)',
        [
          data?.statusName,
          data?.domainId,
        ],
        (err, result) => {
          if (err) {
            logger.error('ccaas_get_agent_status_statusName_availability_check - Unexpected Error:', err);
            reject(err);
          } else {
            resolve(result);
          }
        });
    } catch (error) {
      reject(error);
      logger.error('ccaas_get_agent_status_statusName_availability_check - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @param emailId
 * @returns
 */
export const updateAgentStatusValues = (data: any, emailId: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call updateAgentStatusValuesNew(?,?,?,?,?,?,?,?,?)',
        [
          emailId,
          data?.domainId,
          data?.isVoice,
          data?.isVoiceCallTransfer,
          data?.isChat,
          data?.isChatTransfer,
          data?.isEmail,
          data?.isEmailTransfer,
          data?.statusName ?? 'Not Ready'
        ],
        (err: any, result: any) => {
          if (err) {
            logger.error('updateAgentStatusValuesNew - Unexpected Error:', err);
            reject(err);
          }
          resolve(result);
        }
      );
    } catch (error) {
      reject(error);
      logger.error('updateAgentStatusValuesNew - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getEmailHistory = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccass_getDetailsBySessionId(?,?,?)',
        [
          data.domainId,
          data?.channelType ?? null,
          data?.EmailSessionId ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            logger.error('ccass_getDetailsBySessionId - Unexpected Error:', err);
            reject(err);
          } else {
            if (result && result[0]?.length > 0) {
              resolve(result[0]);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (err) {
      logger.error('ccass_getDetailsBySessionId - Unexpected Error:', err);
      reject(err);
    }
  });
};
