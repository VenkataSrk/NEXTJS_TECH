import { MESSAGES } from "../redux-store/reducer_names";
import { createSlice } from "@reduxjs/toolkit";

const initialState = {
  connectionType: {},
  widgetId: "",
  nodeData: [],
  connectedQueueData: [],
  messageList: [],
  iconLogo: "",
  widgetIconLogo: '',
  personalisationAll: {},
  personalisation: {},
  tableData: [],
  callFlowId: "",
  customerChatDetails: [],
  chatVisible: true,
  formVisible: true,
  formData: {},
  customerFormData: {},
  botMessage: "",
  uuid: "",
  socket: "",
  emojiVisible: false,
  storeIpAddress: "",
  deviceType: "",
  callBackForm: {},
  callBackStatus: false,
  agentNameVisible: false,
  apiStatus: true,
  url: "",
  agentDomainExt: [],
  transferCallCount: 0,
  dtmfData: [],
  currentDtmfData: [],
  agentConnectingStatus: false,
  templateKeywords: [],
  templateData: [],
  mobileCount: 0,
  currentNodeFlow: {},
  questionAnswerList: [],
  checkallQuesAns: [],
  companyId: '',
  chatDurationId: 0,
  formDetails: {},
  automatedResponse: {},
  customerBlockStatus: 0,
  simOrderSts: false,
  inputEntity: [],
  liveAgentStatus: 0,
  paymentData: [],
  paymentForm: false,
  cardDetails: [],
  formgenerate: false,
  transactionData: [],
  secondformgenerate: false,
  secondTransactionData: [],
  closeSession: false,
  flowNodeList: {},
  iconCloseStatus: false,
  endNotify: false,
  currentTranferBlock: '',
  callBackFormStatus: 0,
  callBackButtonStatus: 0,
  sessionClosedStatus: false,
  isprechatFrom: false,
  customerData: [],
  flowBreakMsg: {},
  transferDid: 0,
  paymentEnable: false,
  paymentSource: "",
  paymentMethod: [],
  paymentId: '',
  lottieProcess: false,
  particialOutput: [],
  socketId: '',
  industry_type: '',
  chatHistoryIdSlice: 0,
  customContactFiels: [],
  soundNotification: true,
  dynamicChatFormList: [],
  callbackBtnStatus: false,
  callbackAgentStatus: '',
  integration: {},
  worktualAppointment: { isShow: false, isCancel_Reschedule: false, isCancel_Reschedule_Repeat: 0 },
  carouselData: [],
  datetimePickerNode: [],
  dropDownList: [],
  updateCallbacklistData: "",
  multipleTemp: false,
  superVisorExt: [],
  remainderAction: { isRemainder: false, count: 0 },
  flowBreakParentnode: {},
  remainderFlowId: '',
  remainderFlowData: {},
  triggerData: {},
  integrationApiData: {},
  ansCaptureStatus: false,
  ansCaptureKey: '',
  purchasePlanData: {
    purchasePlanStatus: false,
    purchasePlanUrl: ''
  },
  isConsentForm: false,
  purchaseBackIconStatus: false,
  reminderFlowValue: '',
  reminderFlowStatus: false,
  domainDetails: [],
  fileInputtype: false,
  targetLang: 'en',
  liveAgent:'',
  clientLocation: '',
  isUserChatInitiated: false,
  currentlyHandling: 'Bot',
  webtrackingTimer: null,
  clientLocationDetails: {}
}

