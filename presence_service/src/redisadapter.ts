/**
 * @createdBy <Jayaselva M>
 * @createdOn
 */

import { redisClient, redisStore, tokenRedisStore } from "./db";
import { configs } from "./config";
import * as jwt from 'jsonwebtoken';
import { PresenceAdapters } from "./presence";
import { Server } from 'socket.io';
import bluebird from 'bluebird';
import { createState } from "./dao";
import { logger } from "./log";

export class IOServer {
    private io: any;
    private redisadaptor: PresenceAdapters;

    constructor(httpServer: any) {
        this.io = new Server(httpServer, {
            path: '/presence-service',
            cors: {
                origin: '*',
                credentials: true,
                methods: ["GET", "POST"]
            },
            pingInterval: 25000,
            pingTimeout: 60000,
        });
        this.connectionhandler();
        this.validateSession();
        this.redisadaptor = new PresenceAdapters(this);
        this.addAdaptor();
        bluebird.promisifyAll(redisClient);
    }

    private validateSession(): any {

        this.io.use((socket: any, next: any) => {
            if (!socket?.handshake?.auth?.token) {
                next();
                return;
            }
            const socketData: any = jwt.decode(socket.handshake.auth.token);
            try {
                if (!socketData.sipLoginId || !socketData.domainId || !socketData.deviceId || !socket.handshake.auth.token) {
                    const error_report = {
                        message: 'Unauthorized- Query param missing',
                        code: 403,
                    };
                    next(new Error(JSON.stringify(error_report)));
                } else {
                    try {
                        const decoded: any = jwt.verify(socket.handshake.auth.token, 'sscret');
                        if (parseInt(decoded.sipLoginId, 10) === parseInt(socketData.sipLoginId, 10)) {
                            tokenRedisStore
                                .get(`${decoded.sipLoginId}_${decoded.source}`)
                                .then((res: string) => {
                                    if (res !== null) {
                                        const checkToken = JSON.parse(res ?? 'null');
                                        if (checkToken?.token === socket.handshake.auth.token) {
                                            next();
                                        } else {
                                            const error_report = {
                                                message: 'Unauthorized',
                                                code: 403,
                                            };
                                            next(new Error(JSON.stringify(error_report)));
                                        }
                                    } else {
                                        const error_report = {
                                            message: 'Unauthorized',
                                            code: 403,
                                        };
                                        next(new Error(JSON.stringify(error_report)));
                                    }
                                })
                                .catch((err: any) => {
                                    const error_report = {
                                        message: 'Unauthorized - Token not exist',
                                        code: 403,
                                    };
                                    next(new Error(JSON.stringify(error_report)));
                                });
                        } else {
                            const error_report = {
                                message: 'Unauthorized - sipLoginId mismatch',
                                code: 403,
                            };
                            next(new Error(JSON.stringify(error_report)));
                        }
                    } catch (e: any) {
                        const error_report = {
                            message: 'Unauthorized - Invalid signature',
                            code: 403,
                        };
                        logger.error('Unauthorized - Invalid signature', socket?.handshake?.auth?.token, e)
                        next(new Error(JSON.stringify(error_report)));
                    }
                }
            } catch (e: any) {
                const error_report = {
                    message: 'Unauthorized',
                    code: 403,
                };
                logger.error('Unauthorized', socket?.handshake?.auth?.token, e)
                next(new Error(JSON.stringify(error_report)));
            }
        });
    }

    private addAdaptor(): void {
        this.io.adapter(this.redisadaptor.getredisAdapter());
    }

