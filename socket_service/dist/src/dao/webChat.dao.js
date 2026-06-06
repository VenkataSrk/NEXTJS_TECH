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
exports.updateIsBot = exports.insertUpdateVisitorDao = exports.updateMissedChatFb = exports.sendMessageFb = exports.updateChatMessageFb = exports.updateCallbackStatusFb = exports.sendCallbackRequestFb = exports.getCustomerCoIdFb = exports.getFacebookConfigData = exports.audioMsg = exports.updateCallbackStatus = exports.getCustomerCoId = exports.updateMissedChat = exports.updateWhatsappChatMessage = exports.UpdateCallAnsweredDurationDao = exports.insertUpdateAgentAssist = exports.getDurationHistoryIdDao = exports.insertUpdateChatIntentDao = exports.updateFacebookMessage = exports.getWebchatLiveDashborad = exports.updateSentimentScoreFB = exports.sentimentalScoreDataFB = exports.updateDispositionBotFB = exports.updateDispositionDiscnnectFB = exports.getExt = exports.updateAgentDetailsFB = exports.updateChatDeflectionChat = exports.chatDeflectionData = exports.sentimentalScore = exports.updateSentimentScoreChat = exports.sendmailTranscript = exports.insertUpdateIrrelevantQaDao = exports.getSupervisiorList = exports.updateSentimentScore = exports.updateDispositionBot = exports.updateDispositionDiscnnect = exports.getDispositinStatus = exports.getBotDetail = exports.setTransferChatMessage = exports.updateWhatsappMessage = exports.getTransferAgent = exports.getAgentStatus = exports.setChatMessage = exports.getAIChatMessageDao = exports.InsertUpdateAIChatMessage = exports.updateChatMessage = exports.updateTiggerStatus = exports.updateAgentDetails = exports.updateAgentDetailsdata = exports.createChatSession = void 0;
exports.updateAssistMessage = exports.getQueueWaitTimeInsta = exports.getAgentAssistMessageDtl = exports.getQueueWaitTimeFacebook = exports.getQueueWaitTimeWhatsapp = exports.getQueueWaitTime = exports.insertUpdateQueueWaitTimeWhatsapp = exports.insertUpdateQueueWaitTimeDao = exports.getAllQueueWaitTime = exports.getCustomerConatctDetails = exports.updateIsBotFb = exports.updateQueueIdFacebook = exports.updateQueueId = void 0;
const axios_1 = __importDefault(require("axios"));
const uuid_1 = require("uuid");
const constants_1 = require("../helpers/constants");
const db_1 = require("../plugins/db");
/**
 *
 * WebChat Create session MySql Migration
 *
 */
