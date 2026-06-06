import {
  getAgentByChat,
  getAgentsSkills,
  getAllQueueWaitTime,
  getConcurrentChat,
  getCustomerCoId,
  getCustomerConatctDetails,
  getQueueSettingS,
  getRoutingDetails,
  getRoutingProfiles,
  getRoutingProfileUser,
  getUserList,
  insertStatus,
  insertUpdateQueueWaitTimeDao,
  updateQueueId
} from '../dao/whatsapp';
import { channelType, emitter } from '../helpers/constants';
import { logger } from '../plugins/log';
import { coreRedisClient, ioredisWhatsapp, ioredisWhatsappQueue } from '../plugins/redis';

/**
 *
 * @param channelId
 * @returns
 */
export const transferChatToAvailableAgent = async (channelId: any) => {
  try {

    let agentDetails: any = [];
    const readyAgentsData: any = await readyAgents(channelId); // checking agents in ready state
    logger.info('readyAgentsData', readyAgentsData);

    if (readyAgentsData?.length !== 0) { // Connecting chat with agents
      agentDetails = await longWaitRouting(channelId, undefined, readyAgentsData);
      logger.info('liveagent agentDetails', agentDetails);
    }

    if (agentDetails.length === 0) {
      const readySupervisorsData: any = await readySupervisors(channelId); // checking supervisors in ready state
      logger.info('readySupervisorsData', readySupervisorsData);
      if (readySupervisorsData?.length !== 0) { // Connecting chat with supervisors
        agentDetails = await longWaitRouting(channelId, undefined, readySupervisorsData);
        logger.info('liveagent SupervisorsDetails', agentDetails);
      }
    }

    if (agentDetails?.length === 0) {
      const readyAdminsData: any = await readyAdmins(channelId); // checking admins in ready state
      logger.info('readyAdminsData', readyAdminsData);
      if (readyAdminsData?.length !== 0) { // Connecting chat with admins
        agentDetails = await longWaitRouting(channelId, undefined, readyAdminsData);
        logger.info('liveagent adminDetails', agentDetails);
      }
    }
    return agentDetails;
  } catch (err) {
    return [];
  }
};

/**
 *
 * @param channelId
 * @param currentNode
 * @returns
 */
export const transferChat = async (channelId: any, currentNode: any) => {
  try {

    logger.info('transferChat currentNode', currentNode);
    logger.info('transferChat channelId', currentNode);

    let agentDetails: any = [];
    if (currentNode?.data?.information?.type === 2) { // To direct agent
      const readyAgentsSupervisorsAdminsData: any = await readyAgentsSupervisorsAdmins(channelId); // checking agents in ready state
      logger.info('readyAgentsSupervisorsAdminsData', readyAgentsSupervisorsAdminsData);
      if (readyAgentsSupervisorsAdminsData?.length === 0) { // returning when no agents available
        return agentDetails;
      }
      const filteredAgent = readyAgentsSupervisorsAdminsData?.filter((val: any) => val?.userId === currentNode?.data?.information?.transferTo); // checking agent ready status
      agentDetails = await agentChatLimitFilter(channelId, currentNode, filteredAgent[0]);
    } if (currentNode?.data?.information?.type === 1) { // To queue
      const readyAgentsData: any = await readyAgents(channelId); // checking agents in ready state
      logger.info('readyAgentsData', readyAgentsData);
      if (readyAgentsData?.length !== 0) { // Connecting chat with agents
        agentDetails = await queueChat(channelId, currentNode, readyAgentsData);
        logger.info('transferChat readyAgentsData agentDetails', agentDetails);
      }

      if (agentDetails.length === 0) {
        const readySupervisorsData: any = await readySupervisors(channelId); // checking agents in ready state
        logger.info('readySupervisorsData', readySupervisorsData);
        if (readySupervisorsData?.length !== 0) { // Connecting chat with supervisors
          agentDetails = await queueChat(channelId, currentNode, readySupervisorsData);
          logger.info('transferChat readySupervisorsData agentDetails', agentDetails);
        }
      }

      if (agentDetails.length === 0) {
        const readyAdminsData: any = await readyAdmins(channelId); // checking agents in ready state
        logger.info('readyAdminsData', readyAdminsData);
        if (readyAdminsData?.length !== 0) { // Connecting chat with admins
          agentDetails = await queueChat(channelId, currentNode, readyAdminsData);
          logger.info('transferChat readyAdminsData agentDetails', agentDetails);
        }
      }
    }
    return agentDetails[0];
  } catch (err) {
    logger.error('transferChat err', err);
    return [];
  }
};

