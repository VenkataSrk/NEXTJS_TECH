import { v4 as uuidv4 } from 'uuid';
import { checkAgentStatus, checkAvailableAgents, contextPattern, getAvaiableAgentChatListBtn, getAvaiableAgentChatListBtnForQueue, getAvailableAgentChatList, getParticularAgentChatListBtnForQueue, handleTransferToLiveAgent, handleTransferToQueue, insertChatSession, mergeNestedObjects, replacechatPatternWithVariables, replaceContextWithVariables, replacePatternWithVariables } from './chatflow_helpers';
import { conditionsMapper } from './conditions_mapper';
import { getAgentByChat, getAgentListChat, getConcurrentChat, getConcurrentChatLimit, getDomainStatus, getHoursofOperationById, getIncomingChatDetailsDB, getJSON, getnodesdata, getRoutingProfiles, getRoutingProfileUser, getTransferAgentStatus, insertApiCallCount, insertIncomingChatsDB } from './dao/callflow.dao';
import { getUserListByExt } from './dao/chat.dao';
import { getQueueWaitTime, insertUpdateQueueWaitTimeDao, updateChatMessage } from './dao/webChat.dao';
import { fetchApi, getResponseData, replaceDynamicEndPoint } from './fetch_api';
import { insertCustomerInQueue, transferChat, transferChatToAvailableAgent, transferWithoutRouting } from './helperFunctions/transferToQueue';
import { ioredisChat, otherRedisClient } from './plugins/db';
import { logger } from './plugins/log';

export class ChatFlowAdapters {
  public static queueDetails: any;
  public chatAdapter: any;
  public channelInformation: any;
  public context: any;
  public currentNodeInfo: any;
  public blpopTimeout: any;
  public successTimeout: any;
  public dtmfSuccessTimeout: any;
  public responseTimeout: any;
  public secondResponseTimeout: any;
  public playpromptSuccessTimeout: any;
  public businessHoursTimeout: any;
  private sessionLoops: any;
  private callflowId: any;
  private nodes: any;
  private inputParams: any;
  private redisadaptor: any;
  private sourceLang: any;
  private adapter: any;
  constructor(adapter: any) {
    this.redisadaptor = adapter;
    this.chatAdapter = adapter;
    this.channelInformation = null;
    this.channelInformation = null;
    this.currentNodeInfo = null;
    this.blpopTimeout = false;
    this.successTimeout = false;
    this.responseTimeout = false;
    this.secondResponseTimeout = false;
    this.playpromptSuccessTimeout = false;
    this.businessHoursTimeout = false;
    this.dtmfSuccessTimeout = false;
    this.context = {} as any;
    this.sessionLoops = {} as any;
    this.callflowId = null;
    this.nodes = [];
    this.inputParams = '';
    this.sourceLang = 'en';
    ChatFlowAdapters.queueDetails = null;
  }

  public addContextVariable(key: any, value: any) {
    if (this.context) {
      this.context[key] = value;
    }
  }

  public updateQueueDetails(channelId: any) {
    const queueData = JSON.parse(channelId?.queueUpdate ?? 'null');
    ChatFlowAdapters.queueDetails = queueData?.queueList ?? null;
  }

  public clearAllTimeout() {
    this.blpopTimeout = true;
    this.successTimeout = true;
    this.responseTimeout = true;
    this.secondResponseTimeout = true;
    this.playpromptSuccessTimeout = true;
    this.dtmfSuccessTimeout = true;
    this.businessHoursTimeout = true;
  }

  public setAllTimeout() {
    this.blpopTimeout = false;
    this.successTimeout = false;
    this.responseTimeout = false;
    this.secondResponseTimeout = false;
    this.playpromptSuccessTimeout = false;
    this.dtmfSuccessTimeout = false;
    this.businessHoursTimeout = false;
  }

  public async ChatFlowHandler(channel: any) {
    if (channel && JSON.stringify(this.channelInformation) !== JSON.stringify(channel) && !JSON.parse(channel ?? 'null')?.queueUpdate) {
      this.channelInformation = JSON.parse(channel ?? 'null') ?? '';
    }
    // console.log('ChatFlowHandler >>>>>>', this.channelInformation);
    // console.log('this.callflowId >>>>>>', this.callflowId);

    const io = ioredisChat;
    const channelId = JSON.parse(channel ?? 'null');
    if (!this.callflowId) {
      this.callflowId = channelId?.callFlowId;
    }

    if (this.callflowId || channelId.liveTransfer === 1) {
      if (this.nodes && this.nodes?.length === 0 && this.callflowId) {
        const nodesString: any = await getnodesdata(this.callflowId);
        this.nodes = JSON.parse(nodesString);
      }

      if (channelId?.channelType === 'Chat' && channelId?.call_state === 'init' && channelId.liveTransfer !== 1) {
        const startingNode = await this.getStartNode(this.nodes);
        const currentNodeId = startingNode?.data?.children?.length > 0 ? startingNode?.data?.children[0]?.childId : '';

        if (currentNodeId && currentNodeId?.includes('carousel')) {
          channelId.call_type = 'carousel';
          const entryCarousal: any = [];
          startingNode?.data?.children?.map((list: any) => {
            if (list?.childId?.includes('carousel')) {
              list.type = 'carousel';
              entryCarousal.push(list);
            }
          });
          channelId.carousal_type = entryCarousal;
        }
        return this.handleChatFlowType(currentNodeId, channelId, this.nodes, io);
      }
      let nextNode = channelId?.btnValue;

      if (channelId.inputParams) this.inputParams = channelId.inputParams;

      if (channelId.btnValue && channelId?.btnValue?.includes('transferToQueue')) {
        const nextNodedata = this.nodes?.find((nodes: any) => nodes?.data?.nodeId === channelId?.btnValue) ?? { id: '' };
        nextNode = nextNodedata.id;
      }
      if (channelId.liveTransfer !== 1) {
        return this.handleChatFlowType(nextNode, channelId, this.nodes, io);
      }
      // NLP triggers agent connection
      // let liveAgent: any = [];
      // liveAgent = await getAvailableAgentChatList(channelId);
      // if (liveAgent?.length === 0) {
      //   this.chatAdapter.redisadaptor.resetQueue(channelId?.domain_id);
      //   this.waitForLiveAgentResponse(channelId);
      // }
      // if (liveAgent && liveAgent?.length > 0) {
      //   console.log('FOUND AN AVAILABLE AGENT ...');
      //   await handleTransferToLiveAgent(this.chatAdapter, channelId, null, liveAgent);
      // } else {
      //   const getDomainResult: any = await getDomainStatus(channelId?.domain_id);
      //   this.chatAdapter?.io?.to(channelId?.session_id).emit('chat_response', JSON.stringify({ session_id: channelId?.session_id, req_type: 'DISCONNECT', domainStatus: getDomainResult }));
      // }
      // const agentDetails = await transferChatToAvailableAgent(channelId);
      const agentDetails:any = await getUserListByExt(channelId);
      console.log('\n handleChatFlowType agentDetails\n', JSON.stringify(agentDetails));
      if (agentDetails && agentDetails?.length > 0) {
        this.channelInformation = { ...this.channelInformation, response: 'success', status: null };
        logger.info(`handleTransferToLiveAgent Socket Error: ${JSON.stringify(agentDetails)}`);
        await handleTransferToLiveAgent(this.chatAdapter, channelId, null, agentDetails);
      } else {
        const getDomainResult: any = await getDomainStatus(channelId?.domain_id);
        this.chatAdapter?.io?.to(channelId?.session_id).emit('chat_response', JSON.stringify({ session_id: channelId?.session_id, req_type: 'DISCONNECT', domainStatus: getDomainResult }));
        logger.info(`handleChatFlowType Socket Error: ${JSON.stringify(getDomainResult)}`);
      }
    }
  }

