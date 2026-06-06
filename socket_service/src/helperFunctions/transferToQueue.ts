import { v4 as uuidv4 } from 'uuid';
import { getAgentByChat, getAgentsSkills, getConcurrentChat, getIncomingChatDetailsDB, getPreviousStatus, getQueueSettingS, getRoutingDetails, getRoutingProfiles, getRoutingProfileUser, insertIncomingChatsDB, insertStatus } from '../dao/callflow.dao';
import { getUserList } from '../dao/chat.dao';
import { updateChatMessageInsta } from '../dao/socialmedia';
import { getAllQueueWaitTime, getCustomerConatctDetails, insertUpdateQueueWaitTimeDao, updateChatMessageFb, updateWhatsappChatMessage } from '../dao/webChat.dao';
import { ioredisChat, otherRedisClient, queueGetter } from '../plugins/db';
import { logger } from '../plugins/log';

const sendAgentDetails = async (channelId: any, agent: any, previousStatus: any) => {
  try {
    if (previousStatus?.length === 0 || previousStatus === undefined) {
      const insertArr: any = [+(agent?.ext)];
      const insertNewStatus: any = await insertStatus(channelId?.ChatHistoryId, channelId?.domain_id, insertArr);
      console.log('\n sendAgentDetails insertNewStatus\n', insertNewStatus);
    } else {
      const verifyArr: any = JSON.parse(previousStatus[0]?.chatuserlog);
      const insertArr: any = [...verifyArr, +(agent?.ext)];
      const insertNewStatus: any = await insertStatus(channelId?.ChatHistoryId, channelId?.domain_id, insertArr);
      console.log('\n sendAgentDetails insertNewStatus\n', insertNewStatus);
    }
    // await insertSessionForQueueFunc(channelId, currentNode);
    return [agent];
  } catch (err) {
    console.log('\n sendAgentDetails err\n', err);
    return [];
  }
};

const agentChatLimitFilter = async (channel: any, currentNode: any, filteredData: any) => {
  try {
    const chatLimitData: any = await getConcurrentChat(channel.domain_id);
    console.log('\n chatLimitFilter chatLimitData\n', JSON.stringify(chatLimitData));
    // {"domainId":7288,"isAllAgent":1,"allAgentLimit":2,"isAgent":0,"isRoutingProfile":0,"agentOccupyDetails":null}

    if (!chatLimitData) {
      console.log('\n *** Chat limit is not set ***\n');
      const sendAgent = await sendAgentDetails(channel, filteredData, undefined);
      console.log('\n chatLimitFiltersendAgent\n', JSON.stringify(sendAgent));
      return sendAgent;
    }

    const parsedChatLimitData = JSON.parse(chatLimitData?.agentOccupyDetails ?? 'null') || null;
    console.log('\n chatLimitFilter parsedChatLimitData\n', JSON.stringify(parsedChatLimitData));

    if (parsedChatLimitData === null) {
      console.log('\n *** First chat in the domain, chatlimit set');
      const sendAgent = await sendAgentDetails(channel, filteredData, undefined);
      console.log('\n chatLimitFiltersendAgent\n', JSON.stringify(sendAgent));
      return sendAgent;
    }

    const currentChatCount = parsedChatLimitData?.currentChatCount;
    console.log('\n chatLimitFilter currentChatCount\n', JSON.stringify(currentChatCount));

    console.log('filteredData', filteredData);

    let chatLimitByAgent: any;
    let routingProfileUser: any;
    let filteredAgentLimit: any;
    let filteredRoutingProfile: any;

    if (chatLimitData?.isAgent) {

      chatLimitByAgent = await getAgentByChat(channel.domain_id);
      console.log('\n chatLimitFilter chatLimitByAgent\n', JSON.stringify(chatLimitByAgent));

      filteredAgentLimit = chatLimitByAgent.filter((val: any) => +(val?.ext) === +(filteredData?.ext));
      console.log('filteredAgentLimit', filteredAgentLimit);

    } else if (chatLimitData?.isRoutingProfile) {

      routingProfileUser = await getRoutingProfileUser(channel.domain_id);
      console.log('\n chatLimitFilter routingProfileUser\n', JSON.stringify(routingProfileUser));

      filteredRoutingProfile = routingProfileUser.filter((val: any) => +(val?.RoutingProfileId) === +(filteredData?.routingProfileId));
      console.log('filteredRoutingProfile', filteredRoutingProfile);
    }

    for (const [key, value] of Object.entries(currentChatCount)) {
      if (+(filteredData?.ext) === +(key)) {
        console.log('ext matched');
        if (chatLimitData?.isAllAgent) {
          console.log('isAllAgent limit matched');
          console.log('allAgentLimit before', chatLimitData?.allAgentLimit, value);
          if (chatLimitData?.allAgentLimit > value) {
            console.log('allAgentLimit less than value');
            return [filteredData];
          }
        } else if (chatLimitData?.isAgent) {
          console.log('isAgent limit matched');
          console.log('isAgent before', filteredAgentLimit[0]?.chatLimit, value);
          if (+(filteredAgentLimit[0]?.chatLimit) > +(value)) {
            console.log('allAgentLimit less than value');
            return [filteredData];
          }
        } else if (chatLimitData?.isRoutingProfile) {
          console.log('routingprofile limit matched');
          console.log('routingprofile before', filteredRoutingProfile[0]?.concurrentChat, value);
          if (+(filteredRoutingProfile[0]?.concurrentChat) > +(value)) {
            console.log('allAgentLimit less than value');
            return [filteredData];
          }
        } else {
          return [filteredData];
        }
        break;
      }
    }
    return [];
  } catch (err) {
    return [];
  }
};

