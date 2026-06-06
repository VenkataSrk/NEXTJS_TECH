import { CallflowCallStateHandler } from './callflow_call_state_handler';
import * as Operation from './callflow_helpers';
import { conditionsMapper } from './conditions_mapper';
import { getAgentDetailsById, getHoursofOperationByIdnew, isAgentStatusActive } from './dao/callflow.dao';
import { getchanneldata, getnodesdata, getnodesdataForNewCallFlow } from './dao/channeldata.dao';
import { fetchApi, getResponseData } from './fetch_api';
import { InvalidCallsHandler } from './invalid_calls_handler';
import { notificationHandler } from './notificationHandler';
import { OutboundCallsHandler } from './outbound_calls_handler';
import { ioredisChat } from './plugins/db';
import { loggerError } from './plugins/logger';

export class RedisConstAdapters {
  public context: any;
  public blpopTimeout: any;
  public successTimeout: any;
  public dtmfSuccessTimeout: any;
  public responseTimeout: any;
  public secondResponseTimeout: any;
  public playpromptSuccessTimeout: any;
  public businessHoursTimeout: any;
  public voiceAuthenticationTimeout: any;
  private redisadaptor: any;
  private channelInformation: any;
  private sessionLoops: any;
  private callflowId: any;
  private nodes: any;

  constructor(reidisAdapter?: any) {
    this.redisadaptor = reidisAdapter;
    this.channelInformation = null;
    this.successTimeout = false;
    this.responseTimeout = false;
    this.secondResponseTimeout = false;
    this.playpromptSuccessTimeout = false;
    this.businessHoursTimeout = false;
    this.voiceAuthenticationTimeout = false;
    this.dtmfSuccessTimeout = false;
    this.context = {} as any;
    this.sessionLoops = {} as any;
    this.callflowId = null;
    this.nodes = [];
  }

  public addContextVariable(key: any, value: any) {
    if (this.context) {
      this.context[key] = value;
    }
  }

  public clearAllTimeout() {
    this.successTimeout = true;
    this.responseTimeout = true;
    this.secondResponseTimeout = true;
    this.playpromptSuccessTimeout = true;
    this.dtmfSuccessTimeout = true;
    this.businessHoursTimeout = true;
    this.voiceAuthenticationTimeout = true;
  }

  public setAllTimeout() {
    this.successTimeout = false;
    this.responseTimeout = false;
    this.secondResponseTimeout = false;
    this.playpromptSuccessTimeout = false;
    this.dtmfSuccessTimeout = false;
    this.businessHoursTimeout = false;
    this.voiceAuthenticationTimeout = false;
  }

  public async callflowhandler(channel: any) {
    try {
      if (channel && JSON.stringify(this.channelInformation) !== JSON.stringify(channel) && !JSON.parse(channel ?? 'null')?.queueUpdate) {
        this.channelInformation = JSON.parse(channel ?? 'null') ?? null;
      }
      const io = ioredisChat;
      const channelId = JSON.parse(channel ?? 'null');

      if (!this.callflowId && !channelId?.auto_attendant && !channelId?.virtual_agent) {
        if (!channelId?.call_flow_id) {
          const channeldata: any = await getchanneldata(channelId?.ddi);
          console.log('GET CHANNEL CALLFLOWID :::::::', channeldata?.[0]);
          this.callflowId = channeldata?.[0]?.callFlowId;
        } else {
          this.callflowId = channelId.call_flow_id;
        }
      }

      await InvalidCallsHandler(channelId, this.redisadaptor);
      await OutboundCallsHandler(channelId, io);
      await CallflowCallStateHandler(channelId, this.redisadaptor);
      await notificationHandler(channelId, io);
      await this.queueWeightTimeUpdate(channelId, io);
      await this.handleOtherFunctionalities(channelId);

      if (this.callflowId) {
        if (this.nodes && this.nodes?.length === 0) {
          const nodesString: any = await getnodesdata(this.callflowId, channelId?.domain_id);
          console.log('CALLFLOW DATA::::::::::::', nodesString);
          this.nodes = JSON.parse(nodesString);
        }
        if (channelId?.call_type === 'inbound' && channelId?.call_state === 'init') {
          const startingNode = await this.getStartNode(this.nodes);
          const currentNodeId = startingNode?.data?.children?.at(0)?.childId ?? '';
          const customerStatus: any = await isAgentStatusActive(channelId?.ddi);
          this.setAllTimeout();
          if (customerStatus?.[0]?.status === 1) {
            return this.handleCallflowType(currentNodeId, channelId, this.nodes, io, 0);
          }
          this.handleInvalidCalls(channelId);
        }
      }
    } catch (error) {
      loggerError('callflowhandler', channel, error);
    }
  }