const messageSice = createSlice({
  name: MESSAGES,
  initialState,
  reducers: {
    widgetId: (state, { payload }) => {
      state.widgetId = payload;
    },
    storeNodeData: (state, { payload }) => {
      state.nodeData = payload;
    },
    dropDownList: (state, { payload }) => {
      state.dropDownList = payload;
    },
    setRemainderFlowId: (state, { payload }) => {
      state.remainderFlowId = payload
    },
    setRemainderFlowData: (state, { payload }) => {
      state.remainderFlowData = payload
    },
    particialOutput: (state, { payload }) => {
      state.particialOutput = payload;
    },
    lottieProcess: (state, { payload }) => {
      state.lottieProcess = payload;
    },
    isprechatFrom: (state, { payload }) => {
      state.isprechatFrom = payload;
    },
    storeConnectedQueueData: (state, { payload }) => {
      state.connectedQueueData = [payload];
    },
    simOrderSts: (state, { payload }) => {
      state.simOrderSts = payload;
    },
    setCallbackBtnStatus: (state, { payload }) => {
      state.callbackBtnStatus = payload
    },
    endNotify: (state, { payload }) => {
      state.endNotify = payload;
    },
    flowNodeList: (state, { payload }) => {
      state.flowNodeList = payload;
    },
    secondTransactionData: (state, { payload }) => {
      state.secondTransactionData = payload;
    },
    closeSession: (state, { payload }) => {
      state.closeSession = payload;
    },
    updateChatHistoryId: (state, { payload }) => {
      state.chatHistoryIdSlice = payload;
    },
    updateCallBack: (state, { payload }) => {
      state.updateCallBacklist = payload;
    },
    formgenerate: (state, { payload }) => {
      state.formgenerate = payload;
    },
    secondformgenerate: (state, { payload }) => {
      state.secondformgenerate = payload;
    },
    transactionData: (state, { payload }) => {
      state.transactionData = payload;
    },
    paymentData: (state, { payload }) => {
      state.paymentData = payload;
    },
    sessionClosedStatus: (state, { payload }) => {
      state.sessionClosedStatus = payload
    },
    setTransferDid: (state, { payload }) => {
      state.transferDid = payload
    },
    cardDetails: (state, { payload }) => {
      state.cardDetails = payload;
    },
    setCallbackAgentStatus: (state, { payload }) => {
      state.callbackAgentStatus = payload;
    },
    paymentForm: (state, { payload }) => {
      state.paymentForm = payload;
    },
    iconCloseStatus: (state, { payload }) => {
      state.iconCloseStatus = payload
    },
    inputEntity: (state, { payload }) => {
      // state.inputEntity.push(payload);
      state.inputEntity = payload;
    },
    currentTranferBlock: (state, { payload }) => {
      state.currentTranferBlock = payload
    },
    automatedResponse: (state, { payload }) => {
      state.automatedResponse = payload;
    },
    callBackButtonStatus: (state, { payload }) => {
      state.callBackButtonStatus = payload
    },
    fileUpload: (state, { payload }) => {
      state.url = payload;
    },
    setIconLogo: (state, { payload }) => {
      state.iconLogo = payload;
    },
    setWidgetIconLogo: (state, { payload }) => {
      state.widgetIconLogo = payload;
    },
    setLikeMessage: (state, { payload }) => {
      state.messageList = payload;
    },
    setMessageChat: (state, { payload }) => {
      if (payload?.agentText?.message?.to === "playPrompt_0") {
        state.messageList.unshift(payload)
      } else {
        state.messageList.push(payload);
      }
    },
    personalisationAll: (state, { payload }) => {
      state.personalisationAll = payload;
    },
    liveAgentStatus: (state, { payload }) => {
      state.liveAgentStatus = payload;
    },
    setDynamicChatFormList: (state, { payload }) => {
      state.dynamicChatFormList = payload
    },
    currentNodeFlow: (state, { payload }) => {
      state.currentNodeFlow = payload;
    },
    questionAnswerList: (state, { payload }) => {
      state.questionAnswerList = payload;
    },
    callBackFormStatus: (state, { payload }) => {
      state.callBackFormStatus = payload
    },
    personalisation: (state, { payload }) => {
      state.personalisation = payload;
    },
    tableData: (state, { payload }) => {
      state.tableData = payload;
    },
    templateData: (state, { payload }) => {
      state.templateData = payload;
    },
    callFlowId: (state, { payload }) => {
      state.callFlowId = payload;
    },
    customerBlockStatus: (state, { payload }) => {
      state.customerBlockStatus = payload
    },
    customerChatDetails: (state, { payload }) => {
      state.customerChatDetails = payload;
    },
    templateKeywordList: (state, { payload }) => {
      state.templateKeywords = payload;
    },
    chatVisible: (state, { payload }) => {
      state.chatVisible = payload;
    },
    formVisible: (state, { payload }) => {
      state.formVisible = payload;
    },
    formData: (state, { payload }) => {
      state.formData = payload;
    },
    customerFormData: (state, { payload }) => {
      state.customerFormData = payload;
    },
    botMessage: (state, { payload }) => {
      state.botMessage = payload;
    },
    uuid: (state, { payload }) => {
      state.uuid = payload;
    },
    socket: (state, { payload }) => {
      state.socket = payload;
    },
    emojiVisible: (state, { payload }) => {
      state.emojiVisible = payload;
    },
    storeIpAddress: (state, { payload }) => {
      state.storeIpAddress = payload;
    },
    deviceType: (state, { payload }) => {
      state.deviceType = payload;
    },
    callBackForm: (state, { payload }) => {
      state.callBackForm = payload;
    },
    callBackStatus: (state, { payload }) => {
      state.callBackStatus = payload;
    },
    agentNameVisible: (state, { payload }) => {
      state.agentNameVisible = payload;
    },
    apiStatus: (state, { payload }) => {
      state.apiStatus = payload;
    },
    agentDomainExt: (state, { payload }) => {
      state.agentDomainExt = payload;
    },
    agentDomainExtEmpty: (state, { payload }) => {
      state.agentDomainExt.length = 0;
    },
    transferCallCount: (state, { payload }) => {
      state.transferCallCount = payload

    },
    setConnectionType: (state, { payload }) => {
      state.connectionType = payload;
    },
    setDtmfData: (state, { payload }) => {
      state.dtmfData.push(payload);
      if (state.currentDtmfData.length === 0) {
        state.currentDtmfData = [state.dtmfData[0]];
      }
    },
    setCurrentDtmfData: (state, { payload }) => {
      state.currentDtmfData = payload;
    },
    mobileCount: (state, { payload }) => {
      state.mobileCount = payload;
    },
    setAgentConnectingStatus: (state, { payload }) => {
      state.agentConnectingStatus = payload
    },
    checkallQuesAns: (state, { payload }) => {
      state.checkallQuesAns = payload;
    },
    setCompanyDetails: (state, { payload }) => {
      state.companyId = payload;
    },
    setChatDurationId: (state, { payload }) => {
      state.chatDurationId = payload;
    },
    formDetails: (state, { payload }) => {
      state.formDetails = payload;
    },
    customerData: (state, { payload }) => {
      state.customerData = payload;
    },
    flowBreakMsg: (state, { payload }) => {
      state.flowBreakMsg = payload;
    },
    paymentEnable: (state, { payload }) => {
      state.paymentEnable = payload;
    },
    paymentSource: (state, { payload }) => {
      state.paymentSource = payload;
    },
    paymentMethod: (state, { payload }) => {
      state.paymentMethod = payload;
    },
    paymentId: (state, { payload }) => {
      state.paymentId = payload;
    },
    socketId: (state, { payload }) => {
      state.socketId = payload;
    },
    setIndustryType: (state, { payload }) => {
      state.industry_type = payload;
    },
    updateCustomContactFiels: (state, { payload }) => {
      state.customContactFiels = payload
    },
    soundNotification: (state, { payload }) => {
      state.soundNotification = payload
    },
    setIntegration: (state, { payload }) => {
      state.integration = { ...payload }
    },
    setWorktualBooking: (state, { payload }) => {
      state.worktualAppointment = { ...payload }
    },
    carouselData: (state, { payload }) => {
      state.carouselData = payload
    },
    datetimePickerNode: (state, { payload }) => {
      state.datetimePickerNode = payload
    },
    setSupervisorList: (state, { payload }) => {
      state.superVisorExt = payload
    },
    multipleTemp: (state, { payload }) => {
      state.multipleTemp = payload
    },
    remainderAction: (state, { payload }) => {
      state.remainderAction = { ...payload }
    },
    flowBreakParentnode: (state, { payload }) => {
      state.flowBreakParentnode = payload
    },
    setTriggerData: (state, { payload }) => {
      state.triggerData = payload
    },
    setIntegrationApiData: (state, { payload }) => {
      state.integrationApiData = payload
    },
    setAnsCaptureStatus: (state, { payload }) => {
      state.ansCaptureStatus = payload
    },
    setAnsCaptureKey: (state, { payload }) => {
      state.ansCaptureKey = payload
    },
    setPurchasePlanStatus: (state, { payload }) => {
      state.purchasePlanData.purchasePlanStatus = payload
    },
    setPurchasePlanUrl: (state, { payload }) => {
      state.purchasePlanData.purchasePlanUrl = payload
    },
    isConsentForm: (state, { payload }) => {
      state.isConsentForm = payload
    },
    setPurchaseBackIconStatus: (state, { payload }) => {
      state.purchaseBackIconStatus = payload;
    },
    setRecommedFlowValue: (state, { payload }) => {
      state.reminderFlowValue = payload
    },
    setRecommedFlowStatus: (state, { payload }) => {
      state.reminderFlowStatus = payload
    },
    setDomainDetails: (state, { payload }) => {
      state.domainDetails = payload
    },
    setFileInputtype: (state, { payload }) => {
      state.fileInputtype = payload
    },
    setTargetLang: (state, { payload }) => {
      state.targetLang = payload
    },
    setLiveAgent: (state, { payload }) => {
      state.liveAgent = payload
    },
    storeClientLocation: (state, { payload }) => {
      state.clientLocation = payload;
    },
    userChatInitiated: (state) => {
      state.isUserChatInitiated = true;
    },
    currentlyHandling: (state, { payload }) => {
      state.currentlyHandling = payload;
    },
    setWebtrackingTimer: (state, { payload }) => {
      state.webtrackingTimer = payload;
    },
    storeClientLocationDetails: (state, { payload }) => {
      state.clientLocationDetails = payload;
    },
    reset: () => initialState,
  },
  extraReducers: (builder) => { },
});

export const messageActions = messageSice.actions;
export default messageSice.reducer;
