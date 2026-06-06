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
exports.RedisAdapters = void 0;
const chatflow2_0_1 = require("../chatflow2.0");
const constants_1 = require("../constants");
const constants_2 = require("../helpers/constants");
const db_1 = require("../plugins/db");
const RedisAdapter = require('socket.io-redis');
class RedisAdapters {
    constructor(socketadapter) {
        this.sendB2cSearchSteam = (jsonData) => {
            const streamKey = process.env.B2C_STREAM_BASE_NAME_REQ;
            this.b2csearchbasedStreamClient.xadd(streamKey, '*', 'message', (jsonData), (error, messageId) => {
                if (error) {
                    console.error('Error adding entry to stream:', error);
                }
                else {
                    // console.log('Added entry with message ID:', messageId);
                    // ack(messageId)
                }
            });
        };
        this.sendwebsiteInsightsSteam = (jsonData) => {
            const streamKey = process.env.NLP_VISITORS_INSIGHTS_REQ;
            this.nlpVisitorInsightsStreamClient.xadd(streamKey, '*', 'message', (jsonData), (error, messageId) => {
                if (error) {
                    console.error('Error adding entry to stream:', error);
                }
            });
        };
        this.sendTempBasedSteam = (jsonData) => {
            const streamKey = process.env.CONVER_BASE_STREAM_NAME_REQ;
            //const jsonData = { 'question': 'Hello', 'sessionId': `${new Date().getTime()}`, 'bot_id': 2, 'botType': 'convo', 'domainId': 10300, 'websiteId': '', 'ipaddress': '194.66.175.90', 'isClose': 0 }
            //{ foo: 'Selva', baz: ie, iuid: `274d7496f5f-ccda-123c-11b0-00505690f345_10.150.0.182_${new Date().getTime()}` }; // JSON data to send
            //const streamData = [['data', JSON.stringify(jsonData)]]; // Format the JSON data as a stream entry
            // const that = this;
            console.log('sendSteam >>>>>>', jsonData);
            this.sendtemplateStreamClient.xadd(streamKey, '*', 'message', (jsonData), (error, messageId) => {
                if (error) {
                    console.error('Error adding entry to stream:', error);
                }
                else {
                    // console.log('Added entry with message ID:', messageId);
                    // ack(messageId)
                }
            });
        };
        this.sendRuleBasedSteam = (jsonData) => {
            const streamKey = process.env.TEMP_BASE_STREAM_NAME_REQ;
            this.sendrulebasetemplateStreamClient.xadd(streamKey, '*', 'message', (jsonData), (error, messageId) => {
                if (error) {
                    console.error('Error adding entry to stream:', error);
                }
                else {
                    // console.log('Added entry with message ID:', messageId);
                    // ack(messageId)
                }
            });
        };
        this.pub = db_1.redisClient;
        this.sub = db_1.pubsubRedisClient;
        this.dub = db_1.otherRedisClient;
        this.authpub = db_1.redisClientAuth;
        this.templateStreamClient = db_1.NLPRedisClient.duplicate();
        this.sendtemplateStreamClient = db_1.NLPRedisClient.duplicate();
        this.rulebaseStreamClient = db_1.NLPRedisClient.duplicate();
        this.sendrulebasetemplateStreamClient = db_1.NLPRedisClient.duplicate();
        this.b2csearchbasedStreamClient = db_1.B2cRedisClient.duplicate();
        this.nlpVisitorInsightsStreamClient = db_1.NLPRedisClient.duplicate();
        this.litebaseStreamClient = db_1.NLPRedisClient.duplicate();
        this.sendlitetemplateStreamClient = db_1.NLPRedisClient.duplicate();
        this.agentAssistReqStreamClient = db_1.NLPRedisClient.duplicate();
        this.agentAssistConvoReqStreamClient = db_1.NLPRedisClient.duplicate();
        this.agentAssistVideoReqStreamClient = db_1.NLPRedisClient.duplicate();
        const subscriberConvo = db_1.NLPRedisClient.duplicate();
        const subscriber1 = db_1.NLPRedisClient.duplicate();
        this.nlpWBsub = db_1.NLPRedisClient.duplicate();
        const subscriberweb = db_1.NLPRedisClient.duplicate();
        this.agentbaseStreamClient = db_1.NLPRedisClient.duplicate();
        this.agentbaseConvoStreamClient = db_1.NLPRedisClient.duplicate();
        RedisAdapters.sessionInstances = {};
        this.adaptor = RedisAdapter({ pubClient: this.dub, subClient: db_1.subscriberEmitter });
        this.socketadapter = socketadapter;
        // this.BLPOPPER();
        // this.BLPOP();
        /* tslint:disable */
        const self = this;
        // (async function responseAIblpop() {
        //   console.log("ruleBasereadStream >>>>>>")
        //   client2.xread("BLOCK", 0, "STREAMS", `${process.env.CONVER_STREAM_RESPONSE}`, "$").then((result) => {
        //     // Process the result, which is an array of messages
        //     // console.log("ruleBasereadStream >>>>>>> result", result)
        //     if (result) {
        //       const [stream, messages] = result[0];
        //       messages.forEach(([messageId, messageData]) => {
        //         //console.log(`  ruleBasereadStream: `, JSON.parse(messageData[1]));
        //         self.socketadapter.receiveMessageFromNLP(messageData[1]);
        //       });
        //     } else {
        //       console.log("No new messages.");
        //     }
        //     responseAIblpop();
        //   })
        //     .catch((error) => {
        //       console.error("Error reading from Redis:", error);
        //       responseAIblpop()
        //     });
        // })();
        // (function templateAIblpop() {
        //   console.log("templateAIblpop calling>>>>>", process.env.TEMPLATE_REDIS_CHANNEL_RES)
        //   client4.blpop(`${process.env.TEMPLATE_REDIS_CHANNEL_RES}`, 60000).then((data: any) => {
        //     try {
        //       console.log("templateAIblpop data >>>>>", data);
        //       if (data !== null || data !== undefined) {
        //         self.socketadapter.receiveMessageFromTemplateResponse(data?.at(1));
        //         templateAIblpop();
        //       } else {
        //         templateAIblpop();
        //       }
        //     } catch (error) {
        //       templateAIblpop();
        //     }
        //   })
        // })();
        (function agentAssistConvoblpop() {
            subscriberConvo.blpop(`${process.env.AGENT_ASSIT_CONVO_REDIS_CHANNEL_RES}`, 60000).then((data) => {
                try {
                    if (data !== null || data !== undefined) {
                        self.socketadapter.receiveMessageFromAgentAssist(data === null || data === void 0 ? void 0 : data.at(1));
                        agentAssistConvoblpop();
                    }
                    else {
                        agentAssistConvoblpop();
                    }
                }
                catch (error) {
                    agentAssistConvoblpop();
                }
            });
        })();
        // (function agentAssistblpop() {
        //   console.log("agentAssistblpop calling>>>>>", process.env.AGENT_ASSIT_REDIS_CHANNEL_RES)
        //   subscriber.blpop(`${process.env.AGENT_ASSIT_REDIS_CHANNEL_RES}`, 60000).then((data: any) => {
        //     try {
        //       console.log("agentAssistblpop data >>>>>", data);
        //       if (data !== null || data !== undefined) {
        //         self.socketadapter.receiveMessageFromAgentAssist(data?.at(1));
        //         agentAssistblpop();
        //       } else {
        //         agentAssistblpop();
        //       }
        //     } catch (error) {
        //       agentAssistblpop();
        //     }
        //   })
        // })();
        // (function agentStatus() {
        //   console.log("agent status>>>>>", process.env.PRESENCE_CHANNEL_REQUEST)
        //   subscriber2.blpop(`${process.env.PRESENCE_CHANNEL_REQUEST}`, 60000).then((data: any) => {
        //     try {
        //       if (data !== null || data !== undefined) {
        //         self.socketadapter.presenceResponse(data?.at(1));
        //         agentStatus();
        //       } else {
        //         agentStatus();
        //       }
        //     } catch (error) {
        //       agentStatus();
        //     }
        //   });
        // })();
        (function webScrapeblpop() {
            subscriberweb.blpop(`${process.env.WEBSITE_SCRAPE_RESPONSE}`, 60000).then((data) => {
                try {
                    console.log("agentAssistblpop data >>>>>", data);
                    if (data !== null || data !== undefined) {
                        self.socketadapter.receiveMessageFromWebscrape(data === null || data === void 0 ? void 0 : data.at(1));
                        webScrapeblpop();
                    }
                    else {
                        webScrapeblpop();
                    }
                }
                catch (error) {
                    webScrapeblpop();
                }
            });
        })();
        (function ruleBasereadStream() {
            return __awaiter(this, void 0, void 0, function* () {
                subscriber1.xread("BLOCK", 0, "STREAMS", `${process.env.TEMPLATE_STREAM_RESPONSE}`, "$").then((result) => {
                    // Process the result, which is an array of messages
                    // console.log("ruleBasereadStream >>>>>>> result", result)
                    if (result) {
                        const [stream, messages] = result[0];
                        messages.forEach(([messageId, messageData]) => {
                            self.socketadapter.receiveMessageFromTemplateResponse(messageData[1]);
                        });
                    }
                    else {
                        console.log("No new messages.");
                    }
                    ruleBasereadStream();
                })
                    .catch((error) => {
                    console.error("Error reading from Redis:", error);
                    ruleBasereadStream();
                });
            });
        })();
        /**
         * DELETE Rule Based Stream
         */
        this.templateStreamClient.xtrim(process.env.CONVER_BASE_STREAM_NAME_RES, 'MAXLEN', 0)
            .then(() => {
            console.log(`Stream "${process.env.CONVER_BASE_STREAM_NAME_RES}" has been deleted.`);
            self.convBaseCreateGroup();
        })
            .catch((err) => {
            console.error('Error deleting the stream:', err);
        })
            .finally(() => {
            // client.quit();
        });
        this.rulebaseStreamClient.xtrim(process.env.TEMP_BASE_STREAM_NAME_RES, 'MAXLEN', 0)
            .then(() => {
            console.log(`Stream "${process.env.TEMP_BASE_STREAM_NAME_RES}" has been deleted.`);
            self.ruleBaseCreateGroup();
        })
            .catch((err) => {
            console.error('Error deleting the stream:', err);
        })
            .finally(() => {
            // client.quit();
        });
        //   let i = 0;
        //  setInterval(() => {
        //    self.sendRuleBasedSteam(i)
        //    i++;
        //  }, 6000) 
        this.litebaseStreamClient.xtrim(process.env.LITE_BASE_STREAM_NAME_RES, 'MAXLEN', 0)
            .then(() => {
            console.log(`litebaseStreamClientStream "${process.env.LITE_BASE_STREAM_NAME_RES}" has been deleted.`);
            self.liteBaseCreateGroup();
        })
            .catch((err) => {
            console.error('Error deleting the stream:', err);
        })
            .finally(() => {
            // client.quit();
        });
        this.agentbaseStreamClient.xtrim(process.env.AGENT_ASSIT_STREAM_NAME_RES, 'MAXLEN', 0)
            .then(() => {
            console.log(`litebaseStreamClientStream "${process.env.AGENT_ASSIT_STREAM_NAME_RES}" has been deleted.`);
            self.agentBaseCreateGroup();
        })
            .catch((err) => {
            console.error('Error deleting the stream:', err);
        });
        this.agentbaseConvoStreamClient.xtrim(process.env.AGENT_ASSIT_CONVO_STREAM_NAME_RES, 'MAXLEN', 0)
            .then(() => {
            console.log(`litebaseStreamClientStream "${process.env.AGENT_ASSIT_CONVO_STREAM_NAME_RES}" has been deleted.`);
            self.agentBaseConvoAssistCreateGroup();
        })
            .catch((err) => {
            console.error('Error deleting the stream:', err);
        });
    }
    /**
     * Rule based chat implementation
     */
    convBaseDeleteGroup() {
        let self = this;
        this.templateStreamClient.xgroup('DESTROY', process.env.CONVER_BASE_STREAM_NAME_RES, process.env.CONVER_BASE_CONSUMER_GROUP)
            .then((response) => {
            console.log(`Stream "${process.env.CONVER_BASE_STREAM_NAME_RES}" and consumer group "${process.env.CONVER_BASE_CONSUMER_GROUP}" destroyed.`);
            self.convBaseCreateGroup();
        })
            .catch((err) => {
            console.error('Error destroying stream and consumer group:', err);
            self.convBaseCreateGroup();
        });
    }
    ;
    ruleBaseDeleteGroup() {
        let self = this;
        this.rulebaseStreamClient.xgroup('DESTROY', process.env.TEMP_BASE_STREAM_NAME_RES, process.env.TEMP_BASE_CONSUMER_GROUP)
            .then((response) => {
            self.ruleBaseCreateGroup();
        })
            .catch((err) => {
            console.error('Error destroying stream and consumer group:', err);
            self.ruleBaseCreateGroup();
        });
    }
    ;
    liteBaseDeleteGroup() {
        let self = this;
        this.litebaseStreamClient.xgroup('DESTROY', process.env.LITE_BASE_STREAM_NAME_RES, process.env.LITE_BASE_CONSUMER_GROUP)
            .then((response) => {
            self.liteBaseCreateGroup();
        })
            .catch((err) => {
            console.error('Error destroying stream and consumer group:', err);
            self.liteBaseCreateGroup();
        });
    }
    ;
    agentBaseDeleteGroup() {
        let self = this;
        this.agentbaseStreamClient.xgroup('DESTROY', process.env.AGENT_ASSIT_STREAM_NAME_RES, process.env.AGENT_ASSIT_CONSUMER_GROUP)
            .then((response) => {
            self.agentBaseCreateGroup();
        })
            .catch((err) => {
            console.error('Error destroying stream and consumer group:', err);
            self.agentBaseCreateGroup();
        });
    }
    ;
    agentBaseConvoAssistDeleteGroup() {
        let self = this;
        this.agentbaseConvoStreamClient.xgroup('DESTROY', process.env.AGENT_ASSIT_CONVO_STREAM_NAME_RES, process.env.AGENT_ASSIT_CONVO_CONSUMER_GROUP)
            .then((response) => {
            self.agentBaseConvoAssistCreateGroup();
        })
            .catch((err) => {
            console.error('Error destroying stream and consumer group:', err);
            self.agentBaseConvoAssistCreateGroup();
        });
    }
    ;
    videoChannelAssistDeleteGroup() {
        let self = this;
        this.agentAssistVideoReqStreamClient.xgroup('DESTROY', process.env.VIDEO_CHANNEL_AGENT_ASSIST_STREAM_NAME_RES, process.env.AGENT_ASSIT_CONVO_CONSUMER_GROUP)
            .then((response) => {
            self.videoChannelAssistCreateGroup();
        })
            .catch((err) => {
            console.error('Error destroying stream and consumer group:', err);
            self.videoChannelAssistCreateGroup();
        });
    }
    ;
    convBaseDeleteConsumer() {
        let self = this;
        this.templateStreamClient.xgroup('DELCONSUMER', process.env.CONVER_BASE_STREAM_NAME_RES, process.env.CONVER_BASE_CONSUMER_GROUP, process.env.CONVER_BASE_CONSUMER_NAME)
            .then((response) => {
            if (response === 1) {
                self.convBaseDeleteGroup();
            }
            else {
                self.convBaseDeleteGroup();
            }
        })
            .catch((err) => {
            console.error('Error removing consumer:', err);
            self.convBaseDeleteGroup();
        });
    }
    ;
    ruleBaseDeleteConsumer() {
        let self = this;
        this.rulebaseStreamClient.xgroup('DELCONSUMER', process.env.TEMP_BASE_STREAM_NAME_RES, process.env.TEMP_BASE_CONSUMER_GROUP, process.env.TEMP_BASE_CONSUMER_NAME)
            .then((response) => {
            if (response === 1) {
                self.ruleBaseDeleteGroup();
            }
            else {
                self.ruleBaseDeleteGroup();
            }
        })
            .catch((err) => {
            console.error('Error removing consumer:', err);
            self.ruleBaseDeleteGroup();
        });
    }
    ;
    liteBaseDeleteConsumer() {
        let self = this;
        this.litebaseStreamClient.xgroup('DELCONSUMER', process.env.LITE_BASE_STREAM_NAME_RES, process.env.LITE_BASE_CONSUMER_GROUP, process.env.LITE_BASE_CONSUMER_NAME)
            .then((response) => {
            if (response === 1) {
                self.liteBaseDeleteGroup();
            }
            else {
                self.liteBaseDeleteGroup();
            }
        })
            .catch((err) => {
            console.error('Error removing consumer:', err);
            self.liteBaseDeleteGroup();
        });
    }
    ;
    agentBaseDeleteConsumer() {
        let self = this;
        this.agentbaseStreamClient.xgroup('DELCONSUMER', process.env.AGENT_ASSIT_STREAM_NAME_RES, process.env.AGENT_ASSIT_CONSUMER_GROUP, process.env.AGENT_ASSIT_CONSUMER_NAME)
            .then((response) => {
            if (response === 1) {
                self.agentBaseDeleteGroup();
            }
            else {
                self.agentBaseDeleteGroup();
            }
        })
            .catch((err) => {
            console.error('Error removing consumer:', err);
            self.agentBaseDeleteGroup();
        });
    }
    ;
    agentBaseConvoAssistDeleteConsumer() {
        let self = this;
        this.agentbaseConvoStreamClient.xgroup('DELCONSUMER', process.env.AGENT_ASSIT_CONVO_STREAM_NAME_RES, process.env.AGENT_ASSIT_CONVO_CONSUMER_GROUP, process.env.AGENT_ASSIT_CONVO_CONSUMER_NAME)
            .then((response) => {
            if (response === 1) {
                self.agentBaseConvoAssistDeleteGroup();
            }
            else {
                self.agentBaseConvoAssistDeleteGroup();
            }
        })
            .catch((err) => {
            self.agentBaseConvoAssistDeleteGroup();
        });
    }
    ;
    videoChannelAssistDeleteConsumer() {
        let self = this;
        this.agentAssistVideoReqStreamClient.xgroup('DELCONSUMER', process.env.VIDEO_CHANNEL_AGENT_ASSIST_STREAM_NAME_RES, process.env.AGENT_ASSIT_CONVO_CONSUMER_GROUP, process.env.AGENT_ASSIT_CONVO_CONSUMER_NAME)
            .then((response) => {
            if (response === 1) {
                self.videoChannelAssistDeleteGroup();
            }
            else {
                self.videoChannelAssistDeleteGroup();
            }
        })
            .catch((err) => {
            self.videoChannelAssistDeleteGroup();
        });
    }
    ;
    ruleBasedacknowledgeMessage(stream, group, messageId, messageData) {
        // Use XACK to acknowledge the message
        this.rulebaseStreamClient.xack(stream, group, messageId, (err, result) => {
            if (err) {
            }
            else if (result === 1) {
                console.log(`${stream}_${group}  Acknowledged message ${messageId}_ACK`);
            }
            else {
                console.log(`${stream}_${group}  Message ${messageId}_ACK not found in consumer group`);
                const inputData = {
                    stream,
                    group,
                    messageId,
                    message: messageData,
                    isAck: false
                };
            }
        });
    }
    liteBasedacknowledgeMessage(stream, group, messageId, messageData) {
        // Use XACK to acknowledge the message
        this.litebaseStreamClient.xack(stream, group, messageId, (err, result) => {
            if (err) {
                console.error('Error Androidacknowledging message:', err);
            }
            else if (result === 1) {
                console.log(`${stream}_${group}  Acknowledged message ${messageId}_ACK`);
            }
            else {
                console.log(`${stream}_${group}  Message ${messageId}_ACK not found in consumer group`);
                const inputData = {
                    stream,
                    group,
                    messageId,
                    message: messageData,
                    isAck: false
                };
            }
        });
    }
    agentBasedacknowledgeMessage(stream, group, messageId, messageData) {
        this.agentbaseStreamClient.xack(stream, group, messageId, (err, result) => {
            if (err) {
                console.error('Error Androidacknowledging message:', err);
            }
            else if (result === 1) {
                console.log(`${stream}_${group}  Acknowledged message ${messageId}_ACK`);
            }
            else {
                console.log(`${stream}_${group}  Message ${messageId}_ACK not found in consumer group`);
                const inputData = {
                    stream,
                    group,
                    messageId,
                    message: messageData,
                    isAck: false
                };
            }
        });
    }
    agentBasedConvoAssistacknowledgeMessage(stream, group, messageId, messageData) {
        this.agentbaseConvoStreamClient.xack(stream, group, messageId, (err, result) => {
            if (err) {
                console.error('Error Androidacknowledging message:', err);
            }
            else if (result === 1) {
                console.log(`${stream}_${group}  Acknowledged message ${messageId}_ACK`);
            }
            else {
                console.log(`${stream}_${group}  Message ${messageId}_ACK not found in consumer group`);
                const inputData = {
                    stream,
                    group,
                    messageId,
                    message: messageData,
                    isAck: false
                };
            }
        });
    }
    convBasedacknowledgeMessage(stream, group, messageId, messageData) {
        // Use XACK to acknowledge the message
        this.templateStreamClient.xack(stream, group, messageId, (err, result) => {
            if (err) {
                console.error('Error Androidacknowledging message:', err);
            }
            else if (result === 1) {
                console.log(`${stream}_${group}  Acknowledged message ${messageId}_ACK`);
            }
            else {
                console.log(`${stream}_${group}  Message ${messageData.iuid}_${messageId}_ACK not found in consumer group`);
                const inputData = {
                    stream,
                    group,
                    messageId,
                    message: messageData,
                    isAck: false
                };
            }
        });
    }
    convBaseReadMessages(stream, group, consumer) {
        // Use XREADGROUP to read messages
        let self = this;
        this.templateStreamClient.xreadgroup('GROUP', group, consumer, 'BLOCK', 0, 'STREAMS', stream, '>', (err, streams) => {
            // 'GROUP', group, consumer, 'STREAMS', stream, '$', (err, streams) => {
            if (err) {
                console.error('Error reading messages:', err);
                // redis.quit();
            }
            else {
                // Process the messages
                if (streams) {
                    streams.forEach((entry) => {
                        const [streamName, messages] = entry;
                        messages.forEach(([messageId, messageData]) => {
                            // const rawMessage = `${messageData}`.split('message,');
                            const rawMessage = (0, constants_2.splitFirst)(`${messageData}`, 'message,');
                            console.log("rawMessage >>>>>>", rawMessage);
                            self.convBasedacknowledgeMessage(stream, group, messageId, (0, constants_1.isJSON)(rawMessage[1]) ? JSON.parse(rawMessage[1]) : rawMessage[1]);
                            self.socketadapter.receiveMessageFromNLP(messageData[1]);
                            self.templateStreamClient.xtrim(stream, 'MAXLEN', 100);
                        });
                    });
                }
                // Continue reading messages
                self.convBaseReadMessages(stream, group, consumer);
            }
        });
    }
    ruleBaseReadMessages(stream, group, consumer) {
        // Use XREADGROUP to read messages
        let self = this;
        this.rulebaseStreamClient.xreadgroup('GROUP', group, consumer, 'BLOCK', 0, 'STREAMS', stream, '>', (err, streams) => {
            // 'GROUP', group, consumer, 'STREAMS', stream, '$', (err, streams) => {
            if (err) {
                console.error('Error reading messages:', err);
                // redis.quit();
            }
            else {
                // Process the messages
                if (streams) {
                    streams.forEach((entry) => {
                        const [streamName, messages] = entry;
                        // console.log(`Received messages from ${streamName}:`);
                        messages.forEach(([messageId, messageData]) => {
                            //console.log(`readMessages >>>>> ${stream}_${group} Message ID: ${messageId}, Message Data: ${messageData}`);
                            // const rawMessage = `${messageData}`.split('message,');
                            // console.log("rule base rawMessage >>>>>>", rawMessage);
                            self.ruleBasedacknowledgeMessage(stream, group, messageId, JSON.parse(messageData[1]));
                            self.socketadapter.receiveMessageFromTemplateResponse(messageData[1]);
                            self.rulebaseStreamClient.xtrim(stream, 'MAXLEN', 100);
                        });
                    });
                }
                // Continue reading messages
                self.ruleBaseReadMessages(stream, group, consumer);
            }
        });
    }
    liteBaseReadMessages(stream, group, consumer) {
        // Use XREADGROUP to read messages
        console.log(stream, group, consumer, "consumer");
        let self = this;
        this.litebaseStreamClient.xreadgroup('GROUP', group, consumer, 'BLOCK', 0, 'STREAMS', stream, '>', (err, streams) => {
            // 'GROUP', group, consumer, 'STREAMS', stream, '$', (err, streams) => {
            if (err) {
                console.error('Error reading messages:', err);
                // redis.quit();
            }
            else {
                // Process the messages
                if (streams) {
                    streams.forEach((entry) => {
                        const [streamName, messages] = entry;
                        // console.log(`Received messages from ${streamName}:`);
                        messages.forEach(([messageId, messageData]) => {
                            //console.log(`readMessages >>>>> ${stream}_${group} Message ID: ${messageId}, Message Data: ${messageData}`);
                            // const rawMessage = `${messageData}`.split('message,');
                            const rawMessage = (0, constants_2.splitFirst)(`${messageData}`, 'message,');
                            console.log("rule base rawMessage >>>>>>", rawMessage);
                            self.socketadapter.receiveMessageFromLiteResponse(messageData[1]);
                            self.liteBasedacknowledgeMessage(stream, group, messageId, JSON.parse(rawMessage[1]));
                            self.rulebaseStreamClient.xtrim(stream, 'MAXLEN', 100);
                        });
                    });
                }
                // Continue reading messages
                self.liteBaseReadMessages(stream, group, consumer);
            }
        });
    }
    agentBaseReadMessages(stream, group, consumer) {
        console.log(stream, group, consumer, "consumer");
        let self = this;
        this.agentbaseStreamClient.xreadgroup('GROUP', group, consumer, 'BLOCK', 0, 'STREAMS', stream, '>', (err, streams) => {
            if (err) {
                console.error('Error reading messages:', err);
            }
            else {
                if (streams) {
                    streams.forEach((entry) => {
                        const [streamName, messages] = entry;
                        messages.forEach(([messageId, messageData]) => {
                            // const rawMessage = `${messageData}`.split('message,');
                            const rawMessage = (0, constants_2.splitFirst)(`${messageData}`, 'message,');
                            console.log("rule base rawMessage >>>>>>", rawMessage);
                            self.agentBasedacknowledgeMessage(stream, group, messageId, JSON.parse(rawMessage[1]));
                            self.socketadapter.receiveMessageFromAgentAssitResponse(messageData[1]);
                            self.agentbaseStreamClient.xtrim(stream, 'MAXLEN', 100);
                        });
                    });
                }
                self.agentBaseReadMessages(stream, group, consumer);
            }
        });
    }
    agentBaseConvoAssistReadMessages(stream, group, consumer) {
        let self = this;
        this.agentbaseConvoStreamClient.xreadgroup('GROUP', group, consumer, 'BLOCK', 0, 'STREAMS', stream, '>', (err, streams) => {
            if (err) {
                console.error('Error reading messages:', err);
            }
            else {
                if (streams) {
                    streams.forEach((entry) => {
                        const [streamName, messages] = entry;
                        messages.forEach(([messageId, messageData]) => {
                            // const rawMessage = `${messageData}`.split('message,');
                            const rawMessage = (0, constants_2.splitFirst)(`${messageData}`, 'message,');
                            console.log("rule base rawMessage >>>>>>", rawMessage);
                            self.agentBasedConvoAssistacknowledgeMessage(stream, group, messageId, JSON.parse(rawMessage[1]));
                            self.socketadapter.receiveMessageFromAgentAssitResponse(messageData[1]);
                            self.agentbaseStreamClient.xtrim(stream, 'MAXLEN', 100);
                        });
                    });
                }
                self.agentBaseConvoAssistReadMessages(stream, group, consumer);
            }
        });
    }
    videoChannelAssistReadMessages(stream, group, consumer) {
        let self = this;
        this.agentAssistVideoReqStreamClient.xreadgroup('GROUP', group, consumer, 'BLOCK', 0, 'STREAMS', stream, '>', (err, streams) => {
            if (err) {
                console.error('Error reading messages:', err);
            }
            else {
                if (streams) {
                    streams.forEach((entry) => {
                        const [streamName, messages] = entry;
                        messages.forEach(([messageId, messageData]) => {
                            // const rawMessage = `${messageData}`.split('message,');
                            const rawMessage = (0, constants_2.splitFirst)(`${messageData}`, 'message,');
                            console.log("rule base rawMessage >>>>>>", rawMessage);
                            self.agentBasedConvoAssistacknowledgeMessage(stream, group, messageId, JSON.parse(rawMessage[1]));
                            self.socketadapter.receiveMessageFromAgentAssitResponse(messageData[1]);
                            self.agentbaseStreamClient.xtrim(stream, 'MAXLEN', 100);
                        });
                    });
                }
                self.videoChannelAssistReadMessages(stream, group, consumer);
            }
        });
    }
    convBaseCreateGroup() {
        let self = this;
        this.templateStreamClient.xgroup('CREATE', process.env.CONVER_BASE_STREAM_NAME_RES, process.env.CONVER_BASE_CONSUMER_GROUP, '0', 'MKSTREAM', (err) => {
            if (err && !err.message.includes('BUSYGROUP Consumer Group name already exists')) {
                console.error('Error creating consumer group:', err);
                self.convBaseDeleteConsumer();
            }
            else {
                self.convBaseReadMessages(process.env.CONVER_BASE_STREAM_NAME_RES, process.env.CONVER_BASE_CONSUMER_GROUP, process.env.CONVER_BASE_CONSUMER_NAME);
            }
        });
    }
    ;
    ruleBaseCreateGroup() {
        let self = this;
        this.rulebaseStreamClient.xgroup('CREATE', process.env.TEMP_BASE_STREAM_NAME_RES, process.env.TEMP_BASE_CONSUMER_GROUP, '0', 'MKSTREAM', (err) => {
            if (err && !err.message.includes('BUSYGROUP Consumer Group name already exists')) {
                console.error('Error creating consumer group:', err);
                self.ruleBaseDeleteConsumer();
            }
            else {
                self.ruleBaseReadMessages(process.env.TEMP_BASE_STREAM_NAME_RES, process.env.TEMP_BASE_CONSUMER_GROUP, process.env.TEMP_BASE_CONSUMER_NAME);
            }
        });
    }
    ;
    liteBaseCreateGroup() {
        let self = this;
        this.litebaseStreamClient.xgroup('CREATE', process.env.LITE_BASE_STREAM_NAME_RES, process.env.LITE_BASE_CONSUMER_GROUP, '0', 'MKSTREAM', (err) => {
            if (err && !err.message.includes('BUSYGROUP Consumer Group name already exists')) {
                console.error('Error creating consumer group:', err);
                self.liteBaseDeleteConsumer();
            }
            else {
                self.liteBaseReadMessages(process.env.LITE_BASE_STREAM_NAME_RES, process.env.LITE_BASE_CONSUMER_GROUP, process.env.LITE_BASE_CONSUMER_NAME);
            }
        });
    }
    ;
    agentBaseCreateGroup() {
        let self = this;
        this.agentbaseStreamClient.xgroup('CREATE', process.env.AGENT_ASSIT_STREAM_NAME_RES, process.env.AGENT_ASSIT_CONSUMER_GROUP, '0', 'MKSTREAM', (err) => {
            if (err && !err.message.includes('BUSYGROUP Consumer Group name already exists')) {
                console.error('Error creating consumer group:', err);
                self.agentBaseDeleteConsumer();
            }
            else {
                console.log('Start read readMessage >>>>>>>>');
                self.agentBaseReadMessages(process.env.AGENT_ASSIT_STREAM_NAME_RES, process.env.AGENT_ASSIT_CONSUMER_GROUP, process.env.AGENT_ASSIT_CONSUMER_NAME);
            }
        });
    }
    ;
    agentBaseConvoAssistCreateGroup() {
        let self = this;
        this.agentbaseConvoStreamClient.xgroup('CREATE', process.env.AGENT_ASSIT_CONVO_STREAM_NAME_RES, process.env.AGENT_ASSIT_CONVO_CONSUMER_GROUP, '0', 'MKSTREAM', (err) => {
            if (err && !err.message.includes('BUSYGROUP Consumer Group name already exists')) {
                console.error('Error creating consumer group:', err);
                self.agentBaseConvoAssistDeleteConsumer();
            }
            else {
                self.agentBaseConvoAssistReadMessages(process.env.AGENT_ASSIT_CONVO_STREAM_NAME_RES, process.env.AGENT_ASSIT_CONVO_CONSUMER_GROUP, process.env.AGENT_ASSIT_CONVO_CONSUMER_NAME);
            }
        });
    }
    ;
    videoChannelAssistCreateGroup() {
        let self = this;
        this.agentAssistVideoReqStreamClient.xgroup('CREATE', process.env.VIDEO_CHANNEL_AGENT_ASSIST_STREAM_NAME_RES, process.env.AGENT_ASSIT_CONVO_CONSUMER_GROUP, '0', 'MKSTREAM', (err) => {
            if (err && !err.message.includes('BUSYGROUP Consumer Group name already exists')) {
                console.error('Error creating consumer group:', err);
                self.videoChannelAssistDeleteConsumer();
            }
            else {
                self.videoChannelAssistReadMessages(process.env.VIDEO_CHANNEL_AGENT_ASSIST_STREAM_NAME_RES, process.env.AGENT_ASSIT_CONVO_CONSUMER_GROUP, process.env.AGENT_ASSIT_CONVO_CONSUMER_NAME);
            }
        });
    }
    ;
    /** End of Rule base chat stream implementation   */
    push(channel, msg) {
        this.dub.rpush(channel, JSON.stringify({ msg }));
    }
    // public presencePushToUmn(channel: any, msg: any): void {
    //   this.pub.rpush(channel, JSON.stringify(msg));
    // }
    createSet(key, data) {
        this.dub.sadd(key, data);
    }
    setVariableValue(name, keyName, keyValue) {
        this.dub.hset(name, keyName, keyValue);
    }
    // public presenceAllChannel(data: any) {
    //   this.dub.hset(`${data.domainId}_presence`, `${data?.domainId}_${data?.ext}`, JSON.stringify({ ...data, currentTime: Date.now() }));
    // }
    getapiData() {
        return new Promise((resolve, reject) => {
            try {
                this.dub.hgetall('apiValues', (err, results) => {
                    if (results) {
                        resolve(results);
                    }
                    else {
                        reject(err);
                    }
                });
            }
            catch (error) {
                reject(error);
            }
        });
    }
    getValuefromSet(key) {
        return new Promise((resolve, reject) => {
            this.dub.smembers(key, (err, data) => {
                resolve(data);
            });
        });
    }
    getFlow(key) {
        return new Promise((resolve, reject) => {
            this.sub.get(key).then((result) => {
                resolve(result);
            });
        });
    }
    // DELETE THIS KEY ONCE TRANSFER / MERGE SESSION IS COMPLETED
    publishMessage(key) {
        console.log(key);
        this.sub.publish(process.env.RESPONSECHANNEL, key);
        // this.BLPOP();
    }
    publishVideoMessage(key) {
        console.log(key, "publishVideoMessage===", process.env.RESPONSEVIDEOCHANNEL);
        this.pub.publish(process.env.RESPONSEVIDEOCHANNEL, key);
    }
    publishMessageForAgent(key) {
        console.log(key);
        // this.BLPOP();
    }
    sendLiteSteam(inputdata) {
        console.log("send lite stream callig", inputdata);
        // const streamKey = `${process.env.TEMPLATE_STREAM_REQUEST}` //`${process.env.TEMPLATE_STREAM_RESPONSE}`//`${process.env.TEMPLATE_STREAM_REQUEST}`
        //const jsonData = inputdata; // JSON data to send
        //this.nlpsub.xadd(streamKey, "*", "data", inputdata)
        const streamKey = process.env.LITE_BASE_STREAM_NAME_REQ;
        console.log('sendLiteSteam >>>>>>', inputdata, streamKey);
        this.sendlitetemplateStreamClient.xadd(streamKey, '*', 'message', (inputdata), (error, messageId) => {
            if (error) {
                console.error('Error adding entry to stream:', error);
            }
            else {
                console.log('Added entry with message ID:', messageId);
                // ack(messageId)
            }
        });
        // this.sendRuleBasedSteam(inputdata)
    }
    sendSteam(inputdata) {
        // const streamKey = `${process.env.TEMPLATE_STREAM_REQUEST}` //`${process.env.TEMPLATE_STREAM_RESPONSE}`//`${process.env.TEMPLATE_STREAM_REQUEST}`
        //const jsonData = inputdata; // JSON data to send
        //this.nlpsub.xadd(streamKey, "*", "data", inputdata)
        const streamKey = process.env.TEMP_BASE_STREAM_NAME_REQ;
        this.sendrulebasetemplateStreamClient.xadd(streamKey, '*', 'message', (inputdata), (error, messageId) => {
            if (error) {
                console.error('Error adding entry to stream:', error);
            }
            else {
                // console.log('Added entry with message ID:', messageId);
                // ack(messageId)
            }
        });
        // this.sendRuleBasedSteam(inputdata)
    }
    sendConvSteam(inputdata) {
        // console.log("send stream callig conv", inputdata);
        const streamKey = `${process.env.CONVER_STREAM_REQUEST}`; //`${process.env.TEMPLATE_STREAM_RESPONSE}`//`${process.env.TEMPLATE_STREAM_REQUEST}`
        //const jsonData = inputdata; // JSON data to send
        // this.nlpsub.xadd(streamKey, "*", "data", inputdata)
        this.sendTempBasedSteam(inputdata);
    }
    sendB2cStream(inputdata) {
        this.sendB2cSearchSteam(inputdata);
    }
    /*   public sendCcaasDynamicDashRequest(inputdata: any) {
        this.sendCcaasDynamicDashSteam(inputdata)
      } */
    /*   public sendCcaasAIChatRequest(inputdata: any) {
        this.sendCcaasAiChatSteam(inputdata)
      } */
    agentAssistXadd(inputdata) {
        const streamKey = process.env.AGENT_ASSIT_STREAM_NAME_REQ;
        console.log('sendSteam >>>>>>', inputdata, streamKey);
        this.agentAssistReqStreamClient.xadd(streamKey, '*', 'message', (inputdata), (error, messageId) => {
            if (error) {
                console.error('Error adding entry to stream:', error);
            }
        });
    }
    // For Convo agent assist
    agentAssistForConvoXadd(inputdata) {
        const streamKey = process.env.AGENT_ASSIT_CONVO_STREAM_NAME_REQ;
        this.agentAssistConvoReqStreamClient.xadd(streamKey, '*', 'message', (inputdata), (error, messageId) => {
            if (error) {
                console.error('Error adding entry to stream:', error);
            }
        });
    }
    // For Video channel agent assist
    agentAssistForVideoChannelXadd(inputdata) {
        const streamKey = process.env.VIDEO_CHANNEL_AGENT_ASSIST_STREAM_NAME_REQ;
        this.agentAssistVideoReqStreamClient.xadd(streamKey, '*', 'message', (inputdata), (error, messageId) => {
            if (error) {
                console.error('Error adding entry to stream:', error);
            }
        });
    }
    publishMessageToWebChat(domainId, key) {
        console.log('CHAT: ', JSON.parse(key));
        this.dub.rpush(`${domainId}_${process.env.CHATRESPONSECHANNEL}`, key);
    }
    publishLiteMessageToNLP(key) {
        console.log('publishLiteMessageToNLP: ', process.env.TEMPLATE_STREAM_REQUEST, JSON.parse(key));
        this.sendLiteSteam(key);
        //this.nlpsub.rpush(`${process.env.TEMPLATE_REDIS_CHANNEL_REQ}`, key);
    }
    publishMessageToNLP(key) {
        console.log('publishMessageToNLP: ', process.env.TEMPLATE_STREAM_REQUEST, JSON.parse(key));
        this.sendSteam(key);
        //this.nlpsub.rpush(`${process.env.TEMPLATE_REDIS_CHANNEL_REQ}`, key);
    }
    publishMessageToNLPAI(key) {
        console.log('publishMessageToNLPAI: ', process.env.CONVER_REDIS_CHANNEL_REQ, JSON.parse(key));
        this.sendConvSteam(key);
        // this.nlpsub.rpush(`${process.env.CONVER_REDIS_CHANNEL_REQ}`, key);
    }
    publishMessageToB2c(key) {
        console.log('publishMessageToB2c: ', process.env.CONVER_REDIS_CHANNEL_REQ, JSON.parse(key));
        this.sendB2cStream(key);
    }
    publishWebscrapeToNLP(key) {
        console.log('publishMessageToNLP: ', process.env.WEBSITE_SCRAPE_REQUEST, JSON.parse(key));
        this.nlpWBsub.rpush(`${process.env.WEBSITE_SCRAPE_REQUEST}`, key);
    }
    publishMessageToAgentAssist(key) {
        console.log('publishMessageToAgentAssist: ', process.env.AGENT_ASSIT_REDIS_CHANNEL_REQ, key);
        const subscriber = db_1.NLPRedisClient.duplicate();
        subscriber.rpush(`${process.env.AGENT_ASSIT_REDIS_CHANNEL_REQ}`, key);
    }
    publishMessageToAgentAssistForConvo(key) {
        console.log('publishMessageToAgentAssistForConvo: ', process.env.AGENT_ASSIT_CONVO_REDIS_CHANNEL_REQ, key);
        const subscriber = db_1.NLPRedisClient.duplicate();
        subscriber.rpush(`${process.env.AGENT_ASSIT_CONVO_REDIS_CHANNEL_REQ}`, key);
    }
    publishMessageChat(data) {
        var _a, _b;
        console.log('publishMessageChat >>>>>', data);
        const parsedData = JSON.parse(data);
        if ((parsedData === null || parsedData === void 0 ? void 0 : parsedData.call_state) === 'init') {
            const newObject = new chatflow2_0_1.ChatFlowAdapters(this);
            RedisAdapters.sessionInstances[parsedData === null || parsedData === void 0 ? void 0 : parsedData.session_id] = newObject;
            newObject === null || newObject === void 0 ? void 0 : newObject.ChatFlowHandler(data);
        }
        else if (parsedData === null || parsedData === void 0 ? void 0 : parsedData.disconnectedBy) {
            (_a = RedisAdapters.sessionInstances[parsedData === null || parsedData === void 0 ? void 0 : parsedData.session_id]) === null || _a === void 0 ? void 0 : _a.ChatFlowHandler(data);
            delete RedisAdapters.sessionInstances[parsedData === null || parsedData === void 0 ? void 0 : parsedData.session_id];
        }
        else {
            (_b = RedisAdapters.sessionInstances[parsedData === null || parsedData === void 0 ? void 0 : parsedData.session_id]) === null || _b === void 0 ? void 0 : _b.ChatFlowHandler(data);
        }
        // this.pub.rpush('chatRequestChannel', key);
        // this.BLPOP();
    }
    getredisAdapter() {
        return this.adaptor;
    }
    removeValuebyKey(key) {
        return new Promise((resolve, reject) => {
            this.dub.del(key, (err, reply) => {
                resolve(1);
            });
        });
    }
    BLPOP(res, domainId, data) {
        return __awaiter(this, void 0, void 0, function* () {
            // console.log("BLPOP >>>>>", `${domainId}_${process.env.CHATRESPONSECHANNEL}`)
            this.dub.blpop(`${domainId}_${process.env.CHATRESPONSECHANNEL}`, 1, (err, resp) => __awaiter(this, void 0, void 0, function* () {
                const time = new Date().getTime();
                if (resp !== null || time > (data === null || data === void 0 ? void 0 : data.timestamp)) {
                    console.log(resp, 'pop response');
                    res.status(200).send({ statusCode: 200, message: 'Available agent', Result: resp });
                    return;
                }
                this.BLPOP(res, domainId, data);
            }));
        });
    }
    handleSessionBasedInstances(data) {
        var _a, _b;
        const parsedData = JSON.parse(data);
        if ((parsedData === null || parsedData === void 0 ? void 0 : parsedData.call_state) === 'init') {
            const newObject = new chatflow2_0_1.ChatFlowAdapters(this);
            RedisAdapters.sessionInstances[parsedData === null || parsedData === void 0 ? void 0 : parsedData.session_id] = newObject;
            newObject === null || newObject === void 0 ? void 0 : newObject.ChatFlowHandler(data);
        }
        else if (parsedData === null || parsedData === void 0 ? void 0 : parsedData.disconnectedBy) {
            (_a = RedisAdapters.sessionInstances[parsedData === null || parsedData === void 0 ? void 0 : parsedData.session_id]) === null || _a === void 0 ? void 0 : _a.ChatFlowHandler(data);
            delete RedisAdapters.sessionInstances[parsedData === null || parsedData === void 0 ? void 0 : parsedData.session_id];
        }
        else {
            (_b = RedisAdapters.sessionInstances[parsedData === null || parsedData === void 0 ? void 0 : parsedData.session_id]) === null || _b === void 0 ? void 0 : _b.ChatFlowHandler(data);
        }
    }
    BLPOPPER() {
        var _a, _b;
        return __awaiter(this, void 0, void 0, function* () {
            console.log(process.env.CHATRESPONSECHANNEL, 'process.env.CHATRESPONSECHANNEL', (_a = this.dub) === null || _a === void 0 ? void 0 : _a.blpop);
            (_b = this.dub) === null || _b === void 0 ? void 0 : _b.blpop('chatRequestChannel', 60000, (data) => __awaiter(this, void 0, void 0, function* () {
                console.log('Data >>>>>>.', data);
                if (data !== null && (data === null || data === void 0 ? void 0 : data.length) > 1) {
                    const resultData = data === null || data === void 0 ? void 0 : data.at(1);
                    this.handleSessionBasedInstances(resultData);
                }
                this.BLPOPPER();
            }));
        });
    }
    getValuefromredis(key) {
        return new Promise((resolve, reject) => {
            this.authpub.get(key, (err, data) => {
                resolve(data);
            });
        });
    }
    emailRpush(data, sessionId) {
        const emailData = {
            domain_id: data.domainId,
            channelType: 'Email',
            queueName: data.queueName,
            session_id: sessionId,
        };
        this.dub.rpush(process.env.REQUESTCHANNEL, emailData);
    }
    emailBlpop(data) {
        return new Promise((resolve, reject) => {
            this.dub.blpop('CCAAS_email_response', 1, (err, res) => __awaiter(this, void 0, void 0, function* () {
                if (res !== null) {
                    const agentRes = res[0];
                    db_1.ioredisChat.to(`${data.domainId}_${data.ext}`).emit('email_channel_emit', res);
                    return res;
                }
                this.emailBlpop(data);
            }));
        });
    }
    resetQueue(domainId) {
        this.dub.del(`${domainId}_${process.env.AGENTTRACKERCHANNEL}`);
        // this.newclinet.rpush(process.env.AGENTTRACKERCHANNEL, null);
    }
}
exports.RedisAdapters = RedisAdapters;
//# sourceMappingURL=index.js.map