/**
 *
 * @param channelId
 * @param currentNode
 * @param readyAgentsData
 * @returns
 */
const longWaitRouting = async (channelId: any, currentNode: any, readyAgentsData: any) => {
  try {
    const sortedAgentsBasedOnTime = readyAgentsData.sort((a: any, b: any) => a.stateTime - b.stateTime);
    logger.info('longWaitRouting sortedAgentsBasedOnTime', sortedAgentsBasedOnTime);
    const sortedAgentsBasedOnRole = sortedAgentsBasedOnTime.sort((a: any, b: any) => b.roleId - a.roleId);
    logger.info('longWaitRouting sortedAgentsBasedOnRole', sortedAgentsBasedOnRole);
    const chatLimitCheckedAgent: any = await chatLimitFilter(channelId, currentNode, sortedAgentsBasedOnRole);
    logger.info('longWaitRouting chatLimitCheckedAgent', chatLimitCheckedAgent);
    return chatLimitCheckedAgent;

  } catch (err) {
    logger.error('longWaitRouting err', err);
    return [];
  }

};

const readyAgentsSupervisorsAdmins = async (channel: any) => {
  try {
    const readyAgentsSupervisorsAdminsList = [];
    const data = await coreRedisClient.hgetall(`${channel?.domainId}_presence`);
    const result = Object.values(data).map((value: any) => JSON.parse(value));
    if (!data) {
      return readyAgentsSupervisorsAdminsList;
    }
    for (const item of result) {
      const getData: any = await getUserList(item);
      if (Number(item?.isChat) === 1 && Number(item?.isChatTransfer) === 0) {
        getData.forEach((datas: any) => {
          if (Number(item.ext) === datas.ext && datas.socialMedia === 1) {
            readyAgentsSupervisorsAdminsList.push(item);
          }
        });
      }
    }
    return readyAgentsSupervisorsAdminsList;
  } catch (err) {
    logger.error('readyAgentsSupervisorsAdmins err', err);
    return [];
  }
};

/**
 *
 * @param channel
 * @returns
 */
const readyAgents = async (channel: any) => {
  try {
    const readyAgentsList: any[] = [];
    const data = await coreRedisClient.hgetall(`${channel?.domainId}_presence`);
    if (!data || Object?.keys(data)?.length === 0) return readyAgentsList;
    const parsedItems = Object?.values(data)?.map((value: any) => JSON.parse(value));
    const userLists: any = await Promise.all(parsedItems?.map(item => getUserList(item)));
    parsedItems?.forEach((item, idx) => {
      if (
        Number(item?.isChat) === 1 &&
        Number(item?.isChatTransfer) === 0 &&
        Number(item?.roleId) === 6
      ) {
        const matchingUser = userLists[idx]?.some(
          (dataq: any) => Number(item?.ext) === dataq?.ext && dataq?.socialMedia === 1
        );
        if (matchingUser) {
          readyAgentsList?.push(item);
        }
      }
    });
    return readyAgentsList;
  } catch (err) {
    logger.error('readyAgents err', err);
    return [];
  }
};

/**
 *
 * @param channel
 * @returns
 */
