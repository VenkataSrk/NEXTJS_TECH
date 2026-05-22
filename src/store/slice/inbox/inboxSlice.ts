import { createSlice } from "@reduxjs/toolkit";
import { INBOX } from "../../constants/reducer_names";
import { groupbyday } from "../../../base/utils";
import { filterInboxData } from "../helper";


export const initialState = {
  composeMailToggle: false,
  composeMailInfo: {
    to: "",
    cc: "",
    bcc: "",
    subject: "",
    body: "",
  },
  inboxList_allrecord: [] as any,
  inboxList_groupData: [],
  domainEmailConfig: {},
  perticularInboxData: [],
  perticularInboxContactData: {},
  emailSendSuccess: false,
  forwardedMessage: false,
  forwardedMailDetails: null,
  replyMessage: false,
  replyMailDetails: null,
  replyALLMessage: false,
  replyAllMailDetails: null,
  moveToTrash: false,
  openStatus: false,
  activityTimeLine: [],
  inboxAiChatMessage: [] as any,
  inboxAiChatTyping: {},
  inboxChatSessionId: "",
  customerNotes: [] as any,
  aiSuggestions: [] as any,
  currentSuggestionIndex: 0,
  pendingInsertReply: null as any,
  inboundCallViewData: null as any,
  inboxFilter: {
    fromDate: null,
    toDate: null,
    search: null,
    tab: 'Inbox',
    channelType: 'Email,Call',
    inboxType: null
  },
  inboxRefresh: false,
  unReadCount: 0,
  totalEmailCount: 0,
  aiChatHistoryData: []
};

