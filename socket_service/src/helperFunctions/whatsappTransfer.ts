import { getAgentByChat, getAgentsSkills, getConcurrentChat, getPreviousStatus, getRoutingDetails, getRoutingProfiles, getRoutingProfileUser, insertStatus } from '../dao/callflow.dao';
import { getUserList } from '../dao/chat.dao';
import { updateQueueIdInsta } from '../dao/socialmedia';
import { updateQueueId, updateQueueIdFacebook } from '../dao/webChat.dao';
import { otherRedisClient } from '../plugins/db';

/**
 *
 * @param channelId
 * @returns
 */
export const transferChatToAvailableAgent = async (channelId: any) => {
  try {

    let agentDetails: any = [];

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

    return agentDetails;
  } catch (err) {
    return [];
  }
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

    } if (currentNode?.data?.information?.type === 1) { // To queue

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

/**
 *
 * @param channel
 * @returns
 */
const readyAgentsSupervisorsAdmins = async (channel: any) => {
  try {
    const readyAgentsSupervisorsAdminsList = [];
    const data = await otherRedisClient.hgetall(`${channel?.domainId}_presence`);
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

/**
 *
 * @param channel
 * @returns
 */
const readyAgents = async (channel: any) => {
  try {
    const readyAgentsList = [];
    const data = await otherRedisClient.hgetall(`${channel?.domainId}_presence`);
    const result = Object.values(data).map((value: any) => JSON.parse(value));
    if (!data) {
      return readyAgentsList;
    }
    for (const item of result) {
      const getData: any = await getUserList(item);
      if (Number(item?.isChat) === 1 && Number(item?.isChatTransfer) === 0 && Number(item?.roleId) === 6) {
        getData.forEach((dataq: any) => {
          if (Number(item.ext) === dataq.ext && dataq.socialMedia === 1) {
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

/**
 *
 * @param channel
 * @returns
 */
const readySupervisors = async (channel: any) => {
  try {
    const readyAgentsList = [];
    const data = await otherRedisClient.hgetall(`${channel?.domainId}_presence`);
    const result = Object.values(data).map((value: any) => JSON.parse(value));
    if (!data) {
      return readyAgentsList;
    }
    for (const item of result) {
      const getData: any = await getUserList(item);
      if (Number(item?.isChat) === 1 && Number(item?.isChatTransfer) === 0 && Number(item?.roleId) === 5) {
        getData.forEach((dataa: any) => {
          if (Number(item.ext) === dataa.ext && dataa.socialMedia === 1) {
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
    const data = await otherRedisClient.hgetall(`${channel?.domainId}_presence`);
    if (!data) {
      return readyAgentsList;
    }
    const result = Object.values(data).map(value => JSON.parse(value));
    for (const item of result) {
      const getData: any = await getUserList(item);
      if (Number(item?.isChat) === 1 && Number(item?.isChatTransfer) === 0 && Number(item?.roleId) === 3) {
        getData.forEach((datav: any) => {
          if (Number(item.ext) === datav.ext && datav.socialMedia === 1) {
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

const longWaitRouting = async (channelId: any, currentNode: any, readyAgentsData: any) => {
  try {
    const sortedAgentsBasedOnTime = readyAgentsData.sort((a: any, b: any) => a.stateTime - b.stateTime);
    console.log('\n longWaitRouting sortedAgentsBasedOnTime\n', JSON.stringify(sortedAgentsBasedOnTime));
    const sortedAgentsBasedOnRole = sortedAgentsBasedOnTime.sort((a: any, b: any) => b.roleId - a.roleId);
    console.log('\n longWaitRouting sortedAgentsBasedOnRole\n', JSON.stringify(sortedAgentsBasedOnRole));
    let filteredNotifiedAgents = [];

    const previousStatus: any = await getPreviousStatus(channelId?.historyId, channelId?.domainId); // chat notified to agents list previously
    console.log('\n longWaitRouting previousStatus\n', previousStatus);

    if (previousStatus?.length !== 0) {

      // redirecting from other agents
      const verifyArr: any = JSON.parse(previousStatus[0]?.chatuserlog);
      console.log('\n longWaitRouting verifyArr\n', verifyArr);
      filteredNotifiedAgents = sortedAgentsBasedOnRole.filter((val: any) => !(verifyArr.includes(+(val?.ext))));

      if (filteredNotifiedAgents?.length === 0) {
        return [];
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

/**
 *
 * @param channel
 * @param currentNode
 * @param filteredData
 * @param previousStatus
 * @returns
 */
const chatLimitFilter = async (channel: any, currentNode: any, filteredData: any, previousStatus: any) => {
  try {
    const agentDetails: any = [];
    const chatLimitData: any = await getConcurrentChat(channel.domainId);
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

      chatLimitByAgent = await getAgentByChat(channel.domainId);
      console.log('\n chatLimitFilter chatLimitByAgent\n', JSON.stringify(chatLimitByAgent));

    } else if (chatLimitData?.isRoutingProfile) {

      routingProfileUser = await getRoutingProfileUser(channel.domainId);
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

const agentChatLimitFilter = async (channel: any, currentNode: any, filteredData: any) => {
  try {
    const chatLimitData: any = await getConcurrentChat(channel.domainId);
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

      chatLimitByAgent = await getAgentByChat(channel.domainId);
      console.log('\n chatLimitFilter chatLimitByAgent\n', JSON.stringify(chatLimitByAgent));

      filteredAgentLimit = chatLimitByAgent.filter((val: any) => +(val?.ext) === +(filteredData?.ext));
      console.log('filteredAgentLimit', filteredAgentLimit);

    } else if (chatLimitData?.isRoutingProfile) {

      routingProfileUser = await getRoutingProfileUser(channel.domainId);
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
        }
        break;
      }
    }
    return [];
  } catch (err) {
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
    console.log('\n queueChat routingProfiles\n', JSON.stringify(routingProfiles));
    if (channelId?.channelType === 'Whatsapp') {
      await updateQueueId(channelId, currentNode?.data?.information?.transferTo);
    } else if (channelId?.channelType === 'Facebook') {
      await updateQueueIdFacebook(channelId, currentNode?.data?.information?.transferTo);
    } else {
      await updateQueueIdInsta(channelId, currentNode?.data?.information?.transferTo);
    }
    if (routingProfiles?.length === 0) {
      return [];
    }

    const sortedRoutingProfiles: any = routingProfiles.sort((a: any, b: any) => a.priority - b.priority); // sorting routing profiles based on priority

    for (const profile of sortedRoutingProfiles) {
      filteredAgents = readyAgentsData.filter((a: any) => a?.routingProfileId === profile?.rqmid);
      console.log('\n queueChat filteredAgents\n', JSON.stringify(filteredAgents));

      if (filteredAgents?.length === 0) {
        continue;
      }

      const getRouting: any = await getRoutingDetails(profile?.rqmid, channelId?.domainId);
      console.log('\n queueChat getRouting\n', JSON.stringify(getRouting));

      if (getRouting?.routingMethod !== 'Long wait routing') {
        currentNode.skill = currentNode?.data?.information?.skill;
        currentNode.domainId = channelId?.domainId;
        skillRoutingAgent = await skillRouting(currentNode, filteredAgents);

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
 * @param channelId
 * @param readyAgentsData
 * @returns
 */
const skillRouting = async (channelId: any, readyAgentsData: any) => {
  try {
    const skills = channelId.skill;
    console.log('\n skillRouting skills--------------->', skills);

    const getAgentSkills: any = await getAgentsSkills(channelId.domainId, '1,2,3,5,6');
    console.log('\n skillRouting getAgentSkills------------->', JSON.stringify(getAgentSkills));

    const skilledAgentsExt = [];

    for (const skill of skills) {
      console.log('first loop');
      getAgentSkills.filter((agent: any) => {
        if (agent?.addSkill.includes(skill)) {
          skilledAgentsExt.push(+(agent?.ext));
        }
      });
    }

    console.log('skilledAgentsExt', skilledAgentsExt);

    const filteredUniqueExt = [...new Set(skilledAgentsExt)];
    console.log('filteredUniqueExt', filteredUniqueExt);

    const skilledAgents = [];

    filteredUniqueExt.map((val) => {
      for (const agent of readyAgentsData) {
        if (+(val) === +(agent.ext)) {
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

/**
 *
 * @param channelId
 * @param agent
 * @param previousStatus
 * @returns
 */
const sendAgentDetails = async (channelId: any, agent: any, previousStatus: any) => {
  try {
    if (previousStatus?.length === 0 || previousStatus === undefined) {
      const insertArr: any = [+(agent?.ext)];
      const insertNewStatus: any = await insertStatus(channelId?.historyId, channelId?.domainId, insertArr);
      console.log('\n sendAgentDetails insertNewStatus\n', insertNewStatus);
    } else {
      const verifyArr: any = JSON.parse(previousStatus[0]?.chatuserlog);
      const insertArr: any = [...verifyArr, +(agent?.ext)];
      const insertNewStatus: any = await insertStatus(channelId?.historyId, channelId?.domainId, insertArr);
      console.log('\n sendAgentDetails insertNewStatus\n', insertNewStatus);
    }
    // await insertSessionForQueueFunc(channelId, currentNode);
    return [agent];
  } catch (err) {
    console.log('\n sendAgentDetails err\n', err);
    return [];
  }
};