const readySupervisors = async (channel: any) => {
  try {
    const readySupervisorsList: any[] = [];
    const data = await coreRedisClient.hgetall(`${channel?.domainId}_presence`);
    if (!data || Object?.keys(data)?.length === 0) return readySupervisorsList;
    const parsedItems = Object?.values(data)?.map((value: any) => JSON.parse(value));
    const userLists: any = await Promise.all(parsedItems?.map(item => getUserList(item)));
    parsedItems.forEach((item, idx) => {
      if (
        Number(item?.isChat) === 1 &&
        Number(item?.isChatTransfer) === 0 &&
        Number(item?.roleId) === 5
      ) {
        const matchingUser = userLists[idx]?.some(
          (user: any) => Number(item?.ext) === user?.ext && user?.socialMedia === 1
        );
        if (matchingUser) {
          readySupervisorsList?.push(item);
        }
      }
    });
    return readySupervisorsList;
  } catch (err) {
    logger.error('readySupervisors err', err);
    return [];
  }
};

/**
 *
 * @param channel
 * @returns
 */
const readyAdmins = async (channel: any) => {
  try {
    const readyAdminsList: any[] = [];
    const data = await coreRedisClient.hgetall(`${channel?.domainId}_presence`);
    if (!data || Object?.keys(data)?.length === 0) return readyAdminsList;
    const parsedItems = Object?.values(data)?.map(value => JSON.parse(value));
    const userLists: any = await Promise.all(parsedItems?.map(item => getUserList(item)));
    parsedItems?.forEach((item, idx) => {
      if (
        Number(item?.isChat) === 1 &&
        Number(item?.isChatTransfer) === 0 &&
        Number(item?.roleId) === 3
      ) {
        const hasMatch = userLists[idx]?.some(
          (user: any) => Number(item?.ext) === user?.ext && user?.socialMedia === 1
        );
        if (hasMatch) {
          readyAdminsList?.push(item);
        }
      }
    });
    return readyAdminsList;
  } catch (err) {
    logger.error('readyAdmins err', err);
    return [];
  }
};

