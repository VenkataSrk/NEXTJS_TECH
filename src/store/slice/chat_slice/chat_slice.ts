import _ from "lodash";
import { createSlice } from "@reduxjs/toolkit";
import { isJSON } from "../../../base/utils";
import { v4 as uuidv4 } from "uuid";
import { CHAT_REDUCER } from "../../constants/reducer_names";
import { GetItemFromStorage } from "../../../base/custom_hooks/useStorage";

const initialState = {
  allIncomingSessionDetails: [] as any,
  currentSessionMsgHistory: [] as any,
  currentSessionCallHistory: [] as any,
  isSocketJoin: false,
  messageList: [] as any,
  sessionList: [] as any,
  currentSession: {} as any,
  isIncomingChat: false,
  internalRoom: false,
  internalAccepted: false,
  internalRoomInfo: "",
  internalSessionId: "",
  internalSessionDetailswhilePopup: {} as any,
  updateProfileContact: false,
  contactInfo: [] as any,
  contactTagList: [] as any,
  currentContact: {},
  loading: false,
  single_contact: {} as any,
  agent_work_space_history: [] as any,
  internalChatDetails: { chatMessage: [], chatDetails: {}, reload: false, isEdit: false, editChat: {}, addMsg: false, notification: [], internalChatGroupId: 0 } as any,
  isMessageLoading: false,
  searchAgentName: "" as string,
  agentnameList: [] as any,
  agentList: [] as any,
  isCustomerTyping: false,
  internalchatmessage: "",
  message: "",
  agentworkSpaceDetails: {} as any,
  typeOfMessage: "",
  customerDetails: {} as any,
  isTransfer: false,
  isChat: false,
  isDialPadOpen: true,
  storeIntentForClearTimer: "",
  isContactEditOpen: false,
  sessionUpdateProfile: "",
  coidcontact: "",
  servicePlan: {} as any,
  activityAutoRefresh: false,
  allViewRefresh: false,
  transferDetails: [] as any,
  agentAssitence: [] as any,
  agentTimeResponse: {} as any,
  isEditState: false,
  storeAgentTransfer: {} as any,
  transferAcceptDetails: {} as any,
  agentAssist: [] as any,
  agentAssistRelaod: false as Boolean,
  interactionData: [] as any,
  webscrapProcess: [] as any,
  supervisorList: [] as any,
  supervisorWhisperStatus: "",
  speedometerScoreChat: 0,
  currentEndSession: '',
  currentEndChannelType: '',
  incomingcallDetails: {} as any,
  customerPreviousHistory: [] as any,
  answeredDuration: 0,
  endMeetingDispostion: "",
  additionalChatRequest: [] as any,
  typingBotText: {} as any,
  individualChatId: 0,
  viewProfile: true,
  coChatMobileActiveStatus: false,
  internalAssist: true,
  internalChatInfo: true,
  dropDown: false,
  smsOpen: false,
  callSessionId: "",
  autoAssignTicketing: {} as any,
  aiSummaryStatus: false,
  autoAnswer: 0,
  assistPayload: {},
  broadcasgrouplist: [] as any,
  broadcast: "",
  broadcastAgent: {} as any,
  openbroadcast: "",
  broadcastmessage: [] as any,
  dashCallBackRefresh: false,
  dashPerformanceRefresh: false,
  agentGroupEmitter: false,
  rowData: {} as any,
  broadcastMSGRefresh: false,
  groupMessageRefresh: false,
  unReadMessgCount: [] as any,
  Likemessage: {} as any,
  showClosedInteractions: false,
  showAllViewInteraction: false,
  liveInteractionCount: "",
  assignActivityList: [] as any,
  customerMsg: [] as any,
  sentimentalMessages: [] as any,
  warningMessages: [] as any,
  recommendations: [] as any,
  buyNowModal: false,
  interactionDetails : {} as any,
  showSummary: false,
};

