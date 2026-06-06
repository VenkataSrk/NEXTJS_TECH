import bluebird from 'bluebird';
import * as jwt from 'jsonwebtoken';
import ShortUniqueId from 'short-unique-id';
import { Server, Socket } from 'socket.io';
import { v4 as uuidv4 } from 'uuid';
import { ChatFlowAdapters } from '../chatflow2.0';
import { checkAvailableAgents, deleteSessionDetails, sendQueueUser, updateSessionDetails } from '../chatflow_helpers';
import { configs } from '../config/app';
import { isJSON, updateInactivity } from '../constants';
import { getCustomerChatHistory, getDomainStatus, getIncomingChatDetailsDB, getTransferAgentStatus, insertIncomingChatsDB, updateMessageDao } from '../dao/callflow.dao';
import {
  agentExtSocketIdDelete,
  agentExtSocketIdMapping,
  getAgentDetails,
  getAllUserList,
  getConcurrentChat,
  getCustomerDetails,
  getCustomerInfo,
  getLeadCRMinformation,
  getMessageWebChat,
  getUserListByExt,
  insertNotificationDao,
  insertUpdateConcurrentChat,
} from '../dao/chat.dao';
import { getAgentListByDomianId } from '../dao/dispositionStatus';
import { internalChatReadStatus, newInternalChatMsg } from '../dao/internalChat';
import { AgentAssistEmailSms, AgentAssistSocialMedia, configurationDetails, configurationEmailDetails, configurationFbDetails, configurationInstaDetails, getCompanyDetails, getCustomerCoIdInsta, getDeclineBot, getInstagramConfigData, getKnowledgeBaseId, sendCallbackRequestInsta, sendMessageInstagram, updateAgentDetailsInsta, updateCallbackStatusInsta, updateChatMessageInsta, updateIsBotInsta, updateMissedChatInsta } from '../dao/socialmedia';
import { audioMsg, chatDeflectionData, createChatSession, getAgentAssistMessageDtl, getAgentStatus, getBotDetail, getCustomerCoId, getCustomerCoIdFb, getDispositinStatus, getDurationHistoryIdDao, getExt, getFacebookConfigData, getQueueWaitTime, getQueueWaitTimeFacebook, getQueueWaitTimeInsta, getQueueWaitTimeWhatsapp, getSupervisiorList, getTransferAgent, getWebchatLiveDashborad, insertUpdateAgentAssist, InsertUpdateAIChatMessage, insertUpdateChatIntentDao, insertUpdateIrrelevantQaDao, insertUpdateQueueWaitTimeDao, insertUpdateQueueWaitTimeWhatsapp, insertUpdateVisitorDao, sendCallbackRequestFb, sendmailTranscript, sendMessageFb, sentimentalScore, sentimentalScoreDataFB, setTransferChatMessage, updateAgentDetails, updateAgentDetailsdata, updateAgentDetailsFB, updateAssistMessage, UpdateCallAnsweredDurationDao, updateCallbackStatus, updateCallbackStatusFb, updateChatDeflectionChat, updateChatMessage, updateChatMessageFb, updateDispositionBot, updateDispositionBotFB, updateDispositionDiscnnect, updateDispositionDiscnnectFB, updateFacebookMessage, updateIsBot, updateIsBotFb, updateMissedChat, updateMissedChatFb, updateSentimentScore, updateSentimentScoreChat, updateSentimentScoreFB, updateTiggerStatus, updateWhatsappChatMessage, updateWhatsappMessage } from '../dao/webChat.dao';
import { sendCallbackRequest, sendConnectLiveAgent, sendConnectLiveAgentFB, sendConnectLiveAgentFBNotification, sendConnectLiveAgentInsta, sendConnectLiveAgentInstaNotification, sendConnectLiveAgentNotification, sendMessage, sendMessageFacebook, sentimentalScoreData } from '../fetch_api';
import { transferChat, transferChatToAvailableAgent } from '../helperFunctions/whatsappTransfer';
import { insertQueuedTimeForChat } from '../helpers/constants';
import { agentStatusQueueData, ioredisChat, logOutPubInstance, logOutSubInstance, nlp_sub, NLPRedisClient, otherRedisClient, presenceStore, pubsubRedisClient, redisClient, redisClientAuth, redisForPrensence } from '../plugins/db';
import { logger } from '../plugins/log';
import { RedisAdapters } from '../redisAdapters';
import { Aichat } from '../redisAdapters/aichat';
import { Aidashboard } from '../redisAdapters/aidashboard';
import { AgentQueueManager } from './agentQueueManager';
import { blpopQueuedCustomerByStatus, chatQueueByAgentStatusHandler } from './chat_queue';

