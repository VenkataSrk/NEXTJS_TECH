import { contextPattern, handleTransferToLiveAgent, handleTransferToQueue, replacechatPatternWithVariables, replaceContextWithVariables, replacePatternWithVariables } from './chatflow_helpers';
import { findAgentByEmail, getAgentByChat, getAgentByChatZero, getAgentList, getAgentListChat, getChatHoursOfOperation, getConcurrentChat, getDomainStatus, getHoursofOperationById, getIncomingChatDetailsDB, getJSON, getListChatAgent, getnodesdata, getRoutingProfile, getRoutingProfileUser, getTransferAgentStatus, insertIncomingChatsDB } from './dao/callflow.dao';
import { fetchApi, getResponseData } from './fetch_api';

import { CLIENT_RENEG_LIMIT } from 'tls';
import { v4 as uuidv4 } from 'uuid';
import { conditionsMapper } from './conditions_mapper';
// import { createSession } from './dao/chat.dao';
import { updateChatMessage } from './dao/webChat.dao';
import { ioredisChat, otherRedisClient } from './plugins/db';
import { RedisAdapters } from './redisAdapters';

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
    // this.chatAdapter.broadcastMessage("waitingQueueUpdate", channelId?.queueUpdate);
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
    console.log('ChatFlowHandler >>>>>>', this.channelInformation);
    console.log('this.callflowId >>>>>>', this.callflowId);

    const io = ioredisChat;
    const channelId = JSON.parse(channel ?? 'null');
    // await ChatFlowChatStateHandler(channelId, this.chatAdapter);
    // channelId.targetLang = 'ta'
    if (!this.callflowId) {
      this.callflowId = channelId?.callFlowId;
    }

    if (this.callflowId) {
      if (this.nodes && this.nodes?.length === 0) {
        console.log('this.callflowId88888888 >>>>>>', this.callflowId);
        const nodesString: any = await getnodesdata(this.callflowId);

        // console.log('this.nodes >>>>>>', nodesString);
        this.nodes = JSON.parse(nodesString);
      }

      if (channelId?.channelType === 'Chat' && channelId?.call_state === 'init' && channelId.liveTransfer !== 1) {
        const startingNode = await this.getStartNode(this.nodes);
        const currentNodeId = startingNode?.data?.children?.length > 0 ? startingNode?.data?.children[0]?.childId : '';
        return this.handleChatFlowType(currentNodeId, channelId, this.nodes, io);
      } {
        let nextNode = channelId.btnValue;

        if (channelId.inputParams) this.inputParams = channelId.inputParams;

        if (channelId.btnValue && channelId?.btnValue?.includes('transferToQueue')) {
          const nextNodedata = this.nodes?.find((nodes: any) => nodes?.data?.nodeId === channelId?.btnValue) ?? { id: '' };
          nextNode = nextNodedata.id;
        }
        if (channelId.liveTransfer !== 1) {
          return this.handleChatFlowType(nextNode, channelId, this.nodes, io);
        } {
          // const checkBusinessHours: any = await getChatHoursOfOperation(channelId);
          // if (checkBusinessHours === true) {
          console.log('liveTransfer>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>');
          let liveAgent: any = [];
          // liveAgent = await getListChatAgent(channelId);
          liveAgent = await this.getAvailableAgentChatList(channelId);
          console.log('liveTransfer>>>>>>>>>>>>>>>>>>>', liveAgent);
          if (liveAgent?.length === 0) {
            this.chatAdapter.redisadaptor.resetQueue(channelId?.domain_id);
            console.log('INITIATING AGENT FINDER', liveAgent);
            this.waitForLiveAgentResponse(channelId);
            // await new Promise(async (resolve, reject) => {
            //   try {
            //     const list = [];
            //     await this.LIVEBLPOP(channelId?.domain_id, list);
            //     if (list?.length > 0 && list[0] !== null) {
            //       liveAgent = list;
            //       resolve(true);
            //     } else {
            //       reject(false);
            //     }
            //   } catch (__: any) {
            //     reject(false);
            //   }
            // }).catch((_: any) => {
            //   console.log('err');
            // });
          }
          if (liveAgent && liveAgent?.length > 0) {
            console.log('FOUND AN AVAILABLE AGENT ...');
            await handleTransferToLiveAgent(this.chatAdapter, channelId, null, liveAgent);
          } else {
            const getDomainResult: any = await getDomainStatus(channelId?.domain_id);
            this.chatAdapter?.io?.to(channelId?.session_id).emit('chat_response', JSON.stringify({ session_id: channelId?.session_id, req_type: 'DISCONNECT', domainStatus: getDomainResult }));
          }
          // } else {
          //   this.chatAdapter?.io?.to(channelId?.session_id).emit('chat_response', JSON.stringify({ session_id: channelId?.session_id, req_type: 'DISCONNECT', checkBusinessHours: false }));
          // }
        }
      }

    }
  }

  public async handleChatFlowType(nodeId: any, channelId: any, node: any, io: any) {
    const currentNode: any = await this.getNodeDataById(nodeId, node);
    this.currentNodeInfo = currentNode;
    if (this.blpopTimeout || !this.channelInformation?.session_id) {
      const getDomainResult: any = await getDomainStatus(channelId?.domain_id);
      this.chatAdapter?.io?.to(channelId?.session_id).emit('chat_response', JSON.stringify({ session_id: channelId?.session_id, req_type: 'DISCONNECT', domainStatus: getDomainResult }));
      return;
    }

    console.log('currentNode?.type >>>>', currentNode?.type);
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
            console.log(mergedObject, 'keykeykeykeymergedObject');
            for (const [key, value] of Object.entries(mergedObject)) {
            // console.log(key,"keykeykeykey")

              /* tslint:disable */
              _formattedSelectedKeys.push({ tempkey: '${context.' + key + '}', originalkey: key, [key]: value });
              this.addContextVariable(key, value);
            
          }
        }
        
          const updatedMessage = availableContexts?.length > 0 ? replacechatPatternWithVariables(availableContexts, text, _formattedSelectedKeys) : text;
          const data = { ...currentNode?.data?.information };
          data.message = updatedMessage;
          const targetRestructured: any = { currentNode, data, session_id: channelId?.session_id };
          targetRestructured.msg = updatedMessage;
          const nextNode = currentNode?.data?.children?.find((nodes: any) => nodes?.type === 'success') ?? { childId: '' };
          const nextNodedetails: any = await this.getNodeDataById(nextNode?.childId, node);
          targetRestructured.currentNode = nextNodedetails;
          let message: any = {};
          try {
            if (this.sourceLang !== channelId?.targetLang) {
              await Promise.all(targetRestructured?.currentNode?.data?.children?.map(async (list: any) => {
                const translateResult = await this.translateItem(list?.type, channelId);
                list.type = translateResult;
              }));

              const translateMessageResult = await this.translateItem(targetRestructured.msg, channelId);
              message = {
                messageID: uuidv4(),
                sessionId: channelId?.session_id,
                message: translateMessageResult,
                sendAt: Date.now(),
                messageType: 'text',
                pdfurl: '',
                isBot: true,
                isCustomer: false,
                fileType: '',
                direction: 'OUT',
                isLike: 0
              };
            } else {
              message = {
                messageID: uuidv4(),
                sessionId: channelId?.session_id,
                message: targetRestructured.msg,
                sendAt: Date.now(),
                messageType: 'text',
                pdfurl: '',
                isBot: true,
                isCustomer: false,
                fileType: '',
                direction: 'OUT',
                isLike: 0
              };
            }
            const messageList: any = { message };
            const messageType: any = { agentText: messageList };
            const stringifiedMessage: any = { message: messageType };
            stringifiedMessage.ChatHistoryId = channelId?.ChatHistoryId;
            stringifiedMessage.ChatDurationHistoryId = channelId?.ChatDurationHistoryId;
            try {
              await updateChatMessage(stringifiedMessage);
            } catch (err) {
              console.log(err);
            }

            this.chatAdapter?.io?.to(channelId?.session_id).emit('OneToOneChatReceive', message);
            this.chatAdapter?.io?.to(channelId?.session_id).emit('chat_response', JSON.stringify(targetRestructured));
          } catch (error) {
            console.error('Error:', error);
          }
          // await this.waitForPlayPromptSuccessResponse();
          // const nextNode = currentNode?.data?.children?.find((nodes: any) => nodes?.type === "success") ?? { childId: "" };
          // return this.handleChatFlowType(nextNode?.childId, channelId, node, io);
        } catch (error) {
          const nextNode = currentNode?.data?.children?.find((element: any) => element?.type === 'error') ?? { childId: '' };
          return this.handleChatFlowType(nextNode?.childId, channelId, node, io);
        }
        break;
      }
      case 'playPrompt': {
        try {
          this.channelInformation = { ...this.channelInformation, response: 'pending' };
          this.playpromptSuccessTimeout = false;
          const text = currentNode?.data?.information?.message;
          const availableContexts = text?.match(contextPattern);
          const updatedMessage = availableContexts?.length > 0 ? replacePatternWithVariables(availableContexts, text, this.context) : text;
          const data = { ...currentNode?.data?.information };
          data.message = updatedMessage;
          const targetRestructured = { ...currentNode, data, session_id: channelId?.session_id };
          this.chatAdapter?.io?.to(channelId?.session_id).emit('chat_response', JSON.stringify(targetRestructured));
          // console.log(currentNode, "currentNodecurrentNode")
          // await this.waitForPlayPromptSuccessResponse();
          // console.log(currentNode,"currentNodecurrentNode")
          const nextNode = currentNode?.data?.children?.find((nodes: any) => nodes?.type === 'success') ?? { childId: '' };
          return this.handleChatFlowType(nextNode?.childId, channelId, node, io);
        } catch (error) {
          const nextNode = currentNode?.data?.children?.find((element: any) => element?.type === 'error') ?? { childId: '' };
          return this.handleChatFlowType(nextNode?.childId, channelId, node, io);
        }
        break;
      }
      case 'integration': {
        try {
          this.chatAdapter?.io?.to(channelId?.session_id).emit('chat_response', JSON.stringify(currentNode));
        } catch (error: any) {
          const nextNode = currentNode?.data?.children?.find((nodes: any) => nodes?.type === 'error') ?? { childId: '' };
          return this.handleChatFlowType(nextNode?.childId, channelId, node, io);
        }
        break;
      }
      case 'appointment': {
        try {
          this.chatAdapter?.io?.to(channelId?.session_id).emit('chat_response', JSON.stringify(currentNode));
        } catch (error: any) {
          const nextNode = currentNode?.data?.children?.find((nodes: any) => nodes?.type === 'error') ?? { childId: '' };
          return this.handleChatFlowType(nextNode?.childId, channelId, node, io);
        }
        break;
      }
      case 'customInput': {
        try {

          this.channelInformation = { ...this.channelInformation, response: 'pending' };
          this.playpromptSuccessTimeout = false;
          const text = currentNode?.data?.information?.message;
          const availableContexts = text?.match(contextPattern);
          const updatedMessage = availableContexts?.length > 0 ? replacePatternWithVariables(availableContexts, text, this.context) : text;
          const data = { ...currentNode?.data?.information };
          data.message = updatedMessage;
          const targetRestructured = { currentNode, data, session_id: channelId?.session_id };
          console.log(' <<<<< targetRestructured >>>>', targetRestructured);
          await Promise.all(targetRestructured?.currentNode?.data?.children?.map(async (list: any) => {
            const translateResult = await this.translateItem(list?.type, channelId);
            console.log('translateResult>>>>>>>>>>>>>>', list?.type, translateResult);
            list.type = translateResult;
          }));

          this.chatAdapter?.io?.of('/')?.to(channelId?.session_id).emit('chat_response', JSON.stringify(targetRestructured));
          // this.chatAdapter.io.of("/").to(channelId?.session_id).emit("OneToOneChatReceive", "msg");
          // await this.waitForPlayPromptSuccessResponse();
          // const nextNode = currentNode?.data?.children?.find((nodes: any) => nodes?.type === "Health Tips") ?? { childId: "" };
          // console.log("nextNode >>>>>>", nextNode);
          // return this.handleChatFlowType(nextNode?.childId, channelId, node, io);
        } catch (error) {
          const nextNode = currentNode?.data?.children?.find((element: any) => element?.type === 'error') ?? { childId: '' };
          return this.handleChatFlowType(nextNode?.childId, channelId, node, io);
        }
        break;
      }
      case 'transferToQueue': {
        try {
          console.log('channelId', channelId);
          console.log('currentNode', currentNode);
          console.log('TRANSFER USER INTO QUEUE INITIATED...');
          const getIncomingChatDetails: any = await getIncomingChatDetailsDB(channelId, currentNode.data?.information?.transferTo);
          // const getIncomingChatDetails: any = [];
          console.log('getIncomingChatDetails', getIncomingChatDetails);
          const getIncomingChats: any = JSON.parse(getIncomingChatDetails[0]?.queueList ?? '[]');
          console.log('getIncomingChats', getIncomingChats);
          const incomingChat = getIncomingChats?.filter((val: any) => {
            return val?.session_id == channelId.session_id
          });
          console.log('incomingChat', incomingChat);
          if (incomingChat.length > 0) {
            console.log('incomingChat if', incomingChat);
            channelId.queueList = [...getIncomingChats];
          } else {
            console.log('incomingChat else', incomingChat);
            channelId.queueList = [...getIncomingChats, JSON.parse(JSON.stringify(channelId))];
          }
          let insertIncomingChats: any = await insertIncomingChatsDB(channelId, currentNode.data?.information?.transferTo);
          console.log('insertIncomingChats', insertIncomingChats);
          this.channelInformation = { ...this.channelInformation, response: 'pending' };
          // const routingProfiles = await getRoutingProfile(currentNode.data?.information?.transferTo);
          // console.log('routingProfiles ', { routingProfiles });
          // await transferUserInsideQueue(this.redisadaptor, channelId, currentNode);
          this.waitForAgentResponse(channelId);
          let agentDetails: any = [];
          if (currentNode.data?.information?.type === 2) {
            const getAgentResults: any = await getTransferAgentStatus(currentNode.data?.information?.transferTo, channelId);
            agentDetails = getAgentResults[0];
          } else {
            console.log('TRANSFER TO QUEUE COMPLETED');
            agentDetails = await this.getAvaiableAgentChatListBtn(channelId, currentNode.data?.information?.transferTo);
            // agentDetails = await getAgentListChat(currentNode.data?.information?.transferTo, channelId, this.redisadaptor);
          }
          console.log('agentDetails >>>>>>>', agentDetails);

          if (agentDetails?.length === 0) {
            console.log('Inside agentDetails >>>>.', this.chatAdapter.redisadaptor.resetQueue);
            this.chatAdapter.redisadaptor.resetQueue(channelId?.domain_id);
            console.log('INITIATING AGENT FINDER');
            // await new Promise(async (resolve, reject) => {
            //   try {
            //     const list = [];
            //     await this.BLPOP(channelId?.domain_id, list, routingProfiles, channelId?.session_id, currentNode.data?.information?.transferTo);
            //     if (list?.length > 0 && list[0] !== null) {
            //       agentDetails = list;
            //       resolve(true);
            //     } else {
            //       reject(false);
            //     }
            //   } catch (__: any) {
            //     reject(false);
            //   }
            // }).catch((_: any) => {
            //   const nextNodeData = currentNode?.data?.children?.find((element: any) => element?.type === 'error') ?? { childId: '' };
            //   return this.handleChatFlowType(nextNodeData?.childId, channelId, node, io);
            // });
          }

          if (agentDetails && agentDetails?.length > 0) {
            agentDetails.queueId = currentNode.data?.information?.transferTo;
            console.log('FOUND AN AVAILABLE AGENT ...');
            await handleTransferToQueue(this.chatAdapter, channelId, currentNode, agentDetails);
            this.channelInformation = { ...this.channelInformation, response: 'success', status: null };
            const status = await this.waitForCallStatusResponse();
            // INFO: PUT THIS BLOCK IN A LOOP TO KEEP CHECKING FOR AVAILABLE USER
            console.log('status >>>>>>>', status);
            if (status && currentNode.data?.information?.type !== 2) {
              const checkAgentDetails: any = await getAgentListChat(currentNode.data?.information?.transferTo, channelId, this.redisadaptor);

              console.log('checkAgentDetails >>>>>>>', checkAgentDetails);
              if (checkAgentDetails?.length) {
                this.channelInformation = { ...this.channelInformation, response: 'pending', status: null };
                await handleTransferToQueue(this.redisadaptor, channelId, currentNode, checkAgentDetails);
                const statusNew = await this.waitForSecondCallStatusResponse();
                if (statusNew) {
                  console.log('channelId?.channeltype >>>>>>', channelId?.channeltype);
                  if (channelId?.channeltype === 'Chat') {
                    this.chatAdapter?.io?.of('/')?.to(channelId?.session_id).emit('chat_response', JSON.stringify(agentDetails));
                  } else {
                    // this.redisadaptor.publishMessage(channelId?.domain_id, JSON.stringify({ session_id: channelId?.session_id, req_type: 'DISCONNECT' }));
                  }
                }
              } else {
                console.log('channelId?.channeltype >>>>>>', channelId?.channeltype);
                if (channelId?.channeltype === 'Chat') {
                  console.log('Inside chant', this.redisadaptor.publishMessageToWebChat);
                  this.chatAdapter?.io?.of('/')?.to(channelId?.session_id).emit('chat_response', JSON.stringify(agentDetails));
                  // this.publishMessageToWebChat(channelId?.domain_id, JSON.stringify({ session_id: channelId?.session_id, req_type: 'DISCONNECT' }));
                } else {
                  this.redisadaptor.publishMessage(JSON.stringify({ session_id: channelId?.session_id, req_type: 'DISCONNECT' }));
                }
              }
            }
          } else {
            const nextNodes = currentNode?.data?.children?.find((element: any) => element?.type === 'error') ?? { childId: '' };
            return this.handleChatFlowType(nextNodes?.childId, channelId, node, io);
          }
          // } else {
          //   this.chatAdapter?.io?.to(channelId?.session_id).emit('chat_response', JSON.stringify({ session_id: channelId?.session_id, req_type: 'DISCONNECT', checkBusinessHours: false }));
          // }
        } catch (error: any) {

          console.log('error >>>>>>.', error);
          const nextNodeData = currentNode?.data?.children?.find((element: any) => element?.type === 'error') ?? { childId: '' };
          return this.handleChatFlowType(nextNodeData?.childId, channelId, node, io);
        }
        break;
      }
      case 'branchHoursofOperation': {
        try {
          this.channelInformation = { ...this.channelInformation, response: 'pending' };
          this.businessHoursTimeout = false;
          const cid = currentNode?.data?.information?.outOfBusiness?.cid;
          const businessHours = await getHoursofOperationById(cid);
          const targetRestructured = { ...currentNode, session_id: channelId?.session_id, status: businessHours ? 1 : 0 };
          // playSendprompt(channelId?.session_id)
          this.chatAdapter?.io?.to(channelId?.session_id).emit('chat_response', JSON.stringify(targetRestructured));
          await this.waitForBusinessHoursResponse();
          if (businessHours) {
            const nextNode = currentNode?.data?.children?.find((element: any) => element?.type === 'success') ?? { childId: '' };
            return this.handleChatFlowType(nextNode?.childId, channelId, node, io);
          }
          const nextNodeData = currentNode?.data?.children?.find((element: any) => element?.type === 'fail') ?? { childId: '' };
          return this.handleChatFlowType(nextNodeData?.childId, channelId, node, io);
        } catch (error: any) {
          const nextNodeData = currentNode?.data?.children?.find((element: any) => element?.type === 'error') ?? { childId: '' };
          return this.handleChatFlowType(nextNodeData?.childId, channelId, node, io);
        }
        break;
      }
      case 'makeApiCall': {
        try {
          const type = currentNode?.data?.information?.getMethodApi;
          const rawEndURL = currentNode?.data?.information?.endPointUrl.lastIndexOf('$');
          // console.log(rawEndURL, currentNode?.data?.information?.endPointUrl,"currentNodecurrentNode")
          const endPointUrl = type === 'get' ? currentNode?.data?.information?.endPointUrl.substring(0, rawEndURL) : currentNode?.data?.information?.endPointUrl;
          // const endPointUrl = currentNode?.data?.information?.endPointUrl;
          const url = type === 'get' ? `${endPointUrl}${this.inputParams[0]?.mobile_no}` : `${endPointUrl}`; // 'https://qaccaasapi.worktual.co.uk/core_service/v1/get_mobile_number_status/94988515546677'//;
          // const url = `${endPointUrl}`;
          console.log('url >>>>', url);
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
              const _formattedSelectedKeys = [];
              for (const [key, value] of Object.entries(finalResponse)) {
                if (selectedKeys.has(key)) {

                  /* tslint:disable */
                  _formattedSelectedKeys.push({ tempkey: '${context.' + key + '}', originalkey: key, [key]: value });
                  this.addContextVariable(key, value);
                }
              }
              const finalKeys: any = this.context;
              this.context = {};
              targetRestructured.selectedKeys = finalKeys;
              console.log(nodeNext, "nodeNextnodeNextnodeNext")
              if (nodeNext.childId.includes('customInput')) {
                const nextNodedetails: any = await this.getNodeDataById(nodeNext.childId, node);
                const _cpynextNodedetails: any = { ...nextNodedetails };
                const _childrens = _cpynextNodedetails?.data?.children || {};
                for (let _index = 0; _index < _childrens.length; _index++) {
                  const element = _childrens[_index];
                  for (let _tempi = 0; _tempi < _formattedSelectedKeys.length; _tempi++) {
                    const tempelement = _formattedSelectedKeys[_tempi];
                    if (element.type === tempelement.tempkey) {
                      // console.log(_cpynextNodedetails?.data?.children[_index]);
                      _childrens[_index].type = tempelement.value;

                    }

                  }

                }
                // console.log("_childrens >", _childrens);
                _cpynextNodedetails.data.children = _childrens;
                targetRestructured.currentNode = _cpynextNodedetails;
                targetRestructured.result = finalResponse;
                this.chatAdapter?.io?.to(channelId?.session_id).emit('chat_response', JSON.stringify(targetRestructured));

              } else if (nodeNext.childId.includes('messageFlow')) {
                const nextNodedetails: any = await this.getNodeDataById(nodeNext.childId, node);
                console.log('nextNodedetails >>>>>>>', nextNodedetails);

                const text = nextNodedetails?.data?.information?.message;
                const availableContexts = text?.match(contextPattern);
                const updatedMessage = availableContexts?.length > 0 ? replacechatPatternWithVariables(availableContexts, text, _formattedSelectedKeys) : text;
                const data = { ...nextNodedetails?.data?.information };
                data.message = `${updatedMessage}`;
                const targetRestructured: any = { nextNodedetails, data, session_id: channelId?.session_id };
                targetRestructured.msg = updatedMessage;
                const messagenextNode = nextNodedetails?.data?.children?.find((nodes: any) => nodes?.type === 'success') ?? { childId: '' };

                let messagenextNodedetails: any = {};
                if (messagenextNode.childId) {
                  messagenextNodedetails = await this.getNodeDataById(messagenextNode?.childId, node);
                }

                targetRestructured.currentNode = nextNodedetails;
                const message: any = {
                  messageID: uuidv4(),
                  sessionId: channelId?.session_id,
                  message: `${targetRestructured.msg}`,
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
                  console.log("stringifiedMessage=-=-=-1", stringifiedMessage)
                  await updateChatMessage(stringifiedMessage)
                } catch (err) {
                  console.log(err)
                }
                //createSession(stringifiedMessage, channelId?.session_id);
                this.chatAdapter?.io?.to(channelId?.session_id).emit('OneToOneChatReceive', message);
                const _cpynextNodedetails: any = { ...messagenextNodedetails };
                targetRestructured.currentNode = _cpynextNodedetails;

                this.chatAdapter?.io?.to(channelId?.session_id).emit('chat_response', JSON.stringify(targetRestructured));
              } else if (nodeNext.childId.includes('makeApiCall')) {
                return this.handleChatFlowType(nodeNext?.childId, channelId, node, io);
              }

            } else {
              const nodeNext = currentNode?.data?.children?.find((nodes: any) => nodes?.type === 'error') ?? { childId: '' };
              // console.log(currentNode,"nodeNextnodeNextnodeNextnodeNext999999")
              if (nodeNext.childId.includes('messageFlow')) {
                const nextNodedetails: any = await this.getNodeDataById(nodeNext.childId, node);
                const text = nextNodedetails?.data?.information?.message;
                const availableContexts = text?.match(contextPattern);
                const updatedMessage = availableContexts?.length > 0 ? replacePatternWithVariables(availableContexts, text, this.context) : text;
                const data = { ...nextNodedetails?.data?.information };
                data.message = `${updatedMessage}`;
                const targetRestructured: any = { nextNodedetails, data, session_id: channelId?.session_id };
                targetRestructured.msg = nextNodedetails?.data?.information.message;
                const messagenextNode = nextNodedetails?.data?.children?.find((nodes: any) => nodes?.type === 'success') ?? { childId: '' };

                let messagenextNodedetails: any = {};
                if (messagenextNode.childId) {
                  messagenextNodedetails = await this.getNodeDataById(messagenextNode?.childId, node);
                }

                const message: any = {
                  messageID: uuidv4(),
                  sessionId: channelId?.session_id,
                  message: `${targetRestructured.msg}`,
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
                  console.log('stringifiedMessage-=-=-=78567', stringifiedMessage)
                  await updateChatMessage(stringifiedMessage)
                } catch (err) {
                  console.log(err)
                }
                //createSession(stringifiedMessage, channelId?.session_id);
                this.chatAdapter?.io?.to(channelId?.session_id).emit('OneToOneChatReceive', message);
                const _cpynextNodedetails: any = { ...messagenextNodedetails };
                targetRestructured.currentNode = _cpynextNodedetails;

                console.log('_cpynextNodedetails >>>>>>', _cpynextNodedetails);
                this.chatAdapter?.io?.to(channelId?.session_id).emit('chat_response', JSON.stringify(targetRestructured));
              }
            }

            // const nodeNext = currentNode?.data?.children?.find((nodes: any) => nodes?.type === 'success') ?? { childId: '' };
            // return this.handleChatFlowType(nodeNext?.childId, channelId, node, io);
          }
          // const nextNode = currentNode?.data?.children?.find((nodes: any) => nodes?.type === 'error') ?? { childId: '' };
          // return this.handleChatFlowType(nextNode?.childId, channelId, node, io);
        } catch (error: any) {
          const nextNode = currentNode?.data?.children?.find((nodes: any) => nodes?.type === 'error') ?? { childId: '' };
          return this.handleChatFlowType(nextNode?.childId, channelId, node, io);
        }
        break;
      }
      case 'checkCondition': {
        try {
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
      case 'integration': {
        try {
          this.chatAdapter?.io?.to(channelId?.session_id).emit('chat_response', JSON.stringify(currentNode));
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
              // return this.handleChatFlowType(loopTo?.childId, channelId, node, io);
              this.chatAdapter?.io?.to(channelId?.session_id).emit('chat_response', JSON.stringify(targetRestructured));
            }

          } else {
            const nextNode = currentNode?.data?.children?.find((nodes: any) => nodes?.type === 'complete') ?? { childId: '' };
            const nextNodedetails: any = await this.getNodeDataById(nextNode.childId, node);

            console.log('Loop Complete>>>>>>', nextNode.childId);
            return this.handleChatFlowType(nextNode?.childId, channelId, node, io);
            // const targetRestructured: any = { nextNodedetails, session_id: channelId?.session_id };
            // targetRestructured.currentNode = nextNodedetails
            // this.chatAdapter?.io?.to(channelId?.session_id).emit('chat_response', JSON.stringify(targetRestructured));
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
        return;
      }
      default: {
        console.log('Default calling');
        // if (channelId?.disconnectedBy) {
        this.blpopTimeout = true;
        const getDomainResult: any = await getDomainStatus(channelId?.domain_id)
        this.chatAdapter?.io?.to(channelId?.session_id).emit('chat_response', JSON.stringify({ session_id: channelId?.session_id, req_type: 'DISCONNECT' }));
        return;
        // }
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

  public getCustomhours = (nexttargetedge, msg: any) => {
    return nexttargetedge.filter((list: any) => list.sourceHandle === msg);
  }

  public gettargetedges = (edgeid: any, edge: any) => {
    return edge?.find((list: any) => list.source === edgeid);
  }

  private async waitForPlayPromptSuccessResponse() {
    return new Promise((resolve: any, reject: any) => {
      let timeout = 30;
      const inteval = setInterval(() => {
        if (timeout === 0 || this.playpromptSuccessTimeout) {

          console.log('Clear Intervel callling >>>>>', this.channelInformation?.response);
          clearInterval(inteval);
          reject(false);
        }

        if (this.channelInformation?.response === 'success') {
          console.log('Succs Clear Intervel callling >>>>>');
          this.channelInformation = { ...this.channelInformation, response: 'pending' };
          clearInterval(inteval);
          resolve(true);
        }
        timeout -= 1;
      }, 1000);
    });
  }

  private async waitForSuccessResponse() {
    try {
      return new Promise((resolve: any, reject: any) => {
        try {
          let timeout = 30;
          const inteval = setInterval(() => {
            if (timeout === 0 || this.successTimeout) {
              clearInterval(inteval);
              reject(false);
            }
            if (this.channelInformation?.response === 'success') {
              this.channelInformation = { ...this.channelInformation, response: 'pending' };
              clearInterval(inteval);
              resolve(true);
            }
            timeout -= 1;
          }, 1000);
        } catch (error) {
          console.log('Fi>>>>>', error);
          // clearInterval(inteval);
          reject(false);
        }

      });
    } catch (error) {
      console.log('error', error);
    }

  }

  private async waitForAgentResponse(channelId: any) {
    try {

      let timeout = 60;
      const inteval = setInterval(() => {
        if (timeout === 0 || this.successTimeout) {
          clearInterval(inteval);
          this.blpopTimeout = true;
          // this.chatAdapter?.io?.to(channelId?.session_id).emit("chat_response", JSON.stringify({ session_id: channelId?.session_id, req_type: "DISCONNECT" }));
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

  private async waitForDTMFSuccessResponse(isVariable: any) {
    return new Promise((resolve: any, _: any) => {
      let timeout = 180;
      const inteval = setInterval(() => {
        if (timeout === 0 || this.dtmfSuccessTimeout) {
          clearInterval(inteval);
          resolve('timeout');
        }
        if (this.channelInformation?.response === 'success') {
          this.channelInformation = { ...this.channelInformation, response: 'pending' };
          clearInterval(inteval);
          isVariable ? resolve(this.channelInformation?.variable) : resolve(this.channelInformation?.value);
        }
        timeout -= 1;
      }, 1000);
    });
  }

  private async waitForBusinessHoursResponse() {
    return new Promise((resolve: any, reject: any) => {
      let timeout = 30;
      const inteval = setInterval(() => {
        if (timeout === 0 || this.businessHoursTimeout) {
          clearInterval(inteval);
          reject(false);
        }
        if (this.channelInformation?.response === 'success') {
          this.channelInformation = { ...this.channelInformation, response: 'pending' };
          clearInterval(inteval);
          resolve(true);
        }
        timeout -= 1;
      }, 1000);
    });
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

  private publishMessageToWebChat(domainId: any, key: any) {
    console.log('CHAT: ', `${domainId}_${process.env.CHATRESPONSECHANNEL}`, JSON.parse(key));
    this.chatAdapter.redisadaptor?.dub.rpush(`${domainId}_${process.env.CHATRESPONSECHANNEL}`, key);
  }

  private async BLPOP(domainId: any, list: any, routingProfiles: any, sessionId: any, userId: any) {
    // console.log('090990vxsvxjhbsvjhxbjshbx', ChatFlowAdapters.queueDetails?.at(0)?.sessionId)
    // console.log('sessionId', sessionId)
    // if (ChatFlowAdapters.queueDetails?.at(0)?.sessionId === sessionId) {
    // console.log("SEARCHING FOR AVAILABLE AGENT ...", this.chatAdapter.redisadaptor.dub);
    await otherRedisClient.blpop(`${domainId}_${process.env.AGENTTRACKERCHANNEL}`, 1).then(async (data: any) => {
      // console.log("this.blpopTimeout >>>>>>>", this.blpopTimeout, data)
      if (this.blpopTimeout) {
        console.log('CANCELLING SEARCHING FOR AGENT BECAUSE OF DISCONNECTED CALL ...');
        list.push(null);
        return list;
      }
      if (data !== null && data?.length > 1) {
        const parsedData = JSON.parse(data?.at(1));
        if ((parsedData?.statusName === 'Ready' || parsedData?.statusName === 'Transfer Only') && (routingProfiles?.includes(parsedData?.routingProfileId) || userId === parsedData?.UserID)) {
          console.log('AGENT FOUND AS: ', data);
          list.push([parsedData] ?? null);
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
          list.push([parsedData] ?? null);
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

  private async getAvaiableAgentChatListBtn(channel: any, qid: any) {
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
          console.log('sortedAgents', sortedAgents);

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
                    // else {
                    //   chatLimitAchievedAgents.push(tempAgent);
                    //   agentsToBeAssigned = agentsToBeAssigned.filter((val: any) => val.ext !== tempAgent.ext);
                    //   assignFunction();
                    // }
                  })
                } else if (chatLimitData?.isRoutingProfile) {
                  const routings = {};
                  RoutingProfileUser.map((item: any) => {
                    routings[item.RoutingProfileId] = item.concurrentChat;
                  });
                  console.log('routings', routings);
                  console.log('tempAgent', tempAgent);
                  for (let [key, value] of Object.entries(routings)) {
                    if (tempAgent.routingProfileId == key) {
                      console.log('first if');
                      if (tempAgent.currentChatCount == value) {
                        console.log('second if');
                        chatLimitAchievedAgents.push(tempAgent);
                        agentsToBeAssigned = agentsToBeAssigned.filter((val: any) => val.ext !== tempAgent.ext);
                        if (chatLimitAchievedAgents.length !== sortedAgents.length) {
                          assignFunction();
                        }
                      } else {
                        console.log('else');
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
                console.log('assignFunction tempAgent', tempAgent);
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
              console.log('tempAgent', tempAgent);
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
  private async getAvailableAgentChatList(channel: any) {
    let agentDetails: any = [];
    return new Promise((resolve: any) => {
      otherRedisClient.hgetall(`${channel?.domain_id}_presence`).then(async (data: any) => {
        const result = Object.values(data).map((value: any) => JSON.parse(value));
        // const result: any = await getLiveAgentsList(data, channel);
        // console.log("\ngetAvailableAgentChatList result:", result);
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
          console.log('sortedAgents', sortedAgents);

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
                    // else {
                    //   chatLimitAchievedAgents.push(tempAgent);
                    //   agentsToBeAssigned = agentsToBeAssigned.filter((val: any) => val.ext !== tempAgent.ext);
                    //   assignFunction();
                    // }
                  })
                } else if (chatLimitData?.isRoutingProfile) {
                  const routings = {};
                  RoutingProfileUser.map((item: any) => {
                    routings[item.RoutingProfileId] = item.concurrentChat;
                  });
                  console.log('routings', routings);
                  console.log('tempAgent', tempAgent);
                  for (let [key, value] of Object.entries(routings)) {
                    if (tempAgent.routingProfileId == key) {
                      console.log('first if');
                      if (tempAgent.currentChatCount == value) {
                        console.log('second if');
                        chatLimitAchievedAgents.push(tempAgent);
                        agentsToBeAssigned = agentsToBeAssigned.filter((val: any) => val.ext !== tempAgent.ext);
                        if (chatLimitAchievedAgents.length !== sortedAgents.length) {
                          assignFunction();
                        }
                      } else {
                        console.log('else');
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
                console.log('assignFunction tempAgent', tempAgent);
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
              console.log('tempAgent', tempAgent);
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
}