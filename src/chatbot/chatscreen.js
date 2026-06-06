/* eslint-disable */
import "../../src/chatPreview.scss";
import React, { useEffect, useRef, useState, useLayoutEffect } from "react";
import CallBackRequest from "./callBackRequest";
import CommonPaymentForm from './commonpaymentForm'
import FooterLogo from "./../images/chatbotpoweredlogo1.png";
import Pdficon from "../images/pdf.png";
import grid_ppt from "../images/ppt.png";
import grid_word from "../images/doc.png";
import grid_xls from "../images/xls.png";
import zip from '../images/zip.png'
import { config , nickysDomainAI, grtDomain} from "../base/constants";
import { messageActions } from "../store/slice/message_slice";
import parse from "html-react-parser";
// import sendArrow from "./../images/sendmsg.png";
import sendArrow from "./../images/sendnew.png";
import { useDispatch } from "react-redux";
import useScrollPercentage from "../custom_hooks/useScrollPercentage";
import { useSelector } from "react-redux";
import { v4 as uuidv4 } from "uuid";
import { motion } from "framer-motion";
import { newChatSendMsg, makeApiCall, handleConnectionType, getBrowserName, getDeviceType, getLanguageTrans, createContactApi, getCardType, chatEndMsg, callbackMessage, breakFlowMsg, unhelpfulResponsiveStore, contactStorde, isJsonString, continusBlock, getchatFlowFind, ecommerceProdcutSearch, getIntegrationApiData, postIntegrationApiData, updateChatSessionCount, ecommerceTemplateKey, checkoutProductStock, checkoutExistingCustomer, makeWixPayment, checkoutExistingCustomerForBc, bigcommerceOrderCreation, checkoutExistingCustomerForMagento, wooCommerceOrderCreation, magentoOrderCreation, connectBotAfterAgent, isJSON, nlpConvoSummary } from "./utils"
import { joinWebChat, chatMessageUpdate, sendChatBotReq, updateThumbsUp, updateEndSession, botConnectedTime, siteTracking, updateCallbackData, oneToOneChatSend, abandonedChatDashboard, inboundChatDashboard, oneToOneChatSendTyping, worktualPurchase } from '../service/socketservice'
import store from "../store/redux-store/root_store";
import { getIndexDbItem } from '../indexedDb/index';
import groovyWalkAnimation from "../images/br-login-animated-dots-black_1700557145872.json";
import Lottie from "lottie-react";
import Calendly from '../appointment/calendly'
import BookingCalendar from "../appointment/ms_google";
import CarouselForm from "../chatbot/carousel"
import { ShopingSelectedItem } from "../component/CarouselPothys";
import { CarosuelShoping } from "../component/carosel/caroselSlider";
import { ShopingCartIcon } from '../component/carosel/CartIcon'
import ShopingSelectedItem_nicky  from '../component/carosel/selectedItemNIckys'
import ShopingSelectedItem_Pothys  from '../component/carosel/selectedItemPothys'
import DateTimePicker from '../appointment/datetimepicker'
import DropdownComponent from "./dropDown";
import { chatBuilderActions } from "../store/slice/builder_slice";
import moment from "moment";
import SpeechRecognition, { useSpeechRecognition } from 'react-speech-recognition';
import {
  clearSessionStorage, excelFormats, imageFormat, pptFormats, wordFormats, pdfFormats, selectPaymentMethod,
  handleResendOtp, resendOtpGenerator, storeWorktulaCookiesData, getVisitorInsightsData, isFeatureEnabled, getWebVisitorInsightsDataPothys,
  grtProductPostmessage, sendRNCloseChat
} from '../chatbot/utils2.0';
import FeedBackForm from "./feedbackForm";
import ReadMore from "./readMoreText";
import attachimg from "../images/attach.png";
import { liveTrackerActions } from "../store/slice/live_tracker";
import mic from "../images/mic.png";
import mictwo from "../images/mictwo.png";
import SIMDeals from "../component/simdeals";
import B2ccarousel from "../component/b2ccarousel";
import { encryptData } from "../chatbot/utils";
import Tooltip from 'antd/es/tooltip';
import TextArea from 'antd/lib/input/TextArea';
import BudgetTable from './b2cBudget';
// const Tooltip = lazy(() => import('antd/es/tooltip'));
// const TextArea = lazy(() => import('antd/lib/input/TextArea'));
// const BudgetTable = lazy(() => import('./b2cBudget'));
import newlike from "../images/newlike.png";
import newlikeactive from "../images/newlikeactive.png";
import newdislike from "../images/newdislike.png";
import newdislikeactive from "../images/newdislikeactive.png";
import newspeaker from "../images/newspeaker.png";
import newspeakerstop from "../images/newspeakerstop.png";
import ButtonComponent from "./flow-button";
import { HandelCartList } from "../component/carosel/cartlist/handelcartlist"
import { checkConvoSupported } from "./utils2.0"

