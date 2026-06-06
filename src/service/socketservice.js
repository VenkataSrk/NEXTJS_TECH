import io from "socket.io-client";
import store from "../store/redux-store/root_store";
import { messageActions } from "../store/slice/message_slice";
import { liveTrackerActions } from "../store/slice/live_tracker";
import {
  agentDomainStatus, storeSupervisorExt, getDispostionRes, getLiveTrackerData,
  receiveChatMessage, chatResponse, initiateBot, availableagent, requestAccept,
  requestDecline, getqueueDetails, getChatDurationId, agentDisconnect,
  initgoStream, socketSession, isJSON, setAgentTypingStatus, newChatSendMsg, getBrowserKey
} from "../chatbot/utils";
import { removeUserFromQueue, changeFallBackQueue, getCookie } from '../chatbot/utils2.0';
import { v4 as uuidv4 } from "uuid";
import { config } from "../base/constants";
import { areMessagesSame, nlpConvoSummary } from "../chatbot/utils"

let socket = null

export const initializeSocket = () => {
  const liveTracker = store.getState().liveTracker;
  if (socket && socket.connected) {
    console.log("Socket already connected:", socket.id);
    return socket;
  }

  if (liveTracker?.socketData?.connected) {
    console.log("Redux socket already connected:", liveTracker.socketData?.id);
    return liveTracker.socketData;
  }

  let sessionId = sessionStorage.getItem("sessionUniqueId");
  if (!sessionId) {
    sessionId = `${new Date().getTime()}`;
    sessionStorage.setItem("sessionUniqueId", sessionId);
  }

  store.dispatch(messageActions.uuid(sessionId));
  store.dispatch(liveTrackerActions.uuid(sessionId));
  console.log("Socket Service calling", sessionId);


  socket = io(config().BASE_URL, {
    // socket = io("http://localhost:5008", {
    path: "/ccascall-service",
    auth: {
      token: "",
      id: sessionId,
    },
  });
  socket.on("connect", () => {
    console.log("Connected:", socket.id);
    store.dispatch(liveTrackerActions.setSocketData(socket));
    socketSession(sessionId);
  });

  socket.on("userStatusUpdated", data => console.log("User status updated:", data));
  socket.on("waitingQueueUpdate", data => console.log("Waiting queue update:", data));
  socket.on("get_queue_details", getqueueDetails);
  socket.on("chat_response", message => {
    store.dispatch(messageActions.lottieProcess(false));
    chatResponse(message);
  });
  socket.on("OneToOneChatReceive", message => {
    const receiveId = sessionId;
    if (receiveId === message?.from || receiveId === message?.to || receiveId === message?.sessionId) {
      receiveChatMessage(message);
    }
  });
  socket.on("agentDisconnected", agentDisconnect);
  socket.on("airesponse", (message) => {
    const messageList = store.getState().message;
    const { domainId, chatFlowId } = store.getState().message?.personalisationAll;
    let chatHistoryId = sessionStorage.getItem("ChatHistoryId");
    const storeIntentVal = messageList?.questionAnswerList?.map((elem) => JSON.parse(elem?.intent))?.flat();

    if (message?.action === "others" && !message?.agent_ext) {
      store.dispatch(messageActions.setLiveAgent(''));
      let connectionData = {
        isBotConnection: true,
        isAgentConnection: false,
        isDTMFConnection: false,
      };
      store.dispatch(messageActions.setConnectionType(connectionData));
      store.dispatch(messageActions.setAgentConnectingStatus(false))
    }
    
    if (message?.stream_finished === 0 && message?.trigger_form !== true && !messageList?.simOrderSts) {
      initgoStream(message);
    } else {
      if (message?.action === "Live_Agent" && message?.agent_ext !== '') {
        store.dispatch(messageActions.setLiveAgent(message?.agent_ext));
      }
      store.dispatch(messageActions.setTargetLang(message?.language));
      store.dispatch(liveTrackerActions.setSendMessageStatus(true));
      initiateBot(message);
      if(message?.action === "Live_Agent") {
        nlpConvoSummary();
      }
    }
    if (isJSON(message?.ans_intent) && storeIntentVal.includes(JSON.parse(message?.ans_intent)[0]) && message?.stream_finished === 1) {
      const data = {
        IntentName: JSON.parse(message.ans_intent)[0],
        IntentCount: 1,
        domainId: domainId,
        ChatFlowId: chatFlowId,
        ChatHistoryId: chatHistoryId
      }
      socket.emit("chatIntentEvent", data);
    }
  });

  socket.on('supervisor_ext_response', (data) => {
    storeSupervisorExt(data)
  })

  socket.on("availableagent", (data) => {
    const liveTrackerList = store.getState().liveTracker;
    if (liveTrackerList?.agentSessionAvaiableStatus === true) {
      store.dispatch(messageActions.lottieProcess(false));
      availableagent(data);
    } else {
      if (!liveTrackerList?.restrictedEndChat) {
        let data = {
          isBotConnection: true,
          isAgentConnection: false,
          isDTMFConnection: false,
        };
        store.dispatch(messageActions.templateKeywordList([]));
        store.dispatch(messageActions.setConnectionType(data));
        sessionStorage.setItem("isConnected", "bot");
      }
      newChatSendMsg("Text", "Our agent is currently unavailable. Feel free to continue with our chatbot for instant assistance", "OUT", 1);
    }
  })

  socket.on("requestAccept", (message) => {
    requestAccept(message);
  });
  socket.on("requestDecline", (message) => {
    const state = store.getState();
    const messageList = state.message;
    const liveTrackerList = state.liveTracker;
    const cookieKey = getBrowserKey();
    const payload = {
      question: "",
      sessionId: sessionStorage?.sessionUniqueId,
      bot_id: 2,
      botType: "convo",
      domainId: messageList?.personalisationAll?.domainId,
      websiteId: "",
      ipaddress: messageList?.storeIpAddress,
      company_name: messageList?.personalisationAll?.title || "",
      isClose: 0,
      do_stream: true,
      socketId: messageList?.socketId,
      industry_type: messageList?.industry_type,
      chatId:
        parseInt(messageList?.personalisationAll?.domainId) === 17820
          ? 346954
          : messageList?.personalisationAll?.chatFlowId,
      isTemplate: 0,
      language: liveTrackerList?.selectedTextLanguage?.code,
      worktualCookieStatus: 0,
      webchatUnquieId: messageList?.personalisationAll?.webchatid,
      clientLocation: messageList?.clientLocation,
      chatHistoryId: parseInt(sessionStorage?.ChatHistoryId ?? 0),
      ChatDurationHistoryId: parseInt(sessionStorage?.ChatDurationHistoryId ?? 0),
      customerId: messageList?.customerFormData?.coid ?? 0
    };
    const updatedMessage = { ...message, ...payload, intent: "otherAgent" };
    console.log("📤 Sending Decline Payload:", updatedMessage);
    socket.emit("send_chatbot_req", JSON.stringify({ ...updatedMessage, cookieKey }));
    store.dispatch(messageActions.setLiveAgent(''));
  });
  socket.on('agentTypingStatusListener', (message) => {
    setAgentTypingStatus(message)
  })

  socket.on("botConnectedResponse", (message) => {
    getChatDurationId(message);
  });

  socket.on('get_domain_dispostionId', (result) => {
    getDispostionRes(result)
  })

  socket.on("getLiveTrackStatus", () => {
    getLiveTrackerData()
  });
  socket.on('responseAgentStatus', (message) => {
    agentDomainStatus(message)
  })
  socket.on('paymentstatus', (message) => {
    // paymentStatus(message);

  })

  socket.on('chatQueuePosition', (data) => {
    store.dispatch(liveTrackerActions.setQueuePositionData(data));
  })

  socket.on('changeFallBackQueue', (data) => {
    changeFallBackQueue(data)
  })

  return socket;
};

