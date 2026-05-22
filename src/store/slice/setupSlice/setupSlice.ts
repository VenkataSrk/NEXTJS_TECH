import { createSlice } from "@reduxjs/toolkit";
import { SETUP } from "../../constants/reducer_names";

type SetupState = {
  businessHoursList: any[];
  businessHolidaysList: any[];
  aiAgentChatMessage: any[];
  aiAgentChatTyping: {},
};

const initialState: SetupState = {
  businessHoursList: [],
  businessHolidaysList: [],
  aiAgentChatMessage: [] as any,
  aiAgentChatTyping: {},
};

const setupSlice = createSlice({
  name: SETUP,
  initialState,
  reducers: {
    setBusinessHoursList: (state, { payload }) => {
      state.businessHoursList = Array.isArray(payload) ? payload : [];
    },
    setBusinessHolidaysList: (state, { payload }) => {
      state.businessHolidaysList = Array.isArray(payload) ? payload : [];
    },
    setSetupAiAgentChat: (state, { payload }) => {
      state.aiAgentChatMessage.push(payload);
    },

    setSetupAiAgentChatTyping: (state, { payload }) => {
      const typingValue = payload?.typing === true ? "Typing ..." : payload?.typing;
      state.aiAgentChatTyping[payload?.sessionId] = typingValue;
    },

    setSetupAiAgentChatReset: (state) => {
      state.aiAgentChatMessage = [];
    },

    reset: () => initialState,
  },
});

export const SetupSliceActions = setupSlice.actions;
export default setupSlice.reducer;
