import { createSlice } from "@reduxjs/toolkit";
import { CONTACT } from "../../constants/reducer_names";
import { v4 as uuidV4 } from "uuid";

export const initialState = {
    companyListLoading: false,
    companyList: [],
    companyListHasMore: true,
    contactListLoading: false,
    contactList: [],
    contactListFilters: {},
    contactListRefresh: false,
    contactProjectList: [] as any,
    contactProjectLoading: false,
    contactProjectProId: null as string | null,
    selectedContact: {} as Record<string, any>,
    selectedContactLoading: {} as Record<string, boolean>,
    companyListFilters: {
      company_name: null,
      owner_name: null,
      source_name: null,
      from_date: null,
      to_date: null,
      industry: null,
      company_address: null,
      company_size: null,
      offset: 0,
      limit: 50,
    },
    aiAgentChatMessage: [] as any,
    aiAgentChatTyping: {},
    contactScheduledata: [],
    customerNotes: [] as any,
    customerEmail: {} as any,
    customerDealId: null,
    openContactUpdateModel: {},
    contactPerticularActivity: {},
    contactModuleFollowUp: {
      companyList : {
        sessionId: null,
        response: null,
        searchStatus: null,
        isWaitingResponse: false
      },
      contactList : {
        sessionId: null,
        response: null,
        searchStatus: null,
        isWaitingResponse: false
      }
    },
    contactmoduleSessionId : uuidV4()
 };

const contactDataSlice = createSlice({
  name: CONTACT,
  initialState,
  reducers: {
    setCompanyList: (state, { payload }) => {
      state.companyListLoading = payload?.companyListLoading ?? false;
      if (payload?.companyList !== undefined) {
        state.companyList = payload?.append
          ? [...(state.companyList || []), ...(payload.companyList || [])]
          : (payload.companyList || []);
      }
      if (payload?.companyListHasMore !== undefined) {
        state.companyListHasMore = payload.companyListHasMore;
      }
    },
    setContactList: (state, { payload }) => {
      state.contactListLoading = payload?.contactListLoading ?? false;
      state.contactList = payload?.contactList ?? state.contactList;
    },
    setContactProjectList: (state, { payload }) => {
      state.contactProjectLoading = payload?.contactProjectLoading ?? false;
      if (payload?.contactProjectList !== undefined) {
        state.contactProjectList = payload.contactProjectList || [];
      }
      if (payload?.contactProjectProId !== undefined) {
        state.contactProjectProId = payload.contactProjectProId;
      }
    },
    setSelectedContact: (state, { payload }) => {
      const coid = payload?.coid != null ? String(payload.coid) : null;
      if (!coid) return;
      if (payload?.selectedContactLoading !== undefined) {
        state.selectedContactLoading[coid] = !!payload.selectedContactLoading;
      }
      if (payload?.selectedContact !== undefined) {
        state.selectedContact[coid] = payload.selectedContact || {};
      }
    },
    setContactListFilters: (state, { payload }) => {
      state.contactListFilters = payload;
    },
    setCompanyListFilters: (state, { payload }) => {
      state.companyListFilters = {
        company_name: payload?.company_name !== undefined ? payload?.company_name : state?.companyListFilters?.company_name,
        owner_name: payload?.owner_name !== undefined ? payload?.owner_name : state?.companyListFilters?.owner_name,
        source_name: payload?.source_name !== undefined ? payload?.source_name : state?.companyListFilters?.source_name,
        from_date: payload?.from_date !== undefined ? payload?.from_date : state?.companyListFilters?.from_date,
        to_date: payload?.to_date !== undefined ? payload?.to_date : state?.companyListFilters?.to_date,
        industry: payload?.industry !== undefined ? payload?.industry : state?.companyListFilters?.industry,
        company_address: payload?.company_address !== undefined ? payload?.company_address : state?.companyListFilters?.company_address,
        company_size: payload?.company_size !== undefined ? payload?.company_size : state?.companyListFilters?.company_size,
        offset: payload?.offset !== undefined ? payload?.offset : state?.companyListFilters?.offset,
        limit: payload?.limit !== undefined ? payload?.limit : state?.companyListFilters?.limit,
      };
    },


    setcontactAiAgentChat: (state, { payload }) => {
      state.aiAgentChatMessage.push(payload)
    },
    
    setcontactAiAgentChatTyping: (state, { payload }) => {
      const typingValue = payload?.typing === true ? "Typing ..." : payload?.typing;
      state.aiAgentChatTyping[payload?.sessionId] = typingValue;
    },

    setcontactAiAgentChatReset: (state) => {
      state.aiAgentChatMessage = []
    },

    setContactScheduledata: (state, { payload }) => {
      state.contactScheduledata = payload;
    },

    setCustomerNotes: (state, { payload }) => {
      state.customerNotes = payload;
    },

    setCustomerEmail: (state, { payload }) => {
      state.customerEmail = payload;
    },

    setCustomerDealId: (state, { payload }) => {
      state.customerDealId = payload;
    },

    setContactUpdateModel: (state, { payload }) => {
      state.openContactUpdateModel = payload;
    },

    setContactPerticularActivity: (state, { payload }) => {
      state.contactPerticularActivity = payload;
    },

    setContactModuleFollowUp : (state, { payload }) => {
      const moduleKey = payload?.module;
      if (!moduleKey) return;
      const { module, ...rest } = payload;
      state.contactModuleFollowUp[moduleKey] = {
        ...(state.contactModuleFollowUp[moduleKey] || {}),
        ...rest,
      };
    },

    resetContactModuleFollowUp : (state, { payload }) => {
      const moduleKey = payload?.module;
      if (!moduleKey) return;
      state.contactModuleFollowUp[moduleKey] = {
        sessionId: null,
        response: null,
        searchStatus: null,
        isWaitingResponse: false,
      };
    },

    setUpdatecontactmoduleSessionId : (state) => {
      state.contactmoduleSessionId = uuidV4()
    },

    setContactRefresh : (state) => {
      state.contactListRefresh = !state.contactListRefresh
    },

    reset: () => initialState,
  },
});

export const contactDataSliceActions = contactDataSlice.actions;
export default contactDataSlice.reducer;
