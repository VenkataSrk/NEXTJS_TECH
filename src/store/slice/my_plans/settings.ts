import { createSlice } from "@reduxjs/toolkit";

import { v4 as uuidv4 } from "uuid";
import { EnterPriceDefaultConfig } from "../../../base/onboardConfig";
import { AI_CCAAS_SETTINGS } from "../../constants/reducer_names";

const initialState = {
  onBoardingMessage: [] as any,
  messageList: [] as any,
  sessionList: [] as any,
  currentSession: [] as any,
  // onboardingStaus: [
  //   {
  //     label: 'Business Hours',
  //     desc: "Set your team’s working hours — defines when agents are available to take chats or calls.",
  //     key: 'business_hours',
  //     isCompleted: 0,
  //   },
  //   {
  //     label: 'Queue Management',
  //     desc: "Group of agents who handle a specific type of customer query or task.",
  //     key: 'queue',
  //     isCompleted: 0,
  //   },
  //   {
  //     label: 'User Management',
  //     desc: "Agents or team members assigned to handle chats, calls, or tickets within a queue.",
  //     key: 'user_management',
  //     isCompleted: 0,
  //   },
  //   {
  //     label: 'Channel Configurations',
  //     desc: "The process of setting up and managing the communication pathways",
  //     key: 'call_configurations',
  //     isCompleted: 0,
  //   },

  //   {
  //     label: 'Integration',
  //     desc: "Connect and sync your external tools or platforms to streamline workflows.",
  //     key: 'integration̵',
  //     isCompleted: 0,
  //   },
  // ] as any,

  onboardingStaus:  [...EnterPriceDefaultConfig],
  onboardingActiveKey: null,
  aiAnalyticsMessages: [] as any,
  aiAnalyticsTyping: false,
  searchSessionId: uuidv4(),
  reportSessionId: uuidv4(),
  queueSessionId: uuidv4(),
  agentSessionId: uuidv4(),
  peopleSessionId: uuidv4(),
  campaignMessages: [] as any,
  aiCampampaignTyping: false,
  onboardingPercentage: 0,
  outboundDetails: { email: '', sms: '' },
  currentChatSessionId: null,
}
const AISettings = createSlice({
  name: AI_CCAAS_SETTINGS,
  initialState,
  reducers: {
    updateOnBoardingMsg: (state, { payload }) => {
      state.onBoardingMessage.push(payload)
    },
    chatSessionList: (state, { payload }) => {
      let index = state.sessionList.findIndex((session: any) => session?.sessionId == payload?.sessionId);
      if (index > -1) {
        state.sessionList[index] = payload;
      } else {
        state.sessionList.unshift(payload);
      }
    },
    setCurrentsession: (state, { payload }) => {
      state.currentSession = payload.currentSession
    },
    setAiAnalyticsMessages: (state, { payload }) => {
      state.aiAnalyticsMessages.push(payload)
    },
    clearAiAnalyticsMessages: (state, { payload }) => {
      state.aiAnalyticsMessages = payload;
    },
    setaiAnalyticsTyping: (state, { payload }) => {
      state.aiAnalyticsTyping = payload;
    },

    updateOnBoardingStatus: (state, { payload }) => {
      state.onboardingStaus = payload
    },
    setOnboardingActiveKey: (state, { payload }) => {
      state.onboardingActiveKey = payload
    },

    setCampaignMessages: (state, { payload }) => {
      state.campaignMessages.push(payload)
    },
    setaiCampaignTyping: (state, { payload }) => {
      state.aiCampampaignTyping = payload;
    },
    clearCampaignMessage: (state, { payload }) => {
      state.campaignMessages = payload;
    },
    setCampaignData: (state, { payload }) => {
      state.campaignMessages = payload
      state.aiCampampaignTyping = false
    },
    updateOnBoardingPercentage: (state, { payload }) => {
      state.onboardingPercentage = payload
    },
    setOutboundDetails: (state, { payload }) => {
      state.outboundDetails = payload
    },
    setCurrentChatSessionId: (state, { payload }) => {
      state.currentChatSessionId = payload;
    },
    setReportSessionId: (state, { payload }) => {
      state.reportSessionId = payload;
    },
    setQueueSessionId: (state, { payload }) => {
      state.queueSessionId = payload;
    },
    setAgentSessionId: (state, { payload }) => {
      state.agentSessionId = payload;
    },
    setPeopleSessionId: (state, { payload }) => {
      state.peopleSessionId = payload;
    },
    setSearchSessionId: (state, { payload }) => {
      state.searchSessionId = payload;
    },
    reset: () => initialState,
  },
});
export const AiSettingsAction = AISettings.actions;
export default AISettings.reducer;