function ChatScreen() {
  const anchorRef = useRef(null);
  const [message, setMessage] = useState('');
  const [botStatus, setBotStatus] = useState(false);
  const [sendIcon, setSendIcon] = useState(false);
  const [file, setFile] = useState(null);
  const messageList = useSelector((state) => state.message);
  const liveTrackerList = useSelector((state) => state.liveTracker)
  const builderList = useSelector((state) => state.chatBuilder);
  const shopingStore = useSelector((state) => state.shopingStore);

  const dispatch = useDispatch();
  const scroll = useRef();
  const scrollUser = useRef();
  const [scrollRef, scrollPercentage] = useScrollPercentage();
  const [fileUrl, setFileUrl] = useState(null);
  const [lastSendTime, setLastSendTime] = useState(Date.now());
  const [dtmfBlock, setDtmfBlock] = useState(false);
  const [articlePre, setarticlePre] = useState(false);
  const countDownSeconds = 30;
  const [countDown, setCountDown] = React.useState(countDownSeconds);
  const [isMic, setIsMic] = useState(true)
  const [stillEnd, setStillEnd] = React.useState(false);
  const [timeoutStatus, setTimoutStatus] = useState(false);
  const [articleDes, setarticleDes] = useState('');
  const [queuedContent, setQueuedContent] = useState('');
  const [micLanguageStatus, setMicLanguageStatus] = useState(false);
  const [speakingStatus, setSpeakingStatus] = useState(null);
  const [feedback, setFeedback] = useState('')
  const [likeData, setLikeData] = useState({ data: {}, likeres: null });
  const [isFeedback, setIsFeedback] = useState(false);
  const [payForm, setPayForm] = useState({
    holderName: '',
    cardNumber: '',
    expiryMonth: '',
    expiryYear: '',
    cvv: '',
    cardType: '',
    address: '',
    city: '',
    postalCode: '',
    phone: ''
  });
  const msgContent = "Are we still connected?";
  const msgTempBlock = messageList?.templateData?.length > 0 ? "Could you please rephrase your question or provide more details? This will help me assist you better." : msgContent;
  let today = moment().format('YYYY-MM-DD');
  let startTimestampInSeconds = moment.utc(`${today} 00:00:00`, 'YYYY-MM-DD HH:mm:ss').valueOf();
  let todayStart = moment().format('YYYY-MM-DD');
  let toTimestampInSeconds = moment.utc(`${todayStart} 23:59:59`, 'YYYY-MM-DD HH:mm:ss').valueOf();

  useEffect(() => {
    scrolltoTop();
    if (sessionStorage.isConnected === "agent" || messageList.agentConnectingStatus) {
      clearTimeout(liveTrackerList?.timeOutIntervalData);
    }
  }, [messageList.messageList?.length])


  function handleSend() {
    setLastSendTime(Date.now());
  }

  const {
    transcript,
    finalTranscript,
    resetTranscript,
    browserSupportsSpeechRecognition
  } = useSpeechRecognition();

  let activeStream = null;

  // For caputring the voice while click the mic

  const startCapturing = async () => {
    try {
      setIsMic(false);
      if (activeStream) {
        activeStream.getTracks().forEach(track => track.stop());
        activeStream = null;
      }
      const micEnabled = await isMicrophoneEnabled();
      if (!micEnabled) {
        setIsMic(true);
        alert('Microphone not available');
        return;
      }
      const stream = await navigator.mediaDevices.getUserMedia({ audio: true });
      console.log('Microphone access granted');
      SpeechRecognition.startListening({ language: liveTrackerList?.selectedTextLanguage?.lang_code, continuous: true });
      activeStream = stream;
    } catch (error) {
      setIsMic(true);
      alert('Microphone not available');
    }
  };

  // Function for check the mic is enabled or disabled

  const isMicrophoneEnabled = async () => {
    try {
      const stream = await navigator.mediaDevices.getUserMedia({ audio: true });
      stream.getTracks().forEach(track => track.stop());
      return true;
    } catch (error) {
      return false;
    }
  };

  // Function for stop the caturing

  const stopCapturing = () => {
    setIsMic(true)
    SpeechRecognition.abortListening();
    SpeechRecognition.stopListening();
    if (activeStream) {
      activeStream.getTracks().forEach(track => track.stop());
      activeStream = null;
    }
    dispatch(chatBuilderActions.setSelectedLanguage(''));
  };

  const handleVisibilityChange = () => {
    if (document.visibilityState === 'hidden') {
      stopCapturing();
    }
  };

  document.addEventListener('visibilitychange', handleVisibilityChange);


  const handleMultiLan = () => {
    dispatch(chatBuilderActions.setMultiLanguage(true));
    dispatch(chatBuilderActions.setSelectedLanguage(''));
  }

  const handleSelectLangauage = () => {
    setMicLanguageStatus(false);
    startCapturing();
  };

  useEffect(() => {
    if (isMic && builderList?.seletedLanguage !== '') {
      setMicLanguageStatus(false);
      startCapturing()
    }
  }, [liveTrackerList?.selectedTextLanguage, builderList?.seletedLanguage])

  useEffect(() => {
    const init = async () => {
      if (!isMic) {
        setMessage(transcript);
        setSendIcon(true);
        dispatch(liveTrackerActions.setSpeakingTextStatus(true));
      }
    }
    init()
  }, [transcript])


  useEffect(() => {
    if (liveTrackerList?.speakingTextStatus) {
      let textMessage = messageList?.messageList[messageList?.messageList?.length - 1]
      if (textMessage?.agentText) {
        speakText(textMessage?.agentText, 0);
        dispatch(liveTrackerActions.setSpeakingTextStatus(false));
      }
    }
  }, [messageList?.messageList?.length]);

  const speakText = async (text, index) => {
    if ('speechSynthesis' in window) {
      window.speechSynthesis.cancel();
      if (speakingStatus === index) {
        setSpeakingStatus(null);
        return
      }
      if (text?.message?.message) {
        const cleanText = await stripHtmlTags(text?.message?.message);
        const utterance = new SpeechSynthesisUtterance(cleanText);
        const langCode = text?.message?.query_lang ?? 'en-UK'
        utterance.voice = speechSynthesis.getVoices().find(voice => voice.lang === langCode);
        utterance.pitch = 1;
        utterance.rate = 1;
        window.speechSynthesis.speak(utterance);
        setSpeakingStatus(index);
        utterance.onend = () => setSpeakingStatus(null)
      }
    } else {
      alert("Sorry, your browser does not support speech synthesis.");
    }
  };

  const stripHtmlTags = (text) => {
    const parser = new DOMParser();
    const doc = parser.parseFromString(text, "text/html");
    return doc.body.textContent || "";
  };


  const scrolltoTop = () => {
    setTimeout(() => {
      var element = document.getElementById('messages') || null;
      if (element != null) {
        element.scrollIntoView(true);
        element.scrollIntoView({ behavior: "smooth", block: "end", inline: "nearest" });
        element.scrollTop = 0;
      }
    }, 100);
  }

  // Function for handle are we still connected message to trigger after the time out

  const handleTimeoutSession = () => {
    let timeout = messageList?.automatedResponse && messageList?.automatedResponse?.chatTimeout > 0 ? messageList?.automatedResponse?.chatTimeout : 2;
    let sectimeout = messageList?.automatedResponse && messageList?.automatedResponse?.chatOtherInactivity > 0 && messageList?.automatedResponse?.chatOtherInactivity;
    const intervalId = setInterval(() => {
      const currentTime = Date.now()
      if (lastSendTime <= currentTime && !messageList?.reminderFlowStatus) {
        let msgTime = messageList?.automatedResponse !== undefined ? messageList?.automatedResponse?.chatInactivityMsg : 'Are we still connected?';
        if(!isFeatureEnabled('dontShowAreWeStillConnectAlert')) {
        handleMessageSession(msgTime);
        };
        if (messageList?.templateData?.length > 0) {
          setStillEnd(true);
        } else {
          chatEndMsg(msgTime, 1);
        };
        clearInterval(intervalId);
        clearTimeout(liveTrackerList?.timeOutIntervalData);
        sessionStorage.setItem("status", "Idle");
        let liveTrackData = {
          sesionId: messageList?.uuid,
          domainId: messageList.personalisationAll?.domainId,
          status: 'Idle'
        }
        siteTracking(liveTrackData);
        dispatch(messageActions.lottieProcess(false));
        if (sectimeout > 0) {
          const secTimeOutData = setTimeout(() => {
            handleSecondTimeoutSession();
          }, sectimeout * 60000);
          store.dispatch(liveTrackerActions.setTimeOutIntervalsData(secTimeOutData));
        } else {
          const lastTimeOutData = setTimeout(() => {
            handleLastTimeoutSession();
          }, timeout * 60000);
          store.dispatch(liveTrackerActions.setTimeOutIntervalsData(lastTimeOutData));
        }
      }

    }, 1000);
  }

  // Function for handle are we still connected message to trigger after the second time out

  const handleSecondTimeoutSession = () => {
    let timeout = messageList?.automatedResponse && messageList?.automatedResponse?.chatTimeout > 0 ? messageList?.automatedResponse?.chatTimeout : 2;
    const intervalId = setInterval(() => {
      const currentTime = Date.now()
      if (lastSendTime <= currentTime) {
        let msgTime = messageList?.automatedResponse !== undefined ? messageList?.automatedResponse?.chatOtherInactivityMsg : 'Are we still connected?';
        handleMessageSession(msgTime);
        clearInterval(intervalId);
        const lastTimeOutData = setTimeout(() => {
          handleLastTimeoutSession();
        }, timeout * 60000);
        store.dispatch(liveTrackerActions.setTimeOutIntervalsData(lastTimeOutData));
      }
    }, 1000);
  }
  // Function for handle are we still connected message to trigger after the last time out and auto clear the session

  const handleLastTimeoutSession = () => {
    const intervalId = setInterval(() => {
      const currentTime = Date.now()
      if (lastSendTime <= currentTime) {
        let msgTime = messageList?.automatedResponse !== undefined ? messageList?.automatedResponse?.chatTimeoutMsg :
          "Since I haven't received any response from you, I will be ending this chat session. Thank you for contacting " + messageList.personalisationAll?.title + "!";
        setStillEnd(false);
        setTimoutStatus(true)
        handleMessageSession(msgTime);
        clearInterval(intervalId);
        if (messageList?.templateData?.length === 0) {
          chatEndMsg(msgTime, 2);
        }
        if (messageList?.isprechatFrom) {
          createContactApi();
        }
        const sessionClearData = setTimeout(() => {
          handleSessionClear();
        }, 5000);
        store.dispatch(liveTrackerActions.setTimeOutIntervalsData(sessionClearData));
      }
    }, 1000);
  }

  // Clear all timeouts and setIntervals
  useEffect(() => {
    if (isFeatureEnabled('defaultWebsiteInsights')){
      getVisitorInsightsData()
    }
    if(isFeatureEnabled('webVisitorTracking')){
      getWebVisitorInsightsDataPothys()
    }
    return () => {
      let highestId = window.setTimeout(() => { }, 0);
      for (let i = 0; i <= highestId; i++) {
        clearTimeout(i);
        clearInterval(i);
      }
    };
  }, []);

  const copyCoupon = (data) => {
    navigator.clipboard.writeText(data);
  }

  const handleSessionClear = () => {
    try {
      nlpConvoSummary()
      const body = {
        startTmpstmp: startTimestampInSeconds,
        endTmpstmp: toTimestampInSeconds,
        timeZone: "",
        ChatDurationHistoryId: parseInt(sessionStorage?.ChatDurationHistoryId),
        IsAbandoned: 1,
        domainId: messageList?.personalisationAll?.domainId
      }
      abandonedChatDashboard(body)
      dispatch(messageActions.closeSession(true));
      dispatch(messageActions.sessionClosedStatus(true));
      clearSessionStorage();
    } catch (error) {
      console.log(error)
    }

  }

  const handleMessageSession = (msg) => {
    let timeOutdispatch = {
      messageID: uuidv4(),
      sessionId: messageList.uuid,
      sendAt: Date.now(),
      message: msg,
      TimeOutStatus: true,
      from: messageList.uuid,
      to: `${messageList.agentDomainExt.domainId}_${messageList.agentDomainExt.ext}`,
      isBot: true,
      isCustomer: false,
    };
    let timeOutText = {
      message: timeOutdispatch,
    };
    dispatch(messageActions.setMessageChat({ agentText: timeOutText }));
  }

  const chatDefaultMessage = () => {
    const data = {
      session_id: messageList.uuid,
      domain_id: messageList.personalisationAll?.domainId,
      channelType: 'Chat',
      call_state: 'init',
      callFlowId: messageList.personalisationAll?.chatFlowId,
      call_type: 'inbound',
      messageID: uuidv4(),
      ChatDurationHistoryId: parseInt(sessionStorage?.ChatDurationHistoryId),
      ChatHistoryId: sessionStorage?.ChatHistoryId,
      targetLang: messageList.personalisation?.dynamiclanguage !== undefined ? messageList.personalisation?.dynamiclanguage : 'en',
      liveTransfer: 1
    };
    chatMessageUpdate(data);
  }

  // Function for call the session create api (Chathistory and chatDurationHistory Id created)

  const createChatSession = async (flowId) => {
    try {
      const browserName = getBrowserName();
      const device = getDeviceType();
      let create_session_payload = {
        domainId: messageList.personalisationAll?.domainId,
        companyId: messageList?.companyId !== '' ? messageList?.companyId : 0,
        callFlowId: flowId,
        ipAddress: messageList.storeIpAddress,
        sessionId: messageList.uuid,
        webChatSessionId: messageList.uuid,
        customerData: messageList.customerFormData,
        browser: browserName,
        OS: window.navigator.platform,
        deviceType: device,
        widgetId: messageList.widgetId,
        CustomerId: messageList?.customerFormData?.coid
      };

      try {
        const createSessionResponse = await fetch(`${config().CALLSERVICE_URL}/create_chat_session`, {
          method: 'POST',
          headers: {
            'Content-Type': 'application/json',
          },
          body: JSON.stringify(create_session_payload),
        });

        if (!createSessionResponse.ok) {
          throw new Error(`Create chat session failed: ${createSessionResponse.status}`);
        }
        const createSessionData = await createSessionResponse.json();
        if (createSessionData?.statusCode === 200) {
          sessionStorage.setItem("chatUniqIdentifier", createSessionData.result[0].chatUniqIdentifier);
          sessionStorage.setItem("ChatHistoryId", createSessionData?.result[0].ChatHistoryId);
          dispatch(messageActions?.updateChatHistoryId(createSessionData?.result[0].ChatHistoryId));
          console.log('Success', messageList);
          const data = {
            session_id: messageList.uuid,
            domain_id: messageList.personalisationAll?.domainId,
            channelType: 'Chat',
            call_state: 'init',
            callFlowId: flowId,
            call_type: 'inbound',
            customerId: messageList.customerFormData?.coid,
            messageID: uuidv4(),
            ChatHistoryId: createSessionData?.result[0].ChatHistoryId,
            widgetId: messageList.widgetId,
            // targetLang: messageList.personalisation?.dynamiclanguage !== undefined ? messageList.personalisation?.dynamiclanguage : 'en'
            targetLang: messageList?.targetLang,

          };
          joinWebChat(create_session_payload);
          const connectionResponse = await fetch(`${config().CALLSERVICE_URL}/${liveTrackerList?.agentSessionAvaiableStatus && !liveTrackerList?.botSessionAvaiableStatus ? 'agent_connect_id_generate' : 'bot_connect_id_generate'}`, {
            method: 'POST',
            headers: {
              'Content-Type': 'application/json',
            },
            body: JSON.stringify(data),
          });

          if (!connectionResponse.ok) {
            throw new Error(`Connection ID generation failed: ${connectionResponse.status}`);
          };
          const connectionData = await connectionResponse.json();
          dispatch(messageActions.setChatDurationId(connectionData?.result[0][0]?.ChatDurationHistoryId));
          sessionStorage.setItem("ChatDurationHistoryId", connectionData?.result[0][0]?.ChatDurationHistoryId);
          data.ChatDurationHistoryId = parseInt(connectionData?.result[0][0]?.ChatDurationHistoryId);
          if (isFeatureEnabled('greetingMsg')){
            //  && !isFeatureEnabled('withPhonenumberCallWebsiteInsights')){

            if([grtDomain].includes(`${messageList?.personalisationAll?.domainId}`)){

              const name =  `${messageList?.customerFormData['First name'] || ""} ${messageList?.customerFormData['Last name'] || ""}`?.trim()
              if(name){
                const greeting = `Dear ${name}, Welcome to GRT Jewellers. How may I assist you today?`
                newChatSendMsg("Text", greeting , "AGENT", 0); // prechat form greeting
              }

            } else {
              newChatSendMsg("Text", messageList.personalisationAll?.greeting, "AGENT", 0);
            }

          }
          if (liveTrackerList?.openScreenTriggerStatus) {
            newChatSendMsg("Text", liveTrackerList?.openScreenTriggerMsg, "AGENT", 0);
            let payload = {
              "question": liveTrackerList?.openScreenTriggerMsg,
              "sessionId": messageList.uuid,
              "bot_id": 2,
              "botType": checkConvoSupported() ? 'convo' : "template",
              "domainId": messageList?.personalisationAll?.domainId,
              "websiteId": "",
              "ipaddress": messageList?.storeIpAddress,
              "company_name": messageList?.personalisationAll?.title ? messageList?.personalisationAll?.title : "",
              "isClose": 0,
              "industry_type": messageList?.industry_type,
              "do_stream": true,
              "socketId": messageList?.socketId,
              "industry_type": messageList?.industry_type,
              "chatId": parseInt(messageList?.personalisationAll?.domainId) === 17820 ? 346954 : messageList?.personalisationAll?.chatFlowId,
              "isTemplate": 0,
              "connector": false,
              "shop_name": "",
              "language": liveTrackerList?.selectedTextLanguage?.code,
              'imageUrl': '',
              'webchatUnquieId': messageList?.personalisationAll?.webchatid,
              'worktualCookieStatus': 1,
              'addonLanguages': liveTrackerList?.multiLanguageList,
              'clientLocation': messageList?.clientLocation,
              'chatHistoryId': parseInt(sessionStorage?.ChatHistoryId ?? 0),
              'ChatDurationHistoryId': parseInt(sessionStorage?.ChatDurationHistoryId ?? 0),
              'customerId' : messageList?.customerFormData?.coid ?? 0
            }
            sendChatBotReq(payload);
          };
          let conditionMet = false
          try {
            const planId = messageList?.personalisationAll?.plan_id;
            const productId = messageList?.personalisationAll?.product_id;
            const callFlowName = messageList?.personalisationAll?.callFlowName;
          
            const validCallFlows = ['Default Chat Flow', ''];
            const planIdsWithFlowCheck = new Set([55, 56, 33, 97, 103, 96, 107, 120, 121, 122, 123, 124 ]);
            const directPlanIds = new Set([54, 116, 42, 62]);
            conditionMet = (planIdsWithFlowCheck.has(planId) && validCallFlows.includes(callFlowName)) 
              || directPlanIds.has(planId) 
              || ( productId === 1 && (planId === 32 || planId === 76));
          } catch (error) {
            console.error('Condition evaluation failed:', error);
          }

          if (!conditionMet) {
            if (messageList?.triggerData && messageList?.triggerData?.visitor_says && messageList?.triggerData?.visitor_says.length > 0) {
              const flow_id = messageList?.triggerData?.visitor_says.filter((row) => row?.flowId === flowId);
              if (flow_id.length > 0) {
                dispatch(messageActions.setTriggerData({ ...messageList?.triggerData, flowData: data }));
              } else {
                chatMessageUpdate(data);
              }
            } else {
              chatMessageUpdate(data);
            }
          }
          sessionStorage.setItem("iconStatus", false);
          const connectionTypeData = {
            isBotConnection: true,
            isAgentConnection: sessionStorage.isConnected == "agent" ? true : false,
            isDTMFConnection: false,
          };
          dispatch(messageActions.setConnectionType(connectionTypeData));
          contactStorde();
        }
      } catch (err) {
        console.log('Err:::', err);
      }
    }
    catch (error) {
      console.log("error", error);
    }
    inboundChatDashboard({
      startTmpstmp: startTimestampInSeconds,
      endTmpstmp: toTimestampInSeconds,
      domainId: messageList?.personalisationAll?.domainId
    });
  }

  const updateSessionCount = async() => {
    try {
      let updateData = {};
      if ((messageList.personalisationAll?.plan_id === 54 || messageList.personalisationAll?.plan_id === 116 || messageList.personalisationAll?.plan_id === 76 || messageList.personalisationAll?.plan_id === 32)) {
        updateData = {
          domainId: messageList.personalisationAll?.domainId ?? 0,
          planId: messageList.personalisationAll?.plan_id,
          isBot: 0,
          isAgent: 0,
          IPAdress: messageList.storeIpAddress
        }
      } else {
        updateData = {
          domainId: messageList.personalisationAll?.domainId ?? 0,
          planId: messageList.personalisationAll?.plan_id,
          isBot: 1,
          isAgent: 0,
          IPAdress: messageList.storeIpAddress
        }
      }
      const updateResponse = await fetch(`${config().CORESERVICE_URL}/update_chat_session_count`, {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify(updateData),
      });
    } catch (error) {
      console.error("updateSessionCount Error", error)
    }
  }

  useEffect(() => {
    if (messageList.messageList?.length === 0 && messageList.customerBlockStatus !== 1 && (liveTrackerList?.agentSessionAvaiableStatus === true || liveTrackerList?.botSessionAvaiableStatus === true)) {
      if (messageList?.templateData?.length > 1) {
        dispatch(messageActions.multipleTemp(true))
      } else {
        createChatSession(messageList.personalisationAll?.chatFlowId)
      }
    }

    if (sessionStorage.isConnected == "bot" || sessionStorage.isConnected == "agent") {
      let data = {
        isBotConnection: sessionStorage.isConnected == "bot" ? true : false,
        isAgentConnection: sessionStorage.isConnected == "agent" ? true : false,
        isDTMFConnection: false,
      };
      dispatch(messageActions.setConnectionType(data));
      setDtmfBlock(false);
    } else if (sessionStorage.isConnected == "agent") {
      let data = {
        isBotConnection: false,
        isAgentConnection: true,
        isDTMFConnection: false,
      };
      dispatch(messageActions.setConnectionType(data));
      setDtmfBlock(false);
    } else {
      setDtmfBlock(true);
    }
    let divElement = document.getElementById('bot-layout');
    let width = divElement.clientWidth;
    let height = divElement.clientHeight;
    console.log('bot-layout Width:', width, 'px');
    console.log('bot-layout Height:', height, 'px');
  }, []);

  // UseEffect for show queued position to user when connecting to live agent

  useEffect(() => {
    if (parseInt(liveTrackerList?.queuedPositionData?.domainId) === parseInt(messageList?.personalisationAll?.domainId)) {
      try {
        const queueDetails = liveTrackerList?.queuedPositionData?.queueList;
        const parseResult = isJsonString(queueDetails)
        if (parseResult && JSON.parse(queueDetails).length > 0) {
          JSON.parse(queueDetails)?.map((list, index) => {
            if (list?.sessionId === messageList.uuid) {
              const parsedQueue = isJsonString(list?.queueAnnouncement)
              const announcementParsed = parsedQueue && JSON.parse(list?.queueAnnouncement)
              if (announcementParsed?.AnnouncementValue === 'Queue Number Announcement') {
                let queuedTextData = announcementParsed?.textToSpeech.replace('$waiting_position$', index + 1)
                setQueuedContent(queuedTextData)
              }
            }
          })
        }
      } catch (err) {
        console.log(err)
      }
    }
  }, [liveTrackerList?.queuedPositionData])


  const enterButton = (e) => {
    if (e.key === "Enter" && file !== null && file !== false) {
      sendFileMessage();
    } else if (e.key === "Enter" && message?.trim() !== "") {
      e.preventDefault();
      typingFalseFunction()
      sendMessage('text');
      if (typingTimeoutRef.current) {
        clearTimeout(typingTimeoutRef.current);
      }
    }
  };

  const sendFileMessage = async () => {
    try {
      const formData = new FormData();
      formData.append("doc", file);
      formData.append("sessionId", messageList.uuid);
      const response = await fetch(`${config().CALLSERVICE_URL}/create_customer_file_upload`, {
        method: 'POST',
        body: formData,
      });
      if (!response.ok) {
        throw new Error(`File upload failed: ${response.status}`);
      }
      const data = await response.json();
      setFileUrl(data.fileList[0].url);
      sendMessage('file', data.fileList[0]);
      if (messageList?.fileInputtype) {
        let dataFile = {
          childId: messageList?.currentNodeFlow?.data?.children[0].childId,
          type: 'error'
        };
        continusBlock(dataFile);
        store.dispatch(messageActions.setFileInputtype(false));
      }
      setSendIcon(false);
      setFile(false);
    } catch (error) {
      console.error("Upload failed", error);
    }
  };


  const sendMessage = async (type, fileRes) => {
    // Update after user initite msgp
    if (!messageList?.isUserChatInitiated && messageList.customerBlockStatus !== 1 && (liveTrackerList?.agentSessionAvaiableStatus === true || liveTrackerList?.botSessionAvaiableStatus === true)) {
      if (!messageList?.templateData?.length) {
        updateSessionCount()
        dispatch(messageActions.userChatInitiated());
      }
    }

    if (liveTrackerList?.sendMessageEnable && !messageList?.reminderFlowStatus) {
      let messageToSend = message?.replace(/(<([^>]+)>)|&nbsp;/gi, " ");;
      console.log(messageToSend, "messageToSend");
      if (liveTrackerList?.agentDisconnectStatus) {
        await connectBotAfterAgent()
      }
      if (builderList?.ecommercePurchase?.isPurchase === true && builderList?.catchEmail === true) {
        newChatSendMsg('Text', messageToSend, "IN", 0);
        const emailRegex = new RegExp(/^([a-zA-Z0-9._%+-]+)@([a-zA-Z0-9.-]+)\.([a-zA-Z]{2,})$/);
        if (emailRegex.test(messageToSend.trim())) {
          dispatch(chatBuilderActions.setCustomerEmail(messageToSend));
          dispatch(chatBuilderActions.setCatchEmail(false));
          getCustomerData(messageToSend);
        } else {
          const emailErrorMsg = 'Please provide the valid mail Id for furthur process'
          newChatSendMsg('Text', emailErrorMsg, "OUT", 0);
        }
      } else {
        if (type === "file") {
          if(messageToSend){
            fileRes.message = messageToSend
          }
          messageToSend += `\n${fileRes.name} (${Math.round(fileRes.size / 1024)} KB)`;
          newChatSendMsg('file', fileRes, "IN", 0);
          if (builderList?.connectorStatus) {
            const payload = {
              "domain_id": messageList?.personalisationAll?.domainId,
              "url": fileRes.url,
              "max_no_result": 15,
              "score_filter": 0,
              "company": builderList?.connectionName.toLowerCase(),
            };
            try {
              const response = await fetch(`${config()?.ECOMMERCE_IMAGE_SEARCH}`, {
                method: 'POST',
                headers: {
                  'Content-Type': 'application/json',
                },
                body: JSON.stringify(payload),
              });
              if (!response.ok) {
                throw new Error(`Error: ${response.status} ${response.statusText}`);
              }
              const res = await response.json();
              if (res?.data?.ecommerce_product?.length > 0) {
                const ImageContent = 'These are the search results related to the image you provided.';
                newChatSendMsg('Text', ImageContent, "OUT", 1);
                const recommendModifyIntent = [];
                for (let key in res?.data?.recomended_product) {
                  recommendModifyIntent.push(key);
                }
                dispatch(chatBuilderActions.setEcommercePurchase({
                  isPurchase: true,
                  nlpRecommended: res?.data?.recomended_product,
                  recommendedIntent: recommendModifyIntent
                }));
                ecommerceProdcutSearch(res?.data?.ecommerce_product);
              } else {
                const message = "Oops! Looks like the item you're searching for couldn't be found.";
                newChatSendMsg("Text", message, "OUT", 0);
              }
            } catch (error) {
              console.error("API request failed", error);
            };
          } else {
            let site = JSON.parse(messageList?.personalisationAll?.websiteId);
            let planId = (messageList?.personalisationAll?.plan_id === 33 || messageList?.personalisationAll?.plan_id === 56 || messageList?.personalisationAll?.plan_id === 55 || messageList?.personalisationAll?.plan_id === 141) && messageList?.templateData?.length > 0;
            let payload = {
              "question": '',
              "sessionId": messageList.uuid,
              "bot_id": 2,
              "messageType": 'file',
              // "botType": messageList?.templateData?.length > 0 ? "template" : "convo",
              "botType": checkConvoSupported() ? 'convo' : "template",
              "domainId": messageList?.personalisationAll?.domainId,
              // "websiteId": site?.length > 0 ? site[0] : "",
              "websiteId": Array.isArray(site) && site.length > 0 ? site[0] : site,
              "ipaddress": messageList?.storeIpAddress,
              "company_name": messageList?.personalisationAll?.title ? messageList?.personalisationAll?.title : "",
              "isClose": 0,
              "socketId": messageList?.socketId,
              "industry_type": messageList?.industry_type,
              "do_stream": messageList?.templateData?.length > 0 && messageList?.simOrderSts ? false : true,
              "socketId": messageList?.socketId,
              "chatId": parseInt(messageList?.personalisationAll?.domainId) === 17820 ? 346954 : messageList?.personalisationAll?.chatFlowId,
              "isTemplate": planId ? 1 : 0,
              "connector": builderList?.connectorStatus,
              "shop_name": builderList?.connectionName.toLowerCase() ?? "",
              "language": liveTrackerList?.selectedTextLanguage?.code,
              'imageUrl': fileRes.url,
              'webchatUnquieId': messageList?.personalisationAll?.webchatid,
              'worktualCookieStatus': 0,
              'addonLanguages': liveTrackerList?.multiLanguageList,
              'clientLocation': messageList?.clientLocation,
              'chatHistoryId': parseInt(sessionStorage?.ChatHistoryId ?? 0),
              'ChatDurationHistoryId': parseInt(sessionStorage?.ChatDurationHistoryId ?? 0),
              'customerId' : messageList?.customerFormData?.coid ?? 0
            }
            if (fileRes.url && messageList.connectionType.isBotConnection) {
              sendChatBotReq(payload);
              dispatch(messageActions.lottieProcess(true));
            }
          };
        };
        let worktualResult = null;
        // if ([11165, 18144].includes(parseInt(messageList?.personalisationAll?.domainId)) && !liveTrackerList?.worktualCookiesData) {
        //   worktualResult = await storeWorktulaCookiesData();
        //   dispatch(liveTrackerActions.setWorktualCookiesData(worktualResult))
        // }
        if (typeof messageToSend === "string" && messageToSend?.trim() !== "" && type !== 'file') {
          newChatSendMsg('Text', messageToSend, "IN", 0);
          setSendIcon(false);
          if (parseInt(sessionStorage?.ChatHistoryId) && [54, 116, 76, 32].includes(messageList.personalisationAll?.plan_id) && messageList.connectionType.isBotConnection) {
            console.log("\n ----  chatDefaultMessage   --- \n");
            chatDefaultMessage();
            dispatch(messageActions.liveAgentStatus(1))
            setTimeout(async () => {
              handleConnectionType("agent");
            }, 1500);
            var text = 'Ofcourse! Please bear with me while I connect you to an available agent.'
            dispatch(messageActions.setAgentConnectingStatus(true))
          } else {
            let site = JSON.parse(messageList?.personalisationAll?.websiteId);
            let planId = (messageList?.personalisationAll?.plan_id === 33 || messageList?.personalisationAll?.plan_id === 56 || messageList?.personalisationAll?.plan_id === 55 || messageList?.personalisationAll?.plan_id === 141) && messageList?.templateData?.length > 0;
            if (messageList.connectionType.isBotConnection && liveTrackerList?.botSessionAvaiableStatus === true && parseInt(sessionStorage?.ChatHistoryId)) {
              let payload = {
                "question": messageToSend,
                "sessionId": messageList.uuid,
                "bot_id": 2,
                "messageType": 'text',
                // "botType": messageList?.templateData?.length > 0 ? "template" : "convo",
                "botType": checkConvoSupported() ? 'convo' : "template",
                "domainId": messageList?.personalisationAll?.domainId,
                // "websiteId": site?.length > 0 ? site[0] : "",
                "websiteId": Array.isArray(site) && site.length > 0 ? site[0] : site,
                "ipaddress": messageList?.storeIpAddress,
                "company_name": messageList?.personalisationAll?.title !== undefined ? messageList?.personalisationAll?.title : "",
                "isClose": 0,
                "socketId": messageList?.socketId,
                "industry_type": messageList?.industry_type,
                "do_stream": messageList?.templateData?.length > 0 && messageList?.simOrderSts ? false : true,
                "socketId": messageList?.socketId,
                "industry_type": messageList?.industry_type,
                // "chatId": messageList?.personalisationAll?.chatFlowId,
                "chatId": parseInt(messageList?.personalisationAll?.domainId) === 17820 ? 346954 : messageList?.personalisationAll?.chatFlowId,
                "isTemplate": planId ? 1 : 0,
                "connector": builderList?.connectorStatus,
                "shop_name": builderList?.connectionName.toLowerCase() ?? "",
                "language": liveTrackerList?.selectedTextLanguage?.code,
                'imageUrl': '',
                // 'worktualCookiesData': worktualResult,
                'uniqueCookieId': await getIndexDbItem('uniqueChatId'),
                'webchatUnquieId': messageList?.personalisationAll?.webchatid,
                'worktualCookieStatus': 0,
                'addonLanguages': liveTrackerList?.multiLanguageList,
                'clientLocation': messageList?.clientLocation,
                'chatHistoryId': parseInt(sessionStorage?.ChatHistoryId ?? 0),
                'ChatDurationHistoryId': parseInt(sessionStorage?.ChatDurationHistoryId ?? 0),
                'customerId' : messageList?.customerFormData?.coid ?? 0
              }
              let messageSend = true;
              if (messageList?.triggerData && messageList?.triggerData?.visitor_says && messageList?.triggerData?.visitor_says.length > 0) {
                let index = 0
                for (let data of messageList?.triggerData?.visitor_says) {
                  if (data?.flowId === messageList?.callFlowId &&
                    data.questions?.map(str => str.toLowerCase()).includes(messageToSend?.toLowerCase())) {
                    chatMessageUpdate({ ...messageList?.triggerData?.flowData, callFlowId: data?.flowId, targetLang: messageList?.targetLang, });
                    const visitor_says = [...messageList?.triggerData?.visitor_says]
                    visitor_says.splice(index, 1)
                    dispatch(messageActions.setTriggerData({ ...messageList?.triggerData, visitor_says }))
                    messageSend = false
                    break
                  }
                  index++
                }
              }
              if (messageSend) {
                sendChatBotReq(payload);
                dispatch(messageActions.lottieProcess(true));
                if (messageList?.ansCaptureStatus) {
                  let arr = [...messageList.inputEntity];
                  let index = messageList.inputEntity.findIndex((item) => {
                    return Object.keys(item).includes(messageList.ansCaptureKey);
                  });
                  if (index !== -1) {
                    arr[index] = { ...arr[index], [messageList.ansCaptureKey]: messageToSend };
                  } else {
                    arr = [...arr, { [messageList.ansCaptureKey]: messageToSend }];
                  }
                  dispatch(messageActions.inputEntity(arr));
                }
              }
            }
          }

        }
      }
      handleSend();
      setMessage("");
      setFile(false);
      dispatch(messageActions.emojiVisible(false));
      let timeout = messageList?.automatedResponse && messageList?.automatedResponse?.chatInactivity > 0 ? messageList?.automatedResponse?.chatInactivity : 5;
      clearTimeout(liveTrackerList?.timeOutIntervalData);
      const otpValidation = messageList?.currentNodeFlow?.data;
      if (otpValidation?.information?.inputType === 5 || otpValidation?.information?.inputType === 6) {
        let parentData = messageList?.nodeData.find(o => o?.id === otpValidation?.children?.[0]?.childId);
        const optCompanyData = isJSON(parentData?.data?.information?.data) ? JSON.parse(parentData?.data?.information?.data) : parentData?.data?.information?.data;
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
        if (!toId)
          toId = messageToSend
        if (parentData?.data?.information?.inputType === 4) {
          let otp_payload = {
            domainId: messageList?.personalisationAll?.domainId,
            toId,
            expiryTime: parseInt(optCompanyData?.getInputData?.otp_expiry),
            businessName: optCompanyData?.getInputData?.business_name
          };
          dispatch(chatBuilderActions.setOtpValidationId(otp_payload?.toId));
          dispatch(chatBuilderActions.setOtpResendButtonStatus(true));
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
          dispatch(chatBuilderActions.setOtpValidCount(0));
          dispatch(chatBuilderActions.setOtpResendCount(0));
          resendOtpGenerator(parseInt(optCompanyData?.getInputData?.otp_expiry))
          const createSessionData = await otpResponse.json();
          if (createSessionData?.statusCode === 200) {
            //
          }
        }
      }
      if (messageList.connectionType.isBotConnection && !builderList?.showUpdateAddresss && !builderList?.showCustomerForm && !isFeatureEnabled('dontShowAreWeStillConnectAlert')) {
        const timeOutData = setTimeout(() => {
          handleTimeoutSession();
        }, timeout * 60000);
        store.dispatch(liveTrackerActions.setTimeOutIntervalsData(timeOutData));
      }
      if (messageList?.fileInputtype) {
        store.dispatch(messageActions.setFileInputtype(false));
      }
      stopCapturing();
      resetTranscript();
    }
  };

  // UseEffect handle the connecting message
  useEffect(() => {
    if (messageList?.paymentEnable || messageList?.worktualAppointment?.isShow || messageList?.integration?.isCalendlyShow) {
      clearTimeout(liveTrackerList?.timeOutIntervalData);
    }
  }, [messageList]);

  // Function check the existing customer or not (Ecommerce Flow)

  const getCustomerData = async (email) => {
    if (builderList?.connectionName === 'Wix') {
      const end_point = `${config()?.WIX_TOKEN_GENERATOR}/${messageList.personalisationAll.domainId}`
      const res_data = await getIntegrationApiData(end_point, '')
      if (res_data?.result.length > 0) {
        const tokenId = res_data?.result[0]?.token;
        const end_point_refresh = `${config()?.WIX_REFRESH_TOKEN_GENERATOR}/${messageList.personalisationAll.domainId}`
        const res_data_refresh = await getIntegrationApiData(end_point_refresh, tokenId);
        const token = res_data_refresh?.results?.token
        let url = `${config().WIX_EXISTING_CUSTOMER}/${email}`
        store.dispatch(chatBuilderActions.setEcommercePurchase({ token }));
        const response = await getIntegrationApiData(url, token);
        if (response?.result?.contacts?.length > 0) {
          dispatch(chatBuilderActions.setAddressList(response?.result?.contacts));
          dispatch(chatBuilderActions.setShowAddress(true));
          const customerData = response?.result?.contacts;
          dispatch(chatBuilderActions.setCustomerUpdateAddress(customerData))
        } else {
          dispatch(chatBuilderActions.setEnableCustomerForm(true));
        }
      }
    } else if (builderList?.connectionName === 'BigCommerce') {
      let url = `${config().BIGCOMMERCE_EXISTING_CUSTOMER}/${email}`
      let token = builderList?.ecommercePurchase?.token;
      const response = await getIntegrationApiData(url, token);
      if (response?.statusCode === 200 && response?.result?.data.length > 0) {
        const result = response?.result?.data[0]
        const items = [{ 'address': { addressLine: result?.addresses[0]?.address1, addressLine2: result?.addresses[0]?.address2, city: result?.addresses[0]?.city, postalCode: result?.addresses[0]?.postal_code } }]
        const addressListData = [{
          'info': { 'addresses': { 'items': items } }
        }]
        dispatch(chatBuilderActions.setAddressList(addressListData));
        dispatch(chatBuilderActions.setShowAddress(true));
        const payload = {
          "firstName": result?.first_name,
          "lastName": result?.last_name,
          "email": result?.email,
          "phoneCurrencyCode": 'GB',
          "phoneNumber": result?.phone,
          "contactId": result?.addresses[0]?.customer_id,
          "address": {
            "addressLine": result?.addresses[0]?.address1,
            "addressLine2": result?.addresses[0]?.address2,
            "city": result?.addresses[0]?.city,
            "postalCode": result?.addresses[0]?.postal_code,
            "country": result?.addresses[0]?.country,
            "state": result?.addresses[0]?.state_or_province,
            "country_code": result?.addresses[0]?.country_code

          }
        };
        dispatch(chatBuilderActions.setCustomerPaymentData(payload));
      } else {
        dispatch(chatBuilderActions.setEnableCustomerForm(true));
      }
    } else if (builderList?.connectionName === 'Magento') {
      let url = `${config().MAGENTO_EXISTING_CUSTOMER}/${email}`
      let token = builderList?.ecommercePurchase?.token;
      const response = await getIntegrationApiData(url, token);
      if (response?.statusCode === 200 && response?.result?.items.length > 0) {
        const result = response?.result?.items[0];
        const addressMap = await Promise.all(result?.addresses?.map((a_list) => {
          let a_data = {
            'address': {
              addressLine: a_list?.street[0],
              addressLine2: a_list?.street[0],
              city: a_list?.city,
              postalCode: a_list?.postcode
            }
          }
          return a_data
        }))
        const items = addressMap
        const addressListData = [{
          'info': { 'addresses': { 'items': items } }
        }]
        dispatch(chatBuilderActions.setAddressList(addressListData));
        dispatch(chatBuilderActions.setShowAddress(true));
        const payload = {
          "firstName": result?.firstname,
          "lastName": result?.lastname,
          "email": result?.email,
          "phoneCurrencyCode": 'GB',
          "phoneNumber": result?.addresses[0]?.telephone,
          "contactId": result?.addresses[0]?.customer_id,
          "address": {
            "addressLine": result?.addresses[0]?.street[0],
            "addressLine2": result?.addresses[1]?.street[0],
            "city": result?.addresses[0]?.city,
            "postalCode": result?.addresses[0]?.postcode,
            "country_code": result?.addresses[0]?.country_id,
            "region": result?.addresses[0]?.region?.region,
          }
        };
        dispatch(chatBuilderActions.setCustomerPaymentData(payload));
      } else {
        dispatch(chatBuilderActions.setEnableCustomerForm(true));
      }
    } else if (builderList?.connectionName === 'WooCommerce') {
      let url = `${config().WOOCOMMERCE_EXISTING_CUSTOMER}/${email}`
      let token = builderList?.ecommercePurchase?.token;
      const response = await getIntegrationApiData(url, token);
      if (response?.statusCode === 200 && response?.result?.length > 0) {
        const result = response?.result?.[0]
        const items = [{ 'address': { addressLine: result?.billing?.address_1, addressLine2: result?.billing?.address_2, city: result?.billing?.city, postalCode: result?.billing?.postcode } }]
        const addressListData = [{
          'info': { 'addresses': { 'items': items } }
        }]
        dispatch(chatBuilderActions.setAddressList(addressListData));
        dispatch(chatBuilderActions.setShowAddress(true));
        const payload = {
          "firstName": result?.first_name,
          "lastName": result?.last_name,
          "email": result?.email,
          "userName": result?.userName,
          "phoneCurrencyCode": 'GB',
          "phoneNumber": result?.billing?.phone,
          "contactId": result?.id,
          "address": {
            "addressLine": result?.billing?.address_1,
            "addressLine2": result?.billing?.address_2,
            "city": result?.billing?.city,
            "postalCode": result?.billing?.postcode,
            "country": result?.billing?.country,
            "state": result?.billing?.state,
          }
        };
        dispatch(chatBuilderActions.setCustomerPaymentData(payload));
      } else {
        dispatch(chatBuilderActions.setEnableCustomerForm(true));
      }
    }
  }

  const dateFormatter = (date) => {
    const dateInfo = new Date(date);
    const hours = dateInfo.getHours().toString().padStart(2, "0");
    const minutes = dateInfo.getMinutes().toString().padStart(2, "0");
    const timeString = `${hours}:${minutes}`;
    return timeString;
  };

  const handleFileChange = (e) => {
    if (imageFormat.indexOf(e.target.files[0]?.type) > -1 || excelFormats.indexOf(e.target.files[0]?.type) > -1 || pptFormats.indexOf(e.target.files[0]?.type) > -1 || wordFormats.indexOf(e.target.files[0]?.type) > -1 || pdfFormats.indexOf(e.target.files[0]?.type) > -1) {
      const selectedFile = e.target.files[0];
      selectedFile['filepath'] = window.URL.createObjectURL(selectedFile)
      setFile(selectedFile);
      setSendIcon(true);
      document.getElementById('file-input').value = null;
    } else {
      console.log('File Type not allowed')
    }
  };

  const handleFileChangeForImage = (e) => {
    if (imageFormat.indexOf(e.target.files[0]?.type) > -1 || excelFormats.indexOf(e.target.files[0]?.type) > -1 || pptFormats.indexOf(e.target.files[0]?.type) > -1 || wordFormats.indexOf(e.target.files[0]?.type) > -1 || pdfFormats.indexOf(e.target.files[0]?.type) > -1) {
      const selectedFile = e.target.files[0];
      selectedFile['filepath'] = window.URL.createObjectURL(selectedFile)
      setFile(selectedFile);
      setSendIcon(true);
    } else {
      console.log('File Type not allowed')
    }
  };

  const handleBigCommercePayment = () => {
    if (builderList?.selectedPayment !== '') {
      store.dispatch(chatBuilderActions.setShowBigCommercePayment(false));
      const result = builderList?.bigCommercePaymentType?.filter((list) => {
        if (list?.name === builderList?.selectedPayment) {
          return list
        }
      });
      store.dispatch(chatBuilderActions.setBigCommerceSelectedId(result));
      store.dispatch(chatBuilderActions.setCommonPaymentForm(true))
    }
  }

  // Function handle when user click continue shopping. It will process if there is any recommended item

  const shoppingContinue = () => {
    newChatSendMsg('Text', 'Continue shopping', "IN", 0);
    if (builderList?.ecommercePurchase?.productIntent?.length > 0) {
      const resData = builderList?.ecommercePurchase?.productIntent
      store.dispatch(chatBuilderActions.setEcommercePurchase({ productIntent: resData.slice(1) }));
      if (resData?.length > 1) {
        ecommerceTemplateKey(resData);
      } else if (resData?.length === 1) {
        if (builderList?.connectionName === 'shopify' || builderList?.connectionName === 'Wix' || builderList?.connectionName === 'WooCommerce') {
          const selectedArray = builderList?.ecommercePurchase?.nlpProductIntent[resData[0]];
          ecommerceProdcutSearch(selectedArray)
        } else if (builderList?.connectionName === 'BigCommerce' || builderList?.connectionName === 'Magento') {
          const selectedArray = builderList?.ecommercePurchase?.productIntent[0];
          ecommerceProdcutSearch(selectedArray)
        }

      }
    } else {
      if (builderList?.ecommercePurchase?.recommendedIntent?.length > 0) {
        store.dispatch(chatBuilderActions.setEcommercePurchase({ nlpProductIntent: builderList?.ecommercePurchase?.nlpRecommended }));
        setTimeout(() => {
          checkoutProductStock()
        }, 2000);
      } else {
        let recommendMsg = `Could you specify which product you are interested in?`;
        newChatSendMsg('Text', recommendMsg, "OUT", 0);
      }
    }
  }

  // Function for shopping item checkout in ecommerce

  const shoppingCheckout = async () => {
    if (builderList?.selectedCardItems?.length > 0) {
      if (builderList?.connectionName === 'shopify') {
        let body = {
          line_items: builderList?.selectedCardItems?.map(list => ({
            variant_id: list?.id,
            quantity: list?.quantityCount
          }))
        };
        const response = await postIntegrationApiData(config()?.SHOPIFY_CHECKOUT, builderList?.ecommercePurchase?.token, body)
        store.dispatch(chatBuilderActions.setSelectedCardItemsEmpty());
        store.dispatch(chatBuilderActions.setEcommercePurchase({ productIntent: [] }));
        if (response?.statusCode === 200) {
          const shopUrl = response?.result?.checkout?.web_url;
          window.open(shopUrl, '_blank', 'width=1000,height=auto');
        } else if (response?.statusCode === 422) {
          const msg = `We're sorry, but the item you selected is currently out of stock. If you'd like, we can help you find a suitable alternative. Please get in touch with us for assistance`;
          newChatSendMsg('Text', msg, "OUT", 1);
        }
        setTimeout(() => {
          checkoutProductStock()
        }, 2000);
      } else if (builderList?.connectionName === 'Wix') {
        checkoutExistingCustomer();
      } else if (builderList?.connectionName === 'BigCommerce') {
        checkoutExistingCustomerForBc();
      } else if (builderList?.connectionName === 'Magento') {
        checkoutExistingCustomerForMagento();
      } else if (builderList?.connectionName === 'WooCommerce') {
        wooCommerceOrderCreation()
      }
    } else {
      setTimeout(() => {
        checkoutProductStock()
      }, 2000);
    }
  }

  const handleClose = () => {
    setFile(!file);
    setSendIcon(false);
  };

  const typingTimeoutRef = useRef(null);

  const handleMessage = (e) => {
    let text = e.target.value;

    if (typingTimeoutRef.current) {
      clearTimeout(typingTimeoutRef.current);
    }
    try {
      if (text?.trim() !== "") {
        setSendIcon(true);
        if (messageList?.connectionType?.isAgentConnection) {
          let body = {
            isBotTyping: true,
            from: messageList.uuid,
            to: `${messageList?.agentDomainExt?.domainId}_${messageList?.agentDomainExt?.ext}`,
            sessionId: messageList?.uuid,
          };
          oneToOneChatSendTyping(body)
        }
      } else {
        resetTranscript();
        setIsMic(true)
        setSendIcon(false);
      }
      typingTimeoutRef.current = setTimeout(() => {
        typingFalseFunction();
      }, 1500);
    } catch (error) {
      console.log(error)
    }
    setMessage(e.target.value);
  };

  const typingFalseFunction = () => {
    let body = {
      isBotTyping: false,
      from: messageList.uuid,
      to: `${messageList?.agentDomainExt?.domainId}_${messageList?.agentDomainExt?.ext}`,
      sessionId: messageList?.uuid,
    };
    oneToOneChatSendTyping(body)
  };


  const setPaymentMethodForFlow = (method) => {
    newChatSendMsg("Text", method, "IN", 0);
    const paymentData = {
      domainId: messageList?.personalisationAll?.domainId,
      amount: sessionStorage.getItem("AMOUNT"),
      currency: "INR"
    }
    const paymentEncrypt = encryptData(paymentData)
    store.dispatch(messageActions.paymentEnable(true))
    store.dispatch(messageActions.paymentSource(method))
    store.dispatch(messageActions.paymentId(paymentEncrypt))
  }

  const handleAddressYes = (data) => {
    newChatSendMsg('Text', 'Yes', "IN", 0);
    dispatch(chatBuilderActions.setSelectedAddress(data));
    let carousalPayloads = {
      messageID: uuidv4(),
      from: messageList.uuid,
      to: messageList?.connectionType?.isBotConnection ? messageList.uuid : `${messageList.agentDomainExt.domainId}_${messageList.agentDomainExt.ext}`,
      sessionId: messageList.uuid,
      message: `Select your preferred payment method`,
      sendAt: Date.now(),
      messageType: 'paymentMode',
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
    oneToOneChatSend(carousalPayloads);
    let texts = {
      message: carousalPayloads,
    };
    dispatch(messageActions.setMessageChat({ agentText: texts }));
  }
  const handleAddressNo = (method) => {
    //
  }

  const handlePaymentMode = (method) => {
    newChatSendMsg('Text', method, "IN", 0);
    const productDetails = [];
    const totalAmount = [];
    builderList?.selectedCardItems?.map((list) => {
      totalAmount.push(list?.price * list?.quantityCount)
      const payload = {
        "id": list?.p_id,
        "title": list?.imgTitle,
        "unitPrice": list?.price * 100?.toString(),
        "quantity": list?.quantityCount,
        "category": "DIGITAL"
      }
      productDetails.push(payload)
    });
    let sumOfAmount = 0;
    totalAmount?.map((list) => {
      sumOfAmount = sumOfAmount + list
    })
    const data = {
      "items": productDetails,
      "firstName": builderList?.coustomerPaymentData?.firstName,
      "lastName": builderList?.coustomerPaymentData?.lastName,
      "phone": builderList?.coustomerPaymentData?.phoneNumber,
      "company": builderList?.coustomerPaymentData?.company,
      "email": builderList?.coustomerPaymentData?.email,
      "postalCode": builderList?.coustomerPaymentData?.address?.postalCode,
      "country": builderList?.coustomerPaymentData?.address?.country,
      "city": builderList?.coustomerPaymentData?.address?.city,
      "address": builderList?.selectedAddress,
      "amount": sumOfAmount * 100?.toString(),
      "orginalAmount": sumOfAmount,
      "streetAddressNumber": 20,
    }
    localStorage.setItem('WixPayment', JSON.stringify(data));
    if (method === 'Cash on delivery') {
      makeWixPayment(method, 'cash')
    } else if (method === 'Online') {
      const paymentData = {
        ...data,
        domainId: messageList?.personalisationAll?.domainId,
        currency: "INR"
      }
      const paymentEncrypt = encryptData(paymentData)
      store.dispatch(messageActions.paymentEnable(true));
      store.dispatch(messageActions.paymentSource('wix'));
      store.dispatch(messageActions.paymentId(paymentEncrypt));
    }
  }

  const handleDtmfInput = (data) => {
    newChatSendMsg('Text', data.type, "IN", 0);

    let nextValue = messageList.dtmfData.filter((item) => {
      return item.nodeId == data.childId;
    });
    if (nextValue.length > 0) {
      dispatch(messageActions.setCurrentDtmfData(nextValue));
    } else {
      messageList.nodeData.some(checkCurrentNode);

      function checkCurrentNode(list) {
        switch (list.id === data.childId) {
          case true:
            console.log(list, "list");
            let text = {
              message: {
                message: list.data.information.chatMessage,
                sendAt: Date.now(),
                sessionId: messageList.uuid,
                messageID: uuidv4(),
                isBot: true,
                isCustomer: false,
              },
            };
            messageList.nodeData.some(checkNextNode);
            function checkNextNode(value) {
              switch (value.id === list.data.children[0].childId) {
                case true:
                  switch (value.type) {
                    case "transferToQueue":
                      let connectionData = {
                        isBotConnection: false,
                        isAgentConnection: true,
                        isDTMFConnection: false,
                      };
                      setTimeout(() => {
                        dispatch(
                          messageActions.setMessageChat({ agentText: text })
                        );
                      }, 1000);
                      setTimeout(() => {
                        sessionStorage.setItem("isConnected", "agent");
                        dispatch(
                          messageActions.setConnectionType(connectionData)
                        );
                        makeApiCall();
                      }, 1500);

                      break;
                    case "customInput":
                      console.log(value, "valuevalue");

                      let connectionData1 = {
                        isBotConnection: true,
                        isAgentConnection: false,
                        isDTMFConnection: false,
                      };
                      dispatch(
                        messageActions.setConnectionType(connectionData1)
                      );
                      setBotStatus(true);
                      sessionStorage.setItem("isConnected", "bot");
                      initiateBot("Hi");
                  }
                  break;
                case false:
                  break;
              }
            }
            break;
          case false:
            console.log("false");
            break;

          default:
            console.log("default");
        }
      }
    }



    setDtmfBlock(false);

  };

  const sendTextMessage = () => {
    typingFalseFunction();
    sendMessage('Text');
  };

  const handleFeedBackChange = (event) => {
    setFeedback(event?.target?.value);
  }

  const handleLikeChange = (res, likeres) => {
    if (likeres === 2) {
      setIsFeedback(true)
      setLikeData({ data: res, likeres: likeres })
    } else {
      handleSendLikeDisFeedback(res, likeres)
    }
  }

  const handleCancelFeedback = () => {
    setIsFeedback(false);
    setFeedback('');
  }

  const handleSendLikeFeedback = () => {
    try {
      let data = { ...likeData?.data };
      data.message = { ...data.message, isLike: likeData?.likeres, feedBack: feedback };
      updateThumbsUp(data.message)
      const newMessageList = messageList?.messageList.map((value) => {
        if (value?.agentText && data.message?.messageID === value?.agentText?.message?.messageID && data.message?.messageID) {
          let newMessage = { ...value.agentText };
          newMessage.message = { ...newMessage.message, isLike: likeData?.likeres, feedBack: feedback }
          let likeagent = { agentText: newMessage }
          return likeagent;
        } else {
          return value;
        }
      });
      dispatch(messageActions.setLikeMessage(newMessageList));
      if (likeData?.likeres === 2) {
        console.log(data, "data")
        unhelpfulResponsiveStore(data)
      }
      handleCancelFeedback();
    } catch (error) {
      handleCancelFeedback();
      console.log('handleSendLikeFeedback', error)
    }
  }

  const handleSendLikeDisFeedback = (res, likeres) => {
    try {
      let data = { ...res };
      data.message = { ...data.message, isLike: likeres, feedBack: feedback };
      updateThumbsUp(data.message)
      const newMessageList = messageList?.messageList.map((value) => {
        if (value?.agentText && data.message?.messageID === value?.agentText?.message?.messageID && data.message?.messageID) {
          let newMessage = { ...value.agentText };
          newMessage.message = { ...newMessage.message, isLike: likeres, feedBack: feedback }
          let likeagent = { agentText: newMessage }
          return likeagent;
        } else {
          return value;
        }
      });
      dispatch(messageActions.setLikeMessage(newMessageList));
      if (likeData?.likeres === 2) {
        console.log(data, "data")
        unhelpfulResponsiveStore(data)
      }
      handleCancelFeedback();
    } catch (error) {
      handleCancelFeedback();
      console.log('handleSendLikeFeedback', error)
    }
  }

  function urlify(text) {
    if (text !== undefined) {
      var urlRegex = /(https?:\/\/[^\s]+)/g;
      return text?.replace(urlRegex, function (url) {
        return '<a target="_blank" href="' + url + '">' + url + "</a>";
      });
    }

  }

  const handleTempInput = async (item, type, child) => {
    if (item !== undefined) {
      sessionStorage.setItem("item", JSON.stringify(item));
      newChatSendMsg("Text", item?.type, "IN", 0);
      if (item?.childId === 'product_List12') {
        const filterData = builderList?.ecommercePurchase?.productIntent?.filter((list) => list !== item?.type)
        store.dispatch(chatBuilderActions.setEcommercePurchase({ productIntent: filterData }));

        if (builderList?.connectionName === 'shopify' || builderList?.connectionName === 'Wix' || builderList?.connectionName === 'WooCommerce') {
          const selectedArray = builderList?.ecommercePurchase?.nlpProductIntent[item?.type]
          await ecommerceProdcutSearch(selectedArray);
        } else if (builderList?.connectionName === 'BigCommerce') {
          const selectedArray = item?.type
          await ecommerceProdcutSearch(selectedArray);
        }

        return;
      };
      let caroType = item?.childId?.includes('carousel');
      const data1 = {
        session_id: messageList.uuid,
        domain_id: messageList.personalisationAll?.domainId,
        channelType: 'Chat',
        response: "success",
        btnValue: item?.childId === "breakFlow_no" || item?.childId === "remainderFlow_yes" ? messageList?.flowNodeList?.btnValue ?? messageList?.remainderFlowId : item?.childId,
        callFlowId: item?.childId === "remainderFlow_yes" ? parseInt(messageList?.remainderFlowData?.flowId) ?? messageList.personalisationAll?.chatFlowId : type?.chatFlowId ?? messageList.personalisationAll?.chatFlowId,
        call_type: messageList?.reminderFlowStatus ? messageList?.reminderFlowValue : item?.type,
        messageID: uuidv4(),
        carousal_type: caroType ? child : "",
        stream_finished: messageList.personalisationAll?.plan_id === 56 || messageList.personalisationAll?.plan_id === 55 ? 3 : "",
        //ChatHistoryId: parseInt(localStorage.ChatHistoryId),
        customerId: messageList.customerFormData?.coid,
        ChatDurationHistoryId: parseInt(sessionStorage.ChatDurationHistoryId),
        ChatHistoryId: parseInt(sessionStorage.ChatHistoryId),
        targetLang: messageList.personalisation?.dynamiclanguage !== undefined ? messageList.personalisation?.dynamiclanguage : 'en',
        inputParams: messageList?.simOrderSts ? [...messageList?.inputEntity, { amount: item?.type?.replace('£', '') }] : '',
      };
      chatMessageUpdate(data1);
      store.dispatch(chatBuilderActions.setFlowIntentName(item?.type))
      if (item?.childId === "breakFlow_yes") {
        dispatch(messageActions.simOrderSts(false));
        breakFlowMsg();
        dispatch(messageActions.remainderAction({ isRemainder: true, count: 1 }))
        dispatch(messageActions.setRemainderFlowId(messageList?.currentNodeFlow?.data?.nodeId))
      }

      if (item?.childId === "breakFlow_no") {
        dispatch(messageActions.setRemainderFlowId(''))
        dispatch(messageActions.simOrderSts(true));
      }

      if (messageList?.simOrderSts) {
        let arr = [...messageList?.inputEntity, { amount: item?.type?.replace('£', '') }]
        dispatch(messageActions.inputEntity(arr));
      }
      let timeout = messageList?.automatedResponse && messageList?.automatedResponse?.chatInactivity > 0 ? messageList?.automatedResponse?.chatInactivity : 5;

      clearTimeout(liveTrackerList?.timeOutIntervalData);
      if (messageList.connectionType.isBotConnection && !builderList?.showUpdateAddresss && !builderList?.showCustomerForm) {
        const timeOutData = setTimeout(() => {
          handleTimeoutSession();
        }, timeout * 60000);
        store.dispatch(liveTrackerActions.setTimeOutIntervalsData(timeOutData));
      }
      var trans = item?.childId?.includes('transferToQueue');
      if (trans) {
        dispatch(messageActions.setAgentConnectingStatus(true));
        handleConnectionType("agent");
      }
      if (item?.link !== '' && item?.link !== undefined) {
        if([grtDomain].includes(`${messageList?.personalisationAll?.domainId}`)){
          // GRT post message handler
          grtProductPostmessage(item?.link)
        } else {
          window.open(item?.link, '_blank');
        }
      }
      if (item?.childId === "remainderFlow_yes") {
        dispatch(messageActions.remainderAction({ isRemainder: false, count: 0 }));
        dispatch(messageActions.simOrderSts(true));
      } else if (item?.childId === "remainderFlow_no") {
        dispatch(messageActions.remainderAction({ isRemainder: false, count: 0 }))
        dispatch(messageActions.simOrderSts(false));
        newChatSendMsg("Text", "If you want any further questions please feel free to ask", "OUT", 0);
      } else if (item?.childId === "recommandFlow_no") {
        dispatch(messageActions.simOrderSts(false));
        newChatSendMsg("Text", "If you want any further questions please feel free to ask", "OUT", 0);

      }
      dispatch(messageActions.setRecommedFlowValue(''));
      dispatch(messageActions.setRecommedFlowStatus(false));
    }
  }

  const handlePayment = (e) => {
    newChatSendMsg("Text", e, "IN", 0);
    const paymentData = {
      domainId: messageList?.personalisationAll?.domainId,
      amount: sessionStorage.getItem("AMOUNT"),
      currency: "INR"
    }
    const paymentEncrypt = encryptData(paymentData)
    dispatch(messageActions.paymentEnable(true))
    dispatch(messageActions.paymentSource(e))
    dispatch(messageActions.paymentId(paymentEncrypt))
  }


  const handleInputType = (event, name) => {
    setPayForm((payForm) => ({
      ...payForm,
      [name]: event?.target?.value,
    }));
    if (name === "cardNumber" && event?.target?.value?.length === 16) {
      let type = getCardType(event?.target?.value);
      setPayForm((payForm) => ({
        ...payForm,
        cardType: type === "mastercard" ? "mc" : type
      }));
    }
  }
  const handleSendPay = () => {
    sessionStorage.setItem("payment", true);
    let test = messageList?.currentNodeFlow?.data?.children[0].childId;
    var arrRes = [];
    arrRes.push(payForm);
    const data1 = {
      session_id: messageList.uuid,
      domain_id: messageList.personalisationAll?.domainId,
      channelType: 'Chat',
      response: "success",
      btnValue: test,
      callFlowId: messageList.personalisationAll?.chatFlowId,
      call_type: 'pay',
      messageID: uuidv4(),
      ChatDurationHistoryId: parseInt(sessionStorage?.ChatDurationHistoryId),
      ChatHistoryId: parseInt(sessionStorage.ChatHistoryId),
      targetLang: messageList.personalisation?.dynamiclanguage !== undefined ? messageList.personalisation?.dynamiclanguage : 'en',
      inputParams: [...arrRes, { notificationurl: "https://liveurapi.worktual.co.uk/vectone-myaccount/v1/user/callback_url" }],
    };
    chatMessageUpdate(data1);
    var inpt = [...arrRes, { notificationurl: "https://liveurapi.worktual.co.uk/vectone-myaccount/v1/user/callback_url" }]
    dispatch(messageActions.cardDetails(inpt));
    dispatch(messageActions.paymentForm(false));
  }
  const test = [...messageList.messageList];

  const startNewConv = () => {
    sessionStorage.setItem("iconStatus", true)
    window.location.reload();
  }

  const closeEndSession = () => {
    try {
      nlpConvoSummary()
      const chatDetails = {
        ChatHistoryId: parseInt(sessionStorage?.ChatHistoryId),
        ChatDurationHistoryId: parseInt(sessionStorage?.ChatDurationHistoryId),
        sessionId: messageList?.uuid,
        isBot: messageList?.connectionType?.isBotConnection,
      }
      updateEndSession(chatDetails);
      if (messageList?.templateData?.length === 0) {
        chatEndMsg("closed", 2);
      }
      clearSessionStorage();
      sendRNCloseChat();
    } catch (error) {
      console.log(error);
    }
  }

  const contEndSession = () => {
    dispatch(messageActions.endNotify(false))
  };

  const handleCloseSession = (msg) => {
    store.dispatch(messageActions.templateKeywordList([]));
    if (msg === 'No') {
      newChatSendMsg("Text", msg, "IN");
      setTimeout(() => {
        let data = "If you require further assistance, please initiate a new chat, and I will be happy to help you"
        newChatSendMsg("Text", data, "OUT", 0);
        const timeOutData = setTimeout(() => {
          handleSessionClear();
        }, 3000);
        store.dispatch(liveTrackerActions.setTimeOutIntervalsData(timeOutData));
      }, 1000);
    } else {
      newChatSendMsg("Text", msg, "IN", 0);
      setTimeout(() => {
        let data = "Thank you for confirming. How can I assist you further?"
        newChatSendMsg("Text", data, "OUT", 0);
      }, 1000);
    }
    setStillEnd(false);
    clearTimeout(liveTrackerList?.timeOutIntervalData);
    const body = {
      startTmpstmp: startTimestampInSeconds,
      endTmpstmp: toTimestampInSeconds,
      timeZone: "",
      ChatDurationHistoryId: parseInt(sessionStorage?.ChatDurationHistoryId),
      IsAbandoned: 1,
      domainId: messageList?.personalisationAll?.domainId
    }
    abandonedChatDashboard(body)
  }

  const callBackClick = () => {
    if (messageList?.dynamicChatFormList?.length === 0) {
      let data = {
        ChatHistoryId: parseInt(messageList?.chatHistoryIdSlice),
        domainId: messageList?.personalisationAll?.domainId
      }
      updateCallbackData(data)
    }
    dispatch(messageActions.callBackButtonStatus(0));
    dispatch(messageActions.callBackStatus(1));

  }

  const continueChatClick = () => {
    dispatch(messageActions.callBackButtonStatus(0));
    dispatch(messageActions.setCallbackBtnStatus(false))
    let connectionData = {
      isBotConnection: true,
      isAgentConnection: false,
      isDTMFConnection: false,
    };
    sessionStorage.setItem("isConnected", "bot");
    dispatch(messageActions.setConnectionType(connectionData));
    dispatch(messageActions.transferCallCount(0));
  };

  // Window Replacement
  useEffect(() => {
    if (builderList?.checkoutUrl !== '') {
      console.log('builderList::::', builderList?.checkoutUrl)
      anchorRef.current.click();
      dispatch(chatBuilderActions.setCheckoutUrl(''))
    }
  }, [builderList?.checkoutUrl])

  useEffect(() => {
    const handleMessage = (event) => {
      if (event.data.type === 'PAYMENT_IFRAME') {
        store.dispatch(messageActions.paymentEnable(false));
        store.dispatch(messageActions.paymentSource(""));
        store.dispatch(messageActions.paymentForm(false));
        const message_data = store.getState().message
        let msg = ''
        let status = "success"
        if (event.data.message.status === 'success') {
          if (message_data?.integration?.isMsGoogleShow)
            store.dispatch(messageActions.setIntegration({ ...message_data.integration, isPaymentSuccess: true }))
          msg = "Your payment was successful. Your transaction reference ID is " + event?.data?.message?.orderId + ". Thank you";
          newChatSendMsg("Text", msg, "OUT", 0);
        } else {
          status = "error"
          msg = "Payment failed. Please check your card details and try again or use an alternative payment method"
          newChatSendMsg("Text", msg, "OUT", 0);
        }
        let uniqueChatId = sessionStorage.getItem("item");
        let parentId = JSON.parse(uniqueChatId);
        let filtered_array = messageList?.nodeData.find(o => o.id === parentId?.childId);
        const childId = filtered_array?.data?.children?.filter((item) => item?.type === status)[0]?.childId;
        let data = {
          childId: childId,
          type: 'error'
        }
        setTimeout(() => {
          continusBlock(data);
        }, 1000);

      } else if (event.data.type === 'WORKTUAL_APPOINTMENT_IFRAME') {
        let msg = ''
        console.log("WORKTUAL_APPOINTMENT_IFRAME", event)
        if (event.data.message.status === 'success' && event.data.message.type === 'schedule') {
          setTimeout(() => {
            store.dispatch(messageActions.setWorktualBooking({ ...messageList.worktualAppointment, isShow: false }))
            let uniqueChatId = sessionStorage.getItem("item");
            let jsonData = isJsonString(uniqueChatId) ? JSON.parse(
              uniqueChatId) : ""
            let filtered_array = JSON.parse(uniqueChatId).find(o => o.type === "success");
            let testt = event?.data?.message?.confirmation_data;
            let email = isJsonString(testt?.p_book_form) ? JSON.parse(testt?.p_book_form) : "";
            let emailRes = email?.length > 0 && isJsonString(testt?.p_book_form) ? email[3].Email : ''
            let arr = [...messageList?.inputEntity, { date: testt?.p_book_date, time: testt?.p_book_endtime, email: emailRes }]
            dispatch(messageActions.inputEntity(arr));
            const confirmation_data = event.data.message.confirmation_data
            const customer = isJsonString(confirmation_data?.p_book_form) ? JSON.parse(confirmation_data?.p_book_form) : null
            const isCareHome = (messageList?.domainDetails.length > 0 && messageList?.domainDetails[0]?.iscareHome === 1) ? true : false
            msg = `Thank you! ${isCareHome ? 'Your visiting slot has been booked successfully­­­.' : ' for sharing the appointment details. You will receive a confirmation message/call shortly.'} 
            <span className="worktualnamelists">${isCareHome ? 'Booking' : 'Appointment'} ID </span>
            <strong className="block-detail">${confirmation_data?.app_booked_id || "N/A"}</strong>
            <span className="worktualnamelists">Booking date</span>
            <strong className="block-detail">${confirmation_data?.p_book_date || "N/A"}</strong>
            <span className="worktualnamelists">Timing</span>
            <strong className="block-detail">${confirmation_data?.p_book_starttime || "N/A"} - ${confirmation_data?.p_book_endtime || "N/A"}</strong>
            ${confirmation_data?.service ? `<span className="worktualnamelists">Service</span>
            <strong className="block-detail">${confirmation_data?.service || "N/A"}</strong>` : ''}
            ${(confirmation_data?.p_pay_amount && confirmation_data?.isEnablePricing !== 0) ? `<span className="worktualnamelists">Amount paid</span>
            <strong className="block-detail">${confirmation_data?.p_pay_amount || "N/A"}</strong>` : ''}
            <span className="worktualnamelists">Name</span>
            <strong className="block-detail">${customer !== null ? (customer[0]["First name"] + " " + customer[1]["Last name"]) : "N/A"}</strong>
            <span className="worktualnamelists">Email</span>
            <strong className="block-detail">${customer !== null ? customer[3].Email : "N/A"}</strong>
            <span className="worktualnamelists">Phone</span>
            <strong className="block-detail">${customer !== null ? customer[2]["Phone number"] : "N/A"}</strong>`.trim();
            let data = {
              childId: filtered_array?.childId,
              type: 'error'
            }
            newChatSendMsg("Text", msg, "OUT", 0);
            setTimeout(() => {
              continusBlock(data);
            }, 3000);
            store.dispatch(messageActions.templateKeywordList([]));
          }, 4000);

        } else if (event.data.message.status === 'success' && event.data.message.type === 'Reschedule') {
          store.dispatch(messageActions.setWorktualBooking({ ...messageList.worktualAppointment, isShow: false }))
          setTimeout(() => {
            let uniqueChatId = sessionStorage.getItem("item");
            let filtered_array = JSON.parse(uniqueChatId).find(o => o.type === "success");
            let data = {
              childId: filtered_array?.childId,
              type: 'error'
            }
            let msg = 'Your booking has been successfully rescheduled. An email confirmation will be sent to you shortly.'
            newChatSendMsg("Text", msg, "OUT", 0);
            setTimeout(() => {
              continusBlock(data);
            }, 4000);
          }, 3000);
        } else if (event.data.message.status === 'success' && event.data.message.type === 'cancel') {
          store.dispatch(messageActions.setWorktualBooking({ ...messageList.worktualAppointment, isShow: false }))
          setTimeout(() => {
            let uniqueChatId = sessionStorage.getItem("item");
            let filtered_array = JSON.parse(uniqueChatId).find(o => o.type === "success");
            let data = {
              childId: filtered_array?.childId,
              type: 'error'
            }
            let msg = "Your booking has been successfully canceled. You'll receive a confirmation email shortly to acknowledge the cancellation."
            newChatSendMsg("Text", msg, "OUT", 0);
            setTimeout(() => {
              continusBlock(data);
            }, 4000);
          }, 3000);
        } else {
          store.dispatch(messageActions.setWorktualBooking({ ...messageList.worktualAppointment, isShow: false }))
          msg = "It seems like you haven't completed the booking. Is there anything else I can help with?";
          newChatSendMsg("Text", msg, "OUT", 0);
          dispatch(messageActions.simOrderSts(false));
          store.dispatch(messageActions.templateKeywordList([]));
        }
        if (parseInt(messageList?.personalisationAll?.domainId) === 17820) {
          store.dispatch(messageActions.personalisationAll({ ...messageList?.personalisationAll, chatFlowId: 346954 }));
        };
        store.dispatch(messageActions.setRecommedFlowStatus(false));
        store.dispatch(liveTrackerActions.setSendMessageStatus(true));
      } else if (event.data.type === 'MYACCOUNT_PURCHASE_IFRAME') {
        dispatch(messageActions.setPurchasePlanStatus(false));
        dispatch(messageActions.setPurchasePlanUrl(''));
        if (event.data.message.status === 'success') {
          let textMessage = 'Kindly check your email for the product activation link to complete the purchase.'
          newChatSendMsg("Text", textMessage, "OUT", 0);
        }
      } else if (event.data.type === "WIX_PAYMENT_IFRAME") {
        store.dispatch(messageActions.paymentEnable(false));
        store.dispatch(messageActions.paymentSource(""));
        store.dispatch(messageActions.paymentForm(false));
        console.log("iframemessae", event?.data?.message);
        if (event?.data?.message?.authorization?.status === "SUCCEEDED") {
          makeWixPayment(event?.data?.message, 'online')
        } else {
          const failedMsg = 'Oops! Payment failed. Check your card details and try again.'
          newChatSendMsg("Text", failedMsg, "OUT", 0);
        }
      } else if (event.data.type === "WORKTUAL_PAYMENT") {
        store.dispatch(messageActions.paymentEnable(false))
        store.dispatch(messageActions.paymentSource(''));
        let site = JSON.parse(messageList?.personalisationAll?.websiteId);
        let payloads = {
          messageID: uuidv4(),
          from: messageList.uuid,
          to: messageList?.connectionType?.isBotConnection ? messageList.uuid : sessionStorage?.agentToAddress,
          message: '',
          sendAt: Date.now(),
          messageType: '',
          pdfurl: '',
          fileType: '',
          isBot: true,
          isCustomer: true,
          sender: '',
          isLike: 0,
          type: '',
          isLiveagent: false,
          direction: '',
          ChatHistoryId: parseInt(sessionStorage?.ChatHistoryId),
          ChatDurationHistoryId: parseInt(sessionStorage?.ChatDurationHistoryId),
          socketId: messageList?.socketId,
          assistType: checkConvoSupported() ? "Convo" : "Template",
          supervisorList: messageList?.superVisorExt,
          language: liveTrackerList?.selectedTextLanguage?.code,
          question: '',
          bot_id: 2,
          // botType: messageList?.templateData?.length > 0 ? "template" : "convo",
          botType: checkConvoSupported() ? 'convo' : "template",
          domainId: messageList?.personalisationAll?.domainId,
          sessionId: messageList.uuid,
          websiteId: site?.length > 0 ? site[0] : "",
          ipaddress: messageList?.storeIpAddress,
          isClose: 0,
          company_name: messageList?.personalisationAll?.title ? messageList?.personalisationAll?.title : "",
          do_stream: messageList?.templateData?.length > 0 && messageList?.simOrderSts ? false : true,
          socketId: messageList?.socketId,
          webchatUnquieId: messageList?.personalisationAll?.webchatid,
          clientLocation: messageList?.clientLocation,
          chatHistoryId: parseInt(sessionStorage?.ChatHistoryId ?? 0),
          ChatDurationHistoryId: parseInt(sessionStorage?.ChatDurationHistoryId ?? 0),
          customerId: messageList?.customerFormData?.coid ?? 0
        }
        if (event.data.message.status === 'success') {
          let msg = "Your payment was successful. Your transaction reference ID is " + event?.data?.message?.orderId + "Please wait for a moment your onboard link is generating...";
          newChatSendMsg("Text", msg, "OUT", 0);
          payloads = {
            ...payloads,
            purchase_status: 'Success',
            payment_id: event?.data?.message?.orderId,
          }
          console.log('payloads>>>>>>>>>>>>>>>>>>>>>>', payloads)
        } else {
          let msg = "Payment failed. Please check your card details and try again or use an alternative payment method"
          newChatSendMsg("Text", msg, "OUT", 0);
          payloads = {
            ...payloads,
            purchase_status: 'Failure',
            payment_id: ''
          }
          console.log('payloads>>>>>>>>>>>>>>>>>>>>>>', payloads)
        }
        worktualPurchase(payloads);
        store.dispatch(messageActions.paymentId(''))
      }
    };
    window.addEventListener('message', handleMessage); // Cleanup
    return () => {
      window.removeEventListener('message', handleMessage);
    };
  }, [])

  const handleArticlePreview = (item) => {
    setarticlePre(true);
    setarticleDes(item)
  }

  const handleArticleClose = () => {
    setarticlePre(false);
    setarticleDes('');
  }

  const handleInputChange = (event) => {
    let daata = messageList?.currentNodeFlow?.data?.children;
    newChatSendMsg("Text", event, "IN", 0);
    let findAgent1 = daata.find(o => o?.type === event);
    store.dispatch(messageActions.dropDownList(''));
    if (messageList?.ansCaptureStatus) {
      let arr = [...messageList.inputEntity];
      let index = messageList.inputEntity.findIndex((item) => {
        return Object.keys(item).includes(messageList.ansCaptureKey);
      });
      if (index !== -1) {
        arr[index] = { ...arr[index], [messageList.ansCaptureKey]: event };
      } else {
        arr = [...arr, { [messageList.ansCaptureKey]: event }];
      }
      dispatch(messageActions.inputEntity(arr));
    }
    store.dispatch(messageActions.setAnsCaptureStatus(false));
    store.dispatch(messageActions.setAnsCaptureKey(''));
    let data = {
      childId: findAgent1?.childId,
      type: 'error'
    }
    continusBlock(data);
  }
  const handleMultipleFlow = (list) => {
    let person = { ...messageList.personalisationAll };
    person.chatFlowId = list?.chatFlowId;
    createChatSession(list?.chatFlowId);
    dispatch(messageActions.personalisationAll(person));
    dispatch(messageActions.multipleTemp(false))
    dispatch(messageActions.callFlowId(list?.chatFlowId));
    getchatFlowFind(list?.chatFlowId, messageList.personalisationAll?.domainId)
  }

  const handleShopifyNewAddress = async () => {
    if (builderList?.selectedAddress !== '') {
      dispatch(chatBuilderActions.setShowAddress(false));
      if (builderList?.selectedAddress === 'addNewAddress') {
        if (builderList?.connectionName === 'Wix') {
          dispatch(chatBuilderActions.setShowUpdateAddresss(true))
          dispatch(chatBuilderActions.setSelectedAddress({ selectedAddressValue: '', indexValue: 0 }))
        } else if (builderList?.connectionName === 'WooCommerce') {
          dispatch(chatBuilderActions.setShowUpdateAddresss(true))
          dispatch(chatBuilderActions.setSelectedAddress({ selectedAddressValue: '', indexValue: 0 }))
        }
      } else {
        if (builderList?.connectionName === 'Wix') {
          let carousalPayloads = {
            messageID: uuidv4(),
            from: messageList.uuid,
            to: messageList?.connectionType?.isBotConnection ? messageList.uuid : `${messageList.agentDomainExt.domainId}_${messageList.agentDomainExt.ext}`,
            sessionId: messageList.uuid,
            message: `Select your preferred payment method`,
            sendAt: Date.now(),
            messageType: 'paymentMode',
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
          oneToOneChatSend(carousalPayloads);
          let texts = {
            message: carousalPayloads,
          };
          dispatch(messageActions.setMessageChat({ agentText: texts }));
          const customerResponse = builderList?.customerUpdateAddress[0]

          const payload = {
            "firstName": customerResponse?.info?.name?.first,
            "lastName": customerResponse?.info?.name?.last,
            "email": customerResponse?.info?.emails?.items[0]?.email,
            "phoneCurrencyCode": 'GB',
            "phoneNumber": customerResponse?.info?.phones?.items[0]?.phone,
            "contactId": customerResponse?.id,
            "address": {
              "addressLine": customerResponse?.info?.addresses?.items[builderList?.selectedAddressIndex]?.address?.addressLine,
              "addressLine2": customerResponse?.info?.addresses?.items[builderList?.selectedAddressIndex]?.address?.addressLine2,
              "city": customerResponse?.info?.addresses?.items[builderList?.selectedAddressIndex]?.address?.city,
              "postalCode": customerResponse?.info?.addresses?.items[builderList?.selectedAddressIndex]?.address?.postalCode,
              "country": customerResponse?.info?.addresses?.items[builderList?.selectedAddressIndex]?.address?.country,
            }
          };
          dispatch(chatBuilderActions.setCustomerPaymentData(payload));
        } else if (builderList?.connectionName === 'BigCommerce') {
          bigcommerceOrderCreation()
        } else if (builderList?.connectionName === 'Magento') {
          magentoOrderCreation()
        } else if (builderList?.connectionName === 'WooCommerce') {
          wooCommerceOrderCreation()
        }
      }
    }

  }
  const addQuantity = async (data, event) => {
    let finalData = [];
    builderList?.selectedCardItems?.forEach((list, index) => {
      if (index === event) {
        if (data === 'minus') {
          if (list?.quantityCount > 1) {
            list = { ...list, quantityCount: list?.quantityCount - 1 };
          } else if (list?.quantityCount === 1) {
            return;
          }
        } else if (data === 'plus') {
          list = { ...list, quantityCount: list?.quantityCount + 1 };
        }
      }
      finalData.push(list);
    });
    finalData = finalData.filter(item => item);
    store.dispatch(chatBuilderActions.setSelectedCardItemsEmpty());
    store.dispatch(chatBuilderActions.setTotalCardItems(finalData));
  }

  const worktualAppointmentCancelReschedule = (list, module) => {
    try {
      if (module === 'cancel') {
        store.dispatch(messageActions.setWorktualBooking({ ...messageList.worktualAppointment, isShow: true, url: list?.cancel }))
      } else if (module === 'reschedule') {
        store.dispatch(messageActions.setWorktualBooking({ ...messageList.worktualAppointment, isShow: true, url: list?.reschedule }))
      }
    } catch (error) {
      console.log(error)
    }
  };

  const handleDrop = (event) => {
    event.preventDefault();
    let files = [...event.dataTransfer.files];
    if (files.length === 0) {
      console.error("No files were dropped.");
      return;
    }
    const selectedFile = files[0];
    if (!(selectedFile instanceof File)) {
      console.error("Dropped item is not a valid file.");
      return;
    }
    try {
      selectedFile['filepath'] = window.URL.createObjectURL(selectedFile);
      setFile(selectedFile);
      setSendIcon(true);
    } catch (error) {
      console.error("Failed to create object URL:", error);
    }
  };


  const handleDragEnter = (event) => {
    event.preventDefault();
  };

  const handleDragOver = (event) => {
    event.preventDefault();
  };


  const parentDivRef = useRef(null);
  const [parentHeight, setParentHeight] = useState(0);
  const [viewportHeight, setViewportHeight] = useState("100vh"); // Default to 100vh
  const [isMobile, setIsMobile] = useState(false); // State for detecting mobile devices

  const updateParentHeight = () => {
    if (parentDivRef.current) {
      setParentHeight(parentDivRef.current.offsetHeight);
    }
  }
  const textareaRef = useRef(null);

  useEffect(() => {
    if (textareaRef.current?.resizableTextArea?.textArea) {
      const realTextArea = textareaRef.current.resizableTextArea.textArea;
      realTextArea.scrollTop = realTextArea.scrollHeight;
    }
  }, [message]);



  // Detect viewport height and mobile device
  useEffect(() => {
    const updateViewportHeight = () => {
      const mobileDevice = /Mobi|Android/i.test(navigator.userAgent);
      setIsMobile(mobileDevice); // Update isMobile state
      const heightValue = mobileDevice ? "100dvh" : "100vh"; // Use 100dvh for mobile, 100vh for others
      setViewportHeight(heightValue);
    };

    updateViewportHeight();
    window.addEventListener("resize", updateViewportHeight); // Recheck on resize

    return () => window.removeEventListener("resize", updateViewportHeight); // Cleanup
  }, []);

  // Initialize parent height on mount
  useEffect(() => {
    updateParentHeight();
  }, []);


  const handleMouseEvent = () => {
    if(liveTrackerList?.multiLanguageList?.length > 1){
      setMicLanguageStatus(true)
    }
  };


  const containerRef = useRef(null);
  const caresRef = useRef(null);
  const bottomRef = useRef(null);
  const bottomRef1 = useRef(null);
  const prevLength = useRef(test.length);
  const chatBottomHeight = useRef(0);

  // Track bottom div height
  useEffect(() => {
    if (!bottomRef.current || typeof ResizeObserver === "undefined") return;

    const ro = new ResizeObserver((entries) => {
      for (let entry of entries) {
        chatBottomHeight.current = entry.contentRect.height;
      }
    });

    ro.observe(bottomRef.current);

    return () => ro.disconnect();
  }, []);

  useLayoutEffect(() => {
    const container = containerRef.current;
    if (!container) return;

    const prev = prevLength.current;
    const curr = test.length;
    const isNewMessage = curr > prev;

    if (isNewMessage) {
      const lastMsg = test[curr - 1];
      const isCares = (!!lastMsg?.agentText?.message?.is_pothys || !!lastMsg?.agentText?.message?.is_nickys);
      if (isCares && caresRef.current) {
        const caresEl = caresRef.current;
        const offset = chatBottomHeight.current; // use bottom div height dynamically
        container.scrollTo({
          top: caresEl.offsetTop - offset, // scroll cares to top minus bottom div height
          behavior: "smooth",
        });
      } else {
        // Normal messages scroll to bottom (this part is critical to maintain the scroll at bottom)
        container.scrollTo({
          top: container.scrollHeight, // scroll to the very bottom
          behavior: "smooth",
        });
      }
    } else if (messageList.agentConnectingStatus || liveTrackerList?.agentTypingStatus) {
      container.scrollTo({
        top: container.scrollHeight,
        behavior: "smooth",
      });
    }

    prevLength.current = curr;
  }, [test, messageList.agentConnectingStatus, liveTrackerList?.agentTypingStatus]);

  const [rectDiv, setRectDiv] = useState({})

  useEffect(() => {
    const div = document.querySelector('.chatbottomview');
    const rect = div.getBoundingClientRect();
    setRectDiv(rect)
  }, [])

  useEffect(() => {
    if (bottomRef.current) {
      bottomRef.current.scrollIntoView({ behavior: "smooth" });
    }
  }, [message]); // run whenever messages update

useEffect(() => {
    if (bottomRef1.current) {
      bottomRef1.current.scrollIntoView({ behavior: "smooth" });
    }
  }, [messageList?.particialOutput]); // triggers on updates



  return (
    <>
      <div className="chatinfo" onDrop={(event) => handleDrop(event)}
        onDragOver={(event) => handleDragOver(event)}
        onDragEnter={(event) => handleDragEnter(event)}>
        <div className={!messageList?.worktualAppointment?.isShow && !builderList?.feedBackFormStatus && !messageList?.purchasePlanData?.purchasePlanStatus && !messageList?.reminderFlowStatus ? "chatbottomview" : "chatbottomview withnofooter"} style={
          isMobile
            ? { height: `calc(${viewportHeight} - ${parentHeight}px - 108px)` }
            : undefined // No inline styles for non-mobile devices
        }>
          

          {messageList?.multipleTemp && <div
            className="chatMessage chatMessagefoot scroller"
            id="messages"
          //ref={scrollRef}
          ><div className="welcomeMessage">
              <ul className="welcomeList">
                {messageList?.templateData && messageList?.templateData.map((list, index) => {
                  if (Object.keys(messageList?.triggerData).length === 0 || messageList?.triggerData?.flow_id.includes(list?.chatFlowId))
                    return (
                      <>
                        <li onClick={() => handleMultipleFlow(list)} className="gapFlow">
                          <div className="listText">
                            <div className="welcomeListh5">{list?.templateName}</div>
                            {/* <span> Chat Flow</span> */}
                          </div>
                          <span className="icon__Rightarrow16" >
                            <svg width="24" height="25" viewBox="0 0 24 25" fill="none" xmlns="http://www.w3.org/2000/svg">
                              <path d="M9.84761 18.1658C9.65521 18.1658 9.46281 18.0926 9.31641 17.9451C9.02391 17.6521 9.02491 17.1775 9.31841 16.8846L13.1827 13.0321C13.3248 12.8905 13.4029 12.701 13.4029 12.4999C13.4029 12.2997 13.3248 12.1112 13.1827 11.9696L9.31801 8.11614C9.02501 7.82314 9.02411 7.34854 9.31651 7.05564C9.60851 6.76174 10.0836 6.76174 10.3775 7.05364L14.2418 10.9071C14.6676 11.3319 14.9024 11.8973 14.9029 12.4999C14.9034 13.1025 14.6685 13.6679 14.2418 14.0946L10.3771 17.9471C10.2311 18.0926 10.039 18.1658 9.84761 18.1658Z" fill="#757676" />
                            </svg>
                          </span>
                        </li>
                      </>
                    )
                })}
              </ul>
            </div></div>}
          {articlePre &&
            <div className="articleexpand">
              <h4 onClick={handleArticleClose}><svg xmlns="http://www.w3.org/2000/svg" width="6" height="12" viewBox="0 0 6 12" fill="none">
                <path d="M5.15326 11.666C4.96186 11.666 4.76996 11.5928 4.62346 11.4472L0.758756 7.59371C0.332956 7.16891 0.098157 6.60351 0.097657 6.00091C0.097157 5.39831 0.332056 4.83291 0.758756 4.40621L4.62396 0.553711C4.91696 0.261711 5.39206 0.261712 5.68446 0.555712C5.97696 0.848712 5.97596 1.32331 5.68246 1.61621L1.81776 5.46871C1.67566 5.61031 1.59756 5.79981 1.59756 6.00091C1.59756 6.20111 1.67566 6.38961 1.81776 6.53121L5.68296 10.3847C5.97596 10.6777 5.97686 11.1523 5.68446 11.4452C5.53796 11.5927 5.34556 11.666 5.15326 11.666Z" fill="#757676" />
              </svg>{articleDes?.title} </h4>
              <p>{parse(urlify(articleDes?.description))}</p>
              <a href={articleDes?.url} target="_blank">{articleDes?.url} </a>
            </div>}
          <audio style={{ display: "none" }} id="msg_notify" src="" controls></audio>


          {builderList?.commonPaymentForm && <CommonPaymentForm />}
          {/* </Modal>  */}
          {messageList?.paymentEnable && messageList.paymentSource.toLowerCase() == "stripe" ? <iframe src={`${config().CHATBOT_PAYMENT_SERVICE}/stripe?payment_id=${messageList.paymentId}`} width="100%" height="100%" scrolling="yes" style={{ padding: '0 16px' }}></iframe> : ""}
          {messageList?.paymentEnable && messageList.paymentSource.toLowerCase() == "paypalnew" ? <iframe src={`${config().CHATBOT_PAYMENT_SERVICE}/paypal?payment_id=${messageList.paymentId}`} className="paypalview" scrolling="yes" ></iframe> : ""}
          {messageList?.paymentEnable && messageList.paymentSource.toLowerCase() == "paypal" ? <iframe src={`${config().CHATBOT_PAYMENT_SERVICE}/paypalbraintree?payment_id=${messageList.paymentId}`} className="paypalview" scrolling="yes" ></iframe> : ""}
          {messageList?.paymentEnable && messageList.paymentSource.toLowerCase() == "globalpayment" ? <iframe src={`${config().CHATBOT_PAYMENT_SERVICE}/chatpayment?payment_id=${messageList.paymentId}`} width="100%" height="360" scrolling="yes"></iframe> : ""}
          {messageList?.paymentEnable && messageList.paymentSource.toLowerCase() == "square" ? <iframe src={`${config().CHATBOT_PAYMENT_SERVICE}/square?payment_id=${messageList.paymentId}`} width="370" height="100%" scrolling="yes"></iframe> : ""}
          {messageList?.paymentEnable && messageList.paymentSource.toLowerCase() == "razorpay" ? <iframe src={`${config().CHATBOT_PAYMENT_SERVICE}/razorpay?payment_id=${messageList.paymentId}`} width="370" height="100%" scrolling="yes"></iframe> : ""}
          {messageList?.paymentEnable && messageList.paymentSource.toLowerCase() == "wix" ? <iframe src={`${config().CHATBOT_PAYMENT_SERVICE}/wixpayment?payment_id=${messageList.paymentId}`} width="370" height="100%" scrolling="yes"></iframe> : ""}
          {messageList?.paymentEnable && messageList.paymentSource.toLowerCase() == "worktualpurchase" ? <iframe src={`https://worktual.in/razorpay/?botamt=${messageList.paymentId}`} width="370" height="100%" scrolling="yes"></iframe> : ""}
          {messageList?.paymentEnable && messageList.paymentSource.toLowerCase() == "stripworktualpurchase" ? <iframe src={`https://worktual.in/stripe/?botamt=${messageList.paymentId}`} width="370" height="100%" scrolling="yes"></iframe> : ""}
          {
            (messageList?.purchasePlanData?.purchasePlanStatus && messageList?.purchasePlanData?.purchasePlanUrl !== '') &&
            <iframe src={messageList?.purchasePlanData?.purchasePlanUrl} width="100%" style={{ border: "0", height: "100%" }}></iframe>
          }
          {messageList?.worktualAppointment?.isShow &&
            <iframe src={messageList?.worktualAppointment?.url} width="100%" style={{ border: "0", height: "100%" }}></iframe>
          }
          {messageList?.integration?.isCalendlyShow &&
            <div
              className="chatMessage chatMessagefoot scroller"
              id="messages"
            //ref={scrollRef}
            >
              <Calendly />
            </div>
          }
          {messageList?.integration?.isMsGoogleShow && messageList?.integration?.url !== '' &&
            <div
              className="chatMessage chatMessagefoot scroller"
              id="messages"
            // ref={scrollRef}
            >
              <iframe src={messageList?.integration?.url} width='100%' height='700' style={{ border: 0 }} />
            </div>
          }
          {(!messageList?.paymentEnable && !messageList?.integration?.showIframe) && !builderList?.feedBackFormStatus && !messageList?.multipleTemp && !messageList?.purchasePlanData?.purchasePlanStatus && !messageList?.worktualAppointment?.isShow && !builderList?.commonPaymentForm ?
            <div
              className={
                test.length > 0 && (test[test.length - 1]?.agentText?.message?.is_pothys || test[test.length - 1]?.agentText?.message?.is_nickys)
                  ? "chatMessage chatMessagefoot scroller pthysscrollers"
                  : "chatMessage chatMessagefoot scroller"
              }
              id="messagesnew"
              //  ref={scrollRef}

              ref={containerRef}
            >


              {test.map((item, index) => {
                const {
                    agentText,
                    userText,
                    carouselText,
                    agentNameVisible,
                    timeOutVisible,
                    dropDownText,
                    broadCastMessage,
                } = item;

                const multiFiles = Array.isArray(agentText?.message?.file) ? agentText?.message?.file : [agentText?.message?.file];
                const isLastMessage = index === test.length - 1;
                const isCares = !!item.agentText?.message?.is_pothys || !!item.agentText?.message?.is_nickys;
                console.log("userText?.message", userText?.message, agentText)
                let userMsg = userText?.message && isJsonString(userText?.message?.message)
                    ? JSON.parse(userText?.message?.message)
                    : userText?.message?.message;

                let userMessageData = userMsg?.data || userMsg?.message || ''
                let msgData = agentText?.message && agentText?.message?.message;

                let agentMsg =
                  isJsonString(msgData) && JSON.parse(msgData)?.data !== undefined
                    ? JSON.parse(msgData)?.data
                    : msgData;


                  return (
                    <>
                    <div
                      key={item.id ?? index}
                      className={isCares && isLastMessage ? "cares" : "dsfsfd"}
                      ref={isCares && isLastMessage ? caresRef : null} // only cares ref on last cares
                      style={isCares && isLastMessage ? { minHeight: "390px" } : {}}
                    >


                      {agentText?.message && agentText?.message?.whisper !== true && (
                        <div className="chatleftview">
                          <div className="profilerounds">
                            {<div className="profileroundstop">

                              {!agentText?.message?.mergeMsg && !agentText?.message?.isB2C && <img src={messageList?.personalisation?.botIconLogo !== "" ? messageList?.personalisation?.botIconLogo : messageList?.iconLogo} alt="" />}
                              {agentText?.message?.isBot && !agentText?.message?.isB2C && !agentText?.message?.mergeMsg && <div className="topArea">
                                <span className="botTitle">{messageList?.personalisationAll?.title}</span>
                              </div>}
                              {!agentText?.message?.isBot && agentText?.message?.sender?.Name && <div className="topArea">
                                <span className="botTitle">{agentText?.message?.sender?.Name}</span>
                              </div>}
                            </div>}
                            <div className="botMsgArea">
                              {agentText?.message?.messageType === "Article" &&
                                <>
                                  <div className="chatmsgbox">
                                    <div className="leftagentchat">
                                      <div className="articlebox">
                                        <p>{(agentMsg?.message?.replace(/(<([^>]+)>)|&nbsp;/gi, " "))}</p>

                                        {agentMsg?.getInputData?.length > 0 && (
                                          <>
                                            {
                                              agentMsg?.getInputData?.map((item, index) => {
                                                return (
                                                  <>
                                                    <div className="articlemsgs"><p>{item?.title}</p>
                                                      <svg xmlns="http://www.w3.org/2000/svg" width="4" height="8" viewBox="0 0 4 8" fill="none" onClick={() => handleArticlePreview(item)}>
                                                        <path d="M0.56442 7.74581C0.436154 7.74581 0.307887 7.69701 0.210287 7.59868C0.0152875 7.40335 0.015954 7.08694 0.211621 6.89168L2.78782 4.32335C2.88255 4.22894 2.93462 4.10261 2.93462 3.96854C2.93462 3.83508 2.88255 3.70941 2.78782 3.61501L0.211354 1.04601C0.016021 0.850678 0.0154209 0.534278 0.210354 0.339012C0.405021 0.143078 0.721754 0.143078 0.917688 0.337678L3.49389 2.90668C3.77775 3.18988 3.93429 3.56681 3.93462 3.96854C3.93495 4.37028 3.77835 4.74721 3.49389 5.03168L0.917421 7.60001C0.820087 7.69701 0.69202 7.74581 0.56442 7.74581Z" fill="#757676" />
                                                      </svg>
                                                    </div>
                                                  </>
                                                );
                                              })
                                            }
                                          </>
                                        )}
                                      </div>
                                    </div>
                                    {/* <div className="sendmailagent">
                                        <span>{dateFormatter(agentText?.message?.sendAt)}</span>
                                      </div> */}
                                    {(agentText?.message?.isDropDown !== 1 && agentText?.message?.messageType !== 'Checkout' && agentText?.message?.messageType !== 'resendOtp' && agentText?.message?.messageType !== 'shoppingContinue' && agentText?.message?.likeDislikeStatus) &&
                                      <div className="likedislike">
                                        <img src={agentText?.message?.isLike === 1 ? newlikeactive : newlike} onClick={() => handleLikeChange(agentText, agentText?.message?.isLike === 1 ? 0 : 1)} />
                                        <img src={agentText?.message?.isLike === 2 ? newdislikeactive : newdislike} onClick={() => handleLikeChange(agentText, agentText?.message?.isLike === 2 ? 0 : 2)} />
                                        <img src={newspeaker} onClick={() => speakText(list, index)} />
                                      </div>}
                                  </div>
                                </>
                              }
                              
                              {agentText?.message?.messageType !== "file" && agentText?.message?.messageType !== "Article" &&
                                <>
                                  <div className="chatmsgbox" style={{ display: agentText?.message?.isB2C ? 'none' : '' }}>
                                    {!agentText?.message?.isCoupanStatus && !agentText?.message?.isB2C &&
                                      <div className="leftagentchat">
                                        <p>
                                          {(agentMsg !== "" && agentMsg !== undefined) ? (
                                            typeof agentMsg === 'string' ? <ReadMore text={agentMsg} /> : parse(urlify(agentMsg))
                                          ) : (
                                            msgTempBlock
                                          )}
                                        </p>
                                        {agentText?.message?.isDropDown === 1 && <DropdownComponent dropDownData={agentText?.message} index={index}></DropdownComponent>}
                                        {agentText?.message?.messageType === 'Checkout' && <div className="checkoutBtn worktualshopping">
                                          {builderList?.selectedCardItems?.length > 0 && builderList?.selectedCardItems?.map((list, storeIndex) => {
                                            if (list !== undefined && list !== null) {
                                              return (
                                                <div className="checkoutProperties worktualshoppingproperties">
                                                  <div className="shopleftworktual">

                                                    <div className="checkoutImg"><img src={list?.imageUrl} /></div>
                                                    <div className="checkoutTitle">
                                                      <p>{list?.imgTitle}</p>
                                                      <div className="worktualcounters">
                                                        <div className="worktualcountersnumber" onClick={() => { addQuantity('minus', storeIndex) }}>
                                                          <svg xmlns="http://www.w3.org/2000/svg" width="11" height="2" viewBox="0 0 11 2" fill="none">
                                                            <path d="M10.3434 1.19434H0.656494C0.380427 1.19434 0.156494 0.970403 0.156494 0.694336C0.156494 0.418269 0.380427 0.194336 0.656494 0.194336H10.3434C10.6194 0.194336 10.8434 0.418269 10.8434 0.694336C10.8434 0.970403 10.6194 1.19434 10.3434 1.19434Z" fill="black" />
                                                          </svg>
                                                        </div>
                                                        <input type="number" value={list?.quantityCount} />
                                                        <div className="worktualcountersnumber" onClick={() => { addQuantity('plus', storeIndex) }}>
                                                          <svg xmlns="http://www.w3.org/2000/svg" width="11" height="12" viewBox="0 0 11 12" fill="none">
                                                            <path d="M10.3343 5.19123L5.99643 5.19423V0.85966C5.99643 0.583639 5.77266 0.359863 5.49664 0.359863C5.22062 0.359863 4.99685 0.583639 4.99685 0.85966V5.19496L0.664986 5.19796C0.388968 5.19816 0.165328 5.42207 0.165527 5.69809C0.165727 5.97411 0.389634 6.19776 0.665653 6.19756L4.99685 6.19456V10.5291C4.99685 10.8052 5.22062 11.0289 5.49664 11.0289C5.77266 11.0289 5.99643 10.8052 5.99643 10.5291V6.19389L10.335 6.19089C10.611 6.19069 10.8346 5.96678 10.8344 5.69076C10.8342 5.41474 10.6103 5.19103 10.3343 5.19123Z" fill="black" />
                                                          </svg>
                                                        </div>
                                                      </div>
                                                    </div>
                                                  </div>
                                                  <div className="worktualshoppricelists">
                                                    {/* <p className="worktualshopprice">£ {list?.price}</p>*/}
                                                    <p className="worktualshopprice">{list?.pro_currency_type ?? '£'} {list?.pro_price ?? list?.price}</p>
                                                    {/* <p className="worktualshoppricediscount">{list?.price}</p> */}
                                                  </div>
                                                </div>
                                              )
                                            }
                                          })}
                                          <div className="btns">
                                            <div className="btnsh5" onClick={shoppingContinue}>Continue shopping</div>
                                            <div className="btnsh5" onClick={shoppingCheckout}>Proceed to checkout</div>
                                          </div>
                                        </div>}
                                        {agentText?.message?.messageType === 'resendOtp' && <div className="checkoutBtn worktualshopping">
                                          <div className="btns">
                                            <div className="btnsh5" onClick={handleResendOtp}>Resend Otp</div>
                                          </div>
                                        </div>}
                                        {agentText?.message?.messageType === 'WORKTUAL_CANCEL_RESCHEDULE' && <div className="checkoutBtn worktualshopping">
                                          {agentText?.message?.appointmentData?.length > 0 && agentText?.message?.appointmentData?.map((list, index) => {
                                            if (list !== undefined && list !== null) {
                                              return (
                                                <div className="checkoutBtn reswork" onClick={() => worktualAppointmentCancelReschedule(list, agentText?.message?.module)}>
                                                  <div className="checkoutBtnh5">
                                                    {list?.appBookID?.service_title}
                                                    <span>Appointment Id: #{list?.appBookID?.user_app_id}</span>
                                                  </div>

                                                </div>
                                              )
                                            }
                                          })}
                                        </div>}
                                        {
                                          agentText?.message?.messageType === 'addressLine' && <div className="checkoutBtn">
                                            <div className="checkoutBtn" onClick={() => { handleAddressYes(agentText?.message?.customerAddress) }}>Yes</div>
                                            <div className="checkoutBtn" onClick={() => { handleAddressNo() }}>No</div>
                                          </div>
                                        }
                                        {
                                          agentText?.message?.messageType === 'paymentMode' && <div className="checkoutBtn">
                                            <div className="checkoutBtn" onClick={() => { handlePaymentMode('Cash on delivery') }}>Cash on delivery</div>
                                            <div className="checkoutBtn" onClick={() => { handlePaymentMode('Online') }}>Online payment</div>
                                          </div>
                                        }
                                        {
                                          agentText?.message?.messageType === 'shoppingContinue' && <div className="checkoutBtn">
                                            <div className="checkoutBtn" onClick={shoppingContinue}>Continue shopping</div>
                                          </div>
                                        }

                                        {agentText?.message?.paymentSelector === 1 && <div className="checkoutBtn">
                                          {agentText?.message?.paymentMethod?.map((list) => {
                                            return (<div className="checkoutBtn" onClick={() => { setPaymentMethodForFlow(list) }}>{list}</div>)
                                          })}
                                        </div>}
                                      </div>}
                                    {agentText?.message?.isCoupanStatus && !agentText?.message?.isB2C && <div className="leftagentchat worktualcouponmain">
                                      <p>{(agentMsg !== "" && agentMsg !== undefined) ? parse(urlify(agentMsg)) : (msgTempBlock)}</p>
                                      <div className="worktualcoupons">
                                        <p>{agentText?.message?.couponId}</p>
                                        <svg xmlns="http://www.w3.org/2000/svg" width="18" height="18" viewBox="0 0 18 18" fill="none" onClick={() => { copyCoupon(agentText?.message?.couponId) }}>
                                          <path d="M16.669 1.44233C15.5308 0.362234 13.959 0.179635 12.4312 0.143535C11.1871 0.111335 10.3062 0.111335 9.06597 0.143535C7.53717 0.179635 5.96537 0.361335 4.82717 1.44233C4.35597 1.88913 4.06827 2.43063 3.88657 2.99943C2.84447 3.19673 2.01987 3.57583 1.39107 4.17273C0.140572 5.35923 0.104972 7.18643 0.0790718 8.52133L0.0761719 11.9334L0.0790718 12.0809C0.104972 13.4149 0.140572 15.242 1.39107 16.4383C2.57857 17.5653 4.22357 17.7557 5.82267 17.7938C6.47647 17.8104 7.03457 17.8182 7.59217 17.8182C8.14977 17.8182 8.70837 17.8104 9.36027 17.7938C10.9609 17.7557 12.6059 17.5653 13.7934 16.4383C14.4994 15.7689 14.8041 14.9002 14.9481 14.1204C15.5778 13.929 16.1668 13.6338 16.6689 13.157C17.8652 12.0232 17.9008 10.2986 17.9243 9.14243V5.44223C17.9009 4.30163 17.8653 2.57713 16.669 1.44233ZM16.4119 5.03173C15.6554 5.02323 15.1092 4.99124 14.6787 4.86714C13.4585 4.53024 12.5376 3.40713 12.439 2.13083C12.4256 1.96923 12.4333 1.80733 12.4357 1.64573C13.8345 1.68113 14.9171 1.84693 15.6367 2.53023C16.2375 3.10053 16.3718 4.02703 16.4119 5.03173ZM12.7613 15.3505C11.9874 16.0849 10.8282 16.2587 9.32377 16.2939C8.04447 16.3271 7.14257 16.3271 5.85937 16.2939C4.35647 16.2587 3.19727 16.0839 2.42577 15.3525C1.62787 14.5898 1.60207 13.2412 1.57907 12.0527L1.57617 8.70023L1.57907 8.55083C1.60197 7.36233 1.62787 6.01564 2.42327 5.26084C2.73227 4.96783 3.12217 4.75743 3.60487 4.60723C3.58137 4.95663 3.57427 5.29424 3.56827 5.60354L3.56537 8.84863L3.56827 8.98923C3.59267 10.2636 3.62637 12.0097 4.82657 13.1572C5.96477 14.2373 7.53657 14.4199 9.06437 14.456C9.68647 14.4716 10.2172 14.4794 10.748 14.4794C11.2783 14.4794 11.8095 14.4716 12.4296 14.456C12.729 14.4489 13.0296 14.4331 13.3288 14.4091C13.1957 14.8029 13.012 15.1127 12.7613 15.3505ZM15.6368 12.0693C14.9093 12.7597 13.8145 12.9228 12.3931 12.956C11.1763 12.9863 10.3199 12.9863 9.10107 12.956C7.68117 12.9228 6.58647 12.7597 5.86137 12.0712C5.11427 11.3564 5.08987 10.0839 5.06787 8.96083L5.06547 5.77533L5.06787 5.63173C5.08987 4.50963 5.11427 3.23723 5.85987 2.53013C6.58647 1.83973 7.68117 1.67663 9.10257 1.64343C9.71097 1.62783 10.2291 1.62003 10.7481 1.62003C10.8004 1.62003 10.8549 1.62103 10.9073 1.62123C10.9046 1.83263 10.8967 2.04503 10.9141 2.25283C11.063 4.18643 12.4126 5.82903 14.2632 6.33973C14.8599 6.51183 15.5245 6.55113 16.4243 6.56043V9.12683C16.4004 10.3065 16.3219 11.4199 15.6368 12.0693Z" fill="#757676" />
                                        </svg>
                                      </div>
                                    </div>}
                                    {(agentText?.message?.isB2C && agentText?.message?.budgetData) &&
                                      <BudgetTable budgetData={agentText?.message?.budgetData} explanation={agentText?.message?.explanation} />
                                    }
                                    {/* <div className="sendmailagent">
                                        <span>{dateFormatter(agentText?.message?.sendAt)}</span>
                                      </div> */}
                                    {/* {(agentText?.message?.isDropDown !== 1 && agentText?.message?.messageType !== 'Checkout' && agentText?.message?.messageType !== 'resendOtp' && agentText?.message?.messageType !== 'shoppingContinue' && agentText?.message?.likeDislikeStatus) &&
                                      <div className="likedislike">
                                        <img src={agentText?.message?.isLike === 1 ? newlikeactive : newlike} onClick={() => handleLikeChange(agentText, agentText?.message?.isLike === 1 ? 0 : 1)} />
                                        <img src={agentText?.message?.isLike === 2 ? newdislikeactive : newdislike} onClick={() => handleLikeChange(agentText, agentText?.message?.isLike === 2 ? 0 : 2)} />
                                        <img src={newspeaker} />
                                      </div>
                                      } */}
                                  </div>

                                </>
                              }
                              {agentText?.message?.messageType === "file" && agentText?.message?.messageType !== 'customButton' && <div className="chatmsgbox">
                                <div className="leftagentchat"  onClick={() => window.open(agentMsg?.url, "_blank")} >
                                  <p className="forfiles">
                                  {imageFormat.indexOf(agentMsg?.mimetype) > -1 ?
                                    <img src={agentMsg?.url} alt="Uploaded file" className="imagenotforfiles"  />
                                    : 
                                      (
                                        <>
                                          {
                                            pdfFormats.indexOf(agentMsg?.mimetype) > -1 ? <img src={Pdficon} alt="" className="lftFiledoc" /> :
                                              excelFormats.indexOf(agentMsg?.mimetype) > -1 ? <img src={grid_xls} alt="" className="lftFiledoc" /> :
                                                pptFormats.indexOf(agentMsg?.mimetype) > -1 ? <img src={grid_ppt} alt="" className="lftFiledoc" /> :
                                                  wordFormats.indexOf(agentMsg?.mimetype) > -1 ? <img src={grid_word} alt="" className="lftFiledoc" /> :
                                                    agentMsg?.mimetype === "application/zip" ? <img src={zip} alt="" className="lftFiledoc" /> :
                                                      <img src={file?.filepath} alt="" className="lftFiledoc" />}
                                          <div
                                            key={index}
                                            className="rightChat"
                                            ref={scrollUser}
                                            
                                          >

                                            <span className="fileTitle">{agentMsg?.name}</span>
                                          </div>
                                        </>
                                      )}
                                </p></div>
                              </div>}

                              {agentText?.message?.messageTypeNlp === "file" && agentText?.message?.messageType !== 'customButton' && <div className="chatmsgbox">
                                <div className="leftagentchat" onClick={() => window.open(agentText?.message?.url, "_blank")}>
                                  <p className="forfiles">
                                  {imageFormat.indexOf(agentText?.message?.mimetype) > -1 ?
                                    <img src={agentText?.message?.url} alt="Uploaded file" className="imagenotforfiles" />
                                    : 
                                      (
                                        <>
                                          {
                                            pdfFormats.indexOf(agentText?.message?.mimetype) > -1 ? <img src={Pdficon} alt="" className="lftFiledoc" /> :
                                              excelFormats.indexOf(agentText?.message?.mimetype) > -1 ? <img src={grid_xls} alt="" className="lftFiledoc" /> :
                                                pptFormats.indexOf(agentText?.message?.mimetype) > -1 ? <img src={grid_ppt} alt="" className="lftFiledoc" /> :
                                                  wordFormats.indexOf(agentText?.message?.mimetype) > -1 ? <img src={grid_word} alt="" className="lftFiledoc" /> :
                                                    agentText?.message?.mimetype === "application/zip" ? <img src={zip} alt="" className="lftFiledoc" /> :
                                                      <img src={file?.filepath} alt="" className="lftFiledoc" />}
                                          <div
                                            key={index}
                                            className="rightChat"
                                            ref={scrollUser}
                                          >

                                            <span className="fileTitle">{agentText?.message?.name}</span>
                                          </div>
                                        </>
                                      )}
                                </p></div>

                              </div>}
                              {agentText?.message?.messageType === 'customButton' &&
                                <ButtonComponent data={agentText} handleTempInput={handleTempInput} />
                              }
                              {(agentText?.message?.isDropDown !== 1 && agentText?.message?.messageType !== 'Checkout' && agentText?.message?.messageType !== 'resendOtp' && agentText?.message?.messageType !== 'shoppingContinue' && agentText?.message?.likeDislikeStatus 
                              && `${messageList.personalisationAll?.domainId}` == '11165') && 
                              <div className="likedislike">
                                <img src={agentText?.message?.isLike === 1 ? newlikeactive : newlike} onClick={() => handleLikeChange(agentText, agentText?.message?.isLike === 1 ? 0 : 1)} />
                                <img src={agentText?.message?.isLike === 2 ? newdislikeactive : newdislike} onClick={() => handleLikeChange(agentText, agentText?.message?.isLike === 2 ? 0 : 2)} />
                                <img src={(speakingStatus === index) ? newspeakerstop : newspeaker} onClick={() => speakText(agentText, index)} />
                              </div>}
                            </div>

                            {multiFiles?.map((item, index) => {

                              if(!item?.mimetype){
                                return null
                              }

                              return (
                                <div className="botMsgArea">
                                  <div className="chatmsgbox multifiles">
                                    <div className="leftagentchat">
                                      {imageFormat.indexOf(item?.mimetype) > -1
                                        ? (
                                          <div
                                            className="forfilesimg"
                                            onClick={() => window.open(item?.url, "_blank")}>
                                            <img
                                              src={item?.url}
                                              alt="Uploaded item"
                                              className="image"
                                            />
                                          </div>
                                        ) : (
                                          <p
                                            className="forfilesdoc"
                                            onClick={() => window.open(item?.url, "_blank")}>
                                            {
                                              pdfFormats.indexOf(item?.mimetype) > -1 ? <img src={Pdficon} alt="" className="lftFiledoc" /> :
                                                excelFormats.indexOf(item?.mimetype) > -1 ? <img src={grid_xls} alt="" className="lftFiledoc" /> :
                                                  pptFormats.indexOf(item?.mimetype) > -1 ? <img src={grid_ppt} alt="" className="lftFiledoc" /> :
                                                    wordFormats.indexOf(item?.mimetype) > -1 ? <img src={grid_word} alt="" className="lftFiledoc" /> :
                                                      item?.mimetype === "application/zip" ? <img src={zip} alt="" className="lftFiledoc" /> :
                                                        <img src={item?.filepath} alt="" className="lftFiledoc" />}
                                            <div
                                              key={index}
                                              className="rightChat"
                                            >
                                              <span className="fileTitle">{item?.filename}</span>
                                            </div>
                                          </p>
                                        )}
                                    </div>
                                  </div>
                              </div>)
                            })}
                             
                          </div>

                          {agentText?.message?.is_pothys &&
                            <div className='chatleftshopingcarosel'>
                              {(agentText?.message?.is_pothys
                                && agentText?.message?.pothys_product_data?.products?.length > 0)
                                && <CarosuelShoping
                                  messageId={carouselText?.message?.messageID}
                                  carosuelData={agentText?.message?.pothys_product_data?.products || []}
                                  title="Results"
                                />}

                              {(agentText?.message?.is_pothys
                                && agentText?.message?.pothys_product_data?.recommended_products?.length > 0)
                                && <CarosuelShoping
                                  messageId={carouselText?.message?.messageID}
                                  carosuelData={agentText?.message?.pothys_product_data?.recommended_products || []}
                                  title="You might also like"
                                />}
                            </div>
                          }

                          {agentText?.message?.is_nickys &&
                            <div className='chatleftshopingcarosel'>
                              {(agentText?.message?.nickys_product_data?.products?.length > 0)
                                && <CarosuelShoping
                                  messageId={carouselText?.message?.messageID}
                                  carosuelData={agentText?.message?.nickys_product_data?.products || []}
                                  title="Results"
                                />}

                              {(agentText?.message?.nickys_product_data?.recommended_products?.length > 0)
                                && <CarosuelShoping
                                  messageId={carouselText?.message?.messageID}
                                  carosuelData={agentText?.message?.nickys_product_data?.recommended_products || []}
                                  title="You might also like"
                                />}
                            </div>
                          }

                        </div>
                      )}

                      {userText?.message && (

                        <div className="chatrightview">
                          <ul>
                            <div className="right">
                              <li>
                                <div className={`${imageFormat.indexOf(userText?.message?.message?.mimetype) > -1 ? "" : "pdfimg"}`}>
                                  {userText?.message?.messageType === "file" && imageFormat.indexOf(userText?.message?.message?.mimetype) > -1 ?
                                    <div
                                      key={index}
                                      className="rightChat"
                                      ref={scrollUser}
                                    >
                                      {/* <div className="sendmail">
                                          <span>
                                            {dateFormatter(
                                              userText?.message?.sendAt
                                            )}
                                          </span>
                                        </div> */}
                                      <span className="fileTitle">
                                        <a
                                          style={{ textDecoration: "none" }}
                                          href={userText?.message?.message?.url ?? ""}
                                          target="_blank"
                                        > <img src={userText?.message?.message?.url} alt="Uploaded file" className="imgFiledoc" /></a>
                                      </span>
                                    </div>
                                    : userText?.message?.messageType === "file" ?
                                      (
                                        <div
                                          key={index}
                                          className="rightChat"
                                          ref={scrollUser}
                                        >
                                          {/* <div className="sendmail">
                                              <span>
                                                {dateFormatter(
                                                  userText?.message?.sendAt
                                                )}
                                              </span>
                                            </div> */}
                                          <span className="fileTitle">
                                            <a
                                              style={{ textDecoration: "none" }}
                                              href={userText?.message?.message?.url ?? ""}
                                              target="_blank"
                                            >

                                              {
                                                pdfFormats.indexOf(userText?.message?.message?.mimetype) > -1 ? <img src={Pdficon} alt="" className="pdfFiledoc" /> :
                                                  excelFormats.indexOf(userText?.message?.message?.mimetype) > -1 ? <img src={grid_xls} alt="" className="pdfFiledoc" /> :
                                                    pptFormats.indexOf(userText?.message?.message?.mimetype) > -1 ? <img src={grid_ppt} alt="" className="pdfFiledoc" /> :
                                                      wordFormats.indexOf(userText?.message?.message?.mimetype) > -1 ? <img src={grid_word} alt="" className="pdfFiledoc" /> :
                                                        <img src={userText?.message?.message?.url} alt="" className="pdfFiledoc" />}
                                              <div
                                                key={index}
                                                className="rightChat"
                                                ref={scrollUser}
                                              >
                                                <span className="fileTitle">{userText?.message?.message?.filename}</span>
                                              </div>
                                            </a>

                                          </span>
                                        </div>


                                      ) : ( "")}
                                    {!!userMessageData && (
                                      <div 
                                        key={index} className="rightChat" ref={scrollUser} 
                                        style={userText?.message?.messageType === "file"  ? { marginTop: '24px' } : {}}>
                                        {/* <div className="sendmail">
                                                <span>
                                                  {dateFormatter(
                                                    userText?.message?.sendAt
                                                  )}
                                                </span>
                                              </div> */}
                                        <span className="fileTitle">
                                          {/* {parse(urlify(userMsg?.data))} */}
                                          {parse(urlify(userMessageData?.charAt(0).toUpperCase() + userMessageData?.slice(1)))}
                                          {/* {parse(urlify(userText?.message?.message))} */}
                                        </span>
                                    </div>)}
                                </div>
                              </li>
                            </div>
                          </ul>
                        </div>
                      )}
                      {carouselText?.message?.messageType === "carousel" && <div className="chatmsgbox">
                        <div className="leftagentchat"><p className="forfiles">
                          <CarouselForm carouselData={carouselText?.message?.carouselDataArr}></CarouselForm>
                        </p></div>
                      </div>}
                      {(agentText?.message?.isB2C && agentText?.message?.b2cSimData) &&
                        <SIMDeals b2cSimData={agentText?.message?.b2cSimData} />
                      }
                      {(agentText?.message?.isB2C && agentText?.message?.b2cProductData) &&
                        <B2ccarousel b2cProductData={agentText?.message?.b2cProductData} />
                      }

                      {agentNameVisible?.message && (
                        <div className="chatconnecter" ref={scrollUser}>
                          <span>
                            Agent{" "}
                            {<b>{agentNameVisible.message.agentName}</b> ??
                              "Agent"}{" "}
                            will be assisting you now
                          </span>
                        </div>
                      )}

                      {timeOutVisible?.message && <div className="chatconnecter">{timeOutVisible.message.TimeOutMessage}</div>}
                    </div>
                    </>
                  );
                }
              )}
              {messageList?.particialOutput && messageList?.particialOutput?.length > 0 &&
                <>
                  <div className="chatleftview" id="chatleftviewwere">
                    <div className="profilerounds">
                      <div className="profileroundstop">
                        <img src={messageList?.personalisation?.botIconLogo !== "" ? messageList?.personalisation?.botIconLogo : messageList?.iconLogo} alt="" />
                        <div class="topArea"><span className="botTitle">{messageList?.personalisationAll?.title}</span></div>
                      </div>
                      <div class="leftmessage">
                        <div class="botMsgArea">
                          <div class="chatmsgbox">
                            <div class="leftagentchat">
                              <p>{messageList?.particialOutput?.[0]?.bot_respones ? parse(messageList?.particialOutput[0]?.bot_respones) : parse(messageList?.particialOutput[0]?.que_answer)}</p>
                            </div>

                            {/* <div className="likedislike">
                              <img src={like} /><img src={dislike} />
                            </div> */}
                          </div>
                        </div>
                      </div>
                    </div>
                    {/* <ul className="profilerounds">
                      <div className="left">
                        <li className="lifirsts">
                          <img src={messageList?.personalisation?.botIconLogo !== "" ? messageList?.personalisation?.botIconLogo : messageList?.iconLogo} alt="" />
                        </li>
                        <li>
                          <div class="leftmessage"><div class="botMsgArea"><div class="topArea"> <span className="botTitle">{messageList?.personalisationAll?.title}</span><span class="likebefore"></span><span class="dislikebefore"></span></div>
                            <div class="chatmsgbox"><div class="leftagentchat">
                              <p>
                                {[56, 55, 33].includes(messageList.personalisationAll?.plan_id)
                                  ?
                                  parse(messageList?.particialOutput[0]?.que_answer)
                                  :
                                  parse(messageList?.particialOutput[0]?.bot_respones)
                                }
                              </p>
                            </div><div class="sendmailagent">  <span>{dateFormatter(new Date())}</span></div></div></div></div>
                        </li>
                      </div>
                    </ul> */}
                    <div ref={bottomRef1} id="bottomref1" />
                  </div>
                </>
              }
       
              {messageList.connectionType.isDTMFConnection && (
                <div className="messagedialogBtn">
                  {dtmfBlock
                    ? messageList?.currentDtmfData?.map((item, index) => {
                      return item?.children?.map((data, i) => {
                        if (data.type !== "error") {
                          return (
                            <div
                              className="dialogBtn green"
                              onClick={() => handleDtmfInput(data)}
                            >
                              {data.type}
                            </div>
                          );
                        }
                      });
                    })
                    : ""}
                </div>
              )}
              {stillEnd && <div className="messagedialogBtn">
                <div
                  className="dialogBtn green"
                  onClick={() => handleCloseSession('Yes')}
                >
                  Yes
                </div>
                <div
                  className="dialogBtn green"
                  onClick={() => handleCloseSession('No')}
                >
                  No
                </div>
              </div>}

              {messageList.callBackButtonStatus === 1 ? (
                <div className="messagedialogBtn">
                  {
                    <>{messageList?.callbackBtnStatus && <div className="dialogBtn green" onClick={() => callBackClick()}>
                      Callback</div>}
                      <div className="dialogBtn green" onClick={() => continueChatClick()}>
                        Continue Chat
                      </div></>
                  }
                </div>
              ) : ''}
              {messageList?.templateData?.length > 0 && sessionStorage.isConnected !== "Agent" && messageList?.templateKeywords?.length > 0 && messageList?.particialOutput?.length === 0 && !stillEnd && (
                <div className="messagedialogBtn">
                  {
                    messageList?.templateKeywords?.map((item, index) => {
                      return (
                        <>
                          {item?.type !== undefined && <div
                            className="dialogBtn green"
                            onClick={() => handleTempInput(item, '', [])}
                          >
                            {item.type}
                          </div>}
                        </>
                      );
                    })
                  }
                </div>
              )}

              {messageList?.paymentMethod.length > 0 && messageList?.paymentForm && sessionStorage.isConnected == "bot" && messageList?.templateKeywords && !stillEnd && (
                <div className="messagedialogBtn">
                  {
                    messageList?.paymentMethod?.map((item, index) => {
                      return (
                        <>
                          {item.paymentType !== undefined && <div
                            className="dialogBtn green"
                            onClick={() => handlePayment(item.paymentType)}
                          >
                            {item?.paymentType}
                          </div>}
                        </>
                      );
                    })
                  }
                </div>
              )}

              {<a href={builderList?.checkoutUrl} target="_blank" ref={anchorRef} />}
              <div className="messagedialogBtn">
                {messageList.callBackStatus && messageList?.dynamicChatFormList?.length > 0 && <CallBackRequest />}
                {builderList.showCustomerForm && <CallBackRequest type={builderList?.connectionName} />}
                {builderList?.showUpdateAddresss && <CallBackRequest type={`${builderList?.connectionName}_update`} />}
                {builderList.showAddress &&
                  <div className="addressBlock">
                    <p>Choose the address</p>
                    <div className="checkoutBtn">
                      {builderList?.addressList[0]?.info?.addresses?.items?.map((address, index) => {
                        return <label>
                          <input
                            type="radio"
                            name={`address${index}`}
                            value={address.address?.addressLine}
                            checked={address.address?.addressLine === builderList?.selectedAddress}
                            onChange={(e) => dispatch(chatBuilderActions.setSelectedAddress({ selectedAddressValue: e.target.value, indexValue: index }))}
                          />{' '}
                          {address.address?.addressLine + ' ' + address.address?.addressLine2 + ' ' + address.address?.city + ' ' + address.address?.postalCode}
                        </label>
                      })}

                    </div>
                    <label className="addnewaddresses">
                      <input
                        type="radio"
                        name={`address${builderList?.addressList?.length + 1}`}
                        value={'addNewAddress'}
                        checked={builderList?.selectedAddress === 'addNewAddress'}
                        onChange={(e) => dispatch(chatBuilderActions.setSelectedAddress({ selectedAddressValue: e.target.value }))}
                      />{'  '}
                      {'Add new address'}
                    </label>
                    <div className="addressBlock" onClick={handleShopifyNewAddress}>Confirm</div>
                  </div>}
                {messageList?.sessionClosedStatus && <div className="agentconnect">
                  This conversation has been ended. To start a new conversation, <a href="#" onClick={startNewConv}>Click here</a>
                </div>}
                {builderList.showBigCommercePayment &&
                  <div className="addressBlock">
                    <p>Choose your payment method</p>
                    {builderList?.bigCommercePaymentType?.map((paymentList, index) => {
                      return <div><label>
                        <input
                          type="radio"
                          name={`payment${index}`}
                          value={paymentList?.name}
                          checked={paymentList?.name === builderList?.selectedPayment}
                          onChange={(e) => dispatch(chatBuilderActions.setSelectedPayment(e.target.value))}
                        />{' '}
                        {paymentList?.name}
                      </label>
                      </div>
                    })}

                    <button onClick={handleBigCommercePayment}>Confirm</button>
                  </div>}

                {botStatus && messageList.connectionType.isBotConnection && (
                  <div className="agentconnect">
                    Connecting to Lukas bot. Please wait!
                  </div>
                )}
              </div>

              {messageList.agentConnectingStatus && messageList.connectionType.isAgentConnection && (
                <motion.div
                  initial={{ opacity: 0, scale: 0.5 }}
                  animate={{ opacity: 1, scale: 1 }}
                  transition={{ duration: 0.5 }}
                >
                  <div className="agentconnectqueue">
                    {countDown >= 0 && messageList.customerBlockStatus !== 1
                      ?
                      <>
                        <p>Connecting to a live agent...</p>
                        {queuedContent ? <p><svg width="45" height="30" viewBox="0 0 16 17" fill="none" xmlns="http://www.w3.org/2000/svg">
                          <path d="M4.07347 3.4251C4.24838 3.21141 4.21693 2.89639 4.00324 2.72149C3.78955 2.54659 3.47453 2.57803 3.29963 2.79173L1.6463 4.81173C1.47139 5.02542 1.50284 5.34043 1.71653 5.51534C1.93022 5.69024 2.24524 5.65879 2.42014 5.4451L4.07347 3.4251Z" fill="#808080" />
                          <path d="M12.5842 2.77316C12.399 2.56829 12.0828 2.55231 11.878 2.73746C11.6731 2.92262 11.6571 3.2388 11.8423 3.44367L13.5956 5.38367C13.7808 5.58854 14.0969 5.60452 14.3018 5.41936C14.5067 5.23421 14.5227 4.91803 14.3375 4.71316L12.5842 2.77316Z" fill="#808080" />
                          <path d="M8.53322 5.49508C8.53322 5.21894 8.30936 4.99508 8.03322 4.99508C7.75708 4.99508 7.53322 5.21894 7.53322 5.49508V8.54841C7.53322 8.7056 7.41041 8.82841 7.25322 8.82841H4.82655C4.55041 8.82841 4.32655 9.05227 4.32655 9.32841C4.32655 9.60456 4.55041 9.82841 4.82655 9.82841H7.25322C7.96269 9.82841 8.53322 9.25789 8.53322 8.54841V5.49508Z" fill="#808080" />
                          <path fillRule="evenodd" clipRule="evenodd" d="M8.03322 3.32841C4.71988 3.32841 2.03322 6.01508 2.03322 9.32841C2.03322 12.6417 4.71988 15.3284 8.03322 15.3284C11.3466 15.3284 14.0332 12.6417 14.0332 9.32841C14.0332 6.01508 11.3466 3.32841 8.03322 3.32841ZM8.03322 4.32841C10.7932 4.32841 13.0332 6.56841 13.0332 9.32841C13.0332 12.0884 10.7932 14.3284 8.03322 14.3284C5.27322 14.3284 3.03322 12.0884 3.03322 9.32841C3.03322 6.56841 5.27322 4.32841 8.03322 4.32841Z" fill="#808080" />
                        </svg>
                          {queuedContent}</p> : ''}
                      </>
                      : null}
                  </div>
                </motion.div>
              )}

              {messageList?.integration?.isMsGoogleShow && messageList?.integration?.url === '' &&
                <>
                  <BookingCalendar />
                </>

              }
              {messageList?.datetimePickerNode?.length > 0 && <DateTimePicker />}

              {builderList?.feedBackFormStatus && <FeedBackForm />}

              {/* <SIMDeals /> */}

              {(messageList?.lottieProcess && messageList?.customerBlockStatus !== 1) &&
                <>
                  <div className="chatleftview">
                    <div className="profilerounds">
                      <div className="profileroundstop">
                        <img src={messageList?.personalisation?.botIconLogo !== "" ? messageList?.personalisation?.botIconLogo : messageList?.iconLogo} alt="" />
                        <p className="lottiebot">
                          Typing  <Lottie
                            style={{ width: 30, margin: "0 0 0 8px" }}
                            className="lottie"
                            animationData={groovyWalkAnimation}
                          />
                        </p>
                      </div>
                    </div>
                  </div>
                </>
              }


              {liveTrackerList?.agentTypingStatus &&
                <>
                  <div className="chatleftview">
                    <div className="profilerounds">
                      <div className="profileroundstop">
                        <img src={messageList?.personalisation?.botIconLogo !== "" ? messageList?.personalisation?.botIconLogo : messageList?.iconLogo} alt="" />
                        <div class="topArea"><span className="botTitle">{liveTrackerList?.agentTypingName}</span></div>
                      </div>
                      <p className="lottiebot">
                        Typing  <Lottie
                          style={{ width: 30, margin: "0 0 0 8px" }}
                          className="lottie"
                          animationData={groovyWalkAnimation}
                        />
                      </p>
                    </div>
                  </div>
                </>
              }




              {messageList?.fileInputtype && <div className="chatleftview" style={{ margin: "0" }}>

                <div className="profilerounds">
                  <div className="leftmessage">
                    <div className="botMsgArea">
                      <div className="worktualattachs">
                        <input type="file" id="file" onChange={handleFileChange} />
                        <label for="file">
                          <svg width="32" height="25" viewBox="0 0 32 25" fill="none" xmlns="http://www.w3.org/2000/svg">
                            <path d="M8.15449 23.6553C4.45487 23.6553 1.26783 21.1583 0.576434 17.7182C-0.0877948 14.4195 1.6155 10.989 4.62591 9.5613L4.86017 9.45013L4.87238 9.1912C4.9597 7.34986 5.67301 5.57282 6.88119 4.1875C8.43145 2.41574 10.6748 1.39941 13.0357 1.39941C15.536 1.39941 17.9388 2.59288 19.4633 4.59198L19.6634 4.85451L19.9692 4.7306C20.5819 4.48248 21.2358 4.3569 21.9128 4.3569C22.3503 4.3569 22.7908 4.41041 23.2222 4.51629C25.1778 4.99367 26.7183 6.69781 27.0568 8.75678C27.1422 9.2741 27.1467 9.76589 27.0702 10.2602L27.0652 10.2929V10.3034L27.0078 10.6347L27.3313 10.7736C27.7208 10.9292 28.0837 11.1129 28.4106 11.3195C30.3875 12.5462 31.566 14.657 31.566 16.9678C31.566 20.6552 28.5633 23.6553 24.8726 23.6553H8.15449Z" fill="#ddd" />
                            <path d="M13.0373 1.83314C15.4038 1.83314 17.6782 2.96299 19.1213 4.85549L19.5215 5.38013L20.133 5.13242C20.6937 4.90551 21.2932 4.79045 21.9148 4.79031C22.3175 4.79031 22.7233 4.83979 23.1219 4.93753C24.9107 5.37445 26.3212 6.93802 26.6319 8.82885C26.7098 9.3004 26.7141 9.74743 26.6449 10.1956L26.6365 10.2502L26.5025 10.9081L27.1725 11.176C27.5368 11.3217 27.8762 11.4934 28.1874 11.69C30.0335 12.8354 31.1356 14.8088 31.1356 16.969C31.1356 20.4182 28.3268 23.2245 24.8743 23.2245H8.15636C4.66188 23.2245 1.65312 20.8734 1.00203 17.6338C0.376324 14.5274 1.97884 11.2971 4.81282 9.95288L5.2812 9.73082L5.30572 9.21297C5.38862 7.46809 6.06437 5.78475 7.20765 4.47415C8.67626 2.79581 10.8012 1.83314 13.0373 1.83314ZM13.0374 0.96875C10.5507 0.96875 8.19097 2.03773 6.55714 3.90488C5.27788 5.37142 4.53447 7.22997 4.44229 9.17195C1.24097 10.6905 -0.556998 14.2721 0.154668 17.8046C0.886547 21.4468 4.25212 24.0889 8.15633 24.0889H24.8742C28.8017 24.0889 32 20.8964 32 16.969C32 14.4853 30.7005 12.232 28.6432 10.9555C28.2829 10.7279 27.8969 10.5348 27.4935 10.3734C27.4964 10.3592 27.4992 10.3419 27.4992 10.3274C27.5828 9.78573 27.5771 9.24692 27.4848 8.68804C27.1188 6.4605 25.4505 4.61652 23.327 4.09784C22.8575 3.98278 22.3833 3.92595 21.9145 3.92595C21.1864 3.92595 20.4713 4.06303 19.8087 4.33125C18.198 2.21919 15.6913 0.96875 13.0374 0.96875Z" fill="#000" />
                            <path opacity="0.25" fillRule="evenodd" clipRule="evenodd" d="M31.1278 16.9364C31.05 20.5235 28.6746 22.6689 25.6278 23.2395L15.4179 23.2394C15.4066 23.0455 15.4043 22.8514 15.4043 22.6575C15.4043 20.7812 15.7853 19.007 16.4657 17.432C16.4748 17.4091 16.4861 17.3887 16.4951 17.3657C17.5044 15.0606 19.1553 13.1843 21.1783 12.0458C21.1828 12.0407 21.1873 12.0382 21.1918 12.0382C22.564 11.2648 24.118 10.6572 25.739 10.8334C26.355 10.9003 25.8547 10.8303 26.5615 10.9456C26.5678 10.9513 27.5701 11.3309 27.6114 11.3527C27.9258 11.5015 28.1326 11.6306 28.4195 11.8239C29.9262 12.8393 31.1753 14.7448 31.1278 16.9364Z" fill="#ddd" />
                            <path opacity="0.45" d="M19.2758 13.4729H17.9084C17.65 13.4729 17.4406 13.6987 17.4406 13.9772V17.6735C17.4406 18.0632 17.1475 18.3792 16.7859 18.3792H13.544C13.1825 18.3792 12.8894 18.0632 12.8894 17.6735V13.9772C12.8894 13.6987 12.6799 13.4729 12.4216 13.4729H11.0565C10.4731 13.4729 10.181 12.7125 10.5935 12.2678L14.3034 8.26842C14.7825 7.75192 15.5594 7.75238 16.038 8.26947L19.7392 12.2684C20.151 12.7133 19.8588 13.4729 19.2758 13.4729Z" fill="black" />
                            <path d="M19.1058 13.3205H17.7384C17.4801 13.3205 17.2706 13.5463 17.2706 13.8249V17.5211C17.2706 17.9109 16.9776 18.2268 16.616 18.2268H13.3741C13.0126 18.2268 12.7194 17.9109 12.7194 17.5211V13.8249C12.7194 13.5463 12.51 13.3205 12.2516 13.3205H10.8866C10.3032 13.3205 10.0111 12.5602 10.4236 12.1155L14.1335 8.11607C14.6126 7.59958 15.3895 7.60003 15.8681 8.11713L19.5693 12.1161C19.9811 12.561 19.6888 13.3205 19.1058 13.3205Z" fill="000" />
                          </svg>
                          <span>Click to upload a file<span>or drop a file here</span></span>
                        </label>
                      </div>
                    </div>
                  </div>
                </div>
                {/* <ul className="profilerounds">
                  <div className="left">
                    <li className="lifirsts">
                    </li>
                    <li style={{ background: "var(--widgetChatMessageColor)", padding: "8px 10px", borderRadius: "12px", margin: "2px 0px 12px 0px", width: "auto" }}>
                      <div className="leftmessage">
                        <div className="botMsgArea">
                          <div className="worktualattachs">
                            <input type="file" id="file" onChange={handleFileChange} />
                            <label for="file">
                              <svg width="32" height="25" viewBox="0 0 32 25" fill="none" xmlns="http://www.w3.org/2000/svg">
                                <path d="M8.15449 23.6553C4.45487 23.6553 1.26783 21.1583 0.576434 17.7182C-0.0877948 14.4195 1.6155 10.989 4.62591 9.5613L4.86017 9.45013L4.87238 9.1912C4.9597 7.34986 5.67301 5.57282 6.88119 4.1875C8.43145 2.41574 10.6748 1.39941 13.0357 1.39941C15.536 1.39941 17.9388 2.59288 19.4633 4.59198L19.6634 4.85451L19.9692 4.7306C20.5819 4.48248 21.2358 4.3569 21.9128 4.3569C22.3503 4.3569 22.7908 4.41041 23.2222 4.51629C25.1778 4.99367 26.7183 6.69781 27.0568 8.75678C27.1422 9.2741 27.1467 9.76589 27.0702 10.2602L27.0652 10.2929V10.3034L27.0078 10.6347L27.3313 10.7736C27.7208 10.9292 28.0837 11.1129 28.4106 11.3195C30.3875 12.5462 31.566 14.657 31.566 16.9678C31.566 20.6552 28.5633 23.6553 24.8726 23.6553H8.15449Z" fill="#C4F5E8" />
                                <path d="M13.0373 1.83314C15.4038 1.83314 17.6782 2.96299 19.1213 4.85549L19.5215 5.38013L20.133 5.13242C20.6937 4.90551 21.2932 4.79045 21.9148 4.79031C22.3175 4.79031 22.7233 4.83979 23.1219 4.93753C24.9107 5.37445 26.3212 6.93802 26.6319 8.82885C26.7098 9.3004 26.7141 9.74743 26.6449 10.1956L26.6365 10.2502L26.5025 10.9081L27.1725 11.176C27.5368 11.3217 27.8762 11.4934 28.1874 11.69C30.0335 12.8354 31.1356 14.8088 31.1356 16.969C31.1356 20.4182 28.3268 23.2245 24.8743 23.2245H8.15636C4.66188 23.2245 1.65312 20.8734 1.00203 17.6338C0.376324 14.5274 1.97884 11.2971 4.81282 9.95288L5.2812 9.73082L5.30572 9.21297C5.38862 7.46809 6.06437 5.78475 7.20765 4.47415C8.67626 2.79581 10.8012 1.83314 13.0373 1.83314ZM13.0374 0.96875C10.5507 0.96875 8.19097 2.03773 6.55714 3.90488C5.27788 5.37142 4.53447 7.22997 4.44229 9.17195C1.24097 10.6905 -0.556998 14.2721 0.154668 17.8046C0.886547 21.4468 4.25212 24.0889 8.15633 24.0889H24.8742C28.8017 24.0889 32 20.8964 32 16.969C32 14.4853 30.7005 12.232 28.6432 10.9555C28.2829 10.7279 27.8969 10.5348 27.4935 10.3734C27.4964 10.3592 27.4992 10.3419 27.4992 10.3274C27.5828 9.78573 27.5771 9.24692 27.4848 8.68804C27.1188 6.4605 25.4505 4.61652 23.327 4.09784C22.8575 3.98278 22.3833 3.92595 21.9145 3.92595C21.1864 3.92595 20.4713 4.06303 19.8087 4.33125C18.198 2.21919 15.6913 0.96875 13.0374 0.96875Z" fill="#00B885" />
                                <path opacity="0.25" fillRule="evenodd" clipRule="evenodd" d="M31.1278 16.9364C31.05 20.5235 28.6746 22.6689 25.6278 23.2395L15.4179 23.2394C15.4066 23.0455 15.4043 22.8514 15.4043 22.6575C15.4043 20.7812 15.7853 19.007 16.4657 17.432C16.4748 17.4091 16.4861 17.3887 16.4951 17.3657C17.5044 15.0606 19.1553 13.1843 21.1783 12.0458C21.1828 12.0407 21.1873 12.0382 21.1918 12.0382C22.564 11.2648 24.118 10.6572 25.739 10.8334C26.355 10.9003 25.8547 10.8303 26.5615 10.9456C26.5678 10.9513 27.5701 11.3309 27.6114 11.3527C27.9258 11.5015 28.1326 11.6306 28.4195 11.8239C29.9262 12.8393 31.1753 14.7448 31.1278 16.9364Z" fill="#00E2A4" />
                                <path opacity="0.45" d="M19.2758 13.4729H17.9084C17.65 13.4729 17.4406 13.6987 17.4406 13.9772V17.6735C17.4406 18.0632 17.1475 18.3792 16.7859 18.3792H13.544C13.1825 18.3792 12.8894 18.0632 12.8894 17.6735V13.9772C12.8894 13.6987 12.6799 13.4729 12.4216 13.4729H11.0565C10.4731 13.4729 10.181 12.7125 10.5935 12.2678L14.3034 8.26842C14.7825 7.75192 15.5594 7.75238 16.038 8.26947L19.7392 12.2684C20.151 12.7133 19.8588 13.4729 19.2758 13.4729Z" fill="black" />
                                <path d="M19.1058 13.3205H17.7384C17.4801 13.3205 17.2706 13.5463 17.2706 13.8249V17.5211C17.2706 17.9109 16.9776 18.2268 16.616 18.2268H13.3741C13.0126 18.2268 12.7194 17.9109 12.7194 17.5211V13.8249C12.7194 13.5463 12.51 13.3205 12.2516 13.3205H10.8866C10.3032 13.3205 10.0111 12.5602 10.4236 12.1155L14.1335 8.11607C14.6126 7.59958 15.3895 7.60003 15.8681 8.11713L19.5693 12.1161C19.9811 12.561 19.6888 13.3205 19.1058 13.3205Z" fill="white" />
                              </svg>
                              <span>Click to upload a CSV file<span>or drop a file here</span></span>
                            </label>
                          </div>
                        </div>
                      </div>
                    </li>
                  </div>
                </ul> */}
              </div>}


              <div ref={bottomRef} id="bottomref" /></div> : ""
          }
          {messageList?.endNotify && 
          <div className="conrfirmSection">
            <div>
              <p>Are you sure you want to end this session?</p>
              <div className="btnSection">
                <button className="btnCancel" onClick={closeEndSession}>Yes</button>
                <button className="btnGreen" onClick={contEndSession}>No</button>
              </div>
            </div>
          </div>}

        </div>

        {builderList?.feedBackFormStatus && <FeedBackForm />}
        {!messageList?.worktualAppointment?.isShow && !builderList?.showUpdateAddresss && !builderList?.feedBackFormStatus && !messageList?.purchasePlanData?.purchasePlanStatus && !messageList?.callBackButtonStatus && !builderList?.showCustomerForm && !messageList?.paymentEnable && !timeoutStatus && !stillEnd ? <div className="chatFooter chatmessagefooter" ref={parentDivRef} onMouseLeave={() => setMicLanguageStatus(false)}>

          {(messageList.personalisation.uploadDocs && !messageList?.worktualAppointment?.isShow && !builderList?.feedBackFormStatus && !messageList?.callBackButtonStatus && !messageList?.multipleTemp && messageList?.connectionType?.isAgentConnection) || isFeatureEnabled('botDocUpload') ? (
            <div className="attach">
              <label htmlFor="file-input">
                {/* <span className="icon_attach"></span> */}
                <img src={attachimg} />
              </label>
            </div>
          ) : (
            ""
          )}

          <input
            id="file-input"
            type="file"
            key={file ? file.name : null}
            onChange={handleFileChange}
            style={{ display: "none" }}
          />

          <input
            id="image-file-input"
            type="file"
            key={file ? file.name : null}
            onChange={handleFileChangeForImage}
            style={{ display: "none" }}
            accept="image/*"
          />
          {!messageList?.multipleTemp && <div className="chatFooterinput" id="footerId" style={{ pointerEvents: messageList?.reminderFlowStatus ? "none" : 'auto' }}>
            <div className={builderList?.connectorStatus ? "message newecomm" : "message"}>
              <label htmlFor="file-input">
                {builderList?.connectorStatus && <Tooltip placement="top" title="Image Search" ><svg xmlns="http://www.w3.org/2000/svg" width="18" height="19" viewBox="0 0 18 19" fill="none">
                  <path d="M8.9994 14.7118C6.684 14.7118 4.7994 12.8272 4.7994 10.5118C4.7994 8.19635 6.684 6.31175 8.9994 6.31175C11.3148 6.31175 13.1994 8.19635 13.1994 10.5118C13.1994 12.8272 11.3148 14.7118 8.9994 14.7118ZM8.9994 7.51175C7.3452 7.51175 5.9994 8.85755 5.9994 10.5118C5.9994 12.166 7.3452 13.5118 8.9994 13.5118C10.6536 13.5118 11.9994 12.166 11.9994 10.5118C11.9994 8.85755 10.6536 7.51175 8.9994 7.51175Z" fill="#757676" />
                  <path d="M15.6 18.9688C14.2764 18.9688 13.2 17.8924 13.2 16.5688C13.2 15.2452 14.2764 14.1688 15.6 14.1688C16.9236 14.1688 18 15.2452 18 16.5688C18 17.8924 16.9236 18.9688 15.6 18.9688ZM15.6 15.3688C14.9382 15.3688 14.4 15.907 14.4 16.5688C14.4 17.2306 14.9382 17.7688 15.6 17.7688C16.2618 17.7688 16.8 17.2306 16.8 16.5688C16.8 15.907 16.2618 15.3688 15.6 15.3688Z" fill="#757676" />
                  <path d="M10.7994 4.56875H7.1994C6.207 4.56875 5.3994 3.76115 5.3994 2.76875C5.3994 1.77635 6.207 0.96875 7.1994 0.96875H10.7994C11.7918 0.96875 12.5994 1.77635 12.5994 2.76875C12.5994 3.76115 11.7918 4.56875 10.7994 4.56875ZM7.1994 2.16875C6.8682 2.16875 6.5994 2.43755 6.5994 2.76875C6.5994 3.09995 6.8682 3.36875 7.1994 3.36875H10.7994C11.1306 3.36875 11.3994 3.09995 11.3994 2.76875C11.3994 2.43755 11.1306 2.16875 10.7994 2.16875H7.1994Z" fill="#757676" />
                  <path d="M0.6 8.76935C0.2688 8.76935 0 8.50055 0 8.16935V6.36875C0 4.04915 1.8804 2.16875 4.2 2.16875H5.9994V3.36875H4.2C2.5434 3.36875 1.2 4.71215 1.2 6.36875V8.16935C1.2 8.50055 0.9312 8.76935 0.6 8.76935Z" fill="#757676" />
                  <path d="M11.3994 18.9688H4.2C1.8804 18.9688 0 17.0884 0 14.7688V11.7682C0 11.437 0.2688 11.1682 0.6 11.1682C0.9312 11.1682 1.2 11.437 1.2 11.7682V14.7688C1.2 16.4254 2.5434 17.7688 4.2 17.7688H11.4C11.7312 17.7688 12 18.0375 12 18.3688C12 18.7 11.7306 18.9688 11.3994 18.9688Z" fill="#757676" />
                  <path d="M17.4 13.4542C17.0688 13.4542 16.8 13.1854 16.8 12.8542V6.36875C16.8 4.71215 15.4566 3.36875 13.8 3.36875H11.9994V2.16875H13.8C16.1196 2.16875 18 4.04915 18 6.36875V12.8542C18 13.1854 17.7312 13.4542 17.4 13.4542Z" fill="#757676" />
                  <path d="M8.9994 14.1111C10.9876 14.1111 12.5994 12.4994 12.5994 10.5112C12.5994 8.52292 10.9876 6.91115 8.9994 6.91115C7.01117 6.91115 5.3994 8.52292 5.3994 10.5112C5.3994 12.4994 7.01117 14.1111 8.9994 14.1111Z" fill="#757676" />
                  <path d="M15.6 18.3688C16.5941 18.3688 17.4 17.5629 17.4 16.5688C17.4 15.5746 16.5941 14.7688 15.6 14.7688C14.6059 14.7688 13.8 15.5746 13.8 16.5688C13.8 17.5629 14.6059 18.3688 15.6 18.3688Z" fill="#757676" />
                </svg></Tooltip>}
              </label>
              {file && (
                <>
                  <div className={`${imageFormat.indexOf(file?.type) > -1 ? "imagesAttach" : "textpdf"}`} >
                    <div className="left">
                      {
                        pdfFormats.indexOf(file?.type) > -1 ? <img src={Pdficon} alt="" /> :
                          excelFormats.indexOf(file?.type) > -1 ? <img src={grid_xls} alt="" /> :
                            pptFormats.indexOf(file?.type) > -1 ? <img src={grid_ppt} alt="" /> :
                              wordFormats.indexOf(file?.type) > -1 ? <img src={grid_word} alt="" /> :
                                <img src={file?.filepath} alt="" />}
                    </div>
                    {imageFormat.indexOf(file?.type) === -1 && <div className="right">
                      <p>{file.name}</p>
                      <p>{Math.round(file.size / 1024)} KB</p>
                    </div>}
                    <div className="textpdfclose" onClick={handleClose}>
                      x
                    </div>
                  </div>
                </>
              )}

              <TextArea
                ref={textareaRef}
                placeholder="Type your message here..."
                autoSize={{ minRows: 1, maxRows: 5 }}
                className="MessageInput"
                type="text"
                value={message}
                onClick={() => setMicLanguageStatus(false)}
                onChange={(e) => handleMessage(e)}
                onKeyDown={enterButton}
                onInput={updateParentHeight}
                disabled={
                  messageList.connectionType.isDTMFConnection || messageList.callBackStatus ? true : false
                }
                readOnly={messageList?.reminderFlowStatus ? true : false}
              />
            </div>

            {![54, 116, 76, 32].includes(messageList.personalisationAll?.plan_id) && (
              isMic ? <div className="mics" onClick={handleSelectLangauage} onMouseOver={() => handleMouseEvent()}>
                <img src={mic} /></div>
                :
                <div className="mics activates" onClick={stopCapturing}>
                  {/* <div className="lanpopup">Change mic language</div> */}
                  <img src={mictwo} />
                </div>
            )}

            {micLanguageStatus && <div className="lanpopup" onClick={handleMultiLan}>Change mic language</div>}
            {sendIcon && file !== null && file !== false ? <div className="chaticons"><img src={sendArrow} onClick={sendFileMessage} alt="" /></div> :
              (sendIcon) ? (
                <div className="chaticons">
                  <img src={sendArrow} onClick={sendTextMessage} alt="" />
                </div>
              ) : null}

          </div>}
        </div> : ""}
        {messageList.personalisation.isWidgetUnbranding ? (
          <div className={messageList.personalisation.isConsentForm ? "footercopyright footertopspace" : "footercopyright"} >
            <div className="footercopys">
              <p>Powered by</p>
              <img src={FooterLogo} alt="" onClick={() => window.open('https://worktual.co.uk/', "_blank")} />

            </div>
            {messageList.personalisation.isConsentForm && <p>By chatting, you consent to this chat being stored according to our privacy policy</p>}
          </div>
        ) : ""}
      </div >
      {isFeedback && <div className="endchats">
        <div className="endchatsinner">
          <h3>Share your feedback<svg onClick={handleCancelFeedback} xmlns="http://www.w3.org/2000/svg" width="14" height="14" viewBox="0 0 14 14" fill="none">
            <path d="M8.00078 7.00215L12.8799 2.12256C13.1566 1.84665 13.1566 1.39748 12.8799 1.12156C12.6033 0.844896 12.154 0.844896 11.8789 1.12156L6.99978 6.00106L2.12061 1.12156C1.84553 0.844896 1.39469 0.844896 1.11961 1.12156C0.842943 1.39748 0.842943 1.84665 1.11961 2.12256L5.99878 7.00215L1.11961 11.8817C0.842943 12.1576 0.842943 12.6068 1.11961 12.8827C1.25794 13.0211 1.43861 13.0902 1.62011 13.0902C1.80161 13.0902 1.98303 13.0211 2.12061 12.8827L6.99978 8.00323L11.8789 12.8827C12.0164 13.0211 12.1979 13.0902 12.3794 13.0902C12.5609 13.0902 12.7416 13.0211 12.8799 12.8827C13.1566 12.6068 13.1566 12.1576 12.8799 11.8817L8.00078 7.00215Z" fill="#505152" />
          </svg></h3>
          <div className="endchatslist">
            <p className="emailinputs">

              <TextArea placeholder="Enter Your Feedback" className="formInputHeight formTextareas" onChange={handleFeedBackChange} />
              {/* {mailerror && <span>Enter your mail</span>} */}
            </p>

            {/* <div class="ant-form-item-explain-error">hello</div> */}
          </div>
          <div className="endbottoms">
            <div className="endchatsbtn endchatsgreenplain" onClick={handleCancelFeedback}>Skip</div>
            <div className="endchatsbtn endchatsgreen" onClick={handleSendLikeFeedback}>Submit</div>
          </div>
        </div>
      </div>
      }



      {shopingStore?.showCartModel && <HandelCartList />}
      {shopingStore?.showItemModel && (
        `${messageList.personalisationAll?.domainId}` === nickysDomainAI
        ? <ShopingSelectedItem_nicky /> 
        : <ShopingSelectedItem_Pothys />
      )}
      {shopingStore?.cartData?.length > 0 && <ShopingCartIcon />}

    </>
  );
};
export default ChatScreen;