const chatLimitFilter = async (channel: any, currentNode: any, filteredData: any) => {
  try {
    const agentDetails: any = [];
    const chatLimitData: any = await getConcurrentChat(channel.domainId);
    logger.info('chatLimitFilter chatLimitData', JSON.stringify(chatLimitData));

    if (!chatLimitData) {
      logger.info('*** Chat limit is not set ***');
      const sendAgent = await sendAgentDetails(channel, filteredData, undefined);
      logger.info(' chatLimitFiltersendAgent', sendAgent);
      return sendAgent;
    }

    const parsedChatLimitData = JSON.parse(chatLimitData?.agentOccupyDetails ?? 'null') || null;
    logger.info(' chatLimitFilter parsedChatLimitData', parsedChatLimitData);

    if (parsedChatLimitData === null) {
      const sendAgent = await sendAgentDetails(channel, filteredData[0], undefined);
      logger.info(' chatLimitFiltersendAgent', sendAgent);
      return sendAgent;
    }
    const currentChatCount = parsedChatLimitData?.currentChatCount;
    logger.info(' chatLimitFilter currentChatCount', currentChatCount);

    let chatLimitByAgent: any;
    let routingProfileUser: any;
    if (chatLimitData?.isAgent) {
      chatLimitByAgent = await getAgentByChat(channel?.domainId);
      logger.info(' chatLimitFilter chatLimitByAgent', JSON.stringify(chatLimitByAgent));
    } else if (chatLimitData?.isRoutingProfile) {
      routingProfileUser = await getRoutingProfileUser(channel?.domainId);
      logger.info(' chatLimitFilter routingProfileUser', routingProfileUser);
    }
    if (currentChatCount === null || currentChatCount === undefined) {
      const sendAgent = await sendAgentDetails(channel, filteredData[0], undefined);
      logger.info(' chatLimitFilter sendAgent', sendAgent);
      return sendAgent;
    }

    const appendChatCountToFilteredData = filteredData.map((dt: any) => {
      let updatedDt = { ...dt };
      for (const [key, value] of Object.entries(currentChatCount)) {
        if (+(dt.ext) === +(key)) {
          updatedDt = { ...updatedDt, currentChatCount: value };
          break;
        } else {
          updatedDt = { ...updatedDt, currentChatCount: 0 };
        }
      }
      return updatedDt;
    });

    logger.info(' chatLimitFilter appendChatCountToFilteredData', appendChatCountToFilteredData);

    const chatLimitAchievedAgents: any = [];
    let agentsToBeAssigned: any = [...appendChatCountToFilteredData];
    if (chatLimitData?.isAllAgent || chatLimitData?.isAgent || chatLimitData?.isRoutingProfile) {

      let tempAgent: any;
      const checkLiveAgent = () => {
        logger.info('checkLiveAgent called------>', tempAgent);
        if (tempAgent !== undefined) {
          if (chatLimitData?.isAllAgent) {
            if (tempAgent?.currentChatCount < chatLimitData?.allAgentLimit) {
              agentDetails.push(tempAgent);
            } else {
              if (chatLimitAchievedAgents.length !== appendChatCountToFilteredData.length) {
                chatLimitAchievedAgents.push(tempAgent);
                agentsToBeAssigned = agentsToBeAssigned.filter((val: any) => val.ext !== tempAgent.ext);
                assignFunction();
              }
            }
          } else if (chatLimitData?.isAgent) {
            chatLimitByAgent.map((list: any) => {
              logger.info('map--->', tempAgent.currentChatCount, list.chatLimit, tempAgent.ext, list.ext);
              if (+(tempAgent.ext) === +(list.ext)) {
                if (tempAgent?.currentChatCount < list.chatLimit) {
                  logger.info('condition met, agent added', tempAgent);
                  agentDetails.push(tempAgent);
                } else {
                  logger.info('condition not met, agent removed from list');
                  chatLimitAchievedAgents.push(tempAgent);
                  agentsToBeAssigned = agentsToBeAssigned.filter((val: any) => val.ext !== tempAgent.ext);
                  assignFunction();
                }
              }
            });
          } else if (chatLimitData?.isRoutingProfile) {
            logger.info('isRoutingProfile entered');
            const routings = {};
            routingProfileUser.map((item: any) => {
              routings[item.RoutingProfileId] = item.concurrentChat;
            });
            logger.info('routings', routings);
            for (const [key, value] of Object.entries(routings)) {
              logger.info('key', key, 'id', tempAgent.routingProfileId);
              if (+(tempAgent.routingProfileId) === +(key)) {
                if (tempAgent.currentChatCount < value) {
                  logger.info('condition met, agent added', tempAgent);
                  agentDetails.push(tempAgent);
                } else {
                  logger.info('condition not met, agent removed from list');
                  if (chatLimitAchievedAgents.length !== appendChatCountToFilteredData.length) {
                    chatLimitAchievedAgents.push(tempAgent);
                    agentsToBeAssigned = agentsToBeAssigned.filter((val: any) => val.ext !== tempAgent.ext);
                    assignFunction();
                  }
                }
              }
            }
          }
        }
      };

      const assignFunction = () => {
        logger.info('assignFunction called------>');
        if (agentsToBeAssigned?.length > 0) {
          tempAgent = agentsToBeAssigned.reduce((oldest: any, current: any) => {
            if (current?.currentChatCount < oldest?.currentChatCount) {
              return current;
            } if (current?.currentChatCount === oldest?.currentChatCount && oldest?.stateTime > current?.stateTime) {
              return current;
            }
            return oldest;

          },                                    agentsToBeAssigned[0]);
          checkLiveAgent();
        }
      };
      assignFunction();
    } else {
      logger.info('*** Chat limit turned off ***');
      const sendAgent = await sendAgentDetails(channel, filteredData[0], undefined);
      logger.info('sendAgent', sendAgent);
      return sendAgent;
    }
    if (agentDetails?.length > 0) {
      const sendAgent = await sendAgentDetails(channel, agentDetails[0], undefined);
      logger.info('chatLimitFilter sendAgent', sendAgent);
      return sendAgent;
    }
    return agentDetails;
  } catch (err) {
    logger.info('chatLimitFilter err', err);
    return [];
  }
};

