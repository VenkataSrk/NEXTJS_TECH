
import { createSlice } from "@reduxjs/toolkit";
import { blistenBargeWisperTimer, CallTimer, findIndexWithCalltypeandConference, findIndexWithGuid, findIndexWithGuidIncoming, findIndexWithOutboundCampaign, forSipFailure, forUnavailable, terminateNormal, TransferCallTimer } from "../../../base/utils";
import _ from "lodash";
import { POPUP_CALLS } from "../../constants/reducer_names";

export const initialState = {
  acceptCallRoom: false,
  closeCall: false,
  callSessionDetailswhilePopup: {} as any,
  isCallAccepted: false,
  callDetails: [] as any,
  incomingCallDetail: {} as any,
  status: false,
  conversationNav: "",
  isDialPadOpen: false,
  isCalling: false,
  isTransfer: false,
  isTrackAgent: false,
  trackAgentDetail: {} as any,
  monitorType: "",
  isTransferSuccess: false,
  isMergeButton: false,
  isTransferButton: false,
  mergedCall: false,
  dialedNumber: "",
  callFlowListData: [],
  sessionList: [],
  currentSession: {},
  transfer_type: "",
  currentCallDuration: "00:00:00",
  sessionIDStore: "",
  queueName: "",
  queueDetails: {},
  opendtmf: false,
  dtmf: "",
  chatFlowListData: [],
  isCallBackWaiting: false,
  reject: false,
  callBackDetails: {},
  callBackTimer: 30,
  agentRegisterDetails: {} as any,
  callList: [],
  videoacceptCallRoom: false,
  videoagentaccept: false,
  TransferFrom: [],
  transferTo: [],
  ShowCallerDetails: false,
  topBarVaue: {
    flowName: "",
    version: [],
    status: "",
    description: "",
  } as any,
  chatTransferingData: {} as any,
  isCallTransferFailed: false,
  sessionIdClosedConvo: {} as any,
  outputDevices: [] as any,
  inputDevices: [] as any,
};