const createChatSession = (data) => {
    return new Promise((resolve, reject) => {
        var _a, _b, _c, _d, _e, _f, _g, _h, _j, _k, _l, _m, _o, _p, _q, _r, _s, _t, _u, _v, _w, _x, _y, _z, _0, _1, _2, _3, _4, _5, _6;
        try {
            db_1.mysqlPoolConnection.query('call up_InsertUpdateChatHistory(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)', [
                data === null || data === void 0 ? void 0 : data.ChatHistoryId,
                (_a = data === null || data === void 0 ? void 0 : data.sessionId) !== null && _a !== void 0 ? _a : null,
                (_b = data === null || data === void 0 ? void 0 : data.sessionStartTime) !== null && _b !== void 0 ? _b : null,
                (_c = data === null || data === void 0 ? void 0 : data.sessionEndTime) !== null && _c !== void 0 ? _c : null,
                (_d = data === null || data === void 0 ? void 0 : data.chatFlowSourceId) !== null && _d !== void 0 ? _d : null,
                (_e = data === null || data === void 0 ? void 0 : data.companyId) !== null && _e !== void 0 ? _e : null,
                (_f = data === null || data === void 0 ? void 0 : data.domainId) !== null && _f !== void 0 ? _f : null,
                (_g = data === null || data === void 0 ? void 0 : data.IsInBound) !== null && _g !== void 0 ? _g : null,
                // data?.dialDuration ?? null,
                (_h = data === null || data === void 0 ? void 0 : data.disconnectedBy) !== null && _h !== void 0 ? _h : null,
                (_j = data === null || data === void 0 ? void 0 : data.queueDuration) !== null && _j !== void 0 ? _j : null,
                (_k = data === null || data === void 0 ? void 0 : data.assignedTo) !== null && _k !== void 0 ? _k : null,
                (_l = data === null || data === void 0 ? void 0 : data.assignedBy) !== null && _l !== void 0 ? _l : null,
                (_m = data === null || data === void 0 ? void 0 : data.assignedOn) !== null && _m !== void 0 ? _m : null,
                (_o = data === null || data === void 0 ? void 0 : data.CustomerId) !== null && _o !== void 0 ? _o : null,
                (_p = data === null || data === void 0 ? void 0 : data.Browser) !== null && _p !== void 0 ? _p : null,
                (_q = data === null || data === void 0 ? void 0 : data.IPAdress) !== null && _q !== void 0 ? _q : null,
                (_r = data === null || data === void 0 ? void 0 : data.ChatSubject) !== null && _r !== void 0 ? _r : null,
                (_s = data === null || data === void 0 ? void 0 : data.Message) !== null && _s !== void 0 ? _s : null,
                (_t = data === null || data === void 0 ? void 0 : data.StartTime) !== null && _t !== void 0 ? _t : null,
                (_u = data === null || data === void 0 ? void 0 : data.EndTime) !== null && _u !== void 0 ? _u : null,
                (_v = data === null || data === void 0 ? void 0 : data.Duration) !== null && _v !== void 0 ? _v : null,
                (_w = data === null || data === void 0 ? void 0 : data.AgentId) !== null && _w !== void 0 ? _w : null,
                (_x = data === null || data === void 0 ? void 0 : data.AgentSkillId) !== null && _x !== void 0 ? _x : null,
                (_y = data === null || data === void 0 ? void 0 : data.IsBot) !== null && _y !== void 0 ? _y : null,
                (_z = data === null || data === void 0 ? void 0 : data.BotId) !== null && _z !== void 0 ? _z : null,
                (_0 = data === null || data === void 0 ? void 0 : data.transferredTime) !== null && _0 !== void 0 ? _0 : null,
                (_1 = data === null || data === void 0 ? void 0 : data.QueueId) !== null && _1 !== void 0 ? _1 : null,
                (_2 = data === null || data === void 0 ? void 0 : data.location) !== null && _2 !== void 0 ? _2 : null,
                (_3 = data === null || data === void 0 ? void 0 : data.DeviceType) !== null && _3 !== void 0 ? _3 : null,
                (_4 = data === null || data === void 0 ? void 0 : data.CallBackStatus) !== null && _4 !== void 0 ? _4 : null,
                (_5 = data === null || data === void 0 ? void 0 : data.widgetId) !== null && _5 !== void 0 ? _5 : null,
                (_6 = data === null || data === void 0 ? void 0 : data.IsMissed) !== null && _6 !== void 0 ? _6 : null
            ], (err, result) => {
                if (err) {
                    console.log(err);
                    reject(err);
                }
                else {
                    resolve(result);
                }
            });
        }
        catch (error) {
            console.log(error);
        }
    });
};
exports.createChatSession = createChatSession;
const updateAgentDetailsdata = (data) => {
    return new Promise((resolve, reject) => {
        var _a, _b, _c, _d, _e, _f, _g, _h, _j, _k, _l, _m;
        try {
            db_1.mysqlPoolConnection.query('call up_InsertUpdateWhatappDurationHistory(?,?,?,?,?,?,?,?,?,?,?,?)', [
                (_a = data === null || data === void 0 ? void 0 : data.WhatsappDurationHistoryId) !== null && _a !== void 0 ? _a : null,
                (_b = data === null || data === void 0 ? void 0 : data.WhatsappHistoryId) !== null && _b !== void 0 ? _b : null,
                (_c = data === null || data === void 0 ? void 0 : data.StartTime) !== null && _c !== void 0 ? _c : null,
                (_d = data === null || data === void 0 ? void 0 : data.EndTime) !== null && _d !== void 0 ? _d : null,
                (_e = data === null || data === void 0 ? void 0 : data.Duration) !== null && _e !== void 0 ? _e : null,
                (_f = data === null || data === void 0 ? void 0 : data.AgentId) !== null && _f !== void 0 ? _f : null,
                (_g = data === null || data === void 0 ? void 0 : data.AgentSkillId) !== null && _g !== void 0 ? _g : null,
                (_h = data === null || data === void 0 ? void 0 : data.IsBot) !== null && _h !== void 0 ? _h : null,
                (_j = data === null || data === void 0 ? void 0 : data.BotId) !== null && _j !== void 0 ? _j : null,
                (_k = data === null || data === void 0 ? void 0 : data.QueueId) !== null && _k !== void 0 ? _k : null,
                (_l = data === null || data === void 0 ? void 0 : data.dialDuration) !== null && _l !== void 0 ? _l : null,
                (_m = data === null || data === void 0 ? void 0 : data.IsMissed) !== null && _m !== void 0 ? _m : null
            ], (err, result) => {
                if (err) {
                    reject(err);
                }
                else {
                    resolve(result);
                }
            });
        }
        catch (error) {
            console.log(error);
        }
    });
};
exports.updateAgentDetailsdata = updateAgentDetailsdata;
const updateAgentDetails = (data) => {
    return new Promise((resolve, reject) => {
        var _a, _b, _c, _d, _e, _f, _g, _h, _j, _k, _l, _m, _o, _p, _q, _r, _s, _t, _u, _v, _w, _x, _y, _z, _0;
        try {
            db_1.mysqlPoolConnection.query('call up_InserUpdateChatDurationHistory(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)', [
                ((data === null || data === void 0 ? void 0 : data.ChatDurationHistoryId) !== '' && (data === null || data === void 0 ? void 0 : data.ChatDurationHistoryId)) ? data === null || data === void 0 ? void 0 : data.ChatDurationHistoryId : null,
                (data === null || data === void 0 ? void 0 : data.ChatHistoryId) ? data.ChatHistoryId : null,
                (_a = data === null || data === void 0 ? void 0 : data.StartTime) !== null && _a !== void 0 ? _a : null,
                (_b = data === null || data === void 0 ? void 0 : data.EndTime) !== null && _b !== void 0 ? _b : null,
                (_c = data === null || data === void 0 ? void 0 : data.Duration) !== null && _c !== void 0 ? _c : null,
                (_d = data === null || data === void 0 ? void 0 : data.AgentId) !== null && _d !== void 0 ? _d : null,
                (_e = data === null || data === void 0 ? void 0 : data.AgentSkillId) !== null && _e !== void 0 ? _e : null,
                (_f = data === null || data === void 0 ? void 0 : data.IsBot) !== null && _f !== void 0 ? _f : null,
                (_g = data === null || data === void 0 ? void 0 : data.BotId) !== null && _g !== void 0 ? _g : null,
                (_h = data === null || data === void 0 ? void 0 : data.transferredTime) !== null && _h !== void 0 ? _h : null,
                (_j = data === null || data === void 0 ? void 0 : data.QueueId) !== null && _j !== void 0 ? _j : null,
                (_k = data === null || data === void 0 ? void 0 : data.dispositionId) !== null && _k !== void 0 ? _k : null,
                (_l = data === null || data === void 0 ? void 0 : data.transferedBy) !== null && _l !== void 0 ? _l : null,
                (_m = data === null || data === void 0 ? void 0 : data.transferedTo) !== null && _m !== void 0 ? _m : null,
                (_o = data === null || data === void 0 ? void 0 : data.ticketId) !== null && _o !== void 0 ? _o : null,
                (_p = data === null || data === void 0 ? void 0 : data.Summary) !== null && _p !== void 0 ? _p : null,
                (_q = data === null || data === void 0 ? void 0 : data.SentimentalScore) !== null && _q !== void 0 ? _q : null,
                (_r = data === null || data === void 0 ? void 0 : data.sentimentalScoreCustomer) !== null && _r !== void 0 ? _r : null,
                (_s = data === null || data === void 0 ? void 0 : data.sentimentalScoreAgent) !== null && _s !== void 0 ? _s : null,
                (_t = data === null || data === void 0 ? void 0 : data.markAsRead) !== null && _t !== void 0 ? _t : null,
                (_u = data === null || data === void 0 ? void 0 : data.afterChatWorkTime) !== null && _u !== void 0 ? _u : null,
                (_v = data === null || data === void 0 ? void 0 : data.Istransferred) !== null && _v !== void 0 ? _v : null,
                (_w = data === null || data === void 0 ? void 0 : data.IsMissed) !== null && _w !== void 0 ? _w : null,
                (_x = data === null || data === void 0 ? void 0 : data.IsAbandoned) !== null && _x !== void 0 ? _x : null,
                (_y = data === null || data === void 0 ? void 0 : data.answeredDuration) !== null && _y !== void 0 ? _y : null,
                (_z = data === null || data === void 0 ? void 0 : data.chatDeflection) !== null && _z !== void 0 ? _z : null,
                (_0 = data === null || data === void 0 ? void 0 : data.isTrigger) !== null && _0 !== void 0 ? _0 : 0,
            ], (err, result) => {
                if (err) {
                    console.log('up_InserUpdateChatDurationHistory', err);
                    reject(err);
                }
                else {
                    resolve(result);
                }
            });
        }
        catch (error) {
            console.log('up_InserUpdateChatDurationHistory error', error);
        }
    });
};
exports.updateAgentDetails = updateAgentDetails;
const updateTiggerStatus = (data) => {
    return new Promise((resolve, reject) => {
        var _a, _b;
        try {
            db_1.mysqlPoolConnection.query('call ccaas_update_isTrigger(?,?,?)', [
                (_a = data === null || data === void 0 ? void 0 : data.durationHistoryId) !== null && _a !== void 0 ? _a : null,
                data === null || data === void 0 ? void 0 : data.ChannelType,
                (_b = data === null || data === void 0 ? void 0 : data.isTrigger) !== null && _b !== void 0 ? _b : 0,
            ], (err, result) => {
                if (err) {
                    console.log('ccaas_update_isTrigger', err);
                    reject(err);
                }
                else {
                    resolve(result);
                }
            });
        }
        catch (error) {
            console.log('ccaas_update_isTrigger error', error);
        }
    });
};
exports.updateTiggerStatus = updateTiggerStatus;
const updateChatMessage = (data) => {
    var _a, _b, _c, _d, _e, _f, _g, _h;
    const agentMsg = (_a = data === null || data === void 0 ? void 0 : data.message) === null || _a === void 0 ? void 0 : _a.agentText;
    const userMsg = (_b = data === null || data === void 0 ? void 0 : data.message) === null || _b === void 0 ? void 0 : _b.userText;
    const isCustomer = ((_c = userMsg === null || userMsg === void 0 ? void 0 : userMsg.message) === null || _c === void 0 ? void 0 : _c.isCustomer) ? 1 : 0;
    const isAgent = ((_d = agentMsg === null || agentMsg === void 0 ? void 0 : agentMsg.message) === null || _d === void 0 ? void 0 : _d.agentMsgCount) ? 1 : 0;
    const botCustomerMsgCount = isAgent ? 0 : (((_e = agentMsg === null || agentMsg === void 0 ? void 0 : agentMsg.message) === null || _e === void 0 ? void 0 : _e.isLiveagent) || ((_f = userMsg === null || userMsg === void 0 ? void 0 : userMsg.message) === null || _f === void 0 ? void 0 : _f.isLiveagent)) ? 0 : 1;
    const botMsgCount = !isAgent && agentMsg && !((_g = agentMsg === null || agentMsg === void 0 ? void 0 : agentMsg.message) === null || _g === void 0 ? void 0 : _g.isLiveagent) && !((_h = agentMsg === null || agentMsg === void 0 ? void 0 : agentMsg.message) === null || _h === void 0 ? void 0 : _h.isCustomer) ? 1 : 0;
    const message = JSON.stringify(data.message);
    const chat_id = parseInt(data === null || data === void 0 ? void 0 : data.ChatHistoryId, 10) || 0;
    return new Promise((resolve, reject) => {
        var _a;
        try {
            db_1.mysqlPoolConnection.query('call up_InserUpdateChatMessage(?,?,?,?,?,?,?)', [
                (_a = data === null || data === void 0 ? void 0 : data.ChatDurationHistoryId) !== null && _a !== void 0 ? _a : 0,
                chat_id !== null && chat_id !== void 0 ? chat_id : 0,
                message,
                isCustomer,
                isAgent,
                botCustomerMsgCount,
                botMsgCount
            ], (err, result) => {
                if (err) {
                    reject(err);
                }
                else {
                    resolve(result);
                }
            });
        }
        catch (error) {
            console.log(error);
        }
    });
};
exports.updateChatMessage = updateChatMessage;
const InsertUpdateAIChatMessage = (data) => {
    const message = JSON.stringify(data);
    // const chatHisId = parseInt(data?.ChatHistoryId);
    const chatHisId = parseInt(data === null || data === void 0 ? void 0 : data.ChatHistoryId, 10);
    return new Promise((resolve, reject) => {
        try {
            db_1.mysqlPoolConnection.query('call up_InsertUpdateChatAIMessage(?,?,?)', [chatHisId !== null && chatHisId !== void 0 ? chatHisId : 0, message, data === null || data === void 0 ? void 0 : data.sessionId], (err, result) => {
                if (err) {
                    console.log('err::::::', err);
                    // reject(err);
                }
                else {
                    resolve(result);
                }
            });
        }
        catch (error) {
            console.log(error);
        }
    });
};
exports.InsertUpdateAIChatMessage = InsertUpdateAIChatMessage;
const getAIChatMessageDao = (data) => {
    console.log('getAIChatMessage---', data);
    return new Promise((resolve, reject) => {
        var _a;
        try {
            db_1.mysqlPoolConnection.query('call up_GetChatAIMessage(?)', [(_a = data === null || data === void 0 ? void 0 : data.ChatHistoryId) !== null && _a !== void 0 ? _a : 0], (err, result) => {
                if (err) {
                    reject(err);
                }
                else {
                    resolve(result);
                }
            });
        }
        catch (error) {
            console.log(error);
        }
    });
};
exports.getAIChatMessageDao = getAIChatMessageDao;
/**
 *
 * @param data
 * @returns
 */
