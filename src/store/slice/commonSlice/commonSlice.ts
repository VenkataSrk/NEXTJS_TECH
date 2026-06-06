import { createSlice } from "@reduxjs/toolkit";
import { COMMON_DATA } from "../../../constants/reducer_names";

export const initialState = {
  sessionId: '',
  socketId: '',
  showBuildCampaign:false,
  disableHomeActive:false,
  createNewCampaign:false,
  pastCampaignData:null,
  draftData:{},
  archiveData:{},
  multiSearchData:{},
  initialMsg:"",
  showMainChat:true,
  callOldCampaigns:false,
  navigateOperationHub:false,
  knowledgeBaseDoc:false,
  companyOnboarded:false,
  whatsappConfig:false,
  liveAnalyticsCampaign:{},
  configDone:false,
  isCreateNewCampaign:false,
  currentCampaignChatId:"",
  search:null,
  clearFiles:false,
  integrationDone:false,
  forceLogout:false,
  updateHomeAPI:false,
  contactRefresh:false,
  emailConfigRefresh:false,
  smsConfigRefresh:false,
  appPushRefresh:false,
  archiveRefresh:false,
  connectorsRefresh:false,
  onboardRefresh:false,
  webpushRefresh:false
};

const commonDataSlice: any = createSlice({
  name: COMMON_DATA,
  initialState,
  reducers: {
    setSessionId: (state, { payload }) => {
      state.sessionId = payload
    },
    setSocketId: (state, { payload }) => {
      state.socketId = payload
    },
    setShowBuildCampaign:(state,{payload})=>{
      state.showBuildCampaign = payload
    },
    setDisableHomeActive:(state,{payload})=>{
      state.disableHomeActive= payload
    },
    setCreateNewCampaign:(state,{payload})=>{
      state.createNewCampaign=payload
    },
    setPastCampaign:(state,{payload})=>{
      state.pastCampaignData =payload
    },
    setDraftData:(state,{payload})=>{
      state.draftData =payload
    },
    setArchiveData:(state,{payload})=>{
      state.archiveData =payload
    },
    setMultiSearchData:(state,{payload})=>{
      state.multiSearchData = payload
    },
    setInitialChatMsg:(state,{payload})=>{
      state.initialMsg = payload
    },
    setShowMainChat: (state, { payload }) => {
      state.showMainChat = payload;
    },
    setCallOldCampaign:(state,{payload})=>{
      state.callOldCampaigns = payload;
    },
    setNavigateOperationHub:(state,{payload})=>{
      state.navigateOperationHub = payload;
    },
    setKnowledgeBaseDocAdded:(state,{payload})=>{
      state.knowledgeBaseDoc = payload;
    },
    setCompanyOnboarded:(state,{payload})=>{
      state.companyOnboarded = payload;
    },
    setWhatsAppConfig:(state,{payload })=>{
      state.whatsappConfig = payload
    },
    setLiveAnalyticsCampaign:(state,{payload})=>{
      state.liveAnalyticsCampaign = payload
    },
    setConfigDone:(state,{payload})=>{
      state.configDone = payload;
    },
    setIsNewCampaign:(state,{payload})=>{
      state.isCreateNewCampaign=payload;
    },
    setCurrentCampaignChatId:(state,{payload})=>{
      state.currentCampaignChatId=payload;
    },
    setContactFilter:(state,{payload})=>{
      state.search=payload;
    },
    setClearFiles:(state,{payload})=>{
      state.clearFiles = payload
    },
    setIntegrationDone:(state,{payload})=>{
      state.integrationDone=payload
    },
    setForceLogout:(state,{payload})=>{
      state.forceLogout = payload
    },
    setUpdateHomeAPI:(state,{payload})=>{
      state.updateHomeAPI=payload
    },
    setContactRefresh:(state,{payload})=>{
      state.contactRefresh = payload
    },
    setEmailConfigRefresh:(state,{payload})=>{
      state.emailConfigRefresh = payload
    },
    setSmsConfigRefresh:(state,{payload})=>{
      state.smsConfigRefresh = payload
    },
    setAppPushRefresh:(state,{payload})=>{
      state.appPushRefresh = payload
    },
    setArchiveRefresh:(state,{payload})=>{
      state.archiveRefresh=payload
    },
    setConnectorsRefresh:(state,{payload})=>{
      state.connectorsRefresh = payload
    },
    setOnboardRefresh:(state,{payload})=>{
      state.onboardRefresh=payload
    },
    setWebPushRefresh:(state,{payload})=>{
      state.webpushRefresh = payload
    },
    reset: () => initialState,
  },
});

export const commonDataSliceActions = commonDataSlice.actions;
export default commonDataSlice.reducer;
