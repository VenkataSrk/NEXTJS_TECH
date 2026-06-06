"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.internalChatReadStatus = exports.newInternalChatMsg = void 0;
const db_1 = require("../plugins/db");
const newInternalChatMsg = (data) => {
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
            db_1.mysqlPoolConnection.query('call up_ccaasInsertInternalChat(?,?,?,?,?,?)', [
                data.internalChatId,
                data.internalChatGroupId,
                data.domainId,
                JSON.stringify(data.msg),
                data.senderAgentId,
                data.agentIds,
            ], (err, result) => {
                if (err) {
                    reject(err);
                }
                resolve((result === null || result === void 0 ? void 0 : result.length) > 0 ? result[0] : []);
            });
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.newInternalChatMsg = newInternalChatMsg;
const internalChatReadStatus = (data) => {
    return new Promise((resolve, reject) => {
        try {
            db_1.mysqlPoolConnection.query('call up_ccaasUpdateInternalChatMsgStatus(?,?,?,?)', [
                data.internalChatId,
                data.domainId,
                data.senderAgentId,
                data.msgStatus,
            ], (err, result) => {
                if (err) {
                    reject(err);
                }
                resolve((result === null || result === void 0 ? void 0 : result.length) > 0 ? result[0] : []);
            });
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.internalChatReadStatus = internalChatReadStatus;
//# sourceMappingURL=internalChat.js.map