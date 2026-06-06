
import { v4 as uuidv4 } from "uuid";
import { messageActions } from "../store/slice/message_slice";
import store from "../store/redux-store/root_store";
import { config, worktulDomain, ponpureDomain, ponpureDomainAI } from "../base/constants";
import moment from "moment";
import { findCountry, findAllLanguage } from "../base/helpers";
import { getAgentStatus, updateCallbackData, siteTracking, oneToOneChatSend, chatMessageUpdate, getunresponsiveQA, updateChatSession, jointoLiveAgent, botDisconnectedTime, storeCustomerData, sendChatBotReq, leadCrmContact, missedCallDashboard, sendB2cSearchMessage } from "../service/socketservice"
import { chatBuilderActions } from "../store/slice/builder_slice";
import noImage from '../images/no_image.jpg';
import { getIndexDbItem } from '../indexedDb/index';
import { createVisitorsInsights, clearSessionStorage, resendOtpGenerator, agentDeclineChatNlpIntent } from './utils2.0';
import { liveTrackerActions } from "../store/slice/live_tracker";
import CryptoJS from 'crypto-js';
import { checkConvoSupported } from "./utils2.0";
// Send message to backend

export const newChatSendMsg = (type, datadata, dir, likeStatus, agentCustomerData = { agentName: 'Bot'}) => {
  try {
    if (dir === 'AGENTINTRO' && !datadata) return;
    const messageList = store.getState().message;
    const liveTrackerList = store.getState().liveTracker
    let messageToSend = datadata;
    const checkEmpty = type !== "file" && type !== "Article" ? messageToSend?.replace(/<[^>]*>/g, "").trim().length > 0 ? true : false : true;
    if (checkEmpty) {
      if (type === "file") {
        messageToSend += `\n${datadata.name} (${Math.round(datadata.size / 1024)} KB)`;
      }
      let msgData = {
        type: "text",
        data: messageToSend,
        link: ""
      };
      try {
        let typeRes = !checkConvoSupported() ? "webcrawl" : "assist";
        let site = JSON.parse(messageList?.personalisationAll?.websiteId);
        const payloads = {
          messageID: uuidv4(),
          from: messageList.uuid,
          to: messageList?.connectionType?.isBotConnection ? messageList.uuid : sessionStorage?.agentToAddress,
          sessionId: messageList.uuid,
          message: type === "file" ? datadata : JSON.stringify(msgData),
          sendAt: Date.now(),
          messageType: type,
          pdfurl: type === "file" ? datadata?.url : "",
          fileType: type === "file" ? datadata?.mimetype : "",
          isBot: dir === "IN" ? false : true,
          isCustomer: dir === "IN" ? true : false,
          sender: '',
          isLike: 0,
          type: typeRes,
          websiteId: Array.isArray(site) && site.length > 0 ? site[0] : site,
          domainId: messageList?.personalisationAll?.domainId,
          isLiveagent: messageList?.connectionType?.isAgentConnection ? true : false,
          direction: dir,
          ChatHistoryId: parseInt(sessionStorage?.ChatHistoryId),
          ChatDurationHistoryId: parseInt(sessionStorage?.ChatDurationHistoryId),
          socketId: messageList?.socketId,
          assistType: checkConvoSupported() ? "Convo" : "Template",
          supervisorList: messageList?.superVisorExt,
          language: liveTrackerList?.selectedTextLanguage?.code,
          agentIntro: dir === 'AGENTINTRO' ? true : false,
          likeDislikeStatus: likeStatus === 1 ? true : false,
          planId: messageList?.personalisationAll?.plan_id,
          transferFrom: dir === 'AGENTINTRO' ? messageList?.currentlyHandling || { agentName: "Bot" } : null
        };
        let text = {
          message: payloads,
        };
        oneToOneChatSend(payloads);
        if (dir === "IN") {
          store.dispatch(messageActions.setMessageChat({ userText: text }));
        } else if (dir === "AGENT" || dir === "AGENTINTRO") {
          store.dispatch(messageActions.setMessageChat({ agentText: text }));
          store.dispatch(messageActions.currentlyHandling(agentCustomerData));
        }
        if (messageList?.callBackFormStatus === 1) {
          setTimeout(() => {
            nlpConvoSummary()
            store.dispatch(messageActions.sessionClosedStatus(true));
            clearSessionStorage();
          }, 1000);
        }
      } catch (err) {
        console.log(err, "error");
      }
    } 
  } catch (error) {
    console.log(error)
  }
}


const arraysAreEqual = (arr1, arr2) => arr1.length === arr2.length && arr1.every((value, index) => value === arr2[index]);

// Check the Indent match with aiResponse Intent

export const checkQuesAnswer = (status) => {
  try {
    const messageList = store.getState().message;
    let queData = messageList?.questionAnswerList;
    queData?.length > 0 && queData.map((list) => {
      var result = typeof list?.intent === 'string' ? arraysAreEqual(JSON.parse(list?.intent), JSON.parse(status?.ans_intent)) : [];
      if (result) {
        store.dispatch(messageActions.checkallQuesAns(list))
      }
    });
  } catch (err) {
    console.log(err);
  }
}

// Capture the user intity from aiResponse UserInputIntity

export const multipleIntity = (data) => {
  const messageList = store.getState().message;
  const builderList = store.getState().chatBuilder;
  let queData = data?.User_input_Entity;
  var inputData = []; var ipData = []; var index = -1;
  if (queData?.first_name[0]?.length > 0) {
    inputData.push({ first_name: queData?.first_name[0],firstName: queData?.first_name[0] })
  } else if (queData?.email[0]?.length > 0) {
    inputData.push({ email: queData?.email[0] , primaryEmail: queData?.email[0] })
  } else if (queData?.address[0]?.length > 0) {
    inputData.push({ address: queData?.address[0] })
    ipData.push({ ip_address: messageList?.storeIpAddress })
  } else if (queData?.mobile_number[0]?.length > 0) {
    var str = queData?.mobile_number[0]?.slice(0, 2);
    var mob = str === "07" ? queData?.mobile_number[0]?.replace(str.charAt(0), "44") :
      str === "00" ? queData?.mobile_number[0]?.replace(str, "") : str === "+4" ? queData?.mobile_number[0]?.replace(str, "4") : queData?.mobile_number[0]
    inputData.push({ mobile_no: mob, primaryPhoneNumber: mob });

    index = messageList?.inputEntity?.findIndex(obj => {
      return obj?.mobile_no;
    });
  } else if (queData?.Sim_count[0]?.length > 0 && queData?.Sim_count[0] < 4) {
    inputData.push({ Sim_count: queData?.Sim_count[0] })
    ipData.push({ ip_address: messageList?.storeIpAddress })
  } else if (queData?.last_name[0]?.length > 0) {
    inputData.push({ last_name: queData?.last_name[0], first_name: queData?.last_name[0] })
  } else if (queData?.postcode[0]?.length > 0) {
    inputData.push({ postcode: queData?.postcode[0] })
  } else if (queData?.otp_code[0]?.length > 0) {
    inputData.push({ otp: queData?.otp_code[0] })
  } else if (queData?.plan_prices[0]?.length > 0) {
    inputData.push({ amount: queData?.plan_prices[0] })
  } else if (queData?.order_id[0]?.length > 0) {
    inputData.push({ order_id: queData?.order_id[0] })
  } else if (queData?.country[0]?.length > 0) {
    inputData.push({ country: queData?.country[0] })
  } else if (queData?.organisation[0]?.length > 0) {
    inputData.push({ organisation: queData?.organisation[0] })
  } else if (queData?.city[0]?.length > 0) {
    inputData.push({ city: queData?.city[0] })
  } else if (queData?.count[0]?.length > 0) {
    inputData.push({ count: queData?.count[0] })
  } else if (queData?.website[0]?.length > 0) {
    inputData.push({ url: queData?.website[0] })
  }
  let dataRes = [...messageList?.inputEntity]
  var arr = messageList?.inputEntity;

  if (index !== -1) {
    dataRes.splice(index, dataRes?.length);
    arr = dataRes;
  }

  arr = [...arr, ...inputData, ...ipData]
  if (inputData?.length > 0) {
    store.dispatch(messageActions.inputEntity(arr));
  }
  let caroType = (inputData?.length > 0
    ? messageList?.currentNodeFlow?.data?.children[0]?.childId
    : messageList?.currentNodeFlow?.data?.parentNode
  )?.includes('carousel');
  let carouselData = []
  if (caroType) {
    messageList?.currentNodeFlow?.data?.children?.map((list) => {
      let result = {
        "childId": list.childId,
        "type": "carousel"
      }
      carouselData.push(result);
    })
  }


  const contactCheck = inputData?.length > 0 ? messageList?.currentNodeFlow?.data?.children[0].childId : messageList?.currentNodeFlow?.data?.parentNode;
  if (contactCheck?.includes('transferToQueue')) {
    createContactApi()
  }

  const data1 = {
    session_id: messageList.uuid,
    domain_id: messageList.personalisationAll?.domainId,
    channelType: 'Chat',
    response: "success",
    btnValue: inputData?.length > 0 ? messageList?.currentNodeFlow?.data?.children[0].childId : messageList?.currentNodeFlow?.data?.parentNode,
    callFlowId: messageList.personalisationAll?.chatFlowId,
    call_type: caroType ? "carousel" : "",
    messageID: uuidv4(),
    inputParams: arr,
    stream_finished: messageList.personalisationAll?.plan_id === 56 || messageList.personalisationAll?.plan_id === 55 ? 3 : "",
    carousal_type: caroType ? carouselData : "",
    ChatHistoryId: parseInt(sessionStorage.ChatHistoryId),
    ChatDurationHistoryId: parseInt(sessionStorage?.ChatDurationHistoryId),
    //targetLang : messageList?.personalisation?.dynamiclanguage !== undefined?messageList?.personalisation?.dynamiclanguage :'en',
    targetLang: messageList?.targetLang,
  };

  // Check the resident exist in homecare when user search for the person

  if (builderList?.careHomeVerification?.verificationProcessStatus === 1) {
    switch (builderList?.careHomeVerification?.verificationStep) {
      case 0: {
        const userMailId = queData?.email[0]?.length ? queData?.email[0] : '';
        const data = {
          "loginUserName": userMailId,
          "companyId": messageList?.companyId,
          "isChatBot": '1'
        };
        fetch('https://carehomeapi.audracare.co.uk/homecare_service/v1/validate_guardian_email_bot', {
          method: 'POST',
          headers: {
            'Content-Type': 'application/json'
          },
          body: JSON.stringify(data)
        })
          .then(response => response.json())
          .then(resData => {
            if (resData?.statusCode === 200) {
              store.dispatch(chatBuilderActions.setCareHomeVerification({ verificationStep: 1, userMail: userMailId }));
              newChatSendMsg("Text", builderList?.careHomeVerification?.otpErrorCnt, "OUT", 0);
            } else {
              newChatSendMsg("Text", 'No such user found for that details', "OUT", 0);
            }
          })
          .catch(error => {
            console.error("Request failed:", error);
          });
        break;
      }
      case 1: {
        const otpCode = queData?.otp_code[0]?.length ? queData?.otp_code[0] : 0;
        const data = {
          "loginUserName": builderList?.careHomeVerification?.userMail,
          "otp": otpCode,
          "isChatBot": '1'
        };
        fetch('https://carehomeapi.audracare.co.uk/homecare_service/v1/validate_otp_bot', {
          method: 'POST',
          headers: {
            'Content-Type': 'application/json'
          },
          body: JSON.stringify(data)
        })
          .then(response => response.json())
          .then(resData => {
            if (resData?.statusCode === 200) {
              store.dispatch(chatBuilderActions.setCareHomeVerification({
                verificationProcessStatus: 0,
                authenticationStatus: 1,
                verificationStep: 2
              }));
              chatMessageUpdate(data1);
              store.dispatch(messageActions.flowNodeList(data1));
            } else {
              newChatSendMsg("Text", 'Invalid OTP Code', "OUT", 0);
            }
          })
          .catch(error => {
            console.error("Request failed:", error);
          });
        break;
      }
    }
  } else {
    chatMessageUpdate(data1)
    store.dispatch(messageActions.flowNodeList(data1))
  }
}

// Handle the chatflow when user trigger the flow

export const QueAnsDummyFlow = (data, status) => {
  const messageList = store.getState().message;
  let queData = messageList?.checkallQuesAns;
  let checkCond = checkQuesInt(status?.ans_intent);
  let isCheckAns = isJsonString(status?.ans_intent) && JSON.parse(status?.ans_intent);
  let finalcheck = checkQuesInt1(checkCond);
  if (queData) {
    let findAgent1 = messageList.nodeData.find(o => o?.id === queData?.mapFlow);
    let findAgentChild = finalcheck?.length > 0 ? finalcheck[0].childId : checkCond?.length > 0 ? checkCond[0].mapFlow : messageList?.currentNodeFlow?.data?.children[0].childId;
    console.log(checkCond?.length > 0 && isCheckAns[0] !== "sim_order" ? findAgentChild : findAgent1?.data?.children[0].childId, "findAgent1findAgent1findAgentChild")
    var inputData = [];
    if (status?.User_input_Entity?.mobile_number?.length > 0) {
      inputData.push({ mobile_no: status?.User_input_Entity?.mobile_number[0] })
    }
    let mobile = status?.User_input_Entity?.mobile_number[0]?.length > 0 ? inputData : messageList?.inputEntity;
    const data1 = {
      session_id: messageList.uuid,
      domain_id: messageList.personalisationAll?.domainId,
      channelType: 'Chat',
      response: "success",
      btnValue: messageList?.currentNodeFlow?.type === "loop" ? messageList?.currentNodeFlow?.data?.nodeId :
        checkCond?.length > 0 ? findAgentChild : findAgent1?.data?.children[0].childId,
      callFlowId: parseInt(messageList?.personalisationAll?.domainId) === 17820 ? 346954 : messageList?.personalisationAll?.chatFlowId,
      call_type: mobile,
      messageID: uuidv4(),
      inputParams: mobile,
      stream_finished: messageList.personalisationAll?.plan_id === 56 || messageList.personalisationAll?.plan_id === 55 ? 3 : "",
      ChatHistoryId: parseInt(sessionStorage.ChatHistoryId),
      ChatDurationHistoryId: parseInt(sessionStorage?.ChatDurationHistoryId),
      targetLang: messageList?.targetLang,
    };
    const flowName = isJsonString(status?.ans_intent) && JSON.parse(status?.ans_intent);
    store.dispatch(chatBuilderActions.setFlowIntentName(flowName?.[0]))
    if (data1?.btnValue?.includes('carousel')) {
      let parentData = messageList?.nodeData.find(o => o?.id === findAgent1?.data?.parentNode);
      let caroType = [];
      parentData?.data?.children.some((item) => {
        if (item?.childId?.includes('carousel')) {
          caroType?.push(item);
        }
      });
      parentData?.data?.children.some((item) => {
        if (item?.childId === data1?.btnValue) {
          return data1.call_type = item?.type;
        }
      });
      data1.carousal_type = caroType;
    } 
    chatMessageUpdate(data1)
    store.dispatch(messageActions.simOrderSts(true))
  }
};

// Receive message from oneToOneChatReceive and set in redux

export const receiveChatMessage = (message) => {
  const messageList = store.getState().message;
  try {
    if (!message.isCustomer) {
      const timestamp = Date.now();
      let msgData = (isJsonString(message?.message) && JSON.parse(message?.message)?.data !== undefined) ? JSON.parse(message?.message)?.data : message?.message;
      const context = contextMatch(msgData)
      if (context?.length > 0 && messageList?.integrationApiData?.api_data?.length > 0) {
        const variable_data = messageList?.integrationApiData?.api_data
        for (let context_data of context) {
          for (let temp_data of variable_data) {
            if (temp_data.variable === `\${${context_data}}`) {
              if (temp_data?.data.length > 0)
                msgData = msgData?.replace(`\${${context_data}}`, temp_data?.data[0])
            }
          }
          msgData = msgData.replace(`\${${context_data}}`, '')
        }
      }
      const text = {
        message: {
          ...message,
          chatIconLogo: messageList.iconLogo,
          message: msgData,
          timestamp
        },
      };
      if (message?.direction !== "IN" && message?.direction !== "AGENT" && message?.direction !== "AGENTINTRO") {
        if (message?.stream_finished === 3) {
          var testtest = msgData?.replace(/(<([^>]+)>)/gi, '');
          var arraySplit = testtest?.split(" ");
          let fullLength = ""
          for (var i = 0; i < arraySplit?.length; i++) {
            (function (i) {
              setTimeout(function () {
                console.log(i);
                if (fullLength === "") fullLength = arraySplit[i]
                else fullLength = fullLength + " " + arraySplit[i];
                const textfullLength = [{
                  ...message,
                  chatIconLogo: messageList.iconLogo,
                  que_answer: fullLength,
                  timestamp

                }];
                store.dispatch(messageActions.particialOutput(textfullLength));
                if (arraySplit?.length === (i + 1)) {
                  store.dispatch(messageActions.setMessageChat({ agentText: text }));
                  store.dispatch(messageActions.particialOutput([]));
                }
              }, 150 * i);
            })(i);
          }
        } else {
          store.dispatch(messageActions.setMessageChat({ agentText: text }));
        }
        if (messageList.personalisation?.notificationSound != "None" && messageList.soundNotification) {
          if (messageList.personalisation?.notificationSoundUrl !== "" && messageList.personalisation?.notificationSoundUrl !== undefined) {
            try {
              var myAudio = document.getElementById("msg_notify");
              myAudio.src = messageList.personalisation?.notificationSoundUrl
              if (myAudio?.readyState >= 2) {  // 2 means the media has data and it can be played
                myAudio.play();
              } else {
                myAudio.addEventListener('canplay', function () {
                  myAudio.play();
                });
              }
            } catch (err) {
              console.log(err)
            }
          }
        }
      }
    }
  } catch (err) {
    console.log(err)
  }
}

// Handle function for after agent disconnect 

export const agentDisconnect = async (message) => {
  if (sessionStorage?.agentToAddress === message?.sessionId) {
    const messageList = store.getState().message;
    const timestamp = Date.now();
    const text = {
      message: {
        ...message,
        chatIconLogo: messageList.iconLogo,
        timestamp,
      },
    };
    store.dispatch(messageActions.setMessageChat({ agentText: text }));
    store.dispatch(liveTrackerActions.setRestrictedEndChat(false));
    let connectionData = {
      isBotConnection: true,
      isAgentConnection: false,
      isDTMFConnection: false,
    };
    store.dispatch(messageActions.setChatDurationId(0));
    sessionStorage.removeItem('ChatDurationHistoryId');
    sessionStorage.setItem("isConnected", "bot");
    sessionStorage.setItem("agentToAddress", "");
    store.dispatch(messageActions.setConnectionType(connectionData));
    store.dispatch(liveTrackerActions.setAgentDisconnectStatus(true));
    store.dispatch(messageActions.currentlyHandling({ agentName: 'Bot'}));
    agentDeclineChatNlpIntent(message)
  }
}

// Connect to the bot after agent is disconnect

export const connectBotAfterAgent = async () => {
  const data = {
    "ChatHistoryId": parseInt(sessionStorage.ChatHistoryId),
    "BotId": 1,
    "sourceChatDurationHistoryId": parseInt(sessionStorage?.ChatDurationHistoryId)
  }
  try {
    const response = await fetch(`${config().CALLSERVICE_URL}/bot_connect_id_generate`, {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json'
      },
      body: JSON.stringify(data)
    });
    const responseData = await response.json();
    store.dispatch(messageActions.setChatDurationId(responseData?.result[0][0]?.ChatDurationHistoryId));
    sessionStorage.setItem("ChatDurationHistoryId", responseData?.result[0][0]?.ChatDurationHistoryId);
    store.dispatch(liveTrackerActions.setAgentDisconnectStatus(false));
    return;
  } catch (error) {
    console.error("Request failed:", error);
  }
}

// Handle the function for user in chat flow (All node response handler)

