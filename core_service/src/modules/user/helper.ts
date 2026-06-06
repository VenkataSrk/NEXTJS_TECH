import moment from 'moment';
import { v4 as uuidv4 } from 'uuid';
import { INPUT } from '../../helpers/constants';

export const getBusinessHours = (req: any) => {
  const startTime = new Date();
  const endTime = new Date();
  startTime.setHours(8, 0, 0, 0);
  endTime.setHours(17, 0, 0, 0);
  const BusinessHours: any = {
    outOfBusiness: {
      name: 'Default Hours',
      description: 'The times during which the center is open and able to take calls or handle customer inquiries',
      hours: [
        { day: 'Monday', isHoliday: false, entry: [{ startTimeStamp: startTime.getTime(), endTimeStamp: endTime.getTime(), differenceTime: '', id: 1 }] },
        { day: 'Tuesday', isHoliday: false, entry: [{ startTimeStamp: startTime.getTime(), endTimeStamp: endTime.getTime(), differenceTime: '', id: 1 }] },
        { day: 'Wednesday', isHoliday: false, entry: [{ startTimeStamp: startTime.getTime(), endTimeStamp: endTime.getTime(), differenceTime: '', id: 1 }] },
        { day: 'Thursday', isHoliday: false, entry: [{ startTimeStamp: startTime.getTime(), endTimeStamp: endTime.getTime(), differenceTime: '', id: 1 }] },
        { day: 'Friday', isHoliday: false, entry: [{ startTimeStamp: startTime.getTime(), endTimeStamp: endTime.getTime(), differenceTime: '', id: 1 }] },
        { day: 'Saturday', isHoliday: true, entry: [{ startTimeStamp: '', endTimeStamp: '', differenceTime: '', id: 1 }] },
        { day: 'Sunday', isHoliday: true, entry: [{ startTimeStamp: '', endTimeStamp: '', differenceTime: '', id: 1 }] },
      ],
      timeZone: 'Europe/London (GMT) GMT +00:00',
      domainId: req?.body?.domainId,
      default_hours: req?.body?.default_hours ?? null,
      timeZoneValue: req?.body?.timeZoneValue ?? 'Europe/London',
      type: 'Default',
      createdAt: new Date(),
    },
    audioNfsPath: '/volume/nfsshare/ccaas/prompt/a43b660b-d3af-43d6-95ad-b02d5307ec9c/1688450462043.wav',
    textToSpeech: 'Thank you for calling. Our office is currently closed. Office hours are Monday through Friday 9 AM to 6 PM.',
    type: 1
  };
  return BusinessHours;
};

export const getPlayPrompt = (req: any) => {
  const basicPrompt: any = [
    {
      fieldname: 'doc',
      encoding: '7bit',
      mimetype: 'audio/wav',
      filename: 'ConnectingAudio.wav',
      path: '/volume/nfsshare/ccaas/prompt/ConnectingAudio.wav',
      size: 288044,
      type: 'upload',
      promptType: '0',
      url: 'https://ccaas-storage.worktual.co.uk/prompt/ConnectingAudio.wav',
      name: 'Queue Connecting  Audio',
      description: 'connecting audio file',
      domainId: req?.body?.domainId ?? 0,
      companyId: req?.body?.companyId ?? 0,
      message: req?.body?.message ?? '',
      promptStatus: 'promptStatus'
    },
    {
      fieldname: 'doc',
      encoding: '7bit',
      mimetype: 'audio/wav',
      filename: 'Greeting tone.wav',
      path: '/volume/nfsshare/ccaas/prompt/GreetingTone.wav',
      size: 163660,
      type: 'upload',
      promptType: '0',
      url: 'https://ccaas-storage.worktual.co.uk/prompt/GreetingTone.wav',
      name: 'Queue Greeting  Audio',
      description: 'Greeting tone file',
      domainId: req?.body?.domainId ?? 0,
      companyId: req?.body?.companyId ?? 0,
      message: req?.body?.message ?? '',
      promptStatus: 'promptStatus'
    },
    {
      fieldname: 'doc',
      encoding: '7bit',
      mimetype: 'Hold Audio',
      filename: 'hold_stay_on_line.wav',
      path: '/volume/nfsshare/ccaas/prompt/hold_stay_on_line.wav',
      size: 311340,
      type: 'upload',
      promptType: '0',
      url: 'https://ccaas-storage.worktual.co.uk/prompt/hold_stay_on_line.wav',
      name: 'Hold Audio',
      description: 'Hold tone file',
      domainId: req?.body?.domainId ?? 0,
      companyId: req?.body?.companyId ?? 0,
      message: req?.body?.message ?? '',
      promptStatus: 'promptStatus'
    },
    {
      fieldname: 'doc',
      encoding: '7bit',
      mimetype: 'Queue Audio',
      filename: 'QueueAudio.wav',
      path: '/volume/nfsshare/ccaas/prompt/QueueAudio.wav',
      size: 911626,
      type: 'upload',
      promptType: '0',
      url: 'https://ccaas-storage.worktual.co.uk/prompt/QueueAudio.wav',
      name: 'Queue Audio',
      description: 'Queue Audio file',
      domainId: req?.body?.domainId ?? 0,
      companyId: req?.body?.companyId ?? 0,
      message: req?.body?.message ?? '',
      promptStatus: 'promptStatus'
    }
  ];
  return basicPrompt;
};