const setChatMessage = (data) => {
    console.log('setChatMessage---', data);
    return new Promise((resolve, reject) => {
        try {
            db_1.mysqlPoolConnection.query('call up_SetChatMessage(?,?,?)', [data === null || data === void 0 ? void 0 : data.ChatDurationHistoryId, data === null || data === void 0 ? void 0 : data.ChatHistoryId, data === null || data === void 0 ? void 0 : data.message], (err, result) => {
                if (err) {
                    console.log(err);
                }
                else {
                    resolve(result);
                }
            });
        }
        catch (err) {
            console.log();
        }
    });
};
exports.setChatMessage = setChatMessage;
/*
 * @param userId
 * @param companyId
 * @returns
 */
const getAgentStatus = (data) => {
    const channelCode = constants_1.channelTypeCode[data === null || data === void 0 ? void 0 : data.channelType];
    return new Promise((resolve, reject) => {
        var _a, _b, _c;
        try {
            db_1.mysqlPoolConnection.query('call up_DeclinedUser(?,?,?,?)', [
                (_a = data === null || data === void 0 ? void 0 : data.sessionId) !== null && _a !== void 0 ? _a : null,
                channelCode,
                (_b = data === null || data === void 0 ? void 0 : data.userId) !== null && _b !== void 0 ? _b : null,
                (_c = data === null || data === void 0 ? void 0 : data.domainId) !== null && _c !== void 0 ? _c : null,
            ], (errors, res) => {
                var _a;
                if (errors) {
                    console.error(errors);
                    reject([]);
                }
                else {
                    if (((_a = res[0]) === null || _a === void 0 ? void 0 : _a.length) > 0) {
                        resolve(res);
                    }
                    else {
                        resolve([]);
                    }
                }
            });
        }
        catch (error) {
            console.error(error);
            reject(error);
        }
    });
};
exports.getAgentStatus = getAgentStatus;
/**
 *
 * @param userId
 * @param companyId
 * @returns
 */
const getTransferAgent = (data) => {
    const channelCode = constants_1.channelTypeCode[data === null || data === void 0 ? void 0 : data.channelType];
    return new Promise((resolve, reject) => {
        var _a, _b;
        try {
            db_1.mysqlPoolConnection.query('call up_CheckUserReadyForSocialMedia(?,?,?,?)', [
                -1,
                (_a = data === null || data === void 0 ? void 0 : data.domainId) !== null && _a !== void 0 ? _a : 0,
                (_b = data === null || data === void 0 ? void 0 : data.sessionId) !== null && _b !== void 0 ? _b : null,
                channelCode
            ], (errors, res) => {
                var _a;
                if (errors) {
                    console.log(errors);
                    reject([]);
                }
                else {
                    if (((_a = res[0]) === null || _a === void 0 ? void 0 : _a.length) > 0) {
                        resolve(res[0]);
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
exports.getTransferAgent = getTransferAgent;
/**
 *
 * @param data
 * @returns
 */
const updateWhatsappMessage = (data) => {
    const message = JSON.stringify(data.message);
    const chat_id = parseInt(data === null || data === void 0 ? void 0 : data.WhatsappHistoryId, 10);
    return new Promise((resolve, reject) => {
        try {
            db_1.mysqlPoolConnection.query('call up_InserUpdateWhatsappMessage(?,?)', [chat_id !== null && chat_id !== void 0 ? chat_id : 0, message], (err, result) => {
                if (err) {
                    reject(err);
                }
                else {
                    resolve(result);
                }
            });
        }
        catch (error) {
            console.log(error);
        }
    });
};
exports.updateWhatsappMessage = updateWhatsappMessage;
/**
 *
 * @param data
 * @returns
 */
const setTransferChatMessage = (data) => {
    console.log('setChatMessage---', data);
    return new Promise((resolve, reject) => {
        try {
            db_1.mysqlPoolConnection.query('call up_TransaferChatMessage(?,?)', [
                data === null || data === void 0 ? void 0 : data.SourceChatDurationHistoryId,
                data === null || data === void 0 ? void 0 : data.DestinationChatDurationHistoryId
            ], (err, result) => {
                if (err) {
                    console.log(err);
                }
                else {
                    resolve(result);
                }
            });
        }
        catch (err) {
            console.log(err);
        }
    });
};
exports.setTransferChatMessage = setTransferChatMessage;
const getBotDetail = (data) => {
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        try {
            db_1.mysqlPoolConnection.query('call ccaas_getWhatsappBot(?,?)', [
                data === null || data === void 0 ? void 0 : data.agentNumber,
                data === null || data === void 0 ? void 0 : data.domainId
            ], (err, result) => {
                var _a;
                if (err) {
                    reject(err);
                }
                else {
                    if (((_a = result[0]) === null || _a === void 0 ? void 0 : _a.length) > 0) {
                        resolve(result[0]);
                    }
                    else {
                        resolve([]);
                    }
                }
            });
        }
        catch (error) {
            console.log(error);
        }
    }));
};
exports.getBotDetail = getBotDetail;
const getDispositinStatus = (data) => {
    return new Promise((resolve, reject) => {
        try {
            db_1.mysqlPoolConnection.query('call ccaas_getDispositinStatus_info(?)', [
                data === null || data === void 0 ? void 0 : data.domainId
            ], (err, result) => {
                if (err) {
                    console.log('ccaas_getDispositinStatus_info err: ', err);
                    reject(err);
                }
                resolve(result);
            });
        }
        catch (error) {
            console.log('ccaas_getDispositinStatus_info catch error: ', error);
            reject(error);
        }
    });
};
exports.getDispositinStatus = getDispositinStatus;
/**
 *
 * @param data
 * @returns
 */
const updateDispositionDiscnnect = (data, closedDispositionsId) => {
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        var _a, _b, _c, _d, _e;
        try {
            db_1.mysqlPoolConnection.query('call up_UpdateWhatsappHistoryDisposition(?,?,?,?,?,?,?,?)', [
                data === null || data === void 0 ? void 0 : data.historyId,
                data === null || data === void 0 ? void 0 : data.domainId,
                (_a = closedDispositionsId === null || closedDispositionsId === void 0 ? void 0 : closedDispositionsId.did) !== null && _a !== void 0 ? _a : null,
                (_b = closedDispositionsId === null || closedDispositionsId === void 0 ? void 0 : closedDispositionsId.dispositionName) !== null && _b !== void 0 ? _b : null,
                1,
                (_c = data === null || data === void 0 ? void 0 : data.AfterEmailWorkTime) !== null && _c !== void 0 ? _c : null,
                (_d = data === null || data === void 0 ? void 0 : data.dispositionUpdatedBy) !== null && _d !== void 0 ? _d : null,
                (_e = data === null || data === void 0 ? void 0 : data.duration) !== null && _e !== void 0 ? _e : null
            ], (err, result) => {
                var _a;
                if (err) {
                    console.log(err);
                    reject(err);
                }
                if (((_a = result[0]) === null || _a === void 0 ? void 0 : _a.length) > 0) {
                    resolve(result[0]);
                }
                else {
                    resolve([]);
                }
            });
        }
        catch (err) {
            console.log(err);
            reject(err);
        }
    }));
};
exports.updateDispositionDiscnnect = updateDispositionDiscnnect;
/**
 *
 * @param data
 * @returns
 */
const updateDispositionBot = (data) => {
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        try {
            db_1.mysqlPoolConnection.query('call ccaas_whatsappIsBotUpdate(?)', [
                data === null || data === void 0 ? void 0 : data.historyId
            ], (err, result) => {
                var _a;
                if (err) {
                    console.log(err);
                    reject(err);
                }
                if (((_a = result[0]) === null || _a === void 0 ? void 0 : _a.length) > 0) {
                    resolve(result[0]);
                }
                else {
                    resolve([]);
                }
            });
        }
        catch (err) {
            console.log(err);
            reject(err);
        }
    }));
};
exports.updateDispositionBot = updateDispositionBot;
/**
 *
 * @param data
 * @returns
 */