export const chatResponse = async (message) => {
  let dataRes = message && JSON.parse(message);
  const messageList = store.getState().message;
  const builderList = store.getState().chatBuilder;
  const liveTrackerList = store.getState().liveTracker;
  let nextnode = dataRes?.nextNodedetails?.data?.parentNode;
  let dtmf_array = messageList.nodeData.find(o => o.id === nextnode);
  store.dispatch(messageActions.paymentData(dataRes?.result));
  if (dataRes?.type?.includes('payment') && dataRes?.id?.includes('payment')) {
    const currency = isJSON(dataRes?.data?.information?.payment_data) ? JSON.parse(dataRes?.data?.information?.payment_data) : dataRes?.data?.information?.payment_data
    sessionStorage.setItem('CURR_PAYMENT_CURRENCY', currency?.currency)
  }
  if (dataRes?.result?.status === "success" && dataRes?.result?.server_trans_id !== null) {
    paymentEntry(dataRes?.result);
  } else if (dataRes?.result?.server_trans_id === null) {
    let data = {
      childId: "messageFlow_bZT1SU9Rw",
      type: 'error'
    }
    continusBlock(data);
  } 
  if (dtmf_array?.type === "makeApiCall") {
    if (dtmf_array?.data?.information?.nameAPI !== "generate otp" && dtmf_array?.data?.information?.nameAPI !== "verify otp" && dtmf_array?.data?.information?.nameAPI !== "topup") {
      store.dispatch(messageActions.inputEntity([]));
      store.dispatch(messageActions.simOrderSts(false))
    }
  }
  if (dataRes?.type === "integration" || dataRes?.currentNode?.type === "integration") {
    const data = dataRes?.currentNode?.type === "integration" ? (dataRes?.currentNode?.data?.information?.integration_data !== undefined && JSON.parse(dataRes?.currentNode?.data?.information?.integration_data)) : (dataRes?.data?.information?.integration_data !== undefined && JSON.parse(dataRes?.data?.information?.integration_data))
    if (data?.method === 'connectors' && data?.connector === 7 && data?.main_module === 7) {
      if (data?.sub_module === 69) {
        const url = await getCalendlyEndPoint(messageList, data)
        if (url !== null)
          store.dispatch(messageActions.setIntegration({ ...data, showIframe: true, isCalendlyShow: true, url: url?.scheduling_url }))
      } else if (data?.sub_module === 70) {
        store.dispatch(messageActions.setIntegration({ ...data, showIframe: true, isCalendlyShow: true, url: data.end_point }))
      } else if (data?.sub_module === 71) {
        store.dispatch(messageActions.setIntegration({ ...data, showIframe: true, isCalendlyShow: true, url: data.end_point }))
      } else if (data?.sub_module === 'custom') {
        store.dispatch(messageActions.setIntegration({ ...data, showIframe: true, isCalendlyShow: true, url: data.end_point }))
      }
    } else if (data?.method === 'connectors' && data?.connector === 8 && data?.main_module === 8) {
      if (data?.sub_module === 'custom') {
        store.dispatch(messageActions.setIntegration({ ...data, showIframe: true, isMsGoogleShow: true, url: data.end_point }))
      } else {
        store.dispatch(messageActions.setIntegration({ ...data, isMsGoogleShow: true, url: '' }))
      }
    } else if (data?.method === 'connectors' && data?.connector === 9 && data?.main_module === 9) {
      if (data?.sub_module === 'custom') {
        store.dispatch(messageActions.setIntegration({ ...data, showIframe: true, isMsGoogleShow: true, url: data.end_point }))
      } else {
        store.dispatch(messageActions.setIntegration({ ...data, isMsGoogleShow: true, url: '' }))
      }
    } else if (data?.method === 'connectors' && (data?.connector === 4 || data?.connector === 5)) {
      const connectionList = data?.connecter_data?.filter((row) => row?.shopDomain === data.connection_list)
      let end_point = data?.end_point
      const variableData = data?.variableData ?? null
      const regex = /{([^{}]+)}/g;
      const matches = [];
      let match;
      while ((match = regex.exec(data?.end_point)) !== null) {
        matches.push(match[1]);
      }
      end_point = end_point.replace(/{|}/g, '')
      if (variableData !== null) {
        for (let data of matches) {
          for (let variable of variableData) {
            const temp_data = variable.replace(/[\${}]/g, '').split('.')[1]
            if (temp_data === data) {
              const entity = messageList.inputEntity.filter((row) => Object.keys(row)[0] === data)
              if (entity.length > 0) {
                end_point = end_point.replace(data, entity[0][`${data}`])
              }
            }
          }
        }
      }
      const res_data = await getIntegrationApiData(end_point, connectionList[0]?.token)
      const api_data = []
      if (variableData !== null) {
        for (let var_data of variableData) {
          const temp_data = await returnVariableData(var_data, res_data, false)
          api_data.push({ "variable": var_data, data: [...temp_data] })
        }
      }
      console.log("api_data", api_data)
      store.dispatch(messageActions.setIntegrationApiData({ data: res_data, api_data, shopDomain: data.connection_list }))

      const child_data = dataRes?.currentNode?.type === "integration" ? dataRes?.currentNode?.data : dataRes?.data
      let filtered_array = child_data.children?.filter(o => o.type === "success");
      let caroType = [];
      filtered_array.some((item) => {
        if (Array.isArray(item?.childId) && item.childId.includes('carousel')) {
          caroType?.push(item);
        }
      });
      const intergrateData = {
        session_id: messageList.uuid,
        domain_id: messageList.personalisationAll?.domainId,
        channelType: 'Chat',
        response: "success",
        btnValue: filtered_array[0]?.childId,
        callFlowId: messageList.personalisationAll?.chatFlowId,
        carousal_type: caroType?.length > 0 ? caroType : filtered_array[0]?.childId,
        call_type: filtered_array[0]?.type,
        messageID: uuidv4(),
        ChatHistoryId: parseInt(sessionStorage.ChatHistoryId),
        ChatDurationHistoryId: parseInt(sessionStorage?.ChatDurationHistoryId),
        inputParams: messageList?.inputEntity,
        targetLang: messageList?.targetLang,
      }; 
      chatMessageUpdate(intergrateData);
    } else {
      let data = {
        childId: dataRes?.currentNode?.id,
        type: 'error'
      }
      continusBlock(data)
    }
  }
  if (dataRes?.type === "checkCondition" || dataRes?.currentNode?.type === "checkCondition") {
    const data = dataRes?.currentNode?.type === "checkCondition" ? (dataRes?.currentNode?.data?.information?.condition_data !== undefined && JSON.parse(dataRes?.currentNode?.data?.information?.condition_data)) : (dataRes?.data?.information?.condition_data !== undefined && JSON.parse(dataRes?.data?.information?.condition_data))
    const BrowserName = getBrowserName();
    const getipAddress = await getipAddressDetails();
    const location = await getLocationDetails(getipAddress);
    const country = await findCountry(location?.country);
    const VisitorTye = await getNodeEntryPoint(getipAddress, messageList?.personalisationAll?.domainId);
    const operatingSystem = getOperatingSystem();
    const AgentAvailibility = await getAgentAvailibility(messageList?.personalisationAll?.domainId);
    const language = navigator.language || navigator.userLanguage;
    const languageRes = await findAllLanguage(language);
    for (let groupsIndex = 0; groupsIndex < data?.groups?.length; groupsIndex++) {
      const currentGroupData = data.groups[groupsIndex].data;
      let description = "";
      let isSuccess = false;
      for (let con_data of currentGroupData) {
        if (con_data.content === 1) {
          const queryParams = new URLSearchParams(window.location.search)
          description = queryParams.get('url')
        } else if (con_data.content === 2) {
          const queryParams = new URLSearchParams(window.location.search)
          description = queryParams.get('title')
        } else if (con_data.content === 3) {
          description = BrowserName;
        } else if (con_data.content === 4) {
          description = operatingSystem;
        } else if (con_data.content === 5) {
          description = country;
        } else if (con_data.content === 6) {
          description = location?.city;
        } else if (con_data.content === 7) {
          description = AgentAvailibility;
        } else if (con_data.content === 8) {
          description = getipAddress;
        } else if (con_data.content === 9) {
          if (messageList.customerData && messageList.customerData.length > 0) {
            const firstName = messageList?.customerData[0]?.first_name;
            description = firstName;
          }
        } else if (con_data.content === 10) {
          if (messageList.customerData && messageList.customerData.length > 0) {
            const PhoneNumber = messageList?.customerData[0]?.mobile_no;
            description = PhoneNumber;
          }
        } else if (con_data.content === 11) {
          if (messageList.customerData && messageList.customerData.length > 0) {
            const Email = messageList?.customerData[0]?.email;
            description = Email;
          }
        } else if (con_data.content === 12) {
          description = VisitorTye;
        } else if (con_data.content === 13) {
          description = languageRes;
        } else if (con_data.content === 14) {
          var daysOfWeek = ['Sunday', 'Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday', 'Saturday'];
          description = daysOfWeek[new Date().getDay()]
        } else if (con_data.content === 15) {
          description = "Time of the day";
        }
        if (con_data.condition === 1) {
          if (con_data.description === description) {
            isSuccess = true
          } else {
            isSuccess = false
          }
        } else if (con_data.condition === 2) {
          if (description?.toLowerCase().startsWith(con_data.description?.toLowerCase())) {
            isSuccess = true
          } else {
            isSuccess = false
          }
        } else if (con_data.condition === 3) {
          if (description?.toLowerCase().endsWith(con_data.description?.toLowerCase())) {
            isSuccess = true
          } else {
            isSuccess = false
          }
        } else if (con_data.condition === 4) {
          if (description?.toLowerCase().includes(con_data.description?.toLowerCase())) {
            isSuccess = true
          } else {
            isSuccess = false
          }
        }
        if (isSuccess)
          break
      }
      if (isSuccess) {
        let filtered_array = dataRes?.currentNode?.data?.children?.filter(o => o.type === "success");
        let data = {
          childId: filtered_array[0]?.childId,
          type: 'error'
        }
        continusBlock(data);
      } else {
        let filtered_array = dataRes?.currentNode?.data?.children?.filter(o => o.type === "error");
        let data = {
          childId: filtered_array[0]?.childId,
          type: 'error'
        }
        continusBlock(data);
      }
    }
  }
  if (dataRes?.type === "appointment" || dataRes?.currentNode?.type === "appointment") {
    createContactApi();
    store.dispatch(messageActions.simOrderSts(true))
    const data = dataRes?.currentNode?.type === "appointment" ? (dataRes?.currentNode?.data?.information?.appointment_data !== undefined && JSON?.parse(dataRes?.currentNode?.data?.information?.appointment_data)) : (dataRes?.data?.information?.appointment_data !== undefined && JSON?.parse(dataRes?.data?.information?.appointment_data))
    let dataSess = dataRes?.currentNode?.type === "appointment" ? JSON?.stringify(dataRes?.currentNode?.data?.children) : JSON?.stringify(dataRes?.data?.children)
    sessionStorage.setItem("item", dataSess);
    if (data?.method === 'schedule') {
      store.dispatch(messageActions.setWorktualBooking({ ...data, ...messageList.worktualAppointment, isShow: true, url: `${data?.eventUrl}&coid=${messageList?.customerFormData?.coid}` }))
    } else if (data?.method === 'cancel' || data?.method === 'reschedule') {
      store.dispatch(messageActions.setWorktualBooking({
        ...data,
        ...messageList.worktualAppointment,
        isCancel_Reschedule: true,
        module: data?.method,
      }))
      newChatSendMsg("Text", data.emailContent, "OUT", 0);
    }
  }
  if (dataRes?.type === "datePicker" || dataRes?.currentNode?.type === "datePicker") {
    const dateMsg = dataRes?.type === "datePicker" ? JSON?.parse(dataRes?.data?.information?.datepicker_data) : JSON?.parse(dataRes?.currentNode?.data?.information?.datepicker_data)
    const data = dataRes?.currentNode?.type === "datePicker" ? dataRes?.currentNode : dataRes
    store.dispatch(messageActions.datetimePickerNode([data]))
    store.dispatch(messageActions.templateKeywordList([]));
  }
  if (dataRes?.req_type === "DISCONNECT") {
    let today = moment().format('YYYY-MM-DD');
    let startTimestampInSeconds = moment.utc(`${today} 00:00:00`, 'YYYY-MM-DD HH:mm:ss').valueOf();
    let todayStart = moment().format('YYYY-MM-DD');
    let toTimestampInSeconds = moment.utc(`${todayStart} 23:59:59`, 'YYYY-MM-DD HH:mm:ss').valueOf();
    const body = {
      startTmpstmp: startTimestampInSeconds,
      endTmpstmp: toTimestampInSeconds,
      timeZone: "",
      ChatHistoryId: parseInt(sessionStorage.ChatHistoryId),
      IsBot: 1,
      IsMissed: 1,
      domainId: messageList.personalisationAll?.domainId
    };

    let orderObj = [...messageList?.inputEntity]
    const mergedObject = Object.assign({}, ...orderObj);
    saveContact(mergedObject);
    createContactApi();
    if (builderList?.addFlowStatus) {
      const chatID = parseInt(builderList?.oldChatFlowId)
      const data1 = {
        session_id: messageList.uuid,
        domain_id: messageList.personalisationAll?.domainId,
        channelType: 'Chat',
        response: "success",
        btnValue: builderList?.addFlowChildId,
        callFlowId: chatID,
        call_type: 'error',
        messageID: uuidv4(),
        ChatHistoryId: parseInt(sessionStorage.ChatHistoryId),
        ChatDurationHistoryId: parseInt(sessionStorage?.ChatDurationHistoryId),
        inputParams: messageList?.inputEntity,
        targetLang: messageList?.targetLang,
      }; 
      chatMessageUpdate(data1);
      store.dispatch(messageActions.personalisationAll({ ...messageList?.personalisationAll, chatFlowId: chatID }));
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
        if (dataRes?.domainStatus && dataRes?.domainStatus?.length > 0) {
          missedCallDashboard(body);
          let msgRes = dataRes?.domainStatus[0]?.userStatus === "Unavailable" ? messageList?.automatedResponse?.unavilableMessage : messageList?.automatedResponse?.busyMessage;
          store.dispatch(messageActions.setCallbackAgentStatus(dataRes?.domainStatus[0]?.userStatus));
          if (dataRes?.domainStatus[0]?.userStatus === "Unavailable" && messageList?.automatedResponse?.isUnavailableCallbck === true) {
            store.dispatch(messageActions.setDynamicChatFormList(messageList?.automatedResponse?.chatCallbackUnavaiableFields))
            store.dispatch(messageActions.setCallbackBtnStatus(true))
          } else if (dataRes?.domainStatus[0]?.userStatus === "Busy" && messageList?.automatedResponse?.isBusyCallbck === true) {
            store.dispatch(messageActions.setDynamicChatFormList(messageList?.automatedResponse?.chatCallbackFields))
            store.dispatch(messageActions.setCallbackBtnStatus(true))
          }
          setTimeout(async () => {
            newChatSendMsg("Text", msgRes, "OUT", 1);
            if (messageList.personalisationAll?.plan_id === 54 || messageList.personalisationAll?.plan_id === 32 || messageList.personalisationAll?.plan_id === 116) {
              store.dispatch(messageActions.callBackButtonStatus(0))
            } else {
              store.dispatch(messageActions.callBackButtonStatus(1))
            }

          }, 1500);
          sessionStorage.setItem("isConnected", "");

        }
        store.dispatch(messageActions.simOrderSts(false))
      }
    }
    store.dispatch(chatBuilderActions.setOldChatFlowId(0));
    store.dispatch(chatBuilderActions.setAddFlowStatus(false));
    store.dispatch(chatBuilderActions.setAddFlowChildId(''));
    store.dispatch(messageActions.currentNodeFlow({}))

  } else {
    let data_res = dataRes?.currentNode !== undefined ? dataRes?.currentNode : dataRes
    store.dispatch(messageActions.currentNodeFlow(data_res));
    if (dataRes?.currentNode?.type !== "loop" && dataRes?.type !== "customInput" && dataRes?.currentNode?.type !== "customInput" && dataRes?.currentNode?.type !== "checkCondition" && dataRes?.currentNode?.type !== "transferToQueue" && dataRes?.currentNode?.type !== "messageFlow" && dataRes?.currentNode?.type !== "randomise" && dataRes?.currentNode?.type !== "carousel" && dataRes?.currentNode?.type !== "coupon" && dataRes?.currentNode?.type !== "appointment" && dataRes?.currentNode?.type !== "integration" && dataRes?.currentNode !== undefined && dataRes?.currentNode.length !== 0 && dataRes.length !== 0) {
      let msgData = dataRes?.currentNode?.data?.information?.message;
      newChatSendMsg("Text", dataRes?.currentNode?.data?.information?.message, "OUT", 0);
      let filtered_array = dataRes?.currentNode?.data?.children?.filter(
        o => !["success", "default", "error", "validate"].includes(o.type)
      );
      const unique = [];
      filtered_array.map(x => unique.filter(a => a.type == x.type).length > 0 ? null : unique.push(x));
      const linkData = dataRes?.currentNode?.data?.information?.data !== undefined ? JSON.parse(dataRes?.currentNode?.data?.information?.data) : {};
      // console.log(linkData,"linkDatalinkDatalinkDatalinkDatalinkData")

      if (linkData?.menuType === 'button') {
        const updatedArr1 = unique.map(item1 => {
          const matchingItem = linkData?.getInputData.find(item2 => item2.value === item1.type);
          if (matchingItem) {
            return { ...item1, link: matchingItem.link };
          }
          return item1;
        });
        let customPayloads = {
          messageID: uuidv4(),
          from: messageList.uuid,
          to: messageList?.connectionType?.isBotConnection ? messageList.uuid : `${messageList.agentDomainExt.domainId}_${messageList.agentDomainExt.ext}`,
          sessionId: messageList.uuid,
          sendAt: Date.now(),
          messageType: 'customButton',
          isBot: "IN",
          isCustomer: false,
          sender: '',
          isLike: 0,
          domainId: messageList?.personalisationAll?.domainId,
          isLiveagent: false,
          direction: 'IN',
          ChatHistoryId: parseInt(sessionStorage?.ChatHistoryId),
          ChatDurationHistoryId: parseInt(sessionStorage?.ChatDurationHistoryId),
          socketId: messageList?.socketId,
          assistType: checkConvoSupported() ? "Convo" : "Template",
          supervisorList: messageList?.superVisorExt,
          buttonDataArr: updatedArr1,
          chatFlowId: messageList.personalisationAll?.chatFlowId
        };
        oneToOneChatSend(customPayloads);
        let text = {
          message: customPayloads,
        };
        setTimeout(() => {
          store.dispatch(messageActions.setMessageChat({ agentText: text }));
        }, 2000)
        // store.dispatch(messageActions.templateKeywordList(updatedArr1));
      }
      else {
        let customPayloads = {
          messageID: uuidv4(),
          from: messageList.uuid,
          to: messageList?.connectionType?.isBotConnection ? messageList.uuid : `${messageList.agentDomainExt.domainId}_${messageList.agentDomainExt.ext}`,
          sessionId: messageList.uuid,
          sendAt: Date.now(),
          messageType: 'customButton',
          isBot: "IN",
          isCustomer: false,
          sender: '',
          isLike: 0,
          domainId: messageList?.personalisationAll?.domainId,
          isLiveagent: false,
          direction: 'IN',
          ChatHistoryId: parseInt(sessionStorage?.ChatHistoryId),
          ChatDurationHistoryId: parseInt(sessionStorage?.ChatDurationHistoryId),
          socketId: messageList?.socketId,
          assistType: checkConvoSupported() ? "Convo" : "Template",
          supervisorList: messageList?.superVisorExt,
          buttonDataArr: unique,
          chatFlowId: messageList.personalisationAll?.chatFlowId
        };
        oneToOneChatSend(customPayloads);
        let text = {
          message: customPayloads,
        };
        setTimeout(() => {
          store.dispatch(messageActions.setMessageChat({ agentText: text }));
        }, 2000)
        // store.dispatch(messageActions.templateKeywordList(unique));
      }
    }
    if (dataRes?.currentNode?.data?.information?.inputType === 2 || dataRes?.currentNode?.data?.information?.inputType === 12 || dataRes?.currentNode?.data?.information?.inputType === 7 || dataRes?.currentNode?.data?.information?.inputType === 9 || dataRes?.currentNode?.data?.information?.inputType === 6 || dataRes?.currentNode?.data?.information?.inputType === 5 || dataRes?.currentNode?.data?.information?.inputType === 8) {
      store.dispatch(messageActions.simOrderSts(true))
      let flow_array = messageList?.nodeData.find(o => o?.id === dataRes?.currentNode?.data?.parentNode);
      if (flow_array?.data?.children?.length > 0 && flow_array?.data?.information?.inputType === 1) {
        let flow_arrayyy = flow_array?.data?.children.find(o => o?.childId === dataRes?.currentNode?.id);
        store.dispatch(messageActions?.flowBreakParentnode(flow_arrayyy));

      } else {
        let ffflow_array = messageList?.nodeData.find(o => o?.id === flow_array?.data?.parentNode);
        if (ffflow_array?.data?.children?.length > 0 && ffflow_array?.data?.information?.inputType === 1) {
          let secflow_array = ffflow_array?.data?.children.find(o => o?.childId === dataRes?.currentNode?.data?.parentNode);
          store.dispatch(messageActions?.flowBreakParentnode(secflow_array));
        }
      }
    } else if (dataRes?.currentNode?.data?.information?.inputType === 7) {
      // store.dispatch(messageActions.paymentForm(true));
      // const response = await axios.post('https://mysqlapi.worktual.co.uk/myaccount-mysql/v1/worktual/get_ChatBotPaymentIntegration', {
      //   // p_companyId: messageList?.companyId !== '' ? messageList?.companyId : 0,
      //   p_companyId: messageList?.companyId,
      //   p_domainId: messageList.personalisationAll?.domainId,
      // });
      // if (response?.data?.users?.length > 0) {
      //   store.dispatch(messageActions.paymentMethod(response?.data?.users[0]))
      // }
    } else if (dataRes?.currentNode?.data?.information?.inputType === 3) {
      newChatSendMsg("Article", JSON.parse(dataRes?.currentNode?.data?.information?.data), "OUT", 0);
    } else if (dataRes?.currentNode?.data?.information?.inputType === 14) {
      store.dispatch(messageActions.templateKeywordList([]));
      let drop = JSON.parse(dataRes?.currentNode?.data?.information?.data);
      store.dispatch(messageActions.dropDownList(drop));
      store.dispatch(messageActions.setAnsCaptureStatus(drop?.getInputData?.answerSave));
      store.dispatch(messageActions.setAnsCaptureKey(drop?.getInputData?.contactSave));
      store.dispatch(messageActions.simOrderSts(true));

    } else if (dataRes?.currentNode?.data?.information?.inputType === 16) {
      store.dispatch(messageActions.setFileInputtype(true));
    } else if (dataRes?.currentNode?.data?.information?.inputType === 4) {
      const optCompanyData = isJSON(dataRes?.currentNode?.data?.information?.data) ? JSON.parse(dataRes?.currentNode?.data?.information?.data) : dataRes?.currentNode?.data?.information?.data;
      let toId = ""

      if (optCompanyData?.getInputData?.channel === "sms") {
        toId = messageList?.inputEntity?.find(obj => {
          return obj?.mobile_no;
        })?.mobile_no
      } else if (optCompanyData?.getInputData?.channel === "email") {
        toId = messageList?.inputEntity?.find(obj => {
          return obj?.email;
        })?.email
      }
      if (dataRes?.currentNode?.data?.information?.inputType === 4) {
        let otp_payload = {
          domainId: messageList?.personalisationAll?.domainId,
          toId,
          expiryTime: parseInt(optCompanyData?.getInputData?.otp_expiry),
          businessName: optCompanyData?.getInputData?.business_name
        };
        store.dispatch(chatBuilderActions.setOtpValidationId(otp_payload?.toId));
        store.dispatch(chatBuilderActions.setOtpResendButtonStatus(true));
        clearTimeout(builderList?.resendTimeOutData);
        const otpResponse = await fetch(`${config().CORESERVICE_URL}/generate_otp`, {
          method: 'POST',
          headers: {
            'Content-Type': 'application/json',
          },
          body: JSON.stringify(otp_payload),
        });
        if (!otpResponse.ok) {
          throw new Error(`Otp validation failed: ${otpResponse.status}`);
        }
        store.dispatch(chatBuilderActions.setOtpValidCount(0));
        store.dispatch(chatBuilderActions.setOtpResendCount(0));
        resendOtpGenerator(parseInt(optCompanyData?.getInputData?.otp_expiry))
        const createSessionData = await otpResponse.json();
        if (createSessionData?.statusCode === 200) {
          //
        }
      }
    }

    if (dataRes?.type === "careHomeAuth" || dataRes?.currentNode?.type === "careHomeAuth") {
      if (builderList?.careHomeVerification?.authenticationStatus === 1) {
        const ChildId = dataRes?.currentNode?.data?.children[0] ?? dataRes.data?.children[0];
        const data1 = {
          session_id: messageList.uuid,
          domain_id: messageList.personalisationAll?.domainId,
          channelType: 'Chat',
          response: "success",
          btnValue: ChildId?.childId,
          callFlowId: messageList.personalisationAll?.chatFlowId,
          call_type: ChildId?.type,
          messageID: uuidv4(),
          ChatHistoryId: parseInt(sessionStorage.ChatHistoryId),
          ChatDurationHistoryId: parseInt(sessionStorage?.ChatDurationHistoryId),
          inputParams: messageList?.inputEntity
        }; 
        chatMessageUpdate(data1);
      } else {
        const errorMessage = dataRes?.currentNode !== undefined ? dataRes?.currentNode : dataRes;
        const errorContent = errorMessage?.data?.information?.inputData;
        const authError = errorContent?.message
        const optError = errorContent?.otpMessage
        store.dispatch(chatBuilderActions.setCareHomeVerification({ verificationProcessStatus: 1, authErrorCnt: authError, otpErrorCnt: optError }))
        store.dispatch(messageActions.simOrderSts(true))
        const StepMessage = builderList?.careHomeVerification?.verificationStep === 0 ? authError : optError
        newChatSendMsg("Text", StepMessage, "OUT", 0);
      }

    }
  }

  if (dataRes?.currentNode?.type === "transferToQueue" && (messageList.personalisationAll?.plan_id !== 54 || (messageList.personalisationAll?.plan_id !== 32))) {
    createContactApi();
    setTimeout(async () => {
      handleConnectionType("agent");
      makeApiCall(dataRes?.currentNode);
    }, 1500);
    store.dispatch(messageActions.setAgentConnectingStatus(true))

  }
  if (dataRes?.type === "coupon" || dataRes?.currentNode?.type === "coupon") {
    const ChildId = dataRes?.currentNode?.data?.children[0];
    const data1 = {
      session_id: messageList.uuid,
      domain_id: messageList.personalisationAll?.domainId,
      channelType: 'Chat',
      response: "success",
      btnValue: ChildId?.childId,
      callFlowId: messageList.personalisationAll?.chatFlowId,
      call_type: ChildId?.type,
      messageID: uuidv4(),
      ChatHistoryId: parseInt(sessionStorage.ChatHistoryId),
      ChatDurationHistoryId: parseInt(sessionStorage?.ChatDurationHistoryId),
      inputParams: messageList?.inputEntity,
      targetLang: messageList?.targetLang,
    }; 
    chatMessageUpdate(data1);
  }
  if (dataRes?.currentNode?.type === "messageFlow") {
    let data = {
      childId: dataRes?.currentNode?.data?.children?.length > 0 ? dataRes?.currentNode?.data?.children[0].childId : "",
      type: dataRes?.currentNode?.data?.information?.message
    }
    continusBlock(data);
  }
  if (dataRes?.currentNode?.type === "carousel" || dataRes?.type === "carousel") {
    let Car_information = dataRes?.carousalChildData;
    let site = JSON.parse(messageList?.personalisationAll?.websiteId);
    let typeRes = messageList.personalisationAll?.plan_id === 54 || messageList.personalisationAll?.plan_id === 32 || messageList.personalisationAll?.plan_id === 97 || messageList.personalisationAll?.plan_id === 96 || messageList.personalisationAll?.plan_id === 103 || messageList.personalisationAll?.plan_id === 107 || messageList.personalisationAll?.plan_id === 120 || messageList.personalisationAll?.plan_id === 121 || messageList.personalisationAll?.plan_id == 122 || messageList.personalisationAll?.plan_id === 123 || messageList.personalisationAll?.plan_id === 124 || messageList.personalisationAll?.plan_id === 116 ? "webcrawl" : "assist";
    let carousalPayloads = {
      messageID: uuidv4(),
      from: messageList.uuid,
      to: messageList?.connectionType?.isBotConnection ? messageList.uuid : `${messageList.agentDomainExt.domainId}_${messageList.agentDomainExt.ext}`,
      sessionId: messageList.uuid,
      carouselDataArr: [],
      message: 'carousel',
      sendAt: Date.now(),
      messageType: 'carousel',
      isBot: "IN",
      isCustomer: false,
      sender: '',
      isLike: 0,
      type: typeRes,
      websiteId: site?.length > 0 ? site[0] : "",
      domainId: messageList?.personalisationAll?.domainId,
      isLiveagent: false,
      direction: 'IN',
      ChatHistoryId: parseInt(sessionStorage?.ChatHistoryId),
      ChatDurationHistoryId: parseInt(sessionStorage?.ChatDurationHistoryId),
      socketId: messageList?.socketId,
      assistType: checkConvoSupported() ? "Convo" : "Template",
      supervisorList: messageList?.superVisorExt
    };
    if (dataRes?.carousalChildData[0]?.data?.information?.carousel_data && isJsonString(dataRes?.carousalChildData[0]?.data?.information?.carousel_data)) {
      const temp_data = dataRes && dataRes.carousalChildData && dataRes.carousalChildData[0]
      const keys = Object.keys(messageList?.integrationApiData)
      if (keys.length > 0) {
        let information = JSON.parse(dataRes?.carousalChildData[0]?.data?.information?.carousel_data)
        const desc_data = await returnVariableData(information.description, messageList?.integrationApiData, false)
        const title_data = await returnVariableData(information.title, messageList?.integrationApiData, false)
        const image_data = await returnVariableData(information.variable, messageList?.integrationApiData, true)
        const handle_data = await returnVariableData(`result.products.handle`, messageList?.integrationApiData, true)
        console.log("image_data", information.variable)
        console.log("image_data", desc_data)
        console.log("image_data", title_data)
        const data = []

        let info_carousel = JSON.parse(dataRes?.carousalChildData[0]?.data?.information?.carousel_data)
        for (let i = 0; i < title_data.length; i++) {
          let button = [...info_carousel?.buttons]
          if (button) {
            if (title_data[i] !== undefined)
              button[0]['link'] = `https://${messageList?.integrationApiData?.shopDomain}/products/${handle_data[i]}`
          }
          info_carousel = {
            ...info_carousel,
            file_url: image_data[i] !== undefined ? image_data[i] : information?.file_url,
            title: title_data[i] !== undefined ? title_data[i]?.replace(/<[^>]*>/g, "") : information?.title,
            description: desc_data[i] !== undefined ? desc_data[i]?.replace(/<[^>]*>/g, "") : information?.description,
            buttons: [...button]
          }
          let carousel_data = { ...temp_data };
          if (carousel_data) {
            carousel_data.data = { ...carousel_data.data } || {};
            carousel_data.data.information = { ...carousel_data.data.information } || {};
            carousel_data.data.information = {
              ...carousel_data.data.information,
              carousel_data: JSON.stringify(info_carousel),
            }
            data.push(carousel_data)
          }
        }
        carousalPayloads.carouselDataArr = data;
        oneToOneChatSend(carousalPayloads)
        let texts = {
          message: carousalPayloads,
        };
        store.dispatch(messageActions.setMessageChat({ carouselText: texts }));
        store.dispatch(messageActions.carouselData(data))
        store.dispatch(messageActions.templateKeywordList([]));
        store.dispatch(messageActions.setIntegrationApiData({}))
      } else {
        carousalPayloads.carouselDataArr = Car_information;
        oneToOneChatSend(carousalPayloads);
        let texts = {
          message: carousalPayloads,
        };
        store.dispatch(messageActions.setMessageChat({ carouselText: texts }));
        store.dispatch(messageActions.carouselData(Car_information))
        store.dispatch(messageActions.templateKeywordList([]));
      }
    }
  }
  if (dataRes?.currentNode?.type === "addFlow" || dataRes?.currentNode?.type === "payment" || dataRes?.currentNode?.type === "randomise") {
    let data = {
      childId: dataRes?.currentNode?.id,
      type: 'error'
    }
    continusBlock(data);
    sessionStorage.setItem("item", JSON.stringify(data));
  } else if (dataRes?.type === "addFlow") {
    let addFlowNode = JSON.parse(dataRes?.addFlowNodes) ?? {};
    let arr = [...messageList?.nodeData, ...addFlowNode]
    nextAddFlow(dataRes);
    store.dispatch(messageActions.storeNodeData(arr));
  } else if (dataRes?.type === "payment") {
    let pay = JSON.parse(dataRes?.data?.information?.payment_data) ?? {};
    sessionStorage.setItem("AMOUNT", pay?.amount)
    if (pay?.paymentMethod?.length > 1) {
      let carousalPayloads = {
        messageID: uuidv4(),
        from: messageList.uuid,
        to: messageList?.connectionType?.isBotConnection ? messageList.uuid : `${messageList.agentDomainExt.domainId}_${messageList.agentDomainExt.ext}`,
        sessionId: messageList.uuid,
        message: 'Please Select the payment gateway to make payment',
        paymentMethod: pay?.paymentMethod,
        paymentSelector: 1,
        sendAt: Date.now(),
        messageType: '',
        isBot: "IN",
        isCustomer: false,
        sender: '',
        isLike: 0,
        domainId: messageList?.personalisationAll?.domainId,
        isLiveagent: false,
        direction: 'IN',
        ChatHistoryId: parseInt(sessionStorage?.ChatHistoryId),
        ChatDurationHistoryId: parseInt(sessionStorage?.ChatDurationHistoryId),
        socketId: messageList?.socketId,
        assistType: checkConvoSupported() ? "Convo" : "Template",
        supervisorList: messageList?.superVisorExt
      };
      oneToOneChatSend(carousalPayloads);
      let texts = {
        message: carousalPayloads,
      };
      store.dispatch(messageActions.setMessageChat({ agentText: texts }));
    } else {
      const paymentData = {
        domainId: messageList?.personalisationAll?.domainId,
        amount: +(pay?.amount),
        // currency: "INR"
        //  currency: messageList?.currentNodeFlow?.data?.information?.payment_data?JSON.parse(messageList?.currentNodeFlow?.data?.information?.payment_data)?.currency:""
        currency: sessionStorage.getItem('CURR_PAYMENT_CURRENCY'),
      }
      const paymentEncrypt = await encryptData(paymentData)
      store.dispatch(messageActions.paymentEnable(true))
      store.dispatch(messageActions.paymentSource(pay?.paymentMethod[0] ?? ''))
      store.dispatch(messageActions.paymentId(paymentEncrypt))
    }
    let currentNodeData = {
      childId: dataRes?.data?.nodeId,
      type: ''
    }
    sessionStorage.setItem("item", JSON.stringify(currentNodeData));
  } else if (dataRes?.type === "sendMail" || dataRes?.currentNode?.type === "sendMail") {
    sendEmailBlock(dataRes)
  }
  if (dataRes?.type === "customInput" || dataRes?.currentNode?.type === "customInput" && dataRes?.currentNode?.data?.information?.inputType !== 14) {
    const data = dataRes?.currentNode?.type === "customInput" ? dataRes?.currentNode?.data?.children : dataRes?.data?.children;
    const customData = dataRes?.currentNode?.type === "customInput" ? dataRes?.currentNode?.data : dataRes?.data;
    const saveFlowCustom = JSON.parse(customData?.information?.data) ?? {};
    store.dispatch(messageActions.setAnsCaptureStatus(saveFlowCustom?.getInputData?.answerSave));
    store.dispatch(messageActions.setAnsCaptureKey(saveFlowCustom?.getInputData?.contactSave));
    let filtered_array = data.filter(
      o => o.type !== "success" && o.type !== "default" && o.type !== "error" && o.type !== "validate"
    );
    const unique = [];
    filtered_array.map(x => unique.filter(a => a.type == x.type).length > 0 ? null : unique.push(x));
    const linkData = dataRes?.currentNode?.data?.information?.data !== undefined ? JSON.parse(dataRes?.currentNode?.data?.information?.data) : {};
    if (linkData?.menuType === 'button') {
      const updatedArr1 = unique.map(item1 => {
        const matchingItem = linkData?.getInputData.find(item2 => item2.value === item1.type);
        if (matchingItem) {
          return { ...item1, link: matchingItem.link, color: matchingItem.color || '' };
        }
        return item1;
      });
      let customPayloads = {
        messageID: uuidv4(),
        from: messageList.uuid,
        to: messageList?.connectionType?.isBotConnection ? messageList.uuid : `${messageList.agentDomainExt.domainId}_${messageList.agentDomainExt.ext}`,
        sessionId: messageList.uuid,
        sendAt: Date.now(),
        messageType: 'customButton',
        isBot: "IN",
        isCustomer: false,
        sender: '',
        isLike: 0,
        domainId: messageList?.personalisationAll?.domainId,
        isLiveagent: false,
        direction: 'IN',
        ChatHistoryId: parseInt(sessionStorage?.ChatHistoryId),
        ChatDurationHistoryId: parseInt(sessionStorage?.ChatDurationHistoryId),
        socketId: messageList?.socketId,
        assistType: checkConvoSupported() ? "Convo" : "Template",
        supervisorList: messageList?.superVisorExt,
        buttonDataArr: updatedArr1,
        chatFlowId: messageList.personalisationAll?.chatFlowId
      };
      // oneToOneChatSend(customPayloads);
      let text = {
        message: customPayloads,
      };
      // setTimeout(() => {
      //   store.dispatch(messageActions.setMessageChat({ agentText: text }));
      // }, 2000);

      // store.dispatch(messageActions.templateKeywordList(updatedArr1));
    }
    else {
      let customPayloads = {
        messageID: uuidv4(),
        from: messageList.uuid,
        to: messageList?.connectionType?.isBotConnection ? messageList.uuid : `${messageList.agentDomainExt.domainId}_${messageList.agentDomainExt.ext}`,
        sessionId: messageList.uuid,
        sendAt: Date.now(),
        messageType: 'customButton',
        isBot: "IN",
        isCustomer: false,
        sender: '',
        isLike: 0,
        domainId: messageList?.personalisationAll?.domainId,
        isLiveagent: false,
        direction: 'IN',
        ChatHistoryId: parseInt(sessionStorage?.ChatHistoryId),
        ChatDurationHistoryId: parseInt(sessionStorage?.ChatDurationHistoryId),
        socketId: messageList?.socketId,
        assistType: checkConvoSupported() ? "Convo" : "Template",
        supervisorList: messageList?.superVisorExt,
        buttonDataArr: unique,
        chatFlowId: messageList.personalisationAll?.chatFlowId
      };
      // oneToOneChatSend(customPayloads);
      let text = {
        message: customPayloads,
      };
      // setTimeout(() => {
      //   store.dispatch(messageActions.setMessageChat({ agentText: text }));
      // }, 2000);
      // store.dispatch(messageActions.templateKeywordList(unique));
    }
  }

}

// Default Api call for get method