export class IOServer {
  private static sessionInstances: any;
  private io: any;
  private redisadaptor: RedisAdapters;
  private aichat;
  private aidashboard;
  constructor(httpServer: any) {

    this.io = new Server(httpServer, {
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
    this.redisadaptor = new RedisAdapters(this);
    this.aichat = new Aichat(this);
    this.aidashboard = new Aidashboard(this);
    this.addAdaptor();
    this.addAuthMiddleware();
    bluebird.promisifyAll(redisClient);
    this.io.emit('Test', 'USER');
    console.log('CALLED');
  }

  public receiveMessageFromNLP(data: any): void {
    const parsedData = JSON.parse(data ?? null);
    if (parsedData) {
      console.log('receiveMessageFromNLP parsedData >>>>>>>', parsedData);
      this.io.of('/').to(parsedData.sessionId).emit('airesponse', parsedData);
      logger.info(`receiveMessageFromNLP Socket Error: ${JSON.stringify(parsedData)}`);
    }
  }
  public receiveMessageFromTemplateResponse(data: any): void {
    const parsedData = JSON.parse(data ?? null);
    if (parsedData) {
      console.log('receiveMessageFromTemplateResponse parsedData >>>>>>>', parsedData);
      this.io.of('/').to(parsedData.sessionId).emit('airesponse', parsedData);
      logger.info(`receiveMessageFromTemplateResponse Socket Error: ${JSON.stringify(parsedData)}`);

    }
  }
  public receiveMessageFromWebscrape(data: any): void {
    const parsedData = JSON.parse(data ?? null);
    if (parsedData) {
      console.log('receiveMessageFromWebscrape parsedData >>>>>>>', parsedData);
      this.io.of('/').to(`${parsedData?.domainId}_${parsedData?.ext}`).emit('webscrapresponse', parsedData);
      logger.info(`receiveMessageFromWebscrape Socket Error: ${JSON.stringify(parsedData)}`);
    }
  }

  public receiveMessageFromLiteResponse(data: any): void {
    const parsedData = JSON.parse(data ?? null);
    if (parsedData) {
      console.log('receiveMessageFromLiteResponse parsedData >>>>>>>', parsedData);
      this.io.of('/').to(parsedData.sessionId).emit('airesponse', parsedData);
      logger.info(`receiveMessageFromLiteResponse Socket Error: ${JSON.stringify(parsedData)}`);
    }
  }

  public receiveMessageFromAIDashboardResponse(data: any): void {
    const parsedData = JSON.parse(data ?? null);
    if (parsedData) {
      console.log('Receive AI Dashboard Message from Nlp :', parsedData);
      this.io.of('/').to(parsedData.fromId).emit('ai_dashboard_response', parsedData);
      logger.info(`Receive AI Dashboard Message from Nlp : ${JSON.stringify(parsedData)}`);
    }
  }

  public receiveMessageFromAIChatResponse(data: any): void {
    const parsedData = JSON.parse(data ?? null);
    if (parsedData) {
      console.log('Receive AI chat Message from Nlp :', parsedData);
      this.io.of('/').to(parsedData.fromId).emit('ai_chat_response', parsedData);
      logger.info(`Receive AI chat Message from Nlp : ${JSON.stringify(parsedData)}`);
    }
  }

  public receiveMessageFromAgentAssitResponse(data: any): void {
    const parsedData = JSON.parse(data ?? null);
    if (parsedData) {
      parsedData.messageId = uuidv4();
      try {
        // InsertUpdateAIChatMessage(parsedData);
      } catch (err) {
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
    logger.info(`receiveMessageFromAgentAssitResponse Socket Error: ${JSON.stringify(parsedData)}`);
  }

  // public receiveMessageFromAgentAssist(data: any): void {
  //   console.log('receiveMessageFromAgentAssist', data);
  //   const parsedData = JSON.parse(data ?? null);
  //   if (parsedData) {
  //     console.log('receiveMessageFromAgentAssist parsedData >>>>>>>', parsedData);
  //     this.io.of('/').to(parsedData.sessionId).emit('agentassist', parsedData);
  //   }
  // }
  public receiveMessageFromAgentAssist(data: any): void {
    const parsedData = JSON.parse(data ?? null);
    if (parsedData) {
      parsedData.messageId = uuidv4();
      try {
        InsertUpdateAIChatMessage(parsedData);
      } catch (err) {
        console.log('err:::::', err);
      }
      console.log('receiveMessageFromAgentAssist parsedData >>>>>>>', parsedData.summary);
      this.io.of('/').to(parsedData.to).emit('agentassist', parsedData);
      if (parsedData?.summary) {
        const test: any = JSON.parse(parsedData.data);
        test.message = parsedData.summary;
        const data1: any = {
          ChatHistoryId: test?.ChatHistoryId,
          dispositionId: test?.dispositionId,
          Summary: parsedData?.summary,
          ChatSubject: test?.ChatSubject ?? ''
        };
      }
      logger.info(`receiveMessageFromAgentAssist Socket Error: ${JSON.stringify(parsedData)}`);
    }
  }
  public close(): void {
    if (this.io) {
      this.io.close();
    }
  }

  private addAdaptor(): void {
    this.io.adapter(this.redisadaptor.getredisAdapter());
  }

  private addAuthMiddleware(): void {
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
            const decoded: any = jwt.verify(socket.handshake.auth.token, configs.jwtsecret);
            if (parseInt(decoded.sipLoginId, 10) === parseInt(socketData.sipLoginId, 10)) {
              this.redisadaptor
                .getValuefromredis(`${decoded.sipLoginId}_${decoded.source}`)
                .then((res: string) => {
                  if (res !== null && res !== undefined) {
                    const checkToken = JSON.parse(res ?? 'null');
                    console.log(
                        'checkToken?.token \n', checkToken?.token ,
                        '\n socket.handshake \n', socket.handshake.auth.token,
                        checkToken?.token === socket.handshake.auth.token
                    );
                    if (checkToken?.token === socket.handshake.auth.token) {
                      next();
                    } else {
                      const error_report = {
                        message: 'Unauthorized1',
                        code: 403,
                      };
                      next(new Error(JSON.stringify(error_report)));
                    }
                  } else {
                    const error_report = {
                      message: 'Unauthorized2',
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
            next(new Error(JSON.stringify(error_report)));
          }
        }
      } catch (e: any) {
        const error_report = {
          message: 'Unauthorized',
          code: 403,
        };
        next(new Error(JSON.stringify(error_report)));
      }
    });
  }

  private handleChatSessions(data: any, socket: any) {
    const parsedData: any = JSON.parse(data ?? 'null');
    if (parsedData?.btnValue && parsedData?.btnValue?.includes('transferToQueue')) {
      // this.redisadaptor.BLPOP('', parsedData.domain_id, parsedData);
    }
    if (parsedData?.call_state === 'init') {
      const newObject = new ChatFlowAdapters(this);
      IOServer.sessionInstances[parsedData?.session_id] = newObject;
      const exit = newObject?.ChatFlowHandler(data);
      console.log('FLOW IS NOW EXITING 1', exit);
    } else if (parsedData?.disconnectedBy) {
      const exit = IOServer.sessionInstances[parsedData?.session_id]?.ChatFlowHandler(data);
      console.log('FLOW IS NOW EXITING 2', exit);
      IOServer.sessionInstances[parsedData?.session_id] = null;
      delete IOServer.sessionInstances[parsedData?.session_id];
      try {
        socket.disconnect(true);
      } catch (err) {
        console.log('Error disconnecting socket:', err);
      }
    } else {
      const newObject = new ChatFlowAdapters(this);
      console.log('FLOW IS NOW EXITING 3');
      // const exit = IOServer.sessionInstances[parsedData?.session_id]?.ChatFlowHandler(data);
      const exit = newObject?.ChatFlowHandler(data);
    }
  }

  private connectionhandler(): void {
    this.io.on('connection', async (socket: Socket) => {
      console.log(`User connected: ${socket.id}`);

      socket.on('agent_ext_socket_id_mapping', (data: any) => {
        console.log('agent_ext_socket_id_mapping', data, socket.id);
        agentExtSocketIdMapping(data, socket.id);
      });

      socket.on('disconnect', async (reason: string) => {
        try {
          console.log(`User disconnected: ${socket.id}, Reason: ${reason}`);
          logger.info(`Disconnect Socket Log:::: ${socket.handshake.auth.id} >>> ${JSON.stringify(reason)}`);

          const ignoredReasons = ['ping timeout', 'transport close', 'client namespace disconnect', 'transport error'];
          if (ignoredReasons.includes(reason)) {
            logger.info(`Skipping decline workflow due to auto disconnect: ${reason}`);
            return; // 🔥 stops auto decline
          }

          const agentDetails: any = await getAgentDetails(socket.id);
          if (agentDetails[0]?.domainId && agentDetails[0]?.ext) {
            const concurrentChat: any = await getConcurrentChat(agentDetails[0]);
            if (concurrentChat?.[0]?.agentOccupyDetails) {
              const temp = JSON.parse(concurrentChat?.[0]?.agentOccupyDetails ?? 'null') ?? '';
              temp.currentChatCount = { ...temp?.currentChatCount, [agentDetails?.[0]?.ext]: 0 };
              const value = { ...concurrentChat?.[0], agentOccupyDetails: JSON.stringify(temp) };
              const updateConcurrentLimit = await insertUpdateConcurrentChat(value);
              console.log('updateConcurrentLimit', updateConcurrentLimit);
              if (updateConcurrentLimit?.[0]?.errCode === 0) {
                agentExtSocketIdDelete(socket.id);
              }
            }
          }
          const domainId = socket.handshake.auth.id.split('_')?.[0];
          const ext = socket.handshake.auth.id.split('_')?.[1];
          const token: any = await redisClientAuth.get(`LOGGED_IN_USER_TOKEN_${domainId}_${ext}_Web`);
          if (token === socket.handshake.auth.token) {
            updateInactivity(domainId, ext, true);
          }
        } catch (err) {
          logger.info(`Disconnect Socket Log:::: ${socket.handshake.auth.id} >>> ${JSON.stringify(reason)}`);
          logger.info(`Disconnect Socket Log:::: err ${err}`);
          console.log('disconnect:::::::::', err);
        }
      });

      if (socket?.handshake?.auth?.id) {
        const email = socket.handshake.auth.id.split('_')[0];
        socket.join(`${email}`);
      }

      /** INTERNAL CHAT */

      socket.on('internal_chat_msg', async (data: any) => {
        try {
          if (data.internalChatId === null) {
            const chatRes = await newInternalChatMsg(data);
            if (Array.isArray(chatRes) && chatRes?.length > 0) {
              this.io.of('/').to(data?.agentUserId).emit('internal_chat_msg_recevie', { ...data, internalChatId: chatRes[0]?.internalChatId });
            }
          } else if (data?.internalChatId !== null) {
            const chatReadStatusRes = await internalChatReadStatus(data);
            if (Array.isArray(chatReadStatusRes) && chatReadStatusRes?.length > 0) {
              this.io.of('/').to(data?.agentUserId).emit('internal_chat_msg_status', { ...data, readStatus: chatReadStatusRes });
            }
          }
          logger.info(`internal_chat_msg Socket Error: ${JSON.stringify(data)}`);
        } catch (error) {
          console.log('internal_chat_msg', error);
        }
      });

      // *** For WebChat Connection ***//

      socket.on('joinWebChat', async (room: any) => {
        console.log('joinwebchat>>>>>>>>>>>>>>>>>>>>>>>>>>>', room);
        socket.join(`${room.webChatSessionId}`);
      });

      // socket.on('queue_handling', async (data: any) => {
      //   console.log('queue_handling data', data);
      //   this.io.of('/').to(data?.to).emit('requestDecline', data);
      //   // data.domain_id = data?.domainId;
      //   // const readyAgentsData: any = await readyAgents(data); // checking agents in ready state
      //   // console.log('\nreadyAgentsData------->', JSON.stringify(readyAgentsData));
      //   // updateSessionDetails(data);
      // });

      socket.on('updateRoutingProfile', async (data: any) => {
        console.log('\n\nupdateRoutingProfile----------->', data);
        const response: any = await otherRedisClient.hmget(`${data.domain_id}_presence`, `${data.domain_id}_${data.ext}`);
        const result = Object.values(response).map((value: any) => JSON.parse(value));
        if (result?.length > 0 && result[0] !== null) {
          const ss = await otherRedisClient.hset(`${data.domain_id}_presence`, `${data?.domain_id}_${data?.ext}`, JSON.stringify({ ...result[0], routingProfileId: data?.routing_profile }));
          const sendData = { ext: data?.ext, domainId: data?.domain_id, id: data?.routing_profile };
          this.io.of('/').to(`${data?.domain_id}`).emit('updateAgentRoutingProfile', sendData);
        } else {
          console.log('updateRoutingProfile else case');
        }
        logger.info(`internal_chat_msg Socket Error: ${JSON.stringify(data)}`);
      });

      socket.on('agent_ready_for_queue_users', async (data: any) => {
        try {
          // Use new AgentQueueManager instead of old JSON.stringify approach
          const enqueued = await AgentQueueManager.enqueue(data);

          if (!enqueued) {
            logger.error(`Failed to enqueue - queue may be full. ext:${data.ext}, domainId:${data.domainId}`);

            // Get queue metrics for debugging
            const metrics = await AgentQueueManager.getHealthMetrics();
            logger.error(`Queue metrics: ${JSON.stringify(metrics)}`);
          }

          logger.info(`agent_ready_for_queue_users: ${JSON.stringify(data)}`);
        } catch (err) {
          logger.error(`Error in agent_ready_for_queue_users handler: ${err}`);
        }
      });

      socket.on('botConnectedTime', async (room: any) => {
        const data: any = {
          ChatHistoryId: room?.ChatHistoryId ?? null,
          StartTime: Math.floor(Date.now()),
          IsBot: 1,
          BotId: room?.botId ?? 1,
        };
        try {
          const botStartTime = await updateAgentDetails(data);
          this.io.of('/').to(room?.session_id).emit('botConnectedResponse', botStartTime[0]);
        } catch (err) {
          console.log(err);
        }
        logger.info(`botConnectedTime Socket Error: ${JSON.stringify(room)}`);
      });

      socket.on('botDisconnectTime', async (room: any) => {
        const data: any = {
          EndTime: Date.now(),
          ChatDurationHistoryId: room?.ChatDurationHistoryId ?? null,
          ChatHistoryId: room?.ChatHistoryId ?? null,
          transferredTime: Math.floor(Date.now()),
          dispositionId: room?.dispositionId ?? null
        };
        try {
          await updateAgentDetails(data);
          const nlpData: any = {
            HistoryId: room?.ChatHistoryId,
            ChatDurationHistoryId: room?.ChatDurationHistoryId,
            channelType: 'Chat',
            domainId: room?.domainId,
            ext: room?.ext ?? 0
          };
          const updateScore: any = await sentimentalScore(nlpData);
          if (updateScore?.status === 200) {
            await updateSentimentScoreChat(nlpData, updateScore?.data);
          } else {
            console.error('Failed to update sentiment score:', updateScore?.status);
          }
          logger.info(`botDisconnectTime Socket Error: ${JSON.stringify(room)}`);
        } catch (err) {
          console.log(err);
        }
      });

      socket.on('presence', async (msg: any) => {
        console.log('presence :::::::', msg);
        const data = {
          ext: msg?.ext,
          domainName: msg?.domainName,
          deviceType: msg?.deviceType,
          presence: msg?.presence ?? 0
        };
        if (data?.presence === 1) {
          await presenceStore.set(`${data?.ext}@${data?.domainName}@${data.deviceType}@presence`, data.presence);
        } else if (data?.presence === 0) {
          await presenceStore.del(`${data?.ext}@${data?.domainName}@${data.deviceType}@presence`);
        }
        logger.info(`presence Socket Error: ${JSON.stringify(msg)}`);
      });

      socket.on('join_to_live_agent', async (room: any) => {
        console.log('listener join_to_live_agent>>>>>>>>>>>>>>>>>>>>>>>>>>>', room);
        socket.join(`${room.webChatSessionId}`);
        socket.join(`${room.domainId}_${room.ext}`);
        const payload = {
          agentId: `${room.domainId}_${room.ext}`,
          userId: `${room.webChatSessionId} `,
          sender: room?.sender,
          ChatHistoryId: room?.ChatHistoryId,
          cookiesId: room?.cookiesId,
          status: 'Connection establish successfully',
          transferType: room?.transferType ?? 0,
          primaryAgent: room?.primaryAgent ?? 0,
          ChatDurationHistoryId: room?.ChatDurationHistoryId,
          chatPlanId: room?.chatPlanId,
          queueInformation: room?.queueInformation,
          queueId: room?.queueId,
          queueSettings: room?.queueSettings,
          widgetName: room?.widgetName,
          channelType: 'Chat'
        };
        this.io
          .of('/')
          .to([`${room.webChatSessionId}`, `${room.domainId}_${room.ext}`])
          .emit('webChatConnected', payload);
        otherRedisClient.publish('webChatConnected_mob_FCM', JSON.stringify(payload));
        console.log('webChatConnected_mob_FCM payload>>', payload);
        const responseData: any = await getCustomerChatHistory(room.ChatHistoryId);
        const datares: any = Array?.isArray(responseData) && JSON.parse(responseData[0][0]?.Message ?? null);
        logger.info(`presence Socket Error: ${JSON.stringify(room)}`);
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
      });

      socket.on('chat_message_update', async (data: any) => {
        this.handleChatSessions(data, socket);
        logger.info(`chat_message_update Socket Error: ${JSON.stringify(data)}`);
      });
      socket.on('updateDispostionChatStatus', async (room) => {
        const messageList: any = { message: room };
        try {
          const msgRes = {
            ChatHistoryId: room?.ChatHistoryId,
            ChatDurationHistoryId: room?.ChatDurationHistoryId,
            from: `${room.domainId}_${room.ext}`,
            message: "Thank you for contacting us. If you have any other queries in future or need assitance, please don't hesitate to reach out.Have a great day!",
            messageType: 'Text',
            sessionId: `${room.domainId}_${room.ext}`,
            to: room?.sessionId,
            sender: room?.sender,
            sendAt: Math.floor(Date.now())
          };
          this.io.of('/').to(`${room?.domainId}`).emit('chat_ended_for_tracker_emitter', msgRes);
          const nlpData: any = {
            HistoryId: room?.ChatHistoryId,
            ChatDurationHistoryId: room?.ChatDurationHistoryId,
            channelType: 'Chat',
            domainId: room?.domainId,
            type: 'sentimentAnalyze',
            ext: room?.ext ?? 0
          };
          const updateScore: any = await sentimentalScore(nlpData);
          if (updateScore?.status === 200) {
            await updateSentimentScoreChat(nlpData, updateScore?.data);
          } else {
            console.error('Failed to update sentiment score:', updateScore?.status);
          }
          // if (room?.dispostionStatus !== 'Transferred') {
          this.io.of('/').to(room?.sessionId).emit('agentDisconnected', msgRes);
          // }
          const getMessage: any = await getMessageWebChat(room?.ChatHistoryId);
          const parsedMessage = JSON.parse(getMessage[0][0]?.Message ?? '[]') ?? [];
          // if(data?.type === "Chat"){
          const historyMessage = {
            type: 'summary',
            messages: parsedMessage,
            sessionId: `${room.domainId}_${room.ext}`,
            ChatDurationHistoryId: room?.ChatDurationHistoryId,
            userId: `${room?.sessionId}`,
            agentId: `${room?.domainId}_${room.ext}`,
            ChatHistoryId: room?.ChatHistoryId?.toString(),
            data: JSON.stringify(room)
          };
          this.redisadaptor.publishMessageToAgentAssist(JSON.stringify(historyMessage));
          const data1: any = {
            ChatHistoryId: room?.ChatHistoryId,
            ChatDurationHistoryId: room?.ChatDurationHistoryId,
            dispositionId: room?.dispositionId,
            EndTime: Math.floor(Date.now()),
            transferedBy: room?.transferedBy,
            transferedTo: room?.transferedTo,
            Duration: room?.Duration
          };
          try {
            await updateAgentDetails(data1);
            logger.info(`updateDispostionChatStatus Socket Error: ${JSON.stringify(room)}`);
          } catch (err) {
            console.log(err);
          }
        } catch (err) {
          console.log(err);
        }
      });
      socket.on('store_customer_data', async (data: any) => {
        await createChatSession(data);
        const dispostionRes: any = await getDispositinStatus(data);
        let transferredRes: any;
        dispostionRes[0]?.map((list: any) => {
          if (list?.dispositionName === 'Transferred') {
            return transferredRes = list;
          }
        });
        this.io.of('/').to(data?.sessionId).emit('get_domain_dispostionId', transferredRes);
      });

      socket.on('OneToOneChatSend', async (msg: any) => {
        let messages: any;
        const messageList: any = { message: msg };
        if (msg?.isCustomer === true) {
          messages = { userText: messageList };
        } else if (msg?.isagentname === true) {
          messages = { agentNameVisible: messageList };
        } else if (msg?.TimeOutStatus === true) {
          messages = { timeOutVisible: messageList };
        } else if (msg?.messageType === 'carousel') {
          messages = { carouselText: messageList };
        } else {
          messages = { agentText: messageList };
        }
        const stringifiedMessage: any = { message: messages };
        stringifiedMessage.ChatHistoryId = msg.ChatHistoryId;
        stringifiedMessage.ChatDurationHistoryId = msg?.ChatDurationHistoryId;
        // if(msg?.supervisors == undefined) {
        //   msg.supervisors = ["7179_201"];
        // }
        try {
          await updateChatMessage(stringifiedMessage);
        } catch (err) {
          console.log('updateChatMessageupdateChatMessage', err);
        }
        if (msg.isLiveagent && !msg.agentIntro && msg?.messageType.toLowerCase() === 'text') {
          // let sessionId: any = msg.from;
          // if (msg.isCustomer) {
          //   sessionId = msg.to;
          // }
          // msg.sessionId = sessionId;
          msg.ChatHistoryId = msg.ChatHistoryId?.toString();
          msg.by = 'agent';
          if (msg?.assistType === 'Convo') {
            // this.redisadaptor.publishMessageToAgentAssistForConvo(JSON.stringify(msg));
            this.redisadaptor.agentAssistForConvoXadd(JSON.stringify(msg));
          } else {
            this.redisadaptor.agentAssistXadd(JSON.stringify(msg));
            // this.redisadaptor.publishMessageToAgentAssist(JSON.stringify(msg));
          }
        }
        msg.sessionId = msg?.socketId || msg?.to;
        this.io.of('/').to(msg?.to).emit('OneToOneChatReceive', msg);
        this.io.of('/').to(msg?.supervisorList).emit('supervisor_trackerview_chat', msg);
        logger.info(`OneToOneChatSend Socket Error: ${JSON.stringify(msg)}`);
      });

      socket.on('typingText', async (data) => {
        this.io.of('/').to(`${data.domainId}_${data.ext}`).emit('customerTyping', data);
      });

      socket.on('OneToOneChatSendTyping', (data: any) => {
        this.io.of('/').to(data?.to).emit('typingBotText', data);
      });

      socket.on('agentTypingStatusEmitter', (data: any) => {
        this.io.of('/').to(data?.to).emit('agentTypingStatusListener', data);
      });

      socket.on('unResponsiveQuesAns', async (data) => {
        try {
          await insertUpdateIrrelevantQaDao(data);
        } catch (err) {
          console.log(err);
        }
      });
      socket.on('sendEmailTranscript', async (data) => {
        try {
          await sendmailTranscript(data);
        } catch (err) {
          console.log(err);
        }
      });

      socket.on('visitorCloseTabSite', (data) => {
        if (data?.visitorId) {
          const visitorData = {
            domainId: parseInt(data?.domainId, 10),
            visitorId: parseInt(data?.visitorId, 10),
            sessionEndTimed: data?.sessionEndTime
          };
          insertUpdateVisitorDao(visitorData);
        }
      });

      socket.on('markAsSeen', async (data) => {
        this.io.of('/').to(`${data.domainId}_${data.ext}`).emit('markAsSeen', data);
      });

      socket.on('getSupervisorExt', async (data) => {
        const superVisorList = await getSupervisiorList(data);
        this.io.of('/').to(data?.to).emit('supervisor_ext_response', superVisorList);
      });

      socket.on('agentDeclineRequest', async (data) => {
        this.io.of('/').to(data?.to).emit('requestDecline', data);
        logger.info(`agentDeclineRequest Socket Error: ${JSON.stringify(data)}`);
      });

      socket.on('changeAttendChatStatus', async (data) => {
        try {
          if (data?.queueId) {
            const listed = {
              qid: data?.queueId,
              domainId: data?.domainId
            };
            const getQueueList = await getQueueWaitTime(listed);
            const parsedData = typeof getQueueList?.[0]?.queueList === 'string'
              ? (() => {
                try {
                  return JSON.parse(getQueueList?.[0]?.queueList);
                } catch (error) {
                  return [];
                }
              })()
              : [];
            const resultData = parsedData?.map((list: any) => {
              if (list?.sessionId === data?.sessionId) {
                list.chatNotifyStatus = false;
              }
              return list;
            });
            const payload = {
              qid: getQueueList?.[0]?.qid ?? 0,
              queueName: getQueueList?.[0]?.queueName,
              queueType: 'chat',
              domainId: getQueueList?.[0]?.domainId,
              queueList: JSON.stringify(resultData)
            };
            try {
              await insertUpdateQueueWaitTimeDao(payload);
            } catch (err) {
              console.log(err);
            }
          }
        } catch (error) {
          console.log('changeAttendChatStatus>>>>>', error);
        }
      });

      socket.on('agentNotAttendEmitter', async (data) => {
        this.io.of('/').to(data?.to).emit('agentNotAttendEmitter', data);
      });

      socket.on('update_callback_data', async (chatID: any) => {
        console.log('chatID', chatID);
        const data: any = {
          ChatHistoryId: chatID?.ChatHistoryId,
          CallBackStatus: 1,
          sessionEndTime: Math.floor(Date.now()),
          CustomerId: chatID?.coid
        };
        console.log('chatID?.coid', chatID?.coid);
        if (chatID?.coid) {
          const notification = {
            uuid: uuidv4(),
            toExt: data.ext,
            notificationMsg: 'Chat Callback received from',
            channelType: 'Chat',
            markAsRead: 0,
            type: 'Chat CALLBACK',
            timeStamp: new Date().getTime(),
            coid: chatID?.coid,
            customerValue: null,
            domainId: chatID?.domainId
          };
          console.log('CHAT CALLBACK', notification);
          insertNotificationDao(notification);
          ioredisChat.to(`${chatID?.domainId}`).emit('refreshNotification');
        }
        const data1 = await createChatSession(data);
        this.io.of('/').to(`${chatID?.domainId}`).emit('chatCallbackEmitter');
      });

      socket.on('socialMediaAgentDeclineRequest', async (data) => {
        console.log('socialMediaAgentDeclineRequest', data);
        const getDeclineData = await getDeclineBot(data);
        const socialMedia: any = data?.isTransferChat === 1 ? await getUserListByExt(getDeclineData) : await transferChat(data, data?.currentNode);
        if (socialMedia?.length > 0) {
          console.log('sendInvite', `${socialMedia[0]?.domainId}_${socialMedia[0]?.ext}`);
          this.io.of('/').to(`${socialMedia[0]?.domainId}_${socialMedia[0]?.ext}`).emit('sendInvite', { ...data, ...socialMedia[0] });
        } else {
          if (data?.channelType === 'Whatsapp') {
            const getCallbck = await getBotDetail(data);
            if (getCallbck[0]?.callBackToggle === 1) {
              const getCoid = await getCustomerCoId(data);
              const redirectUrlCallbck = `WhatsappWebcallback?coId=${getCoid[0]?.coid}&domainId=${data?.domainId}&phoneNumber=${data?.customerNumber}&historyId=${data?.historyId}`;
              await sendCallbackRequest(data, redirectUrlCallbck);
              await updateCallbackStatus(data);
              let message: any = {};
              try {
                message = {
                  messageID: uuidv4(),
                  sessionId: data?.sessionId,
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
                const messageList: any = { message };
                const messageType: any = { agentText: messageList };
                const stringifiedMessage: any = { message: messageType };
                stringifiedMessage.historyId = data?.historyId;
                await updateWhatsappChatMessage(stringifiedMessage);
                updateIsBot(data?.historyId);
              } catch (error) {
                console.log(error);
              }
            } else {
              const getMessage = await sendMessage(data);
              let message: any = {};
              try {
                message = {
                  messageID: uuidv4(),
                  sessionId: data?.sessionId,
                  message: getMessage?.content,
                  sendAt: Date.now(),
                  messageType: 'text',
                  pdfurl: '',
                  isBot: true,
                  isCustomer: false,
                  fileType: '',
                  direction: 'OUT',
                  isLike: 0
                };
                const messageList: any = { message };
                const messageType: any = { agentText: messageList };
                const stringifiedMessage: any = { message: messageType };
                stringifiedMessage.historyId = data?.historyId;
                await updateWhatsappChatMessage(stringifiedMessage);
                updateIsBot(data?.historyId);
              } catch (error) {
                console.log(error);
              }
              await updateMissedChat(data);
              const sentimentSupervisorBot: any = await getSupervisiorList(data);
              sentimentSupervisorBot.forEach((ext: any) => {
                console.log('sentimentWhatsappBot', `${data?.domainId}_${ext?.ext}`);
                this.io.of('/').to(`${data?.domainId}_${ext?.ext}`).emit('supervisiorActivityBot');
              });
            }
          } else if (data?.channelType === 'Facebook') {
            const getCallbckFb = await getFacebookConfigData(data);
            if (getCallbckFb[0]?.callBackToggle === 1) {
              const getCoid = await getCustomerCoIdFb(data);
              const redirectUrlCallbck = `FacebookWebcallback?coId=${getCoid[0]?.coid}&domainId=${data?.domainId}&facebookId=${data?.customerNumber}&historyId=${data?.historyId}`;
              await sendCallbackRequestFb(data, redirectUrlCallbck);
              await updateCallbackStatusFb(data);
              let message: any = {};
              try {
                message = {
                  messageID: uuidv4(),
                  sessionId: data?.sessionId,
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
                const messageList: any = { message };
                const messageType: any = { agentText: messageList };
                const stringifiedMessage: any = { message: messageType };
                stringifiedMessage.historyId = data?.historyId;
                await updateChatMessageFb(stringifiedMessage);
                updateIsBotFb(data?.historyId);
              } catch (error) {
                console.log(error);
              }
            } else {
              await sendMessageFb(data);
              let message: any = {};
              try {
                message = {
                  messageID: uuidv4(),
                  sessionId: data?.sessionId,
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
                const messageList: any = { message };
                const messageType: any = { agentText: messageList };
                const stringifiedMessage: any = { message: messageType };
                stringifiedMessage.historyId = data?.historyId;
                await updateChatMessageFb(stringifiedMessage);
                updateIsBotFb(data?.historyId);
              } catch (error) {
                console.log(error);
              }
              await updateMissedChatFb(data);
              const sentimentSupervisorBot: any = await getSupervisiorList(data);
              sentimentSupervisorBot.forEach((ext: any) => {
                console.log('sentimentWhatsappBot', `${data?.domainId}_${ext?.ext}`);
                this.io.of('/').to(`${data?.domainId}_${ext?.ext}`).emit('supervisiorActivityBot');
              });
            }
          } else {
            const getCallbckInsta = await getInstagramConfigData(data);
            const getCoid = await getCustomerCoIdInsta(data);
            if (getCallbckInsta[0]?.callBackToggle === 1) {
              const redirectUrlCallbck = `InstagramWebcallback?coId=${getCoid[0]?.coid}&domainId=${data?.domainId}&instagramId=${data?.customerNumber}&historyId=${data?.historyId}`;
              await sendCallbackRequestInsta(data, redirectUrlCallbck);
              await updateCallbackStatusInsta(data);
              let message: any = {};
              try {
                message = {
                  messageID: uuidv4(),
                  sessionId: data?.sessionId,
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
                const messageList: any = { message };
                const messageType: any = { agentText: messageList };
                const stringifiedMessage: any = { message: messageType };
                stringifiedMessage.historyId = data?.historyId;
                await updateChatMessageInsta(stringifiedMessage);
                updateIsBotInsta(data?.historyId);
              } catch (error) {
                console.log(error);
              }
            } else {
              await sendMessageInstagram(data);
              let message: any = {};
              try {
                message = {
                  messageID: uuidv4(),
                  sessionId: data?.sessionId,
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
                const messageList: any = { message };
                const messageType: any = { agentText: messageList };
                const stringifiedMessage: any = { message: messageType };
                stringifiedMessage.historyId = data?.historyId;
                await updateChatMessageInsta(stringifiedMessage);
                updateIsBotInsta(data?.historyId);
              } catch (error) {
                console.log(error);
              }
              await updateMissedChatInsta(data);
              const sentimentSupervisorBot: any = await getSupervisiorList(data);
              sentimentSupervisorBot.forEach((ext: any) => {
                console.log('sentimentWhatsappBot', `${data?.domainId}_${ext?.ext}`);
                this.io.of('/').to(`${data?.domainId}_${ext?.ext}`).emit('supervisiorActivityBot');
              });
            }
          }
        }
        logger.info(`socialMediaAgentDeclineRequest Socket Error: ${JSON.stringify(data)}`);
      });

      socket.on('agentAcceptRequest', async (room) => {
        console.log('agentAcceptRequest >>>>>>>', room);
        try {
          this.io.of('/').to(room?.to).emit('requestAccept', room);
          if (room?.transferType === 1) {
            const transferData: any = {
              ChatHistoryId: room?.ChatHistoryId ?? 0,
              Istransferred: room?.Istransferred
            };
            // await createChatSession(transferData);
            this.io.of('/').to(room?.primaryAgent).emit('transferChatAccepted', room);
            logger.info(`agentAcceptRequest :: ${JSON.stringify(room)}`);
          }
        } catch (err) {
          console.log(err);
        }
      });

      socket.on('deleteChatSession', async (data: any) => {
        console.log('deleteChatSession data', data);
        deleteSessionDetails(data);
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
      });

      socket.on('socialMediaAgentAcceptRequest', async (room) => {
        console.log('socialMediaAgentAcceptRequest >>>>>>>', room);
        if (room?.channelType === 'Whatsapp') {
          const data: any = {
            sessionId: room?.sessionId,
            WhatsappHistoryId: room?.historyId ?? 0,
            AgentId: room?.agentId,
            IsBot: 0,
            agentExt: room?.agentExt,
            dialDuration: room?.dialDuration,
            IsMissed: room?.IsMissed ?? 0,
          };
          try {
            // await sendConnectLiveAgentNotification(room);
            // sendLiveNotification(room);
            await sendConnectLiveAgent(room);
            let message: any = {};
            try {
              message = {
                messageID: uuidv4(),
                sessionId: room?.sessionId,
                message: `Your conversation has been transferred to ${room?.agentName}`,
                sendAt: Date.now(),
                messageType: 'text',
                pdfurl: '',
                isBot: true,
                isCustomer: false,
                fileType: '',
                direction: 'OUT',
                isLike: 0
              };
              const messageList: any = { message };
              const messageType: any = { agentText: messageList };
              const stringifiedMessage: any = { message: messageType };
              stringifiedMessage.historyId = room?.historyId;
              await updateWhatsappChatMessage(stringifiedMessage);
            } catch (error) {
              console.log(error);
            }
            await updateAgentDetailsdata(data);
          } catch (err) {
            console.log(err);
          }
        } else if (room?.channelType === 'Facebook') {
          const data: any = {
            sessionId: room?.sessionId,
            FacebookHistoryId: room?.historyId ?? 0,
            AgentId: room?.agentId,
            IsBot: 0,
            agentExt: room?.agentExt,
            domainId: room?.domainId,
            dialDuration: room?.dialDuration ?? null
          };
          try {
            // await sendConnectLiveAgentFBNotification(room);
            // sendLiveNotificationFb(room);
            await sendConnectLiveAgentFB(room);
            let message: any = {};
            try {
              message = {
                messageID: uuidv4(),
                sessionId: room?.sessionId,
                message: `Your conversation has been transferred to ${room?.agentName}`,
                sendAt: Date.now(),
                messageType: 'text',
                pdfurl: '',
                isBot: true,
                isCustomer: false,
                fileType: '',
                direction: 'OUT',
                isLike: 0
              };
              const messageList: any = { message };
              const messageType: any = { agentText: messageList };
              const stringifiedMessage: any = { message: messageType };
              stringifiedMessage.historyId = room?.historyId;
              await updateChatMessageFb(stringifiedMessage);
            } catch (error) {
              console.log(error);
            }
            await updateAgentDetailsFB(data);
          } catch (err) {
            console.log(err);
          }
        } else {
          const data: any = {
            sessionId: room?.sessionId,
            InstagramHistoryId: room?.historyId ?? 0,
            AgentId: room?.agentId,
            IsBot: 0,
            agentExt: room?.agentExt,
            domainId: room?.domainId,
            dialDuration: room?.dialDuration ?? null
          };
          try {
            // await sendConnectLiveAgentInstaNotification(room);
            // sendLiveNotificationInsta(room);
            await sendConnectLiveAgentInsta(room);
            let message: any = {};
            try {
              message = {
                messageID: uuidv4(),
                sessionId: room?.sessionId,
                message: `Your conversation has been transferred to ${room?.agentName}`,
                sendAt: Date.now(),
                messageType: 'text',
                pdfurl: '',
                isBot: true,
                isCustomer: false,
                fileType: '',
                direction: 'OUT',
                isLike: 0
              };
              const messageList: any = { message };
              const messageType: any = { agentText: messageList };
              const stringifiedMessage: any = { message: messageType };
              stringifiedMessage.historyId = room?.historyId;
              await updateChatMessageInsta(stringifiedMessage);
            } catch (error) {
              console.log(error);
            }
            await updateAgentDetailsInsta(data);
          } catch (err) {
            console.log(err);
          }
        }
        logger.info(`socialMediaAgentAcceptRequest Socket Error: ${JSON.stringify(room)}`);
      });

      socket.on('socialMediaAgentAssistDetails', async (data: any) => {
        try {
          let nlpRes: any;
          const comDetails = await getCompanyDetails(data);
          nlpRes = await AgentAssistSocialMedia(data, comDetails);
          if (!nlpRes?.data) {
            console.error('AgentAssistSocialMedia returned invalid response:', nlpRes);
            return;
          }
          nlpRes.data.channelId = data?.agentNumber;
          nlpRes.data.messageId = uuidv4();
          nlpRes.data.timeStamp = new Date().getTime();
          console.log('agent assist emit>>>>>>', `${data?.domainId}_${data?.agentExt}`);
          this.io.of('/').to(`${data?.domainId}_${data?.agentExt}`).emit('agentAssistOmniChannel', nlpRes.data);
        } catch (error) {
          console.error('Error in socialMediaAgentAssistDetails handler:', error);
        }
      });

      socket.on('emailSmsAgentAssistDetails', async (data: any) => {
        try {
          let botDetails: any;
          let websiteId: any;
          let nlpRes: any;
          if (data?.channelType === 'Email') {
            botDetails = await configurationEmailDetails(data, 1);
            websiteId = await getKnowledgeBaseId(data.domainId, botDetails[0]?.knowledgeBasedId);
          } else {
            botDetails = await configurationEmailDetails(data, 2);
            websiteId = await getKnowledgeBaseId(data.domainId, botDetails[0]?.knowledgeBasedId);
          }
          data.messageId = uuidv4();
          nlpRes = await AgentAssistEmailSms(data, websiteId[0]);
          if (!nlpRes?.data) {
            console.error('AgentAssistSocialMedia returned invalid response:', nlpRes);
            return;
          }
          nlpRes.data.messageId = nlpRes.data.messageId || data.messageId;
          nlpRes.data.channelId = data?.agentNumber;
          nlpRes.data.timeStamp = new Date().getTime();
          console.log('agent assist emit>>>>>>', `${data?.domainId}_${data?.agentExt}`);
          this.io.of('/').to(`${data?.domainId}_${data?.agentExt}`).emit('agentAssistOmniChannel', nlpRes.data);
        } catch (error) {
          console.error('Error in emailSmsAgentAssistDetails handler:', error);
        }
      });

      socket.on('agentDisconnectedTime', async (room) => {
        const data: any = {
          ChatHistoryId: room?.ChatHistoryId ?? 0,
          ChatDurationHistoryId: room?.ChatDurationHistoryId ?? 0,
          EndTime: Math.floor(Date.now()),
        };
        try {
          await updateAgentDetails(data);
          logger.info(`agentDisconnectedTime Socket Error: ${JSON.stringify(room)}`);
        } catch (err) {
          console.log(err);
        }
      });

      socket.on('requestAgentStatus', async (msg: any) => {
        try {
          const getDomainResult = await getDomainStatus(msg?.domain_id);
          this.io.of('/').to(msg?.session_id).emit('responseAgentStatus', { domainStatus: getDomainResult, result: msg });
        } catch (err) {
          console.log(err);
        }
      });

      socket.on('closeRingingChat', async (data: any) => {
        try {
          this.io.of('/').to(`${data?.domainId}`).emit('ringingChatCloseEmitter', data);
        } catch (err) {
          console.log(err);
        }
      });

      socket.on('sessionTimeOut', async (data) => {
        let messages: any;
        const messageList: any = { message: data };
        messages = { timeOutVisible: messageList };
        const getMessage: any = await getMessageWebChat(data?.sessionId);
        let newMessage: any;
        if (!getMessage[0]?.message) {
          newMessage = [messages];
        } else {
          const parsedMessage = JSON.parse(getMessage[0]?.message ?? '[]') ?? [];
          if (parsedMessage?.length > 0) {
            parsedMessage.push(messages);
            newMessage = parsedMessage;
          }
        }
        const stringifiedMessage = { message: newMessage, chatSession: 1, sessionEndTime: Date.now(), agentDisconnectTime: Date.now() };
        // createSession(stringifiedMessage, data?.sessionId);
      });

      socket.on('updateThumbsUp', async (data: any) => {
        const responseData: any = await getCustomerChatHistory(data?.ChatDurationHistoryId);
        const messageArr: any = [];
        const datares: any = Array?.isArray(responseData) && JSON.parse(responseData[0][0]?.Message ?? null);
        datares?.length > 0 &&
          datares?.map((msg: any) => {
            if (data?.messageID === msg?.agentText?.message?.messageID) {
              msg.agentText.message.isLike = data?.isLike;
              msg.agentText.message.feedBack = data?.feedBack;
            }
            messageArr.push(msg);
          });
        try {
          await updateMessageDao(data, messageArr);
        } catch (err) {
          console.log(err);
        }
      });

      socket.on('refershLiveTracker', async () => {
        this.io.emit('getLiveTrackStatus');
      });

      socket.on('chatAcceptedDeviceType', (data: any) => {
        this.io.of('/').to(data?.to).emit('chatAcceptedDeviceTypeListener', data);
        logger.info(`chatAcceptedDeviceType Socket Error: ${JSON.stringify(data)}`);
      });

      socket.on('dequeueUser', async (data: any) => {
        if (data?.qid) {
          const getQueueData: any = await getQueueWaitTime(data);
          const queueResult = getQueueData?.[0]?.queueList && JSON.parse(getQueueData?.[0]?.queueList);
          const insertData = queueResult?.filter((list: any) => list?.sessionId !== data?.sessionId);
          const matchData = queueResult?.filter((list: any) => list?.sessionId === data?.sessionId);
          const payload = {
            qid: getQueueData?.[0]?.qid ?? 0,
            queueName: getQueueData?.[0]?.queueName,
            queueType: 'chat',
            domainId: getQueueData?.[0]?.domainId,
            queueList: JSON.stringify(insertData)
          };
          try {
            await insertUpdateQueueWaitTimeDao(payload);
            insertQueuedTimeForChat(matchData?.length > 0 ? matchData : {});
          } catch (err) {
            console.log(err);
          }
          this.io.emit('callQueueWait', payload);
        }
        logger.info(`dequeueUser Socket Error: ${JSON.stringify(data)}`);
      });

      socket.on('chatDeclinedDeviceType', (data: any) => {
        this.io.of('/').to(data?.to).emit('chatDeclinedDeviceTypeListener', data);
        logger.info(`chatDeclinedDeviceType Socket Error: ${JSON.stringify(data)}`);
      });

      socket.on('removeChatQueueUser', async (data: any) => {
        try {
          const list = {
            qid: data?.removeQueueId,
            domainId: data?.domainId
          };
          const getQueueList = await getQueueWaitTime(list);
          const queueResult = getQueueList?.[0]?.queueList ? JSON.parse(getQueueList?.[0]?.queueList) : [];
          const queueListChanges = queueResult?.filter((_l: any) => _l?.sessionId !== data?.sessionId);
          const matchData = queueResult?.filter((_l: any) => _l?.sessionId === data?.sessionId);
          const payload = {
            qid: getQueueList?.[0]?.qid ?? 0,
            queueName: getQueueList?.[0]?.queueName,
            queueType: 'chat',
            domainId: getQueueList?.[0]?.domainId,
            queueList: JSON.stringify(queueListChanges)
          };
          await insertUpdateQueueWaitTimeDao(payload);
          this.io.emit('callQueueWait', payload);
          insertQueuedTimeForChat(matchData?.length > 0 ? matchData : {});
          logger.info(`removeChatQueueUser Socket Error: ${JSON.stringify(data)}`);
        } catch (err) {
          console.log(err);
        }
      });

      socket.on('removeChatQueueUserSocialmedia', async (data: any) => {
        try {
          if (data?.channelType === 'Whatsapp') {
            const list = {
              qid: data?.removeQueueId,
              domainId: data?.domainId
            };
            const getQueueList = await getQueueWaitTimeWhatsapp(list);
            const queueResult = getQueueList?.[0]?.queueList ? JSON.parse(getQueueList?.[0]?.queueList) : [];
            const queueListChanges = queueResult?.filter((_l: any) => _l?.sessionId !== data?.sessionId);
            const payload = {
              qid: getQueueList?.[0]?.qid ?? 0,
              queueName: getQueueList?.[0]?.queueName,
              queueType: 'whatsapp',
              domainId: getQueueList?.[0]?.domainId,
              queueList: JSON.stringify(queueListChanges)
            };
            await insertUpdateQueueWaitTimeWhatsapp(payload);
            this.io.emit('callQueueWait', payload);
          } else if (data?.channelType === 'Facebook') {
            const list = {
              qid: data?.removeQueueId,
              domainId: data?.domainId
            };
            const getQueueList = await getQueueWaitTimeFacebook(list);
            const queueResult = getQueueList?.[0]?.queueList ? JSON.parse(getQueueList?.[0]?.queueList) : [];
            const queueListChanges = queueResult?.filter((_l: any) => _l?.sessionId !== data?.sessionId);
            const payload = {
              qid: getQueueList?.[0]?.qid ?? 0,
              queueName: getQueueList?.[0]?.queueName,
              queueType: 'facebook',
              domainId: getQueueList?.[0]?.domainId,
              queueList: JSON.stringify(queueListChanges)
            };
            await insertUpdateQueueWaitTimeWhatsapp(payload);
            this.io.emit('callQueueWait', payload);
          } else {
            const list = {
              qid: data?.removeQueueId,
              domainId: data?.domainId
            };
            const getQueueList = await getQueueWaitTimeInsta(list);
            const queueResult = getQueueList?.[0]?.queueList ? JSON.parse(getQueueList?.[0]?.queueList) : [];
            const queueListChanges = queueResult?.filter((_l: any) => _l?.sessionId !== data?.sessionId);
            const payload = {
              qid: getQueueList?.[0]?.qid ?? 0,
              queueName: getQueueList?.[0]?.queueName,
              queueType: 'instagram',
              domainId: getQueueList?.[0]?.domainId,
              queueList: JSON.stringify(queueListChanges)
            };
            await insertUpdateQueueWaitTimeWhatsapp(payload);
            this.io.emit('callQueueWait', payload);
          }
          logger.info(`removeChatQueueUserSocialmedia Socket Error: ${JSON.stringify(data)}`);
        } catch (err) {
          console.log(err);
        }
      });
      socket.on('socialMediaDeclinedDeviceType', (data: any) => {
        this.io.of('/').to(data?.to).emit('socialMediaDeclinedDeviceTypeListener', data);
        logger.info(`socialMediaDeclinedDeviceType Socket Error: ${JSON.stringify(data)}`);
      });
      socket.on('queue_list_update', (data: any) => {
        this.io.emit('queue_name_update', data);
      });

      socket.on('update_end_session', async (data: any) => {
        data.EndTime = Math.floor(Date.now());
        data.sessionEndTime = Math.floor(Date.now());
        console.log('update_end_session_payloadd>>>', data);
        try {
          if (data?.isBot === true) {
            const dispostionRes: any = await getDispositinStatus(data);
            let transferredRes: any;
            dispostionRes[0]?.map((list: any) => {
              if (list?.dispositionName === 'Closed') {
                return transferredRes = list;
              }
            });
            data.dispositionId = transferredRes?.did;
            createChatSession(data);
            updateAgentDetails(data);
            const nlpData: any = {
              HistoryId: data?.ChatHistoryId,
              ChatDurationHistoryId: data?.ChatDurationHistoryId,
              channelType: 'Chat',
              domainId: data?.domainId,
              ext: data?.ext ?? 0
            };
            try {
              const updateScore: any = await sentimentalScore(nlpData);
              if (updateScore?.status === 200) {
                await updateSentimentScoreChat(nlpData, updateScore?.data);

                const deflectionData: any = await chatDeflectionData(nlpData);
                if (deflectionData?.status === 200) {
                  await updateChatDeflectionChat(nlpData, deflectionData?.data?.deflection_count);
                } else {
                  console.error('Failed to get deflection data:', deflectionData?.status);
                }
              } else {
                console.error('Failed to update sentiment score:', updateScore?.status);
              }
            } catch (err) {
              console.error('Error in sentiment analysis or deflection update:', err);
            }
            this.io.of('/').to(data?.domainId).emit('customerbot_end_session', data);
          } else {
            console.log('customer_end_session>>', data);
            this.io.of('/').to(data?.to).emit('customer_end_session', data);
          }
          logger.info(`update_end_session Socket Error: ${JSON.stringify(data)}`);
        } catch (err) {
          console.error('Error during end session update:', err);
        }
      });

      /**************  End Of Webchat   *******************/

      socket.join(socket.handshake.auth.id);
      socket.on('language_switch', async (data: any) => {
        nlp_sub.publish('multi_lang', JSON.stringify(data));
      });

      socket.on('callback_session', async (data: any) => {
        pubsubRedisClient.publish(process.env.RESPONSECHANNEL, JSON.stringify(data));
        console.log('callback_session', data, '`${data?.domain_id}${data?.ext}`', `${data?.domain_id}${data?.ext}`);
        this.io.of('/').to(`${data?.domain_id}_${data?.ext}`).emit('callback_update', data);

        if (data?.status === 'rejected') {
          const body = {
            domainId: data?.domain_id,
            ext: null
          };
          const userDetails: any = await getAllUserList(body);
          if (userDetails.length > 0) {
            const userList = userDetails.filter((i: any) => {
              return i.isVoice === 1;
            });
            if (userList.length === 0) {
              const info = {
                phoneNumber: data?.callbackDid,
                domainId: data?.domain_id
              };
              // const datas: any = await getCustomerDetails(data?.callbackDid);
              const datas: any = await getCustomerInfo(info);
              console.log('getCustomerInfo callback_session', datas);

              const notification = {
                uuid: uuidv4(),
                toExt: data.ext,
                notificationMsg: `Call Callback received from ${datas[0]?.primaryPhoneNumber}`,
                channelType: 'Call',
                markAsRead: 0,
                type: `Call ${data.req_type}`,
                timeStamp: new Date().getTime(),
                coid: datas[0]?.coid,
                customerValue: datas[0]?.primaryPhoneNumber,
                domainId: datas[0]?.domainId
              };
              console.log('CAll CAllBACk', notification);
              await insertNotificationDao(notification);
              this.io.of('/').to(`${data?.domain_id}`).emit('assignActivityListener', { data });
            }
          }
        }
      });

      // live visit tracking
      socket.on('liveTracking', async (data: any) => {
        console.log(data);
        this.io.of('/').to(`${data?.domainId}`).emit('liveTrackingReport', data);
      });
      socket.on('assignActivityEmitter', async (data: any) => {
        const agentExt = data.assignedTo[0];
        this.io.of('/').to(`${data?.domainId}_${agentExt?.ext}`).emit('assignActivityListener', { data });
      });

      socket.on('queue_list_update', (data: any) => {
        this.io.emit('queue_name_update', data);
      });

      socket.on('reAssignActivityEmitter', async (data: any) => {
        const agentExt = data.assignedTo[0];
        this.io.of('/').to(`${data?.domainId}_${agentExt?.ext}`).emit('reAssignActivityListener', { data });
      });

      socket.on('init_transfer', (event: any) => {
        this.redisadaptor.publishMessage(JSON.stringify(event));
      });

      socket.on('call_merge', (event: any) => {
        if (event?.ext) {
          this.io.to(event?.ext?.toString()).emit('callMerged', event);
        }
      });

      socket.on('leave_call_primary', (event: any) => {
        if (event?.ext) {
          for (const [i, agent] of event?.ext.entries()) {
            this.io.to(`${event.domainId}_${agent}`).emit('leave_call', event);
          }
        }
      });

      socket.on('leave_call_Secondary', (event: any) => {
        console.log(event, 'leave call sec');
        if (event?.domainWithExt) {
          for (const [i, agent] of event?.domainWithExt.entries()) {
            console.log(agent, 'agent>>>>');
            this.io.to(`${agent}`).emit('leave_call_by_secondary', event);
          }
        }
      });

      socket.on('biometricsEnroll', (event: any) => {
        this.redisadaptor.publishMessage(JSON.stringify(event));
      });

      socket.on('bwl_switch', (event: any) => {
        this.redisadaptor.publishMessage(JSON.stringify(event));
      });

      socket.on('agent_register_request', (event: any) => {
        this.redisadaptor.publishMessage(JSON.stringify(event));
      });

      socket.on('worktualPurchase', (data: any) => {
        const parsedData = JSON.parse(data ?? null);
        if (parsedData.botType === 'convo') {
          this.redisadaptor.publishMessageToNLPAI(data);
        } else if (parsedData?.isTemplate === 1) {
          this.redisadaptor.publishLiteMessageToNLP(data);
        } else {
          this.redisadaptor.publishMessageToNLP(data);
        }
      });

      socket.on('updatedstatuslist', async (event: any) => {
        if (event?.domainId) {
          const results: any = await getAgentListByDomianId(event);
          const extractedData = results?.map((data: any) => `${data?.domainId}_${data?.ext}`);
          this.io.to(extractedData)?.emit('updatedstatuslist', event);
        }
      });

      socket.on('multi_login_restriction', async (data: any, callBack: any) => {
        try {
          if (typeof callBack !== 'function') {
            console.error('No callback function provided by client');
            return;
          }

          const key = `${data?.domainId}_presence`;
          const inactivityKey = `${data?.domainId}_inactivity`;
          const field = `${data?.domainId}_${data.ext}`;
          const presenceRes = await redisForPrensence.hget(key, field);
          const inactivityRes = await pubsubRedisClient.hget(inactivityKey, field);
          const presence = isJSON(presenceRes) ? JSON.parse(presenceRes) : presenceRes;
          const inactivity = isJSON(inactivityRes) ? JSON.parse(inactivityRes) : inactivityRes;
          console.log('multi_login_restriction Presence status:>>>>>>>>>>>', ['Offline', 'Away'].includes(presence?.statusName), inactivity?.ccaasInactive !== false, inactivity?.ticketingInactive !== false);
          const decoded: any = jwt.verify(socket.handshake.auth.token, configs.jwtsecret);
          this.redisadaptor
            .getValuefromredis(`${decoded.sipLoginId}_${decoded.source}`)
            .then(async (res: string) => {
              if (res) {
                // Away -- temp for login
                if ((['Offline'].includes(presence?.statusName) || inactivity?.ccaasInactive !== false)) {
                  await redisClientAuth.set(`LOGGED_IN_USER_TOKEN_${decoded.domainId}_${decoded.ext}_${decoded.source}`, socket.handshake.auth.token);
                  await logOutPubInstance.publish('worktual-logout-channel_yoo', JSON.stringify({
                    domainId: data?.domainId,
                    ext: data?.ext,
                    loginTime: data?.loginTime,
                    source: data?.source,
                  }));
                  // this.io.of('/').to(field).emit('newLoginUpdate', data);
                  updateInactivity(data?.domainId, data?.ext, false);
                  return callBack({ status: '200', message: 'Success', response: { ...presence, ...inactivity } });
                }  {
                  const checkToken = JSON.parse(res ?? 'null');
                  const token = await redisClientAuth.get(`LOGGED_IN_USER_TOKEN_${decoded.domainId}_${decoded.ext}_${decoded.source}`);
                  const dataForRedis = {
                    ...checkToken,
                    token: token ? token : socket.handshake.auth.token,
                    refreshToken: token ? token : socket.handshake.auth.token,
                  };
                  await redisClientAuth.set(`${decoded.sipLoginId}_${decoded.source}`, JSON.stringify(dataForRedis));
                  return callBack({ status: '404', message: 'Credentials Currently active in Another device!', response: { ...presence, ...inactivity } });
                }
              }
              await logOutPubInstance.publish('worktual-logout-channel_yoo', JSON.stringify({
                domainId: data?.domainId,
                ext: data?.ext,
                loginTime: data?.loginTime,
                source: data?.source,
              }));
              // this.io.of('/').to(field).emit('newLoginUpdate', data);
              updateInactivity(data?.domainId, data?.ext, false);
              return callBack({ status: '200', message: 'Success', response: { ...presence, ...inactivity } });
            });
        } catch (error) {
          console.error('multi_login_restriction error:', error);
          return callBack({ status: '500', message: 'Server error' });
        }
      });

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

      socket.on('waitingQueueUpdate', (data: any) => {
        this.io.of('/').emit('waitingQueueUpdate', data);
      });

      socket.on('reconnect', (message: any) => {
        socket.join(message);
      });

      socket.on('send_chatbot_req', async (data: any) => {
        console.log('send_chatbot_req', data);
        const parsedData = JSON.parse(data ?? null);
        if (parsedData.botType === 'convo') {
          this.redisadaptor.publishMessageToNLPAI(data);
        } else if (parsedData?.isTemplate === 1) {
          this.redisadaptor.publishLiteMessageToNLP(data);
        } else {
          this.redisadaptor.publishMessageToNLP(data);
        }

      });

      socket.on('send_ai_req', async (data: any) => {
        console.log('send_chatbot_req', data);
        this.redisadaptor.publishMessageToNLPAI(JSON.stringify(data));
      });

      socket.on('send_b2c_search_message', async (data: any) => {
        console.log('send_b2c_search_message', data);
        this.redisadaptor.publishMessageToB2c(JSON.stringify(data));
      });

      socket.on('ai_dashboard_query', async (data: any) => {
        console.log('ai_dashboard_query', data);
        // this.aidashboard.sendCcaasDynamicDashRequest(JSON.stringify(data));
        const streamKey = process.env.DYNAMIC_DASH_STREAM_NAME_REQ;
        NLPRedisClient.xadd(streamKey, '*', 'message', (JSON.stringify(data)), (error: any, messageId: any) => {
          if (error) {
            console.error('Error adding entry to stream:', error);
          }
        });
      });

      socket.on('ai_chat_query', async (data: any) => {
        console.log('ai_chat_query', data);
        // this.aichat.sendCcaasAIChatRequest(JSON.stringify(data));
        const streamKey = process.env.AI_CHAT_STREAM_NAME_REQ;
        NLPRedisClient.xadd(streamKey, '*', 'message', (JSON.stringify(data)), (error: any, messageId: any) => {
          if (error) {
            console.error('Error adding entry to stream:', error);
          }
        });
      });

      socket.on('send_visitor_insights_nlp', async (data: any) => {
        console.log('send_visitor_insights_nlp', data);
        this.redisadaptor.sendwebsiteInsightsSteam(JSON.stringify(data));
      });

      socket.on('lead_contact_update', async (data: any) => {
        getLeadCRMinformation(data);
      });

      socket.on('videoMeetDecline', async (data: any) => {
        this.redisadaptor.publishVideoMessage(JSON.stringify(data));
      });

      socket.on('missed_call_dashboard', async (data: any) => {
        const ext = await getExt(data).then((result: any[]) => {
          return result.map((row: { ext: any }) => `${data?.domainId}_${row.ext}`);
        });
        await createChatSession(data).then(async (result) => {
          if (result) {
            const body = {
              startTmpstmp: data?.startTmpstmp ?? null,
              endTmpstmp: data?.endTmpstmp ?? null,
              domainId: data?.domainId ?? null,
              ext: data?.ext ?? null
            };
            const getWebchatChat: any = await getWebchatLiveDashborad(body);
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
            ioredisChat.to(ext).emit('webchatLiveDashboard', webchat);
          }
        });
      });

      socket.on('agent_missed_call_dashboard', async (data: any) => {
        const DestinationChatDurationHistoryId = await updateAgentDetails(data);
        const dupEntry = {
          SourceChatDurationHistoryId: data?.SourceChatDurationHistoryId,
          DestinationChatDurationHistoryId: DestinationChatDurationHistoryId?.[0]?.[0]?.ChatDurationHistoryId
        };
        setTransferChatMessage(dupEntry);
        const body = {
          startTmpstmp: data?.startTmpstmp ?? null,
          endTmpstmp: data?.endTmpstmp ?? null,
          domainId: data?.domainId ?? null,
          ext: data?.ext ?? null
        };
        const getWebchatChat: any = await getWebchatLiveDashborad(body);
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
        ioredisChat.to(`${data?.domainId}_${data?.ext}`).emit('webchatLiveDashboard', webchat);
      });

      socket.on('abandoned_chat_dashboard', async (data: any) => {
        const test: any = updateAgentDetails(data);
        test.then(async (result) => {
          const ext = await getExt(data).then((spdata: any[]) => {
            return spdata.map((row: { ext: any }) => `${data?.domainId}_${row.ext}`);
          });
          if (result) {
            const body = {
              startTmpstmp: data?.startTmpstmp ?? null,
              endTmpstmp: data?.endTmpstmp ?? null,
              domainId: data?.domainId ?? null,
              ext: data?.ext ?? 0
            };
            const getWebchatChat: any = await getWebchatLiveDashborad(body);
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
            ioredisChat.to(ext).emit('webchatLiveDashboard', webchat);
          }
        });
      });

      socket.on('disposition_status_dashboard', async (data: any) => {
        ioredisChat.to(`${data?.domainId}`).emit('updated_webchat_LiveDashboard');
      });

      socket.on('chat_notify_dial_duration', async (data: any) => {
        console.log('chat_notify_dial_duration', data);
        updateAgentDetails(data);
      });

      socket.on('inbound_chat_dashboard', async (data: any) => {
        const adminSupervisorext = await getExt(data).then((result: any[]) => {
          return result?.map((row: { ext: any }) => `${data?.domainId}_${row.ext}`);
        });
        const body = {
          startTmpstmp: data?.startTmpstmp ?? null,
          endTmpstmp: data?.endTmpstmp ?? null,
          domainId: data?.domainId ?? null,
          ext: data?.ext ?? 0
        };
        const getWebchatChat: any = await getWebchatLiveDashborad(body);
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
        ioredisChat.to(adminSupervisorext).emit('webchatLiveDashboard', webchat);
      });

      socket.on('dashboard_chat_callback', async (data: any) => {
        const adminSupervisorext = await getExt(data).then((result: any[]) => {
          return result.map((row: { ext: any }) => `${data?.domainId}_${row.ext}`);
        });
        const body = {
          startTmpstmp: data?.startTmpstmp ?? null,
          endTmpstmp: data?.endTmpstmp ?? null,
          domainId: data?.domainId ?? null,
          ext: data?.ext ?? 0
        };
        const getWebchatChat: any = await getWebchatLiveDashborad(body);
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
        ioredisChat.to(adminSupervisorext).emit('webchatLiveDashboard', webchat);
      });

      socket.on('allviewsRefresh', async (data: any) => {
        ioredisChat.to(`${data?.domainId}`).emit('allviewsRefresh');
      });

      socket.on('chatIntentEvent', async (data) => {
        console.log('Received data:::', data);
        try {
          await insertUpdateChatIntentDao(data);
        } catch (error: any) {
          console.log('err', error);
        }
      });

      socket.on('dashboard_call', async (data: any) => {
        console.log('callLiveDashboard data:::', data);
        ioredisChat.to(`${data}`).emit('callLiveDashboard');
      });

      socket.on('dashboard_email', async (data: any) => {
        ioredisChat.to(`${data}`).emit('emailLiveDashboard');
      });

      socket.on('insert_update_agent_assist', async (data: any) => {
        try {
          if (data?.isUpdate === 1) {
            const id = {
              HistoryId: data?.HistoryId,
              DurationHistoryId: data?.DurationHistoryId,
              domainId: data?.domainId
            };

            const getAssistMessageResponse = await getAgentAssistMessageDtl(id);
            const assistMessagesRaw = getAssistMessageResponse[0][0]?.assistMsg;

            if (assistMessagesRaw) {
              try {
                const assistMessages = JSON.parse(assistMessagesRaw);

                if (data?.AssistMsg?.messageId) {

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
                  const replacedArray = assistMessages?.map((elem: any) => {

                    if (elem.messageId === data?.AssistMsg?.messageId) {

                      if (data?.AssistMsg.isUsed === 1) {
                        return {
                          ...elem,
                          isUsed: 1,
                          // isLike: data?.AssistMsg.isLike || 0,
                          // isDislike: data?.AssistMsg.isDisLike || 0
                        };
                      }
                      if (data?.AssistMsg.isLike === 1) {
                        return {
                          ...elem,
                          isLike: 1,
                          isDislike: 0
                        };
                      }
                      if (data?.AssistMsg.isDislike === 1) {
                        return {
                          ...elem,
                          isDislike: 1,
                          isLike: 0,
                        };
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
                  await insertUpdateAgentAssist(data);

                  return;
                }
              } catch (jsonParseError) {
                console.log('err', jsonParseError);
              }
            }

            return;
          }
          if (data?.ChannelType === 'Call' && data?.HistoryId) {
            data.CallDurationHistoryId = await getDurationHistoryIdDao(data);
          }
          insertUpdateAgentAssist(data);
        } catch (error) {
          console.log('insert_update_agent_assist', error);
        }
      });

      socket.on('dashboard_sms', async (data: any) => {
        ioredisChat.to(`${data}`).emit('smsLiveDashboard');
      });

      socket.on('dashboard_socialmedia', async (data: any) => {
        ioredisChat.to(`${data}`).emit('socialMediaLiveDashboard');
      });

      socket.on('insert_notification_email', async (data: any) => {
        console.log('insert_notification_email>>>>>>>>', data);
        // ioredisChat.to(`${data}`).emit('insertSuccess');
      });

      socket.on('whatsapp_agent_missed_call', async (data: any) => {
        updateAgentDetailsdata(data);
      });

      socket.on('facebook_agent_missed_call', async (data: any) => {
        updateAgentDetailsFB(data);
      });
      socket.on('instagram_agent_missed_call', async (data: any) => {
        updateAgentDetailsInsta(data);
      });
      socket.on('callTransferFrom', async (data: any) => {
        console.log('callTransferFrom:', data);
        ioredisChat.to(`${data?.domainId}_${data?.toExt}`).emit('callTransferDetails', data);
      });

      socket.on('call_answered_duration', (data: any) => {
        console.log('callAnswerDuration:', data);
        setTimeout(async () => {
          await UpdateCallAnsweredDurationDao(data);
        },         10000);
      });

      socket.on('callViaSmsEmail', async (data: any) => {
        try {
          const messages = JSON.stringify(data);
          console.log('callViaSmsEmail:', data);
          const processedData = {
            domainId: data?.domainId,
            sessionId: data?.sessionId,
            message: messages,
            ext: data?.ext
          };
          const response = await audioMsg(processedData);
          console.log('audioMsg response:', response);
        } catch (error) {
          console.error('Error handling callViaSmsEmail event:', error);
        }
      });

      socket.on('insert_Trigger', async (data: any) => {
        try {
          if (data?.ChannelType === 'Chat') {
            await updateAgentDetails(data);
          } else {
            await updateTiggerStatus(data);
          }
          this.io.of('/').to(`${data?.domain_id}`).emit('trigger_emitter', data);

        } catch (error) {
          console.log('trigger_emitter Errorrr', error);
        }
      });

      socket.on('first_assist_msg', async (data: any) => {
        this.redisadaptor.agentAssistXadd(JSON.stringify(data));
      });

      socket.on('agent_sender', async (data: any) => {
        this.io.of('/').to(`${data?.domainId}`).emit('admin_receiver', data?.bcHistoryId);
      });

      socket.on('update_logout_cross_domain', async () => {
        const data = {
          domainId: socket.handshake.auth.id.split('_')?.[0],
          ext: socket.handshake.auth.id.split('_')?.[1],
        };
        await logOutPubInstance.publish('worktual-logout-channel_yoo', JSON.stringify(data));
      });

      socket.on('update_inactivity_presence', async (isInactive: boolean) => {
        const domainId = socket.handshake.auth.id.split('_')?.[0];
        const ext = socket.handshake.auth.id.split('_')?.[1];
        updateInactivity(domainId, ext, isInactive);
      });

      socket.on('video_channel_agent_assist', (msg: any) => {
        this.redisadaptor.agentAssistForVideoChannelXadd(JSON.stringify(msg));
      });

      socket.on('update_thumbsup_Feedback_chat', async (data: any) => {
        const responseData: any = await getCustomerChatHistory(data?.ChatDurationHistoryId);
        const messageArr: any = [];
        const datares: any = Array?.isArray(responseData) && JSON.parse(responseData[0][0]?.Message ?? null);
        datares?.length > 0 &&
          datares?.map((msg: any) => {
            if (data?.messageID === msg?.userText?.message?.messageID) {
              msg.userText.message.isLike = data?.isLike;
              msg.userText.message.feedBack = data?.feedBack;
            }
            messageArr.push(msg);
          });
        try {
          await updateMessageDao(data, messageArr);
        } catch (err) {
          console.log(err);
        }
      });

      socket.on('customer_update_ai', async (data: any) => {
        console.log(data);
        this.io.of('/').to(`${data?.domainId}`).emit('customerContactEmitter', data);
      });

      socket.on('socialMedia_disposition_send', async (data: any) => {
        console.log(data);
        this.io.of('/').to(`${data?.domainId}`).emit('socialMedia_disposition_receive', data);
      });

      socket.on('admin_to_agent_deactivate', async (data: any) => {
        console.log('admin_to_agent_deactivate>>>>', data);
        ioredisChat.to(`${data.domainId}_${data.ext}`).emit('force_logout', data);
      });

      const gettargetedges = (edgeid, edge) => {
        return edge.filter((list: any) => {
          if (list.source === edgeid) {
            return list;
          }
        });
      };
    });
  }
}
