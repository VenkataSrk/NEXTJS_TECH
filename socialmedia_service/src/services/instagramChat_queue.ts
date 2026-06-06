import {
  getAllQueueWaitTime,
  getQueueSettingS
} from '../dao/facebook';
import {
  getPersonalDetailsInstagram,
  getQueueWaitTimeInsta,
  insertUpdateQueueWaitTimeInsta
} from '../dao/instagram';
import { channelType, emiiter, RESPONSE } from '../helpers/constants';
import {
  insertCustomerInQueueInsta
} from '../modules/instagram/handler';
import { logger } from '../plugins/log';
import { ioredisInstagramQueue, ioredisSocialMedia } from '../plugins/redis';
import { transferChat } from './transferToQueue';

// handle queue function for instagram
export const chatQueueInstagramHandler = () => {
  try {
    setInterval(async () => {
      const redisQueueData: any = await ioredisInstagramQueue.get(process.env.INSTA_QUEUE);
      if (redisQueueData) {
        const redisData = JSON.parse(redisQueueData);
        if (redisData?.length > 0) {
          redisData?.map(async (list: any) => {
            const getQueueList = await getQueueWaitTimeInsta(list);
            const queueResult = typeof getQueueList?.[0]?.queueList === 'string' ? (() => {
              try {
                return JSON.parse(getQueueList?.[0]?.queueList);
              } catch (err) {
                logger.error('chatQueueByAgentStatusInstaHandler err', err);
                return [];
              }
            })() : [];
            if (queueResult?.length > 0) {
              let agentData: any = [];
              queueResult?.map(async (data: any, index: any) => {
                const agentDetail = await transferChat(data?.channelId, data?.currentNode);
                const queueSetting: any = await getQueueSettingS(getQueueList?.[0]?.qid, data?.channelId?.domainId);
                const agentDetails = agentDetail ? [agentDetail] : [];
                const agentFinalList = [];
                agentDetails?.map((_l: any) => {
                  if (!agentData.includes(parseInt(_l.ext, 10))) {
                    agentFinalList.push(_l);
                  }
                });
                if (agentFinalList && agentFinalList?.length > 0) {
                  agentFinalList[0].queueId = data?.currentNode.data?.information?.transferTo;
                  agentFinalList[0].queueSettings = queueSetting;
                  const instagramCcaasTable = [data?.channelId];
                  try {
                    if (Array.isArray(agentFinalList) && agentFinalList.length > 0) {
                      agentData.push(parseInt(agentFinalList[0]?.ext, 10));
                      const getPersonalData: any = await getPersonalDetailsInstagram(instagramCcaasTable[0].customerInstagramId, instagramCcaasTable[0].domainId);
                      console.log(emiiter?.sendInvite, `${agentFinalList[0]?.domainId}_${agentFinalList[0]?.ext}`);
                      ioredisSocialMedia.to(`${agentFinalList[0]?.domainId}_${agentFinalList[0]?.ext}`).emit(emiiter?.sendInvite, {
                        channelType: instagramCcaasTable[0].channelType,
                        customerNumber: instagramCcaasTable[0].customerInstagramId,
                        agentNumber: instagramCcaasTable[0].agentInstagramId,
                        historyId: instagramCcaasTable[0]?.InstagramHistoryId,
                        sessionId: instagramCcaasTable[0].sessionId,
                        calledPersonDetails: getPersonalData[0] ?? null,
                        allAgent: true,
                        isTransferChat: 0,
                        currentNode: getQueueList?.[0]?.qid ?? 0
                      });
                    }
                  } catch (err) {
                    logger.error('chatQueueByAgentStatusInstaHandler err', err);
                  }
                } else {
                  const queueSettings = typeof data?.queueSetting === 'string'
                    ? (() => {
                      try {
                        return JSON.parse(data?.queueSetting);
                      } catch (err) {
                        logger.error('chatQueueByAgentStatusInstaHandler err', err);
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
                        queueType: channelType?.instagram,
                        domainId: getQueueList?.[0]?.domainId,
                        queueList: JSON.stringify(queueListChanges)
                      };
                      try {
                        await insertUpdateQueueWaitTimeInsta(payload, queueSetting, data?.channelId);
                      } catch (err) {
                        logger.error('chatQueueByAgentStatusInstaHandler err', err);
                      }
                      ioredisSocialMedia.emit(emiiter?.queueWait, payload);
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
                            type: channelType?.Instagram,
                            chatHistoryId: data?.FacebookHistoryId,
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
                            queueType: channelType?.Instagram,
                            domainId: getQueueList?.[0]?.domainId,
                            queueList: JSON.stringify(queueListData),
                            sessionId: data?.channelId?.sessionId
                          };
                          try {
                            await insertUpdateQueueWaitTimeInsta(payload, queueSetting, data?.channelId);
                          } catch (err) {
                            console.log(err);
                          }
                          ioredisSocialMedia.emit(emiiter?.queueWait, payload);
                          ioredisSocialMedia.to(data?.channelId?.sessionId).emit(emiiter?.fallBackQueue, payloads);
                          const fallBackAgentDetail = await transferChat(modifyData?.channelId, modifyData?.currentNode);
                          const fallBackQueueSetting: any = await getQueueSettingS(modifyData?.channelId?.queueId, data?.channelId?.domainId);
                          const fallBackAgentDetails = fallBackAgentDetail ? [fallBackAgentDetail] : [];
                          if (fallBackAgentDetails && fallBackAgentDetails?.length > 0) {
                            fallBackAgentDetails[0].queueId = data?.currentNode.data?.information?.transferTo;
                            fallBackAgentDetails[0].queueSettings = fallBackQueueSetting;
                            const instagramCcaasTable = [data?.channelId];
                            try {
                              if (Array.isArray(agentDetails) && agentDetails.length > 0) {
                                const getPersonalData: any = await getPersonalDetailsInstagram(instagramCcaasTable[0].customerInstagramId, instagramCcaasTable[0].domainId);
                                console.log(emiiter?.sendInvite, `${agentDetails[0]?.domainId}_${agentDetails[0]?.ext}`);
                                ioredisSocialMedia.to(`${agentDetails[0]?.domainId}_${agentDetails[0]?.ext}`).emit(emiiter?.sendInvite, {
                                  channelType: instagramCcaasTable[0].channelType,
                                  customerNumber: instagramCcaasTable[0].customerInstagramId,
                                  agentNumber: instagramCcaasTable[0].agentInstagramId,
                                  historyId: instagramCcaasTable[0]?.InstagramHistoryId,
                                  sessionId: instagramCcaasTable[0].sessionId,
                                  calledPersonDetails: getPersonalData[0] ?? null,
                                  allAgent: true,
                                  isTransferChat: 0,
                                  currentNode: getQueueList?.[0]?.qid ?? 0
                                });
                              }
                            } catch (err) {
                              logger.error('chatQueueByAgentStatusInstaHandler err', err);
                            }
                          } else {
                            const redisDataObj = {
                              qid: queueSettings?.fallBackQueue,
                              queueType: channelType?.Instagram,
                              domainId: getQueueList?.[0]?.domainId,
                            };
                            if (redisData) {
                              const hasQid = redisData?.some((item: any) => item.qid === queueSettings?.fallBackQueue);
                              if (!hasQid) {
                                redisData.push(redisDataObj);
                                ioredisInstagramQueue.set(process.env.INSTA_QUEUE, JSON.stringify(redisData));
                              }
                            } else {
                              ioredisInstagramQueue.set(process.env.INSTA_QUEUE, JSON.stringify([redisDataObj]));
                            }
                          }
                        } else {
                          data.channelId.queueId = waitTimmer?.overFlowQueue;
                          insertCustomerInQueueInsta(data.channelId, data?.currentNode, waitTimmer?.overFlowQueue);
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
              ioredisInstagramQueue.set(process.env.INSTA_QUEUE, JSON.stringify(redisQueueArr));
            }
          });
        }
      }
    },          60000);
  } catch (err) {
    logger.error('chatQueueInstagramHandler err', err);
  }
};

export const chatQueueByAgentStatusInstaHandler = async (req: any, res: any, done: any) => {
  res.status(200).send({ statusCode: 200, message: RESPONSE.success_message });
  try {
    const datas: any = {
      ext: req?.body?.ext,
      domainId: req?.body.domainId
    };
    const redisQueueData: any = await ioredisInstagramQueue.get(process.env.INSTA_QUEUE);
    if (redisQueueData) {
      const redisData = JSON.parse(redisQueueData);
      const filterResult = redisData?.length > 0 && redisData?.filter((_f: any) => _f?.domainId === datas?.domainId);
      if (filterResult?.length > 0) {
        filterResult?.map(async (list: any) => {
          const getQueueList = await getQueueWaitTimeInsta(list);
          const queueResult = typeof getQueueList?.[0]?.queueList === 'string'
            ? (() => {
              try {
                return JSON.parse(getQueueList?.[0]?.queueList);
              } catch (error) {
                return [];
              }
            })()
            : [];
          if (queueResult?.length > 0) {
            const agentData: any = [];
            queueResult?.map(async (data: any) => {
              const agentDetail = await transferChat(data?.channelId, data?.currentNode);
              const queueSetting: any = await getQueueSettingS(getQueueList?.[0]?.qid, data?.channelId?.domainId);
              const agentDetails = agentDetail ? [agentDetail] : [];
              const agentFinalList = [];
              agentDetails?.map((_l: any) => {
                if (!agentData.includes(parseInt(_l.ext, 10))) {
                  agentFinalList.push(_l);
                }
              });
              if (agentFinalList && agentFinalList?.length > 0) {
                agentFinalList[0].queueId = data?.currentNode.data?.information?.transferTo;
                agentFinalList[0].queueSettings = queueSetting;
                const instagramCcaasTable = [data?.channelId];
                try {
                  if (Array.isArray(agentFinalList) && agentFinalList.length > 0) {
                    agentData.push(parseInt(agentFinalList[0]?.ext, 10));
                    const getPersonalData: any = await getPersonalDetailsInstagram(instagramCcaasTable[0].customerInstagramId, instagramCcaasTable[0].domainId);
                    console.log(emiiter?.sendInvite, `${agentFinalList[0]?.domainId}_${agentFinalList[0]?.ext}`);
                    ioredisSocialMedia.to(`${agentFinalList[0]?.domainId}_${agentFinalList[0]?.ext}`).emit(emiiter?.sendInvite, {
                      channelType: instagramCcaasTable[0].channelType,
                      customerNumber: instagramCcaasTable[0].customerInstagramId,
                      agentNumber: instagramCcaasTable[0].agentInstagramId,
                      historyId: instagramCcaasTable[0]?.InstagramHistoryId,
                      sessionId: instagramCcaasTable[0].sessionId,
                      calledPersonDetails: getPersonalData[0] ?? null,
                      allAgent: true,
                      isTransferChat: 0,
                      currentNode: getQueueList?.[0]?.qid ?? 0
                    });
                  }
                } catch (err) {
                  logger.error('chatQueueByAgentStatusInstaHandler err', err);
                }
              }
            });
          }
        });
      }
    }
  } catch (err) {
    logger.error('chatQueueByAgentStatusInstaHandler err', err);
  }
};