export const getIntegrationApiData = (url, token) => {
  return new Promise((resolve) => {
    try {
      fetch(url, {
        method: 'GET',
        headers: {
          Authorization: token,
        }
      })
        .then(async (response) => {
          const data = await response.json();
          if (data?.statusCode === 200 || data?.statusCode === 401) {
            resolve(data);
          } else {
            resolve(null);
          }
        })
        .catch((error) => {
          console.error(error);
          resolve(null);
        });
    } catch (error) {
      console.error(error);
      resolve(null);
    }
  });
};

// Default Api call for post method

export const postIntegrationApiData = (url, token, payload) => {
  return new Promise((resolve) => {
    try {
      fetch(url, {
        method: 'POST',
        headers: {
          "Authorization": token,
          "Content-Type": "application/json"
        },
        body: JSON.stringify(payload)
      })
        .then(async (response) => {
          const data = await response.json();
          if (data?.statusCode === 200 || data?.statusCode === 422 || data?.statusCode === 409) {
            resolve(data);
          } else {
            resolve(data);
          }
        })
        .catch((error) => {
          console.error(error);
          resolve(null);
        });
    } catch (error) {
      console.error(error);
      resolve(null);
    }
  });
};

// Default Api call for patch method

export const patchIntegrationApiData = (url, token, payload) => {
  return new Promise((resolve) => {
    try {
      fetch(url, {
        method: 'PATCH',
        headers: {
          "Authorization": token,
          "Content-Type": "application/json"
        },
        body: JSON.stringify(payload)
      })
        .then(async (response) => {
          const data = await response.json();
          if (data?.statusCode === 200 || data?.statusCode === 422 || data?.statusCode === 409) {
            resolve(data);
          } else {
            resolve(null);
          }
        })
        .catch((error) => {
          console.error(error);
          resolve(null);
        });
    } catch (error) {
      console.error(error);
      resolve(null);
    }
  });
};


// Handle Add flow block

export const nextAddFlow = (dataRes) => {
  const messageList = store.getState().message;
  const data = {
    session_id: messageList.uuid,
    domain_id: messageList.personalisationAll?.domainId,
    channelType: 'Chat',
    call_state: 'init',
    callFlowId: dataRes?.data?.information?.flow_id,
    call_type: 'inbound',
    messageID: uuidv4(),
    ChatHistoryId: parseInt(sessionStorage.ChatHistoryId),
    response: "success",
    targetLang: messageList?.targetLang,
   inputParams: messageList?.inputEntity
  }; 
  chatMessageUpdate(data);
  store.dispatch(chatBuilderActions.setOldChatFlowId(messageList.personalisationAll?.chatFlowId));
  store.dispatch(chatBuilderActions.setAddFlowStatus(true));
  store.dispatch(chatBuilderActions.setAddFlowChildId(dataRes?.data?.children[0]?.childId));
  store.dispatch(messageActions.personalisationAll({ ...messageList?.personalisationAll, chatFlowId: dataRes?.data?.information?.flow_id }));


}

function extractValues(obj, desc) {
  if (!obj || !Array.isArray(desc) || desc.length === 0) {
    return [];
  }


  function extract(obj, desc, index = 0) {
    try {
      const key = desc[index].replace(/\s/g, '');
      if (obj && key in obj) {
        const value = obj[key];
        if (index === desc.length - 1) {
          return Array.isArray(value)
            ? value.map(item => ({ [key]: item }))
            : [{ [key]: value }];
        } else {
          const nextDesc = desc.slice(index + 1);
          return Array.isArray(value)
            ? value.flatMap(item => extract(item, nextDesc))
            : extract(value, nextDesc);
        }
      }
      return [];
    } catch (error) {
      console.log(error)
    }

  }

  return extract(obj, desc);
}

const contextMatch = (str) => {
  try {
    const regex = /\${(.*?)}/g;

    const matches = [];
    let match;
    while ((match = regex.exec(str)) !== null) {
      matches.push(match[1]);
    }
    return matches
  } catch (error) {
    console.log(error)
  }
}

const returnVariableData = (str, integrationApiData, isImage) => {
  return new Promise((resolve) => {
    try {
      const regex = /\${(.*?)}/g;
      const matches = [];
      if (!isImage) {
        let match;
        while ((match = regex.exec(str)) !== null) {
          matches.push(match[1]);
        }
      } else {
        matches.push(str)
      }
      let i = 0
      const desc_demo = []
      for (let desc of matches) {
        const row = desc.split('.').filter((row) => row !== 0 && row !== "0" && row !== '' && row !== 'context')
        const temp = extractValues(integrationApiData?.data, row)
        let j = 0
        for (let temp_data of temp) {
          const replacementValue = temp_data[row[row.length - 1]];

          const regex = /\${(.*?)}/g;
          let matches = [];
          let match;
          while ((match = regex.exec(str)) !== null) {
            matches.push(match[0]);
          }

          if (i === 0) {
            if (!isImage) {
              let result = str.replace(`\${${desc}}`, replacementValue);
              desc_demo.push(result)
            } else {
              let result = str.replace(`${desc}`, replacementValue);
              desc_demo.push(result)
            }
          } else {
            if (desc_demo[j] !== undefined) {
              let result = desc_demo[j].replace(`\${${desc}}`, replacementValue);
              desc_demo[j] = result
            }
          }
          j++
        }
        i++
      }
      resolve(desc_demo)
    } catch (error) {
      resolve([])
      console.log(error)
    }
  })

}

// Handle the Email block

const sendEmailBlock = (next) => {
  try {
    const messageList = store.getState().message;
    let mailData = isJsonString(next?.data?.information?.template_data) ? (next?.data?.information?.template_data !== undefined && JSON.parse(next?.data?.information?.template_data)) : "";
    let tomail = messageList?.inputEntity?.find(obj => {
      return obj?.email;
    });
    let sendTo = mailData?.sendTo

    return new Promise(async (resolve) => {
      try {
        if (sendTo.length === 0 || sendTo === null || sendTo === undefined)
          return resolve(null);
        let count = 2;
        if (sendTo.length == 1) {
          count = 1;
        }
        for (let i = 0; i < count; i++) {
          let data = {
            "domainId": messageList.personalisationAll?.domainId,
            "to": [sendTo[i] == 2 ? tomail?.email : null, (sendTo[i] == 1 && messageList?.domainDetails?.length > 0) ? messageList?.domainDetails[0]?.adminEmail : null],
            "subject": mailData?.subject,
          };
          if (mailData?.type === "2") {
            data = {
              ...data,
              "text": replaceContextPlaceholders(sendTo[i] == 1 ? mailData?.message : mailData?.customerMessage, messageList?.inputEntity),
            }
          } else {
            data = {
              ...data,
              "css": mailData?.data?.template_body,
              "html": mailData?.data?.template_logo_url_path,
            }
          }
          if (data?.to?.filter(o => o !== null).length === 0)
            return resolve(null);
          let url = mailData?.type === "1" ? 'send_email_template' : 'send_email_plaintext';
          const response = await fetch(`${config().EMAIL_URL}/${url}`, {
            method: 'POST',
            headers: {
              'Content-Type': 'application/json',
            },
            body: JSON.stringify(data),
          });
          await response.json();
        }
        const childData = {
          childId: next?.data?.children[0]?.childId,
          type: 'error',
        };
        continusBlock(childData);
        resolve(null);

      } catch (error) {
        console.log(error);
        resolve(null);
      }
    })
  } catch (error) {
    console.log(error);
  }
}
const replaceContextPlaceholders = (template, contextArray) => {
  return template.replace(/\$\{context\.([a-zA-Z0-9_]+)\}/g, (_, key) => {
    const match = contextArray.find(obj => key in obj);
    return match ? match[key] : '';
  });
}

// Handle the calender

const getCalendlyEndPoint = async (messageList, data) => {
  try {
    const response = await fetch(`${data?.end_point}/${messageList.personalisationAll.domainId}`);
    if (!response.ok) {
      console.log('Error:', response.statusText);
      return null;
    }
    const responseData = await response.json();
    if (responseData?.statusCode === 200) {
      return responseData;
    }
  } catch (error) {
    console.log(error);
  }
  return null;
};

// Handle the calendly in connectors

export const getCalendlySchedulePoint = async (datares) => {
  const messageList = store.getState().message;
  try {
    const data = {
      domainId: messageList.personalisationAll.domainId,
      event: datares?.event,
      invitee: datares?.invitee,
    };
    const response = await fetch('https://ccaasapi.worktual.co.uk/connector/calendar/v1/calendly/scheduledDetails', {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify(data),
    });
    if (!response.ok) {
      console.log('Error:', response.statusText);
      return null;
    }
    const responseData = await response.json();
    if (responseData?.statusCode === 200) {
      const email = responseData?.result?.inviteeResponse?.resource?.email;
      const createdAt = responseData?.result?.eventResponse?.resource?.start_time;
      const _timestamp = moment(new Date(createdAt)).format('MMM DD, YYYY hh:mm a');
      const msg = `All set! Your demo meet is scheduled on ${_timestamp}. I've sent an email confirmation to ${email}.`;
      newChatSendMsg('Text', msg, 'OUT', 0);
      return responseData;
    }
  } catch (error) {
    console.log(error);
  }
  return null;
};

// function handler for streaming

export const initgoStream = (messageToSend) => {
  const message = store.getState().message;
  // console.log('messagemessagemessage',message?.messageList[message?.messageList?.length - 1]?.agentText);
  // if (message?.messageList[message.messageList?.length - 1]?.agentText) return;
  let data = [];
  data.push(messageToSend);
  store.dispatch(messageActions.particialOutput(data));
  store.dispatch(messageActions.lottieProcess(false));
}

// function handler for aiResponse for convo and template based 

export const initiateBot = async (messageToSend) => {
  const messageList = store.getState().message;
  const builderList = store.getState().chatBuilder;
  const liveTrackerList = store.getState().liveTracker;
  try {
    if (messageList?.socketId === messageToSend?.socketId) {
      let dtmf_array = messageList.nodeData.find(o => o.type === "transferToQueue");
      store.dispatch(messageActions.particialOutput([]));
      store.dispatch(messageActions.lottieProcess(false));
      unResponsiveQAStore(messageToSend);
      if (parseInt(sessionStorage?.ChatHistoryId) && messageList.personalisationAll?.plan_id === 15 || messageList.personalisationAll?.plan_id === 32 || messageList.personalisationAll?.plan_id === 116) {
        if ((messageToSend?.ans_intent?.toLowerCase() === "transfer_live_agent")) {
          store.dispatch(messageActions.setAgentConnectingStatus(true));
          store.dispatch(messageActions.templateKeywordList([]));
          store.dispatch(messageActions.liveAgentStatus(1));
          setTimeout(async () => {
            handleConnectionType("agent");
            makeApiCall(dtmf_array, "live_agent");
          }, 1500);
        } else {
          newChatSendMsg("Text", messageToSend?.que_answer, "OUT", 1);
        }

      } else {
        // if (messageList?.templateData?.length > 0) { 
        if (!checkConvoSupported()) {
          let remainderAgentStatus = false;
          storageCustomerIntity(messageToSend);
          store.dispatch(messageActions.templateKeywordList([]));
          let checkCond = checkQuesInt(messageToSend?.ans_intent);
          let type = messageList?.currentNodeFlow?.data?.information;
          let again = multipleEnIntity(messageToSend);
          if (messageList.worktualAppointment?.isCancel_Reschedule) {
            let appointmentPayloads = {
              messageID: uuidv4(),
              from: messageList.uuid,
              to: messageList?.connectionType?.isBotConnection ? messageList.uuid : `${messageList.agentDomainExt.domainId}_${messageList.agentDomainExt.ext}`,
              sessionId: messageList.uuid,
              module: messageList.worktualAppointment?.module,
              isBot: "IN",
              isCustomer: false,
              sender: '',
              isLike: 0,
              websiteId: "",
              domainId: messageList?.personalisationAll?.domainId,
              isLiveagent: false,
              direction: 'IN',
              ChatHistoryId: parseInt(sessionStorage?.ChatHistoryId),
              ChatDurationHistoryId: parseInt(sessionStorage?.ChatDurationHistoryId),
              socketId: messageList?.socketId,
              assistType: messageList?.personalisationAll?.plan_id === 62 || messageList?.personalisationAll?.plan_id === 42 || messageList?.personalisationAll?.plan_id === 57 ? "Convo" : "Template",
              supervisorList: messageList?.superVisorExt
            };
            const repeatValue = messageList.worktualAppointment?.isCancel_Reschedule_Repeat + 1
            if (messageToSend?.User_input_Entity?.email.length > 0) {
              const email = messageToSend.User_input_Entity.email[0];
              const companyId = messageList.personalisationAll?.domainId;
              const frontUrl = process.env.REACT_APP_ENV === "qa"
                ? "https://stagebooking.worktual.co.uk"
                : "https://booking.worktual.co.uk";

              const requestData = {
                p_email: email,
                p_company_id: companyId,
                p_front_url: frontUrl,
              };

              try {
                const response = await fetch(`${config().WORKTUAL_APPOINTMENT}/ab_get_filter_by_endUser_email`, {
                  method: 'POST',
                  headers: {
                    'Content-Type': 'application/json',
                  },
                  body: JSON.stringify(requestData),
                });
                if (!response.ok) {
                  throw new Error(`HTTP error! status: ${response.status}`);
                }
                const data = await response.json();
                if (data?.err_code === 0 && data?.finalresponse.length > 0) {
                  appointmentPayloads = {
                    ...appointmentPayloads,
                    sendAt: Date.now(),
                    message: `Kindly select the appointment you want to ${messageList.worktualAppointment?.module === 'cancel' ? 'cancel' : 'reschedule'} from the list below.`,
                    messageType: 'WORKTUAL_CANCEL_RESCHEDULE',
                    appointmentData: data.finalresponse,
                  };
                  oneToOneChatSend(appointmentPayloads);
                  store.dispatch(messageActions.setMessageChat({ agentText: { message: appointmentPayloads } }));
                  store.dispatch(messageActions.setWorktualBooking({ ...messageList.worktualAppointment, isCancel_Reschedule: false, isCancel_Reschedule_Repeat: 0 }));
                } else {
                  appointmentPayloads = {
                    ...appointmentPayloads,
                    sendAt: Date.now(),
                    message: `Unfortunately, there are no appointments matching the provided email address at this time.`,
                    messageType: 'text',
                  };

                  oneToOneChatSend(appointmentPayloads);
                  store.dispatch(messageActions.setMessageChat({ agentText: { message: appointmentPayloads } }));
                  store.dispatch(messageActions.setWorktualBooking({ ...messageList.worktualAppointment, isCancel_Reschedule: false, isCancel_Reschedule_Repeat: 0 }));
                }
              } catch (error) {
                console.error("Error fetching appointments:", error);
              }
            } else {
              if (repeatValue === 3) {
                appointmentPayloads = {
                  ...appointmentPayloads,
                  sendAt: Date.now(),
                  message: `Sorry, I can't provide a relevant answer at the moment. If you have another question or need assistance with something else, feel free to ask. Thank you!`,
                  messageType: 'text',
                }
              } else {
                appointmentPayloads = {
                  ...appointmentPayloads,
                  sendAt: Date.now(),
                  message: `Please enter a valid email`,
                  messageType: 'text',
                }
              }
              oneToOneChatSend(appointmentPayloads);
              let texts = {
                message: appointmentPayloads,
              };
              store.dispatch(messageActions.setMessageChat({ agentText: texts }));
              store.dispatch(messageActions.setWorktualBooking({ ...messageList.worktualAppointment, isCancel_Reschedule_Repeat: repeatValue }));
            }
            if (repeatValue <= 2)
              return
            store.dispatch(messageActions.setWorktualBooking({ ...messageList.worktualAppointment, isCancel_Reschedule: false, isCancel_Reschedule_Repeat: 0 }));
            let uniqueChatId = sessionStorage.getItem("item");
            let filtered_array = JSON.parse(uniqueChatId).find(o => o.type === "success");
            let data = {
              childId: filtered_array?.childId,
              type: 'error'
            }
            continusBlock(data);
            return
          };
          // For care home two step verification
          if (messageToSend?.care_ent?.first_name !== '' && messageToSend?.care_ent?.first_name !== undefined && messageList?.companyId) {
            let body = {
              "companyId": messageList?.companyId ?? 0,
              "residentName": messageToSend?.care_ent?.first_name,
              "isChatBot": "1"
            }
            try {
              const response = await fetch(`${config().CARE_HOME}/check_resident_name`, {
                method: 'POST',
                headers: {
                  'Content-Type': 'application/json',
                },
                body: JSON.stringify(body),
              });
              if (!response.ok) {
                const data = await response.json();
                if (data?.statusCode === 404) {
                  let residentNotExistMsg = 'Sorry, the resident name you are searching for does not exist in our care home.';
                  newChatSendMsg("Text", residentNotExistMsg, "OUT", 0);
                  return;
                }
              }
            } catch (error) {
              console.error("Error checking resident name:", error);
            }

          }
          // For ecommerce store purchase
          if (messageToSend?.ecommerce_intent !== '') {
            store.dispatch(chatBuilderActions.setPriceFilter(messageToSend?.price_filter));
          }
          if (messageToSend?.ecommerce_intent === 'Product Enquiry') {
            const productDetails = [];
            const recommendModifyIntent = [];
            const productModifyIntent = { ...messageToSend?.ecommerce_product }
            for (let key in messageToSend?.ecommerce_product) {
              productDetails?.push(key)
            }
            for (let key in messageToSend?.recomended_product) {
              recommendModifyIntent?.push(key)
            }
            store.dispatch(chatBuilderActions.setEcommercePurchase({ isPurchase: true, nlpProductIntent: productModifyIntent, nlpRecommended: { ...messageToSend?.recomended_product }, recommendedIntent: recommendModifyIntent }));
            checkValidProductEnquiry(productDetails)
            return;
          }
          if (messageToSend?.ecommerce_intent === 'offer enquiry') {
            const productDetails = [];
            const recommendModifyIntent = [];
            const productModifyIntent = { ...messageToSend?.ecommerce_product }
            for (let key in messageToSend?.ecommerce_product) {
              productDetails?.push(key)
            }
            for (let key in messageToSend?.recomended_product) {
              recommendModifyIntent?.push(key)
            }
            store.dispatch(chatBuilderActions.setEcommercePurchase({ isPurchase: true, nlpProductIntent: productModifyIntent, nlpRecommended: { ...messageToSend?.recomended_product }, recommendedIntent: recommendModifyIntent }));
            for (let value in messageToSend?.ecommerce_product) {
              if (value === 'None') {
                store.dispatch(chatBuilderActions.setOffers(true))
              }
            }
            checkValidOfferEnquiry(productDetails)
            return;
          }
          if (messageToSend?.ecommerce_intent === 'Product Purchase') {
            const productModifyIntent = { ...messageToSend?.ecommerce_product }
            const productDetails = [];
            const recommendModifyIntent = [];
            for (let key in productModifyIntent) {
              productDetails?.push(key)
            }
            for (let key in messageToSend?.recomended_product) {
              recommendModifyIntent?.push(key)
            }
            if (productDetails?.length === 1) {
              store.dispatch(chatBuilderActions.setEcommercePurchase({ isPurchase: true, productIntent: productDetails.slice(1), nlpProductIntent: productModifyIntent, nlpRecommended: { ...messageToSend?.recomended_product }, recommendedIntent: recommendModifyIntent }));
              let selectedArray = '';
              if (builderList?.connectionName === 'shopify' || builderList?.connectionName === 'Wix' || builderList?.connectionName === 'WooCommerce') {
                selectedArray = productModifyIntent[productDetails[0]];
              } else if (builderList?.connectionName === 'BigCommerce' || builderList?.connectionName === 'Magento') {
                selectedArray = productDetails[0];
              }
              ecommerceProdcutSearch(selectedArray);
            } else {
              store.dispatch(chatBuilderActions.setEcommercePurchase({ isPurchase: true, productIntent: productDetails, nlpProductIntent: productModifyIntent, nlpRecommended: { ...messageToSend?.recomended_product }, recommendedIntent: recommendModifyIntent }));
              ecommerceTemplateKey(productDetails)
            }
            return
          }
          if (type?.inputType === 12 || type?.inputType === 2) {
            let err_array = messageList?.currentNodeFlow?.data?.children.find(o => o.type === "default" || o.type === "success");
            const caroType = [];
            if (err_array?.childId?.includes('carousel')) {
              messageList?.currentNodeFlow?.data?.children?.map((list) => {
                let data = {
                  childId: list?.childId,
                  type: 'carousel'
                }
                caroType.push(data);
              })
            }
            const data1 = {
              session_id: messageList.uuid,
              domain_id: messageList.personalisationAll?.domainId,
              channelType: 'Chat',
              response: "success",
              btnValue: err_array?.childId,
              callFlowId: messageList.personalisationAll?.chatFlowId,
              call_type: caroType?.length > 0 ? "carousel" : "",
              messageID: uuidv4(),
              inputParams: messageList?.inputEntity,
              carousal_type: caroType?.length > 0 ? caroType : "",
              ChatHistoryId: parseInt(sessionStorage.ChatHistoryId),
              ChatDurationHistoryId: parseInt(sessionStorage?.ChatDurationHistoryId),
              targetLang: messageList?.targetLang,
            }; 
            chatMessageUpdate(data1)
          } else {
            let checkCond2 = again?.length === 0;
            if ((checkCond?.length > 0 || checkCond2) && messageList?.simOrderSts && type?.inputType !== 4) {
              let testagin = multipleOtherIntity(messageToSend);
              let errorStatus = messageList?.currentNodeFlow?.data?.nodeId?.startsWith('careHomeAuth');
              let carouselValidate = !messageList?.currentNodeFlow?.data?.nodeId?.startsWith('carousel');
              // !carouselValidate
              if (testagin?.length > 0 && !errorStatus && type?.inputType !== 14 && carouselValidate) {
                let errorMsgCountMsg = JSON.parse(messageList?.currentNodeFlow?.data?.information?.data) ?? ''
                let errorMsgCount = parseInt(errorMsgCountMsg?.getInputData?.noOfRepeat, 10);
                if (builderList?.customBlockError?.errorCount >= errorMsgCount) {
                  newChatSendMsg("Text", messageToSend?.que_answer, "OUT", 1);
                  store.dispatch(chatBuilderActions.setCustomBlockError({ errorNode: '', errorCount: 0 }));
                  store.dispatch(messageActions.simOrderSts(false))
                } else {
                  let errorMsg = JSON.parse(messageList?.currentNodeFlow?.data?.information?.data);
                  const errorMsgCnt = errorMsg?.getInputData?.errorMessage !== '' ? errorMsg?.getInputData?.errorMessage : 'Please enter a correct value';
                  newChatSendMsg("Text", errorMsgCnt, "OUT", 0);
                  store.dispatch(chatBuilderActions.setCustomBlockError({ errorNode: messageList?.currentNodeFlow?.data?.nodeId }));
                }
              } else if (errorStatus && testagin?.length > 0) {
                newChatSendMsg("Text", 'Please enter a correct value', "OUT", 0);
              } else {
                store.dispatch(messageActions.setRecommedFlowStatus(true));
                store.dispatch(messageActions.setRemainderFlowId(messageList?.currentNodeFlow?.data?.nodeId));
                store.dispatch(messageActions.setRemainderFlowData(messageList?.currentNodeFlow));
                newChatSendMsg("Text", `Are you sure you want to discontinue ${builderList?.flowIntentName ? builderList?.flowIntentName : 'this'} process and require assistance with something else?`, "OUT", 0);
                let data = [{
                  childId: 'breakFlow_yes', type: 'Yes'
                },
                {
                  childId: 'breakFlow_no', type: 'No'
                }]
                store.dispatch(messageActions.flowBreakMsg(messageToSend))
                store.dispatch(messageActions.templateKeywordList(data));
              };
            } else {
              // Otp block validation
              if (type?.inputType === 4) {
                const otpExtract = messageToSend?.User_input_Entity;
                const otpValue = otpExtract?.count?.length > 0 ? otpExtract?.count : otpExtract?.otp_code?.length > 0 ? otpExtract?.otp_code : [];
                if (otpValue?.length > 0) {
                  let otp_payload = {
                    domainId: messageList?.personalisationAll?.domainId,
                    toId: builderList?.otpValidationId,
                    verificationCode: otpValue[0],
                  }
                  const otpResponse = await fetch(`${config().CORESERVICE_URL}/validate_otp`, {
                    method: 'POST',
                    headers: {
                      'Content-Type': 'application/json',
                    },
                    body: JSON.stringify(otp_payload),
                  });
                  if (!otpResponse.ok) {
                    throw new Error(`Otp validation failed: ${otpResponse.status}`);
                  }
                  const createSessionData = await otpResponse.json();
                  if (createSessionData?.statusCode === 200) {
                    let err_array = messageList?.currentNodeFlow?.data?.children.find(o => o.type === "default" || o.type === "success");
                    const caroType = [];
                    if (err_array?.childId?.includes('carousel')) {
                      messageList?.currentNodeFlow?.data?.children?.map((list) => {
                        let data = {
                          childId: list?.childId,
                          type: 'carousel'
                        }
                        caroType.push(data);
                      })
                    }
                    const data1 = {
                      session_id: messageList.uuid,
                      domain_id: messageList.personalisationAll?.domainId,
                      channelType: 'Chat',
                      response: "success",
                      btnValue: err_array?.childId,
                      callFlowId: messageList.personalisationAll?.chatFlowId,
                      call_type: caroType?.length > 0 ? "carousel" : "",
                      messageID: uuidv4(),
                      inputParams: messageList?.inputEntity,
                      carousal_type: caroType?.length > 0 ? caroType : "",
                      ChatHistoryId: parseInt(sessionStorage.ChatHistoryId),
                      ChatDurationHistoryId: parseInt(sessionStorage?.ChatDurationHistoryId),
                      targetLang: messageList?.targetLang,
                    }; 
                    chatMessageUpdate(data1);
                    store.dispatch(chatBuilderActions.setOtpResendButtonStatus(false));
                    clearTimeout(builderList?.resendTimeOutData);
                  } else {
                    try {
                      const optCompanyData = JSON.parse(type?.data)
                      let otpErrorMsg = 'OTP is wrong.<br/> Please try again';
                      if (parseInt(optCompanyData?.getInputData?.retry_limit) > builderList?.otpValidCount) {
                        newChatSendMsg("Text", otpErrorMsg, "OUT", 0);
                        const incrementCount = builderList?.otpValidCount + 1
                        store.dispatch(chatBuilderActions.setOtpValidCount(incrementCount))
                      } else {
                        let site = JSON.parse(messageList?.personalisationAll?.websiteId);
                        let typeRes = messageList.personalisationAll?.plan_id === 54 || messageList.personalisationAll?.plan_id === 32 || messageList.personalisationAll?.plan_id === 97 || messageList.personalisationAll?.plan_id === 96 || messageList.personalisationAll?.plan_id === 103 || messageList.personalisationAll?.plan_id === 107 || messageList.personalisationAll?.plan_id === 120 || messageList.personalisationAll?.plan_id === 121 || messageList.personalisationAll?.plan_id == 122 || messageList.personalisationAll?.plan_id === 123 || messageList.personalisationAll?.plan_id === 124 || messageList.personalisationAll?.plan_id === 116 ? "webcrawl" : "assist";
                        let resendOtpPayloads = {
                          messageID: uuidv4(),
                          from: messageList.uuid,
                          to: messageList?.connectionType?.isBotConnection ? messageList.uuid : `${messageList.agentDomainExt.domainId}_${messageList.agentDomainExt.ext}`,
                          sessionId: messageList.uuid,
                          message: 'Would you like us to resend the OTP?',
                          sendAt: Date.now(),
                          messageType: 'resendOtp',
                          isBot: "IN",
                          isCustomer: false,
                          sender: '',
                          isLike: 0,
                          type: typeRes,
                          websiteId: site?.length > 0 ? site[0] : "",
                          domainId: messageList?.personalisationAll?.domainId,
                          isLiveagent: false,
                          direction: 'IN',
                          ChatHistoryId: parseInt(sessionStorage?.ChatHistoryId),
                          ChatDurationHistoryId: parseInt(sessionStorage?.ChatDurationHistoryId),
                          socketId: messageList?.socketId,
                          assistType: checkConvoSupported() ? "Convo" : "Template",
                          supervisorList: messageList?.superVisorExt
                        };
                        oneToOneChatSend(resendOtpPayloads);
                        let texts = {
                          message: resendOtpPayloads,
                        };
                        store.dispatch(messageActions.setMessageChat({ agentText: texts }));
                      }
                    } catch (err) {
                      console.log(err)
                    }
                  }
                } else {
                  store.dispatch(messageActions.setRecommedFlowStatus(true));
                  store.dispatch(messageActions.setRemainderFlowId(messageList?.currentNodeFlow?.data?.nodeId));
                  store.dispatch(messageActions.setRemainderFlowData(messageList?.currentNodeFlow));
                  newChatSendMsg("Text", "Are you sure you want to discontinue this process and require assistance with something else?", "OUT", 0);
                  let data = [{
                    childId: 'breakFlow_yes', type: 'Yes'
                  },
                  {
                    childId: 'breakFlow_no', type: 'No'
                  }]
                  store.dispatch(messageActions.flowBreakMsg(messageToSend))
                  store.dispatch(messageActions.templateKeywordList(data));
                };
              } else {
                checkQuesAnswer(messageToSend);
                console.log("asdasdddddddddddddd>>>>>", checkCond?.length > 0)
                if ((messageToSend?.ans_intent?.toLowerCase() !== "transfer_live_agent") && (!messageList?.simOrderSts) && checkCond?.length === 0) {
                  //newChatSendMsg("Text", messageToSend?.que_answer, "OUT", 1);
                  let typeRes = messageList.personalisationAll?.plan_id === 54 || messageList.personalisationAll?.plan_id === 32 || messageList.personalisationAll?.plan_id === 97 || messageList.personalisationAll?.plan_id === 96 || messageList.personalisationAll?.plan_id === 103 || messageList.personalisationAll?.plan_id === 107 || messageList.personalisationAll?.plan_id === 120 || messageList.personalisationAll?.plan_id === 121 || messageList.personalisationAll?.plan_id == 122 || messageList.personalisationAll?.plan_id === 123 || messageList.personalisationAll?.plan_id === 124 || messageList.personalisationAll?.plan_id === 116 ? "webcrawl" : "assist";
                  let site = JSON.parse(messageList?.personalisationAll?.websiteId);
                  let text = {
                    message: {
                      message: messageToSend?.que_answer,
                      sendAt: Date.now(),
                      sessionId: messageList.uuid,
                      messageID: uuidv4(),
                      isBot: true,
                      isCustomer: false,
                      isLike: 0,
                      ChatHistoryId: parseInt(sessionStorage?.ChatHistoryId),
                      ChatDurationHistoryId: parseInt(sessionStorage?.ChatDurationHistoryId),
                      socketId: messageToSend?.socketId,
                      messageType: 'text',
                      type: typeRes,
                      websiteId: site?.length > 0 ? site[0] : "",
                      domainId: messageList?.personalisationAll?.domainId,
                      isLiveagent: messageList?.connectionType?.isAgentConnection ? true : false,
                      assistType: checkConvoSupported() ? "Convo" : "Template",
                      supervisorList: messageList?.superVisorExt,
                      language: liveTrackerList?.selectedTextLanguage?.code,
                      agentIntro: false,
                      likeDislikeStatus: true,
                      query_lang: messageToSend?.query_lang
                    },
                  };
                  oneToOneChatSend(text.message);
                  store.dispatch(messageActions.setMessageChat({ agentText: text }));
                  if (messageToSend?.image_url?.length > 0) {
                    messageToSend?.image_url?.map((list) => {
                      let fileData = {
                        url: list,
                        mimetype: "image/jpeg"
                      }
                      newChatSendMsg("file", fileData, "OUT", 1);
                    })
                  }
                }
                if (checkCond?.length > 0) {
                  let isCheckAns = JSON.parse(messageToSend?.ans_intent) ?? [];
                  if (isCheckAns[0] === "sim_order") {
                    store.dispatch(messageActions.simOrderSts(true))
                  }
                  QueAnsDummyFlow(messageList.personalisationAll.domainId, messageToSend);
                }
                if (messageList?.simOrderSts) {
                  multipleIntity(messageToSend);
                }
                if ((messageToSend?.ans_intent?.toLowerCase() === "transfer_live_agent")) {
                  remainderAgentStatus = true
                  store.dispatch(messageActions.setAgentConnectingStatus(true));
                  store.dispatch(messageActions.templateKeywordList([]));
                  store.dispatch(messageActions.liveAgentStatus(1))
                  setTimeout(async () => {
                    handleConnectionType("agent");
                    makeApiCall(dtmf_array, "live_agent");
                  }, 1500);
                  let msg = messageList?.automatedResponse?.handoffMessage ? messageList?.automatedResponse?.handoffMessage : "Ofcourse! Please bear with me while I connect you to an available agent."
                  newChatSendMsg("Text", msg, "OUT", 0);
                }
                store.dispatch(messageActions.templateKeywordList([]));
                store.dispatch(messageActions.setAnsCaptureStatus(false));
                store.dispatch(messageActions.setAnsCaptureKey(''));
              }
            }
          }
          startRemainderFlow(remainderAgentStatus);
          let recommandCond = (messageToSend?.recommanded_intent !== "" && liveTrackerList?.remainderActiveCount < (parseInt(messageList?.personalisation?.chatRecommentCount) ?? 3)) ? checkRecommedInt(messageToSend?.recommanded_intent) : 0;
        } else {
          // For convo based response
          let text = {
            message: {
              message: messageToSend?.bot_respones,
              sendAt: Date.now(),
              sessionId: messageList.uuid,
              messageID: uuidv4(),
              isBot: true,
              isCustomer: false,
              isLike: 0,
              ChatHistoryId: parseInt(sessionStorage?.ChatHistoryId),
              ChatDurationHistoryId: parseInt(sessionStorage?.ChatDurationHistoryId),
              socketId: messageToSend?.socketId,
              likeDislikeStatus: true,
              query_lang: messageToSend?.query_lang,
              is_pothys: messageToSend?.is_pothys ?? false,
              pothys_product_data: messageToSend?.pothys_product_data ?? [],
              is_nickys: messageToSend?.is_nickys ?? false,
              nickys_product_data: messageToSend?.nickys_product_data ?? [],
              messageTypeNlp: messageToSend?.messageType || "",
              mimetype: messageToSend?.mimetype || "",
              url: messageToSend?.url || "",
              name: messageToSend?.name || "",
            },
          };
          if (messageToSend?.trigger_form === true) {
            store.dispatch(messageActions.setPurchasePlanStatus(true));
            store.dispatch(messageActions.setPurchaseBackIconStatus(true))
            store.dispatch(messageActions.setPurchasePlanUrl(messageToSend?.plan_link));
          } else if (messageToSend?.b2c === 'budget') {
            let bot_respone = messageToSend?.bot_respones || {}
            const budgetData = isJSON(bot_respone) ? JSON.parse(bot_respone) : bot_respone
            const groupedData = Object.keys(budgetData).reduce((acc, key) => {
              if (key !== 'total' && key !== 'explanation') {
                budgetData[key].forEach(item => {
                  if (!acc[item.p_sellerInformation]) {
                    acc[item.p_sellerInformation] = [];
                  }
                  acc[item.p_sellerInformation].push({ ...item, category: key });
                });
              }
              return acc;
            }, {});
            let payload = {
              messageID: uuidv4(),
              from: messageList.uuid,
              to: messageList?.connectionType?.isBotConnection ? messageList.uuid : `${messageList.agentDomainExt.domainId}_${messageList.agentDomainExt.ext}`,
              sessionId: messageList.uuid,
              module: messageList.worktualAppointment?.module,
              isBot: "IN",
              isCustomer: false,
              sender: '',
              isLike: 0,
              websiteId: "",
              domainId: messageList?.personalisationAll?.domainId,
              isLiveagent: false,
              direction: 'IN',
              ChatHistoryId: parseInt(sessionStorage?.ChatHistoryId),
              ChatDurationHistoryId: parseInt(sessionStorage?.ChatDurationHistoryId),
              socketId: messageList?.socketId,
              assistType: checkConvoSupported() ? "Convo" : "Template",
              supervisorList: messageList?.superVisorExt,
              sendAt: Date.now(),
              message: ``,
              budgetData: { ...groupedData, "total": budgetData?.total },
              isB2C: true,
              explanation: budgetData?.explanation,
              query_lang: messageToSend?.query_lang
            };
            oneToOneChatSend(payload);
            store.dispatch(messageActions.setMessageChat({ agentText: { message: payload } }));
          } else if ((messageToSend?.b2c === 'product_purchase' || messageToSend?.b2c === 'simonly_deals') && messageToSend?.stream_finished == 1) {
            let b2c_payload = {
              searchQuery: messageToSend?.question ?? '',
              type: messageToSend?.b2c,
              userId: sessionStorage.getItem('B2C_SESSIONID')
            };
            sendB2cSearchMessage(b2c_payload);
            let payload = {
              messageID: uuidv4(),
              from: messageList.uuid,
              to: messageList?.connectionType?.isBotConnection ? messageList.uuid : `${messageList.agentDomainExt.domainId}_${messageList.agentDomainExt.ext}`,
              sessionId: messageList.uuid,
              module: messageList.worktualAppointment?.module,
              isBot: "IN",
              isCustomer: false,
              sender: '',
              isLike: 0,
              websiteId: "",
              domainId: messageList?.personalisationAll?.domainId,
              isLiveagent: false,
              direction: 'IN',
              ChatHistoryId: parseInt(sessionStorage?.ChatHistoryId),
              ChatDurationHistoryId: parseInt(sessionStorage?.ChatDurationHistoryId),
              socketId: messageList?.socketId,
              assistType: checkConvoSupported() ? "Convo" : "Template",
              supervisorList: messageList?.superVisorExt,
              sendAt: Date.now(),
              message: messageToSend?.bot_respones,
              isB2C: false,
              query_lang: messageToSend?.query_lang
            };
            oneToOneChatSend(payload);
            store.dispatch(messageActions.setMessageChat({ agentText: { message: payload } }));
            if (messageToSend?.b2c === 'product_purchase') {
              const data = {
                'p_id': Array.isArray(messageToSend?.product_ids) ? messageToSend?.product_ids?.join(",") : '',
                'p_countrycode': 'IN',
              };
              const response = await fetch(`${config().B2C_PRODUCT_SEARCH}/product/get_product_data`, {
                method: 'POST',
                headers: {
                  'Content-Type': 'application/json',
                },
                body: JSON.stringify(data),
              });
              if (!response.ok) {
                throw new Error(`HTTP error! Status: ${response.status}`);
              }
              const responseData = await response.json();
              payload = {
                ...payload,
                isB2C: true,
                b2cProductData: responseData?.result[0],
                message: '',
              }
            } else if (messageToSend?.b2c === 'simonly_deals') {
              const data = {
                'p_uuid': Array.isArray(messageToSend?.product_ids) ? messageToSend?.product_ids?.splice(0, 3)?.join(',') : '',
                "p_contractLength": '',
                "p_monthlyCost": '',
                "p_data": '',
                "p_callMinutes": '',
                "p_sellerId": '',
              };
              const response = await fetch(`${config().B2C_PRODUCT_SEARCH}/services/get_sim_only_search`, {
                method: 'POST',
                headers: {
                  'Content-Type': 'application/json',
                },
                body: JSON.stringify(data),
              });
              if (!response.ok) {
                throw new Error(`HTTP error! Status: ${response.status}`);
              }
              const responseData = await response.json();
              payload = {
                ...payload,
                isB2C: true,
                b2cSimData: Array.isArray(responseData?.result[0]) && responseData?.result[0]?.length > 0
                  ? responseData?.result[0]
                  : [],
                message: '',
              }
            }

            oneToOneChatSend(payload);
            store.dispatch(messageActions.setMessageChat({ agentText: { message: payload } }));
          } else if (messageToSend?.is_purchase == 1 && messageToSend?.stream_finished == 1) {
            let payload = {
              messageID: uuidv4(),
              from: messageList.uuid,
              to: messageList?.connectionType?.isBotConnection ? messageList.uuid : `${messageList.agentDomainExt.domainId}_${messageList.agentDomainExt.ext}`,
              sessionId: messageList.uuid,
              module: messageList.worktualAppointment?.module,
              isBot: "IN",
              isCustomer: false,
              sender: '',
              isLike: 0,
              websiteId: "",
              domainId: messageList?.personalisationAll?.domainId,
              isLiveagent: false,
              direction: 'IN',
              ChatHistoryId: parseInt(sessionStorage?.ChatHistoryId),
              ChatDurationHistoryId: parseInt(sessionStorage?.ChatDurationHistoryId),
              socketId: messageList?.socketId,
              assistType: checkConvoSupported() ? "Convo" : "Template",
              supervisorList: messageList?.superVisorExt,
              sendAt: Date.now(),
              message: messageToSend?.bot_respones,
              isB2C: false,
              likeDislikeStatus: true,
              query_lang: messageToSend?.query_lang
            };
            oneToOneChatSend(payload);
            store.dispatch(messageActions.setMessageChat({ agentText: { message: payload } }));
            setTimeout(() => {
              const paymentEncrypt = messageToSend?.currency === "INR"
                ? encryptData(messageToSend?.billing_amount)
                : encryptData({ 'amount': messageToSend?.billing_amount, 'currency': messageToSend?.currency });
              const paymentSource = messageToSend?.currency === "INR" ? 'worktualpurchase' : 'stripworktualpurchase';
              store.dispatch(messageActions.paymentEnable(true));
              store.dispatch(messageActions.paymentSource(paymentSource));
              store.dispatch(messageActions.paymentId(paymentEncrypt));
            }, 1500);

          } else {
            if ((messageToSend?.queue_key === 1)) {
              store.dispatch(messageActions.liveAgentStatus(1));
              setTimeout(async () => {
                await handleConnectionType("agent");
                makeApiCall(dtmf_array, "live_agent");

              }, 1500);
              if (messageToSend?.is_agent !== 'decline') {
                text.message['message'] = 'Ofcourse! Please bear with me while I connect you to an available agent.'
              }
              store.dispatch(messageActions.setAgentConnectingStatus(true))
            }

            // messageToSend?.is_agent !== 'decline' is for without routing profile now handle by nlp;
            // it is just an intimation to client;
            if(messageToSend?.is_agent !== 'decline'){
              oneToOneChatSend(text.message);
              if (messageToSend?.session_connect !== 1) {
                store.dispatch(messageActions.setMessageChat({ agentText: text }));
              }
              storageAICustomerIntity(messageToSend)
            }
          }
        }
      }
    }
  } catch (err) {
    console.log(err);
  };
};

