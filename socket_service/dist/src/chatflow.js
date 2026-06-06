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
exports.ChatFlowAdapters = void 0;
const chatflow_helpers_1 = require("./chatflow_helpers");
const callflow_dao_1 = require("./dao/callflow.dao");
const fetch_api_1 = require("./fetch_api");
const uuid_1 = require("uuid");
const conditions_mapper_1 = require("./conditions_mapper");
// import { createSession } from './dao/chat.dao';
const webChat_dao_1 = require("./dao/webChat.dao");
const db_1 = require("./plugins/db");
class ChatFlowAdapters {
    constructor(adapter) {
        this.getNodeDataById = (nodeid, node) => {
            return node === null || node === void 0 ? void 0 : node.find((list) => list.id === nodeid);
        };
        this.getStartNode = (node) => {
            return node === null || node === void 0 ? void 0 : node.find((list) => list.type === 'entryPoint');
        };
        this.getCustomhours = (nexttargetedge, msg) => {
            return nexttargetedge.filter((list) => list.sourceHandle === msg);
        };
        this.gettargetedges = (edgeid, edge) => {
            return edge === null || edge === void 0 ? void 0 : edge.find((list) => list.source === edgeid);
        };
        this.redisadaptor = adapter;
        this.chatAdapter = adapter;
        this.channelInformation = null;
        this.channelInformation = null;
        this.currentNodeInfo = null;
        this.blpopTimeout = false;
        this.successTimeout = false;
        this.responseTimeout = false;
        this.secondResponseTimeout = false;
        this.playpromptSuccessTimeout = false;
        this.businessHoursTimeout = false;
        this.dtmfSuccessTimeout = false;
        this.context = {};
        this.sessionLoops = {};
        this.callflowId = null;
        this.nodes = [];
        this.inputParams = '';
        this.sourceLang = 'en';
        ChatFlowAdapters.queueDetails = null;
    }
    addContextVariable(key, value) {
        if (this.context) {
            this.context[key] = value;
        }
    }
    updateQueueDetails(channelId) {
        var _a, _b;
        // this.chatAdapter.broadcastMessage("waitingQueueUpdate", channelId?.queueUpdate);
        const queueData = JSON.parse((_a = channelId === null || channelId === void 0 ? void 0 : channelId.queueUpdate) !== null && _a !== void 0 ? _a : 'null');
        ChatFlowAdapters.queueDetails = (_b = queueData === null || queueData === void 0 ? void 0 : queueData.queueList) !== null && _b !== void 0 ? _b : null;
    }
    clearAllTimeout() {
        this.blpopTimeout = true;
        this.successTimeout = true;
        this.responseTimeout = true;
        this.secondResponseTimeout = true;
        this.playpromptSuccessTimeout = true;
        this.dtmfSuccessTimeout = true;
        this.businessHoursTimeout = true;
    }
    setAllTimeout() {
        this.blpopTimeout = false;
        this.successTimeout = false;
        this.responseTimeout = false;
        this.secondResponseTimeout = false;
        this.playpromptSuccessTimeout = false;
        this.dtmfSuccessTimeout = false;
        this.businessHoursTimeout = false;
    }
    ChatFlowHandler(channel) {
        var _a, _b, _c, _d, _e, _f, _g, _h, _j, _k, _l, _m;
        return __awaiter(this, void 0, void 0, function* () {
            if (channel && JSON.stringify(this.channelInformation) !== JSON.stringify(channel) && !((_a = JSON.parse(channel !== null && channel !== void 0 ? channel : 'null')) === null || _a === void 0 ? void 0 : _a.queueUpdate)) {
                this.channelInformation = (_b = JSON.parse(channel !== null && channel !== void 0 ? channel : 'null')) !== null && _b !== void 0 ? _b : '';
            }
            console.log('ChatFlowHandler >>>>>>', this.channelInformation);
            console.log('this.callflowId >>>>>>', this.callflowId);
            const io = db_1.ioredisChat;
            const channelId = JSON.parse(channel !== null && channel !== void 0 ? channel : 'null');
            // await ChatFlowChatStateHandler(channelId, this.chatAdapter);
            // channelId.targetLang = 'ta'
            if (!this.callflowId) {
                this.callflowId = channelId === null || channelId === void 0 ? void 0 : channelId.callFlowId;
            }
            if (this.callflowId) {
                if (this.nodes && ((_c = this.nodes) === null || _c === void 0 ? void 0 : _c.length) === 0) {
                    console.log('this.callflowId88888888 >>>>>>', this.callflowId);
                    const nodesString = yield (0, callflow_dao_1.getnodesdata)(this.callflowId);
                    // console.log('this.nodes >>>>>>', nodesString);
                    this.nodes = JSON.parse(nodesString);
                }
                if ((channelId === null || channelId === void 0 ? void 0 : channelId.channelType) === 'Chat' && (channelId === null || channelId === void 0 ? void 0 : channelId.call_state) === 'init' && channelId.liveTransfer !== 1) {
                    const startingNode = yield this.getStartNode(this.nodes);
                    const currentNodeId = ((_e = (_d = startingNode === null || startingNode === void 0 ? void 0 : startingNode.data) === null || _d === void 0 ? void 0 : _d.children) === null || _e === void 0 ? void 0 : _e.length) > 0 ? (_g = (_f = startingNode === null || startingNode === void 0 ? void 0 : startingNode.data) === null || _f === void 0 ? void 0 : _f.children[0]) === null || _g === void 0 ? void 0 : _g.childId : '';
                    return this.handleChatFlowType(currentNodeId, channelId, this.nodes, io);
                }
                {
                    let nextNode = channelId.btnValue;
                    if (channelId.inputParams)
                        this.inputParams = channelId.inputParams;
                    if (channelId.btnValue && ((_h = channelId === null || channelId === void 0 ? void 0 : channelId.btnValue) === null || _h === void 0 ? void 0 : _h.includes('transferToQueue'))) {
                        const nextNodedata = (_k = (_j = this.nodes) === null || _j === void 0 ? void 0 : _j.find((nodes) => { var _a; return ((_a = nodes === null || nodes === void 0 ? void 0 : nodes.data) === null || _a === void 0 ? void 0 : _a.nodeId) === (channelId === null || channelId === void 0 ? void 0 : channelId.btnValue); })) !== null && _k !== void 0 ? _k : { id: '' };
                        nextNode = nextNodedata.id;
                    }
                    if (channelId.liveTransfer !== 1) {
                        return this.handleChatFlowType(nextNode, channelId, this.nodes, io);
                    }
                    {
                        // const checkBusinessHours: any = await getChatHoursOfOperation(channelId);
                        // if (checkBusinessHours === true) {
                        console.log('liveTransfer>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>');
                        let liveAgent = [];
                        // liveAgent = await getListChatAgent(channelId);
                        liveAgent = yield this.getAvailableAgentChatList(channelId);
                        console.log('liveTransfer>>>>>>>>>>>>>>>>>>>', liveAgent);
                        if ((liveAgent === null || liveAgent === void 0 ? void 0 : liveAgent.length) === 0) {
                            this.chatAdapter.redisadaptor.resetQueue(channelId === null || channelId === void 0 ? void 0 : channelId.domain_id);
                            console.log('INITIATING AGENT FINDER', liveAgent);
                            this.waitForLiveAgentResponse(channelId);
                            // await new Promise(async (resolve, reject) => {
                            //   try {
                            //     const list = [];
                            //     await this.LIVEBLPOP(channelId?.domain_id, list);
                            //     if (list?.length > 0 && list[0] !== null) {
                            //       liveAgent = list;
                            //       resolve(true);
                            //     } else {
                            //       reject(false);
                            //     }
                            //   } catch (__: any) {
                            //     reject(false);
                            //   }
                            // }).catch((_: any) => {
                            //   console.log('err');
                            // });
                        }
                        if (liveAgent && (liveAgent === null || liveAgent === void 0 ? void 0 : liveAgent.length) > 0) {
                            console.log('FOUND AN AVAILABLE AGENT ...');
                            yield (0, chatflow_helpers_1.handleTransferToLiveAgent)(this.chatAdapter, channelId, null, liveAgent);
                        }
                        else {
                            const getDomainResult = yield (0, callflow_dao_1.getDomainStatus)(channelId === null || channelId === void 0 ? void 0 : channelId.domain_id);
                            (_m = (_l = this.chatAdapter) === null || _l === void 0 ? void 0 : _l.io) === null || _m === void 0 ? void 0 : _m.to(channelId === null || channelId === void 0 ? void 0 : channelId.session_id).emit('chat_response', JSON.stringify({ session_id: channelId === null || channelId === void 0 ? void 0 : channelId.session_id, req_type: 'DISCONNECT', domainStatus: getDomainResult }));
                        }
                        // } else {
                        //   this.chatAdapter?.io?.to(channelId?.session_id).emit('chat_response', JSON.stringify({ session_id: channelId?.session_id, req_type: 'DISCONNECT', checkBusinessHours: false }));
                        // }
                    }
                }
            }
        });
    }
    handleChatFlowType(nodeId, channelId, node, io) {
        var _a, _b, _c, _d, _e, _f, _g, _h, _j, _k, _l, _m, _o, _p, _q, _r, _s, _t, _u, _v, _w, _x, _y, _z, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, _81, _82, _83, _84, _85, _86, _87, _88, _89, _90, _91, _92, _93, _94, _95, _96, _97, _98, _99, _100, _101, _102, _103, _104, _105, _106, _107, _108, _109, _110, _111, _112, _113, _114, _115, _116, _117, _118, _119, _120, _121, _122, _123, _124, _125, _126, _127, _128, _129, _130, _131, _132, _133, _134, _135, _136, _137, _138, _139, _140, _141, _142, _143, _144, _145, _146, _147, _148, _149, _150, _151, _152, _153, _154, _155, _156, _157, _158, _159, _160, _161, _162, _163;
        return __awaiter(this, void 0, void 0, function* () {
            const currentNode = yield this.getNodeDataById(nodeId, node);
            this.currentNodeInfo = currentNode;
            if (this.blpopTimeout || !((_a = this.channelInformation) === null || _a === void 0 ? void 0 : _a.session_id)) {
                const getDomainResult = yield (0, callflow_dao_1.getDomainStatus)(channelId === null || channelId === void 0 ? void 0 : channelId.domain_id);
                (_c = (_b = this.chatAdapter) === null || _b === void 0 ? void 0 : _b.io) === null || _c === void 0 ? void 0 : _c.to(channelId === null || channelId === void 0 ? void 0 : channelId.session_id).emit('chat_response', JSON.stringify({ session_id: channelId === null || channelId === void 0 ? void 0 : channelId.session_id, req_type: 'DISCONNECT', domainStatus: getDomainResult }));
                return;
            }
            console.log('currentNode?.type >>>>', currentNode === null || currentNode === void 0 ? void 0 : currentNode.type);
            switch (currentNode === null || currentNode === void 0 ? void 0 : currentNode.type) {
                case 'entryPoint': {
                    return this.handleChatFlowType(currentNode.data.children.at(0).childId, channelId, node, io);
                    break;
                }
                case 'messageFlow': {
                    try {
                        this.channelInformation = Object.assign(Object.assign({}, this.channelInformation), { response: 'pending' });
                        this.playpromptSuccessTimeout = false;
                        const text = (_e = (_d = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _d === void 0 ? void 0 : _d.information) === null || _e === void 0 ? void 0 : _e.message;
                        const availableContexts = text === null || text === void 0 ? void 0 : text.match(chatflow_helpers_1.contextPattern);
                        const _formattedSelectedKeys = [];
                        if (((_f = this.inputParams) === null || _f === void 0 ? void 0 : _f.length) > 0) {
                            const mergedObject = this.inputParams.reduce((result, currentObj) => {
                                Object.keys(currentObj).forEach((key) => {
                                    result[key] = currentObj[key];
                                });
                                return result;
                            }, {});
                            console.log(mergedObject, 'keykeykeykeymergedObject');
                            for (const [key, value] of Object.entries(mergedObject)) {
                                // console.log(key,"keykeykeykey")
                                /* tslint:disable */
                                _formattedSelectedKeys.push({ tempkey: '${context.' + key + '}', originalkey: key, [key]: value });
                                this.addContextVariable(key, value);
                            }
                        }
                        const updatedMessage = (availableContexts === null || availableContexts === void 0 ? void 0 : availableContexts.length) > 0 ? (0, chatflow_helpers_1.replacechatPatternWithVariables)(availableContexts, text, _formattedSelectedKeys) : text;
                        const data = Object.assign({}, (_g = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _g === void 0 ? void 0 : _g.information);
                        data.message = updatedMessage;
                        const targetRestructured = { currentNode, data, session_id: channelId === null || channelId === void 0 ? void 0 : channelId.session_id };
                        targetRestructured.msg = updatedMessage;
                        const nextNode = (_k = (_j = (_h = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _h === void 0 ? void 0 : _h.children) === null || _j === void 0 ? void 0 : _j.find((nodes) => (nodes === null || nodes === void 0 ? void 0 : nodes.type) === 'success')) !== null && _k !== void 0 ? _k : { childId: '' };
                        const nextNodedetails = yield this.getNodeDataById(nextNode === null || nextNode === void 0 ? void 0 : nextNode.childId, node);
                        targetRestructured.currentNode = nextNodedetails;
                        let message = {};
                        try {
                            if (this.sourceLang !== (channelId === null || channelId === void 0 ? void 0 : channelId.targetLang)) {
                                yield Promise.all((_o = (_m = (_l = targetRestructured === null || targetRestructured === void 0 ? void 0 : targetRestructured.currentNode) === null || _l === void 0 ? void 0 : _l.data) === null || _m === void 0 ? void 0 : _m.children) === null || _o === void 0 ? void 0 : _o.map((list) => __awaiter(this, void 0, void 0, function* () {
                                    const translateResult = yield this.translateItem(list === null || list === void 0 ? void 0 : list.type, channelId);
                                    list.type = translateResult;
                                })));
                                const translateMessageResult = yield this.translateItem(targetRestructured.msg, channelId);
                                message = {
                                    messageID: (0, uuid_1.v4)(),
                                    sessionId: channelId === null || channelId === void 0 ? void 0 : channelId.session_id,
                                    message: translateMessageResult,
                                    sendAt: Date.now(),
                                    messageType: 'text',
                                    pdfurl: '',
                                    isBot: true,
                                    isCustomer: false,
                                    fileType: '',
                                    direction: 'OUT',
                                    isLike: 0
                                };
                            }
                            else {
                                message = {
                                    messageID: (0, uuid_1.v4)(),
                                    sessionId: channelId === null || channelId === void 0 ? void 0 : channelId.session_id,
                                    message: targetRestructured.msg,
                                    sendAt: Date.now(),
                                    messageType: 'text',
                                    pdfurl: '',
                                    isBot: true,
                                    isCustomer: false,
                                    fileType: '',
                                    direction: 'OUT',
                                    isLike: 0
                                };
                            }
                            const messageList = { message };
                            const messageType = { agentText: messageList };
                            const stringifiedMessage = { message: messageType };
                            stringifiedMessage.ChatHistoryId = channelId === null || channelId === void 0 ? void 0 : channelId.ChatHistoryId;
                            stringifiedMessage.ChatDurationHistoryId = channelId === null || channelId === void 0 ? void 0 : channelId.ChatDurationHistoryId;
                            try {
                                yield (0, webChat_dao_1.updateChatMessage)(stringifiedMessage);
                            }
                            catch (err) {
                                console.log(err);
                            }
                            (_q = (_p = this.chatAdapter) === null || _p === void 0 ? void 0 : _p.io) === null || _q === void 0 ? void 0 : _q.to(channelId === null || channelId === void 0 ? void 0 : channelId.session_id).emit('OneToOneChatReceive', message);
                            (_s = (_r = this.chatAdapter) === null || _r === void 0 ? void 0 : _r.io) === null || _s === void 0 ? void 0 : _s.to(channelId === null || channelId === void 0 ? void 0 : channelId.session_id).emit('chat_response', JSON.stringify(targetRestructured));
                        }
                        catch (error) {
                            console.error('Error:', error);
                        }
                        // await this.waitForPlayPromptSuccessResponse();
                        // const nextNode = currentNode?.data?.children?.find((nodes: any) => nodes?.type === "success") ?? { childId: "" };
                        // return this.handleChatFlowType(nextNode?.childId, channelId, node, io);
                    }
                    catch (error) {
                        const nextNode = (_v = (_u = (_t = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _t === void 0 ? void 0 : _t.children) === null || _u === void 0 ? void 0 : _u.find((element) => (element === null || element === void 0 ? void 0 : element.type) === 'error')) !== null && _v !== void 0 ? _v : { childId: '' };
                        return this.handleChatFlowType(nextNode === null || nextNode === void 0 ? void 0 : nextNode.childId, channelId, node, io);
                    }
                    break;
                }
                case 'playPrompt': {
                    try {
                        this.channelInformation = Object.assign(Object.assign({}, this.channelInformation), { response: 'pending' });
                        this.playpromptSuccessTimeout = false;
                        const text = (_x = (_w = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _w === void 0 ? void 0 : _w.information) === null || _x === void 0 ? void 0 : _x.message;
                        const availableContexts = text === null || text === void 0 ? void 0 : text.match(chatflow_helpers_1.contextPattern);
                        const updatedMessage = (availableContexts === null || availableContexts === void 0 ? void 0 : availableContexts.length) > 0 ? (0, chatflow_helpers_1.replacePatternWithVariables)(availableContexts, text, this.context) : text;
                        const data = Object.assign({}, (_y = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _y === void 0 ? void 0 : _y.information);
                        data.message = updatedMessage;
                        const targetRestructured = Object.assign(Object.assign({}, currentNode), { data, session_id: channelId === null || channelId === void 0 ? void 0 : channelId.session_id });
                        (_0 = (_z = this.chatAdapter) === null || _z === void 0 ? void 0 : _z.io) === null || _0 === void 0 ? void 0 : _0.to(channelId === null || channelId === void 0 ? void 0 : channelId.session_id).emit('chat_response', JSON.stringify(targetRestructured));
                        // console.log(currentNode, "currentNodecurrentNode")
                        // await this.waitForPlayPromptSuccessResponse();
                        // console.log(currentNode,"currentNodecurrentNode")
                        const nextNode = (_3 = (_2 = (_1 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _1 === void 0 ? void 0 : _1.children) === null || _2 === void 0 ? void 0 : _2.find((nodes) => (nodes === null || nodes === void 0 ? void 0 : nodes.type) === 'success')) !== null && _3 !== void 0 ? _3 : { childId: '' };
                        return this.handleChatFlowType(nextNode === null || nextNode === void 0 ? void 0 : nextNode.childId, channelId, node, io);
                    }
                    catch (error) {
                        const nextNode = (_6 = (_5 = (_4 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _4 === void 0 ? void 0 : _4.children) === null || _5 === void 0 ? void 0 : _5.find((element) => (element === null || element === void 0 ? void 0 : element.type) === 'error')) !== null && _6 !== void 0 ? _6 : { childId: '' };
                        return this.handleChatFlowType(nextNode === null || nextNode === void 0 ? void 0 : nextNode.childId, channelId, node, io);
                    }
                    break;
                }
                case 'integration': {
                    try {
                        (_8 = (_7 = this.chatAdapter) === null || _7 === void 0 ? void 0 : _7.io) === null || _8 === void 0 ? void 0 : _8.to(channelId === null || channelId === void 0 ? void 0 : channelId.session_id).emit('chat_response', JSON.stringify(currentNode));
                    }
                    catch (error) {
                        const nextNode = (_11 = (_10 = (_9 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _9 === void 0 ? void 0 : _9.children) === null || _10 === void 0 ? void 0 : _10.find((nodes) => (nodes === null || nodes === void 0 ? void 0 : nodes.type) === 'error')) !== null && _11 !== void 0 ? _11 : { childId: '' };
                        return this.handleChatFlowType(nextNode === null || nextNode === void 0 ? void 0 : nextNode.childId, channelId, node, io);
                    }
                    break;
                }
                case 'appointment': {
                    try {
                        (_13 = (_12 = this.chatAdapter) === null || _12 === void 0 ? void 0 : _12.io) === null || _13 === void 0 ? void 0 : _13.to(channelId === null || channelId === void 0 ? void 0 : channelId.session_id).emit('chat_response', JSON.stringify(currentNode));
                    }
                    catch (error) {
                        const nextNode = (_16 = (_15 = (_14 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _14 === void 0 ? void 0 : _14.children) === null || _15 === void 0 ? void 0 : _15.find((nodes) => (nodes === null || nodes === void 0 ? void 0 : nodes.type) === 'error')) !== null && _16 !== void 0 ? _16 : { childId: '' };
                        return this.handleChatFlowType(nextNode === null || nextNode === void 0 ? void 0 : nextNode.childId, channelId, node, io);
                    }
                    break;
                }
                case 'customInput': {
                    try {
                        this.channelInformation = Object.assign(Object.assign({}, this.channelInformation), { response: 'pending' });
                        this.playpromptSuccessTimeout = false;
                        const text = (_18 = (_17 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _17 === void 0 ? void 0 : _17.information) === null || _18 === void 0 ? void 0 : _18.message;
                        const availableContexts = text === null || text === void 0 ? void 0 : text.match(chatflow_helpers_1.contextPattern);
                        const updatedMessage = (availableContexts === null || availableContexts === void 0 ? void 0 : availableContexts.length) > 0 ? (0, chatflow_helpers_1.replacePatternWithVariables)(availableContexts, text, this.context) : text;
                        const data = Object.assign({}, (_19 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _19 === void 0 ? void 0 : _19.information);
                        data.message = updatedMessage;
                        const targetRestructured = { currentNode, data, session_id: channelId === null || channelId === void 0 ? void 0 : channelId.session_id };
                        console.log(' <<<<< targetRestructured >>>>', targetRestructured);
                        yield Promise.all((_22 = (_21 = (_20 = targetRestructured === null || targetRestructured === void 0 ? void 0 : targetRestructured.currentNode) === null || _20 === void 0 ? void 0 : _20.data) === null || _21 === void 0 ? void 0 : _21.children) === null || _22 === void 0 ? void 0 : _22.map((list) => __awaiter(this, void 0, void 0, function* () {
                            const translateResult = yield this.translateItem(list === null || list === void 0 ? void 0 : list.type, channelId);
                            console.log('translateResult>>>>>>>>>>>>>>', list === null || list === void 0 ? void 0 : list.type, translateResult);
                            list.type = translateResult;
                        })));
                        (_25 = (_24 = (_23 = this.chatAdapter) === null || _23 === void 0 ? void 0 : _23.io) === null || _24 === void 0 ? void 0 : _24.of('/')) === null || _25 === void 0 ? void 0 : _25.to(channelId === null || channelId === void 0 ? void 0 : channelId.session_id).emit('chat_response', JSON.stringify(targetRestructured));
                        // this.chatAdapter.io.of("/").to(channelId?.session_id).emit("OneToOneChatReceive", "msg");
                        // await this.waitForPlayPromptSuccessResponse();
                        // const nextNode = currentNode?.data?.children?.find((nodes: any) => nodes?.type === "Health Tips") ?? { childId: "" };
                        // console.log("nextNode >>>>>>", nextNode);
                        // return this.handleChatFlowType(nextNode?.childId, channelId, node, io);
                    }
                    catch (error) {
                        const nextNode = (_28 = (_27 = (_26 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _26 === void 0 ? void 0 : _26.children) === null || _27 === void 0 ? void 0 : _27.find((element) => (element === null || element === void 0 ? void 0 : element.type) === 'error')) !== null && _28 !== void 0 ? _28 : { childId: '' };
                        return this.handleChatFlowType(nextNode === null || nextNode === void 0 ? void 0 : nextNode.childId, channelId, node, io);
                    }
                    break;
                }
                case 'transferToQueue': {
                    try {
                        console.log('channelId', channelId);
                        console.log('currentNode', currentNode);
                        console.log('TRANSFER USER INTO QUEUE INITIATED...');
                        const getIncomingChatDetails = yield (0, callflow_dao_1.getIncomingChatDetailsDB)(channelId, (_30 = (_29 = currentNode.data) === null || _29 === void 0 ? void 0 : _29.information) === null || _30 === void 0 ? void 0 : _30.transferTo);
                        // const getIncomingChatDetails: any = [];
                        console.log('getIncomingChatDetails', getIncomingChatDetails);
                        const getIncomingChats = JSON.parse((_32 = (_31 = getIncomingChatDetails[0]) === null || _31 === void 0 ? void 0 : _31.queueList) !== null && _32 !== void 0 ? _32 : '[]');
                        console.log('getIncomingChats', getIncomingChats);
                        const incomingChat = getIncomingChats === null || getIncomingChats === void 0 ? void 0 : getIncomingChats.filter((val) => {
                            return (val === null || val === void 0 ? void 0 : val.session_id) == channelId.session_id;
                        });
                        console.log('incomingChat', incomingChat);
                        if (incomingChat.length > 0) {
                            console.log('incomingChat if', incomingChat);
                            channelId.queueList = [...getIncomingChats];
                        }
                        else {
                            console.log('incomingChat else', incomingChat);
                            channelId.queueList = [...getIncomingChats, JSON.parse(JSON.stringify(channelId))];
                        }
                        let insertIncomingChats = yield (0, callflow_dao_1.insertIncomingChatsDB)(channelId, (_34 = (_33 = currentNode.data) === null || _33 === void 0 ? void 0 : _33.information) === null || _34 === void 0 ? void 0 : _34.transferTo);
                        console.log('insertIncomingChats', insertIncomingChats);
                        this.channelInformation = Object.assign(Object.assign({}, this.channelInformation), { response: 'pending' });
                        // const routingProfiles = await getRoutingProfile(currentNode.data?.information?.transferTo);
                        // console.log('routingProfiles ', { routingProfiles });
                        // await transferUserInsideQueue(this.redisadaptor, channelId, currentNode);
                        this.waitForAgentResponse(channelId);
                        let agentDetails = [];
                        if (((_36 = (_35 = currentNode.data) === null || _35 === void 0 ? void 0 : _35.information) === null || _36 === void 0 ? void 0 : _36.type) === 2) {
                            const getAgentResults = yield (0, callflow_dao_1.getTransferAgentStatus)((_38 = (_37 = currentNode.data) === null || _37 === void 0 ? void 0 : _37.information) === null || _38 === void 0 ? void 0 : _38.transferTo, channelId);
                            agentDetails = getAgentResults[0];
                        }
                        else {
                            console.log('TRANSFER TO QUEUE COMPLETED');
                            agentDetails = yield this.getAvaiableAgentChatListBtn(channelId, (_40 = (_39 = currentNode.data) === null || _39 === void 0 ? void 0 : _39.information) === null || _40 === void 0 ? void 0 : _40.transferTo);
                            // agentDetails = await getAgentListChat(currentNode.data?.information?.transferTo, channelId, this.redisadaptor);
                        }
                        console.log('agentDetails >>>>>>>', agentDetails);
                        if ((agentDetails === null || agentDetails === void 0 ? void 0 : agentDetails.length) === 0) {
                            console.log('Inside agentDetails >>>>.', this.chatAdapter.redisadaptor.resetQueue);
                            this.chatAdapter.redisadaptor.resetQueue(channelId === null || channelId === void 0 ? void 0 : channelId.domain_id);
                            console.log('INITIATING AGENT FINDER');
                            // await new Promise(async (resolve, reject) => {
                            //   try {
                            //     const list = [];
                            //     await this.BLPOP(channelId?.domain_id, list, routingProfiles, channelId?.session_id, currentNode.data?.information?.transferTo);
                            //     if (list?.length > 0 && list[0] !== null) {
                            //       agentDetails = list;
                            //       resolve(true);
                            //     } else {
                            //       reject(false);
                            //     }
                            //   } catch (__: any) {
                            //     reject(false);
                            //   }
                            // }).catch((_: any) => {
                            //   const nextNodeData = currentNode?.data?.children?.find((element: any) => element?.type === 'error') ?? { childId: '' };
                            //   return this.handleChatFlowType(nextNodeData?.childId, channelId, node, io);
                            // });
                        }
                        if (agentDetails && (agentDetails === null || agentDetails === void 0 ? void 0 : agentDetails.length) > 0) {
                            agentDetails.queueId = (_42 = (_41 = currentNode.data) === null || _41 === void 0 ? void 0 : _41.information) === null || _42 === void 0 ? void 0 : _42.transferTo;
                            console.log('FOUND AN AVAILABLE AGENT ...');
                            yield (0, chatflow_helpers_1.handleTransferToQueue)(this.chatAdapter, channelId, currentNode, agentDetails);
                            this.channelInformation = Object.assign(Object.assign({}, this.channelInformation), { response: 'success', status: null });
                            const status = yield this.waitForCallStatusResponse();
                            // INFO: PUT THIS BLOCK IN A LOOP TO KEEP CHECKING FOR AVAILABLE USER
                            console.log('status >>>>>>>', status);
                            if (status && ((_44 = (_43 = currentNode.data) === null || _43 === void 0 ? void 0 : _43.information) === null || _44 === void 0 ? void 0 : _44.type) !== 2) {
                                const checkAgentDetails = yield (0, callflow_dao_1.getAgentListChat)((_46 = (_45 = currentNode.data) === null || _45 === void 0 ? void 0 : _45.information) === null || _46 === void 0 ? void 0 : _46.transferTo, channelId, this.redisadaptor);
                                console.log('checkAgentDetails >>>>>>>', checkAgentDetails);
                                if (checkAgentDetails === null || checkAgentDetails === void 0 ? void 0 : checkAgentDetails.length) {
                                    this.channelInformation = Object.assign(Object.assign({}, this.channelInformation), { response: 'pending', status: null });
                                    yield (0, chatflow_helpers_1.handleTransferToQueue)(this.redisadaptor, channelId, currentNode, checkAgentDetails);
                                    const statusNew = yield this.waitForSecondCallStatusResponse();
                                    if (statusNew) {
                                        console.log('channelId?.channeltype >>>>>>', channelId === null || channelId === void 0 ? void 0 : channelId.channeltype);
                                        if ((channelId === null || channelId === void 0 ? void 0 : channelId.channeltype) === 'Chat') {
                                            (_49 = (_48 = (_47 = this.chatAdapter) === null || _47 === void 0 ? void 0 : _47.io) === null || _48 === void 0 ? void 0 : _48.of('/')) === null || _49 === void 0 ? void 0 : _49.to(channelId === null || channelId === void 0 ? void 0 : channelId.session_id).emit('chat_response', JSON.stringify(agentDetails));
                                        }
                                        else {
                                            // this.redisadaptor.publishMessage(channelId?.domain_id, JSON.stringify({ session_id: channelId?.session_id, req_type: 'DISCONNECT' }));
                                        }
                                    }
                                }
                                else {
                                    console.log('channelId?.channeltype >>>>>>', channelId === null || channelId === void 0 ? void 0 : channelId.channeltype);
                                    if ((channelId === null || channelId === void 0 ? void 0 : channelId.channeltype) === 'Chat') {
                                        console.log('Inside chant', this.redisadaptor.publishMessageToWebChat);
                                        (_52 = (_51 = (_50 = this.chatAdapter) === null || _50 === void 0 ? void 0 : _50.io) === null || _51 === void 0 ? void 0 : _51.of('/')) === null || _52 === void 0 ? void 0 : _52.to(channelId === null || channelId === void 0 ? void 0 : channelId.session_id).emit('chat_response', JSON.stringify(agentDetails));
                                        // this.publishMessageToWebChat(channelId?.domain_id, JSON.stringify({ session_id: channelId?.session_id, req_type: 'DISCONNECT' }));
                                    }
                                    else {
                                        this.redisadaptor.publishMessage(JSON.stringify({ session_id: channelId === null || channelId === void 0 ? void 0 : channelId.session_id, req_type: 'DISCONNECT' }));
                                    }
                                }
                            }
                        }
                        else {
                            const nextNodes = (_55 = (_54 = (_53 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _53 === void 0 ? void 0 : _53.children) === null || _54 === void 0 ? void 0 : _54.find((element) => (element === null || element === void 0 ? void 0 : element.type) === 'error')) !== null && _55 !== void 0 ? _55 : { childId: '' };
                            return this.handleChatFlowType(nextNodes === null || nextNodes === void 0 ? void 0 : nextNodes.childId, channelId, node, io);
                        }
                        // } else {
                        //   this.chatAdapter?.io?.to(channelId?.session_id).emit('chat_response', JSON.stringify({ session_id: channelId?.session_id, req_type: 'DISCONNECT', checkBusinessHours: false }));
                        // }
                    }
                    catch (error) {
                        console.log('error >>>>>>.', error);
                        const nextNodeData = (_58 = (_57 = (_56 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _56 === void 0 ? void 0 : _56.children) === null || _57 === void 0 ? void 0 : _57.find((element) => (element === null || element === void 0 ? void 0 : element.type) === 'error')) !== null && _58 !== void 0 ? _58 : { childId: '' };
                        return this.handleChatFlowType(nextNodeData === null || nextNodeData === void 0 ? void 0 : nextNodeData.childId, channelId, node, io);
                    }
                    break;
                }
                case 'branchHoursofOperation': {
                    try {
                        this.channelInformation = Object.assign(Object.assign({}, this.channelInformation), { response: 'pending' });
                        this.businessHoursTimeout = false;
                        const cid = (_61 = (_60 = (_59 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _59 === void 0 ? void 0 : _59.information) === null || _60 === void 0 ? void 0 : _60.outOfBusiness) === null || _61 === void 0 ? void 0 : _61.cid;
                        const businessHours = yield (0, callflow_dao_1.getHoursofOperationById)(cid);
                        const targetRestructured = Object.assign(Object.assign({}, currentNode), { session_id: channelId === null || channelId === void 0 ? void 0 : channelId.session_id, status: businessHours ? 1 : 0 });
                        // playSendprompt(channelId?.session_id)
                        (_63 = (_62 = this.chatAdapter) === null || _62 === void 0 ? void 0 : _62.io) === null || _63 === void 0 ? void 0 : _63.to(channelId === null || channelId === void 0 ? void 0 : channelId.session_id).emit('chat_response', JSON.stringify(targetRestructured));
                        yield this.waitForBusinessHoursResponse();
                        if (businessHours) {
                            const nextNode = (_66 = (_65 = (_64 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _64 === void 0 ? void 0 : _64.children) === null || _65 === void 0 ? void 0 : _65.find((element) => (element === null || element === void 0 ? void 0 : element.type) === 'success')) !== null && _66 !== void 0 ? _66 : { childId: '' };
                            return this.handleChatFlowType(nextNode === null || nextNode === void 0 ? void 0 : nextNode.childId, channelId, node, io);
                        }
                        const nextNodeData = (_69 = (_68 = (_67 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _67 === void 0 ? void 0 : _67.children) === null || _68 === void 0 ? void 0 : _68.find((element) => (element === null || element === void 0 ? void 0 : element.type) === 'fail')) !== null && _69 !== void 0 ? _69 : { childId: '' };
                        return this.handleChatFlowType(nextNodeData === null || nextNodeData === void 0 ? void 0 : nextNodeData.childId, channelId, node, io);
                    }
                    catch (error) {
                        const nextNodeData = (_72 = (_71 = (_70 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _70 === void 0 ? void 0 : _70.children) === null || _71 === void 0 ? void 0 : _71.find((element) => (element === null || element === void 0 ? void 0 : element.type) === 'error')) !== null && _72 !== void 0 ? _72 : { childId: '' };
                        return this.handleChatFlowType(nextNodeData === null || nextNodeData === void 0 ? void 0 : nextNodeData.childId, channelId, node, io);
                    }
                    break;
                }
                case 'makeApiCall': {
                    try {
                        const type = (_74 = (_73 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _73 === void 0 ? void 0 : _73.information) === null || _74 === void 0 ? void 0 : _74.getMethodApi;
                        const rawEndURL = (_76 = (_75 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _75 === void 0 ? void 0 : _75.information) === null || _76 === void 0 ? void 0 : _76.endPointUrl.lastIndexOf('$');
                        // console.log(rawEndURL, currentNode?.data?.information?.endPointUrl,"currentNodecurrentNode")
                        const endPointUrl = type === 'get' ? (_78 = (_77 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _77 === void 0 ? void 0 : _77.information) === null || _78 === void 0 ? void 0 : _78.endPointUrl.substring(0, rawEndURL) : (_80 = (_79 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _79 === void 0 ? void 0 : _79.information) === null || _80 === void 0 ? void 0 : _80.endPointUrl;
                        // const endPointUrl = currentNode?.data?.information?.endPointUrl;
                        const url = type === 'get' ? `${endPointUrl}${(_81 = this.inputParams[0]) === null || _81 === void 0 ? void 0 : _81.mobile_no}` : `${endPointUrl}`; // 'https://qaccaasapi.worktual.co.uk/core_service/v1/get_mobile_number_status/94988515546677'//;
                        // const url = `${endPointUrl}`;
                        console.log('url >>>>', url);
                        const method = (_84 = (_83 = (_82 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _82 === void 0 ? void 0 : _82.information) === null || _83 === void 0 ? void 0 : _83.getMethodApi) !== null && _84 !== void 0 ? _84 : 'get';
                        const selectedKeys = new Set((_87 = (_86 = (_85 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _85 === void 0 ? void 0 : _85.information) === null || _86 === void 0 ? void 0 : _86.selectedKeyFromResponse) !== null && _87 !== void 0 ? _87 : []);
                        const body = (_89 = (_88 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _88 === void 0 ? void 0 : _88.information) === null || _89 === void 0 ? void 0 : _89.apiBody;
                        const headers = (_91 = (_90 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _90 === void 0 ? void 0 : _90.information) === null || _91 === void 0 ? void 0 : _91.apiHeaders;
                        const apiResponse = yield (0, fetch_api_1.fetchApi)({ urlEndPoint: url, urlMethod: method, urlBody: body, urlHeaders: headers, inputParams: this.inputParams });
                        const targetRestructured = { session_id: channelId === null || channelId === void 0 ? void 0 : channelId.session_id };
                        const nameAPI = (_93 = (_92 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _92 === void 0 ? void 0 : _92.information) === null || _93 === void 0 ? void 0 : _93.nameAPI;
                        console.log('apiResponse >>>>>>', apiResponse);
                        if (apiResponse === null || apiResponse === void 0 ? void 0 : apiResponse.results) {
                            if ((apiResponse === null || apiResponse === void 0 ? void 0 : apiResponse.results.status) === 200 || (nameAPI === 'Sim order' && ((_95 = (_94 = apiResponse === null || apiResponse === void 0 ? void 0 : apiResponse.results) === null || _94 === void 0 ? void 0 : _94.data) === null || _95 === void 0 ? void 0 : _95.status_code) !== 500)) {
                                // if (apiResponse?.results?.data?.status_code !== 500) {
                                let finalResponse = yield (0, fetch_api_1.getResponseData)(apiResponse === null || apiResponse === void 0 ? void 0 : apiResponse.results);
                                const nodeNext = (_98 = (_97 = (_96 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _96 === void 0 ? void 0 : _96.children) === null || _97 === void 0 ? void 0 : _97.find((nodes) => (nodes === null || nodes === void 0 ? void 0 : nodes.type) === 'success')) !== null && _98 !== void 0 ? _98 : { childId: '' };
                                if (Array.isArray(finalResponse) && finalResponse.length > 0) {
                                    finalResponse = (_99 = finalResponse[0]) !== null && _99 !== void 0 ? _99 : { childId: '' };
                                }
                                const _formattedSelectedKeys = [];
                                for (const [key, value] of Object.entries(finalResponse)) {
                                    if (selectedKeys.has(key)) {
                                        /* tslint:disable */
                                        _formattedSelectedKeys.push({ tempkey: '${context.' + key + '}', originalkey: key, [key]: value });
                                        this.addContextVariable(key, value);
                                    }
                                }
                                const finalKeys = this.context;
                                this.context = {};
                                targetRestructured.selectedKeys = finalKeys;
                                console.log(nodeNext, "nodeNextnodeNextnodeNext");
                                if (nodeNext.childId.includes('customInput')) {
                                    const nextNodedetails = yield this.getNodeDataById(nodeNext.childId, node);
                                    const _cpynextNodedetails = Object.assign({}, nextNodedetails);
                                    const _childrens = ((_100 = _cpynextNodedetails === null || _cpynextNodedetails === void 0 ? void 0 : _cpynextNodedetails.data) === null || _100 === void 0 ? void 0 : _100.children) || {};
                                    for (let _index = 0; _index < _childrens.length; _index++) {
                                        const element = _childrens[_index];
                                        for (let _tempi = 0; _tempi < _formattedSelectedKeys.length; _tempi++) {
                                            const tempelement = _formattedSelectedKeys[_tempi];
                                            if (element.type === tempelement.tempkey) {
                                                // console.log(_cpynextNodedetails?.data?.children[_index]);
                                                _childrens[_index].type = tempelement.value;
                                            }
                                        }
                                    }
                                    // console.log("_childrens >", _childrens);
                                    _cpynextNodedetails.data.children = _childrens;
                                    targetRestructured.currentNode = _cpynextNodedetails;
                                    targetRestructured.result = finalResponse;
                                    (_102 = (_101 = this.chatAdapter) === null || _101 === void 0 ? void 0 : _101.io) === null || _102 === void 0 ? void 0 : _102.to(channelId === null || channelId === void 0 ? void 0 : channelId.session_id).emit('chat_response', JSON.stringify(targetRestructured));
                                }
                                else if (nodeNext.childId.includes('messageFlow')) {
                                    const nextNodedetails = yield this.getNodeDataById(nodeNext.childId, node);
                                    console.log('nextNodedetails >>>>>>>', nextNodedetails);
                                    const text = (_104 = (_103 = nextNodedetails === null || nextNodedetails === void 0 ? void 0 : nextNodedetails.data) === null || _103 === void 0 ? void 0 : _103.information) === null || _104 === void 0 ? void 0 : _104.message;
                                    const availableContexts = text === null || text === void 0 ? void 0 : text.match(chatflow_helpers_1.contextPattern);
                                    const updatedMessage = (availableContexts === null || availableContexts === void 0 ? void 0 : availableContexts.length) > 0 ? (0, chatflow_helpers_1.replacechatPatternWithVariables)(availableContexts, text, _formattedSelectedKeys) : text;
                                    const data = Object.assign({}, (_105 = nextNodedetails === null || nextNodedetails === void 0 ? void 0 : nextNodedetails.data) === null || _105 === void 0 ? void 0 : _105.information);
                                    data.message = `${updatedMessage}`;
                                    const targetRestructured = { nextNodedetails, data, session_id: channelId === null || channelId === void 0 ? void 0 : channelId.session_id };
                                    targetRestructured.msg = updatedMessage;
                                    const messagenextNode = (_108 = (_107 = (_106 = nextNodedetails === null || nextNodedetails === void 0 ? void 0 : nextNodedetails.data) === null || _106 === void 0 ? void 0 : _106.children) === null || _107 === void 0 ? void 0 : _107.find((nodes) => (nodes === null || nodes === void 0 ? void 0 : nodes.type) === 'success')) !== null && _108 !== void 0 ? _108 : { childId: '' };
                                    let messagenextNodedetails = {};
                                    if (messagenextNode.childId) {
                                        messagenextNodedetails = yield this.getNodeDataById(messagenextNode === null || messagenextNode === void 0 ? void 0 : messagenextNode.childId, node);
                                    }
                                    targetRestructured.currentNode = nextNodedetails;
                                    const message = {
                                        messageID: (0, uuid_1.v4)(),
                                        sessionId: channelId === null || channelId === void 0 ? void 0 : channelId.session_id,
                                        message: `${targetRestructured.msg}`,
                                        sendAt: Date.now(),
                                        messageType: 'text',
                                        pdfurl: '',
                                        isBot: true,
                                        isCustomer: false,
                                        fileType: '',
                                        direction: 'OUT',
                                        isLike: 0
                                    };
                                    const messageList = { message: message };
                                    const messageType = { agentText: messageList };
                                    const stringifiedMessage = { message: messageType };
                                    stringifiedMessage.ChatHistoryId = channelId === null || channelId === void 0 ? void 0 : channelId.ChatHistoryId;
                                    stringifiedMessage.ChatDurationHistoryId = channelId === null || channelId === void 0 ? void 0 : channelId.ChatDurationHistoryId;
                                    try {
                                        console.log("stringifiedMessage=-=-=-1", stringifiedMessage);
                                        yield (0, webChat_dao_1.updateChatMessage)(stringifiedMessage);
                                    }
                                    catch (err) {
                                        console.log(err);
                                    }
                                    //createSession(stringifiedMessage, channelId?.session_id);
                                    (_110 = (_109 = this.chatAdapter) === null || _109 === void 0 ? void 0 : _109.io) === null || _110 === void 0 ? void 0 : _110.to(channelId === null || channelId === void 0 ? void 0 : channelId.session_id).emit('OneToOneChatReceive', message);
                                    const _cpynextNodedetails = Object.assign({}, messagenextNodedetails);
                                    targetRestructured.currentNode = _cpynextNodedetails;
                                    (_112 = (_111 = this.chatAdapter) === null || _111 === void 0 ? void 0 : _111.io) === null || _112 === void 0 ? void 0 : _112.to(channelId === null || channelId === void 0 ? void 0 : channelId.session_id).emit('chat_response', JSON.stringify(targetRestructured));
                                }
                                else if (nodeNext.childId.includes('makeApiCall')) {
                                    return this.handleChatFlowType(nodeNext === null || nodeNext === void 0 ? void 0 : nodeNext.childId, channelId, node, io);
                                }
                            }
                            else {
                                const nodeNext = (_115 = (_114 = (_113 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _113 === void 0 ? void 0 : _113.children) === null || _114 === void 0 ? void 0 : _114.find((nodes) => (nodes === null || nodes === void 0 ? void 0 : nodes.type) === 'error')) !== null && _115 !== void 0 ? _115 : { childId: '' };
                                // console.log(currentNode,"nodeNextnodeNextnodeNextnodeNext999999")
                                if (nodeNext.childId.includes('messageFlow')) {
                                    const nextNodedetails = yield this.getNodeDataById(nodeNext.childId, node);
                                    const text = (_117 = (_116 = nextNodedetails === null || nextNodedetails === void 0 ? void 0 : nextNodedetails.data) === null || _116 === void 0 ? void 0 : _116.information) === null || _117 === void 0 ? void 0 : _117.message;
                                    const availableContexts = text === null || text === void 0 ? void 0 : text.match(chatflow_helpers_1.contextPattern);
                                    const updatedMessage = (availableContexts === null || availableContexts === void 0 ? void 0 : availableContexts.length) > 0 ? (0, chatflow_helpers_1.replacePatternWithVariables)(availableContexts, text, this.context) : text;
                                    const data = Object.assign({}, (_118 = nextNodedetails === null || nextNodedetails === void 0 ? void 0 : nextNodedetails.data) === null || _118 === void 0 ? void 0 : _118.information);
                                    data.message = `${updatedMessage}`;
                                    const targetRestructured = { nextNodedetails, data, session_id: channelId === null || channelId === void 0 ? void 0 : channelId.session_id };
                                    targetRestructured.msg = (_119 = nextNodedetails === null || nextNodedetails === void 0 ? void 0 : nextNodedetails.data) === null || _119 === void 0 ? void 0 : _119.information.message;
                                    const messagenextNode = (_122 = (_121 = (_120 = nextNodedetails === null || nextNodedetails === void 0 ? void 0 : nextNodedetails.data) === null || _120 === void 0 ? void 0 : _120.children) === null || _121 === void 0 ? void 0 : _121.find((nodes) => (nodes === null || nodes === void 0 ? void 0 : nodes.type) === 'success')) !== null && _122 !== void 0 ? _122 : { childId: '' };
                                    let messagenextNodedetails = {};
                                    if (messagenextNode.childId) {
                                        messagenextNodedetails = yield this.getNodeDataById(messagenextNode === null || messagenextNode === void 0 ? void 0 : messagenextNode.childId, node);
                                    }
                                    const message = {
                                        messageID: (0, uuid_1.v4)(),
                                        sessionId: channelId === null || channelId === void 0 ? void 0 : channelId.session_id,
                                        message: `${targetRestructured.msg}`,
                                        sendAt: Date.now(),
                                        messageType: 'text',
                                        pdfurl: '',
                                        isBot: true,
                                        isCustomer: false,
                                        fileType: '',
                                        direction: 'OUT',
                                        isLike: 0
                                    };
                                    const messageList = { message: message };
                                    const messageType = { agentText: messageList };
                                    const stringifiedMessage = { message: messageType };
                                    stringifiedMessage.ChatHistoryId = channelId === null || channelId === void 0 ? void 0 : channelId.ChatHistoryId;
                                    stringifiedMessage.ChatDurationHistoryId = channelId === null || channelId === void 0 ? void 0 : channelId.ChatDurationHistoryId;
                                    try {
                                        console.log('stringifiedMessage-=-=-=78567', stringifiedMessage);
                                        yield (0, webChat_dao_1.updateChatMessage)(stringifiedMessage);
                                    }
                                    catch (err) {
                                        console.log(err);
                                    }
                                    //createSession(stringifiedMessage, channelId?.session_id);
                                    (_124 = (_123 = this.chatAdapter) === null || _123 === void 0 ? void 0 : _123.io) === null || _124 === void 0 ? void 0 : _124.to(channelId === null || channelId === void 0 ? void 0 : channelId.session_id).emit('OneToOneChatReceive', message);
                                    const _cpynextNodedetails = Object.assign({}, messagenextNodedetails);
                                    targetRestructured.currentNode = _cpynextNodedetails;
                                    console.log('_cpynextNodedetails >>>>>>', _cpynextNodedetails);
                                    (_126 = (_125 = this.chatAdapter) === null || _125 === void 0 ? void 0 : _125.io) === null || _126 === void 0 ? void 0 : _126.to(channelId === null || channelId === void 0 ? void 0 : channelId.session_id).emit('chat_response', JSON.stringify(targetRestructured));
                                }
                            }
                            // const nodeNext = currentNode?.data?.children?.find((nodes: any) => nodes?.type === 'success') ?? { childId: '' };
                            // return this.handleChatFlowType(nodeNext?.childId, channelId, node, io);
                        }
                        // const nextNode = currentNode?.data?.children?.find((nodes: any) => nodes?.type === 'error') ?? { childId: '' };
                        // return this.handleChatFlowType(nextNode?.childId, channelId, node, io);
                    }
                    catch (error) {
                        const nextNode = (_129 = (_128 = (_127 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _127 === void 0 ? void 0 : _127.children) === null || _128 === void 0 ? void 0 : _128.find((nodes) => (nodes === null || nodes === void 0 ? void 0 : nodes.type) === 'error')) !== null && _129 !== void 0 ? _129 : { childId: '' };
                        return this.handleChatFlowType(nextNode === null || nextNode === void 0 ? void 0 : nextNode.childId, channelId, node, io);
                    }
                    break;
                }
                case 'checkCondition': {
                    try {
                        const data = (_130 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _130 === void 0 ? void 0 : _130.information;
                        const leftOperand = (0, chatflow_helpers_1.replaceContextWithVariables)(data === null || data === void 0 ? void 0 : data.leftOperand, this.context);
                        const rightOperands = (_131 = data === null || data === void 0 ? void 0 : data.conditions) !== null && _131 !== void 0 ? _131 : [];
                        let resultIndex = -1;
                        for (const stock of rightOperands) {
                            const result = yield (0, conditions_mapper_1.conditionsMapper)(leftOperand, stock === null || stock === void 0 ? void 0 : stock.operand, (0, chatflow_helpers_1.replaceContextWithVariables)(stock === null || stock === void 0 ? void 0 : stock.rightOperand, this.context));
                            if (result) {
                                resultIndex = stock === null || stock === void 0 ? void 0 : stock.index;
                                break;
                            }
                        }
                        if (resultIndex !== -1) {
                            const nodeNext = (_134 = (_133 = (_132 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _132 === void 0 ? void 0 : _132.children) === null || _133 === void 0 ? void 0 : _133.find((nodes) => (nodes === null || nodes === void 0 ? void 0 : nodes.type) === `condition${resultIndex + 1}`)) !== null && _134 !== void 0 ? _134 : { childId: '' };
                            return this.handleChatFlowType(nodeNext === null || nodeNext === void 0 ? void 0 : nodeNext.childId, channelId, node, io);
                        }
                        const nextNode = (_137 = (_136 = (_135 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _135 === void 0 ? void 0 : _135.children) === null || _136 === void 0 ? void 0 : _136.find((nodes) => (nodes === null || nodes === void 0 ? void 0 : nodes.type) === 'error')) !== null && _137 !== void 0 ? _137 : { childId: '' };
                        return this.handleChatFlowType(nextNode === null || nextNode === void 0 ? void 0 : nextNode.childId, channelId, node, io);
                    }
                    catch (error) {
                        const nextNode = (_140 = (_139 = (_138 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _138 === void 0 ? void 0 : _138.children) === null || _139 === void 0 ? void 0 : _139.find((nodes) => (nodes === null || nodes === void 0 ? void 0 : nodes.type) === 'error')) !== null && _140 !== void 0 ? _140 : { childId: '' };
                        return this.handleChatFlowType(nextNode === null || nextNode === void 0 ? void 0 : nextNode.childId, channelId, node, io);
                    }
                    break;
                }
                case 'integration': {
                    try {
                        (_142 = (_141 = this.chatAdapter) === null || _141 === void 0 ? void 0 : _141.io) === null || _142 === void 0 ? void 0 : _142.to(channelId === null || channelId === void 0 ? void 0 : channelId.session_id).emit('chat_response', JSON.stringify(currentNode));
                    }
                    catch (error) {
                        const nextNode = (_145 = (_144 = (_143 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _143 === void 0 ? void 0 : _143.children) === null || _144 === void 0 ? void 0 : _144.find((nodes) => (nodes === null || nodes === void 0 ? void 0 : nodes.type) === 'error')) !== null && _145 !== void 0 ? _145 : { childId: '' };
                        return this.handleChatFlowType(nextNode === null || nextNode === void 0 ? void 0 : nextNode.childId, channelId, node, io);
                    }
                    break;
                }
                case 'loop': {
                    try {
                        console.log('this.sessionLoops[channelId?.session_id] >>>>>>', this.sessionLoops[channelId === null || channelId === void 0 ? void 0 : channelId.session_id]);
                        if (!this.sessionLoops[channelId === null || channelId === void 0 ? void 0 : channelId.session_id] && this.sessionLoops[channelId === null || channelId === void 0 ? void 0 : channelId.session_id] !== 0) {
                            this.sessionLoops[channelId === null || channelId === void 0 ? void 0 : channelId.session_id] = (_148 = (_147 = (_146 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _146 === void 0 ? void 0 : _146.information) === null || _147 === void 0 ? void 0 : _147.loopValue) !== null && _148 !== void 0 ? _148 : 0;
                        }
                        const loopTo = (_151 = (_150 = (_149 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _149 === void 0 ? void 0 : _149.children) === null || _150 === void 0 ? void 0 : _150.find((nodee) => (nodee === null || nodee === void 0 ? void 0 : nodee.type) === 'loop')) !== null && _151 !== void 0 ? _151 : { childId: '' };
                        if (loopTo && this.sessionLoops[channelId === null || channelId === void 0 ? void 0 : channelId.session_id] > 0) {
                            console.log('Loop inner>>>>>>', loopTo);
                            this.sessionLoops[channelId === null || channelId === void 0 ? void 0 : channelId.session_id] -= 1;
                            const nextNodedetails = yield this.getNodeDataById(loopTo.childId, node);
                            if (loopTo.childId.includes('makeApiCall')) {
                                return this.handleChatFlowType(loopTo === null || loopTo === void 0 ? void 0 : loopTo.childId, channelId, node, io);
                            }
                            {
                                const targetRestructured = { nextNodedetails, session_id: channelId === null || channelId === void 0 ? void 0 : channelId.session_id };
                                console.log('nextNodedetails >>>>>>>', nextNodedetails);
                                targetRestructured.currentNode = nextNodedetails;
                                // return this.handleChatFlowType(loopTo?.childId, channelId, node, io);
                                (_153 = (_152 = this.chatAdapter) === null || _152 === void 0 ? void 0 : _152.io) === null || _153 === void 0 ? void 0 : _153.to(channelId === null || channelId === void 0 ? void 0 : channelId.session_id).emit('chat_response', JSON.stringify(targetRestructured));
                            }
                        }
                        else {
                            const nextNode = (_156 = (_155 = (_154 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _154 === void 0 ? void 0 : _154.children) === null || _155 === void 0 ? void 0 : _155.find((nodes) => (nodes === null || nodes === void 0 ? void 0 : nodes.type) === 'complete')) !== null && _156 !== void 0 ? _156 : { childId: '' };
                            const nextNodedetails = yield this.getNodeDataById(nextNode.childId, node);
                            console.log('Loop Complete>>>>>>', nextNode.childId);
                            return this.handleChatFlowType(nextNode === null || nextNode === void 0 ? void 0 : nextNode.childId, channelId, node, io);
                            // const targetRestructured: any = { nextNodedetails, session_id: channelId?.session_id };
                            // targetRestructured.currentNode = nextNodedetails
                            // this.chatAdapter?.io?.to(channelId?.session_id).emit('chat_response', JSON.stringify(targetRestructured));
                        }
                    }
                    catch (error) {
                        const nextNode = (_159 = (_158 = (_157 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _157 === void 0 ? void 0 : _157.children) === null || _158 === void 0 ? void 0 : _158.find((nodee) => (nodee === null || nodee === void 0 ? void 0 : nodee.type) === 'error')) !== null && _159 !== void 0 ? _159 : { childId: '' };
                        return this.handleChatFlowType(nextNode === null || nextNode === void 0 ? void 0 : nextNode.childId, channelId, node, io);
                    }
                    break;
                }
                case 'disconnect': {
                    console.log('FOUND DISCONNECTED NODE. GOING TO DISCONNECT');
                    yield new Promise(resolve => setTimeout(resolve, 500));
                    const getDomainResult = yield (0, callflow_dao_1.getDomainStatus)(channelId === null || channelId === void 0 ? void 0 : channelId.domain_id);
                    (_161 = (_160 = this.chatAdapter) === null || _160 === void 0 ? void 0 : _160.io) === null || _161 === void 0 ? void 0 : _161.to(channelId === null || channelId === void 0 ? void 0 : channelId.session_id).emit('chat_response', JSON.stringify({ session_id: channelId === null || channelId === void 0 ? void 0 : channelId.session_id, req_type: 'DISCONNECT', domainStatus: getDomainResult }));
                    return;
                }
                default: {
                    console.log('Default calling');
                    // if (channelId?.disconnectedBy) {
                    this.blpopTimeout = true;
                    const getDomainResult = yield (0, callflow_dao_1.getDomainStatus)(channelId === null || channelId === void 0 ? void 0 : channelId.domain_id);
                    (_163 = (_162 = this.chatAdapter) === null || _162 === void 0 ? void 0 : _162.io) === null || _163 === void 0 ? void 0 : _163.to(channelId === null || channelId === void 0 ? void 0 : channelId.session_id).emit('chat_response', JSON.stringify({ session_id: channelId === null || channelId === void 0 ? void 0 : channelId.session_id, req_type: 'DISCONNECT' }));
                    return;
                    // }
                    return;
                }
            }
        });
    }
    waitForPlayPromptSuccessResponse() {
        return __awaiter(this, void 0, void 0, function* () {
            return new Promise((resolve, reject) => {
                let timeout = 30;
                const inteval = setInterval(() => {
                    var _a, _b;
                    if (timeout === 0 || this.playpromptSuccessTimeout) {
                        console.log('Clear Intervel callling >>>>>', (_a = this.channelInformation) === null || _a === void 0 ? void 0 : _a.response);
                        clearInterval(inteval);
                        reject(false);
                    }
                    if (((_b = this.channelInformation) === null || _b === void 0 ? void 0 : _b.response) === 'success') {
                        console.log('Succs Clear Intervel callling >>>>>');
                        this.channelInformation = Object.assign(Object.assign({}, this.channelInformation), { response: 'pending' });
                        clearInterval(inteval);
                        resolve(true);
                    }
                    timeout -= 1;
                }, 1000);
            });
        });
    }
    waitForSuccessResponse() {
        return __awaiter(this, void 0, void 0, function* () {
            try {
                return new Promise((resolve, reject) => {
                    try {
                        let timeout = 30;
                        const inteval = setInterval(() => {
                            var _a;
                            if (timeout === 0 || this.successTimeout) {
                                clearInterval(inteval);
                                reject(false);
                            }
                            if (((_a = this.channelInformation) === null || _a === void 0 ? void 0 : _a.response) === 'success') {
                                this.channelInformation = Object.assign(Object.assign({}, this.channelInformation), { response: 'pending' });
                                clearInterval(inteval);
                                resolve(true);
                            }
                            timeout -= 1;
                        }, 1000);
                    }
                    catch (error) {
                        console.log('Fi>>>>>', error);
                        // clearInterval(inteval);
                        reject(false);
                    }
                });
            }
            catch (error) {
                console.log('error', error);
            }
        });
    }
    waitForAgentResponse(channelId) {
        return __awaiter(this, void 0, void 0, function* () {
            try {
                let timeout = 60;
                const inteval = setInterval(() => {
                    var _a;
                    if (timeout === 0 || this.successTimeout) {
                        clearInterval(inteval);
                        this.blpopTimeout = true;
                        // this.chatAdapter?.io?.to(channelId?.session_id).emit("chat_response", JSON.stringify({ session_id: channelId?.session_id, req_type: "DISCONNECT" }));
                    }
                    if (((_a = this.channelInformation) === null || _a === void 0 ? void 0 : _a.response) === 'success') {
                        clearInterval(inteval);
                        this.channelInformation = Object.assign(Object.assign({}, this.channelInformation), { response: 'pending' });
                    }
                    timeout -= 1;
                }, 1000);
            }
            catch (error) {
                console.log('error', error);
            }
        });
    }
    waitForLiveAgentResponse(channelId) {
        return __awaiter(this, void 0, void 0, function* () {
            try {
                let timeout = 60;
                const inteval = setInterval(() => {
                    if (timeout === 0 || this.successTimeout) {
                        clearInterval(inteval);
                        this.blpopTimeout = true;
                    }
                    timeout -= 1;
                }, 1000);
            }
            catch (error) {
                console.log('error', error);
            }
        });
    }
    waitForDTMFSuccessResponse(isVariable) {
        return __awaiter(this, void 0, void 0, function* () {
            return new Promise((resolve, _) => {
                let timeout = 180;
                const inteval = setInterval(() => {
                    var _a, _b, _c;
                    if (timeout === 0 || this.dtmfSuccessTimeout) {
                        clearInterval(inteval);
                        resolve('timeout');
                    }
                    if (((_a = this.channelInformation) === null || _a === void 0 ? void 0 : _a.response) === 'success') {
                        this.channelInformation = Object.assign(Object.assign({}, this.channelInformation), { response: 'pending' });
                        clearInterval(inteval);
                        isVariable ? resolve((_b = this.channelInformation) === null || _b === void 0 ? void 0 : _b.variable) : resolve((_c = this.channelInformation) === null || _c === void 0 ? void 0 : _c.value);
                    }
                    timeout -= 1;
                }, 1000);
            });
        });
    }
    waitForBusinessHoursResponse() {
        return __awaiter(this, void 0, void 0, function* () {
            return new Promise((resolve, reject) => {
                let timeout = 30;
                const inteval = setInterval(() => {
                    var _a;
                    if (timeout === 0 || this.businessHoursTimeout) {
                        clearInterval(inteval);
                        reject(false);
                    }
                    if (((_a = this.channelInformation) === null || _a === void 0 ? void 0 : _a.response) === 'success') {
                        this.channelInformation = Object.assign(Object.assign({}, this.channelInformation), { response: 'pending' });
                        clearInterval(inteval);
                        resolve(true);
                    }
                    timeout -= 1;
                }, 1000);
            });
        });
    }
    waitForCallStatusResponse() {
        return __awaiter(this, void 0, void 0, function* () {
            const status = new Set(['dialTimeout', 'cancelled']);
            return new Promise((resolve, _) => {
                let timeout = 180;
                const inteval = setInterval(() => {
                    var _a, _b, _c, _d, _e, _f, _g;
                    if (timeout === 0 || this.responseTimeout) {
                        resolve(null);
                        clearInterval(inteval);
                    }
                    else {
                        if (((_a = this.channelInformation) === null || _a === void 0 ? void 0 : _a.status) && status.has((_b = this.channelInformation) === null || _b === void 0 ? void 0 : _b.status)) {
                            this.channelInformation = Object.assign(Object.assign({}, this.channelInformation), { response: 'pending' });
                            console.log(`${(_d = (_c = this.channelInformation) === null || _c === void 0 ? void 0 : _c.status) === null || _d === void 0 ? void 0 : _d.toUpperCase()}, LOOKING FOR ANOTHER USER`);
                            clearInterval(inteval);
                            resolve((_e = this.channelInformation) === null || _e === void 0 ? void 0 : _e.status);
                        }
                        else if (((_f = this.channelInformation) === null || _f === void 0 ? void 0 : _f.status) && !status.has((_g = this.channelInformation) === null || _g === void 0 ? void 0 : _g.status)) {
                            resolve(null);
                            clearInterval(timeout);
                        }
                    }
                    timeout -= 1;
                }, 1000);
            });
        });
    }
    waitForSecondCallStatusResponse() {
        return __awaiter(this, void 0, void 0, function* () {
            const status = new Set(['dialTimeout', 'cancelled']);
            return new Promise((resolve, _) => {
                let timeout = 180;
                const inteval = setInterval(() => {
                    var _a, _b, _c, _d, _e;
                    if (timeout === 0 || this.secondResponseTimeout) {
                        resolve(null);
                        clearInterval(inteval);
                    }
                    else {
                        if (((_a = this.channelInformation) === null || _a === void 0 ? void 0 : _a.status) && status.has((_b = this.channelInformation) === null || _b === void 0 ? void 0 : _b.status)) {
                            this.channelInformation = Object.assign(Object.assign({}, this.channelInformation), { response: 'pending' });
                            clearInterval(inteval);
                            resolve((_c = this.channelInformation) === null || _c === void 0 ? void 0 : _c.status);
                        }
                        else if (((_d = this.channelInformation) === null || _d === void 0 ? void 0 : _d.status) !== 'pending' && !status.has((_e = this.channelInformation) === null || _e === void 0 ? void 0 : _e.status)) {
                            resolve(null);
                            clearInterval(timeout);
                        }
                    }
                    timeout -= 1;
                }, 1000);
            });
        });
    }
    publishMessageToWebChat(domainId, key) {
        var _a;
        console.log('CHAT: ', `${domainId}_${process.env.CHATRESPONSECHANNEL}`, JSON.parse(key));
        (_a = this.chatAdapter.redisadaptor) === null || _a === void 0 ? void 0 : _a.dub.rpush(`${domainId}_${process.env.CHATRESPONSECHANNEL}`, key);
    }
    BLPOP(domainId, list, routingProfiles, sessionId, userId) {
        return __awaiter(this, void 0, void 0, function* () {
            // console.log('090990vxsvxjhbsvjhxbjshbx', ChatFlowAdapters.queueDetails?.at(0)?.sessionId)
            // console.log('sessionId', sessionId)
            // if (ChatFlowAdapters.queueDetails?.at(0)?.sessionId === sessionId) {
            // console.log("SEARCHING FOR AVAILABLE AGENT ...", this.chatAdapter.redisadaptor.dub);
            yield db_1.otherRedisClient.blpop(`${domainId}_${process.env.AGENTTRACKERCHANNEL}`, 1).then((data) => __awaiter(this, void 0, void 0, function* () {
                var _a;
                // console.log("this.blpopTimeout >>>>>>>", this.blpopTimeout, data)
                if (this.blpopTimeout) {
                    console.log('CANCELLING SEARCHING FOR AGENT BECAUSE OF DISCONNECTED CALL ...');
                    list.push(null);
                    return list;
                }
                if (data !== null && (data === null || data === void 0 ? void 0 : data.length) > 1) {
                    const parsedData = JSON.parse(data === null || data === void 0 ? void 0 : data.at(1));
                    if (((parsedData === null || parsedData === void 0 ? void 0 : parsedData.statusName) === 'Ready' || (parsedData === null || parsedData === void 0 ? void 0 : parsedData.statusName) === 'Transfer Only') && ((routingProfiles === null || routingProfiles === void 0 ? void 0 : routingProfiles.includes(parsedData === null || parsedData === void 0 ? void 0 : parsedData.routingProfileId)) || userId === (parsedData === null || parsedData === void 0 ? void 0 : parsedData.UserID))) {
                        console.log('AGENT FOUND AS: ', data);
                        list.push((_a = [parsedData]) !== null && _a !== void 0 ? _a : null);
                        return list;
                    }
                }
                return yield this.BLPOP(domainId, list, routingProfiles, sessionId, userId);
            }));
        });
    }
    LIVEBLPOP(domainId, list) {
        return __awaiter(this, void 0, void 0, function* () {
            yield db_1.otherRedisClient.blpop(`${domainId}_${process.env.AGENTTRACKERCHANNEL}`, 1).then((data) => __awaiter(this, void 0, void 0, function* () {
                var _a;
                if (this.blpopTimeout) {
                    console.log('CANCELLING SEARCHING FOR AGENT BECAUSE OF DISCONNECTED CALL ...');
                    list.push(null);
                    return list;
                }
                if (data !== null && (data === null || data === void 0 ? void 0 : data.length) > 1) {
                    const parsedData = JSON.parse(data === null || data === void 0 ? void 0 : data.at(1));
                    if (((parsedData === null || parsedData === void 0 ? void 0 : parsedData.statusName) === 'Ready' || (parsedData === null || parsedData === void 0 ? void 0 : parsedData.statusName) === 'Transfer Only')) {
                        console.log('AGENT FOUND AS: ', data);
                        list.push((_a = [parsedData]) !== null && _a !== void 0 ? _a : null);
                        return list;
                    }
                }
                return yield this.LIVEBLPOP(domainId, list);
            }));
        });
    }
    translateItem(list, channelId) {
        return __awaiter(this, void 0, void 0, function* () {
            var sourceLang = 'en';
            var targetLang = channelId === null || channelId === void 0 ? void 0 : channelId.targetLang;
            var url = "https://translate.googleapis.com/translate_a/single?client=gtx&sl=" +
                sourceLang + "&tl=" + targetLang + "&dt=t&q=" + encodeURI(list);
            return new Promise((resolve, reject) => {
                (0, callflow_dao_1.getJSON)(url, function (err, data) {
                    if (err) {
                        reject(err);
                    }
                    else {
                        resolve(data[0][0][0]);
                    }
                });
            });
        });
    }
    getAvaiableAgentChatListBtn(channel, qid) {
        return __awaiter(this, void 0, void 0, function* () {
            let agentDetails = [];
            return new Promise((resolve) => {
                db_1.otherRedisClient.hgetall(`${channel === null || channel === void 0 ? void 0 : channel.domain_id}_presence`).then((data) => __awaiter(this, void 0, void 0, function* () {
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
                        console.log('sortedAgents', sortedAgents);
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
                                            // else {
                                            //   chatLimitAchievedAgents.push(tempAgent);
                                            //   agentsToBeAssigned = agentsToBeAssigned.filter((val: any) => val.ext !== tempAgent.ext);
                                            //   assignFunction();
                                            // }
                                        });
                                    }
                                    else if (chatLimitData === null || chatLimitData === void 0 ? void 0 : chatLimitData.isRoutingProfile) {
                                        const routings = {};
                                        RoutingProfileUser.map((item) => {
                                            routings[item.RoutingProfileId] = item.concurrentChat;
                                        });
                                        console.log('routings', routings);
                                        console.log('tempAgent', tempAgent);
                                        for (let [key, value] of Object.entries(routings)) {
                                            if (tempAgent.routingProfileId == key) {
                                                console.log('first if');
                                                if (tempAgent.currentChatCount == value) {
                                                    console.log('second if');
                                                    chatLimitAchievedAgents.push(tempAgent);
                                                    agentsToBeAssigned = agentsToBeAssigned.filter((val) => val.ext !== tempAgent.ext);
                                                    if (chatLimitAchievedAgents.length !== sortedAgents.length) {
                                                        assignFunction();
                                                    }
                                                }
                                                else {
                                                    console.log('else');
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
                                    console.log('assignFunction tempAgent', tempAgent);
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
                                console.log('tempAgent', tempAgent);
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
    }
    getAvailableAgentChatList(channel) {
        return __awaiter(this, void 0, void 0, function* () {
            let agentDetails = [];
            return new Promise((resolve) => {
                db_1.otherRedisClient.hgetall(`${channel === null || channel === void 0 ? void 0 : channel.domain_id}_presence`).then((data) => __awaiter(this, void 0, void 0, function* () {
                    var _a;
                    const result = Object.values(data).map((value) => JSON.parse(value));
                    // const result: any = await getLiveAgentsList(data, channel);
                    // console.log("\ngetAvailableAgentChatList result:", result);
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
                        console.log('sortedAgents', sortedAgents);
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
                                            // else {
                                            //   chatLimitAchievedAgents.push(tempAgent);
                                            //   agentsToBeAssigned = agentsToBeAssigned.filter((val: any) => val.ext !== tempAgent.ext);
                                            //   assignFunction();
                                            // }
                                        });
                                    }
                                    else if (chatLimitData === null || chatLimitData === void 0 ? void 0 : chatLimitData.isRoutingProfile) {
                                        const routings = {};
                                        RoutingProfileUser.map((item) => {
                                            routings[item.RoutingProfileId] = item.concurrentChat;
                                        });
                                        console.log('routings', routings);
                                        console.log('tempAgent', tempAgent);
                                        for (let [key, value] of Object.entries(routings)) {
                                            if (tempAgent.routingProfileId == key) {
                                                console.log('first if');
                                                if (tempAgent.currentChatCount == value) {
                                                    console.log('second if');
                                                    chatLimitAchievedAgents.push(tempAgent);
                                                    agentsToBeAssigned = agentsToBeAssigned.filter((val) => val.ext !== tempAgent.ext);
                                                    if (chatLimitAchievedAgents.length !== sortedAgents.length) {
                                                        assignFunction();
                                                    }
                                                }
                                                else {
                                                    console.log('else');
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
                                    console.log('assignFunction tempAgent', tempAgent);
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
                                console.log('tempAgent', tempAgent);
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
    }
}
exports.ChatFlowAdapters = ChatFlowAdapters;
//# sourceMappingURL=chatflow.js.map