const chatLimitFilter = async (channel: any, currentNode: any, filteredData: any, previousStatus: any) => {
  try {
    const agentDetails: any = [];
    const chatLimitData: any = await getConcurrentChat(channel.domain_id);
    console.log('\n chatLimitFilter chatLimitData\n', JSON.stringify(chatLimitData));
    // {"domainId":7288,"isAllAgent":1,"allAgentLimit":2,"isAgent":0,"isRoutingProfile":0,"agentOccupyDetails":null}

    if (!chatLimitData) {
      console.log('\n *** Chat limit is not set ***\n');
      const sendAgent = await sendAgentDetails(channel, filteredData, undefined);
      console.log('\n chatLimitFiltersendAgent\n', JSON.stringify(sendAgent));
      return sendAgent;
    }

    const parsedChatLimitData = JSON.parse(chatLimitData?.agentOccupyDetails ?? 'null') || null;
    console.log('\n chatLimitFilter parsedChatLimitData\n', JSON.stringify(parsedChatLimitData));

    if (parsedChatLimitData === null) {
      const sendAgent = await sendAgentDetails(channel, filteredData[0], previousStatus);
      console.log('\n chatLimitFiltersendAgent\n', JSON.stringify(sendAgent));
      return sendAgent;
    }

    const currentChatCount = parsedChatLimitData?.currentChatCount;
    console.log('\n chatLimitFilter currentChatCount\n', JSON.stringify(currentChatCount));

    let chatLimitByAgent: any;
    let routingProfileUser: any;

    if (chatLimitData?.isAgent) {

      chatLimitByAgent = await getAgentByChat(channel.domain_id);
      console.log('\n chatLimitFilter chatLimitByAgent\n', JSON.stringify(chatLimitByAgent));

    } else if (chatLimitData?.isRoutingProfile) {

      routingProfileUser = await getRoutingProfileUser(channel.domain_id);
      console.log('\n chatLimitFilter routingProfileUser\n', JSON.stringify(routingProfileUser));
    }

    if (currentChatCount === null || currentChatCount === undefined) {
      const sendAgent = await sendAgentDetails(channel, filteredData[0], previousStatus);
      console.log('\n chatLimitFilter sendAgent\n', JSON.stringify(sendAgent));
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

    console.log('\n chatLimitFilter appendChatCountToFilteredData\n', JSON.stringify(appendChatCountToFilteredData));

    const chatLimitAchievedAgents: any = [];
    let agentsToBeAssigned: any = [...appendChatCountToFilteredData];

    if (chatLimitData?.isAllAgent || chatLimitData?.isAgent || chatLimitData?.isRoutingProfile) {

      let tempAgent: any;

      const checkLiveAgent = () => {
        console.log('checkLiveAgent called------>', tempAgent);
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
              console.log('map--->', tempAgent.currentChatCount, list.chatLimit, tempAgent.ext, list.ext);
              if (+(tempAgent.ext) === +(list.ext)) {
                if (tempAgent?.currentChatCount < list.chatLimit) {
                  console.log('\n condition met, agent added', tempAgent);
                  agentDetails.push(tempAgent);
                } else {
                  console.log('condition not met, agent removed from list');
                  chatLimitAchievedAgents.push(tempAgent);
                  agentsToBeAssigned = agentsToBeAssigned.filter((val: any) => val.ext !== tempAgent.ext);
                  assignFunction();
                }
              }
            });
          } else if (chatLimitData?.isRoutingProfile) {
            console.log('isRoutingProfile entered');
            const routings = {};
            routingProfileUser.map((item: any) => {
              routings[item.RoutingProfileId] = item.concurrentChat;
            });
            console.log('routings', routings);
            for (const [key, value] of Object.entries(routings)) {
              console.log('key', key, 'id', tempAgent.routingProfileId);
              if (+(tempAgent.routingProfileId) === +(key)) {
                if (tempAgent.currentChatCount < value) {
                  console.log('\n condition met, agent added', tempAgent);
                  agentDetails.push(tempAgent);
                } else {
                  console.log('condition not met, agent removed from list');
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
        console.log('assignFunction called------>');
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
      console.log('\n *** Chat limit turned off ***\n');
      const sendAgent = await sendAgentDetails(channel, filteredData[0], undefined);
      console.log('\n sendAgent\n', JSON.stringify(sendAgent));
      return sendAgent;
    }
    if (agentDetails?.length > 0) {
      const sendAgent = await sendAgentDetails(channel, agentDetails[0], previousStatus);
      console.log('\n chatLimitFilter sendAgent\n', JSON.stringify(sendAgent));
      return sendAgent;
    }
    return agentDetails;
  } catch (err) {
    console.log('\n chatLimitFilter err\n', err);
    return [];
  }
};

export const longWaitRouting = async (channelId: any, currentNode: any, readyAgentsData: any) => {
  try {
    const sortedAgentsBasedOnTime = readyAgentsData.sort((a: any, b: any) => a.stateTime - b.stateTime);
    console.log('\n longWaitRouting sortedAgentsBasedOnTime\n', JSON.stringify(sortedAgentsBasedOnTime));
    const sortedAgentsBasedOnRole = sortedAgentsBasedOnTime.sort((a: any, b: any) => b.roleId - a.roleId);
    console.log('\n longWaitRouting sortedAgentsBasedOnRole\n', JSON.stringify(sortedAgentsBasedOnRole));
    let filteredNotifiedAgents = [];

    const previousStatus: any = await getPreviousStatus(channelId?.ChatHistoryId, channelId?.domain_id); // chat notified to agents list previously
    console.log('\n longWaitRouting previousStatus\n', previousStatus);

    if (previousStatus?.length !== 0) {

      // redirecting from other agents
      const verifyArr: any = JSON.parse(previousStatus[0]?.chatuserlog);
      console.log('\n longWaitRouting verifyArr\n', verifyArr, sortedAgentsBasedOnRole);
      if (sortedAgentsBasedOnRole?.length > 1) {
        filteredNotifiedAgents = sortedAgentsBasedOnRole.filter((val: any) => !(verifyArr.includes(+(parseInt(val?.ext, 10)))));
      } else {
        filteredNotifiedAgents = sortedAgentsBasedOnRole;
      }
      if (filteredNotifiedAgents?.length === 0) {
        const verifyUserArr = [verifyArr.pop()];
        filteredNotifiedAgents = sortedAgentsBasedOnRole.filter((val: any) => !(verifyUserArr.includes(+(parseInt(val?.ext, 10)))));
        if (filteredNotifiedAgents?.length === 0) {
          return [];
        }
      }
    } else {
      filteredNotifiedAgents = [...sortedAgentsBasedOnRole];
    }
    console.log('\n longWaitRouting filteredNotifiedAgents\n', JSON.stringify(filteredNotifiedAgents));

    const chatLimitCheckedAgent: any = await chatLimitFilter(channelId, currentNode, filteredNotifiedAgents, previousStatus);
    console.log('\n longWaitRouting chatLimitCheckedAgent\n', JSON.stringify(chatLimitCheckedAgent));
    return chatLimitCheckedAgent;

  } catch (err) {
    console.log('\n longWaitRouting err\n', err);
    return [];
  }

};

const skillRouting = async (channelId: any, readyAgentsData: any) => {
  try {
    const skills = channelId?.skills;
    const getAgentSkills: any = await getAgentsSkills(channelId.domain_id, '1,2,3,5,6');
    const skilledAgentsExt = [];

    for (const skill of skills) {
      getAgentSkills.filter((agent: any) => {
        if (agent?.addSkill?.includes(skill)) {
          skilledAgentsExt.push(+(agent?.ext));
        }
      });

      // for (const agent of getAgentSkills) {
      //   console.log('second loop', agent);
      //   console.log('agent.ext', agent?.ext);
      //   if (agent?.addSkill.includes(skill)) {
      //     console.log('entered', agent?.ext);
      //     skilledAgentsExt.push(+(agent?.ext));
      //   }
      // }
    }

    console.log('skilledAgentsExt', skilledAgentsExt);

    const filteredUniqueExt = [...new Set(skilledAgentsExt)];
    console.log('filteredUniqueExt', filteredUniqueExt);

    const skilledAgents = [];

    filteredUniqueExt.map((val) => {
      for (const agent of readyAgentsData) {
        if (+(`${val}`) === +(`${agent.ext}`)) {
          skilledAgents.push(agent);
        }
      }
    });

    console.log('\n skilledAgents\n', JSON.stringify(skilledAgents));
    return skilledAgents;
  } catch (err) {
    console.log('\n skillRouting err\n', err);
    return [];
  }
};

const queueChat = async (channelId: any, currentNode: any, readyAgentsData: any) => {
  try {
    let filteredAgents: any = [];
    let skillRoutingAgent: any = [];

    const routingProfiles: any = await getRoutingProfiles(channelId?.queueId, channelId?.domain_id); // getting routing profiles mapped to queue
    console.log('\n queueChat routingProfiles\n', JSON.stringify(routingProfiles));

    if (routingProfiles?.length === 0) {
      return [];
    }

    const sortedRoutingProfiles: any = routingProfiles.sort((a: any, b: any) => a.priority - b.priority); // sorting routing profiles based on priority

    for (const profile of sortedRoutingProfiles) {
      filteredAgents = readyAgentsData.filter((a: any) => a?.routingProfileId === profile?.rqmid && profile?.chat === 1);
      console.log('\n queueChat filteredAgents\n', JSON.stringify(filteredAgents));

      if (filteredAgents?.length === 0) {
        continue;
      }

      const getRouting: any = await getRoutingDetails(profile?.rqmid, channelId?.domain_id);
      console.log('\n queueChat getRouting\n', JSON.stringify(getRouting));

      if (getRouting?.routingMethod !== 'Long wait routing') {
        skillRoutingAgent = await skillRouting(channelId, filteredAgents);

        if (skillRoutingAgent?.length <= 0) {
          return [];
        }

        filteredAgents = [...skillRoutingAgent];
        console.log('\n\n queueChat skillRoutingAgent\n', JSON.stringify(skillRoutingAgent));

      }

      const longWaitRoutingAgent = await longWaitRouting(channelId, currentNode, filteredAgents);
      console.log('\n\n queuChat longWaitRoutingAgent\n', JSON.stringify(longWaitRoutingAgent));

      if (longWaitRoutingAgent?.length > 0) {
        return longWaitRoutingAgent;
      }

    }
    return [];
  } catch (err) {
    console.log('\n queueChat err\n', err);
    return [];
  }

};

const readyAgentsSupervisorsAdmins = async (channel: any) => {
  try {
    const readyAgentsSupervisorsAdminsList = [];
    const data = await otherRedisClient.hgetall(`${channel?.domain_id}_presence`);
    const result = Object.values(data).map((value: any) => JSON.parse(value));
    if (!data) {
      return readyAgentsSupervisorsAdminsList;
    }
    for (const item of result) {
      const getData: any = await getUserList(item);
      if (Number(item?.isChat) === 1 && Number(item?.isChatTransfer) === 0) {
        getData.forEach((datas: any) => {
          if (Number(item.ext) === datas.ext && datas.chat === 1) {
            readyAgentsSupervisorsAdminsList.push(item);
          }
        });
      }
    }
    return readyAgentsSupervisorsAdminsList;
  } catch (err) {
    return [];
  }
};

const readyAgents = async (channel: any) => {
  try {
    const readyAgentsList = [];
    const data = await otherRedisClient.hgetall(`${channel?.domain_id}_presence`);
    const result = Object.values(data).map((value: any) => JSON.parse(value));
    if (!data) {
      return readyAgentsList;
    }
    const getData: any = await getUserList(result[0]);
    for (const item of result) {
      // if (Number(item?.isChat) === 1 && Number(item?.isChatTransfer) === 0 && Number(item?.roleId) === 6) {
      if (Number(item?.isChat) === 1 && Number(item?.isChatTransfer) === 0) {
        getData.forEach((dataq: any) => {
          if (Number(item.ext) === dataq.ext && dataq.chat === 1) {
            readyAgentsList.push(item);
          }
        });
      }
    }
    return readyAgentsList;
  } catch (err) {
    return [];
  }
};

const readySupervisors = async (channel: any) => {
  try {
    const readyAgentsList = [];
    const data = await otherRedisClient.hgetall(`${channel?.domain_id}_presence`);
    const result = Object.values(data).map((value: any) => JSON.parse(value));
    if (!data) {
      return readyAgentsList;
    }
    for (const item of result) {
      const getData: any = await getUserList(item);
      if (Number(item?.isChat) === 1 && Number(item?.isChatTransfer) === 0 && Number(item?.roleId) === 5) {
        getData.forEach((dataa: any) => {
          if (Number(item.ext) === dataa.ext && dataa.chat === 1) {
            readyAgentsList.push(item);
          }
        });
      }
    }
    return readyAgentsList;
  } catch (err) {
    return [];
  }
};

const readyAdmins = async (channel: any) => {
  try {
    const readyAgentsList = [];
    const data = await otherRedisClient.hgetall(`${channel?.domain_id}_presence`);
    if (!data) {
      return readyAgentsList;
    }
    const result = Object.values(data).map(value => JSON.parse(value));
    for (const item of result) {
      const getData: any = await getUserList(item);
      if (Number(item?.isChat) === 1 && Number(item?.isChatTransfer) === 0 && Number(item?.roleId) === 3) {
        getData.forEach((datav: any) => {
          if (Number(item.ext) === datav.ext && datav.chat === 1) {
            readyAgentsList.push(item);
          }
        });
      }
    }
    return readyAgentsList;
  } catch (err) {
    console.error(err);
    return [];
  }
};

export const transferChatToAgent = async (channelId: any, currentNode: any) => {

  // roleId

  // 6 Agent, 5 Supervisor, 3 Admin

};

export const transferChat = async (channelId: any, currentNode: any) => {
  try {

    console.log('\n transferChat currentNode\n', JSON.stringify(currentNode));
    console.log('\n transferChat channelId\n', JSON.stringify(channelId));

    let agentDetails: any = [];

    if (currentNode?.data?.information?.type === 2) { // To direct agent

      const readyAgentsSupervisorsAdminsData: any = await readyAgentsSupervisorsAdmins(channelId); // checking agents in ready state
      console.log('\n readyAgentsSupervisorsAdminsData\n', JSON.stringify(readyAgentsSupervisorsAdminsData));

      if (readyAgentsSupervisorsAdminsData?.length === 0) { // returning when no agents available
        return agentDetails;
      }

      const filteredAgent = readyAgentsSupervisorsAdminsData?.filter((val: any) => val?.userId === currentNode?.data?.information?.transferTo); // checking agent ready status
      agentDetails = agentChatLimitFilter(channelId, currentNode, filteredAgent[0]);

    }
    if (currentNode?.data?.information?.type === 1) { // To queue

      const readyAgentsData: any = await readyAgents(channelId); // checking agents in ready state
      console.log('\n readyAgentsData\n', JSON.stringify(readyAgentsData));

      if (readyAgentsData?.length !== 0) { // Connecting chat with agents

        agentDetails = await queueChat(channelId, currentNode, readyAgentsData);
        console.log('\ntransferChat readyAgentsData agentDetails\n', JSON.stringify(agentDetails));
      }

      if (agentDetails.length === 0) {
        const readySupervisorsData: any = await readySupervisors(channelId); // checking agents in ready state
        console.log('\n readySupervisorsData\n', JSON.stringify(readySupervisorsData));

        if (readySupervisorsData?.length !== 0) { // Connecting chat with supervisors

          agentDetails = await queueChat(channelId, currentNode, readySupervisorsData);
          console.log('\ntransferChat readySupervisorsData agentDetails\n', JSON.stringify(agentDetails));
        }
      }

      if (agentDetails.length === 0) {
        const readyAdminsData: any = await readyAdmins(channelId); // checking agents in ready state
        console.log('\n readyAdminsData\n', JSON.stringify(readyAdminsData));

        if (readyAdminsData?.length !== 0) { // Connecting chat with admins

          agentDetails = await queueChat(channelId, currentNode, readyAdminsData);
          console.log('\ntransferChat readyAdminsData agentDetails\n', JSON.stringify(agentDetails));
        }
      }
    }
    return agentDetails;

  } catch (err) {
    console.log('err', err);
    return [];
  }
};

export const transferWithoutRouting = async (channelId: any, currentNode: any) => {
  try {
    let filteredAgents: any = [];
    let skillRoutingAgent: any = [];
    const readyAgentsData: any = await readyAgents(channelId);
    skillRoutingAgent = await skillRouting(channelId, readyAgentsData);
    if (skillRoutingAgent?.length <= 0) {
      return [];
    }
    filteredAgents = [...skillRoutingAgent];
    return filteredAgents;

  } catch (err) {
    console.log('\n queueChat err\n', err);
    return [];
  }
};

export const transferChatToAvailableAgent = async (channelId: any) => {
  try {

    let agentDetails: any = [];
    logger.info(`agentDetails Socket Error: ${JSON.stringify(agentDetails)}`);
    const readyAgentsData: any = await readyAgents(channelId); // checking agents in ready state
    console.log('\n readyAgentsData\n', JSON.stringify(readyAgentsData));

    if (readyAgentsData?.length !== 0) { // Connecting chat with agents

      agentDetails = await longWaitRouting(channelId, undefined, readyAgentsData);
      console.log('\n\n liveagent agentDetails\n', JSON.stringify(agentDetails));
    }

    if (agentDetails.length === 0) {
      const readySupervisorsData: any = await readySupervisors(channelId); // checking supervisors in ready state
      console.log('\n readySupervisorsData\n', JSON.stringify(readySupervisorsData));

      if (readySupervisorsData?.length !== 0) { // Connecting chat with supervisors

        agentDetails = await longWaitRouting(channelId, undefined, readySupervisorsData);
        console.log('\n\n liveagent agentDetails\n', JSON.stringify(agentDetails));
      }
    }

    if (agentDetails.length === 0) {
      const readyAdminsData: any = await readyAdmins(channelId); // checking admins in ready state
      console.log('\n readyAdminsData\n', JSON.stringify(readyAdminsData));

      if (readyAdminsData?.length !== 0) { // Connecting chat with admins
        agentDetails = await longWaitRouting(channelId, undefined, readyAdminsData);
        console.log('\n\n liveagent agentDetails\n', JSON.stringify(agentDetails));
      }
    }
    logger.info(`agentDetails12 Socket Error: ${JSON.stringify(agentDetails)}`);
    return agentDetails;
  } catch (err) {
    return [];
  }
};

export const insertCustomerInQueue = async (data: any, currentData: any, queueId: any) => {
  try {
    const queueDetails: any = await getQueueSettingS(queueId, data?.domain_id);
    const customerDetails: any = await getCustomerConatctDetails(data?.domain_id, data?.customerId);
    const queueWeightTime: any = await getAllQueueWaitTime(data?.domain_id);
    const result = queueWeightTime?.filter((list: any) => list?.qid === queueId && list?.queueName === queueDetails?.name && list?.queueType === 'chat');
    const queueListData = [];
    const waitTimmer: any = JSON.parse(queueDetails?.maximumCallers);
    const waitQueueListData: any = result?.length > 0 ? JSON.parse(result[0]?.queueList) : [];
    if (waitTimmer?.type !== 'Set Max Caller limit' || waitTimmer?.queueLimit > waitQueueListData?.length) {
      const queueTypeData = {
        firstName: customerDetails[0]?.firstName ?? null,
        lastName: customerDetails[0]?.lastName ?? null,
        sessionId: data?.session_id,
        queuedTime: Math.floor(Date.now() / 1000),
        type: 'Chat',
        chatHistoryId: data?.ChatHistoryId,
        queueSetting: queueDetails?.maximumWaitTime,
        queueUserCount: queueDetails?.maximumCallers,
        queueAnnouncement: queueDetails?.announcement,
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
        queueType: 'chat',
        domainId: data?.domain_id,
        queueList: JSON.stringify(queueListData)
      };
      try {
        await insertUpdateQueueWaitTimeDao(payload);
        ioredisChat.to(data?.session_id).emit('changeFallBackQueue', payload);
      } catch (err) {
        console.log(err);
      }
      ioredisChat.emit('callQueueWait', payload);
      const redisData = {
        qid: queueId,
        queueType: 'chat',
        domainId: data?.domain_id,
      };
      const redisQueueData: any = await queueGetter.get('queuedCustomer');
      if (redisQueueData) {
        const redisMap = JSON.parse(redisQueueData);
        const hasQid = redisMap?.some((item: any) => item.qid === queueId);
        if (!hasQid) {
          redisMap.push(redisData);
          queueGetter.set('queuedCustomer', JSON.stringify(redisMap));
        }
      } else {
        queueGetter.set('queuedCustomer', JSON.stringify([redisData]));
      }
    } else {
      data.queueId = waitTimmer?.overFlowQueue;
      insertCustomerInQueue(data, currentData, waitTimmer?.overFlowQueue);
    }
    return;
  } catch (err) {
    console.log(err);
  }
};

// Transfer chat for queued customer

export const transferChatForQueuedCustomer = async (channelId: any, currentNode: any, agentExtList: any) => {
  try {
    console.log('agentExtList::::::', agentExtList);
    let agentDetails: any = [];
    // const fullAgentDetails: any = [];
    if (currentNode?.data?.information?.type === 1) {
      const readyAgentsData: any = await readyAgents(channelId);
      console.log('\n readyAgentsData\n', JSON.stringify(readyAgentsData));
      if (readyAgentsData?.length !== 0) {
        agentDetails = await queueChatQueued(channelId, currentNode, readyAgentsData);
        // fullAgentDetails.push([...agentDetails]);
        console.log('\ntransferChat readyAgentsData agentDetails\n', JSON.stringify(agentDetails));
      }
      if (agentDetails.length === 0) {
        const readySupervisorsData: any = await readySupervisors(channelId);
        console.log('\n readySupervisorsData\n', JSON.stringify(readySupervisorsData));
        if (readySupervisorsData?.length !== 0) {
          agentDetails = await queueChatQueued(channelId, currentNode, readySupervisorsData);
          // fullAgentDetails.push([...agentDetails]);
          console.log('\ntransferChat readySupervisorsData agentDetails\n', JSON.stringify(agentDetails));
        }
      }
      if (agentDetails.length === 0) {
        const readyAdminsData: any = await readyAdmins(channelId);
        console.log('\n readyAdminsData\n', JSON.stringify(readyAdminsData));
        if (readyAdminsData?.length !== 0) {
          agentDetails = await queueChatQueued(channelId, currentNode, readyAdminsData);
          // fullAgentDetails.push([...agentDetails]);
          console.log('\ntransferChat readyAdminsData agentDetails\n', JSON.stringify(agentDetails));
        }
      }
    }
    return agentDetails;
  } catch (err) {
    console.log('err', err);
    return [];
  }
};

const queueChatQueued = async (channelId: any, currentNode: any, readyAgentsData: any) => {
  try {
    let filteredAgents: any = [];
    let skillRoutingAgent: any = [];
    const routingProfiles: any = await getRoutingProfiles(channelId?.queueId, channelId?.domain_id); // getting routing profiles mapped to queue
    console.log('\n queueChat routingProfiles\n', JSON.stringify(routingProfiles));
    if (routingProfiles?.length === 0) {
      return [];
    }
    const sortedRoutingProfiles: any = routingProfiles.sort((a: any, b: any) => a.priority - b.priority); // sorting routing profiles based on priority
    for (const profile of sortedRoutingProfiles) {
      filteredAgents = readyAgentsData.filter((a: any) => a?.routingProfileId === profile?.rqmid && profile?.chat === 1);
      console.log('\n queueChat filteredAgents\n', JSON.stringify(filteredAgents));
      if (filteredAgents?.length === 0) {
        continue;
      }
      const getRouting: any = await getRoutingDetails(profile?.rqmid, channelId?.domain_id);
      console.log('\n queueChat getRouting\n', JSON.stringify(getRouting));
      if (getRouting?.routingMethod !== 'Long wait routing') {
        skillRoutingAgent = await skillRouting(channelId, filteredAgents);

        if (skillRoutingAgent?.length <= 0) {
          return [];
        }
        filteredAgents = [...skillRoutingAgent];
        console.log('\n\n queueChat skillRoutingAgent\n', JSON.stringify(skillRoutingAgent));
      }
      const longWaitRoutingAgent = await longWaitRouting(channelId, currentNode, filteredAgents);
      console.log('\n\n queuChat longWaitRoutingAgent\n', JSON.stringify(longWaitRoutingAgent));

      if (longWaitRoutingAgent?.length > 0) {
        return longWaitRoutingAgent;
      }
    }
    return [];
  } catch (err) {
    console.log('\n queueChat err\n', err);
    return [];
  }

};

/**
 *
 * @param room
 * @returns
 */
export const sendLiveNotification = async (room: any) => {
  return new Promise(async (resolve, reject) => {
    let message: any = {};
    try {
      message = {
        messageID: uuidv4(),
        sessionId: room?.sessionId,
        message: 'Connecting to live agent...',
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
      reject(error);
    }
  });
};

/**
 *
 * @param room
 * @returns
 */
export const sendLiveNotificationFb = async (room: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      let message = {};
      message = {
        messageID: uuidv4(),
        sessionId: room?.sessionId,
        message: 'Connecting to live agent...',
        sendAt: Date.now(),
        messageType: 'text',
        sendMediaUrl: '',
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
      reject(error);
    }
  });
};

/**
 *
 * @param room
 */
export const sendLiveNotificationInsta = async (room: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      let message = {};
      message = {
        messageID: uuidv4(),
        sessionId: room?.sessionId,
        message: 'Connecting to live agent...',
        sendAt: Date.now(),
        messageType: 'text',
        sendMediaUrl: '',
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
      reject(error);
    }
  });
};
