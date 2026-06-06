import {
  getAllQueueWaitTime,
  getPersonalDetails,
  getQueueSettingS,
  getQueueWaitTime,
  insertUpdateQueueWaitTimeDao
 } from '../dao/facebook';
import { channelType, emiiter, RESPONSE } from '../helpers/constants';
import { insertCustomerInQueue } from '../modules/facebook/handler';
import { logger } from '../plugins/log';
import { ioredisFacebookQueue, ioredisSocialMedia } from '../plugins/redis';
import { transferChat } from './transferToQueue';

// handle queue function for Facebook
export const chatQueueHandler = () => {
  try {
    setInterval(async () => {
      const redisQueueData: any = await ioredisFacebookQueue.get(process.env.FACEBOOK_QUEUE);
      if (redisQueueData) {
        const redisData = JSON.parse(redisQueueData);
        if (redisData?.length > 0) {
          redisData?.map(async (list: any) => {
            const getQueueList = await getQueueWaitTime(list);
            const queueResult = typeof getQueueList?.[0]?.queueList === 'string' ? (() => {
              try {
                return JSON.parse(getQueueList?.[0]?.queueList);
              } catch (err) {
                logger.error('chatQueueHandler err', err);
                return [];
              }
            })() : [];
            if (queueResult?.length > 0) {
              let agentData = [];
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
                  const facebookCcaasTable = [data?.channelId];
                  try {
                    if (Array.isArray(agentFinalList) && agentFinalList?.length > 0) {
                      agentData.push(parseInt(agentFinalList[0]?.ext, 10));
                      const getPersonalData: any = await getPersonalDetails(facebookCcaasTable[0]?.customerFacebookId, facebookCcaasTable[0]?.domainId);
                      console.log(emiiter?.sendInvite, `${agentFinalList[0]?.domainId}_${agentFinalList[0]?.ext}`);
                      ioredisSocialMedia.to(`${agentFinalList[0]?.domainId}_${agentFinalList[0]?.ext}`).emit(emiiter?.sendInvite, {
                        channelType: facebookCcaasTable[0].channelType, customerNumber: facebookCcaasTable[0].customerFacebookId,
                        agentNumber: facebookCcaasTable[0].agentFacebookId, sessionId: facebookCcaasTable[0].sessionId, historyId: facebookCcaasTable[0]?.FacebookHistoryId,
                        calledPersonDetails: getPersonalData[0] ?? null,
                        allAgent: true,
                        isTransferChat: 0,
                        currentNode: getQueueList?.[0]?.qid ?? 0
                      });
                    }
                  } catch (err) {
                    logger.error('chatQueueHandler err', err);
                  }
                } else {
                  const queueSettings = typeof data?.queueSetting === 'string'
                    ? (() => {
                      try {
                        return JSON.parse(data?.queueSetting);
                      } catch (err) {
                        logger.error('chatQueueHandler err', err);
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
                        queueType: channelType?.facebook,
                        domainId: getQueueList?.[0]?.domainId,
                        queueList: JSON.stringify(queueListChanges)
                      };
                      try {
                        await insertUpdateQueueWaitTimeDao(payload, queueSetting, data?.channelId);
                      } catch (err) {
                        logger.error('chatQueueHandler err', err);
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
                            type: channelType?.Facebook,
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
                            queueType: channelType?.Facebook,
                            domainId: getQueueList?.[0]?.domainId,
                            queueList: JSON.stringify(queueListData),
                            sessionId: data?.channelId?.sessionId
                          };
                          try {
                            await insertUpdateQueueWaitTimeDao(payload, queueSetting, data?.channelId);
                          } catch (err) {
                            logger.error('chatQueueHandler err', err);
                          }
                          ioredisSocialMedia.emit(emiiter?.queueWait, payload);
                          ioredisSocialMedia.to(data?.channelId?.sessionId).emit(emiiter?.fallBackQueue, payloads);
                          const fallBackAgentDetail = await transferChat(modifyData?.channelId, modifyData?.currentNode);
                          const fallBackQueueSetting: any = await getQueueSettingS(modifyData?.channelId?.queueId, data?.channelId?.domainId);
                          const fallBackAgentDetails = fallBackAgentDetail ? [fallBackAgentDetail] : [];
                          if (fallBackAgentDetails && fallBackAgentDetails?.length > 0) {
                            fallBackAgentDetails[0].queueId = data?.currentNode.data?.information?.transferTo;
                            fallBackAgentDetails[0].queueSettings = fallBackQueueSetting;
                            const facebookCcaasTable = [data?.channelId];
                            try {
                              if (Array.isArray(agentDetails) && agentDetails.length > 0) {
                                const getPersonalData: any = await getPersonalDetails(facebookCcaasTable[0].customerFacebookId, facebookCcaasTable[0].domainId);
                                console.log(emiiter?.sendInvite, `${agentDetails[0]?.domainId}_${agentDetails[0]?.ext}`);
                                ioredisSocialMedia.to(`${agentDetails[0]?.domainId}_${agentDetails[0]?.ext}`).emit(emiiter?.sendInvite, {
                                  channelType: facebookCcaasTable[0].channelType, customerNumber: facebookCcaasTable[0].customerFacebookId,
                                  agentNumber: facebookCcaasTable[0].agentFacebookId, sessionId: facebookCcaasTable[0].sessionId, historyId: facebookCcaasTable[0]?.FacebookHistoryId,
                                  calledPersonDetails: getPersonalData[0] ?? null,
                                  allAgent: true,
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
                              queueType: channelType?.Facebook,
                              domainId: getQueueList?.[0]?.domainId,
                            };
                            if (redisData) {
                              const hasQid = redisData?.some((item: any) => item.qid === queueSettings?.fallBackQueue);
                              if (!hasQid) {
                                redisData.push(redisDataObj);
                                ioredisFacebookQueue.set(process.env.FACEBOOK_QUEUE, JSON.stringify(redisData));
                              }
                            } else {
                              ioredisFacebookQueue.set(process.env.FACEBOOK_QUEUE, JSON.stringify([redisDataObj]));
                            }
                          }
                        } else {
                          data.channelId.queueId = waitTimmer?.overFlowQueue;
                          insertCustomerInQueue(data.channelId, data?.currentNode, waitTimmer?.overFlowQueue);
                        }
                      }
                    }
                    logger.error(`Difference in minutes: ${differenceInMinutes}`);
                  }
                }
                if (queueResult?.length - 1 === index) {
                  agentData = [];
                }
              });
            } else {
              const redisQueueArr = redisData?.filter((_d: any) => _d?.qid !== getQueueList?.[0]?.qid);
              ioredisFacebookQueue.set(process.env.FACEBOOK_QUEUE, JSON.stringify(redisQueueArr));
            }
          });
        }
      }
    },          60000);
  } catch (err) {
    logger.error('chatQueueHandler err', err);
  }
};