//Handle the function get the ecommerce prduct

export const ecommerceProdcutSearch = async (intent) => {
  try {
    store.dispatch(messageActions.templateKeywordList([]))
    const messageList = store.getState().message;
    const builderList = store.getState().chatBuilder
    if (builderList?.connectionName === 'shopify') {
      const end_point = `${config()?.SHOPIFY_CONNECTION_LIST}/${messageList.personalisationAll.domainId}`
      const res_data = await getIntegrationApiData(end_point, '')
      if (res_data?.result.length > 0) {
        const token = res_data?.result[0]?.token;
        const currencyMethod = isJsonString(res_data?.result[0]?.settings) ? JSON.parse(res_data?.result[0]?.settings) : ''
        store.dispatch(chatBuilderActions.setEcommercePurchase({ token, currencyType: currencyMethod?.shopify_currency }));
        // const url = `${config()?.SHOPIFY_TOKEN_GENERATOR}/${intent}`
        const url = `${config()?.SHOPIFY_PRODUCT_SEARCH}`
        let payload = {
          product_id: intent,
          priceCondition: builderList?.priceFilter?.symbol === 'GT' ? 'greaterThan' : builderList?.priceFilter?.symbol === 'LS' ? 'lesserThan' : builderList?.priceFilter?.symbol === 'BT' ? 'between' : '',
          price: builderList?.priceFilter?.from ? parseInt(builderList?.priceFilter?.from, 10) : 0,
          toPrice: builderList?.priceFilter?.to ? parseInt(builderList?.priceFilter?.to, 10) : 0,
          currencyType: currencyMethod?.shopify_currency ?? ''
        }
        const products = await postIntegrationApiData(url, token, payload);
        store.dispatch(chatBuilderActions.setPriceFilter({}))
        if (products?.result?.length > 0) {
          const resultData = await Promise.all(
            products?.result?.map(async (list) => {
              //const priceList = list?.variants[0]?.price
              const sizeVariants = [];
              const colourVariants = [];
              let defaultSize = '';
              let defaultColour = '';
              await Promise.all(list?.options?.map((option) => {
                if (option?.name.toLowerCase() === 'size') {
                  defaultSize = option?.values[0]
                  sizeVariants.push(option?.values)
                } else if (option?.name.toLowerCase() === 'color') {
                  defaultColour = option?.values[0];
                  colourVariants.push(option?.values)
                }
              }));
              let currentSymbol = ''
              let priceList = ''
              await Promise.all(list?.variants[0]?.presentment_prices?.map((c_opt) => {
                if (c_opt?.price?.currency_code === currencyMethod?.shopify_currency) {
                  currentSymbol = c_opt?.price?.currency_code === 'INR' ? '₹' : c_opt?.price?.currency_code === 'USD' ? '$' : '£';
                  priceList = c_opt?.price?.amount;
                }
              }));
              let carouselResponse = {
                isDynamicPrice: true,
                message: list?.title,
                file_url: list?.image?.src !== undefined ? list?.image?.src : noImage,
                description: list?.body_html?.replace(/<[^>]*>/g, "") ?? '',
                title: list?.title,
                price: priceList ?? list?.variants[0]?.price,
                currencySign: currentSymbol ?? '£',
                variantSize: sizeVariants,
                variantColour: colourVariants,
                buttons: [{ "value": "Select", 'carouselId': 'yes_Carousel_1', 'productDetails': list, 'quantityCount': 1, 'selectedColour': defaultColour, 'selectedSize': defaultSize }]
              }
              return ({ data: { information: { type: 'carousel', carousel_data: JSON.stringify(carouselResponse) } } });
            })
          );
          let site = JSON.parse(messageList?.personalisationAll?.websiteId);
          let typeRes = messageList.personalisationAll?.plan_id === 54 || messageList.personalisationAll?.plan_id === 32 || messageList.personalisationAll?.plan_id === 97 || messageList.personalisationAll?.plan_id === 96 || messageList.personalisationAll?.plan_id === 103 || messageList.personalisationAll?.plan_id === 107 || messageList.personalisationAll?.plan_id === 120 || messageList.personalisationAll?.plan_id === 121 || messageList.personalisationAll?.plan_id == 122 || messageList.personalisationAll?.plan_id === 123 || messageList.personalisationAll?.plan_id === 124 || messageList.personalisationAll?.plan_id === 116 ? "webcrawl" : "assist";
          let carousalPayloads = {
            messageID: uuidv4(),
            from: messageList.uuid,
            to: messageList?.connectionType?.isBotConnection ? messageList.uuid : `${messageList.agentDomainExt.domainId}_${messageList.agentDomainExt.ext}`,
            sessionId: messageList.uuid,
            carouselDataArr: resultData,
            message: 'carousel',
            sendAt: Date.now(),
            messageType: 'carousel',
            isBot: "IN",
            isCustomer: false,
            sender: '',
            isLike: 0,
            type: typeRes,
            websiteId: site?.length > 0 ? site[0] : "",
            domainId: messageList?.personalisationAll?.domainId,
            isLiveagent: false,
            direction: 'IN',
            ChatHistoryId: parseInt(sessionStorage?.ChatHistoryId),
            ChatDurationHistoryId: parseInt(sessionStorage?.ChatDurationHistoryId),
            socketId: messageList?.socketId,
            assistType: checkConvoSupported() ? "Convo" : "Template",
            supervisorList: messageList?.superVisorExt
          };
          oneToOneChatSend(carousalPayloads);
          let texts = {
            message: carousalPayloads,
          };
          store.dispatch(messageActions.setMessageChat({ carouselText: texts }));
          store.dispatch(messageActions.carouselData(resultData))
        } else {
          const message = "Oops! Looks like the item you're searching for couldn't be found."
          newChatSendMsg("Text", message, "OUT", 0);
          ecommerceTemplateKey(builderList?.ecommercePurchase?.productIntent);
        }
      }
    } else if (builderList?.connectionName === 'Magento') {
      const end_point = `${config()?.MAGENTO_TOKEN_GENERATOR}/${messageList.personalisationAll.domainId}`
      const res_data = await getIntegrationApiData(end_point, '')
      console.log("products", res_data)
      if (res_data?.result.length > 0) {
        const token = res_data?.result[0]?.token;
        store.dispatch(chatBuilderActions.setEcommercePurchase({ token }));
        store.dispatch(chatBuilderActions.setMagentoDomain(res_data?.result[0]?.shopDomain))
        // const url = `${config()?.MAGENTO_PRODUCT_SEARCH}/${intent}`
        const url = `${config()?.MAGENTO_PRODUCT_SEARCH}/${intent}`
        const products = await getIntegrationApiData(url, token)
        if (products?.statusCode === 401) {
          const url = `${config()?.MAGENTO_REFRESH_TOKEN}/${messageList.personalisationAll.domainId}`
          const refreshToken = await getIntegrationApiData(url, token)
          ecommerceProdcutSearch(intent);
        } else {
          store.dispatch(chatBuilderActions.setPriceFilter({}))
          if (products?.statusCode === 200 && products?.result?.items?.length > 0) {
            const resultData = await Promise.all(products?.result?.items?.map(async (list) => {
              let proDescription = ''
              await Promise.all(list?.custom_attributes?.map((desc) => {
                if (desc?.attribute_code === 'description') {
                  if (desc?.value !== undefined) {
                    proDescription = desc?.value;
                  }
                }
              }))
              const sizeVariants = [];
              const colourVariants = [];
              let defaultSize = '';
              let defaultColour = '';
              list?.options?.map((variants) => {
                if (variants?.title.toLowerCase() === 'size') {
                  variants?.values?.map((data) => {
                    defaultSize = variants?.values[0]?.title
                    sizeVariants?.push(data?.title)
                  })
                } else if (variants?.title.toLowerCase() === 'color') {
                  variants?.values?.map((data) => {
                    defaultColour = variants?.values[0]?.title
                    colourVariants?.push(data?.title)
                  })
                }
              })

              let carouselResponse = {
                isDynamicPrice: true,
                message: list?.name,
                file_url: `${res_data?.result[0]?.shopDomain}media/catalog/product${list?.media_gallery_entries[0]?.file}` ?? noImage,
                description: proDescription?.replace(/<[^>]*>/g, "") ?? '',
                title: list?.name,
                price: list?.price,
                variantSize: [sizeVariants],
                variantColour: [colourVariants],
                buttons: [{ "value": "Select", 'carouselId': 'yes_Carousel_1', 'productDetails': list, 'quantityCount': 1, 'selectedColour': defaultColour, 'selectedSize': defaultSize }]
              }
              return ({ data: { information: { type: 'carousel', carousel_data: JSON.stringify(carouselResponse) } } });
            }))
            let site = JSON.parse(messageList?.personalisationAll?.websiteId);
            let typeRes = messageList.personalisationAll?.plan_id === 54 || messageList.personalisationAll?.plan_id === 32 || messageList.personalisationAll?.plan_id === 97 || messageList.personalisationAll?.plan_id === 96 || messageList.personalisationAll?.plan_id === 103 || messageList.personalisationAll?.plan_id === 107 || messageList.personalisationAll?.plan_id === 120 || messageList.personalisationAll?.plan_id === 121 || messageList.personalisationAll?.plan_id == 122 || messageList.personalisationAll?.plan_id === 123 || messageList.personalisationAll?.plan_id === 124 || messageList.personalisationAll?.plan_id === 116 ? "webcrawl" : "assist";
            let carousalPayloads = {
              messageID: uuidv4(),
              from: messageList.uuid,
              to: messageList?.connectionType?.isBotConnection ? messageList.uuid : `${messageList.agentDomainExt.domainId}_${messageList.agentDomainExt.ext}`,
              sessionId: messageList.uuid,
              carouselDataArr: resultData,
              message: 'carousel',
              sendAt: Date.now(),
              messageType: 'carousel',
              isBot: "IN",
              isCustomer: false,
              sender: '',
              isLike: 0,
              type: typeRes,
              websiteId: site?.length > 0 ? site[0] : "",
              domainId: messageList?.personalisationAll?.domainId,
              isLiveagent: false,
              direction: 'IN',
              ChatHistoryId: parseInt(sessionStorage?.ChatHistoryId),
              ChatDurationHistoryId: parseInt(sessionStorage?.ChatDurationHistoryId),
              socketId: messageList?.socketId,
              assistType: checkConvoSupported() ? "Convo" : "Template",
              supervisorList: messageList?.superVisorExt
            };
            oneToOneChatSend(carousalPayloads);
            let texts = {
              message: carousalPayloads,
            };
            store.dispatch(messageActions.setMessageChat({ carouselText: texts }));
            store.dispatch(messageActions.carouselData(resultData))
          } else {
            const message = "Oops! Looks like the item you're searching for couldn't be found."
            newChatSendMsg("Text", message, "OUT", 0);
            ecommerceTemplateKey(builderList?.ecommercePurchase?.productIntent);
          }
        }
      }
    } else if (builderList?.connectionName === 'BigCommerce') {
      const end_point = `${config()?.BIGCOMMERCE_TOKEN_GENERATOR}/${messageList.personalisationAll.domainId}`
      const res_data = await getIntegrationApiData(end_point, '')
      if (res_data?.result.length > 0) {
        const token = res_data?.result[0]?.token;
        store.dispatch(chatBuilderActions.setEcommercePurchase({ token }));
        const url = `${config()?.BIGCOMMERCE_SEARCH_LIST}/${intent}`
        const products = await getIntegrationApiData(url, token);
        store.dispatch(chatBuilderActions.setPriceFilter({}))
        if (products?.statusCode === 200) {
          if (products?.result?.data?.length > 0) {
            const productResponse = [];
            for (let data of products?.result?.data) {
              let url = `${config()?.BIGCOMMERCE_GET_VARIANTS}/${data?.id}/variant`
              const variants = await getIntegrationApiData(url, token);
              let result = {
                productData: data,
                variantData: variants?.result?.data
              }
              productResponse?.push(result)
            }
            const resultData = productResponse?.map((list) => {
              const sizeVariants = [];
              const colourVariants = [];
              let defaultSize = '';
              let defaultColour = '';
              list?.variantData?.map((v_data) => {
                v_data?.option_values.map((datas) => {
                  if (datas?.option_display_name.toLowerCase() === 'color') {
                    colourVariants?.push(datas?.label)
                  } else if (datas?.option_display_name.toLowerCase() === 'size') {
                    sizeVariants?.push(datas?.label)
                  }
                })
              })
              defaultColour = colourVariants[0];
              defaultSize = sizeVariants[0];
              let uniqueSize = [...new Set(sizeVariants)];
              let uniqueColour = [...new Set(colourVariants)];
              let carouselResponse = {
                isDynamicPrice: true,
                message: list?.productData?.name,
                file_url: list?.variantData[0]?.image_url !== undefined ? list?.variantData[0]?.image_url : noImage,
                description: list?.productData?.description?.replace(/<[^>]*>/g, "") ?? '',
                title: list?.productData?.name,
                price: list?.variantData[0]?.price,
                variantSize: uniqueSize?.length > 0 ? [uniqueSize] : [],
                variantColour: uniqueColour?.length > 0 ? [uniqueColour] : [],
                productId: list?.productData?.id,
                variantId: list?.variantData[0]?.id,
                buttons: [{ "value": "Select", 'carouselId': 'yes_Carousel_1', 'productDetails': list, 'quantityCount': 1, 'selectedColour': defaultColour, 'selectedSize': defaultSize }]
              }
              return ({ data: { information: { type: 'carousel', carousel_data: JSON.stringify(carouselResponse) } } });
            })
            let site = JSON.parse(messageList?.personalisationAll?.websiteId);
            let typeRes = messageList.personalisationAll?.plan_id === 54 || messageList.personalisationAll?.plan_id === 32 || messageList.personalisationAll?.plan_id === 97 || messageList.personalisationAll?.plan_id === 96 || messageList.personalisationAll?.plan_id === 103 || messageList.personalisationAll?.plan_id === 107 || messageList.personalisationAll?.plan_id === 120 || messageList.personalisationAll?.plan_id === 121 || messageList.personalisationAll?.plan_id == 122 || messageList.personalisationAll?.plan_id === 123 || messageList.personalisationAll?.plan_id === 124 || messageList.personalisationAll?.plan_id === 116 ? "webcrawl" : "assist";
            let carousalPayloads = {
              messageID: uuidv4(),
              from: messageList.uuid,
              to: messageList?.connectionType?.isBotConnection ? messageList.uuid : `${messageList.agentDomainExt.domainId}_${messageList.agentDomainExt.ext}`,
              sessionId: messageList.uuid,
              carouselDataArr: resultData,
              message: 'carousel',
              sendAt: Date.now(),
              messageType: 'carousel',
              isBot: "IN",
              isCustomer: false,
              sender: '',
              isLike: 0,
              type: typeRes,
              websiteId: site?.length > 0 ? site[0] : "",
              domainId: messageList?.personalisationAll?.domainId,
              isLiveagent: false,
              direction: 'IN',
              ChatHistoryId: parseInt(sessionStorage?.ChatHistoryId),
              ChatDurationHistoryId: parseInt(sessionStorage?.ChatDurationHistoryId),
              socketId: messageList?.socketId,
              assistType: checkConvoSupported() ? "Convo" : "Template",
              supervisorList: messageList?.superVisorExt
            };
            oneToOneChatSend(carousalPayloads);
            let texts = {
              message: carousalPayloads,
            };
            store.dispatch(messageActions.setMessageChat({ carouselText: texts }));
            store.dispatch(messageActions.carouselData(resultData))
          } else {
            const message = "Oops! Looks like the item you're searching for couldn't be found."
            newChatSendMsg("Text", message, "OUT", 0);
            ecommerceTemplateKey(builderList?.ecommercePurchase?.productIntent);
          }
        }
      }
    } else if (builderList?.connectionName === 'Wix') {
      const end_point = `${config()?.WIX_TOKEN_GENERATOR}/${messageList.personalisationAll.domainId}`
      const res_data = await getIntegrationApiData(end_point, '')
      if (res_data?.result.length > 0) {
        const tokenId = res_data?.result[0]?.token;
        const end_point_refresh = `${config()?.WIX_REFRESH_TOKEN_GENERATOR}/${messageList.personalisationAll.domainId}`
        const res_data_refresh = await getIntegrationApiData(end_point_refresh, tokenId);
        const token = res_data_refresh?.results?.token
        store.dispatch(chatBuilderActions.setEcommercePurchase({ token }));
        //const url = `${config()?.WIX_PRODUCT_SEARCH}/${intent}`
        //const products = await getIntegrationApiData(url, token);
        const url = config()?.WIX_PRODUCT_SEARCH_ID;
        let payload = {
          product_id: intent,
          priceCondition: builderList?.priceFilter?.symbol === 'GT' ? 'greaterThan' : builderList?.priceFilter?.symbol === 'LS' ? 'lesserThan' : builderList?.priceFilter?.symbol === 'BT' ? 'between' : '',
          price: builderList?.priceFilter?.from !== '' ? parseInt(builderList?.priceFilter?.from, 10) : 0,
          toPrice: builderList?.priceFilter?.to !== '' ? parseInt(builderList?.priceFilter?.to, 10) : 0
        }
        const products = await postIntegrationApiData(url, token, payload);
        store.dispatch(chatBuilderActions.setPriceFilter({}))
        if (products?.result?.products?.length > 0) {
          const resultData = await Promise.all(
            products?.result?.products?.map(async (list) => {
              const sizeVariants = [];
              const colourVariants = [];
              let defaultSize = '';
              let defaultColour = '';
              list?.productOptions?.map((variant) => {
                if (variant?.name === 'Size') {
                  defaultSize = variant?.choices[0]?.value
                  const result = variant?.choices?.map((s) => {
                    return s?.value
                  })
                  sizeVariants.push(result);
                } else if (variant?.name === 'Color') {
                  defaultColour = variant?.choices[0]?.value;
                  const result = variant?.choices?.map((s) => {
                    return s?.value
                  })
                  colourVariants.push(result);
                }
              })
              const priceList = list?.price?.price;
              let carouselResponse = {
                isDynamicPrice: true,
                message: list?.name,
                file_url: list?.media?.items[0]?.image?.url !== undefined ? list?.media?.items[0]?.image?.url : noImage,
                file_url_id: list?.media?.items[0]?.id,
                description: list?.description?.replace(/<[^>]*>/g, "") ?? '',
                title: list?.name,
                price: priceList,
                variantSize: sizeVariants,
                variantColour: colourVariants,
                defaultVariantId: list?.variants[0]?.id,
                buttons: [{ "value": "Select", 'carouselId': 'yes_Carousel_1', 'productDetails': list, 'quantityCount': 1, 'selectedColour': defaultColour, 'selectedSize': defaultSize }]
              }
              return ({ data: { information: { type: 'carousel', carousel_data: JSON.stringify(carouselResponse) } } });
            })
          );
          let site = JSON.parse(messageList?.personalisationAll?.websiteId);
          let typeRes = messageList.personalisationAll?.plan_id === 54 || messageList.personalisationAll?.plan_id === 32 || messageList.personalisationAll?.plan_id === 97 || messageList.personalisationAll?.plan_id === 96 || messageList.personalisationAll?.plan_id === 103 || messageList.personalisationAll?.plan_id === 107 || messageList.personalisationAll?.plan_id === 120 || messageList.personalisationAll?.plan_id === 121 || messageList.personalisationAll?.plan_id == 122 || messageList.personalisationAll?.plan_id === 123 || messageList.personalisationAll?.plan_id === 124 || messageList.personalisationAll?.plan_id === 116 ? "webcrawl" : "assist";
          let carousalPayloads = {
            messageID: uuidv4(),
            from: messageList.uuid,
            to: messageList?.connectionType?.isBotConnection ? messageList.uuid : `${messageList.agentDomainExt.domainId}_${messageList.agentDomainExt.ext}`,
            sessionId: messageList.uuid,
            carouselDataArr: resultData,
            message: 'carousel',
            sendAt: Date.now(),
            messageType: 'carousel',
            isBot: "IN",
            isCustomer: false,
            sender: '',
            isLike: 0,
            type: typeRes,
            websiteId: site?.length > 0 ? site[0] : "",
            domainId: messageList?.personalisationAll?.domainId,
            isLiveagent: false,
            direction: 'IN',
            ChatHistoryId: parseInt(sessionStorage?.ChatHistoryId),
            ChatDurationHistoryId: parseInt(sessionStorage?.ChatDurationHistoryId),
            socketId: messageList?.socketId,
            assistType: checkConvoSupported() ? "Convo" : "Template",
            supervisorList: messageList?.superVisorExt
          };
          oneToOneChatSend(carousalPayloads);
          let texts = {
            message: carousalPayloads,
          };
          store.dispatch(messageActions.setMessageChat({ carouselText: texts }));
          store.dispatch(messageActions.carouselData(resultData))
        } else {
          const message = "Oops! Looks like the item you're searching for couldn't be found."
          newChatSendMsg("Text", message, "OUT", 0);
          ecommerceTemplateKey(builderList?.ecommercePurchase?.productIntent);
        }
      }
    } else if (builderList?.connectionName === 'WooCommerce') {
      const end_point = `${config()?.WOOCOMMERCE_TOKEN_GENERATOR}/${messageList.personalisationAll.domainId}`
      const res_data = await getIntegrationApiData(end_point, '')
      if (res_data?.result.length > 0) {
        const token = res_data?.result[0]?.token;
        store.dispatch(chatBuilderActions.setEcommercePurchase({ token }));
        const url = config()?.WOOCOMMERCE_PRODUCT_SEARCH_ID;
        let payload = {
          product_id: intent
        }
        const products = await postIntegrationApiData(url, token, payload)
        if (products?.statusCode === 200) {
          if (products?.result?.products?.length > 0) {
            const resultData = await Promise.all(products?.result?.products?.map(async (list) => {
              const sizeVariants = [];
              const colourVariants = [];
              let defaultSize = '';
              let defaultColour = '';
              const url = `${config()?.WOOCOMMERCE_GET_VARIANT}/${list?.id}/variations`;
              const variantProducts = await getIntegrationApiData(url, token)
              list.variantData = variantProducts?.result;
              list?.attributes?.map((v_data) => {
                if (v_data?.name.toLowerCase() === 'color') {
                  colourVariants?.push(v_data?.options)
                } else if (v_data?.name.toLowerCase() === 'size') {
                  sizeVariants?.push(v_data?.options)
                }
              })
              if (sizeVariants?.length === 0) {
                list?.attributes?.map((v_data) => {
                  if (v_data?.options?.length > 0 && v_data?.name.toLowerCase() !== 'color') {
                    sizeVariants?.push(v_data?.options)
                  }
                })
              }
              defaultColour = Array.isArray(colourVariants[0]) ? colourVariants[0][0] : colourVariants[0];
              defaultSize = Array.isArray(sizeVariants[0]) ? sizeVariants[0][0] : sizeVariants[0];
              let carouselResponse = {
                isDynamicPrice: true,
                message: list?.name,
                file_url: list?.images[0]?.src !== undefined ? list?.images[0]?.src : noImage,
                description: list?.description?.replace(/<[^>]*>/g, "") ?? '',
                title: list?.name,
                price: list?.price,
                variantSize: sizeVariants ?? [],
                variantColour: colourVariants ?? [],
                productId: list?.id,
                variantId: list?.variations[0],
                buttons: [{ "value": "Add to cart", 'carouselId': 'yes_Carousel_1', 'productDetails': list, 'quantityCount': 1, 'selectedColour': defaultColour, 'selectedSize': defaultSize }]
              }
              return ({ data: { information: { type: 'carousel', carousel_data: JSON.stringify(carouselResponse) } } });
            }))
            let carousalPayloads = {
              messageID: uuidv4(),
              from: messageList.uuid,
              to: messageList?.connectionType?.isBotConnection ? messageList.uuid : `${messageList.agentDomainExt.domainId}_${messageList.agentDomainExt.ext}`,
              sessionId: messageList.uuid,
              carouselDataArr: resultData,
              message: 'carousel',
              sendAt: Date.now(),
              messageType: 'carousel',
              isBot: "IN",
              isCustomer: false,
              sender: '',
              isLike: 0,
              domainId: messageList?.personalisationAll?.domainId,
              isLiveagent: false,
              direction: 'IN',
              ChatHistoryId: parseInt(sessionStorage?.ChatHistoryId),
              ChatDurationHistoryId: parseInt(sessionStorage?.ChatDurationHistoryId),
              socketId: messageList?.socketId,
              assistType: checkConvoSupported() ? "Convo" : "Template",
              supervisorList: messageList?.superVisorExt
            };
            oneToOneChatSend(carousalPayloads);
            let texts = {
              message: carousalPayloads,
            };
            store.dispatch(messageActions.setMessageChat({ carouselText: texts }));
            store.dispatch(messageActions.carouselData(resultData))
          } else {
            const message = "Oops! Looks like the item you're searching for couldn't be found."
            newChatSendMsg("Text", message, "OUT", 0);
            ecommerceTemplateKey(builderList?.ecommercePurchase?.productIntent);
          }
        }
      }
    }
  } catch (error) {
    console.log(error)
  }
}