const agentChatLimitFilter = async (channel: any, currentNode: any, filteredData: any) => {
  try {
    const chatLimitData: any = await getConcurrentChat(channel.domainId);
    logger.info('chatLimitFilter chatLimitData', chatLimitData);
    // {"domainId":7288,"isAllAgent":1,"allAgentLimit":2,"isAgent":0,"isRoutingProfile":0,"agentOccupyDetails":null}

    if (!chatLimitData) {
      logger.info('*** Chat limit is not set ***');
      const sendAgent = await sendAgentDetails(channel, filteredData, undefined);
      logger.info(' chatLimitFiltersendAgent', sendAgent);
      return sendAgent;
    }
    const parsedChatLimitData = JSON.parse(chatLimitData?.agentOccupyDetails ?? 'null') || null;
    logger.info('chatLimitFilter parsedChatLimitData', parsedChatLimitData);
    if (parsedChatLimitData === null) {
      logger.info('*** First chat in the domain, chatlimit set');
      const sendAgent = await sendAgentDetails(channel, filteredData, undefined);
      logger.info('chatLimitFiltersendAgent', sendAgent);
      return sendAgent;
    }

    const currentChatCount = parsedChatLimitData?.currentChatCount;
    logger.info(' chatLimitFilter currentChatCount', currentChatCount);
    logger.info('filteredData', filteredData);
    let chatLimitByAgent: any;
    let routingProfileUser: any;
    let filteredAgentLimit: any;
    let filteredRoutingProfile: any;

    if (chatLimitData?.isAgent) {
      chatLimitByAgent = await getAgentByChat(channel.domainId);
      logger.info(' chatLimitFilter chatLimitByAgent', chatLimitByAgent);
      filteredAgentLimit = chatLimitByAgent.filter((val: any) => +(val?.ext) === +(filteredData?.ext));
      logger.info('filteredAgentLimit', filteredAgentLimit);
    } else if (chatLimitData?.isRoutingProfile) {
      routingProfileUser = await getRoutingProfileUser(channel.domainId);
      logger.info(' chatLimitFilter routingProfileUser', routingProfileUser);
      filteredRoutingProfile = routingProfileUser.filter((val: any) => +(val?.RoutingProfileId) === +(filteredData?.routingProfileId));
      logger.info('filteredRoutingProfile', filteredRoutingProfile);
    }
    for (const [key, value] of Object.entries(currentChatCount)) {
      if (+(filteredData?.ext) === +(key)) {
        logger.info('ext matched');
        if (chatLimitData?.isAllAgent) {
          logger.info('isAllAgent limit matched');
          logger.info('allAgentLimit before', chatLimitData?.allAgentLimit, value);
          if (chatLimitData?.allAgentLimit > value) {
            logger.info('allAgentLimit less than value');
            return [filteredData];
          }
        } else if (chatLimitData?.isAgent) {
          logger.info('isAgent limit matched');
          logger.info('isAgent before', filteredAgentLimit[0]?.chatLimit, value);
          if (+(filteredAgentLimit[0]?.chatLimit) > +(value)) {
            logger.info('allAgentLimit less than value');
            return [filteredData];
          }
        } else if (chatLimitData?.isRoutingProfile) {
          logger.info('routingprofile limit matched');
          logger.info('routingprofile before', filteredRoutingProfile[0]?.concurrentChat, value);
          if (+(filteredRoutingProfile[0]?.concurrentChat) > +(value)) {
            logger.info('allAgentLimit less than value');
            return [filteredData];
          }
        }
        break;
      }
    }
    return [];
  } catch (err) {
    logger.error('agentChatLimitFilter', err);
    return [];
  }
};