    private connectionhandler(): void {
        this.io.on("connection", async (socket: any) => {
            const decode: any = jwt.decode(socket.handshake.auth.token)
            const id = socket.handshake.auth.id;
            if (id && id !== '') {
                const domainId = id?.split('_')
                socket.join(id);
                socket.join(`${domainId[0]}`)
            }

            socket.on('presence', async (msg: any) => {
                try {
                    console.log('presence :::::::', msg);
                    const data = {
                        ext: msg?.ext,
                        domainName: msg?.domainName,
                        deviceType: msg?.deviceType,
                        presence: msg?.presence ?? 0,
                        domainId: msg?.domainId,
                    };
                    const chatPresence = `${data.ext}@${data.domainId}@presence_chat`;
                    console.log('chatPresence :::::::', chatPresence, data);
                    if (['CCAAS:BROWSER', 'CCAAS:IOS', 'CCAAS:AND'].includes(data?.deviceType)) {
                        if (data?.presence === 1) {
                            await tokenRedisStore.set(`${data?.ext}@${data?.domainName}@${data.deviceType}@presence`, 1);
                            //CHAT
                            data.domainId && await tokenRedisStore.set(chatPresence, JSON.stringify(data));
                        } else if (data?.presence === 0) {
                            await tokenRedisStore.del(`${data?.ext}@${data?.domainName}@${data.deviceType}@presence`);
                            //CHAT
                            data.domainId && await tokenRedisStore.set(chatPresence, JSON.stringify(data));
                        }
                    }
                } catch (error) {
                    logger.error('presence', msg, error)
                }
            });

            socket.on('request_check_login_status', async (data: any) => {
                try {
                    if (data?.domainId && data?.ext && data?.timeStamp) {
                        this.io.of('/').to(`${data?.domainId}_${data?.ext}`).emit('received_check_login_status', data?.timeStamp);
                    }
                } catch (error) {
                    logger.error('request_check_login_status', data, error)
                }
            });

            socket.on('updatestate_backend', async (data: any) => {
                try {
                    console.log('EMITTING EVENT FOR STATE UPDATE: ', data);
                    if (data?.statusName && data?.statusName?.trim() !== '') {
                        const presence = {
                            ext: data?.ext,
                            statusName: data?.statusName,
                            UserName: data?.userName ?? '',
                            isVoice: data?.isVoice,
                            isVoiceCallTransfer: data?.isVoiceCallTransfer,
                            domainId: data?.domainId,
                            email: data?.email ?? '',
                            userId: `${data?.userId}`,
                            agentStatus: data.agentStatus,
                            skill: data?.skill ?? [],
                            routingMethod: data?.routingMethod ?? 0,
                            inboundCall: data?.inboundCall ?? 0,
                            outboundCampaignCall: data?.outboundCampaignCall ?? 0,
                            is_outbound_campaign_specific_agent: data?.is_outbound_campaign_specific_agent ?? 0,
                            roleId: data?.roleId ?? 0,
                            forced_presence: data?.forced_presence ?? 0,
                            quId: data?.quId ?? 0,
                            queueName: data?.queueName ?? '',
                        }
                        console.log("RESPONSE CHANNEL NAME::::", configs.responseChannel)
                        // const channel =TOKEN_REDIS_STORE_IP
                        const channelName = await this.redisChannel(data?.domainId)
                        console.log('getRedisChannel: updatestate_backend', channelName);

                        redisClient.publish(`${channelName}`, JSON.stringify(presence));
                        await redisStore.hset(`${data.domainId}_presence`, `${data?.domainId}_${data?.ext}`, JSON.stringify({ ...data, currentTime: Date.now() }));
                    }
                } catch (error: any) {
                    console.log(error);
                    logger.error('updatestate_backend', data, error)
                }
            });

            socket.on('disconnect', async (reason: any) => {
                try {
                    let deviceType;
                    switch (decode?.source) {
                        case "Web":
                            deviceType = "CCAAS:BROWSER"
                            break;
                        case "IOS":
                            deviceType = "CCAAS:IOS"
                            break;
                        case "Android":
                            deviceType = "CCAAS:AND"
                            break;
                        default:
                            deviceType = ""
                            break;
                    }
                    console.log("Decode ::::", decode);
                    console.log(`reason :::: ${reason} :::: deviceType ::: ${deviceType}`)
                    if (deviceType !== '') {
                        let presenceDetails = {
                            domainId: decode.domainId,
                            isVoice: 0,
                            isVoiceCallTransfer: 0,
                            isChat: 0,
                            isChatTransfer: 0,
                            isEmail: 0,
                            isEmailTransfer: 0,
                            stateTime: Date.now(),
                            statusName: 'Offline',
                            ext: decode.ext,
                            email: decode.Email,
                            userId: 0,
                            routingProfileId: 0,
                            userName: decode.username,
                            roleId: decode.role_id,
                            agentStatus: 0
                        }
                        console.log('socket disconnect', `${decode?.ext}@${decode?.domain_name}@${deviceType}@presence`)
                        await tokenRedisStore.del(`${decode.ext}@${decode.domain_name}@${deviceType}@presence`);
                        if (decode?.source === 'Web') {
                            await redisStore.hset(`${decode.domainId}_presence`, `${decode?.domainId}_${decode?.ext}`, JSON.stringify({ ...presenceDetails, currentTime: Date.now() }));
                            const response: any = await redisStore.hmget(`${decode?.domainId}_presence`, `${decode?.domainId}_${decode?.ext}`)
                            const result = Object.values(response).map((value: any) => JSON.parse(value));
                            if (result?.length > 0 && result?.[0] !== null) {
                                const presence = {
                                    ext: result?.[0].ext,
                                    statusName: 'Offline',
                                    UserName: decode.profileName ?? '',
                                    isVoice: 0,
                                    isVoiceCallTransfer: 0,
                                    domainId: result[0].domainId,
                                    email: result?.[0]?.email ?? '',
                                    userId: `${result[0].userId}`,
                                    agentStatus: 1,
                                    skill: result?.[0]?.skill ?? [],
                                    routingMethod: result?.[0]?.routingMethod ?? 0
                                }
                                console.log("RESPONSE CHANNEL NAME::::", configs.responseChannel)
                                const channelName = await this.redisChannel(result[0]?.domainId)
                                console.log('getRedisChannel: updatestate_backend', channelName);
                                redisClient.publish(`${channelName}`, JSON.stringify(presence));
                                await createState({ ext: presence.ext, domainId: presence?.domainId, startTimestamp: Date.now() })
                            }
                        }
                    }
                    this.io.of('/').to(id).emit('presence_disconnect', id);
                    socket.disconnect();
                } catch (err) {
                    socket.to(`${socket.id}`).emit('error', err);
                    logger.error('disconnect', decode, reason, err)
                    return;
                }

            });
        })
    }

