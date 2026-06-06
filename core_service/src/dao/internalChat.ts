import { ICreateinternalChat, IDelete, IEdit, IGetinternalChat, IGetparticularInternalChat, IInternalChatByHistoryId, IReadInternalChat } from '../modules/internalChat/interface';
import { mysqlPoolConnection } from '../plugins/db';
import { logger } from '../plugins/winston';

export const create = (data: ICreateinternalChat) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query('call up_ccaasInsertUpdateinternalChatGroup(?,?,?,?,?,?,?,?,?)', [
        data.internalChatGroupId,
        data.groupName,
        data.historyId,
        data.channelType,
        data.customerId,
        data.hostUserId,
        data.agentUserId,
        data.queueId,
        data.domainId,
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('up_ccaasInsertUpdateinternalChatGroup - Unexpected Error:', err);
        } else {
          resolve(result?.length > 0 ? result[0] : []);
        }
      });
    } catch (error) {
      reject(error);
      logger.error('up_ccaasInsertUpdateinternalChatGroup - Unexpected Error:', error);
    }
  });
};

export const getInternalChat = (data: IGetinternalChat) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query('call up_ccaasGetInternalChatGroupList(?,?)', [
        data.domainId,
        data.hostUserId
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('up_ccaasGetInternalChatGroupList - Unexpected Error:', err);
        } else {
          resolve(result?.length > 0 ? result[0] : []);
        }
      });
    } catch (error) {
      reject(error);
      logger.error('up_ccaasGetInternalChatGroupList - Unexpected Error:', error);
    }
  });
};

export const getParticularInternalChat = (data: IGetparticularInternalChat) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query('call up_ccaasGetInternalChat(?,?,?)', [
        data.internalChatGroupId,
        data.domainId,
        data.hostUserId
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('up_ccaasGetInternalChat - Unexpected Error:', err);
        } else {
          resolve(result?.length > 0 ? result[0] : []);
        }
      });
    } catch (error) {
      reject(error);
      logger.error('up_ccaasGetInternalChat - Unexpected Error:', error);
    }
  });
};

export const isEdit = (data: IEdit) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query('call up_ccaasUpdateInternalChatMsg(?,?,?,?)', [
        data.internalChatId,
        data.domainId,
        JSON.stringify(data.msg),
        data.agentId
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('up_ccaasUpdateInternalChatMsg - Unexpected Error:', err);
        } else {
          resolve(result?.length > 0 ? result[0] : []);
        }
      });
    } catch (error) {
      reject(error);
      logger.error('up_ccaasUpdateInternalChatMsg - Unexpected Error:', error);
    }
  });
};

export const isDelete = (data: IDelete) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query('call up_ccaasDeleteInternalChatMsg(?,?,?)', [
        data.internalChatId,
        data.domainId,
        data.agentId
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('up_ccaasDeleteInternalChatMsg - Unexpected Error:', err);
        } else {
          resolve(result?.length > 0 ? result[0] : []);
        }
      });
    } catch (error) {
      reject(error);
      logger.error('up_ccaasDeleteInternalChatMsg - Unexpected Error:', error);
    }
  });
};

export const markAsRead = (data: IReadInternalChat) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query('call up_ccaasInternalChatAllMsgStatus(?,?,?,?)', [
        data.internalChatGroupId,
        data.agentId,
        data.domainId,
        data.msgStatus

      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('up_ccaasInternalChatAllMsgStatus - Unexpected Error:', err);
        } else {
          resolve(result?.length > 0 ? result[0] : []);
        }
      });
    } catch (error) {
      reject(error);
      logger.error('up_ccaasInternalChatAllMsgStatus - Unexpected Error:', error);
    }
  });
};

export const getHistoryId = (data: IInternalChatByHistoryId) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query('call up_ccaasGetInternalChatsForHistoryId(?,?,?,?)', [
        data.historyId,
        data.channelType,
        data.agentId,
        data.domainId,
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('up_ccaasGetInternalChatsForHistoryId - Unexpected Error:', err);
        } else {
          resolve(result?.length > 0 ? result[0] : []);
        }
      });
    } catch (error) {
      reject(error);
      logger.error('up_ccaasGetInternalChatsForHistoryId - Unexpected Error:', error);
    }
  });
};
