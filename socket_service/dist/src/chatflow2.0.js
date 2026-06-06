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
const uuid_1 = require("uuid");
const chatflow_helpers_1 = require("./chatflow_helpers");
const conditions_mapper_1 = require("./conditions_mapper");
const callflow_dao_1 = require("./dao/callflow.dao");
const chat_dao_1 = require("./dao/chat.dao");
const webChat_dao_1 = require("./dao/webChat.dao");
const fetch_api_1 = require("./fetch_api");
const transferToQueue_1 = require("./helperFunctions/transferToQueue");
const db_1 = require("./plugins/db");
const log_1 = require("./plugins/log");
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
        var _a, _b, _c, _d, _e, _f, _g, _h, _j, _k, _l, _m, _o, _p;
        return __awaiter(this, void 0, void 0, function* () {
            if (channel && JSON.stringify(this.channelInformation) !== JSON.stringify(channel) && !((_a = JSON.parse(channel !== null && channel !== void 0 ? channel : 'null')) === null || _a === void 0 ? void 0 : _a.queueUpdate)) {
                this.channelInformation = (_b = JSON.parse(channel !== null && channel !== void 0 ? channel : 'null')) !== null && _b !== void 0 ? _b : '';
            }
            // console.log('ChatFlowHandler >>>>>>', this.channelInformation);
            // console.log('this.callflowId >>>>>>', this.callflowId);
            const io = db_1.ioredisChat;
            const channelId = JSON.parse(channel !== null && channel !== void 0 ? channel : 'null');
            if (!this.callflowId) {
                this.callflowId = channelId === null || channelId === void 0 ? void 0 : channelId.callFlowId;
            }
            if (this.callflowId || channelId.liveTransfer === 1) {
                if (this.nodes && ((_c = this.nodes) === null || _c === void 0 ? void 0 : _c.length) === 0 && this.callflowId) {
                    const nodesString = yield (0, callflow_dao_1.getnodesdata)(this.callflowId);
                    this.nodes = JSON.parse(nodesString);
                }
                if ((channelId === null || channelId === void 0 ? void 0 : channelId.channelType) === 'Chat' && (channelId === null || channelId === void 0 ? void 0 : channelId.call_state) === 'init' && channelId.liveTransfer !== 1) {
                    const startingNode = yield this.getStartNode(this.nodes);
                    const currentNodeId = ((_e = (_d = startingNode === null || startingNode === void 0 ? void 0 : startingNode.data) === null || _d === void 0 ? void 0 : _d.children) === null || _e === void 0 ? void 0 : _e.length) > 0 ? (_g = (_f = startingNode === null || startingNode === void 0 ? void 0 : startingNode.data) === null || _f === void 0 ? void 0 : _f.children[0]) === null || _g === void 0 ? void 0 : _g.childId : '';
                    if (currentNodeId && (currentNodeId === null || currentNodeId === void 0 ? void 0 : currentNodeId.includes('carousel'))) {
                        channelId.call_type = 'carousel';
                        const entryCarousal = [];
                        (_j = (_h = startingNode === null || startingNode === void 0 ? void 0 : startingNode.data) === null || _h === void 0 ? void 0 : _h.children) === null || _j === void 0 ? void 0 : _j.map((list) => {
                            var _a;
                            if ((_a = list === null || list === void 0 ? void 0 : list.childId) === null || _a === void 0 ? void 0 : _a.includes('carousel')) {
                                list.type = 'carousel';
                                entryCarousal.push(list);
                            }
                        });
                        channelId.carousal_type = entryCarousal;
                    }
                    return this.handleChatFlowType(currentNodeId, channelId, this.nodes, io);
                }
                let nextNode = channelId === null || channelId === void 0 ? void 0 : channelId.btnValue;
                if (channelId.inputParams)
                    this.inputParams = channelId.inputParams;
                if (channelId.btnValue && ((_k = channelId === null || channelId === void 0 ? void 0 : channelId.btnValue) === null || _k === void 0 ? void 0 : _k.includes('transferToQueue'))) {
                    const nextNodedata = (_m = (_l = this.nodes) === null || _l === void 0 ? void 0 : _l.find((nodes) => { var _a; return ((_a = nodes === null || nodes === void 0 ? void 0 : nodes.data) === null || _a === void 0 ? void 0 : _a.nodeId) === (channelId === null || channelId === void 0 ? void 0 : channelId.btnValue); })) !== null && _m !== void 0 ? _m : { id: '' };
                    nextNode = nextNodedata.id;
                }
                if (channelId.liveTransfer !== 1) {
                    return this.handleChatFlowType(nextNode, channelId, this.nodes, io);
                }
                // NLP triggers agent connection
                // let liveAgent: any = [];
                // liveAgent = await getAvailableAgentChatList(channelId);
                // if (liveAgent?.length === 0) {
                //   this.chatAdapter.redisadaptor.resetQueue(channelId?.domain_id);
                //   this.waitForLiveAgentResponse(channelId);
                // }
                // if (liveAgent && liveAgent?.length > 0) {
                //   console.log('FOUND AN AVAILABLE AGENT ...');
                //   await handleTransferToLiveAgent(this.chatAdapter, channelId, null, liveAgent);
                // } else {
                //   const getDomainResult: any = await getDomainStatus(channelId?.domain_id);
                //   this.chatAdapter?.io?.to(channelId?.session_id).emit('chat_response', JSON.stringify({ session_id: channelId?.session_id, req_type: 'DISCONNECT', domainStatus: getDomainResult }));
                // }
                // const agentDetails = await transferChatToAvailableAgent(channelId);
                const agentDetails = yield (0, chat_dao_1.getUserListByExt)(channelId);
                console.log('\n handleChatFlowType agentDetails\n', JSON.stringify(agentDetails));
                if (agentDetails && (agentDetails === null || agentDetails === void 0 ? void 0 : agentDetails.length) > 0) {
                    this.channelInformation = Object.assign(Object.assign({}, this.channelInformation), { response: 'success', status: null });
                    log_1.logger.info(`handleTransferToLiveAgent Socket Error: ${JSON.stringify(agentDetails)}`);
                    yield (0, chatflow_helpers_1.handleTransferToLiveAgent)(this.chatAdapter, channelId, null, agentDetails);
                }
                else {
                    const getDomainResult = yield (0, callflow_dao_1.getDomainStatus)(channelId === null || channelId === void 0 ? void 0 : channelId.domain_id);
                    (_p = (_o = this.chatAdapter) === null || _o === void 0 ? void 0 : _o.io) === null || _p === void 0 ? void 0 : _p.to(channelId === null || channelId === void 0 ? void 0 : channelId.session_id).emit('chat_response', JSON.stringify({ session_id: channelId === null || channelId === void 0 ? void 0 : channelId.session_id, req_type: 'DISCONNECT', domainStatus: getDomainResult }));
                    log_1.logger.info(`handleChatFlowType Socket Error: ${JSON.stringify(getDomainResult)}`);
                }
            }
        });
    }
    handleChatFlowType(nodeId, channelId, node, io) {
        var _a, _b, _c, _d, _e, _f, _g, _h, _j, _k, _l, _m, _o, _p, _q, _r, _s, _t, _u, _v, _w, _x, _y, _z, _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69, _70, _71, _72, _73, _74, _75, _76, _77, _78, _79, _80, _81, _82, _83, _84, _85, _86, _87, _88, _89, _90, _91, _92, _93, _94, _95, _96, _97, _98, _99, _100, _101, _102, _103, _104, _105, _106, _107, _108, _109, _110, _111, _112, _113, _114, _115, _116, _117, _118, _119, _120, _121, _122, _123, _124, _125, _126, _127, _128, _129, _130, _131, _132, _133, _134, _135, _136, _137, _138, _139, _140, _141, _142, _143, _144, _145, _146, _147, _148, _149, _150, _151, _152, _153, _154, _155, _156, _157, _158, _159, _160, _161, _162, _163, _164, _165, _166, _167, _168, _169, _170, _171, _172, _173, _174, _175, _176, _177, _178, _179, _180, _181, _182, _183, _184, _185, _186, _187, _188, _189, _190, _191, _192, _193, _194, _195, _196, _197, _198, _199, _200, _201, _202, _203, _204, _205, _206, _207, _208, _209, _210, _211, _212, _213, _214, _215, _216, _217, _218, _219;
        return __awaiter(this, void 0, void 0, function* () {
            console.log('\n****************handleChatFlowType*******************>\n');
            let currentNode = {};
            if (nodeId && (nodeId === null || nodeId === void 0 ? void 0 : nodeId.includes('carousel'))) {
                const childArr = [];
                yield ((_a = channelId === null || channelId === void 0 ? void 0 : channelId.carousal_type) === null || _a === void 0 ? void 0 : _a.map((list) => {
                    if ((list === null || list === void 0 ? void 0 : list.type) === (channelId === null || channelId === void 0 ? void 0 : channelId.call_type)) {
                        childArr.push(list);
                    }
                }));
                const carouselData = [];
                yield (childArr === null || childArr === void 0 ? void 0 : childArr.map((list) => __awaiter(this, void 0, void 0, function* () {
                    const currentNodeData = yield this.getNodeDataById(list === null || list === void 0 ? void 0 : list.childId, node);
                    carouselData.push(currentNodeData);
                })));
                currentNode.carousalChildData = carouselData;
                currentNode.type = 'carousel';
                const carousalParentData = yield this.getNodeDataById(nodeId, node);
                const carousalMessage = JSON === null || JSON === void 0 ? void 0 : JSON.parse((_c = (_b = carousalParentData === null || carousalParentData === void 0 ? void 0 : carousalParentData.data) === null || _b === void 0 ? void 0 : _b.information) === null || _c === void 0 ? void 0 : _c.carousel_data);
                currentNode.message = carousalMessage === null || carousalMessage === void 0 ? void 0 : carousalMessage.message;
            }
            else {
                currentNode = yield this.getNodeDataById(nodeId, node);
                this.currentNodeInfo = currentNode;
            }
            if (this.blpopTimeout || !((_d = this.channelInformation) === null || _d === void 0 ? void 0 : _d.session_id)) {
                const getDomainResult = yield (0, callflow_dao_1.getDomainStatus)(channelId === null || channelId === void 0 ? void 0 : channelId.domain_id);
                (_f = (_e = this.chatAdapter) === null || _e === void 0 ? void 0 : _e.io) === null || _f === void 0 ? void 0 : _f.to(channelId === null || channelId === void 0 ? void 0 : channelId.session_id).emit('chat_response', JSON.stringify({ session_id: channelId === null || channelId === void 0 ? void 0 : channelId.session_id, req_type: 'DISCONNECT', domainStatus: getDomainResult }));
                log_1.logger.info(`blpopTimeout Socket Error: ${JSON.stringify(getDomainResult)}`);
                return;
            }
            switch (currentNode === null || currentNode === void 0 ? void 0 : currentNode.type) {
                case 'entryPoint': {
                    return this.handleChatFlowType(currentNode.data.children.at(0).childId, channelId, node, io);
                    break;
                }
                case 'messageFlow': {
                    try {
                        this.channelInformation = Object.assign(Object.assign({}, this.channelInformation), { response: 'pending' });
                        this.playpromptSuccessTimeout = false;
                        const text = (_h = (_g = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _g === void 0 ? void 0 : _g.information) === null || _h === void 0 ? void 0 : _h.message;
                        const availableContexts = text === null || text === void 0 ? void 0 : text.match(chatflow_helpers_1.contextPattern);
                        const _formattedSelectedKeys = [];
                        if (((_j = this.inputParams) === null || _j === void 0 ? void 0 : _j.length) > 0) {
                            const mergedObject = this.inputParams.reduce((result, currentObj) => {
                                Object.keys(currentObj).forEach((key) => {
                                    result[key] = currentObj[key];
                                });
                                return result;
                            }, {});
                            for (const [key, value] of Object.entries(mergedObject)) {
                                /* tslint:disable */
                                _formattedSelectedKeys.push({ tempkey: '${context.' + key + '}', originalkey: key, [key]: value });
                                this.addContextVariable(key, value);
                            }
                        }
                        // const updatedMessage = availableContexts?.length > 0 ? replacechatPatternWithVariables(availableContexts, text, _formattedSelectedKeys) : text;
                        const data = Object.assign({}, (_k = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _k === void 0 ? void 0 : _k.information);
                        //data.message = updatedMessage;
                        const targetRestructured = { currentNode, data, session_id: channelId === null || channelId === void 0 ? void 0 : channelId.session_id };
                        targetRestructured.msg = (_m = (_l = targetRestructured === null || targetRestructured === void 0 ? void 0 : targetRestructured.data) === null || _l === void 0 ? void 0 : _l.message) !== null && _m !== void 0 ? _m : [];
                        const nextNode = (_q = (_p = (_o = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _o === void 0 ? void 0 : _o.children) === null || _p === void 0 ? void 0 : _p.find((nodes) => (nodes === null || nodes === void 0 ? void 0 : nodes.type) === 'success')) !== null && _q !== void 0 ? _q : { childId: '' };
                        const nextNodedetails = yield this.getNodeDataById(nextNode === null || nextNode === void 0 ? void 0 : nextNode.childId, node);
                        const currentNodeArr = [];
                        yield ((_s = (_r = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _r === void 0 ? void 0 : _r.children) === null || _s === void 0 ? void 0 : _s.map((list) => __awaiter(this, void 0, void 0, function* () {
                            if ((list === null || list === void 0 ? void 0 : list.type) === 'success') {
                                const currentNodeArr1 = yield this.getNodeDataById(list === null || list === void 0 ? void 0 : list.childId, node);
                                currentNodeArr.push(currentNodeArr1);
                            }
                        })));
                        targetRestructured.currentNode = nextNodedetails !== null && nextNodedetails !== void 0 ? nextNodedetails : [];
                        targetRestructured.carousalChildData = currentNodeArr;
                        let message = {};
                        let i = 0;
                        try {
                            (_t = JSON.parse(targetRestructured.msg)) === null || _t === void 0 ? void 0 : _t.map((list) => __awaiter(this, void 0, void 0, function* () {
                                var _220, _221;
                                const textRes = (list === null || list === void 0 ? void 0 : list.type) !== 'text' ? list : list === null || list === void 0 ? void 0 : list.data;
                                const updatedMessage = (availableContexts === null || availableContexts === void 0 ? void 0 : availableContexts.length) > 0 ? (0, chatflow_helpers_1.replacechatPatternWithVariables)(availableContexts, textRes, _formattedSelectedKeys) : textRes;
                                message = {
                                    messageID: (0, uuid_1.v4)(),
                                    sessionId: channelId === null || channelId === void 0 ? void 0 : channelId.session_id,
                                    message: updatedMessage,
                                    sendAt: Date.now(),
                                    messageType: (list === null || list === void 0 ? void 0 : list.type) !== 'text' ? 'file' : 'text',
                                    pdfurl: '',
                                    isBot: true,
                                    isCustomer: false,
                                    fileType: '',
                                    direction: 'OUT',
                                    isLike: 0,
                                    mergeMsg: i > 0 ? 1 : 0
                                };
                                i++;
                                const messageList = { message };
                                const messageType = { agentText: messageList };
                                const stringifiedMessage = { message: messageType };
                                stringifiedMessage.ChatHistoryId = channelId === null || channelId === void 0 ? void 0 : channelId.ChatHistoryId;
                                stringifiedMessage.ChatDurationHistoryId = channelId === null || channelId === void 0 ? void 0 : channelId.ChatDurationHistoryId;
                                (_221 = (_220 = this.chatAdapter) === null || _220 === void 0 ? void 0 : _220.io) === null || _221 === void 0 ? void 0 : _221.to(channelId === null || channelId === void 0 ? void 0 : channelId.session_id).emit('OneToOneChatReceive', message);
                                try {
                                    yield (0, webChat_dao_1.updateChatMessage)(stringifiedMessage);
                                }
                                catch (err) {
                                    console.log(err);
                                }
                            }));
                            targetRestructured.req_type = nextNodedetails === undefined ? 'DISCONNECT' : '';
                            if (((_v = (_u = targetRestructured === null || targetRestructured === void 0 ? void 0 : targetRestructured.currentNode) === null || _u === void 0 ? void 0 : _u.id) === null || _v === void 0 ? void 0 : _v.includes('customInput')) || ((_x = (_w = targetRestructured === null || targetRestructured === void 0 ? void 0 : targetRestructured.currentNode) === null || _w === void 0 ? void 0 : _w.id) === null || _x === void 0 ? void 0 : _x.includes('coupon')) || ((_z = (_y = targetRestructured === null || targetRestructured === void 0 ? void 0 : targetRestructured.currentNode) === null || _y === void 0 ? void 0 : _y.id) === null || _z === void 0 ? void 0 : _z.includes('sendMail'))) {
                                return this.handleChatFlowType((_0 = targetRestructured === null || targetRestructured === void 0 ? void 0 : targetRestructured.currentNode) === null || _0 === void 0 ? void 0 : _0.id, channelId, node, io);
                            }
                            else {
                                (_2 = (_1 = this.chatAdapter) === null || _1 === void 0 ? void 0 : _1.io) === null || _2 === void 0 ? void 0 : _2.to(channelId === null || channelId === void 0 ? void 0 : channelId.session_id).emit('chat_response', JSON.stringify(targetRestructured));
                                log_1.logger.info(`customInput Socket Error: ${JSON.stringify(targetRestructured)}`);
                            }
                        }
                        catch (error) {
                            console.error('Error:', error);
                        }
                    }
                    catch (error) {
                        const nextNode = (_5 = (_4 = (_3 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _3 === void 0 ? void 0 : _3.children) === null || _4 === void 0 ? void 0 : _4.find((element) => (element === null || element === void 0 ? void 0 : element.type) === 'error')) !== null && _5 !== void 0 ? _5 : { childId: '' };
                        return this.handleChatFlowType(nextNode === null || nextNode === void 0 ? void 0 : nextNode.childId, channelId, node, io);
                    }
                    break;
                }
                case 'integration': {
                    try {
                        if (((_7 = (_6 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _6 === void 0 ? void 0 : _6.information) === null || _7 === void 0 ? void 0 : _7.method) === 'api_block') {
                            try {
                                const type = (_9 = (_8 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _8 === void 0 ? void 0 : _8.information) === null || _9 === void 0 ? void 0 : _9.getMethodApi;
                                const rawEndURL = (_11 = (_10 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _10 === void 0 ? void 0 : _10.information) === null || _11 === void 0 ? void 0 : _11.endPointUrl.lastIndexOf('$');
                                const endPointUrl = (type === 'get' && rawEndURL !== -1) ? (_13 = (_12 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _12 === void 0 ? void 0 : _12.information) === null || _13 === void 0 ? void 0 : _13.endPointUrl.substring(0, rawEndURL) : (_15 = (_14 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _14 === void 0 ? void 0 : _14.information) === null || _15 === void 0 ? void 0 : _15.endPointUrl;
                                const url = (type === 'get' && rawEndURL !== -1) ? yield (0, fetch_api_1.replaceDynamicEndPoint)((_17 = (_16 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _16 === void 0 ? void 0 : _16.information) === null || _17 === void 0 ? void 0 : _17.endPointUrl, this.inputParams) : `${endPointUrl}`; // 'https://qaccaasapi.worktual.co.uk/core_service/v1/get_mobile_number_status/94988515546677'//;
                                console.log('url >>>>', url, (_18 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _18 === void 0 ? void 0 : _18.information);
                                const method = (_21 = (_20 = (_19 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _19 === void 0 ? void 0 : _19.information) === null || _20 === void 0 ? void 0 : _20.getMethodApi) !== null && _21 !== void 0 ? _21 : 'get';
                                const selectedKeys = new Set((_24 = (_23 = (_22 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _22 === void 0 ? void 0 : _22.information) === null || _23 === void 0 ? void 0 : _23.selectedKeyFromResponse) !== null && _24 !== void 0 ? _24 : []);
                                const body = (_26 = (_25 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _25 === void 0 ? void 0 : _25.information) === null || _26 === void 0 ? void 0 : _26.apiBody;
                                const headers = (_28 = (_27 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _27 === void 0 ? void 0 : _27.information) === null || _28 === void 0 ? void 0 : _28.apiHeaders;
                                const apiResponse = yield (0, fetch_api_1.fetchApi)({ urlEndPoint: url, urlMethod: method, urlBody: body, urlHeaders: headers, inputParams: this.inputParams });
                                const targetRestructured = { session_id: channelId === null || channelId === void 0 ? void 0 : channelId.session_id };
                                const nameAPI = (_30 = (_29 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _29 === void 0 ? void 0 : _29.information) === null || _30 === void 0 ? void 0 : _30.nameAPI;
                                console.log('apiResponse >>>>>>', apiResponse);
                                if (apiResponse === null || apiResponse === void 0 ? void 0 : apiResponse.results) {
                                    if ((apiResponse === null || apiResponse === void 0 ? void 0 : apiResponse.results.status) === 200 || (nameAPI === 'Sim order' && ((_32 = (_31 = apiResponse === null || apiResponse === void 0 ? void 0 : apiResponse.results) === null || _31 === void 0 ? void 0 : _31.data) === null || _32 === void 0 ? void 0 : _32.status_code) !== 500)) {
                                        // if (apiResponse?.results?.data?.status_code !== 500) {
                                        let finalResponse = yield (0, fetch_api_1.getResponseData)(apiResponse === null || apiResponse === void 0 ? void 0 : apiResponse.results);
                                        const nodeNext = (_35 = (_34 = (_33 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _33 === void 0 ? void 0 : _33.children) === null || _34 === void 0 ? void 0 : _34.find((nodes) => (nodes === null || nodes === void 0 ? void 0 : nodes.type) === 'success')) !== null && _35 !== void 0 ? _35 : { childId: '' };
                                        if (Array.isArray(finalResponse) && finalResponse.length > 0) {
                                            finalResponse = (_36 = finalResponse[0]) !== null && _36 !== void 0 ? _36 : { childId: '' };
                                        }
                                        const response = yield (0, chatflow_helpers_1.mergeNestedObjects)(finalResponse);
                                        console.log('response:::::::::::', response);
                                        if ((response === null || response === void 0 ? void 0 : response.statusCode) === 200 || (response === null || response === void 0 ? void 0 : response.statuscode) === 200 || (response === null || response === void 0 ? void 0 : response.status_code) === 200) {
                                            const _formattedSelectedKeys = [];
                                            for (const [key, value] of Object.entries(response)) {
                                                if (selectedKeys.has(key)) {
                                                    /* tslint:disable */
                                                    _formattedSelectedKeys.push({ tempkey: '${context.' + key + '}', originalkey: key, [key]: value });
                                                    this.addContextVariable(key, value);
                                                }
                                            }
                                            const finalKeys = this.context;
                                            this.context = {};
                                            targetRestructured.selectedKeys = finalKeys;
                                            if (nodeNext.childId.includes('customInput')) {
                                                const nextNodedetails = yield this.getNodeDataById(nodeNext.childId, node);
                                                const _cpynextNodedetails = Object.assign({}, nextNodedetails);
                                                const _childrens = ((_37 = _cpynextNodedetails === null || _cpynextNodedetails === void 0 ? void 0 : _cpynextNodedetails.data) === null || _37 === void 0 ? void 0 : _37.children) || {};
                                                for (let _index = 0; _index < _childrens.length; _index++) {
                                                    const element = _childrens[_index];
                                                    for (let _tempi = 0; _tempi < _formattedSelectedKeys.length; _tempi++) {
                                                        const tempelement = _formattedSelectedKeys[_tempi];
                                                        if (element.type === tempelement.tempkey) {
                                                            _childrens[_index].type = tempelement.value;
                                                        }
                                                    }
                                                }
                                                _cpynextNodedetails.data.children = _childrens;
                                                targetRestructured.currentNode = _cpynextNodedetails;
                                                targetRestructured.result = finalResponse;
                                                return this.handleChatFlowType(nodeNext === null || nodeNext === void 0 ? void 0 : nodeNext.childId, channelId, node, io);
                                                //this.chatAdapter?.io?.to(channelId?.session_id).emit('chat_response', JSON.stringify(targetRestructured));
                                            }
                                            else if (nodeNext.childId.includes('messageFlow')) {
                                                const nextNodedetails = yield this.getNodeDataById(nodeNext.childId, node);
                                                console.log('nextNodedetails >>>>>>>', nextNodedetails);
                                                const text = (_39 = (_38 = nextNodedetails === null || nextNodedetails === void 0 ? void 0 : nextNodedetails.data) === null || _38 === void 0 ? void 0 : _38.information) === null || _39 === void 0 ? void 0 : _39.message;
                                                const availableContexts = text === null || text === void 0 ? void 0 : text.match(chatflow_helpers_1.contextPattern);
                                                const updatedMessage = (availableContexts === null || availableContexts === void 0 ? void 0 : availableContexts.length) > 0 ? (0, chatflow_helpers_1.replacechatPatternWithVariables)(availableContexts, text, _formattedSelectedKeys) : text;
                                                const data = Object.assign({}, (_40 = nextNodedetails === null || nextNodedetails === void 0 ? void 0 : nextNodedetails.data) === null || _40 === void 0 ? void 0 : _40.information);
                                                data.message = `${updatedMessage[0]}`;
                                                const targetRestructured = { nextNodedetails, data, session_id: channelId === null || channelId === void 0 ? void 0 : channelId.session_id };
                                                targetRestructured.msg = updatedMessage;
                                                const messagenextNode = (_43 = (_42 = (_41 = nextNodedetails === null || nextNodedetails === void 0 ? void 0 : nextNodedetails.data) === null || _41 === void 0 ? void 0 : _41.children) === null || _42 === void 0 ? void 0 : _42.find((nodes) => (nodes === null || nodes === void 0 ? void 0 : nodes.type) === 'success')) !== null && _43 !== void 0 ? _43 : { childId: '' };
                                                let messagenextNodedetails = {};
                                                if (messagenextNode.childId) {
                                                    messagenextNodedetails = yield this.getNodeDataById(messagenextNode === null || messagenextNode === void 0 ? void 0 : messagenextNode.childId, node);
                                                }
                                                const match = targetRestructured.msg.match(/"data":"([^"]*)"/);
                                                const extractedData = match ? match[1] : null;
                                                targetRestructured.currentNode = nextNodedetails;
                                                const message = {
                                                    messageID: (0, uuid_1.v4)(),
                                                    sessionId: channelId === null || channelId === void 0 ? void 0 : channelId.session_id,
                                                    message: `${extractedData}`,
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
                                                    yield (0, webChat_dao_1.updateChatMessage)(stringifiedMessage);
                                                }
                                                catch (err) {
                                                    console.log(err);
                                                }
                                                //createSession(stringifiedMessage, channelId?.session_id);
                                                (_45 = (_44 = this.chatAdapter) === null || _44 === void 0 ? void 0 : _44.io) === null || _45 === void 0 ? void 0 : _45.to(channelId === null || channelId === void 0 ? void 0 : channelId.session_id).emit('OneToOneChatReceive', message);
                                                const _cpynextNodedetails = Object.assign({}, messagenextNodedetails);
                                                targetRestructured.currentNode = _cpynextNodedetails;
                                                if (((_47 = (_46 = targetRestructured === null || targetRestructured === void 0 ? void 0 : targetRestructured.currentNode) === null || _46 === void 0 ? void 0 : _46.id) === null || _47 === void 0 ? void 0 : _47.includes('customInput')) || ((_49 = (_48 = targetRestructured === null || targetRestructured === void 0 ? void 0 : targetRestructured.currentNode) === null || _48 === void 0 ? void 0 : _48.id) === null || _49 === void 0 ? void 0 : _49.includes('coupon')) || ((_51 = (_50 = targetRestructured === null || targetRestructured === void 0 ? void 0 : targetRestructured.currentNode) === null || _50 === void 0 ? void 0 : _50.id) === null || _51 === void 0 ? void 0 : _51.includes('sendMail'))) {
                                                    return this.handleChatFlowType(nodeNext === null || nodeNext === void 0 ? void 0 : nodeNext.childId, channelId, node, io);
                                                }
                                                else {
                                                    (_53 = (_52 = this.chatAdapter) === null || _52 === void 0 ? void 0 : _52.io) === null || _53 === void 0 ? void 0 : _53.to(channelId === null || channelId === void 0 ? void 0 : channelId.session_id).emit('chat_response', JSON.stringify(targetRestructured));
                                                    log_1.logger.info(`integration Socket Error: ${JSON.stringify(targetRestructured)}`);
                                                    return this.handleChatFlowType((_54 = targetRestructured === null || targetRestructured === void 0 ? void 0 : targetRestructured.currentNode) === null || _54 === void 0 ? void 0 : _54.id, channelId, node, io);
                                                }
                                            }
                                            else if (nodeNext.childId.includes('integration')) {
                                                return this.handleChatFlowType(nodeNext === null || nodeNext === void 0 ? void 0 : nodeNext.childId, channelId, node, io);
                                            }
                                        }
                                        else {
                                            const nodeNext = (_57 = (_56 = (_55 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _55 === void 0 ? void 0 : _55.children) === null || _56 === void 0 ? void 0 : _56.find((nodes) => (nodes === null || nodes === void 0 ? void 0 : nodes.type) === 'error')) !== null && _57 !== void 0 ? _57 : { childId: '' };
                                            return this.handleChatFlowType(nodeNext === null || nodeNext === void 0 ? void 0 : nodeNext.childId, channelId, node, io);
                                        }
                                    }
                                    else {
                                        const nodeNext = (_60 = (_59 = (_58 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _58 === void 0 ? void 0 : _58.children) === null || _59 === void 0 ? void 0 : _59.find((nodes) => (nodes === null || nodes === void 0 ? void 0 : nodes.type) === 'error')) !== null && _60 !== void 0 ? _60 : { childId: '' };
                                        // if (nodeNext.childId.includes('messageFlow')) {
                                        //   const nextNodedetails: any = await this.getNodeDataById(nodeNext.childId, node);
                                        //   const text = nextNodedetails?.data?.information?.message;
                                        //   const availableContexts = text?.match(contextPattern);
                                        //   const updatedMessage = availableContexts?.length > 0 ? replacePatternWithVariables(availableContexts, text, this.context) : text;
                                        //   const data = { ...nextNodedetails?.data?.information };
                                        //   data.message = `${updatedMessage}`;
                                        //   const targetRestructured: any = { nextNodedetails, data, session_id: channelId?.session_id };
                                        //   targetRestructured.msg = nextNodedetails?.data?.information.message;
                                        //   const messagenextNode = nextNodedetails?.data?.children?.find((nodes: any) => nodes?.type === 'success') ?? { childId: '' };
                                        //   let messagenextNodedetails: any = {};
                                        //   if (messagenextNode.childId) {
                                        //     messagenextNodedetails = await this.getNodeDataById(messagenextNode?.childId, node);
                                        //   }
                                        //   const message: any = {
                                        //     messageID: uuidv4(),
                                        //     sessionId: channelId?.session_id,
                                        //     message: `${targetRestructured.msg}`,
                                        //     sendAt: Date.now(),
                                        //     messageType: 'text',
                                        //     pdfurl: '',
                                        //     isBot: true,
                                        //     isCustomer: false,
                                        //     fileType: '',
                                        //     direction: 'OUT',
                                        //     isLike: 0
                                        //   };
                                        //   const messageList: any = { message: message };
                                        //   const messageType: any = { agentText: messageList }
                                        //   const stringifiedMessage: any = { message: messageType };
                                        //   stringifiedMessage.ChatHistoryId = channelId?.ChatHistoryId;
                                        //   stringifiedMessage.ChatDurationHistoryId = channelId?.ChatDurationHistoryId;
                                        //   try {
                                        //     await updateChatMessage(stringifiedMessage)
                                        //   } catch (err) {
                                        //     console.log(err)
                                        //   }
                                        //   this.chatAdapter?.io?.to(channelId?.session_id).emit('OneToOneChatReceive', message);
                                        //   const _cpynextNodedetails: any = { ...messagenextNodedetails };
                                        //   targetRestructured.currentNode = _cpynextNodedetails;
                                        //   this.chatAdapter?.io?.to(channelId?.session_id).emit('chat_response', JSON.stringify(targetRestructured));
                                        // }
                                        return this.handleChatFlowType(nodeNext === null || nodeNext === void 0 ? void 0 : nodeNext.childId, channelId, node, io);
                                    }
                                }
                            }
                            catch (error) {
                                const nextNode = (_63 = (_62 = (_61 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _61 === void 0 ? void 0 : _61.children) === null || _62 === void 0 ? void 0 : _62.find((nodes) => (nodes === null || nodes === void 0 ? void 0 : nodes.type) === 'error')) !== null && _63 !== void 0 ? _63 : { childId: '' };
                                return this.handleChatFlowType(nextNode === null || nextNode === void 0 ? void 0 : nextNode.childId, channelId, node, io);
                            }
                            const body = {
                                domainId: channelId === null || channelId === void 0 ? void 0 : channelId.domain_id,
                                chatFlowId: channelId === null || channelId === void 0 ? void 0 : channelId.callFlowId,
                                apiCallCount: 1
                            };
                            yield (0, callflow_dao_1.insertApiCallCount)(body);
                        }
                        else {
                            (_65 = (_64 = this.chatAdapter) === null || _64 === void 0 ? void 0 : _64.io) === null || _65 === void 0 ? void 0 : _65.to(channelId === null || channelId === void 0 ? void 0 : channelId.session_id).emit('chat_response', JSON.stringify(currentNode));
                            (_67 = (_66 = this.chatAdapter) === null || _66 === void 0 ? void 0 : _66.io) === null || _67 === void 0 ? void 0 : _67.to(channelId === null || channelId === void 0 ? void 0 : channelId.session_id).emit('insertApiCallCount', JSON.stringify(currentNode));
                        }
                    }
                    catch (error) {
                        const nextNode = (_70 = (_69 = (_68 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _68 === void 0 ? void 0 : _68.children) === null || _69 === void 0 ? void 0 : _69.find((nodes) => (nodes === null || nodes === void 0 ? void 0 : nodes.type) === 'error')) !== null && _70 !== void 0 ? _70 : { childId: '' };
                        return this.handleChatFlowType(nextNode === null || nextNode === void 0 ? void 0 : nextNode.childId, channelId, node, io);
                    }
                    break;
                }
                case 'datePicker': {
                    try {
                        const parseDateMessage = JSON.parse((_72 = (_71 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _71 === void 0 ? void 0 : _71.information) === null || _72 === void 0 ? void 0 : _72.datepicker_data);
                        const _formattedSelectedKeys = [];
                        if (((_73 = this.inputParams) === null || _73 === void 0 ? void 0 : _73.length) > 0) {
                            const mergedObject = this.inputParams.reduce((result, currentObj) => {
                                Object.keys(currentObj).forEach((key) => {
                                    result[key] = currentObj[key];
                                });
                                return result;
                            }, {});
                            for (const [key, value] of Object.entries(mergedObject)) {
                                /* tslint:disable */
                                _formattedSelectedKeys.push({ tempkey: '${context.' + key + '}', originalkey: key, [key]: value });
                                this.addContextVariable(key, value);
                            }
                        }
                        const text = parseDateMessage === null || parseDateMessage === void 0 ? void 0 : parseDateMessage.message;
                        const availableContexts = text === null || text === void 0 ? void 0 : text.match(chatflow_helpers_1.contextPattern);
                        const updatedMessage = (availableContexts === null || availableContexts === void 0 ? void 0 : availableContexts.length) > 0 ? (0, chatflow_helpers_1.replacechatPatternWithVariables)(availableContexts, text, _formattedSelectedKeys) : text;
                        const checkEmpty = updatedMessage.replace(/<[^>]*>/g, "").trim().length > 0 ? true : false;
                        if (checkEmpty) {
                            let message = {
                                messageID: (0, uuid_1.v4)(),
                                sessionId: channelId === null || channelId === void 0 ? void 0 : channelId.session_id,
                                message: updatedMessage,
                                sendAt: Date.now(),
                                messageType: 'text',
                                pdfurl: '',
                                isBot: true,
                                isCustomer: false,
                                fileType: '',
                                direction: 'OUT',
                                isLike: 0,
                            };
                            const messageList = { message };
                            const messageType = { agentText: messageList };
                            const stringifiedMessage = { message: messageType };
                            stringifiedMessage.ChatHistoryId = channelId === null || channelId === void 0 ? void 0 : channelId.ChatHistoryId;
                            stringifiedMessage.ChatDurationHistoryId = channelId === null || channelId === void 0 ? void 0 : channelId.ChatDurationHistoryId;
                            (_75 = (_74 = this.chatAdapter) === null || _74 === void 0 ? void 0 : _74.io) === null || _75 === void 0 ? void 0 : _75.to(channelId === null || channelId === void 0 ? void 0 : channelId.session_id).emit('OneToOneChatReceive', message);
                            try {
                                yield (0, webChat_dao_1.updateChatMessage)(stringifiedMessage);
                            }
                            catch (err) {
                                console.log(err);
                            }
                        }
                        (_77 = (_76 = this.chatAdapter) === null || _76 === void 0 ? void 0 : _76.io) === null || _77 === void 0 ? void 0 : _77.to(channelId === null || channelId === void 0 ? void 0 : channelId.session_id).emit('chat_response', JSON.stringify(currentNode));
                        log_1.logger.info(`chatAdapter Socket Error: ${JSON.stringify(currentNode)}`);
                    }
                    catch (error) {
                        const nextNode = (_80 = (_79 = (_78 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _78 === void 0 ? void 0 : _78.children) === null || _79 === void 0 ? void 0 : _79.find((nodes) => (nodes === null || nodes === void 0 ? void 0 : nodes.type) === 'error')) !== null && _80 !== void 0 ? _80 : { childId: '' };
                        return this.handleChatFlowType(nextNode === null || nextNode === void 0 ? void 0 : nextNode.childId, channelId, node, io);
                    }
                    break;
                }
                case 'appointment': {
                    try {
                        (_82 = (_81 = this.chatAdapter) === null || _81 === void 0 ? void 0 : _81.io) === null || _82 === void 0 ? void 0 : _82.to(channelId === null || channelId === void 0 ? void 0 : channelId.session_id).emit('chat_response', JSON.stringify(currentNode));
                        log_1.logger.info(`appointment Socket Error: ${JSON.stringify(currentNode)}`);
                    }
                    catch (error) {
                        const nextNode = (_85 = (_84 = (_83 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _83 === void 0 ? void 0 : _83.children) === null || _84 === void 0 ? void 0 : _84.find((nodes) => (nodes === null || nodes === void 0 ? void 0 : nodes.type) === 'error')) !== null && _85 !== void 0 ? _85 : { childId: '' };
                        return this.handleChatFlowType(nextNode === null || nextNode === void 0 ? void 0 : nextNode.childId, channelId, node, io);
                    }
                    break;
                }
                case 'careHomeAuth': {
                    try {
                        log_1.logger.info(`careHomeAuth Socket Error: ${JSON.stringify(currentNode)}`);
                        return (_87 = (_86 = this.chatAdapter) === null || _86 === void 0 ? void 0 : _86.io) === null || _87 === void 0 ? void 0 : _87.to(channelId === null || channelId === void 0 ? void 0 : channelId.session_id).emit('chat_response', JSON.stringify(currentNode));
                    }
                    catch (err) {
                        const nextNode = (_90 = (_89 = (_88 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _88 === void 0 ? void 0 : _88.children) === null || _89 === void 0 ? void 0 : _89.find((nodes) => (nodes === null || nodes === void 0 ? void 0 : nodes.type) === 'error')) !== null && _90 !== void 0 ? _90 : { childId: '' };
                        return this.handleChatFlowType(nextNode === null || nextNode === void 0 ? void 0 : nextNode.childId, channelId, node, io);
                    }
                }
                case 'coupon': {
                    try {
                        const CoupanData = {
                            currentNode,
                            type: 'coupon'
                        };
                        const parseDateMessage = JSON.parse((_92 = (_91 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _91 === void 0 ? void 0 : _91.information) === null || _92 === void 0 ? void 0 : _92.coupon_data);
                        const _formattedSelectedKeys = [];
                        if (((_93 = this.inputParams) === null || _93 === void 0 ? void 0 : _93.length) > 0) {
                            const mergedObject = this.inputParams.reduce((result, currentObj) => {
                                Object.keys(currentObj).forEach((key) => {
                                    result[key] = currentObj[key];
                                });
                                return result;
                            }, {});
                            for (const [key, value] of Object.entries(mergedObject)) {
                                /* tslint:disable */
                                _formattedSelectedKeys.push({ tempkey: '${context.' + key + '}', originalkey: key, [key]: value });
                                this.addContextVariable(key, value);
                            }
                        }
                        const text = parseDateMessage === null || parseDateMessage === void 0 ? void 0 : parseDateMessage.message;
                        const availableContexts = text === null || text === void 0 ? void 0 : text.match(chatflow_helpers_1.contextPattern);
                        const updatedMessage = (availableContexts === null || availableContexts === void 0 ? void 0 : availableContexts.length) > 0 ? (0, chatflow_helpers_1.replacechatPatternWithVariables)(availableContexts, text, _formattedSelectedKeys) : text;
                        const checkEmpty = updatedMessage.replace(/<[^>]*>/g, "").trim().length > 0 ? true : false;
                        if (checkEmpty) {
                            let message = {
                                messageID: (0, uuid_1.v4)(),
                                sessionId: channelId === null || channelId === void 0 ? void 0 : channelId.session_id,
                                couponId: parseDateMessage === null || parseDateMessage === void 0 ? void 0 : parseDateMessage.coupon,
                                message: updatedMessage,
                                sendAt: Date.now(),
                                messageType: 'text',
                                pdfurl: '',
                                isBot: true,
                                isCustomer: false,
                                fileType: '',
                                direction: 'OUT',
                                isCoupanStatus: true,
                                isLike: 0,
                            };
                            const messageList = { message };
                            const messageType = { agentText: messageList };
                            const stringifiedMessage = { message: messageType };
                            stringifiedMessage.ChatHistoryId = channelId === null || channelId === void 0 ? void 0 : channelId.ChatHistoryId;
                            stringifiedMessage.ChatDurationHistoryId = channelId === null || channelId === void 0 ? void 0 : channelId.ChatDurationHistoryId;
                            (_95 = (_94 = this.chatAdapter) === null || _94 === void 0 ? void 0 : _94.io) === null || _95 === void 0 ? void 0 : _95.to(channelId === null || channelId === void 0 ? void 0 : channelId.session_id).emit('OneToOneChatReceive', message);
                            try {
                                yield (0, webChat_dao_1.updateChatMessage)(stringifiedMessage);
                            }
                            catch (err) {
                                console.log(err);
                            }
                        }
                        (_97 = (_96 = this.chatAdapter) === null || _96 === void 0 ? void 0 : _96.io) === null || _97 === void 0 ? void 0 : _97.to(channelId === null || channelId === void 0 ? void 0 : channelId.session_id).emit('chat_response', JSON.stringify(CoupanData));
                        log_1.logger.info(`CoupanData Socket Error: ${JSON.stringify(CoupanData)}`);
                    }
                    catch (error) {
                        const nextNode = (_100 = (_99 = (_98 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _98 === void 0 ? void 0 : _98.children) === null || _99 === void 0 ? void 0 : _99.find((nodes) => (nodes === null || nodes === void 0 ? void 0 : nodes.type) === 'error')) !== null && _100 !== void 0 ? _100 : { childId: '' };
                        return this.handleChatFlowType(nextNode === null || nextNode === void 0 ? void 0 : nextNode.childId, channelId, node, io);
                    }
                    break;
                }
                case 'addFlow': {
                    try {
                        const nodesString = yield (0, callflow_dao_1.getnodesdata)((_102 = (_101 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _101 === void 0 ? void 0 : _101.information) === null || _102 === void 0 ? void 0 : _102.flow_id);
                        currentNode.addFlowNodes = nodesString;
                        (_104 = (_103 = this.chatAdapter) === null || _103 === void 0 ? void 0 : _103.io) === null || _104 === void 0 ? void 0 : _104.to(channelId === null || channelId === void 0 ? void 0 : channelId.session_id).emit('chat_response', JSON.stringify(currentNode));
                        log_1.logger.info(`addFlow Socket Error: ${JSON.stringify(currentNode)}`);
                    }
                    catch (error) {
                        const nextNode = (_107 = (_106 = (_105 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _105 === void 0 ? void 0 : _105.children) === null || _106 === void 0 ? void 0 : _106.find((nodes) => (nodes === null || nodes === void 0 ? void 0 : nodes.type) === 'error')) !== null && _107 !== void 0 ? _107 : { childId: '' };
                        return this.handleChatFlowType(nextNode === null || nextNode === void 0 ? void 0 : nextNode.childId, channelId, node, io);
                    }
                    break;
                }
                case 'carousel': {
                    try {
                        const _formattedSelectedKeys = [];
                        if (((_108 = this.inputParams) === null || _108 === void 0 ? void 0 : _108.length) > 0) {
                            const mergedObject = this.inputParams.reduce((result, currentObj) => {
                                Object.keys(currentObj).forEach((key) => {
                                    result[key] = currentObj[key];
                                });
                                return result;
                            }, {});
                            for (const [key, value] of Object.entries(mergedObject)) {
                                /* tslint:disable */
                                _formattedSelectedKeys.push({ tempkey: '${context.' + key + '}', originalkey: key, [key]: value });
                                this.addContextVariable(key, value);
                            }
                        }
                        const text = currentNode === null || currentNode === void 0 ? void 0 : currentNode.message;
                        const availableContexts = text === null || text === void 0 ? void 0 : text.match(chatflow_helpers_1.contextPattern);
                        const updatedMessage = (availableContexts === null || availableContexts === void 0 ? void 0 : availableContexts.length) > 0 ? (0, chatflow_helpers_1.replacechatPatternWithVariables)(availableContexts, text, _formattedSelectedKeys) : text;
                        const checkEmpty = updatedMessage.replace(/<[^>]*>/g, "").trim().length > 0 ? true : false;
                        if (checkEmpty) {
                            let message = {
                                messageID: (0, uuid_1.v4)(),
                                sessionId: channelId === null || channelId === void 0 ? void 0 : channelId.session_id,
                                message: updatedMessage,
                                sendAt: Date.now(),
                                messageType: 'text',
                                pdfurl: '',
                                isBot: true,
                                isCustomer: false,
                                fileType: '',
                                direction: 'OUT',
                                isLike: 0,
                            };
                            const messageList = { message };
                            const messageType = { agentText: messageList };
                            const stringifiedMessage = { message: messageType };
                            stringifiedMessage.ChatHistoryId = channelId === null || channelId === void 0 ? void 0 : channelId.ChatHistoryId;
                            stringifiedMessage.ChatDurationHistoryId = channelId === null || channelId === void 0 ? void 0 : channelId.ChatDurationHistoryId;
                            (_110 = (_109 = this.chatAdapter) === null || _109 === void 0 ? void 0 : _109.io) === null || _110 === void 0 ? void 0 : _110.to(channelId === null || channelId === void 0 ? void 0 : channelId.session_id).emit('OneToOneChatReceive', message);
                            try {
                                yield (0, webChat_dao_1.updateChatMessage)(stringifiedMessage);
                            }
                            catch (err) {
                                console.log(err);
                            }
                        }
                        (_112 = (_111 = this.chatAdapter) === null || _111 === void 0 ? void 0 : _111.io) === null || _112 === void 0 ? void 0 : _112.to(channelId === null || channelId === void 0 ? void 0 : channelId.session_id).emit('chat_response', JSON.stringify(currentNode));
                        log_1.logger.info(`carousel Socket Error: ${JSON.stringify(currentNode)}`);
                    }
                    catch (error) {
                        const nextNode = (_115 = (_114 = (_113 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _113 === void 0 ? void 0 : _113.children) === null || _114 === void 0 ? void 0 : _114.find((nodes) => (nodes === null || nodes === void 0 ? void 0 : nodes.type) === 'error')) !== null && _115 !== void 0 ? _115 : { childId: '' };
                        return this.handleChatFlowType(nextNode === null || nextNode === void 0 ? void 0 : nextNode.childId, channelId, node, io);
                    }
                    break;
                }
                case 'payment': {
                    try {
                        (_117 = (_116 = this.chatAdapter) === null || _116 === void 0 ? void 0 : _116.io) === null || _117 === void 0 ? void 0 : _117.to(channelId === null || channelId === void 0 ? void 0 : channelId.session_id).emit('chat_response', JSON.stringify(currentNode));
                        log_1.logger.info(`payment Socket Error: ${JSON.stringify(currentNode)}`);
                    }
                    catch (error) {
                        const nextNode = (_120 = (_119 = (_118 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _118 === void 0 ? void 0 : _118.children) === null || _119 === void 0 ? void 0 : _119.find((nodes) => (nodes === null || nodes === void 0 ? void 0 : nodes.type) === 'error')) !== null && _120 !== void 0 ? _120 : { childId: '' };
                        return this.handleChatFlowType(nextNode === null || nextNode === void 0 ? void 0 : nextNode.childId, channelId, node, io);
                    }
                    break;
                }
                case 'sendMail': {
                    try {
                        (_122 = (_121 = this.chatAdapter) === null || _121 === void 0 ? void 0 : _121.io) === null || _122 === void 0 ? void 0 : _122.to(channelId === null || channelId === void 0 ? void 0 : channelId.session_id).emit('chat_response', JSON.stringify(currentNode));
                        log_1.logger.info(`sendMail Socket Error: ${JSON.stringify(currentNode)}`);
                    }
                    catch (error) {
                        const nextNode = (_125 = (_124 = (_123 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _123 === void 0 ? void 0 : _123.children) === null || _124 === void 0 ? void 0 : _124.find((nodes) => (nodes === null || nodes === void 0 ? void 0 : nodes.type) === 'error')) !== null && _125 !== void 0 ? _125 : { childId: '' };
                        return this.handleChatFlowType(nextNode === null || nextNode === void 0 ? void 0 : nextNode.childId, channelId, node, io);
                    }
                    break;
                }
                case 'randomise': {
                    try {
                        const randonIndex = Math.floor(Math.random() * ((_127 = (_126 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _126 === void 0 ? void 0 : _126.children) === null || _127 === void 0 ? void 0 : _127.length));
                        const childIdData = (_128 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _128 === void 0 ? void 0 : _128.children[randonIndex];
                        if ((_129 = childIdData === null || childIdData === void 0 ? void 0 : childIdData.childId) === null || _129 === void 0 ? void 0 : _129.includes('messageFlow')) {
                            return this.handleChatFlowType(childIdData === null || childIdData === void 0 ? void 0 : childIdData.childId, channelId, node, io);
                        }
                        else {
                            const currentNodeData = yield this.getNodeDataById(childIdData === null || childIdData === void 0 ? void 0 : childIdData.childId, node);
                            if ((_130 = childIdData === null || childIdData === void 0 ? void 0 : childIdData.childId) === null || _130 === void 0 ? void 0 : _130.includes('carousel')) {
                                const data = {};
                                data.carousalChildData = [currentNodeData];
                                data.type = 'carousel';
                                (_132 = (_131 = this.chatAdapter) === null || _131 === void 0 ? void 0 : _131.io) === null || _132 === void 0 ? void 0 : _132.to(channelId === null || channelId === void 0 ? void 0 : channelId.session_id).emit('chat_response', JSON.stringify(data));
                                log_1.logger.info(`randomise Socket Error: ${JSON.stringify(data)}`);
                            }
                            else {
                                (_134 = (_133 = this.chatAdapter) === null || _133 === void 0 ? void 0 : _133.io) === null || _134 === void 0 ? void 0 : _134.to(channelId === null || channelId === void 0 ? void 0 : channelId.session_id).emit('chat_response', JSON.stringify(currentNodeData));
                                log_1.logger.info(`randomise1 Socket Error: ${JSON.stringify(currentNodeData)}`);
                            }
                        }
                    }
                    catch (error) {
                        const nextNode = (_137 = (_136 = (_135 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _135 === void 0 ? void 0 : _135.children) === null || _136 === void 0 ? void 0 : _136.find((nodes) => (nodes === null || nodes === void 0 ? void 0 : nodes.type) === 'error')) !== null && _137 !== void 0 ? _137 : { childId: '' };
                        return this.handleChatFlowType(nextNode === null || nextNode === void 0 ? void 0 : nextNode.childId, channelId, node, io);
                    }
                    break;
                }
                case 'customInput': {
                    try {
                        const _formattedSelectedKeys = [];
                        if (((_138 = this.inputParams) === null || _138 === void 0 ? void 0 : _138.length) > 0) {
                            const mergedObject = this.inputParams.reduce((result, currentObj) => {
                                Object.keys(currentObj).forEach((key) => {
                                    result[key] = currentObj[key];
                                });
                                return result;
                            }, {});
                            for (const [key, value] of Object.entries(mergedObject)) {
                                /* tslint:disable */
                                _formattedSelectedKeys.push({ tempkey: '${context.' + key + '}', originalkey: key, [key]: value });
                                this.addContextVariable(key, value);
                            }
                        }
                        // console.log("cus999999999999999999999999999999999",channelId)
                        this.channelInformation = Object.assign(Object.assign({}, this.channelInformation), { response: 'pending' });
                        this.playpromptSuccessTimeout = false;
                        const text = (_140 = (_139 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _139 === void 0 ? void 0 : _139.information) === null || _140 === void 0 ? void 0 : _140.message;
                        const availableContexts = text === null || text === void 0 ? void 0 : text.match(chatflow_helpers_1.contextPattern);
                        const updatedMessage = (availableContexts === null || availableContexts === void 0 ? void 0 : availableContexts.length) > 0 ? (0, chatflow_helpers_1.replacechatPatternWithVariables)(availableContexts, text, _formattedSelectedKeys) : text;
                        const checkEmpty = updatedMessage.replace(/<[^>]*>/g, "").trim().length > 0 ? true : false;
                        if ((checkEmpty || [14, 15].includes((_142 = (_141 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _141 === void 0 ? void 0 : _141.information) === null || _142 === void 0 ? void 0 : _142.inputType)) && ((_144 = (_143 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _143 === void 0 ? void 0 : _143.information) === null || _144 === void 0 ? void 0 : _144.inputType) !== 3) {
                            let message = {
                                messageID: (0, uuid_1.v4)(),
                                sessionId: channelId === null || channelId === void 0 ? void 0 : channelId.session_id,
                                message: updatedMessage,
                                sendAt: Date.now(),
                                messageType: 'text',
                                pdfurl: '',
                                isBot: true,
                                isCustomer: false,
                                fileType: '',
                                direction: 'OUT',
                                isLike: 0,
                                stream_finished: channelId === null || channelId === void 0 ? void 0 : channelId.stream_finished,
                                chatFlowId: channelId === null || channelId === void 0 ? void 0 : channelId.callFlowId
                            };
                            if ([1, 15].includes((_146 = (_145 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _145 === void 0 ? void 0 : _145.information) === null || _146 === void 0 ? void 0 : _146.inputType)) {
                                message.buttonDataArr = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data;
                                message.flowIdData = currentNode;
                                message.isButton = 1;
                                message.messageType = "customButton";
                            }
                            if (((_148 = (_147 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _147 === void 0 ? void 0 : _147.information) === null || _148 === void 0 ? void 0 : _148.inputType) === 14) {
                                message.isDropDown = 1;
                                message.dropDownDataArr = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data;
                                message.flowIdData = currentNode;
                            }
                            const messageList = { message };
                            const messageType = { agentText: messageList };
                            const stringifiedMessage = { message: messageType };
                            stringifiedMessage.ChatHistoryId = channelId === null || channelId === void 0 ? void 0 : channelId.ChatHistoryId;
                            stringifiedMessage.ChatDurationHistoryId = channelId === null || channelId === void 0 ? void 0 : channelId.ChatDurationHistoryId;
                            (_150 = (_149 = this.chatAdapter) === null || _149 === void 0 ? void 0 : _149.io) === null || _150 === void 0 ? void 0 : _150.to(channelId === null || channelId === void 0 ? void 0 : channelId.session_id).emit('OneToOneChatReceive', message);
                            try {
                                yield (0, webChat_dao_1.updateChatMessage)(stringifiedMessage);
                            }
                            catch (err) {
                                console.log(err);
                            }
                        }
                        // const updatedMessage = availableContexts?.length > 0 ? replacePatternWithVariables(availableContexts, text, this.context) : text;
                        const data = Object.assign({}, (_151 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _151 === void 0 ? void 0 : _151.information);
                        data.message = updatedMessage;
                        const targetRestructured = { currentNode, data, session_id: channelId === null || channelId === void 0 ? void 0 : channelId.session_id };
                        targetRestructured.msg = updatedMessage;
                        targetRestructured.currentNode.data.information.message = updatedMessage;
                        yield Promise.all((_154 = (_153 = (_152 = targetRestructured === null || targetRestructured === void 0 ? void 0 : targetRestructured.currentNode) === null || _152 === void 0 ? void 0 : _152.data) === null || _153 === void 0 ? void 0 : _153.children) === null || _154 === void 0 ? void 0 : _154.map((list) => __awaiter(this, void 0, void 0, function* () {
                            const translateResult = yield this.translateItem(list === null || list === void 0 ? void 0 : list.type, channelId);
                            list.type = translateResult;
                        })));
                        (_157 = (_156 = (_155 = this.chatAdapter) === null || _155 === void 0 ? void 0 : _155.io) === null || _156 === void 0 ? void 0 : _156.of('/')) === null || _157 === void 0 ? void 0 : _157.to(channelId === null || channelId === void 0 ? void 0 : channelId.session_id).emit('chat_response', JSON.stringify(targetRestructured));
                        log_1.logger.info(`translateItem Socket Error: ${JSON.stringify(targetRestructured)}`);
                    }
                    catch (error) {
                        const nextNode = (_160 = (_159 = (_158 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _158 === void 0 ? void 0 : _158.children) === null || _159 === void 0 ? void 0 : _159.find((element) => (element === null || element === void 0 ? void 0 : element.type) === 'error')) !== null && _160 !== void 0 ? _160 : { childId: '' };
                        return this.handleChatFlowType(nextNode === null || nextNode === void 0 ? void 0 : nextNode.childId, channelId, node, io);
                    }
                    break;
                }
                case 'transferToQueue': {
                    try {
                        channelId.skills = (_162 = (_161 = currentNode.data) === null || _161 === void 0 ? void 0 : _161.information) === null || _162 === void 0 ? void 0 : _162.skill;
                        channelId.queueId = (_164 = (_163 = currentNode.data) === null || _163 === void 0 ? void 0 : _163.information) === null || _164 === void 0 ? void 0 : _164.transferTo;
                        this.channelInformation = Object.assign(Object.assign({}, this.channelInformation), { response: 'pending' });
                        this.waitForAgentResponse(channelId);
                        if (((_166 = (_165 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _165 === void 0 ? void 0 : _165.information) === null || _166 === void 0 ? void 0 : _166.type) === 1) {
                            yield (0, transferToQueue_1.insertCustomerInQueue)(channelId, currentNode, channelId === null || channelId === void 0 ? void 0 : channelId.queueId);
                        }
                        const currentAgentDetails = yield (0, transferToQueue_1.transferWithoutRouting)(channelId, currentNode);
                        const agentDetails = yield this.checkConcurrentChatFlagCheck(currentAgentDetails);
                        if (agentDetails && (agentDetails === null || agentDetails === void 0 ? void 0 : agentDetails.length) > 0) {
                            if (((_168 = (_167 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _167 === void 0 ? void 0 : _167.information) === null || _168 === void 0 ? void 0 : _168.type) === 2) {
                                yield (0, chatflow_helpers_1.handleTransferToQueue)(this.chatAdapter, channelId, currentNode, agentDetails);
                                this.channelInformation = Object.assign(Object.assign({}, this.channelInformation), { response: 'success', status: null });
                            }
                            else {
                                const list = {
                                    qid: (_170 = (_169 = currentNode.data) === null || _169 === void 0 ? void 0 : _169.information) === null || _170 === void 0 ? void 0 : _170.transferTo,
                                    domainId: channelId === null || channelId === void 0 ? void 0 : channelId.domain_id
                                };
                                const getQueueList = yield (0, webChat_dao_1.getQueueWaitTime)(list);
                                const parsedData = JSON.parse((_171 = getQueueList === null || getQueueList === void 0 ? void 0 : getQueueList[0]) === null || _171 === void 0 ? void 0 : _171.queueList);
                                const resultData = parsedData === null || parsedData === void 0 ? void 0 : parsedData.map((list) => {
                                    if ((list === null || list === void 0 ? void 0 : list.sessionId) === (channelId === null || channelId === void 0 ? void 0 : channelId.session_id)) {
                                        list.chatNotifyStatus = true;
                                        list.notifyQueuedTime = Math.floor(Date.now() / 1000);
                                    }
                                    return list;
                                });
                                const payload = {
                                    qid: (_173 = (_172 = getQueueList === null || getQueueList === void 0 ? void 0 : getQueueList[0]) === null || _172 === void 0 ? void 0 : _172.qid) !== null && _173 !== void 0 ? _173 : 0,
                                    queueName: (_174 = getQueueList === null || getQueueList === void 0 ? void 0 : getQueueList[0]) === null || _174 === void 0 ? void 0 : _174.queueName,
                                    queueType: 'chat',
                                    domainId: (_175 = getQueueList === null || getQueueList === void 0 ? void 0 : getQueueList[0]) === null || _175 === void 0 ? void 0 : _175.domainId,
                                    queueList: JSON.stringify(resultData)
                                };
                                try {
                                    yield (0, webChat_dao_1.insertUpdateQueueWaitTimeDao)(payload);
                                }
                                catch (err) {
                                    console.log(err);
                                }
                                agentDetails.queueId = (_177 = (_176 = currentNode.data) === null || _176 === void 0 ? void 0 : _176.information) === null || _177 === void 0 ? void 0 : _177.transferTo;
                                yield (0, chatflow_helpers_1.handleTransferToQueue)(this.chatAdapter, channelId, currentNode, agentDetails);
                                this.channelInformation = Object.assign(Object.assign({}, this.channelInformation), { response: 'success', status: null });
                            }
                        }
                        else {
                            if (((_179 = (_178 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _178 === void 0 ? void 0 : _178.information) === null || _179 === void 0 ? void 0 : _179.type) === 2) {
                                this.chatAdapter.redisadaptor.resetQueue(channelId === null || channelId === void 0 ? void 0 : channelId.domain_id); // added new line
                                const nextNodes = (_182 = (_181 = (_180 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _180 === void 0 ? void 0 : _180.children) === null || _181 === void 0 ? void 0 : _181.find((element) => (element === null || element === void 0 ? void 0 : element.type) === 'error')) !== null && _182 !== void 0 ? _182 : { childId: '' };
                                return this.handleChatFlowType(nextNodes === null || nextNodes === void 0 ? void 0 : nextNodes.childId, channelId, node, io);
                            }
                            // else {
                            //   insertCustomerInQueue(channelId, currentNode, channelId?.queueId)
                            // }
                        }
                    }
                    catch (error) {
                        const nextNodeData = (_185 = (_184 = (_183 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _183 === void 0 ? void 0 : _183.children) === null || _184 === void 0 ? void 0 : _184.find((element) => (element === null || element === void 0 ? void 0 : element.type) === 'error')) !== null && _185 !== void 0 ? _185 : { childId: '' };
                        return this.handleChatFlowType(nextNodeData === null || nextNodeData === void 0 ? void 0 : nextNodeData.childId, channelId, node, io);
                    }
                    break;
                }
                case 'checkCondition': {
                    try {
                        const parseDateMessage = JSON.parse((_187 = (_186 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _186 === void 0 ? void 0 : _186.information) === null || _187 === void 0 ? void 0 : _187.condition_data);
                        const _formattedSelectedKeys = [];
                        if (((_188 = this.inputParams) === null || _188 === void 0 ? void 0 : _188.length) > 0) {
                            const mergedObject = this.inputParams.reduce((result, currentObj) => {
                                Object.keys(currentObj).forEach((key) => {
                                    result[key] = currentObj[key];
                                });
                                return result;
                            }, {});
                            for (const [key, value] of Object.entries(mergedObject)) {
                                /* tslint:disable */
                                _formattedSelectedKeys.push({ tempkey: '${context.' + key + '}', originalkey: key, [key]: value });
                                this.addContextVariable(key, value);
                            }
                        }
                        const text = parseDateMessage === null || parseDateMessage === void 0 ? void 0 : parseDateMessage.message;
                        const availableContexts = text === null || text === void 0 ? void 0 : text.match(chatflow_helpers_1.contextPattern);
                        const updatedMessage = (availableContexts === null || availableContexts === void 0 ? void 0 : availableContexts.length) > 0 ? (0, chatflow_helpers_1.replacechatPatternWithVariables)(availableContexts, text, _formattedSelectedKeys) : text;
                        const checkEmpty = updatedMessage.replace(/<[^>]*>/g, "").trim().length > 0 ? true : false;
                        if (checkEmpty) {
                            let message = {
                                messageID: (0, uuid_1.v4)(),
                                sessionId: channelId === null || channelId === void 0 ? void 0 : channelId.session_id,
                                message: updatedMessage,
                                sendAt: Date.now(),
                                messageType: 'text',
                                pdfurl: '',
                                isBot: true,
                                isCustomer: false,
                                fileType: '',
                                direction: 'OUT',
                                isLike: 0,
                            };
                            const messageList = { message };
                            const messageType = { agentText: messageList };
                            const stringifiedMessage = { message: messageType };
                            stringifiedMessage.ChatHistoryId = channelId === null || channelId === void 0 ? void 0 : channelId.ChatHistoryId;
                            stringifiedMessage.ChatDurationHistoryId = channelId === null || channelId === void 0 ? void 0 : channelId.ChatDurationHistoryId;
                            (_190 = (_189 = this.chatAdapter) === null || _189 === void 0 ? void 0 : _189.io) === null || _190 === void 0 ? void 0 : _190.to(channelId === null || channelId === void 0 ? void 0 : channelId.session_id).emit('OneToOneChatReceive', message);
                            try {
                                yield (0, webChat_dao_1.updateChatMessage)(stringifiedMessage);
                            }
                            catch (err) {
                                console.log(err);
                            }
                        }
                        const data = (_191 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _191 === void 0 ? void 0 : _191.information;
                        const leftOperand = (0, chatflow_helpers_1.replaceContextWithVariables)(data === null || data === void 0 ? void 0 : data.leftOperand, this.context);
                        const rightOperands = (_192 = data === null || data === void 0 ? void 0 : data.conditions) !== null && _192 !== void 0 ? _192 : [];
                        let resultIndex = -1;
                        for (const stock of rightOperands) {
                            const result = yield (0, conditions_mapper_1.conditionsMapper)(leftOperand, stock === null || stock === void 0 ? void 0 : stock.operand, (0, chatflow_helpers_1.replaceContextWithVariables)(stock === null || stock === void 0 ? void 0 : stock.rightOperand, this.context));
                            if (result) {
                                resultIndex = stock === null || stock === void 0 ? void 0 : stock.index;
                                break;
                            }
                        }
                        if (resultIndex !== -1) {
                            const nodeNext = (_195 = (_194 = (_193 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _193 === void 0 ? void 0 : _193.children) === null || _194 === void 0 ? void 0 : _194.find((nodes) => (nodes === null || nodes === void 0 ? void 0 : nodes.type) === `condition${resultIndex + 1}`)) !== null && _195 !== void 0 ? _195 : { childId: '' };
                            return this.handleChatFlowType(nodeNext === null || nodeNext === void 0 ? void 0 : nodeNext.childId, channelId, node, io);
                        }
                        const nextNode = (_198 = (_197 = (_196 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _196 === void 0 ? void 0 : _196.children) === null || _197 === void 0 ? void 0 : _197.find((nodes) => (nodes === null || nodes === void 0 ? void 0 : nodes.type) === 'error')) !== null && _198 !== void 0 ? _198 : { childId: '' };
                        return this.handleChatFlowType(nextNode === null || nextNode === void 0 ? void 0 : nextNode.childId, channelId, node, io);
                    }
                    catch (error) {
                        const nextNode = (_201 = (_200 = (_199 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _199 === void 0 ? void 0 : _199.children) === null || _200 === void 0 ? void 0 : _200.find((nodes) => (nodes === null || nodes === void 0 ? void 0 : nodes.type) === 'error')) !== null && _201 !== void 0 ? _201 : { childId: '' };
                        return this.handleChatFlowType(nextNode === null || nextNode === void 0 ? void 0 : nextNode.childId, channelId, node, io);
                    }
                    break;
                }
                case 'loop': {
                    try {
                        console.log('this.sessionLoops[channelId?.session_id] >>>>>>', this.sessionLoops[channelId === null || channelId === void 0 ? void 0 : channelId.session_id]);
                        if (!this.sessionLoops[channelId === null || channelId === void 0 ? void 0 : channelId.session_id] && this.sessionLoops[channelId === null || channelId === void 0 ? void 0 : channelId.session_id] !== 0) {
                            this.sessionLoops[channelId === null || channelId === void 0 ? void 0 : channelId.session_id] = (_204 = (_203 = (_202 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _202 === void 0 ? void 0 : _202.information) === null || _203 === void 0 ? void 0 : _203.loopValue) !== null && _204 !== void 0 ? _204 : 0;
                        }
                        const loopTo = (_207 = (_206 = (_205 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _205 === void 0 ? void 0 : _205.children) === null || _206 === void 0 ? void 0 : _206.find((nodee) => (nodee === null || nodee === void 0 ? void 0 : nodee.type) === 'loop')) !== null && _207 !== void 0 ? _207 : { childId: '' };
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
                                (_209 = (_208 = this.chatAdapter) === null || _208 === void 0 ? void 0 : _208.io) === null || _209 === void 0 ? void 0 : _209.to(channelId === null || channelId === void 0 ? void 0 : channelId.session_id).emit('chat_response', JSON.stringify(targetRestructured));
                                log_1.logger.info(`nextNodedetails Socket Error: ${JSON.stringify(targetRestructured)}`);
                            }
                        }
                        else {
                            const nextNode = (_212 = (_211 = (_210 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _210 === void 0 ? void 0 : _210.children) === null || _211 === void 0 ? void 0 : _211.find((nodes) => (nodes === null || nodes === void 0 ? void 0 : nodes.type) === 'complete')) !== null && _212 !== void 0 ? _212 : { childId: '' };
                            const nextNodedetails = yield this.getNodeDataById(nextNode.childId, node);
                            console.log('Loop Complete>>>>>>', nextNode.childId);
                            return this.handleChatFlowType(nextNode === null || nextNode === void 0 ? void 0 : nextNode.childId, channelId, node, io);
                        }
                    }
                    catch (error) {
                        const nextNode = (_215 = (_214 = (_213 = currentNode === null || currentNode === void 0 ? void 0 : currentNode.data) === null || _213 === void 0 ? void 0 : _213.children) === null || _214 === void 0 ? void 0 : _214.find((nodee) => (nodee === null || nodee === void 0 ? void 0 : nodee.type) === 'error')) !== null && _215 !== void 0 ? _215 : { childId: '' };
                        return this.handleChatFlowType(nextNode === null || nextNode === void 0 ? void 0 : nextNode.childId, channelId, node, io);
                    }
                    break;
                }
                case 'disconnect': {
                    console.log('FOUND DISCONNECTED NODE. GOING TO DISCONNECT');
                    yield new Promise(resolve => setTimeout(resolve, 500));
                    const getDomainResult = yield (0, callflow_dao_1.getDomainStatus)(channelId === null || channelId === void 0 ? void 0 : channelId.domain_id);
                    (_217 = (_216 = this.chatAdapter) === null || _216 === void 0 ? void 0 : _216.io) === null || _217 === void 0 ? void 0 : _217.to(channelId === null || channelId === void 0 ? void 0 : channelId.session_id).emit('chat_response', JSON.stringify({ session_id: channelId === null || channelId === void 0 ? void 0 : channelId.session_id, req_type: 'DISCONNECT', domainStatus: getDomainResult }));
                    log_1.logger.info(`disconnect Socket Error: ${JSON.stringify(getDomainResult)}`);
                    return;
                }
                default: {
                    console.log('Default calling');
                    this.blpopTimeout = true;
                    const getDomainResult = yield (0, callflow_dao_1.getDomainStatus)(channelId === null || channelId === void 0 ? void 0 : channelId.domain_id);
                    (_219 = (_218 = this.chatAdapter) === null || _218 === void 0 ? void 0 : _218.io) === null || _219 === void 0 ? void 0 : _219.to(channelId === null || channelId === void 0 ? void 0 : channelId.session_id).emit('chat_response', JSON.stringify({ session_id: channelId === null || channelId === void 0 ? void 0 : channelId.session_id, req_type: 'DISCONNECT' }));
                    return;
                }
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
    BLPOP(domainId, list, routingProfiles, sessionId, userId) {
        return __awaiter(this, void 0, void 0, function* () {
            yield db_1.otherRedisClient.blpop(`${domainId}_${process.env.AGENTTRACKERCHANNEL}`, 1).then((data) => __awaiter(this, void 0, void 0, function* () {
                if (this.blpopTimeout) {
                    console.log('CANCELLING SEARCHING FOR AGENT BECAUSE OF DISCONNECTED CALL ...');
                    list.push(null);
                    return list;
                }
                if (data !== null && (data === null || data === void 0 ? void 0 : data.length) > 1) {
                    const parsedData = JSON.parse(data === null || data === void 0 ? void 0 : data.at(1));
                    if (((parsedData === null || parsedData === void 0 ? void 0 : parsedData.statusName) === 'Ready' || (parsedData === null || parsedData === void 0 ? void 0 : parsedData.statusName) === 'Transfer Only') && ((routingProfiles === null || routingProfiles === void 0 ? void 0 : routingProfiles.includes(parsedData === null || parsedData === void 0 ? void 0 : parsedData.routingProfileId)) || userId === (parsedData === null || parsedData === void 0 ? void 0 : parsedData.UserID))) {
                        console.log('AGENT FOUND AS: ', data);
                        list.push([parsedData]);
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
                if (this.blpopTimeout) {
                    console.log('CANCELLING SEARCHING FOR AGENT BECAUSE OF DISCONNECTED CALL ...');
                    list.push(null);
                    return list;
                }
                if (data !== null && (data === null || data === void 0 ? void 0 : data.length) > 1) {
                    const parsedData = JSON.parse(data === null || data === void 0 ? void 0 : data.at(1));
                    if (((parsedData === null || parsedData === void 0 ? void 0 : parsedData.statusName) === 'Ready' || (parsedData === null || parsedData === void 0 ? void 0 : parsedData.statusName) === 'Transfer Only')) {
                        console.log('AGENT FOUND AS: ', data);
                        list.push([parsedData]);
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
    checkConcurrentChatFlagCheck(array) {
        return __awaiter(this, void 0, void 0, function* () {
            try {
                for (const item of array) {
                    const res = yield (0, callflow_dao_1.getConcurrentChatLimit)({
                        domainId: item.domainId,
                        UserID: item.userId
                    });
                    if (res.flagresult === 1) {
                        return [item];
                    }
                }
                return [];
            }
            catch (err) {
                console.error(err);
                return [];
            }
        });
    }
}
exports.ChatFlowAdapters = ChatFlowAdapters;
//# sourceMappingURL=chatflow2.0.js.map