  public async handleCallflowType(nodeId: any, channelId: any, node: any, io: any, loop: any) {
    try {
      const currentNode: any = await this.getNodeDataById(nodeId, node);
      if (!this.channelInformation?.session_id) {
        this.redisadaptor.publishMessage(JSON.stringify({ session_id: channelId?.session_id, req_type: 'DISCONNECT' }));
        return;
      }
      console.log('CURRENT NODE TYPE :::::::::', currentNode?.type);
      switch (currentNode?.type) {
        case 'entryPoint': {
          try {
            return this.handleCallflowType(currentNode.data.children.at(0).childId, channelId, node, io, 0);
          } catch (error) {
            loggerError('entryPoint', { nodeId, channelId, node, io, loop }, error);
          }
          break;
        }
        case 'playPrompt': {
          try {
            this.channelInformation = { ...this.channelInformation, response: 'pending' };
            this.playpromptSuccessTimeout = false;
            if (channelId) {
              const text = currentNode?.data?.information?.message;
              const availableContexts = text?.match(Operation.contextPattern);
              const updatedMessage = availableContexts?.length > 0 ? Operation.replacePatternWithVariables(availableContexts, text, this.context) : text;
              const data = { ...currentNode?.data?.information };
              data.message = updatedMessage;
              const targetRestructured = { loop, ...currentNode, data, session_id: channelId?.session_id };
              this.redisadaptor.publishMessage(JSON.stringify(targetRestructured));
              await this.waitForPlayPromptSuccessResponse();
            }
            console.log('PLAY PROMPT CHILD NODE::::::::::::', currentNode?.data?.children);
            const nextNode = currentNode?.data?.children?.find((nodes: any) => nodes?.type === 'success') ?? { childId: '' };
            console.log('PLAY PROMPT CHILD NODE RESULT::::::::::::', nextNode);
            return this.handleCallflowType(nextNode?.childId, channelId, node, io, 0);
          } catch (error) {
            loggerError('playPrompt', { nodeId, channelId, node, io, loop }, error);
            console.log('PLAY PROMPT CHILD NODE ERROR::::::::::::', currentNode?.data?.children, error);
            const nextNode = currentNode?.data?.children?.find((element: any) => element?.type === 'error') ?? { childId: '' };
            console.log('PLAY PROMPT CHILD NODE ERROR RESULT::::::::::::', nextNode);
            return this.handleCallflowType(nextNode?.childId, channelId, node, io, 0);
          }
          break;
        }
        case 'customInput': {
          try {
            this.channelInformation = { ...this.channelInformation, response: 'pending' };
            this.dtmfSuccessTimeout = false;
            const inputType = currentNode?.data?.information?.inputType;
            const dtmfinputCount = currentNode?.data?.information?.dtmfinputCount;
            const children = currentNode?.data?.children;
            // INFO: Input type 1 is for DTMF Input and type 2 is for BOT transfer
            if ([1, 2, 3].includes(inputType)) {
              const targetRestructured = { loop, ...currentNode, session_id: channelId?.session_id, data: { ...currentNode?.data?.information } };
              const dtmfinput = targetRestructured?.data?.dtmf ?? [];
              if (inputType === 1) {
                const arr: any = [];
                dtmfinputCount.forEach((list) => {
                  children.forEach((ss) => {
                    if (ss?.type === list.value) {
                      arr.push(ss?.type);
                    }
                  });
                });
                console.log('dtmfinputCount>>>>>>>>>>arr>>>>>', arr, 'dtmfinputCount', dtmfinputCount, 'children', children);
                targetRestructured.data = { ...targetRestructured.data, dtmf: arr };
              }
              this.redisadaptor?.publishMessage(JSON.stringify(targetRestructured));
              if (inputType === 1) {
                await this.waitForDTMFSuccessResponse(false).then((response: any) => {
                  if (!isNaN(response) && parseInt(response, 10) <= dtmfinput.length) {
                    const findChild = currentNode?.data?.children?.find((nodeData: any) => nodeData?.type?.toString() === response?.toString());
                    if (findChild) {
                      const nodeNextData = findChild ?? { childId: '' };
                      return this.handleCallflowType(nodeNextData?.childId, channelId, node, io, 0);
                    }
                    const nodeNext = currentNode?.data?.children?.find((nodes: any) => nodes?.type === 'success' ? 'success' : 'default') ?? { childId: '' };
                    return this.handleCallflowType(nodeNext?.childId, channelId, node, io, 0);
                  }
                  const nextNode = currentNode?.data?.children?.find((nodes: any) => nodes?.type === response) ?? { childId: '' };
                  return this.handleCallflowType(nextNode?.childId, channelId, node, io, 0);
                });
              } else if (inputType === 3) {
                await this.waitForDTMFSuccessResponse(true).then((response: any) => {
                  if (['default', 'timeout', 'error'].includes(response)) {
                    const nodeNext = currentNode?.data?.children?.find((element: any) => element?.type === response) ?? { childId: '' };
                    return this.handleCallflowType(nodeNext?.childId, channelId, node, io, 0);
                  }
                  this.addContextVariable(currentNode?.data?.information?.keyVariable, response);
                  const nextNodeData = currentNode?.data?.children?.find((element: any) => element?.type === 'validate') ?? { childId: '' };
                  return this.handleCallflowType(nextNodeData?.childId, channelId, node, io, 0);
                });
              }
            }
          } catch (error: any) {
            loggerError('customInput', { nodeId, channelId, node, io, loop }, error);
            const nextNode = currentNode?.data?.children?.find((nodee: any) => nodee?.type === 'error') ?? { childId: '' };
            return this.handleCallflowType(nextNode?.childId, channelId, node, io, 0);
          }
          break;
        }
        case 'branchHoursofOperation': {
          try {
            this.channelInformation = { ...this.channelInformation, response: 'pending' };
            this.businessHoursTimeout = false;
            const cid = currentNode?.data?.information?.outOfBusiness?.cid;
            const businessHours = await getHoursofOperationByIdnew(cid);
            const data = { ...currentNode?.data, status: businessHours ? 1 : 0 };
            const targetRestructured = { loop, ...currentNode, data, session_id: channelId?.session_id };
            this.redisadaptor?.publishMessage(JSON.stringify(targetRestructured));
            await this.waitForBusinessHoursResponse();
            console.log('BUSINESS HOURS TRUE OF FALSE::::::::', businessHours);
            if (businessHours) {
              console.log('BUSINESS HOURS INSIDE OPERTATION CHILD NODE::::::::::::', currentNode?.data?.children);
              const nextNode = currentNode?.data?.children?.find((element: any) => element?.type === 'success') ?? { childId: '' };
              console.log('BUSINESS HOURS INSIDE OPERTATION CHILD NODE RESULT::::::', nextNode);
              return this.handleCallflowType(nextNode?.childId, channelId, node, io, 0);
            }
            console.log('BUSINESS HOURS OUTSIDE OPERTATION CHILD NODE::::::::::::', currentNode?.data?.children);
            const nextNodeData = currentNode?.data?.children?.find((element: any) => element?.type === 'fail') ?? { childId: '' };
            console.log('BUSINESS HOURS OUTSIDE OPERTATION CHILD NODE RESULT::::::', nextNodeData);
            return this.handleCallflowType(nextNodeData?.childId, channelId, node, io, 0);
          } catch (error: any) {
            loggerError('branchHoursofOperation', { nodeId, channelId, node, io, loop }, error);
            const nextNodeData = currentNode?.data?.children?.find((element: any) => element?.type === 'error') ?? { childId: '' };
            return this.handleCallflowType(nextNodeData?.childId, channelId, node, io, 0);
          }
          break;
        }
        case 'transferToQueue': {
          try {
            if (['3', '5'].includes(currentNode?.data?.information?.type?.toString())) {
              console.log('TRANSFER TO EXTERNAL CALL::::::');
              const targetRestructured = {
                ...currentNode,
                session_id: channelId?.session_id,
                data: { ...currentNode?.data?.information, externalCallResume: currentNode?.data?.information?.externalCallResume === true ? 1 : 0 },
                ...(currentNode?.data?.information?.type?.toString() === '3' && { type: 'transferToExternalNumber' }),
                loop
              };
              this.redisadaptor?.publishMessage(JSON.stringify(targetRestructured));
            } else if (currentNode?.data?.information?.type?.toString() === '2') {
              console.log('TRANSFER TO AGENT CALL::::::');
              const uid = currentNode?.data?.information?.transferTo;
              if (uid) {
                const agentDetails: any = await getAgentDetailsById(uid, channelId?.domain_id);
                let prompt = '';
                if (currentNode?.data?.information?.voicemailPrompt && currentNode?.data?.information?.voicemailPrompt?.path !== '') {
                  if (currentNode?.data?.information?.voicemailPrompt?.path.includes(process.env.FILEGETURL_PATH)) {
                    prompt = currentNode?.data?.information?.voicemailPrompt?.path.replace(process.env.FILEGETURL_PATH, process.env.UPLOAD_DESINATION_PATH);
                  } else if (currentNode?.data?.information?.voicemailPrompt?.path.includes(process.env.UPLOAD_DESINATION_PATH)) {
                    prompt = currentNode?.data?.information?.voicemailPrompt?.path;
                  }
                }
                const agent = {
                  loop,
                  data: {
                    agentDetials: agentDetails?.length > 0 ?
                    {
                      ...agentDetails?.at(0),
                      userId: agentDetails?.at(0).UserID,
                      agent_name: agentDetails?.at(0).UserName
                    } : null,
                    isVoicemailEnabled: currentNode?.data?.information?.isVoicemailEnabled === true ? 1 : 0,
                    voicemailText: currentNode?.data?.information?.voicemailText ?? '',
                    voicemailPrompt: prompt,
                    inputType: currentNode?.data?.information?.inputType
                  },
                  type: 'agentDetails',
                  id: 'agentdetails',
                  session_id: channelId.session_id
                };
                this.redisadaptor?.publishMessage(JSON.stringify(agent));
                const nextNodeData = currentNode?.data?.children?.find((element: any) => element?.type === 'success') ?? { childId: '' };
                return this.handleCallflowType(nextNodeData?.childId, channelId, node, io, 0);
              }
              const nextNodes = currentNode?.data?.children?.find((element: any) => element?.type === 'error') ?? { childId: '' };
              return this.handleCallflowType(nextNodes?.childId, channelId, node, io, 0);
            } else if (currentNode?.data?.information?.type?.toString() === '4') {
              console.log('TRANSFER TO NEW CALL FLOW::::::');
              const newCallFlow: any = await getnodesdataForNewCallFlow(currentNode?.data?.information?.transferTo);
              this.nodes = JSON.parse(newCallFlow);
              console.log('GET CALL FLOW RESPONSE::::::::::::', this.nodes);
              const startingCFNode = await this.getStartNode(this.nodes);
              const currentCFNodeId = startingCFNode?.data?.children?.at(0).childId ?? '';
              return this.handleCallflowType(currentCFNodeId, channelId, this.nodes, io, 0);
            } else {
              console.log('TRANSFER USER INTO QUEUE INITIATED::::::::::');
              await Operation.transferUserInsideQueue(this.redisadaptor, channelId, currentNode);
              console.log('TRANSFER TO QUEUE COMPLETED:::::::::');
            }
            console.log('TRANSFER CHILD NODE::::::::::::', currentNode?.data?.children);
            const nextNode = currentNode?.data?.children?.find((element: any) => element?.type === 'success') ?? { childId: '' };
            console.log('TRANSFER CHILD NODE RESULT::::::', nextNode);
            return this.handleCallflowType(nextNode?.childId, channelId, node, io, 0);
          } catch (error: any) {
            loggerError('transferToQueue', { nodeId, channelId, node, io, loop }, error);
            console.log('TRANSFER CHILD NODE ERROR::::::::', currentNode?.data?.children, error);
            const nextNode = currentNode?.data?.children?.find((element: any) => element?.type === 'error') ?? { childId: '' };
            console.log('TRANSFER CHILD NODE ERROR RESULT::::::::', nextNode);
            return this.handleCallflowType(nextNode?.childId, channelId, node, io, 0);
          }
          break;
        }
        case 'voiceAuthentication': {
          try {
            const targetRestructured = { loop, ...currentNode, data: { ...currentNode?.data?.information }, session_id: channelId?.session_id };
            this.redisadaptor?.publishMessage(JSON.stringify(targetRestructured));
            await this.waitForVoiceAuthenticationResponse();
            const nextNode = currentNode?.data?.children?.find((element: any) => element?.type === 'success') ?? { childId: '' };
            return this.handleCallflowType(nextNode?.childId, channelId, node, io, 0);
          } catch (error: any) {
            loggerError('voiceAuthentication', { nodeId, channelId, node, io, loop }, error);
            const nextNode = currentNode?.data?.children?.find((element: any) => element?.type === 'error') ?? { childId: '' };
            return this.handleCallflowType(nextNode?.childId, channelId, node, io, 0);
          }
          break;
        }
        case 'makeApiCall': {
          try {
            const url = currentNode?.data?.information?.endPointUrl;
            const method = currentNode?.data?.information?.getMethodApi ?? 'get';
            const selectedKeys = new Set(currentNode?.data?.information?.selectedKeyFromResponse ?? []);
            const body = currentNode?.data?.information?.apiBody;
            const headers = currentNode?.data?.information?.apiHeaders;
            const apiResponse = await fetchApi({ urlEndPoint: url, urlMethod: method, urlBody: body, urlHeaders: headers });
            const targetRestructured = { loop, ...currentNode, session_id: channelId?.session_id, data: { ...currentNode?.data?.information } };
            if (channelId?.channelType !== 'Chat') {
              this.redisadaptor?.publishMessage(JSON.stringify(targetRestructured));
            }
            if (apiResponse?.result) {
              let finalResponse: any = await getResponseData(apiResponse?.result);
              if (Array.isArray(finalResponse) && finalResponse.length > 0) {
                finalResponse = finalResponse[0] ?? { childId: '' };
              }
              for (const [key, value] of Object.entries(finalResponse)) {
                if (selectedKeys.has(key)) {
                  this.addContextVariable(key, value);
                }
              }
              const nodeNext = currentNode?.data?.children?.find((nodes: any) => nodes?.type === 'success') ?? { childId: '' };
              return this.handleCallflowType(nodeNext?.childId, channelId, node, io, 0);
            }
            const nextNode = currentNode?.data?.children?.find((nodes: any) => nodes?.type === 'error') ?? { childId: '' };
            return this.handleCallflowType(nextNode?.childId, channelId, node, io, 0);
          } catch (error: any) {
            loggerError('makeApiCall', { nodeId, channelId, node, io, loop }, error);
            const nextNode = currentNode?.data?.children?.find((nodes: any) => nodes?.type === 'error') ?? { childId: '' };
            return this.handleCallflowType(nextNode?.childId, channelId, node, io, 0);
          }
          break;
        }
        case 'checkCondition': {
          try {
            const data = currentNode?.data?.information;
            const leftOperand = Operation.replaceContextWithVariables(data?.leftOperand, this.context);
            const rightOperands = data?.conditions ?? [];
            let resultIndex = -1;
            for (const stock of rightOperands) {
              const result = await conditionsMapper(leftOperand, stock?.operand, Operation.replaceContextWithRightVariables(stock?.rightOperand, this.context));
              if (result) {
                resultIndex = stock?.index;
                break;
              }
            }
            if (resultIndex !== -1) {
              const nodeNext = currentNode?.data?.children?.find((nodes: any) => nodes?.type === `condition${resultIndex + 1}`) ?? { childId: '' };
              return this.handleCallflowType(nodeNext?.childId, channelId, node, io, 0);
            }
            const nextNode = currentNode?.data?.children?.find((nodes: any) => nodes?.type === 'error') ?? { childId: '' };
            return this.handleCallflowType(nextNode?.childId, channelId, node, io, 0);
          } catch (error: any) {
            loggerError('checkCondition', { nodeId, channelId, node, io, loop }, error);
            const nextNode = currentNode?.data?.children?.find((nodes: any) => nodes?.type === 'error') ?? { childId: '' };
            return this.handleCallflowType(nextNode?.childId, channelId, node, io, 0);
          }
          break;
        }
        case 'loop': {
          try {
            if (!this.sessionLoops[channelId?.session_id] && this.sessionLoops[channelId?.session_id] !== 0) {
              this.sessionLoops[channelId?.session_id] = currentNode?.data?.information?.loopValue ?? 0;
            }
            const loopTo = currentNode?.data?.children?.find((nodee: any) => nodee?.type === 'loop') ?? { childId: '' };
            if (loopTo && this.sessionLoops[channelId?.session_id] > 0) {
              this.sessionLoops[channelId?.session_id] -= 1;
              return this.handleCallflowType(loopTo?.childId, channelId, node, io, 1);
            }
            const nextNode = currentNode?.data?.children?.find((nodes: any) => nodes?.type === 'complete') ?? { childId: '' };
            return this.handleCallflowType(nextNode?.childId, channelId, node, io, 1);
          } catch (error: any) {
            loggerError('loop', { nodeId, channelId, node, io, loop }, error);
            const nextNode = currentNode?.data?.children?.find((nodee: any) => nodee?.type === 'error') ?? { childId: '' };
            return this.handleCallflowType(nextNode?.childId, channelId, node, io, 1);
          }
          break;
        }
        case 'disconnect': {
          try {
            await new Promise((resolve: any) => setTimeout(resolve, 500));
            this.redisadaptor.publishMessage(JSON.stringify({ session_id: channelId?.session_id, req_type: 'DISCONNECT' }));
            return;
          } catch (error) {
            loggerError('disconnect', { nodeId, channelId, node, io, loop }, error);
          }
        }
        default: {
          try {
            if (channelId?.disconnectedBy) {
              this.redisadaptor.publishMessage(JSON.stringify({ session_id: channelId?.session_id, req_type: 'DISCONNECT' }));
              return;
            }
            return;
          } catch (error) {
            loggerError('default', { nodeId, channelId, node, io, loop }, error);
          }
        }
      }
    } catch (error) {
      loggerError('handleCallflowType', { nodeId, channelId, node, io, loop }, error);
    }
  }