export const getQueueData = (req: any, hoursOfOperation: any, cid: any) => {
  const currentDate: any = moment(new Date()).format('YYYY-MM-DD HH:mm:ss');
  const basicQueueData: any = {
    domainId: req?.body?.domainId,
    name: 'Default Queue',
    description: 'Call or inquiry is placed in a queue to be handled by the next available agent.',
    OutboundCallerId: null,
    outBoundNumber: null,
    maxCallHandlingTimeMM: 5,
    callWrapUpTime: 30,
    hours_of_operation: hoursOfOperation,
    outBoundCallStatus: 1,
    settingAudio: {
      connectingAudio: {
        ConnectingAudioFile: {
          fieldname: 'doc',
          encoding: '7bit',
          mimetype: 'audio/wav',
          filename: 'ConnectingAudio.wav',
          path: '/volume/nfsshare/ccaas/prompt/ConnectingAudio.wav',
          size: 288044,
          uuid: 'abf71855-1e7a-45fd-b950-b11075dad780',
          type: 'upload',
          promptType: '0',
          url: 'https://ccaas-storage.worktual.co.uk/prompt/ConnectingAudio.wav',
          name: 'Queue Connecting audio',
          description: 'connecting audio file',
          domainId: req?.body?.domainId,
          companyId: req?.body?.companyId,
          message: '',
          createdat: currentDate,
          __v: 0,
        },
        ConnectingAudioName: 'connecting audio',
      },
      greetingAudio: {
        greetingAudioFile: {
          fieldname: 'doc',
          encoding: '7bit',
          mimetype: 'audio/wav',
          filename: 'Greeting tone.wav',
          path: '/volume/nfsshare/ccaas/prompt/GreetingTone.wav',
          size: 163660,
          uuid: '974115f2-6130-429a-bf16-d1c897a3b852',
          type: 'upload',
          promptType: '0',
          url: 'https://ccaas-storage.worktual.co.uk/prompt/GreetingTone.wav',
          name: 'Queue Greeting tone',
          description: 'Greeting tone file',
          domainId: req?.body?.domainId,
          companyId: req?.body?.companyId,
          message: '',
          createdat: currentDate,
          __v: 0,
        },
        greetingAudioName: 'Greeting tone',
      },
      queueAudio: {
        queueAudioFile: {
          fieldname: 'doc',
          encoding: '7bit',
          mimetype: 'audio/wav',
          filename: 'QueueAudio.wav',
          path: '/volume/nfsshare/ccaas/prompt/QueueAudio.wav',
          size: 911626,
          uuid: '9cb83c28-9a63-4e46-a03d-089d6d5d2207',
          type: 'upload',
          promptType: '0',
          url: 'https://ccaas-storage.worktual.co.uk/prompt/QueueAudio.wav',
          name: 'Queue Audio',
          description: 'Queue Audio file',
          domainId: req?.body?.domainId,
          companyId: req?.body?.companyId,
          message: '',
          createdat: currentDate,
          __v: 0,
        },
        queueAudioName: 'Queue Audio',
      },
      holdAudio: {
        holdAudioFile: {
          fieldname: 'doc',
          encoding: '7bit',
          mimetype: 'audio/wav',
          filename: 'Classical.wav',
          path: '/volume/nfsshare/ccaas/prompt/hold_stay_on_line.wav',
          size: 311340,
          uuid: '7e424bc7-1b26-480d-b161-77fcd810add3',
          type: 'upload',
          promptType: '0',
          url: 'https://ccaas-storage.worktual.co.uk/prompt/GreetingTone.wav',
          name: 'Queue Hold tone',
          description: 'Hold tone file',
          domainId: req?.body?.domainId,
          companyId: req?.body?.companyId,
          message: '',
          createdat: currentDate,
          __v: 0,
        },
        holdAudioName: 'Hold tone',
      },
    },
    announcement: INPUT.announcement,
    maximumCallers: INPUT.maximumCallers,
    maximumWaitTime: INPUT.maximumWaitTime,
    hoursOfOperatioId: cid,
    outOfBusinessHours: {
      busineHoursType: 0,
      busineHoursValue: 'Announcement',
      fileName: 'Greeting tone',
      file: {
        fieldname: 'doc',
        encoding: '7bit',
        mimetype: 'audio/wav',
        filename: 'Out of Hours Announcement.wav',
        path: '/volume/nfsshare/ccaas/prompt/outOfBusinessHoursTone.wav',
        size: 163660,
        uuid: '974115f2-6130-429a-bf16-d1c897a3b852',
        type: 'upload',
        promptType: '0',
        url: 'https://ccaas-storage.worktual.co.uk/prompt/outOfBusinessHoursTone.wav',
        name: 'Out of Hours Announcement',
        description: 'Out of Hours Announcement tone file',
        domainId: req?.body?.domainId,
        companyId: req?.body?.companyId,
        message: '',
        createdat: currentDate,
        __v: 0,
      },
    },
    status: 1,
    type: 'Default',
  };
  return basicQueueData;
};

