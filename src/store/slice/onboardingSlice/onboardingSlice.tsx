import { createSlice } from "@reduxjs/toolkit";
import { v4 as uuidv4 } from "uuid";
import { CRM_ONBOARDING } from "../../constants/reducer_names";
import { EnterPriceDefaultConfig } from "../../../base/onboardConfig";

const initialState = {
  onBoardingMessage: [] as any,
  onboardingStaus: [...EnterPriceDefaultConfig],
  onboardMessages: [] as any,
  aiAgentChatTyping: {},
}

const AiOnboarding = createSlice({
  name: CRM_ONBOARDING,
  initialState,
  reducers: {
    updateOnBoardingMsg: (state, { payload }) => {
      state.onBoardingMessage.push(payload)
    },

    updateOnBoardingStatus: (state, { payload }) => {
      state.onboardingStaus = payload
    },
    setOnboardMessages: (state, { payload }) => {
      state.onboardMessages.push(payload)
    },
    clearAiAnalyticsMessages: (state, { payload }) => {
      state.onboardMessages = payload;
    },
    setOnboardTyping: (state, { payload }) => {
      const typingValue = payload?.typing === true ? "Typing ..." : payload?.typing;
      state.aiAgentChatTyping[payload?.sessionId] = typingValue;
    },

    reset: () => initialState,
  },
});
export const AiOnboardingAction = AiOnboarding.actions;
export default AiOnboarding.reducer;