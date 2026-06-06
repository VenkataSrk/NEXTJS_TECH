import _ from 'lodash';
import moment from 'moment';
import { ChatFlowAdapters } from './chatflow2.0';
import { getAgentByChat, getAgentsSkills, getConcurrentChat, getHoursofOperation, getIncomingChatDetailsDB, getQueueSettings, getRoutingProfile, getRoutingProfileUser, insertIncomingChatsDB } from './dao/callflow.dao';
import { chatUpdateDao, getAllUserList } from './dao/chat.dao';
import { ioredisChat, otherRedisClient, redisClient } from './plugins/db';
import { logger } from './plugins/log';
import { RedisAdapters } from './redisAdapters';

export const contextPattern = /\${(.*?)}/g;

export const replacePatternWithVariables = (availableContexts: any, text: any, context: any) => {
  let originalText = text;
  for (const ctx of availableContexts) {
    const ctxString = ctx.toString();
    const variableName = ctxString.slice(2, ctxString.length - 1);
    const variable = variableName.split('.')[1];
    const specialPattern = /[${}]/g;
    originalText = originalText.replace(specialPattern, '');
    const patternString = `\\bcontext.${variable}\\b`; // Creating pattern with a dynamic string
    const pattern = new RegExp(patternString, 'gi');
    originalText = originalText?.replace(pattern, context[variable]);
  }
  return originalText;
};
export const replacechatPatternWithVariables = (availableContexts: any, text: any, context: any) => {
  let originalText = text;
  for (const ctx of availableContexts) {
    const ctxString = ctx.toString();
    const variableName = ctxString.slice(2, ctxString.length - 1);
    const variable = variableName.split('.')[1];
    const specialPattern = /[${}]/g;
    originalText = originalText.replace(specialPattern, '');
    const patternString = `\\bcontext.${variable}\\b`; // Creating pattern with a dynamic string
    const pattern = new RegExp(patternString, 'gi');
    /* tslint:disable */
    const test = _.find(context, function (o) { return o.originalkey === variable; });
    console.log(test, 'pattern');
    // originalText = originalText?.replace(pattern, test[variable]);
    originalText = test?.originalkey === "registerdate" ? originalText?.replace(pattern, moment(new Date(test[variable])).format('YYYY-MM-DD'))
      : test !== undefined ? originalText?.replace(pattern, test[variable]) : text;
  }
  return originalText;
};

export const replaceContextWithVariables = (text: any, context: any) => {
  const originalText = text;
  const variableName = originalText?.slice(2, originalText?.length - 1);
  const variable = variableName.split('.')[1];
  if (context[variable]) {
    return context[variable];
  }
  return variable;
};

export const handleTransferToQueue = async (adaptor: any, channel: any, targetNode: any, agentDetails: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      const queueSettings: any = await getQueueSettings(targetNode.data?.information?.transferTo);
      const validateHours: any = true; // await getHoursofOperation(targetNode.data?.information?.transferTo);
      // const agentCount: any = await getAgentCount(targetNode.data?.information?.transferTo);
      const mostSuitableAgent = agentDetails;
      targetNode.data.information.agentDetials = mostSuitableAgent;
      const skillName = mostSuitableAgent?.add_skill ? JSON.parse(mostSuitableAgent?.add_skill ?? 'null')?.at(0)?.value : '';
      // await chatUpdateDao({ ...channel, skillName, domainId: channel?.domain_id, sessionId: channel?.session_id, agentName: mostSuitableAgent?.name });
      const queueData = {
        ...queueSettings,
        settingAudio: JSON.parse(queueSettings?.settingAudio ?? 'null'),
        announcement: JSON.parse(queueSettings?.announcement ?? 'null'),
        maximumCallers: JSON.parse(queueSettings?.maximumCallers ?? 'null'),
        maximumWaitTime: JSON.parse(queueSettings?.maximumWaitTime ?? 'null'),
        outOfBusinessHours: JSON.parse(queueSettings?.outOfBusinessHours ?? 'null'),
      };

      targetNode.data.information.queueSettings = queueData;
      targetNode.data.information.queueSettings.hoursOfOperation = validateHours ? 1 : 0;
      targetNode.data.information.queueSettings.agentCount = agentDetails?.length ?? 0;
      const targetRestructured = { ...targetNode, session_id: channel?.session_id, data: { ...targetNode?.data?.information }, req_type: 'QUEUE_CONNECT' };
      adaptor?.io?.of('/')?.to(channel?.session_id).emit('availableagent', JSON.stringify(targetRestructured));
      logger.info(`availableagent Socket Error: ${JSON.stringify(targetRestructured)}`);
      // adaptor.emitMessage(channel?.session_id, 'get_queue_details', { message: targetRestructured });
      // if (channel?.channelType === 'Chat') {
      //   //adaptor.publishMessageToWebChat(channel?.domain_id, JSON.stringify(targetRestructured));

      //   adaptor.emitMessage(channel?.session_id, 'get_queue_details', { message: targetRestructured });
      // } else {
      //   adaptor.publishMessage(JSON.stringify(targetRestructured));
      // }
      const nextNode = targetNode?.data?.children?.find((node: any) => node?.type === 'success') ?? { childId: '' };
      resolve(nextNode?.childId);
    } catch (error: any) {
      console.log(error);
      const nextNode = targetNode?.data?.children?.find((node: any) => node?.type === 'error') ?? { childId: '' };
      reject(nextNode?.childId);
    }
  });
};


export const handleTransferToLiveAgent = async (adaptor: any, channel: any, targetNode: any, agentDetails: any) => {
  console.log('handleTransferToLiveAgent agentDetails', agentDetails);
  return new Promise(async (resolve, reject) => {
    try {
      const mostSuitableAgent: any = { agentDetials: agentDetails };
      const data: any = { data: mostSuitableAgent }
      adaptor?.io?.of('/')?.to(channel?.session_id).emit('availableagent', JSON.stringify(data));
      logger.info(`availableagent Socket Error: ${JSON.stringify(data)}`);
    } catch (error: any) {
      console.log(error);
    }
  });
};