export const getRoutingProfileData = (req: any, qid: any) => {
  const basicRoutingProfile: any = {
    domainId: req?.body?.domainId,
    name: 'Default Routing',
    description: 'Routing refers to the process of directing incoming customer inquiries to the appropriate agents or resources',
    No_of_linked_queues: 1,
    default_outbound_queue: 0,
    status: 1,
    routingMethod: 'Long wait routing',
    concurrentChat: 3,
    type: 'Default',
    routingqueue: [
      {
        qid,
        voice: 1,
        chat: 0,
        priority: 1,
        delay_in_seconds: 10,
      },
    ],
  };
  return basicRoutingProfile;
};

export const getBasicSkill = (req: any) => {
  const basicSkill: any = [
    {
      domainId: req?.body?.domainId,
      skill_name: 'Problem-solving',
      skill_description: 'The ability to identify and resolve customer issues or concerns in an efficient manner',
      status: 0,
    },
    {
      domainId: req?.body?.domainId,
      skill_name: 'Conflict resolution',
      skill_description: 'The ability to handle difficult or emotional customer interactions and find mutually-beneficial solutions to problems',
      status: 0,
    },
    {
      domainId: req?.body?.domainId,
      skill_name: 'Product/service knowledge',
      skill_description: 'A thorough understanding of the company products and services',
      status: 0,
    },
    {
      domainId: req?.body?.domainId,
      skill_name: 'Empathy',
      skill_description: 'The ability to understand and respond to customer emotions and needs in a compassionate and understanding way',
      status: 0,
    },
    {
      domainId: req?.body?.domainId,
      skill_name: 'Technical expertise',
      skill_description: 'Depending on the industry, Agents may also be expected to have technical expertise in a particular area',
      status: 0,
    },
  ];
  return basicSkill;
};

