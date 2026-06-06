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
exports.sendLiveNotificationInsta = exports.sendLiveNotificationFb = exports.sendLiveNotification = exports.transferChatForQueuedCustomer = exports.insertCustomerInQueue = exports.transferChatToAvailableAgent = exports.transferWithoutRouting = exports.transferChat = exports.transferChatToAgent = exports.longWaitRouting = void 0;
const uuid_1 = require("uuid");
const callflow_dao_1 = require("../dao/callflow.dao");
const chat_dao_1 = require("../dao/chat.dao");
const socialmedia_1 = require("../dao/socialmedia");
const webChat_dao_1 = require("../dao/webChat.dao");
const db_1 = require("../plugins/db");
const log_1 = require("../plugins/log");
const sendAgentDetails = (channelId, agent, previousStatus) => __awaiter(void 0, void 0, void 0, function* () {
    var _a;
    try {
        if ((previousStatus === null || previousStatus === void 0 ? void 0 : previousStatus.length) === 0 || previousStatus === undefined) {
            const insertArr = [+(agent === null || agent === void 0 ? void 0 : agent.ext)];
            const insertNewStatus = yield (0, callflow_dao_1.insertStatus)(channelId === null || channelId === void 0 ? void 0 : channelId.ChatHistoryId, channelId === null || channelId === void 0 ? void 0 : channelId.domain_id, insertArr);
            console.log('\n sendAgentDetails insertNewStatus\n', insertNewStatus);
        }
        else {
            const verifyArr = JSON.parse((_a = previousStatus[0]) === null || _a === void 0 ? void 0 : _a.chatuserlog);
            const insertArr = [...verifyArr, +(agent === null || agent === void 0 ? void 0 : agent.ext)];
            const insertNewStatus = yield (0, callflow_dao_1.insertStatus)(channelId === null || channelId === void 0 ? void 0 : channelId.ChatHistoryId, channelId === null || channelId === void 0 ? void 0 : channelId.domain_id, insertArr);
            console.log('\n sendAgentDetails insertNewStatus\n', insertNewStatus);
        }
        // await insertSessionForQueueFunc(channelId, currentNode);
        return [agent];
    }
    catch (err) {
        console.log('\n sendAgentDetails err\n', err);
        return [];
    }
});
const agentChatLimitFilter = (channel, currentNode, filteredData) => __awaiter(void 0, void 0, void 0, function* () {
    var _b, _c, _d, _e, _f;
    try {
        const chatLimitData = yield (0, callflow_dao_1.getConcurrentChat)(channel.domain_id);
        console.log('\n chatLimitFilter chatLimitData\n', JSON.stringify(chatLimitData));
        // {"domainId":7288,"isAllAgent":1,"allAgentLimit":2,"isAgent":0,"isRoutingProfile":0,"agentOccupyDetails":null}
        if (!chatLimitData) {
            console.log('\n *** Chat limit is not set ***\n');
            const sendAgent = yield sendAgentDetails(channel, filteredData, undefined);
            console.log('\n chatLimitFiltersendAgent\n', JSON.stringify(sendAgent));
            return sendAgent;
        }
        const parsedChatLimitData = JSON.parse((_b = chatLimitData === null || chatLimitData === void 0 ? void 0 : chatLimitData.agentOccupyDetails) !== null && _b !== void 0 ? _b : 'null') || null;
        console.log('\n chatLimitFilter parsedChatLimitData\n', JSON.stringify(parsedChatLimitData));
        if (parsedChatLimitData === null) {
            console.log('\n *** First chat in the domain, chatlimit set');
            const sendAgent = yield sendAgentDetails(channel, filteredData, undefined);
            console.log('\n chatLimitFiltersendAgent\n', JSON.stringify(sendAgent));
            return sendAgent;
        }
        const currentChatCount = parsedChatLimitData === null || parsedChatLimitData === void 0 ? void 0 : parsedChatLimitData.currentChatCount;
        console.log('\n chatLimitFilter currentChatCount\n', JSON.stringify(currentChatCount));
        console.log('filteredData', filteredData);
        let chatLimitByAgent;
        let routingProfileUser;
        let filteredAgentLimit;
        let filteredRoutingProfile;
        if (chatLimitData === null || chatLimitData === void 0 ? void 0 : chatLimitData.isAgent) {
            chatLimitByAgent = yield (0, callflow_dao_1.getAgentByChat)(channel.domain_id);
            console.log('\n chatLimitFilter chatLimitByAgent\n', JSON.stringify(chatLimitByAgent));
            filteredAgentLimit = chatLimitByAgent.filter((val) => +(val === null || val === void 0 ? void 0 : val.ext) === +(filteredData === null || filteredData === void 0 ? void 0 : filteredData.ext));
            console.log('filteredAgentLimit', filteredAgentLimit);
        }
        else if (chatLimitData === null || chatLimitData === void 0 ? void 0 : chatLimitData.isRoutingProfile) {
            routingProfileUser = yield (0, callflow_dao_1.getRoutingProfileUser)(channel.domain_id);
            console.log('\n chatLimitFilter routingProfileUser\n', JSON.stringify(routingProfileUser));
            filteredRoutingProfile = routingProfileUser.filter((val) => +(val === null || val === void 0 ? void 0 : val.RoutingProfileId) === +(filteredData === null || filteredData === void 0 ? void 0 : filteredData.routingProfileId));
            console.log('filteredRoutingProfile', filteredRoutingProfile);
        }
        for (const [key, value] of Object.entries(currentChatCount)) {
            if (+(filteredData === null || filteredData === void 0 ? void 0 : filteredData.ext) === +(key)) {
                console.log('ext matched');
                if (chatLimitData === null || chatLimitData === void 0 ? void 0 : chatLimitData.isAllAgent) {
                    console.log('isAllAgent limit matched');
                    console.log('allAgentLimit before', chatLimitData === null || chatLimitData === void 0 ? void 0 : chatLimitData.allAgentLimit, value);
                    if ((chatLimitData === null || chatLimitData === void 0 ? void 0 : chatLimitData.allAgentLimit) > value) {
                        console.log('allAgentLimit less than value');
                        return [filteredData];
                    }
                }
                else if (chatLimitData === null || chatLimitData === void 0 ? void 0 : chatLimitData.isAgent) {
                    console.log('isAgent limit matched');
                    console.log('isAgent before', (_c = filteredAgentLimit[0]) === null || _c === void 0 ? void 0 : _c.chatLimit, value);
                    if (+((_d = filteredAgentLimit[0]) === null || _d === void 0 ? void 0 : _d.chatLimit) > +(value)) {
                        console.log('allAgentLimit less than value');
                        return [filteredData];
                    }
                }
                else if (chatLimitData === null || chatLimitData === void 0 ? void 0 : chatLimitData.isRoutingProfile) {
                    console.log('routingprofile limit matched');
                    console.log('routingprofile before', (_e = filteredRoutingProfile[0]) === null || _e === void 0 ? void 0 : _e.concurrentChat, value);
                    if (+((_f = filteredRoutingProfile[0]) === null || _f === void 0 ? void 0 : _f.concurrentChat) > +(value)) {
                        console.log('allAgentLimit less than value');
                        return [filteredData];
                    }
                }
                else {
                    return [filteredData];
                }
                break;
            }
        }
        return [];
    }
    catch (err) {
        return [];
    }
});
const chatLimitFilter = (channel, currentNode, filteredData, previousStatus) => __awaiter(void 0, void 0, void 0, function* () {
    var _g;
    try {
        const agentDetails = [];
        const chatLimitData = yield (0, callflow_dao_1.getConcurrentChat)(channel.domain_id);
        console.log('\n chatLimitFilter chatLimitData\n', JSON.stringify(chatLimitData));
        // {"domainId":7288,"isAllAgent":1,"allAgentLimit":2,"isAgent":0,"isRoutingProfile":0,"agentOccupyDetails":null}
        if (!chatLimitData) {
            console.log('\n *** Chat limit is not set ***\n');
            const sendAgent = yield sendAgentDetails(channel, filteredData, undefined);
            console.log('\n chatLimitFiltersendAgent\n', JSON.stringify(sendAgent));
            return sendAgent;
        }
        const parsedChatLimitData = JSON.parse((_g = chatLimitData === null || chatLimitData === void 0 ? void 0 : chatLimitData.agentOccupyDetails) !== null && _g !== void 0 ? _g : 'null') || null;
        console.log('\n chatLimitFilter parsedChatLimitData\n', JSON.stringify(parsedChatLimitData));
        if (parsedChatLimitData === null) {
            const sendAgent = yield sendAgentDetails(channel, filteredData[0], previousStatus);
            console.log('\n chatLimitFiltersendAgent\n', JSON.stringify(sendAgent));
            return sendAgent;
        }
        const currentChatCount = parsedChatLimitData === null || parsedChatLimitData === void 0 ? void 0 : parsedChatLimitData.currentChatCount;
        console.log('\n chatLimitFilter currentChatCount\n', JSON.stringify(currentChatCount));
        let chatLimitByAgent;
        let routingProfileUser;
        if (chatLimitData === null || chatLimitData === void 0 ? void 0 : chatLimitData.isAgent) {
            chatLimitByAgent = yield (0, callflow_dao_1.getAgentByChat)(channel.domain_id);
            console.log('\n chatLimitFilter chatLimitByAgent\n', JSON.stringify(chatLimitByAgent));
        }
        else if (chatLimitData === null || chatLimitData === void 0 ? void 0 : chatLimitData.isRoutingProfile) {
            routingProfileUser = yield (0, callflow_dao_1.getRoutingProfileUser)(channel.domain_id);
            console.log('\n chatLimitFilter routingProfileUser\n', JSON.stringify(routingProfileUser));
        }
        if (currentChatCount === null || currentChatCount === undefined) {
            const sendAgent = yield sendAgentDetails(channel, filteredData[0], previousStatus);
            console.log('\n chatLimitFilter sendAgent\n', JSON.stringify(sendAgent));
            return sendAgent;
        }
        const appendChatCountToFilteredData = filteredData.map((dt) => {
            let updatedDt = Object.assign({}, dt);
            for (const [key, value] of Object.entries(currentChatCount)) {
                if (+(dt.ext) === +(key)) {
                    updatedDt = Object.assign(Object.assign({}, updatedDt), { currentChatCount: value });
                    break;
                }
                else {
                    updatedDt = Object.assign(Object.assign({}, updatedDt), { currentChatCount: 0 });
                }
            }
            return updatedDt;
        });
        console.log('\n chatLimitFilter appendChatCountToFilteredData\n', JSON.stringify(appendChatCountToFilteredData));
        const chatLimitAchievedAgents = [];
        let agentsToBeAssigned = [...appendChatCountToFilteredData];
        if ((chatLimitData === null || chatLimitData === void 0 ? void 0 : chatLimitData.isAllAgent) || (chatLimitData === null || chatLimitData === void 0 ? void 0 : chatLimitData.isAgent) || (chatLimitData === null || chatLimitData === void 0 ? void 0 : chatLimitData.isRoutingProfile)) {
            let tempAgent;
            const checkLiveAgent = () => {
                console.log('checkLiveAgent called------>', tempAgent);
                if (tempAgent !== undefined) {
                    if (chatLimitData === null || chatLimitData === void 0 ? void 0 : chatLimitData.isAllAgent) {
                        if ((tempAgent === null || tempAgent === void 0 ? void 0 : tempAgent.currentChatCount) < (chatLimitData === null || chatLimitData === void 0 ? void 0 : chatLimitData.allAgentLimit)) {
                            agentDetails.push(tempAgent);
                        }
                        else {
                            if (chatLimitAchievedAgents.length !== appendChatCountToFilteredData.length) {
                                chatLimitAchievedAgents.push(tempAgent);
                                agentsToBeAssigned = agentsToBeAssigned.filter((val) => val.ext !== tempAgent.ext);
                                assignFunction();
                            }
                        }
                    }
                    else if (chatLimitData === null || chatLimitData === void 0 ? void 0 : chatLimitData.isAgent) {
                        chatLimitByAgent.map((list) => {
                            console.log('map--->', tempAgent.currentChatCount, list.chatLimit, tempAgent.ext, list.ext);
                            if (+(tempAgent.ext) === +(list.ext)) {
                                if ((tempAgent === null || tempAgent === void 0 ? void 0 : tempAgent.currentChatCount) < list.chatLimit) {
                                    console.log('\n condition met, agent added', tempAgent);
                                    agentDetails.push(tempAgent);
                                }
                                else {
                                    console.log('condition not met, agent removed from list');
                                    chatLimitAchievedAgents.push(tempAgent);
                                    agentsToBeAssigned = agentsToBeAssigned.filter((val) => val.ext !== tempAgent.ext);
                                    assignFunction();
                                }
                            }
                        });
                    }
                    else if (chatLimitData === null || chatLimitData === void 0 ? void 0 : chatLimitData.isRoutingProfile) {
                        console.log('isRoutingProfile entered');
                        const routings = {};
                        routingProfileUser.map((item) => {
                            routings[item.RoutingProfileId] = item.concurrentChat;
                        });
                        console.log('routings', routings);
                        for (const [key, value] of Object.entries(routings)) {
                            console.log('key', key, 'id', tempAgent.routingProfileId);
                            if (+(tempAgent.routingProfileId) === +(key)) {
                                if (tempAgent.currentChatCount < value) {
                                    console.log('\n condition met, agent added', tempAgent);
                                    agentDetails.push(tempAgent);
                                }
                                else {
                                    console.log('condition not met, agent removed from list');
                                    if (chatLimitAchievedAgents.length !== appendChatCountToFilteredData.length) {
                                        chatLimitAchievedAgents.push(tempAgent);
                                        agentsToBeAssigned = agentsToBeAssigned.filter((val) => val.ext !== tempAgent.ext);
                                        assignFunction();
                                    }
                                }
                            }
                        }
                    }
                }
            };
            const assignFunction = () => {
                console.log('assignFunction called------>');
                if ((agentsToBeAssigned === null || agentsToBeAssigned === void 0 ? void 0 : agentsToBeAssigned.length) > 0) {
                    tempAgent = agentsToBeAssigned.reduce((oldest, current) => {
                        if ((current === null || current === void 0 ? void 0 : current.currentChatCount) < (oldest === null || oldest === void 0 ? void 0 : oldest.currentChatCount)) {
                            return current;
                        }
                        if ((current === null || current === void 0 ? void 0 : current.currentChatCount) === (oldest === null || oldest === void 0 ? void 0 : oldest.currentChatCount) && (oldest === null || oldest === void 0 ? void 0 : oldest.stateTime) > (current === null || current === void 0 ? void 0 : current.stateTime)) {
                            return current;
                        }
                        return oldest;
                    }, agentsToBeAssigned[0]);
                    checkLiveAgent();
                }
            };
            assignFunction();
        }
        else {
            console.log('\n *** Chat limit turned off ***\n');
            const sendAgent = yield sendAgentDetails(channel, filteredData[0], undefined);
            console.log('\n sendAgent\n', JSON.stringify(sendAgent));
            return sendAgent;
        }
        if ((agentDetails === null || agentDetails === void 0 ? void 0 : agentDetails.length) > 0) {
            const sendAgent = yield sendAgentDetails(channel, agentDetails[0], previousStatus);
            console.log('\n chatLimitFilter sendAgent\n', JSON.stringify(sendAgent));
            return sendAgent;
        }
        return agentDetails;
    }
    catch (err) {
        console.log('\n chatLimitFilter err\n', err);
        return [];
    }
});
const longWaitRouting = (channelId, currentNode, readyAgentsData) => __awaiter(void 0, void 0, void 0, function* () {
    var _h;
    try {
        const sortedAgentsBasedOnTime = readyAgentsData.sort((a, b) => a.stateTime - b.stateTime);
        console.log('\n longWaitRouting sortedAgentsBasedOnTime\n', JSON.stringify(sortedAgentsBasedOnTime));
        const sortedAgentsBasedOnRole = sortedAgentsBasedOnTime.sort((a, b) => b.roleId - a.roleId);
        console.log('\n longWaitRouting sortedAgentsBasedOnRole\n', JSON.stringify(sortedAgentsBasedOnRole));
        let filteredNotifiedAgents = [];
        const previousStatus = yield (0, callflow_dao_1.getPreviousStatus)(channelId === null || channelId === void 0 ? void 0 : channelId.ChatHistoryId, channelId === null || channelId === void 0 ? void 0 : channelId.domain_id); // chat notified to agents list previously
        console.log('\n longWaitRouting previousStatus\n', previousStatus);
        if ((previousStatus === null || previousStatus === void 0 ? void 0 : previousStatus.length) !== 0) {
            // redirecting from other agents
            const verifyArr = JSON.parse((_h = previousStatus[0]) === null || _h === void 0 ? void 0 : _h.chatuserlog);
            console.log('\n longWaitRouting verifyArr\n', verifyArr, sortedAgentsBasedOnRole);
            if ((sortedAgentsBasedOnRole === null || sortedAgentsBasedOnRole === void 0 ? void 0 : sortedAgentsBasedOnRole.length) > 1) {
                filteredNotifiedAgents = sortedAgentsBasedOnRole.filter((val) => !(verifyArr.includes(+(parseInt(val === null || val === void 0 ? void 0 : val.ext, 10)))));
            }
            else {
                filteredNotifiedAgents = sortedAgentsBasedOnRole;
            }
            if ((filteredNotifiedAgents === null || filteredNotifiedAgents === void 0 ? void 0 : filteredNotifiedAgents.length) === 0) {
                const verifyUserArr = [verifyArr.pop()];
                filteredNotifiedAgents = sortedAgentsBasedOnRole.filter((val) => !(verifyUserArr.includes(+(parseInt(val === null || val === void 0 ? void 0 : val.ext, 10)))));
                if ((filteredNotifiedAgents === null || filteredNotifiedAgents === void 0 ? void 0 : filteredNotifiedAgents.length) === 0) {
                    return [];
                }
            }
        }
        else {
            filteredNotifiedAgents = [...sortedAgentsBasedOnRole];
        }
        console.log('\n longWaitRouting filteredNotifiedAgents\n', JSON.stringify(filteredNotifiedAgents));
        const chatLimitCheckedAgent = yield chatLimitFilter(channelId, currentNode, filteredNotifiedAgents, previousStatus);
        console.log('\n longWaitRouting chatLimitCheckedAgent\n', JSON.stringify(chatLimitCheckedAgent));
        return chatLimitCheckedAgent;
    }
    catch (err) {
        console.log('\n longWaitRouting err\n', err);
        return [];
    }
});
exports.longWaitRouting = longWaitRouting;
const skillRouting = (channelId, readyAgentsData) => __awaiter(void 0, void 0, void 0, function* () {
    try {
        const skills = channelId === null || channelId === void 0 ? void 0 : channelId.skills;
        const getAgentSkills = yield (0, callflow_dao_1.getAgentsSkills)(channelId.domain_id, '1,2,3,5,6');
        const skilledAgentsExt = [];
        for (const skill of skills) {
            getAgentSkills.filter((agent) => {
                var _a;
                if ((_a = agent === null || agent === void 0 ? void 0 : agent.addSkill) === null || _a === void 0 ? void 0 : _a.includes(skill)) {
                    skilledAgentsExt.push(+(agent === null || agent === void 0 ? void 0 : agent.ext));
                }
            });
            // for (const agent of getAgentSkills) {
            //   console.log('second loop', agent);
            //   console.log('agent.ext', agent?.ext);
            //   if (agent?.addSkill.includes(skill)) {
            //     console.log('entered', agent?.ext);
            //     skilledAgentsExt.push(+(agent?.ext));
            //   }
            // }
        }
        console.log('skilledAgentsExt', skilledAgentsExt);
        const filteredUniqueExt = [...new Set(skilledAgentsExt)];
        console.log('filteredUniqueExt', filteredUniqueExt);
        const skilledAgents = [];
        filteredUniqueExt.map((val) => {
            for (const agent of readyAgentsData) {
                if (+(`${val}`) === +(`${agent.ext}`)) {
                    skilledAgents.push(agent);
                }
            }
        });
        console.log('\n skilledAgents\n', JSON.stringify(skilledAgents));
        return skilledAgents;
    }
    catch (err) {
        console.log('\n skillRouting err\n', err);
        return [];
    }
});
const queueChat = (channelId, currentNode, readyAgentsData) => __awaiter(void 0, void 0, void 0, function* () {
    try {
        let filteredAgents = [];
        let skillRoutingAgent = [];
        const routingProfiles = yield (0, callflow_dao_1.getRoutingProfiles)(channelId === null || channelId === void 0 ? void 0 : channelId.queueId, channelId === null || channelId === void 0 ? void 0 : channelId.domain_id); // getting routing profiles mapped to queue
        console.log('\n queueChat routingProfiles\n', JSON.stringify(routingProfiles));
        if ((routingProfiles === null || routingProfiles === void 0 ? void 0 : routingProfiles.length) === 0) {
            return [];
        }
        const sortedRoutingProfiles = routingProfiles.sort((a, b) => a.priority - b.priority); // sorting routing profiles based on priority
        for (const profile of sortedRoutingProfiles) {
            filteredAgents = readyAgentsData.filter((a) => (a === null || a === void 0 ? void 0 : a.routingProfileId) === (profile === null || profile === void 0 ? void 0 : profile.rqmid) && (profile === null || profile === void 0 ? void 0 : profile.chat) === 1);
            console.log('\n queueChat filteredAgents\n', JSON.stringify(filteredAgents));
            if ((filteredAgents === null || filteredAgents === void 0 ? void 0 : filteredAgents.length) === 0) {
                continue;
            }
            const getRouting = yield (0, callflow_dao_1.getRoutingDetails)(profile === null || profile === void 0 ? void 0 : profile.rqmid, channelId === null || channelId === void 0 ? void 0 : channelId.domain_id);
            console.log('\n queueChat getRouting\n', JSON.stringify(getRouting));
            if ((getRouting === null || getRouting === void 0 ? void 0 : getRouting.routingMethod) !== 'Long wait routing') {
                skillRoutingAgent = yield skillRouting(channelId, filteredAgents);
                if ((skillRoutingAgent === null || skillRoutingAgent === void 0 ? void 0 : skillRoutingAgent.length) <= 0) {
                    return [];
                }
                filteredAgents = [...skillRoutingAgent];
                console.log('\n\n queueChat skillRoutingAgent\n', JSON.stringify(skillRoutingAgent));
            }
            const longWaitRoutingAgent = yield (0, exports.longWaitRouting)(channelId, currentNode, filteredAgents);
            console.log('\n\n queuChat longWaitRoutingAgent\n', JSON.stringify(longWaitRoutingAgent));
            if ((longWaitRoutingAgent === null || longWaitRoutingAgent === void 0 ? void 0 : longWaitRoutingAgent.length) > 0) {
                return longWaitRoutingAgent;
            }
        }
        return [];
    }
    catch (err) {
        console.log('\n queueChat err\n', err);
        return [];
    }
});
const readyAgentsSupervisorsAdmins = (channel) => __awaiter(void 0, void 0, void 0, function* () {
    try {
        const readyAgentsSupervisorsAdminsList = [];
        const data = yield db_1.otherRedisClient.hgetall(`${channel === null || channel === void 0 ? void 0 : channel.domain_id}_presence`);
        const result = Object.values(data).map((value) => JSON.parse(value));
        if (!data) {
            return readyAgentsSupervisorsAdminsList;
        }
        for (const item of result) {
            const getData = yield (0, chat_dao_1.getUserList)(item);
            if (Number(item === null || item === void 0 ? void 0 : item.isChat) === 1 && Number(item === null || item === void 0 ? void 0 : item.isChatTransfer) === 0) {
                getData.forEach((datas) => {
                    if (Number(item.ext) === datas.ext && datas.chat === 1) {
                        readyAgentsSupervisorsAdminsList.push(item);
                    }
                });
            }
        }
        return readyAgentsSupervisorsAdminsList;
    }
    catch (err) {
        return [];
    }
});
const readyAgents = (channel) => __awaiter(void 0, void 0, void 0, function* () {
    try {
        const readyAgentsList = [];
        const data = yield db_1.otherRedisClient.hgetall(`${channel === null || channel === void 0 ? void 0 : channel.domain_id}_presence`);
        const result = Object.values(data).map((value) => JSON.parse(value));
        if (!data) {
            return readyAgentsList;
        }
        const getData = yield (0, chat_dao_1.getUserList)(result[0]);
        for (const item of result) {
            // if (Number(item?.isChat) === 1 && Number(item?.isChatTransfer) === 0 && Number(item?.roleId) === 6) {
            if (Number(item === null || item === void 0 ? void 0 : item.isChat) === 1 && Number(item === null || item === void 0 ? void 0 : item.isChatTransfer) === 0) {
                getData.forEach((dataq) => {
                    if (Number(item.ext) === dataq.ext && dataq.chat === 1) {
                        readyAgentsList.push(item);
                    }
                });
            }
        }
        return readyAgentsList;
    }
    catch (err) {
        return [];
    }
});
const readySupervisors = (channel) => __awaiter(void 0, void 0, void 0, function* () {
    try {
        const readyAgentsList = [];
        const data = yield db_1.otherRedisClient.hgetall(`${channel === null || channel === void 0 ? void 0 : channel.domain_id}_presence`);
        const result = Object.values(data).map((value) => JSON.parse(value));
        if (!data) {
            return readyAgentsList;
        }
        for (const item of result) {
            const getData = yield (0, chat_dao_1.getUserList)(item);
            if (Number(item === null || item === void 0 ? void 0 : item.isChat) === 1 && Number(item === null || item === void 0 ? void 0 : item.isChatTransfer) === 0 && Number(item === null || item === void 0 ? void 0 : item.roleId) === 5) {
                getData.forEach((dataa) => {
                    if (Number(item.ext) === dataa.ext && dataa.chat === 1) {
                        readyAgentsList.push(item);
                    }
                });
            }
        }
        return readyAgentsList;
    }
    catch (err) {
        return [];
    }
});
const readyAdmins = (channel) => __awaiter(void 0, void 0, void 0, function* () {
    try {
        const readyAgentsList = [];
        const data = yield db_1.otherRedisClient.hgetall(`${channel === null || channel === void 0 ? void 0 : channel.domain_id}_presence`);
        if (!data) {
            return readyAgentsList;
        }
        const result = Object.values(data).map(value => JSON.parse(value));
        for (const item of result) {
            const getData = yield (0, chat_dao_1.getUserList)(item);
            if (Number(item === null || item === void 0 ? void 0 : item.isChat) === 1 && Number(item === null || item === void 0 ? void 0 : item.isChatTransfer) === 0 && Number(item === null || item === void 0 ? void 0 : item.roleId) === 3) {
                getData.forEach((datav) => {
                    if (Number(item.ext) === datav.ext && datav.chat === 1) {
                        readyAgentsList.push(item);
                    }
                });
            }
        }
        return readyAgentsList;
    }
    catch (err) {
        console.error(err);
        return [];
    }
});
const transferChatToAgent = (channelId, currentNode) => __awaiter(void 0, void 0, void 0, function* () {
    // roleId
    // 6 Agent, 5 Supervisor, 3 Admin
});
exports.transferChatToAgent = transferChatToAgent;
const transferChat = (channelId, currentNode) => __awaiter(void 0, void 0, void 0, function* () {
    var _j, _k, _l, _m;
    try {
        console.log('\n transferChat currentNode\n', JSON.stringify(currentNode));
        console.log('\n transferChat channelId\n', JSON.stringify(channelId));
        let agentDetails = [];
        if (((_k = (_j = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _j === void 0 ? void 0 : _j.information) === null || _k === void 0 ? void 0 : _k.type) === 2) { // To direct agent
            const readyAgentsSupervisorsAdminsData = yield readyAgentsSupervisorsAdmins(channelId); // checking agents in ready state
            console.log('\n readyAgentsSupervisorsAdminsData\n', JSON.stringify(readyAgentsSupervisorsAdminsData));
            if ((readyAgentsSupervisorsAdminsData === null || readyAgentsSupervisorsAdminsData === void 0 ? void 0 : readyAgentsSupervisorsAdminsData.length) === 0) { // returning when no agents available
                return agentDetails;
            }
            const filteredAgent = readyAgentsSupervisorsAdminsData === null || readyAgentsSupervisorsAdminsData === void 0 ? void 0 : readyAgentsSupervisorsAdminsData.filter((val) => { var _a, _b; return (val === null || val === void 0 ? void 0 : val.userId) === ((_b = (_a = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _a === void 0 ? void 0 : _a.information) === null || _b === void 0 ? void 0 : _b.transferTo); }); // checking agent ready status
            agentDetails = agentChatLimitFilter(channelId, currentNode, filteredAgent[0]);
        }
        if (((_m = (_l = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _l === void 0 ? void 0 : _l.information) === null || _m === void 0 ? void 0 : _m.type) === 1) { // To queue
            const readyAgentsData = yield readyAgents(channelId); // checking agents in ready state
            console.log('\n readyAgentsData\n', JSON.stringify(readyAgentsData));
            if ((readyAgentsData === null || readyAgentsData === void 0 ? void 0 : readyAgentsData.length) !== 0) { // Connecting chat with agents
                agentDetails = yield queueChat(channelId, currentNode, readyAgentsData);
                console.log('\ntransferChat readyAgentsData agentDetails\n', JSON.stringify(agentDetails));
            }
            if (agentDetails.length === 0) {
                const readySupervisorsData = yield readySupervisors(channelId); // checking agents in ready state
                console.log('\n readySupervisorsData\n', JSON.stringify(readySupervisorsData));
                if ((readySupervisorsData === null || readySupervisorsData === void 0 ? void 0 : readySupervisorsData.length) !== 0) { // Connecting chat with supervisors
                    agentDetails = yield queueChat(channelId, currentNode, readySupervisorsData);
                    console.log('\ntransferChat readySupervisorsData agentDetails\n', JSON.stringify(agentDetails));
                }
            }
            if (agentDetails.length === 0) {
                const readyAdminsData = yield readyAdmins(channelId); // checking agents in ready state
                console.log('\n readyAdminsData\n', JSON.stringify(readyAdminsData));
                if ((readyAdminsData === null || readyAdminsData === void 0 ? void 0 : readyAdminsData.length) !== 0) { // Connecting chat with admins
                    agentDetails = yield queueChat(channelId, currentNode, readyAdminsData);
                    console.log('\ntransferChat readyAdminsData agentDetails\n', JSON.stringify(agentDetails));
                }
            }
        }
        return agentDetails;
    }
    catch (err) {
        console.log('err', err);
        return [];
    }
});
exports.transferChat = transferChat;
const transferWithoutRouting = (channelId, currentNode) => __awaiter(void 0, void 0, void 0, function* () {
    try {
        let filteredAgents = [];
        let skillRoutingAgent = [];
        const readyAgentsData = yield readyAgents(channelId);
        skillRoutingAgent = yield skillRouting(channelId, readyAgentsData);
        if ((skillRoutingAgent === null || skillRoutingAgent === void 0 ? void 0 : skillRoutingAgent.length) <= 0) {
            return [];
        }
        filteredAgents = [...skillRoutingAgent];
        return filteredAgents;
    }
    catch (err) {
        console.log('\n queueChat err\n', err);
        return [];
    }
});
exports.transferWithoutRouting = transferWithoutRouting;
const transferChatToAvailableAgent = (channelId) => __awaiter(void 0, void 0, void 0, function* () {
    try {
        let agentDetails = [];
        log_1.logger.info(`agentDetails Socket Error: ${JSON.stringify(agentDetails)}`);
        const readyAgentsData = yield readyAgents(channelId); // checking agents in ready state
        console.log('\n readyAgentsData\n', JSON.stringify(readyAgentsData));
        if ((readyAgentsData === null || readyAgentsData === void 0 ? void 0 : readyAgentsData.length) !== 0) { // Connecting chat with agents
            agentDetails = yield (0, exports.longWaitRouting)(channelId, undefined, readyAgentsData);
            console.log('\n\n liveagent agentDetails\n', JSON.stringify(agentDetails));
        }
        if (agentDetails.length === 0) {
            const readySupervisorsData = yield readySupervisors(channelId); // checking supervisors in ready state
            console.log('\n readySupervisorsData\n', JSON.stringify(readySupervisorsData));
            if ((readySupervisorsData === null || readySupervisorsData === void 0 ? void 0 : readySupervisorsData.length) !== 0) { // Connecting chat with supervisors
                agentDetails = yield (0, exports.longWaitRouting)(channelId, undefined, readySupervisorsData);
                console.log('\n\n liveagent agentDetails\n', JSON.stringify(agentDetails));
            }
        }
        if (agentDetails.length === 0) {
            const readyAdminsData = yield readyAdmins(channelId); // checking admins in ready state
            console.log('\n readyAdminsData\n', JSON.stringify(readyAdminsData));
            if ((readyAdminsData === null || readyAdminsData === void 0 ? void 0 : readyAdminsData.length) !== 0) { // Connecting chat with admins
                agentDetails = yield (0, exports.longWaitRouting)(channelId, undefined, readyAdminsData);
                console.log('\n\n liveagent agentDetails\n', JSON.stringify(agentDetails));
            }
        }
        log_1.logger.info(`agentDetails12 Socket Error: ${JSON.stringify(agentDetails)}`);
        return agentDetails;
    }
    catch (err) {
        return [];
    }
});
exports.transferChatToAvailableAgent = transferChatToAvailableAgent;
const insertCustomerInQueue = (data, currentData, queueId) => __awaiter(void 0, void 0, void 0, function* () {
    var _o, _p, _q, _r, _s, _t;
    try {
        const queueDetails = yield (0, callflow_dao_1.getQueueSettingS)(queueId, data === null || data === void 0 ? void 0 : data.domain_id);
        const customerDetails = yield (0, webChat_dao_1.getCustomerConatctDetails)(data === null || data === void 0 ? void 0 : data.domain_id, data === null || data === void 0 ? void 0 : data.customerId);
        const queueWeightTime = yield (0, webChat_dao_1.getAllQueueWaitTime)(data === null || data === void 0 ? void 0 : data.domain_id);
        const result = queueWeightTime === null || queueWeightTime === void 0 ? void 0 : queueWeightTime.filter((list) => (list === null || list === void 0 ? void 0 : list.qid) === queueId && (list === null || list === void 0 ? void 0 : list.queueName) === (queueDetails === null || queueDetails === void 0 ? void 0 : queueDetails.name) && (list === null || list === void 0 ? void 0 : list.queueType) === 'chat');
        const queueListData = [];
        const waitTimmer = JSON.parse(queueDetails === null || queueDetails === void 0 ? void 0 : queueDetails.maximumCallers);
        const waitQueueListData = (result === null || result === void 0 ? void 0 : result.length) > 0 ? JSON.parse((_o = result[0]) === null || _o === void 0 ? void 0 : _o.queueList) : [];
        if ((waitTimmer === null || waitTimmer === void 0 ? void 0 : waitTimmer.type) !== 'Set Max Caller limit' || (waitTimmer === null || waitTimmer === void 0 ? void 0 : waitTimmer.queueLimit) > (waitQueueListData === null || waitQueueListData === void 0 ? void 0 : waitQueueListData.length)) {
            const queueTypeData = {
                firstName: (_q = (_p = customerDetails[0]) === null || _p === void 0 ? void 0 : _p.firstName) !== null && _q !== void 0 ? _q : null,
                lastName: (_s = (_r = customerDetails[0]) === null || _r === void 0 ? void 0 : _r.lastName) !== null && _s !== void 0 ? _s : null,
                sessionId: data === null || data === void 0 ? void 0 : data.session_id,
                queuedTime: Math.floor(Date.now() / 1000),
                type: 'Chat',
                chatHistoryId: data === null || data === void 0 ? void 0 : data.ChatHistoryId,
                queueSetting: queueDetails === null || queueDetails === void 0 ? void 0 : queueDetails.maximumWaitTime,
                queueUserCount: queueDetails === null || queueDetails === void 0 ? void 0 : queueDetails.maximumCallers,
                queueAnnouncement: queueDetails === null || queueDetails === void 0 ? void 0 : queueDetails.announcement,
                channelId: data,
                currentNode: currentData
            };
            if ((result === null || result === void 0 ? void 0 : result.length) > 0) {
                const preQueueData = JSON.parse((_t = result[0]) === null || _t === void 0 ? void 0 : _t.queueList);
                preQueueData === null || preQueueData === void 0 ? void 0 : preQueueData.push(queueTypeData);
                queueListData.push(...preQueueData);
            }
            else {
                queueListData.push(queueTypeData);
            }
            const payload = {
                qid: queueId !== null && queueId !== void 0 ? queueId : 0,
                queueName: queueDetails === null || queueDetails === void 0 ? void 0 : queueDetails.name,
                queueType: 'chat',
                domainId: data === null || data === void 0 ? void 0 : data.domain_id,
                queueList: JSON.stringify(queueListData)
            };
            try {
                yield (0, webChat_dao_1.insertUpdateQueueWaitTimeDao)(payload);
                db_1.ioredisChat.to(data === null || data === void 0 ? void 0 : data.session_id).emit('changeFallBackQueue', payload);
            }
            catch (err) {
                console.log(err);
            }
            db_1.ioredisChat.emit('callQueueWait', payload);
            const redisData = {
                qid: queueId,
                queueType: 'chat',
                domainId: data === null || data === void 0 ? void 0 : data.domain_id,
            };
            const redisQueueData = yield db_1.queueGetter.get('queuedCustomer');
            if (redisQueueData) {
                const redisMap = JSON.parse(redisQueueData);
                const hasQid = redisMap === null || redisMap === void 0 ? void 0 : redisMap.some((item) => item.qid === queueId);
                if (!hasQid) {
                    redisMap.push(redisData);
                    db_1.queueGetter.set('queuedCustomer', JSON.stringify(redisMap));
                }
            }
            else {
                db_1.queueGetter.set('queuedCustomer', JSON.stringify([redisData]));
            }
        }
        else {
            data.queueId = waitTimmer === null || waitTimmer === void 0 ? void 0 : waitTimmer.overFlowQueue;
            (0, exports.insertCustomerInQueue)(data, currentData, waitTimmer === null || waitTimmer === void 0 ? void 0 : waitTimmer.overFlowQueue);
        }
        return;
    }
    catch (err) {
        console.log(err);
    }
});
exports.insertCustomerInQueue = insertCustomerInQueue;
// Transfer chat for queued customer
const transferChatForQueuedCustomer = (channelId, currentNode, agentExtList) => __awaiter(void 0, void 0, void 0, function* () {
    var _u, _v;
    try {
        console.log('agentExtList::::::', agentExtList);
        let agentDetails = [];
        // const fullAgentDetails: any = [];
        if (((_v = (_u = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _u === void 0 ? void 0 : _u.information) === null || _v === void 0 ? void 0 : _v.type) === 1) {
            const readyAgentsData = yield readyAgents(channelId);
            console.log('\n readyAgentsData\n', JSON.stringify(readyAgentsData));
            if ((readyAgentsData === null || readyAgentsData === void 0 ? void 0 : readyAgentsData.length) !== 0) {
                agentDetails = yield queueChatQueued(channelId, currentNode, readyAgentsData);
                // fullAgentDetails.push([...agentDetails]);
                console.log('\ntransferChat readyAgentsData agentDetails\n', JSON.stringify(agentDetails));
            }
            if (agentDetails.length === 0) {
                const readySupervisorsData = yield readySupervisors(channelId);
                console.log('\n readySupervisorsData\n', JSON.stringify(readySupervisorsData));
                if ((readySupervisorsData === null || readySupervisorsData === void 0 ? void 0 : readySupervisorsData.length) !== 0) {
                    agentDetails = yield queueChatQueued(channelId, currentNode, readySupervisorsData);
                    // fullAgentDetails.push([...agentDetails]);
                    console.log('\ntransferChat readySupervisorsData agentDetails\n', JSON.stringify(agentDetails));
                }
            }
            if (agentDetails.length === 0) {
                const readyAdminsData = yield readyAdmins(channelId);
                console.log('\n readyAdminsData\n', JSON.stringify(readyAdminsData));
                if ((readyAdminsData === null || readyAdminsData === void 0 ? void 0 : readyAdminsData.length) !== 0) {
                    agentDetails = yield queueChatQueued(channelId, currentNode, readyAdminsData);
                    // fullAgentDetails.push([...agentDetails]);
                    console.log('\ntransferChat readyAdminsData agentDetails\n', JSON.stringify(agentDetails));
                }
            }
        }
        return agentDetails;
    }
    catch (err) {
        console.log('err', err);
        return [];
    }
});
exports.transferChatForQueuedCustomer = transferChatForQueuedCustomer;
const queueChatQueued = (channelId, currentNode, readyAgentsData) => __awaiter(void 0, void 0, void 0, function* () {
    try {
        let filteredAgents = [];
        let skillRoutingAgent = [];
        const routingProfiles = yield (0, callflow_dao_1.getRoutingProfiles)(channelId === null || channelId === void 0 ? void 0 : channelId.queueId, channelId === null || channelId === void 0 ? void 0 : channelId.domain_id); // getting routing profiles mapped to queue
        console.log('\n queueChat routingProfiles\n', JSON.stringify(routingProfiles));
        if ((routingProfiles === null || routingProfiles === void 0 ? void 0 : routingProfiles.length) === 0) {
            return [];
        }
        const sortedRoutingProfiles = routingProfiles.sort((a, b) => a.priority - b.priority); // sorting routing profiles based on priority
        for (const profile of sortedRoutingProfiles) {
            filteredAgents = readyAgentsData.filter((a) => (a === null || a === void 0 ? void 0 : a.routingProfileId) === (profile === null || profile === void 0 ? void 0 : profile.rqmid) && (profile === null || profile === void 0 ? void 0 : profile.chat) === 1);
            console.log('\n queueChat filteredAgents\n', JSON.stringify(filteredAgents));
            if ((filteredAgents === null || filteredAgents === void 0 ? void 0 : filteredAgents.length) === 0) {
                continue;
            }
            const getRouting = yield (0, callflow_dao_1.getRoutingDetails)(profile === null || profile === void 0 ? void 0 : profile.rqmid, channelId === null || channelId === void 0 ? void 0 : channelId.domain_id);
            console.log('\n queueChat getRouting\n', JSON.stringify(getRouting));
            if ((getRouting === null || getRouting === void 0 ? void 0 : getRouting.routingMethod) !== 'Long wait routing') {
                skillRoutingAgent = yield skillRouting(channelId, filteredAgents);
                if ((skillRoutingAgent === null || skillRoutingAgent === void 0 ? void 0 : skillRoutingAgent.length) <= 0) {
                    return [];
                }
                filteredAgents = [...skillRoutingAgent];
                console.log('\n\n queueChat skillRoutingAgent\n', JSON.stringify(skillRoutingAgent));
            }
            const longWaitRoutingAgent = yield (0, exports.longWaitRouting)(channelId, currentNode, filteredAgents);
            console.log('\n\n queuChat longWaitRoutingAgent\n', JSON.stringify(longWaitRoutingAgent));
            if ((longWaitRoutingAgent === null || longWaitRoutingAgent === void 0 ? void 0 : longWaitRoutingAgent.length) > 0) {
                return longWaitRoutingAgent;
            }
        }
        return [];
    }
    catch (err) {
        console.log('\n queueChat err\n', err);
        return [];
    }
});
/**
 *
 * @param room
 * @returns
 */
const sendLiveNotification = (room) => __awaiter(void 0, void 0, void 0, function* () {
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        let message = {};
        try {
            message = {
                messageID: (0, uuid_1.v4)(),
                sessionId: room === null || room === void 0 ? void 0 : room.sessionId,
                message: 'Connecting to live agent...',
                sendAt: Date.now(),
                messageType: 'text',
                pdfurl: '',
                isBot: true,
                isCustomer: false,
                fileType: '',
                direction: 'OUT',
                isLike: 0
            };
            const messageList = { message };
            const messageType = { agentText: messageList };
            const stringifiedMessage = { message: messageType };
            stringifiedMessage.historyId = room === null || room === void 0 ? void 0 : room.historyId;
            yield (0, webChat_dao_1.updateWhatsappChatMessage)(stringifiedMessage);
        }
        catch (error) {
            console.log(error);
            reject(error);
        }
    }));
});
exports.sendLiveNotification = sendLiveNotification;
/**
 *
 * @param room
 * @returns
 */
const sendLiveNotificationFb = (room) => __awaiter(void 0, void 0, void 0, function* () {
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        try {
            let message = {};
            message = {
                messageID: (0, uuid_1.v4)(),
                sessionId: room === null || room === void 0 ? void 0 : room.sessionId,
                message: 'Connecting to live agent...',
                sendAt: Date.now(),
                messageType: 'text',
                sendMediaUrl: '',
                isBot: true,
                isCustomer: false,
                fileType: '',
                direction: 'OUT',
                isLike: 0
            };
            const messageList = { message };
            const messageType = { agentText: messageList };
            const stringifiedMessage = { message: messageType };
            stringifiedMessage.historyId = room === null || room === void 0 ? void 0 : room.historyId;
            yield (0, webChat_dao_1.updateChatMessageFb)(stringifiedMessage);
        }
        catch (error) {
            console.log(error);
            reject(error);
        }
    }));
});
exports.sendLiveNotificationFb = sendLiveNotificationFb;
/**
 *
 * @param room
 */
const sendLiveNotificationInsta = (room) => __awaiter(void 0, void 0, void 0, function* () {
    return new Promise((resolve, reject) => __awaiter(void 0, void 0, void 0, function* () {
        try {
            let message = {};
            message = {
                messageID: (0, uuid_1.v4)(),
                sessionId: room === null || room === void 0 ? void 0 : room.sessionId,
                message: 'Connecting to live agent...',
                sendAt: Date.now(),
                messageType: 'text',
                sendMediaUrl: '',
                isBot: true,
                isCustomer: false,
                fileType: '',
                direction: 'OUT',
                isLike: 0
            };
            const messageList = { message };
            const messageType = { agentText: messageList };
            const stringifiedMessage = { message: messageType };
            stringifiedMessage.historyId = room === null || room === void 0 ? void 0 : room.historyId;
            yield (0, socialmedia_1.updateChatMessageInsta)(stringifiedMessage);
        }
        catch (error) {
            console.log(error);
            reject(error);
        }
    }));
});
exports.sendLiveNotificationInsta = sendLiveNotificationInsta;
//# sourceMappingURL=transferToQueue.js.map