const updateSentimentScore = (data, sentiment) => {
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        var _a, _b, _c, _d, _e, _f, _g;
        try {
            db_1.mysqlPoolConnection.query('call ccaas_updateSentimentalScore(?,?,?,?,?,?,?,?)', [
                (_a = data === null || data === void 0 ? void 0 : data.historyId) !== null && _a !== void 0 ? _a : 0,
                (_b = data === null || data === void 0 ? void 0 : data.WhatsappDurationHistoryId) !== null && _b !== void 0 ? _b : 0,
                'Whatsapp',
                (_c = data === null || data === void 0 ? void 0 : data.domainId) !== null && _c !== void 0 ? _c : null,
                (_d = sentiment === null || sentiment === void 0 ? void 0 : sentiment.sentimentalScore) !== null && _d !== void 0 ? _d : null,
                (_e = sentiment === null || sentiment === void 0 ? void 0 : sentiment.sentimentalScoreUser) !== null && _e !== void 0 ? _e : null,
                (_f = sentiment === null || sentiment === void 0 ? void 0 : sentiment.sentimentalScoreAgent) !== null && _f !== void 0 ? _f : null,
                (_g = data === null || data === void 0 ? void 0 : data.ext) !== null && _g !== void 0 ? _g : null,
            ], (err, result) => {
                var _a;
                if (err) {
                    reject(err);
                    console.log(err);
                }
                if (((_a = result[0]) === null || _a === void 0 ? void 0 : _a.length) > 0) {
                    resolve(result[0]);
                }
                else {
                    resolve([]);
                }
            });
        }
        catch (err) {
            reject(err);
        }
    }));
};
exports.updateSentimentScore = updateSentimentScore;
const getSupervisiorList = (data) => {
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        try {
            db_1.mysqlPoolConnection.query('call ccaas_get_supervisor_ext(?)', [
                data === null || data === void 0 ? void 0 : data.domainId
            ], (err, result) => {
                var _a;
                if (err) {
                    reject(err);
                    console.log(err);
                }
                if (((_a = result[0]) === null || _a === void 0 ? void 0 : _a.length) > 0) {
                    resolve(result[0]);
                }
                else {
                    resolve([]);
                }
            });
        }
        catch (err) {
            reject(err);
        }
    }));
};
exports.getSupervisiorList = getSupervisiorList;
const insertUpdateIrrelevantQaDao = (data) => {
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        var _a;
        try {
            db_1.mysqlPoolConnection.query('call up_InsertUpdateIrrelevantChatQuestionAnswer(?,?,?,?)', [
                data === null || data === void 0 ? void 0 : data.chatFlowId,
                data === null || data === void 0 ? void 0 : data.domainId,
                (_a = JSON.stringify(data === null || data === void 0 ? void 0 : data.questionAnswer)) !== null && _a !== void 0 ? _a : null,
                data === null || data === void 0 ? void 0 : data.websiteId
            ], (err, result) => {
                console.log(result, 'resultresultresultresult');
                if (err) {
                    console.log(err);
                    reject(err);
                }
                resolve(result);
            });
        }
        catch (err) {
            reject(err);
        }
    }));
};
exports.insertUpdateIrrelevantQaDao = insertUpdateIrrelevantQaDao;
const sendmailTranscript = (datares) => __awaiter(void 0, void 0, void 0, function* () {
    const blobFilePath = datares === null || datares === void 0 ? void 0 : datares.url;
    try {
        // Define email options
        const email = datares === null || datares === void 0 ? void 0 : datares.mail;
        const body = {
            id: (0, uuid_1.v4)(),
            subject: 'Chat Transcript',
            from: 'support@worktual.com',
            to: email,
            text: 'Kindly have a look on the generated chat transcript report',
            sessionId: (0, uuid_1.v4)(),
            attachments: [
                {
                    filename: 'chattranscript.pdf',
                    path: blobFilePath, // Attached file content
                    // content: Buffer.from('hello word', 'utf-8')
                },
            ],
        };
        console.log('body-=-=-=-=-=-=-', body);
        yield axios_1.default
            .post('https://ccaasapi.worktual.co.uk/emailService/v1/send_mail_lead', body)
            .then((response) => {
            console.log(response.status);
        });
        // Send email
        // const info = await transporter.sendMail(mailOptions);
        // console.log('Email sent: ', info.response);
        // Remove the generated PDF file
        // fs.unlinkSync(res.filename);
    }
    catch (error) {
        console.error('Error sending email: ', error);
    }
});
exports.sendmailTranscript = sendmailTranscript;
const updateSentimentScoreChat = (data, sentiment) => {
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        var _a, _b, _c, _d, _e, _f, _g;
        try {
            db_1.mysqlPoolConnection.query('call ccaas_updateSentimentalScore(?,?,?,?,?,?,?,?)', [
                (_a = data === null || data === void 0 ? void 0 : data.HistoryId) !== null && _a !== void 0 ? _a : 0,
                (_b = data === null || data === void 0 ? void 0 : data.ChatDurationHistoryId) !== null && _b !== void 0 ? _b : 0,
                'Chat',
                (_c = data === null || data === void 0 ? void 0 : data.domainId) !== null && _c !== void 0 ? _c : null,
                (_d = sentiment === null || sentiment === void 0 ? void 0 : sentiment.sentimentalScore) !== null && _d !== void 0 ? _d : null,
                (_e = sentiment === null || sentiment === void 0 ? void 0 : sentiment.sentimentalScoreUser) !== null && _e !== void 0 ? _e : null,
                (_f = sentiment === null || sentiment === void 0 ? void 0 : sentiment.sentimentalScoreAgent) !== null && _f !== void 0 ? _f : null,
                (_g = data === null || data === void 0 ? void 0 : data.ext) !== null && _g !== void 0 ? _g : null,
            ], (err, result) => {
                var _a;
                if (err) {
                    reject(err);
                    console.log(err);
                }
                if (((_a = result[0]) === null || _a === void 0 ? void 0 : _a.length) > 0) {
                    resolve(result[0]);
                }
                else {
                    resolve([]);
                }
            });
        }
        catch (err) {
            reject(err);
        }
    }));
};
exports.updateSentimentScoreChat = updateSentimentScoreChat;
function sentimentalScore(data) {
    var _a, _b;
    return __awaiter(this, void 0, void 0, function* () {
        try {
            const body = {
                HistoryId: data === null || data === void 0 ? void 0 : data.HistoryId,
                ChatDurationHistoryId: (_a = data === null || data === void 0 ? void 0 : data.ChatDurationHistoryId) !== null && _a !== void 0 ? _a : 0,
                channelType: data === null || data === void 0 ? void 0 : data.channelType,
                domainId: data === null || data === void 0 ? void 0 : data.domainId,
                ext: parseInt(data === null || data === void 0 ? void 0 : data.ext, 10) || 0
            };
            const response = yield axios_1.default.post((_b = process === null || process === void 0 ? void 0 : process.env) === null || _b === void 0 ? void 0 : _b.Sentimental, body, {
                headers: { 'Content-Type': 'application/json' },
                timeout: 5000
            });
            return response !== null && response !== void 0 ? response : {};
        }
        catch (error) {
            return {};
        }
    });
}
exports.sentimentalScore = sentimentalScore;
// chat deflection
function chatDeflectionData(data) {
    var _a, _b, _c;
    return __awaiter(this, void 0, void 0, function* () {
        try {
            const body = {
                HistoryId: data === null || data === void 0 ? void 0 : data.HistoryId,
                ChatDurationHistoryId: (_a = data === null || data === void 0 ? void 0 : data.ChatDurationHistoryId) !== null && _a !== void 0 ? _a : 0,
                channelType: data === null || data === void 0 ? void 0 : data.channelType,
                domainId: data === null || data === void 0 ? void 0 : data.domainId,
                ext: (_b = data === null || data === void 0 ? void 0 : data.ext) !== null && _b !== void 0 ? _b : 0
            };
            console.log(' chat deflection::', body);
            return yield axios_1.default.post((_c = process === null || process === void 0 ? void 0 : process.env) === null || _c === void 0 ? void 0 : _c.Deflection, body);
        }
        catch (error) {
            console.error('Error sending message:', error);
            // You can handle the error here, e.g., log it or return a specific response
        }
    });
}
exports.chatDeflectionData = chatDeflectionData;
const updateChatDeflectionChat = (data, chatDeflection) => {
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        var _a, _b;
        try {
            db_1.mysqlPoolConnection.query('call up_ccaas_updateChatDeflection(?,?,?)', [
                (_a = data === null || data === void 0 ? void 0 : data.HistoryId) !== null && _a !== void 0 ? _a : null,
                (_b = data === null || data === void 0 ? void 0 : data.ChatDurationHistoryId) !== null && _b !== void 0 ? _b : 0,
                chatDeflection !== null && chatDeflection !== void 0 ? chatDeflection : 0,
            ], (err, result) => {
                var _a;
                if (err) {
                    reject(err);
                    console.log(err);
                }
                if (((_a = result[0]) === null || _a === void 0 ? void 0 : _a.length) > 0) {
                    resolve(result[0]);
                }
                else {
                    resolve([]);
                }
            });
        }
        catch (err) {
            reject(err);
        }
    }));
};
exports.updateChatDeflectionChat = updateChatDeflectionChat;
/**
 *
 * @param userId
 * @param companyId
 * @returns
 */