export const getDefaultChatflowData = (req: any) => {
  const d = new Date();
  const datestring = `${d.getDate()}/${d.getMonth() + 1}/${d.getFullYear()} ${d.getHours()}:${d.getMinutes()} `;
  const version = req?.body?.status !== 1 ? `Version ${datestring} : Published` : `Version ${datestring} : Save`;
  const data: any = {
    version,
    domainId: req?.body?.domainId,
    companyId: req?.body?.companyId,
    description: 'This is Default Chat Flow',
    status: 2,
    templateName: 'Default Chat Flow',
    createDate: Date.now(),
    lastModifiedDate: Date.now(),
    type: 'Default',
  };
  return data;
};

export const getDefaultCallflowData = (req: any) => {
  const d = new Date();
  const datestring = `${d.getDate()}/${d.getMonth() + 1}/${d.getFullYear()} ${d.getHours()}:${d.getMinutes()} `;
  const version = req?.body?.status !== 1 ? `Version ${datestring} : Published` : `Version ${datestring} : Save`;
  const cfid = req.body.cfid === '' || req.body.cfid === undefined ? process.env.DEFAULT_CALLFLOW_ID : req.body.cfid;
  const data: any = {
    cfid,
    version,
    domainId: req?.body?.domainId,
    companyId: req?.body?.companyId,
    description: req?.body?.description,
    status: 2,
    templateName: 'Default Call Flow',
    callFlowStatus: 2,
    uuid: uuidv4(),
    createDate: Date.now(),
    lastModifiedDate: Date.now(),
    type: 'Default',
  };
  return data;
};
export const getCallflowNodes = (businessHours: any, qid: any) => {
  const allNodes = [
    {
      id: 'entryNode_ngAR44Xcf',
      type: 'entryPoint',
      data: {
        nodeId: '0',
        label: 'input node',
        parentNode: '0',
        children: [{ childId: 'playPrompt_txyyz2qQ5', type: 'entry_source_entryNode_ngAR44Xcf' }],
        isLinkedToDefault: true,
        information: {},
      },
      position: { x: -30, y: -15 },
      width: 260,
      height: 120,
      selected: false,
      positionAbsolute: { x: -30, y: -15 },
      dragging: false,
    },
    {
      id: 'playPrompt_txyyz2qQ5',
      type: 'playPrompt',
      position: { x: 315, y: -45 },
      data: {
        label: 'playPrompt_node',
        nodeId: 'playPrompt_txyyz2qQ5',
        parentNode: 'entryNode_ngAR44Xcf',
        isLinkedToDefault: true,
        children: [{ childId: 'branchHoursofOperation_FXP_1dmFb', type: 'success' }],
        information: {
          type: 'textToSpeech',
          message: 'Thank you for calling. Your call is important to us. Please hold while we connect you to the available agent.',
          playFile: '/volume/nfsshare/ccaas/prompt/bd0d4c8f-c89e-44d5-b602-c90d7f58c5fc/1688032116478.wav',
          audioPrompt: {},
          chatMessage: '',
          isChat: false,
        },
      },
      width: 260,
      height: 192,
      selected: false,
      positionAbsolute: { x: 315, y: -45 },
      dragging: false,
    },
    {
      id: 'branchHoursofOperation_FXP_1dmFb',
      type: 'branchHoursofOperation',
      position: { x: 675, y: -60 },
      data: {
        label: 'branchHoursofOperation_node',
        nodeId: 'branchHoursofOperation_FXP_1dmFb',
        parentNode: 'playPrompt_txyyz2qQ5',
        isLinkedToDefault: true,
        children: [
          { childId: 'transferToQueue_ZtkEpWpt8', type: 'success' },
          { childId: 'disconnect_8bm9pMGfk', type: 'fail' },
        ],
        information: businessHours,
      },
      width: 260,
      height: 215,
      selected: false,
      positionAbsolute: { x: 675, y: -60 },
      dragging: false,
    },
    {
      id: 'transferToQueue_ZtkEpWpt8',
      type: 'transferToQueue',
      position: { x: 1035, y: -90 },
      data: {
        label: 'transferToQueue_node',
        nodeId: 'transferToQueue_ZtkEpWpt8',
        parentNode: 'branchHoursofOperation_FXP_1dmFb',
        isLinkedToDefault: true,
        children: [{ childId: 'disconnect_8bm9pMGfk', type: 'error' }],
        information: {
          type: 1,
          transferTo: qid,
          skill: ['Problem-solving', 'Conflict resolution', 'Product service knowledge f'],
          externalNumber: '',
          isVoicemailEnabled: false,
          inputType: null,
          voicemailText: '',
          voicemailPrompt: null,
          externalCallResume: false,
        },
      },
      width: 260,
      height: 168,
      selected: true,
      positionAbsolute: { x: 1035, y: -90 },
      dragging: false,
    },
    {
      id: 'disconnect_8bm9pMGfk',
      type: 'disconnect',
      position: { x: 1455, y: 0 },
      data: {
        label: 'disconnect_node',
        nodeId: 'disconnect_8bm9pMGfk',
        parentNode: 'branchHoursofOperation_FXP_1dmFb',
        isLinkedToDefault: true,
        children: [],
        information: {},
      },
      width: 260,
      height: 84,
      selected: false,
      positionAbsolute: { x: 1455, y: 0 },
      dragging: false,
    },
  ];
  return allNodes;
};

