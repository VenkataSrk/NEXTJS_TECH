"use strict";
var __awaiter = (this && this.__awaiter) || function (thisArg, _arguments, P, generator) {
    function adopt(value) { return value instanceof P ? value : new P(function (resolve) { resolve(value); }); }
    return new (P || (P = Promise))(function (resolve, reject) {
        function fulfilled(value) { try { step(generator.next(value)); } catch (e) { reject(e); } }
        function rejected(value) { try { step(generator["throw"](value)); } catch (e) { reject(e); } }
        function step(result) { result.done ? resolve(result.value) : adopt(result.value).then(fulfilled, rejected); }
        step((generator = generator.apply(thisArg, _arguments || [])).next());
    });
};
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.getUserListByExt = exports.getCustomerInfo = exports.getCustomerDetails = exports.insertNotificationDao = exports.getMaxWaitTime = exports.agentExtSocketIdDelete = exports.insertUpdateConcurrentChat = exports.getConcurrentChat = exports.getAgentDetails = exports.agentExtSocketIdMapping = exports.getUserList = exports.getSalesforceToken = exports.getLeadCRMinformation = exports.getAllSessionsDao = exports.getAllChatFlowIdDao = exports.getChatFlowIdDao = exports.saveChatFlowDao = exports.storeCochatDetailsByIDDao = exports.availableAgentData = exports.sendMessageToCoChat = exports.getAllMessageId = exports.coChatUpdateDao = exports.createCoChatChannel = exports.getChatByEndTimeDao = exports.getChatByNumberDao = exports.getCallSessionIdDao = exports.getChatSessionIdDao = exports.getCallSessionDao = exports.getChatSessionDao = exports.chatUpdateDao = exports.getChatHistory = exports.storeCustomerDetails = exports.getAllUserList = exports.storeMessageHistory = exports.getMessageWebChat = exports.createSession = exports.updateMessage = void 0;
const axios_1 = __importDefault(require("axios"));
const db_1 = require("../plugins/db");
/**
 *
 * @param data
 * @returns
 */
