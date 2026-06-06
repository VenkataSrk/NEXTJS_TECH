"use strict";
var __createBinding = (this && this.__createBinding) || (Object.create ? (function(o, m, k, k2) {
    if (k2 === undefined) k2 = k;
    var desc = Object.getOwnPropertyDescriptor(m, k);
    if (!desc || ("get" in desc ? !m.__esModule : desc.writable || desc.configurable)) {
      desc = { enumerable: true, get: function() { return m[k]; } };
    }
    Object.defineProperty(o, k2, desc);
}) : (function(o, m, k, k2) {
    if (k2 === undefined) k2 = k;
    o[k2] = m[k];
}));
var __setModuleDefault = (this && this.__setModuleDefault) || (Object.create ? (function(o, v) {
    Object.defineProperty(o, "default", { enumerable: true, value: v });
}) : function(o, v) {
    o["default"] = v;
});
var __importStar = (this && this.__importStar) || function (mod) {
    if (mod && mod.__esModule) return mod;
    var result = {};
    if (mod != null) for (var k in mod) if (k !== "default" && Object.prototype.hasOwnProperty.call(mod, k)) __createBinding(result, mod, k);
    __setModuleDefault(result, mod);
    return result;
};
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
exports.IOServer = void 0;
const bluebird_1 = __importDefault(require("bluebird"));
const jwt = __importStar(require("jsonwebtoken"));
const socket_io_1 = require("socket.io");
const uuid_1 = require("uuid");
const chatflow2_0_1 = require("../chatflow2.0");
const chatflow_helpers_1 = require("../chatflow_helpers");
const app_1 = require("../config/app");
const constants_1 = require("../constants");
const callflow_dao_1 = require("../dao/callflow.dao");
const chat_dao_1 = require("../dao/chat.dao");
const dispositionStatus_1 = require("../dao/dispositionStatus");
const internalChat_1 = require("../dao/internalChat");
const socialmedia_1 = require("../dao/socialmedia");
const webChat_dao_1 = require("../dao/webChat.dao");
const fetch_api_1 = require("../fetch_api");
const whatsappTransfer_1 = require("../helperFunctions/whatsappTransfer");
const constants_2 = require("../helpers/constants");
const db_1 = require("../plugins/db");
const log_1 = require("../plugins/log");
const redisAdapters_1 = require("../redisAdapters");
const aichat_1 = require("../redisAdapters/aichat");
const aidashboard_1 = require("../redisAdapters/aidashboard");
const agentQueueManager_1 = require("./agentQueueManager");
class IOServer {
    constructor(httpServer) {
        this.io = new socket_io_1.Server(httpServer, {
            path: '/ccascall-service',
            cors: {
                origin: '*',
                credentials: true,
            },
            pingInterval: 25000,
            pingTimeout: 60000,
        });
        IOServer.sessionInstances = {};
        this.connectionhandler();
        this.redisadaptor = new redisAdapters_1.RedisAdapters(this);
        this.aichat = new aichat_1.Aichat(this);
        this.aidashboard = new aidashboard_1.Aidashboard(this);
        this.addAdaptor();
        this.addAuthMiddleware();
        bluebird_1.default.promisifyAll(db_1.redisClient);
        this.io.emit('Test', 'USER');
        console.log('CALLED');
    }
    receiveMessageFromNLP(data) {
        const parsedData = JSON.parse(data !== null && data !== void 0 ? data : null);
        if (parsedData) {
            console.log('receiveMessageFromNLP parsedData >>>>>>>', parsedData);
            this.io.of('/').to(parsedData.sessionId).emit('airesponse', parsedData);
            log_1.logger.info(`receiveMessageFromNLP Socket Error: ${JSON.stringify(parsedData)}`);
        }
    }
    receiveMessageFromTemplateResponse(data) {
        const parsedData = JSON.parse(data !== null && data !== void 0 ? data : null);
        if (parsedData) {
            console.log('receiveMessageFromTemplateResponse parsedData >>>>>>>', parsedData);
            this.io.of('/').to(parsedData.sessionId).emit('airesponse', parsedData);
            log_1.logger.info(`receiveMessageFromTemplateResponse Socket Error: ${JSON.stringify(parsedData)}`);
        }
    }
    receiveMessageFromWebscrape(data) {
        const parsedData = JSON.parse(data !== null && data !== void 0 ? data : null);
        if (parsedData) {
            console.log('receiveMessageFromWebscrape parsedData >>>>>>>', parsedData);
            this.io.of('/').to(`${parsedData === null || parsedData === void 0 ? void 0 : parsedData.domainId}_${parsedData === null || parsedData === void 0 ? void 0 : parsedData.ext}`).emit('webscrapresponse', parsedData);
            log_1.logger.info(`receiveMessageFromWebscrape Socket Error: ${JSON.stringify(parsedData)}`);
        }
    }
    receiveMessageFromLiteResponse(data) {
        const parsedData = JSON.parse(data !== null && data !== void 0 ? data : null);
        if (parsedData) {
            console.log('receiveMessageFromLiteResponse parsedData >>>>>>>', parsedData);
            this.io.of('/').to(parsedData.sessionId).emit('airesponse', parsedData);
            log_1.logger.info(`receiveMessageFromLiteResponse Socket Error: ${JSON.stringify(parsedData)}`);
        }
    }
    receiveMessageFromAIDashboardResponse(data) {
        const parsedData = JSON.parse(data !== null && data !== void 0 ? data : null);
        if (parsedData) {
            console.log('Receive AI Dashboard Message from Nlp :', parsedData);
            this.io.of('/').to(parsedData.fromId).emit('ai_dashboard_response', parsedData);
            log_1.logger.info(`Receive AI Dashboard Message from Nlp : ${JSON.stringify(parsedData)}`);
        }
    }
    receiveMessageFromAIChatResponse(data) {
        const parsedData = JSON.parse(data !== null && data !== void 0 ? data : null);
        if (parsedData) {
            console.log('Receive AI chat Message from Nlp :', parsedData);
            this.io.of('/').to(parsedData.fromId).emit('ai_chat_response', parsedData);
            log_1.logger.info(`Receive AI chat Message from Nlp : ${JSON.stringify(parsedData)}`);
        }
    }
    receiveMessageFromAgentAssitResponse(data) {
        const parsedData = JSON.parse(data !== null && data !== void 0 ? data : null);
        if (parsedData) {
            parsedData.messageId = (0, uuid_1.v4)();
            try {
                // InsertUpdateAIChatMessage(parsedData);
            }
            catch (err) {
                console.log('err:::::', err);
            }
            console.log('receiveMessageFromAgentAssist parsedData >>>>>>>', parsedData.summary);
            this.io.of('/').to(parsedData.to).emit('agentassist', parsedData);
            // if (parsedData?.summary) {
            //   const test: any = JSON.parse(parsedData.data);
            //   test.message = parsedData.summary;
            //   const data1: any = {
            //     ChatHistoryId: test?.ChatHistoryId,
            //     dispositionId: test?.dispositionId,
            //     Summary: parsedData?.summary,
            //     ChatSubject: test?.ChatSubject ?? ''
            //   };
            //   // const stringifiedMessage = { message: res, chatSession: 1, sessionEndTime: Date.now(), agentDisconnectTime: Date.now() };
            //   // createSession(stringifiedMessage, test?.sessionId);
            //   // createChatSession(data1);
            // }
        }
        log_1.logger.info(`receiveMessageFromAgentAssitResponse Socket Error: ${JSON.stringify(parsedData)}`);
    }
    // public receiveMessageFromAgentAssist(data: any): void {
    //   console.log('receiveMessageFromAgentAssist', data);
    //   const parsedData = JSON.parse(data ?? null);
    //   if (parsedData) {
    //     console.log('receiveMessageFromAgentAssist parsedData >>>>>>>', parsedData);
    //     this.io.of('/').to(parsedData.sessionId).emit('agentassist', parsedData);
    //   }
    // }
    receiveMessageFromAgentAssist(data) {
        var _a;
        const parsedData = JSON.parse(data !== null && data !== void 0 ? data : null);
        if (parsedData) {
            parsedData.messageId = (0, uuid_1.v4)();
            try {
                (0, webChat_dao_1.InsertUpdateAIChatMessage)(parsedData);
            }
            catch (err) {
                console.log('err:::::', err);
            }
            console.log('receiveMessageFromAgentAssist parsedData >>>>>>>', parsedData.summary);
            this.io.of('/').to(parsedData.to).emit('agentassist', parsedData);
            if (parsedData === null || parsedData === void 0 ? void 0 : parsedData.summary) {
                const test = JSON.parse(parsedData.data);
                test.message = parsedData.summary;
                const data1 = {
                    ChatHistoryId: test === null || test === void 0 ? void 0 : test.ChatHistoryId,
                    dispositionId: test === null || test === void 0 ? void 0 : test.dispositionId,
                    Summary: parsedData === null || parsedData === void 0 ? void 0 : parsedData.summary,
                    ChatSubject: (_a = test === null || test === void 0 ? void 0 : test.ChatSubject) !== null && _a !== void 0 ? _a : ''
                };
            }
            log_1.logger.info(`receiveMessageFromAgentAssist Socket Error: ${JSON.stringify(parsedData)}`);
        }
    }
    close() {
        if (this.io) {
            this.io.close();
        }
    }
    addAdaptor() {
        this.io.adapter(this.redisadaptor.getredisAdapter());
    }
    addAuthMiddleware() {
        this.io.use((socket, next) => {
            var _a, _b;
            if (!((_b = (_a = socket === null || socket === void 0 ? void 0 : socket.handshake) === null || _a === void 0 ? void 0 : _a.auth) === null || _b === void 0 ? void 0 : _b.token)) {
                next();
                return;
            }
            const socketData = jwt.decode(socket.handshake.auth.token);
            try {
                if (!socketData.sipLoginId || !socketData.domainId || !socketData.deviceId || !socket.handshake.auth.token) {
                    const error_report = {
                        message: 'Unauthorized- Query param missing',
                        code: 403,
                    };
                    next(new Error(JSON.stringify(error_report)));
                }
                else {
                    try {
                        const decoded = jwt.verify(socket.handshake.auth.token, app_1.configs.jwtsecret);
                        if (parseInt(decoded.sipLoginId, 10) === parseInt(socketData.sipLoginId, 10)) {
                            this.redisadaptor
                                .getValuefromredis(`${decoded.sipLoginId}_${decoded.source}`)
                                .then((res) => {
                                if (res !== null && res !== undefined) {
                                    const checkToken = JSON.parse(res !== null && res !== void 0 ? res : 'null');
                                    console.log('checkToken?.token \n', checkToken === null || checkToken === void 0 ? void 0 : checkToken.token, '\n socket.handshake \n', socket.handshake.auth.token, (checkToken === null || checkToken === void 0 ? void 0 : checkToken.token) === socket.handshake.auth.token);
                                    if ((checkToken === null || checkToken === void 0 ? void 0 : checkToken.token) === socket.handshake.auth.token) {
                                        next();
                                    }
                                    else {
                                        const error_report = {
                                            message: 'Unauthorized1',
                                            code: 403,
                                        };
                                        next(new Error(JSON.stringify(error_report)));
                                    }
                                }
                                else {
                                    const error_report = {
                                        message: 'Unauthorized2',
                                        code: 403,
                                    };
                                    next(new Error(JSON.stringify(error_report)));
                                }
                            })
                                .catch((err) => {
                                const error_report = {
                                    message: 'Unauthorized - Token not exist',
                                    code: 403,
                                };
                                next(new Error(JSON.stringify(error_report)));
                            });
                        }
                        else {
                            const error_report = {
                                message: 'Unauthorized - sipLoginId mismatch',
                                code: 403,
                            };
                            next(new Error(JSON.stringify(error_report)));
                        }
                    }
                    catch (e) {
                        const error_report = {
                            message: 'Unauthorized - Invalid signature',
                            code: 403,
                        };
                        next(new Error(JSON.stringify(error_report)));
                    }
                }
            }
            catch (e) {
                const error_report = {
                    message: 'Unauthorized',
                    code: 403,
                };
                next(new Error(JSON.stringify(error_report)));
            }
        });
    }
    handleChatSessions(data, socket) {
        var _a, _b;
        const parsedData = JSON.parse(data !== null && data !== void 0 ? data : 'null');
        if ((parsedData === null || parsedData === void 0 ? void 0 : parsedData.btnValue) && ((_a = parsedData === null || parsedData === void 0 ? void 0 : parsedData.btnValue) === null || _a === void 0 ? void 0 : _a.includes('transferToQueue'))) {
            // this.redisadaptor.BLPOP('', parsedData.domain_id, parsedData);
        }
        if ((parsedData === null || parsedData === void 0 ? void 0 : parsedData.call_state) === 'init') {
            const newObject = new chatflow2_0_1.ChatFlowAdapters(this);
            IOServer.sessionInstances[parsedData === null || parsedData === void 0 ? void 0 : parsedData.session_id] = newObject;
            const exit = newObject === null || newObject === void 0 ? void 0 : newObject.ChatFlowHandler(data);
            console.log('FLOW IS NOW EXITING 1', exit);
        }
        else if (parsedData === null || parsedData === void 0 ? void 0 : parsedData.disconnectedBy) {
            const exit = (_b = IOServer.sessionInstances[parsedData === null || parsedData === void 0 ? void 0 : parsedData.session_id]) === null || _b === void 0 ? void 0 : _b.ChatFlowHandler(data);
            console.log('FLOW IS NOW EXITING 2', exit);
            IOServer.sessionInstances[parsedData === null || parsedData === void 0 ? void 0 : parsedData.session_id] = null;
            delete IOServer.sessionInstances[parsedData === null || parsedData === void 0 ? void 0 : parsedData.session_id];
            try {
                socket.disconnect(true);
            }
            catch (err) {
                console.log('Error disconnecting socket:', err);
            }
        }
        else {
            const newObject = new chatflow2_0_1.ChatFlowAdapters(this);
            console.log('FLOW IS NOW EXITING 3');
            // const exit = IOServer.sessionInstances[parsedData?.session_id]?.ChatFlowHandler(data);
            const exit = newObject === null || newObject === void 0 ? void 0 : newObject.ChatFlowHandler(data);
        }
    }
    connectionhandler() {
        this.io.on('connection', (socket) => __awaiter(this, void 0, void 0, function* () {
            var _a, _b;
            console.log(`User connected: ${socket.id}`);
            socket.on('agent_ext_socket_id_mapping', (data) => {
                console.log('agent_ext_socket_id_mapping', data, socket.id);
                (0, chat_dao_1.agentExtSocketIdMapping)(data, socket.id);
            });
            socket.on('disconnect', (reason) => __awaiter(this, void 0, void 0, function* () {
                var _c, _d, _e, _f, _g, _h, _j, _k, _m, _o;
                try {
                    console.log(`User disconnected: ${socket.id}, Reason: ${reason}`);
                    log_1.logger.info(`Disconnect Socket Log:::: ${socket.handshake.auth.id} >>> ${JSON.stringify(reason)}`);
                    const ignoredReasons = ['ping timeout', 'transport close', 'client namespace disconnect', 'transport error'];
                    if (ignoredReasons.includes(reason)) {
                        log_1.logger.info(`Skipping decline workflow due to auto disconnect: ${reason}`);
                        return; // 🔥 stops auto decline
                    }
                    const agentDetails = yield (0, chat_dao_1.getAgentDetails)(socket.id);
                    if (((_c = agentDetails[0]) === null || _c === void 0 ? void 0 : _c.domainId) && ((_d = agentDetails[0]) === null || _d === void 0 ? void 0 : _d.ext)) {
                        const concurrentChat = yield (0, chat_dao_1.getConcurrentChat)(agentDetails[0]);
                        if ((_e = concurrentChat === null || concurrentChat === void 0 ? void 0 : concurrentChat[0]) === null || _e === void 0 ? void 0 : _e.agentOccupyDetails) {
                            const temp = (_h = JSON.parse((_g = (_f = concurrentChat === null || concurrentChat === void 0 ? void 0 : concurrentChat[0]) === null || _f === void 0 ? void 0 : _f.agentOccupyDetails) !== null && _g !== void 0 ? _g : 'null')) !== null && _h !== void 0 ? _h : '';
                            temp.currentChatCount = Object.assign(Object.assign({}, temp === null || temp === void 0 ? void 0 : temp.currentChatCount), { [(_j = agentDetails === null || agentDetails === void 0 ? void 0 : agentDetails[0]) === null || _j === void 0 ? void 0 : _j.ext]: 0 });
                            const value = Object.assign(Object.assign({}, concurrentChat === null || concurrentChat === void 0 ? void 0 : concurrentChat[0]), { agentOccupyDetails: JSON.stringify(temp) });
                            const updateConcurrentLimit = yield (0, chat_dao_1.insertUpdateConcurrentChat)(value);
                            console.log('updateConcurrentLimit', updateConcurrentLimit);
                            if (((_k = updateConcurrentLimit === null || updateConcurrentLimit === void 0 ? void 0 : updateConcurrentLimit[0]) === null || _k === void 0 ? void 0 : _k.errCode) === 0) {
                                (0, chat_dao_1.agentExtSocketIdDelete)(socket.id);
                            }
                        }
                    }
                    const domainId = (_m = socket.handshake.auth.id.split('_')) === null || _m === void 0 ? void 0 : _m[0];
                    const ext = (_o = socket.handshake.auth.id.split('_')) === null || _o === void 0 ? void 0 : _o[1];
                    const token = yield db_1.redisClientAuth.get(`LOGGED_IN_USER_TOKEN_${domainId}_${ext}_Web`);
                    if (token === socket.handshake.auth.token) {
                        (0, constants_1.updateInactivity)(domainId, ext, true);
                    }
                }
                catch (err) {
                    log_1.logger.info(`Disconnect Socket Log:::: ${socket.handshake.auth.id} >>> ${JSON.stringify(reason)}`);
                    log_1.logger.info(`Disconnect Socket Log:::: err ${err}`);
                    console.log('disconnect:::::::::', err);
                }
            }));
            if ((_b = (_a = socket === null || socket === void 0 ? void 0 : socket.handshake) === null || _a === void 0 ? void 0 : _a.auth) === null || _b === void 0 ? void 0 : _b.id) {
                const email = socket.handshake.auth.id.split('_')[0];
                socket.join(`${email}`);
            }
            /** INTERNAL CHAT */
            socket.on('internal_chat_msg', (data) => __awaiter(this, void 0, void 0, function* () {
                var _p;
                try {
                    if (data.internalChatId === null) {
                        const chatRes = yield (0, internalChat_1.newInternalChatMsg)(data);
                        if (Array.isArray(chatRes) && (chatRes === null || chatRes === void 0 ? void 0 : chatRes.length) > 0) {
                            this.io.of('/').to(data === null || data === void 0 ? void 0 : data.agentUserId).emit('internal_chat_msg_recevie', Object.assign(Object.assign({}, data), { internalChatId: (_p = chatRes[0]) === null || _p === void 0 ? void 0 : _p.internalChatId }));
                        }
                    }
                    else if ((data === null || data === void 0 ? void 0 : data.internalChatId) !== null) {
                        const chatReadStatusRes = yield (0, internalChat_1.internalChatReadStatus)(data);
                        if (Array.isArray(chatReadStatusRes) && (chatReadStatusRes === null || chatReadStatusRes === void 0 ? void 0 : chatReadStatusRes.length) > 0) {
                            this.io.of('/').to(data === null || data === void 0 ? void 0 : data.agentUserId).emit('internal_chat_msg_status', Object.assign(Object.assign({}, data), { readStatus: chatReadStatusRes }));
                        }
                    }
                    log_1.logger.info(`internal_chat_msg Socket Error: ${JSON.stringify(data)}`);
                }
                catch (error) {
                    console.log('internal_chat_msg', error);
                }
            }));
            // *** For WebChat Connection ***//
            socket.on('joinWebChat', (room) => __awaiter(this, void 0, void 0, function* () {
                console.log('joinwebchat>>>>>>>>>>>>>>>>>>>>>>>>>>>', room);
                socket.join(`${room.webChatSessionId}`);
            }));
            // socket.on('queue_handling', async (data: any) => {
            //   console.log('queue_handling data', data);
            //   this.io.of('/').to(data?.to).emit('requestDecline', data);
            //   // data.domain_id = data?.domainId;
            //   // const readyAgentsData: any = await readyAgents(data); // checking agents in ready state
            //   // console.log('\nreadyAgentsData------->', JSON.stringify(readyAgentsData));
            //   // updateSessionDetails(data);
            // });
            socket.on('updateRoutingProfile', (data) => __awaiter(this, void 0, void 0, function* () {
                console.log('\n\nupdateRoutingProfile----------->', data);
                const response = yield db_1.otherRedisClient.hmget(`${data.domain_id}_presence`, `${data.domain_id}_${data.ext}`);
                const result = Object.values(response).map((value) => JSON.parse(value));
                if ((result === null || result === void 0 ? void 0 : result.length) > 0 && result[0] !== null) {
                    const ss = yield db_1.otherRedisClient.hset(`${data.domain_id}_presence`, `${data === null || data === void 0 ? void 0 : data.domain_id}_${data === null || data === void 0 ? void 0 : data.ext}`, JSON.stringify(Object.assign(Object.assign({}, result[0]), { routingProfileId: data === null || data === void 0 ? void 0 : data.routing_profile })));
                    const sendData = { ext: data === null || data === void 0 ? void 0 : data.ext, domainId: data === null || data === void 0 ? void 0 : data.domain_id, id: data === null || data === void 0 ? void 0 : data.routing_profile };
                    this.io.of('/').to(`${data === null || data === void 0 ? void 0 : data.domain_id}`).emit('updateAgentRoutingProfile', sendData);
                }
                else {
                    console.log('updateRoutingProfile else case');
                }
                log_1.logger.info(`internal_chat_msg Socket Error: ${JSON.stringify(data)}`);
            }));
            socket.on('agent_ready_for_queue_users', (data) => __awaiter(this, void 0, void 0, function* () {
                try {
                    // Use new AgentQueueManager instead of old JSON.stringify approach
                    const enqueued = yield agentQueueManager_1.AgentQueueManager.enqueue(data);
                    if (!enqueued) {
                        log_1.logger.error(`Failed to enqueue - queue may be full. ext:${data.ext}, domainId:${data.domainId}`);
                        // Get queue metrics for debugging
                        const metrics = yield agentQueueManager_1.AgentQueueManager.getHealthMetrics();
                        log_1.logger.error(`Queue metrics: ${JSON.stringify(metrics)}`);
                    }
                    log_1.logger.info(`agent_ready_for_queue_users: ${JSON.stringify(data)}`);
                }
                catch (err) {
                    log_1.logger.error(`Error in agent_ready_for_queue_users handler: ${err}`);
                }
            }));
            socket.on('botConnectedTime', (room) => __awaiter(this, void 0, void 0, function* () {
                var _q, _r;
                const data = {
                    ChatHistoryId: (_q = room === null || room === void 0 ? void 0 : room.ChatHistoryId) !== null && _q !== void 0 ? _q : null,
                    StartTime: Math.floor(Date.now()),
                    IsBot: 1,
                    BotId: (_r = room === null || room === void 0 ? void 0 : room.botId) !== null && _r !== void 0 ? _r : 1,
                };
                try {
                    const botStartTime = yield (0, webChat_dao_1.updateAgentDetails)(data);
                    this.io.of('/').to(room === null || room === void 0 ? void 0 : room.session_id).emit('botConnectedResponse', botStartTime[0]);
                }
                catch (err) {
                    console.log(err);
                }
                log_1.logger.info(`botConnectedTime Socket Error: ${JSON.stringify(room)}`);
            }));
            socket.on('botDisconnectTime', (room) => __awaiter(this, void 0, void 0, function* () {
                var _s, _t, _u, _v;
                const data = {
                    EndTime: Date.now(),
                    ChatDurationHistoryId: (_s = room === null || room === void 0 ? void 0 : room.ChatDurationHistoryId) !== null && _s !== void 0 ? _s : null,
                    ChatHistoryId: (_t = room === null || room === void 0 ? void 0 : room.ChatHistoryId) !== null && _t !== void 0 ? _t : null,
                    transferredTime: Math.floor(Date.now()),
                    dispositionId: (_u = room === null || room === void 0 ? void 0 : room.dispositionId) !== null && _u !== void 0 ? _u : null
                };
                try {
                    yield (0, webChat_dao_1.updateAgentDetails)(data);
                    const nlpData = {
                        HistoryId: room === null || room === void 0 ? void 0 : room.ChatHistoryId,
                        ChatDurationHistoryId: room === null || room === void 0 ? void 0 : room.ChatDurationHistoryId,
                        channelType: 'Chat',
                        domainId: room === null || room === void 0 ? void 0 : room.domainId,
                        ext: (_v = room === null || room === void 0 ? void 0 : room.ext) !== null && _v !== void 0 ? _v : 0
                    };
                    const updateScore = yield (0, webChat_dao_1.sentimentalScore)(nlpData);
                    if ((updateScore === null || updateScore === void 0 ? void 0 : updateScore.status) === 200) {
                        yield (0, webChat_dao_1.updateSentimentScoreChat)(nlpData, updateScore === null || updateScore === void 0 ? void 0 : updateScore.data);
                    }
                    else {
                        console.error('Failed to update sentiment score:', updateScore === null || updateScore === void 0 ? void 0 : updateScore.status);
                    }
                    log_1.logger.info(`botDisconnectTime Socket Error: ${JSON.stringify(room)}`);
                }
                catch (err) {
                    console.log(err);
                }
            }));
            socket.on('presence', (msg) => __awaiter(this, void 0, void 0, function* () {
                var _w;
                console.log('presence :::::::', msg);
                const data = {
                    ext: msg === null || msg === void 0 ? void 0 : msg.ext,
                    domainName: msg === null || msg === void 0 ? void 0 : msg.domainName,
                    deviceType: msg === null || msg === void 0 ? void 0 : msg.deviceType,
                    presence: (_w = msg === null || msg === void 0 ? void 0 : msg.presence) !== null && _w !== void 0 ? _w : 0
                };
                if ((data === null || data === void 0 ? void 0 : data.presence) === 1) {
                    yield db_1.presenceStore.set(`${data === null || data === void 0 ? void 0 : data.ext}@${data === null || data === void 0 ? void 0 : data.domainName}@${data.deviceType}@presence`, data.presence);
                }
                else if ((data === null || data === void 0 ? void 0 : data.presence) === 0) {
                    yield db_1.presenceStore.del(`${data === null || data === void 0 ? void 0 : data.ext}@${data === null || data === void 0 ? void 0 : data.domainName}@${data.deviceType}@presence`);
                }
                log_1.logger.info(`presence Socket Error: ${JSON.stringify(msg)}`);
            }));
            socket.on('join_to_live_agent', (room) => __awaiter(this, void 0, void 0, function* () {
                var _x, _y, _z, _0;
                console.log('listener join_to_live_agent>>>>>>>>>>>>>>>>>>>>>>>>>>>', room);
                socket.join(`${room.webChatSessionId}`);
                socket.join(`${room.domainId}_${room.ext}`);
                const payload = {
                    agentId: `${room.domainId}_${room.ext}`,
                    userId: `${room.webChatSessionId} `,
                    sender: room === null || room === void 0 ? void 0 : room.sender,
                    ChatHistoryId: room === null || room === void 0 ? void 0 : room.ChatHistoryId,
                    cookiesId: room === null || room === void 0 ? void 0 : room.cookiesId,
                    status: 'Connection establish successfully',
                    transferType: (_x = room === null || room === void 0 ? void 0 : room.transferType) !== null && _x !== void 0 ? _x : 0,
                    primaryAgent: (_y = room === null || room === void 0 ? void 0 : room.primaryAgent) !== null && _y !== void 0 ? _y : 0,
                    ChatDurationHistoryId: room === null || room === void 0 ? void 0 : room.ChatDurationHistoryId,
                    chatPlanId: room === null || room === void 0 ? void 0 : room.chatPlanId,
                    queueInformation: room === null || room === void 0 ? void 0 : room.queueInformation,
                    queueId: room === null || room === void 0 ? void 0 : room.queueId,
                    queueSettings: room === null || room === void 0 ? void 0 : room.queueSettings,
                    widgetName: room === null || room === void 0 ? void 0 : room.widgetName,
                    channelType: 'Chat'
                };
                this.io
                    .of('/')
                    .to([`${room.webChatSessionId}`, `${room.domainId}_${room.ext}`])
                    .emit('webChatConnected', payload);
                db_1.otherRedisClient.publish('webChatConnected_mob_FCM', JSON.stringify(payload));
                console.log('webChatConnected_mob_FCM payload>>', payload);
                const responseData = yield (0, callflow_dao_1.getCustomerChatHistory)(room.ChatHistoryId);
                const datares = (Array === null || Array === void 0 ? void 0 : Array.isArray(responseData)) && JSON.parse((_0 = (_z = responseData[0][0]) === null || _z === void 0 ? void 0 : _z.Message) !== null && _0 !== void 0 ? _0 : null);
                log_1.logger.info(`presence Socket Error: ${JSON.stringify(room)}`);
                // const test = datares.slice(datares?.length - 3, datares?.length);
                // console.log(test,"responseData")
                // const historyMessage = {
                //   type: 'history',
                //   messages: JSON.stringify(test),
                //   sessionId: `${room.domainId}_${room.ext}`,
                //   userId: `${room.webChatSessionId}`,
                //   agentId: `${room.domainId}_${room.ext}`,
                // }
                // this.redisadaptor.publishMessageToAgentAssist(JSON.stringify(historyMessage));
            }));
            socket.on('chat_message_update', (data) => __awaiter(this, void 0, void 0, function* () {
                this.handleChatSessions(data, socket);
                log_1.logger.info(`chat_message_update Socket Error: ${JSON.stringify(data)}`);
            }));
            socket.on('updateDispostionChatStatus', (room) => __awaiter(this, void 0, void 0, function* () {
                var _1, _2, _3, _4, _5;
                const messageList = { message: room };
                try {
                    const msgRes = {
                        ChatHistoryId: room === null || room === void 0 ? void 0 : room.ChatHistoryId,
                        ChatDurationHistoryId: room === null || room === void 0 ? void 0 : room.ChatDurationHistoryId,
                        from: `${room.domainId}_${room.ext}`,
                        message: "Thank you for contacting us. If you have any other queries in future or need assitance, please don't hesitate to reach out.Have a great day!",
                        messageType: 'Text',
                        sessionId: `${room.domainId}_${room.ext}`,
                        to: room === null || room === void 0 ? void 0 : room.sessionId,
                        sender: room === null || room === void 0 ? void 0 : room.sender,
                        sendAt: Math.floor(Date.now())
                    };
                    this.io.of('/').to(`${room === null || room === void 0 ? void 0 : room.domainId}`).emit('chat_ended_for_tracker_emitter', msgRes);
                    const nlpData = {
                        HistoryId: room === null || room === void 0 ? void 0 : room.ChatHistoryId,
                        ChatDurationHistoryId: room === null || room === void 0 ? void 0 : room.ChatDurationHistoryId,
                        channelType: 'Chat',
                        domainId: room === null || room === void 0 ? void 0 : room.domainId,
                        type: 'sentimentAnalyze',
                        ext: (_1 = room === null || room === void 0 ? void 0 : room.ext) !== null && _1 !== void 0 ? _1 : 0
                    };
                    const updateScore = yield (0, webChat_dao_1.sentimentalScore)(nlpData);
                    if ((updateScore === null || updateScore === void 0 ? void 0 : updateScore.status) === 200) {
                        yield (0, webChat_dao_1.updateSentimentScoreChat)(nlpData, updateScore === null || updateScore === void 0 ? void 0 : updateScore.data);
                    }
                    else {
                        console.error('Failed to update sentiment score:', updateScore === null || updateScore === void 0 ? void 0 : updateScore.status);
                    }
                    // if (room?.dispostionStatus !== 'Transferred') {
                    this.io.of('/').to(room === null || room === void 0 ? void 0 : room.sessionId).emit('agentDisconnected', msgRes);
                    // }
                    const getMessage = yield (0, chat_dao_1.getMessageWebChat)(room === null || room === void 0 ? void 0 : room.ChatHistoryId);
                    const parsedMessage = (_4 = JSON.parse((_3 = (_2 = getMessage[0][0]) === null || _2 === void 0 ? void 0 : _2.Message) !== null && _3 !== void 0 ? _3 : '[]')) !== null && _4 !== void 0 ? _4 : [];
                    // if(data?.type === "Chat"){
                    const historyMessage = {
                        type: 'summary',
                        messages: parsedMessage,
                        sessionId: `${room.domainId}_${room.ext}`,
                        ChatDurationHistoryId: room === null || room === void 0 ? void 0 : room.ChatDurationHistoryId,
                        userId: `${room === null || room === void 0 ? void 0 : room.sessionId}`,
                        agentId: `${room === null || room === void 0 ? void 0 : room.domainId}_${room.ext}`,
                        ChatHistoryId: (_5 = room === null || room === void 0 ? void 0 : room.ChatHistoryId) === null || _5 === void 0 ? void 0 : _5.toString(),
                        data: JSON.stringify(room)
                    };
                    this.redisadaptor.publishMessageToAgentAssist(JSON.stringify(historyMessage));
                    const data1 = {
                        ChatHistoryId: room === null || room === void 0 ? void 0 : room.ChatHistoryId,
                        ChatDurationHistoryId: room === null || room === void 0 ? void 0 : room.ChatDurationHistoryId,
                        dispositionId: room === null || room === void 0 ? void 0 : room.dispositionId,
                        EndTime: Math.floor(Date.now()),
                        transferedBy: room === null || room === void 0 ? void 0 : room.transferedBy,
                        transferedTo: room === null || room === void 0 ? void 0 : room.transferedTo,
                        Duration: room === null || room === void 0 ? void 0 : room.Duration
                    };
                    try {
                        yield (0, webChat_dao_1.updateAgentDetails)(data1);
                        log_1.logger.info(`updateDispostionChatStatus Socket Error: ${JSON.stringify(room)}`);
                    }
                    catch (err) {
                        console.log(err);
                    }
                }
                catch (err) {
                    console.log(err);
                }
            }));
            socket.on('store_customer_data', (data) => __awaiter(this, void 0, void 0, function* () {
                var _6;
                yield (0, webChat_dao_1.createChatSession)(data);
                const dispostionRes = yield (0, webChat_dao_1.getDispositinStatus)(data);
                let transferredRes;
                (_6 = dispostionRes[0]) === null || _6 === void 0 ? void 0 : _6.map((list) => {
                    if ((list === null || list === void 0 ? void 0 : list.dispositionName) === 'Transferred') {
                        return transferredRes = list;
                    }
                });
                this.io.of('/').to(data === null || data === void 0 ? void 0 : data.sessionId).emit('get_domain_dispostionId', transferredRes);
            }));
            socket.on('OneToOneChatSend', (msg) => __awaiter(this, void 0, void 0, function* () {
                var _7;
                let messages;
                const messageList = { message: msg };
                if ((msg === null || msg === void 0 ? void 0 : msg.isCustomer) === true) {
                    messages = { userText: messageList };
                }
                else if ((msg === null || msg === void 0 ? void 0 : msg.isagentname) === true) {
                    messages = { agentNameVisible: messageList };
                }
                else if ((msg === null || msg === void 0 ? void 0 : msg.TimeOutStatus) === true) {
                    messages = { timeOutVisible: messageList };
                }
                else if ((msg === null || msg === void 0 ? void 0 : msg.messageType) === 'carousel') {
                    messages = { carouselText: messageList };
                }
                else {
                    messages = { agentText: messageList };
                }
                const stringifiedMessage = { message: messages };
                stringifiedMessage.ChatHistoryId = msg.ChatHistoryId;
                stringifiedMessage.ChatDurationHistoryId = msg === null || msg === void 0 ? void 0 : msg.ChatDurationHistoryId;
                // if(msg?.supervisors == undefined) {
                //   msg.supervisors = ["7179_201"];
                // }
                try {
                    yield (0, webChat_dao_1.updateChatMessage)(stringifiedMessage);
                }
                catch (err) {
                    console.log('updateChatMessageupdateChatMessage', err);
                }
                if (msg.isLiveagent && !msg.agentIntro && (msg === null || msg === void 0 ? void 0 : msg.messageType.toLowerCase()) === 'text') {
                    // let sessionId: any = msg.from;
                    // if (msg.isCustomer) {
                    //   sessionId = msg.to;
                    // }
                    // msg.sessionId = sessionId;
                    msg.ChatHistoryId = (_7 = msg.ChatHistoryId) === null || _7 === void 0 ? void 0 : _7.toString();
                    msg.by = 'agent';
                    if ((msg === null || msg === void 0 ? void 0 : msg.assistType) === 'Convo') {
                        // this.redisadaptor.publishMessageToAgentAssistForConvo(JSON.stringify(msg));
                        this.redisadaptor.agentAssistForConvoXadd(JSON.stringify(msg));
                    }
                    else {
                        this.redisadaptor.agentAssistXadd(JSON.stringify(msg));
                        // this.redisadaptor.publishMessageToAgentAssist(JSON.stringify(msg));
                    }
                }
                msg.sessionId = (msg === null || msg === void 0 ? void 0 : msg.socketId) || (msg === null || msg === void 0 ? void 0 : msg.to);
                this.io.of('/').to(msg === null || msg === void 0 ? void 0 : msg.to).emit('OneToOneChatReceive', msg);
                this.io.of('/').to(msg === null || msg === void 0 ? void 0 : msg.supervisorList).emit('supervisor_trackerview_chat', msg);
                log_1.logger.info(`OneToOneChatSend Socket Error: ${JSON.stringify(msg)}`);
            }));
            socket.on('typingText', (data) => __awaiter(this, void 0, void 0, function* () {
                this.io.of('/').to(`${data.domainId}_${data.ext}`).emit('customerTyping', data);
            }));
            socket.on('OneToOneChatSendTyping', (data) => {
                this.io.of('/').to(data === null || data === void 0 ? void 0 : data.to).emit('typingBotText', data);
            });
            socket.on('agentTypingStatusEmitter', (data) => {
                this.io.of('/').to(data === null || data === void 0 ? void 0 : data.to).emit('agentTypingStatusListener', data);
            });
            socket.on('unResponsiveQuesAns', (data) => __awaiter(this, void 0, void 0, function* () {
                try {
                    yield (0, webChat_dao_1.insertUpdateIrrelevantQaDao)(data);
                }
                catch (err) {
                    console.log(err);
                }
            }));
            socket.on('sendEmailTranscript', (data) => __awaiter(this, void 0, void 0, function* () {
                try {
                    yield (0, webChat_dao_1.sendmailTranscript)(data);
                }
                catch (err) {
                    console.log(err);
                }
            }));
            socket.on('visitorCloseTabSite', (data) => {
                if (data === null || data === void 0 ? void 0 : data.visitorId) {
                    const visitorData = {
                        domainId: parseInt(data === null || data === void 0 ? void 0 : data.domainId, 10),
                        visitorId: parseInt(data === null || data === void 0 ? void 0 : data.visitorId, 10),
                        sessionEndTimed: data === null || data === void 0 ? void 0 : data.sessionEndTime
                    };
                    (0, webChat_dao_1.insertUpdateVisitorDao)(visitorData);
                }
            });
            socket.on('markAsSeen', (data) => __awaiter(this, void 0, void 0, function* () {
                this.io.of('/').to(`${data.domainId}_${data.ext}`).emit('markAsSeen', data);
            }));
            socket.on('getSupervisorExt', (data) => __awaiter(this, void 0, void 0, function* () {
                const superVisorList = yield (0, webChat_dao_1.getSupervisiorList)(data);
                this.io.of('/').to(data === null || data === void 0 ? void 0 : data.to).emit('supervisor_ext_response', superVisorList);
            }));
            socket.on('agentDeclineRequest', (data) => __awaiter(this, void 0, void 0, function* () {
                this.io.of('/').to(data === null || data === void 0 ? void 0 : data.to).emit('requestDecline', data);
                log_1.logger.info(`agentDeclineRequest Socket Error: ${JSON.stringify(data)}`);
            }));
            socket.on('changeAttendChatStatus', (data) => __awaiter(this, void 0, void 0, function* () {
                var _8, _9, _10, _11, _12;
                try {
                    if (data === null || data === void 0 ? void 0 : data.queueId) {
                        const listed = {
                            qid: data === null || data === void 0 ? void 0 : data.queueId,
                            domainId: data === null || data === void 0 ? void 0 : data.domainId
                        };
                        const getQueueList = yield (0, webChat_dao_1.getQueueWaitTime)(listed);
                        const parsedData = typeof ((_8 = getQueueList === null || getQueueList === void 0 ? void 0 : getQueueList[0]) === null || _8 === void 0 ? void 0 : _8.queueList) === 'string'
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
                        const resultData = parsedData === null || parsedData === void 0 ? void 0 : parsedData.map((list) => {
                            if ((list === null || list === void 0 ? void 0 : list.sessionId) === (data === null || data === void 0 ? void 0 : data.sessionId)) {
                                list.chatNotifyStatus = false;
                            }
                            return list;
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
                catch (error) {
                    console.log('changeAttendChatStatus>>>>>', error);
                }
            }));
            socket.on('agentNotAttendEmitter', (data) => __awaiter(this, void 0, void 0, function* () {
                this.io.of('/').to(data === null || data === void 0 ? void 0 : data.to).emit('agentNotAttendEmitter', data);
            }));
            socket.on('update_callback_data', (chatID) => __awaiter(this, void 0, void 0, function* () {
                console.log('chatID', chatID);
                const data = {
                    ChatHistoryId: chatID === null || chatID === void 0 ? void 0 : chatID.ChatHistoryId,
                    CallBackStatus: 1,
                    sessionEndTime: Math.floor(Date.now()),
                    CustomerId: chatID === null || chatID === void 0 ? void 0 : chatID.coid
                };
                console.log('chatID?.coid', chatID === null || chatID === void 0 ? void 0 : chatID.coid);
                if (chatID === null || chatID === void 0 ? void 0 : chatID.coid) {
                    const notification = {
                        uuid: (0, uuid_1.v4)(),
                        toExt: data.ext,
                        notificationMsg: 'Chat Callback received from',
                        channelType: 'Chat',
                        markAsRead: 0,
                        type: 'Chat CALLBACK',
                        timeStamp: new Date().getTime(),
                        coid: chatID === null || chatID === void 0 ? void 0 : chatID.coid,
                        customerValue: null,
                        domainId: chatID === null || chatID === void 0 ? void 0 : chatID.domainId
                    };
                    console.log('CHAT CALLBACK', notification);
                    (0, chat_dao_1.insertNotificationDao)(notification);
                    db_1.ioredisChat.to(`${chatID === null || chatID === void 0 ? void 0 : chatID.domainId}`).emit('refreshNotification');
                }
                const data1 = yield (0, webChat_dao_1.createChatSession)(data);
                this.io.of('/').to(`${chatID === null || chatID === void 0 ? void 0 : chatID.domainId}`).emit('chatCallbackEmitter');
            }));
            socket.on('socialMediaAgentDeclineRequest', (data) => __awaiter(this, void 0, void 0, function* () {
                var _13, _14, _15, _16, _17, _18, _19, _20, _21, _22;
                console.log('socialMediaAgentDeclineRequest', data);
                const getDeclineData = yield (0, socialmedia_1.getDeclineBot)(data);
                const socialMedia = (data === null || data === void 0 ? void 0 : data.isTransferChat) === 1 ? yield (0, chat_dao_1.getUserListByExt)(getDeclineData) : yield (0, whatsappTransfer_1.transferChat)(data, data === null || data === void 0 ? void 0 : data.currentNode);
                if ((socialMedia === null || socialMedia === void 0 ? void 0 : socialMedia.length) > 0) {
                    console.log('sendInvite', `${(_13 = socialMedia[0]) === null || _13 === void 0 ? void 0 : _13.domainId}_${(_14 = socialMedia[0]) === null || _14 === void 0 ? void 0 : _14.ext}`);
                    this.io.of('/').to(`${(_15 = socialMedia[0]) === null || _15 === void 0 ? void 0 : _15.domainId}_${(_16 = socialMedia[0]) === null || _16 === void 0 ? void 0 : _16.ext}`).emit('sendInvite', Object.assign(Object.assign({}, data), socialMedia[0]));
                }
                else {
                    if ((data === null || data === void 0 ? void 0 : data.channelType) === 'Whatsapp') {
                        const getCallbck = yield (0, webChat_dao_1.getBotDetail)(data);
                        if (((_17 = getCallbck[0]) === null || _17 === void 0 ? void 0 : _17.callBackToggle) === 1) {
                            const getCoid = yield (0, webChat_dao_1.getCustomerCoId)(data);
                            const redirectUrlCallbck = `WhatsappWebcallback?coId=${(_18 = getCoid[0]) === null || _18 === void 0 ? void 0 : _18.coid}&domainId=${data === null || data === void 0 ? void 0 : data.domainId}&phoneNumber=${data === null || data === void 0 ? void 0 : data.customerNumber}&historyId=${data === null || data === void 0 ? void 0 : data.historyId}`;
                            yield (0, fetch_api_1.sendCallbackRequest)(data, redirectUrlCallbck);
                            yield (0, webChat_dao_1.updateCallbackStatus)(data);
                            let message = {};
                            try {
                                message = {
                                    messageID: (0, uuid_1.v4)(),
                                    sessionId: data === null || data === void 0 ? void 0 : data.sessionId,
                                    message: 'Our agents are currently not available. Please feel free to leave a message with a brief summary of your inquiry along with your contact information, or you can get in touch with us at a later time.',
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
                                stringifiedMessage.historyId = data === null || data === void 0 ? void 0 : data.historyId;
                                yield (0, webChat_dao_1.updateWhatsappChatMessage)(stringifiedMessage);
                                (0, webChat_dao_1.updateIsBot)(data === null || data === void 0 ? void 0 : data.historyId);
                            }
                            catch (error) {
                                console.log(error);
                            }
                        }
                        else {
                            const getMessage = yield (0, fetch_api_1.sendMessage)(data);
                            let message = {};
                            try {
                                message = {
                                    messageID: (0, uuid_1.v4)(),
                                    sessionId: data === null || data === void 0 ? void 0 : data.sessionId,
                                    message: getMessage === null || getMessage === void 0 ? void 0 : getMessage.content,
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
                                stringifiedMessage.historyId = data === null || data === void 0 ? void 0 : data.historyId;
                                yield (0, webChat_dao_1.updateWhatsappChatMessage)(stringifiedMessage);
                                (0, webChat_dao_1.updateIsBot)(data === null || data === void 0 ? void 0 : data.historyId);
                            }
                            catch (error) {
                                console.log(error);
                            }
                            yield (0, webChat_dao_1.updateMissedChat)(data);
                            const sentimentSupervisorBot = yield (0, webChat_dao_1.getSupervisiorList)(data);
                            sentimentSupervisorBot.forEach((ext) => {
                                console.log('sentimentWhatsappBot', `${data === null || data === void 0 ? void 0 : data.domainId}_${ext === null || ext === void 0 ? void 0 : ext.ext}`);
                                this.io.of('/').to(`${data === null || data === void 0 ? void 0 : data.domainId}_${ext === null || ext === void 0 ? void 0 : ext.ext}`).emit('supervisiorActivityBot');
                            });
                        }
                    }
                    else if ((data === null || data === void 0 ? void 0 : data.channelType) === 'Facebook') {
                        const getCallbckFb = yield (0, webChat_dao_1.getFacebookConfigData)(data);
                        if (((_19 = getCallbckFb[0]) === null || _19 === void 0 ? void 0 : _19.callBackToggle) === 1) {
                            const getCoid = yield (0, webChat_dao_1.getCustomerCoIdFb)(data);
                            const redirectUrlCallbck = `FacebookWebcallback?coId=${(_20 = getCoid[0]) === null || _20 === void 0 ? void 0 : _20.coid}&domainId=${data === null || data === void 0 ? void 0 : data.domainId}&facebookId=${data === null || data === void 0 ? void 0 : data.customerNumber}&historyId=${data === null || data === void 0 ? void 0 : data.historyId}`;
                            yield (0, webChat_dao_1.sendCallbackRequestFb)(data, redirectUrlCallbck);
                            yield (0, webChat_dao_1.updateCallbackStatusFb)(data);
                            let message = {};
                            try {
                                message = {
                                    messageID: (0, uuid_1.v4)(),
                                    sessionId: data === null || data === void 0 ? void 0 : data.sessionId,
                                    message: 'Our agents are currently not available. Please feel free to leave a message with a brief summary of your inquiry along with your contact information, or you can get in touch with us at a later time.',
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
                                stringifiedMessage.historyId = data === null || data === void 0 ? void 0 : data.historyId;
                                yield (0, webChat_dao_1.updateChatMessageFb)(stringifiedMessage);
                                (0, webChat_dao_1.updateIsBotFb)(data === null || data === void 0 ? void 0 : data.historyId);
                            }
                            catch (error) {
                                console.log(error);
                            }
                        }
                        else {
                            yield (0, webChat_dao_1.sendMessageFb)(data);
                            let message = {};
                            try {
                                message = {
                                    messageID: (0, uuid_1.v4)(),
                                    sessionId: data === null || data === void 0 ? void 0 : data.sessionId,
                                    message: 'Our agents are currently not available. Please feel free to leave a message with a brief summary of your inquiry along with your contact information, or you can get in touch with us at a later time.',
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
                                stringifiedMessage.historyId = data === null || data === void 0 ? void 0 : data.historyId;
                                yield (0, webChat_dao_1.updateChatMessageFb)(stringifiedMessage);
                                (0, webChat_dao_1.updateIsBotFb)(data === null || data === void 0 ? void 0 : data.historyId);
                            }
                            catch (error) {
                                console.log(error);
                            }
                            yield (0, webChat_dao_1.updateMissedChatFb)(data);
                            const sentimentSupervisorBot = yield (0, webChat_dao_1.getSupervisiorList)(data);
                            sentimentSupervisorBot.forEach((ext) => {
                                console.log('sentimentWhatsappBot', `${data === null || data === void 0 ? void 0 : data.domainId}_${ext === null || ext === void 0 ? void 0 : ext.ext}`);
                                this.io.of('/').to(`${data === null || data === void 0 ? void 0 : data.domainId}_${ext === null || ext === void 0 ? void 0 : ext.ext}`).emit('supervisiorActivityBot');
                            });
                        }
                    }
                    else {
                        const getCallbckInsta = yield (0, socialmedia_1.getInstagramConfigData)(data);
                        const getCoid = yield (0, socialmedia_1.getCustomerCoIdInsta)(data);
                        if (((_21 = getCallbckInsta[0]) === null || _21 === void 0 ? void 0 : _21.callBackToggle) === 1) {
                            const redirectUrlCallbck = `InstagramWebcallback?coId=${(_22 = getCoid[0]) === null || _22 === void 0 ? void 0 : _22.coid}&domainId=${data === null || data === void 0 ? void 0 : data.domainId}&instagramId=${data === null || data === void 0 ? void 0 : data.customerNumber}&historyId=${data === null || data === void 0 ? void 0 : data.historyId}`;
                            yield (0, socialmedia_1.sendCallbackRequestInsta)(data, redirectUrlCallbck);
                            yield (0, socialmedia_1.updateCallbackStatusInsta)(data);
                            let message = {};
                            try {
                                message = {
                                    messageID: (0, uuid_1.v4)(),
                                    sessionId: data === null || data === void 0 ? void 0 : data.sessionId,
                                    message: 'Our agents are currently not available. Please feel free to leave a message with a brief summary of your inquiry along with your contact information, or you can get in touch with us at a later time.',
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
                                stringifiedMessage.historyId = data === null || data === void 0 ? void 0 : data.historyId;
                                yield (0, socialmedia_1.updateChatMessageInsta)(stringifiedMessage);
                                (0, socialmedia_1.updateIsBotInsta)(data === null || data === void 0 ? void 0 : data.historyId);
                            }
                            catch (error) {
                                console.log(error);
                            }
                        }
                        else {
                            yield (0, socialmedia_1.sendMessageInstagram)(data);
                            let message = {};
                            try {
                                message = {
                                    messageID: (0, uuid_1.v4)(),
                                    sessionId: data === null || data === void 0 ? void 0 : data.sessionId,
                                    message: 'Our agents are currently not available. Please feel free to leave a message with a brief summary of your inquiry along with your contact information, or you can get in touch with us at a later time.',
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
                                stringifiedMessage.historyId = data === null || data === void 0 ? void 0 : data.historyId;
                                yield (0, socialmedia_1.updateChatMessageInsta)(stringifiedMessage);
                                (0, socialmedia_1.updateIsBotInsta)(data === null || data === void 0 ? void 0 : data.historyId);
                            }
                            catch (error) {
                                console.log(error);
                            }
                            yield (0, socialmedia_1.updateMissedChatInsta)(data);
                            const sentimentSupervisorBot = yield (0, webChat_dao_1.getSupervisiorList)(data);
                            sentimentSupervisorBot.forEach((ext) => {
                                console.log('sentimentWhatsappBot', `${data === null || data === void 0 ? void 0 : data.domainId}_${ext === null || ext === void 0 ? void 0 : ext.ext}`);
                                this.io.of('/').to(`${data === null || data === void 0 ? void 0 : data.domainId}_${ext === null || ext === void 0 ? void 0 : ext.ext}`).emit('supervisiorActivityBot');
                            });
                        }
                    }
                }
                log_1.logger.info(`socialMediaAgentDeclineRequest Socket Error: ${JSON.stringify(data)}`);
            }));
            socket.on('agentAcceptRequest', (room) => __awaiter(this, void 0, void 0, function* () {
                var _23;
                console.log('agentAcceptRequest >>>>>>>', room);
                try {
                    this.io.of('/').to(room === null || room === void 0 ? void 0 : room.to).emit('requestAccept', room);
                    if ((room === null || room === void 0 ? void 0 : room.transferType) === 1) {
                        const transferData = {
                            ChatHistoryId: (_23 = room === null || room === void 0 ? void 0 : room.ChatHistoryId) !== null && _23 !== void 0 ? _23 : 0,
                            Istransferred: room === null || room === void 0 ? void 0 : room.Istransferred
                        };
                        // await createChatSession(transferData);
                        this.io.of('/').to(room === null || room === void 0 ? void 0 : room.primaryAgent).emit('transferChatAccepted', room);
                        log_1.logger.info(`agentAcceptRequest :: ${JSON.stringify(room)}`);
                    }
                }
                catch (err) {
                    console.log(err);
                }
            }));
            socket.on('deleteChatSession', (data) => __awaiter(this, void 0, void 0, function* () {
                console.log('deleteChatSession data', data);
                (0, chatflow_helpers_1.deleteSessionDetails)(data);
                // const getIncomingChatDetails: any = await getIncomingChatDetailsDB(data, data?.queueId);
                // if (getIncomingChatDetails.length > 0) {
                //   console.log('deleteChatSession getIncomingChatDetails', getIncomingChatDetails);
                //   const getIncomingChats: any = JSON.parse(getIncomingChatDetails[0]?.queueList ?? '[]');
                //   console.log('deleteChatSession getIncomingChats', getIncomingChats);
                //   const incomingChat = getIncomingChats?.filter((val: any) => {
                //     return val?.session_id !== data?.sessionId;
                //   });
                //   console.log('deleteChatSession incomingChat', incomingChat);
                //   data.queueList = incomingChat;
                //   insertIncomingChatsDB(data, data?.queueId);
                // } else {
                //   const getIncomingChatDetailsBasedOnAgentId: any = await getIncomingChatDetailsDB(data, data?.agentId);
                //   if (getIncomingChatDetailsBasedOnAgentId.length > 0) {
                //     console.log('deleteChatSession getIncomingChatDetailsBasedOnAgentId', getIncomingChatDetailsBasedOnAgentId);
                //     const getIncomingChatsBasedOnAgentId: any = JSON.parse(getIncomingChatDetailsBasedOnAgentId[0]?.queueList ?? '[]');
                //     console.log('deleteChatSession getIncomingChatsBasedOnAgentId', getIncomingChatsBasedOnAgentId);
                //     const incomingChat = getIncomingChatsBasedOnAgentId?.filter((val: any) => {
                //       return val?.session_id !== data?.sessionId;
                //     });
                //     console.log('deleteChatSession incomingChat', incomingChat);
                //     data.queueList = incomingChat;
                //     insertIncomingChatsDB(data, data?.agentId);
                //   }
                // }
            }));
            socket.on('socialMediaAgentAcceptRequest', (room) => __awaiter(this, void 0, void 0, function* () {
                var _24, _25, _26, _27, _28, _29;
                console.log('socialMediaAgentAcceptRequest >>>>>>>', room);
                if ((room === null || room === void 0 ? void 0 : room.channelType) === 'Whatsapp') {
                    const data = {
                        sessionId: room === null || room === void 0 ? void 0 : room.sessionId,
                        WhatsappHistoryId: (_24 = room === null || room === void 0 ? void 0 : room.historyId) !== null && _24 !== void 0 ? _24 : 0,
                        AgentId: room === null || room === void 0 ? void 0 : room.agentId,
                        IsBot: 0,
                        agentExt: room === null || room === void 0 ? void 0 : room.agentExt,
                        dialDuration: room === null || room === void 0 ? void 0 : room.dialDuration,
                        IsMissed: (_25 = room === null || room === void 0 ? void 0 : room.IsMissed) !== null && _25 !== void 0 ? _25 : 0,
                    };
                    try {
                        // await sendConnectLiveAgentNotification(room);
                        // sendLiveNotification(room);
                        yield (0, fetch_api_1.sendConnectLiveAgent)(room);
                        let message = {};
                        try {
                            message = {
                                messageID: (0, uuid_1.v4)(),
                                sessionId: room === null || room === void 0 ? void 0 : room.sessionId,
                                message: `Your conversation has been transferred to ${room === null || room === void 0 ? void 0 : room.agentName}`,
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
                        }
                        yield (0, webChat_dao_1.updateAgentDetailsdata)(data);
                    }
                    catch (err) {
                        console.log(err);
                    }
                }
                else if ((room === null || room === void 0 ? void 0 : room.channelType) === 'Facebook') {
                    const data = {
                        sessionId: room === null || room === void 0 ? void 0 : room.sessionId,
                        FacebookHistoryId: (_26 = room === null || room === void 0 ? void 0 : room.historyId) !== null && _26 !== void 0 ? _26 : 0,
                        AgentId: room === null || room === void 0 ? void 0 : room.agentId,
                        IsBot: 0,
                        agentExt: room === null || room === void 0 ? void 0 : room.agentExt,
                        domainId: room === null || room === void 0 ? void 0 : room.domainId,
                        dialDuration: (_27 = room === null || room === void 0 ? void 0 : room.dialDuration) !== null && _27 !== void 0 ? _27 : null
                    };
                    try {
                        // await sendConnectLiveAgentFBNotification(room);
                        // sendLiveNotificationFb(room);
                        yield (0, fetch_api_1.sendConnectLiveAgentFB)(room);
                        let message = {};
                        try {
                            message = {
                                messageID: (0, uuid_1.v4)(),
                                sessionId: room === null || room === void 0 ? void 0 : room.sessionId,
                                message: `Your conversation has been transferred to ${room === null || room === void 0 ? void 0 : room.agentName}`,
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
                            yield (0, webChat_dao_1.updateChatMessageFb)(stringifiedMessage);
                        }
                        catch (error) {
                            console.log(error);
                        }
                        yield (0, webChat_dao_1.updateAgentDetailsFB)(data);
                    }
                    catch (err) {
                        console.log(err);
                    }
                }
                else {
                    const data = {
                        sessionId: room === null || room === void 0 ? void 0 : room.sessionId,
                        InstagramHistoryId: (_28 = room === null || room === void 0 ? void 0 : room.historyId) !== null && _28 !== void 0 ? _28 : 0,
                        AgentId: room === null || room === void 0 ? void 0 : room.agentId,
                        IsBot: 0,
                        agentExt: room === null || room === void 0 ? void 0 : room.agentExt,
                        domainId: room === null || room === void 0 ? void 0 : room.domainId,
                        dialDuration: (_29 = room === null || room === void 0 ? void 0 : room.dialDuration) !== null && _29 !== void 0 ? _29 : null
                    };
                    try {
                        // await sendConnectLiveAgentInstaNotification(room);
                        // sendLiveNotificationInsta(room);
                        yield (0, fetch_api_1.sendConnectLiveAgentInsta)(room);
                        let message = {};
                        try {
                            message = {
                                messageID: (0, uuid_1.v4)(),
                                sessionId: room === null || room === void 0 ? void 0 : room.sessionId,
                                message: `Your conversation has been transferred to ${room === null || room === void 0 ? void 0 : room.agentName}`,
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
                            yield (0, socialmedia_1.updateChatMessageInsta)(stringifiedMessage);
                        }
                        catch (error) {
                            console.log(error);
                        }
                        yield (0, socialmedia_1.updateAgentDetailsInsta)(data);
                    }
                    catch (err) {
                        console.log(err);
                    }
                }
                log_1.logger.info(`socialMediaAgentAcceptRequest Socket Error: ${JSON.stringify(room)}`);
            }));
            socket.on('socialMediaAgentAssistDetails', (data) => __awaiter(this, void 0, void 0, function* () {
                try {
                    let nlpRes;
                    const comDetails = yield (0, socialmedia_1.getCompanyDetails)(data);
                    nlpRes = yield (0, socialmedia_1.AgentAssistSocialMedia)(data, comDetails);
                    if (!(nlpRes === null || nlpRes === void 0 ? void 0 : nlpRes.data)) {
                        console.error('AgentAssistSocialMedia returned invalid response:', nlpRes);
                        return;
                    }
                    nlpRes.data.channelId = data === null || data === void 0 ? void 0 : data.agentNumber;
                    nlpRes.data.messageId = (0, uuid_1.v4)();
                    nlpRes.data.timeStamp = new Date().getTime();
                    console.log('agent assist emit>>>>>>', `${data === null || data === void 0 ? void 0 : data.domainId}_${data === null || data === void 0 ? void 0 : data.agentExt}`);
                    this.io.of('/').to(`${data === null || data === void 0 ? void 0 : data.domainId}_${data === null || data === void 0 ? void 0 : data.agentExt}`).emit('agentAssistOmniChannel', nlpRes.data);
                }
                catch (error) {
                    console.error('Error in socialMediaAgentAssistDetails handler:', error);
                }
            }));
            socket.on('emailSmsAgentAssistDetails', (data) => __awaiter(this, void 0, void 0, function* () {
                var _30, _31;
                try {
                    let botDetails;
                    let websiteId;
                    let nlpRes;
                    if ((data === null || data === void 0 ? void 0 : data.channelType) === 'Email') {
                        botDetails = yield (0, socialmedia_1.configurationEmailDetails)(data, 1);
                        websiteId = yield (0, socialmedia_1.getKnowledgeBaseId)(data.domainId, (_30 = botDetails[0]) === null || _30 === void 0 ? void 0 : _30.knowledgeBasedId);
                    }
                    else {
                        botDetails = yield (0, socialmedia_1.configurationEmailDetails)(data, 2);
                        websiteId = yield (0, socialmedia_1.getKnowledgeBaseId)(data.domainId, (_31 = botDetails[0]) === null || _31 === void 0 ? void 0 : _31.knowledgeBasedId);
                    }
                    data.messageId = (0, uuid_1.v4)();
                    nlpRes = yield (0, socialmedia_1.AgentAssistEmailSms)(data, websiteId[0]);
                    if (!(nlpRes === null || nlpRes === void 0 ? void 0 : nlpRes.data)) {
                        console.error('AgentAssistSocialMedia returned invalid response:', nlpRes);
                        return;
                    }
                    nlpRes.data.messageId = nlpRes.data.messageId || data.messageId;
                    nlpRes.data.channelId = data === null || data === void 0 ? void 0 : data.agentNumber;
                    nlpRes.data.timeStamp = new Date().getTime();
                    console.log('agent assist emit>>>>>>', `${data === null || data === void 0 ? void 0 : data.domainId}_${data === null || data === void 0 ? void 0 : data.agentExt}`);
                    this.io.of('/').to(`${data === null || data === void 0 ? void 0 : data.domainId}_${data === null || data === void 0 ? void 0 : data.agentExt}`).emit('agentAssistOmniChannel', nlpRes.data);
                }
                catch (error) {
                    console.error('Error in emailSmsAgentAssistDetails handler:', error);
                }
            }));
            socket.on('agentDisconnectedTime', (room) => __awaiter(this, void 0, void 0, function* () {
                var _32, _33;
                const data = {
                    ChatHistoryId: (_32 = room === null || room === void 0 ? void 0 : room.ChatHistoryId) !== null && _32 !== void 0 ? _32 : 0,
                    ChatDurationHistoryId: (_33 = room === null || room === void 0 ? void 0 : room.ChatDurationHistoryId) !== null && _33 !== void 0 ? _33 : 0,
                    EndTime: Math.floor(Date.now()),
                };
                try {
                    yield (0, webChat_dao_1.updateAgentDetails)(data);
                    log_1.logger.info(`agentDisconnectedTime Socket Error: ${JSON.stringify(room)}`);
                }
                catch (err) {
                    console.log(err);
                }
            }));
            socket.on('requestAgentStatus', (msg) => __awaiter(this, void 0, void 0, function* () {
                try {
                    const getDomainResult = yield (0, callflow_dao_1.getDomainStatus)(msg === null || msg === void 0 ? void 0 : msg.domain_id);
                    this.io.of('/').to(msg === null || msg === void 0 ? void 0 : msg.session_id).emit('responseAgentStatus', { domainStatus: getDomainResult, result: msg });
                }
                catch (err) {
                    console.log(err);
                }
            }));
            socket.on('closeRingingChat', (data) => __awaiter(this, void 0, void 0, function* () {
                try {
                    this.io.of('/').to(`${data === null || data === void 0 ? void 0 : data.domainId}`).emit('ringingChatCloseEmitter', data);
                }
                catch (err) {
                    console.log(err);
                }
            }));
            socket.on('sessionTimeOut', (data) => __awaiter(this, void 0, void 0, function* () {
                var _34, _35, _36, _37;
                let messages;
                const messageList = { message: data };
                messages = { timeOutVisible: messageList };
                const getMessage = yield (0, chat_dao_1.getMessageWebChat)(data === null || data === void 0 ? void 0 : data.sessionId);
                let newMessage;
                if (!((_34 = getMessage[0]) === null || _34 === void 0 ? void 0 : _34.message)) {
                    newMessage = [messages];
                }
                else {
                    const parsedMessage = (_37 = JSON.parse((_36 = (_35 = getMessage[0]) === null || _35 === void 0 ? void 0 : _35.message) !== null && _36 !== void 0 ? _36 : '[]')) !== null && _37 !== void 0 ? _37 : [];
                    if ((parsedMessage === null || parsedMessage === void 0 ? void 0 : parsedMessage.length) > 0) {
                        parsedMessage.push(messages);
                        newMessage = parsedMessage;
                    }
                }
                const stringifiedMessage = { message: newMessage, chatSession: 1, sessionEndTime: Date.now(), agentDisconnectTime: Date.now() };
                // createSession(stringifiedMessage, data?.sessionId);
            }));
            socket.on('updateThumbsUp', (data) => __awaiter(this, void 0, void 0, function* () {
                var _38, _39;
                const responseData = yield (0, callflow_dao_1.getCustomerChatHistory)(data === null || data === void 0 ? void 0 : data.ChatDurationHistoryId);
                const messageArr = [];
                const datares = (Array === null || Array === void 0 ? void 0 : Array.isArray(responseData)) && JSON.parse((_39 = (_38 = responseData[0][0]) === null || _38 === void 0 ? void 0 : _38.Message) !== null && _39 !== void 0 ? _39 : null);
                (datares === null || datares === void 0 ? void 0 : datares.length) > 0 &&
                    (datares === null || datares === void 0 ? void 0 : datares.map((msg) => {
                        var _a, _b;
                        if ((data === null || data === void 0 ? void 0 : data.messageID) === ((_b = (_a = msg === null || msg === void 0 ? void 0 : msg.agentText) === null || _a === void 0 ? void 0 : _a.message) === null || _b === void 0 ? void 0 : _b.messageID)) {
                            msg.agentText.message.isLike = data === null || data === void 0 ? void 0 : data.isLike;
                            msg.agentText.message.feedBack = data === null || data === void 0 ? void 0 : data.feedBack;
                        }
                        messageArr.push(msg);
                    }));
                try {
                    yield (0, callflow_dao_1.updateMessageDao)(data, messageArr);
                }
                catch (err) {
                    console.log(err);
                }
            }));
            socket.on('refershLiveTracker', () => __awaiter(this, void 0, void 0, function* () {
                this.io.emit('getLiveTrackStatus');
            }));
            socket.on('chatAcceptedDeviceType', (data) => {
                this.io.of('/').to(data === null || data === void 0 ? void 0 : data.to).emit('chatAcceptedDeviceTypeListener', data);
                log_1.logger.info(`chatAcceptedDeviceType Socket Error: ${JSON.stringify(data)}`);
            });
            socket.on('dequeueUser', (data) => __awaiter(this, void 0, void 0, function* () {
                var _40, _41, _42, _43, _44, _45;
                if (data === null || data === void 0 ? void 0 : data.qid) {
                    const getQueueData = yield (0, webChat_dao_1.getQueueWaitTime)(data);
                    const queueResult = ((_40 = getQueueData === null || getQueueData === void 0 ? void 0 : getQueueData[0]) === null || _40 === void 0 ? void 0 : _40.queueList) && JSON.parse((_41 = getQueueData === null || getQueueData === void 0 ? void 0 : getQueueData[0]) === null || _41 === void 0 ? void 0 : _41.queueList);
                    const insertData = queueResult === null || queueResult === void 0 ? void 0 : queueResult.filter((list) => (list === null || list === void 0 ? void 0 : list.sessionId) !== (data === null || data === void 0 ? void 0 : data.sessionId));
                    const matchData = queueResult === null || queueResult === void 0 ? void 0 : queueResult.filter((list) => (list === null || list === void 0 ? void 0 : list.sessionId) === (data === null || data === void 0 ? void 0 : data.sessionId));
                    const payload = {
                        qid: (_43 = (_42 = getQueueData === null || getQueueData === void 0 ? void 0 : getQueueData[0]) === null || _42 === void 0 ? void 0 : _42.qid) !== null && _43 !== void 0 ? _43 : 0,
                        queueName: (_44 = getQueueData === null || getQueueData === void 0 ? void 0 : getQueueData[0]) === null || _44 === void 0 ? void 0 : _44.queueName,
                        queueType: 'chat',
                        domainId: (_45 = getQueueData === null || getQueueData === void 0 ? void 0 : getQueueData[0]) === null || _45 === void 0 ? void 0 : _45.domainId,
                        queueList: JSON.stringify(insertData)
                    };
                    try {
                        yield (0, webChat_dao_1.insertUpdateQueueWaitTimeDao)(payload);
                        (0, constants_2.insertQueuedTimeForChat)((matchData === null || matchData === void 0 ? void 0 : matchData.length) > 0 ? matchData : {});
                    }
                    catch (err) {
                        console.log(err);
                    }
                    this.io.emit('callQueueWait', payload);
                }
                log_1.logger.info(`dequeueUser Socket Error: ${JSON.stringify(data)}`);
            }));
            socket.on('chatDeclinedDeviceType', (data) => {
                this.io.of('/').to(data === null || data === void 0 ? void 0 : data.to).emit('chatDeclinedDeviceTypeListener', data);
                log_1.logger.info(`chatDeclinedDeviceType Socket Error: ${JSON.stringify(data)}`);
            });
            socket.on('removeChatQueueUser', (data) => __awaiter(this, void 0, void 0, function* () {
                var _46, _47, _48, _49, _50, _51;
                try {
                    const list = {
                        qid: data === null || data === void 0 ? void 0 : data.removeQueueId,
                        domainId: data === null || data === void 0 ? void 0 : data.domainId
                    };
                    const getQueueList = yield (0, webChat_dao_1.getQueueWaitTime)(list);
                    const queueResult = ((_46 = getQueueList === null || getQueueList === void 0 ? void 0 : getQueueList[0]) === null || _46 === void 0 ? void 0 : _46.queueList) ? JSON.parse((_47 = getQueueList === null || getQueueList === void 0 ? void 0 : getQueueList[0]) === null || _47 === void 0 ? void 0 : _47.queueList) : [];
                    const queueListChanges = queueResult === null || queueResult === void 0 ? void 0 : queueResult.filter((_l) => (_l === null || _l === void 0 ? void 0 : _l.sessionId) !== (data === null || data === void 0 ? void 0 : data.sessionId));
                    const matchData = queueResult === null || queueResult === void 0 ? void 0 : queueResult.filter((_l) => (_l === null || _l === void 0 ? void 0 : _l.sessionId) === (data === null || data === void 0 ? void 0 : data.sessionId));
                    const payload = {
                        qid: (_49 = (_48 = getQueueList === null || getQueueList === void 0 ? void 0 : getQueueList[0]) === null || _48 === void 0 ? void 0 : _48.qid) !== null && _49 !== void 0 ? _49 : 0,
                        queueName: (_50 = getQueueList === null || getQueueList === void 0 ? void 0 : getQueueList[0]) === null || _50 === void 0 ? void 0 : _50.queueName,
                        queueType: 'chat',
                        domainId: (_51 = getQueueList === null || getQueueList === void 0 ? void 0 : getQueueList[0]) === null || _51 === void 0 ? void 0 : _51.domainId,
                        queueList: JSON.stringify(queueListChanges)
                    };
                    yield (0, webChat_dao_1.insertUpdateQueueWaitTimeDao)(payload);
                    this.io.emit('callQueueWait', payload);
                    (0, constants_2.insertQueuedTimeForChat)((matchData === null || matchData === void 0 ? void 0 : matchData.length) > 0 ? matchData : {});
                    log_1.logger.info(`removeChatQueueUser Socket Error: ${JSON.stringify(data)}`);
                }
                catch (err) {
                    console.log(err);
                }
            }));
            socket.on('removeChatQueueUserSocialmedia', (data) => __awaiter(this, void 0, void 0, function* () {
                var _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, _65, _66, _67, _68, _69;
                try {
                    if ((data === null || data === void 0 ? void 0 : data.channelType) === 'Whatsapp') {
                        const list = {
                            qid: data === null || data === void 0 ? void 0 : data.removeQueueId,
                            domainId: data === null || data === void 0 ? void 0 : data.domainId
                        };
                        const getQueueList = yield (0, webChat_dao_1.getQueueWaitTimeWhatsapp)(list);
                        const queueResult = ((_52 = getQueueList === null || getQueueList === void 0 ? void 0 : getQueueList[0]) === null || _52 === void 0 ? void 0 : _52.queueList) ? JSON.parse((_53 = getQueueList === null || getQueueList === void 0 ? void 0 : getQueueList[0]) === null || _53 === void 0 ? void 0 : _53.queueList) : [];
                        const queueListChanges = queueResult === null || queueResult === void 0 ? void 0 : queueResult.filter((_l) => (_l === null || _l === void 0 ? void 0 : _l.sessionId) !== (data === null || data === void 0 ? void 0 : data.sessionId));
                        const payload = {
                            qid: (_55 = (_54 = getQueueList === null || getQueueList === void 0 ? void 0 : getQueueList[0]) === null || _54 === void 0 ? void 0 : _54.qid) !== null && _55 !== void 0 ? _55 : 0,
                            queueName: (_56 = getQueueList === null || getQueueList === void 0 ? void 0 : getQueueList[0]) === null || _56 === void 0 ? void 0 : _56.queueName,
                            queueType: 'whatsapp',
                            domainId: (_57 = getQueueList === null || getQueueList === void 0 ? void 0 : getQueueList[0]) === null || _57 === void 0 ? void 0 : _57.domainId,
                            queueList: JSON.stringify(queueListChanges)
                        };
                        yield (0, webChat_dao_1.insertUpdateQueueWaitTimeWhatsapp)(payload);
                        this.io.emit('callQueueWait', payload);
                    }
                    else if ((data === null || data === void 0 ? void 0 : data.channelType) === 'Facebook') {
                        const list = {
                            qid: data === null || data === void 0 ? void 0 : data.removeQueueId,
                            domainId: data === null || data === void 0 ? void 0 : data.domainId
                        };
                        const getQueueList = yield (0, webChat_dao_1.getQueueWaitTimeFacebook)(list);
                        const queueResult = ((_58 = getQueueList === null || getQueueList === void 0 ? void 0 : getQueueList[0]) === null || _58 === void 0 ? void 0 : _58.queueList) ? JSON.parse((_59 = getQueueList === null || getQueueList === void 0 ? void 0 : getQueueList[0]) === null || _59 === void 0 ? void 0 : _59.queueList) : [];
                        const queueListChanges = queueResult === null || queueResult === void 0 ? void 0 : queueResult.filter((_l) => (_l === null || _l === void 0 ? void 0 : _l.sessionId) !== (data === null || data === void 0 ? void 0 : data.sessionId));
                        const payload = {
                            qid: (_61 = (_60 = getQueueList === null || getQueueList === void 0 ? void 0 : getQueueList[0]) === null || _60 === void 0 ? void 0 : _60.qid) !== null && _61 !== void 0 ? _61 : 0,
                            queueName: (_62 = getQueueList === null || getQueueList === void 0 ? void 0 : getQueueList[0]) === null || _62 === void 0 ? void 0 : _62.queueName,
                            queueType: 'facebook',
                            domainId: (_63 = getQueueList === null || getQueueList === void 0 ? void 0 : getQueueList[0]) === null || _63 === void 0 ? void 0 : _63.domainId,
                            queueList: JSON.stringify(queueListChanges)
                        };
                        yield (0, webChat_dao_1.insertUpdateQueueWaitTimeWhatsapp)(payload);
                        this.io.emit('callQueueWait', payload);
                    }
                    else {
                        const list = {
                            qid: data === null || data === void 0 ? void 0 : data.removeQueueId,
                            domainId: data === null || data === void 0 ? void 0 : data.domainId
                        };
                        const getQueueList = yield (0, webChat_dao_1.getQueueWaitTimeInsta)(list);
                        const queueResult = ((_64 = getQueueList === null || getQueueList === void 0 ? void 0 : getQueueList[0]) === null || _64 === void 0 ? void 0 : _64.queueList) ? JSON.parse((_65 = getQueueList === null || getQueueList === void 0 ? void 0 : getQueueList[0]) === null || _65 === void 0 ? void 0 : _65.queueList) : [];
                        const queueListChanges = queueResult === null || queueResult === void 0 ? void 0 : queueResult.filter((_l) => (_l === null || _l === void 0 ? void 0 : _l.sessionId) !== (data === null || data === void 0 ? void 0 : data.sessionId));
                        const payload = {
                            qid: (_67 = (_66 = getQueueList === null || getQueueList === void 0 ? void 0 : getQueueList[0]) === null || _66 === void 0 ? void 0 : _66.qid) !== null && _67 !== void 0 ? _67 : 0,
                            queueName: (_68 = getQueueList === null || getQueueList === void 0 ? void 0 : getQueueList[0]) === null || _68 === void 0 ? void 0 : _68.queueName,
                            queueType: 'instagram',
                            domainId: (_69 = getQueueList === null || getQueueList === void 0 ? void 0 : getQueueList[0]) === null || _69 === void 0 ? void 0 : _69.domainId,
                            queueList: JSON.stringify(queueListChanges)
                        };
                        yield (0, webChat_dao_1.insertUpdateQueueWaitTimeWhatsapp)(payload);
                        this.io.emit('callQueueWait', payload);
                    }
                    log_1.logger.info(`removeChatQueueUserSocialmedia Socket Error: ${JSON.stringify(data)}`);
                }
                catch (err) {
                    console.log(err);
                }
            }));
            socket.on('socialMediaDeclinedDeviceType', (data) => {
                this.io.of('/').to(data === null || data === void 0 ? void 0 : data.to).emit('socialMediaDeclinedDeviceTypeListener', data);
                log_1.logger.info(`socialMediaDeclinedDeviceType Socket Error: ${JSON.stringify(data)}`);
            });
            socket.on('queue_list_update', (data) => {
                this.io.emit('queue_name_update', data);
            });
            socket.on('update_end_session', (data) => __awaiter(this, void 0, void 0, function* () {
                var _70, _71, _72;
                data.EndTime = Math.floor(Date.now());
                data.sessionEndTime = Math.floor(Date.now());
                console.log('update_end_session_payloadd>>>', data);
                try {
                    if ((data === null || data === void 0 ? void 0 : data.isBot) === true) {
                        const dispostionRes = yield (0, webChat_dao_1.getDispositinStatus)(data);
                        let transferredRes;
                        (_70 = dispostionRes[0]) === null || _70 === void 0 ? void 0 : _70.map((list) => {
                            if ((list === null || list === void 0 ? void 0 : list.dispositionName) === 'Closed') {
                                return transferredRes = list;
                            }
                        });
                        data.dispositionId = transferredRes === null || transferredRes === void 0 ? void 0 : transferredRes.did;
                        (0, webChat_dao_1.createChatSession)(data);
                        (0, webChat_dao_1.updateAgentDetails)(data);
                        const nlpData = {
                            HistoryId: data === null || data === void 0 ? void 0 : data.ChatHistoryId,
                            ChatDurationHistoryId: data === null || data === void 0 ? void 0 : data.ChatDurationHistoryId,
                            channelType: 'Chat',
                            domainId: data === null || data === void 0 ? void 0 : data.domainId,
                            ext: (_71 = data === null || data === void 0 ? void 0 : data.ext) !== null && _71 !== void 0 ? _71 : 0
                        };
                        try {
                            const updateScore = yield (0, webChat_dao_1.sentimentalScore)(nlpData);
                            if ((updateScore === null || updateScore === void 0 ? void 0 : updateScore.status) === 200) {
                                yield (0, webChat_dao_1.updateSentimentScoreChat)(nlpData, updateScore === null || updateScore === void 0 ? void 0 : updateScore.data);
                                const deflectionData = yield (0, webChat_dao_1.chatDeflectionData)(nlpData);
                                if ((deflectionData === null || deflectionData === void 0 ? void 0 : deflectionData.status) === 200) {
                                    yield (0, webChat_dao_1.updateChatDeflectionChat)(nlpData, (_72 = deflectionData === null || deflectionData === void 0 ? void 0 : deflectionData.data) === null || _72 === void 0 ? void 0 : _72.deflection_count);
                                }
                                else {
                                    console.error('Failed to get deflection data:', deflectionData === null || deflectionData === void 0 ? void 0 : deflectionData.status);
                                }
                            }
                            else {
                                console.error('Failed to update sentiment score:', updateScore === null || updateScore === void 0 ? void 0 : updateScore.status);
                            }
                        }
                        catch (err) {
                            console.error('Error in sentiment analysis or deflection update:', err);
                        }
                        this.io.of('/').to(data === null || data === void 0 ? void 0 : data.domainId).emit('customerbot_end_session', data);
                    }
                    else {
                        console.log('customer_end_session>>', data);
                        this.io.of('/').to(data === null || data === void 0 ? void 0 : data.to).emit('customer_end_session', data);
                    }
                    log_1.logger.info(`update_end_session Socket Error: ${JSON.stringify(data)}`);
                }
                catch (err) {
                    console.error('Error during end session update:', err);
                }
            }));
            /**************  End Of Webchat   *******************/
            socket.join(socket.handshake.auth.id);
            socket.on('language_switch', (data) => __awaiter(this, void 0, void 0, function* () {
                db_1.nlp_sub.publish('multi_lang', JSON.stringify(data));
            }));
            socket.on('callback_session', (data) => __awaiter(this, void 0, void 0, function* () {
                var _73, _74, _75, _76;
                db_1.pubsubRedisClient.publish(process.env.RESPONSECHANNEL, JSON.stringify(data));
                console.log('callback_session', data, '`${data?.domain_id}${data?.ext}`', `${data === null || data === void 0 ? void 0 : data.domain_id}${data === null || data === void 0 ? void 0 : data.ext}`);
                this.io.of('/').to(`${data === null || data === void 0 ? void 0 : data.domain_id}_${data === null || data === void 0 ? void 0 : data.ext}`).emit('callback_update', data);
                if ((data === null || data === void 0 ? void 0 : data.status) === 'rejected') {
                    const body = {
                        domainId: data === null || data === void 0 ? void 0 : data.domain_id,
                        ext: null
                    };
                    const userDetails = yield (0, chat_dao_1.getAllUserList)(body);
                    if (userDetails.length > 0) {
                        const userList = userDetails.filter((i) => {
                            return i.isVoice === 1;
                        });
                        if (userList.length === 0) {
                            const info = {
                                phoneNumber: data === null || data === void 0 ? void 0 : data.callbackDid,
                                domainId: data === null || data === void 0 ? void 0 : data.domain_id
                            };
                            // const datas: any = await getCustomerDetails(data?.callbackDid);
                            const datas = yield (0, chat_dao_1.getCustomerInfo)(info);
                            console.log('getCustomerInfo callback_session', datas);
                            const notification = {
                                uuid: (0, uuid_1.v4)(),
                                toExt: data.ext,
                                notificationMsg: `Call Callback received from ${(_73 = datas[0]) === null || _73 === void 0 ? void 0 : _73.primaryPhoneNumber}`,
                                channelType: 'Call',
                                markAsRead: 0,
                                type: `Call ${data.req_type}`,
                                timeStamp: new Date().getTime(),
                                coid: (_74 = datas[0]) === null || _74 === void 0 ? void 0 : _74.coid,
                                customerValue: (_75 = datas[0]) === null || _75 === void 0 ? void 0 : _75.primaryPhoneNumber,
                                domainId: (_76 = datas[0]) === null || _76 === void 0 ? void 0 : _76.domainId
                            };
                            console.log('CAll CAllBACk', notification);
                            yield (0, chat_dao_1.insertNotificationDao)(notification);
                            this.io.of('/').to(`${data === null || data === void 0 ? void 0 : data.domain_id}`).emit('assignActivityListener', { data });
                        }
                    }
                }
            }));
            // live visit tracking
            socket.on('liveTracking', (data) => __awaiter(this, void 0, void 0, function* () {
                console.log(data);
                this.io.of('/').to(`${data === null || data === void 0 ? void 0 : data.domainId}`).emit('liveTrackingReport', data);
            }));
            socket.on('assignActivityEmitter', (data) => __awaiter(this, void 0, void 0, function* () {
                const agentExt = data.assignedTo[0];
                this.io.of('/').to(`${data === null || data === void 0 ? void 0 : data.domainId}_${agentExt === null || agentExt === void 0 ? void 0 : agentExt.ext}`).emit('assignActivityListener', { data });
            }));
            socket.on('queue_list_update', (data) => {
                this.io.emit('queue_name_update', data);
            });
            socket.on('reAssignActivityEmitter', (data) => __awaiter(this, void 0, void 0, function* () {
                const agentExt = data.assignedTo[0];
                this.io.of('/').to(`${data === null || data === void 0 ? void 0 : data.domainId}_${agentExt === null || agentExt === void 0 ? void 0 : agentExt.ext}`).emit('reAssignActivityListener', { data });
            }));
            socket.on('init_transfer', (event) => {
                this.redisadaptor.publishMessage(JSON.stringify(event));
            });
            socket.on('call_merge', (event) => {
                var _a;
                if (event === null || event === void 0 ? void 0 : event.ext) {
                    this.io.to((_a = event === null || event === void 0 ? void 0 : event.ext) === null || _a === void 0 ? void 0 : _a.toString()).emit('callMerged', event);
                }
            });
            socket.on('leave_call_primary', (event) => {
                if (event === null || event === void 0 ? void 0 : event.ext) {
                    for (const [i, agent] of event === null || event === void 0 ? void 0 : event.ext.entries()) {
                        this.io.to(`${event.domainId}_${agent}`).emit('leave_call', event);
                    }
                }
            });
            socket.on('leave_call_Secondary', (event) => {
                console.log(event, 'leave call sec');
                if (event === null || event === void 0 ? void 0 : event.domainWithExt) {
                    for (const [i, agent] of event === null || event === void 0 ? void 0 : event.domainWithExt.entries()) {
                        console.log(agent, 'agent>>>>');
                        this.io.to(`${agent}`).emit('leave_call_by_secondary', event);
                    }
                }
            });
            socket.on('biometricsEnroll', (event) => {
                this.redisadaptor.publishMessage(JSON.stringify(event));
            });
            socket.on('bwl_switch', (event) => {
                this.redisadaptor.publishMessage(JSON.stringify(event));
            });
            socket.on('agent_register_request', (event) => {
                this.redisadaptor.publishMessage(JSON.stringify(event));
            });
            socket.on('worktualPurchase', (data) => {
                const parsedData = JSON.parse(data !== null && data !== void 0 ? data : null);
                if (parsedData.botType === 'convo') {
                    this.redisadaptor.publishMessageToNLPAI(data);
                }
                else if ((parsedData === null || parsedData === void 0 ? void 0 : parsedData.isTemplate) === 1) {
                    this.redisadaptor.publishLiteMessageToNLP(data);
                }
                else {
                    this.redisadaptor.publishMessageToNLP(data);
                }
            });
            socket.on('updatedstatuslist', (event) => __awaiter(this, void 0, void 0, function* () {
                var _77;
                if (event === null || event === void 0 ? void 0 : event.domainId) {
                    const results = yield (0, dispositionStatus_1.getAgentListByDomianId)(event);
                    const extractedData = results === null || results === void 0 ? void 0 : results.map((data) => `${data === null || data === void 0 ? void 0 : data.domainId}_${data === null || data === void 0 ? void 0 : data.ext}`);
                    (_77 = this.io.to(extractedData)) === null || _77 === void 0 ? void 0 : _77.emit('updatedstatuslist', event);
                }
            }));
            socket.on('multi_login_restriction', (data, callBack) => __awaiter(this, void 0, void 0, function* () {
                try {
                    if (typeof callBack !== 'function') {
                        console.error('No callback function provided by client');
                        return;
                    }
                    const key = `${data === null || data === void 0 ? void 0 : data.domainId}_presence`;
                    const inactivityKey = `${data === null || data === void 0 ? void 0 : data.domainId}_inactivity`;
                    const field = `${data === null || data === void 0 ? void 0 : data.domainId}_${data.ext}`;
                    const presenceRes = yield db_1.redisForPrensence.hget(key, field);
                    const inactivityRes = yield db_1.pubsubRedisClient.hget(inactivityKey, field);
                    const presence = (0, constants_1.isJSON)(presenceRes) ? JSON.parse(presenceRes) : presenceRes;
                    const inactivity = (0, constants_1.isJSON)(inactivityRes) ? JSON.parse(inactivityRes) : inactivityRes;
                    console.log('multi_login_restriction Presence status:>>>>>>>>>>>', ['Offline', 'Away'].includes(presence === null || presence === void 0 ? void 0 : presence.statusName), (inactivity === null || inactivity === void 0 ? void 0 : inactivity.ccaasInactive) !== false, (inactivity === null || inactivity === void 0 ? void 0 : inactivity.ticketingInactive) !== false);
                    const decoded = jwt.verify(socket.handshake.auth.token, app_1.configs.jwtsecret);
                    this.redisadaptor
                        .getValuefromredis(`${decoded.sipLoginId}_${decoded.source}`)
                        .then((res) => __awaiter(this, void 0, void 0, function* () {
                        if (res) {
                            // Away -- temp for login
                            if ((['Offline'].includes(presence === null || presence === void 0 ? void 0 : presence.statusName) || (inactivity === null || inactivity === void 0 ? void 0 : inactivity.ccaasInactive) !== false)) {
                                yield db_1.redisClientAuth.set(`LOGGED_IN_USER_TOKEN_${decoded.domainId}_${decoded.ext}_${decoded.source}`, socket.handshake.auth.token);
                                yield db_1.logOutPubInstance.publish('worktual-logout-channel_yoo', JSON.stringify({
                                    domainId: data === null || data === void 0 ? void 0 : data.domainId,
                                    ext: data === null || data === void 0 ? void 0 : data.ext,
                                    loginTime: data === null || data === void 0 ? void 0 : data.loginTime,
                                    source: data === null || data === void 0 ? void 0 : data.source,
                                }));
                                // this.io.of('/').to(field).emit('newLoginUpdate', data);
                                (0, constants_1.updateInactivity)(data === null || data === void 0 ? void 0 : data.domainId, data === null || data === void 0 ? void 0 : data.ext, false);
                                return callBack({ status: '200', message: 'Success', response: Object.assign(Object.assign({}, presence), inactivity) });
                            }
                            {
                                const checkToken = JSON.parse(res !== null && res !== void 0 ? res : 'null');
                                const token = yield db_1.redisClientAuth.get(`LOGGED_IN_USER_TOKEN_${decoded.domainId}_${decoded.ext}_${decoded.source}`);
                                const dataForRedis = Object.assign(Object.assign({}, checkToken), { token: token ? token : socket.handshake.auth.token, refreshToken: token ? token : socket.handshake.auth.token });
                                yield db_1.redisClientAuth.set(`${decoded.sipLoginId}_${decoded.source}`, JSON.stringify(dataForRedis));
                                return callBack({ status: '404', message: 'Credentials Currently active in Another device!', response: Object.assign(Object.assign({}, presence), inactivity) });
                            }
                        }
                        yield db_1.logOutPubInstance.publish('worktual-logout-channel_yoo', JSON.stringify({
                            domainId: data === null || data === void 0 ? void 0 : data.domainId,
                            ext: data === null || data === void 0 ? void 0 : data.ext,
                            loginTime: data === null || data === void 0 ? void 0 : data.loginTime,
                            source: data === null || data === void 0 ? void 0 : data.source,
                        }));
                        // this.io.of('/').to(field).emit('newLoginUpdate', data);
                        (0, constants_1.updateInactivity)(data === null || data === void 0 ? void 0 : data.domainId, data === null || data === void 0 ? void 0 : data.ext, false);
                        return callBack({ status: '200', message: 'Success', response: Object.assign(Object.assign({}, presence), inactivity) });
                    }));
                }
                catch (error) {
                    console.error('multi_login_restriction error:', error);
                    return callBack({ status: '500', message: 'Server error' });
                }
            }));
            /* socket.on('updatestate_backend', async (data: any) => {
              console.log('EMITTING EVENT FOR STATE UPDATE: ', data);
              try {
                if (['Ready', 'Not Ready', 'Transfer Only', 'Away', 'Offline', 'Busy'].includes(data?.statusName)) {
                  this.redisadaptor.presencePushToUmn(process.env.PRESENCECHANNEL, {
                    ext: data?.ext,
                    statusName: data?.statusName,
                    UserName: data?.userName ?? '',
                    isVoice: data?.isVoice,
                    domainId: data?.domainId,
                    email: data?.email ?? '',
                    userId: `${data?.userId}` ?? '',
                  });
                  this.redisadaptor.presenceAllChannel(data);
                }
                const userDetails: any = {
                  statusName: data?.statusName,
                  ext: parseInt(data?.ext, 10),
                  email: data?.email ?? '',
                  UserName: data?.userName ?? '',
                  isChat: data?.isChat,
                  domainId: data?.domainId,
                  UserID: data?.userId,
                  routingProfileId: data?.routingProfileId
                };
                if (data?.statusName === 'Ready' || data?.statusName === 'Transfer Only') {
                  otherRedisClient.rpush(`${data?.domainId}_${process.env.AGENTTRACKERCHANNEL}`, JSON.stringify(userDetails));
                }
              } catch (error: any) {
                console.log(error);
              }
            }); */
            socket.on('waitingQueueUpdate', (data) => {
                this.io.of('/').emit('waitingQueueUpdate', data);
            });
            socket.on('reconnect', (message) => {
                socket.join(message);
            });
            socket.on('send_chatbot_req', (data) => __awaiter(this, void 0, void 0, function* () {
                console.log('send_chatbot_req', data);
                const parsedData = JSON.parse(data !== null && data !== void 0 ? data : null);
                if (parsedData.botType === 'convo') {
                    this.redisadaptor.publishMessageToNLPAI(data);
                }
                else if ((parsedData === null || parsedData === void 0 ? void 0 : parsedData.isTemplate) === 1) {
                    this.redisadaptor.publishLiteMessageToNLP(data);
                }
                else {
                    this.redisadaptor.publishMessageToNLP(data);
                }
            }));
            socket.on('send_ai_req', (data) => __awaiter(this, void 0, void 0, function* () {
                console.log('send_chatbot_req', data);
                this.redisadaptor.publishMessageToNLPAI(JSON.stringify(data));
            }));
            socket.on('send_b2c_search_message', (data) => __awaiter(this, void 0, void 0, function* () {
                console.log('send_b2c_search_message', data);
                this.redisadaptor.publishMessageToB2c(JSON.stringify(data));
            }));
            socket.on('ai_dashboard_query', (data) => __awaiter(this, void 0, void 0, function* () {
                console.log('ai_dashboard_query', data);
                // this.aidashboard.sendCcaasDynamicDashRequest(JSON.stringify(data));
                const streamKey = process.env.DYNAMIC_DASH_STREAM_NAME_REQ;
                db_1.NLPRedisClient.xadd(streamKey, '*', 'message', (JSON.stringify(data)), (error, messageId) => {
                    if (error) {
                        console.error('Error adding entry to stream:', error);
                    }
                });
            }));
            socket.on('ai_chat_query', (data) => __awaiter(this, void 0, void 0, function* () {
                console.log('ai_chat_query', data);
                // this.aichat.sendCcaasAIChatRequest(JSON.stringify(data));
                const streamKey = process.env.AI_CHAT_STREAM_NAME_REQ;
                db_1.NLPRedisClient.xadd(streamKey, '*', 'message', (JSON.stringify(data)), (error, messageId) => {
                    if (error) {
                        console.error('Error adding entry to stream:', error);
                    }
                });
            }));
            socket.on('send_visitor_insights_nlp', (data) => __awaiter(this, void 0, void 0, function* () {
                console.log('send_visitor_insights_nlp', data);
                this.redisadaptor.sendwebsiteInsightsSteam(JSON.stringify(data));
            }));
            socket.on('lead_contact_update', (data) => __awaiter(this, void 0, void 0, function* () {
                (0, chat_dao_1.getLeadCRMinformation)(data);
            }));
            socket.on('videoMeetDecline', (data) => __awaiter(this, void 0, void 0, function* () {
                this.redisadaptor.publishVideoMessage(JSON.stringify(data));
            }));
            socket.on('missed_call_dashboard', (data) => __awaiter(this, void 0, void 0, function* () {
                const ext = yield (0, webChat_dao_1.getExt)(data).then((result) => {
                    return result.map((row) => `${data === null || data === void 0 ? void 0 : data.domainId}_${row.ext}`);
                });
                yield (0, webChat_dao_1.createChatSession)(data).then((result) => __awaiter(this, void 0, void 0, function* () {
                    var _78, _79, _80, _81;
                    if (result) {
                        const body = {
                            startTmpstmp: (_78 = data === null || data === void 0 ? void 0 : data.startTmpstmp) !== null && _78 !== void 0 ? _78 : null,
                            endTmpstmp: (_79 = data === null || data === void 0 ? void 0 : data.endTmpstmp) !== null && _79 !== void 0 ? _79 : null,
                            domainId: (_80 = data === null || data === void 0 ? void 0 : data.domainId) !== null && _80 !== void 0 ? _80 : null,
                            ext: (_81 = data === null || data === void 0 ? void 0 : data.ext) !== null && _81 !== void 0 ? _81 : null
                        };
                        const getWebchatChat = yield (0, webChat_dao_1.getWebchatLiveDashborad)(body);
                        const webchat = {
                            TotalChat: getWebchatChat[0][0].Total,
                            Inbound: getWebchatChat[0][0].Inbound,
                            Resolved: getWebchatChat[0][0].Resolved,
                            unresolved: getWebchatChat[0][0].Unresolved,
                            ChatAbandonRate: getWebchatChat[0][0].Abandoned,
                            ChatTransferRate: getWebchatChat[0][0].Transferred,
                            servicelevel: getWebchatChat[0][0].servicelevel,
                            Missed: getWebchatChat[0][0].Missed,
                            QueuedCustomers: getWebchatChat[0][0].QueuedCustomers,
                            Callback_requested: getWebchatChat[0][0].Callback_requested
                        };
                        db_1.ioredisChat.to(ext).emit('webchatLiveDashboard', webchat);
                    }
                }));
            }));
            socket.on('agent_missed_call_dashboard', (data) => __awaiter(this, void 0, void 0, function* () {
                var _82, _83, _84, _85, _86, _87;
                const DestinationChatDurationHistoryId = yield (0, webChat_dao_1.updateAgentDetails)(data);
                const dupEntry = {
                    SourceChatDurationHistoryId: data === null || data === void 0 ? void 0 : data.SourceChatDurationHistoryId,
                    DestinationChatDurationHistoryId: (_83 = (_82 = DestinationChatDurationHistoryId === null || DestinationChatDurationHistoryId === void 0 ? void 0 : DestinationChatDurationHistoryId[0]) === null || _82 === void 0 ? void 0 : _82[0]) === null || _83 === void 0 ? void 0 : _83.ChatDurationHistoryId
                };
                (0, webChat_dao_1.setTransferChatMessage)(dupEntry);
                const body = {
                    startTmpstmp: (_84 = data === null || data === void 0 ? void 0 : data.startTmpstmp) !== null && _84 !== void 0 ? _84 : null,
                    endTmpstmp: (_85 = data === null || data === void 0 ? void 0 : data.endTmpstmp) !== null && _85 !== void 0 ? _85 : null,
                    domainId: (_86 = data === null || data === void 0 ? void 0 : data.domainId) !== null && _86 !== void 0 ? _86 : null,
                    ext: (_87 = data === null || data === void 0 ? void 0 : data.ext) !== null && _87 !== void 0 ? _87 : null
                };
                const getWebchatChat = yield (0, webChat_dao_1.getWebchatLiveDashborad)(body);
                const webchat = {
                    TotalChat: getWebchatChat[0][0].Total,
                    Inbound: getWebchatChat[0][0].Inbound,
                    Resolved: getWebchatChat[0][0].Resolved,
                    unresolved: getWebchatChat[0][0].Unresolved,
                    ChatAbandonRate: getWebchatChat[0][0].Abandoned,
                    ChatTransferRate: getWebchatChat[0][0].Transferred,
                    servicelevel: getWebchatChat[0][0].servicelevel,
                    Missed: getWebchatChat[0][0].Missed,
                    QueuedCustomers: getWebchatChat[0][0].QueuedCustomers,
                    Callback_requested: getWebchatChat[0][0].Callback_requested
                };
                db_1.ioredisChat.to(`${data === null || data === void 0 ? void 0 : data.domainId}_${data === null || data === void 0 ? void 0 : data.ext}`).emit('webchatLiveDashboard', webchat);
            }));
            socket.on('abandoned_chat_dashboard', (data) => __awaiter(this, void 0, void 0, function* () {
                const test = (0, webChat_dao_1.updateAgentDetails)(data);
                test.then((result) => __awaiter(this, void 0, void 0, function* () {
                    var _88, _89, _90, _91;
                    const ext = yield (0, webChat_dao_1.getExt)(data).then((spdata) => {
                        return spdata.map((row) => `${data === null || data === void 0 ? void 0 : data.domainId}_${row.ext}`);
                    });
                    if (result) {
                        const body = {
                            startTmpstmp: (_88 = data === null || data === void 0 ? void 0 : data.startTmpstmp) !== null && _88 !== void 0 ? _88 : null,
                            endTmpstmp: (_89 = data === null || data === void 0 ? void 0 : data.endTmpstmp) !== null && _89 !== void 0 ? _89 : null,
                            domainId: (_90 = data === null || data === void 0 ? void 0 : data.domainId) !== null && _90 !== void 0 ? _90 : null,
                            ext: (_91 = data === null || data === void 0 ? void 0 : data.ext) !== null && _91 !== void 0 ? _91 : 0
                        };
                        const getWebchatChat = yield (0, webChat_dao_1.getWebchatLiveDashborad)(body);
                        console.log('getWebchatChat:::::::', getWebchatChat);
                        const webchat = {
                            TotalChat: getWebchatChat[0][0].Total,
                            Inbound: getWebchatChat[0][0].Inbound,
                            Resolved: getWebchatChat[0][0].Resolved,
                            unresolved: getWebchatChat[0][0].Unresolved,
                            ChatAbandonRate: getWebchatChat[0][0].Abandoned,
                            ChatTransferRate: getWebchatChat[0][0].Transferred,
                            servicelevel: getWebchatChat[0][0].servicelevel,
                            Missed: getWebchatChat[0][0].Missed,
                            QueuedCustomers: getWebchatChat[0][0].QueuedCustomers,
                            Callback_requested: getWebchatChat[0][0].Callback_requested,
                        };
                        console.log('webchat::::', webchat);
                        db_1.ioredisChat.to(ext).emit('webchatLiveDashboard', webchat);
                    }
                }));
            }));
            socket.on('disposition_status_dashboard', (data) => __awaiter(this, void 0, void 0, function* () {
                db_1.ioredisChat.to(`${data === null || data === void 0 ? void 0 : data.domainId}`).emit('updated_webchat_LiveDashboard');
            }));
            socket.on('chat_notify_dial_duration', (data) => __awaiter(this, void 0, void 0, function* () {
                console.log('chat_notify_dial_duration', data);
                (0, webChat_dao_1.updateAgentDetails)(data);
            }));
            socket.on('inbound_chat_dashboard', (data) => __awaiter(this, void 0, void 0, function* () {
                var _92, _93, _94, _95;
                const adminSupervisorext = yield (0, webChat_dao_1.getExt)(data).then((result) => {
                    return result === null || result === void 0 ? void 0 : result.map((row) => `${data === null || data === void 0 ? void 0 : data.domainId}_${row.ext}`);
                });
                const body = {
                    startTmpstmp: (_92 = data === null || data === void 0 ? void 0 : data.startTmpstmp) !== null && _92 !== void 0 ? _92 : null,
                    endTmpstmp: (_93 = data === null || data === void 0 ? void 0 : data.endTmpstmp) !== null && _93 !== void 0 ? _93 : null,
                    domainId: (_94 = data === null || data === void 0 ? void 0 : data.domainId) !== null && _94 !== void 0 ? _94 : null,
                    ext: (_95 = data === null || data === void 0 ? void 0 : data.ext) !== null && _95 !== void 0 ? _95 : 0
                };
                const getWebchatChat = yield (0, webChat_dao_1.getWebchatLiveDashborad)(body);
                const webchat = {
                    TotalChat: getWebchatChat[0][0].Total,
                    Inbound: getWebchatChat[0][0].Inbound,
                    Resolved: getWebchatChat[0][0].Resolved,
                    unresolved: getWebchatChat[0][0].Unresolved,
                    ChatAbandonRate: getWebchatChat[0][0].Abandoned,
                    ChatTransferRate: getWebchatChat[0][0].Transferred,
                    servicelevel: getWebchatChat[0][0].servicelevel,
                    Missed: getWebchatChat[0][0].Missed,
                    QueuedCustomers: getWebchatChat[0][0].QueuedCustomers,
                    Callback_requested: getWebchatChat[0][0].Callback_requested
                };
                db_1.ioredisChat.to(adminSupervisorext).emit('webchatLiveDashboard', webchat);
            }));
            socket.on('dashboard_chat_callback', (data) => __awaiter(this, void 0, void 0, function* () {
                var _96, _97, _98, _99;
                const adminSupervisorext = yield (0, webChat_dao_1.getExt)(data).then((result) => {
                    return result.map((row) => `${data === null || data === void 0 ? void 0 : data.domainId}_${row.ext}`);
                });
                const body = {
                    startTmpstmp: (_96 = data === null || data === void 0 ? void 0 : data.startTmpstmp) !== null && _96 !== void 0 ? _96 : null,
                    endTmpstmp: (_97 = data === null || data === void 0 ? void 0 : data.endTmpstmp) !== null && _97 !== void 0 ? _97 : null,
                    domainId: (_98 = data === null || data === void 0 ? void 0 : data.domainId) !== null && _98 !== void 0 ? _98 : null,
                    ext: (_99 = data === null || data === void 0 ? void 0 : data.ext) !== null && _99 !== void 0 ? _99 : 0
                };
                const getWebchatChat = yield (0, webChat_dao_1.getWebchatLiveDashborad)(body);
                const webchat = {
                    TotalChat: getWebchatChat[0][0].Total,
                    Inbound: getWebchatChat[0][0].Inbound,
                    Resolved: getWebchatChat[0][0].Resolved,
                    unresolved: getWebchatChat[0][0].Unresolved,
                    ChatAbandonRate: getWebchatChat[0][0].Abandoned,
                    ChatTransferRate: getWebchatChat[0][0].Transferred,
                    servicelevel: getWebchatChat[0][0].servicelevel,
                    Missed: getWebchatChat[0][0].Missed,
                    QueuedCustomers: getWebchatChat[0][0].QueuedCustomers,
                    Callback_requested: getWebchatChat[0][0].Callback_requested
                };
                db_1.ioredisChat.to(adminSupervisorext).emit('webchatLiveDashboard', webchat);
            }));
            socket.on('allviewsRefresh', (data) => __awaiter(this, void 0, void 0, function* () {
                db_1.ioredisChat.to(`${data === null || data === void 0 ? void 0 : data.domainId}`).emit('allviewsRefresh');
            }));
            socket.on('chatIntentEvent', (data) => __awaiter(this, void 0, void 0, function* () {
                console.log('Received data:::', data);
                try {
                    yield (0, webChat_dao_1.insertUpdateChatIntentDao)(data);
                }
                catch (error) {
                    console.log('err', error);
                }
            }));
            socket.on('dashboard_call', (data) => __awaiter(this, void 0, void 0, function* () {
                console.log('callLiveDashboard data:::', data);
                db_1.ioredisChat.to(`${data}`).emit('callLiveDashboard');
            }));
            socket.on('dashboard_email', (data) => __awaiter(this, void 0, void 0, function* () {
                db_1.ioredisChat.to(`${data}`).emit('emailLiveDashboard');
            }));
            socket.on('insert_update_agent_assist', (data) => __awaiter(this, void 0, void 0, function* () {
                var _100, _101;
                try {
                    if ((data === null || data === void 0 ? void 0 : data.isUpdate) === 1) {
                        const id = {
                            HistoryId: data === null || data === void 0 ? void 0 : data.HistoryId,
                            DurationHistoryId: data === null || data === void 0 ? void 0 : data.DurationHistoryId,
                            domainId: data === null || data === void 0 ? void 0 : data.domainId
                        };
                        const getAssistMessageResponse = yield (0, webChat_dao_1.getAgentAssistMessageDtl)(id);
                        const assistMessagesRaw = (_100 = getAssistMessageResponse[0][0]) === null || _100 === void 0 ? void 0 : _100.assistMsg;
                        if (assistMessagesRaw) {
                            try {
                                const assistMessages = JSON.parse(assistMessagesRaw);
                                if ((_101 = data === null || data === void 0 ? void 0 : data.AssistMsg) === null || _101 === void 0 ? void 0 : _101.messageId) {
                                    // if (element?.isDisLike === 1) {
                                    //   element['isLike'] = 0
                                    // }else if(element?.islike === 1){
                                    //   element['isDisLike'] = 0
                                    // }
                                    // const element = data?.AssistMsg?.isLike == 1 ? 1 : 0;
                                    // const element2 = data?.AssistMsg?.isDisLike == 1 ? data?.AssistMsg?.isL : 0
                                    // const index = assistMessages.findIndex((message: any) => message.messageId === updatedMessageId);
                                    // if (index !== -1) {
                                    //   assistMessages[index] = {
                                    //     // ...assistMessages[index],
                                    //     ...data?.AssistMsg,
                                    //     isLike: element,
                                    //     isDisLike: !element,
                                    //   };
                                    const replacedArray = assistMessages === null || assistMessages === void 0 ? void 0 : assistMessages.map((elem) => {
                                        var _a;
                                        if (elem.messageId === ((_a = data === null || data === void 0 ? void 0 : data.AssistMsg) === null || _a === void 0 ? void 0 : _a.messageId)) {
                                            if ((data === null || data === void 0 ? void 0 : data.AssistMsg.isUsed) === 1) {
                                                return Object.assign(Object.assign({}, elem), { isUsed: 1 });
                                            }
                                            if ((data === null || data === void 0 ? void 0 : data.AssistMsg.isLike) === 1) {
                                                return Object.assign(Object.assign({}, elem), { isLike: 1, isDislike: 0 });
                                            }
                                            if ((data === null || data === void 0 ? void 0 : data.AssistMsg.isDislike) === 1) {
                                                return Object.assign(Object.assign({}, elem), { isDislike: 1, isLike: 0 });
                                            }
                                            return elem;
                                        }
                                        return elem;
                                    });
                                    // const params = {
                                    //   historyId: data?.HistoryId,
                                    //   durationHistoryId: data?.DurationHistoryId,
                                    //   domainId: data?.domainId,
                                    //   assistMsg: JSON.stringify(replacedArray),
                                    //   noOfUsedAssistMsg: data?.NoOfUsedAssistMsg,
                                    //   totalNoOfassist: data?.totalNoOfassist,
                                    // };                                               ----- Commented out to switch from update SP handling to insert SP handling.
                                    console.log('<<<<<<<<<<<<<<<<<<<<params>>>>>>>>>>>>>>>>>>>>', data);
                                    yield (0, webChat_dao_1.insertUpdateAgentAssist)(data);
                                    return;
                                }
                            }
                            catch (jsonParseError) {
                                console.log('err', jsonParseError);
                            }
                        }
                        return;
                    }
                    if ((data === null || data === void 0 ? void 0 : data.ChannelType) === 'Call' && (data === null || data === void 0 ? void 0 : data.HistoryId)) {
                        data.CallDurationHistoryId = yield (0, webChat_dao_1.getDurationHistoryIdDao)(data);
                    }
                    (0, webChat_dao_1.insertUpdateAgentAssist)(data);
                }
                catch (error) {
                    console.log('insert_update_agent_assist', error);
                }
            }));
            socket.on('dashboard_sms', (data) => __awaiter(this, void 0, void 0, function* () {
                db_1.ioredisChat.to(`${data}`).emit('smsLiveDashboard');
            }));
            socket.on('dashboard_socialmedia', (data) => __awaiter(this, void 0, void 0, function* () {
                db_1.ioredisChat.to(`${data}`).emit('socialMediaLiveDashboard');
            }));
            socket.on('insert_notification_email', (data) => __awaiter(this, void 0, void 0, function* () {
                console.log('insert_notification_email>>>>>>>>', data);
                // ioredisChat.to(`${data}`).emit('insertSuccess');
            }));
            socket.on('whatsapp_agent_missed_call', (data) => __awaiter(this, void 0, void 0, function* () {
                (0, webChat_dao_1.updateAgentDetailsdata)(data);
            }));
            socket.on('facebook_agent_missed_call', (data) => __awaiter(this, void 0, void 0, function* () {
                (0, webChat_dao_1.updateAgentDetailsFB)(data);
            }));
            socket.on('instagram_agent_missed_call', (data) => __awaiter(this, void 0, void 0, function* () {
                (0, socialmedia_1.updateAgentDetailsInsta)(data);
            }));
            socket.on('callTransferFrom', (data) => __awaiter(this, void 0, void 0, function* () {
                console.log('callTransferFrom:', data);
                db_1.ioredisChat.to(`${data === null || data === void 0 ? void 0 : data.domainId}_${data === null || data === void 0 ? void 0 : data.toExt}`).emit('callTransferDetails', data);
            }));
            socket.on('call_answered_duration', (data) => {
                console.log('callAnswerDuration:', data);
                setTimeout(() => __awaiter(this, void 0, void 0, function* () {
                    yield (0, webChat_dao_1.UpdateCallAnsweredDurationDao)(data);
                }), 10000);
            });
            socket.on('callViaSmsEmail', (data) => __awaiter(this, void 0, void 0, function* () {
                try {
                    const messages = JSON.stringify(data);
                    console.log('callViaSmsEmail:', data);
                    const processedData = {
                        domainId: data === null || data === void 0 ? void 0 : data.domainId,
                        sessionId: data === null || data === void 0 ? void 0 : data.sessionId,
                        message: messages,
                        ext: data === null || data === void 0 ? void 0 : data.ext
                    };
                    const response = yield (0, webChat_dao_1.audioMsg)(processedData);
                    console.log('audioMsg response:', response);
                }
                catch (error) {
                    console.error('Error handling callViaSmsEmail event:', error);
                }
            }));
            socket.on('insert_Trigger', (data) => __awaiter(this, void 0, void 0, function* () {
                try {
                    if ((data === null || data === void 0 ? void 0 : data.ChannelType) === 'Chat') {
                        yield (0, webChat_dao_1.updateAgentDetails)(data);
                    }
                    else {
                        yield (0, webChat_dao_1.updateTiggerStatus)(data);
                    }
                    this.io.of('/').to(`${data === null || data === void 0 ? void 0 : data.domain_id}`).emit('trigger_emitter', data);
                }
                catch (error) {
                    console.log('trigger_emitter Errorrr', error);
                }
            }));
            socket.on('first_assist_msg', (data) => __awaiter(this, void 0, void 0, function* () {
                this.redisadaptor.agentAssistXadd(JSON.stringify(data));
            }));
            socket.on('agent_sender', (data) => __awaiter(this, void 0, void 0, function* () {
                this.io.of('/').to(`${data === null || data === void 0 ? void 0 : data.domainId}`).emit('admin_receiver', data === null || data === void 0 ? void 0 : data.bcHistoryId);
            }));
            socket.on('update_logout_cross_domain', () => __awaiter(this, void 0, void 0, function* () {
                var _102, _103;
                const data = {
                    domainId: (_102 = socket.handshake.auth.id.split('_')) === null || _102 === void 0 ? void 0 : _102[0],
                    ext: (_103 = socket.handshake.auth.id.split('_')) === null || _103 === void 0 ? void 0 : _103[1],
                };
                yield db_1.logOutPubInstance.publish('worktual-logout-channel_yoo', JSON.stringify(data));
            }));
            socket.on('update_inactivity_presence', (isInactive) => __awaiter(this, void 0, void 0, function* () {
                var _104, _105;
                const domainId = (_104 = socket.handshake.auth.id.split('_')) === null || _104 === void 0 ? void 0 : _104[0];
                const ext = (_105 = socket.handshake.auth.id.split('_')) === null || _105 === void 0 ? void 0 : _105[1];
                (0, constants_1.updateInactivity)(domainId, ext, isInactive);
            }));
            socket.on('video_channel_agent_assist', (msg) => {
                this.redisadaptor.agentAssistForVideoChannelXadd(JSON.stringify(msg));
            });
            socket.on('update_thumbsup_Feedback_chat', (data) => __awaiter(this, void 0, void 0, function* () {
                var _106, _107;
                const responseData = yield (0, callflow_dao_1.getCustomerChatHistory)(data === null || data === void 0 ? void 0 : data.ChatDurationHistoryId);
                const messageArr = [];
                const datares = (Array === null || Array === void 0 ? void 0 : Array.isArray(responseData)) && JSON.parse((_107 = (_106 = responseData[0][0]) === null || _106 === void 0 ? void 0 : _106.Message) !== null && _107 !== void 0 ? _107 : null);
                (datares === null || datares === void 0 ? void 0 : datares.length) > 0 &&
                    (datares === null || datares === void 0 ? void 0 : datares.map((msg) => {
                        var _a, _b;
                        if ((data === null || data === void 0 ? void 0 : data.messageID) === ((_b = (_a = msg === null || msg === void 0 ? void 0 : msg.userText) === null || _a === void 0 ? void 0 : _a.message) === null || _b === void 0 ? void 0 : _b.messageID)) {
                            msg.userText.message.isLike = data === null || data === void 0 ? void 0 : data.isLike;
                            msg.userText.message.feedBack = data === null || data === void 0 ? void 0 : data.feedBack;
                        }
                        messageArr.push(msg);
                    }));
                try {
                    yield (0, callflow_dao_1.updateMessageDao)(data, messageArr);
                }
                catch (err) {
                    console.log(err);
                }
            }));
            socket.on('customer_update_ai', (data) => __awaiter(this, void 0, void 0, function* () {
                console.log(data);
                this.io.of('/').to(`${data === null || data === void 0 ? void 0 : data.domainId}`).emit('customerContactEmitter', data);
            }));
            socket.on('socialMedia_disposition_send', (data) => __awaiter(this, void 0, void 0, function* () {
                console.log(data);
                this.io.of('/').to(`${data === null || data === void 0 ? void 0 : data.domainId}`).emit('socialMedia_disposition_receive', data);
            }));
            socket.on('admin_to_agent_deactivate', (data) => __awaiter(this, void 0, void 0, function* () {
                console.log('admin_to_agent_deactivate>>>>', data);
                db_1.ioredisChat.to(`${data.domainId}_${data.ext}`).emit('force_logout', data);
            }));
            const gettargetedges = (edgeid, edge) => {
                return edge.filter((list) => {
                    if (list.source === edgeid) {
                        return list;
                    }
                });
            };
        }));
    }
}
exports.IOServer = IOServer;
//# sourceMappingURL=socket-pubsub.js.map