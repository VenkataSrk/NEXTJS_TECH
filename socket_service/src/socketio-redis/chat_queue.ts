
import { handleTransferToLiveAgentToQueue, handleTransferToQueue } from '../chatflow_helpers';
import { getDomainStatus, getQueueSettings, getQueueSettingS } from '../dao/callflow.dao';
import { getAllQueueWaitTime, getQueueWaitTime, insertUpdateQueueWaitTimeDao } from '../dao/webChat.dao';
import { insertCustomerInQueue, longWaitRouting, transferChatForQueuedCustomer } from '../helperFunctions/transferToQueue';
import { insertQueuedTimeForChat } from '../helpers/constants';
import { agentStatusQueueData, ioredisChat, queueGetter } from '../plugins/db';

export const chatQueueHandler = () => {
  try {
    setInterval(async () => {
      const redisQueueData: any = await queueGetter.get('queuedCustomer');
      if (redisQueueData) {
        const redisData = JSON.parse(redisQueueData);
        if (redisData?.length > 0) {
          // redisData?.map(async (list: any) => {
          for (const list of redisData) {
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
              const agentExtList = [];
              // queueResult?.map(async (data: any) => {
              for (const data of queueResult) {
                const now = Math.floor(Date.now() / 1000);
                const isOlderThan30Minutes = (now - data?.queuedTime) > 30 * 60;
                if (isOlderThan30Minutes) {
                  if (getQueueList?.[0]?.qid) {
                    const insertData = queueResult?.filter((listed: any) => listed?.sessionId !== data?.sessionId);
                    const matchData = queueResult?.filter((listmatch: any) => listmatch?.sessionId === data?.sessionId);
                    const payload = {
                      qid: getQueueList?.[0]?.qid ?? 0,
                      queueName: getQueueList?.[0]?.queueName,
                      queueType: 'chat',
                      domainId: getQueueList?.[0]?.domainId,
                      queueList: JSON.stringify(insertData)
                    };
                    try {
                      await insertUpdateQueueWaitTimeDao(payload);
                      insertQueuedTimeForChat(matchData?.length > 0 ? matchData : {});
                    } catch (err) {
                      console.log(err);
                    }
                    ioredisChat.emit('callQueueWait', payload);
                    const getDomainResult: any = await getDomainStatus(data?.channelId?.domain_id);
                    ioredisChat?.to(data?.session_id).emit('chat_response', JSON.stringify({ session_id: data?.channelId?.session_id, req_type: 'DISCONNECT', domainStatus: getDomainResult }));
                  }

                } else {
                  if (!data?.chatNotifyStatus) {
                    const longWaitRoutingAgent = await transferChatForQueuedCustomer(data?.channelId, data?.currentNode, agentExtList);
                    const queueSetting: any = await getQueueSettings(getQueueList?.[0]?.qid);
                    const agentFinalList = [];
                    longWaitRoutingAgent?.map((_list: any) => {
                      if (!agentExtList?.includes(_list?.ext)) {
                        agentFinalList?.push(_list);
                      }
                    });
                    if (agentFinalList && agentFinalList?.length > 0) {
                      agentExtList.push(agentFinalList[0]?.ext);
                      agentFinalList[0].queueId = data?.currentNode.data?.information?.transferTo;
                      agentFinalList[0].queueSettings = queueSetting;
                      await handleTransferToLiveAgentToQueue(data?.channelId, data?.currentNode, agentFinalList);
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
                            queueType: 'chat',
                            domainId: getQueueList?.[0]?.domainId,
                            queueList: JSON.stringify(queueListChanges)
                          };
                          try {
                            await insertUpdateQueueWaitTimeDao(payload);
                          } catch (err) {
                            console.log(err);
                          }
                          ioredisChat.emit('callQueueWait', payload);
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
                                type: 'Chat',
                                chatHistoryId: data?.ChatHistoryId,
                                queueSetting: queueDetails?.maximumWaitTime,
                                queueUserCount: queueDetails?.maximumCallers,
                                queueAnnouncement: queueDetails?.announcement,
                                channelId: data?.channelId,
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
                                queueType: 'chat',
                                domainId: getQueueList?.[0]?.domainId,
                                queueList: JSON.stringify(queueListData),
                                sessionId: data?.channelId?.session_id
                              };
                              try {
                                await insertUpdateQueueWaitTimeDao(payloads);
                              } catch (err) {
                                console.log(err);
                              }
                              ioredisChat.emit('callQueueWait', payload);
                              ioredisChat.to(data?.channelId?.session_id).emit('changeFallBackQueue', payloads);
                              const fallBackAgentDetails = await transferChatForQueuedCustomer(modifyData?.channelId, modifyData?.currentNode, []);
                              const fallBackQueueSetting: any = await getQueueSettings(modifyData?.channelId?.queueId);
                              if (fallBackAgentDetails && fallBackAgentDetails?.length > 0) {
                                fallBackAgentDetails[0].queueId = data?.currentNode.data?.information?.transferTo;
                                fallBackAgentDetails[0].queueSettings = fallBackQueueSetting;
                                await handleTransferToLiveAgentToQueue(data?.channelId, data?.currentNode, fallBackAgentDetails);
                              } else {
                                const redisDataObj = {
                                  qid: queueSettings?.fallBackQueue,
                                  queueType: 'chat',
                                  domainId: getQueueList?.[0]?.domainId,
                                };
                                if (redisData) {
                                  const hasQid = redisData?.some((item: any) => item.qid === queueSettings?.fallBackQueue);
                                  if (!hasQid) {
                                    redisData.push(redisDataObj);
                                    queueGetter.set('queuedCustomer', JSON.stringify(redisData));
                                  }
                                } else {
                                  queueGetter.set('queuedCustomer', JSON.stringify([redisDataObj]));
                                }
                              }
                            } else {
                              data.channelId.queueId = waitTimmer?.overFlowQueue;
                              insertCustomerInQueue(data.channelId, data?.currentNode, waitTimmer?.overFlowQueue);
                            }
                          }
                        }
                        console.log(`Difference in minutes: ${differenceInMinutes}`);
                      }
                    }
                  } else {
                    const currentTime = Math.floor(Date.now() / 1000);
                    const chatNotifyDiff = currentTime - data?.notifyQueuedTime;
                    if (chatNotifyDiff > 60) {
                      const resultData = queueResult?.map((list_m: any) => {
                        if (list_m?.sessionId === data?.channelId?.session_id) {
                          list_m.chatNotifyStatus = false;
                        }
                        return list_m;
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
                  }
                }
              }
            } else {
              const redisQueueArr = redisData?.filter((_d: any) => _d?.qid !== getQueueList?.[0]?.qid);
              queueGetter.set('queuedCustomer', JSON.stringify(redisQueueArr));
            }
          }
        }
      }
    },          60000);
  } catch (err) {
    console.log(err);
  }
};