export const handleTransferToLiveAgentToQueue = async (channel: any, targetNode: any, agentDetails: any) => {
  console.log('handleTransferToLiveAgentToQueue agentDetails', agentDetails, 'channel', channel);
  return new Promise(async (resolve, reject) => {
    try {
      const mostSuitableAgent: any = { agentDetials: agentDetails };
      const data: any = { data: mostSuitableAgent };
      ioredisChat?.to(channel?.session_id).emit('availableagent', JSON.stringify(data));
      logger.info(`availableagent Socket Error: ${JSON.stringify(data)}`);
      resolve([]);
    } catch (error: any) {
      console.log(error);
    }
  });
};

export const deleteSessionDetails = async (data: any) => {
  try {
    let queueArray: any = [];
    let agentUserId: any = '';
    const queueId: any = await getAllUserList(data);
    queueId.map((list: any) => {
      if (data.ext == list.ext) {
        agentUserId = list.UserID;
        queueArray = (list.quId).split(",");
      }
    });
    let flag = false;
    queueArray.map(async (val: any) => {
      if (flag) return;
      const getIncomingChatDetails: any = await getIncomingChatDetailsDB(data, val);
      console.log('updateSessionDetails getIncomingChatDetails', getIncomingChatDetails);
      const getIncomingChats: any = JSON.parse(getIncomingChatDetails[0]?.queueList ?? '[]');
      // console.log('updateSessionDetails getIncomingChats', getIncomingChats);

      if (getIncomingChats.length > 0) {
        const temp = getIncomingChats.filter((val: any) => val.session_id != data.to);
        getIncomingChatDetails[0].queueList = temp;
        const insertIncomingChats = await insertIncomingChatsDB(getIncomingChatDetails[0], val)
        if (insertIncomingChats[0].errCode == 0) {
          console.log('Deleted Successfully!!!');
        }
        // getIncomingChats?.map(async (value: any) => {
        //   // console.log('updateSessionDetails value', value, 'data', data);
        //   if (value?.session_id == data.to) {
        //     temp.push({ ...value, sentToAgent: 0 });

        //     // console.log('updateSessionDetails getIncomingChatDetails[0]', getIncomingChatDetails[0]);

        //     // Assuming insertIncomingChatsDB returns a Promise


        //       // console.log('updateSessionDetails insertIncomingChats if entered', insertIncomingChats[0].errCode);
        //       flag = true;
        //       sendQueueUser(data);
        //       // flag = true
        //     }
        //   }
        // });
      }
    });
    if (!flag) {
      const getIncomingChatDetailsUsingUserID: any = await getIncomingChatDetailsDB(data, agentUserId);

      const getIncomingChatsUsingUserID: any = JSON.parse(getIncomingChatDetailsUsingUserID[0]?.queueList ?? '[]');
      if (getIncomingChatsUsingUserID.length > 0) {
        const temp = [];
        await Promise.all(getIncomingChatsUsingUserID?.map(async (value: any) => {
          if (value?.session_id == data.to) {
            temp.push({ ...value, sentToAgent: 0 });
            getIncomingChatDetailsUsingUserID[0].queueList = temp;
            const insertIncomingChatsUsingUserID = await insertIncomingChatsDB(getIncomingChatDetailsUsingUserID[0], agentUserId)
            console.log('updateSessionDetails insertIncomingChatsUsingUserID', insertIncomingChatsUsingUserID);
            if (insertIncomingChatsUsingUserID[0].errCode == 0) {
              // console.log('updateSessionDetails insertIncomingChatsUsingUserID if entered', insertIncomingChatsUsingUserID[0].errCode);
              sendQueueUser(data);
            }
          }
        }));
      }
    }
  } catch (error) {
    console.log('error', error);
    // reject(error);
  }

  // });

}

export const updateSessionDetails = async (data: any) => {
  try {
    let queueArray: any = [];
    let agentUserId: any = '';
    const queueId: any = await getAllUserList(data);
    queueId.map((list: any) => {
      if (data.ext == list.ext) {
        agentUserId = list.UserID;
        queueArray = (list.quId).split(",");
      }
    });
    let flag = false;
    queueArray.map(async (val: any) => {
      if (flag) return;
      const getIncomingChatDetails: any = await getIncomingChatDetailsDB(data, val);
      console.log('updateSessionDetails getIncomingChatDetails', getIncomingChatDetails);
      const getIncomingChats: any = JSON.parse(getIncomingChatDetails[0]?.queueList ?? '[]');
      if (getIncomingChats.length > 0) {
        const temp = [];
        getIncomingChats?.map(async (value: any) => {
          if (value?.session_id == data.to) {
            temp.push({ ...value, sentToAgent: 0 });
            getIncomingChatDetails[0].queueList = temp;
            // Assuming insertIncomingChatsDB returns a Promise
            const insertIncomingChats = await insertIncomingChatsDB(getIncomingChatDetails[0], val)
            if (insertIncomingChats[0].errCode == 0) {
              flag = true;
              sendQueueUser(data);
              // flag = true
            }
          }
        });
      }
    });
    if (!flag) {
      const getIncomingChatDetailsUsingUserID: any = await getIncomingChatDetailsDB(data, agentUserId);
      console.log('updateSessionDetails getIncomingChatDetailsUsingUserID', getIncomingChatDetailsUsingUserID);
      const getIncomingChatsUsingUserID: any = JSON.parse(getIncomingChatDetailsUsingUserID[0]?.queueList ?? '[]');
      if (getIncomingChatsUsingUserID.length > 0) {
        const temp = [];
        await Promise.all(getIncomingChatsUsingUserID?.map(async (value: any) => {
          if (value?.session_id == data.to) {
            temp.push({ ...value, sentToAgent: 0 });
            getIncomingChatDetailsUsingUserID[0].queueList = temp;
            const insertIncomingChatsUsingUserID = await insertIncomingChatsDB(getIncomingChatDetailsUsingUserID[0], agentUserId)
            if (insertIncomingChatsUsingUserID[0].errCode == 0) {
              sendQueueUser(data);
            }
          }
        }));
      }
    }
  } catch (error) {
    console.log('error', error);
    // reject(error);
  }

  // });

}