  public async queueWeightTimeUpdate(channelId: any, ioHandler: any) {
    try {
      if (channelId?.queueUpdate && channelId?.domain_id) {
        ioHandler.to(`${channelId?.domain_id}`).emit('waitingQueueUpdate', channelId?.queueUpdate);
      }
    } catch (error) {
      loggerError('queueWeightTimeUpdate', channelId, error);
    }
  }

  public async handleOtherFunctionalities(channelId: any) {
    try {
      if (channelId?.disconnectedBy && channelId?.status !== 'cancelled') {
        this.clearAllTimeout();
        delete this.sessionLoops[channelId?.session_id];
        this.redisadaptor.publishMessage(JSON.stringify({ session_id: channelId?.session_id, req_type: 'DISCONNECT' }));
      }
    } catch (error) {
      loggerError('handleOtherFunctionalities', channelId, error);
    }
  }

  public async handleInvalidCalls(channel: any) {
    try {
      const disconnectNode = { session_id: channel?.session_id, req_type: 'DISCONNECT' };
      await this.redisadaptor.publishMessage(JSON.stringify(disconnectNode));
    } catch (error) {
      loggerError('handleInvalidCalls', channel, error);
    }
  }

  public getCustomInputResponse = (nexttargetedge, msg) => {
    try {
      return nexttargetedge.find((list: any) => {
        if (list.sourceHandle === msg.value) {
          return list;
        }
      });
    } catch (error) {
      loggerError('getCustomInputResponse', { nexttargetedge, msg }, error);
    }
  }