const updateAgentDetailsFB = (data) => {
    return new Promise((resolve, reject) => {
        var _a, _b, _c, _d, _e, _f;
        try {
            db_1.mysqlPoolConnection.query('call up_UpdateFacebookHistory(?,?,?,?,?,?,?,?,?)', [
                data === null || data === void 0 ? void 0 : data.FacebookHistoryId,
                (_a = data === null || data === void 0 ? void 0 : data.AgentId) !== null && _a !== void 0 ? _a : null,
                data === null || data === void 0 ? void 0 : data.domainId,
                data === null || data === void 0 ? void 0 : data.IsBot,
                (_b = data === null || data === void 0 ? void 0 : data.BotId) !== null && _b !== void 0 ? _b : null,
                (_c = data === null || data === void 0 ? void 0 : data.agentStartTime) !== null && _c !== void 0 ? _c : null,
                (_d = data === null || data === void 0 ? void 0 : data.agentEndTime) !== null && _d !== void 0 ? _d : null,
                (_e = data === null || data === void 0 ? void 0 : data.duration) !== null && _e !== void 0 ? _e : null,
                (_f = data === null || data === void 0 ? void 0 : data.dialDuration) !== null && _f !== void 0 ? _f : null,
            ], (errors, res) => {
                var _a;
                if (errors) {
                    console.log(errors);
                    reject([]);
                }
                else {
                    if (((_a = res[0]) === null || _a === void 0 ? void 0 : _a.length) > 0) {
                        resolve(res[0]);
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
exports.updateAgentDetailsFB = updateAgentDetailsFB;
/**
 *
 * @param data
 * @returns
 */
const getExt = (data) => {
    return new Promise((resolve, reject) => {
        try {
            if (data === null || data === void 0 ? void 0 : data.domainId) {
                const mysqlq = `select distinct(ext) from tb_User where roleid in (3,5) and domainId = ${data === null || data === void 0 ? void 0 : data.domainId}`;
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
                reject();
            }
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.getExt = getExt;
/**
 *
 * @param data
 * @returns
 */
const updateDispositionDiscnnectFB = (data, closedDispositionsId) => {
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        var _a, _b, _c, _d;
        try {
            db_1.mysqlPoolConnection.query('call up_UpdateFacebookHistoryDisposition(?,?,?,?,?,?,?)', [
                data === null || data === void 0 ? void 0 : data.historyId,
                data === null || data === void 0 ? void 0 : data.domainId,
                (_a = closedDispositionsId === null || closedDispositionsId === void 0 ? void 0 : closedDispositionsId.did) !== null && _a !== void 0 ? _a : null,
                (_b = closedDispositionsId === null || closedDispositionsId === void 0 ? void 0 : closedDispositionsId.dispositionName) !== null && _b !== void 0 ? _b : null,
                1,
                (_c = data === null || data === void 0 ? void 0 : data.afterFacebookWorkTime) !== null && _c !== void 0 ? _c : null,
                (_d = data === null || data === void 0 ? void 0 : data.dispositionUpdatedBy) !== null && _d !== void 0 ? _d : null,
            ], (err, result) => {
                if (err) {
                    console.log(err);
                    reject(err);
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
            console.log(err);
            reject(err);
        }
    }));
};
exports.updateDispositionDiscnnectFB = updateDispositionDiscnnectFB;
/**
 *
 * @param data
 * @returns
 */
const updateDispositionBotFB = (data) => {
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        try {
            db_1.mysqlPoolConnection.query('call ccaas_FacebookIsBotUpdate(?)', [
                data === null || data === void 0 ? void 0 : data.historyId
            ], (err, result) => {
                if (err) {
                    console.log(err);
                    reject(err);
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
            console.log(err);
            reject(err);
        }
    }));
};
exports.updateDispositionBotFB = updateDispositionBotFB;
// senti mental Score facebook
function sentimentalScoreDataFB(data) {
    var _a, _b, _c;
    return __awaiter(this, void 0, void 0, function* () {
        try {
            const body = {
                HistoryId: data === null || data === void 0 ? void 0 : data.historyId,
                ChatDurationHistoryId: (_a = data === null || data === void 0 ? void 0 : data.ChatDurationHistoryId) !== null && _a !== void 0 ? _a : 0,
                channelType: data === null || data === void 0 ? void 0 : data.channelType,
                domainId: data === null || data === void 0 ? void 0 : data.domainId,
                ext: (_b = parseInt(data === null || data === void 0 ? void 0 : data.ext, 10)) !== null && _b !== void 0 ? _b : 0
            };
            return yield axios_1.default.post((_c = process === null || process === void 0 ? void 0 : process.env) === null || _c === void 0 ? void 0 : _c.Sentimental, body);
        }
        catch (error) {
            console.error('Error sending message:', error);
            // You can handle the error here, e.g., log it or return a specific response
        }
    });
}
exports.sentimentalScoreDataFB = sentimentalScoreDataFB;
/**
 *
 * @param data
 * @returns
 */
const updateSentimentScoreFB = (data, sentiment) => {
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        var _a, _b, _c, _d, _e, _f, _g;
        try {
            db_1.mysqlPoolConnection.query('call ccaas_updateSentimentalScore(?,?,?,?,?,?,?,?)', [
                (_a = data === null || data === void 0 ? void 0 : data.historyId) !== null && _a !== void 0 ? _a : 0,
                (_b = data === null || data === void 0 ? void 0 : data.ChatDurationHistoryId) !== null && _b !== void 0 ? _b : 0,
                'Facebook',
                (_c = data === null || data === void 0 ? void 0 : data.domainId) !== null && _c !== void 0 ? _c : null,
                (_d = sentiment === null || sentiment === void 0 ? void 0 : sentiment.sentimentalScore) !== null && _d !== void 0 ? _d : null,
                (_e = sentiment === null || sentiment === void 0 ? void 0 : sentiment.sentimentalScoreUser) !== null && _e !== void 0 ? _e : null,
                (_f = sentiment === null || sentiment === void 0 ? void 0 : sentiment.sentimentalScoreAgent) !== null && _f !== void 0 ? _f : null,
                (_g = data === null || data === void 0 ? void 0 : data.ext) !== null && _g !== void 0 ? _g : null,
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
exports.updateSentimentScoreFB = updateSentimentScoreFB;
/**
 *
 * @param data
 * @returns
 */
const getWebchatLiveDashborad = (data) => {
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        var _a, _b, _c;
        try {
            db_1.mysqlPoolConnection.query('call ccaas_get_chat_dashboard (?,?,?,?)', [
                (_a = data.startTmpstmp) !== null && _a !== void 0 ? _a : null,
                (_b = data.endTmpstmp) !== null && _b !== void 0 ? _b : null,
                (_c = data.domainId) !== null && _c !== void 0 ? _c : null,
                parseInt(data.ext, 10) ? parseInt(data.ext, 10) : null
            ], (err, result) => {
                if (err) {
                    reject(err);
                }
                else {
                    if (result && result.length > 0) {
                        resolve(result);
                    }
                    else {
                        reject(err);
                    }
                }
            });
        }
        catch (err) {
            reject(err);
        }
    }));
};
exports.getWebchatLiveDashborad = getWebchatLiveDashborad;
/**
 *
 * @param data
 * @returns
 */
const updateFacebookMessage = (data) => {
    const message = JSON.stringify(data.message);
    const chat_id = parseInt(data === null || data === void 0 ? void 0 : data.FacebookHistoryId, 10);
    return new Promise((resolve, reject) => {
        try {
            db_1.mysqlPoolConnection.query('call up_InserUpdateFacebookMessage(?,?)', [chat_id !== null && chat_id !== void 0 ? chat_id : 0, message], (err, result) => {
                if (err) {
                    reject(err);
                }
                else {
                    resolve(result);
                }
            });
        }
        catch (error) {
            console.log(error);
        }
    });
};
exports.updateFacebookMessage = updateFacebookMessage;
/**
 *
 * @param data
 * @returns
 */
const insertUpdateChatIntentDao = (data) => {
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        try {
            db_1.mysqlPoolConnection.query('call up_ccaas_InsertOrUpdateChatIntent(?,?,?,?,?)', [
                data.IntentName,
                data.IntentCount,
                data.domainId,
                data.ChatFlowId,
                data.ChatHistoryId,
            ], (err, result) => {
                var _a;
                if (err) {
                    reject(err);
                    console.log(err);
                }
                if (result && ((_a = result[0]) === null || _a === void 0 ? void 0 : _a.length) > 0) {
                    resolve(result[0]);
                }
                else {
                    resolve([]);
                }
            });
        }
        catch (err) {
            reject(err);
        }
    }));
};
exports.insertUpdateChatIntentDao = insertUpdateChatIntentDao;
const getDurationHistoryIdDao = (data) => {
    return new Promise((resolve, reject) => {
        try {
            db_1.mysqlPoolConnection.query(`select CallDurationHistoryId from tb_CallDurationHistory where CallHistoryId = ${data === null || data === void 0 ? void 0 : data.HistoryId} and AgentId = ${data === null || data === void 0 ? void 0 : data.userId}`, (err, result) => {
                var _a;
                if (err) {
                    reject(err);
                }
                else {
                    resolve((_a = result[0]) === null || _a === void 0 ? void 0 : _a.CallDurationHistoryId);
                    console.log('result--->>', result);
                }
            });
        }
        catch (error) {
            console.log(error);
        }
    });
};
exports.getDurationHistoryIdDao = getDurationHistoryIdDao;
const insertUpdateAgentAssist = (data) => {
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        var _a, _b, _c, _d, _e, _f, _g;
        try {
            db_1.mysqlPoolConnection.query('call up_insert_update_chatAgentAssistMessage(?,?,?,?,?,?,?)', [
                (_a = parseInt(data === null || data === void 0 ? void 0 : data.HistoryId, 10)) !== null && _a !== void 0 ? _a : null,
                (data === null || data === void 0 ? void 0 : data.ChannelType) === 'Call' ? data === null || data === void 0 ? void 0 : data.CallDurationHistoryId : (_b = data === null || data === void 0 ? void 0 : data.DurationHistoryId) !== null && _b !== void 0 ? _b : null,
                (_c = data === null || data === void 0 ? void 0 : data.domainId) !== null && _c !== void 0 ? _c : null,
                (_d = JSON.stringify(data === null || data === void 0 ? void 0 : data.AssistMsg)) !== null && _d !== void 0 ? _d : null,
                (_e = data === null || data === void 0 ? void 0 : data.NoOfUsedAssistMsg) !== null && _e !== void 0 ? _e : null,
                (_f = data === null || data === void 0 ? void 0 : data.totalNoOfassist) !== null && _f !== void 0 ? _f : null,
                (_g = data === null || data === void 0 ? void 0 : data.ChannelType) !== null && _g !== void 0 ? _g : null
            ], (err, result) => {
                if (err) {
                    reject(err);
                    console.log(err);
                }
                else {
                    resolve(result);
                    console.log(result);
                }
            });
        }
        catch (err) {
            reject(err);
            console.log(err);
        }
    }));
};
exports.insertUpdateAgentAssist = insertUpdateAgentAssist;
/**
 *
 * @param data
 * @returns
 */
const UpdateCallAnsweredDurationDao = (data) => {
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        const answeredDuration = typeof data.answeredDuration === 'string' ? data.answeredDuration.trim() || 0 : data.answeredDuration || 0;
        try {
            db_1.mysqlPoolConnection.query('call ccaas_updateCallAnsweredDuration(?,?,?,?)', [
                data.CallHistoryId,
                data.domainId,
                data.AgentId,
                answeredDuration,
            ], (err, result) => {
                var _a;
                if (err) {
                    reject(err);
                    console.log(err);
                }
                if (result && ((_a = result[0]) === null || _a === void 0 ? void 0 : _a.length) > 0) {
                    resolve(result[0]);
                }
                else {
                    resolve([]);
                }
            });
        }
        catch (err) {
            reject(err);
        }
    }));
};
exports.UpdateCallAnsweredDurationDao = UpdateCallAnsweredDurationDao;
/**
 *
 * @param data
 * @returns
 */
const updateWhatsappChatMessage = (data) => {
    const message = JSON.stringify(data.message);
    const chat_id = parseInt(data === null || data === void 0 ? void 0 : data.historyId, 10);
    return new Promise((resolve, reject) => {
        try {
            db_1.mysqlPoolConnection.query('call up_InserUpdateWhatsappMessage(?,?)', [chat_id !== null && chat_id !== void 0 ? chat_id : 0, message], (err, result) => {
                if (err) {
                    reject(err);
                }
                else {
                    resolve(result);
                }
            });
        }
        catch (error) {
            console.log(error);
        }
    });
};
exports.updateWhatsappChatMessage = updateWhatsappChatMessage;
/**
 *
 * @param data
 * @returns
 */
const updateMissedChat = (data) => {
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        try {
            db_1.mysqlPoolConnection.query('call ccaas_update_socialMedia_missed_dtl(?,?,?)', [
                data === null || data === void 0 ? void 0 : data.domainId,
                data === null || data === void 0 ? void 0 : data.historyId,
                'Whatsapp'
            ], (err, result) => {
                if (err) {
                    console.log(err);
                    reject(err);
                }
                if (result && result.length > 0) {
                    resolve(result[0]);
                }
                else {
                    resolve([]);
                }
            });
        }
        catch (err) {
            console.log(err);
            reject(err);
        }
    }));
};
exports.updateMissedChat = updateMissedChat;
/**
 *
 * @param data
 * @returns
 */
const getCustomerCoId = (data) => {
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        try {
            db_1.mysqlPoolConnection.query('call ccaas_get_coid_by_customerNumber(?,?)', [
                data === null || data === void 0 ? void 0 : data.customerNumber,
                data === null || data === void 0 ? void 0 : data.domainId
            ], (err, result) => {
                if (err) {
                    console.log(err);
                    reject(err);
                }
                if (result && result.length > 0) {
                    resolve(result[0]);
                }
                else {
                    resolve([]);
                }
            });
        }
        catch (err) {
            console.log(err);
            reject(err);
        }
    }));
};
exports.getCustomerCoId = getCustomerCoId;
/**
 *
 * @param data
 * @returns
 */