export const sendQueueUser = async (data: any) => {
  try {
    let queueArray = [];
    let agentUserId: any = '';
    const queueId: any = await getAllUserList(data)
    // console.log('sendQueueUser queueId', queueId)
    queueId.map((list: any) => {
      if (data.ext == list.ext) {
        agentUserId = list.UserID;
        queueArray = (list.quId).split(",");
      }
    })
    let flag = false;
    queueArray.map(async (val) => {
      if (flag) return;
      const getIncomingChatDetails: any = await getIncomingChatDetailsDB(data, val);
      console.log('sendQueueUser getIncomingChatDetails', getIncomingChatDetails);
      const getIncomingChats: any = JSON.parse(getIncomingChatDetails[0]?.queueList ?? '[]');
      if (getIncomingChats.length > 0) {
        getIncomingChats.map(async (ct: any) => {
          if (!(ct?.sentToAgent)) {
            if (data?.declined) {
              ct.ext = data?.ext;
            }
            let agentDetails: any = [];
            agentDetails = await getAvaiableAgentChatListBtnForQueue(ct);
            if (agentDetails && agentDetails?.length > 0) {
              const filteredChats = getIncomingChats.filter((chat: any) => chat.session_id != ct.session_id);
              ct.sentToAgent = 1;
              const insertChats = [...filteredChats, ct];
              getIncomingChatDetails[0].queueList = insertChats;
              const updateSession = await insertIncomingChatsDB(getIncomingChatDetails[0], val);
              console.log('sendQueueUser updateSession', updateSession);
              if (updateSession[0].errCode == 0) {
                agentDetails.queueId = val;
                flag = true;
                await handleTransferToLiveAgentToQueue(ct, val, agentDetails);
                // await handleTransferToLiveAgentToQueue2(ct,val, agentDetails);
              }
            }
          }
        })
      }
    })
    if (!flag) {
      const getIncomingChatDetailsUsingUserID: any = await getIncomingChatDetailsDB(data, agentUserId);
      const getIncomingChatsUsingUserID: any = JSON.parse(getIncomingChatDetailsUsingUserID[0]?.queueList ?? '[]');
      if (getIncomingChatsUsingUserID.length > 0) {
        getIncomingChatsUsingUserID.map(async (ct: any) => {
          if (!(ct?.sentToAgent)) {
            if (data?.declined) {
              ct.ext = data?.ext;
            }
            let agentDetails: any = [];
            data.domain_id = data.domainId;
            data.queueId = agentUserId;
            const checkAgent: any = await getParticularAgentChatListBtnForQueue(data);
            const filterParticularAgent = checkAgent.filter((val: any) => val.ext == data.ext);
            if (filterParticularAgent.length > 0) {
              const filteredChats = getIncomingChatsUsingUserID.filter((chat: any) => chat.session_id != ct.session_id);
              ct.sentToAgent = 1;
              const insertChats = [...filteredChats, ct];
              getIncomingChatDetailsUsingUserID[0].queueList = insertChats;
              const updateSession = await insertIncomingChatsDB(getIncomingChatDetailsUsingUserID[0], agentUserId);
              console.log('sendQueueUser updateSession', updateSession);
              if (updateSession[0].errCode == 0) {
                filterParticularAgent[0].queueId = agentUserId;
                agentDetails = [...filterParticularAgent];
                flag = true;
                await handleTransferToLiveAgentToQueue(ct, agentUserId, agentDetails);
                // await handleTransferToLiveAgentToQueue2(ct,val, agentDetails);
              }
            }
          }
        })
      }
    }
  } catch (err) {
    console.log('sendQueueUser err', err);
  }

  // const incomingChat = getIncomingChats?.filter((val: any) => {
  //   return val?.session_id == channelId.session_id
  // });
  // if (incomingChat.length > 0) {
  //   channelId.queueList = [...getIncomingChats];
  // } else {
  //   channelId.queueList = [...getIncomingChats, JSON.parse(JSON.stringify({ ...channelId, currentNode: currentNode }))];
  // }
  // let insertIncomingChats: any = await insertIncomingChatsDB(channelId, currentNode.data?.information?.transferTo);
  // let agentDetails: any = [];
  // agentDetails = await getAvaiableAgentChatListBtn(channelId, currentNode.data?.information?.transferTo);

  // if (agentDetails && agentDetails?.length > 0) {
  //   agentDetails.queueId = currentNode.data?.information?.transferTo;
  //   await handleTransferToLiveAgent(ioredisChat, channelId, currentNode, agentDetails);
  // }
}

