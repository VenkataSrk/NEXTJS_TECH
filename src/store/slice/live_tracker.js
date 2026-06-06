import { MESSAGES } from "../redux-store/reducer_names";
import { createSlice } from "@reduxjs/toolkit";
import { v4 as uuidv4 } from "uuid";

const initialState = {
  uuid: '',
  agentSessionAvaiableStatus: false,
  botSessionAvaiableStatus: false,
  transferQueueId: 0,
  queuedPositionData: {},
  iconHideStatus: false,
  mobileIconStatus: false,
  settingStatus: false,
  reloadSettings: false,
  socketData: null,
  timeOutIntervalData: '',
  agentDisconnectStatus: false,
  restrictedEndChat: false,
  selectedTextLanguage: {
    text: 'English (Default)',
    code: 'en',
    symbol: 'EN',
    micCode: 'en-UK'
  },
  multiLanguageList: [],
  refMultiLanguageList: [
    {
      text: 'English (Default)',
      code: 'en',
      symbol: 'EN',
      micCode: 'en-UK'
    },
    {
      text: 'Malayalam',
      code: 'Malayalam',
      symbol: 'MA',
      micCode: 'ml-IN'
    },
    {
      text: 'Telugu',
      code: 'te',
      symbol: 'TE',
      micCode: 'te-IN'
    },
    {
      text: 'Tamil',
      code: 'ta',
      symbol: 'TA',
      micCode: 'ta-IN'
    },
    {
      text: 'Hindi',
      code: 'hi',
      symbol: 'HI',
      micCode: 'hi-IN'
    },
    {
      text: 'Marathi',
      code: 'mar',
      symbol: 'MA',
      micCode: 'mr-IN'
    },
    {
      text: 'Odia',
      code: 'od',
      symbol: 'OD',
      micCode: 'or-IN'
    },
    {
      text: 'Kannada',
      code: 'ka',
      symbol: 'KA',
      micCode: 'kn-IN'
    },
    {
      text: 'Gujarathi',
      code: 'gu',
      symbol: 'GU',
      micCode: 'gu-IN'
    },
    {
      text: 'Bengali',
      code: 'ba',
      symbol: 'BN',
      micCode: 'bn-IN'
    },
    {
      text: 'Spanish',
      code: 'spa',
      symbol: 'SP',
      micCode: 'es-ES'
    },
    {
      text: 'Italian',
      code: 'it',
      symbol: 'IT',
      micCode: 'it-IT'
    },
    {
      text: 'German',
      code: 'ge',
      symbol: 'GE',
      micCode: 'de-DE'
    },
    {
      text: 'thai',
      code: 'th',
      symbol: 'TH',
      micCode: 'th-TH'
    },
    {
      text: 'french',
      code: 'fr',
      symbol: 'FR',
      micCode: 'fr-FR'
    }
  ],
  liveChatCutData: {},
  sendMessageEnable: true,
  worktualCookiesData: null,
  remainderActiveCount: 0,
  agentTypingStatus: false,
  agentTypingName: '',
  openScreenTriggerStatus: false,
  openScreenTriggerMsg: '',
  speakingTextStatus: false
};

const liveTrackerSice = createSlice({
  name: MESSAGES,
  initialState,
  reducers: {
    uuid: (state, { payload }) => {
      state.uuid = payload;
    },
    setAgentSessionAvaiableStatus: (state, { payload }) => {
      state.agentSessionAvaiableStatus = payload
    },
    setBotSessionAvaiableStatus: (state, { payload }) => {
      state.botSessionAvaiableStatus = payload
    },
    setTransferQueueId: (state, { payload }) => {
      state.transferQueueId = payload
    },
    setIconHideStatus: (state, { payload }) => {
      state.iconHideStatus = payload
    },
    setMobileIconStatus: (state, { payload }) => {
      state.mobileIconStatus = payload
    },
    setSettingStatus: (state, { payload }) => {
      state.settingStatus = payload
    },
    setReloadSettings: (state, { }) => {
      state.reloadSettings = uuidv4()
    },
    setSocketData: (state, { payload }) => {
      state.socketData = payload
    },
    setQueuePositionData: (state, { payload }) => {
      state.queuedPositionData = payload
    },
    setTimeOutIntervalsData: (state, { payload }) => {
      state.timeOutIntervalData = payload
    },
    setAgentDisconnectStatus: (state, { payload }) => {
      state.agentDisconnectStatus = payload
    },
    setSelectedTextLanguage: (state, { payload }) => {
      state.selectedTextLanguage = payload
    },
    setRestrictedEndChat: (state, { payload }) => {
      state.restrictedEndChat = payload
    },
    setLiveChatCutData: (state, { payload }) => {
      state.liveChatCutData = payload
    },
    setSendMessageStatus: (state, { payload }) => {
      state.sendMessageEnable = payload
    },
    setMultiLanguageList: (state, { payload }) => {
      state.multiLanguageList = payload
    },
    setWorktualCookiesData: (state, { payload }) => {
      state.worktualCookiesData = payload
    },
    setRemainderActiveCount: (state, { payload }) => {
      state.remainderActiveCount = state.remainderActiveCount + 1
    },
    setAgentTypingStatus: (state, { payload }) => {
      state.agentTypingStatus = payload.status
      state.agentTypingName = payload.name
    },
    setOpenScreenTriggerStatus: (state, { payload }) => {
      state.openScreenTriggerStatus = payload
    },
    setOpenScreenTriggerMessage: (state, { payload }) => {
      state.openScreenTriggerMsg = payload
    },
    setSpeakingTextStatus: (state, { payload }) =>{
      state.speakingTextStatus = payload
    },
    reset: () => initialState,
  },
});

export const liveTrackerActions = liveTrackerSice.actions;
export default liveTrackerSice.reducer;
