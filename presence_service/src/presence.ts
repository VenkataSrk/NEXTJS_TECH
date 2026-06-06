/**
 * @createdBy <Jayaselva M>
 * @createdOn
 */

import { configs } from './config';
import { redisClient } from './db';
import { createAdapter, RedisAdapter } from "socket.io-redis";
var redis = require('redis')
import { RedisClient } from "redis";
import { logger } from './log';

export class PresenceAdapters {
    private adaptor: RedisAdapter;
    private socketadapter: any;
    private streamclient: any;
    private videoStreamclient: any;
    constructor(socketadapter?: any) {
        const pop = redisClient.duplicate();
        const pubClient = new RedisClient({
            host: configs.redisClient.host,
            port: configs.redisClient.port,
            no_ready_check: '',
            auth_pass: configs.redisClient.password
        });
        const subscriber = redis.createClient({
            host: configs.redisClient.host,
            port: configs.redisClient.port,
            no_ready_check: '',
            auth_pass: configs.redisClient.password
        });
        subscriber.on("message", (channel: any, message: any) => {
            console.log("mess123", message);
        });
        pubClient.on("error", (err) => console.log("Error" + err));
        pubClient.on("message", (err) => console.log("message" + err));
        pubClient.on("message_buffer", (err) => console.log("message_buffer" + err));
        pubClient.on("pmessage", (err) => console.log("pmessage" + err));
        pubClient.on("subscribe", (err) => console.log("subscribe" + err));
        pubClient.on("psubscribe", (err) => console.log("psubscribe" + err));

        const subClient = pubClient.duplicate();
        this.adaptor = createAdapter({ pubClient, subClient });
        this.socketadapter = socketadapter;
        this.streamclient = redisClient.duplicate();
        this.videoStreamclient = redisClient.duplicate();
        const self: any = this;

        this.streamclient.xtrim(process.env.CALL_STREAM_NAME, 'MAXLEN', 0)
            .then(() => {
                console.log(`Stream ${process.env.CALL_STREAM_NAME} has been deleted.`);
                self.createGroup();
            })
            .catch((err) => {
                console.error('Error deleting the stream:', err);
                logger.error('Error deleting the stream:', process.env.CALL_STREAM_NAME, err)
            })
            .finally(() => {
                // client.quit();
            });

        this.videoStreamclient.xtrim(process.env.VIDEO_CALL_STREAM_NAME, 'MAXLEN', 0)
            .then(() => {
                console.log(`Stream ${process.env.VIDEO_CALL_STREAM_NAME} has been deleted.`);
                self.createVideoGroup();
            })
            .catch((err) => {
                console.error('Video Error deleting the stream:', err);
                logger.error('Video Error deleting the stream:', process.env.VIDEO_CALL_STREAM_NAME, err)
            })
            .finally(() => {
                // client.quit();
            });
    }

    public deleteGroup() {
        let self = this;
        this.streamclient.xgroup('DESTROY', process.env.CALL_STREAM_NAME, process.env.CALL_CONSUMER_GROUP)
            .then((response) => {
                console.log(`Stream "${process.env.CALL_STREAM_NAME}" and consumer group "${process.env.CALL_CONSUMER_GROUP}" destroyed.`);
                self.createGroup();

            })
            .catch((err) => {
                console.error('Error destroying stream and consumer group:', err);
                logger.error('Error destroying stream and consumer group:', 'DESTROY', process.env.CALL_STREAM_NAME, process.env.CALL_CONSUMER_GROU, err);
                self.createGroup();
            });
    };


    public deleteConsumer() {
        let self = this;
        this.streamclient.xgroup('DELCONSUMER', process.env.CALL_STREAM_NAME, process.env.CALL_CONSUMER_GROUP, process.env.CALL_CONSUMER_NAME)
            .then((response) => {
                if (response === 1) {
                    console.log(`Consumer "${process.env.CALL_CONSUMER_NAME}" removed from group "${process.env.CALL_CONSUMER_GROUP}"`);

                    self.deleteGroup();
                } else {
                    console.log(`Consumer "${process.env.CALL_CONSUMER_NAME}" not found in group "${process.env.CALL_CONSUMER_GROUP}"`);
                    self.deleteGroup();
                }
            })
            .catch((err) => {
                console.error('Error removing consumer:', err);
                logger.error('Error removing consumer:', 'DELCONSUMER', process.env.CALL_STREAM_NAME, process.env.CALL_CONSUMER_GROUP, process.env.CALL_CONSUMER_NAME, err)
                self.deleteGroup();
            });
    };