export const chatQueueByAgentStatusHandler = async (agentData: any, skipRecursion = false) => {
  try {
    const redisQueueData: any = await queueGetter.get('queuedCustomer');
    if (redisQueueData) {
      const redisData = JSON.parse(redisQueueData);
      const filterResult = redisData?.length > 0 && redisData?.filter((_f: any) => _f?.domainId === agentData?.domainId);
      if (filterResult?.length > 0) {
        // await Promise.all(filterResult?.map(async (list: any) => {
        for (const list of filterResult) {
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
            const agentExtList = [];
            // queueResult?.map(async (data: any) => {
            for (const data of queueResult) {
              const now = Math.floor(Date.now() / 1000);
              const isOlderThan30Minutes = (now - data?.queuedTime) > 30 * 60;
              if (isOlderThan30Minutes) {
                if (getQueueList?.[0]?.qid) {
                  const insertData = queueResult?.filter((listed: any) => listed?.sessionId !== data?.sessionId);
                  const matchData = queueResult?.filter((listmatch: any) => listmatch?.sessionId === data?.sessionId);
                  const payload = {
                    qid: getQueueList?.[0]?.qid ?? 0,
                    queueName: getQueueList?.[0]?.queueName,
                    queueType: 'chat',
                    domainId: getQueueList?.[0]?.domainId,
                    queueList: JSON.stringify(insertData)
                  };
                  try {
                    await insertUpdateQueueWaitTimeDao(payload);
                    insertQueuedTimeForChat(matchData?.length > 0 ? matchData : {});
                  } catch (err) {
                    console.log(err);
                  }
                  ioredisChat.emit('callQueueWait', payload);
                  const getDomainResult: any = await getDomainStatus(data?.channelId?.domain_id);
                  ioredisChat?.to(data?.session_id).emit('chat_response', JSON.stringify({ session_id: data?.channelId?.session_id, req_type: 'DISCONNECT', domainStatus: getDomainResult }));
                }
              } else {
                if (!data?.chatNotifyStatus) {
                  const longWaitRoutingAgent = await transferChatForQueuedCustomer(data?.channelId, data?.currentNode, agentExtList);
                  const queueSetting: any = await getQueueSettings(getQueueList?.[0]?.qid);
                  const agentFinalList = [];
                  longWaitRoutingAgent?.map((_list: any) => {
                    if (!agentExtList?.includes(_list?.ext)) {
                      agentFinalList?.push(_list);
                    }
                  });
                  console.log('agentFinalList:::::::::1212', agentFinalList, agentExtList);
                  if (agentFinalList && agentFinalList?.length > 0) {
                    agentExtList.push(agentFinalList[0]?.ext);
                    const listed = {
                      qid: data?.currentNode.data?.information?.transferTo,
                      domainId: data?.channelId?.domain_id
                    };
                    const getQueueListed = await getQueueWaitTime(listed);
                    const parsedData = JSON.parse(getQueueListed?.[0]?.queueList);
                    const resultData = parsedData?.map((list_1: any) => {
                      if (list_1?.sessionId === data?.channelId?.session_id) {
                        list_1.chatNotifyStatus = true;
                        list.notifyQueuedTime = Math.floor(Date.now() / 1000);
                      }
                      return list_1;
                    });
                    const payload = {
                      qid: getQueueListed?.[0]?.qid ?? 0,
                      queueName: getQueueListed?.[0]?.queueName,
                      queueType: 'chat',
                      domainId: getQueueListed?.[0]?.domainId,
                      queueList: JSON.stringify(resultData)
                    };
                    try {
                      await insertUpdateQueueWaitTimeDao(payload);
                    } catch (err) {
                      console.log(err);
                    }
                    agentFinalList[0].queueId = data?.currentNode.data?.information?.transferTo;
                    agentFinalList[0].queueSettings = queueSetting;
                    await handleTransferToLiveAgentToQueue(data?.channelId, data?.currentNode, agentFinalList);
                  }
                }
              }
            }
          }
        }
      }
    }
  } catch (err) {
    console.log(err);
  } finally {
    // CRITICAL FIX: Only call setTimeout if NOT called from AgentQueueManager
    // This prevents the infinite recursive loop that was causing memory buildup
    if (!skipRecursion) {
      setTimeout(() => {
        blpopQueuedCustomerByStatus();
      },         2000);
    }
  }
};

export const blpopQueuedCustomerByStatus = async () => {
  // DEPRECATED: This function is no longer used
  // Queue processing is now handled by AgentQueueManager
  // Keeping for backward compatibility, but it does nothing
  console.warn('blpopQueuedCustomerByStatus called but is deprecated - use AgentQueueManager instead');
};
