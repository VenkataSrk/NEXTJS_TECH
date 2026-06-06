"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.Aichat = void 0;
const constants_1 = require("../helpers/constants");
const db_1 = require("../plugins/db");
class Aichat {
    constructor(socketadapter) {
        // const self: any = this;
        this.socketadapter = socketadapter;
        this.streamclient = db_1.NLPRedisClient.duplicate();
        console.log('TRIM STREAM NAME:', process.env.AI_CHAT_STREAM_NAME_RES);
        this.streamclient.xtrim(process.env.AI_CHAT_STREAM_NAME_RES, 'MAXLEN', 0)
            .then(() => {
            console.log(`Stream ${process.env.AI_CHAT_STREAM_NAME_RES} has been deleted.`);
            this.createGroup();
        })
            .catch((err) => {
            console.error('Error deleting the stream:', err);
        });
    }
    deleteGroup() {
        // const self = this;
        this.streamclient.xgroup('DESTROY', process.env.AI_CHAT_STREAM_NAME_RES, process.env.AI_CHAT_CONSUMER_GROUP)
            .then((response) => {
            console.log(`Stream "${process.env.AI_CHAT_STREAM_NAME_RES}" and consumer group "${process.env.AI_CHAT_CONSUMER_GROUP}" destroyed.`);
            this.createGroup();
        })
            .catch((err) => {
            console.error('Error destroying stream and consumer group:', err);
            this.createGroup();
        });
    }
    deleteConsumer() {
        // const self = this;
        this.streamclient.xgroup('DELCONSUMER', process.env.AI_CHAT_STREAM_NAME_RES, process.env.AI_CHAT_CONSUMER_GROUP, process.env.AI_CHAT_CONSUMER_NAME)
            .then((response) => {
            if (response === 1) {
                console.log(`Consumer "${process.env.AI_CHAT_CONSUMER_NAME}" removed from group "${process.env.AI_CHAT_CONSUMER_GROUP}"`);
                this.deleteGroup();
            }
            else {
                console.log(`Consumer "${process.env.AI_CHAT_CONSUMER_NAME}" not found in group "${process.env.AI_CHAT_CONSUMER_GROUP}"`);
                this.deleteGroup();
            }
        })
            .catch((err) => {
            console.error('Error removing consumer:', err);
            this.deleteGroup();
        });
    }
    acknowledgeMessage(stream, group, messageId, messageData) {
        this.streamclient.xack(stream, group, messageId, (err, result) => {
            var _a, _b;
            if (err) {
                console.error('Error Androidacknowledging message:', err);
            }
            else if (result === 1) {
                console.log(`${stream}_${group} Ai Chat Acknowledged message ${(_a = messageData === null || messageData === void 0 ? void 0 : messageData.session_id) !== null && _a !== void 0 ? _a : ''}_${messageId}_ACK`);
            }
            else {
                console.log(`${stream}_${group} Ai Chat Message ${(_b = messageData === null || messageData === void 0 ? void 0 : messageData.session_id) !== null && _b !== void 0 ? _b : ''}_${messageId}_ACK not found in consumer group`);
            }
        });
    }
    readMessages(stream, group, consumer) {
        console.log(stream, group, consumer);
        // const self = this;
        this.streamclient.xreadgroup('GROUP', group, consumer, 'BLOCK', 0, 'STREAMS', stream, '>', (err, streams) => {
            if (err) {
                console.error('Error reading messages:', err);
            }
            else {
                if (streams) {
                    streams.forEach((entry) => {
                        const [streamName, messages] = entry;
                        messages.forEach(([messageId, messageData]) => {
                            console.log(`READ MESSAGE::::: ${stream}_${group} Message ID: ${messageId}, Message Data: ${messageData}`);
                            if (messageData) {
                                // const rawMessage = `${messageData}`.split('message,');
                                const rawMessage = (0, constants_1.splitFirst)(`${messageData}`, 'message,');
                                this.acknowledgeMessage(stream, group, messageId, JSON.parse(rawMessage[1]));
                                this.socketadapter.receiveMessageFromAIChatResponse(messageData[1]);
                            }
                            else {
                                console.log('Ai Chat NO MSG FOUND :::  Error  >>>>> >>>> ', messageId, messageData);
                            }
                        });
                    });
                }
                this.readMessages(stream, group, consumer);
            }
        });
    }
    createGroup() {
        // const self = this;
        this.streamclient.xgroup('CREATE', process.env.AI_CHAT_STREAM_NAME_RES, process.env.AI_CHAT_CONSUMER_GROUP, '0', 'MKSTREAM', (err) => {
            if (err && !err.message.includes('BUSYGROUP Consumer Group name already exists')) {
                console.error('Error creating consumer group:', err);
                this.deleteConsumer();
            }
            else {
                console.log('START READING MESSAGE::::::');
                this.readMessages(process.env.AI_CHAT_STREAM_NAME_RES, process.env.AI_CHAT_CONSUMER_GROUP, process.env.AI_CHAT_CONSUMER_NAME);
            }
        });
    }
}
exports.Aichat = Aichat;
//# sourceMappingURL=aichat.js.map