// Function for ecommerce recommendation

export const ecommerceTemplateKey = (tempKey) => {
  try {
    if (tempKey?.length > 1) {
      const message = `Here are some additional products you might be interested in.`
      newChatSendMsg("Text", message, "OUT", 0);
      const resultData = tempKey?.map((list) => {
        let datas = {
          type: list,
          childId: 'product_List12'
        }
        return datas;
      })
      store.dispatch(messageActions.templateKeywordList(resultData));
    }
  } catch (err) {
    console.log(err)
  }
}


export const makeApiCall = (async (data, res) => {
  try {
    const messageList = store.getState().message;
    var afterSomeMinutes = new Date(new Date().getTime() + 1 * 30000);
    if (sessionStorage.isConnected !== "bot") {
      let get_agent_payload = {
        domain_id: messageList.personalisationAll?.domainId,
        session_id: messageList.uuid,
        transferCallCount: parseInt(messageList?.transferCallCount),
        callFlowId: messageList.personalisationAll?.chatFlowId,
        timestamp: afterSomeMinutes.getTime(),
        response: 'success',
        channeltype: 'Chat',
        btnValue: data?.id !== undefined ? data?.id : "transferToQueue",
        customerId: messageList.customerFormData?.coid,
        call_type: 'inbound',
        ChatHistoryId: parseInt(sessionStorage.ChatHistoryId),
        ChatDurationHistoryId: parseInt(sessionStorage?.ChatDurationHistoryId),
        //targetLang : messageList.personalisation?.dynamiclanguage !== undefined?messageList.personalisation?.dynamiclanguage :'en',
        liveTransfer: res === "live_agent" ? 1 : 0,
        targetLang: messageList?.targetLang,
        ext: messageList?.liveAgent || 0
      }; 
      chatMessageUpdate(get_agent_payload);
      // messageList.socket.emit("chat_message_update", JSON.stringify(get_agent_payload));
    }
  } catch (err) {
    console.log(err)
  }
});

// Change the session storage details

export const handleConnectionType = (data) => {
  const liveTrackerList = store.getState().liveTracker;
  const messageList = store.getState().message;
  if (data === "agent") {
    let connectionData = {
      isBotConnection: false,
      isAgentConnection: true,
      isDTMFConnection: false,
    };
    sessionStorage.setItem("isConnected", "agent");
    store.dispatch(messageActions.setConnectionType(connectionData));
    let data = {
      ChatHistoryId: sessionStorage?.ChatHistoryId,
      ChatDurationHistoryId: sessionStorage?.ChatDurationHistoryId,
      dispositionId: messageList?.transferDid,
      domainId: messageList.personalisationAll?.domainId
    }
    if (liveTrackerList?.botSessionAvaiableStatus && messageList.connectionType.isBotConnection) {
      botDisconnectedTime(data);
    }
  }

}

// Function make the connection between user and agent

export const availableagent = async (data) => {
  try {
    let agentData = JSON.parse(data);
    const messageList = store.getState().message;
    let agentCustomerData = {
      domainId: agentData.data?.agentDetials[0]?.domainId,
      ext: agentData.data?.agentDetials[0]?.ext,
      webChatSessionId: messageList?.uuid.trim(),
      sender: messageList.customerFormData,
      agentName: agentData.data?.agentDetials[0]?.UserName,
      ChatHistoryId: sessionStorage.ChatHistoryId,
      cookiesId: await getIndexDbItem('uniqueChatId'),
      ChatDurationHistoryId: sessionStorage?.ChatDurationHistoryId,
      chatPlanId: messageList.personalisationAll?.plan_id,
      queueInformation: agentData?.data?.transferTo !== undefined ? agentData?.data?.transferTo : agentData.data?.agentDetials[0]?.queueId,
      queueId: agentData?.data?.transferTo !== undefined ? agentData?.data?.transferTo : agentData.data?.agentDetials[0]?.queueId,
      queueSettings: agentData?.data?.queueSettings !== undefined ? agentData?.data?.queueSettings?.qid : agentData.data?.agentDetials[0]?.queueSettings?.qid,
      widgetName: messageList.personalisationAll.aliasTitle
    };
    jointoLiveAgent(agentCustomerData);
    updateChatSession(messageList.uuid);
    store.dispatch(liveTrackerActions.setLiveChatCutData(agentCustomerData));
  } catch (err) {
    console.log(err)
  }
}

// Function for after agent accept the chat

export const requestAccept = async (message) => {
  const messageList = store.getState().message;
  store.dispatch(liveTrackerActions.setRestrictedEndChat(true));
  let agentNamedispatch = {
    messageID: uuidv4(),
    sessionId: messageList.uuid,
    sendAt: Date.now(),
    agentName: message.agentName,
    isagentname: true,
    isLike: 0
  };
  let agentNameText = {
    message: agentNamedispatch,
  };
  let agentStatus = {
    ext: message.agentExt,
    agentName: message.agentName,
    webChatSessionId: message.to,
    domainId: message.domainId,
  };
  store.dispatch(messageActions.agentDomainExt(agentStatus));
  store.dispatch(messageActions.setChatDurationId(message?.agentChatDurationHistoryId));
  sessionStorage.setItem("ChatDurationHistoryId", message?.agentChatDurationHistoryId);
  store.dispatch(
    messageActions.setMessageChat({ agentNameVisible: agentNameText })
  );
  store.dispatch(messageActions.setAgentConnectingStatus(false));
  store.dispatch(liveTrackerActions.setLiveChatCutData({}));
  let agentCustomerData = {
    domainId: message.domainId,
    ext: message.agentExt,
    webChatSessionId: messageList?.uuid.trim(),
    sender: messageList.customerFormData,
    agentName: message.agentName,
  };
  store.dispatch(messageActions.agentDomainExt(agentCustomerData));
  sessionStorage.setItem("agentToAddress", `${message.domainId}_${message.agentExt}`);
  handleConnectionType("agent");
  //get connected agent details if user refreshed
  updateChatSession(messageList.uuid);
  setTimeout(() => {
    let text = `Welcome${messageList?.personalisationAll?.title ? ` to ${messageList.personalisationAll.title}` : ''}! I am ${message?.agentName || 'Agent'}, ready to assist you further and provide personalized support. Please feel free to share your questions, concerns, or anything you need help with.`
    newChatSendMsg('Text', text, "AGENTINTRO", 1, agentCustomerData);
  }, 1000);
  const updateData = {
    domainId: messageList.personalisationAll?.domainId ?? 0,
    planId: messageList.personalisationAll?.plan_id,
    isBot: 0,
    isAgent: 1,
    IPAdress: messageList.storeIpAddress
  };
  try {
    const response = await fetch(`${config().CORESERVICE_URL}/update_chat_session_count`, {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify(updateData),
    });
    if (!response.ok) {
      const errorData = await response.json();
      console.error('Error updating chat session count:', errorData);
    } else {
      const data = await response.json();
      console.log('Chat session count updated successfully:', data);
    }
  } catch (error) {
    console.error('Network error:', error);
  }
}

// Function deal when the agent decline the chat request and send the emitter for searching another available agent

export const requestDecline = (message) => {
  try {
    const messageList = store.getState().message;
    var afterSomeMinutes = new Date(new Date().getTime() + 1 * 30000);
    store.dispatch(liveTrackerActions.setLiveChatCutData({}));
    if (message?.to !== "") {
      store.dispatch(messageActions.agentDomainExtEmpty());
      sessionStorage.setItem("agentToAddress", {});
      let transferAdd = messageList.transferCallCount + 1;
      store.dispatch(messageActions.transferCallCount(transferAdd));
      console.log("transfer call count");
      let get_agent_payload = {
        domain_id: messageList.personalisationAll?.domainId,
        session_id: messageList.uuid,
        transferCallCount: parseInt(messageList?.transferCallCount),
        callFlowId: messageList.personalisationAll?.chatFlowId,
        timestamp: afterSomeMinutes.getTime(),
        response: 'success',
        channeltype: 'Chat',
        btnValue: messageList?.currentTranferBlock ?? "transferToQueue",
        //btnValue: dtmf_array?.id !== undefined?dtmf_array?.id :"transferToQueue",
        call_type: 'inbound',
        ext: message?.ext,
        ChatHistoryId: parseInt(sessionStorage.ChatHistoryId),
        ChatDurationHistoryId: parseInt(sessionStorage?.ChatDurationHistoryId),
        //targetLang : messageList.personalisation?.dynamiclanguage !== undefined?messageList.personalisation?.dynamiclanguage :'en',
      };
      if (messageList?.liveAgentStatus === 1) {
        get_agent_payload.liveTransfer = 1
      } 
      chatMessageUpdate(get_agent_payload);
    }
  } catch (err) {
    console.log(err)
  }
}


export const getqueueDetails = (data) => {
  store.dispatch(messageActions.storeConnectedQueueData(data.message.data.queueSettings))
}

export const getChatDurationId = (data) => {
  store.dispatch(messageActions.setChatDurationId(data?.ChatDurationHistoryId))
}

const handleTyping = debounce(() => { 
  store.dispatch(liveTrackerActions.setAgentTypingStatus({ status: false, name: '' }));
}, 2000);

export const setAgentTypingStatus = (data) => {
  const messageList = store.getState().message;
  if (messageList?.uuid === data?.to) {
    store.dispatch(liveTrackerActions.setAgentTypingStatus({ status: true, name: data?.sender?.Name }));
    handleTyping();
  }
};

function debounce(func, delay) {
  let timer;
  return function(...args) {
    clearTimeout(timer);
    timer = setTimeout(() => {
      func.apply(this, args);
    }, delay);
  };
}

// const debounce = (func, delay, data) => {
//   let timer;
//   return function (...args) {
//     store.dispatch(liveTrackerActions.setAgentTypingStatus({ status: true, name: data?.sender?.Name }));
//     clearTimeout(timer);
//     timer = setTimeout(() => {
//       func.apply(this, args);
//       store.dispatch(liveTrackerActions.setAgentTypingStatus({ status: false, name: '' }));
//     }, delay);
//   };
// };

// Function for getting node details continous in flow

export const continusBlock = (item) => {
  try {
    const messageList = store.getState().message;
    if (item?.type !== "error") {
      let mesgData = isJsonString(item?.type) ? JSON.parse(item?.type) : [];
      if (mesgData?.length > 0) {
        mesgData?.length > 0 && mesgData.map((list) => {
          newChatSendMsg("Text", list?.data, "OUT", 0);
        });
      } else {
        newChatSendMsg("Text", item?.type, "OUT", 0);
      }
    }
    const data1 = {
      session_id: messageList.uuid,
      domain_id: messageList.personalisationAll?.domainId,
      channelType: 'Chat',
      response: "success",
      btnValue: item?.childId,
      callFlowId: messageList.personalisationAll?.chatFlowId,
      call_type: item?.type,
      messageID: uuidv4(),
      ChatHistoryId: parseInt(sessionStorage.ChatHistoryId),
      ChatDurationHistoryId: parseInt(sessionStorage?.ChatDurationHistoryId),
      inputParams: messageList?.inputEntity,
      //targetLang : messageList.personalisation?.dynamiclanguage !== undefined?messageList.personalisation?.dynamiclanguage :'en',
      targetLang: messageList?.targetLang,
    }; 
    chatMessageUpdate(data1);
  } catch (err) {
    console.log(err)
  }
}

// Function for save the contact details of user

export const saveContact = async (list) => {
  const messageList = store.getState().message;
  const liveTracker = store.getState().liveTracker;
  const dataColumn = [
    'First name', 'Last name', 'Phone number', 'Account number', 'Location', 'Address', 'Country', 'State', 'City', 'Postal code', 'Facebook id', 'Instagram id', 'Whatsapp number', 'Twitter id', 'Email'
  ]
  const notPresentKeys = Object.keys(list)
    .filter(key => !dataColumn.includes(key))
    .reduce((obj, key) => {
      obj[key] = list[key];
      return obj;
    }, {});
  let createObj = {}
  if (Array.isArray(messageList?.customContactFiels)) {
    Object.keys(notPresentKeys)
      .filter(key =>
        messageList?.customContactFiels?.map((list) => {
          if (list?.columnLabel === key) {
            createObj[list.columName] = notPresentKeys[key]
          }
        }))
  };
  const concatenatedKeys = Object.keys(createObj).join('|');
  const concatenatedValues = Object.values(createObj).map(value => `"${value}"`).join('|');
  let data = {
    coid: messageList?.customerFormData?.coid ?? null,
    firstName: list['First name'] ?? null,
    lastName: list['Last name'] ?? null,
    phoneNumber: [],
    email: null,
    primaryPhoneNumber: list['Phone number'],
    primaryEmail: list['Email'],
    domainId: messageList.personalisationAll?.domainId,
    cookiesId: await getIndexDbItem('uniqueChatId'),
    accountNumber: list['Account number'],
    location: list['Location'] ?? null,
    address: list['Address'] ?? null,
    country: list['Country'] ?? null,
    state: list['State'] ?? null,
    city: list['City'] ?? null,
    postalCode: list['Postal code'] ?? null,
    facebook: list['Facebook id'] ?? null,
    instagram: list['Instagram id'] ?? null,
    whatsapp: list['Whatsapp number'] ?? null,
    twitter: list['Twitter id'] ?? null,
    customField: concatenatedKeys ?? null,
    customValues: concatenatedValues ?? null,
    timeZone: Intl.DateTimeFormat().resolvedOptions().timeZone,
    source: messageList?.personalisationAll?.preChartFormFlag == 1 ? "Pre Chat Form" : "API",
    domainId: messageList.personalisationAll?.domainId ?? 0,
    companyId: messageList?.companyId !== '' ? messageList?.companyId : 0,
    plan_id: messageList.personalisationAll?.plan_id ?? 0,
  }
  try {
    const response = await fetch(`${config().CORESERVICE_URL}/insert_update_customer_contact_noauth`, {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify(data),
    });
    const responseData = await response.json();
    if (response.ok) {
      if (responseData?.statusCode === 200) {
        store.dispatch(messageActions.customerFormData({ ...responseData?.insertContactResult, ...list }));
        if (data.source === "Pre Chat Form" && responseData?.insertContactResult?.errcode === 0) {
          let liveTrackData = {
            sesionId: liveTracker?.uuid,
            domainId: messageList?.personalisationAll?.domainId,
            status: "Pre Chat Form"
          }
          siteTracking(liveTrackData)
        }
      } else if (responseData?.statusCode === 403) {
        store.dispatch(messageActions.customerBlockStatus(1));
      } else {
        store.dispatch(messageActions.customerFormData({ ...responseData?.insertContactResult[0], ...list }));
        store.dispatch(messageActions.customerBlockStatus(responseData?.insertContactResult[0]?.blockCustomer));
      }
      contactStorde();
      store.dispatch(messageActions.isprechatFrom(true));
      let dadata = {
        first_name: list['First name'] ?? null,
        last_name: list['Last name'] ?? null,
        email: list?.Email ?? null,
        mobile_no: list['Phone number'] ?? null,
        address: "",
        domainId: messageList.personalisationAll?.domainId ?? 0,
        companyId: messageList?.companyId !== '' ? messageList?.companyId : 0,
        plan_id: messageList.personalisationAll?.plan_id ?? 0,
      };
      leadCrmContact(data);
      // zohoCrm(data) 
    } else {
      console.error('Error response:', responseData);
    }
  } catch (err) {
    console.log('Network error:', err);
  }

}

// Function for calling saveContact function

export const contactStorde = () => {
  const messageList = store.getState().message;
  let ChatHistoryId = sessionStorage?.ChatHistoryId
  let data = {
    ChatHistoryId: parseInt(ChatHistoryId, 10),
    CustomerId: messageList.customerFormData?.coid ?? 0,
    sessionId: sessionStorage.getItem("sessionUniqueId"),
    domainId: messageList.personalisationAll?.domainId,
  }
  if (ChatHistoryId !== null && ChatHistoryId !== undefined) {
    storeCustomerData(data);
    createVisitorsInsights()
  }
}

// Handle tracking the user state

export const getLiveTrackerData = () => {
  const messageList = store.getState().message;
  let getLiveDataStatus = sessionStorage.getItem('status')
  let getLiveData = {
    sesionId: messageList?.uuid,
    domainId: messageList.personalisationAll?.domainId,
    status: getLiveDataStatus
  }
  siteTracking(getLiveData)
}

// Function getting the user device types

export const getBrowserName = () => {
  const userAgent = window.navigator.userAgent;
  const vendor = window.navigator.vendor;
  if (/Opera|OPR/.test(userAgent) || /Opera/.test(vendor)) {
    return 'Opera';
  } else if (/Edg/.test(userAgent) || /Edge/.test(vendor)) {
    return 'Edge';
  } else if (/Chrome/.test(userAgent) || /Google Inc/.test(vendor)) {
    return 'Chrome';
  } else if (/Firefox/.test(userAgent)) {
    return 'Firefox';
  } else if (/Safari/.test(userAgent) && !/Chrome/.test(userAgent)) {
    return 'Safari';
  } else if (/Naver/.test(userAgent)) {
    return 'Naver';
  } else if (/Sogou/.test(userAgent)) {
    return 'Sogou';
  } else if (/Yandex/.test(userAgent)) {
    return 'Yandex';
  } else if (/Baidu/.test(userAgent)) {
    return 'Baidu';
  } else if (/searX/.test(userAgent)) {
    return 'searX';
  } else if (/WolframAlpha/.test(userAgent)) {
    return 'WolframAlpha';
  } else if (/Neeva/.test(userAgent)) {
    return 'Neeva';
  } else if (/Brave/.test(userAgent) && !/Chrome/.test(userAgent) && !/Chromium/.test(userAgent)) {
    return 'Brave';
  } else if (/Wayback Machine/.test(userAgent)) {
    return 'Wayback Machine';
  } else if (/SlideShare/.test(userAgent)) {
    return 'SlideShare';
  } else if (/Twitter/.test(userAgent)) {
    return 'Twitter';
  } else if (/Wiki.com/.test(userAgent)) {
    return 'Wiki.com';
  } else if (/Gibiru/.test(userAgent)) {
    return 'Gibiru';
  } else if (/Swisscows/.test(userAgent)) {
    return 'Swisscows';
  } else if (/Startpage/.test(userAgent)) {
    return 'Startpage';
  } else if (/DuckDuckGo/.test(userAgent)) {
    return 'DuckDuckGo';
  } else if (/Yahoo.com/.test(userAgent)) {
    return 'Yahoo.com';
  } else if (/Bing.com/.test(userAgent)) {
    return 'Bing.com';
  } else if (/Openverse/.test(userAgent)) {
    return 'Openverse';
  } else if (/Yep.com/.test(userAgent)) {
    return 'Yep.com';
  } else if (/You.com/.test(userAgent)) {
    return 'You.com';
  } else {
    return 'unknown';
  }
};



export const isMobileCheck = () => {
  var check = false;
  (function (a) {
    if (/(android|bb\d+|meego).+mobile|avantgo|bada\/|blackberry|blazer|compal|elaine|fennec|hiptop|iemobile|ip(hone|od)|iris|kindle|lge |maemo|midp|mmp|mobile.+firefox|netfront|opera m(ob|in)i|palm( os)?|phone|p(ixi|re)\/|plucker|pocket|psp|series(4|6)0|symbian|treo|up\.(browser|link)|vodafone|wap|windows ce|xda|xiino/i.test(a) || /1207|6310|6590|3gso|4thp|50[1-6]i|770s|802s|a wa|abac|ac(er|oo|s\-)|ai(ko|rn)|al(av|ca|co)|amoi|an(ex|ny|yw)|aptu|ar(ch|go)|as(te|us)|attw|au(di|\-m|r |s )|avan|be(ck|ll|nq)|bi(lb|rd)|bl(ac|az)|br(e|v)w|bumb|bw\-(n|u)|c55\/|capi|ccwa|cdm\-|cell|chtm|cldc|cmd\-|co(mp|nd)|craw|da(it|ll|ng)|dbte|dc\-s|devi|dica|dmob|do(c|p)o|ds(12|\-d)|el(49|ai)|em(l2|ul)|er(ic|k0)|esl8|ez([4-7]0|os|wa|ze)|fetc|fly(\-|_)|g1 u|g560|gene|gf\-5|g\-mo|go(\.w|od)|gr(ad|un)|haie|hcit|hd\-(m|p|t)|hei\-|hi(pt|ta)|hp( i|ip)|hs\-c|ht(c(\-| |_|a|g|p|s|t)|tp)|hu(aw|tc)|i\-(20|go|ma)|i230|iac( |\-|\/)|ibro|idea|ig01|ikom|im1k|inno|ipaq|iris|ja(t|v)a|jbro|jemu|jigs|kddi|keji|kgt( |\/)|klon|kpt |kwc\-|kyo(c|k)|le(no|xi)|lg( g|\/(k|l|u)|50|54|\-[a-w])|libw|lynx|m1\-w|m3ga|m50\/|ma(te|ui|xo)|mc(01|21|ca)|m\-cr|me(rc|ri)|mi(o8|oa|ts)|mmef|mo(01|02|bi|de|do|t(\-| |o|v)|zz)|mt(50|p1|v )|mwbp|mywa|n10[0-2]|n20[2-3]|n30(0|2)|n50(0|2|5)|n7(0(0|1)|10)|ne((c|m)\-|on|tf|wf|wg|wt)|nok(6|i)|nzph|o2im|op(ti|wv)|oran|owg1|p800|pan(a|d|t)|pdxg|pg(13|\-([1-8]|c))|phil|pire|pl(ay|uc)|pn\-2|po(ck|rt|se)|prox|psio|pt\-g|qa\-a|qc(07|12|21|32|60|\-[2-7]|i\-)|qtek|r380|r600|raks|rim9|ro(ve|zo)|s55\/|sa(ge|ma|mm|ms|ny|va)|sc(01|h\-|oo|p\-)|sdk\/|se(c(\-|0|1)|47|mc|nd|ri)|sgh\-|shar|sie(\-|m)|sk\-0|sl(45|id)|sm(al|ar|b3|it|t5)|so(ft|ny)|sp(01|h\-|v\-|v )|sy(01|mb)|t2(18|50)|t6(00|10|18)|ta(gt|lk)|tcl\-|tdg\-|tel(i|m)|tim\-|t\-mo|to(pl|sh)|ts(70|m\-|m3|m5)|tx\-9|up(\.b|g1|si)|utst|v400|v750|veri|vi(rg|te)|vk(40|5[0-3]|\-v)|vm40|voda|vulc|vx(52|53|60|61|70|80|81|83|85|98)|w3c(\-| )|webc|whit|wi(g |nc|nw)|wmlb|wonu|x700|yas\-|your|zeto|zte\-/i.test(a.substr(0, 4)))
      check = true;
  })(navigator.userAgent || navigator.vendor || window.opera);
  return check;

}