export const getAvaiableAgentChatListBtn = async (channel: any, qid: any) => {
  let agentDetails: any = [];
  return new Promise((resolve: any) => {
    otherRedisClient.hgetall(`${channel?.domain_id}_presence`).then(async (data: any) => {
      const result = Object.values(data).map((value: any) => JSON.parse(value));
      // Getting routing profiles mapped with qid
      const routingProfiles: any = await getRoutingProfile(qid);
      const routingProfilesId = [];
      routingProfiles.map((val: any) => val?.rqid);
      let filteredData: any = [];
      if (channel?.ext !== undefined && channel?.ext !== null && channel?.ext !== '') {
        const agentFilter: any = result.filter((item: any) => (item.statusName === 'Ready' && (item.ext !== channel.ext) && parseInt(item.roleId, 10) === 6 && routingProfiles.includes(item.routingProfileId)));
        if (agentFilter?.length === 0) {
          const supFilter: any = result.filter((item: any) => (item.statusName === 'Ready' && (item.ext !== channel.ext) && parseInt(item.roleId, 10) === 5));
          filteredData = supFilter?.length !== 0 ? supFilter : [];
        } else {
          filteredData = agentFilter;
        }
      }
      else {
        const agentFilter: any = result.filter((item: any) => (item.statusName === 'Ready' && parseInt(item.roleId, 10) === 6 && routingProfiles.includes(item.routingProfileId)));
        if (agentFilter?.length === 0) {
          const supFilter: any = result.filter((item: any) => (item.statusName === 'Ready' && parseInt(item.roleId, 10) === 5));
          filteredData = supFilter?.length !== 0 ? supFilter : [];
        } else {
          filteredData = agentFilter;
        }
      }
      if (filteredData?.length === 0) {
        filteredData = result.filter((item: any) => (item.statusName === 'Ready' && (item.ext !== channel.ext) && parseInt(item.roleId, 10) === 3));
      }
      const chatLimitByAgent: any = await getAgentByChat(channel.domain_id);
      const chatLimitData: any = await getConcurrentChat(channel.domain_id);
      const RoutingProfileUser: any = await getRoutingProfileUser(channel.domain_id);
      const parsedChatLimitData = JSON.parse(chatLimitData?.agentOccupyDetails ?? 'null') || null;
      const currentChatCount = parsedChatLimitData?.currentChatCount;

      if (currentChatCount !== null && currentChatCount !== undefined) {
        const appendChatCountToFilteredData = filteredData.map((dt: any) => {
          for (let [key, value] of Object.entries(currentChatCount)) {
            if (dt.ext == key) {
              dt = { ...dt, 'currentChatCount': value };
              break;
            } else {
              dt = { ...dt, 'currentChatCount': 0 };
            }
          }
          return dt;
        });

        const sortedAgents = appendChatCountToFilteredData.sort((a: any, b: any) => a.currentTime < b.currentTime);
        let chatLimitAchievedAgents: any = [];
        let agentsToBeAssigned: any = [...sortedAgents];

        if (chatLimitData?.isAllAgent || chatLimitData?.isAgent || chatLimitData?.isRoutingProfile) {
          let tempAgent: any;
          const checkLiveAgent = () => {
            if (tempAgent !== undefined) {
              if (chatLimitData?.isAllAgent) {
                if (tempAgent?.currentChatCount < chatLimitData?.allAgentLimit) {
                  agentDetails.push(tempAgent);
                } else {
                  if (chatLimitAchievedAgents.length !== sortedAgents.length) {
                    chatLimitAchievedAgents.push(tempAgent);
                    agentsToBeAssigned = agentsToBeAssigned.filter((val: any) => val.ext !== tempAgent.ext);
                    assignFunction();
                  }
                }
              } else if (chatLimitData?.isAgent) {
                chatLimitByAgent.filter((list: any) => {
                  if (tempAgent?.currentChatCount <= (list.chatLimit - 1) && `${tempAgent.ext}` === `${list.ext}`) {
                    agentDetails.push(tempAgent)
                  }
                })
              } else if (chatLimitData?.isRoutingProfile) {
                const routings = {};
                RoutingProfileUser.map((item: any) => {
                  routings[item.RoutingProfileId] = item.concurrentChat;
                });
                for (let [key, value] of Object.entries(routings)) {
                  if (tempAgent.routingProfileId == key) {
                    if (tempAgent.currentChatCount == value) {
                      chatLimitAchievedAgents.push(tempAgent);
                      agentsToBeAssigned = agentsToBeAssigned.filter((val: any) => val.ext !== tempAgent.ext);
                      if (chatLimitAchievedAgents.length !== sortedAgents.length) {
                        assignFunction();
                      }
                    } else {
                      agentDetails.push(tempAgent);
                    }
                  }
                }
              }
            }
          }
          const assignFunction = () => {
            if (agentsToBeAssigned.length > 0) {
              tempAgent = agentsToBeAssigned.reduce((oldest: any, current: any) => {
                if (current.currentChatCount < oldest.currentChatCount) {
                  return current;
                } else if (current.currentChatCount === oldest.currentChatCount && current.currentTime < oldest.currentTime) {
                  return current;
                } else {
                  return oldest;
                }
              }, agentsToBeAssigned[0]);
              checkLiveAgent();
            }
          }
          assignFunction();
        } else {
          let tempAgent: any;
          const checkLiveAgent = () => {
            if (tempAgent !== undefined) {
              if (tempAgent.currentChatCount < 1) {
                agentDetails.push(tempAgent);
              } else {
                if (chatLimitAchievedAgents.length < agentsToBeAssigned.length) {
                  chatLimitAchievedAgents.push(tempAgent);
                  agentsToBeAssigned = agentsToBeAssigned.filter((val: any) => val.ext !== tempAgent.ext);
                  assignFunction();
                }
              }
            }
          }
          const assignFunction = () => {
            tempAgent = agentsToBeAssigned.reduce((oldest: any, current: any) => {
              if (current.currentChatCount < oldest.currentChatCount) {
                return current;
              } else if (current.currentChatCount === oldest.currentChatCount && current.currentTime < oldest.currentTime) {
                return current;
              } else {
                return oldest;
              }
            }, agentsToBeAssigned[0]);
            checkLiveAgent();
          }

          assignFunction();
        }

      } else {
        const oldestReadyItem = await filteredData.reduce((oldest: any, current: any) => {
          return current.currentTime < oldest.currentTime ? current : oldest;
        }, filteredData[0]);
        if (oldestReadyItem !== undefined) {
          agentDetails.push(oldestReadyItem);
        } else {
          agentDetails = [];
        }
      }
      resolve(agentDetails);
    });
  });
}