/**
 *
 * @param channelId
 * @param currentNode
 * @param readyAgentsData
 * @returns
 */
const queueChat = async (channelId: any, currentNode: any, readyAgentsData: any) => {
  try {
    let filteredAgents: any = [];
    let skillRoutingAgent: any = [];
    const routingProfiles: any = await getRoutingProfiles(currentNode?.data?.information?.transferTo, channelId?.domainId); // getting routing profiles mapped to queue
    logger.info('queueChat routingProfiles', routingProfiles);
    await updateQueueId(channelId, currentNode?.data?.information?.transferTo);
    if (routingProfiles?.length === 0) {
      return [];
    }

    const sortedRoutingProfiles: any = routingProfiles.sort((a: any, b: any) => a.priority - b.priority); // sorting routing profiles based on priority
    for (const profile of sortedRoutingProfiles) {
      filteredAgents = readyAgentsData.filter((a: any) => a?.routingProfileId === profile?.rqmid);
      logger.info('queueChat filteredAgents', filteredAgents);
      if (filteredAgents?.length === 0) {
        continue;
      }
      const getRouting: any = await getRoutingDetails(profile?.rqmid, channelId?.domainId);
      logger.info('queueChat getRouting', getRouting);

      if (getRouting?.routingMethod !== 'Long wait routing') {
        currentNode.skill = currentNode?.data?.information?.skill;
        currentNode.domainId = channelId?.domainId;
        skillRoutingAgent = await skillRouting(currentNode, filteredAgents);
        if (skillRoutingAgent?.length <= 0) {
          return [];
        }
        filteredAgents = [...skillRoutingAgent];
        logger.info('queueChat skillRoutingAgent', filteredAgents);
      }

      const longWaitRoutingAgent = await longWaitRouting(channelId, currentNode, filteredAgents);
      logger.info('queuChat longWaitRoutingAgent', longWaitRoutingAgent);

      if (longWaitRoutingAgent?.length > 0) {
        return longWaitRoutingAgent;
      }

    }
    return [];
  } catch (err) {
    logger.error(' queueChat err', err);
    return [];
  }

};

/**
 *
 * @param channelId
 * @param readyAgentsData
 * @returns
 */
const skillRouting = async (channelId: any, readyAgentsData: any) => {
  try {
    const skills = channelId.skill;
    logger.info('skillRouting skills--------------->', skills);
    const getAgentSkills: any = await getAgentsSkills(channelId.domainId, '1,2,3,5,6');
    logger.info('skillRouting getAgentSkills------------->', getAgentSkills);
    const skilledAgentsExt = [];
    for (const skill of skills) {
      logger.info('first loop');
      getAgentSkills.filter((agent: any) => {
        if (agent?.addSkill.includes(skill)) {
          skilledAgentsExt.push(+(agent?.ext));
        }
      });
    }
    logger.info('skilledAgentsExt', skilledAgentsExt);
    const filteredUniqueExt = [...new Set(skilledAgentsExt)];
    logger.info('filteredUniqueExt', filteredUniqueExt);
    const skilledAgents = [];
    filteredUniqueExt.map((val) => {
      for (const agent of readyAgentsData) {
        if (+(val) === +(agent.ext)) {
          skilledAgents.push(agent);
        }
      }
    });
    logger.info(' skilledAgents', skilledAgents);
    return skilledAgents;
  } catch (err) {
    logger.error(' skilledAgents err', err);
    return [];
  }
};