const CallPopUpAgentSlice = createSlice({
  name: POPUP_CALLS,
  initialState,
  reducers: {
    openDFMF: (state) => {
      state.opendtmf = !state.opendtmf
    },
    setDtmf: (state, { payload }) => {
      state.dtmf = payload
    },
    setWholeData: (state, { payload }) => {
      state.acceptCallRoom = payload.acceptCallRoom;
      state.callSessionDetailswhilePopup = payload.callSessionDetailswhilePopup;
      state.isCallAccepted = payload.isCallAccepted;
    },
    setCallBackInfo: (state, { payload }) => {
      state.isCallBackWaiting = payload.isCallBackWaiting;
      state.callBackDetails = payload.callBackDetails;
      state.callBackTimer = payload.callBackTimer;
      state.reject = payload.reject;
    },
    setagentRegisterDetails: (state, { payload }) => {
      state.agentRegisterDetails = payload;
    },
    setSessionId: (state, { payload }) => {
      state.sessionIDStore = payload.trim();
    },

    mergeCallDataSetUp: (state, { payload }) => {
      let data = payload;
      state.incomingCallDetail["isConference"] = true;
      state.incomingCallDetail["isCurrent"] = true;
      state.incomingCallDetail["contact"] = data.AgentDetails;
      state.incomingCallDetail["callType"] = "conCall";
      state.incomingCallDetail["AllAgentWithDomainId"] = payload.AllAgent.map((item: any) => `${payload.userData.domain_id}_${item}`);
      state.incomingCallDetail["AllAgent"] = payload.AllAgent;
      state.incomingCallDetail["muteState"] = false;
      state.incomingCallDetail["holdState"] = false;
      data.AgentDetails.filter((item: any) => {
        return item.isLogin === true;
      });
      // if (data.AgentDetails.length === 1) {
      data.AgentDetails.map((item: any) => {
        // console.log(item.isPrimary, item.isPrimary === payload.userData.ext, "item.isPrimary");
        return state.incomingCallDetail["agentType"] = item.isPrimary === payload.userData.ext ? "PRIMARY" : item.agentType
      });
      // }
      let index = _.findIndex(state.callDetails, (item: any) => {
        return item.guid === state.incomingCallDetail.guid;
      });
      if (index > -1) {
        state.callDetails[index] = state.incomingCallDetail;
      }
      state.mergedCall = true;
    },
    leaveCallDataSetUp: (state, { payload }) => {
      let setData: any = payload;
      let index = _.findIndex(state.incomingCallDetail.contact, (item: any) => {
        return item.from === setData.endBy;
      });
      // let FromIndex = _.findIndex(state.incomingCallDetail.contact, (item: any) => {
      //   return item.to === setData.endBy;
      // });
      // let isCustomer = _.findIndex(state.incomingCallDetail.contact, (item: any) => {
      //   return item.isCustomer;
      // });
      let cindex = _.findIndex(state.callDetails, (item: any) => {
        return item.guid === state.incomingCallDetail.guid;
      });
      if (index > -1) {
        state.incomingCallDetail.contact = _.filter(state.incomingCallDetail.contact, (item: any) => {
          return item.to !== setData.endBy;
        });
        state.mergedCall = state.incomingCallDetail.contact.length <= 2 ? false : true
        if (state.incomingCallDetail.contact.length <= 2) {
          state.incomingCallDetail.contactDetails = state.incomingCallDetail.contact[0].contactDetails
        }
        state.incomingCallDetail["isConference"] = state.incomingCallDetail.contact.length <= 2 ? false : true;
        state.incomingCallDetail["callType"] = state.incomingCallDetail.contact.length <= 2 ? "direct" : "conCall";
        state.incomingCallDetail["agentType"] = setData.agentType === "PRIMARY" ? "PRIMARY" : state.incomingCallDetail.agentType;
        if (cindex > -1) {
          state.callDetails[cindex] = state.incomingCallDetail;
        }
      }
    },

    leaveCallSecondaryAgent: (state, { payload }) => {
      let setData: any = payload;
      let index = _.findIndex(state.incomingCallDetail.contact, (item: any) => {
        return item.to === setData.endBy;
      });
      if (index > -1) {
        if (state.incomingCallDetail.contact.length > 3) {
          state.incomingCallDetail.contact = _.filter(state.incomingCallDetail.contact, (item: any) => {
            return item.to !== setData.endBy;
          });
        } else {
          state.incomingCallDetail.contact = _.filter(state.incomingCallDetail.contact, (item: any) => {
            return item.isCustomer
          });
          state.incomingCallDetail['typeOf'] = "EXTERNAL"
          state.incomingCallDetail["agentType"] = "PRIMARY"
          state.incomingCallDetail.contactDetails = state.incomingCallDetail.contact[0].contactDetails
          state.mergedCall = false;
          state.incomingCallDetail.contact = [];
          state.incomingCallDetail['isCustomer'] = true;
          state.incomingCallDetail['isTransfer'] = "";
        }
        state.incomingCallDetail["isConference"] = state.incomingCallDetail.contact.length <= 2 ? false : true;
        state.incomingCallDetail["callType"] = state.incomingCallDetail.contact.length <= 2 ? "direct" : "conCall";

        let cindex = _.findIndex(state.callDetails, (item: any) => {
          return item.guid === state.incomingCallDetail.guid;
        });
        if (cindex > -1) {
          state.callDetails[cindex] = state.incomingCallDetail;
        }
      }

    },

    setCurrentCallDetails: (state, { payload }) => {
      state.callDetails = payload;
    },

    pushCallDetails: (state, { payload }) => {
      if (state.callDetails.length > 0) {
        let index = findIndexWithGuid(state.callDetails, payload.guid);

        if (index > -1) {
          state.callDetails[index] = payload;
          state.incomingCallDetail = payload;
        } else {
          state.callDetails.push(payload);
          state.incomingCallDetail = payload;
          state.status = true;
        }
      } else {
        state.callDetails.push(payload);
        state.incomingCallDetail = payload;
        state.status = true;
      }
    },
    setOBCState: (state) => {
      if (state.callDetails.length > 0) {
        let index = findIndexWithOutboundCampaign(state.callDetails);
        if (index > -1) {
          state.callDetails[index] = { ...state.callDetails[index], outboundCampaign: false, callduration: "00:00:00" };
          state.incomingCallDetail = { ...state.incomingCallDetail, outboundCampaign: false, callduration: "00:00:00" };
        }
      }
    },

    pushCallForTransfer: (state, { payload }) => {
      state.callDetails.push(payload);
      state.incomingCallDetail = payload;
      state.isCalling = true;
      state.isTransfer = false;
    },

    callEnded: (state) => {
      state.callDetails = [];
      state.incomingCallDetail = {};
      state.isCalling = false;
      state.acceptCallRoom = false;
    },

    setTransferTyper: (state) => {
      //  agentType: "PRIMARY",

      let index = findIndexWithGuid(state.callDetails, state.incomingCallDetail.guid);
      if (index > -1) {
        state.callDetails[index].agentType = "PRIMARY";
        state.incomingCallDetail.agentType = "PRIMARY";
        state.incomingCallDetail.callTransfered = "1"
      }
    },

    forMergeBye: (state, { payload }) => {
      state.mergedCall = true;
      let detailIndex = forSipFailure(state.callDetails, payload);
      if (detailIndex > -1) {
        if (state.callDetails.length > 1) {
          state.callDetails.splice(detailIndex, 1);
          state.isCalling = true;
        } else {
          state.callDetails = [];
          state.incomingCallDetail = {};
          state.isCalling = false;
          state.dialedNumber = "";
          state.queueDetails = {};
          state.queueName = "";
        }
      }
    },
    cleartimer: (state) => {
      state.currentCallDuration = "00:00:00";
    },

    forSipFailiure: (state, { payload }) => {
      if (state.callDetails.length > 0) {
        let detailIndex = forSipFailure(state.callDetails, payload);
        if (detailIndex > -1) {
          if (state.callDetails.length === 1) {
            state.callDetails = [];
            state.incomingCallDetail = {};
            state.isCalling = false;
            state.dialedNumber = "";
            state.queueDetails = {};
            state.queueName = "";
            state.mergedCall = false;
          } else {
            state.callDetails = state.callDetails.splice(detailIndex, 1);
            state.incomingCallDetail = state.callDetails[state.callDetails.length - 1];
            state.isCalling = true;
            state.dialedNumber = "";
            state.queueDetails = {};
            state.queueName = "";

            // state.mergedCall=state.callDetails.length>1 && state.incomingCallDetail.isConference?true:false;
          }
        }
      } else {
        state.callDetails = [];
        state.incomingCallDetail = {};
        state.isCalling = false;
        state.dialedNumber = "";
        state.queueDetails = {};
        state.queueName = "";
      }
    },

    terminateNormal: (state, { payload }) => {
      let detailIndex = terminateNormal(state.callDetails, payload);

      if (detailIndex > -1) {
        if (state.callDetails.length > 1) {
          if (state.callDetails[detailIndex].typeOf === "EXTERNAL") {
            state.currentCallDuration = state.callDetails[detailIndex].callduration;
            state.callDetails = [];
            state.incomingCallDetail = {};
            state.status = false;
            state.isCalling = false;
            state.isMergeButton = false;
            state.isTransferButton = false;
            state.isDialPadOpen = false;
            state.dialedNumber = "";
            state.queueDetails = {};
            state.queueName = ""
          } else {
            state.callDetails.splice(detailIndex, 1);
            state.callDetails[state.callDetails.length - 1].holdState = false;
            state.incomingCallDetail = state.callDetails[state.callDetails.length - 1];
            state.status = true;
            state.isCalling = true;
            state.isMergeButton = false;
            state.isTransferButton = false;
            state.dialedNumber = "";
            state.queueDetails = {};
            state.queueName = ""
          }
        } else {
          state.currentCallDuration = state.callDetails[state.callDetails.length - 1].callduration;
          state.isDialPadOpen = false;
          state.callDetails.splice(detailIndex, 1);
          state.incomingCallDetail = {};
          state.isCalling = false;
          state.dialedNumber = "";
          state.queueDetails = {};
          state.queueName = "";
          state.isMergeButton = false;
          state.isTransferButton = false;
        }
      }
    },

    setIsCurrent: (state) => {
      state.callDetails.map((call: any) => call.isCurrent = false);
    },

    clearCallForBusy: (state) => {
      state.callDetails = [];
      state.incomingCallDetail = {};
      state.isCalling = false;
      state.status = false;
    },

    clearCallForBusyforMore: (state) => {
      let detailIndex = terminateNormal(state.callDetails, state.incomingCallDetail.from);
      if (detailIndex > -1) {
        state.callDetails.splice(detailIndex, 1);
        state.incomingCallDetail = state.callDetails[state.callDetails.length - 1];
        state.isCalling = state.callDetails.length > 0 ? true : false;
        state.status = state.callDetails.length > 0 ? true : false;
      }
    },

    forUnavailable: (state) => {
      if (state.callDetails.length === 1) {
        let detailIndex = forUnavailable(state.callDetails);
        if (detailIndex > -1) {
          state.callDetails.splice(detailIndex, 1);
          state.incomingCallDetail = {};
          state.isCalling = false;
          state.isDialPadOpen = state.isDialPadOpen ? true : false;
          state.dialedNumber = "";
          state.queueDetails = {};
          state.queueName = "";
        }
      } else {
        let detailIndex = findIndexWithGuidIncoming(state.callDetails, state.incomingCallDetail);
        if (detailIndex > -1) {
          state.callDetails.splice(detailIndex, 1);
          state.callDetails[state.callDetails.length - 1].holdState = false;
          state.incomingCallDetail = state.callDetails[state.callDetails.length - 1];
          state.isCalling = true;
          state.isDialPadOpen = false;
          state.dialedNumber = "";
          state.queueDetails = {};
          state.queueName = "";
        }
      }
    },

    setSpliceCallDetails: (state, { payload }) => {
      let index = findIndexWithGuid(state.callDetails, payload);
      if (index > -1) {
        state.callDetails.splice(index, 1);
        if (state.callDetails.length >= 1) {
          state.callDetails[state.callDetails.length - 1].holdState = false;
          state.incomingCallDetail = state.callDetails[state.callDetails.length - 1];
          state.status = true;
          state.isCalling = true;
          state.dialedNumber = "";
          state.queueName = "";
          state.queueDetails = {};
        } else {
          state.callDetails = [];
          state.incomingCallDetail = {};
          state.status = false;
          state.isDialPadOpen = true;
          state.isCalling = false;
          state.dialedNumber = "";
          state.queueName = "";
          state.queueDetails = {};
        }
      } else {
        state.callDetails = [];
        state.incomingCallDetail = {};
        state.status = false;
      }
    },

    setSpliceDirectDetails: (state) => {
      let index = findIndexWithCalltypeandConference(state.callDetails);
      if (index > -1) {
        state.callDetails.splice(index, 1);
        if (state.callDetails.length > 0) {
          state.callDetails[state.callDetails.length - 1].holdState = false;
          state.incomingCallDetail = state.callDetails[state.callDetails.length - 1];
          state.status = true;
          state.isDialPadOpen = true;
          state.isCalling = true;
        } else {
          state.incomingCallDetail = {};
          state.status = false;
          state.isDialPadOpen = true;
          state.isCalling = false;
        }
      } else {
        state.callDetails = [];
        state.incomingCallDetail = {};
        state.status = false;
      }
    },

    setIncomingCallDetails: (state, { payload }) => {
      state.incomingCallDetail = payload.incomingCallDetail;
      state.status = payload.status;
    },
    setCallDetailsMuteState: (state, { payload }) => {
      let index = findIndexWithGuid(state.callDetails, payload);
      if (index > -1) {
        state.callDetails[index].muteState = true
        state.incomingCallDetail.muteState = true
      }
    },
    setCallDetailsUnMuteState: (state, { payload }) => {
      let index = findIndexWithGuid(state.callDetails, payload);
      if (index > -1) {
        state.callDetails[index].muteState = false;
        state.incomingCallDetail.muteState = false;
      }
    },
    setCallDetailsHoldState: (state, { payload }) => {
      let index = findIndexWithGuid(state.callDetails, payload);
      if (index > -1) {
        state.callDetails[index].holdState = !state.callDetails[index].holdState;
        state.incomingCallDetail.holdState = !state.incomingCallDetail.holdState;
      }
    },
    setCallDetailsUnHoldState: (state, { payload }) => {
      let index = findIndexWithGuid(state.callDetails, payload);
      if (index > -1) {
        state.callDetails[index].holdState = !state.callDetails[index].holdState;
        state.incomingCallDetail.holdState = !state.incomingCallDetail.holdState;
        // state.callDetails[index].muteState = false;
        // state.incomingCallDetail.muteState = false;
      }
    },
    updateContactInPopup: (state, { payload }) => {
      let index = findIndexWithGuid(state.callDetails, state.incomingCallDetail.guid);
      if (index > -1) {
        state.callDetails[index].contactDetails = payload;
        state.incomingCallDetail.contactDetails = payload;
      }
    },
    updateCallState: (state, { payload }) => {
      let index = findIndexWithGuid(state.callDetails, state.incomingCallDetail.guid);
      if (index > -1) {
        state.callDetails[index].callStatus = payload;
        state.incomingCallDetail.callStatus = payload;
      }
    },
    updateIncomingCallState: (state, { payload }) => {
      state.incomingCallDetail.callStatus = payload;
    },

    agentTrackTimer: (state) => {
      blistenBargeWisperTimer(state);
    },

    // setInitialTime: (state: any) => {
    //   let index = helper.findIndexWithGuid(state.callDetails, state.incomingCallDetail.guid);
    //   state.incomingCallDetail.callduration = "00:00:00";
    //   state.callDetails[0].callduration = "00:00:00";
    // },
    setInitialTime: (state: any) => {
      let index = findIndexWithGuid(state.callDetails, state.incomingCallDetail?.guid);
      if (state.incomingCallDetail) {
        state.incomingCallDetail.callduration = "00:00:00";
      }
      if (state.callDetails && state.callDetails.length > 0) {
        state.callDetails[0].callduration = "00:00:00";
      }
    },
    updateCallTimer: (state) => {
      let index =findIndexWithGuid(state.callDetails, state.incomingCallDetail.guid);

      if (index > -1) {
        CallTimer(state, index);
      }
    },

    timerForTransferAgent: (state) => {
      let index = findIndexWithGuid(state.callDetails, state.incomingCallDetail.guid);
      if (index > -1) {
        TransferCallTimer(state, index);
      }
    },

    openCalledDialPad: (state, { payload }) => {
      state.isCalling = payload.isCalling;
      state.isMergeButton = payload.isMergeButton;
      state.isTransferButton = payload.isTransferButton;
    },
    closeMergeButton: (state) => {
      state.isMergeButton = false
      state.isTransferButton = false
    },
    closemergedCall: (state) => {
      state.mergedCall = false
    },
    TransferSuccess: (state) => {
      state.isTransferSuccess = !state.isTransferSuccess;
      state.isCalling = false;
      state.isMergeButton = false;
      state.isTransferButton = false;
      state.isDialPadOpen = false;
      state.isTransfer = false;
      state.mergedCall = false;
    },
    chatTransferSuccess: (state) => {
      state.isTransferSuccess = false;
      state.isCalling = false;
      state.isMergeButton = false;
      state.isTransferButton = false;
      state.isDialPadOpen = false;
      state.isTransfer = false;
    },

    endForAll: (state) => {
      state.isTransferSuccess = false;
      state.isCalling = false;
      state.isMergeButton = false;
      state.isTransferButton = false;
      state.isDialPadOpen = false;
      state.isTransfer = false;
      state.mergedCall = true;
    },

    DialPadChanges: (state, { payload }) => {
      state.isDialPadOpen = payload.isDialPadOpen;
      state.isCalling = payload.isCalling;
      state.isTransfer = payload.isTransfer;
      state.isTransferSuccess = payload.isTransferSuccess;
      state.isMergeButton = payload.isMergeButton;
      state.isTransferButton = payload.isTransferButton;
    },

    mergeCallChanges: (state, { payload }) => {
      state.mergedCall = payload;
    },

    TrackChanges: (state, { payload }) => {
      state.isTrackAgent = payload.isTrackAgent;
      state.trackAgentDetail = payload.trackAgentDetail;
    },

    muteTrackAgent: (state, { payload }) => {
      state.trackAgentDetail.muteState = payload;
    },

    TrackStart: (state) => {
      state.isTrackAgent = !state.isTrackAgent;
    },

    TrackDataUpdate: (state) => {
      state.trackAgentDetail.status = "connected";
    },
    MonitorType: (state, { payload }) => {
      state.monitorType = payload;
    },

    setDialPadNumber: (state, { payload }) => {
      state.dialedNumber = state.dialedNumber.concat(payload.toString());
    },

    setNumber: (state, { payload }) => {
      state.dialedNumber = payload;
    },
    setNumberfromContact: (state, { payload }) => {
      state.dialedNumber = payload;
      state.isDialPadOpen = true;
    },
    removeDialPadNumber: (state) => {
      state.dialedNumber = state.dialedNumber.slice(0, state.dialedNumber.length - 1);
    },

    setDialPadOnKeyPressUP: (state, { payload }) => {
      state.dialedNumber = state.dialedNumber.concat(payload);
    },

    backSpace: (state) => {
      state.dialedNumber = state.dialedNumber.substring(0, state.dialedNumber.length - 1);
    },

    setSessionList: (state, { payload }) => {
      state.sessionList = payload;
    },

    setCurrentSession: (state, { payload }) => {
      state.currentSession = payload;
    },

    updateTransferType: (state, { payload }) => {
      state.transfer_type = payload.transfer_type;
    },

    updateTopBar: (state, { payload }) => {
      state.topBarVaue = payload
    },
    updateCallflowListData: (state, { payload }) => {
      state.callFlowListData = payload.callFlowListData;
    },

    updateChatflowListData: (state, { payload }) => {
      state.chatFlowListData = payload.chatFlowListData;
    },

    setCalling: (state) => {
      state.isCalling = !state.isCalling;
    },
    setCallingval: (state, { payload }) => {
      state.isCalling = payload;
    },
    setMergerTransfer: (state) => {
      state.isMergeButton = false;
      state.isTransferButton = false;
    },
    NotAttendedCall: (state) => {
      state.acceptCallRoom = false;
      state.callSessionDetailswhilePopup = {};
      // state.isCallAccepted = state.isCallAccepted;
    },
    setCloseCall: (state, { payload }) => {
      state.closeCall = payload;
    },
    isCallAccepted: (state) => {
      state.isCallAccepted = false
    },
    setIsTranfer: (state) => {
      state.isTransfer = !state.isTransfer;
      state.isCalling = true;
      state.isDialPadOpen = false;
    },
    setActivityIsTranfer: (state) => {
      state.isTransfer = !state.isTransfer;
      state.isCalling = false;
      state.isDialPadOpen = false;
    },

    closeTransfer: (state) => {
      state.isTransfer = false;
      state.isCalling = false;
      state.isDialPadOpen = false;
    },

    closeDialPad: (state) => {
      state.isDialPadOpen = !state.isDialPadOpen;
    },
    closeDialPadOnTopBar: (state) => {
      // alert()
      state.isDialPadOpen = false;
    },


    closeDialPadForAll: (state) => {
      state.isDialPadOpen = false;
    },
    setDialPad: (state) => {
      state.isDialPadOpen = true;
      state.isCalling = true;

    },

    updatecurrentState: (state, { payload }) => {
      let index =findIndexWithGuid(state.callDetails, payload);
      if (index > -1) {
        state.callDetails[index].isCurrent = false;
        state.callDetails[index].holdState = true;
      }
    },
    removeLastCallDetails: (state) => {
      //  state.callDetails.pop()
      if (state.callDetails.length === 1) {
        state.isCallAccepted = true
        state.isCalling = false
        state.status = true
        // state.incomingCallDetail.useragent = state.callDetails?.[0].useragent
        state.incomingCallDetail = state.callDetails?.[0]
      }
    },
    setEndTransferData: (state) => {
      state.incomingCallDetail = state.callDetails?.[0]
    },
    removeCallDetails: (state, { payload }) => {
      state.callDetails = state.callDetails.filter((i: any) => payload.guid !== i.guid)
      state.incomingCallDetail.holdState = false
    },
    setScondaryagent: (state, { payload }) => {
      const hang = state.callDetails.filter((i: any) => payload.guid !== i.guid)
      state.callDetails = hang
      state.incomingCallDetail.holdState = false
      if (hang.length === 1) {
        state.incomingCallDetail = hang[0]
      }
    },
    setQueueName: (state, { payload }) => {
      state.queueName = payload.value;
      state.queueDetails = payload;
    },

    clearDialedNumber: (state) => {
      state.dialedNumber = "";
    },
    callList: (state, { payload }) => {
      state.callList = payload.length > 0 ? payload : []
    },
    videoAttendedCall: (state, { payload }) => {
      state.videoacceptCallRoom = payload.videoacceptCallRoom;
    },
    VideoCallAgentStatus: (state, { payload }) => {
      state.videoagentaccept = payload.videoagentaccept;
    },
    setTransferFrom: (state, { payload }) => {
      state.TransferFrom = payload
    },
    setTransferto: (state, { payload }) => {
      state.transferTo = payload;
    },
    //  setTransferkey :(state,{payload})=>{
    //   state.transferTo.transferkey=payload;
    //  state.transferTo.transfertime = Date.now()

    // },  
    setTransferReset: (state) => {
      state.transferTo = []

    },
    setShowCallerDetails: (state, { payload }) => {
      state.ShowCallerDetails = payload
    },
    removePstnCallerId: (state, { payload }) => {
      if (state.incomingCallDetail?.["contact"] && state.incomingCallDetail?.["contact"].length > 0) {
        const filterContactList = state.incomingCallDetail["contact"].filter((i: any) => { return i?.to !== payload?.participant_left })
        state.incomingCallDetail["contact"] = filterContactList
        state.callDetails[0]["contact"] = filterContactList
      }
    },
    chatTransferingpopup: (state, { payload }) => {
      // console.log('payloadpayloadpayloadpayload',payload)
      state.chatTransferingData = payload
    },
    CallTransferingpopup: (state, { payload }) => {
      state.isCallTransferFailed = payload
    },
    clearIncomingCallDetail: (state) => {
      state.incomingCallDetail = {}
      state.callDetails = []
      state.isCalling = false;
      state.acceptCallRoom = false;
    },
    setsessionIdClosedConvo: (state, { payload }) => {
      state.sessionIdClosedConvo = payload;
    },

    // setChangeToSpeaker: (state, { payload }) => {
    //   let index = helper.findIndexWithGuid(state.callDetails, payload.id);
    //   if (index > -1) {
    //     state.callDetails[index].speaker = payload.speaker;
    //     state.incomingCallDetail.speaker = payload.speaker;
    //   }
    // },
    setAudioOutputDevices: (state, { payload }) => {
      state.outputDevices = payload
    },

    setAudioInputDevices: (state, { payload }) => {
      state.inputDevices = payload
    },

    reset: () => { },
  }
});

export const CallPopUpAgentActions = CallPopUpAgentSlice.actions;
export default CallPopUpAgentSlice.reducer;
