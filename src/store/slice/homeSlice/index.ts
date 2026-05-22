import { createSlice } from "@reduxjs/toolkit";
import { HOME } from "../../constants/reducer_names";
import  * as helper  from '../helper'

export const initialState = {
    aiAgentChatMessage: [] as any,
    notification: [] as any,
    notificationUnreadCount: 0,
    aiAgentChatTyping: {},
    homeSales: null as any,
    homeSalesLoader: false,
    homeMarketing: null as any,
    homeMarketingLoader: false,
    homeProject: null as any,
    homeProjectLoader: false,
 };

const homeDataSlice = createSlice({
  name: HOME,
  initialState,
  reducers: {
    setHomeAiAgentChat: (state, { payload }) => {
      state.aiAgentChatMessage.push(payload)
    },
    setHomeAiAgentChatTyping: (state, { payload }) => {
      const typingValue = payload?.typing === true ? "Typing ..." : payload?.typing;
      state.aiAgentChatTyping[payload?.sessionId] = typingValue;
    },
    setHomeAiAgentChatReset: (state) => {
      state.aiAgentChatMessage = []
    },
    setNotification: (state, { payload }) => {
      state.notification = payload;
      state.notificationUnreadCount = helper.calculateNotiUnreadCount(payload);
    },
    setNotificationUnreadCount: (state, { payload }) => {
      state.notificationUnreadCount = payload;
    },
    setHomeSales: (state, { payload }) => {
      if (payload?.data !== undefined) state.homeSales = payload.data;
      if (payload?.loader !== undefined) state.homeSalesLoader = payload.loader;
    },
    setHomeMarketing: (state, { payload }) => {
      if (payload?.data !== undefined) state.homeMarketing = payload.data;
      if (payload?.loader !== undefined) state.homeMarketingLoader = payload.loader;
    },
    setHomeProject: (state, { payload }) => {
      if (payload?.data !== undefined) state.homeProject = payload.data;
      if (payload?.loader !== undefined) state.homeProjectLoader = payload.loader;
    },
    reset: () => initialState,
  },
});

export const homeDataSliceActions = homeDataSlice.actions;
export default homeDataSlice.reducer;
