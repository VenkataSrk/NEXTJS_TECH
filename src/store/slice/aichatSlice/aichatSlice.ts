import { createSlice } from "@reduxjs/toolkit";
import { AI_AGENT } from "../../constants/reducer_names";

const initialState = {
  aiAgentChatMessage: [] as any,
  aiAgentChatTyping: false,
  isQualifiedCustomer: null,
}
const AIAgentChat = createSlice({
  name: AI_AGENT,
  initialState,
  reducers: {
    
    setAiAgentChat: (state, { payload }) => {
      state.aiAgentChatMessage.push(payload)
    },
    
    setAiAgentChatTyping: (state, { payload }) => {
      state.aiAgentChatTyping = payload;
    },

    setAiAgentChatReset: (state) => {
      state.aiAgentChatMessage = []
    },

    setIsQualifiedCustomer: (state, { payload }) => {
      state.isQualifiedCustomer = payload
    },

    reset: () => initialState,
  },
});
export const AIAgentChatAction = AIAgentChat.actions;
export default AIAgentChat.reducer;