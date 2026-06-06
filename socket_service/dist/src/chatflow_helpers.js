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
exports.mergeNestedObjects = exports.getParticularAgentChatListBtnForQueue = exports.getAvaiableAgentChatListBtnForQueue = exports.insertChatSession = exports.checkAvailableAgents = exports.checkAgentStatus = exports.getAvailableAgentChatList = exports.getAvaiableAgentChatListBtn = exports.sendQueueUser = exports.updateSessionDetails = exports.deleteSessionDetails = exports.handleTransferToLiveAgentToQueue = exports.handleTransferToLiveAgent = exports.handleTransferToQueue = exports.replaceContextWithVariables = exports.replacechatPatternWithVariables = exports.replacePatternWithVariables = exports.contextPattern = void 0;
const lodash_1 = __importDefault(require("lodash"));
const moment_1 = __importDefault(require("moment"));
const callflow_dao_1 = require("./dao/callflow.dao");
const chat_dao_1 = require("./dao/chat.dao");
const db_1 = require("./plugins/db");
const log_1 = require("./plugins/log");
exports.contextPattern = /\${(.*?)}/g;
const replacePatternWithVariables = (availableContexts, text, context) => {
    let originalText = text;
    for (const ctx of availableContexts) {
        const ctxString = ctx.toString();
        const variableName = ctxString.slice(2, ctxString.length - 1);
        const variable = variableName.split('.')[1];
        const specialPattern = /[${}]/g;
        originalText = originalText.replace(specialPattern, '');
        const patternString = `\\bcontext.${variable}\\b`; // Creating pattern with a dynamic string
        const pattern = new RegExp(patternString, 'gi');
        originalText = originalText === null || originalText === void 0 ? void 0 : originalText.replace(pattern, context[variable]);
    }
    return originalText;
};
exports.replacePatternWithVariables = replacePatternWithVariables;
const replacechatPatternWithVariables = (availableContexts, text, context) => {
    let originalText = text;
    for (const ctx of availableContexts) {
        const ctxString = ctx.toString();
        const variableName = ctxString.slice(2, ctxString.length - 1);
        const variable = variableName.split('.')[1];
        const specialPattern = /[${}]/g;
        originalText = originalText.replace(specialPattern, '');
        const patternString = `\\bcontext.${variable}\\b`; // Creating pattern with a dynamic string
        const pattern = new RegExp(patternString, 'gi');
        /* tslint:disable */
        const test = lodash_1.default.find(context, function (o) { return o.originalkey === variable; });
        console.log(test, 'pattern');
        // originalText = originalText?.replace(pattern, test[variable]);
        originalText = (test === null || test === void 0 ? void 0 : test.originalkey) === "registerdate" ? originalText === null || originalText === void 0 ? void 0 : originalText.replace(pattern, (0, moment_1.default)(new Date(test[variable])).format('YYYY-MM-DD'))
            : test !== undefined ? originalText === null || originalText === void 0 ? void 0 : originalText.replace(pattern, test[variable]) : text;
    }
    return originalText;
};
exports.replacechatPatternWithVariables = replacechatPatternWithVariables;
const replaceContextWithVariables = (text, context) => {
    const originalText = text;
    const variableName = originalText === null || originalText === void 0 ? void 0 : originalText.slice(2, (originalText === null || originalText === void 0 ? void 0 : originalText.length) - 1);
    const variable = variableName.split('.')[1];
    if (context[variable]) {
        return context[variable];
    }
    return variable;
};
exports.replaceContextWithVariables = replaceContextWithVariables;
const handleTransferToQueue = (adaptor, channel, targetNode, agentDetails) => __awaiter(void 0, void 0, void 0, function* () {
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        var _a, _b, _c, _d, _e, _f, _g, _h, _j, _k, _l, _m, _o, _p, _q, _r, _s, _t, _u, _v;
        try {
            const queueSettings = yield (0, callflow_dao_1.getQueueSettings)((_b = (_a = targetNode.data) === null || _a === void 0 ? void 0 : _a.information) === null || _b === void 0 ? void 0 : _b.transferTo);
            const validateHours = true; // await getHoursofOperation(targetNode.data?.information?.transferTo);
            // const agentCount: any = await getAgentCount(targetNode.data?.information?.transferTo);
            const mostSuitableAgent = agentDetails;
            targetNode.data.information.agentDetials = mostSuitableAgent;
            const skillName = (mostSuitableAgent === null || mostSuitableAgent === void 0 ? void 0 : mostSuitableAgent.add_skill) ? (_e = (_d = JSON.parse((_c = mostSuitableAgent === null || mostSuitableAgent === void 0 ? void 0 : mostSuitableAgent.add_skill) !== null && _c !== void 0 ? _c : 'null')) === null || _d === void 0 ? void 0 : _d.at(0)) === null || _e === void 0 ? void 0 : _e.value : '';
            // await chatUpdateDao({ ...channel, skillName, domainId: channel?.domain_id, sessionId: channel?.session_id, agentName: mostSuitableAgent?.name });
            const queueData = Object.assign(Object.assign({}, queueSettings), { settingAudio: JSON.parse((_f = queueSettings === null || queueSettings === void 0 ? void 0 : queueSettings.settingAudio) !== null && _f !== void 0 ? _f : 'null'), announcement: JSON.parse((_g = queueSettings === null || queueSettings === void 0 ? void 0 : queueSettings.announcement) !== null && _g !== void 0 ? _g : 'null'), maximumCallers: JSON.parse((_h = queueSettings === null || queueSettings === void 0 ? void 0 : queueSettings.maximumCallers) !== null && _h !== void 0 ? _h : 'null'), maximumWaitTime: JSON.parse((_j = queueSettings === null || queueSettings === void 0 ? void 0 : queueSettings.maximumWaitTime) !== null && _j !== void 0 ? _j : 'null'), outOfBusinessHours: JSON.parse((_k = queueSettings === null || queueSettings === void 0 ? void 0 : queueSettings.outOfBusinessHours) !== null && _k !== void 0 ? _k : 'null') });
            targetNode.data.information.queueSettings = queueData;
            targetNode.data.information.queueSettings.hoursOfOperation = validateHours ? 1 : 0;
            targetNode.data.information.queueSettings.agentCount = (_l = agentDetails === null || agentDetails === void 0 ? void 0 : agentDetails.length) !== null && _l !== void 0 ? _l : 0;
            const targetRestructured = Object.assign(Object.assign({}, targetNode), { session_id: channel === null || channel === void 0 ? void 0 : channel.session_id, data: Object.assign({}, (_m = targetNode === null || targetNode === void 0 ? void 0 : targetNode.data) === null || _m === void 0 ? void 0 : _m.information), req_type: 'QUEUE_CONNECT' });
            (_p = (_o = adaptor === null || adaptor === void 0 ? void 0 : adaptor.io) === null || _o === void 0 ? void 0 : _o.of('/')) === null || _p === void 0 ? void 0 : _p.to(channel === null || channel === void 0 ? void 0 : channel.session_id).emit('availableagent', JSON.stringify(targetRestructured));
            log_1.logger.info(`availableagent Socket Error: ${JSON.stringify(targetRestructured)}`);
            // adaptor.emitMessage(channel?.session_id, 'get_queue_details', { message: targetRestructured });
            // if (channel?.channelType === 'Chat') {
            //   //adaptor.publishMessageToWebChat(channel?.domain_id, JSON.stringify(targetRestructured));
            //   adaptor.emitMessage(channel?.session_id, 'get_queue_details', { message: targetRestructured });
            // } else {
            //   adaptor.publishMessage(JSON.stringify(targetRestructured));
            // }
            const nextNode = (_s = (_r = (_q = targetNode === null || targetNode === void 0 ? void 0 : targetNode.data) === null || _q === void 0 ? void 0 : _q.children) === null || _r === void 0 ? void 0 : _r.find((node) => (node === null || node === void 0 ? void 0 : node.type) === 'success')) !== null && _s !== void 0 ? _s : { childId: '' };
            resolve(nextNode === null || nextNode === void 0 ? void 0 : nextNode.childId);
        }
        catch (error) {
            console.log(error);
            const nextNode = (_v = (_u = (_t = targetNode === null || targetNode === void 0 ? void 0 : targetNode.data) === null || _t === void 0 ? void 0 : _t.children) === null || _u === void 0 ? void 0 : _u.find((node) => (node === null || node === void 0 ? void 0 : node.type) === 'error')) !== null && _v !== void 0 ? _v : { childId: '' };
            reject(nextNode === null || nextNode === void 0 ? void 0 : nextNode.childId);
        }
    }));
});
exports.handleTransferToQueue = handleTransferToQueue;
const handleTransferToLiveAgent = (adaptor, channel, targetNode, agentDetails) => __awaiter(void 0, void 0, void 0, function* () {
    console.log('handleTransferToLiveAgent agentDetails', agentDetails);
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        var _w, _x;
        try {
            const mostSuitableAgent = { agentDetials: agentDetails };
            const data = { data: mostSuitableAgent };
            (_x = (_w = adaptor === null || adaptor === void 0 ? void 0 : adaptor.io) === null || _w === void 0 ? void 0 : _w.of('/')) === null || _x === void 0 ? void 0 : _x.to(channel === null || channel === void 0 ? void 0 : channel.session_id).emit('availableagent', JSON.stringify(data));
            log_1.logger.info(`availableagent Socket Error: ${JSON.stringify(data)}`);
        }
        catch (error) {
            console.log(error);
        }
    }));
});
exports.handleTransferToLiveAgent = handleTransferToLiveAgent;
const handleTransferToLiveAgentToQueue = (channel, targetNode, agentDetails) => __awaiter(void 0, void 0, void 0, function* () {
    console.log('handleTransferToLiveAgentToQueue agentDetails', agentDetails, 'channel', channel);
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        try {
            const mostSuitableAgent = { agentDetials: agentDetails };
            const data = { data: mostSuitableAgent };
            db_1.ioredisChat === null || db_1.ioredisChat === void 0 ? void 0 : db_1.ioredisChat.to(channel === null || channel === void 0 ? void 0 : channel.session_id).emit('availableagent', JSON.stringify(data));
            log_1.logger.info(`availableagent Socket Error: ${JSON.stringify(data)}`);
            resolve([]);
        }
        catch (error) {
            console.log(error);
        }
    }));
});
exports.handleTransferToLiveAgentToQueue = handleTransferToLiveAgentToQueue;
const deleteSessionDetails = (data) => __awaiter(void 0, void 0, void 0, function* () {
    var _y, _z;
    try {
        let queueArray = [];
        let agentUserId = '';
        const queueId = yield (0, chat_dao_1.getAllUserList)(data);
        queueId.map((list) => {
            if (data.ext == list.ext) {
                agentUserId = list.UserID;
                queueArray = (list.quId).split(",");
            }
        });
        let flag = false;
        queueArray.map((val) => __awaiter(void 0, void 0, void 0, function* () {
            var _0, _1;
            if (flag)
                return;
            const getIncomingChatDetails = yield (0, callflow_dao_1.getIncomingChatDetailsDB)(data, val);
            console.log('updateSessionDetails getIncomingChatDetails', getIncomingChatDetails);
            const getIncomingChats = JSON.parse((_1 = (_0 = getIncomingChatDetails[0]) === null || _0 === void 0 ? void 0 : _0.queueList) !== null && _1 !== void 0 ? _1 : '[]');
            // console.log('updateSessionDetails getIncomingChats', getIncomingChats);
            if (getIncomingChats.length > 0) {
                const temp = getIncomingChats.filter((val) => val.session_id != data.to);
                getIncomingChatDetails[0].queueList = temp;
                const insertIncomingChats = yield (0, callflow_dao_1.insertIncomingChatsDB)(getIncomingChatDetails[0], val);
                if (insertIncomingChats[0].errCode == 0) {
                    console.log('Deleted Successfully!!!');
                }
                // getIncomingChats?.map(async (value: any) => {
                //   // console.log('updateSessionDetails value', value, 'data', data);
                //   if (value?.session_id == data.to) {
                //     temp.push({ ...value, sentToAgent: 0 });
                //     // console.log('updateSessionDetails getIncomingChatDetails[0]', getIncomingChatDetails[0]);
                //     // Assuming insertIncomingChatsDB returns a Promise
                //       // console.log('updateSessionDetails insertIncomingChats if entered', insertIncomingChats[0].errCode);
                //       flag = true;
                //       sendQueueUser(data);
                //       // flag = true
                //     }
                //   }
                // });
            }
        }));
        if (!flag) {
            const getIncomingChatDetailsUsingUserID = yield (0, callflow_dao_1.getIncomingChatDetailsDB)(data, agentUserId);
            const getIncomingChatsUsingUserID = JSON.parse((_z = (_y = getIncomingChatDetailsUsingUserID[0]) === null || _y === void 0 ? void 0 : _y.queueList) !== null && _z !== void 0 ? _z : '[]');
            if (getIncomingChatsUsingUserID.length > 0) {
                const temp = [];
                yield Promise.all(getIncomingChatsUsingUserID === null || getIncomingChatsUsingUserID === void 0 ? void 0 : getIncomingChatsUsingUserID.map((value) => __awaiter(void 0, void 0, void 0, function* () {
                    if ((value === null || value === void 0 ? void 0 : value.session_id) == data.to) {
                        temp.push(Object.assign(Object.assign({}, value), { sentToAgent: 0 }));
                        getIncomingChatDetailsUsingUserID[0].queueList = temp;
                        const insertIncomingChatsUsingUserID = yield (0, callflow_dao_1.insertIncomingChatsDB)(getIncomingChatDetailsUsingUserID[0], agentUserId);
                        console.log('updateSessionDetails insertIncomingChatsUsingUserID', insertIncomingChatsUsingUserID);
                        if (insertIncomingChatsUsingUserID[0].errCode == 0) {
                            // console.log('updateSessionDetails insertIncomingChatsUsingUserID if entered', insertIncomingChatsUsingUserID[0].errCode);
                            (0, exports.sendQueueUser)(data);
                        }
                    }
                })));
            }
        }
    }
    catch (error) {
        console.log('error', error);
        // reject(error);
    }
    // });
});
exports.deleteSessionDetails = deleteSessionDetails;
const updateSessionDetails = (data) => __awaiter(void 0, void 0, void 0, function* () {
    var _2, _3;
    try {
        let queueArray = [];
        let agentUserId = '';
        const queueId = yield (0, chat_dao_1.getAllUserList)(data);
        queueId.map((list) => {
            if (data.ext == list.ext) {
                agentUserId = list.UserID;
                queueArray = (list.quId).split(",");
            }
        });
        let flag = false;
        queueArray.map((val) => __awaiter(void 0, void 0, void 0, function* () {
            var _4, _5;
            if (flag)
                return;
            const getIncomingChatDetails = yield (0, callflow_dao_1.getIncomingChatDetailsDB)(data, val);
            console.log('updateSessionDetails getIncomingChatDetails', getIncomingChatDetails);
            const getIncomingChats = JSON.parse((_5 = (_4 = getIncomingChatDetails[0]) === null || _4 === void 0 ? void 0 : _4.queueList) !== null && _5 !== void 0 ? _5 : '[]');
            if (getIncomingChats.length > 0) {
                const temp = [];
                getIncomingChats === null || getIncomingChats === void 0 ? void 0 : getIncomingChats.map((value) => __awaiter(void 0, void 0, void 0, function* () {
                    if ((value === null || value === void 0 ? void 0 : value.session_id) == data.to) {
                        temp.push(Object.assign(Object.assign({}, value), { sentToAgent: 0 }));
                        getIncomingChatDetails[0].queueList = temp;
                        // Assuming insertIncomingChatsDB returns a Promise
                        const insertIncomingChats = yield (0, callflow_dao_1.insertIncomingChatsDB)(getIncomingChatDetails[0], val);
                        if (insertIncomingChats[0].errCode == 0) {
                            flag = true;
                            (0, exports.sendQueueUser)(data);
                            // flag = true
                        }
                    }
                }));
            }
        }));
        if (!flag) {
            const getIncomingChatDetailsUsingUserID = yield (0, callflow_dao_1.getIncomingChatDetailsDB)(data, agentUserId);
            console.log('updateSessionDetails getIncomingChatDetailsUsingUserID', getIncomingChatDetailsUsingUserID);
            const getIncomingChatsUsingUserID = JSON.parse((_3 = (_2 = getIncomingChatDetailsUsingUserID[0]) === null || _2 === void 0 ? void 0 : _2.queueList) !== null && _3 !== void 0 ? _3 : '[]');
            if (getIncomingChatsUsingUserID.length > 0) {
                const temp = [];
                yield Promise.all(getIncomingChatsUsingUserID === null || getIncomingChatsUsingUserID === void 0 ? void 0 : getIncomingChatsUsingUserID.map((value) => __awaiter(void 0, void 0, void 0, function* () {
                    if ((value === null || value === void 0 ? void 0 : value.session_id) == data.to) {
                        temp.push(Object.assign(Object.assign({}, value), { sentToAgent: 0 }));
                        getIncomingChatDetailsUsingUserID[0].queueList = temp;
                        const insertIncomingChatsUsingUserID = yield (0, callflow_dao_1.insertIncomingChatsDB)(getIncomingChatDetailsUsingUserID[0], agentUserId);
                        if (insertIncomingChatsUsingUserID[0].errCode == 0) {
                            (0, exports.sendQueueUser)(data);
                        }
                    }
                })));
            }
        }
    }
    catch (error) {
        console.log('error', error);
        // reject(error);
    }
    // });
});
exports.updateSessionDetails = updateSessionDetails;
const sendQueueUser = (data) => __awaiter(void 0, void 0, void 0, function* () {
    var _6, _7;
    try {
        let queueArray = [];
        let agentUserId = '';
        const queueId = yield (0, chat_dao_1.getAllUserList)(data);
        // console.log('sendQueueUser queueId', queueId)
        queueId.map((list) => {
            if (data.ext == list.ext) {
                agentUserId = list.UserID;
                queueArray = (list.quId).split(",");
            }
        });
        let flag = false;
        queueArray.map((val) => __awaiter(void 0, void 0, void 0, function* () {
            var _8, _9;
            if (flag)
                return;
            const getIncomingChatDetails = yield (0, callflow_dao_1.getIncomingChatDetailsDB)(data, val);
            console.log('sendQueueUser getIncomingChatDetails', getIncomingChatDetails);
            const getIncomingChats = JSON.parse((_9 = (_8 = getIncomingChatDetails[0]) === null || _8 === void 0 ? void 0 : _8.queueList) !== null && _9 !== void 0 ? _9 : '[]');
            if (getIncomingChats.length > 0) {
                getIncomingChats.map((ct) => __awaiter(void 0, void 0, void 0, function* () {
                    if (!(ct === null || ct === void 0 ? void 0 : ct.sentToAgent)) {
                        if (data === null || data === void 0 ? void 0 : data.declined) {
                            ct.ext = data === null || data === void 0 ? void 0 : data.ext;
                        }
                        let agentDetails = [];
                        agentDetails = yield (0, exports.getAvaiableAgentChatListBtnForQueue)(ct);
                        if (agentDetails && (agentDetails === null || agentDetails === void 0 ? void 0 : agentDetails.length) > 0) {
                            const filteredChats = getIncomingChats.filter((chat) => chat.session_id != ct.session_id);
                            ct.sentToAgent = 1;
                            const insertChats = [...filteredChats, ct];
                            getIncomingChatDetails[0].queueList = insertChats;
                            const updateSession = yield (0, callflow_dao_1.insertIncomingChatsDB)(getIncomingChatDetails[0], val);
                            console.log('sendQueueUser updateSession', updateSession);
                            if (updateSession[0].errCode == 0) {
                                agentDetails.queueId = val;
                                flag = true;
                                yield (0, exports.handleTransferToLiveAgentToQueue)(ct, val, agentDetails);
                                // await handleTransferToLiveAgentToQueue2(ct,val, agentDetails);
                            }
                        }
                    }
                }));
            }
        }));
        if (!flag) {
            const getIncomingChatDetailsUsingUserID = yield (0, callflow_dao_1.getIncomingChatDetailsDB)(data, agentUserId);
            const getIncomingChatsUsingUserID = JSON.parse((_7 = (_6 = getIncomingChatDetailsUsingUserID[0]) === null || _6 === void 0 ? void 0 : _6.queueList) !== null && _7 !== void 0 ? _7 : '[]');
            if (getIncomingChatsUsingUserID.length > 0) {
                getIncomingChatsUsingUserID.map((ct) => __awaiter(void 0, void 0, void 0, function* () {
                    if (!(ct === null || ct === void 0 ? void 0 : ct.sentToAgent)) {
                        if (data === null || data === void 0 ? void 0 : data.declined) {
                            ct.ext = data === null || data === void 0 ? void 0 : data.ext;
                        }
                        let agentDetails = [];
                        data.domain_id = data.domainId;
                        data.queueId = agentUserId;
                        const checkAgent = yield (0, exports.getParticularAgentChatListBtnForQueue)(data);
                        const filterParticularAgent = checkAgent.filter((val) => val.ext == data.ext);
                        if (filterParticularAgent.length > 0) {
                            const filteredChats = getIncomingChatsUsingUserID.filter((chat) => chat.session_id != ct.session_id);
                            ct.sentToAgent = 1;
                            const insertChats = [...filteredChats, ct];
                            getIncomingChatDetailsUsingUserID[0].queueList = insertChats;
                            const updateSession = yield (0, callflow_dao_1.insertIncomingChatsDB)(getIncomingChatDetailsUsingUserID[0], agentUserId);
                            console.log('sendQueueUser updateSession', updateSession);
                            if (updateSession[0].errCode == 0) {
                                filterParticularAgent[0].queueId = agentUserId;
                                agentDetails = [...filterParticularAgent];
                                flag = true;
                                yield (0, exports.handleTransferToLiveAgentToQueue)(ct, agentUserId, agentDetails);
                                // await handleTransferToLiveAgentToQueue2(ct,val, agentDetails);
                            }
                        }
                    }
                }));
            }
        }
    }
    catch (err) {
        console.log('sendQueueUser err', err);
    }
    // const incomingChat = getIncomingChats?.filter((val: any) => {
    //   return val?.session_id == channelId.session_id
    // });
    // if (incomingChat.length > 0) {
    //   channelId.queueList = [...getIncomingChats];
    // } else {
    //   channelId.queueList = [...getIncomingChats, JSON.parse(JSON.stringify({ ...channelId, currentNode: currentNode }))];
    // }
    // let insertIncomingChats: any = await insertIncomingChatsDB(channelId, currentNode.data?.information?.transferTo);
    // let agentDetails: any = [];
    // agentDetails = await getAvaiableAgentChatListBtn(channelId, currentNode.data?.information?.transferTo);
    // if (agentDetails && agentDetails?.length > 0) {
    //   agentDetails.queueId = currentNode.data?.information?.transferTo;
    //   await handleTransferToLiveAgent(ioredisChat, channelId, currentNode, agentDetails);
    // }
});
exports.sendQueueUser = sendQueueUser;
const getAvaiableAgentChatListBtn = (channel, qid) => __awaiter(void 0, void 0, void 0, function* () {
    let agentDetails = [];
    return new Promise((resolve) => {
        db_1.otherRedisClient.hgetall(`${channel === null || channel === void 0 ? void 0 : channel.domain_id}_presence`).then((data) => __awaiter(void 0, void 0, void 0, function* () {
            var _a;
            const result = Object.values(data).map((value) => JSON.parse(value));
            // Getting routing profiles mapped with qid
            const routingProfiles = yield (0, callflow_dao_1.getRoutingProfile)(qid);
            const routingProfilesId = [];
            routingProfiles.map((val) => val === null || val === void 0 ? void 0 : val.rqid);
            let filteredData = [];
            if ((channel === null || channel === void 0 ? void 0 : channel.ext) !== undefined && (channel === null || channel === void 0 ? void 0 : channel.ext) !== null && (channel === null || channel === void 0 ? void 0 : channel.ext) !== '') {
                const agentFilter = result.filter((item) => (item.statusName === 'Ready' && (item.ext !== channel.ext) && parseInt(item.roleId, 10) === 6 && routingProfiles.includes(item.routingProfileId)));
                if ((agentFilter === null || agentFilter === void 0 ? void 0 : agentFilter.length) === 0) {
                    const supFilter = result.filter((item) => (item.statusName === 'Ready' && (item.ext !== channel.ext) && parseInt(item.roleId, 10) === 5));
                    filteredData = (supFilter === null || supFilter === void 0 ? void 0 : supFilter.length) !== 0 ? supFilter : [];
                }
                else {
                    filteredData = agentFilter;
                }
            }
            else {
                const agentFilter = result.filter((item) => (item.statusName === 'Ready' && parseInt(item.roleId, 10) === 6 && routingProfiles.includes(item.routingProfileId)));
                if ((agentFilter === null || agentFilter === void 0 ? void 0 : agentFilter.length) === 0) {
                    const supFilter = result.filter((item) => (item.statusName === 'Ready' && parseInt(item.roleId, 10) === 5));
                    filteredData = (supFilter === null || supFilter === void 0 ? void 0 : supFilter.length) !== 0 ? supFilter : [];
                }
                else {
                    filteredData = agentFilter;
                }
            }
            if ((filteredData === null || filteredData === void 0 ? void 0 : filteredData.length) === 0) {
                filteredData = result.filter((item) => (item.statusName === 'Ready' && (item.ext !== channel.ext) && parseInt(item.roleId, 10) === 3));
            }
            const chatLimitByAgent = yield (0, callflow_dao_1.getAgentByChat)(channel.domain_id);
            const chatLimitData = yield (0, callflow_dao_1.getConcurrentChat)(channel.domain_id);
            const RoutingProfileUser = yield (0, callflow_dao_1.getRoutingProfileUser)(channel.domain_id);
            const parsedChatLimitData = JSON.parse((_a = chatLimitData === null || chatLimitData === void 0 ? void 0 : chatLimitData.agentOccupyDetails) !== null && _a !== void 0 ? _a : 'null') || null;
            const currentChatCount = parsedChatLimitData === null || parsedChatLimitData === void 0 ? void 0 : parsedChatLimitData.currentChatCount;
            if (currentChatCount !== null && currentChatCount !== undefined) {
                const appendChatCountToFilteredData = filteredData.map((dt) => {
                    for (let [key, value] of Object.entries(currentChatCount)) {
                        if (dt.ext == key) {
                            dt = Object.assign(Object.assign({}, dt), { 'currentChatCount': value });
                            break;
                        }
                        else {
                            dt = Object.assign(Object.assign({}, dt), { 'currentChatCount': 0 });
                        }
                    }
                    return dt;
                });
                const sortedAgents = appendChatCountToFilteredData.sort((a, b) => a.currentTime < b.currentTime);
                let chatLimitAchievedAgents = [];
                let agentsToBeAssigned = [...sortedAgents];
                if ((chatLimitData === null || chatLimitData === void 0 ? void 0 : chatLimitData.isAllAgent) || (chatLimitData === null || chatLimitData === void 0 ? void 0 : chatLimitData.isAgent) || (chatLimitData === null || chatLimitData === void 0 ? void 0 : chatLimitData.isRoutingProfile)) {
                    let tempAgent;
                    const checkLiveAgent = () => {
                        if (tempAgent !== undefined) {
                            if (chatLimitData === null || chatLimitData === void 0 ? void 0 : chatLimitData.isAllAgent) {
                                if ((tempAgent === null || tempAgent === void 0 ? void 0 : tempAgent.currentChatCount) < (chatLimitData === null || chatLimitData === void 0 ? void 0 : chatLimitData.allAgentLimit)) {
                                    agentDetails.push(tempAgent);
                                }
                                else {
                                    if (chatLimitAchievedAgents.length !== sortedAgents.length) {
                                        chatLimitAchievedAgents.push(tempAgent);
                                        agentsToBeAssigned = agentsToBeAssigned.filter((val) => val.ext !== tempAgent.ext);
                                        assignFunction();
                                    }
                                }
                            }
                            else if (chatLimitData === null || chatLimitData === void 0 ? void 0 : chatLimitData.isAgent) {
                                chatLimitByAgent.filter((list) => {
                                    if ((tempAgent === null || tempAgent === void 0 ? void 0 : tempAgent.currentChatCount) <= (list.chatLimit - 1) && `${tempAgent.ext}` === `${list.ext}`) {
                                        agentDetails.push(tempAgent);
                                    }
                                });
                            }
                            else if (chatLimitData === null || chatLimitData === void 0 ? void 0 : chatLimitData.isRoutingProfile) {
                                const routings = {};
                                RoutingProfileUser.map((item) => {
                                    routings[item.RoutingProfileId] = item.concurrentChat;
                                });
                                for (let [key, value] of Object.entries(routings)) {
                                    if (tempAgent.routingProfileId == key) {
                                        if (tempAgent.currentChatCount == value) {
                                            chatLimitAchievedAgents.push(tempAgent);
                                            agentsToBeAssigned = agentsToBeAssigned.filter((val) => val.ext !== tempAgent.ext);
                                            if (chatLimitAchievedAgents.length !== sortedAgents.length) {
                                                assignFunction();
                                            }
                                        }
                                        else {
                                            agentDetails.push(tempAgent);
                                        }
                                    }
                                }
                            }
                        }
                    };
                    const assignFunction = () => {
                        if (agentsToBeAssigned.length > 0) {
                            tempAgent = agentsToBeAssigned.reduce((oldest, current) => {
                                if (current.currentChatCount < oldest.currentChatCount) {
                                    return current;
                                }
                                else if (current.currentChatCount === oldest.currentChatCount && current.currentTime < oldest.currentTime) {
                                    return current;
                                }
                                else {
                                    return oldest;
                                }
                            }, agentsToBeAssigned[0]);
                            checkLiveAgent();
                        }
                    };
                    assignFunction();
                }
                else {
                    let tempAgent;
                    const checkLiveAgent = () => {
                        if (tempAgent !== undefined) {
                            if (tempAgent.currentChatCount < 1) {
                                agentDetails.push(tempAgent);
                            }
                            else {
                                if (chatLimitAchievedAgents.length < agentsToBeAssigned.length) {
                                    chatLimitAchievedAgents.push(tempAgent);
                                    agentsToBeAssigned = agentsToBeAssigned.filter((val) => val.ext !== tempAgent.ext);
                                    assignFunction();
                                }
                            }
                        }
                    };
                    const assignFunction = () => {
                        tempAgent = agentsToBeAssigned.reduce((oldest, current) => {
                            if (current.currentChatCount < oldest.currentChatCount) {
                                return current;
                            }
                            else if (current.currentChatCount === oldest.currentChatCount && current.currentTime < oldest.currentTime) {
                                return current;
                            }
                            else {
                                return oldest;
                            }
                        }, agentsToBeAssigned[0]);
                        checkLiveAgent();
                    };
                    assignFunction();
                }
            }
            else {
                const oldestReadyItem = yield filteredData.reduce((oldest, current) => {
                    return current.currentTime < oldest.currentTime ? current : oldest;
                }, filteredData[0]);
                if (oldestReadyItem !== undefined) {
                    agentDetails.push(oldestReadyItem);
                }
                else {
                    agentDetails = [];
                }
            }
            resolve(agentDetails);
        }));
    });
});
exports.getAvaiableAgentChatListBtn = getAvaiableAgentChatListBtn;
const getAvailableAgentChatList = (channel) => __awaiter(void 0, void 0, void 0, function* () {
    let agentDetails = [];
    return new Promise((resolve) => {
        db_1.otherRedisClient.hgetall(`${channel === null || channel === void 0 ? void 0 : channel.domain_id}_presence`).then((data) => __awaiter(void 0, void 0, void 0, function* () {
            var _a;
            const result = Object.values(data).map((value) => JSON.parse(value));
            let filteredData = [];
            if ((channel === null || channel === void 0 ? void 0 : channel.ext) !== undefined && (channel === null || channel === void 0 ? void 0 : channel.ext) !== null && (channel === null || channel === void 0 ? void 0 : channel.ext) !== '') {
                const agentFilter = result.filter((item) => (item.statusName === 'Ready' && (item.ext !== channel.ext) && parseInt(item.roleId, 10) === 6));
                if ((agentFilter === null || agentFilter === void 0 ? void 0 : agentFilter.length) === 0) {
                    const supFilter = result.filter((item) => (item.statusName === 'Ready' && (item.ext !== channel.ext) && parseInt(item.roleId, 10) === 5));
                    filteredData = (supFilter === null || supFilter === void 0 ? void 0 : supFilter.length) !== 0 ? supFilter : [];
                }
                else {
                    filteredData = agentFilter;
                }
            }
            else {
                const agentFilter = result.filter((item) => (item.statusName === 'Ready' && parseInt(item.roleId, 10) === 6));
                if ((agentFilter === null || agentFilter === void 0 ? void 0 : agentFilter.length) === 0) {
                    const supFilter = result.filter((item) => (item.statusName === 'Ready' && parseInt(item.roleId, 10) === 5));
                    filteredData = (supFilter === null || supFilter === void 0 ? void 0 : supFilter.length) !== 0 ? supFilter : [];
                }
                else {
                    filteredData = agentFilter;
                }
            }
            if ((filteredData === null || filteredData === void 0 ? void 0 : filteredData.length) === 0) {
                filteredData = result.filter((item) => (item.statusName === 'Ready' && (item.ext !== channel.ext) && parseInt(item.roleId, 10) === 3));
            }
            const chatLimitByAgent = yield (0, callflow_dao_1.getAgentByChat)(channel.domain_id);
            const chatLimitData = yield (0, callflow_dao_1.getConcurrentChat)(channel.domain_id);
            console.log("chatLimitData>>>>>>>>>>>>>>>>", chatLimitData);
            const RoutingProfileUser = yield (0, callflow_dao_1.getRoutingProfileUser)(channel.domain_id);
            const parsedChatLimitData = JSON.parse((_a = chatLimitData === null || chatLimitData === void 0 ? void 0 : chatLimitData.agentOccupyDetails) !== null && _a !== void 0 ? _a : 'null') || null;
            const currentChatCount = parsedChatLimitData === null || parsedChatLimitData === void 0 ? void 0 : parsedChatLimitData.currentChatCount;
            if (currentChatCount !== null && currentChatCount !== undefined) {
                const appendChatCountToFilteredData = filteredData.map((dt) => {
                    for (let [key, value] of Object.entries(currentChatCount)) {
                        if (dt.ext == key) {
                            dt = Object.assign(Object.assign({}, dt), { 'currentChatCount': value });
                            break;
                        }
                        else {
                            dt = Object.assign(Object.assign({}, dt), { 'currentChatCount': 0 });
                        }
                    }
                    return dt;
                });
                const sortedAgents = appendChatCountToFilteredData.sort((a, b) => a.currentTime < b.currentTime);
                // console.log('sortedAgents', sortedAgents);
                let chatLimitAchievedAgents = [];
                let agentsToBeAssigned = [...sortedAgents];
                if ((chatLimitData === null || chatLimitData === void 0 ? void 0 : chatLimitData.isAllAgent) || (chatLimitData === null || chatLimitData === void 0 ? void 0 : chatLimitData.isAgent) || (chatLimitData === null || chatLimitData === void 0 ? void 0 : chatLimitData.isRoutingProfile)) {
                    let tempAgent;
                    const checkLiveAgent = () => {
                        if (tempAgent !== undefined) {
                            if (chatLimitData === null || chatLimitData === void 0 ? void 0 : chatLimitData.isAllAgent) {
                                if ((tempAgent === null || tempAgent === void 0 ? void 0 : tempAgent.currentChatCount) < (chatLimitData === null || chatLimitData === void 0 ? void 0 : chatLimitData.allAgentLimit)) {
                                    agentDetails.push(tempAgent);
                                }
                                else {
                                    if (chatLimitAchievedAgents.length !== sortedAgents.length) {
                                        chatLimitAchievedAgents.push(tempAgent);
                                        agentsToBeAssigned = agentsToBeAssigned.filter((val) => val.ext !== tempAgent.ext);
                                        assignFunction();
                                    }
                                }
                            }
                            else if (chatLimitData === null || chatLimitData === void 0 ? void 0 : chatLimitData.isAgent) {
                                chatLimitByAgent.filter((list) => {
                                    if ((tempAgent === null || tempAgent === void 0 ? void 0 : tempAgent.currentChatCount) <= (list.chatLimit - 1) && `${tempAgent.ext}` === `${list.ext}`) {
                                        agentDetails.push(tempAgent);
                                    }
                                });
                            }
                            else if (chatLimitData === null || chatLimitData === void 0 ? void 0 : chatLimitData.isRoutingProfile) {
                                const routings = {};
                                RoutingProfileUser.map((item) => {
                                    routings[item.RoutingProfileId] = item.concurrentChat;
                                });
                                for (let [key, value] of Object.entries(routings)) {
                                    if (tempAgent.routingProfileId == key) {
                                        if (tempAgent.currentChatCount == value) {
                                            chatLimitAchievedAgents.push(tempAgent);
                                            agentsToBeAssigned = agentsToBeAssigned.filter((val) => val.ext !== tempAgent.ext);
                                            if (chatLimitAchievedAgents.length !== sortedAgents.length) {
                                                assignFunction();
                                            }
                                        }
                                        else {
                                            // console.log('else');
                                            agentDetails.push(tempAgent);
                                        }
                                    }
                                }
                            }
                        }
                    };
                    const assignFunction = () => {
                        if (agentsToBeAssigned.length > 0) {
                            tempAgent = agentsToBeAssigned.reduce((oldest, current) => {
                                if (current.currentChatCount < oldest.currentChatCount) {
                                    return current;
                                }
                                else if (current.currentChatCount === oldest.currentChatCount && current.currentTime < oldest.currentTime) {
                                    return current;
                                }
                                else {
                                    return oldest;
                                }
                            }, agentsToBeAssigned[0]);
                            checkLiveAgent();
                        }
                    };
                    assignFunction();
                }
                else {
                    let tempAgent;
                    const checkLiveAgent = () => {
                        if (tempAgent !== undefined) {
                            if (tempAgent.currentChatCount < 1) {
                                agentDetails.push(tempAgent);
                            }
                            else {
                                if (chatLimitAchievedAgents.length < agentsToBeAssigned.length) {
                                    chatLimitAchievedAgents.push(tempAgent);
                                    agentsToBeAssigned = agentsToBeAssigned.filter((val) => val.ext !== tempAgent.ext);
                                    assignFunction();
                                }
                            }
                        }
                    };
                    const assignFunction = () => {
                        tempAgent = agentsToBeAssigned.reduce((oldest, current) => {
                            if (current.currentChatCount < oldest.currentChatCount) {
                                return current;
                            }
                            else if (current.currentChatCount === oldest.currentChatCount && current.currentTime < oldest.currentTime) {
                                return current;
                            }
                            else {
                                return oldest;
                            }
                        }, agentsToBeAssigned[0]);
                        checkLiveAgent();
                    };
                    assignFunction();
                }
            }
            else {
                const oldestReadyItem = yield filteredData.reduce((oldest, current) => {
                    return current.currentTime < oldest.currentTime ? current : oldest;
                }, filteredData[0]);
                if (oldestReadyItem !== undefined) {
                    agentDetails.push(oldestReadyItem);
                }
                else {
                    agentDetails = [];
                }
            }
            resolve(agentDetails);
        }));
    });
});
exports.getAvailableAgentChatList = getAvailableAgentChatList;
const checkAgentStatus = (channel) => __awaiter(void 0, void 0, void 0, function* () {
    let agentFilter = [];
    const data = yield db_1.otherRedisClient.hgetall(`${channel === null || channel === void 0 ? void 0 : channel.domain_id}_presence`);
    const result = Object.values(data).map((value) => JSON.parse(value));
    agentFilter = result.filter((item) => (item.statusName === 'Ready' && item.userId == channel.queueId));
    console.log('agentFilter checkAgentStatus------------>', agentFilter);
    return agentFilter;
});
exports.checkAgentStatus = checkAgentStatus;
const checkAvailableAgents = (channel) => __awaiter(void 0, void 0, void 0, function* () {
    var _10;
    let agentFilter = [];
    let filteredData = [];
    const data = yield db_1.otherRedisClient.hgetall(`${channel === null || channel === void 0 ? void 0 : channel.domain_id}_presence`);
    const result = Object.values(data).map((value) => JSON.parse(value));
    // Getting routing profiles mapped with qid
    const routingProfiles = yield (0, callflow_dao_1.getRoutingProfile)(channel.queueId);
    // let filteredData: any = [];
    // let agentFilter: any = [];
    agentFilter = result.filter((item) => (item.statusName === 'Ready' && parseInt(item.roleId, 10) === 6 && routingProfiles.includes(item.routingProfileId)));
    if ((agentFilter === null || agentFilter === void 0 ? void 0 : agentFilter.length) === 0) {
        const supFilter = result.filter((item) => (item.statusName === 'Ready' && parseInt(item.roleId, 10) === 5));
        agentFilter = (supFilter === null || supFilter === void 0 ? void 0 : supFilter.length) !== 0 ? supFilter : [];
    }
    if ((agentFilter === null || agentFilter === void 0 ? void 0 : agentFilter.length) === 0) {
        agentFilter = result.filter((item) => (item.statusName === 'Ready' && parseInt(item.roleId, 10) === 3));
    }
    const getAgentSkills = yield (0, callflow_dao_1.getAgentsSkills)(channel.domain_id, '1,2,3,5,6');
    console.log('getAgentSkills------------->', getAgentSkills);
    let skilledAgents = [];
    (_10 = channel === null || channel === void 0 ? void 0 : channel.skills) === null || _10 === void 0 ? void 0 : _10.map((nodeSkill) => {
        getAgentSkills === null || getAgentSkills === void 0 ? void 0 : getAgentSkills.map((agent) => {
            var _a;
            if (agent === null || agent === void 0 ? void 0 : agent.addSkill) {
                (_a = JSON.parse(agent.addSkill)) === null || _a === void 0 ? void 0 : _a.map((val) => {
                    if (nodeSkill == (val === null || val === void 0 ? void 0 : val.value)) {
                        skilledAgents.push(agent);
                    }
                });
            }
        });
    });
    const extValues = skilledAgents.map((agent) => agent.ext);
    const filteredSkilledAgents = agentFilter.filter((agent) => extValues.includes(agent.ext) || extValues.includes(+(agent.ext)));
    let filter = [...filteredSkilledAgents];
    console.log('filter after skill filtering----------->', filter);
    if (filter.length > 1) {
        filteredData = filter.filter((vl) => vl.ext != (channel === null || channel === void 0 ? void 0 : channel.ext));
    }
    else {
        filteredData = filter;
    }
    console.log('filteredData after ext filtering----------->', filteredData);
    return filteredData;
});
exports.checkAvailableAgents = checkAvailableAgents;
const insertChatSession = (channelId, currentNode) => __awaiter(void 0, void 0, void 0, function* () {
    var _11, _12, _13, _14, _15, _16;
    const getIncomingChatDetails = yield (0, callflow_dao_1.getIncomingChatDetailsDB)(channelId, (_12 = (_11 = currentNode.data) === null || _11 === void 0 ? void 0 : _11.information) === null || _12 === void 0 ? void 0 : _12.transferTo);
    const getIncomingChats = JSON.parse((_14 = (_13 = getIncomingChatDetails[0]) === null || _13 === void 0 ? void 0 : _13.queueList) !== null && _14 !== void 0 ? _14 : '[]');
    const incomingChat = getIncomingChats === null || getIncomingChats === void 0 ? void 0 : getIncomingChats.filter((val) => {
        return (val === null || val === void 0 ? void 0 : val.session_id) == channelId.session_id;
    });
    // console.log('channelId-------------------->', channelId);
    if (incomingChat.length > 0) {
        channelId.queueList = [...getIncomingChats];
    }
    else {
        channelId.queueList = [...getIncomingChats, JSON.parse(JSON.stringify(channelId))];
    }
    const insert = yield (0, callflow_dao_1.insertIncomingChatsDB)(channelId, (_16 = (_15 = currentNode.data) === null || _15 === void 0 ? void 0 : _15.information) === null || _16 === void 0 ? void 0 : _16.transferTo);
    // console.log('insert', insert);
    return JSON.parse(JSON.stringify(insert));
});
exports.insertChatSession = insertChatSession;
const getAvaiableAgentChatListBtnForQueue = (channel) => __awaiter(void 0, void 0, void 0, function* () {
    console.log('getAvaiableAgentChatListBtnForQueue channel--------------->', channel);
    try {
        let agentDetails = [];
        return new Promise((resolve) => {
            db_1.otherRedisClient.hgetall(`${channel === null || channel === void 0 ? void 0 : channel.domain_id}_presence`).then((data) => __awaiter(void 0, void 0, void 0, function* () {
                var _a, _b;
                console.log('getAvaiableAgentChatListBtnForQueue data----------->', data);
                const result = Object.values(data).map((value) => JSON.parse(value));
                const routingProfiles = yield (0, callflow_dao_1.getRoutingProfile)(channel.queueId);
                let filteredData = [];
                let agentFilter = [];
                agentFilter = result.filter((item) => (item.statusName === 'Ready' && parseInt(item.roleId, 10) === 6 && routingProfiles.includes(item.routingProfileId)));
                if ((agentFilter === null || agentFilter === void 0 ? void 0 : agentFilter.length) === 0) {
                    const supFilter = result.filter((item) => (item.statusName === 'Ready' && parseInt(item.roleId, 10) === 5));
                    agentFilter = (supFilter === null || supFilter === void 0 ? void 0 : supFilter.length) !== 0 ? supFilter : [];
                }
                if ((agentFilter === null || agentFilter === void 0 ? void 0 : agentFilter.length) === 0) {
                    agentFilter = result.filter((item) => (item.statusName === 'Ready' && parseInt(item.roleId, 10) === 3));
                }
                const getAgentSkills = yield (0, callflow_dao_1.getAgentsSkills)(channel.domain_id, '1,2,3,5,6');
                let skilledAgents = [];
                (_a = channel === null || channel === void 0 ? void 0 : channel.skills) === null || _a === void 0 ? void 0 : _a.map((nodeSkill) => {
                    getAgentSkills === null || getAgentSkills === void 0 ? void 0 : getAgentSkills.map((agent) => {
                        var _a;
                        if (agent === null || agent === void 0 ? void 0 : agent.addSkill) {
                            (_a = JSON.parse(agent.addSkill)) === null || _a === void 0 ? void 0 : _a.map((val) => {
                                if (nodeSkill == (val === null || val === void 0 ? void 0 : val.value)) {
                                    skilledAgents.push(agent);
                                }
                            });
                        }
                    });
                });
                const extValues = skilledAgents.map((agent) => agent.ext);
                const filteredSkilledAgents = agentFilter.filter((agent) => extValues.includes(agent.ext) || extValues.includes(+(agent.ext)));
                let filter = [...filteredSkilledAgents];
                if (filter.length > 1) {
                    filteredData = filter.filter((vl) => vl.ext != (channel === null || channel === void 0 ? void 0 : channel.ext));
                }
                else {
                    filteredData = filter;
                }
                const chatLimitByAgent = yield (0, callflow_dao_1.getAgentByChat)(channel.domain_id);
                const chatLimitData = yield (0, callflow_dao_1.getConcurrentChat)(channel.domain_id);
                console.log("chatLimitData==================", chatLimitData);
                const RoutingProfileUser = yield (0, callflow_dao_1.getRoutingProfileUser)(channel.domain_id);
                const parsedChatLimitData = JSON.parse((_b = chatLimitData === null || chatLimitData === void 0 ? void 0 : chatLimitData.agentOccupyDetails) !== null && _b !== void 0 ? _b : 'null') || null;
                const currentChatCount = parsedChatLimitData === null || parsedChatLimitData === void 0 ? void 0 : parsedChatLimitData.currentChatCount;
                if (currentChatCount !== null && currentChatCount !== undefined) {
                    const appendChatCountToFilteredData = filteredData.map((dt) => {
                        for (let [key, value] of Object.entries(currentChatCount)) {
                            if (dt.ext == key) {
                                dt = Object.assign(Object.assign({}, dt), { 'currentChatCount': value });
                                break;
                            }
                            else {
                                dt = Object.assign(Object.assign({}, dt), { 'currentChatCount': 0 });
                            }
                        }
                        return dt;
                    });
                    const sortedAgents = appendChatCountToFilteredData.sort((a, b) => a.currentTime < b.currentTime);
                    let chatLimitAchievedAgents = [];
                    let agentsToBeAssigned = [...sortedAgents];
                    if ((chatLimitData === null || chatLimitData === void 0 ? void 0 : chatLimitData.isAllAgent) || (chatLimitData === null || chatLimitData === void 0 ? void 0 : chatLimitData.isAgent) || (chatLimitData === null || chatLimitData === void 0 ? void 0 : chatLimitData.isRoutingProfile)) {
                        let tempAgent;
                        const checkLiveAgent = () => {
                            if (tempAgent !== undefined) {
                                if (chatLimitData === null || chatLimitData === void 0 ? void 0 : chatLimitData.isAllAgent) {
                                    if ((tempAgent === null || tempAgent === void 0 ? void 0 : tempAgent.currentChatCount) < (chatLimitData === null || chatLimitData === void 0 ? void 0 : chatLimitData.allAgentLimit)) {
                                        agentDetails.push(tempAgent);
                                    }
                                    else {
                                        if (chatLimitAchievedAgents.length !== sortedAgents.length) {
                                            chatLimitAchievedAgents.push(tempAgent);
                                            agentsToBeAssigned = agentsToBeAssigned.filter((val) => val.ext !== tempAgent.ext);
                                            assignFunction();
                                        }
                                    }
                                }
                                else if (chatLimitData === null || chatLimitData === void 0 ? void 0 : chatLimitData.isAgent) {
                                    chatLimitByAgent.filter((list) => {
                                        if ((tempAgent === null || tempAgent === void 0 ? void 0 : tempAgent.currentChatCount) <= (list.chatLimit - 1) && `${tempAgent.ext}` === `${list.ext}`) {
                                            agentDetails.push(tempAgent);
                                        }
                                    });
                                }
                                else if (chatLimitData === null || chatLimitData === void 0 ? void 0 : chatLimitData.isRoutingProfile) {
                                    const routings = {};
                                    RoutingProfileUser.map((item) => {
                                        routings[item.RoutingProfileId] = item.concurrentChat;
                                    });
                                    for (let [key, value] of Object.entries(routings)) {
                                        if (tempAgent.routingProfileId == key) {
                                            if (tempAgent.currentChatCount == value) {
                                                chatLimitAchievedAgents.push(tempAgent);
                                                agentsToBeAssigned = agentsToBeAssigned.filter((val) => val.ext !== tempAgent.ext);
                                                if (chatLimitAchievedAgents.length !== sortedAgents.length) {
                                                    assignFunction();
                                                }
                                            }
                                            else {
                                                agentDetails.push(tempAgent);
                                            }
                                        }
                                    }
                                }
                            }
                        };
                        const assignFunction = () => {
                            if (agentsToBeAssigned.length > 0) {
                                tempAgent = agentsToBeAssigned.reduce((oldest, current) => {
                                    if (current.currentChatCount < oldest.currentChatCount) {
                                        return current;
                                    }
                                    else if (current.currentChatCount === oldest.currentChatCount && current.currentTime < oldest.currentTime) {
                                        return current;
                                    }
                                    else {
                                        return oldest;
                                    }
                                }, agentsToBeAssigned[0]);
                                checkLiveAgent();
                            }
                        };
                        assignFunction();
                    }
                    else {
                        let tempAgent;
                        const checkLiveAgent = () => {
                            if (tempAgent !== undefined) {
                                if (tempAgent.currentChatCount < 1) {
                                    agentDetails.push(tempAgent);
                                }
                                else {
                                    if (chatLimitAchievedAgents.length < agentsToBeAssigned.length) {
                                        chatLimitAchievedAgents.push(tempAgent);
                                        agentsToBeAssigned = agentsToBeAssigned.filter((val) => val.ext !== tempAgent.ext);
                                        assignFunction();
                                    }
                                }
                            }
                        };
                        const assignFunction = () => {
                            tempAgent = agentsToBeAssigned.reduce((oldest, current) => {
                                if (current.currentChatCount < oldest.currentChatCount) {
                                    return current;
                                }
                                else if (current.currentChatCount === oldest.currentChatCount && current.currentTime < oldest.currentTime) {
                                    return current;
                                }
                                else {
                                    return oldest;
                                }
                            }, agentsToBeAssigned[0]);
                            checkLiveAgent();
                        };
                        assignFunction();
                    }
                }
                else {
                    const oldestReadyItem = yield filteredData.reduce((oldest, current) => {
                        return current.currentTime < oldest.currentTime ? current : oldest;
                    }, filteredData[0]);
                    if (oldestReadyItem !== undefined) {
                        agentDetails.push(oldestReadyItem);
                    }
                    else {
                        agentDetails = [];
                    }
                }
                resolve(agentDetails);
            }));
        });
    }
    catch (err) {
        console.log('getAvaiableAgentChatListBtnForQueue err', err);
    }
});
exports.getAvaiableAgentChatListBtnForQueue = getAvaiableAgentChatListBtnForQueue;
const getParticularAgentChatListBtnForQueue = (channel) => __awaiter(void 0, void 0, void 0, function* () {
    try {
        let agentDetails = [];
        return new Promise((resolve) => {
            db_1.otherRedisClient.hgetall(`${channel === null || channel === void 0 ? void 0 : channel.domain_id}_presence`).then((data) => __awaiter(void 0, void 0, void 0, function* () {
                var _a;
                console.log('getAvaiableAgentChatListBtnForQueue data----------->', data);
                const result = Object.values(data).map((value) => JSON.parse(value));
                // console.log('getAvaiableAgentChatListBtnForQueue result----------->', result);
                // Getting routing profiles mapped with qid
                // const routingProfiles: any = await getRoutingProfile(channel.queueId);
                // console.log('routingProfiles----------->', routingProfiles);
                let filteredData = [];
                let agentFilter = [];
                agentFilter = result.filter((item) => (item.statusName === 'Ready' && parseInt(item.roleId, 10) === 6));
                if ((agentFilter === null || agentFilter === void 0 ? void 0 : agentFilter.length) === 0) {
                    const supFilter = result.filter((item) => (item.statusName === 'Ready' && parseInt(item.roleId, 10) === 5));
                    agentFilter = (supFilter === null || supFilter === void 0 ? void 0 : supFilter.length) !== 0 ? supFilter : [];
                }
                if ((agentFilter === null || agentFilter === void 0 ? void 0 : agentFilter.length) === 0) {
                    agentFilter = result.filter((item) => (item.statusName === 'Ready' && parseInt(item.roleId, 10) === 3));
                }
                console.log('agentFilter--------------->', agentFilter);
                if (agentFilter.length > 1) {
                    filteredData = agentFilter.filter((vl) => vl.ext != (channel === null || channel === void 0 ? void 0 : channel.ext));
                }
                else {
                    filteredData = agentFilter;
                }
                // console.log('filteredData after ext filtering----------->', filteredData);
                const chatLimitByAgent = yield (0, callflow_dao_1.getAgentByChat)(channel.domain_id);
                const chatLimitData = yield (0, callflow_dao_1.getConcurrentChat)(channel.domain_id);
                const RoutingProfileUser = yield (0, callflow_dao_1.getRoutingProfileUser)(channel.domain_id);
                const parsedChatLimitData = JSON.parse((_a = chatLimitData === null || chatLimitData === void 0 ? void 0 : chatLimitData.agentOccupyDetails) !== null && _a !== void 0 ? _a : 'null') || null;
                const currentChatCount = parsedChatLimitData === null || parsedChatLimitData === void 0 ? void 0 : parsedChatLimitData.currentChatCount;
                if (currentChatCount !== null && currentChatCount !== undefined) {
                    const appendChatCountToFilteredData = filteredData.map((dt) => {
                        for (let [key, value] of Object.entries(currentChatCount)) {
                            if (dt.ext == key) {
                                dt = Object.assign(Object.assign({}, dt), { 'currentChatCount': value });
                                break;
                            }
                            else {
                                dt = Object.assign(Object.assign({}, dt), { 'currentChatCount': 0 });
                            }
                        }
                        return dt;
                    });
                    const sortedAgents = appendChatCountToFilteredData.sort((a, b) => a.currentTime < b.currentTime);
                    let chatLimitAchievedAgents = [];
                    let agentsToBeAssigned = [...sortedAgents];
                    if ((chatLimitData === null || chatLimitData === void 0 ? void 0 : chatLimitData.isAllAgent) || (chatLimitData === null || chatLimitData === void 0 ? void 0 : chatLimitData.isAgent) || (chatLimitData === null || chatLimitData === void 0 ? void 0 : chatLimitData.isRoutingProfile)) {
                        let tempAgent;
                        const checkLiveAgent = () => {
                            if (tempAgent !== undefined) {
                                if (chatLimitData === null || chatLimitData === void 0 ? void 0 : chatLimitData.isAllAgent) {
                                    if ((tempAgent === null || tempAgent === void 0 ? void 0 : tempAgent.currentChatCount) < (chatLimitData === null || chatLimitData === void 0 ? void 0 : chatLimitData.allAgentLimit)) {
                                        agentDetails.push(tempAgent);
                                    }
                                    else {
                                        if (chatLimitAchievedAgents.length !== sortedAgents.length) {
                                            chatLimitAchievedAgents.push(tempAgent);
                                            agentsToBeAssigned = agentsToBeAssigned.filter((val) => val.ext !== tempAgent.ext);
                                            assignFunction();
                                        }
                                    }
                                }
                                else if (chatLimitData === null || chatLimitData === void 0 ? void 0 : chatLimitData.isAgent) {
                                    chatLimitByAgent.filter((list) => {
                                        if ((tempAgent === null || tempAgent === void 0 ? void 0 : tempAgent.currentChatCount) <= (list.chatLimit - 1) && `${tempAgent.ext}` === `${list.ext}`) {
                                            agentDetails.push(tempAgent);
                                        }
                                    });
                                }
                                else if (chatLimitData === null || chatLimitData === void 0 ? void 0 : chatLimitData.isRoutingProfile) {
                                    const routings = {};
                                    RoutingProfileUser.map((item) => {
                                        routings[item.RoutingProfileId] = item.concurrentChat;
                                    });
                                    for (let [key, value] of Object.entries(routings)) {
                                        if (tempAgent.routingProfileId == key) {
                                            if (tempAgent.currentChatCount == value) {
                                                chatLimitAchievedAgents.push(tempAgent);
                                                agentsToBeAssigned = agentsToBeAssigned.filter((val) => val.ext !== tempAgent.ext);
                                                if (chatLimitAchievedAgents.length !== sortedAgents.length) {
                                                    assignFunction();
                                                }
                                            }
                                            else {
                                                agentDetails.push(tempAgent);
                                            }
                                        }
                                    }
                                }
                            }
                        };
                        const assignFunction = () => {
                            if (agentsToBeAssigned.length > 0) {
                                tempAgent = agentsToBeAssigned.reduce((oldest, current) => {
                                    if (current.currentChatCount < oldest.currentChatCount) {
                                        return current;
                                    }
                                    else if (current.currentChatCount === oldest.currentChatCount && current.currentTime < oldest.currentTime) {
                                        return current;
                                    }
                                    else {
                                        return oldest;
                                    }
                                }, agentsToBeAssigned[0]);
                                checkLiveAgent();
                            }
                        };
                        assignFunction();
                    }
                    else {
                        let tempAgent;
                        const checkLiveAgent = () => {
                            if (tempAgent !== undefined) {
                                if (tempAgent.currentChatCount < 1) {
                                    agentDetails.push(tempAgent);
                                }
                                else {
                                    if (chatLimitAchievedAgents.length < agentsToBeAssigned.length) {
                                        chatLimitAchievedAgents.push(tempAgent);
                                        agentsToBeAssigned = agentsToBeAssigned.filter((val) => val.ext !== tempAgent.ext);
                                        assignFunction();
                                    }
                                }
                            }
                        };
                        const assignFunction = () => {
                            tempAgent = agentsToBeAssigned.reduce((oldest, current) => {
                                if (current.currentChatCount < oldest.currentChatCount) {
                                    return current;
                                }
                                else if (current.currentChatCount === oldest.currentChatCount && current.currentTime < oldest.currentTime) {
                                    return current;
                                }
                                else {
                                    return oldest;
                                }
                            }, agentsToBeAssigned[0]);
                            checkLiveAgent();
                        };
                        assignFunction();
                    }
                }
                else {
                    const oldestReadyItem = yield filteredData.reduce((oldest, current) => {
                        return current.currentTime < oldest.currentTime ? current : oldest;
                    }, filteredData[0]);
                    if (oldestReadyItem !== undefined) {
                        agentDetails.push(oldestReadyItem);
                    }
                    else {
                        agentDetails = [];
                    }
                }
                resolve(agentDetails);
            }));
        });
    }
    catch (err) {
        console.log('getAvaiableAgentChatListBtnForQueue err', err);
    }
});
exports.getParticularAgentChatListBtnForQueue = getParticularAgentChatListBtnForQueue;
const mergeNestedObjects = (obj) => {
    let result = {};
    Object.entries(obj).forEach(([key, value]) => {
        if (typeof value === 'object' && value !== null) {
            result = Object.assign(Object.assign({}, result), (0, exports.mergeNestedObjects)(value));
        }
        else {
            result[key] = value;
        }
    });
    return result;
};
exports.mergeNestedObjects = mergeNestedObjects;
//# sourceMappingURL=chatflow_helpers.js.map