export const getCallflowEdges = () => {
  const allEdges = [
    {
      source: 'entryNode_ngAR44Xcf',
      sourceHandle: 'entry_source_entryNode_ngAR44Xcf',
      target: 'playPrompt_txyyz2qQ5',
      targetHandle: 'playPrompt_txyyz2qQ5',
      type: 'buttonedge',
      id: 'reactflow__edge-entryNode_ngAR44Xcfentry_source_entryNode_ngAR44Xcf-playPrompt_txyyz2qQ5playPrompt_txyyz2qQ5',
    },
    {
      source: 'playPrompt_txyyz2qQ5',
      sourceHandle: 'success',
      target: 'branchHoursofOperation_FXP_1dmFb',
      targetHandle: 'branchHoursofOperation_FXP_1dmFb',
      type: 'buttonedge',
      id: 'reactflow__edge-playPrompt_txyyz2qQ5success-branchHoursofOperation_FXP_1dmFbbranchHoursofOperation_FXP_1dmFb',
    },
    {
      source: 'branchHoursofOperation_FXP_1dmFb',
      sourceHandle: 'success}',
      target: 'transferToQueue_ZtkEpWpt8',
      targetHandle: 'transferToQueue_ZtkEpWpt8',
      type: 'buttonedge',
      id: 'reactflow__edge-branchHoursofOperation_FXP_1dmFbsuccess}-transferToQueue_ZtkEpWpt8transferToQueue_ZtkEpWpt8',
    },
    {
      source: 'transferToQueue_ZtkEpWpt8',
      sourceHandle: 'error',
      target: 'disconnect_8bm9pMGfk',
      targetHandle: 'b',
      type: 'buttonedge',
      id: 'reactflow__edge-transferToQueue_ZtkEpWpt8error-disconnect_8bm9pMGfkb',
    },
    {
      source: 'branchHoursofOperation_FXP_1dmFb',
      sourceHandle: 'fail',
      target: 'disconnect_8bm9pMGfk',
      targetHandle: 'b',
      type: 'buttonedge',
      id: 'reactflow__edge-branchHoursofOperation_FXP_1dmFbfail-disconnect_8bm9pMGfkb',
    },
  ];
  return allEdges;
};