export const emitMessage = (channelname, payload) => {
  socket.emit(channelname, payload)
}
export const updateChatSession = (sessionId) => {
  socket.emit("updateChatSession", {
    sessionId: sessionId
  });
}
export const joinWebChat = (data) => {
  socket.emit("joinWebChat", data);
}
export const storeCustomerData = (data) => {
  socket.emit("store_customer_data", data);
}

export const sendB2cSearchMessage = (data) => {
  socket.emit("send_b2c_search_message", data);
}

export const botConnectedTime = (data) => {

  socket.emit("botConnectedTime", data);
}
export const chatMessageUpdate = (data) => {
  const messageList = store.getState().message;
  if (messageList?.customerBlockStatus !== 1) {
    if (data?.btnValue !== '' && data?.btnValue !== null) {
      store.dispatch(messageActions.currentTranferBlock(data?.btnValue))
    }
    store.dispatch(messageActions.templateKeywordList([]))
    socket.emit("chat_message_update", JSON.stringify(data));
    // store.dispatch(messageActions.lottieProcess(true));
    if (data?.btnValue && data?.btnValue.includes('transferToQueue')) {
      removeUserFromQueue(data?.btnValue);
    }
  }
}

export const jointoLiveAgent = (data) => {
  socket.emit("join_to_live_agent", data);
}

