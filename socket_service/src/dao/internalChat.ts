import { mysqlPoolConnection } from '../plugins/db';

export const newInternalChatMsg = (data: any) => {
  console.log([
    data.intenalChatId,
    data.internalChatGroupId,
    data.domainId,
    JSON.stringify(data.msg),
    data.senderAgentId,
    data.agentIds,
  ]);

  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query('call up_ccaasInsertInternalChat(?,?,?,?,?,?)', [
        data.internalChatId,
        data.internalChatGroupId,
        data.domainId,
        JSON.stringify(data.msg),
        data.senderAgentId,
        data.agentIds,
      ],
                                (err: any, result: any) => {
                                  if (err) {
                                    reject(err);
                                  }
                                  resolve(result?.length > 0 ? result[0] : []);
                                }
      );
    } catch (error) {
      reject(error);
    }
  });
};

export const internalChatReadStatus = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaasUpdateInternalChatMsgStatus(?,?,?,?)', [
          data.internalChatId,
          data.domainId,
          data.senderAgentId,
          data.msgStatus,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
          }
          resolve(result?.length > 0 ? result[0] : []);
        }
      );
    } catch (error) {
      reject(error);
    }
  });
};