    public acknowledgeMessage(stream: any, group: any, messageId: any, messageData: any) {
        try {
            this.streamclient.xack(stream, group, messageId, (err, result) => {
                if (err) {
                    console.error('Error Androidacknowledging message:', err);
                    logger.error('Error Androidacknowledging message:', JSON.stringify({ stream, group, messageId, messageData }), err)
                } else if (result === 1) {

                    console.log(`${stream}_${group}  Acknowledged message ${messageData.iuid}_${messageId}_ACK`);

                } else {
                    console.log(`${stream}_${group}  Message ${messageData.iuid}_${messageId}_ACK not found in consumer group`);
                }
            });
        } catch (error) {
            logger.error('acknowledgeMessage', JSON.stringify({ stream, group, messageId, messageData }), error)
        }
    }

    public readMessages(stream: any, group: any, consumer: any) {
        try {
            let self = this;
            this.streamclient.xreadgroup(
                'GROUP', group, consumer, 'BLOCK', 0, 'STREAMS', stream, '>', (err, streams) => {
                    if (err) {
                        console.error('Error reading messages:', err);
                        logger.error('Error reading messages:', JSON.stringify({ stream, group, consumer }), err);
                    } else {
                        if (streams) {
                            streams.forEach((entry: any) => {
                                const [streamName, messages] = entry;
                                messages.forEach(([messageId, messageData]) => {
                                    console.log(`READ MESSAGE::::: ${stream}_${group} Message ID: ${messageId}, Message Data: ${messageData}`);
                                    const rawMessage = `${messageData}`.split('message,');
                                    self.acknowledgeMessage(stream, group, messageId, JSON.parse(rawMessage[1]));
                                    self.socketadapter.presenceResponse(rawMessage[1]);
                                });
                            });
                        }
                        self.readMessages(stream, group, consumer);
                    }
                }
            );
        } catch (error) {
            logger.error('readMessages', JSON.stringify({ stream, group, consumer }), error)
        }
    }

    public createGroup() {
        try {
            let self = this;
            this.streamclient.xgroup('CREATE', process.env.CALL_STREAM_NAME, process.env.CALL_CONSUMER_GROUP, '0', 'MKSTREAM', (err) => {
                if (err && !err.message.includes('BUSYGROUP Consumer Group name already exists')) {
                    console.error('Error creating consumer group:', err);
                    logger.error('Error creating consumer group:', process.env.CALL_STREAM_NAME, process.env.CALL_CONSUMER_GROUP, err);
                    self.deleteConsumer();
                } else {
                    console.log('START READING MESSAGE::::::');
                    self.readMessages(process.env.CALL_STREAM_NAME, process.env.CALL_CONSUMER_GROUP, process.env.CALL_CONSUMER_NAME);
                }
            });
        } catch (error) {
            logger.error('createGroup', 'CREATE', process.env.CALL_STREAM_NAME, process.env.CALL_CONSUMER_GROUP, '0', 'MKSTREAM')
        }
    };

    // video meet

    public deleteVideoGroup() {
        let self = this;
        this.videoStreamclient.xgroup('DESTROY', process.env.VIDEO_CALL_STREAM_NAME, process.env.VIDEO_CALL_CONSUMER_GROUP)
            .then((response) => {
                console.log(`Stream "${process.env.VIDEO_CALL_STREAM_NAME}" and consumer group "${process.env.VIDEO_CALL_CONSUMER_GROUP}" destroyed.`);
                self.createVideoGroup();

            })
            .catch((err) => {
                console.error('VIDEO Error destroying stream and consumer group:', err);
                logger.error('VIDEO Error destroying stream and consumer group:', 'DESTROY', process.env.VIDEO_CALL_STREAM_NAME, process.env.VIDEO_CALL_CONSUMER_GROUP, err)
                self.createVideoGroup();
            });
    };