  public async handleChatFlowType(nodeId: any, channelId: any, node: any, io: any) {
    console.log('\n****************handleChatFlowType*******************>\n');
    let currentNode: any = {};
    if (nodeId && nodeId?.includes('carousel')) {
      const childArr: any = [];
      await channelId?.carousal_type?.map((list: any) => {
        if (list?.type === channelId?.call_type) {
          childArr.push(list);
        }
      });
      const carouselData: any = [];
      await childArr?.map(async (list: any) => {
        const currentNodeData: any = await this.getNodeDataById(list?.childId, node);
        carouselData.push(currentNodeData);
      });
      currentNode.carousalChildData = carouselData;
      currentNode.type = 'carousel';
      const carousalParentData: any = await this.getNodeDataById(nodeId, node);
      const carousalMessage = JSON?.parse(carousalParentData?.data?.information?.carousel_data);
      currentNode.message = carousalMessage?.message;
    } else {
      currentNode = await this.getNodeDataById(nodeId, node);
      this.currentNodeInfo = currentNode;
    }
    if (this.blpopTimeout || !this.channelInformation?.session_id) {
      const getDomainResult: any = await getDomainStatus(channelId?.domain_id);
      this.chatAdapter?.io?.to(channelId?.session_id).emit('chat_response', JSON.stringify({ session_id: channelId?.session_id, req_type: 'DISCONNECT', domainStatus: getDomainResult }));
      logger.info(`blpopTimeout Socket Error: ${JSON.stringify(getDomainResult)}`);
      return;
    }
    switch (currentNode?.type) {
      case 'entryPoint': {
        return this.handleChatFlowType(currentNode.data.children.at(0).childId, channelId, node, io);
        break;
      }
      case 'messageFlow': {
        try {
          this.channelInformation = { ...this.channelInformation, response: 'pending' };
          this.playpromptSuccessTimeout = false;
          const text = currentNode?.data?.information?.message;
          const availableContexts = text?.match(contextPattern);
          const _formattedSelectedKeys = [];

          if (this.inputParams?.length > 0) {
            const mergedObject = this.inputParams.reduce((result, currentObj) => {
              Object.keys(currentObj).forEach((key) => {
                result[key] = currentObj[key];
              });
              return result;
            },                                           {});
            for (const [key, value] of Object.entries(mergedObject)) {
              /* tslint:disable */
              _formattedSelectedKeys.push({ tempkey: '${context.' + key + '}', originalkey: key, [key]: value });
              this.addContextVariable(key, value);
            }
          }

          // const updatedMessage = availableContexts?.length > 0 ? replacechatPatternWithVariables(availableContexts, text, _formattedSelectedKeys) : text;
          const data = { ...currentNode?.data?.information };
          //data.message = updatedMessage;
          const targetRestructured: any = { currentNode, data, session_id: channelId?.session_id };
          targetRestructured.msg = targetRestructured?.data?.message ?? [];
          const nextNode = currentNode?.data?.children?.find((nodes: any) => nodes?.type === 'success') ?? { childId: '' };
          const nextNodedetails: any = await this.getNodeDataById(nextNode?.childId, node); const currentNodeArr: any = [];
          await currentNode?.data?.children?.map(async (list: any) => {
            if (list?.type === 'success') {
              const currentNodeArr1 = await this.getNodeDataById(list?.childId, node);
              currentNodeArr.push(currentNodeArr1);
            }
          });
          targetRestructured.currentNode = nextNodedetails ?? [];
          targetRestructured.carousalChildData = currentNodeArr;
          let message: any = {};
          let i = 0
          try {

            JSON.parse(targetRestructured.msg)?.map(async (list: any) => {
              const textRes = list?.type !== 'text' ? list : list?.data;
              const updatedMessage = availableContexts?.length > 0 ? replacechatPatternWithVariables(availableContexts, textRes, _formattedSelectedKeys) : textRes;
              message = {
                messageID: uuidv4(),
                sessionId: channelId?.session_id,
                message: updatedMessage,
                sendAt: Date.now(),
                messageType: list?.type !== 'text' ? 'file' : 'text',
                pdfurl: '',
                isBot: true,
                isCustomer: false,
                fileType: '',
                direction: 'OUT',
                isLike: 0,
                mergeMsg: i > 0 ? 1 : 0
              };
              i++
              const messageList: any = { message };
              const messageType: any = { agentText: messageList };
              const stringifiedMessage: any = { message: messageType };
              stringifiedMessage.ChatHistoryId = channelId?.ChatHistoryId;
              stringifiedMessage.ChatDurationHistoryId = channelId?.ChatDurationHistoryId;
              this.chatAdapter?.io?.to(channelId?.session_id).emit('OneToOneChatReceive', message);
              try {
                await updateChatMessage(stringifiedMessage);
              } catch (err) {
                console.log(err);
              }
            });
            targetRestructured.req_type = nextNodedetails === undefined ? 'DISCONNECT' : ''
            if (targetRestructured?.currentNode?.id?.includes('customInput') || targetRestructured?.currentNode?.id?.includes('coupon') || targetRestructured?.currentNode?.id?.includes('sendMail')) {
              return this.handleChatFlowType(targetRestructured?.currentNode?.id, channelId, node, io);
            } else {
              this.chatAdapter?.io?.to(channelId?.session_id).emit('chat_response', JSON.stringify(targetRestructured));
              logger.info(`customInput Socket Error: ${JSON.stringify(targetRestructured)}`);
            }
          } catch (error) {
            console.error('Error:', error);
          }

        } catch (error) {
          const nextNode = currentNode?.data?.children?.find((element: any) => element?.type === 'error') ?? { childId: '' };
          return this.handleChatFlowType(nextNode?.childId, channelId, node, io);
        }
        break;
      }
      case 'integration': {
        try {
          if (currentNode?.data?.information?.method === 'api_block') {
            try {
              const type = currentNode?.data?.information?.getMethodApi;
              const rawEndURL = currentNode?.data?.information?.endPointUrl.lastIndexOf('$');
              const endPointUrl = (type === 'get' && rawEndURL !== -1) ? currentNode?.data?.information?.endPointUrl.substring(0, rawEndURL) : currentNode?.data?.information?.endPointUrl;
              const url = (type === 'get' && rawEndURL !== -1) ? await replaceDynamicEndPoint(currentNode?.data?.information?.endPointUrl, this.inputParams) : `${endPointUrl}`; // 'https://qaccaasapi.worktual.co.uk/core_service/v1/get_mobile_number_status/94988515546677'//;
              console.log('url >>>>', url, currentNode?.data?.information);
              const method = currentNode?.data?.information?.getMethodApi ?? 'get';
              const selectedKeys: any = new Set(currentNode?.data?.information?.selectedKeyFromResponse ?? []);
              const body = currentNode?.data?.information?.apiBody;
              const headers = currentNode?.data?.information?.apiHeaders;
              const apiResponse = await fetchApi({ urlEndPoint: url, urlMethod: method, urlBody: body, urlHeaders: headers, inputParams: this.inputParams });
              const targetRestructured: any = { session_id: channelId?.session_id };
              const nameAPI = currentNode?.data?.information?.nameAPI;
              console.log('apiResponse >>>>>>', apiResponse);
              if (apiResponse?.results) {
                if (apiResponse?.results.status === 200 || (nameAPI === 'Sim order' && apiResponse?.results?.data?.status_code !== 500)) {
                  // if (apiResponse?.results?.data?.status_code !== 500) {
                  let finalResponse: any = await getResponseData(apiResponse?.results);
                  const nodeNext = currentNode?.data?.children?.find((nodes: any) => nodes?.type === 'success') ?? { childId: '' };

                  if (Array.isArray(finalResponse) && finalResponse.length > 0) {
                    finalResponse = finalResponse[0] ?? { childId: '' };
                  }
                  const response: any = await mergeNestedObjects(finalResponse)
                  console.log('response:::::::::::', response)
                  if (response?.statusCode === 200 || response?.statuscode === 200 || response?.status_code === 200) {
                    const _formattedSelectedKeys = [];
                    for (const [key, value] of Object.entries(response)) {
                      if (selectedKeys.has(key)) {

                        /* tslint:disable */
                        _formattedSelectedKeys.push({ tempkey: '${context.' + key + '}', originalkey: key, [key]: value });
                        this.addContextVariable(key, value);
                      }
                    }
                    const finalKeys: any = this.context;
                    this.context = {};
                    targetRestructured.selectedKeys = finalKeys;
                    if (nodeNext.childId.includes('customInput')) {
                      const nextNodedetails: any = await this.getNodeDataById(nodeNext.childId, node);
                      const _cpynextNodedetails: any = { ...nextNodedetails };
                      const _childrens = _cpynextNodedetails?.data?.children || {};
                      for (let _index = 0; _index < _childrens.length; _index++) {
                        const element = _childrens[_index];
                        for (let _tempi = 0; _tempi < _formattedSelectedKeys.length; _tempi++) {
                          const tempelement = _formattedSelectedKeys[_tempi];
                          if (element.type === tempelement.tempkey) {
                            _childrens[_index].type = tempelement.value;
                          }
                        }
                      }
                      _cpynextNodedetails.data.children = _childrens;
                      targetRestructured.currentNode = _cpynextNodedetails;
                      targetRestructured.result = finalResponse;
                      return this.handleChatFlowType(nodeNext?.childId, channelId, node, io);
                      //this.chatAdapter?.io?.to(channelId?.session_id).emit('chat_response', JSON.stringify(targetRestructured));

                    } else if (nodeNext.childId.includes('messageFlow')) {
                      const nextNodedetails: any = await this.getNodeDataById(nodeNext.childId, node);
                      console.log('nextNodedetails >>>>>>>', nextNodedetails);

                      const text = nextNodedetails?.data?.information?.message;
                      const availableContexts = text?.match(contextPattern);
                      const updatedMessage = availableContexts?.length > 0 ? replacechatPatternWithVariables(availableContexts, text, _formattedSelectedKeys) : text;
                      const data = { ...nextNodedetails?.data?.information };
                      data.message = `${updatedMessage[0]}`;
                      const targetRestructured: any = { nextNodedetails, data, session_id: channelId?.session_id };
                      targetRestructured.msg = updatedMessage;
                      const messagenextNode = nextNodedetails?.data?.children?.find((nodes: any) => nodes?.type === 'success') ?? { childId: '' };

                      let messagenextNodedetails: any = {};
                      if (messagenextNode.childId) {
                        messagenextNodedetails = await this.getNodeDataById(messagenextNode?.childId, node);
                      }
                      const match = targetRestructured.msg.match(/"data":"([^"]*)"/);
                      const extractedData = match ? match[1] : null;
                      targetRestructured.currentNode = nextNodedetails;
                      const message: any = {
                        messageID: uuidv4(),
                        sessionId: channelId?.session_id,
                        message: `${extractedData}`,
                        sendAt: Date.now(),
                        messageType: 'text',
                        pdfurl: '',
                        isBot: true,
                        isCustomer: false,
                        fileType: '',
                        direction: 'OUT',
                        isLike: 0
                      };
                      const messageList: any = { message: message };
                      const messageType: any = { agentText: messageList }
                      const stringifiedMessage: any = { message: messageType };
                      stringifiedMessage.ChatHistoryId = channelId?.ChatHistoryId;
                      stringifiedMessage.ChatDurationHistoryId = channelId?.ChatDurationHistoryId;
                      try {
                        await updateChatMessage(stringifiedMessage)
                      } catch (err) {
                        console.log(err)
                      }
                      //createSession(stringifiedMessage, channelId?.session_id);
                      this.chatAdapter?.io?.to(channelId?.session_id).emit('OneToOneChatReceive', message);
                      const _cpynextNodedetails: any = { ...messagenextNodedetails };
                      targetRestructured.currentNode = _cpynextNodedetails;
                      if (targetRestructured?.currentNode?.id?.includes('customInput') || targetRestructured?.currentNode?.id?.includes('coupon') || targetRestructured?.currentNode?.id?.includes('sendMail')) {
                        return this.handleChatFlowType(nodeNext?.childId, channelId, node, io);
                      } else {
                        this.chatAdapter?.io?.to(channelId?.session_id).emit('chat_response', JSON.stringify(targetRestructured));
                        logger.info(`integration Socket Error: ${JSON.stringify(targetRestructured)}`);
                        return this.handleChatFlowType(targetRestructured?.currentNode?.id, channelId, node, io);
                      }
                    } else if (nodeNext.childId.includes('integration')) {
                      return this.handleChatFlowType(nodeNext?.childId, channelId, node, io);
                    }
                  } else {
                    const nodeNext = currentNode?.data?.children?.find((nodes: any) => nodes?.type === 'error') ?? { childId: '' };
                    return this.handleChatFlowType(nodeNext?.childId, channelId, node, io);
                  }
                } else {
                  const nodeNext = currentNode?.data?.children?.find((nodes: any) => nodes?.type === 'error') ?? { childId: '' };
                  // if (nodeNext.childId.includes('messageFlow')) {
                  //   const nextNodedetails: any = await this.getNodeDataById(nodeNext.childId, node);
                  //   const text = nextNodedetails?.data?.information?.message;
                  //   const availableContexts = text?.match(contextPattern);
                  //   const updatedMessage = availableContexts?.length > 0 ? replacePatternWithVariables(availableContexts, text, this.context) : text;
                  //   const data = { ...nextNodedetails?.data?.information };
                  //   data.message = `${updatedMessage}`;
                  //   const targetRestructured: any = { nextNodedetails, data, session_id: channelId?.session_id };
                  //   targetRestructured.msg = nextNodedetails?.data?.information.message;
                  //   const messagenextNode = nextNodedetails?.data?.children?.find((nodes: any) => nodes?.type === 'success') ?? { childId: '' };

                  //   let messagenextNodedetails: any = {};
                  //   if (messagenextNode.childId) {
                  //     messagenextNodedetails = await this.getNodeDataById(messagenextNode?.childId, node);
                  //   }

                  //   const message: any = {
                  //     messageID: uuidv4(),
                  //     sessionId: channelId?.session_id,
                  //     message: `${targetRestructured.msg}`,
                  //     sendAt: Date.now(),
                  //     messageType: 'text',
                  //     pdfurl: '',
                  //     isBot: true,
                  //     isCustomer: false,
                  //     fileType: '',
                  //     direction: 'OUT',
                  //     isLike: 0
                  //   };
                  //   const messageList: any = { message: message };
                  //   const messageType: any = { agentText: messageList }
                  //   const stringifiedMessage: any = { message: messageType };
                  //   stringifiedMessage.ChatHistoryId = channelId?.ChatHistoryId;
                  //   stringifiedMessage.ChatDurationHistoryId = channelId?.ChatDurationHistoryId;
                  //   try {
                  //     await updateChatMessage(stringifiedMessage)
                  //   } catch (err) {
                  //     console.log(err)
                  //   }
                  //   this.chatAdapter?.io?.to(channelId?.session_id).emit('OneToOneChatReceive', message);
                  //   const _cpynextNodedetails: any = { ...messagenextNodedetails };
                  //   targetRestructured.currentNode = _cpynextNodedetails;
                  //   this.chatAdapter?.io?.to(channelId?.session_id).emit('chat_response', JSON.stringify(targetRestructured));
                  // }
                  return this.handleChatFlowType(nodeNext?.childId, channelId, node, io);
                }
              }
            } catch (error: any) {
              const nextNode = currentNode?.data?.children?.find((nodes: any) => nodes?.type === 'error') ?? { childId: '' };
              return this.handleChatFlowType(nextNode?.childId, channelId, node, io);
            }
            const body = {
              domainId: channelId?.domain_id,
              chatFlowId: channelId?.callFlowId,
              apiCallCount: 1
            }
            await insertApiCallCount(body);
          } else {
            this.chatAdapter?.io?.to(channelId?.session_id).emit('chat_response', JSON.stringify(currentNode));
            this.chatAdapter?.io?.to(channelId?.session_id).emit('insertApiCallCount', JSON.stringify(currentNode));
          }

        } catch (error: any) {
          const nextNode = currentNode?.data?.children?.find((nodes: any) => nodes?.type === 'error') ?? { childId: '' };
          return this.handleChatFlowType(nextNode?.childId, channelId, node, io);
        }
        break;
      }
      case 'datePicker': {
        try {
          const parseDateMessage: any = JSON.parse(currentNode?.data?.information?.datepicker_data);
          const _formattedSelectedKeys = [];
          if (this.inputParams?.length > 0) {
            const mergedObject = this.inputParams.reduce((result: any, currentObj: any) => {
              Object.keys(currentObj).forEach((key) => {
                result[key] = currentObj[key];
              });
              return result;
            }, {});
            for (const [key, value] of Object.entries(mergedObject)) {
              /* tslint:disable */
              _formattedSelectedKeys.push({ tempkey: '${context.' + key + '}', originalkey: key, [key]: value });
              this.addContextVariable(key, value);
            }
          }
          const text = parseDateMessage?.message;
          const availableContexts = text?.match(contextPattern);
          const updatedMessage = availableContexts?.length > 0 ? replacechatPatternWithVariables(availableContexts, text, _formattedSelectedKeys) : text;
          const checkEmpty = updatedMessage.replace(/<[^>]*>/g, "").trim().length > 0 ? true : false;
          if (checkEmpty) {
            let message = {
              messageID: uuidv4(),
              sessionId: channelId?.session_id,
              message: updatedMessage,
              sendAt: Date.now(),
              messageType: 'text',
              pdfurl: '',
              isBot: true,
              isCustomer: false,
              fileType: '',
              direction: 'OUT',
              isLike: 0,
            };
            const messageList: any = { message };
            const messageType: any = { agentText: messageList };
            const stringifiedMessage: any = { message: messageType };
            stringifiedMessage.ChatHistoryId = channelId?.ChatHistoryId;
            stringifiedMessage.ChatDurationHistoryId = channelId?.ChatDurationHistoryId;
            this.chatAdapter?.io?.to(channelId?.session_id).emit('OneToOneChatReceive', message);
            try {
              await updateChatMessage(stringifiedMessage);
            } catch (err) {
              console.log(err);
            }
          }
          this.chatAdapter?.io?.to(channelId?.session_id).emit('chat_response', JSON.stringify(currentNode));
          logger.info(`chatAdapter Socket Error: ${JSON.stringify(currentNode)}`);
        } catch (error: any) {
          const nextNode = currentNode?.data?.children?.find((nodes: any) => nodes?.type === 'error') ?? { childId: '' };
          return this.handleChatFlowType(nextNode?.childId, channelId, node, io);
        }
        break;
      }
      case 'appointment': {
        try {
          this.chatAdapter?.io?.to(channelId?.session_id).emit('chat_response', JSON.stringify(currentNode));
          logger.info(`appointment Socket Error: ${JSON.stringify(currentNode)}`);
        } catch (error: any) {
          const nextNode = currentNode?.data?.children?.find((nodes: any) => nodes?.type === 'error') ?? { childId: '' };
          return this.handleChatFlowType(nextNode?.childId, channelId, node, io);
        }
        break;
      }
      case 'careHomeAuth': {
        try {
          logger.info(`careHomeAuth Socket Error: ${JSON.stringify(currentNode)}`);
          return this.chatAdapter?.io?.to(channelId?.session_id).emit('chat_response', JSON.stringify(currentNode));
        } catch (err: any) {
          const nextNode = currentNode?.data?.children?.find((nodes: any) => nodes?.type === 'error') ?? { childId: '' };
          return this.handleChatFlowType(nextNode?.childId, channelId, node, io);
        }
      }
      case 'coupon': {
        try {
          const CoupanData: any = {
            currentNode,
            type: 'coupon'
          }
          const parseDateMessage: any = JSON.parse(currentNode?.data?.information?.coupon_data);
          const _formattedSelectedKeys = [];
          if (this.inputParams?.length > 0) {
            const mergedObject = this.inputParams.reduce((result: any, currentObj: any) => {
              Object.keys(currentObj).forEach((key) => {
                result[key] = currentObj[key];
              });
              return result;
            }, {});
            for (const [key, value] of Object.entries(mergedObject)) {
              /* tslint:disable */
              _formattedSelectedKeys.push({ tempkey: '${context.' + key + '}', originalkey: key, [key]: value });
              this.addContextVariable(key, value);
            }
          }
          const text = parseDateMessage?.message;
          const availableContexts = text?.match(contextPattern);
          const updatedMessage = availableContexts?.length > 0 ? replacechatPatternWithVariables(availableContexts, text, _formattedSelectedKeys) : text;
          const checkEmpty = updatedMessage.replace(/<[^>]*>/g, "").trim().length > 0 ? true : false;
          if (checkEmpty) {
            let message = {
              messageID: uuidv4(),
              sessionId: channelId?.session_id,
              couponId: parseDateMessage?.coupon,
              message: updatedMessage,
              sendAt: Date.now(),
              messageType: 'text',
              pdfurl: '',
              isBot: true,
              isCustomer: false,
              fileType: '',
              direction: 'OUT',
              isCoupanStatus: true,
              isLike: 0,
            };
            const messageList: any = { message };
            const messageType: any = { agentText: messageList };
            const stringifiedMessage: any = { message: messageType };
            stringifiedMessage.ChatHistoryId = channelId?.ChatHistoryId;
            stringifiedMessage.ChatDurationHistoryId = channelId?.ChatDurationHistoryId;
            this.chatAdapter?.io?.to(channelId?.session_id).emit('OneToOneChatReceive', message);
            try {
              await updateChatMessage(stringifiedMessage);
            } catch (err) {
              console.log(err);
            }
          }
          this.chatAdapter?.io?.to(channelId?.session_id).emit('chat_response', JSON.stringify(CoupanData));
          logger.info(`CoupanData Socket Error: ${JSON.stringify(CoupanData)}`);
        } catch (error: any) {
          const nextNode = currentNode?.data?.children?.find((nodes: any) => nodes?.type === 'error') ?? { childId: '' };
          return this.handleChatFlowType(nextNode?.childId, channelId, node, io);
        }
        break;
      }
      case 'addFlow': {
        try {
          const nodesString: any = await getnodesdata(currentNode?.data?.information?.flow_id);
          currentNode.addFlowNodes = nodesString;
          this.chatAdapter?.io?.to(channelId?.session_id).emit('chat_response', JSON.stringify(currentNode));
          logger.info(`addFlow Socket Error: ${JSON.stringify(currentNode)}`);
        } catch (error: any) {
          const nextNode = currentNode?.data?.children?.find((nodes: any) => nodes?.type === 'error') ?? { childId: '' };
          return this.handleChatFlowType(nextNode?.childId, channelId, node, io);
        }
        break;
      }
      case 'carousel': {
        try {
          const _formattedSelectedKeys = [];
          if (this.inputParams?.length > 0) {
            const mergedObject = this.inputParams.reduce((result: any, currentObj: any) => {
              Object.keys(currentObj).forEach((key) => {
                result[key] = currentObj[key];
              });
              return result;
            }, {});
            for (const [key, value] of Object.entries(mergedObject)) {
              /* tslint:disable */
              _formattedSelectedKeys.push({ tempkey: '${context.' + key + '}', originalkey: key, [key]: value });
              this.addContextVariable(key, value);
            }
          }
          const text = currentNode?.message;
          const availableContexts = text?.match(contextPattern);
          const updatedMessage = availableContexts?.length > 0 ? replacechatPatternWithVariables(availableContexts, text, _formattedSelectedKeys) : text;
          const checkEmpty = updatedMessage.replace(/<[^>]*>/g, "").trim().length > 0 ? true : false;
          if (checkEmpty) {
            let message = {
              messageID: uuidv4(),
              sessionId: channelId?.session_id,
              message: updatedMessage,
              sendAt: Date.now(),
              messageType: 'text',
              pdfurl: '',
              isBot: true,
              isCustomer: false,
              fileType: '',
              direction: 'OUT',
              isLike: 0,
            };
            const messageList: any = { message };
            const messageType: any = { agentText: messageList };
            const stringifiedMessage: any = { message: messageType };
            stringifiedMessage.ChatHistoryId = channelId?.ChatHistoryId;
            stringifiedMessage.ChatDurationHistoryId = channelId?.ChatDurationHistoryId;
            this.chatAdapter?.io?.to(channelId?.session_id).emit('OneToOneChatReceive', message);
            try {
              await updateChatMessage(stringifiedMessage);
            } catch (err) {
              console.log(err);
            }
          }
          this.chatAdapter?.io?.to(channelId?.session_id).emit('chat_response', JSON.stringify(currentNode));
          logger.info(`carousel Socket Error: ${JSON.stringify(currentNode)}`);
        } catch (error: any) {
          const nextNode = currentNode?.data?.children?.find((nodes: any) => nodes?.type === 'error') ?? { childId: '' };
          return this.handleChatFlowType(nextNode?.childId, channelId, node, io);
        }
        break;
      }
      case 'payment': {
        try {
          this.chatAdapter?.io?.to(channelId?.session_id).emit('chat_response', JSON.stringify(currentNode));
          logger.info(`payment Socket Error: ${JSON.stringify(currentNode)}`);
        } catch (error: any) {
          const nextNode = currentNode?.data?.children?.find((nodes: any) => nodes?.type === 'error') ?? { childId: '' };
          return this.handleChatFlowType(nextNode?.childId, channelId, node, io);
        }
        break;
      }
      case 'sendMail': {
        try {
          this.chatAdapter?.io?.to(channelId?.session_id).emit('chat_response', JSON.stringify(currentNode));
          logger.info(`sendMail Socket Error: ${JSON.stringify(currentNode)}`);
        } catch (error: any) {
          const nextNode = currentNode?.data?.children?.find((nodes: any) => nodes?.type === 'error') ?? { childId: '' };
          return this.handleChatFlowType(nextNode?.childId, channelId, node, io);
        }
        break;
      }
      case 'randomise': {
        try {
          const randonIndex = Math.floor(Math.random() * currentNode?.data?.children?.length);
          const childIdData = currentNode?.data?.children[randonIndex];
          if (childIdData?.childId?.includes('messageFlow')) {
            return this.handleChatFlowType(childIdData?.childId, channelId, node, io);
          } else {
            const currentNodeData: any = await this.getNodeDataById(childIdData?.childId, node);
            if (childIdData?.childId?.includes('carousel')) {
              const data: any = {}
              data.carousalChildData = [currentNodeData];
              data.type = 'carousel';
              this.chatAdapter?.io?.to(channelId?.session_id).emit('chat_response', JSON.stringify(data));
              logger.info(`randomise Socket Error: ${JSON.stringify(data)}`);
            } else {
              this.chatAdapter?.io?.to(channelId?.session_id).emit('chat_response', JSON.stringify(currentNodeData));
              logger.info(`randomise1 Socket Error: ${JSON.stringify(currentNodeData)}`);
            }
          }
        } catch (error: any) {
          const nextNode = currentNode?.data?.children?.find((nodes: any) => nodes?.type === 'error') ?? { childId: '' };
          return this.handleChatFlowType(nextNode?.childId, channelId, node, io);
        }
        break;
      }
      case 'customInput': {
        try {
          const _formattedSelectedKeys = [];

          if (this.inputParams?.length > 0) {
            const mergedObject = this.inputParams.reduce((result, currentObj) => {
              Object.keys(currentObj).forEach((key) => {
                result[key] = currentObj[key];
              });
              return result;
            }, {});
            for (const [key, value] of Object.entries(mergedObject)) {
              /* tslint:disable */
              _formattedSelectedKeys.push({ tempkey: '${context.' + key + '}', originalkey: key, [key]: value });
              this.addContextVariable(key, value);
            }
          }
          // console.log("cus999999999999999999999999999999999",channelId)
          this.channelInformation = { ...this.channelInformation, response: 'pending' };
          this.playpromptSuccessTimeout = false;
          const text = currentNode?.data?.information?.message;
          const availableContexts = text?.match(contextPattern);
          const updatedMessage = availableContexts?.length > 0 ? replacechatPatternWithVariables(availableContexts, text, _formattedSelectedKeys) : text;
          const checkEmpty = updatedMessage.replace(/<[^>]*>/g, "").trim().length > 0 ? true : false;
          
          if ((checkEmpty || [14,15].includes(currentNode?.data?.information?.inputType)) && currentNode?.data?.information?.inputType !== 3) {
            let message: any = {
              messageID: uuidv4(),
              sessionId: channelId?.session_id,
              message: updatedMessage,
              sendAt: Date.now(),
              messageType: 'text',
              pdfurl: '',
              isBot: true,
              isCustomer: false,
              fileType: '',
              direction: 'OUT',
              isLike: 0,
              stream_finished: channelId?.stream_finished,
              chatFlowId: channelId?.callFlowId
            };
            if ([1,15].includes(currentNode?.data?.information?.inputType)) {
              message.buttonDataArr=currentNode?.data;
              message.flowIdData = currentNode;
              message.isButton = 1
              message.messageType="customButton"
            }
            if(currentNode?.data?.information?.inputType===14){
              message.isDropDown = 1
              message.dropDownDataArr = currentNode?.data;
              message.flowIdData = currentNode;
            }
            const messageList: any = { message };
            const messageType: any = { agentText: messageList };
            const stringifiedMessage: any = { message: messageType };
            stringifiedMessage.ChatHistoryId = channelId?.ChatHistoryId;
            stringifiedMessage.ChatDurationHistoryId = channelId?.ChatDurationHistoryId;
            this.chatAdapter?.io?.to(channelId?.session_id).emit('OneToOneChatReceive', message);
            try {
              await updateChatMessage(stringifiedMessage);
            } catch (err) {
              console.log(err);
            }
          }
          // const updatedMessage = availableContexts?.length > 0 ? replacePatternWithVariables(availableContexts, text, this.context) : text;
          const data = { ...currentNode?.data?.information };
          data.message = updatedMessage;
          const targetRestructured: any = { currentNode, data, session_id: channelId?.session_id };
          targetRestructured.msg = updatedMessage;
          targetRestructured.currentNode.data.information.message = updatedMessage
          await Promise.all(targetRestructured?.currentNode?.data?.children?.map(async (list: any) => {
            const translateResult = await this.translateItem(list?.type, channelId);
            list.type = translateResult;
          }));
          this.chatAdapter?.io?.of('/')?.to(channelId?.session_id).emit('chat_response', JSON.stringify(targetRestructured));
          logger.info(`translateItem Socket Error: ${JSON.stringify(targetRestructured)}`);
        } catch (error) {
          const nextNode = currentNode?.data?.children?.find((element: any) => element?.type === 'error') ?? { childId: '' };
          return this.handleChatFlowType(nextNode?.childId, channelId, node, io);
        }
        break;
      }
      case 'transferToQueue': {
        try {
          channelId.skills = currentNode.data?.information?.skill;
          channelId.queueId = currentNode.data?.information?.transferTo;
          this.channelInformation = { ...this.channelInformation, response: 'pending' };
          this.waitForAgentResponse(channelId);
          if (currentNode?.data?.information?.type === 1) {
            await insertCustomerInQueue(channelId, currentNode, channelId?.queueId)
          }
          const currentAgentDetails = await transferWithoutRouting(channelId, currentNode);
          const agentDetails :any= await this.checkConcurrentChatFlagCheck(currentAgentDetails);
          if (agentDetails && agentDetails?.length > 0) {
            if (currentNode?.data?.information?.type === 2) {
              await handleTransferToQueue(this.chatAdapter, channelId, currentNode, agentDetails);
              this.channelInformation = { ...this.channelInformation, response: 'success', status: null };
            } else {
              const list = {
                qid: currentNode.data?.information?.transferTo,
                domainId: channelId?.domain_id
              }
              const getQueueList = await getQueueWaitTime(list);
              const parsedData = JSON.parse(getQueueList?.[0]?.queueList);
              const resultData = parsedData?.map((list: any) => {
                if (list?.sessionId === channelId?.session_id) {
                  list.chatNotifyStatus = true
                  list.notifyQueuedTime = Math.floor(Date.now() / 1000)
                }
                return list
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
              agentDetails.queueId = currentNode.data?.information?.transferTo;
              await handleTransferToQueue(this.chatAdapter, channelId, currentNode, agentDetails);
              this.channelInformation = { ...this.channelInformation, response: 'success', status: null };
            }
          } else {
            if (currentNode?.data?.information?.type === 2) {
              this.chatAdapter.redisadaptor.resetQueue(channelId?.domain_id); // added new line
              const nextNodes = currentNode?.data?.children?.find((element: any) => element?.type === 'error') ?? { childId: '' };
              return this.handleChatFlowType(nextNodes?.childId, channelId, node, io);
            }
            // else {
            //   insertCustomerInQueue(channelId, currentNode, channelId?.queueId)
            // }
          }
        } catch (error: any) {
          const nextNodeData = currentNode?.data?.children?.find((element: any) => element?.type === 'error') ?? { childId: '' };
          return this.handleChatFlowType(nextNodeData?.childId, channelId, node, io);
        }
        break;
      }
      case 'checkCondition': {
        try {
          const parseDateMessage: any = JSON.parse(currentNode?.data?.information?.condition_data);
          const _formattedSelectedKeys = [];
          if (this.inputParams?.length > 0) {
            const mergedObject = this.inputParams.reduce((result: any, currentObj: any) => {
              Object.keys(currentObj).forEach((key) => {
                result[key] = currentObj[key];
              });
              return result;
            }, {});
            for (const [key, value] of Object.entries(mergedObject)) {
              /* tslint:disable */
              _formattedSelectedKeys.push({ tempkey: '${context.' + key + '}', originalkey: key, [key]: value });
              this.addContextVariable(key, value);
            }
          }
          const text = parseDateMessage?.message;
          const availableContexts = text?.match(contextPattern);
          const updatedMessage = availableContexts?.length > 0 ? replacechatPatternWithVariables(availableContexts, text, _formattedSelectedKeys) : text;
          const checkEmpty = updatedMessage.replace(/<[^>]*>/g, "").trim().length > 0 ? true : false;
          if (checkEmpty) {
            let message = {
              messageID: uuidv4(),
              sessionId: channelId?.session_id,
              message: updatedMessage,
              sendAt: Date.now(),
              messageType: 'text',
              pdfurl: '',
              isBot: true,
              isCustomer: false,
              fileType: '',
              direction: 'OUT',
              isLike: 0,
            };
            const messageList: any = { message };
            const messageType: any = { agentText: messageList };
            const stringifiedMessage: any = { message: messageType };
            stringifiedMessage.ChatHistoryId = channelId?.ChatHistoryId;
            stringifiedMessage.ChatDurationHistoryId = channelId?.ChatDurationHistoryId;
            this.chatAdapter?.io?.to(channelId?.session_id).emit('OneToOneChatReceive', message);
            try {
              await updateChatMessage(stringifiedMessage);
            } catch (err) {
              console.log(err);
            }
          }
          const data = currentNode?.data?.information;
          const leftOperand = replaceContextWithVariables(data?.leftOperand, this.context);
          const rightOperands = data?.conditions ?? [];
          let resultIndex = -1;
          for (const stock of rightOperands) {
            const result = await conditionsMapper(leftOperand, stock?.operand, replaceContextWithVariables(stock?.rightOperand, this.context));
            if (result) {
              resultIndex = stock?.index;
              break;
            }
          }
          if (resultIndex !== -1) {
            const nodeNext = currentNode?.data?.children?.find((nodes: any) => nodes?.type === `condition${resultIndex + 1}`) ?? { childId: '' };
            return this.handleChatFlowType(nodeNext?.childId, channelId, node, io);
          }
          const nextNode = currentNode?.data?.children?.find((nodes: any) => nodes?.type === 'error') ?? { childId: '' };
          return this.handleChatFlowType(nextNode?.childId, channelId, node, io);
        } catch (error: any) {
          const nextNode = currentNode?.data?.children?.find((nodes: any) => nodes?.type === 'error') ?? { childId: '' };
          return this.handleChatFlowType(nextNode?.childId, channelId, node, io);
        }
        break;
      }
      case 'loop': {
        try {

          console.log('this.sessionLoops[channelId?.session_id] >>>>>>', this.sessionLoops[channelId?.session_id]);
          if (!this.sessionLoops[channelId?.session_id] && this.sessionLoops[channelId?.session_id] !== 0) {
            this.sessionLoops[channelId?.session_id] = currentNode?.data?.information?.loopValue ?? 0;
          }
          const loopTo = currentNode?.data?.children?.find((nodee: any) => nodee?.type === 'loop') ?? { childId: '' };
          if (loopTo && this.sessionLoops[channelId?.session_id] > 0) {
            console.log('Loop inner>>>>>>', loopTo);
            this.sessionLoops[channelId?.session_id] -= 1;
            const nextNodedetails: any = await this.getNodeDataById(loopTo.childId, node);
            if (loopTo.childId.includes('makeApiCall')) {
              return this.handleChatFlowType(loopTo?.childId, channelId, node, io);
            } {
              const targetRestructured: any = { nextNodedetails, session_id: channelId?.session_id };
              console.log('nextNodedetails >>>>>>>', nextNodedetails);
              targetRestructured.currentNode = nextNodedetails;
              this.chatAdapter?.io?.to(channelId?.session_id).emit('chat_response', JSON.stringify(targetRestructured));
              logger.info(`nextNodedetails Socket Error: ${JSON.stringify(targetRestructured)}`);
            }

          } else {
            const nextNode = currentNode?.data?.children?.find((nodes: any) => nodes?.type === 'complete') ?? { childId: '' };
            const nextNodedetails: any = await this.getNodeDataById(nextNode.childId, node);

            console.log('Loop Complete>>>>>>', nextNode.childId);
            return this.handleChatFlowType(nextNode?.childId, channelId, node, io);
          }

        } catch (error: any) {
          const nextNode = currentNode?.data?.children?.find((nodee: any) => nodee?.type === 'error') ?? { childId: '' };
          return this.handleChatFlowType(nextNode?.childId, channelId, node, io);
        }
        break;
      }
      case 'disconnect': {
        console.log('FOUND DISCONNECTED NODE. GOING TO DISCONNECT');
        await new Promise(resolve => setTimeout(resolve, 500));
        const getDomainResult: any = await getDomainStatus(channelId?.domain_id)
        this.chatAdapter?.io?.to(channelId?.session_id).emit('chat_response', JSON.stringify({ session_id: channelId?.session_id, req_type: 'DISCONNECT', domainStatus: getDomainResult }));
        logger.info(`disconnect Socket Error: ${JSON.stringify(getDomainResult)}`);
        return;
      }
      default: {
        console.log('Default calling');
        this.blpopTimeout = true;
        const getDomainResult: any = await getDomainStatus(channelId?.domain_id)
        this.chatAdapter?.io?.to(channelId?.session_id).emit('chat_response', JSON.stringify({ session_id: channelId?.session_id, req_type: 'DISCONNECT' }));
        return;
      }
    }
  }

  public getNodeDataById = (nodeid: any, node: any) => {
    return node?.find((list: any) => list.id === nodeid);
  }

  public getStartNode = (node: any) => {
    return node?.find((list: any) => list.type === 'entryPoint');
  }

  public getCustomhours = (nexttargetedge: any, msg: any) => {
    return nexttargetedge.filter((list: any) => list.sourceHandle === msg);
  }

  public gettargetedges = (edgeid: any, edge: any) => {
    return edge?.find((list: any) => list.source === edgeid);
  }

  private async waitForAgentResponse(channelId: any) {
    try {

      let timeout = 60;
      const inteval = setInterval(() => {
        if (timeout === 0 || this.successTimeout) {
          clearInterval(inteval);
          this.blpopTimeout = true;
        }
        if (this.channelInformation?.response === 'success') {
          clearInterval(inteval);
          this.channelInformation = { ...this.channelInformation, response: 'pending' };

        }
        timeout -= 1;
      }, 1000);

    } catch (error) {
      console.log('error', error);
    }

  }

  private async waitForLiveAgentResponse(channelId: any) {
    try {

      let timeout = 60;
      const inteval = setInterval(() => {
        if (timeout === 0 || this.successTimeout) {
          clearInterval(inteval);
          this.blpopTimeout = true;
        }
        timeout -= 1;
      }, 1000);

    } catch (error) {
      console.log('error', error);
    }

  }

  private async waitForCallStatusResponse() {
    const status = new Set(['dialTimeout', 'cancelled']);
    return new Promise((resolve: any, _: any) => {
      let timeout = 180;
      const inteval = setInterval(() => {
        if (timeout === 0 || this.responseTimeout) {
          resolve(null);
          clearInterval(inteval);
        } else {
          if (this.channelInformation?.status && status.has(this.channelInformation?.status)) {
            this.channelInformation = { ...this.channelInformation, response: 'pending' };
            console.log(`${this.channelInformation?.status?.toUpperCase()}, LOOKING FOR ANOTHER USER`);
            clearInterval(inteval);
            resolve(this.channelInformation?.status);
          } else if (this.channelInformation?.status && !status.has(this.channelInformation?.status)) {
            resolve(null);
            clearInterval(timeout);
          }
        }
        timeout -= 1;
      }, 1000);
    });
  }

  private async waitForSecondCallStatusResponse() {
    const status = new Set(['dialTimeout', 'cancelled']);
    return new Promise((resolve: any, _: any) => {
      let timeout = 180;
      const inteval = setInterval(() => {
        if (timeout === 0 || this.secondResponseTimeout) {
          resolve(null);
          clearInterval(inteval);
        } else {
          if (this.channelInformation?.status && status.has(this.channelInformation?.status)) {
            this.channelInformation = { ...this.channelInformation, response: 'pending' };
            clearInterval(inteval);
            resolve(this.channelInformation?.status);
          } else if (this.channelInformation?.status !== 'pending' && !status.has(this.channelInformation?.status)) {
            resolve(null);
            clearInterval(timeout);
          }
        }
        timeout -= 1;
      }, 1000);
    });
  }

  private async BLPOP(domainId: any, list: any, routingProfiles: any, sessionId: any, userId: any) {
    await otherRedisClient.blpop(`${domainId}_${process.env.AGENTTRACKERCHANNEL}`, 1).then(async (data: any) => {
      if (this.blpopTimeout) {
        console.log('CANCELLING SEARCHING FOR AGENT BECAUSE OF DISCONNECTED CALL ...');
        list.push(null);
        return list;
      }
      if (data !== null && data?.length > 1) {
        const parsedData = JSON.parse(data?.at(1));
        if ((parsedData?.statusName === 'Ready' || parsedData?.statusName === 'Transfer Only') && (routingProfiles?.includes(parsedData?.routingProfileId) || userId === parsedData?.UserID)) {
          console.log('AGENT FOUND AS: ', data);
          list.push([parsedData]);
          return list;
        }
      }
      return await this.BLPOP(domainId, list, routingProfiles, sessionId, userId);
    });
  }

  private async LIVEBLPOP(domainId: any, list: any) {
    await otherRedisClient.blpop(`${domainId}_${process.env.AGENTTRACKERCHANNEL}`, 1).then(async (data: any) => {
      if (this.blpopTimeout) {
        console.log('CANCELLING SEARCHING FOR AGENT BECAUSE OF DISCONNECTED CALL ...');
        list.push(null);
        return list;
      }
      if (data !== null && data?.length > 1) {
        const parsedData = JSON.parse(data?.at(1));
        if ((parsedData?.statusName === 'Ready' || parsedData?.statusName === 'Transfer Only')) {
          console.log('AGENT FOUND AS: ', data);
          list.push([parsedData]);
          return list;
        }
      }
      return await this.LIVEBLPOP(domainId, list);
    });
  }

  private async translateItem(list: any, channelId: any) {
    var sourceLang = 'en';
    var targetLang = channelId?.targetLang;
    var url =
      "https://translate.googleapis.com/translate_a/single?client=gtx&sl=" +
      sourceLang + "&tl=" + targetLang + "&dt=t&q=" + encodeURI(list);
    return new Promise<void>((resolve, reject) => {
      getJSON(url, function (err: any, data: any) {
        if (err) {
          reject(err);
        } else {
          resolve(data[0][0][0]);
        }
      });
    });
  }

  private async checkConcurrentChatFlagCheck(array: any[]) {
    try {
      for (const item of array) {
        const res: any = await getConcurrentChatLimit({
          domainId: item.domainId,
          UserID: item.userId
        });
  
        if (res.flagresult === 1) {
          return [item];
        }
      }
      return [];
    } catch (err) {
      console.error(err);
      return [];
    }
  }
}