export const getDeviceType = () => {
  const userAgent = navigator.userAgent.toLowerCase();
  if (/android/i.test(userAgent)) {
    return 'Android';
  } else if (/iphone|ipad|ipod/i.test(userAgent)) {
    return 'IOS';
  } else if (/iemobile|wpdesktop/i.test(userAgent)) {
    return 'Mobile';
  } else {
    return 'Desktop';
  }
};

export const getOperatingSystem = () => {
  const userAgent = navigator.userAgent;
  if (userAgent.includes("Windows")) {
    return "Windows";
  } else if (userAgent.includes("Mac OS")) {
    return "MacOS";
  } else if (userAgent.includes("Linux")) {
    return "Linux";
  } else {
    return "Unknown";
  }
};

// Google Translate api

export const getLanguageTrans = async (msg, type) => {
  try {
    const messageList = store.getState().message;
    var sourceLang = 'en';
    var targetLang = messageList?.personalisation?.dynamiclanguage;
    let langConvert;
    var url = "https://translate.googleapis.com/translate_a/single?client=gtx&sl=" + sourceLang + "&tl=" + targetLang + "&dt=t&q=" + encodeURIComponent(msg);
    await fetch(url).then(response => response.json())
      .then((jsonData) => {
        langConvert = jsonData[0][0][0];
        let data = { ...messageList?.personalisationAll };
        if (type === 'title') {
          data['title'] = langConvert
          store.dispatch(messageActions.personalisationAll({ ...messageList?.personalisationAll, title: langConvert }));
        } else if (type === 'description') {

          data['description'] = langConvert
          store.dispatch(messageActions.personalisationAll({ ...messageList?.personalisationAll, description: langConvert }));
        }
      })
      .catch((error) => {
        console.error(error)
      })
    return langConvert;
  } catch (err) {
    console.log(err)
  }
}

// Check for intent trigger

export const checkQuesInt = (list) => {
  try {
    const messageList = store.getState().message;
    let isCheckAns = isJsonString(list) && JSON.parse(list);
    let isChecklist = isCheckAns?.length > 0 ? isCheckAns[0] : list
    let queData = messageList?.questionAnswerList ?? [];
    let filtered_array = queData.filter(o =>
      typeof o?.intent === 'string' ? JSON.parse(o?.intent)?.includes(isChecklist) : o?.intent?.includes(isChecklist)
    );
    return filtered_array;
  } catch (err) {
    console.log(err)
  }
}

// Function check the recommendation during the chat flow

export const checkRecommedInt = (list) => {
  store.dispatch(messageActions.setRecommedFlowValue(list));
  const messageList = store.getState().message;
  let queData = messageList?.questionAnswerList;
  let filtered_array = queData.filter(o =>
    typeof o?.intent === 'string' ? JSON.parse(o?.intent)?.some(item => list.includes(item)) : []
  );
  if (filtered_array[0]?.mapFlow?.includes('carousel')) {
    let parentData = messageList?.nodeData.find(o => o?.id === filtered_array[0]?.mapFlow);
    let parentDataId = messageList?.nodeData.find(o => o?.id === parentData?.data?.parentNode);
    store.dispatch(messageActions.currentNodeFlow(parentDataId));
  }
  if (filtered_array?.length > 0 && !messageList?.simOrderSts) {
    store.dispatch(messageActions.setRecommedFlowStatus(true));
    store.dispatch(liveTrackerActions.setRemainderActiveCount());
    let msg = "Would you like to  " + list;
    setTimeout(() => {
      newChatSendMsg("Text", msg, "OUT", 0);
      store.dispatch(messageActions.templateKeywordList(data));
    }, 3000);
    let data = [{
      childId: filtered_array[0]?.mapFlow, type: 'Yes'
    },
    {
      childId: 'recommandFlow_no', type: 'No'
    }]
  }
  return filtered_array;
}

export const checkQuesInt1 = (list) => {
  const messageList = store.getState().message;
  let findAgent12 = messageList.nodeData.find(o => o?.id === list[0]?.mapFlow);
  let filtered_array = findAgent12?.data?.children.filter(o => {
    var arr = JSON.parse(list[0]?.intent);
    return arr.some(x => x.toLowerCase() === o?.type?.toLowerCase());
  });
  return filtered_array;
}

export const isJsonString = (str) => {
  try {
    JSON.parse(str);
  } catch (e) {
    return false;
  }
  return true;
};

// Capture the user details from the aiResponse user input intity

export const multipleEnIntity = (data) => {
  const messageList = store.getState().message;
  let type = messageList?.currentNodeFlow?.data?.information;
  const builderList = store.getState().chatBuilder;
  let queData = data?.User_input_Entity;
  var inputData = [];
  if (queData?.first_name[0]?.length > 0 && type?.inputType === 2) {
    inputData.push({ first_name: queData?.first_name[0] , firstName: queData?.first_name[0]})
  } else if (queData?.first_name[0]?.length > 0 && type?.inputType === 9) {
    inputData.push({ first_name: queData?.first_name[0],firstName: queData?.first_name[0]  })
  } else if (queData?.email[0]?.length > 0 && type?.inputType === 6) {
    inputData.push({ email: queData?.email[0] ,primaryEmail : queData?.email[0] })
  } else if (queData?.address[0]?.length > 0 && type?.inputType === 2) {
    inputData.push({ address: queData?.address[0] })
  } else if (queData?.Sim_count[0]?.length > 0 && queData?.Sim_count[0] < 4 && type?.inputType === 7) {
    inputData.push({ Sim_count: queData?.Sim_count[0] })
  } else if (queData?.mobile_number[0]?.length > 0 && type?.inputType === 5) {
    inputData.push({ mobile_no: queData?.mobile_number[0] , primaryPhoneNumber : queData?.mobile_number[0] })
  } else if (queData?.last_name[0]?.length > 0 && type?.inputType === 2 || queData?.last_name[0]?.length > 0 && type?.inputType === 9) {
    inputData.push({ last_name: queData?.last_name[0] })
  } else if (queData?.postcode[0]?.length > 0) {
    inputData.push({ postcode: queData?.postcode[0] })
  } else if (queData?.otp_code[0]?.length > 0 && type?.inputType === 4) {
    inputData.push({ otp: queData?.otp_code[0] })
  } else if (queData?.plan_prices[0]?.length > 0 && queData?.Sim_count[0]?.length === 0) {
    inputData.push({ amount: queData?.plan_prices[0] })
  } else if (queData?.count[0]?.length > 0 && type?.inputType === 7) {
    inputData.push({ count: queData?.count[0] })
  } else if (queData?.order_id[0]?.length > 0 && type?.inputType === 7) {
    inputData.push({ order_id: queData?.order_id[0] })
  } else if (queData?.country[0]?.length > 0 && type?.inputType === 2) {
    inputData.push({ country: queData?.country[0] })
  } else if (queData?.organisation[0]?.length > 0 && type?.inputType === 2) {
    inputData.push({ organisation: queData?.organisation[0] })
  } else if (queData?.city[0]?.length > 0 && type?.inputType === 2) {
    inputData.push({ city: queData?.city[0] })
  } else if (queData?.email[0]?.length > 0 && messageList?.currentNodeFlow?.data?.nodeId?.startsWith('careHomeAuth') && builderList?.careHomeVerification?.userMail === '') {
    inputData.push({ email: queData?.email[0] })
  } else if (queData?.otp_code[0]?.length > 0 && messageList?.currentNodeFlow?.data?.nodeId?.startsWith('careHomeAuth') && builderList?.careHomeVerification?.userOtp === '') {
    inputData.push({ otp: queData?.otp_code[0] })
  } else if (queData?.website[0]?.length > 0) {
    inputData.push({ url: queData?.website[0] })
  }
  return inputData;
};

export const multipleOtherIntity = (data) => {
  const messageList = store.getState().message;
  let type = messageList?.currentNodeFlow?.data?.information;
  const builderList = store.getState().chatBuilder;
  let queData = data?.User_input_Entity;
  var inputData = [];
  if (queData?.first_name[0]?.length > 0 && queData?.first_name[0] !== 'sim') {
    inputData.push({ first_name: queData?.first_name[0] , firstName : queData?.first_name[0]})
  } else if (queData?.email[0]?.length > 0) {
    inputData.push({ email: queData?.email[0] ,primaryEmail : queData?.email[0] })
  } else if (queData?.address[0]?.length > 0) {
    inputData.push({ address: queData?.address[0] })
  } else if (queData?.Sim_count[0]?.length > 0 && queData?.Sim_count[0] < 4) {
    inputData.push({ Sim_count: queData?.Sim_count[0] })
  } else if (queData?.mobile_number[0]?.length > 0 && type?.inputType === 5) {
    inputData.push({ mobile_no: queData?.mobile_number[0] ,primaryPhoneNumber :  queData?.mobile_number[0] })
  } else if (queData?.last_name[0]?.length > 0) {
    inputData.push({ last_name: queData?.last_name[0] })
  } else if (queData?.postcode[0]?.length > 0) {
    inputData.push({ postcode: queData?.postcode[0] })
  } else if (queData?.otp_code[0]?.length > 0) {
    inputData.push({ otp: queData?.otp_code[0] })
  } else if (queData?.plan_prices[0]?.length > 0) {
    inputData.push({ amount: queData?.plan_prices[0] })
  } else if (queData?.order_id[0]?.length > 0) {
    inputData.push({ order_id: queData?.order_id[0] })
  } else if (queData?.country[0]?.length > 0 && type?.inputType === 2) {
    inputData.push({ country: queData?.country[0] })
  } else if (queData?.organisation[0]?.length > 0) {
    inputData.push({ organisation: queData?.organisation[0] })
  } else if (queData?.city[0]?.length > 0) {
    inputData.push({ city: queData?.city[0] })
  } else if (queData?.count[0]?.length > 0 && type?.inputType === 7) {
    inputData.push({ count: queData?.count[0] })
  } else if (queData?.email[0]?.length > 0 && messageList?.currentNodeFlow?.data?.nodeId?.startsWith('careHomeAuth') && builderList?.careHomeVerification?.userMail === '') {
    inputData.push({ email: queData?.email[0] })
  } else if (queData?.otp_code[0]?.length > 0 && messageList?.currentNodeFlow?.data?.nodeId?.startsWith('careHomeAuth') && builderList?.careHomeVerification?.userOtp === '') {
    inputData.push({ otp: queData?.otp_code[0] })
  } else if (queData?.website[0]?.length > 0) {
    inputData.push({ url: queData?.website[0] })
  }
  return inputData;
};

export const getCardType = (number) => {
  let ddd = "5566772423449304"
  console.log(number, "number")
  const re = {
    electron: /^(4026|417500|4405|4508|4844|4913|4917)\d+$/,
    maestro: /^(5018|5020|5038|5612|5893|6304|6759|6761|6762|6763|0604|6390)\d+$/,
    dankort: /^(5019)\d+$/,
    interpayment: /^(636)\d+$/,
    unionpay: /^(62|88)\d+$/,
    visa: /^4[0-9]{12}(?:[0-9]{3})?$/,
    mastercard: /^(5[1-5][0-9]{14}|2(22[1-9][0-9]{12}|2[3-9][0-9]{13}|[3-6][0-9]{14}|7[0-1][0-9]{13}|720[0-9]{12}))$/,
    amex: /^3[47][0-9]{13}$/,
    diners: /^3(?:0[0-5]|[68][0-9])[0-9]{11}$/,
    discover: /^6(?:011|5[0-9]{2})[0-9]{12}$/,
    jcb: /^(?:2131|1800|35\d{3})\d{11}$/
  }

  for (var key in re) {
    if (re[key].test(number)) {
      return key
    }
  }

  return 'unknown'
}

export const paymentEntry = (datares) => {
  const messageList = store.getState().message;
  const screenWidth = window.screen.width;
  const screenHeight = window.screen.height;
  const userAgent = navigator.userAgent;
  const language = navigator.language;
  const isJavaEnabled = navigator.javaEnabled();
  let data = {
    "p_transactionId": datares?.server_trans_id,
    "p_transaction_status": "Pending",
    "p_jsoncol": {
      "cvv": messageList?.cardDetails[0]?.cvv,
      "device_type": "mobile",
      "browser": "Mobile Safari-15.6.1",
      "os_version": "iOS",
      "topup_url": "vmbundle/subscribe/MTFAQDE4MzE=",
      "bundle_id": "1831",
      "expirydate": `20${messageList?.cardDetails[0].expiryYear}${messageList?.cardDetails[0].expiryMonth}`,
      "cardtype": messageList?.cardDetails[0]?.cardType.toUpperCase(),
      "productcode": "EBUN",
      "sitecode": "MCM",
      "applicationcode": "VMUK",
      "paymentagent": "VMUK",
      "servicetype": "RES",
      "paymentmode": "DEF",
      "accountid": messageList?.inputEntity[0].mobile_no,
      "email": "Lsinghjudo@hotmail.com",
      "IPpaymentagent": "188.31.1.248",
      "authentication_source": "BROWSER",
      "authentication_request_type": "PAYMENT_TRANSACTION",
      "message_category": "PAYMENT_AUTHENTICATION",
      "message_version": "2.2.0",
      "challenge_request_indicator": "NO_PREFERENCE",
      // "challenge_request_indicator": "CHALLENGE_PREFERRED",
      "server_trans_id": datares?.server_trans_id,
      "card_detail": {
        "number": messageList?.cardDetails[0]?.cardNumber,
        "scheme": messageList?.cardDetails[0]?.cardType.toUpperCase(),
        "expiry_month": messageList?.cardDetails[0].expiryMonth,
        "expiry_year": messageList?.cardDetails[0].expiryYear,
        "full_name": messageList?.cardDetails[0].holderName
      },
      "order": {
        "date_time_created": new Date().toLocaleString("en-GB", { timeZone: 'UTC' }),
        "amount": (messageList?.inputEntity[1].amount * 100),
        "currency": "GBP",
        "id": "447574515951-90870",
        "address_match_indicator": "FALSE",
        "shipping_address": {
          "line1": messageList?.cardDetails[0]?.address,
          "line2": "",
          "line3": "",
          "city": messageList?.cardDetails[0]?.city,
          "postal_code": messageList?.cardDetails[0]?.postalCode,
          "state": "UK",
          "country": "826"
        }
      },
      "payer": {
        "email": "Lsinghjudo@hotmail.com",
        "billing_address": {
          "line1": messageList?.cardDetails[0]?.address,
          "line2": "",
          "line3": "",
          "city": messageList?.cardDetails[0]?.city,
          "postal_code": messageList?.cardDetails[0]?.postalCode,
          "country": "826"
        },
        "mobile_phone": {
          "country_code": "44",
          "subscriber_number": messageList?.inputEntity[0].mobile_no,
        }
      },
      "challenge_notification_url": "https://liveurapi.worktual.co.uk/vectone-myaccount/v1/user/callback_url_v1",
      "method_url_completion": "YES",
      "browser_data": {
        "accept_header": "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8",
        "color_depth": "32",
        "ip": "188.31.1.248",
        "java_enabled": `${isJavaEnabled}`,
        "javascript_enabled": "true",
        "language": `${language}`,
        "screen_height": `${screenHeight}`,
        "screen_width": `${screenWidth}`,
        "challenge_window_size": "FULL_SCREEN",
        "timezone": "-60",
        "user_agent": `${userAgent}` //"Mozilla/5.0 (iPhone; CPU iPhone OS 15_6_1 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/15.6.1 Mobile/15E148 Safari/604.1"
      },
      "merchant_contact_url": "https://www.vectonemobile.co.uk/vmcontact/details",
      "website": window.location.href
    }
  };
  console.log(data, "datadatadata")
  fetch(`https://liveurapi.worktual.co.uk/vectone-myaccount/v1/user/create_json_transaction_data`, {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json',
    },
    body: JSON.stringify(data)
  })
    .then(response => {
      if (response.ok) {
        return response.json();
      } else {
        throw new Error(`Error: ${response.status}`);
      }
    })
    .then(responseData => {
      console.log(responseData, "response");
      store.dispatch(messageActions.formgenerate(true));
      document.getElementById("formButton").click();
    })
    .catch(error => {
      console.error('Fetch error:', error);
    });

  newChatSendMsg("Text", "Your payment is processing. Please wait for a moment until the transaction is complete", "OUT", 0);
};


export const getPaymentSuccess = () => {
  const messageList = store.getState().message;
  let queryParams = new URLSearchParams(window.location.search);
  let transaction_id = queryParams.get("transaction_id");
  let data = {
    "p_transactionId": transaction_id // messageList?.paymentData?.server_trans_id
  }
  fetch(`https://liveurapi.worktual.co.uk/vectone-myaccount/v1/user/get_json_transaction_data`, {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json',
    },
    body: JSON.stringify(data)
  })
    .then(response => {
      if (response.ok) {
        return response.json();
      } else {
        throw new Error(`Error: ${response.status}`);
      }
    })
    .then(responseData => {
      console.log(responseData, "response");
      if (responseData?.results[0]?.transaction_status.toLowerCase() === "success") {
        store.dispatch(messageActions.transactionData(responseData?.results));
        store.dispatch(messageActions.formgenerate(false));
      } else if (responseData?.results[0]?.transaction_status === "CHALLENGE_REQUIRED") {
        store.dispatch(messageActions.secondTransactionData(responseData?.results));
        console.log(messageList?.secondTransactionData, "messageList");

        if (messageList?.secondTransactionData?.length > 0 && !messageList?.secondformgenerate) {
          document.getElementById("formButtonn").click();
          store.dispatch(messageActions.secondformgenerate(true));
          store.dispatch(messageActions.formgenerate(false));
        }
      }
    })
    .catch(error => {
      console.error('Fetch error:', error);
    });
}

export const chatEndMsg = (msg, type) => {
  const messageList = store.getState().message;
  const liveTrackerList = store.getState().liveTracker;
  let payload = {
    "question": msg,
    "sessionId": sessionStorage?.sessionUniqueId,
    "bot_id": 2,
    "botType": "convo",
    "domainId": messageList?.personalisationAll?.domainId,
    "websiteId": "",
    "ipaddress": messageList?.storeIpAddress,
    "company_name": messageList?.personalisationAll?.title !== undefined ? messageList?.personalisationAll?.title : "",
    "isClose": type,
    "do_stream": true,
    "socketId": messageList?.socketId,
    "industry_type": messageList?.industry_type,
    // "chatId": messageList?.personalisationAll?.chatFlowId,
    "chatId": parseInt(messageList?.personalisationAll?.domainId) === 17820 ? 346954 : messageList?.personalisationAll?.chatFlowId,
    "isTemplate": 0,
    "language": liveTrackerList?.selectedTextLanguage?.code,
    'worktualCookieStatus': 0,
    'clientLocation': messageList?.clientLocation,
    'chatHistoryId': parseInt(sessionStorage?.ChatHistoryId ?? 0),
    'customerId' : messageList?.customerFormData?.coid ?? 0
  }
  sendChatBotReq(payload);
}

// Function for chat call back

export const callbackMessage = () => {
  const messageList = store.getState().message;
  let msgContent = `All right! Provide a overview of what you are looking for with your contact details in the message box below. I appreciate you allowing me to assist you. Have an amazing day!`
  newChatSendMsg("Text", msgContent, "OUT", 1);
  let data = {
    ChatHistoryId: parseInt(messageList?.chatHistoryIdSlice),
    domainId: messageList?.personalisationAll?.domainId
  }
  updateCallbackData(data)
  store.dispatch(messageActions.callBackFormStatus(1));
  sessionStorage.setItem("isConnected", "callback");
  let connectionData = {
    isBotConnection: false,
    isAgentConnection: false,
    isDTMFConnection: false,
  };
  store.dispatch(messageActions.setConnectionType(connectionData));
}

// Function for Agent domain details when searching for available agent

export const agentDomainStatus = (dataRes) => {
  const messageList = store.getState().message;
  let data = {
    isBotConnection: true,
    isAgentConnection: false,
    isDTMFConnection: false,
  };
  store.dispatch(messageActions.setConnectionType(data));
  sessionStorage.setItem("isConnected", "bot");
  if (dataRes?.domainStatus && dataRes?.domainStatus?.length > 0) {
    let msgRes = dataRes?.domainStatus[0]?.userStatus === "Unavailable" ? messageList?.automatedResponse?.unavilableMessage : messageList?.automatedResponse?.busyMessage;
    store.dispatch(messageActions.setCallbackAgentStatus(dataRes?.domainStatus[0]?.userStatus));
    if (dataRes?.domainStatus[0]?.userStatus === "Unavailable" && messageList?.automatedResponse?.isUnavailableCallbck === true) {
      store.dispatch(messageActions.setDynamicChatFormList(messageList?.automatedResponse?.chatCallbackUnavaiableFields))
      store.dispatch(messageActions.setCallbackBtnStatus(true))
    } else if (dataRes?.domainStatus[0]?.userStatus === "Busy" && messageList?.automatedResponse?.isBusyCallbck === true) {
      store.dispatch(messageActions.setDynamicChatFormList(messageList?.automatedResponse?.chatCallbackFields))
      store.dispatch(messageActions.setCallbackBtnStatus(true))
    }
    setTimeout(async () => {
      newChatSendMsg("Text", msgRes, "OUT", 1);
      if (messageList.personalisationAll?.plan_id === 54 || messageList.personalisationAll?.plan_id === 32 || messageList.personalisationAll?.plan_id === 116) {
        store.dispatch(messageActions.callBackButtonStatus(0))
      } else {
        store.dispatch(messageActions.callBackButtonStatus(1))
      }

    }, 1500);
    sessionStorage.setItem("isConnected", "");

  }
}

// Contact store api

export const createContactApi = async () => {
  const messageList = store.getState().message;
  const object = Object.assign({}, ...messageList?.customerData);
  if ((object?.email !== "" || object?.mobile_no !== "") && (object?.email !== undefined || object?.mobile_no !== undefined)) {
    let data = {
      coid: messageList?.customerFormData?.coid ?? 0,
      firstName: object?.first_name ?? null,
      lastName: object?.last_name ?? null,
      phoneNumber: [],
      email: null,
      primaryPhoneNumber: object?.mobile_no,
      primaryEmail: object?.email,
      domainId: messageList.personalisationAll?.domainId,
      cookiesId: await getIndexDbItem('uniqueChatId'),
      address: object?.address ?? null,
      timeZone: Intl.DateTimeFormat().resolvedOptions().timeZone,
    }
    fetch(`${config().CORESERVICE_URL}/insert_update_customer_contact_noauth`, {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify(data)
    })
      .then(response => {
        if (!response.ok) {
          throw new Error(`HTTP error! status: ${response.status}`);
        }
        return response.json();
      })
      .then(responseData => {
        if (responseData?.statusCode === 200) {
          store.dispatch(messageActions.customerFormData(responseData?.insertContactResult));
          store.dispatch(messageActions.customerBlockStatus(responseData?.insertContactResult?.blockCustomer));
        } else {
          store.dispatch(messageActions.customerFormData(responseData?.insertContactResult[0]));
          store.dispatch(messageActions.customerBlockStatus(responseData?.insertContactResult[0]?.blockCustomer));
        }
      })
      .catch(error => {
        console.error('Fetch error:', error);
      });

    let dadata = {
      first_name: object?.first_name ?? null,
      last_name: object?.last_name ?? null,
      email: object?.email ?? null,
      mobile_no: object?.mobile_no ?? null,
      address: object?.address ?? null,
      domainId: messageList.personalisationAll?.domainId ?? 0,
      companyId: messageList?.companyId !== '' ? messageList?.companyId : 0,
      plan_id: messageList.personalisationAll?.plan_id ?? 0
    }
    leadCrmContact(dadata);
  }
  return true;
}

export const storageCustomerIntity = (data) => {
  const messageList = store.getState().message;
  let queData = data?.User_input_Entity;
  var inputData = [];
  if (queData?.first_name[0]?.length > 0 && queData?.first_name[0] !== 'sim') {
    inputData.push({ first_name: queData?.first_name[0] })
  } else if (queData?.email[0]?.length > 0) {
    inputData.push({ email: queData?.email[0] })
  } else if (queData?.address[0]?.length > 0) {
    inputData.push({ address: queData?.address[0] })
  } else if (queData?.mobile_number[0]?.length > 0) {
    inputData.push({ mobile_no: queData?.mobile_number[0] })
  } else if (queData?.last_name[0]?.length > 0) {
    inputData.push({ last_name: queData?.last_name[0] })
  } else if (queData?.address[0]?.length > 0) {
    inputData.push({ address: queData?.address[0] })
  }
  if (inputData?.length > 0) {
    let dataRes = [...messageList?.customerData, ...inputData]
    console.log(dataRes, "dataRes")
    store.dispatch(messageActions.customerData(dataRes))
  }
}

// Function for reaminder the flow user disconnect previously

export const startRemainderFlow = (agentStatus) => {
  const messageList = store.getState().message;
  if (messageList?.remainderAction?.isRemainder && !agentStatus && !messageList?.simOrderSts) {
    store.dispatch(messageActions.remainderAction({ isRemainder: true, count: messageList?.remainderAction?.count + 1 }))
    if (messageList?.remainderAction?.count > 1) {
      store.dispatch(messageActions.setRecommedFlowStatus(true));
      // let msgdata = "Would you like to continue the " + (messageList?.flowBreakParentnode?.type !== undefined ? messageList?.flowBreakParentnode?.type : " flow") + "?";
      let msgdata = `Would you like to continue the ${messageList?.flowBreakParentnode?.type ? messageList?.flowBreakParentnode?.type : 'this'} process from where we left?`;

      setTimeout(() => {
        if (!messageList?.simOrderSts) {
          newChatSendMsg("Text", msgdata, "OUT", 0);
          store.dispatch(messageActions.templateKeywordList(data));
        }
      }, 3000)
      let data = [{
        childId: 'remainderFlow_yes', type: 'Yes'
      },
      {
        childId: 'remainderFlow_no', type: 'No'
      }]
      store.dispatch(messageActions.simOrderSts(true));
    }
  } else {
    store.dispatch(messageActions.remainderAction({ isRemainder: false, count: 0 }))
  }
}

export const storageAICustomerIntity = (data) => {
  const messageList = store.getState().message;
  let condRes = !data?.User_input_Entity || (typeof data.User_input_Entity === 'object' && Object.keys(data.User_input_Entity).length === 0);
  if (!condRes) {
    let queData = data?.User_input_Entity;
    var inputData = [];
    if (queData?.name !== '' && queData?.mobile[0]?.length > 0 && queData?.email[0]?.length > 0) {
      inputData.push({ first_name: queData?.name, email: queData?.email[0], mobile_no: queData?.mobile[0] })
    } else if (queData?.name !== '' && queData?.mobile[0]?.length > 0) {
      inputData.push({ first_name: queData?.name, mobile_no: queData?.mobile[0] })
      // } else if (queData?.name !== '') {
      //   inputData.push({ first_name: queData?.name })
    } else if (queData?.first_name !== '' && queData?.last_name !== '' && queData?.email[0]?.length > 0) {
      inputData.push({ first_name: queData?.first_name, email: queData?.email[0], last_name: queData?.last_name })
    } else if (queData?.first_name !== '') {
      inputData.push({ first_name: queData?.first_name })
    } else if (queData?.last_name !== '') {
      inputData.push({ last_name: queData?.last_name })
    } else if (queData?.email[0]?.length > 0) {
      inputData.push({ email: queData?.email[0] })
    } else if (queData?.address !== "") {
      inputData.push({ address: queData?.address })
    } else if (queData?.mobile[0]?.length > 0) {
      inputData.push({ mobile_no: queData?.mobile[0] })
    }
    if (inputData?.length > 0) {
      let dataRes = [...messageList?.customerData, ...inputData];
      store.dispatch(messageActions.customerData(dataRes));
      setTimeout(() => {
        createContactApi()
      }, 2000)
    }
  }
}

export const breakFlowMsg = () => {
  const messageList = store.getState().message;
  let flowmsg = messageList?.flowBreakMsg;
  let checkCond = checkQuesInt(flowmsg?.ans_intent);
  if (checkCond?.length > 0) {
    QueAnsDummyFlow(messageList.personalisationAll.domainId, flowmsg);
  } else {
    newChatSendMsg("Text", flowmsg?.que_answer, "OUT", 1);
  }
  if (flowmsg?.ans_intent === 'transfer_live_agent') {
    store.dispatch(messageActions.setAgentConnectingStatus(true));
    store.dispatch(messageActions.templateKeywordList([]));
    store.dispatch(messageActions.liveAgentStatus(1));
    setTimeout(async () => {
      handleConnectionType("agent");
      makeApiCall({}, "live_agent");
    }, 1500);
  }
}