const updateCallbackStatus = (data) => {
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        try {
            db_1.mysqlPoolConnection.query('CALL ccaas_update_whatsapp_callback_status(?,?)', [
                data === null || data === void 0 ? void 0 : data.domainId,
                data === null || data === void 0 ? void 0 : data.historyId,
            ], (err, result) => {
                if (err) {
                    console.log(err);
                    reject(err);
                }
                if (result && result.length > 0) {
                    resolve(result[0]);
                }
                else {
                    resolve([]);
                }
            });
        }
        catch (err) {
            console.log(err);
            reject(err);
        }
    }));
};
exports.updateCallbackStatus = updateCallbackStatus;
const audioMsg = (data) => __awaiter(void 0, void 0, void 0, function* () {
    try {
        return new Promise((resolve, reject) => {
            var _a, _b;
            if (data === null || data === void 0 ? void 0 : data.ext) {
                db_1.mysqlPoolConnection.query('CALL ccaas_call_create_message_details(?,?,?,?)', [
                    data === null || data === void 0 ? void 0 : data.domainId,
                    data === null || data === void 0 ? void 0 : data.sessionId,
                    (_a = data === null || data === void 0 ? void 0 : data.message) !== null && _a !== void 0 ? _a : null,
                    (_b = data === null || data === void 0 ? void 0 : data.ext) !== null && _b !== void 0 ? _b : null
                ], (err, response) => {
                    if (err) {
                        console.log('audioMsg reject error: ', err);
                        reject(err);
                    }
                    else {
                        resolve(response);
                    }
                });
            }
        });
    }
    catch (error) {
        console.log('audioMsg catch err: ', error);
    }
});
exports.audioMsg = audioMsg;
/**
 *
 * @param data
 * @returns
 */
