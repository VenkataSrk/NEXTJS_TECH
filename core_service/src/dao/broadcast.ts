import { ICreateBroadCast, IDeleteAgentDtl, IDeleteBoardCastMsg, IGetAllGroup, IGetBoardCastMsg, IGetBroadCastGroup, IInsertBoardCastMsg } from '../../src/modules/broadcast/interface';
import { mysqlPoolConnection } from '../plugins/db';
import { logger } from '../plugins/winston';

export const insertBroadCastDao = (data: ICreateBroadCast) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query('call bc_insert_createBroadCastGroup(?,?,?,?,?)', [
        data?.domainId,
        data?.bcHistoryId ?? null,
        data?.bcGroupName,
        data?.bcMembers,
        data?.createBy,

      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('bc_insert_createBroadCastGroup - Unexpected Error:', err);
        } else {
          resolve(result?.length > 0 ? result[0] : []);
        }
      });
    } catch (error) {
      reject(error);
      logger.error('bc_insert_createBroadCastGroup - Unexpected Error:', error);
    }
  });
};

export const getAllGroupDetailsDao = (data: IGetAllGroup) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query('call bc_get_AllGroupDtl(?,?)', [
        data?.domainId,
        data?.userId
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('bc_get_AllGroupDtl - Unexpected Error:', err);
        } else {
          resolve(result?.length > 0 ? result[0] : []);
        }
      });
    } catch (error) {
      reject(error);
      logger.error('bc_get_AllGroupDtl - Unexpected Error:', error);
    }
  });
};

export const getBroadCastGroupDao = (data: IGetBroadCastGroup) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query('call bc_get_BroadCastAgentDtl(?,?,?)', [
        data?.domainId,
        data?.bcGroupName,
        data?.bcMembers ?? null
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('bc_get_BroadCastAgentDtl - Unexpected Error:', err);
        } else {
          resolve(result?.length > 0 ? result : []);
        }
      });
    } catch (error) {
      reject(error);
      logger.error('bc_get_BroadCastAgentDtl - Unexpected Error:', error);
    }
  });
};

export const deleteBroadcastAgentDetailsDao = (data: IDeleteAgentDtl) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query('call bc_delete_BroadCastAgentDtl(?,?,?)', [
        data?.domainId,
        data?.bcGroupName,
        data?.bcMembers,
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('bc_delete_BroadCastAgentDtl - Unexpected Error:', err);
        } else {
          resolve(result?.length > 0 ? result[0] : []);
        }
      });
    } catch (error) {
      reject(error);
      logger.error('bc_delete_BroadCastAgentDtl - Unexpected Error:', error);
    }
  });
};

export const  insertBroadcastGroupMsgDao = (data: IInsertBoardCastMsg) => {
  return new Promise((resolve, reject) => {
    const expiryDateValue = data?.expiryDate === '' ? null : data?.expiryDate;
    try {
      mysqlPoolConnection.query('call bc_insert_BroadCastGroupMessage(?,?,?,?,?,?)', [
        data?.bcHistoryId ,
        data?.domainId,
        data?.message ,
        data?.AgentId,
        expiryDateValue,
        data?.sendTimestamp
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('bc_insert_BroadCastGroupMessage - Unexpected Error:', err);
        } else {
          resolve(result?.length > 0 ? result[0] : []);
        }
      });
    } catch (error) {
      reject(error);
      logger.error('bc_insert_BroadCastGroupMessage - Unexpected Error:', error);
    }
  });
};

export const  getBroadcastGroupMsgDao = (data: IGetBoardCastMsg) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query('call bc_get_broadCastGroupMessageDtl(?,?,?)', [
        data?.domainId,
        data?.bcHistoryId ,
        data?.AgentId,
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('bc_get_broadCastGroupMessageDtl - Unexpected Error:', err);
        } else {
          resolve(result?.length > 0 ? result[0] : []);
        }
      });
    } catch (error) {
      reject(error);
      logger.error('bc_get_broadCastGroupMessageDtl - Unexpected Error:', error);
    }
  });
};

export const  deleteBroadcastGroupMsgDao = (data: IDeleteBoardCastMsg) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query('call bc_delete_BroadCastGroupMessage(?,?,?)', [
        data?.bcDurationHistoryId ,
        data?.bcHistoryId ,
        data?.domainId,
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('bc_delete_BroadCastGroupMessage - Unexpected Error:', err);
        } else {
          resolve(result?.length > 0 ? result[0] : []);
        }
      });
    } catch (error) {
      reject(error);
      logger.error('bc_delete_BroadCastGroupMessage - Unexpected Error:', error);
    }
  });
};