    public deleteVideoConsumer() {
        let self = this;
        this.videoStreamclient.xgroup('DELCONSUMER', process.env.VIDEO_CALL_STREAM_NAME, process.env.VIDEO_CALL_CONSUMER_GROUP, process.env.VIDEO_CALL_CONSUMER_NAME)
            .then((response) => {
                if (response === 1) {
                    console.log(`Consumer "${process.env.VIDEO_CALL_CONSUMER_NAME}" removed from group "${process.env.VIDEO_CALL_CONSUMER_GROUP}"`);

                    self.deleteVideoGroup();
                } else {
                    console.log(`Consumer "${process.env.VIDEO_CALL_CONSUMER_NAME}" not found in group "${process.env.VIDEO_CALL_CONSUMER_GROUP}"`);
                    self.deleteVideoGroup();
                }
            })
            .catch((err) => {
                console.error('VIDEO Error removing consumer:', err);
                logger.error('VIDEO Error removing consumer:', 'DELCONSUMER', process.env.VIDEO_CALL_STREAM_NAME, process.env.VIDEO_CALL_CONSUMER_GROUP, process.env.VIDEO_CALL_CONSUMER_NAME, err)
                self.deleteVideoGroup();
            });
    };

    public acknowledgeVideoMessage(stream: any, group: any, messageId: any, messageData: any) {
        try {
            this.videoStreamclient.xack(stream, group, messageId, (err, result) => {
                if (err) {
                    console.error('VIDEO Error Androidacknowledging message:', err);
                } else if (result === 1) {

                    console.log(`${stream}_${group}  Acknowledged message ${messageData.iuid}_${messageId}_ACK`);

                } else {
                    console.log(`${stream}_${group}  Message ${messageData.iuid}_${messageId}_ACK not found in consumer group`);
                }
            });
        } catch (error) {
            logger.error('acknowledgeVideoMessage', JSON.stringify({ stream, group, messageId, messageData }), error)
        }
    }

    public readVideoMessages(stream: any, group: any, consumer: any) {
        try {
            let self = this;
            this.videoStreamclient.xreadgroup(
                'GROUP', group, consumer, 'BLOCK', 0, 'STREAMS', stream, '>', (err, streams) => {
                    if (err) {
                        console.error('VIDEO Error reading messages:', err);
                        logger.error('VIDEO Error reading messages:', JSON.stringify({ stream, group, consumer }), err);
                    } else {
                        if (streams) {
                            streams.forEach((entry: any) => {
                                const [streamName, messages] = entry;
                                messages.forEach(([messageId, messageData]) => {
                                    console.log(`READ VIDEO MESSAGE::::: ${stream}_${group} Message ID: ${messageId}, Message Data: ${messageData}`);
                                    const rawMessage = `${messageData}`.split('message,');
                                    self.acknowledgeVideoMessage(stream, group, messageId, JSON.parse(rawMessage[1]));
                                    self.socketadapter.inviteVideoRequest(rawMessage[1]);
                                });
                            });
                        }
                        self.readVideoMessages(stream, group, consumer);
                    }
                }
            );
        } catch (error) {
            logger.error('readVideoMessages', JSON.stringify({ stream, group, consumer }), error)
        }
    }

    public createVideoGroup() {
        try {
            let self = this;
            this.videoStreamclient.xgroup('CREATE', process.env.VIDEO_CALL_STREAM_NAME, process.env.VIDEO_CALL_CONSUMER_GROUP, '0', 'MKSTREAM', (err) => {
                if (err && !err.message.includes('BUSYGROUP Consumer Group name already exists')) {
                    console.error('VIDEO Error creating consumer group:', err);
                    logger.error('VIDEO Error creating consumer group:', 'CREATE', process.env.VIDEO_CALL_STREAM_NAME, process.env.VIDEO_CALL_CONSUMER_GROUP, '0', 'MKSTREAM', err);
                    self.deleteVideoConsumer();
                } else {
                    console.log('START VIDEO READING MESSAGE::::::');
                    self.readVideoMessages(process.env.VIDEO_CALL_STREAM_NAME, process.env.VIDEO_CALL_CONSUMER_GROUP, process.env.VIDEO_CALL_CONSUMER_NAME);
                }
            });
        } catch (error) {
            logger.error('createVideoGroup', 'CREATE', process.env.VIDEO_CALL_STREAM_NAME, process.env.VIDEO_CALL_CONSUMER_GROUP, '0', 'MKSTREAM', error)
        }
    };

    public getredisAdapter(): RedisAdapter {
        return this.adaptor;
    }
}