const updateMessage = (data) => {
    return new Promise((resolve, reject) => {
        try {
            const updateq = `UPDATE conversationHistory SET message = '${data}' WHERE id = '${data.id}'`;
            db_1.mysqlPoolConnection.query(updateq, (err, result) => __awaiter(void 0, void 0, void 0, function* () {
                if (err) {
                    reject(err);
                }
                else {
                    resolve(result);
                }
            }));
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.updateMessage = updateMessage;
// export const createCallSession = async (data: any, callflowid: any) => {
//   try {
//     const initialData: any = {
//       ipAddress: data?.ipAddress ? data?.ipAddress : '',
//       callFlowSourceId: callflowid || '',
//       sessionId: data.session_id,
//       browser: data?.browser ? data?.browser : '',
//       source: data?.source ? data?.source : '',
//       sessionStartTime: new Date().getTime() / 1000,
//       channelType: 'Call',
//       sessionEndTime: data?.sessionEndTime ? data?.sessionEndTime : null,
//       botStartTime: data?.botStartTime ? data?.botStartTime : null,
//       botEndTime: data?.botEndTime ? data?.botEndTime : null,
//       agentConnectTime: data?.agentConnectTime ? data.agentConnectTime : null,
//       agentDisconnectTime: data?.agentDisconnectTime ? data?.agentDisconnectTime : null,
//       lastMessage: data?.lastMessage ? data?.lastMessage : '',
//       botDetails: data?.botDetails ? data?.botDetails : {},
//       category: data?.category ? data?.category : '',
//       isBot: true,
//       agentComments: data?.agentComments ? data.agentComments : '',
//       agentDetails: data?.agentDetails ? data.agentDetails : '',
//       customerDetails: data?.cli ? data.cli : '',
//       OS: data?.OS ? data?.OS : '',
//       onlineStatus: true,
//       skillsIdentify: data?.skillsIdentify ? data?.skillsIdentify : '',
//       location: data?.location ? data?.location : '',
//       deviceType: data?.deviceType ? data?.deviceType : '',
//     };
//     return new Promise(async (resolve, reject) => {
//       /*   sessionHistoryModel.find({ chatSessionId: initialData.chatSessionId }).then((res) => {
//           ;
//           if (res && res.length > 0) {
//             sessionHistoryModel.updateOne({ chatSessionId: initialData.chatSessionId }, { $set: initialData }).then((result) => {
//               return true;
//             }).catch((err) => {
//               return err;
//             });
//           } else {
//             ;
//             sessionHistoryModel.create(initialData)
//             return true;
//           }
//         }).catch((err) => {
//           return err;
//         }); */
//       sessionHistoryModel
//         .create(initialData)
//         .then((response: any) => {
//           resolve(response);
//         })
//         .catch((errors: any) => {
//           reject(errors);
//         });
//     });
//   } catch (err: any) {
//     console.log(err);
//   }
// };
/* export const createSession = async (data: any, Id: any) => {
  try {
    const initialData: any = {
      ipAddress: data?.ipAddress ? data?.ipAddress : '',
      callFlowSourceId: data?.webChatId ? data.webChatId : '',
      sessionId: Id,
      browser: data?.browser ? data?.browser : '',
      source: data?.source ? data?.source : '',
      sessionStartTime: new Date().getTime() / 1000,
      channelType: 'Chat',
      sessionEndTime: data?.sessionEndTime ? data?.sessionEndTime : null,
      botStartTime: data?.botStartTime ? data?.botStartTime : null,
      botEndTime: data?.botEndTime ? data?.botEndTime : null,
      agentConnectTime: data?.agentConnectTime ? data.agentConnectTime : null,
      agentDisconnectTime: data?.agentDisconnectTime
        ? data?.agentDisconnectTime
        : null,
      lastMessage: data?.lastMessage ? data?.lastMessage : '',
      botDetails: data?.botDetails ? data?.botDetails : {},
      category: data?.category ? data?.category : '',
      isBot: true,
      agentComments: data?.agentComments ? data.agentComments : '',
      agentDetails: data?.agentDetails ? data.agentDetails : '',
      customerDetails: '',
      OS: data?.OS ? data?.OS : '',
      onlineStatus: true,
      skillsIdentify: data?.skillsIdentify ? data?.skillsIdentify : '',
      location: data?.location ? data?.location : '',
      deviceType: data?.deviceType ? data?.deviceType : '',
    };

    return new Promise(async (resolve, reject) => {
      sessionHistoryModel
        .find({ sessionId: initialData.sessionId })
        .then((res) => {
          if (res && res.length > 0) {
            sessionHistoryModel
              .updateOne(
                { sessionId: initialData.sessionId },
                { $set: initialData }
              )
              .then((result) => {
                return true;
              })
              .catch((err) => {
                return err;
              });
          } else {
            sessionHistoryModel.create(initialData);
            return true;
          }
        })
        .catch((err) => {
          return err;
        });
    });
  } catch (err: any) {
    console.log(err);
  }
}; */
/**
 *  chat history initial data
 */
const createSession = (data, id) => {
    console.log('<<<<<<<<<<<<<<<<<<<<<data>>>>>>>>>>>>>>>>>>>>>>>>>>>>', data);
    // console.log('<<<<<<<<<<<<<<<<<<<<<<message>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>', JSON.stringify(data?.message));
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        var _a, _b, _c, _d, _e, _f, _g, _h, _j, _k, _l, _m, _o, _p, _q, _r, _s, _t, _u, _v, _w, _x, _y, _z, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49;
        try {
            const botDetails = JSON.stringify(data === null || data === void 0 ? void 0 : data.botDetails);
            const customerData = JSON.stringify(data === null || data === void 0 ? void 0 : data.customerData);
            const message = JSON.stringify(data === null || data === void 0 ? void 0 : data.message);
            const customerCallBackData = JSON.stringify(data === null || data === void 0 ? void 0 : data.callBackRequest);
            db_1.mysqlPoolConnection.query('call ccaas_session_history_details(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)', [
                id,
                (_a = data === null || data === void 0 ? void 0 : data.sessionStartTime) !== null && _a !== void 0 ? _a : null,
                (_b = data === null || data === void 0 ? void 0 : data.sessionEndTime) !== null && _b !== void 0 ? _b : null,
                (_c = data === null || data === void 0 ? void 0 : data.botStartTime) !== null && _c !== void 0 ? _c : null,
                (_d = data === null || data === void 0 ? void 0 : data.botEndTime) !== null && _d !== void 0 ? _d : null,
                (_e = data === null || data === void 0 ? void 0 : data.agentConnectTime) !== null && _e !== void 0 ? _e : null,
                (_f = data === null || data === void 0 ? void 0 : data.agentDisconnectTime) !== null && _f !== void 0 ? _f : null,
                (_g = data === null || data === void 0 ? void 0 : data.ipAddress) !== null && _g !== void 0 ? _g : null,
                (_h = data === null || data === void 0 ? void 0 : data.source) !== null && _h !== void 0 ? _h : null,
                (_j = data === null || data === void 0 ? void 0 : data.lastMessage) !== null && _j !== void 0 ? _j : null,
                botDetails !== null && botDetails !== void 0 ? botDetails : null,
                (_k = data === null || data === void 0 ? void 0 : data.category) !== null && _k !== void 0 ? _k : null,
                true,
                (_l = data === null || data === void 0 ? void 0 : data.agentComments) !== null && _l !== void 0 ? _l : null,
                (_m = data === null || data === void 0 ? void 0 : data.agentDetails) !== null && _m !== void 0 ? _m : null,
                (_o = data === null || data === void 0 ? void 0 : data.cli) !== null && _o !== void 0 ? _o : null,
                (_p = data === null || data === void 0 ? void 0 : data.browser) !== null && _p !== void 0 ? _p : null,
                (_q = data === null || data === void 0 ? void 0 : data.OS) !== null && _q !== void 0 ? _q : null,
                true,
                (_r = data === null || data === void 0 ? void 0 : data.skillsIdentify) !== null && _r !== void 0 ? _r : null,
                (_s = data === null || data === void 0 ? void 0 : data.callFlowSourceId) !== null && _s !== void 0 ? _s : null,
                (_t = data === null || data === void 0 ? void 0 : data.channelType) !== null && _t !== void 0 ? _t : 'Chat',
                (_u = data === null || data === void 0 ? void 0 : data.location) !== null && _u !== void 0 ? _u : null,
                (_v = data === null || data === void 0 ? void 0 : data.deviceType) !== null && _v !== void 0 ? _v : null,
                customerData !== null && customerData !== void 0 ? customerData : null,
                (_w = data.domain_id) !== null && _w !== void 0 ? _w : null,
                (_x = data === null || data === void 0 ? void 0 : data.companyId) !== null && _x !== void 0 ? _x : null,
                (_y = data === null || data === void 0 ? void 0 : data.disposition) !== null && _y !== void 0 ? _y : null,
                (_z = data === null || data === void 0 ? void 0 : data.status) !== null && _z !== void 0 ? _z : null,
                (_0 = data === null || data === void 0 ? void 0 : data.priority) !== null && _0 !== void 0 ? _0 : null,
                (_1 = data === null || data === void 0 ? void 0 : data.summary) !== null && _1 !== void 0 ? _1 : null,
                (_2 = data === null || data === void 0 ? void 0 : data.follow_up_action) !== null && _2 !== void 0 ? _2 : null,
                (_3 = data === null || data === void 0 ? void 0 : data.recordingUrl) !== null && _3 !== void 0 ? _3 : null,
                (_4 = data === null || data === void 0 ? void 0 : data.callType) !== null && _4 !== void 0 ? _4 : null,
                (_5 = data === null || data === void 0 ? void 0 : data.queue) !== null && _5 !== void 0 ? _5 : null,
                (_6 = data === null || data === void 0 ? void 0 : data.wrapTime) !== null && _6 !== void 0 ? _6 : null,
                (_7 = data === null || data === void 0 ? void 0 : data.botDuration) !== null && _7 !== void 0 ? _7 : null,
                (_8 = data === null || data === void 0 ? void 0 : data.agentDuration) !== null && _8 !== void 0 ? _8 : null,
                (_9 = data === null || data === void 0 ? void 0 : data.agentRecordingUrl) !== null && _9 !== void 0 ? _9 : null,
                (_10 = data === null || data === void 0 ? void 0 : data.disconnectedBy) !== null && _10 !== void 0 ? _10 : null,
                (_11 = data === null || data === void 0 ? void 0 : data.qid) !== null && _11 !== void 0 ? _11 : null,
                (_12 = data === null || data === void 0 ? void 0 : data.assigned) !== null && _12 !== void 0 ? _12 : null,
                (_13 = data === null || data === void 0 ? void 0 : data.assignedTo) !== null && _13 !== void 0 ? _13 : null,
                (_14 = data === null || data === void 0 ? void 0 : data.assignedBy) !== null && _14 !== void 0 ? _14 : null,
                (_15 = data === null || data === void 0 ? void 0 : data.waitDuration) !== null && _15 !== void 0 ? _15 : null,
                (_16 = data === null || data === void 0 ? void 0 : data.agentHoldDuration) !== null && _16 !== void 0 ? _16 : null,
                (_17 = data === null || data === void 0 ? void 0 : data.dialDuration) !== null && _17 !== void 0 ? _17 : null,
                (_18 = data === null || data === void 0 ? void 0 : data.calledNumber) !== null && _18 !== void 0 ? _18 : null,
                (_19 = data === null || data === void 0 ? void 0 : data.queueDuration) !== null && _19 !== void 0 ? _19 : null,
                (_20 = data === null || data === void 0 ? void 0 : data.queueName) !== null && _20 !== void 0 ? _20 : null,
                (_21 = data === null || data === void 0 ? void 0 : data.callDuration) !== null && _21 !== void 0 ? _21 : null,
                message !== null && message !== void 0 ? message : null,
                (_22 = data === null || data === void 0 ? void 0 : data.customerFile) !== null && _22 !== void 0 ? _22 : null,
                (_23 = data === null || data === void 0 ? void 0 : data.agentName) !== null && _23 !== void 0 ? _23 : null,
                (_24 = data === null || data === void 0 ? void 0 : data.skillName) !== null && _24 !== void 0 ? _24 : null,
                (_25 = data === null || data === void 0 ? void 0 : data.dispositionName) !== null && _25 !== void 0 ? _25 : null,
                (_26 = data === null || data === void 0 ? void 0 : data.afterCallWorkTime) !== null && _26 !== void 0 ? _26 : null,
                (data === null || data === void 0 ? void 0 : data.callRecorded) === 1 ? 1 : 0,
                (_27 = data === null || data === void 0 ? void 0 : data.callDuration) !== null && _27 !== void 0 ? _27 : null,
                (_28 = data === null || data === void 0 ? void 0 : data.transferred) !== null && _28 !== void 0 ? _28 : null,
                (_29 = data === null || data === void 0 ? void 0 : data.activeChatTime) !== null && _29 !== void 0 ? _29 : null,
                (_30 = data === null || data === void 0 ? void 0 : data.responseTime) !== null && _30 !== void 0 ? _30 : null,
                (_31 = data === null || data === void 0 ? void 0 : data.assignedOn) !== null && _31 !== void 0 ? _31 : null,
                (_32 = data === null || data === void 0 ? void 0 : data.chatSession) !== null && _32 !== void 0 ? _32 : null,
                (_33 = data === null || data === void 0 ? void 0 : data.transferredTo) !== null && _33 !== void 0 ? _33 : null,
                (_34 = data === null || data === void 0 ? void 0 : data.markAsRead) !== null && _34 !== void 0 ? _34 : null,
                (_35 = data === null || data === void 0 ? void 0 : data.agentStatus) !== null && _35 !== void 0 ? _35 : null,
                (_36 = data === null || data === void 0 ? void 0 : data.calledPersonDetails) !== null && _36 !== void 0 ? _36 : null,
                (_37 = data === null || data === void 0 ? void 0 : data.subject) !== null && _37 !== void 0 ? _37 : null,
                (_38 = data === null || data === void 0 ? void 0 : data.primarySkill) !== null && _38 !== void 0 ? _38 : null,
                (_39 = data === null || data === void 0 ? void 0 : data.primaryAgent) !== null && _39 !== void 0 ? _39 : null,
                (_40 = data === null || data === void 0 ? void 0 : data.transferredSkill) !== null && _40 !== void 0 ? _40 : null,
                (_41 = data === null || data === void 0 ? void 0 : data.transferredAgent) !== null && _41 !== void 0 ? _41 : null,
                (_42 = data === null || data === void 0 ? void 0 : data.primaryQueue) !== null && _42 !== void 0 ? _42 : null,
                (_43 = data === null || data === void 0 ? void 0 : data.transferredQueue) !== null && _43 !== void 0 ? _43 : null,
                (_44 = data === null || data === void 0 ? void 0 : data.transferredTime) !== null && _44 !== void 0 ? _44 : null,
                (_45 = data === null || data === void 0 ? void 0 : data.transferredType) !== null && _45 !== void 0 ? _45 : null,
                (_46 = data === null || data === void 0 ? void 0 : data.voicemailUrl) !== null && _46 !== void 0 ? _46 : null,
                (_47 = data === null || data === void 0 ? void 0 : data.vmsTranscript) !== null && _47 !== void 0 ? _47 : null,
                customerCallBackData !== null && customerCallBackData !== void 0 ? customerCallBackData : null,
                (_48 = data === null || data === void 0 ? void 0 : data.supervisorName) !== null && _48 !== void 0 ? _48 : null,
                (_49 = data === null || data === void 0 ? void 0 : data.ticketId) !== null && _49 !== void 0 ? _49 : null,
            ], (err, result) => {
                if (err) {
                    reject(err);
                    console.log(err);
                }
                resolve(result);
            });
        }
        catch (err) {
            reject(err);
        }
    }));
};
exports.createSession = createSession;
// export const createSession = (data: any, id: any) => {
//   console.log('1 createSession data message: ', data);
//  // console.log('createSession message: ', JSON.stringify(data?.message));
//   return new Promise(async (resolve, reject) => {
//     try {
//       const message = JSON.stringify(data?.message);
//       mysqlPoolConnection.query(
//         'call ccaas_create_call_session_history_details(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
//         [
//           data?.session_id,
//           data?.sessionStartTime,
//           data?.sessionEndTime ?? null,
//           data?.cli ?? null,
//           data?.callFlowSourceId ?? null,
//           data?.companyId ?? null,
//           data.domain_id ?? null,
//           data?.dispositionId ?? null,
//           data?.agentStatus ?? null,
//           data?.callType === 'inbound' ? 1 : 0 ?? null,
//           data?.dialDuration ?? null,
//           data?.disconnectedBy === '' ? null : null ?? null,
//           data?.queueDuration ?? null,
//           data?.assignedTo ?? null,
//           data?.assignedBy ?? null,
//           data?.assignedOn ?? null,
//           data?.calledNumber ? Number(data?.calledNumber) : null,
//           data?.waitDuration ?? null,
//           data?.afterCallWorkTime ?? null,
//           data?.recordingTypeId ?? null,
//           data?.istransferred ?? null,
//           data?.transferredTypeId ?? null,
//           data?.markAsRead ?? null,
//           data?.customerId ?? null,
//           data?.voicemailUrl ?? null,
//           data?.ticketId ?? null,
//           data?.agentConnectTime ?? null,
//           data?.agentDisconnectTime ?? null,
//           data?.agentDuration ?? null,
//           data?.agentHoldDuration ?? null,
//           data?.agentId ?? null,
//           data?.agentSkillId ?? null,
//           data?.isBot ?? null,
//           data?.botId ?? null,
//           data?.recordingUrl ?? null,
//           data?.transferredTime ?? null,
//           data?.recordingDuration ?? null,
//           data?.qid ? Number(data?.qid) : 0,
//           message ?? null,
//           data?.callHistoryId ?? null,
//           data?.callDuration ?? null,
//         ],
//         (err: any, result: any) => {
//           if (err) {
//             reject(err);
//             console.log(err);
//           }
//           resolve(result);
//         }
//       );
//     } catch (err) {
//       reject(err);
//     }
//   });
// };
/**
 *
 * @param data
 * @returns get Message WebChat
 */
const getMessageWebChat = (id) => {
    return new Promise((resolve, reject) => {
        try {
            db_1.mysqlPoolConnection.query('call up_GetChatMessageById(?)', [
                id !== null && id !== void 0 ? id : 0,
            ], (err, result) => {
                if (err) {
                    reject(err);
                }
                resolve(result);
            });
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.getMessageWebChat = getMessageWebChat;
/**
 * @param data
 * @returns
 */
const storeMessageHistory = (data) => {
    return new Promise((resolve, reject) => {
        try {
            // const message = JSON.stringify(data.message);
            db_1.mysqlPoolConnection.query('call ccass_create_conv_history_details(?,?,?,?,?,?,?,?,?,?,?,?,?)', [
                data.sessionId,
                data.from,
                data.to,
                data.messageType,
                data.message,
                data.id,
                data.timeStamp,
                data.agentState,
                data.customerState,
                data.botDetails,
                data.domainId,
                data.companyId,
                data.isBot,
            ], (err, result) => {
                if (err) {
                    reject(err);
                }
                resolve(result);
            });
            const mysqlqs = `UPDATE sessionHistory SET lastMessage= '${data === null || data === void 0 ? void 0 : data.message}' WHERE sessionId= '${data === null || data === void 0 ? void 0 : data.sessionId}'`;
            db_1.mysqlPoolConnection.query(mysqlqs, (err, results) => {
                if (err) {
                    reject(err);
                }
                else {
                    resolve(results);
                }
            });
        }
        catch (error) {
            console.log(error);
        }
    });
};
exports.storeMessageHistory = storeMessageHistory;
const getAllUserList = (data) => {
    return new Promise((resolve, reject) => {
        var _a;
        try {
            db_1.mysqlPoolConnection.query('call ccaas_getAllUserList_info(?,?)', [data === null || data === void 0 ? void 0 : data.domainId, (_a = data === null || data === void 0 ? void 0 : data.ext) !== null && _a !== void 0 ? _a : 0], (err, result) => {
                if (err) {
                    reject(err);
                    console.log('getAllUserList reject err: ', err);
                }
                else {
                    resolve(result.length > 0 ? result[0] : []);
                }
            });
        }
        catch (error) {
            console.log('getAllUserList catch error: ', error);
            reject(error);
        }
    });
};
exports.getAllUserList = getAllUserList;
/* export const storeMessageHistory = async (data: any) => {
  try {
    return new Promise(async (resolve, reject) => {
      conversationHistoryModel
        .create(data)
        .then((result) => {
          resolve(result);
          sessionHistoryModel
            .updateOne(
              { sessionId: data.sessionId },
              { $set: { lastMessage: data.message } }
            )
            .then((rsp) => {
              return true;
            });
        })
        .catch((err) => {
          reject(err);
        });
    });
  } catch (err: any) {
    console.log(err);
  }
}; */
/**
 *
 * @param id
 * @param number
 * @returns
 */
const storeCustomerDetails = (id, number) => __awaiter(void 0, void 0, void 0, function* () {
    return new Promise((resolve, reject) => {
        try {
            const mysqlq = `SELECT * FROM customer_contact WHERE phoneNumber = '${number}'`;
            db_1.mysqlPoolConnection.query(mysqlq, (err, result) => {
                var _a;
                if (err) {
                    reject(err);
                }
                else {
                    if (result.length !== 0) {
                        const response = JSON.parse((_a = JSON.stringify(result)) !== null && _a !== void 0 ? _a : 'null');
                        const updateQuery = `UPDATE sessionHistory SET customerData='${JSON.stringify(response[0])}' WHERE sessionId='${id}'`;
                        db_1.mysqlPoolConnection.query(updateQuery, (error, res) => {
                            if (error) {
                                reject(error);
                            }
                            else {
                                resolve(res);
                            }
                        });
                    }
                    else {
                        const updateQuery = `UPDATE sessionHistory SET customerData='{}' WHERE sessionId='${id}'`;
                        db_1.mysqlPoolConnection.query(updateQuery, (error, res) => {
                            if (error) {
                                reject(error);
                            }
                            else {
                                resolve(res);
                            }
                        });
                    }
                }
            });
        }
        catch (error) {
            reject(error);
        }
    });
});
exports.storeCustomerDetails = storeCustomerDetails;
/* export const storeCustomerDetails = async (id: any, number: any) => {
  try {
    return new Promise(async (resolve, reject) => {
      await axios
        .post(
          'https://liveurapi.worktual.co.uk/vectone-myaccount/v1/user/Websitecentralgetcustomerinfo',
          { mundio_product: 'vmuk', searchby: 'mobileno', searchvalue: number },
          { headers: { Authorization: 'eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1dWlkIjoiN2UwNTJmYTAtMzc4Ni00MDZjLWJiNjctZDYxNzFlOWJkMTlhIiwidXNlcm5hbWUiOiJwZXJzaGliYS52QHZlY3RvbmUuY29tIiwiZGV2aWNlSWQiOiJhMTJhZWVjMi1iNGJiLTRjZjEtYmI1OC1kZmQ1YzQzOWZlZTkiLCJzaXBMb2dpbklkIjoiNTQ0OCIsInJvbGVJZCI6NCwiZG9tYWluSWQiOjMzMjgsImV4dCI6NTg0LCJjb21wYW55SWQiOjMzNjIsImVuZXRlcHJpc2VpZCI6ODAzMiwiaG9zdEFkZHJlc3MiOiI4MDMyLnVyY2hhdC51bmlmaWVkcmluZy5jby51ayIsIm9yZGVySWQiOjExMDE4LCJkaXJVc2VySWQiOjMyNzA2LCJwcm9maWxlTmFtZSI6IlBlcnNoaWJhIFZlbHVzYW15IiwicHJvZmlsZUltZyI6Imh0dHBzOi8vdXJzdG9yYWdlLnVuaWZpZWRyaW5nLmNvLnVrL2ZpbGVzL3VzZXIvMzM2Mi81NDQ4XzU4NC8xNjUzNTQ1MTQ1MzQwLzEwODBwLWdpcmwtQmVhdXRpZnVsLVdoYXRzYXBwLURwLVByb2ZpbGUtSW1hZ2VzLXBob3RvLWhkLmpwZWciLCJzb3VyY2UiOiJXZWIiLCJpcGFkZHJlc3MiOiIxMzYuMTQ0LjU2LjI1NTo0NDMiLCJpYXQiOjE2NTU0NDI4OTZ9.yhEcej3x7miIQebCQRrAzAznckyLWhHkYslIPKa-BGk' } }
        )
        .then((result: any) => {
          if (result.data.result) {
            sessionHistoryModel
              .updateOne(
                { sessionId: id },
                { $set: { customerData: result.data.result[0] } }
              )
              .then((res: any) => {
                resolve(res);
              })
              .catch((err: any) => {
                reject(err);
              });
          } else {
            sessionHistoryModel
              .updateOne({ sessionId: id }, { $set: { customerData: {} } })
              .then((res: any) => {
                resolve(res);
              })
              .catch((err: any) => {
                reject(err);
              });
          }
        })
        .catch((err: any) => {
          reject(err);
        });
    });
  } catch (err: any) {
    console.log(err);
  }
};

/* export const storeCallHistory = async (data: any) => {
  // ;
  try {
    return new Promise(async (resolve, reject) => {
      conversationHistoryModel
        .create(data)
        .then((result) => {
          resolve(result);
        })
        .catch((err) => {
          reject(err);
        });
    });
  } catch (err: any) {
    console.log(err);
  }
}; */
/**
 *
 * @param data
 * @returns get Chat History
 */
const getChatHistory = (data) => {
    return new Promise((resolve, reject) => {
        try {
            const mysqlq = `SELECT * FROM conversationHistory where sessionId = '${data}'`;
            db_1.mysqlPoolConnection.query(mysqlq, (err, result) => {
                if (err) {
                    reject(err);
                }
                else {
                    resolve(result);
                }
            });
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.getChatHistory = getChatHistory;
/* export const getChatHistory = async (id: any) => {
  try {
    return new Promise(async (resolve, reject) => {
      conversationHistoryModel
        .find({ sessionId: id })
        .then((result) => {
          resolve(result);
        })
        .catch((err) => {
          reject(err);
        });
    });
  } catch (err: any) {
    console.log(err);
  }
}; */
// export const chatUpdateDao = async (data: any) => {
//   try {
//     // ;
//     return new Promise(async (resolve, reject) => {
//       sessionHistoryModel
//         .updateOne({ sessionId: data.sessionId }, { $set: data })
//         .then((res: any) => {
//           resolve(res);
//         })
//         .catch((err: any) => {
//           reject(err);
//         });
//     });
//   } catch (err: any) {
//     console.log(err);
//   }
// };
/**
 *  call history initial data
 */
const chatUpdateDao = (data) => {
    console.log('chatUpdateDao>>>>>>>>', data);
    console.log('<<<<<<<<<<<<<<<<<<<<<<chatUpdateDao>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>', JSON.stringify(data === null || data === void 0 ? void 0 : data.message));
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        var _a, _b, _c, _d, _e, _f, _g, _h, _j, _k, _l, _m, _o, _p, _q, _r, _s, _t, _u, _v, _w, _x, _y, _z, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48;
        const botDetails = JSON.stringify(data === null || data === void 0 ? void 0 : data.botDetails);
        const customerData = JSON.stringify(data === null || data === void 0 ? void 0 : data.customerData);
        const message = JSON.stringify(data === null || data === void 0 ? void 0 : data.message);
        const domainId = parseInt(data === null || data === void 0 ? void 0 : data.domain_id, 10);
        try {
            db_1.mysqlPoolConnection.query('call ccaas_session_history_details(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,??,?,?,?,?,?,?,?,?,?,?,?,?,?,?)', [
                data === null || data === void 0 ? void 0 : data.session_id,
                (_a = data === null || data === void 0 ? void 0 : data.sessionStartTime) !== null && _a !== void 0 ? _a : null,
                (_b = data === null || data === void 0 ? void 0 : data.sessionEndTime) !== null && _b !== void 0 ? _b : null,
                (_c = data === null || data === void 0 ? void 0 : data.botStartTime) !== null && _c !== void 0 ? _c : null,
                (_d = data === null || data === void 0 ? void 0 : data.botEndTime) !== null && _d !== void 0 ? _d : null,
                (_e = data === null || data === void 0 ? void 0 : data.agentConnectTime) !== null && _e !== void 0 ? _e : null,
                (_f = data === null || data === void 0 ? void 0 : data.agentDisconnectTime) !== null && _f !== void 0 ? _f : null,
                (_g = data === null || data === void 0 ? void 0 : data.ipAddress) !== null && _g !== void 0 ? _g : null,
                (_h = data === null || data === void 0 ? void 0 : data.source) !== null && _h !== void 0 ? _h : null,
                (_j = data === null || data === void 0 ? void 0 : data.lastMessage) !== null && _j !== void 0 ? _j : null,
                botDetails !== null && botDetails !== void 0 ? botDetails : null,
                (_k = data === null || data === void 0 ? void 0 : data.category) !== null && _k !== void 0 ? _k : null,
                true,
                (_l = data === null || data === void 0 ? void 0 : data.agentComments) !== null && _l !== void 0 ? _l : null,
                (_m = data === null || data === void 0 ? void 0 : data.agentDetails) !== null && _m !== void 0 ? _m : null,
                (_o = data === null || data === void 0 ? void 0 : data.cli) !== null && _o !== void 0 ? _o : null,
                (_p = data === null || data === void 0 ? void 0 : data.browser) !== null && _p !== void 0 ? _p : null,
                (_q = data === null || data === void 0 ? void 0 : data.OS) !== null && _q !== void 0 ? _q : null,
                true,
                (_r = data === null || data === void 0 ? void 0 : data.skillsIdentify) !== null && _r !== void 0 ? _r : null,
                (_s = data === null || data === void 0 ? void 0 : data.callFlowSourceId) !== null && _s !== void 0 ? _s : null,
                'Call',
                (_t = data === null || data === void 0 ? void 0 : data.location) !== null && _t !== void 0 ? _t : null,
                (_u = data === null || data === void 0 ? void 0 : data.deviceType) !== null && _u !== void 0 ? _u : null,
                customerData !== null && customerData !== void 0 ? customerData : null,
                domainId !== null && domainId !== void 0 ? domainId : null,
                (_v = data === null || data === void 0 ? void 0 : data.companyId) !== null && _v !== void 0 ? _v : null,
                (_w = data === null || data === void 0 ? void 0 : data.disposition) !== null && _w !== void 0 ? _w : null,
                (_x = data === null || data === void 0 ? void 0 : data.status) !== null && _x !== void 0 ? _x : null,
                (_y = data === null || data === void 0 ? void 0 : data.priority) !== null && _y !== void 0 ? _y : null,
                (_z = data === null || data === void 0 ? void 0 : data.summary) !== null && _z !== void 0 ? _z : null,
                (_0 = data === null || data === void 0 ? void 0 : data.follow_up_action) !== null && _0 !== void 0 ? _0 : null,
                (_1 = data === null || data === void 0 ? void 0 : data.recordingUrl) !== null && _1 !== void 0 ? _1 : null,
                (_2 = data === null || data === void 0 ? void 0 : data.callType) !== null && _2 !== void 0 ? _2 : null,
                (_3 = data === null || data === void 0 ? void 0 : data.queue) !== null && _3 !== void 0 ? _3 : null,
                (_4 = data === null || data === void 0 ? void 0 : data.wrapTime) !== null && _4 !== void 0 ? _4 : null,
                (_5 = data === null || data === void 0 ? void 0 : data.botDuration) !== null && _5 !== void 0 ? _5 : null,
                (_6 = data === null || data === void 0 ? void 0 : data.agentDuration) !== null && _6 !== void 0 ? _6 : null,
                (_7 = data === null || data === void 0 ? void 0 : data.agentRecordingUrl) !== null && _7 !== void 0 ? _7 : null,
                (_8 = data === null || data === void 0 ? void 0 : data.disconnectedBy) !== null && _8 !== void 0 ? _8 : null,
                (_9 = data === null || data === void 0 ? void 0 : data.qid) !== null && _9 !== void 0 ? _9 : null,
                (_10 = data === null || data === void 0 ? void 0 : data.assigned) !== null && _10 !== void 0 ? _10 : null,
                (_11 = data === null || data === void 0 ? void 0 : data.assignedTo) !== null && _11 !== void 0 ? _11 : null,
                (_12 = data === null || data === void 0 ? void 0 : data.assignedBy) !== null && _12 !== void 0 ? _12 : null,
                (_13 = data === null || data === void 0 ? void 0 : data.waitDuration) !== null && _13 !== void 0 ? _13 : null,
                (_14 = data === null || data === void 0 ? void 0 : data.agentHoldDuration) !== null && _14 !== void 0 ? _14 : null,
                (_15 = data === null || data === void 0 ? void 0 : data.dialDuration) !== null && _15 !== void 0 ? _15 : null,
                (_16 = data === null || data === void 0 ? void 0 : data.calledNumber) !== null && _16 !== void 0 ? _16 : null,
                (_17 = data === null || data === void 0 ? void 0 : data.queueDuration) !== null && _17 !== void 0 ? _17 : null,
                (_18 = data === null || data === void 0 ? void 0 : data.queueName) !== null && _18 !== void 0 ? _18 : null,
                (_19 = data === null || data === void 0 ? void 0 : data.callDuration) !== null && _19 !== void 0 ? _19 : null,
                message !== null && message !== void 0 ? message : null,
                (_20 = data === null || data === void 0 ? void 0 : data.customerFile) !== null && _20 !== void 0 ? _20 : null,
                (_21 = data === null || data === void 0 ? void 0 : data.agentName) !== null && _21 !== void 0 ? _21 : null,
                (_22 = data === null || data === void 0 ? void 0 : data.skillName) !== null && _22 !== void 0 ? _22 : null,
                (_23 = data === null || data === void 0 ? void 0 : data.dispositionName) !== null && _23 !== void 0 ? _23 : null,
                (_24 = data === null || data === void 0 ? void 0 : data.afterCallWorkTime) !== null && _24 !== void 0 ? _24 : null,
                (data === null || data === void 0 ? void 0 : data.callRecorded) === 1 ? 1 : 0,
                (_25 = data === null || data === void 0 ? void 0 : data.callDuration) !== null && _25 !== void 0 ? _25 : null,
                (_26 = data === null || data === void 0 ? void 0 : data.transferred) !== null && _26 !== void 0 ? _26 : null,
                (_27 = data === null || data === void 0 ? void 0 : data.activeChatTime) !== null && _27 !== void 0 ? _27 : null,
                (_28 = data === null || data === void 0 ? void 0 : data.responseTime) !== null && _28 !== void 0 ? _28 : null,
                (_29 = data === null || data === void 0 ? void 0 : data.assignedOn) !== null && _29 !== void 0 ? _29 : null,
                (_30 = data === null || data === void 0 ? void 0 : data.chatSession) !== null && _30 !== void 0 ? _30 : null,
                (_31 = data === null || data === void 0 ? void 0 : data.transferredTo) !== null && _31 !== void 0 ? _31 : null,
                (_32 = data === null || data === void 0 ? void 0 : data.markAsRead) !== null && _32 !== void 0 ? _32 : null,
                (_33 = data === null || data === void 0 ? void 0 : data.agentStatus) !== null && _33 !== void 0 ? _33 : null,
                (_34 = data === null || data === void 0 ? void 0 : data.calledPersonDetails) !== null && _34 !== void 0 ? _34 : null,
                (_35 = data === null || data === void 0 ? void 0 : data.subject) !== null && _35 !== void 0 ? _35 : null,
                (_36 = data === null || data === void 0 ? void 0 : data.primarySkill) !== null && _36 !== void 0 ? _36 : null,
                (_37 = data === null || data === void 0 ? void 0 : data.primaryAgent) !== null && _37 !== void 0 ? _37 : null,
                (_38 = data === null || data === void 0 ? void 0 : data.transferredSkill) !== null && _38 !== void 0 ? _38 : null,
                (_39 = data === null || data === void 0 ? void 0 : data.transferredAgent) !== null && _39 !== void 0 ? _39 : null,
                (_40 = data === null || data === void 0 ? void 0 : data.primaryQueue) !== null && _40 !== void 0 ? _40 : null,
                (_41 = data === null || data === void 0 ? void 0 : data.transferredQueue) !== null && _41 !== void 0 ? _41 : null,
                (_42 = data === null || data === void 0 ? void 0 : data.transferredTime) !== null && _42 !== void 0 ? _42 : null,
                (_43 = data === null || data === void 0 ? void 0 : data.transferredType) !== null && _43 !== void 0 ? _43 : null,
                (_44 = data === null || data === void 0 ? void 0 : data.voicemailUrl) !== null && _44 !== void 0 ? _44 : null,
                (_45 = data === null || data === void 0 ? void 0 : data.vmsTranscript) !== null && _45 !== void 0 ? _45 : null,
                (_46 = data === null || data === void 0 ? void 0 : data.callBackDetails) !== null && _46 !== void 0 ? _46 : null,
                (_47 = data === null || data === void 0 ? void 0 : data.supervisorName) !== null && _47 !== void 0 ? _47 : null,
                (_48 = data === null || data === void 0 ? void 0 : data.ticketId) !== null && _48 !== void 0 ? _48 : null,
            ], (err, result) => {
                if (err) {
                    reject(err);
                    console.log(err);
                }
                resolve(result);
            });
        }
        catch (err) {
            reject(err);
        }
    }));
};
exports.chatUpdateDao = chatUpdateDao;
/* export const getChatSessionDao = async (ext: number) => {
  try {
    return new Promise(async (resolve, reject) => {
      sessionHistoryModel
        .find({ agentDetails: ext, channelType: 'Chat' })
        .then((res: any) => {
          resolve(res);
        })
        .catch((err: any) => {
          reject(err);
        });
    });
  } catch (err: any) {
    console.log(err);
  }
}; */
/**
 * @description get Chat Session
 * @param data
 * @returns
 */
const getChatSessionDao = (ext) => {
    return new Promise((resolve, reject) => {
        try {
            const mysqlq = `SELECT * FROM sessionHistory where agentDetails = ${ext} AND channelType: 'Chat'`;
            db_1.mysqlPoolConnection.query(mysqlq, (err, result) => {
                if (err) {
                    reject(err);
                }
                else {
                    resolve(result);
                }
            });
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.getChatSessionDao = getChatSessionDao;
/**
 *
 * @param ext
 * @returns
 */
/* export const getCallSessionDao = async (ext: number) => {
  try {
    return new Promise(async (resolve, reject) => {
      sessionHistoryModel
        .find({ agentDetails: ext, channelType: 'Call' })
        .then((res: any) => {
          resolve(res);
        })
        .catch((err: any) => {
          reject(err);
        });
    });
  } catch (err: any) {
    console.log(err);
  }
}; */
/**
 * @description get Call Session
 * @param data
 * @returns
 */
const getCallSessionDao = (data) => {
    return new Promise((resolve, reject) => {
        try {
            const mysqlq = `SELECT * FROM sessionHistory where domainId = ${data.domainId} AND isBot = 0 AND agentDetails is not null AND dispositionName is null`;
            db_1.mysqlPoolConnection.query(mysqlq, (err, result) => {
                if (err) {
                    reject(err);
                }
                else {
                    resolve(result);
                }
            });
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.getCallSessionDao = getCallSessionDao;
/* export const getChatSessionIdDao = async (id: string) => {
  try {
    return new Promise(async (resolve, reject) => {
      sessionHistoryModel
        .findOne({ sessionId: id })
        .then((res: any) => {
          resolve(res);
        })
        .catch((err: any) => {
          reject(err);
        });
    });
  } catch (err: any) {
    console.log(err);
  }
}; */
/**
 * @description get the chat session
 * @param data
 * @returns
 */
const getChatSessionIdDao = (data) => {
    return new Promise((resolve, reject) => {
        try {
            console.log(data, 'datadatadatadatadatadatadatadatadata');
            const mysqlq = `SELECT * FROM tb_ChatHistory where ChatHistoryId=${data}`;
            db_1.mysqlPoolConnection.query(mysqlq, (err, result) => {
                if (err) {
                    reject(err);
                }
                else {
                    console.log('result-------------', result);
                    resolve(result);
                }
            });
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.getChatSessionIdDao = getChatSessionIdDao;
/**
 * @description get the Call session
 * @param data
 * @returns
 */
const getCallSessionIdDao = (data) => {
    return new Promise((resolve, reject) => {
        try {
            const mysqlq = `SELECT * FROM tb_CallHistory where sessionId = '${data}'`;
            db_1.mysqlPoolConnection.query(mysqlq, (err, result) => {
                if (err) {
                    reject(err);
                }
                else {
                    resolve(result);
                }
            });
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.getCallSessionIdDao = getCallSessionIdDao;
/**
 * @description get chat number from
 * @param data
 * @returns
 */
// check with db team customerNumber
const getChatByNumberDao = (data) => {
    return new Promise((resolve, reject) => {
        try {
            if (data) {
                // const mysqlq = `SELECT * FROM sessionHistory where customerDetails = '${data}'`;
                const mysqlq = `SELECT * FROM tb_CallHistory where customerNumber = '${data}'`;
                db_1.mysqlPoolConnection.query(mysqlq, (err, result) => {
                    if (err) {
                        reject(err);
                    }
                    else {
                        resolve(result);
                    }
                });
            }
            else {
                resolve([]);
            }
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.getChatByNumberDao = getChatByNumberDao;
/**
 * @description get chat by endTime
 * @param data
 * @returns
 */
const getChatByEndTimeDao = (data) => {
    return new Promise((resolve, reject) => {
        try {
            const mysqlq = `SELECT * FROM sessionHistory where agentDetails = ${data} AND wrapTime = 0`;
            db_1.mysqlPoolConnection.query(mysqlq, (err, result) => {
                if (err) {
                    reject(err);
                }
                else {
                    resolve(result);
                }
            });
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.getChatByEndTimeDao = getChatByEndTimeDao;
/* export const getChatByNumberDao = async (number: string) => {
  try {
    return new Promise(async (resolve, reject) => {
      sessionHistoryModel
        .find({ customerDetails: number })
        .then((res: any) => {
          resolve(res);
        })
        .catch((err: any) => {
          reject(err);
        });
    });
  } catch (err: any) {
    console.log(err);
  }
}; */
/* export const getChatByEndTimeDao = async (ext: number) => {
  try {
    return new Promise(async (resolve, reject) => {
      sessionHistoryModel
        .find({ agentDetails: ext, wrapTime: null })
        .then((res: any) => {
          resolve(res);
        })
        .catch((err: any) => {
          reject(err);
        });
    });
  } catch (err: any) {
    console.log(err);
  }
}; */
/**
 *
 * @param data
 * @returns get Message WebChat
 */
const createCoChatChannel = (channelDetails) => {
    return new Promise((resolve, reject) => {
        try {
            const mysqlq = `INSERT INTO coChat (subscribedUsers, customerInfo, channelId, uid, channelName) VALUES (${channelDetails === null || channelDetails === void 0 ? void 0 : channelDetails.agentListData}, ${channelDetails === null || channelDetails === void 0 ? void 0 : channelDetails.customerDetails}, ${channelDetails === null || channelDetails === void 0 ? void 0 : channelDetails.channelId}, ${channelDetails === null || channelDetails === void 0 ? void 0 : channelDetails.coid}, ${channelDetails === null || channelDetails === void 0 ? void 0 : channelDetails.channelName}) `;
            db_1.mysqlPoolConnection.query(mysqlq, (err, result) => {
                if (err) {
                    reject(err);
                }
                else {
                    resolve(result);
                }
            });
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.createCoChatChannel = createCoChatChannel;
/**
 *
 * @param data
 * @param dataq
 * @returns
 */
const coChatUpdateDao = (data, dataq) => __awaiter(void 0, void 0, void 0, function* () {
    const query = `UPDATE coChat SET subscribedUsers = CONCAT(subscribedUsers, ?) WHERE uid = '${dataq.uid}' AND channelId = '${dataq.channelId}' AND  domainId = ${dataq.domainId}`;
    const values = [JSON.stringify(data), dataq.id];
    try {
        return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
            db_1.mysqlPoolConnection.query(query, values, (err, res) => {
                if (err) {
                    reject(err);
                }
                else {
                    resolve(res);
                }
            });
        }));
    }
    catch (err) {
        console.log(err);
    }
});
exports.coChatUpdateDao = coChatUpdateDao;
/**
 * @description
 * @param data
 * @returns
 */
const getAllMessageId = (channelId, domainId) => {
    return new Promise((resolve, reject) => {
        try {
            const mysqlq = `SELECT * FROM coChat WHERE channelId = '${channelId}' AND domainId = ${domainId}`;
            db_1.mysqlPoolConnection.query(mysqlq, (err, result) => {
                if (err) {
                    reject(err);
                }
                else {
                    resolve(result);
                }
            });
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.getAllMessageId = getAllMessageId;
/**
 *
 * @param data
 * @param dataq
 * @returns
 */
const sendMessageToCoChat = (data, dataq) => __awaiter(void 0, void 0, void 0, function* () {
    const query = `UPDATE coChat SET messages = CONCAT(messages, ?) WHERE uid = '${dataq.uid}' AND channelId = '${dataq.channelId}' AND  domainId = ${dataq.domainId}`;
    const values = [JSON.stringify(data), dataq.id];
    try {
        return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
            db_1.mysqlPoolConnection.query(query, values, (err, res) => {
                if (err) {
                    reject(err);
                }
                else {
                    resolve(res);
                }
            });
        }));
    }
    catch (err) {
        console.log(err);
    }
});
exports.sendMessageToCoChat = sendMessageToCoChat;
const availableAgentData = (data) => __awaiter(void 0, void 0, void 0, function* () {
    const query = `SELECT * from user where domainId=${data.domain_id} AND statusName='Ready'`;
    try {
        return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
            db_1.mysqlPoolConnection.query(query, (err, res) => {
                if (err) {
                    reject(err);
                }
                else {
                    resolve(res);
                }
            });
        }));
    }
    catch (err) {
        console.log(err);
    }
});
exports.availableAgentData = availableAgentData;
const storeCochatDetailsByIDDao = (data) => {
    return new Promise((resolve, reject) => {
        var _a, _b;
        try {
            const mysqlMessage = `SELECT messages FROM coChat Where domainId = ${(_a = data === null || data === void 0 ? void 0 : data.domainId) !== null && _a !== void 0 ? _a : null} AND channelId = '${(_b = data === null || data === void 0 ? void 0 : data.channelId) !== null && _b !== void 0 ? _b : null}'`;
            db_1.mysqlPoolConnection.query(mysqlMessage, (err, result) => {
                var _a, _b, _c, _d, _e;
                if (err) {
                    reject(err);
                }
                else {
                    let resultMessage = [];
                    if ((result === null || result === void 0 ? void 0 : result.length) > 0 && ((_b = (_a = result[0]) === null || _a === void 0 ? void 0 : _a.messages) === null || _b === void 0 ? void 0 : _b.length) > 0) {
                        resultMessage = JSON.parse((_d = (_c = result[0]) === null || _c === void 0 ? void 0 : _c.messages) !== null && _d !== void 0 ? _d : 'null');
                        resultMessage.push(data);
                    }
                    else {
                        resultMessage.push(data);
                    }
                    const parseResultMessage = JSON.stringify(resultMessage);
                    const mysqlq = `UPDATE coChat SET messages='${parseResultMessage}' WHERE domainId = ${(_e = data === null || data === void 0 ? void 0 : data.domainId) !== null && _e !== void 0 ? _e : null} AND channelId = '${data.channelId}'`;
                    db_1.mysqlPoolConnection.query(mysqlq, (errs, result1) => {
                        if (errs) {
                            reject(errs);
                        }
                        else {
                            resolve(result1);
                        }
                    });
                }
            });
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.storeCochatDetailsByIDDao = storeCochatDetailsByIDDao;
const saveChatFlowDao = (data, nodeData) => {
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        try {
            db_1.mysqlPoolConnection.query('call up_InsertUpdatechatFlowHistory(?,?,?,?,?,?,?,?,?,?,?,?,?)', [
                data === null || data === void 0 ? void 0 : data.cfid,
                data === null || data === void 0 ? void 0 : data.templateName,
                nodeData === null || nodeData === void 0 ? void 0 : nodeData.node,
                nodeData === null || nodeData === void 0 ? void 0 : nodeData.edge,
                data === null || data === void 0 ? void 0 : data.version,
                data === null || data === void 0 ? void 0 : data.type,
                data === null || data === void 0 ? void 0 : data.status,
                data === null || data === void 0 ? void 0 : data.companyId,
                data === null || data === void 0 ? void 0 : data.domainId,
                data === null || data === void 0 ? void 0 : data.description,
                data === null || data === void 0 ? void 0 : data.publishedDataArr,
                data === null || data === void 0 ? void 0 : data.action,
                data === null || data === void 0 ? void 0 : data.category
            ], (err, result) => {
                if (err) {
                    reject(err);
                    console.log(err);
                }
                resolve(result);
            });
        }
        catch (err) {
            reject(err);
        }
    }));
};
exports.saveChatFlowDao = saveChatFlowDao;
const getChatFlowIdDao = (data) => {
    return new Promise((resolve, reject) => {
        try {
            const mysqlq = `SELECT * FROM chatFlowHistory where chatFlowId='${data.chatFlowId}' AND domainId=${data.domainId}`;
            db_1.mysqlPoolConnection.query(mysqlq, (err, result) => {
                if (err) {
                    reject(err);
                }
                else {
                    resolve(result);
                }
            });
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.getChatFlowIdDao = getChatFlowIdDao;
const getAllChatFlowIdDao = (data) => {
    return new Promise((resolve, reject) => {
        try {
            const mysqlq = `SELECT * FROM chatFlowHistory where domainId=${data.domainId} ORDER BY chatFlowId DESC`;
            db_1.mysqlPoolConnection.query(mysqlq, (err, result) => {
                if (err) {
                    reject(err);
                }
                else {
                    resolve(result);
                }
            });
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.getAllChatFlowIdDao = getAllChatFlowIdDao;
const getAllSessionsDao = (data) => {
    return new Promise((resolve, reject) => {
        try {
            const mysqlq = `SELECT * FROM sessionHistory WHERE domainId=${data.domainId} order by shid desc limit ${data.limit} offset ${data.offset}`;
            db_1.mysqlPoolConnection.query(mysqlq, (err, result) => {
                if (err) {
                    reject(err);
                }
                else {
                    resolve(result);
                }
            });
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.getAllSessionsDao = getAllSessionsDao;
/**
 *
 * @param data
 * @returns get Message WebChat
 */
const getLeadCRMinformation = (data) => __awaiter(void 0, void 0, void 0, function* () {
    return new Promise((resolve, reject) => {
        try {
            const query = `SELECT * FROM tb_LeadGeneration WHERE domainId=${data === null || data === void 0 ? void 0 : data.domainId}`;
            db_1.mysqlPoolConnection.query(query, (err, result) => {
                var _a, _b, _c;
                if (err) {
                    reject(err);
                    console.log(err);
                }
                // console.log('LeadGeneration>>>>>>>>>>>>>>>', result);
                if (result && (result === null || result === void 0 ? void 0 : result.length) > 0) {
                    const dataRes = (_a = result[0]) === null || _a === void 0 ? void 0 : _a.connector;
                    if (dataRes === null || dataRes === void 0 ? void 0 : dataRes.includes('Worktual')) {
                        const worktualList = {
                            First_Name: data === null || data === void 0 ? void 0 : data.first_name,
                            Last_Name: data === null || data === void 0 ? void 0 : data.last_name,
                            Contact_owner: '1',
                            Phone_number: data === null || data === void 0 ? void 0 : data.mobile_no,
                            Email: data === null || data === void 0 ? void 0 : data.email,
                            p_UserId: 5,
                            p_CompanyId: data === null || data === void 0 ? void 0 : data.companyId,
                            user_contact_id: 0 // if contact exist , give user_contact_id
                        };
                        // console.log('dataRes>>>>>>>>>>', dataRes);
                        axios_1.default.post('https://stagemarketingapi.worktual.co.uk/marketing_automation/v1/contact/create_contact', worktualList).then((response) => {
                            console.log(response, 'response');
                        }).catch((error) => { console.log(error); });
                    }
                    if (dataRes === null || dataRes === void 0 ? void 0 : dataRes.includes('Salesforce')) {
                        const salesForceList = {
                            FirstName: data === null || data === void 0 ? void 0 : data.first_name,
                            LastName: data === null || data === void 0 ? void 0 : data.first_name,
                            OtherStreet: (_b = data === null || data === void 0 ? void 0 : data.address) !== null && _b !== void 0 ? _b : null,
                            Phone: (_c = data === null || data === void 0 ? void 0 : data.mobile_no) !== null && _c !== void 0 ? _c : null,
                            Email: data === null || data === void 0 ? void 0 : data.email
                        };
                        db_1.mysqlPoolConnection.query('call ccaas_get_crm_connector_token(?)', [data === null || data === void 0 ? void 0 : data.domainId], (err1, result1) => {
                            var _a;
                            if (err1) {
                                reject(err1);
                                console.log(err1);
                            }
                            const nextNode = (_a = result1[0]) === null || _a === void 0 ? void 0 : _a.find((nodes) => (nodes === null || nodes === void 0 ? void 0 : nodes.connector_name) === 'salesforce');
                            // console.log(nextNode?.token,"yfuwyediwyrweuwooppop")
                            axios_1.default.post('https://ccaasapi.worktual.co.uk/connector/v1/salesforce/contacts', salesForceList, {
                                headers: {
                                    'Content-Type': 'application/json',
                                    Authorization: nextNode === null || nextNode === void 0 ? void 0 : nextNode.token
                                }
                            }).then((response) => __awaiter(void 0, void 0, void 0, function* () {
                                var _b;
                                if ((response === null || response === void 0 ? void 0 : response.data.statusCode) === 401) {
                                    const refersh = yield (0, exports.getSalesforceToken)(data === null || data === void 0 ? void 0 : data.domainId, nextNode === null || nextNode === void 0 ? void 0 : nextNode.token);
                                    axios_1.default.post('https://ccaasapi.worktual.co.uk/connector/v1/salesforce/contacts', salesForceList, {
                                        headers: {
                                            'Content-Type': 'application/json',
                                            Authorization: (_b = refersh === null || refersh === void 0 ? void 0 : refersh.data) === null || _b === void 0 ? void 0 : _b.token
                                        }
                                    }).then((response1) => {
                                        // console.log(response1, 'response');
                                    }).catch((error) => { console.log(error); });
                                }
                                // console.log(response,"response")
                            })).catch((error) => { console.log(error); });
                        });
                    }
                }
                // const list = {
                //   First_Name: data?.firstName,
                //   Last_Name: data?.lastName,
                //   Contact_owner: '1',
                //   Phone_number: data?.primaryPhoneNumber,
                //   Email: data?.primaryEmail,
                //   p_UserId: 5,
                //   p_CompanyId: data?.companyId,
                //   user_contact_id: 0  // if contact exist , give user_contact_id
                // };
                // axios.post('https://stagemarketingapi.worktual.co.uk/marketing_automation/v1/contact/create_contact', list).then((response) => {
                //   console.log(response, 'response');
                // }).catch((error: any) => { console.log('create_contact errr>>>>', error); });
                // if (data?.primaryEmail) {
                //   const payload = {
                //     address: data?.address,
                //     city: data?.city,
                //     company: data?.companyId,
                //     country: data?.country,
                //     website: '',
                //     email: data?.primaryEmail,
                //     firstName: data?.firstName,
                //     phone: data?.primaryPhoneNumber,
                //     message: '',
                //     lastName: data?.lastName
                //   };
                //   axios.post('https://mysqlapi.worktual.co.uk/urpricing-mysql/v1/contactUs_hubspot_form', payload).then((response: any) => {
                //     if (response.status === 200) {
                //       console.log('contactUs_hubspot_form>>>>>', response.data);
                //       if (response?.data.statusCode === 401 && response?.data?.errCode === -1 && response?.data?.message?.message.includes('Contact already exists')) {
                //         const hubspotId = response.data?.message?.message?.split(':')?.[1];
                //         const updatePayload = { ...payload, hubspotId: Number(hubspotId) };
                //         axios.post('https://mysqlapi.worktual.co.uk/urpricing-mysql/v1/contactUs_hubspot_form_update', updatePayload).then((updateResponse: any) => {
                //           console.log('contactUs_hubspot_form_update>>>>', updateResponse.data);
                //         }).catch((error: any) => { console.log('contactUs_hubspot_form_update err>>>>', error); });
                //       }
                //     }
                //   }).catch((error: any) => { console.log('contactUs_hubspot_form errr>>>>', error); });
                // }
                resolve(result);
            });
        }
        catch (error) {
            reject(error);
        }
    });
});
exports.getLeadCRMinformation = getLeadCRMinformation;
const getSalesforceToken = (domainId, token) => {
    return new Promise((resolve, reject) => {
        try {
            axios_1.default.get(`https://ccaasapi.worktual.co.uk/connector/v1/salesforce/refresh_token/${domainId}`, {
                headers: {
                    'Content-Type': 'application/json',
                    Authorization: token
                }
            }).then((res) => {
                resolve(res);
                // console.log(res,"response88888888888")
            }).catch((error) => { console.log(error); });
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.getSalesforceToken = getSalesforceToken;
/**
 *
 * @param data
 * @param dataq
 * @returns
 */
const getUserList = (data) => {
    return new Promise((resolve, reject) => {
        try {
            db_1.mysqlPoolConnection.query('call ccaas_getUserListByRoleHandler(?,?)', [data === null || data === void 0 ? void 0 : data.domainId, '1,2,3,4,5,6'], (err, result) => {
                var _a;
                if (err) {
                    reject(err);
                }
                else {
                    if (result && ((_a = result[0]) === null || _a === void 0 ? void 0 : _a.length) > 0) {
                        resolve(result[0]);
                    }
                    else {
                        resolve([]);
                    }
                }
            });
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.getUserList = getUserList;
/**
 *
 * @param data
 * @param dataq
 * @returns
 */
const agentExtSocketIdMapping = (data, id) => {
    return new Promise((resolve, reject) => {
        try {
            db_1.mysqlPoolConnection.query('CALL InsertChatConnection(?,?,?)', [data === null || data === void 0 ? void 0 : data.domainId, data === null || data === void 0 ? void 0 : data.ext, id], (err, result) => {
                var _a;
                console.log('result', result);
                console.log('err', err);
                if (err) {
                    reject(err);
                }
                else {
                    if (result && ((_a = result[0]) === null || _a === void 0 ? void 0 : _a.length) > 0) {
                        resolve(result[0]);
                    }
                    else {
                        resolve([]);
                    }
                }
            });
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.agentExtSocketIdMapping = agentExtSocketIdMapping;
/**
 *
 * @param data
 * @param dataq
 * @returns
 */
const getAgentDetails = (id) => {
    return new Promise((resolve, reject) => {
        try {
            db_1.mysqlPoolConnection.query('CALL GetChatConnectionBySocketId(?)', [id], (err, result) => {
                var _a;
                // console.log('result', result[0][0]);
                // console.log('err', err);
                if (err) {
                    reject(err);
                }
                else {
                    if (result && ((_a = result[0]) === null || _a === void 0 ? void 0 : _a.length) > 0) {
                        resolve(result[0]);
                    }
                    else {
                        resolve([]);
                    }
                }
            });
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.getAgentDetails = getAgentDetails;
/**
 *
 * @param data
 * @returns
 */
const getConcurrentChat = (data) => {
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        var _a;
        try {
            db_1.mysqlPoolConnection.query('call up_GetConcurrentAgentLimit(?)', [
                (_a = data === null || data === void 0 ? void 0 : data.domainId) !== null && _a !== void 0 ? _a : null
            ], (err, result) => {
                console.log('up_GetConcurrentAgentLimit result', result);
                console.log('up_GetConcurrentAgentLimit err', err);
                if (err) {
                    reject(err);
                    console.log(err);
                }
                if (result && result.length > 0) {
                    resolve(result[0]);
                }
                else {
                    reject(err);
                }
            });
        }
        catch (err) {
            reject(err);
        }
    }));
};
exports.getConcurrentChat = getConcurrentChat;
/**
 *
 * @param data
 * @returns
 */
const insertUpdateConcurrentChat = (data) => {
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        var _a, _b, _c, _d, _e;
        try {
            db_1.mysqlPoolConnection.query('call up_InsertUpdateConcurrentAgentLimit(?,?,?,?,?,?)', [
                data === null || data === void 0 ? void 0 : data.domainId,
                (_a = data === null || data === void 0 ? void 0 : data.isAllAgent) !== null && _a !== void 0 ? _a : null,
                (_b = data === null || data === void 0 ? void 0 : data.allAgentLimit) !== null && _b !== void 0 ? _b : null,
                (_c = data === null || data === void 0 ? void 0 : data.isAgent) !== null && _c !== void 0 ? _c : null,
                (_d = data === null || data === void 0 ? void 0 : data.isRoutingProfile) !== null && _d !== void 0 ? _d : null,
                (_e = data === null || data === void 0 ? void 0 : data.agentOccupyDetails) !== null && _e !== void 0 ? _e : null
            ], (err, result) => {
                if (err) {
                    reject(err);
                    console.log(err);
                }
                if (result && result.length > 0) {
                    resolve(result[0]);
                }
                else {
                    reject(err);
                }
            });
        }
        catch (err) {
            reject(err);
        }
    }));
};
exports.insertUpdateConcurrentChat = insertUpdateConcurrentChat;
/**
 *
 * @param data
 * @param dataq
 * @returns
 */
const agentExtSocketIdDelete = (id) => {
    return new Promise((resolve, reject) => {
        try {
            db_1.mysqlPoolConnection.query('CALL DeleteChatConnection(?)', [id], (err, result) => {
                var _a;
                if (err) {
                    reject(err);
                }
                else {
                    if (result && ((_a = result[0]) === null || _a === void 0 ? void 0 : _a.length) > 0) {
                        resolve(result[0]);
                    }
                    else {
                        resolve([]);
                    }
                }
            });
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.agentExtSocketIdDelete = agentExtSocketIdDelete;
/**
 *
 * @param data
 * @param dataq
 * @returns
 */
const getMaxWaitTime = (data) => {
    return new Promise((resolve, reject) => {
        try {
            db_1.mysqlPoolConnection.query('call GetMaximumWaitTime(?)', [data === null || data === void 0 ? void 0 : data.queueId], (err, result) => {
                var _a;
                if (err) {
                    reject(err);
                }
                else {
                    if (result && ((_a = result[0]) === null || _a === void 0 ? void 0 : _a.length) > 0) {
                        resolve(result[0]);
                    }
                    else {
                        resolve([]);
                    }
                }
            });
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.getMaxWaitTime = getMaxWaitTime;
const insertNotificationDao = (data) => {
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        try {
            db_1.mysqlPoolConnection.query('call ccaas_notification_details(?,?,?,?,?,?,?,?,?,?)', [
                data === null || data === void 0 ? void 0 : data.uuid,
                data === null || data === void 0 ? void 0 : data.domainId,
                data === null || data === void 0 ? void 0 : data.toExt,
                data === null || data === void 0 ? void 0 : data.notificationMsg,
                data === null || data === void 0 ? void 0 : data.markAsRead,
                data === null || data === void 0 ? void 0 : data.timeStamp,
                data === null || data === void 0 ? void 0 : data.type,
                data === null || data === void 0 ? void 0 : data.channelType,
                data === null || data === void 0 ? void 0 : data.coid,
                data === null || data === void 0 ? void 0 : data.customerValue
            ], (err, result) => {
                console.log('result', result);
                if (err) {
                    console.log('err:', err);
                    reject(err);
                }
                else {
                    if ((result === null || result === void 0 ? void 0 : result.length) > 0) {
                        resolve(result);
                    }
                    else {
                        resolve([]);
                    }
                }
            });
        }
        catch (err) {
            reject(err);
        }
    }));
};
exports.insertNotificationDao = insertNotificationDao;
const getCustomerDetails = (customerNumber) => {
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        try {
            db_1.mysqlPoolConnection.query('call ccaas_get_customer_details_by_phoneNumber(?)', [
                customerNumber,
            ], (err, result) => {
                if (err) {
                    reject(err);
                    console.log('getCustomerDetails reject err: ', err);
                }
                else {
                    console.log('getCustomerDetails inside output');
                    resolve((result === null || result === void 0 ? void 0 : result.length) > 0 ? result[0] : null);
                }
            });
        }
        catch (err) {
            reject(err);
        }
    }));
};
exports.getCustomerDetails = getCustomerDetails;
const getCustomerInfo = (data) => {
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        try {
            db_1.mysqlPoolConnection.query('call ccaas_get_customer_dtl_by_phoneNumber_domainId(?,?)', [
                data === null || data === void 0 ? void 0 : data.phoneNumber,
                data === null || data === void 0 ? void 0 : data.domainId
            ], (err, result) => {
                if (err) {
                    reject(err);
                    console.log('getCustomerInfo reject err: ', err);
                }
                else {
                    console.log('getCustomerInfo inside output');
                    resolve((result === null || result === void 0 ? void 0 : result.length) > 0 ? result[0] : null);
                }
            });
        }
        catch (err) {
            reject(err);
        }
    }));
};
exports.getCustomerInfo = getCustomerInfo;
/**
 *
 * @param data
 * @param dataq
 * @returns
 */
const getUserListByExt = (data) => {
    return new Promise((resolve, reject) => {
        try {
            db_1.mysqlPoolConnection.query('call ccaas_getUserListByExtHandler(?,?)', [data === null || data === void 0 ? void 0 : data.domain_id, data === null || data === void 0 ? void 0 : data.ext], (err, result) => {
                var _a;
                if (err) {
                    reject(err);
                }
                else {
                    if (result && ((_a = result[0]) === null || _a === void 0 ? void 0 : _a.length) > 0) {
                        resolve(result[0]);
                    }
                    else {
                        resolve([]);
                    }
                }
            });
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.getUserListByExt = getUserListByExt;
//# sourceMappingURL=chat.dao.js.map