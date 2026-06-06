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
Object.defineProperty(exports, "__esModule", { value: true });
exports.blpopQueuedCustomerByStatus = exports.chatQueueByAgentStatusHandler = exports.chatQueueHandler = void 0;
const chatflow_helpers_1 = require("../chatflow_helpers");
const callflow_dao_1 = require("../dao/callflow.dao");
const webChat_dao_1 = require("../dao/webChat.dao");
const transferToQueue_1 = require("../helperFunctions/transferToQueue");
const constants_1 = require("../helpers/constants");
const db_1 = require("../plugins/db");
const chatQueueHandler = () => {
    try {
        setInterval(() => __awaiter(void 0, void 0, void 0, function* () {
            var _a, _b, _c, _e, _g, _h, _j, _k, _m, _o, _p, _q, _r, _s, _t, _u, _v, _w, _x, _y, _z, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12;
            const redisQueueData = yield db_1.queueGetter.get('queuedCustomer');
            if (redisQueueData) {
                const redisData = JSON.parse(redisQueueData);
                if ((redisData === null || redisData === void 0 ? void 0 : redisData.length) > 0) {
                    // redisData?.map(async (list: any) => {
                    for (const list of redisData) {
                        const getQueueList = yield (0, webChat_dao_1.getQueueWaitTime)(list);
                        const queueResult = typeof ((_a = getQueueList === null || getQueueList === void 0 ? void 0 : getQueueList[0]) === null || _a === void 0 ? void 0 : _a.queueList) === 'string'
                            ? (() => {
                                var _a;
                                try {
                                    return JSON.parse((_a = getQueueList === null || getQueueList === void 0 ? void 0 : getQueueList[0]) === null || _a === void 0 ? void 0 : _a.queueList);
                                }
                                catch (error) {
                                    return [];
                                }
                            })()
                            : [];
                        if ((queueResult === null || queueResult === void 0 ? void 0 : queueResult.length) > 0) {
                            const agentExtList = [];
                            // queueResult?.map(async (data: any) => {
                            for (const data of queueResult) {
                                const now = Math.floor(Date.now() / 1000);
                                const isOlderThan30Minutes = (now - (data === null || data === void 0 ? void 0 : data.queuedTime)) > 30 * 60;
                                if (isOlderThan30Minutes) {
                                    if ((_b = getQueueList === null || getQueueList === void 0 ? void 0 : getQueueList[0]) === null || _b === void 0 ? void 0 : _b.qid) {
                                        const insertData = queueResult === null || queueResult === void 0 ? void 0 : queueResult.filter((listed) => (listed === null || listed === void 0 ? void 0 : listed.sessionId) !== (data === null || data === void 0 ? void 0 : data.sessionId));
                                        const matchData = queueResult === null || queueResult === void 0 ? void 0 : queueResult.filter((listmatch) => (listmatch === null || listmatch === void 0 ? void 0 : listmatch.sessionId) === (data === null || data === void 0 ? void 0 : data.sessionId));
                                        const payload = {
                                            qid: (_e = (_c = getQueueList === null || getQueueList === void 0 ? void 0 : getQueueList[0]) === null || _c === void 0 ? void 0 : _c.qid) !== null && _e !== void 0 ? _e : 0,
                                            queueName: (_g = getQueueList === null || getQueueList === void 0 ? void 0 : getQueueList[0]) === null || _g === void 0 ? void 0 : _g.queueName,
                                            queueType: 'chat',
                                            domainId: (_h = getQueueList === null || getQueueList === void 0 ? void 0 : getQueueList[0]) === null || _h === void 0 ? void 0 : _h.domainId,
                                            queueList: JSON.stringify(insertData)
                                        };
                                        try {
                                            yield (0, webChat_dao_1.insertUpdateQueueWaitTimeDao)(payload);
                                            (0, constants_1.insertQueuedTimeForChat)((matchData === null || matchData === void 0 ? void 0 : matchData.length) > 0 ? matchData : {});
                                        }
                                        catch (err) {
                                            console.log(err);
                                        }
                                        db_1.ioredisChat.emit('callQueueWait', payload);
                                        const getDomainResult = yield (0, callflow_dao_1.getDomainStatus)((_j = data === null || data === void 0 ? void 0 : data.channelId) === null || _j === void 0 ? void 0 : _j.domain_id);
                                        db_1.ioredisChat === null || db_1.ioredisChat === void 0 ? void 0 : db_1.ioredisChat.to(data === null || data === void 0 ? void 0 : data.session_id).emit('chat_response', JSON.stringify({ session_id: (_k = data === null || data === void 0 ? void 0 : data.channelId) === null || _k === void 0 ? void 0 : _k.session_id, req_type: 'DISCONNECT', domainStatus: getDomainResult }));
                                    }
                                }
                                else {
                                    if (!(data === null || data === void 0 ? void 0 : data.chatNotifyStatus)) {
                                        const longWaitRoutingAgent = yield (0, transferToQueue_1.transferChatForQueuedCustomer)(data === null || data === void 0 ? void 0 : data.channelId, data === null || data === void 0 ? void 0 : data.currentNode, agentExtList);
                                        const queueSetting = yield (0, callflow_dao_1.getQueueSettings)((_m = getQueueList === null || getQueueList === void 0 ? void 0 : getQueueList[0]) === null || _m === void 0 ? void 0 : _m.qid);
                                        const agentFinalList = [];
                                        longWaitRoutingAgent === null || longWaitRoutingAgent === void 0 ? void 0 : longWaitRoutingAgent.map((_list) => {
                                            if (!(agentExtList === null || agentExtList === void 0 ? void 0 : agentExtList.includes(_list === null || _list === void 0 ? void 0 : _list.ext))) {
                                                agentFinalList === null || agentFinalList === void 0 ? void 0 : agentFinalList.push(_list);
                                            }
                                        });
                                        if (agentFinalList && (agentFinalList === null || agentFinalList === void 0 ? void 0 : agentFinalList.length) > 0) {
                                            agentExtList.push((_o = agentFinalList[0]) === null || _o === void 0 ? void 0 : _o.ext);
                                            agentFinalList[0].queueId = (_q = (_p = data === null || data === void 0 ? void 0 : data.currentNode.data) === null || _p === void 0 ? void 0 : _p.information) === null || _q === void 0 ? void 0 : _q.transferTo;
                                            agentFinalList[0].queueSettings = queueSetting;
                                            yield (0, chatflow_helpers_1.handleTransferToLiveAgentToQueue)(data === null || data === void 0 ? void 0 : data.channelId, data === null || data === void 0 ? void 0 : data.currentNode, agentFinalList);
                                        }
                                        else {
                                            const queueSettings = typeof (data === null || data === void 0 ? void 0 : data.queueSetting) === 'string'
                                                ? (() => {
                                                    try {
                                                        return JSON.parse(data === null || data === void 0 ? void 0 : data.queueSetting);
                                                    }
                                                    catch (error) {
                                                        return [];
                                                    }
                                                })()
                                                : [];
                                            if ((queueSettings === null || queueSettings === void 0 ? void 0 : queueSettings.type) === 'Set Wait Time') {
                                                const givenTimestamp = (data === null || data === void 0 ? void 0 : data.queuedTime) * 1000;
                                                const currentTimestamp = Date.now();
                                                const differenceInMilliseconds = currentTimestamp - givenTimestamp;
                                                const differenceInMinutes = Math.floor(differenceInMilliseconds / 60000);
                                                if ((queueSettings === null || queueSettings === void 0 ? void 0 : queueSettings.maxWaitTime) <= differenceInMinutes) {
                                                    const queueListChanges = queueResult === null || queueResult === void 0 ? void 0 : queueResult.filter((_l) => (_l === null || _l === void 0 ? void 0 : _l.sessionId) !== (data === null || data === void 0 ? void 0 : data.sessionId));
                                                    const payload = {
                                                        qid: (_s = (_r = getQueueList === null || getQueueList === void 0 ? void 0 : getQueueList[0]) === null || _r === void 0 ? void 0 : _r.qid) !== null && _s !== void 0 ? _s : 0,
                                                        queueName: (_t = getQueueList === null || getQueueList === void 0 ? void 0 : getQueueList[0]) === null || _t === void 0 ? void 0 : _t.queueName,
                                                        queueType: 'chat',
                                                        domainId: (_u = getQueueList === null || getQueueList === void 0 ? void 0 : getQueueList[0]) === null || _u === void 0 ? void 0 : _u.domainId,
                                                        queueList: JSON.stringify(queueListChanges)
                                                    };
                                                    try {
                                                        yield (0, webChat_dao_1.insertUpdateQueueWaitTimeDao)(payload);
                                                    }
                                                    catch (err) {
                                                        console.log(err);
                                                    }
                                                    db_1.ioredisChat.emit('callQueueWait', payload);
                                                    if ((queueSettings === null || queueSettings === void 0 ? void 0 : queueSettings.fallBackQueue) !== 0 && (queueSettings === null || queueSettings === void 0 ? void 0 : queueSettings.fallBackQueue)) {
                                                        const queueWeightTime = yield (0, webChat_dao_1.getAllQueueWaitTime)((_v = getQueueList === null || getQueueList === void 0 ? void 0 : getQueueList[0]) === null || _v === void 0 ? void 0 : _v.domainId);
                                                        const queueDetails = yield (0, callflow_dao_1.getQueueSettingS)(queueSettings === null || queueSettings === void 0 ? void 0 : queueSettings.fallBackQueue, (_w = getQueueList === null || getQueueList === void 0 ? void 0 : getQueueList[0]) === null || _w === void 0 ? void 0 : _w.domainId);
                                                        const result = queueWeightTime === null || queueWeightTime === void 0 ? void 0 : queueWeightTime.filter((lists) => (lists === null || lists === void 0 ? void 0 : lists.qid) === (queueSettings === null || queueSettings === void 0 ? void 0 : queueSettings.fallBackQueue) && (lists === null || lists === void 0 ? void 0 : lists.queueName) === (queueSettings === null || queueSettings === void 0 ? void 0 : queueSettings.fallBackQueueName) && (lists === null || lists === void 0 ? void 0 : lists.queueType) === 'chat');
                                                        data.channelId.queueId = queueSettings === null || queueSettings === void 0 ? void 0 : queueSettings.fallBackQueue;
                                                        const waitTimmer = JSON.parse(queueDetails === null || queueDetails === void 0 ? void 0 : queueDetails.maximumCallers);
                                                        const waitQueueListData = (result === null || result === void 0 ? void 0 : result.length) > 0 ? JSON.parse((_x = result[0]) === null || _x === void 0 ? void 0 : _x.queueList) : [];
                                                        if ((waitTimmer === null || waitTimmer === void 0 ? void 0 : waitTimmer.type) !== 'Set Max Caller limit' || (waitTimmer === null || waitTimmer === void 0 ? void 0 : waitTimmer.queueLimit) > (waitQueueListData === null || waitQueueListData === void 0 ? void 0 : waitQueueListData.length)) {
                                                            const modifyData = {
                                                                firstName: (_y = data === null || data === void 0 ? void 0 : data.firstName) !== null && _y !== void 0 ? _y : null,
                                                                lastName: (_z = data === null || data === void 0 ? void 0 : data.lastName) !== null && _z !== void 0 ? _z : null,
                                                                sessionId: data === null || data === void 0 ? void 0 : data.sessionId,
                                                                queuedTime: Math.floor(Date.now() / 1000),
                                                                type: 'Chat',
                                                                chatHistoryId: data === null || data === void 0 ? void 0 : data.ChatHistoryId,
                                                                queueSetting: queueDetails === null || queueDetails === void 0 ? void 0 : queueDetails.maximumWaitTime,
                                                                queueUserCount: queueDetails === null || queueDetails === void 0 ? void 0 : queueDetails.maximumCallers,
                                                                queueAnnouncement: queueDetails === null || queueDetails === void 0 ? void 0 : queueDetails.announcement,
                                                                channelId: data === null || data === void 0 ? void 0 : data.channelId,
                                                                currentNode: data === null || data === void 0 ? void 0 : data.currentNode
                                                            };
                                                            const queueListData = [];
                                                            if ((result === null || result === void 0 ? void 0 : result.length) > 0) {
                                                                const preQueueData = JSON.parse((_0 = result[0]) === null || _0 === void 0 ? void 0 : _0.queueList);
                                                                preQueueData === null || preQueueData === void 0 ? void 0 : preQueueData.push(modifyData);
                                                                queueListData.push(...preQueueData);
                                                            }
                                                            else {
                                                                queueListData.push(modifyData);
                                                            }
                                                            const payloads = {
                                                                qid: (_1 = queueSettings === null || queueSettings === void 0 ? void 0 : queueSettings.fallBackQueue) !== null && _1 !== void 0 ? _1 : 0,
                                                                queueName: queueSettings === null || queueSettings === void 0 ? void 0 : queueSettings.fallBackQueueName,
                                                                queueType: 'chat',
                                                                domainId: (_2 = getQueueList === null || getQueueList === void 0 ? void 0 : getQueueList[0]) === null || _2 === void 0 ? void 0 : _2.domainId,
                                                                queueList: JSON.stringify(queueListData),
                                                                sessionId: (_3 = data === null || data === void 0 ? void 0 : data.channelId) === null || _3 === void 0 ? void 0 : _3.session_id
                                                            };
                                                            try {
                                                                yield (0, webChat_dao_1.insertUpdateQueueWaitTimeDao)(payloads);
                                                            }
                                                            catch (err) {
                                                                console.log(err);
                                                            }
                                                            db_1.ioredisChat.emit('callQueueWait', payload);
                                                            db_1.ioredisChat.to((_4 = data === null || data === void 0 ? void 0 : data.channelId) === null || _4 === void 0 ? void 0 : _4.session_id).emit('changeFallBackQueue', payloads);
                                                            const fallBackAgentDetails = yield (0, transferToQueue_1.transferChatForQueuedCustomer)(modifyData === null || modifyData === void 0 ? void 0 : modifyData.channelId, modifyData === null || modifyData === void 0 ? void 0 : modifyData.currentNode, []);
                                                            const fallBackQueueSetting = yield (0, callflow_dao_1.getQueueSettings)((_5 = modifyData === null || modifyData === void 0 ? void 0 : modifyData.channelId) === null || _5 === void 0 ? void 0 : _5.queueId);
                                                            if (fallBackAgentDetails && (fallBackAgentDetails === null || fallBackAgentDetails === void 0 ? void 0 : fallBackAgentDetails.length) > 0) {
                                                                fallBackAgentDetails[0].queueId = (_7 = (_6 = data === null || data === void 0 ? void 0 : data.currentNode.data) === null || _6 === void 0 ? void 0 : _6.information) === null || _7 === void 0 ? void 0 : _7.transferTo;
                                                                fallBackAgentDetails[0].queueSettings = fallBackQueueSetting;
                                                                yield (0, chatflow_helpers_1.handleTransferToLiveAgentToQueue)(data === null || data === void 0 ? void 0 : data.channelId, data === null || data === void 0 ? void 0 : data.currentNode, fallBackAgentDetails);
                                                            }
                                                            else {
                                                                const redisDataObj = {
                                                                    qid: queueSettings === null || queueSettings === void 0 ? void 0 : queueSettings.fallBackQueue,
                                                                    queueType: 'chat',
                                                                    domainId: (_8 = getQueueList === null || getQueueList === void 0 ? void 0 : getQueueList[0]) === null || _8 === void 0 ? void 0 : _8.domainId,
                                                                };
                                                                if (redisData) {
                                                                    const hasQid = redisData === null || redisData === void 0 ? void 0 : redisData.some((item) => item.qid === (queueSettings === null || queueSettings === void 0 ? void 0 : queueSettings.fallBackQueue));
                                                                    if (!hasQid) {
                                                                        redisData.push(redisDataObj);
                                                                        db_1.queueGetter.set('queuedCustomer', JSON.stringify(redisData));
                                                                    }
                                                                }
                                                                else {
                                                                    db_1.queueGetter.set('queuedCustomer', JSON.stringify([redisDataObj]));
                                                                }
                                                            }
                                                        }
                                                        else {
                                                            data.channelId.queueId = waitTimmer === null || waitTimmer === void 0 ? void 0 : waitTimmer.overFlowQueue;
                                                            (0, transferToQueue_1.insertCustomerInQueue)(data.channelId, data === null || data === void 0 ? void 0 : data.currentNode, waitTimmer === null || waitTimmer === void 0 ? void 0 : waitTimmer.overFlowQueue);
                                                        }
                                                    }
                                                }
                                                console.log(`Difference in minutes: ${differenceInMinutes}`);
                                            }
                                        }
                                    }
                                    else {
                                        const currentTime = Math.floor(Date.now() / 1000);
                                        const chatNotifyDiff = currentTime - (data === null || data === void 0 ? void 0 : data.notifyQueuedTime);
                                        if (chatNotifyDiff > 60) {
                                            const resultData = queueResult === null || queueResult === void 0 ? void 0 : queueResult.map((list_m) => {
                                                var _a;
                                                if ((list_m === null || list_m === void 0 ? void 0 : list_m.sessionId) === ((_a = data === null || data === void 0 ? void 0 : data.channelId) === null || _a === void 0 ? void 0 : _a.session_id)) {
                                                    list_m.chatNotifyStatus = false;
                                                }
                                                return list_m;
                                            });
                                            const payload = {
                                                qid: (_10 = (_9 = getQueueList === null || getQueueList === void 0 ? void 0 : getQueueList[0]) === null || _9 === void 0 ? void 0 : _9.qid) !== null && _10 !== void 0 ? _10 : 0,
                                                queueName: (_11 = getQueueList === null || getQueueList === void 0 ? void 0 : getQueueList[0]) === null || _11 === void 0 ? void 0 : _11.queueName,
                                                queueType: 'chat',
                                                domainId: (_12 = getQueueList === null || getQueueList === void 0 ? void 0 : getQueueList[0]) === null || _12 === void 0 ? void 0 : _12.domainId,
                                                queueList: JSON.stringify(resultData)
                                            };
                                            try {
                                                yield (0, webChat_dao_1.insertUpdateQueueWaitTimeDao)(payload);
                                            }
                                            catch (err) {
                                                console.log(err);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else {
                            const redisQueueArr = redisData === null || redisData === void 0 ? void 0 : redisData.filter((_d) => { var _a; return (_d === null || _d === void 0 ? void 0 : _d.qid) !== ((_a = getQueueList === null || getQueueList === void 0 ? void 0 : getQueueList[0]) === null || _a === void 0 ? void 0 : _a.qid); });
                            db_1.queueGetter.set('queuedCustomer', JSON.stringify(redisQueueArr));
                        }
                    }
                }
            }
        }), 60000);
    }
    catch (err) {
        console.log(err);
    }
};
exports.chatQueueHandler = chatQueueHandler;
const chatQueueByAgentStatusHandler = (agentData, skipRecursion = false) => __awaiter(void 0, void 0, void 0, function* () {
    var _a, _b, _c, _e, _g, _h, _j, _k, _m, _o, _p, _q, _r, _s, _t, _u, _v, _w, _x, _y;
    try {
        const redisQueueData = yield db_1.queueGetter.get('queuedCustomer');
        if (redisQueueData) {
            const redisData = JSON.parse(redisQueueData);
            const filterResult = (redisData === null || redisData === void 0 ? void 0 : redisData.length) > 0 && (redisData === null || redisData === void 0 ? void 0 : redisData.filter((_f) => (_f === null || _f === void 0 ? void 0 : _f.domainId) === (agentData === null || agentData === void 0 ? void 0 : agentData.domainId)));
            if ((filterResult === null || filterResult === void 0 ? void 0 : filterResult.length) > 0) {
                // await Promise.all(filterResult?.map(async (list: any) => {
                for (const list of filterResult) {
                    const getQueueList = yield (0, webChat_dao_1.getQueueWaitTime)(list);
                    const queueResult = typeof ((_a = getQueueList === null || getQueueList === void 0 ? void 0 : getQueueList[0]) === null || _a === void 0 ? void 0 : _a.queueList) === 'string'
                        ? (() => {
                            var _a;
                            try {
                                return JSON.parse((_a = getQueueList === null || getQueueList === void 0 ? void 0 : getQueueList[0]) === null || _a === void 0 ? void 0 : _a.queueList);
                            }
                            catch (error) {
                                return [];
                            }
                        })()
                        : [];
                    if ((queueResult === null || queueResult === void 0 ? void 0 : queueResult.length) > 0) {
                        const agentExtList = [];
                        // queueResult?.map(async (data: any) => {
                        for (const data of queueResult) {
                            const now = Math.floor(Date.now() / 1000);
                            const isOlderThan30Minutes = (now - (data === null || data === void 0 ? void 0 : data.queuedTime)) > 30 * 60;
                            if (isOlderThan30Minutes) {
                                if ((_b = getQueueList === null || getQueueList === void 0 ? void 0 : getQueueList[0]) === null || _b === void 0 ? void 0 : _b.qid) {
                                    const insertData = queueResult === null || queueResult === void 0 ? void 0 : queueResult.filter((listed) => (listed === null || listed === void 0 ? void 0 : listed.sessionId) !== (data === null || data === void 0 ? void 0 : data.sessionId));
                                    const matchData = queueResult === null || queueResult === void 0 ? void 0 : queueResult.filter((listmatch) => (listmatch === null || listmatch === void 0 ? void 0 : listmatch.sessionId) === (data === null || data === void 0 ? void 0 : data.sessionId));
                                    const payload = {
                                        qid: (_e = (_c = getQueueList === null || getQueueList === void 0 ? void 0 : getQueueList[0]) === null || _c === void 0 ? void 0 : _c.qid) !== null && _e !== void 0 ? _e : 0,
                                        queueName: (_g = getQueueList === null || getQueueList === void 0 ? void 0 : getQueueList[0]) === null || _g === void 0 ? void 0 : _g.queueName,
                                        queueType: 'chat',
                                        domainId: (_h = getQueueList === null || getQueueList === void 0 ? void 0 : getQueueList[0]) === null || _h === void 0 ? void 0 : _h.domainId,
                                        queueList: JSON.stringify(insertData)
                                    };
                                    try {
                                        yield (0, webChat_dao_1.insertUpdateQueueWaitTimeDao)(payload);
                                        (0, constants_1.insertQueuedTimeForChat)((matchData === null || matchData === void 0 ? void 0 : matchData.length) > 0 ? matchData : {});
                                    }
                                    catch (err) {
                                        console.log(err);
                                    }
                                    db_1.ioredisChat.emit('callQueueWait', payload);
                                    const getDomainResult = yield (0, callflow_dao_1.getDomainStatus)((_j = data === null || data === void 0 ? void 0 : data.channelId) === null || _j === void 0 ? void 0 : _j.domain_id);
                                    db_1.ioredisChat === null || db_1.ioredisChat === void 0 ? void 0 : db_1.ioredisChat.to(data === null || data === void 0 ? void 0 : data.session_id).emit('chat_response', JSON.stringify({ session_id: (_k = data === null || data === void 0 ? void 0 : data.channelId) === null || _k === void 0 ? void 0 : _k.session_id, req_type: 'DISCONNECT', domainStatus: getDomainResult }));
                                }
                            }
                            else {
                                if (!(data === null || data === void 0 ? void 0 : data.chatNotifyStatus)) {
                                    const longWaitRoutingAgent = yield (0, transferToQueue_1.transferChatForQueuedCustomer)(data === null || data === void 0 ? void 0 : data.channelId, data === null || data === void 0 ? void 0 : data.currentNode, agentExtList);
                                    const queueSetting = yield (0, callflow_dao_1.getQueueSettings)((_m = getQueueList === null || getQueueList === void 0 ? void 0 : getQueueList[0]) === null || _m === void 0 ? void 0 : _m.qid);
                                    const agentFinalList = [];
                                    longWaitRoutingAgent === null || longWaitRoutingAgent === void 0 ? void 0 : longWaitRoutingAgent.map((_list) => {
                                        if (!(agentExtList === null || agentExtList === void 0 ? void 0 : agentExtList.includes(_list === null || _list === void 0 ? void 0 : _list.ext))) {
                                            agentFinalList === null || agentFinalList === void 0 ? void 0 : agentFinalList.push(_list);
                                        }
                                    });
                                    console.log('agentFinalList:::::::::1212', agentFinalList, agentExtList);
                                    if (agentFinalList && (agentFinalList === null || agentFinalList === void 0 ? void 0 : agentFinalList.length) > 0) {
                                        agentExtList.push((_o = agentFinalList[0]) === null || _o === void 0 ? void 0 : _o.ext);
                                        const listed = {
                                            qid: (_q = (_p = data === null || data === void 0 ? void 0 : data.currentNode.data) === null || _p === void 0 ? void 0 : _p.information) === null || _q === void 0 ? void 0 : _q.transferTo,
                                            domainId: (_r = data === null || data === void 0 ? void 0 : data.channelId) === null || _r === void 0 ? void 0 : _r.domain_id
                                        };
                                        const getQueueListed = yield (0, webChat_dao_1.getQueueWaitTime)(listed);
                                        const parsedData = JSON.parse((_s = getQueueListed === null || getQueueListed === void 0 ? void 0 : getQueueListed[0]) === null || _s === void 0 ? void 0 : _s.queueList);
                                        const resultData = parsedData === null || parsedData === void 0 ? void 0 : parsedData.map((list_1) => {
                                            var _a;
                                            if ((list_1 === null || list_1 === void 0 ? void 0 : list_1.sessionId) === ((_a = data === null || data === void 0 ? void 0 : data.channelId) === null || _a === void 0 ? void 0 : _a.session_id)) {
                                                list_1.chatNotifyStatus = true;
                                                list.notifyQueuedTime = Math.floor(Date.now() / 1000);
                                            }
                                            return list_1;
                                        });
                                        const payload = {
                                            qid: (_u = (_t = getQueueListed === null || getQueueListed === void 0 ? void 0 : getQueueListed[0]) === null || _t === void 0 ? void 0 : _t.qid) !== null && _u !== void 0 ? _u : 0,
                                            queueName: (_v = getQueueListed === null || getQueueListed === void 0 ? void 0 : getQueueListed[0]) === null || _v === void 0 ? void 0 : _v.queueName,
                                            queueType: 'chat',
                                            domainId: (_w = getQueueListed === null || getQueueListed === void 0 ? void 0 : getQueueListed[0]) === null || _w === void 0 ? void 0 : _w.domainId,
                                            queueList: JSON.stringify(resultData)
                                        };
                                        try {
                                            yield (0, webChat_dao_1.insertUpdateQueueWaitTimeDao)(payload);
                                        }
                                        catch (err) {
                                            console.log(err);
                                        }
                                        agentFinalList[0].queueId = (_y = (_x = data === null || data === void 0 ? void 0 : data.currentNode.data) === null || _x === void 0 ? void 0 : _x.information) === null || _y === void 0 ? void 0 : _y.transferTo;
                                        agentFinalList[0].queueSettings = queueSetting;
                                        yield (0, chatflow_helpers_1.handleTransferToLiveAgentToQueue)(data === null || data === void 0 ? void 0 : data.channelId, data === null || data === void 0 ? void 0 : data.currentNode, agentFinalList);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    catch (err) {
        console.log(err);
    }
    finally {
        // CRITICAL FIX: Only call setTimeout if NOT called from AgentQueueManager
        // This prevents the infinite recursive loop that was causing memory buildup
        if (!skipRecursion) {
            setTimeout(() => {
                (0, exports.blpopQueuedCustomerByStatus)();
            }, 2000);
        }
    }
});
exports.chatQueueByAgentStatusHandler = chatQueueByAgentStatusHandler;
const blpopQueuedCustomerByStatus = () => __awaiter(void 0, void 0, void 0, function* () {
    // DEPRECATED: This function is no longer used
    // Queue processing is now handled by AgentQueueManager
    // Keeping for backward compatibility, but it does nothing
    console.warn('blpopQueuedCustomerByStatus called but is deprecated - use AgentQueueManager instead');
});
exports.blpopQueuedCustomerByStatus = blpopQueuedCustomerByStatus;
//# sourceMappingURL=chat_queue.js.map