  public getNodeDataById = (nodeid: any, node: any) => {
    try {
      return node?.find((list: any) => list.id === nodeid);
    } catch (error) {
      loggerError('getNodeDataById', { nodeid, node }, error);
    }
  }

  public getStartNode = (node: any) => {
    try {
      return node?.find((list: any) => list.type === 'entryPoint');
    } catch (error) {
      loggerError('getStartNode', node, error);
    }
  }

  public getCustomhours = (nexttargetedge, msg: any) => {
    try {
      return nexttargetedge.filter((list: any) => list.sourceHandle === msg);
    } catch (error) {
      loggerError('getCustomhours', { nexttargetedge, msg }, error);
    }
  }

  public gettargetedges = (edgeid: any, edge: any) => {
    try {
      return edge?.find((list: any) => list.source === edgeid);
    } catch (error) {
      loggerError('gettargetedges', { edgeid, edge }, error);
    }
  }

  private async waitForPlayPromptSuccessResponse() {
    try {
      return new Promise((resolve: any, reject: any) => {
        let timeout = 500;
        const inteval = setInterval(() => {
          if (timeout === 0 || this.playpromptSuccessTimeout) {
            clearInterval(inteval);
            reject(false);
          }
          if (this.channelInformation?.response === 'success') {
            this.channelInformation = { ...this.channelInformation, response: 'pending' };
            clearInterval(inteval);
            resolve(true);
          }
          timeout -= 1;
        },                          1000);
      });
    } catch (error) {
      loggerError('waitForPlayPromptSuccessResponse', {}, error);
    }
  }

  private async waitForDTMFSuccessResponse(isVariable: any) {
    try {
      return new Promise((resolve: any, _: any) => {
        let timeout = 500;
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
        },                          1000);
      });
    } catch (error) {
      loggerError('waitForDTMFSuccessResponse', isVariable, error);
    }
  }

  private async waitForBusinessHoursResponse() {
    try {
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
        },                          1000);
      });
    } catch (error) {
      loggerError('waitForBusinessHoursResponse', {}, error);
    }
  }

  private async waitForVoiceAuthenticationResponse() {
    try {
      return new Promise((resolve: any, reject: any) => {
        let timeout = 30;
        const inteval = setInterval(() => {
          if (timeout === 0 || this.voiceAuthenticationTimeout) {
            clearInterval(inteval);
            reject(false);
          }
          if (this.channelInformation?.response === 'success') {
            this.channelInformation = { ...this.channelInformation, response: 'pending' };
            clearInterval(inteval);
            resolve(true);
          }
          timeout -= 1;
        },                          1000);
      });
    } catch (error) {
      loggerError('waitForVoiceAuthenticationResponse', {}, error);
    }
  }
}