    public async presenceResponse(data: any): Promise<void> {
        try {
            const parsedData = JSON.parse(data ?? null);
            if (parsedData) {
                console.log('PRESENCE RESPONSE FROM UMN::::::::::', parsedData);
                this.io.of('/').to(`${parsedData?.domainId}_${parsedData?.ext}`).emit('updatedstatus', parsedData);
                this.io.of('/').to(`${parsedData?.domainId}`).emit('userStatusUpdated', { email: parsedData?.email ?? '', domainId: parsedData?.domainId, status: parsedData?.statusName ?? 'Offline' });
                const response: any = await redisStore.hmget(`${parsedData?.domainId}_presence`, `${parsedData?.domainId}_${parsedData?.ext}`)
                const result = Object.values(response).map((value: any) => JSON.parse(value));
                if (result?.length > 0 && result[0] !== null) {
                    const details = {
                        status: parsedData?.statusName,
                        ext: result[0].ext,
                        domainId: result[0].domainId,
                        statusName: parsedData?.statusName,
                        isChat: parsedData?.statusName !== 'Ready' ? 0 : 1,
                    }
                    console.log('presenceResponse :::::::', { ...result[0], ...details, currentTime: Date.now() })
                    await redisStore.hset(`${details.domainId}_presence`, `${details?.domainId}_${details?.ext}`, JSON.stringify({ ...result[0], ...details, currentTime: Date.now() }));
                }
            }
        } catch (error) {
            logger.error('presenceResponse', data, error)
        }
    }

    public inviteVideoRequest(data: any): void {
        try {
            const parsedData = JSON.parse(data ?? 'null');
            if (parsedData) {
                console.log('VIDEO CALL RESPONSE FROM UMN::::::::::', parsedData);
                this.io.of('/').to(`${parsedData?.domain_id}_${parsedData?.ext}`).emit('inviteVideoRequest', parsedData);
            }
        } catch (error) {
            logger.error('inviteVideoRequest', data, error)
        }
    }

    public async redisChannel(domainId: any): Promise<string> {
        try {
            let channel = '';
            const data = await tokenRedisStore.get(`PRESENCE_ACD_CHANNEL_${domainId}`);
            const domain = typeof domainId === 'number' ? domainId : parseInt(domainId, 10);

            console.log('getRedisChannel:', data);

            if (data !== 'null' && data) {
                channel = data;
            } else if (domain % 2 === 0) {
                channel = process.env.PRESENCE_CHANNEL_ONE;
            } else {
                channel = process.env.PRESENCE_CHANNEL_TWO;
            }

            logger.trace('getRedisChannel', channel, data, domain)
            return channel;
        } catch (error) {
            logger.error('redisChannel', domainId, error)
        }
    }

}



