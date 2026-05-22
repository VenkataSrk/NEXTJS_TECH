import { createSlice } from "@reduxjs/toolkit";
import { MY_PLANS } from "../../constants/reducer_names";

const initialState = {
  myplanProfiledata: null,
  usermanagementdetails: null,
  orderHistory:null,
  subscriptionManagement: null,
  addonList: null,
  purchaseAddonList: null,
  botResponse:null,
  myPlanUpdation: false,
  aiAgentChatMessage: [] as any,
  aiAgentChatTyping: {},
};

const userExtensionSlice = createSlice({
  name: MY_PLANS,
  initialState,
  reducers: {
    setUserExtensionData: (state, action) => {
      state.myplanProfiledata = action.payload;
    },
    setUserManagementDetails: (state, action) => {
      state.usermanagementdetails = action.payload;
    },
    setOredrHistory: (state, action) => {
      state.orderHistory = action.payload;
    },
    setSubscriptionManagement: (state, action) => {
      state.subscriptionManagement = action.payload;
    },
    setAddonList: (state, action) => {
      state.addonList = action.payload;
    },
    setPurchaseAddonList: (state, action) => {
      state.purchaseAddonList = action.payload;
    },
    setMyplanBotResponse:(state,action)=>{
      state.botResponse = action.payload;
    },
    setMyPlanUpdation:(state,action)=>{
      state.myPlanUpdation = action.payload;
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
  },
});

export const MyplanProfileActions = userExtensionSlice.actions;
export default userExtensionSlice.reducer;