export const getAgentStatuses = (req: any) => {
  const agentsData: any = [
    {
      statusName: 'Ready',
      type: 'Default',
      description: 'The agent is available and able to take work',
      domainId: req?.body?.domainId,
      channels: 'All Channels Ready',
      status: 0,
      isVoice: req?.body?.isVoice ?? 1,
      isVoiceCallTransfer: req?.body?.isVoiceCallTransfer ?? 1,
      isChat: req?.body?.isChat ?? 1,
      isChatTransfer: req?.body?.isChatTransfer ?? 0,
      isEmail: req?.body?.isEmail ?? 1,
      isEmailTransfer: req?.body?.isEmailTransfer ?? 1,
      colorCode: '#007152',
    },
    {
      statusName: 'Not Ready',
      type: 'Default',
      description: 'The agent is unavailable to take work',
      domainId: req?.body?.domainId,
      channels: 'All Channels Not ready',
      status: 0,
      isVoice: req?.body?.isVoice ?? 0,
      isVoiceCallTransfer: req?.body?.isVoiceCallTransfer ?? 0,
      isChat: req?.body?.isChat ?? 0,
      isChatTransfer: req?.body?.isChatTransfer ?? 0,
      isEmail: req?.body?.isEmail ?? 0,
      isEmailTransfer: req?.body?.isEmailTransfer ?? 0,
      colorCode: '#f22e57',
    },
    {
      statusName: 'Transfer Only',
      type: 'Default',
      description: 'Agent Can receive work from other agents',
      domainId: req?.body?.domainId,
      channels: 'All Channels Transfer Only',
      status: 0,
      isVoice: req?.body?.isVoice ?? 0,
      isVoiceCallTransfer: req?.body?.isVoiceCallTransfer ?? 0,
      isChat: req?.body?.isChat ?? 0,
      isChatTransfer: req?.body?.isChatTransfer ?? 0,
      isEmail: req?.body?.isEmail ?? 0,
      isEmailTransfer: req?.body?.isEmailTransfer ?? 0,
      colorCode: '#803D1F',
    },
    {
      statusName: 'Away',
      type: 'Default',
      description: 'The agent is unavailable to take work',
      domainId: req?.body?.domainId,
      channels: 'All Channels Away',
      status: 0,
      isVoice: req?.body?.isVoice ?? 0,
      isVoiceCallTransfer: req?.body?.isVoiceCallTransfer ?? 0,
      isChat: req?.body?.isChat ?? 0,
      isChatTransfer: req?.body?.isChatTransfer ?? 0,
      isEmail: req?.body?.isEmail ?? 0,
      isEmailTransfer: req?.body?.isEmailTransfer ?? 0,
      colorCode: '#FF793D',
    },
    {
      statusName: 'Offline',
      type: 'Default',
      description: 'Agent is not available to handle customer inquiries or requests',
      domainId: req?.body?.domainId,
      channels: 'All Channels Offline',
      status: 0,
      isVoice: req?.body?.isVoice ?? 0,
      isVoiceCallTransfer: req?.body?.isVoiceCallTransfer ?? 0,
      isChat: req?.body?.isChat ?? 0,
      isChatTransfer: req?.body?.isChatTransfer ?? 0,
      isEmail: req?.body?.isEmail ?? 0,
      isEmailTransfer: req?.body?.isEmailTransfer ?? 0,
      colorCode: '#808080',
    },
    {
      statusName: 'Busy',
      type: 'Default',
      description: 'Agent is busy in handling customer inquiries or requests',
      domainId: req?.body?.domainId,
      channels: 'All Channels Offline',
      status: 0,
      isVoice: req?.body?.isVoice ?? 0,
      isVoiceCallTransfer: req?.body?.isVoiceCallTransfer ?? 0,
      isChat: req?.body?.isChat ?? 0,
      isChatTransfer: req?.body?.isChatTransfer ?? 0,
      isEmail: req?.body?.isEmail ?? 0,
      isEmailTransfer: req?.body?.isEmailTransfer ?? 0,
      colorCode: '#f22e57',
    },
  ];
  return agentsData;
};