export const oneToOneChatSend = (data) => {
  const messageList = store.getState().message;
  const cookieKey = getBrowserKey();
  const payload = {
    ...data,
    ipAddress: messageList?.storeIpAddress,
    cookieKey,
  }
  socket.emit("OneToOneChatSend", payload);
}
export const worktualPurchase = (data) => {
  socket.emit("worktualPurchase", JSON.stringify(data));
}
export const oneToOneChatSendTyping = (data) => {
  socket.emit("OneToOneChatSendTyping", data);
}
export const sendChatBotReq = (data) => {
  const messageList = store.getState().message;
  if (messageList?.customerBlockStatus !== 1) {
    const cookieKey = getBrowserKey();
    socket.emit("send_chatbot_req", JSON.stringify({ ...data, cookieKey }));
    store.dispatch(messageActions.templateKeywordList([]));
    // if ((data?.question !== 'Are we still Connected!!' || data?.question !== messageList?.automatedResponse?.chatOtherInactivityMsg) && data?.worktualCookieStatus !== 1 && data?.messageType !== 'file')
    //   store.dispatch(liveTrackerActions.setSendMessageStatus(false));
    if(
      !(
        areMessagesSame(data?.question, 'Are we still Connected') 
        || areMessagesSame(data?.question, messageList?.automatedResponse?.chatOtherInactivityMsg)
        || data?.isClose === 1
      )
      && data?.worktualCookieStatus !== 1 && data?.messageType !== 'file'
    ){
      store.dispatch(liveTrackerActions.setSendMessageStatus(false));
    }
  }
};

export const updateThumbsUp = (data) => {
  socket.emit("updateThumbsUp", data);
}

export const botDisconnectedTime = (data) => {
  socket.emit("botDisconnectTime", data);
}

export const visitorCloseTab = (data) => {
  socket.emit("visitorCloseTabSite", data);
}

export const siteTracking = (data) => {
  socket.emit('liveTracking', data)
}

export const dummyEmitter = () => {
  socket.emit('refershLiveTracker')
}

export const getAgentStatus = (data) => {
  socket.emit('requestAgentStatus', data)
}

export const getSupervisorExt = (data) => {
  socket.emit('getSupervisorExt', data)
}

export const leadCrmContact = (data) => {
  socket.emit('lead_contact_update', data)
}
export const getunresponsiveQA = (data) => {
  socket.emit('unResponsiveQuesAns', data)
}
export const sendEmailTranscript = (data) => {
  socket.emit('sendEmailTranscript', data)
}

export const updateCallbackData = (data) => {
  socket.emit('update_callback_data', data)
}

export const dashboardWebChatCallBack = (data) => {
  socket.emit('dashboard_chat_callback', data)
}

export const updateEndSession = (data) => {
  socket.emit('update_end_session', data)
}

export const missedCallDashboard = (data) => {
  socket.emit('missed_call_dashboard', data)
}

export const abandonedChatDashboard = (data) => {
  socket.emit('abandoned_chat_dashboard', data)
}

export const inboundChatDashboard = (data) => {
  socket.emit('inbound_chat_dashboard', data)
}

export const sendVisitorInsightsData = (data) => {
  socket.emit('send_visitor_insights_nlp', data)
}

export const disconnectSocket = () => {
  if (socket && socket.connected) {
    store.dispatch(liveTrackerActions.setSocketData(null));
    socket.disconnect();
    console.log("Socket disconnected successfully.");
  } else {
    console.warn("Socket is not connected or already disconnected.");
  }
};

export default initializeSocket;