const getFacebookConfigData = (data) => {
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        var _a, _b;
        try {
            db_1.mysqlPoolConnection.query('call up_CheckFacebookBot (?,?)', [
                (_a = data === null || data === void 0 ? void 0 : data.agentNumber) !== null && _a !== void 0 ? _a : null,
                (_b = data === null || data === void 0 ? void 0 : data.domainId) !== null && _b !== void 0 ? _b : null,
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
exports.getFacebookConfigData = getFacebookConfigData;
/**
 *
 * @param data
 * @returns
 */
const getCustomerCoIdFb = (data) => {
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        try {
            db_1.mysqlPoolConnection.query('call ccaas_get_coid_by_customerId(?,?,?)', [
                data === null || data === void 0 ? void 0 : data.customerNumber,
                data === null || data === void 0 ? void 0 : data.domainId,
                'Facebook'
            ], (err, result) => {
                if (err) {
                    console.log(err);
                    reject(err);
                }
                if (result && result.length > 0) {
                    resolve(result[0]);
                }
                else {
                    resolve([]);
                }
            });
        }
        catch (err) {
            console.log('getDispositinStatus catch err: ', err);
            reject(err);
        }
    }));
};
exports.getCustomerCoIdFb = getCustomerCoIdFb;
// send Button fb
function sendCallbackRequestFb(req, buttonLink) {
    return __awaiter(this, void 0, void 0, function* () {
        try {
            const body = {
                domainId: req === null || req === void 0 ? void 0 : req.domainId,
                customerId: req === null || req === void 0 ? void 0 : req.customerNumber,
                pageId: req === null || req === void 0 ? void 0 : req.agentNumber,
                postbackText: 'Our agents are currently not available. Please feel free to leave a message with a brief summary of your inquiry along with your contact information, or you can get in touch with us at a later time.',
                buttons: [
                    {
                        type: 'web_url',
                        url: `${process.env.API_URL}/${buttonLink}`,
                        title: 'callback',
                        webview_height_ratio: 'full'
                    }
                ]
            };
            return yield axios_1.default.post(process.env.postbackButton, body);
        }
        catch (error) {
            console.error('Error sending message:', error);
        }
    });
}
exports.sendCallbackRequestFb = sendCallbackRequestFb;
/**
 *
 * @param domainId
 * @returns
 */
const updateCallbackStatusFb = (data) => {
    return new Promise((resolve, reject) => {
        try {
            db_1.mysqlPoolConnection.query('call ccaas_update_facebook_callback_status(?,?)', [
                data === null || data === void 0 ? void 0 : data.domainId,
                data === null || data === void 0 ? void 0 : data.historyId
            ], (errors, res) => {
                var _a;
                if (errors) {
                    console.log(errors);
                    reject([]);
                }
                else {
                    if (((_a = res[0]) === null || _a === void 0 ? void 0 : _a.length) > 0) {
                        resolve(res[0]);
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
exports.updateCallbackStatusFb = updateCallbackStatusFb;
/**
 *
 * @param data
 * @returns
 */
const updateChatMessageFb = (data) => {
    const message = JSON.stringify(data.message);
    const chat_id = parseInt(data === null || data === void 0 ? void 0 : data.historyId, 10);
    return new Promise((resolve, reject) => {
        try {
            db_1.mysqlPoolConnection.query('call up_InserUpdateFacebookMessage(?,?)', [chat_id !== null && chat_id !== void 0 ? chat_id : 0, message], (err, result) => {
                if (err) {
                    reject(err);
                }
                else {
                    resolve(result);
                }
            });
        }
        catch (error) {
            console.log(error);
        }
    });
};
exports.updateChatMessageFb = updateChatMessageFb;
// send message fb
function sendMessageFb(req) {
    return __awaiter(this, void 0, void 0, function* () {
        try {
            const body = {
                domainId: req === null || req === void 0 ? void 0 : req.domainId,
                customerId: req === null || req === void 0 ? void 0 : req.customerNumber,
                pageId: req === null || req === void 0 ? void 0 : req.agentNumber,
                message: 'Our agents are currently not available. Please feel free to leave a message with a brief summary of your inquiry along with your contact information, or you can get in touch with us at a later time.'
            };
            const response = yield axios_1.default.post(process.env.FacebookOut, body);
            return body;
        }
        catch (error) {
            console.error('Error sending message:', error);
            // You can handle the error here, e.g., log it or return a specific response
        }
    });
}
exports.sendMessageFb = sendMessageFb;
/**
 *
 * @param data
 * @returns
 */
const updateMissedChatFb = (data) => {
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        try {
            db_1.mysqlPoolConnection.query('call ccaas_update_socialMedia_missed_dtl(?,?,?)', [
                data === null || data === void 0 ? void 0 : data.domainId,
                data === null || data === void 0 ? void 0 : data.historyId,
                'Facebook'
            ], (err, result) => {
                if (err) {
                    console.log(err);
                    reject(err);
                }
                if (result && result.length > 0) {
                    resolve(result[0]);
                }
                else {
                    resolve([]);
                }
            });
        }
        catch (err) {
            console.log(err);
            reject(err);
        }
    }));
};
exports.updateMissedChatFb = updateMissedChatFb;
/**
 *
 * @param data
 * @returns
 */
const insertUpdateVisitorDao = (data) => {
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        var _a, _b, _c, _d, _e, _f, _g, _h, _j;
        try {
            db_1.mysqlPoolConnection.query('call up_ccaas_InsertUpdateVisitorInsight(?,?,?,?,?,?,?,?,?,?)', [
                (_a = data === null || data === void 0 ? void 0 : data.visitorId) !== null && _a !== void 0 ? _a : null,
                (data === null || data === void 0 ? void 0 : data.domainId) || null,
                (_b = data === null || data === void 0 ? void 0 : data.customerId) !== null && _b !== void 0 ? _b : null,
                (_c = data === null || data === void 0 ? void 0 : data.sessionStartTime) !== null && _c !== void 0 ? _c : null,
                (_d = data === null || data === void 0 ? void 0 : data.sessionEndTimed) !== null && _d !== void 0 ? _d : null,
                (_e = data === null || data === void 0 ? void 0 : data.location) !== null && _e !== void 0 ? _e : null,
                (_f = data === null || data === void 0 ? void 0 : data.device) !== null && _f !== void 0 ? _f : null,
                (_g = data === null || data === void 0 ? void 0 : data.ipAddress) !== null && _g !== void 0 ? _g : null,
                (_h = data === null || data === void 0 ? void 0 : data.browser) !== null && _h !== void 0 ? _h : null,
                (_j = data === null || data === void 0 ? void 0 : data.language) !== null && _j !== void 0 ? _j : null
            ], (err, result) => {
                if (err) {
                    console.log(err);
                    reject(err);
                }
                if (result && result.length > 0) {
                    resolve(result);
                }
                else {
                    resolve([]);
                }
            });
        }
        catch (err) {
            console.log(err);
            reject(err);
        }
    }));
};
exports.insertUpdateVisitorDao = insertUpdateVisitorDao;
/**
 *
 * @param data
 * @returns
 */
const updateIsBot = (HistoryId) => {
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        try {
            db_1.mysqlPoolConnection.query('CALL ccaas_update_flow_bot_check(?)', [
                HistoryId
            ], (err, result) => {
                if (err) {
                    console.log(err);
                    reject(err);
                }
                if (result && result.length > 0) {
                    resolve(result[0]);
                }
                else {
                    resolve([]);
                }
            });
        }
        catch (err) {
            reject(err);
        }
    }));
};
exports.updateIsBot = updateIsBot;
/**
 *
 * @param data
 * @returns
 */
