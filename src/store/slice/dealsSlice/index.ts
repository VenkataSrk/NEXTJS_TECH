import { createSlice } from "@reduxjs/toolkit";
import { DEALS } from "../../constants/reducer_names";

export const initialState = {
  aiAgentChatMessage: [] as any,
  aiAgentChatTyping: {},
  dealsListFilters: {
    fromDate: null,
    toDate: null,
    contact: null,
    dealName: null,
    companyName: null,
    pipelineStage: null,
    dealEstimatedClosed: null,
    location: null,
    totalAmount: null,
    closedAmount: null,
    ownerName: null,
  },
  customerDealData: [] as any,
  dealsloader: false
};

const dealsDataSlice = createSlice({
  name: DEALS,
  initialState,
  reducers: {
    setdealsAiAgentChat: (state, { payload }) => {
      state.aiAgentChatMessage.push(payload);
    },

    setdealsAiAgentChatTyping: (state, { payload }) => {
      const typingValue = payload?.typing === true ? "Typing ..." : payload?.typing;
      state.aiAgentChatTyping[payload?.sessionId] = typingValue;
    },

    setDealsListFilters: (state, { payload }) => {
      state.dealsListFilters = {
        fromDate: payload?.fromDate ?? null,
        toDate: payload?.toDate ?? null,
        contact: payload?.contact ?? null,
        dealName: payload?.dealName ?? null,
        companyName: payload?.companyName ?? null,
        pipelineStage: payload?.pipelineStage ?? null,
        dealEstimatedClosed: payload?.dealEstimatedClosed ?? null,
        location: payload?.location ?? null,
        totalAmount: payload?.totalAmount ?? null,
        closedAmount: payload?.closedAmount ?? null,
        ownerName: payload?.ownerName ?? null,
      };
    },

    setdealsAiAgentChatReset: (state) => {
      state.aiAgentChatMessage = [];
    },

    SetCustomerDealData: (state, { payload }) => {
      state.customerDealData = payload.data;
      state.dealsloader = payload.loader;
    },

    reset: () => initialState,
  },
});

export const dealsDataSliceActions = dealsDataSlice.actions;
export default dealsDataSlice.reducer;