export const getAvailableAgentChatList = async (channel: any) => {
  let agentDetails: any = [];
  return new Promise((resolve: any) => {
    otherRedisClient.hgetall(`${channel?.domain_id}_presence`).then(async (data: any) => {
      const result = Object.values(data).map((value: any) => JSON.parse(value));
      let filteredData: any = [];
      if (channel?.ext !== undefined && channel?.ext !== null && channel?.ext !== '') {
        const agentFilter: any = result.filter((item: any) => (item.statusName === 'Ready' && (item.ext !== channel.ext) && parseInt(item.roleId, 10) === 6));
        if (agentFilter?.length === 0) {
          const supFilter: any = result.filter((item: any) => (item.statusName === 'Ready' && (item.ext !== channel.ext) && parseInt(item.roleId, 10) === 5));
          filteredData = supFilter?.length !== 0 ? supFilter : [];
        } else {
          filteredData = agentFilter;
        }
      }
      else {
        const agentFilter: any = result.filter((item: any) => (item.statusName === 'Ready' && parseInt(item.roleId, 10) === 6));
        if (agentFilter?.length === 0) {
          const supFilter: any = result.filter((item: any) => (item.statusName === 'Ready' && parseInt(item.roleId, 10) === 5));
          filteredData = supFilter?.length !== 0 ? supFilter : [];
        } else {
          filteredData = agentFilter;
        }
      }
      if (filteredData?.length === 0) {
        filteredData = result.filter((item: any) => (item.statusName === 'Ready' && (item.ext !== channel.ext) && parseInt(item.roleId, 10) === 3));
      }

      const chatLimitByAgent: any = await getAgentByChat(channel.domain_id);
      const chatLimitData: any = await getConcurrentChat(channel.domain_id);
      console.log("chatLimitData>>>>>>>>>>>>>>>>", chatLimitData);
      const RoutingProfileUser: any = await getRoutingProfileUser(channel.domain_id);
      const parsedChatLimitData = JSON.parse(chatLimitData?.agentOccupyDetails ?? 'null') || null;
      const currentChatCount = parsedChatLimitData?.currentChatCount;

      if (currentChatCount !== null && currentChatCount !== undefined) {
        const appendChatCountToFilteredData = filteredData.map((dt: any) => {
          for (let [key, value] of Object.entries(currentChatCount)) {
            if (dt.ext == key) {
              dt = { ...dt, 'currentChatCount': value };
              break;
            } else {
              dt = { ...dt, 'currentChatCount': 0 };
            }
          }
          return dt;
        });

        const sortedAgents = appendChatCountToFilteredData.sort((a: any, b: any) => a.currentTime < b.currentTime);
        // console.log('sortedAgents', sortedAgents);

        let chatLimitAchievedAgents: any = [];
        let agentsToBeAssigned: any = [...sortedAgents];

        if (chatLimitData?.isAllAgent || chatLimitData?.isAgent || chatLimitData?.isRoutingProfile) {

          let tempAgent: any;

          const checkLiveAgent = () => {
            if (tempAgent !== undefined) {
              if (chatLimitData?.isAllAgent) {
                if (tempAgent?.currentChatCount < chatLimitData?.allAgentLimit) {
                  agentDetails.push(tempAgent);
                } else {
                  if (chatLimitAchievedAgents.length !== sortedAgents.length) {
                    chatLimitAchievedAgents.push(tempAgent);
                    agentsToBeAssigned = agentsToBeAssigned.filter((val: any) => val.ext !== tempAgent.ext);
                    assignFunction();
                  }
                }
              } else if (chatLimitData?.isAgent) {
                chatLimitByAgent.filter((list: any) => {
                  if (tempAgent?.currentChatCount <= (list.chatLimit - 1) && `${tempAgent.ext}` === `${list.ext}`) {
                    agentDetails.push(tempAgent)
                  }
                })
              } else if (chatLimitData?.isRoutingProfile) {
                const routings = {};
                RoutingProfileUser.map((item: any) => {
                  routings[item.RoutingProfileId] = item.concurrentChat;
                });
                for (let [key, value] of Object.entries(routings)) {
                  if (tempAgent.routingProfileId == key) {
                    if (tempAgent.currentChatCount == value) {
                      chatLimitAchievedAgents.push(tempAgent);
                      agentsToBeAssigned = agentsToBeAssigned.filter((val: any) => val.ext !== tempAgent.ext);
                      if (chatLimitAchievedAgents.length !== sortedAgents.length) {
                        assignFunction();
                      }
                    } else {
                      // console.log('else');
                      agentDetails.push(tempAgent);
                    }
                  }
                }
              }
            }
          }

          const assignFunction = () => {
            if (agentsToBeAssigned.length > 0) {
              tempAgent = agentsToBeAssigned.reduce((oldest: any, current: any) => {
                if (current.currentChatCount < oldest.currentChatCount) {
                  return current;
                } else if (current.currentChatCount === oldest.currentChatCount && current.currentTime < oldest.currentTime) {
                  return current;
                } else {
                  return oldest;
                }
              }, agentsToBeAssigned[0]);
              checkLiveAgent();
            }
          }

          assignFunction();
        } else {
          let tempAgent: any;

          const checkLiveAgent = () => {
            if (tempAgent !== undefined) {
              if (tempAgent.currentChatCount < 1) {
                agentDetails.push(tempAgent);
              } else {
                if (chatLimitAchievedAgents.length < agentsToBeAssigned.length) {
                  chatLimitAchievedAgents.push(tempAgent);
                  agentsToBeAssigned = agentsToBeAssigned.filter((val: any) => val.ext !== tempAgent.ext);
                  assignFunction();
                }
              }
            }
          }

          const assignFunction = () => {
            tempAgent = agentsToBeAssigned.reduce((oldest: any, current: any) => {
              if (current.currentChatCount < oldest.currentChatCount) {
                return current;
              } else if (current.currentChatCount === oldest.currentChatCount && current.currentTime < oldest.currentTime) {
                return current;
              } else {
                return oldest;
              }
            }, agentsToBeAssigned[0]);
            checkLiveAgent();
          }

          assignFunction();
        }

      } else {
        const oldestReadyItem = await filteredData.reduce((oldest: any, current: any) => {
          return current.currentTime < oldest.currentTime ? current : oldest;
        }, filteredData[0]);
        if (oldestReadyItem !== undefined) {
          agentDetails.push(oldestReadyItem);
        } else {
          agentDetails = [];
        }
      }

      resolve(agentDetails);
    });
  })
}

