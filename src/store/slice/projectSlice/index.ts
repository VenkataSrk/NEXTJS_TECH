import { createSlice } from "@reduxjs/toolkit";
import { PROJECT } from "../../constants/reducer_names";


export const initialState = {
    aiAgentChatMessage: [] as any,
    aiAgentChatTyping: {},
    projectInsight: [] as any,
    projectInsightLoader: false,
 };

const projectDataSlice = createSlice({
  name: PROJECT,
  initialState,
  reducers: {
    
    setprojectAiAgentChat: (state, { payload }) => {
      state.aiAgentChatMessage.push(payload)
    },
    
    setprojectAiAgentChatTyping: (state, { payload }) => {
      const typingValue = payload?.typing === true ? "Typing ..." : payload?.typing;
      state.aiAgentChatTyping[payload?.sessionId] = typingValue;
    },

    setprojectAiAgentChatReset: (state) => {
      state.aiAgentChatMessage = []
    },

    setProjectInsight: (state, { payload }) => {
      if (payload?.data !== undefined) state.projectInsight = payload.data;
      if (payload?.loader !== undefined) state.projectInsightLoader = payload.loader;
    },

    reset: () => initialState,
  },
});

export const projectDataSliceActions = projectDataSlice.actions;
export default projectDataSlice.reducer;
