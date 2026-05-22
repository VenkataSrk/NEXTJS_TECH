import { createSlice } from "@reduxjs/toolkit";
import { MARKETING_AGENT } from "../../constants/reducer_names";

const initialState = {
  marketingAgentChatMessage: {} as Record<string,any[]>,
  marketingAgentTemplateChatMessage: {} as Record<string,any[]>,
  aiAgentChatTyping: {},
  marketingConversationType:{} as any,
  marketingProgress:{},
  loader:{},
  pendingApproval:{},
  marketingInsight: null as any,
  marketingInsightLoader: false,
}
const MarketingAgent = createSlice({
  name: MARKETING_AGENT,
  initialState,
  reducers: {
    
    setMarketingAgentChat: (state, { payload }) => {
       const sessionId = payload?.sessionId;
      if (!sessionId) return;

      if (!state.marketingAgentChatMessage[sessionId]) {
        state.marketingAgentChatMessage[sessionId] = [];
      }

      state.marketingAgentChatMessage[sessionId].push(payload);
    },
    
    setMarketingAgentChatTyping: (state, { payload }) => {
      const typingValue = payload?.typing === true ? "Typing ..." : payload?.typing;
      state.aiAgentChatTyping[payload?.sessionId] = typingValue;
    },

    setMarketingAgentChatReset: (state) => {
      state.marketingAgentChatMessage = {}
    },
    setTemplateChatConversation:(state, { payload })=>{
      const templateId = payload?.templateId;
      if (!templateId) return;

      // if (!state.marketingAgentTemplateChatMessage[templateId]) {
      //   state.marketingAgentTemplateChatMessage[templateId] = [];
      // }

      state.marketingAgentTemplateChatMessage[templateId].push(payload);
    },
    setTemplateConversation: (state, {payload} ) => {
      console.log("templateIdslice",payload)
      const templateId = payload?.[0]?.templateId;
      if (!templateId) return;

      // if (!state.marketingAgentTemplateChatMessage[templateId]) {
      //   state.marketingAgentTemplateChatMessage[templateId] = [];
      // }

      state.marketingAgentTemplateChatMessage[templateId]=[...payload];
    },
    setSessionConversationType:(state,{payload})=>{
      const sessionId = payload?.sessionId;
      if (!sessionId) return;

      if (!state.marketingConversationType[sessionId]) {
        state.marketingConversationType[sessionId] = {};
      }

      state.marketingConversationType[sessionId]={...payload};
    },
    setMarketingChatProgress:(state,{payload})=>{
      const sessionId = payload?.sessionId;
      if(!sessionId) return;
      if(payload?.progressMessage=="completed"){
        state.marketingProgress[sessionId]={};
      }
        state.marketingProgress[sessionId]=payload;
    },
    setMarketingTemplateChatLoader: (state, { payload }) => {
      const sessionId = payload?.sessionId;
      if (!sessionId) return;
      state.loader[sessionId] = payload;
    },
    setPendingApprovalData: (state, { payload }) => {
     const sessionId = payload?.sessionId;
      if (!sessionId) return;
      state.pendingApproval[sessionId] = payload;
    },
    setFullMarketingAgentChat: (state, { payload }) => {
      const sessionId = payload?.[0]?.sessionId;
      state.marketingAgentChatMessage[sessionId]=payload;
    },
    setMarketingInsight: (state, { payload }) => {
      if (payload?.data !== undefined) state.marketingInsight = payload.data;
      if (payload?.loader !== undefined) state.marketingInsightLoader = payload.loader;
    },
    reset: () => initialState,
  },

});
export const MarketingAgentChatAction = MarketingAgent.actions;
export default MarketingAgent.reducer;