const inboxDataSlice = createSlice({
  name: INBOX,
  initialState,
  reducers: {
    setInboxList: (state, { payload }) => {
      state.inboxList_allrecord = payload?.allrecord ?? [];
      state.inboxList_groupData = payload?.groupData ?? [];
    },
    setComposeMailToggle: (state, { payload }) => {
      state.composeMailToggle = payload;
    },

    setDomainEmailconfig: (state, { payload }) => {
      state.domainEmailConfig = payload;
    },

    setPerticularInboxData: (state, { payload }) => {
      state.perticularInboxData = payload;
    },

    setPerticularInboxContactData: (state, { payload }) => {
      state.perticularInboxContactData = payload;
    },

    setEmailSendSuccess: (state) => {
      state.emailSendSuccess = !state.emailSendSuccess;
    },
    setForwardedMessage: (state, { payload }) => {
      state.forwardedMessage = payload;
    },
    setForwardMailDetails: (state, { payload }) => {
      state.forwardedMailDetails = payload;
    },
    setReplyMessage: (state, { payload }) => {
      state.replyMessage = payload;
    },
    setReplyMailDetails: (state, { payload }) => {
      state.replyMailDetails = payload;
    },
    setReplyAllMessage: (state, { payload }) => {
      state.replyALLMessage = payload;
    },
    setReplyAllMailDetails: (state, { payload }) => {
      state.replyAllMailDetails = payload;
    },
    setCleanReplyForword : (state) => {
      state.forwardedMessage = false;
      state.forwardedMailDetails = null;
      state.replyMessage = false;
      state.replyMailDetails = null;
      state.replyALLMessage = false;
      state.replyAllMailDetails = null;
    },
    setMoveToTrash: (state, { payload }) => {
      console.log("setMoveToTrash :::: >>>>>", payload)
      const filterData = filterInboxData(state.inboxList_allrecord, payload);
      state.inboxList_allrecord = filterData;
      state.inboxList_groupData= groupbyday(filterData) ?? [];
    },
    markEmailOpened: (state, { payload }) => {
      const id = payload?.historyId;
      if (!id) return;
    
      state.inboxList_allrecord?.forEach((mail: any) => {
        if (mail?.EmailHistoryId === id || mail?.SMSHistoryId === id) mail.isOpened = 1;
      });
    
      state.inboxList_groupData?.forEach((group: any) => {
        const items: any = Object.values(group || {})[0];
        if (Array.isArray(items)) {
          items?.forEach((mail: any) => {
            if (mail?.EmailHistoryId === id || mail?.SMSHistoryId === id) mail.isOpened = 1;
          });
        }
      });
    },
    markEmailOpenStatus: (state, { payload }) => {
      const id = payload?.historyId;
      const isOpened = Number(payload?.isOpened ?? 0);
      if (!id) return;

      state.inboxList_allrecord?.forEach((mail: any) => {
        if (mail?.EmailHistoryId === id || mail?.SMSHistoryId === id) mail.isOpened = isOpened;
      });

      state.inboxList_groupData?.forEach((group: any) => {
        const items: any = Object.values(group || {})[0];
        if (Array.isArray(items)) {
          items?.forEach((mail: any) => {
            if (mail?.EmailHistoryId === id || mail?.SMSHistoryId === id) mail.isOpened = isOpened;
          });
        }
      });
    },
    markEmailPinStatus: (state, { payload }) => {
      try {
        const id = Number(payload?.historyId);
        const isPin = Number(payload?.isPin ?? 0);
        if (!id) return;

        const list = state.inboxList_allrecord;
        const index = list.findIndex((m: any) => Number(m?.EmailHistoryId) === id || Number(m?.SMSHistoryId) === id);
        if (index === -1) return;
    
        const item = list[index];
        item.is_pin = isPin;
        
        // if (isPin === 1 && index !== 0) {
        //   list.splice(index, 1);
        //   list.unshift(item);
        // }
        
        state.inboxList_groupData = groupbyday(list);
      } catch (e) {
        console.error('markPinStatus error', e);
      }
    },
    setActivityTimeLine: (state, { payload }) => {
      state.activityTimeLine = payload;
    },

    setInboxAichatMessage: (state, { payload }) => {
      state.inboxAiChatMessage.push(payload);
    },
    setInboxAichatMessageOld: (state, { payload }) => {
      state.inboxAiChatMessage = payload?.data ?? [];
      state.inboxChatSessionId = payload?.sessionId ?? "";
    },

    setInboxAiChatTyping: (state, { payload }) => {
      const typingValue = payload?.typing === true ? "Typing ..." : payload?.typing;
      state.inboxAiChatTyping[payload?.sessionId] = typingValue;
    },

    setInboxChatSessionId: (state, { payload }) => {
      state.inboxChatSessionId = payload ?? "";
    },

    setCustomerNotes: (state, { payload }) => {
      state.customerNotes = payload ?? "";
    },

    setInboundCallViewData: (state, { payload }) => {
      state.inboundCallViewData = payload ?? null;
    },

    resetInboxChat: (state) => {
      state.inboxAiChatMessage = [];
      state.inboxAiChatTyping = {};
      state.inboxChatSessionId = "";
    },
    setAiSuggestions: (state, action) => {
      state.aiSuggestions = action.payload || [];
      state.currentSuggestionIndex = 0;
    },
    
    addAiSuggestion: (state, action) => {
      if (!state.aiSuggestions) {
        state.aiSuggestions = [];
      }
      state.aiSuggestions.push(action.payload);
    },
    
    setCurrentSuggestionIndex: (state, action) => {
      const index = action.payload;
      if (index >= 0 && index < state.aiSuggestions.length) {
        state.currentSuggestionIndex = index;
      }
    },
    
    replaceAiSuggestion: (state, action) => {
      const { index, data } = action.payload;
      if (
        Array.isArray(state.aiSuggestions) &&
        index >= 0 &&
        index < state.aiSuggestions.length
      ) {
        state.aiSuggestions[index] = data;
      }
    },
    
    clearAiSuggestions: (state) => {
      state.aiSuggestions = [];
      state.currentSuggestionIndex = 0;
    },

    setPendingInsertReply: (state, { payload }) => {
      state.pendingInsertReply = payload ?? null;
    },

    clearPendingInsertReply: (state) => {
      state.pendingInsertReply = null;
    },

    setInboxFilter: (state, { payload }) => {
      state.inboxFilter = {...state.inboxFilter, ...payload};
    },

    setInboxRefresh : (state) => {
      state.inboxRefresh = !state.inboxRefresh;
    },

    setUnReadCount : (state, { payload }) => {
      state.unReadCount = payload;
    },

    setTotalEmailCount : (state, { payload }) => {
      state.totalEmailCount = payload;
    },


    setAiChatHistoryData: (state, { payload }) => {
      state.aiChatHistoryData = payload;
    },

    reset: () => initialState,
  },
});

export const inboxDataSliceActions = inboxDataSlice.actions;
export default inboxDataSlice.reducer;