export const chatQueueByAgentStatusHandler = async (req: any, res: any, done: any) => {
  res.status(200).send({ statusCode: 200, message: RESPONSE.success_message });
  try {
    const datas: any = {
      ext: req?.body?.ext,
      domainId: req?.body.domainId
    };
    const redisQueueData: any = await ioredisFacebookQueue.get(process.env.FACEBOOK_QUEUE);
    if (redisQueueData) {
      const redisData = JSON.parse(redisQueueData);
      const filterResult = redisData?.length > 0 && redisData?.filter((_f: any) => _f?.domainId === datas?.domainId);
      if (filterResult?.length > 0) {
        filterResult?.map(async (list: any) => {
          const getQueueList = await getQueueWaitTime(list);
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
                const facebookCcaasTable = [data?.channelId];
                try {
                  if (Array.isArray(agentFinalList) && agentFinalList?.length > 0) {
                    agentData.push(parseInt(agentFinalList[0]?.ext, 10));
                    const getPersonalData: any = await getPersonalDetails(facebookCcaasTable[0]?.customerFacebookId, facebookCcaasTable[0]?.domainId);
                    console.log(emiiter?.sendInvite, `${agentFinalList[0]?.domainId}_${agentFinalList[0]?.ext}`);
                    ioredisSocialMedia.to(`${agentFinalList[0]?.domainId}_${agentFinalList[0]?.ext}`).emit(emiiter?.sendInvite, {
                      channelType: facebookCcaasTable[0].channelType, customerNumber: facebookCcaasTable[0].customerFacebookId,
                      agentNumber: facebookCcaasTable[0].agentFacebookId, sessionId: facebookCcaasTable[0].sessionId, historyId: facebookCcaasTable[0]?.FacebookHistoryId,
                      calledPersonDetails: getPersonalData[0] ?? null,
                      allAgent: true,
                      isTransferChat: 0,
                      currentNode: getQueueList?.[0]?.qid ?? 0
                    });
                  }
                } catch (err) {
                  logger.error('chatQueueByAgentStatusHandler err', err);
                }
              }
            });
          }
        });
      }
    }
  } catch (err) {
    logger.error('chatQueueByAgentStatusHandler err', err);
  }
};