export const paymentStatus = (message) => {
  if (message?.result === "00") {
    store.dispatch(messageActions.paymentEnable(false));
    store.dispatch(messageActions.paymentSource(""));
    store.dispatch(messageActions.paymentForm(false));
    let msg = "Your payment was successful. Your transaction reference ID is " + message?.orderId + ". Thank you";
    newChatSendMsg("Text", msg, "OUT", 0);

  } else {
    store.dispatch(messageActions.paymentEnable(false));
    store.dispatch(messageActions.paymentSource(""));
    store.dispatch(messageActions.paymentForm(false));
    let msg = "Payment failed. Please check your card details and try again or use an alternative payment method";
    newChatSendMsg("Text", msg, "OUT", 0);
  }

}

export const getDispostionRes = (data) => {
  store.dispatch(messageActions.setTransferDid(data?.did));
}


export const unResponsiveQAStore = (data) => {
  const messageList = store.getState().message;
  let site = JSON.parse(messageList?.personalisationAll?.websiteId);
  if (data?.unresponse_key === 1) {
    let payload = {
      chatFlowId: messageList?.callFlowId,
      domainId: data?.domainId,
      websiteId: site?.length > 0 ? site[0] : "",
      questionAnswer: {
        Question: data?.unresponse_query,
        Answer: data?.que_answer,
        domainId: data?.domainId,
        sessionId: data?.sessionId,
        created_At: new Date(),
        isLike: 0,
        ChatHistoryId: parseInt(sessionStorage?.ChatHistoryId),
        ChatDurationHistoryId: parseInt(sessionStorage?.ChatDurationHistoryId),
      }
    }
    getunresponsiveQA(payload);
  }
}

export const unhelpfulResponsiveStore = (data) => {
  const messageList = store.getState().message;
  let site = JSON.parse(messageList?.personalisationAll?.websiteId);

  let payload = {
    chatFlowId: messageList?.callFlowId,
    domainId: data?.message?.domainId,
    websiteId: site?.length > 0 ? site[0] : "",
    questionAnswer: {
      Question: data?.message?.message,
      Answer: data?.message?.message,
      domainId: data?.message?.domainId,
      sessionId: data?.message?.sessionId,
      created_At: new Date(),
      isLike: 1,
      ChatHistoryId: parseInt(sessionStorage?.ChatHistoryId),
      ChatDurationHistoryId: parseInt(sessionStorage?.ChatDurationHistoryId),
    }
  }
  getunresponsiveQA(payload);
}

export const socketSession = (data) => {
  store.dispatch(messageActions.socketId(data));
}

export const storeSupervisorExt = async (data) => {
  const messageList = store.getState().message;
  const supervisorList = await data?.map((list) => {
    return `${messageList.personalisationAll.domainId}_${list?.ext}`
  })
  store.dispatch(messageActions.setSupervisorList(supervisorList))
}


export const getchatFlowFind = async (flowId, domainId) => {
  let body = {
    cfid: flowId,
    domainId: domainId,
  };
  try {
    const responsePost = await fetch(`${config().CALLSERVICE_URL}/get_chatflow_nodes_list`, {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify(body),
    });

    if (!responsePost.ok) {
      throw new Error(`HTTP error! status: ${responsePost.status}`);
    }

    const dataPost = await responsePost.json();
    let nodeData = JSON.parse(dataPost.result[0].nodes);
    store.dispatch(messageActions.storeNodeData(nodeData));
    store.dispatch(messageActions.setCompanyDetails(dataPost.result[0].companyId));

    console.log(nodeData, "nodeData");
    nodeData.forEach((list) => {
      if (list.type === "customInput") {
        store.dispatch(messageActions.setDtmfData(list.data));
      }
    });
    const responseGet = await fetch(`${config().CORESERVICE_URL}/get_question_answer_list?limit=99&offset=0&domainId=${domainId}&chatFlowId=${flowId}`);

    if (responseGet.ok) {
      const dataGet = await responseGet.json();
      store.dispatch(messageActions.questionAnswerList(dataGet.data));
    }

  } catch (error) {
    console.error('Fetch error:', error);
  }
};


// get ipaddress details
export const getipAddressDetails = async () => {
  try {
    const response = await fetch('https://getip.worktual.co.uk/');
    if (!response.ok) {
      throw new Error(`HTTP error! status: ${response.status}`);
    }
    const data = await response.json();
    return data.clientIPv6 ?? null;

  } catch (error) {
    console.error('Error fetching IP address details:', error);
    throw error;
  }
};


// get location details
export const getLocationDetails = async (data) => {
  try {
    const response = await fetch(`${config().CALLSERVICE_URL}/get_location_details/${data}`);
    if (!response.ok) {
      console.error('Error fetching data:', response);
      throw new Error('Network response was not ok: ' + response.statusText);
    }
    const result = await response.json();
    if (result?.statusCode === 200) {
      return result.getlocationResponse ?? null;
    } else {
      console.error('Invalid response format:', result);
      throw new Error('Invalid response format');
    }
  } catch (error) {
    console.error('Error fetching data:', error);
    throw error;
  }
};



export const getNodeEntryPoint = async (ipAddress, domainId) => {
  try {
    const requestData = {
      multiChatId: [123],
      domainId: domainId,
      ipAddress: ipAddress,
    };
    const response = await fetch(`${config().CORESERVICE_URL}/get_nodes_entry_point`, {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify(requestData),
    });
    if (!response.ok) {
      console.error('Error fetching data:', response);
      throw new Error('Network response was not ok: ' + response.statusText);
    }
    const result = await response.json();

    if (result?.statusCode === 200) {
      return result?.data?.ipExistinguser[0][0]?.isExists ?? null;
    } else {
      console.error('Invalid response format:', result);
      throw new Error('Invalid response format');
    }
  } catch (error) {
    console.error('Error fetching data:', error);
    throw error;
  }
};


// get all agent list
export const getAgentAvailibility = async (domainId) => {
  try {
    const requestData = {
      domainId: domainId,
    };
    const response = await fetch(`${config().CALLSERVICE_URL}/get_agent_availability`, {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify(requestData),
    });
    if (!response.ok) {
      console.error('Error fetching data:', response);
      throw new Error('Network response was not ok: ' + response.statusText);
    }
    const result = await response.json();
    if (result?.statusCode === 200) {
      return result.getAgentResponse ?? null;
    } else {
      console.error('Invalid response format:', result);
      throw new Error('Invalid response format');
    }
  } catch (error) {
    console.error('Error fetching data:', error);
    throw error;
  }
};


export const updateChatSessionCount = async () => {
  const messageList = store.getState().message;
  const updateData = {
    domainId: messageList.personalisationAll?.domainId ?? 0,
    planId: messageList.personalisationAll?.plan_id,
  };

  try {
    const response = await fetch(`${config().CORESERVICE_URL}/update_chat_session_count`, {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify(updateData),
    });

    if (!response.ok) {
      console.error('Error updating chat session count:', response);
      throw new Error('Network response was not ok: ' + response.statusText);
    }

    const result = await response.json(); // Parse the JSON response
    console.log('Chat session count updated successfully:', result);
  } catch (error) {
    console.error('Error updating chat session count:', error);
  }
};


export const storeSelectedItems = (productDetails, selectColour, selectSize, quantityCount, overAllData) => {
  productDetails?.variants?.some((list) => {
    list.pro_currency_type = overAllData?.currencySign ?? '£'
    list.pro_price = overAllData?.price ?? list?.price
    if (selectColour !== '' && selectColour !== undefined && selectSize !== undefined && selectSize !== '') {
      if ((list?.option1 === selectSize || list?.option2 === selectSize || list?.option3 === selectSize) &&
        (list?.option1 === selectColour || list?.option2 === selectColour || list?.option3 === selectColour)) {
        list.quantityCount = quantityCount;
        list.imageUrl = overAllData?.file_url;
        list.imgTitle = overAllData?.title;
        store.dispatch(chatBuilderActions.setSelectedCardItems(list));
        return true;
      }
    } else if (selectColour !== '' && selectColour !== undefined) {
      if ((list?.option1 === selectColour || list?.option2 === selectColour || list?.option3 === selectColour)) {
        list.quantityCount = quantityCount;
        list.imageUrl = overAllData?.file_url;
        list.imgTitle = overAllData?.title;
        store.dispatch(chatBuilderActions.setSelectedCardItems(list));
        return true;
      }
    } else if (selectSize !== '' && selectSize !== undefined) {
      if ((list?.option1 === selectSize || list?.option2 === selectSize || list?.option3 === selectSize)) {
        list.quantityCount = quantityCount;
        list.imageUrl = overAllData?.file_url;
        list.imgTitle = overAllData?.title;
        store.dispatch(chatBuilderActions.setSelectedCardItems(list));
        return true;
      }
    } else {
      list.quantityCount = quantityCount;
      list.imageUrl = overAllData?.file_url;
      list.imgTitle = overAllData?.title;
      store.dispatch(chatBuilderActions.setSelectedCardItems(list));
      return true;
    }

  })
}

export const checkoutProductStock = async () => {
  const builderList = store.getState().chatBuilder;
  if (builderList?.ecommercePurchase?.recommendedIntent?.length > 0) {
    const validIntent = [];
    if (builderList?.connectionName === 'shopify') {
      await Promise.all(builderList?.ecommercePurchase?.recommendedIntent?.map(async (list) => {
        const url = `${config()?.SHOPIFY_PRODUCT_SEARCH}`
        let selectedArray = builderList?.ecommercePurchase?.nlpRecommended[list];
        let payload = {
          product_id: selectedArray,
          priceCondition: "",
          price: 0,
          toPrice: 0,
          currencyType: builderList?.ecommercePurchase?.currencyType ?? ''
        }
        const products = await postIntegrationApiData(url, builderList?.ecommercePurchase?.token, payload)
        // const products = await getIntegrationApiData(url, builderList?.ecommercePurchase?.token);
        if (products?.result?.length > 0) {
          validIntent.push(list)
        }
      }))
    } else if (builderList?.connectionName === 'Wix') {
      await Promise.all(builderList?.ecommercePurchase?.recommendedIntent?.map(async (list) => {
        // const url = `${config()?.WIX_PRODUCT_SEARCH}/${list}`
        // const products = await getIntegrationApiData(url, builderList?.ecommercePurchase?.token);
        let selectedArray = builderList?.ecommercePurchase?.nlpRecommended[list];
        const url = config()?.WIX_PRODUCT_SEARCH_ID;
        let payload = {
          product_id: selectedArray
        }
        const products = await postIntegrationApiData(url, builderList?.ecommercePurchase?.token, payload)
        if (products?.result?.products?.length > 0) {
          validIntent.push(list)
        }
      }))
    } else if (builderList?.connectionName === 'BigCommerce') {
      await Promise.all(builderList?.ecommercePurchase?.recommendedIntent?.map(async (list) => {
        const url = `${config()?.BIGCOMMERCE_SEARCH_LIST}/${list}`
        const products = await getIntegrationApiData(url, builderList?.ecommercePurchase?.token)
        if (products?.statusCode === 200) {
          if (products?.result?.length > 0) {
            for (let data of products?.result?.data) {
              let url = `${config()?.BIGCOMMERCE_GET_VARIANTS}/${data?.id}/variant`
              const variants = await getIntegrationApiData(url, builderList?.ecommercePurchase?.token);
              if (variants) {
                validIntent.push(list)
              }
            }
          }
        }
      }))
    } else if (builderList?.connectionName === 'Magento') {
      await Promise.all(builderList?.ecommercePurchase?.recommendedIntent?.map(async (list) => {
        const url = `${config()?.MAGENTO_PRODUCT_SEARCH}/${list}`
        const products = await getIntegrationApiData(url, builderList?.ecommercePurchase?.token);
        if (products?.result?.items?.length > 0) {
          validIntent.push(list)
        }
      }))
    } else if (builderList?.connectionName === 'WooCommerce') {
      await Promise.all(builderList?.ecommercePurchase?.recommendedIntent?.map(async (list) => {

        const url = config()?.WOOCOMMERCE_PRODUCT_SEARCH_ID;
        let selectedArray = builderList?.ecommercePurchase?.nlpRecommended[list];
        let payload = {
          product_id: selectedArray
        }
        const products = await postIntegrationApiData(url, builderList?.ecommercePurchase?.token, payload)
        if (products?.result?.products?.length > 0 && products?.statusCode === 200) {
          validIntent.push(list)
        }
      }))
    }
    if (validIntent?.length === 1) {
      const recomMsg = `Hi there! Based on your preferences, we recommend below products`
      newChatSendMsg('Text', recomMsg, "OUT", 0);
      if (builderList?.connectionName === 'shopify' || builderList?.connectionName === 'Wix' || builderList?.connectionName === 'WooCommerce') {
        const selectedArray = builderList?.ecommercePurchase?.nlpRecommended[validIntent[0]];
        ecommerceProdcutSearch(selectedArray)
      } else if (builderList?.connectionName === 'BigCommerce' || builderList?.connectionName === 'Magento') {
        ecommerceProdcutSearch(validIntent[0])
      }

    } else if (validIntent?.length === 0) {
      let recommendMsg = `Could you specify which product you are interested in?`;
      newChatSendMsg('Text', recommendMsg, "OUT", 0);
    } else {
      const recomMsg = `Could you choose which one you are interested in?`
      newChatSendMsg('Text', recomMsg, "OUT", 0);

      if (builderList?.connectionName === 'shopify' || builderList?.connectionName === 'Wix' || builderList?.connectionName === 'WooCommerce') {
        const combinedArray = validIntent.reduce((acc, key) => {
          if (builderList?.ecommercePurchase?.nlpRecommended[key]) {
            acc.push(...builderList?.ecommercePurchase?.nlpRecommended[key]);
          }
          return acc;
        }, []);
        combinedArray.sort(() => Math.random() - 0.5);
        ecommerceProdcutSearch(combinedArray)
      } else {
        store.dispatch(chatBuilderActions.setEcommercePurchase({ productIntent: validIntent }));
        const resultData = validIntent?.map((list) => {
          let datas = {
            type: list,
            childId: 'product_List12'
          }
          return datas;
        })
        store.dispatch(messageActions.templateKeywordList(resultData));
      }
    }
    store.dispatch(chatBuilderActions.setEcommercePurchase({ recommendedIntent: [] }));
  }
}

export const checkValidProductEnquiry = async (list) => {
  const messageList = store.getState().message
  const builderList = store.getState().chatBuilder
  if (builderList?.connectionName === 'shopify') {
    const end_point = `${config()?.SHOPIFY_CONNECTION_LIST}/${messageList.personalisationAll.domainId}`
    const res_data = await getIntegrationApiData(end_point, '')
    if (res_data?.result.length > 0) {
      const token = res_data?.result[0]?.token;
      store.dispatch(chatBuilderActions.setEcommercePurchase({ token }));
      const stockIntent = [];
      const outOfStockIntent = []
      await Promise.all(list?.map(async (data) => {
        // const url = `${config()?.SHOPIFY_TOKEN_GENERATOR}/${data}`
        const url = `${config()?.SHOPIFY_PRODUCT_SEARCH}`
        let selectedArray = builderList?.ecommercePurchase?.nlpProductIntent[data];
        let payload = {
          product_id: selectedArray,
          priceCondition: "",
          price: 0,
          toPrice: 0,
          currencyType: ''
        }
        const products = await postIntegrationApiData(url, token, payload)
        if (products?.result?.length > 0) {
          stockIntent.push(data)
        } else {
          outOfStockIntent.push(data)
        }
      }))
      if (outOfStockIntent?.length > 0) {
        const message = `Oops! Looks like ${outOfStockIntent[0]} item you're searching for couldn't be found.`
        newChatSendMsg("Text", message, "OUT", 0);
      }
      if (stockIntent.length > 0) {
        if (stockIntent?.length > 1) {
          ecommerceTemplateKey(stockIntent);
        } else {
          const message = `Here you go with the fresh arrivals of branded ${stockIntent[0]}`;
          newChatSendMsg("Text", message, "OUT", 0);
          const selectedArray = builderList?.ecommercePurchase?.nlpProductIntent[stockIntent[0]];
          ecommerceProdcutSearch(selectedArray)
        }
        store.dispatch(chatBuilderActions.setEcommercePurchase({ productIntent: stockIntent.slice(1) }));
      }
    }
    return;
  } else if (builderList?.connectionName === 'Wix') {
    const stockIntent = [];
    const outOfStockIntent = [];
    const end_point = `${config()?.WIX_TOKEN_GENERATOR}/${messageList.personalisationAll.domainId}`
    const res_data = await getIntegrationApiData(end_point, '')
    if (res_data?.result.length > 0) {
      const tokenId = res_data?.result[0]?.token;
      const end_point_refresh = `${config()?.WIX_REFRESH_TOKEN_GENERATOR}/${messageList.personalisationAll.domainId}`
      const res_data_refresh = await getIntegrationApiData(end_point_refresh, tokenId);
      const token = res_data_refresh?.results?.token
      store.dispatch(chatBuilderActions.setEcommercePurchase({ token }));
      await Promise.all(list?.map(async (data) => {
        // const url = `${config()?.WIX_PRODUCT_SEARCH}/${data}`
        // const products = await getIntegrationApiData(url, res_data?.data?.result[0]?.token);
        let selectedArray = builderList?.ecommercePurchase?.nlpProductIntent[data];
        const url = config()?.WIX_PRODUCT_SEARCH_ID;
        let payload = {
          product_id: selectedArray
        }
        const products = await postIntegrationApiData(url, res_data?.result[0]?.token, payload)
        if (products?.result?.products?.length > 0) {
          stockIntent.push(data);
        } else {
          outOfStockIntent.push(data)
        }
      }));
      if (outOfStockIntent?.length > 0) {
        const message = `Oops! Looks like ${outOfStockIntent[0]} item you're searching for couldn't be found.`
        newChatSendMsg("Text", message, "OUT", 0);
      }
      if (stockIntent.length > 0) {
        if (stockIntent?.length > 1) {
          store.dispatch(chatBuilderActions.setEcommercePurchase({ productIntent: stockIntent }));
          ecommerceTemplateKey(stockIntent);
        } else {
          const message = `Here you go with the fresh arrivals of branded ${stockIntent[0]}`;
          newChatSendMsg("Text", message, "OUT", 0);
          const selectedArray = builderList?.ecommercePurchase?.nlpProductIntent[stockIntent[0]];
          ecommerceProdcutSearch(selectedArray)
          store.dispatch(chatBuilderActions.setEcommercePurchase({ productIntent: stockIntent.slice(1) }));
        }
      }
    }
  } else if (builderList?.connectionName === 'BigCommerce') {
    const stockIntent = [];
    const outOfStockIntent = [];
    const end_point = `${config()?.BIGCOMMERCE_TOKEN_GENERATOR}/${messageList.personalisationAll.domainId}`
    const res_data = await getIntegrationApiData(end_point, '')
    if (res_data?.result.length > 0) {
      const token = res_data?.result[0]?.token;
      store.dispatch(chatBuilderActions.setEcommercePurchase({ token }));
      await Promise.all(list?.map(async (data) => {
        const url = `${config()?.BIGCOMMERCE_SEARCH_LIST}/${data}`
        const products = await getIntegrationApiData(url, token)
        if (products?.statusCode === 200 && products?.result?.data?.length > 0) {
          stockIntent.push(data);
        } else {
          outOfStockIntent.push(data)
        }
      }));
      if (outOfStockIntent?.length > 0) {
        const message = `Oops! Looks like ${outOfStockIntent[0]} item you're searching for couldn't be found.`
        newChatSendMsg("Text", message, "OUT", 0);
      }
      if (stockIntent.length > 0) {
        if (stockIntent?.length > 1) {
          store.dispatch(chatBuilderActions.setEcommercePurchase({ productIntent: stockIntent }));
          ecommerceTemplateKey(stockIntent);
        } else {
          const message = `Here you go with the fresh arrivals of branded ${stockIntent[0]}`;
          newChatSendMsg("Text", message, "OUT", 0);
          ecommerceProdcutSearch(stockIntent[0])
          store.dispatch(chatBuilderActions.setEcommercePurchase({ productIntent: stockIntent.slice(1) }));
        }
      }

    }
  } else if (builderList?.connectionName === 'Magento') {
    const stockIntent = [];
    const outOfStockIntent = [];
    const end_point = `${config()?.MAGENTO_TOKEN_GENERATOR}/${messageList.personalisationAll.domainId}`
    const res_data = await getIntegrationApiData(end_point, '')
    if (res_data?.result.length > 0) {
      const token = res_data?.result[0]?.token;
      store.dispatch(chatBuilderActions.setEcommercePurchase({ token }));
      store.dispatch(chatBuilderActions.setMagentoDomain(res_data?.result[0]?.shopDomain))
      await Promise.all(list?.map(async (data) => {
        const url = `${config()?.MAGENTO_PRODUCT_SEARCH}/${data}`
        const products = await getIntegrationApiData(url, token)
        if (products?.statusCode === 401) {
          const url = `${config()?.MAGENTO_REFRESH_TOKEN}/${messageList.personalisationAll.domainId}`
          const refreshToken = await getIntegrationApiData(url, token)
          checkValidProductEnquiry(list);
        } else {
          if (products?.statusCode === 200 && products?.result?.items?.length > 0) {
            stockIntent.push(data);
          } else {
            outOfStockIntent.push(data)
          }
        }
      }));
      if (outOfStockIntent?.length > 0) {
        const message = `Oops! Looks like ${outOfStockIntent[0]} item you're searching for couldn't be found.`
        newChatSendMsg("Text", message, "OUT", 0);
      }
      if (stockIntent.length > 0) {
        if (stockIntent?.length > 1) {
          store.dispatch(chatBuilderActions.setEcommercePurchase({ productIntent: stockIntent }));
          ecommerceTemplateKey(stockIntent);
        } else {
          const message = `Here you go with the fresh arrivals of branded ${stockIntent[0]}`;
          newChatSendMsg("Text", message, "OUT", 0);
          ecommerceProdcutSearch(stockIntent[0])
          store.dispatch(chatBuilderActions.setEcommercePurchase({ productIntent: stockIntent.slice(1) }));
        }
      }

    }

  } else if (builderList?.connectionName === 'WooCommerce') {
    const stockIntent = [];
    const outOfStockIntent = [];
    const p_name = [];

    const end_point = `${config()?.WOOCOMMERCE_TOKEN_GENERATOR}/${messageList.personalisationAll.domainId}`
    const res_data = await getIntegrationApiData(end_point, '')
    if (res_data?.result.length > 0) {
      const token = res_data?.result[0]?.token;
      store.dispatch(chatBuilderActions.setEcommercePurchase({ token }));
      await Promise.all(list?.map(async (data) => {
        const url = config()?.WOOCOMMERCE_PRODUCT_SEARCH_ID;
        let selectedArray = builderList?.ecommercePurchase?.nlpProductIntent[data];
        let payload = {
          product_id: selectedArray
        }
        const products = await postIntegrationApiData(url, token, payload)
        if (products?.result?.products?.length > 0 && products?.statusCode === 200) {
          stockIntent.push(selectedArray);
          p_name.push(data)
        } else {
          outOfStockIntent.push(selectedArray);
          p_name.push(data)
        }
      }))
    }
    if (outOfStockIntent?.length > 0) {
      const message = `Oops! Looks like ${p_name[0]} item you're searching for couldn't be found.`
      newChatSendMsg("Text", message, "OUT", 0);
    }
    if (stockIntent.length > 0) {
      if (stockIntent?.length > 1) {
        store.dispatch(chatBuilderActions.setEcommercePurchase({ productIntent: p_name }));
        ecommerceTemplateKey(p_name);
      } else {
        const message = `Here you go with the fresh arrivals of branded ${p_name[0]}`;
        newChatSendMsg("Text", message, "OUT", 0);
        ecommerceProdcutSearch(stockIntent[0])
        store.dispatch(chatBuilderActions.setEcommercePurchase({ productIntent: p_name.slice(1) }));
      }
    }


  }
}


export const checkValidOfferEnquiry = async (list) => {
  const messageList = store.getState().message
  const builderList = store.getState().chatBuilder
  switch (builderList?.connectionName) {
    case 'WooCommerce':
      if (list?.length > 0 && !builderList?.offersNone) {
        checkValidProductEnquiry(list);
      } else {
        const end_point = `${config()?.WOOCOMMERCE_TOKEN_GENERATOR}/${messageList.personalisationAll.domainId}`
        const res_data = await getIntegrationApiData(end_point, '')
        if (res_data?.result.length > 0) {
          const token = res_data?.result[0]?.token;
          store.dispatch(chatBuilderActions.setEcommercePurchase({ token }));
          const offerUrl = config()?.WOOCOMMERCE_OFFER_SEARCH
          const products = await getIntegrationApiData(offerUrl, token)
          if (products?.statusCode === 200) {
            const message = !builderList?.offersNone ? `we do have some exciting promotions on our products that you might be interested in`
              : `These are current ongoing offers`;
            newChatSendMsg("Text", message, "OUT", 0);
            if (products?.result?.length > 0) {
              const resultData = await Promise.all(products?.result?.map(async (list) => {
                const sizeVariants = [];
                const colourVariants = [];
                let defaultSize = '';
                let defaultColour = '';
                const url = `${config()?.WOOCOMMERCE_GET_VARIANT}/${list?.id}/variations`;
                const variantProducts = await getIntegrationApiData(url, token)
                list.variantData = variantProducts?.result;
                list?.attributes?.map((v_data) => {
                  if (v_data?.name.toLowerCase() === 'color') {
                    colourVariants?.push(v_data?.options)
                  } else if (v_data?.name.toLowerCase() === 'size') {
                    sizeVariants?.push(v_data?.options)
                  }
                })
                if (sizeVariants?.length === 0) {
                  list?.attributes?.map((v_data) => {
                    if (v_data?.options?.length > 0 && v_data?.name.toLowerCase() !== 'color') {
                      sizeVariants?.push(v_data?.options)
                    }
                  })
                }
                defaultColour = Array.isArray(colourVariants[0]) ? colourVariants[0][0] : colourVariants[0];
                defaultSize = Array.isArray(sizeVariants[0]) ? sizeVariants[0][0] : sizeVariants[0];
                let carouselResponse = {
                  isDynamicPrice: true,
                  message: list?.name,
                  file_url: list?.images[0]?.src !== undefined ? list?.images[0]?.src : noImage,
                  description: list?.description?.replace(/<[^>]*>/g, "") ?? '',
                  title: list?.name,
                  price: list?.price,
                  variantSize: sizeVariants ?? [],
                  variantColour: colourVariants ?? [],
                  productId: list?.id,
                  variantId: list?.variations[0],
                  buttons: [{ "value": "Add to cart", 'carouselId': 'yes_Carousel_1', 'productDetails': list, 'quantityCount': 1, 'selectedColour': defaultColour, 'selectedSize': defaultSize }]
                }
                return ({ data: { information: { type: 'carousel', carousel_data: JSON.stringify(carouselResponse) } } });
              }))
              let carousalPayloads = {
                messageID: uuidv4(),
                from: messageList.uuid,
                to: messageList?.connectionType?.isBotConnection ? messageList.uuid : `${messageList.agentDomainExt.domainId}_${messageList.agentDomainExt.ext}`,
                sessionId: messageList.uuid,
                carouselDataArr: resultData,
                message: 'carousel',
                sendAt: Date.now(),
                messageType: 'carousel',
                isBot: "IN",
                isCustomer: false,
                sender: '',
                isLike: 0,
                domainId: messageList?.personalisationAll?.domainId,
                isLiveagent: false,
                direction: 'IN',
                ChatHistoryId: parseInt(sessionStorage?.ChatHistoryId),
                ChatDurationHistoryId: parseInt(sessionStorage?.ChatDurationHistoryId),
                socketId: messageList?.socketId,
                assistType: checkConvoSupported() ? "Convo" : "Template",
                supervisorList: messageList?.superVisorExt
              };
              oneToOneChatSend(carousalPayloads);
              let texts = {
                message: carousalPayloads,
              };
              store.dispatch(messageActions.setMessageChat({ carouselText: texts }));
              store.dispatch(messageActions.carouselData(resultData));
            } else {
              const message = "Oops! Looks like the item you're searching for couldn't be found."
              newChatSendMsg("Text", message, "OUT", 0);
              ecommerceTemplateKey(builderList?.ecommercePurchase?.productIntent);
            }
          }
        }

      }
      store.dispatch(chatBuilderActions.setOffers(false))

  }
}


export const storeWixSelectedItems = async (list, value) => {
  try {
    const shoppingPayload = {
      quantityCount: list.quantityCount,
      imageUrl: value?.file_url,
      imgTitle: list?.productDetails?.name,
      p_id: value?.buttons[0]?.productDetails?.id,
      price: value?.price,
      allDetails: value?.buttons[0]?.productDetails,
      selectSize: value?.selectSize,
      selectColour: value?.selectColour,
      defaultVariantId: value?.defaultVariantId,
      file_url_id: value?.file_url_id
    }
    store.dispatch(chatBuilderActions.setSelectedCardItems(shoppingPayload));
  } catch (error) {
    console.error('Error fetching data:', error);
    throw error;
  }
};