const chatSlice = createSlice({
  name: CHAT_REDUCER,
  initialState,
  reducers: {
    storeAllSessions: (state, { payload }) => {
      state.allIncomingSessionDetails.unshift(payload);
    },
    setCallSessionId: (state, { payload }) => {
      state.callSessionId = payload
    },
    supervisorList: (state, { payload }) => {
      state.supervisorList = payload;
    },
    webscrapProcess: (state, { payload }) => {
      state.webscrapProcess = payload
    },
    insertTransferDetails: (state, { payload }) => {
      state.transferDetails.push(payload)
    },
    transferAcceptDetails: (state, { payload }) => {
      state.transferAcceptDetails = payload
    },
    servicePlan: (state, { payload }) => {
      state.servicePlan = payload
    },
    setEditState: (state, { payload }) => {
      state.isEditState = payload
    },
    storeAgentTransfer: (state, { payload }) => {
      state.storeAgentTransfer = payload
    },
    agentTimeResponse: (state, { payload }) => {
      state.agentTimeResponse = payload
    },
    storeCurrentSessionMsgHistory: (state, { payload }) => {
      let msgData = state.agent_work_space_history.filter((item: any) => item?.sessionId === payload?.sessionId);
      state.currentSessionMsgHistory = msgData;
    },
    storeCurrentSessionCallHistory: (state, { payload }) => {
      let msgData = state.agent_work_space_history.filter((item: any) => item?.sessionId === payload?.sessionId);
      state.currentSessionCallHistory = msgData;
    },
    setCustomerPreviousHistory: (state, { payload }) => {
      if(!Array.isArray(state.customerPreviousHistory)) state.customerPreviousHistory = []
      if(payload?.currentSessionId){
        state.customerPreviousHistory?.push(payload)
      }
    },
    removeCustomerPreviousHistory: (state, { payload }) => {
      let index = state.customerPreviousHistory.findIndex(
        (session: any) => session?.currentSessionId === payload
      );
      if (index > -1) {
        state.customerPreviousHistory.splice(index, 1);
      }      
    },
    updateInternalMessage: (state, { payload }) => {
      state.internalchatmessage = payload;
    },
    setSpeedometerScoreChat: (state, { payload }) => {
      state.speedometerScoreChat = payload;
    },
    updateMessage: (state, { payload }) => {
      state.messageList.push(payload.messageList);
    },
    // agaentAssitence: (state, { payload }) => {
    //   console.log('payload>>>>>>',payload)
    //   state.agentAssitence.push(payload);
    // },
    agaentAssitence: (state, { payload }) => {
      const index = state?.agentAssitence?.findIndex(
        (item: any) =>
          (item?.messageId ?? item?.messageID) ==
          (payload?.messageId ?? payload?.messageID)
      );

      if (index !== -1) {
        state.agentAssitence[index] = {
          ...state?.agentAssitence?.[index],
          ...payload,
          message: payload?.message ?? payload?.response,
          response: payload?.message ?? payload?.response,
        };
      } else {
        state?.agentAssitence?.push({
          ...payload,
          message: payload?.message ?? payload?.response,
          response: payload?.message ?? payload?.response,
        });
      }
    },     
    setUnreadMessageCount: (state, { payload }) => {
      const { sessionId } = payload;
      if (state.currentSession?.sessionId === sessionId) {
        return;
      }
      const sessionExists = state.sessionList.some(
        (session: any) => session?.sessionId === sessionId
      );
      if (!sessionExists) return;
      const unreadIndex: any = state.unReadMessgCount.findIndex(
        (item: any) => item.sessionId === sessionId
      );
      if (unreadIndex > -1) {
        state.unReadMessgCount[unreadIndex].unReadCount += 1;
      } else {
        state.unReadMessgCount.push({
          sessionId: sessionId,
          unReadCount: 1,
        });
      }
    },

    clearUnreadMessageCount: (state, { payload }) => {
      const { sessionId } = payload;
      const unreadIndex = state.unReadMessgCount.findIndex(
        (item: any) => item.sessionId === sessionId
      );
      if (unreadIndex > -1) {
        state.unReadMessgCount[unreadIndex].unReadCount = 0;
      }
    },

    agaentLikeAssitence: (state, { payload }) => {
      const messageId = payload?.AssistMsg?.messageId || payload?.messageId;
      const index = state.agentAssitence.findIndex(
        (item: { messageId: string }) => item?.messageId === messageId
      );
      if (index !== -1) {
        state.agentAssitence[index] = {
          ...state.agentAssitence[index],
          AssistMsg: {
            ...state.agentAssitence[index].AssistMsg,
            isLike: payload?.AssistMsg?.isLike ?? state.agentAssitence[index]?.AssistMsg?.isLike,
            isDislike: payload?.AssistMsg?.isDislike ?? state.agentAssitence[index]?.AssistMsg?.isDislike,
            isUsed: payload?.AssistMsg?.isUsed ?? state.agentAssitence[index]?.AssistMsg?.isUsed,
          },
          response: payload.assistMsg ?? state.agentAssitence[index].response,
        };

      }

    },

    setAgentAssistance: (state, { payload }) => {
      state.agentAssitence = payload;
    },
    incomingChat: (state, { payload }) => {
      state.isIncomingChat = true;
      state.internalSessionDetailswhilePopup = {
        fromCallername: payload.sender["Phone Number"],
        sessionId: payload.userId,
        contactDetails: payload.contactDetails,
        queueName: payload.queueName,
        chatWidgetName: payload?.widgetName,
        fromCallerNumber: payload.sender["Phone Number"],
        Number: payload.sender["Phone Number"],
        ChatHistoryId: payload.ChatHistoryId,
        transferType: payload?.transferType ?? 0,
        primaryAgent: payload?.primaryAgent ?? 0,
        cookiesId: payload?.cookiesId ?? '',
        ChatDurationHistoryId: payload?.ChatDurationHistoryId,
        chatPlanId: payload?.chatPlanId,
        queueId: payload?.queueInformation,
        queueSettingsId: payload?.queueSettings
      };
    },
    updateTimerForCall: (state, { payload }) => {
      let index = _.findIndex(state.sessionList, (item: any) => item.sessionId === payload.callPopUpDetails.incomingCallDetail.sessionId);

      // if (index > -1) {
      //   state.sessionList[index].callDuration = payload.callPopUpDetails.callDetails?.[0]?.callduration ?? state.currentSession.callDuration;
      //   if (state.sessionList[index].sessionId === state.currentSession.sessionId) {
      //     state.currentSession.callDuration = payload.callPopUpDetails.incomingCallDetail.callduration;
      //   }
      // }
      if (index > -1) {
        state.sessionList[index].callDuration = payload.callPopUpDetails.callDetails?.[0]?.callduration ?? state.currentSession.callDuration;
        if (state.sessionList[index].sessionId === state.currentSession.sessionId) {
          state.currentSession.callDuration = payload.callPopUpDetails.callDetails?.[0]?.callduration ?? state.currentSession.callDuration;
        }
      }

    },

    updateTransferModalStatus: (state, { payload }) => {
      let index = _.findIndex(state.sessionList, (session: any) => session?.ChatHistoryId == payload?.ChatHistoryId);
      if (index > -1) {
        if (state?.currentSession?.ChatHistoryId == payload?.ChatHistoryId) {
          state.currentSession = {
            ...state.currentSession,
            transferName: payload?.transferName ?? state.currentSession?.transferName,
            istransferringmodal: payload?.istransferringmodal ?? false,
            istransferSuccess: payload?.istransferSuccess ?? false,
            istransferDeclined: payload?.istransferDeclined ?? false,
            istransferring: payload?.istransferring ?? false,
          };
        }
        state.sessionList[index] = {
          ...state.sessionList[index],
          transferName: payload?.transferName ?? state.currentSession?.transferName,
          istransferringmodal: payload?.istransferringmodal ?? false,
          istransferSuccess: payload?.istransferSuccess ?? false,
          istransferDeclined: payload?.istransferDeclined ?? false,
          istransferring: payload?.istransferring ?? false,
        };
      }
    },

    closeTransferModalStatus: (state, { payload }) => {

      let index = _.findIndex(state.sessionList, (session: any) => session?.ChatHistoryId == payload?.ChatHistoryId);
      if (index > -1) {
        if (state?.currentSession?.ChatHistoryId == payload?.ChatHistoryId) {
          state.currentSession = {
            ...state.currentSession,
            istransferringmodal: false,
            istransferSuccess: false,
            istransferDeclined: false,
            istransferring: false,
          };
        }
        state.sessionList[index] = {
          ...state.sessionList[index],
          istransferringmodal: false,
          istransferSuccess: false,
          istransferDeclined: false,
          istransferring: false,
        };
      }
    },

    clearsessionTimer: (state, { payload }) => {
      state.storeIntentForClearTimer = payload;
    },

    acceptChat: (state, { payload }) => {
      state.internalAccepted = true;
      state.isIncomingChat = false;
      state.internalSessionId = payload.sessionId;
      state.internalSessionDetailswhilePopup = {};
    },
    declineChat: (state) => {
      state.internalAccepted = false;
      state.isIncomingChat = false;
      state.internalSessionId = "";
      state.internalSessionDetailswhilePopup = {};
    },

    internalChat: (state, { payload }) => {
      state.internalRoom = payload.internalRoom;
      state.internalAccepted = payload.internalAccepted;
      state.internalRoomInfo = payload.internalRoomInfo;
      state.internalSessionId = payload.internalSessionId;
      state.internalSessionDetailswhilePopup = payload.internalSessionDetailswhilePopup;
    },
    chatSessionList: (state, { payload }) => {
      //  state.sessionList = payload;
      if(payload?.sessionId){
        let index = _.findIndex(state.sessionList, (session: any) => session?.sessionId == payload?.sessionId);
        if (index > -1) {
          state.sessionList[index] = payload;
        } else {
          state.sessionList.unshift(payload);
        }
      }
    },
    meetSessionList: (state, { payload }) => {
      state.sessionList = payload;
    },
    endMeetingDispostion: (state, { payload }) => {
      state.endMeetingDispostion = payload;
    },
    setliveInteractionCount:(state, { payload }) => {
      state.liveInteractionCount = payload;
    },
    contactSessionList: (state, { payload }) => {
      let index = _.findIndex(state.sessionList, (session: any) => session?.sessionId == payload?.sessionId);
      if (index > -1) {
        const csdata = state.sessionList?.filter((data: any) => data?.sessionId == payload.sessionId)
        state.sessionList[index] = { ...csdata[0], calledPersonDetails: payload.data };
        state.currentSession = { ...csdata[0], calledPersonDetails: payload.data };
      }
    },
    setChatSessionList: (state, { payload }) => {
      let index = _.findIndex(state.sessionList, (session: any) => session?.sessionId === payload?.sessionId);
      if (index > -1) {
        if (state?.currentSession?.sessionId === payload?.sessionId){
          state.currentSession = { 
            ...payload, 
            dialDuration: state.currentSession?.dialDuration , 
            sessionOnGoingTime: state.currentSession?.sessionOnGoingTime , 
            calledPersonDetails: state.currentSession?.calledPersonDetails
          }
        }
        state.sessionList[index] = { 
          ...payload,
          dialDuration: state.sessionList[index]?.dialDuration, 
          sessionOnGoingTime: state.sessionList[index]?.sessionOnGoingTime, 
          calledPersonDetails: state.sessionList[index]?.calledPersonDetails};
      }
    },
    setCallSessionList: (state, { payload }) => {
      // console.log('setCallSessionList====>', payload)
      let index = _.findIndex(state.sessionList, (session: any) => session?.sessionId === payload?.sessionId);
      // console.log('setCallSessionList====>', index)
      if (index > -1) {
        if (state?.currentSession?.sessionId === payload?.sessionId) {
          state.currentSession = { ...state.currentSession, callDispositionOpen: payload.state }
        }
        state.sessionList[index] = { ...state.sessionList[index], callDispositionOpen: payload.state };
      }
    },
    setTriggerKey: (state, { payload }) => {
      let index = _.findIndex(state.sessionList, (session: any) => session?.sessionId === payload?.sessionId);
      if (state?.currentSession?.sessionId === payload?.sessionId && index > -1) {
        state.currentSession.foundKeyword = true
        state.sessionList[index].foundKeyword = true;
      }
    },
    chatAgentAssistList: (state, { payload }) => {
      state.agentAssist.push(payload)
    },
    chatAgentAssistReload: (state, { payload }) => {
      state.agentAssistRelaod = payload
    },
    setAssistPayload: (state, { payload }) => {
      state.assistPayload = payload
    },
    chatAgentAssistListReset: (state) => {
      state.agentAssist = []
    },
    chatAgentAssistListFilter: (state, { payload }) => {
      state.agentAssist = state.agentAssist?.filter((data: any) => data.sessionId !== payload)
    },
    setBlockDataforContact: (state) => {
      state.currentSession.calledPersonDetails.blockCustomer = state.currentSession.calledPersonDetails.blockCustomer === 0 ? 1 : 0;
    },
    setBlockDataReqforContact: (state) => {
      state.currentSession.calledPersonDetails.blockCustomerReq = state.currentSession.calledPersonDetails.blockCustomerReq === 0 ? 1 : 0;
    },
    setDNDDataforContact: (state) => {
      state.currentSession.calledPersonDetails.doNotDisturb = state.currentSession.calledPersonDetails.doNotDisturb === 0 ? 1 : 0;
    },
    setDNDDataReqforContact: (state) => {
      state.currentSession.calledPersonDetails.doNotDisturbReq = state.currentSession.calledPersonDetails.doNotDisturbReq === 0 ? 1 : 0;
    },

    currentChatSession: (state, { payload }) => {
      state.currentSession = payload;
    },
    currentTransferState: (state, { payload }) => {
      state.currentSession.Istransferred = payload;
    },
    updateCurrentTransferState: (state, { payload }) => {
      state.sessionList?.map((list: any) => {
        if (list.ChatHistoryId === state.currentSession?.ChatHistoryId) {
          list.Istransferred = 1
        }
      })
    },
    removeCurrentSession: (state) => {
      let index = _.findIndex(state.sessionList, (session: any) => session?.sessionId === state.currentSession?.sessionId);
      if (index > -1) {
        state.sessionList.splice(index, 1);
        state.currentSession = {};
      }
    },

    updateContactTemPStore: (state, { payload }) => {
      state.currentContact = payload;
      state.single_contact = payload.single_contact;
      state.currentSession["calledPersonDetails"] = payload;
      state.currentSession["Number"] = payload.phoneNumber;
    },
    updateContactProfile: (state, { payload }) => {
      state.updateProfileContact = payload.updateProfileContact;
      state.contactInfo = payload.contactInfo;
      state.contactTagList = payload.contactTagList;
    },
    updateSocketConnect: (state) => {
      state.isSocketJoin = true;
    },
    agentWorkSpaceHistory: (state, { payload }) => {
      // console.log('list:',payload.agent_work_space_history)
      state.agent_work_space_history?.push(payload.agent_work_space_history);
    },
    agentWorkSpaceTansferHistory: (state, { payload }) => {
      state.agent_work_space_history?.push(payload);
    },
    // agentTansferHistory: (state, { payload }) => {
    //   state.agent_work_space_history = [...state.agent_work_space_history, ...payload]
    // },
    updateCurrentEndSession: (state, { payload }) => {
      state.currentSession = { ...state.currentSession, isChatEnded: payload }
    },
    agentViewWorkSpaceHistory: (state, { payload }) => {
      let msgData = state.agent_work_space_history.filter((item: any) => item?.sessionId !== payload);
      state.agent_work_space_history = msgData;
    },
    agentViewWorkSpaceHistory_WB: (state, { payload }) => {
      state.agent_work_space_history = [...state.agent_work_space_history, ...payload];
    },
    update_called_person_details: (state, { payload }) => {
      let payloadData = { ...payload };
      const primaryPhoneNumber = payloadData.primaryPhoneNumber ? payloadData.primaryPhoneNumber : '';
      const phoneNumber = payloadData.phoneNumber ? payloadData.phoneNumber : [];
      payloadData.phoneNumber = JSON.stringify([primaryPhoneNumber, ...phoneNumber]);
      const primaryEmail = payloadData.primaryEmail ? payloadData.primaryEmail : '';
      const email = payloadData.email ? payloadData.email : [];
      payloadData.email = JSON.stringify([primaryEmail, ...email]);
      if (state?.currentSession?.calledPersonDetails?.coid === payload?.coid) {
        state.currentSession.calledPersonDetails = payloadData;
      }
      let index = state.sessionList.findIndex((list: any) => list?.calledPersonDetails?.coid === payload?.coid);
      if (index > -1) {
        state.sessionList[index].calledPersonDetails = payloadData;
      }
    },
    agentworkSpaceDetails: (state, { payload }) => {
      state.agentworkSpaceDetails = payload.agentworkSpaceDetails;
    },
    agentAddMessage: (state, { payload }) => {
      const message = state?.agentworkSpaceDetails?.message ?? [];
      state.agent_work_space_history?.push(payload);
      state.agentworkSpaceDetails = { ...state.agentworkSpaceDetails, messages: message };
    },
    customerDetails: (state, { payload }) => {
      state.customerDetails = payload;
    },
    updateAddAgentMessage: (state, { payload }) => {
      state.message = payload;
    },
    updateTypeOfMessage: (state, { payload }) => {
      state.typeOfMessage = payload.typeOfMessage;
    },

    textTyping: (state, { payload }) => {
      state.isCustomerTyping = payload;
    },

    typingBotText: (state, { payload }) => {
      state.typingBotText = payload;
    },

    seenHandler: (state, { payload }) => {
      let index = _.findIndex(state.agent_work_space_history, (data: any) => data.messageID === payload.id);
      if (index > -1) {
        state.agent_work_space_history[index].msgSeenStatus = payload.status;
      }
    },
    transfer: (state) => {
      state.isTransfer = !state.isTransfer;
      state.isChat = true;
      state.isDialPadOpen = false;
    },

    isContactEditOpenset: (state, { payload }) => {
      state.isContactEditOpen = payload.isUpdate;
      state.sessionUpdateProfile = payload.sessionId;
      state.coidcontact = payload.coidcontact;
    },
    setInteraction: (state, { payload }) => {
      state.currentSession.interactions = payload;

      let index = state.sessionList.findIndex((list: any) => list.sessionId === state.currentSession.sessionId);
      if (index > -1) {
        state.sessionList[index].interactions = payload;
      }
    },
    updateContactDetails: (state, { payload }) => {
      //contactSession
      let contactSession: any = GetItemFromStorage("contactSession");
      let index: any = state.sessionList.findIndex((item: any) => item.sessionId === contactSession);
      if (index > -1) {
        state.sessionList[index].calledPersonDetails = payload;
        // if (state.currentSession.sessionId === contactSession) {
        state.currentSession.calledPersonDetails = payload;
        //}
        state.isContactEditOpen = false;
        state.sessionUpdateProfile = "";
        state.coidcontact = "";
      }
    },
    sessionRemoveAfterEnd: (state, { payload }) => {
      if (state.sessionList.length > 1) {
        let index = _.findIndex(state.sessionList, (session: any) => session?.sessionId === payload);
        if (index > -1) {
          state.sessionList.splice(index, 1);
          state.currentSession = state.sessionList[state.sessionList.length - 1];

          if(state.sessionList[state.sessionList.length - 1]?.isInteractionView){
            state.showClosedInteractions = true
          };
          
        }
      } else {
        state.sessionList = [];
        state.currentSession = {};
      }
    },

    removeTags: (state, { payload }) => {
      try {
        let tagsList = isJSON(state.currentSession.calledPersonDetails.tags)
          ? JSON.parse(state.currentSession.calledPersonDetails.tags)
          : Array.isArray(state.currentSession.calledPersonDetails.tags)
            ? state.currentSession.calledPersonDetails.tags
            : []
        let index: any = tagsList.findIndex((item: any) => item === payload);
        if (index > -1) {
          tagsList.splice(index, 1);
        }
        let findCurrent = _.findIndex(state.sessionList, (session: any) => session?.sessionId === state.currentSession.sessionId);
        if (findCurrent > -1) {
          state.sessionList[findCurrent].calledPersonDetails.tags = JSON.stringify(tagsList);
        }
        state.currentSession.calledPersonDetails.tags = JSON.stringify(tagsList);
      } catch (error) {
        console.log(error)
      }
    },

    updateTags: (state, { payload }) => {
      try {
        let tags: any = state.currentSession?.calledPersonDetails?.tags && typeof state.currentSession?.calledPersonDetails?.tags === 'string'
          ? JSON.parse(state.currentSession?.calledPersonDetails?.tags)
          : Array.isArray(state.currentSession?.calledPersonDetails?.tags)
            ? state.currentSession?.calledPersonDetails?.tags
            : [];
        tags = tags ?? []
        if (tags?.length > 0) {
          let index: any = tags.findIndex((item: any) => item === payload);
          if (index < 0) {
            tags.push(payload);
          } else {
            //tags.splice(index, 1);
          }
        } else {
          tags.push(payload);
        }
        let findCurrent = _.findIndex(state.sessionList, (session: any) => session?.sessionId === state.currentSession.sessionId);
        if (findCurrent > -1) {
          state.sessionList[findCurrent].calledPersonDetails.tags = JSON.stringify(tags);
        }
        state.currentSession.calledPersonDetails.tags = JSON.stringify(tags);
      } catch (error) {
        console.log(error)
      }
    },
    clearCallBackSession: (state, { payload }) => {
      if (state.sessionList.length > 1) {
        let index = _.findIndex(state.sessionList, (session: any) => session?.sessionId === payload);
        if (index > -1) {
          state.sessionList.splice(index, 1);
          // state.currentSession = state.sessionList[state.sessionList.length - 1];
        }
      } else {
        state.sessionList = [];
        state.currentSession = {};
      }
    },

    setActivityAutoRefresh: (state) => {
      state.activityAutoRefresh = !state.activityAutoRefresh;
    },
    setAllViewAutoRefresh: (state) => {
      state.allViewRefresh = !state.allViewRefresh;
    },
    setinteractionData: (state, { payload }) => {
      state.interactionData = payload
    },
    setCurrentEndSession: (state, { payload }) => {
      state.currentEndSession = payload
    },
    setCurrentEndChannelType: (state, { payload }) => {
      state.currentEndChannelType = payload
    },
    setNotes: (state, { payload }) => {
      let notesData = Array.isArray(payload) ? payload : [payload];
      let index = _.findIndex(state.sessionList, (session: any) => session?.sessionId === state.currentSession?.sessionId);
      if (index > -1) {
        if (!state.currentSession.notes) {
          state.currentSession.notes = [];
          state.sessionList[index].notes = [];
        }
        state.currentSession.notes.push(...notesData)
        state.sessionList[index].notes.push(...notesData);
      }
    },
    setReplaceNotes: (state, { payload }) => {
      // state.currentSession.notes = payload
      let index = _.findIndex(state.sessionList, (session: any) => session?.sessionId === state.currentSession?.sessionId);
      if (index > -1) {
        if (!state.currentSession.notes) {
          state.currentSession.notes = [];
          state.sessionList[index].notes = [];
        }
        state.currentSession.notes = payload
        state.sessionList[index].notes = payload;
      }
    },
    setEditNotes: (state, action) => {
      const { id, content } = action.payload;
      let newNotes = state?.currentSession?.notes?.map((msg: any) =>
        msg.id === id ? { ...msg, content } : msg
      );
      let index = _.findIndex(state.sessionList, (session: any) => session?.sessionId === state.currentSession?.sessionId);
      if (index > -1) {
        if (!state.currentSession.notes) {
          state.currentSession.notes = [];
          state.sessionList[index].notes = [];
        }
        state.currentSession.notes = newNotes
        state.sessionList[index].notes = newNotes;
      }

    },
    supervisorWhisperStatus: (state, { payload }) => {
      state.supervisorWhisperStatus = payload
    },
    setInternalChatDetails: (state, { payload }) => {
      state.internalChatDetails = payload
    },
    setIndividualChatId: (state, { payload }) => {
      state.individualChatId = payload
    },
    incomingcallDetails: (state, { payload }) => {
      state.incomingcallDetails = payload
    },

    emptyWorkspaceData: (state, { payload }) => {
      state.agent_work_space_history = state.agent_work_space_history.filter((item: any) => item?.sessionId !== payload)
      state.agentAssitence = state.agentAssitence.filter((item: any) => item?.sessionId !== payload)
    },
    answeredDuration: (state, { payload }) => {
      state.answeredDuration = payload
    },
    setAdditionalChat: (state, { payload }) => {
      state.additionalChatRequest.push(payload)
    },

    setSessionTime: (state, { payload }) => {
      let index = state.sessionList.findIndex((list: any) => list.sessionId === payload.sessionId);
      if (index > -1) {
        state.sessionList[index].sessionOnGoingTime = payload.time;
      }
      if (state.currentSession.sessionId === payload.sessionId) {
        state.currentSession.sessionOnGoingTime = payload.time;
      }
    },

    setAdditionChatReplaceData: (state, { payload }) => {
      state.additionalChatRequest = payload
    },
    setViewProfile: (state, { payload }) => {
      state.viewProfile = payload
    },

    setInternalAssist: (state, { payload }) => {
      state.internalAssist = payload
    },

    setInternalChatInfo: (state, { payload }) => {
      state.internalChatInfo = payload
    },

    setDropDown: (state, { payload }) => {
      state.dropDown = payload
    },

    setSmsOpen: (state, { payload }) => {
      state.smsOpen = payload
    },

    setCoChatMobileActiveStatus: (state, { payload }) => {
      state.coChatMobileActiveStatus = payload
    },

    setAutoAssignTicket: (state, { payload }) => {
      state.autoAssignTicketing = payload
    },

    setAiSummaryStatus: (state, { payload }) => {
      state.aiSummaryStatus = payload
    },
    setAutoAnswer: (state, { payload }) => {
      state.autoAnswer = payload ?? 0
    },
    setBroadcast: (state, { payload }) => {
      state.broadcast = payload
    },
    setBroadcastGroupList: (state, { payload }) => {
      state.broadcasgrouplist = payload
    },
    setRowData: (state, { payload }) => {
      state.rowData = payload
    },
    updateBroadcastGroupMessage: (state, { payload }) => {
      const { bcHistoryId, message, lastUpdated } = payload;
      state.broadcasgrouplist = state.broadcasgrouplist.map((group: any) => {
        if (group.bcHistoryId === bcHistoryId) {
          return {
            ...group,
            message: { message },
            lastUpdated,
            UnReadCount: 0,
          };
        }
        return group;
      });
    },
    setAgentGroupEmitter: (state, { payload }) => {
      state.agentGroupEmitter = payload
    },

    setBroadAgentDetails: (state, { payload }) => {
      state.broadcastAgent = payload
    },

    broadcastRefresh: (state) => {
      state.broadcastMSGRefresh = !state.broadcastMSGRefresh
    },

    setOpenBroad: (state, { payload }) => {
      state.openbroadcast = payload
    },
    // setMessage: (state, { payload }) => {
    //   if(payload.new){
    //     state.broadcastmessage = payload.message
    //   }else {
    //     state.broadcastmessage.push(payload.message)
    //   }
    // },

    setMessage: (state, { payload }) => {

      if (payload.new) {
        state.broadcastmessage = payload.message;
      } else if (payload.deleted) {
        state.broadcastmessage = state.broadcastmessage.filter(
          (message: any) => message.bcDurationHistoryId !== payload.message.bcDurationHistoryId
        );
      } else {
        if (state.rowData?.bcHistoryId === payload.message?.bcHistoryId) {
          state.groupMessageRefresh = !state.groupMessageRefresh;
        } else {
          return
        }
      }
    },

    setDispositionAISummary: (state, { payload }) => {
      let index = _.findIndex(state.sessionList, (session: any) => session?.sessionId === payload?.sessionId);
      if (index > -1) {
        if (state?.currentSession?.sessionId === payload?.sessionId)
          state.currentSession = { ...state.currentSession, ...payload }
        state.sessionList[index] = { ...state.currentSession, ...payload };
      }
    },
    setDashCallBackRefresh: (state) => {
      state.dashCallBackRefresh = !state.dashCallBackRefresh;
    },
    setLikeMessage: (state, { payload }) => {
      state.agent_work_space_history = payload
    },
    setDashPerformanceRefresh: (state) => {
      state.dashPerformanceRefresh = !state.dashPerformanceRefresh;
    },
    openClosedInteractions: (state) => {
      state.showClosedInteractions = true;
    },
    closeClosedInteractions: (state) => {
      state.showClosedInteractions = false;
    },
    openAllViewInteraction: (state) => {
      state.showAllViewInteraction = true;
    },
    closeAllViewInteraction: (state) => {
      state.showAllViewInteraction = false;
    },
    closeIntractions: (state) => {
      state.showAllViewInteraction = false;
      state.showClosedInteractions = false;
    },
    setShowSummary: (state) => {
      state.showSummary = !state.showSummary;
      // state.route = action.payload;
    },
    setAssignActivityList: (state, { payload }) => {
      state.assignActivityList = payload;
    },

    messageSentimental: (state, { payload }) => {
      let index = _.findIndex(state.agent_work_space_history, (data: any) => {
        return data.messageID == payload.messageId
      });
      if (index > -1) {
        state.agent_work_space_history[index].sentimental = payload;
      }
    },
    setRecommendations: (state, { payload }) => {

      let raw = Array.isArray(payload) ? payload : [payload];

      let data = raw?.map((item) => ({
        id: uuidv4(),
        title: item?.products || item?.product || "",
        desc: item?.reason || "",
        necessary: item?.necessary,
        sid: item?.sid,
        message_id: item?.message_id,
        domain_ext: item?.domain_ext,
        company_id: item?.company_id,
        productIconName: item?.productIconName,
      }));
      state.recommendations.push(...data);
    },
    

    setCustomerMsg: (state, { payload }) => {
      state.customerMsg = payload
    },

    messageSentimental_socialmedia: (state, { payload }) => {
      state.sentimentalMessages.push(payload)
      state.warningMessages = state?.warningMessages.filter((msg: any) => msg?.sessionId !== payload.sessionId);
      state.warningMessages.push(payload);
    },

    messageWarning_remove: (state, { payload }) => {
      state.warningMessages = state.warningMessages.filter((i :any)=> i.sessionId != payload)
    },
    setBuyNowModal: (state, { payload }) => {
      state.buyNowModal = payload
    },
    setInteractionDetails: (state, { payload }) => {
      state.interactionDetails = payload
    },
    reset: () => { },
  },
  extraReducers: (builder) => {

    // builder.addCase(getSessionList.fulfilled, (state, { payload }) => {
    //   state.sessionList = payload?.data?.result;
    //   state.currentSession = payload?.data?.result[payload?.data?.result.length - 1];
    // });

    // builder.addCase(getMessageFromSessionID.pending, (state) => {
    //   state.isMessageLoading = true;
    // });

    // builder.addCase(getMessageFromSessionID.fulfilled, (state, { payload }) => {
    //   if (payload?.data?.statusCode === 200) {
    //     let messageList = payload?.data?.getConversationResListRes.length > 0 ? payload?.data?.getConversationResListRes : [];
    //     messageList?.length > 0 &&
    //       messageList?.map((list: any, index: any) => {
    //         const timeStamp = list?.timeStamp * 1000;
    //         const dateObj = new Date(timeStamp);
    //         const time = dateObj.getHours() + ":" + dateObj.getMinutes();

    //         messageList[index].time = time;
    //       });
    //     state.agent_work_space_history = messageList.length > 0 ? messageList : [];
    //     state.isMessageLoading = false;
    //   }
    // });
    // builder.addCase(getMessageFromSessionID.rejected, (state, { payload }) => {
    //   state.isMessageLoading = false;
    // });


  },
});

export const ChatActions = chatSlice.actions;
export default chatSlice.reducer;