const updateQueueId = (data, queueId) => {
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        try {
            db_1.mysqlPoolConnection.query('CALL up_UpdateWhatsappQueueidBasedOnDomainId(?,?)', [
                data === null || data === void 0 ? void 0 : data.WhatsappHistoryId,
                queueId !== null && queueId !== void 0 ? queueId : null
            ], (err, result) => {
                if (err) {
                    console.log(err);
                    reject(err);
                }
                if (result && result.length > 0) {
                    resolve(result[0]);
                }
                else {
                    resolve([]);
                }
            });
        }
        catch (err) {
            reject(err);
        }
    }));
};
exports.updateQueueId = updateQueueId;
/**
 *
 * @param data
 * @returns
 */
const updateQueueIdFacebook = (data, queueId) => {
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        try {
            db_1.mysqlPoolConnection.query('CALL up_UpdateFacebookQueueidBasedOnDomainId(?,?)', [
                data === null || data === void 0 ? void 0 : data.FacebookHistoryId,
                queueId !== null && queueId !== void 0 ? queueId : null
            ], (err, result) => {
                if (err) {
                    console.log(err);
                    reject(err);
                }
                if (result && result.length > 0) {
                    resolve(result[0]);
                }
                else {
                    resolve([]);
                }
            });
        }
        catch (err) {
            reject(err);
        }
    }));
};
exports.updateQueueIdFacebook = updateQueueIdFacebook;
/**
 *
 * @param data
 * @returns
 */
const updateIsBotFb = (HistoryId) => {
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        try {
            db_1.mysqlPoolConnection.query('CALL ccaas_update_flow_bot_check_fb(?)', [
                HistoryId
            ], (err, result) => {
                if (err) {
                    console.log(err);
                    reject(err);
                }
                if (result && result.length > 0) {
                    resolve(result[0]);
                }
                else {
                    resolve([]);
                }
            });
        }
        catch (err) {
            reject(err);
        }
    }));
};
exports.updateIsBotFb = updateIsBotFb;
/**
 *
 * @param data
 * @returns
 */
const getCustomerConatctDetails = (domainId, coid) => {
    return new Promise((resolve, reject) => {
        try {
            if (coid === null && coid === undefined) {
                reject(new Error('coid cannot be null'));
            }
            else {
                db_1.mysqlPoolConnection.query('CALL ccaas_getCustomerConatctDetails(?,?)', [
                    coid !== null && coid !== void 0 ? coid : 0,
                    domainId
                ], (err, result) => {
                    if (err) {
                        console.log(err);
                        resolve(err);
                    }
                    else {
                        if (result && result.length > 0) {
                            resolve(result[0]);
                        }
                        else {
                            reject(err);
                        }
                    }
                });
            }
        }
        catch (err) {
            reject(err);
        }
    });
};
exports.getCustomerConatctDetails = getCustomerConatctDetails;
const getAllQueueWaitTime = (domainId) => {
    return new Promise((resolve, reject) => {
        try {
            db_1.mysqlPoolConnection.query('call ccaas_getAllQueueWaitTime_info(?)', [
                domainId
            ], (err, result) => {
                if (err) {
                    console.log('getAllQueueWaitTime err: ', err);
                    reject(err);
                }
                resolve(result[0]);
            });
        }
        catch (error) {
            console.log('getAllQueueWaitTime catch error: ', error);
            reject(error);
        }
    });
};
exports.getAllQueueWaitTime = getAllQueueWaitTime;
const insertUpdateQueueWaitTimeDao = (data) => {
    return new Promise((resolve, reject) => {
        var _a;
        try {
            db_1.mysqlPoolConnection.query('call ccaas_queue_Weigh_Time_details(?,?,?,?,?)', [
                data === null || data === void 0 ? void 0 : data.qid,
                data === null || data === void 0 ? void 0 : data.queueName,
                data === null || data === void 0 ? void 0 : data.queueType,
                (_a = data === null || data === void 0 ? void 0 : data.domainId) !== null && _a !== void 0 ? _a : 0,
                data === null || data === void 0 ? void 0 : data.queueList
            ], (err, result) => {
                if (err) {
                    reject(err);
                }
                db_1.ioredisChat.emit('chatQueuePosition', data);
                resolve(result);
            });
        }
        catch (error) {
            reject(error);
        }
    });
};
exports.insertUpdateQueueWaitTimeDao = insertUpdateQueueWaitTimeDao;
/**
 *
 * @param data
 * @returns
 */
const insertUpdateQueueWaitTimeWhatsapp = (data) => {
    return new Promise((resolve, reject) => {
        var _a;
        try {
            db_1.mysqlPoolConnection.query('call ccaas_queue_Weigh_Time_details(?,?,?,?,?)', [
                data === null || data === void 0 ? void 0 : data.qid,
                data === null || data === void 0 ? void 0 : data.queueName,
                data === null || data === void 0 ? void 0 : data.queueType,
                (_a = data === null || data === void 0 ? void 0 : data.domainId) !== null && _a !== void 0 ? _a : 0,
                data === null || data === void 0 ? void 0 : data.queueList
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
exports.insertUpdateQueueWaitTimeWhatsapp = insertUpdateQueueWaitTimeWhatsapp;
/**
 * @description get queue wait time
 * @param data
 * @returns
 */
const getQueueWaitTime = (data) => {
    return new Promise((resolve, reject) => {
        try {
            const mysqlq = `select * from queueWeighTime where qid = '${data.qid}' AND domainId = ${data.domainId} AND queueType='chat'`;
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
exports.getQueueWaitTime = getQueueWaitTime;
/**
 * @description get queue wait time
 * @param data
 * @returns
 */
const getQueueWaitTimeWhatsapp = (data) => {
    return new Promise((resolve, reject) => {
        try {
            const mysqlq = `select * from queueWeighTime where qid = '${data.qid}' AND domainId = ${data.domainId} AND queueType='whatsapp'`;
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
exports.getQueueWaitTimeWhatsapp = getQueueWaitTimeWhatsapp;
/**
 * @description get queue wait time
 * @param data
 * @returns
 */
const getQueueWaitTimeFacebook = (data) => {
    return new Promise((resolve, reject) => {
        try {
            const mysqlq = `select * from queueWeighTime where qid = '${data.qid}' AND domainId = ${data.domainId} AND queueType='facebook'`;
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
exports.getQueueWaitTimeFacebook = getQueueWaitTimeFacebook;
/**
 *
 * @param data
 * @returns
 */
const getAgentAssistMessageDtl = (data) => {
    return new Promise((resolve, reject) => {
        var _a, _b, _c;
        try {
            db_1.mysqlPoolConnection.query('call ccaas_get_agentAssistMessage_dtl(?,?,?)', [
                (_a = data === null || data === void 0 ? void 0 : data.HistoryId) !== null && _a !== void 0 ? _a : null,
                (_b = data === null || data === void 0 ? void 0 : data.DurationHistoryId) !== null && _b !== void 0 ? _b : null,
                (_c = data === null || data === void 0 ? void 0 : data.domainId) !== null && _c !== void 0 ? _c : null,
            ], (err, result) => {
                console.log('result>>>', result);
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
exports.getAgentAssistMessageDtl = getAgentAssistMessageDtl;
/**
 * @description get queue wait time
 * @param data
 * @returns
 */
const getQueueWaitTimeInsta = (data) => {
    return new Promise((resolve, reject) => {
        try {
            const mysqlq = `select * from queueWeighTime where qid = '${data.qid}' AND domainId = ${data.domainId} AND queueType='instagram'`;
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
exports.getQueueWaitTimeInsta = getQueueWaitTimeInsta;
/**
 *
 * @param data
 * @returns
 */
const updateAssistMessage = (data) => {
    return new Promise((resolve, reject) => {
        var _a, _b, _c, _d, _e, _f;
        try {
            db_1.mysqlPoolConnection.query('call update_assistMsg(?,?,?,?,?,?)', [
                (_a = data === null || data === void 0 ? void 0 : data.historyId) !== null && _a !== void 0 ? _a : null,
                (_b = data === null || data === void 0 ? void 0 : data.durationHistoryId) !== null && _b !== void 0 ? _b : null,
                (_c = data === null || data === void 0 ? void 0 : data.domainId) !== null && _c !== void 0 ? _c : null,
                (_d = data === null || data === void 0 ? void 0 : data.assistMsg) !== null && _d !== void 0 ? _d : null,
                (_e = data === null || data === void 0 ? void 0 : data.noOfUsedAssistMsg) !== null && _e !== void 0 ? _e : null,
                (_f = data === null || data === void 0 ? void 0 : data.totalNoOfassist) !== null && _f !== void 0 ? _f : null
            ], (err, result) => {
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
exports.updateAssistMessage = updateAssistMessage;
//# sourceMappingURL=webChat.dao.js.map