const sendAgentDetails = async (channelId: any, agent: any, previousStatus: any) => {
  try {
    if (previousStatus?.length === 0 || previousStatus === undefined) {
      const insertArr: any = [+(agent?.ext)];
      const insertNewStatus: any = await insertStatus(channelId?.WhatsappHistoryId, channelId?.domainId, insertArr);
      logger.info('sendAgentDetails insertNewStatus', insertNewStatus);
    } else {
      const verifyArr: any = JSON.parse(previousStatus[0]?.chatuserlog);
      const insertArr: any = [...verifyArr, +(agent?.ext)];
      const insertNewStatus: any = await insertStatus(channelId?.WhatsappHistoryId, channelId?.domainId, insertArr);
      logger.info('sendAgentDetails insertNewStatus', insertNewStatus);
    }
    return [agent];
  } catch (err) {
    logger.error(' sendAgentDetails err', err);
    return [];
  }
};

/**
 *
 * @param data
 * @param currentData
 * @param queueId
 */
export const insertCustomerInQueue = async (data: any, currentData: any, queueId: any) => {
  try {
    const getCId = await getCustomerCoId(data);
    const queueDetails: any = await getQueueSettingS(queueId, data?.domainId);
    const customerDetails: any = await getCustomerConatctDetails(data?.domainId, getCId[0]);
    const queueWeightTime: any = await getAllQueueWaitTime(data?.domainId);
    const result = queueWeightTime?.filter((list: any) => list?.qid === queueId && list?.queueName === queueDetails?.name && list?.queueType === channelType?.Whatsapp);
    const queueListData = [];
    const waitTimmer: any = JSON.parse(queueDetails?.maximumCallers);
    const waitQueueListData: any = result?.length > 0 ? JSON.parse(result[0]?.queueList) : [];
    if (waitTimmer?.type !== 'Set Max Caller limit' || waitTimmer?.queueLimit > waitQueueListData?.length) {
      const queueTypeData = {
        firstName: customerDetails[0]?.firstName ?? null,
        lastName: customerDetails[0]?.lastName ?? null,
        sessionId: data?.sessionId,
        queuedTime: Math.floor(Date.now() / 1000),
        type: channelType?.Whatsapp,
        chatHistoryId: data?.WhatsappHistoryId,
        queueSetting: queueDetails?.maximumWaitTime,
        queueUserCount: queueDetails?.maximumCallers,
        channelId: data,
        currentNode: currentData
      };
      if (result?.length > 0) {
        const preQueueData: any = JSON.parse(result[0]?.queueList);
        preQueueData?.push(queueTypeData);
        queueListData.push(...preQueueData);
      } else {
        queueListData.push(queueTypeData);
      }
      const payload = {
        qid: queueId ?? 0,
        queueName: queueDetails?.name,
        queueType: channelType?.Whatsapp,
        domainId: data?.domainId,
        queueList: JSON.stringify(queueListData)
      };
      try {
        await insertUpdateQueueWaitTimeDao(payload, queueDetails, data);
      } catch (err) {
        console.log(err);
      }
      ioredisWhatsapp.emit(emitter?.queueWait, payload);
      const redisData = {
        qid: queueId,
        queueType: channelType?.Whatsapp,
        domainId: data?.domainId,
      };
      const redisQueueData: any = await ioredisWhatsappQueue.get(process.env.WHATSAPP_QUEUE);
      if (redisQueueData) {
        const redisMap = JSON.parse(redisQueueData);
        const hasQid = redisMap?.some((item: any) => item?.qid === queueId);
        if (!hasQid) {
          redisMap.push(redisData);
          ioredisWhatsappQueue.set(process.env.WHATSAPP_QUEUE, JSON.stringify(redisMap));
        }
      } else {
        ioredisWhatsappQueue.set(process.env.WHATSAPP_QUEUE, JSON.stringify([redisData]));
      }
    } else {
      data.queueId = waitTimmer?.overFlowQueue;
      insertCustomerInQueue(data, currentData, waitTimmer?.overFlowQueue);
    }
  } catch (err) {
    logger.error('insertCustomerInQueue err', err);
  }
};