export const checkAgentStatus = async (channel: any) => {
  let agentFilter: any = [];
  const data = await otherRedisClient.hgetall(`${channel?.domain_id}_presence`);
  const result = Object.values(data).map((value: any) => JSON.parse(value));
  agentFilter = result.filter((item: any) => (item.statusName === 'Ready' && item.userId == channel.queueId));
  console.log('agentFilter checkAgentStatus------------>', agentFilter)
  return agentFilter;
}

export const checkAvailableAgents = async (channel: any) => {
  let agentFilter: any = [];
  let filteredData: any = [];
  const data = await otherRedisClient.hgetall(`${channel?.domain_id}_presence`);
  const result = Object.values(data).map((value: any) => JSON.parse(value));
  // Getting routing profiles mapped with qid
  const routingProfiles: any = await getRoutingProfile(channel.queueId);
  // let filteredData: any = [];
  // let agentFilter: any = [];
  agentFilter = result.filter((item: any) => (item.statusName === 'Ready' && parseInt(item.roleId, 10) === 6 && routingProfiles.includes(item.routingProfileId)));
  if (agentFilter?.length === 0) {
    const supFilter: any = result.filter((item: any) => (item.statusName === 'Ready' && parseInt(item.roleId, 10) === 5));
    agentFilter = supFilter?.length !== 0 ? supFilter : [];
  }

  if (agentFilter?.length === 0) {
    agentFilter = result.filter((item: any) => (item.statusName === 'Ready' && parseInt(item.roleId, 10) === 3));
  }

  const getAgentSkills: any = await getAgentsSkills(channel.domain_id, '1,2,3,5,6');
  console.log('getAgentSkills------------->', getAgentSkills);
  let skilledAgents = [];
  channel?.skills?.map((nodeSkill: any) => {
    getAgentSkills?.map((agent: any) => {
      if (agent?.addSkill) {
        JSON.parse(agent.addSkill)?.map((val: any) => {
          if (nodeSkill == val?.value) {
            skilledAgents.push(agent);
          }
        })
      }
    })
  })

  const extValues = skilledAgents.map((agent: any) => agent.ext);
  const filteredSkilledAgents = agentFilter.filter((agent: any) => extValues.includes(agent.ext) || extValues.includes(+(agent.ext)));
  let filter = [...filteredSkilledAgents];

  console.log('filter after skill filtering----------->', filter);
  if (filter.length > 1) {
    filteredData = filter.filter((vl: any) => vl.ext != channel?.ext);
  } else {
    filteredData = filter;
  }
  console.log('filteredData after ext filtering----------->', filteredData);
  return filteredData;
}

export const insertChatSession = async (channelId: any, currentNode: any) => {
  const getIncomingChatDetails: any = await getIncomingChatDetailsDB(channelId, currentNode.data?.information?.transferTo);
  const getIncomingChats: any = JSON.parse(getIncomingChatDetails[0]?.queueList ?? '[]');
  const incomingChat = getIncomingChats?.filter((val: any) => {
    return val?.session_id == channelId.session_id
  });
  // console.log('channelId-------------------->', channelId);
  if (incomingChat.length > 0) {
    channelId.queueList = [...getIncomingChats];
  } else {
    channelId.queueList = [...getIncomingChats, JSON.parse(JSON.stringify(channelId))];
  }
  const insert = await insertIncomingChatsDB(channelId, currentNode.data?.information?.transferTo);
  // console.log('insert', insert);
  return JSON.parse(JSON.stringify(insert));
}