export const checkoutExistingCustomer = async () => {
  //const messageList = store.getState().message;
  //const builderList = store.getState().chatBuilder
  // if (messageList?.customerFormData?.Email || messageList?.inputEntity[0]?.primaryEmail) {
  //   let url = `${config().WIX_EXISTING_CUSTOMER}/${messageList?.customerFormData?.Email || messageList?.inputEntity[0]?.primaryEmail}`
  //   let token = builderList?.ecommercePurchase?.token;
  //   const response = await getIntegrationApiData(url, token);
  //   if (response?.data?.result?.contacts?.length > 0) {
  //     store.dispatch(chatBuilderActions.setAddressList(response?.data?.result?.contacts));
  //     store.dispatch(chatBuilderActions.setShowAddress(true));
  //     const customerData = response?.data?.result?.contacts
  //     const payload = {
  //       "firstName": customerData[0]?.info?.name?.first,
  //       "lastName": customerData[0]?.info?.name?.last,
  //       "email": customerData[0]?.info?.emails?.items[0]?.email,
  //       "phoneCurrencyCode": 'GB',
  //       "phoneNumber": customerData[0]?.info?.phones?.items[0]?.phone,
  //       "address": {
  //         "addressLine": customerData[0]?.info?.addresses?.items[0]?.address?.addressLine,
  //         "addressLine2": customerData[0]?.info?.addresses?.items[0]?.address?.addressLine2,
  //         "city": customerData[0]?.info?.addresses?.items[0]?.address?.city,
  //         "postalCode": customerData[0]?.info?.addresses?.items[0]?.address?.postalCode,
  //         "country": customerData[0]?.info?.addresses?.items[0]?.address?.country,
  //       }
  //     };
  //     store.dispatch(chatBuilderActions.setCustomerPaymentData(payload));
  //   } else {
  //     store.dispatch(chatBuilderActions.setEnableCustomerForm(true));
  //   }
  // } else {
  let recommendMsg = 'Please provide your registered email address'
  newChatSendMsg('Text', recommendMsg, "OUT", 0);
  store.dispatch(chatBuilderActions.setCatchEmail(true));
  // }
}


export const makeWixPayment = async (data, mode) => {
  const messageList = store.getState().message
  const builderList = store.getState().chatBuilder;
  let paymentData = localStorage.getItem('WixPayment')
  const resultData = JSON.parse(paymentData)

  const allProducts = [];
  builderList?.selectedCardItems?.map((list) => {
    let itemPayload = {
      "productName": {
        "original": list?.imgTitle
      },
      "catalogReference": {
        "catalogItemId": list?.p_id,
        "appId": "215238eb-22a5-4c36-9e7b-e7c08025e04e",
        "options": {
          "variantId": list?.defaultVariantId
        }
      },
      "quantity": list?.quantityCount,
      "totalDiscount": {
        "amount": "0",
        "formattedAmount": "$0.00"
      },
      "descriptionLines": [
        {
          "name": {
            "original": 'color',
            "translated": 'color'
          },
          "plainText": {
            "original": list?.selectedColour ?? '',
            "translated": list?.selectedColour ?? ''
          }
        },
        {
          "name": {
            "original": 'size',
            "translated": 'size'
          },
          "plainText": {
            "original": list?.selectedSize ?? '',
            "translated": list?.selectedSize ?? ''
          }
        }
      ],
      "image": {
        "id": list?.file_url_id,
        "url": list?.imageUrl,
        "width": 1400,
        "height": 1588
      },
      "itemType": {
        "preset": list?.allDetails?.productType.toUpperCase()
      },
      "price": {
        "amount": list?.price.toString(),
        "formattedAmount": `$${list?.price}`
      },
      "taxDetails": {
        "taxRate": "",
        "totalTax": {
          "amount": "0",
          "formattedAmount": "$0.00"
        }
      },
      "totalPriceBeforeTax": {
        "amount": list?.price.toString(),
        "formattedAmount": `$${list?.price}`
      },
      "totalPriceAfterTax": {
        "amount": list?.price.toString(),
        "formattedAmount": `$${list?.price}`
      },
      "paymentPption": mode === 'cash' ? "FULL_PAYMENT_OFFLINE" : "FULL_PAYMENT_ONLINE",
      "lineItemPrice": {
        "amount": list?.price.toString(),
        "formattedAmount": `$${list?.price}`
      }
    }
    allProducts.push(itemPayload)
  })

  const orderPayload = {
    "firstName": builderList?.coustomerPaymentData?.firstName,
    "lastName": builderList?.coustomerPaymentData?.lastName,
    "phone": builderList?.coustomerPaymentData?.phoneNumber,
    "contactId": builderList?.coustomerPaymentData?.contactId,
    "contactEmail": builderList?.coustomerPaymentData?.email,
    "subtotal": {
      "amount": resultData?.orginalAmount.toString(),
      "formattedAmount": `$${resultData?.orginalAmount}`
    },
    "shipping": {
      "amount": "0",
      "formattedAmount": "$0.00"
    },
    "tax": {
      "amount": "0",
      "formattedAmount": "$0.00"
    },
    "discount": {
      "amount": "0",
      "formattedAmount": "$0.00"
    },
    "total": {
      "amount": resultData?.orginalAmount.toString(),
      "formattedAmount": `$${resultData?.orginalAmount}`
    },
    "address": {
      "addressLine": builderList?.coustomerPaymentData?.address?.addressLine,
      "addressLine2": builderList?.coustomerPaymentData?.address?.addressLine2,
      "city": builderList?.coustomerPaymentData?.address?.city,
      "postalCode": builderList?.coustomerPaymentData?.address?.postalCode,
      "country": builderList?.coustomerPaymentData?.address?.country,
      "countryFullname": 'UNITED KINGDOM',
      "subdivisionFullname": ""
    },
    "shippingInfo": {
      "contactDetails": {
        "firstName": builderList?.coustomerPaymentData?.firstName,
        "lastName": builderList?.coustomerPaymentData?.lastName,
        "phone": builderList?.coustomerPaymentData?.phoneNumber,
        "company": ""
      },
      "shippingDestination": {
        "address": {
          "addressLine": builderList?.coustomerPaymentData?.address?.addressLine,
          "addressLine2": builderList?.coustomerPaymentData?.address?.addressLine2,
          "city": builderList?.coustomerPaymentData?.address?.city,
          "postalCode": builderList?.coustomerPaymentData?.address?.postalCode,
          "country": builderList?.coustomerPaymentData?.address?.country,
          "countryFullname": 'UNITED KINGDOM',
          "subdivisionFullname": ""
        }
      }
    },
    "lineItems": allProducts,
    "paymentOrderId": mode === 'cash' ? '' : data?.id,
    "gatewayTransactionId": mode === 'cash' ? '' : data?.id,
    "paymentMethod": mode === 'cash' ? '' : data?.paymentMethod?.typeId,
    "providerTransactionId": mode === 'cash' ? '' : data?.providerTransactionId,
    "paymentStatus": mode === 'cash' ? "NOT_PAID" : 'PAID',
    "paymentMethod": mode === 'cash' ? "Offline" : 'Online',
    "offlinePayment": mode === 'cash' ? true : false
  }
  const end_point = `${config()?.WIX_TOKEN_GENERATOR}/${messageList.personalisationAll.domainId}`
  const res_data = await getIntegrationApiData(end_point, '')
  if (res_data?.result.length > 0) {
    const tokenId = res_data?.result[0]?.token;
    const end_point_refresh = `${config()?.WIX_REFRESH_TOKEN_GENERATOR}/${messageList.personalisationAll.domainId}`
    const res_data_refresh = await getIntegrationApiData(end_point_refresh, tokenId);
    const token = res_data_refresh?.results?.token
    store.dispatch(chatBuilderActions.setEcommercePurchase({ token }));
    const url = `${config().WIX_ORDER_CREATION}`;
    const products = await postIntegrationApiData(url, token, orderPayload);
    if (products?.statusCode === 200) {

      const recommendMsg = mode === 'cash' ? `Thank you for selecting cash on delivery as your payment method. 
  <span className="worktualnamelists">Order ID </span>
    <strong>${products?.result?.orderTransactions?.orderId || "N/A"}</strong>
    <span className="worktualnamelists">Item Ordered</span>
    <strong>${builderList?.selectedCardItems?.map((list) => list?.imgTitle).join(',<br/>')}
</strong>
    <span className="worktualnamelists">Payment Method</span>
    <strong>Cash On Delivery</strong>
    <span className="worktualnamelists">Amount to be paid</span>
    <strong> £ ${resultData?.orginalAmount || "N/A"}</strong>
    <span className="worktualnamelists">Email</span>
    <strong>${builderList?.coustomerPaymentData?.email || "N/A"}</strong>
    <span className="worktualnamelists">Phone Number</span>
    <strong>${builderList?.coustomerPaymentData?.phoneNumber || "N/A"}</strong>
    <span className="worktualnamelists">Address</span>
    <strong>${builderList?.coustomerPaymentData?.address?.addressLine + " " + builderList?.coustomerPaymentData?.address?.addressLine2 + ' ' + builderList?.coustomerPaymentData.address?.city + ' ' + builderList?.coustomerPaymentData.address?.postalCode || "N/A"}</strong>`.trim() :
        `Great! Your payment is successful and Thank you for your purchase. 
    <span className="worktualnamelists">Order ID </span>
      <strong>${products?.result?.orderTransactions?.orderId || "N/A"}</strong>
      <span className="worktualnamelists">Item Ordered</span>
      <strong>${builderList?.selectedCardItems?.map((list) => list?.imgTitle)}</strong>
      <span className="worktualnamelists">Payment Method</span>
      <strong>Online Payment</strong>
      <span className="worktualnamelists">Amount paid</span>
      <strong>${resultData?.orginalAmount || "N/A"}</strong>
      <span className="worktualnamelists">Email</span>
      <strong>${builderList?.coustomerPaymentData?.email || "N/A"}</strong>
      <span className="worktualnamelists">Phone Number</span>
      <strong>${builderList?.coustomerPaymentData?.phoneNumber || "N/A"}</strong>
      <span className="worktualnamelists">Address</span>
      <strong>${builderList?.coustomerPaymentData?.address?.addressLine + " " + builderList?.coustomerPaymentData?.address?.addressLine2 + ' ' + builderList?.coustomerPaymentData.address?.city + ' ' + builderList?.coustomerPaymentData.address?.postalCode || "N/A"}</strong>`.trim()
      newChatSendMsg('Text', recommendMsg, "OUT", 1);

      let carousalPayloads = {
        messageID: uuidv4(),
        from: messageList.uuid,
        to: messageList?.connectionType?.isBotConnection ? messageList.uuid : `${messageList.agentDomainExt.domainId}_${messageList.agentDomainExt.ext}`,
        sessionId: messageList.uuid,
        message: 'It is currently being prepared for shipment and will be dispatched shortly.',
        sendAt: Date.now(),
        messageType: 'shoppingContinue',
        isBot: "IN",
        isCustomer: false,
        sender: '',
        isLike: 0,
        domainId: messageList?.personalisationAll?.domainId,
        isLiveagent: false,
        direction: 'IN',
        ChatHistoryId: parseInt(sessionStorage?.ChatHistoryId),
        ChatDurationHistoryId: parseInt(sessionStorage?.ChatDurationHistoryId),
        socketId: messageList?.socketId,
        assistType: checkConvoSupported() ? "Convo" : "Template",
      };
      setTimeout(() => {
        oneToOneChatSend(carousalPayloads);
        let texts = {
          message: carousalPayloads,
        };
        store.dispatch(messageActions.setMessageChat({ agentText: texts }))
      }, 4000)

    } else {
      const errorMsg = 'Oops! Payment failed. Check your card details and try again.'
      newChatSendMsg('Text', errorMsg, "IN", 0);
    }
  }
  store.dispatch(chatBuilderActions.setSelectedCardItemsEmpty());
  localStorage.setItem('WixPayment', '')
}

export const storeBigCommerceItem = async (list, value) => {
  try {
    const shoppingPayload = {
      quantityCount: list.quantityCount,
      imageUrl: value?.file_url,
      imgTitle: value?.title,
      p_id: value?.productId,
      price: value?.price,
      allDetails: value,
      VariantId: value?.variantId,
    }
    store.dispatch(chatBuilderActions.setSelectedCardItems(shoppingPayload));
  } catch (error) {
    console.error('Error fetching data:', error);
    throw error;
  }
};


export const checkoutExistingCustomerForBc = async () => {
  const messageList = store.getState().message;
  const builderList = store.getState().chatBuilder
  if (messageList?.customerFormData?.Email || messageList?.inputEntity[0]?.primaryEmail) {
    let url = `${config().BIGCOMMERCE_EXISTING_CUSTOMER}/${messageList?.customerFormData?.Email || messageList?.inputEntity[0]?.primaryEmail}`
    let token = builderList?.ecommercePurchase?.token;
    const response = await getIntegrationApiData(url, token);
    if (response?.statusCode === 200 && response?.result?.data.length > 0) {
      store.dispatch(chatBuilderActions.setAddressList(response?.result?.data));
      store.dispatch(chatBuilderActions.setShowAddress(true));
      const customerData = response?.result?.data
      const payload = {
        "firstName": customerData[0]?.info?.name?.first,
        "lastName": customerData[0]?.info?.name?.last,
        "email": customerData[0]?.info?.emails?.items[0]?.email,
        "phoneCurrencyCode": 'GB',
        "phoneNumber": customerData[0]?.info?.phones?.items[0]?.phone,
        "address": {
          "addressLine": customerData[0]?.info?.addresses?.items[0]?.address?.addressLine,
          "addressLine2": customerData[0]?.info?.addresses?.items[0]?.address?.addressLine2,
          "city": customerData[0]?.info?.addresses?.items[0]?.address?.city,
          "postalCode": customerData[0]?.info?.addresses?.items[0]?.address?.postalCode,
          "country": customerData[0]?.info?.addresses?.items[0]?.address?.country,
        }
      };
      store.dispatch(chatBuilderActions.setCustomerPaymentData(payload));
    } else {
      store.dispatch(chatBuilderActions.setEnableCustomerForm(true));
    }
  } else {
    let recommendMsg = 'Please provide your registered email address'
    newChatSendMsg('Text', recommendMsg, "OUT", 0);
    store.dispatch(chatBuilderActions.setCatchEmail(true));
  }
}


export const bigcommerceOrderCreation = async () => {
  const builderList = store.getState().chatBuilder
  const itemDetais = builderList?.selectedCardItems?.map((list) => {
    let payload = {
      "quantity": list?.quantityCount,
      "product_id": list?.p_id,
      "variant_id": list?.VariantId
    }
    return payload
  })
  let orderPayload = {
    "customerId": builderList?.coustomerPaymentData?.contactId,
    "products": itemDetais,
    "billing_address": {
      "first_name": builderList?.coustomerPaymentData?.firstName,
      "last_name": builderList?.coustomerPaymentData?.lastName,
      "street_1": builderList?.selectedAddress,
      "city": builderList?.coustomerPaymentData?.address?.city,
      "state": builderList?.coustomerPaymentData?.address?.state,
      "zip": builderList?.coustomerPaymentData?.address?.postalCode,
      "country": builderList?.coustomerPaymentData?.country,
      "country_iso2": builderList?.coustomerPaymentData?.country_code,
      "email": builderList?.coustomerPaymentData?.email
    },
    "shipping_addresses": [
      {
        "first_name": builderList?.coustomerPaymentData?.firstName,
        "last_name": builderList?.coustomerPaymentData?.lastName,
        "street_1": builderList?.selectedAddress,
        "city": builderList?.coustomerPaymentData?.address?.city,
        "state": builderList?.coustomerPaymentData?.address?.state,
        "zip": builderList?.coustomerPaymentData?.address?.postalCode,
        "country": builderList?.coustomerPaymentData?.country,
        "country_iso2": builderList?.coustomerPaymentData?.country_code,
        "email": builderList?.coustomerPaymentData?.email
      }
    ]
  }
  const url = `${config().BIGCOMMERCE_CREATE_ORDER}`;
  let token = builderList?.ecommercePurchase?.token;
  const products = await postIntegrationApiData(url, token, orderPayload);
  if (products?.statusCode === 200 && products?.result?.paymentMethod?.data?.length > 0) {
    store.dispatch(chatBuilderActions.setBigCommercePaymentType(products?.result?.paymentMethod?.data));
    store.dispatch(chatBuilderActions.setShowBigCommercePayment(true));
    const orderIdData = {
      'orderId': products?.result?.order?.id
    }
    const orderUrl = `${config().ORDER_TOKEN_GENERATOR}`;
    const tokenResult = await postIntegrationApiData(orderUrl, token, orderIdData);
    if (tokenResult?.statusCode === 200) {
      store.dispatch(chatBuilderActions.setBigCommerceOrderToken(tokenResult?.result?.data?.id))
    }
  }

}

export const checkoutExistingCustomerForMagento = async () => {
  const messageList = store.getState().message;
  const builderList = store.getState().chatBuilder
  if (messageList?.customerFormData?.Email || messageList?.inputEntity[0]?.primaryEmail) {
    let url = `${config().MAGENTO_EXISTING_CUSTOMER}/${messageList?.customerFormData?.Email || messageList?.inputEntity[0]?.primaryEmail}`
    let token = builderList?.ecommercePurchase?.token;
    const response = await getIntegrationApiData(url, token);
    if (response?.result?.contacts?.length > 0) {
      store.dispatch(chatBuilderActions.setAddressList(response?.result?.contacts));
      store.dispatch(chatBuilderActions.setShowAddress(true));
      const customerData = response?.result?.contacts
      const payload = {
        "firstName": customerData[0]?.info?.name?.first,
        "lastName": customerData[0]?.info?.name?.last,
        "email": customerData[0]?.info?.emails?.items[0]?.email,
        "phoneCurrencyCode": 'GB',
        "phoneNumber": customerData[0]?.info?.phones?.items[0]?.phone,
        "address": {
          "addressLine": customerData[0]?.info?.addresses?.items[0]?.address?.addressLine,
          "addressLine2": customerData[0]?.info?.addresses?.items[0]?.address?.addressLine2,
          "city": customerData[0]?.info?.addresses?.items[0]?.address?.city,
          "postalCode": customerData[0]?.info?.addresses?.items[0]?.address?.postalCode,
          "country": customerData[0]?.info?.addresses?.items[0]?.address?.country,
        }
      };
      store.dispatch(chatBuilderActions.setCustomerPaymentData(payload));
    } else {
      store.dispatch(chatBuilderActions.setEnableCustomerForm(true));
    }
  } else {
    let recommendMsg = 'Please provide your registered email address'
    newChatSendMsg('Text', recommendMsg, "OUT", 0);
    store.dispatch(chatBuilderActions.setCatchEmail(true));
  }
}


export const storeMagentoItem = async (list, value) => {
  try {
    const shoppingPayload = {
      quantityCount: list.quantityCount,
      imageUrl: value?.file_url,
      imgTitle: value?.title,
      price: value?.price,
      allDetails: value,
    }
    store.dispatch(chatBuilderActions.setSelectedCardItems(shoppingPayload));
  } catch (error) {
    console.error('Error fetching data:', error);
    throw error;
  }
};


export const storeWooCommerceItem = async (list, value) => {
  try {
    const shoppingPayload = {
      quantityCount: list.quantityCount,
      imageUrl: value?.file_url,
      imgTitle: value?.title,
      p_id: value?.productId,
      price: value?.price,
      allDetails: value,
      VariantId: value?.variantId,
    }
    const builderList = store.getState().chatBuilder;
    let index = builderList?.selectedCardItems.findIndex((e) => e.p_id === value?.productId);
    if (index > -1) {
      let result = builderList?.selectedCardItems?.map((items) => {
        if (items?.p_id === value?.productId) {
          items = { ...items, quantityCount: items?.quantityCount + 1 };
        }
        return items
      })
      store.dispatch(chatBuilderActions.setTotalCardItems(result));
    } else {
      store.dispatch(chatBuilderActions.setSelectedCardItems(shoppingPayload));
    }
  } catch (error) {
    console.error('Error fetching data:', error);
    throw error;
  }
};

export const wooCommerceOrderCreation = async () => {
  const builderList = store.getState().chatBuilder;
  let selectedList;
  if (builderList?.selectedCardItems?.length > 1) {
    selectedList = builderList?.selectedCardItems?.map((list) => {
      return `${list?.p_id}:${list?.quantityCount}`;
    });
  } else if (builderList?.selectedCardItems?.length === 1) {
    if (builderList?.selectedCardItems[0]?.quantityCount > 1) {
      selectedList = [`${builderList?.selectedCardItems[0]?.p_id}:1`, `${builderList?.selectedCardItems[0]?.p_id}:${builderList?.selectedCardItems[0]?.quantityCount - 1}`]
    } else {
      selectedList = [builderList?.selectedCardItems[0]?.p_id]
    }
  }
  const IdList = selectedList.join(',')
  const IdList2 = localStorage.getItem('dynamicDomainUrl');
  // let domainUrl= `${builderList?.dynamicDomainURL}/cart/?add-to-cart=${IdList}`;
  let domainUrl = `${IdList2}/cart/?add-to-cart=${IdList}`;
  store.dispatch(chatBuilderActions.setCheckoutUrl(domainUrl))
  //window.open(domainUrl)
  let message = `Your checkout URL id ${domainUrl}`
  newChatSendMsg('Text', message, "OUT", 0);
  store.dispatch(chatBuilderActions.setSelectedCardItemsEmpty());
  setTimeout(() => {
    checkoutProductStock()
  }, 2000);

  // const selectedList = builderList?.selectedCardItems?.map((list) => {
  //   let data = {
  //     "product_id": list?.p_id,
  //     "variation_id": list?.VariantId,
  //     "quantity": list?.quantityCount
  //   }
  //   return data;
  // })
  // let orderPayload = {
  //   "billing": {
  //     "first_name": builderList?.coustomerPaymentData?.firstName,
  //     "last_name": builderList?.coustomerPaymentData?.lastName,
  //     "company": "",
  //     "address_1": builderList?.coustomerPaymentData?.address?.addressLine,
  //     "address_2": builderList?.coustomerPaymentData?.address?.addressLine2,
  //     "city": builderList?.coustomerPaymentData?.address?.city,
  //     "state": builderList?.coustomerPaymentData?.address?.state,
  //     "postcode": builderList?.coustomerPaymentData?.address?.postalCode,
  //     "country": builderList?.coustomerPaymentData?.address?.country,
  //     "email": builderList?.coustomerPaymentData?.email,
  //     "phone": builderList?.coustomerPaymentData?.phoneNumber
  //   },
  //   "shipping": {
  //     "first_name": builderList?.coustomerPaymentData?.firstName,
  //     "last_name": builderList?.coustomerPaymentData?.lastName,
  //     "company": "",
  //     "address_1": builderList?.coustomerPaymentData?.address?.addressLine,
  //     "address_2": builderList?.coustomerPaymentData?.address?.addressLine2,
  //     "city": builderList?.coustomerPaymentData?.address?.city,
  //     "state": builderList?.coustomerPaymentData?.address?.state,
  //     "postcode": builderList?.coustomerPaymentData?.address?.postalCode,
  //     "country": builderList?.coustomerPaymentData?.address?.country,
  //     "email": builderList?.coustomerPaymentData?.email,
  //     "phone": builderList?.coustomerPaymentData?.phoneNumber
  //   },
  //   "line_items": selectedList
  // }

  // const url = `${config().WOOCOMMERCE_ORDER_CREATION}`;
  // let token = builderList?.ecommercePurchase?.token;
  // const products = await postIntegrationApiData(url, token, orderPayload);
  // if (products?.data?.statusCode === 200 && products?.data?.result) {
  //   store.dispatch(chatBuilderActions.setCheckoutUrl(builderList?.selectedCardItems))
  //   store.dispatch(chatBuilderActions.setSelectedCardItemsEmpty());
  //   const shopUrl = products?.data?.result?.payment_url;
  //    let message = `Your checkout URL id ${shopUrl}`
  //    newChatSendMsg('Text', message, "OUT");
  //   setTimeout(() => {
  //     checkoutProductStock()
  //   }, 2000);

  // } else {
  //   let recommendMsg = 'Oops! order is failed. Please try again'
  //   newChatSendMsg('Text', recommendMsg, "OUT");
  // }
}

export const magentoOrderCreation = async () => {
  const builderList = store.getState().chatBuilder
  builderList?.selectedCardItems?.map((list) => {
    let orderPayload = {
      "sku": list?.allDetails?.buttons[0]?.productDetails?.sku,
      "qty": list?.quantityCount,
      "region": "UK",
      "countryId": "GB",
      "postalCode": builderList?.coustomerPaymentData?.address?.postalCode,
      "city": builderList?.coustomerPaymentData?.address?.city,
      "firstName": builderList?.coustomerPaymentData?.firstName,
      "lastName": builderList?.coustomerPaymentData?.lastName,
      "telephone": builderList?.coustomerPaymentData?.phoneNumber,
      "customerId": builderList?.coustomerPaymentData?.contactId,
      "street": [
        builderList?.coustomerPaymentData?.address?.addressLine
      ]
    }
    const url = `${config().MAGENTO_CREATE_ORDER}`;
    let token = builderList?.ecommercePurchase?.token;
    const products = postIntegrationApiData(url, token, orderPayload);
  })

}

export function isJSON(str) {
  try {
    return JSON.parse(str);
  } catch (e) {
    return false;
  }
}


export const encryptData = async (data) => {
  try {
    const secretKey = process.env.REACT_APP_ENCRYPT_KEY;
    const encrypt = CryptoJS.AES.encrypt(JSON.stringify(data), secretKey).toString();
    console.log("encryptData", encrypt)
    return encrypt;
  } catch (error) {
    console.log(error)
    return '';
  }
}
export const getBrowserKey=()=> {
  try {
    let rawData = localStorage.getItem("user_uid");
    if (!rawData) {
      rawData = uuidv4(); // generates unique UUID (browser API)
      localStorage.setItem("user_uid", rawData);
    }
    return rawData;  
  } catch (error) {
    console.log(error);
    return "browser_unknown";
  }
}

export const nlpConvoSummary=()=>{
  try {

      const convoAllowedDomains = [ worktulDomain, ponpureDomain, ponpureDomainAI ];
      const phoneNumberAsCookie = [ ponpureDomain, ponpureDomainAI ];

      const messageList = store.getState().message;
      // const EnterPrice = ['62', '57', '42'].includes(`${messageList?.personalisationAll?.plan_id}`);
      if(!checkConvoSupported())
        return

      let cookieKey = getBrowserKey()
      if( phoneNumberAsCookie.includes(`${messageList?.personalisationAll?.domainId}`) 
        && messageList?.customerFormData['Phone number'] ){
        cookieKey = `${messageList?.customerFormData['Phone number']}`;
      }

      const bodyData = JSON.stringify({
        "cookie_id":cookieKey,
        "session_id":messageList.uuid,
        "ip_address":messageList.storeIpAddress,
        "customer_id":messageList?.customerFormData?.coid,
        "domain_id": messageList?.personalisationAll?.domainId ?? 0,
        "clientLocation": messageList?.clientLocation,
        "chatHistoryId": parseInt(sessionStorage?.ChatHistoryId ?? 0),
        "customerId" : messageList?.customerFormData?.coid ?? 0,
        "bot_id" : messageList?.personalisationAll?.webchatid || '',
      })

      fetch(`${config().NLP_CONVO_SUMMARY}`, {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: bodyData,
      });
  } catch (error) {
        console.log(error)
  }
}

export const zohoCrm = async (payload) => {

  if ([ponpureDomain, ponpureDomainAI].includes(`${payload.domainId}`)) {
    console.log("zohoCrm payload === ", JSON.stringify(payload, 2, null));

    const headers = { 'Content-Type': 'application/json' };
    const bodyData = {
      company: "",
      lastName: payload?.lastName ?? "",
      email: payload?.primaryEmail ?? "",
      fullName: `${payload?.firstName ?? ""} ${payload?.lastName ?? ""}`.trim(),
      firstName: payload?.firstName ?? "",
      industry: "",
      zipCode: payload?.postalCode ?? "",
      message: "",
      country: payload?.country ?? "",
      city: payload?.city ?? "",
      mobile: payload?.primaryPhoneNumber ?? "",
    };

    const getZohoToken = async () => {
      try {
        const response = await fetch(`${config().CONNECTOR_COMMON_LIST}/${payload.domainId}`, { method: 'GET', headers });
        if (!response.ok) return null;

        const res = await response.json();
        return res?.results?.find((item) => item?.connector_name === 'zoho')?.token ?? null;
      } catch (error) {
        console.error("Error fetching Zoho token:", error);
        return null;
      }
    };

    const refreshZohoToken = async (oldToken) => {
      try {
        const response = await fetch(`${config().CONNECTOR_ZOHO_REFRESH}/${payload.domainId}`, {
          method: 'GET',
          headers: { ...headers, 'Authorization': oldToken },
        });
        const data = await response.json();

        if (data?.statusCode === 200 && data?.results?.token) {
          console.log("Refreshed Zoho token");
          return data.results.token;
        }
      } catch (error) {
        console.error("Error refreshing Zoho token:", error);
      }
      return null;
    };

    const sendToZoho = async (zohoToken) => {
      try {
        const response = await fetch(`${config().CONNECTOR_ZOHO_LEADS}`, {
          method: 'POST',
          headers: { ...headers, 'Authorization': zohoToken },
          body: JSON.stringify(bodyData),
        });
        const result = await response.json();

        if (result?.statusCode === 401 && result?.result?.message === "Session expired or invalid") {
          console.warn("Zoho token expired, refreshing...");
          const newToken = await refreshZohoToken(zohoToken);
          if (newToken) await sendToZoho(newToken);
        } else {
          console.log("Zoho CRM update result:", result);
        }
      } catch (error) {
        console.error("Error updating Zoho CRM:", error);
      }
    };

    try {
      const zohoToken = await getZohoToken();
      if (zohoToken) await sendToZoho(zohoToken);
      else console.warn("Zoho token not found");
    } catch (error) {
      console.error("zohoCrm error:", error);
    }
  };
};

export function areMessagesSame(msg1, msg2) {
  try {
    return (
      msg1.replace(/[!?]/g, '').trim().toLowerCase() ===
      msg2.replace(/[!?]/g, '').trim().toLowerCase()
    );
  } catch (e) {
    return false;
  }
}