export const getDispositionData = (req: any) => {
  const dispositionData: any = [
    {
      dispositionName: 'Resolved',
      type: 'Default',
      description: 'The customer issue was resolved to their satisfaction during the interaction',
      domainId: req?.body?.domainId,
      status: req?.body?.status ?? 1,
      colorCode: req.body?.colorCode ?? null,
    },
    {
      dispositionName: 'Unresolved',
      type: 'Default',
      description: 'The customer issue was not resolved to their satisfaction during  the interaction',
      domainId: req?.body?.domainId,
      status: req?.body?.status ?? 1,
      colorCode: req.body?.colorCode ?? null,
    },
    {
      dispositionName: 'Follow-up',
      type: 'Default',
      description: 'The customer call or query need to follow up to ensure that customer issues are fully addressed',
      domainId: req?.body?.domainId,
      status: req?.body?.status ?? 1,
      colorCode: req.body?.colorCode ?? null,
    },
    {
      dispositionName: 'Transferred',
      type: 'Default',
      description: 'The customer call or query was transferred to another agent or Queue for further assistance',
      domainId: req?.body?.domainId,
      status: req?.body?.status ?? 1,
      colorCode: req.body?.colorCode ?? null,
    },
    {
      dispositionName: 'Closed',
      type: 'Default',
      description: 'The customer cancelled their call or query before it was completed',
      domainId: req?.body?.domainId,
      status: req?.body?.status ?? 1,
      colorCode: req.body?.colorCode ?? null,
    },
    {
      dispositionName: 'Do not contact',
      type: 'Default',
      description: 'Customer to be removed from a company contact list or to opt out of receiving marketing or other communications from the company',
      domainId: req?.body?.domainId,
      status: req?.body?.status ?? 1,
      colorCode: req.body?.colorCode ?? null,
    },
  ];
  return dispositionData;
};

export const getCustomerTag = (req: any) => {
  const customerData: any = [
    {
      customerTagName: 'Repeat Caller',
      type: 'Default',
      description:
        'Repeat callers may have a variety of reasons for contacting the business, such as seeking customer support, placing orders, making appointments, or asking questions',
      status: req?.body?.status ?? 1,
      domainId: req?.body?.domainId,
    },
    {
      customerTagName: 'Complaints',
      type: 'Default',
      description: 'Customer complaints is an important part of managing customer relationships and maintaining customer satisfaction',
      status: req?.body?.status ?? 1,
      domainId: req?.body?.domainId,
    },
    {
      customerTagName: 'First Time Caller',
      type: 'Default',
      description: 'A first-time caller is a customer who is contacting a business or organization for the first time',
      status: req?.body?.status ?? 1,
      domainId: req?.body?.domainId,
    },
    {
      customerTagName: 'Longtime Customer',
      type: 'Default',
      description: 'A long-time customer is a customer who has been doing business with a company  or organization for an extended period of time',
      status: req?.body?.status ?? 1,
      domainId: req?.body?.domainId,
    },
  ];
  return customerData;
};

export const getKevinChatflowNodes = (businessHours: any, qid: any) => {
  const allNodes = [
    {
      id: 'entryNode_ngAR44Xcf',
      type: 'entryPoint',
      data: {
        nodeId: '0',
        label: 'input node',
        parentNode: '0',
        children: [{ childId: 'transferToQueue_ZtkEpWpt8', type: 'entry_source_entryNode_ngAR44Xcf' }],
        isLinkedToDefault: true,
        information: {},
      },
      position: { x: -30, y: -15 },
      width: 260,
      height: 120,
      selected: false,
      positionAbsolute: { x: -30, y: -15 },
      dragging: false,
    }
  ];
  return allNodes;
};

export const getKevinChatflowEdges = () => {
  const allEdges = [
    {
      source: 'entryNode_ngAR44Xcf',
      sourceHandle: 'entry_source_entryNode_ngAR44Xcf',
      target: 'transferToQueue_ZtkEpWpt8',
      targetHandle: 'transferToQueue_ZtkEpWpt8',
      type: 'buttonedge',
      id: 'reactflow__edge-entryNode_ngAR44Xcfentry_source_entryNode_ngAR44Xcf-playPrompt_txyyz2qQ5playPrompt_txyyz2qQ5',
    }
  ];
  return allEdges;
};