export const getAvaiableAgentChatListBtnForQueue = async (channel: any) => {
  console.log('getAvaiableAgentChatListBtnForQueue channel--------------->', channel);
  try {
    let agentDetails: any = [];
    return new Promise((resolve: any) => {
      otherRedisClient.hgetall(`${channel?.domain_id}_presence`).then(async (data: any) => {
        console.log('getAvaiableAgentChatListBtnForQueue data----------->', data);
        const result = Object.values(data).map((value: any) => JSON.parse(value));
        const routingProfiles: any = await getRoutingProfile(channel.queueId);
        let filteredData: any = [];
        let agentFilter: any = [];
        agentFilter = result.filter((item: any) => (item.statusName === 'Ready' && parseInt(item.roleId, 10) === 6 && routingProfiles.includes(item.routingProfileId)));
        if (agentFilter?.length === 0) {
          const supFilter: any = result.filter((item: any) => (item.statusName === 'Ready' && parseInt(item.roleId, 10) === 5));
          agentFilter = supFilter?.length !== 0 ? supFilter : [];
        }

        if (agentFilter?.length === 0) {
          agentFilter = result.filter((item: any) => (item.statusName === 'Ready' && parseInt(item.roleId, 10) === 3));
        }
        const getAgentSkills: any = await getAgentsSkills(channel.domain_id, '1,2,3,5,6');
        let skilledAgents = [];
        channel?.skills?.map((nodeSkill: any) => {
          getAgentSkills?.map((agent: any) => {
            if (agent?.addSkill) {
              JSON.parse(agent.addSkill)?.map((val: any) => {
                if (nodeSkill == val?.value) {
                  skilledAgents.push(agent);
                }
              })
            }
          })
        })

        const extValues = skilledAgents.map((agent: any) => agent.ext);
        const filteredSkilledAgents = agentFilter.filter((agent: any) => extValues.includes(agent.ext) || extValues.includes(+(agent.ext)));
        let filter = [...filteredSkilledAgents];
        if (filter.length > 1) {
          filteredData = filter.filter((vl: any) => vl.ext != channel?.ext);
        } else {
          filteredData = filter;
        }
        const chatLimitByAgent: any = await getAgentByChat(channel.domain_id);
        const chatLimitData: any = await getConcurrentChat(channel.domain_id);
        console.log("chatLimitData==================", chatLimitData);
        const RoutingProfileUser: any = await getRoutingProfileUser(channel.domain_id);
        const parsedChatLimitData = JSON.parse(chatLimitData?.agentOccupyDetails ?? 'null') || null;
        const currentChatCount = parsedChatLimitData?.currentChatCount;

        if (currentChatCount !== null && currentChatCount !== undefined) {
          const appendChatCountToFilteredData = filteredData.map((dt: any) => {
            for (let [key, value] of Object.entries(currentChatCount)) {
              if (dt.ext == key) {
                dt = { ...dt, 'currentChatCount': value };
                break;
              } else {
                dt = { ...dt, 'currentChatCount': 0 };
              }
            }
            return dt;
          });

          const sortedAgents = appendChatCountToFilteredData.sort((a: any, b: any) => a.currentTime < b.currentTime);

          let chatLimitAchievedAgents: any = [];
          let agentsToBeAssigned: any = [...sortedAgents];

          if (chatLimitData?.isAllAgent || chatLimitData?.isAgent || chatLimitData?.isRoutingProfile) {
            let tempAgent: any;
            const checkLiveAgent = () => {
              if (tempAgent !== undefined) {
                if (chatLimitData?.isAllAgent) {
                  if (tempAgent?.currentChatCount < chatLimitData?.allAgentLimit) {
                    agentDetails.push(tempAgent);
                  } else {
                    if (chatLimitAchievedAgents.length !== sortedAgents.length) {
                      chatLimitAchievedAgents.push(tempAgent);
                      agentsToBeAssigned = agentsToBeAssigned.filter((val: any) => val.ext !== tempAgent.ext);
                      assignFunction();
                    }
                  }
                } else if (chatLimitData?.isAgent) {
                  chatLimitByAgent.filter((list: any) => {
                    if (tempAgent?.currentChatCount <= (list.chatLimit - 1) && `${tempAgent.ext}` === `${list.ext}`) {
                      agentDetails.push(tempAgent)
                    }
                  })
                } else if (chatLimitData?.isRoutingProfile) {
                  const routings = {};
                  RoutingProfileUser.map((item: any) => {
                    routings[item.RoutingProfileId] = item.concurrentChat;
                  });
                  for (let [key, value] of Object.entries(routings)) {
                    if (tempAgent.routingProfileId == key) {
                      if (tempAgent.currentChatCount == value) {
                        chatLimitAchievedAgents.push(tempAgent);
                        agentsToBeAssigned = agentsToBeAssigned.filter((val: any) => val.ext !== tempAgent.ext);
                        if (chatLimitAchievedAgents.length !== sortedAgents.length) {
                          assignFunction();
                        }
                      } else {
                        agentDetails.push(tempAgent);
                      }
                    }
                  }
                }
              }
            }
            const assignFunction = () => {
              if (agentsToBeAssigned.length > 0) {
                tempAgent = agentsToBeAssigned.reduce((oldest: any, current: any) => {
                  if (current.currentChatCount < oldest.currentChatCount) {
                    return current;
                  } else if (current.currentChatCount === oldest.currentChatCount && current.currentTime < oldest.currentTime) {
                    return current;
                  } else {
                    return oldest;
                  }
                }, agentsToBeAssigned[0]);
                checkLiveAgent();
              }
            }
            assignFunction();
          } else {
            let tempAgent: any;
            const checkLiveAgent = () => {
              if (tempAgent !== undefined) {
                if (tempAgent.currentChatCount < 1) {
                  agentDetails.push(tempAgent);
                } else {
                  if (chatLimitAchievedAgents.length < agentsToBeAssigned.length) {
                    chatLimitAchievedAgents.push(tempAgent);
                    agentsToBeAssigned = agentsToBeAssigned.filter((val: any) => val.ext !== tempAgent.ext);
                    assignFunction();
                  }
                }
              }
            }
            const assignFunction = () => {
              tempAgent = agentsToBeAssigned.reduce((oldest: any, current: any) => {
                if (current.currentChatCount < oldest.currentChatCount) {
                  return current;
                } else if (current.currentChatCount === oldest.currentChatCount && current.currentTime < oldest.currentTime) {
                  return current;
                } else {
                  return oldest;
                }
              }, agentsToBeAssigned[0]);
              checkLiveAgent();
            }
            assignFunction();
          }

        } else {
          const oldestReadyItem = await filteredData.reduce((oldest: any, current: any) => {
            return current.currentTime < oldest.currentTime ? current : oldest;
          }, filteredData[0]);
          if (oldestReadyItem !== undefined) {
            agentDetails.push(oldestReadyItem);
          } else {
            agentDetails = [];
          }
        }
        resolve(agentDetails);
      });
    });
  } catch (err) {
    console.log('getAvaiableAgentChatListBtnForQueue err', err);
  }
}


