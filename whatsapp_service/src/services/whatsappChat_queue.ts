
import {
  getAllQueueWaitTime,
  getPersonalDetails,
  getQueueSettingS,
  getQueueWaitTime,
  insertUpdateQueueWaitTimeDao
} from '../dao/whatsapp';

import { channelType, emitter } from '../helpers/constants';
import { logger } from '../plugins/log';
import { ioredisWhatsapp, ioredisWhatsappQueue } from '../plugins/redis';
import { insertCustomerInQueue, transferChat } from './transferToQueue';

// handle queue function for whatsapp
export const chatQueueHandler = () => {
  try {
    setInterval(async () => {
      const redisQueueData: any = await ioredisWhatsappQueue.get(process.env.WHATSAPP_QUEUE);
      if (redisQueueData) {
        const redisData = JSON.parse(redisQueueData);
        if (redisData?.length > 0) {
          redisData?.map(async (list: any) => {
            const getQueueList = await getQueueWaitTime(list);
            const queueResult = typeof getQueueList?.[0]?.queueList === 'string' ? (() => {
              try {
                return JSON.parse(getQueueList?.[0]?.queueList);
              } catch (error) {
                return [];
              }
            })() : [];
            if (queueResult?.length > 0) {
              let agentData: any = [];
              queueResult?.map(async (data: any, index: any) => {
                const getPersonalData: any = await getPersonalDetails(data?.channelId.customerNumber, data?.channelId.domainId);
                const agentDetailRes = await transferChat(data?.channelId, data?.currentNode);
                const agentDetails = agentDetailRes ? [agentDetailRes] : [];
                const queueSetting: any = await getQueueSettingS(getQueueList?.[0]?.qid, data?.channelId?.domainId);
                const agentFinalList = [];
                agentDetails?.map((_l: any) => {
                  if (!agentData.includes(parseInt(_l.ext, 10))) {
                    agentFinalList.push(_l);
                  }
                });
                if (agentFinalList && agentFinalList?.length > 0) {
                  agentFinalList[0].queueId = data?.currentNode.data?.information?.transferTo;
                  agentFinalList[0].queueSettings = queueSetting;
                  const whatsappCcaasTable = [data?.channelId];
                  try {
                    if (Array.isArray(agentFinalList) && agentFinalList?.length > 0) {
                      agentData.push(parseInt(agentFinalList[0]?.ext, 10));
                      console.log(emitter?.invite, `${agentFinalList[0]?.domainId}_${agentFinalList[0]?.ext}`);
                      ioredisWhatsapp.to(`${agentFinalList[0]?.domainId}_${agentFinalList[0]?.ext}`).emit(emitter?.invite, {
                        channelType: whatsappCcaasTable[0].channelType, customerNumber: whatsappCcaasTable[0].customerNumber,
                        agentNumber: whatsappCcaasTable[0].calledNumber, sessionId: whatsappCcaasTable[0].sessionId, historyId: whatsappCcaasTable[0]?.WhatsappHistoryId,
                        calledPersonDetails: getPersonalData[0] ?? null,
                        allAgent: true, WhatsappDurationHistoryId: whatsappCcaasTable[0]?.WhatsappDurationHistoryId,
                        isTransferChat: 0,
                        currentNode: getQueueList?.[0]?.qid ?? 0
                      });
                    }
                  } catch (error) {
                    console.log(error);
                  }
                } else {
                  const queueSettings = typeof data?.queueSetting === 'string'
                    ? (() => {
                      try {
                        return JSON.parse(data?.queueSetting);
                      } catch (error) {
                        return [];
                      }
                    })()
                    : [];
                  if (queueSettings?.type === 'Set Wait Time') {
                    const givenTimestamp = data?.queuedTime * 1000;
                    const currentTimestamp = Date.now();
                    const differenceInMilliseconds = currentTimestamp - givenTimestamp;
                    const differenceInMinutes = Math.floor(differenceInMilliseconds / 60000);
                    if (queueSettings?.maxWaitTime <= differenceInMinutes) {
                      const queueListChanges = queueResult?.filter((_l: any) => _l?.sessionId !== data?.sessionId);
                      const payload = {
                        qid: getQueueList?.[0]?.qid ?? 0,
                        queueName: getQueueList?.[0]?.queueName,
                        queueType: channelType?.Whatsapp,
                        domainId: getQueueList?.[0]?.domainId,
                        queueList: JSON.stringify(queueListChanges)
                      };
                      try {
                        await insertUpdateQueueWaitTimeDao(payload, queueSetting, data?.channelId);
                      } catch (err) {
                        console.log(err);
                      }
                      ioredisWhatsapp.emit(emitter?.queueWait, payload);
                      if (queueSettings?.fallBackQueue !== 0 && queueSettings?.fallBackQueue) {
                        const queueWeightTime: any = await getAllQueueWaitTime(getQueueList?.[0]?.domainId);
                        const queueDetails: any = await getQueueSettingS(queueSettings?.fallBackQueue, getQueueList?.[0]?.domainId);
                        const result = queueWeightTime?.filter((lists: any) => lists?.qid === queueSettings?.fallBackQueue && lists?.queueName === queueSettings?.fallBackQueueName && lists?.queueType === 'chat');
                        data.channelId.queueId = queueSettings?.fallBackQueue;
                        const waitTimmer: any = JSON.parse(queueDetails?.maximumCallers);
                        const waitQueueListData: any = result?.length > 0 ? JSON.parse(result[0]?.queueList) : [];
                        if (waitTimmer?.type !== 'Set Max Caller limit' || waitTimmer?.queueLimit > waitQueueListData?.length) {
                          const modifyData = {
                            firstName: data?.firstName ?? null,
                            lastName: data?.lastName ?? null,
                            sessionId: data?.sessionId,
                            queuedTime: Math.floor(Date.now() / 1000),
                            type: channelType?.Whatsapp,
                            chatHistoryId: data?.WhatsappHistoryId,
                            queueSetting: queueDetails?.maximumWaitTime,
                            queueUserCount: queueDetails?.maximumCallers,
                            channelId: data,
                            currentNode: data?.currentNode
                          };
                          const queueListData = [];
                          if (result?.length > 0) {
                            const preQueueData: any = JSON.parse(result[0]?.queueList);
                            preQueueData?.push(modifyData);
                            queueListData.push(...preQueueData);
                          } else {
                            queueListData.push(modifyData);
                          }
                          const payloads = {
                            qid: queueSettings?.fallBackQueue ?? 0,
                            queueName: queueSettings?.fallBackQueueName,
                            queueType: channelType?.Whatsapp,
                            domainId: getQueueList?.[0]?.domainId,
                            queueList: JSON.stringify(queueListData),
                            sessionId: data?.channelId?.sessionId
                          };
                          try {
                            await insertUpdateQueueWaitTimeDao(payload, queueSetting, data?.channelId);
                          } catch (err) {
                            console.log(err);
                          }
                          ioredisWhatsapp.emit(emitter?.queueWait, payload);
                          ioredisWhatsapp.to(data?.channelId?.sessionId).emit(emitter?.fallBackQueue, payloads);
                          const fallBackAgentDetails = await transferChat(modifyData?.channelId, modifyData?.currentNode);
                          const fallBackQueueSetting: any = await getQueueSettingS(modifyData?.channelId?.queueId, data?.channelId?.domainId);
                          if (fallBackAgentDetails && fallBackAgentDetails?.length > 0) {
                            fallBackAgentDetails[0].queueId = data?.currentNode.data?.information?.transferTo;
                            fallBackAgentDetails[0].queueSettings = fallBackQueueSetting;
                            const whatsappCcaasTable = [data?.channelId];
                            try {
                              if (Array.isArray(agentDetails) && agentDetails?.length > 0) {
                                console.log(emitter?.invite, `${agentDetails[0]?.domainId}_${agentDetails[0]?.ext}`);
                                ioredisWhatsapp.to(`${agentDetails[0]?.domainId}_${agentDetails[0]?.ext}`).emit(emitter?.invite, {
                                  channelType: whatsappCcaasTable[0].channelType, customerNumber: whatsappCcaasTable[0].customerNumber,
                                  agentNumber: whatsappCcaasTable[0].calledNumber, sessionId: whatsappCcaasTable[0].sessionId, historyId: whatsappCcaasTable[0]?.WhatsappHistoryId,
                                  calledPersonDetails: getPersonalData[0] ?? null,
                                  allAgent: true, WhatsappDurationHistoryId: whatsappCcaasTable[0]?.WhatsappDurationHistoryId,
                                  isTransferChat: 0,
                                  currentNode: getQueueList?.[0]?.qid ?? 0
                                });
                              }
                            } catch (error) {
                              console.log(error);
                            }
                          } else {
                            const redisDataObj = {
                              qid: queueSettings?.fallBackQueue,
                              queueType: channelType?.Whatsapp,
                              domainId: getQueueList?.[0]?.domainId,
                            };
                            if (redisData) {
                              const hasQid = redisData?.some((item: any) => item.qid === queueSettings?.fallBackQueue);
                              if (!hasQid) {
                                redisData.push(redisDataObj);
                                ioredisWhatsappQueue.set(process.env.WHATSAPP_QUEUE, JSON.stringify(redisData));
                              }
                            } else {
                              ioredisWhatsappQueue.set(process.env.WHATSAPP_QUEUE, JSON.stringify([redisDataObj]));
                            }
                          }
                        } else {
                          data.channelId.queueId = waitTimmer?.overFlowQueue;
                          insertCustomerInQueue(data.channelId, data?.currentNode, waitTimmer?.overFlowQueue);
                        }
                      }
                    }
                    logger.info(`Difference in minutes: ${differenceInMinutes}`);
                  }
                }
                if (queueResult?.length - 1 === index) {
                  agentData = [];
                }
              });
            } else {
              const redisQueueArr = redisData?.filter((_d: any) => _d?.qid !== getQueueList?.[0]?.qid);
              ioredisWhatsappQueue.set(process.env.WHATSAPP_QUEUE, JSON.stringify(redisQueueArr));
            }
          });
        }
      }
    },          60000);
  } catch (err) {
    logger.error('chatQueueHandler err', err);
  }
};
