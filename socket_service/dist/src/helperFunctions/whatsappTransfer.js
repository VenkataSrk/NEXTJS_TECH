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
exports.transferChat = exports.transferChatToAvailableAgent = void 0;
const callflow_dao_1 = require("../dao/callflow.dao");
const chat_dao_1 = require("../dao/chat.dao");
const socialmedia_1 = require("../dao/socialmedia");
const webChat_dao_1 = require("../dao/webChat.dao");
const db_1 = require("../plugins/db");
/**
 *
 * @param channelId
 * @returns
 */
const transferChatToAvailableAgent = (channelId) => __awaiter(void 0, void 0, void 0, function* () {
    try {
        let agentDetails = [];
        const readyAgentsData = yield readyAgents(channelId); // checking agents in ready state
        console.log('\n readyAgentsData\n', JSON.stringify(readyAgentsData));
        if ((readyAgentsData === null || readyAgentsData === void 0 ? void 0 : readyAgentsData.length) !== 0) { // Connecting chat with agents
            agentDetails = yield longWaitRouting(channelId, undefined, readyAgentsData);
            console.log('\n\n liveagent agentDetails\n', JSON.stringify(agentDetails));
        }
        if (agentDetails.length === 0) {
            const readySupervisorsData = yield readySupervisors(channelId); // checking supervisors in ready state
            console.log('\n readySupervisorsData\n', JSON.stringify(readySupervisorsData));
            if ((readySupervisorsData === null || readySupervisorsData === void 0 ? void 0 : readySupervisorsData.length) !== 0) { // Connecting chat with supervisors
                agentDetails = yield longWaitRouting(channelId, undefined, readySupervisorsData);
                console.log('\n\n liveagent agentDetails\n', JSON.stringify(agentDetails));
            }
        }
        if (agentDetails.length === 0) {
            const readyAdminsData = yield readyAdmins(channelId); // checking admins in ready state
            console.log('\n readyAdminsData\n', JSON.stringify(readyAdminsData));
            if ((readyAdminsData === null || readyAdminsData === void 0 ? void 0 : readyAdminsData.length) !== 0) { // Connecting chat with admins
                agentDetails = yield longWaitRouting(channelId, undefined, readyAdminsData);
                console.log('\n\n liveagent agentDetails\n', JSON.stringify(agentDetails));
            }
        }
        return agentDetails;
    }
    catch (err) {
        return [];
    }
});
exports.transferChatToAvailableAgent = transferChatToAvailableAgent;
const transferChat = (channelId, currentNode) => __awaiter(void 0, void 0, void 0, function* () {
    var _a, _b, _c, _d;
    try {
        console.log('\n transferChat currentNode\n', JSON.stringify(currentNode));
        console.log('\n transferChat channelId\n', JSON.stringify(channelId));
        let agentDetails = [];
        if (((_b = (_a = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _a === void 0 ? void 0 : _a.information) === null || _b === void 0 ? void 0 : _b.type) === 2) { // To direct agent
            const readyAgentsSupervisorsAdminsData = yield readyAgentsSupervisorsAdmins(channelId); // checking agents in ready state
            console.log('\n readyAgentsSupervisorsAdminsData\n', JSON.stringify(readyAgentsSupervisorsAdminsData));
            if ((readyAgentsSupervisorsAdminsData === null || readyAgentsSupervisorsAdminsData === void 0 ? void 0 : readyAgentsSupervisorsAdminsData.length) === 0) { // returning when no agents available
                return agentDetails;
            }
            const filteredAgent = readyAgentsSupervisorsAdminsData === null || readyAgentsSupervisorsAdminsData === void 0 ? void 0 : readyAgentsSupervisorsAdminsData.filter((val) => { var _a, _b; return (val === null || val === void 0 ? void 0 : val.userId) === ((_b = (_a = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _a === void 0 ? void 0 : _a.information) === null || _b === void 0 ? void 0 : _b.transferTo); }); // checking agent ready status
            agentDetails = agentChatLimitFilter(channelId, currentNode, filteredAgent[0]);
        }
        if (((_d = (_c = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _c === void 0 ? void 0 : _c.information) === null || _d === void 0 ? void 0 : _d.type) === 1) { // To queue
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
/**
 *
 * @param channel
 * @returns
 */
const readyAgentsSupervisorsAdmins = (channel) => __awaiter(void 0, void 0, void 0, function* () {
    try {
        const readyAgentsSupervisorsAdminsList = [];
        const data = yield db_1.otherRedisClient.hgetall(`${channel === null || channel === void 0 ? void 0 : channel.domainId}_presence`);
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
/**
 *
 * @param channel
 * @returns
 */
const readyAgents = (channel) => __awaiter(void 0, void 0, void 0, function* () {
    try {
        const readyAgentsList = [];
        const data = yield db_1.otherRedisClient.hgetall(`${channel === null || channel === void 0 ? void 0 : channel.domainId}_presence`);
        const result = Object.values(data).map((value) => JSON.parse(value));
        if (!data) {
            return readyAgentsList;
        }
        for (const item of result) {
            const getData = yield (0, chat_dao_1.getUserList)(item);
            if (Number(item === null || item === void 0 ? void 0 : item.isChat) === 1 && Number(item === null || item === void 0 ? void 0 : item.isChatTransfer) === 0 && Number(item === null || item === void 0 ? void 0 : item.roleId) === 6) {
                getData.forEach((dataq) => {
                    if (Number(item.ext) === dataq.ext && dataq.socialMedia === 1) {
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
/**
 *
 * @param channel
 * @returns
 */
const readySupervisors = (channel) => __awaiter(void 0, void 0, void 0, function* () {
    try {
        const readyAgentsList = [];
        const data = yield db_1.otherRedisClient.hgetall(`${channel === null || channel === void 0 ? void 0 : channel.domainId}_presence`);
        const result = Object.values(data).map((value) => JSON.parse(value));
        if (!data) {
            return readyAgentsList;
        }
        for (const item of result) {
            const getData = yield (0, chat_dao_1.getUserList)(item);
            if (Number(item === null || item === void 0 ? void 0 : item.isChat) === 1 && Number(item === null || item === void 0 ? void 0 : item.isChatTransfer) === 0 && Number(item === null || item === void 0 ? void 0 : item.roleId) === 5) {
                getData.forEach((dataa) => {
                    if (Number(item.ext) === dataa.ext && dataa.socialMedia === 1) {
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
        const data = yield db_1.otherRedisClient.hgetall(`${channel === null || channel === void 0 ? void 0 : channel.domainId}_presence`);
        if (!data) {
            return readyAgentsList;
        }
        const result = Object.values(data).map(value => JSON.parse(value));
        for (const item of result) {
            const getData = yield (0, chat_dao_1.getUserList)(item);
            if (Number(item === null || item === void 0 ? void 0 : item.isChat) === 1 && Number(item === null || item === void 0 ? void 0 : item.isChatTransfer) === 0 && Number(item === null || item === void 0 ? void 0 : item.roleId) === 3) {
                getData.forEach((datav) => {
                    if (Number(item.ext) === datav.ext && datav.socialMedia === 1) {
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
const longWaitRouting = (channelId, currentNode, readyAgentsData) => __awaiter(void 0, void 0, void 0, function* () {
    var _e;
    try {
        const sortedAgentsBasedOnTime = readyAgentsData.sort((a, b) => a.stateTime - b.stateTime);
        console.log('\n longWaitRouting sortedAgentsBasedOnTime\n', JSON.stringify(sortedAgentsBasedOnTime));
        const sortedAgentsBasedOnRole = sortedAgentsBasedOnTime.sort((a, b) => b.roleId - a.roleId);
        console.log('\n longWaitRouting sortedAgentsBasedOnRole\n', JSON.stringify(sortedAgentsBasedOnRole));
        let filteredNotifiedAgents = [];
        const previousStatus = yield (0, callflow_dao_1.getPreviousStatus)(channelId === null || channelId === void 0 ? void 0 : channelId.historyId, channelId === null || channelId === void 0 ? void 0 : channelId.domainId); // chat notified to agents list previously
        console.log('\n longWaitRouting previousStatus\n', previousStatus);
        if ((previousStatus === null || previousStatus === void 0 ? void 0 : previousStatus.length) !== 0) {
            // redirecting from other agents
            const verifyArr = JSON.parse((_e = previousStatus[0]) === null || _e === void 0 ? void 0 : _e.chatuserlog);
            console.log('\n longWaitRouting verifyArr\n', verifyArr);
            filteredNotifiedAgents = sortedAgentsBasedOnRole.filter((val) => !(verifyArr.includes(+(val === null || val === void 0 ? void 0 : val.ext))));
            if ((filteredNotifiedAgents === null || filteredNotifiedAgents === void 0 ? void 0 : filteredNotifiedAgents.length) === 0) {
                return [];
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
/**
 *
 * @param channel
 * @param currentNode
 * @param filteredData
 * @param previousStatus
 * @returns
 */
const chatLimitFilter = (channel, currentNode, filteredData, previousStatus) => __awaiter(void 0, void 0, void 0, function* () {
    var _f;
    try {
        const agentDetails = [];
        const chatLimitData = yield (0, callflow_dao_1.getConcurrentChat)(channel.domainId);
        console.log('\n chatLimitFilter chatLimitData\n', JSON.stringify(chatLimitData));
        // {"domainId":7288,"isAllAgent":1,"allAgentLimit":2,"isAgent":0,"isRoutingProfile":0,"agentOccupyDetails":null}
        if (!chatLimitData) {
            console.log('\n *** Chat limit is not set ***\n');
            const sendAgent = yield sendAgentDetails(channel, filteredData, undefined);
            console.log('\n chatLimitFiltersendAgent\n', JSON.stringify(sendAgent));
            return sendAgent;
        }
        const parsedChatLimitData = JSON.parse((_f = chatLimitData === null || chatLimitData === void 0 ? void 0 : chatLimitData.agentOccupyDetails) !== null && _f !== void 0 ? _f : 'null') || null;
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
            chatLimitByAgent = yield (0, callflow_dao_1.getAgentByChat)(channel.domainId);
            console.log('\n chatLimitFilter chatLimitByAgent\n', JSON.stringify(chatLimitByAgent));
        }
        else if (chatLimitData === null || chatLimitData === void 0 ? void 0 : chatLimitData.isRoutingProfile) {
            routingProfileUser = yield (0, callflow_dao_1.getRoutingProfileUser)(channel.domainId);
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
const agentChatLimitFilter = (channel, currentNode, filteredData) => __awaiter(void 0, void 0, void 0, function* () {
    var _g, _h, _j, _k, _l;
    try {
        const chatLimitData = yield (0, callflow_dao_1.getConcurrentChat)(channel.domainId);
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
            chatLimitByAgent = yield (0, callflow_dao_1.getAgentByChat)(channel.domainId);
            console.log('\n chatLimitFilter chatLimitByAgent\n', JSON.stringify(chatLimitByAgent));
            filteredAgentLimit = chatLimitByAgent.filter((val) => +(val === null || val === void 0 ? void 0 : val.ext) === +(filteredData === null || filteredData === void 0 ? void 0 : filteredData.ext));
            console.log('filteredAgentLimit', filteredAgentLimit);
        }
        else if (chatLimitData === null || chatLimitData === void 0 ? void 0 : chatLimitData.isRoutingProfile) {
            routingProfileUser = yield (0, callflow_dao_1.getRoutingProfileUser)(channel.domainId);
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
                    console.log('isAgent before', (_h = filteredAgentLimit[0]) === null || _h === void 0 ? void 0 : _h.chatLimit, value);
                    if (+((_j = filteredAgentLimit[0]) === null || _j === void 0 ? void 0 : _j.chatLimit) > +(value)) {
                        console.log('allAgentLimit less than value');
                        return [filteredData];
                    }
                }
                else if (chatLimitData === null || chatLimitData === void 0 ? void 0 : chatLimitData.isRoutingProfile) {
                    console.log('routingprofile limit matched');
                    console.log('routingprofile before', (_k = filteredRoutingProfile[0]) === null || _k === void 0 ? void 0 : _k.concurrentChat, value);
                    if (+((_l = filteredRoutingProfile[0]) === null || _l === void 0 ? void 0 : _l.concurrentChat) > +(value)) {
                        console.log('allAgentLimit less than value');
                        return [filteredData];
                    }
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
/**
 *
 * @param channelId
 * @param currentNode
 * @param readyAgentsData
 * @returns
 */
const queueChat = (channelId, currentNode, readyAgentsData) => __awaiter(void 0, void 0, void 0, function* () {
    var _m, _o, _p, _q, _r, _s, _t, _u, _v, _w;
    try {
        let filteredAgents = [];
        let skillRoutingAgent = [];
        const routingProfiles = yield (0, callflow_dao_1.getRoutingProfiles)((_o = (_m = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _m === void 0 ? void 0 : _m.information) === null || _o === void 0 ? void 0 : _o.transferTo, channelId === null || channelId === void 0 ? void 0 : channelId.domainId); // getting routing profiles mapped to queue
        console.log('\n queueChat routingProfiles\n', JSON.stringify(routingProfiles));
        if ((channelId === null || channelId === void 0 ? void 0 : channelId.channelType) === 'Whatsapp') {
            yield (0, webChat_dao_1.updateQueueId)(channelId, (_q = (_p = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _p === void 0 ? void 0 : _p.information) === null || _q === void 0 ? void 0 : _q.transferTo);
        }
        else if ((channelId === null || channelId === void 0 ? void 0 : channelId.channelType) === 'Facebook') {
            yield (0, webChat_dao_1.updateQueueIdFacebook)(channelId, (_s = (_r = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _r === void 0 ? void 0 : _r.information) === null || _s === void 0 ? void 0 : _s.transferTo);
        }
        else {
            yield (0, socialmedia_1.updateQueueIdInsta)(channelId, (_u = (_t = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _t === void 0 ? void 0 : _t.information) === null || _u === void 0 ? void 0 : _u.transferTo);
        }
        if ((routingProfiles === null || routingProfiles === void 0 ? void 0 : routingProfiles.length) === 0) {
            return [];
        }
        const sortedRoutingProfiles = routingProfiles.sort((a, b) => a.priority - b.priority); // sorting routing profiles based on priority
        for (const profile of sortedRoutingProfiles) {
            filteredAgents = readyAgentsData.filter((a) => (a === null || a === void 0 ? void 0 : a.routingProfileId) === (profile === null || profile === void 0 ? void 0 : profile.rqmid));
            console.log('\n queueChat filteredAgents\n', JSON.stringify(filteredAgents));
            if ((filteredAgents === null || filteredAgents === void 0 ? void 0 : filteredAgents.length) === 0) {
                continue;
            }
            const getRouting = yield (0, callflow_dao_1.getRoutingDetails)(profile === null || profile === void 0 ? void 0 : profile.rqmid, channelId === null || channelId === void 0 ? void 0 : channelId.domainId);
            console.log('\n queueChat getRouting\n', JSON.stringify(getRouting));
            if ((getRouting === null || getRouting === void 0 ? void 0 : getRouting.routingMethod) !== 'Long wait routing') {
                currentNode.skill = (_w = (_v = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _v === void 0 ? void 0 : _v.information) === null || _w === void 0 ? void 0 : _w.skill;
                currentNode.domainId = channelId === null || channelId === void 0 ? void 0 : channelId.domainId;
                skillRoutingAgent = yield skillRouting(currentNode, filteredAgents);
                if ((skillRoutingAgent === null || skillRoutingAgent === void 0 ? void 0 : skillRoutingAgent.length) <= 0) {
                    return [];
                }
                filteredAgents = [...skillRoutingAgent];
                console.log('\n\n queueChat skillRoutingAgent\n', JSON.stringify(skillRoutingAgent));
            }
            const longWaitRoutingAgent = yield longWaitRouting(channelId, currentNode, filteredAgents);
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
 * @param channelId
 * @param readyAgentsData
 * @returns
 */
const skillRouting = (channelId, readyAgentsData) => __awaiter(void 0, void 0, void 0, function* () {
    try {
        const skills = channelId.skill;
        console.log('\n skillRouting skills--------------->', skills);
        const getAgentSkills = yield (0, callflow_dao_1.getAgentsSkills)(channelId.domainId, '1,2,3,5,6');
        console.log('\n skillRouting getAgentSkills------------->', JSON.stringify(getAgentSkills));
        const skilledAgentsExt = [];
        for (const skill of skills) {
            console.log('first loop');
            getAgentSkills.filter((agent) => {
                if (agent === null || agent === void 0 ? void 0 : agent.addSkill.includes(skill)) {
                    skilledAgentsExt.push(+(agent === null || agent === void 0 ? void 0 : agent.ext));
                }
            });
        }
        console.log('skilledAgentsExt', skilledAgentsExt);
        const filteredUniqueExt = [...new Set(skilledAgentsExt)];
        console.log('filteredUniqueExt', filteredUniqueExt);
        const skilledAgents = [];
        filteredUniqueExt.map((val) => {
            for (const agent of readyAgentsData) {
                if (+(val) === +(agent.ext)) {
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
/**
 *
 * @param channelId
 * @param agent
 * @param previousStatus
 * @returns
 */
const sendAgentDetails = (channelId, agent, previousStatus) => __awaiter(void 0, void 0, void 0, function* () {
    var _x;
    try {
        if ((previousStatus === null || previousStatus === void 0 ? void 0 : previousStatus.length) === 0 || previousStatus === undefined) {
            const insertArr = [+(agent === null || agent === void 0 ? void 0 : agent.ext)];
            const insertNewStatus = yield (0, callflow_dao_1.insertStatus)(channelId === null || channelId === void 0 ? void 0 : channelId.historyId, channelId === null || channelId === void 0 ? void 0 : channelId.domainId, insertArr);
            console.log('\n sendAgentDetails insertNewStatus\n', insertNewStatus);
        }
        else {
            const verifyArr = JSON.parse((_x = previousStatus[0]) === null || _x === void 0 ? void 0 : _x.chatuserlog);
            const insertArr = [...verifyArr, +(agent === null || agent === void 0 ? void 0 : agent.ext)];
            const insertNewStatus = yield (0, callflow_dao_1.insertStatus)(channelId === null || channelId === void 0 ? void 0 : channelId.historyId, channelId === null || channelId === void 0 ? void 0 : channelId.domainId, insertArr);
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
//# sourceMappingURL=whatsappTransfer.js.map