export const getParticularAgentChatListBtnForQueue = async (channel: any) => {
  try {
    let agentDetails: any = [];
    return new Promise((resolve: any) => {
      otherRedisClient.hgetall(`${channel?.domain_id}_presence`).then(async (data: any) => {
        console.log('getAvaiableAgentChatListBtnForQueue data----------->', data);
        const result = Object.values(data).map((value: any) => JSON.parse(value));
        // console.log('getAvaiableAgentChatListBtnForQueue result----------->', result);
        // Getting routing profiles mapped with qid
        // const routingProfiles: any = await getRoutingProfile(channel.queueId);
        // console.log('routingProfiles----------->', routingProfiles);
        let filteredData: any = [];
        let agentFilter: any = [];

        agentFilter = result.filter((item: any) => (item.statusName === 'Ready' && parseInt(item.roleId, 10) === 6));
        if (agentFilter?.length === 0) {
          const supFilter: any = result.filter((item: any) => (item.statusName === 'Ready' && parseInt(item.roleId, 10) === 5));
          agentFilter = supFilter?.length !== 0 ? supFilter : [];
        }

        if (agentFilter?.length === 0) {
          agentFilter = result.filter((item: any) => (item.statusName === 'Ready' && parseInt(item.roleId, 10) === 3));
        }
        console.log('agentFilter--------------->', agentFilter);
        if (agentFilter.length > 1) {
          filteredData = agentFilter.filter((vl: any) => vl.ext != channel?.ext);
        } else {
          filteredData = agentFilter;
        }

        // console.log('filteredData after ext filtering----------->', filteredData);

        const chatLimitByAgent: any = await getAgentByChat(channel.domain_id);
        const chatLimitData: any = await getConcurrentChat(channel.domain_id);
        const RoutingProfileUser: any = await getRoutingProfileUser(channel.domain_id);
        const parsedChatLimitData = JSON.parse(chatLimitData?.agentOccupyDetails ?? 'null') || null;
        const currentChatCount = parsedChatLimitData?.currentChatCount;

        if (currentChatCount !== null && currentChatCount !== undefined) {
          const appendChatCountToFilteredData = filteredData.map((dt: any) => {
            for (let [key, value] of Object.entries(currentChatCount)) {
              if (dt.ext == key) {
                dt = { ...dt, 'currentChatCount': value };
                break;
              } else {
                dt = { ...dt, 'currentChatCount': 0 };
              }
            }
            return dt;
          });

          const sortedAgents = appendChatCountToFilteredData.sort((a: any, b: any) => a.currentTime < b.currentTime);

          let chatLimitAchievedAgents: any = [];
          let agentsToBeAssigned: any = [...sortedAgents];

          if (chatLimitData?.isAllAgent || chatLimitData?.isAgent || chatLimitData?.isRoutingProfile) {
            let tempAgent: any;
            const checkLiveAgent = () => {
              if (tempAgent !== undefined) {
                if (chatLimitData?.isAllAgent) {
                  if (tempAgent?.currentChatCount < chatLimitData?.allAgentLimit) {
                    agentDetails.push(tempAgent);
                  } else {
                    if (chatLimitAchievedAgents.length !== sortedAgents.length) {
                      chatLimitAchievedAgents.push(tempAgent);
                      agentsToBeAssigned = agentsToBeAssigned.filter((val: any) => val.ext !== tempAgent.ext);
                      assignFunction();
                    }
                  }
                } else if (chatLimitData?.isAgent) {
                  chatLimitByAgent.filter((list: any) => {
                    if (tempAgent?.currentChatCount <= (list.chatLimit - 1) && `${tempAgent.ext}` === `${list.ext}`) {
                      agentDetails.push(tempAgent)
                    }
                  })
                } else if (chatLimitData?.isRoutingProfile) {
                  const routings = {};
                  RoutingProfileUser.map((item: any) => {
                    routings[item.RoutingProfileId] = item.concurrentChat;
                  });
                  for (let [key, value] of Object.entries(routings)) {
                    if (tempAgent.routingProfileId == key) {
                      if (tempAgent.currentChatCount == value) {
                        chatLimitAchievedAgents.push(tempAgent);
                        agentsToBeAssigned = agentsToBeAssigned.filter((val: any) => val.ext !== tempAgent.ext);
                        if (chatLimitAchievedAgents.length !== sortedAgents.length) {
                          assignFunction();
                        }
                      } else {
                        agentDetails.push(tempAgent);
                      }
                    }
                  }
                }
              }
            }
            const assignFunction = () => {
              if (agentsToBeAssigned.length > 0) {
                tempAgent = agentsToBeAssigned.reduce((oldest: any, current: any) => {
                  if (current.currentChatCount < oldest.currentChatCount) {
                    return current;
                  } else if (current.currentChatCount === oldest.currentChatCount && current.currentTime < oldest.currentTime) {
                    return current;
                  } else {
                    return oldest;
                  }
                }, agentsToBeAssigned[0]);
                checkLiveAgent();
              }
            }
            assignFunction();
          } else {
            let tempAgent: any;
            const checkLiveAgent = () => {
              if (tempAgent !== undefined) {
                if (tempAgent.currentChatCount < 1) {
                  agentDetails.push(tempAgent);
                } else {
                  if (chatLimitAchievedAgents.length < agentsToBeAssigned.length) {
                    chatLimitAchievedAgents.push(tempAgent);
                    agentsToBeAssigned = agentsToBeAssigned.filter((val: any) => val.ext !== tempAgent.ext);
                    assignFunction();
                  }
                }
              }
            }
            const assignFunction = () => {
              tempAgent = agentsToBeAssigned.reduce((oldest: any, current: any) => {
                if (current.currentChatCount < oldest.currentChatCount) {
                  return current;
                } else if (current.currentChatCount === oldest.currentChatCount && current.currentTime < oldest.currentTime) {
                  return current;
                } else {
                  return oldest;
                }
              }, agentsToBeAssigned[0]);
              checkLiveAgent();
            }
            assignFunction();
          }

        } else {
          const oldestReadyItem = await filteredData.reduce((oldest: any, current: any) => {
            return current.currentTime < oldest.currentTime ? current : oldest;
          }, filteredData[0]);
          if (oldestReadyItem !== undefined) {
            agentDetails.push(oldestReadyItem);
          } else {
            agentDetails = [];
          }
        }
        resolve(agentDetails);
      });
    });
  } catch (err) {
    console.log('getAvaiableAgentChatListBtnForQueue err', err);
  }
}

export const mergeNestedObjects = (obj: any) => {
  let result = {};
  Object.entries(obj).forEach(([key, value]) => {
    if (typeof value === 'object' && value !== null) {
      result = { ...result, ...mergeNestedObjects(value) };
    } else {
      result[key] = value;
    }
  });
  return result;
}


