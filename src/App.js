/* eslint-disable */
import "../src/chatPreview.scss";
import "./styles/style-fonts.css";

import React, { useEffect, useState, useRef } from "react";
import { useDispatch, useSelector } from "react-redux";
import { config } from "../src/base/constants";
import { messageActions } from "./store/slice/message_slice";
import { liveTrackerActions } from "./store/slice/live_tracker";
import { v4 as uuidv4 } from "uuid";
import { siteTracking, updateEndSession, sendEmailTranscript, getSupervisorExt, visitorCloseTab } from "./service/socketservice"
import { getLanguageTrans, getPaymentSuccess, newChatSendMsg, isMobileCheck, chatEndMsg, saveContact, isJsonString, isJSON,nlpConvoSummary, getLocationDetails } from "./chatbot/utils"
import { clearSessionStorage, getItemWithExpiry, visitorInsightsInterval, isFeatureEnabled, domainFeatureMap, getClientAddress, sendRNCloseChat } from '../src/chatbot/utils2.0';
import closemsgs from "../src/images/closemessage.png";
import moment from 'moment'
import jsPDF from 'jspdf';
import { chatBuilderActions } from "./store/slice/builder_slice";
import { setIndexDbItem, getIndexDbItem, removeIndexDbItem, clearIndexDb } from './indexedDb/index';
import initializeSocket from './service/socketservice';
import html2pdf from 'html2pdf.js';
require("./service/socketservice");
import ChatBot from './chatbot/index';
import ChatScreen from './chatbot/chatscreen';
import ConsentForm from './chatbot/consentForm';
// const ChatBot = lazy(() => import("./chatbot/index"));
// const ChatScreen = lazy(() => import("./chatbot/chatscreen"));
// const ConsentForm = lazy(() => import('./chatbot/consentForm'))
import Tooltip from 'antd/es/tooltip';
import Input from "antd/es/input/Input";
// const Tooltip = lazy(() => import('antd/es/tooltip'));
// const Input = lazy(() => import('antd/es/input'));

let uniqueChatId = await getIndexDbItem('uniqueChatId');
if (!uniqueChatId) {
  uniqueChatId = uuidv4();
  setIndexDbItem('uniqueChatId', uniqueChatId)
}


sessionStorage.setItem("status", "Active");

let liveTrackStatus = sessionStorage.getItem("status");
let Chatheight = 0;
let Chatwidth = 0;
let flexend = 'flex-end';
let flexstart = 'flex-start';
let chatbotlayoutcss = 'chatbotlayout'
let chatbotLayoutRightcss = 'chatbotlayout chatbotLayoutRight'
let mobileBrowser = 'worktualmobileBrowser'
let chatPreviewCss = 'chatPreview'
let chatPreviewRigtCss = 'chatPreview chatright'
let chatpreviewleftview = 'chatPreview chatpreviewleftview'
let chatPreviewMobileVisible = 'chatPreviewMobileVisible'
let chatPreviewMobileHide = 'chatPreviewMobileHide'
let chatIconCss = 'chatIcon';
let chatIConLeftRigtCss = 'chatIcon chatIconPositionLeft';
const socket = initializeSocket();


function App() {
  const messageList = useSelector((state) => state.message);
  const liveTracker = useSelector((state) => state.liveTracker);
  const builderList = useSelector((state) => state.chatBuilder);
  const dispatch = useDispatch();
  const opentext = <span>How can i help you!</span>;
  const [close, setClose] = useState(false);
  const [msgShow, setmsgShow] = useState('');
  const [timeoutMsg, setTimeoutMsg] = useState(null)
  const [settingShow, setsettingShow] = useState(false);
  const [isFirstClick, setFirstclick] = useState(false);
  const [isMobile, setMobile] = useState(false);
  const [isMobileIcon, setMobileIcon] = useState(false);
  const [isTestBot, setTestbot] = useState(false);
  const [ismailShow, setismailShow] = useState(false);
  const [mailerror, setmailerror] = useState(false);
  const [mailEvt, setmailEvt] = useState('');
  const [notifySound, setNotifySound] = useState(true);
  const [isIconHide, setIconHide] = useState(false);
  const [triggerStyle, setTriggerStyle] = useState(true);
  const [multiLanguageForText, setMultiLanguageForText] = useState(false);
  let checkInserted = false;
  const divRef = useRef(null);
  const chatIconRef = useRef(null);
  const chatcloseIconRef = useRef(null);
  const chatLayoutRef = useRef(null);
  const RNCheck = new URLSearchParams(window.location.search);
  const isRNCheckHeader = RNCheck.get("isHeader") === '1'
  const handleChangeFormRef = useRef(null);

  useEffect(() => {

    try {
      const device = isMobileCheck();
      setMobile(device)
      let queryParams = new URLSearchParams(window.location.search);
      let callflowIdparams = ''
      const chatBotElement = document.getElementById('chat_bot');

      if (chatBotElement.dataset.myparam) {
        callflowIdparams = chatBotElement.dataset.myparam
      } else {
        callflowIdparams = queryParams.get("webchatid");
      }
      const company_domain = queryParams.get("url");

      console.log('company_domain::::::::::', callflowIdparams, queryParams.get("webchatid"))
      fetch(`${config().CORESERVICE_URL}/client_chat_widget_details/${callflowIdparams}`)
        .then((response) => {
          if (!response.ok) {
            throw new Error('Network response was not ok');
          }
          return response.json();
        })
        .then(async (data) => {
          if (data?.statusCode === 200) {
            try {
              const domain = await getDomain(data.getWebchatWidgetById[0].domainId);
              if (domain !== null && domain[0]?.length > 0) {
                let url = domain[0][0]?.website;
                if(url) {
                  if (!url?.startsWith('http://') && !url?.startsWith('https://')) {
                    url = 'http://' + url;
                  }
                  localStorage.setItem('dynamicDomainUrl', url);
                  if (company_domain !== null) {
                    dispatch(chatBuilderActions.setDynamicDomainURL(url));
                    const hostname = getDomainFromUrl(url);
                    const company_url = new URL(company_domain).hostname?.split('.');
                    let domain1 = hostname[0];
                    const filter_data = company_url.filter((row) => row.includes(domain1));
                    if (filter_data.length === 0) {
                      // setIconHide(true);
                      dispatch(liveTrackerActions.setIconHideStatus(true))
                      return;
                    }
                  }
                }
              }
            } catch (error) {
              console.log(error)
            }
            dispatch(messageActions.widgetId(data.getWebchatWidgetById[0]?.aid));
            checkTriggerData(data.getWebchatWidgetById[0]);
            dispatch(messageActions.personalisationAll(data.getWebchatWidgetById[0]));
            const personalizationData = data.getWebchatWidgetById[0].personalization;
            const personalisationParsed = JSON.parse(personalizationData);

            dispatch(messageActions.personalisation(personalisationParsed));
            dispatch(messageActions.setIconLogo(data.getWebchatWidgetById[0].chatIconLogo));
            dispatch(messageActions.tableData(JSON.parse(data.getWebchatWidgetById[0].primaryForm)));
            const callflowdetailsIDs = data.getWebchatWidgetById[0].chatFlowId;
            dispatch(messageActions.callFlowId(callflowdetailsIDs));
            dispatch(messageActions.setIconLogo(data.getWebchatWidgetById[0].chatIconLogo));
            dispatch(messageActions.setWidgetIconLogo(personalisationParsed?.widgetIconLogo));
            let dataRes = data.getWebchatWidgetById[0].templateData && JSON.parse(data.getWebchatWidgetById[0]?.templateData);
            dispatch(messageActions.templateData(dataRes));
            dispatch(messageActions.formDetails(JSON.parse(data.getWebchatWidgetById[0].formDetails)));
            chatFlowFind(callflowdetailsIDs, data.getWebchatWidgetById[0].domainId);
            dispatch(messageActions.automatedResponse(JSON.parse(data.getWebchatWidgetById[0].autoResponse)));
            getAddonMultiLanguage(data.getWebchatWidgetById[0]);
            customerHistory(data.getWebchatWidgetById[0].domainId);
            sessionStorage.setItem("domainId", data.getWebchatWidgetById[0].domainId);
            if (sessionStorage?.isConnected !== 'agent') {
              sessionStorage.setItem("isConnected", "bot");
            }
            setTimeout(() => {
              getLanguageTrans(data.getWebchatWidgetById[0]?.description, 'description');
            }, 6000);
            console.log(data.getWebchatWidgetById[0]?.description, "response.data.getWebchatWidgetById[0]?.description");
            if (data.getWebchatWidgetById[0].preChartFormFlag === 1) {
              dispatch(messageActions.chatVisible(true));
            }
            const privacyValidation = getItemWithExpiry("privacyPolicy");
            if (personalisationParsed?.isConsentForm == 1 && !privacyValidation) {
              dispatch(messageActions.isConsentForm(true));
            }

            if(isFeatureEnabled('autoBotOpen') && !parseInt(sessionStorage?.ChatHistoryId) && !parseInt(sessionStorage?.autoBotOpen)){
              
              const domainId = data.getWebchatWidgetById[0].domainId;
              const timing = domainFeatureMap[`${domainId}`]?.timing ?? 10000;

              setTimeout(()=> {
                sessionStorage.setItem('autoBotOpen', '1')
                handleChangeFormRef.current()
              },timing)
            }

            await getClientAddress(dispatch)
            visitorInsightsInterval();
            try {
              let planDetails = {
                domainId: data.getWebchatWidgetById[0].domainId,
                planId: data.getWebchatWidgetById[0]?.plan_id ?? null,
              };

              fetch(`${config().CORESERVICE_URL}/get_chat_session_availability`, {
                method: 'POST',
                headers: {
                  'Content-Type': 'application/json',
                },
                body: JSON.stringify(planDetails),
              })
                .then((res) => res.json())
                .then((res) => {
                  if (res?.getwebsiteIdCompanyTypeRes[0][0]?.chatRemainingSessionAgent > 0 || res?.getwebsiteIdCompanyTypeRes[0][0]?.isUnilimitedChat === 1) {
                    dispatch(liveTrackerActions.setAgentSessionAvaiableStatus(true));
                  }
                  if (res?.getwebsiteIdCompanyTypeRes[0][0]?.chatRemainingSessionBot > 0 || res?.getwebsiteIdCompanyTypeRes[0][0]?.isUnilimitedChat === 1) {
                    dispatch(liveTrackerActions.setBotSessionAvaiableStatus(true));
                  }
                })
                .catch((error) => {
                  console.log(error);
                });
            } catch (err) {
              console.log(err);
            }

            // if (data.getWebchatWidgetById[0]?.websiteId !== '') {
            //   getIndustryType(data.getWebchatWidgetById[0]?.websiteId, data.getWebchatWidgetById[0]?.domainId);
            // }
            let sessionId = sessionStorage.getItem("sessionUniqueId");
            if (!sessionId) {
              initializeSocket();
            }
            try {
              showWelcomeMsg(data.getWebchatWidgetById[0], data.getWebchatWidgetById[0].autoResponse);

              let domainData = {
                domainId: data.getWebchatWidgetById[0].domainId,
                to: messageList.uuid,
              };
              getSupervisorExt(domainData);

              fetch(`${config().CORESERVICE_URL}/get_custom_contact_no_auth/${data.getWebchatWidgetById[0].domainId}`)
                .then((response) => response.json())
                .then((responseData) => {
                  dispatch(messageActions.updateCustomContactFiels(responseData?.result[0]));
                });
            } catch (err) {
              console.log(err);
            }
            if (parseInt(data.getWebchatWidgetById[0]?.domainId, 10) === 16522) {
              handleServiceCompany(data.getWebchatWidgetById[0]);
            }
            if(isRNCheckHeader){
              setTimeout(() => {
                handleChangeFormRef.current()    
              }, 1000);
            }
          }
        })
        .catch((error) => {
          console.log(error);
        });


        return () => {
          if(messageList.webtrackingTimer){
            clearInterval(messageList.webtrackingTimer);
          }
        }

    } catch (err) {
      console.log("APP JS ERROR : ::", err)
    }

  }, [liveTracker?.reloadSettings]);

  function getDomainFromUrl(url) {
    try {
      url = url.replace(/^(https?:\/\/)?(www\.)?/, '');
      const parts = url.split('.');
      const domain = parts.slice(-2).join('.');

      return domain.split('.');
    } catch (error) {
      console.log(error)
    }
  }

  const getDomain = (domainId) => {
    return new Promise(resolve => {
      try {
        fetch(`${config().CORESERVICE_URL}/get_company_domain_url/${domainId}`)
          .then((response) => {
            if (!response.ok) {
              throw new Error('Network response was not ok');
            }
            return response.json();
          })
          .then((data) => {
            if (data?.statusCode === 200 && data?.data.length > 0) {
              dispatch(messageActions.setDomainDetails(data?.data[0]));
              //getAddonMultiLanguage(data?.data[0]?.[0]?.companyId);
              resolve(data?.data);
            } else {
              resolve(null);
            }
          })
          .catch((error) => {
            console.error('Fetch error:', error);
            resolve(null);
          });

      } catch (error) {
        console.log(error)
        resolve(null)
      }
    })
  }
  const checkTriggerData = async (response) => {
    try {
      const template_data = JSON.parse(response.templateData)
      const flowId = template_data.map((row) => row.chatFlowId)
      if (template_data.length === 0)
        setTriggerStyle(true)
      else {
        try {
          const ipResponse = await fetch('https://getip.worktual.co.uk/');
          if (!ipResponse.ok) {
            throw new Error('Network response was not ok');
          }
          const ipData = await ipResponse.json();
          const data = {
            "multiChatId": flowId,
            "domainId": response?.domainId,
            "ipAddress": ipData?.clientIPv6
          };
          const postResponse = await fetch(`${config().CORESERVICE_URL}/get_nodes_entry_point`, {
            method: 'POST',
            headers: {
              'Content-Type': 'application/json',
            },
            body: JSON.stringify(data),
          });

          if (!postResponse.ok) {
            throw new Error('Network response was not ok');
          }

          const postData = await postResponse.json();

          if (postData?.statusCode === 200 && postData?.data) {
            const flow_id = [];
            const visitor_says = [];
            let index = 0;

            for (let entry_data of postData?.data?.entryResult) {
              const info = entry_data?.data?.information;
              if (Object.keys(info).length === 0) {
                flow_id.push(flowId[index]);
              } else {
                const trigger_data = isJSON(info?.trigger_data) ? JSON.parse(info?.trigger_data) : [];
                const trigger_id = trigger_data.map((row) => row.triggerKey);

                for (let key of trigger_id) {
                  if (key === "1") {
                    flow_id.push(flowId[index]);
                    break;
                  } else if (key === "2") {
                    let queryParams = new URLSearchParams(window.location.search);
                    const url = queryParams.get("url");
                    const row_data = trigger_data.filter((row) => row.triggerKey === key);

                    if (row_data.length > 0) {
                      if (row_data[0]?.option === "exact_address" && row_data[0]?.inputValue === url) {
                        flow_id.push(flowId[index]);
                        break;
                      } else if (row_data[0]?.option === "address_contains" && row_data[0]?.inputValue.includes(url)) {
                        flow_id.push(flowId[index]);
                        break;
                      }
                    }
                  } else if (key === "4") {
                    const userExist = postData?.data?.ipExistinguser[0][0]?.isExists;
                    if (!userExist) {
                      flow_id.push(flowId[index]);
                      break;
                    }
                  } else if (key === "5") {
                    const userExist = postData?.data?.ipExistinguser[0][0]?.isExists;
                    if (userExist) {
                      flow_id.push(flowId[index]);
                      break;
                    }
                  } else if (key === "7") {
                    const row_data = trigger_data.filter((row) => row.triggerKey === key);
                    visitor_says.push({ questions: row_data[0]?.inputValues.map((row) => row.value), flowId: flowId[index] });
                    flow_id.push(flowId[index]);
                    break;
                  }
                }

                if (trigger_data.length === 0) {
                  flow_id.push(flowId[index]);
                }
              }
              index++;
            }
            dispatch(messageActions.setTriggerData({ flow_id, visitor_says }));
            setTriggerStyle(flow_id.length > 0);
          }
        } catch (error) {
          console.log(error);
        }

      }
    } catch (error) {
      console.log(error)
    }
  }
  const showWelcomeMsg = (data, autoResponsed) => {
    const autoResponse = JSON.parse(autoResponsed)
    const messageShownBefore = JSON.parse(data?.autoResponse);
    let timer = messageShownBefore?.isenableTracker ? messageShownBefore?.trackerDelay : 3;
    // let message = data?.title == "Worktual Bot"
    //   ? "Hi, I'm Lukas. How can I assist you today?"
    //   : data?.title == "Vectone Bot"
    //     ? "Have questions about how Vectone can help to overcome communication challenges? I'm here to help."
    //     : autoResponse?.trackerMsg !== '' ? autoResponse?.trackerMsg : '';
    let message = autoResponse?.trackerMsg !== '' ? autoResponse?.trackerMsg : '';
    (setTimeout(() => {
      setClose(true);
      console.log("timer", timer)
      setmsgShow(message);
    }, timer * 1000))

  }

  const getIndustryType = async (websiteId, domainId) => {
    try {
      const sitedId = isJSON(websiteId) ? JSON.parse(websiteId) : ''

      if (!Array.isArray(sitedId) && sitedId === '')
        return
      const data = {
        websiteId: Array.isArray(sitedId) ? sitedId[0] : sitedId,
        domainId: domainId
      };
      const response = await fetch(`${config().CORESERVICE_URL}/get_company_type_website`, {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify(data),
      });
      if (!response.ok) {
        throw new Error('Network response was not ok');
      }
      const responseData = await response.json();
      if (responseData?.statusCode === 200) {
        if (responseData?.getwebsiteIdCompanyTypeRes.length > 0) {
          if (responseData?.getwebsiteIdCompanyTypeRes[0]?.CompanyType !== null) {
            dispatch(messageActions.setIndustryType(responseData?.getwebsiteIdCompanyTypeRes[0]?.CompanyType));
          }
        }
      }
    } catch (error) {
      console.log(error)
    }
  }

  useEffect(() => {
    if (liveTracker.uuid !== '' && liveTracker.uuid) {
      let liveTrackData = {
        sesionId: liveTracker?.uuid,
        domainId: messageList?.personalisationAll?.domainId,
        status: liveTrackStatus
      }
      siteTracking(liveTrackData)
    }
  }, [liveTracker?.uuid])

  useEffect(() => {
    const handleBeforeUnload = (event) => {
      const isAgentConnection = sessionStorage.getItem("isConnected")
      if (isAgentConnection === 'agent') {
        let liveConnectionPaylaod = {
          domainId: sessionStorage.getItem('domainId'),
          sessionId: messageList?.uuid?.trim(),
          historyId: parseInt(sessionStorage?.ChatHistoryId),
          ext: liveTracker?.liveChatCutData?.ext
        }
        socket.emit('closeRingingChat', liveConnectionPaylaod);
      }
      const chatDetails = {
        ChatHistoryId: parseInt(sessionStorage?.ChatHistoryId),
        ChatDurationHistoryId: parseInt(sessionStorage?.ChatDurationHistoryId),
        to: sessionStorage?.agentToAddress,
        isBot: messageList?.connectionType?.isBotConnection,
        domainId: messageList?.personalisationAll?.domainId,
        sessionId: messageList?.uuid
      }
      // if (![54, 116, 76, 32].includes(messageList.personalisationAll?.plan_id) && chatDetails?.ChatDurationHistoryId) {
      if (chatDetails?.ChatDurationHistoryId) {
        updateEndSession(chatDetails);
      }
      let visitorData = {
        domainId: sessionStorage.getItem('domainId'),
        visitorId: sessionStorage.getItem('visitorId'),
        sessionEndTime: new Date().getTime()
      }
      socket.emit('visitorCloseTabSite', visitorData);
      // if (messageList?.agentConnectingStatus) {
      let closeQueueUser = {
        domainId: parseInt(sessionStorage.getItem('domainId')),
        sessionId: messageList?.uuid,
        historyId: parseInt(sessionStorage?.ChatHistoryId),
        qid: parseInt(sessionStorage.getItem('qid'))
      }
      socket.emit('dequeueUser', closeQueueUser)
      // }

      sendRNCloseChat();
    };
    window.addEventListener('beforeunload', handleBeforeUnload);
    return () => {
      window.removeEventListener('beforeunload', handleBeforeUnload);
    };
  }, []);


  const chatFlowFind = async (flowId, domainId) => {
    let body = {
      cfid: flowId,
      domainId: domainId,
    };
    try {
      const body = {
        cfid: flowId,
        domainId: domainId,
      };
      const response = await fetch(`${config().CALLSERVICE_URL}/get_chatflow_nodes_list`, {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify(body),
      });
      if (!response.ok) {
        throw new Error('Network response was not ok');
      }
      const responseData = await response.json();
      const nodeData = JSON.parse(responseData.result[0].nodes);
      dispatch(messageActions.storeNodeData(nodeData));
      dispatch(messageActions.setCompanyDetails(responseData.result[0].companyId));
      //getAddonMultiLanguage(responseData.result[0].companyId);
      const paymentData = {
        domainId: domainId,
        companyId: responseData.result[0].companyId,
      };
      console.log(nodeData, "nodeData");
      nodeData.forEach((list) => {
        if (list.type === "customInput") {
          dispatch(messageActions.setDtmfData(list.data));
        }
      });
      const resultNode = JSON.parse(responseData.result[0].nodes);
      resultNode.forEach((list) => {
        if (list?.id.includes('integration')) {
          const methodType = list?.data?.information;
          if (methodType?.method === 'connectors') {
            const integrationData = JSON.parse(methodType?.integration_data);
            switch (integrationData?.connector) {
              case 4:
                dispatch(chatBuilderActions.setConnectorStatus(true));
                dispatch(chatBuilderActions.setConnectionName('shopify'));
                break;
              case 5:
                dispatch(chatBuilderActions.setConnectorStatus(true));
                dispatch(chatBuilderActions.setConnectionName('Magento'));
                break;
              case 11:
                dispatch(chatBuilderActions.setConnectorStatus(true));
                dispatch(chatBuilderActions.setConnectionName('BigCommerce'));
                break;
              case 13:
                dispatch(chatBuilderActions.setConnectorStatus(true));
                dispatch(chatBuilderActions.setConnectionName('Wix'));
                break;
              case 12:
                dispatch(chatBuilderActions.setConnectorStatus(true));
                dispatch(chatBuilderActions.setConnectionName('WooCommerce'));
                break;
              default:
                break;
            }
          }
        }
      });
    } catch (error) {
      console.error(error);
    }
    try {
      const url = `${config().CORESERVICE_URL}/get_question_answer_list?limit=99&offset=0&domainId=${domainId}&chatFlowId=${flowId}`;
      const response = await fetch(url, {
        method: 'GET',
        headers: {
          'Content-Type': 'application/json',
        },
      });
      if (!response.ok) {
        throw new Error('Network response was not ok');
      }
      const responseData = await response.json();
      if (response.status === 200) {
        dispatch(messageActions.questionAnswerList(responseData?.data));
      }
    } catch (error) {
      console.error(error);
    }

    try {
      const url = `${config().CORESERVICE_URL}/get_fallback_message_list_no_auth/${domainId}`;
      const response = await fetch(url, {
        method: 'GET',
        headers: {
          'Content-Type': 'application/json',
        },
      });
      if (!response.ok) {
        throw new Error(`Network response was not ok: ${response.statusText}`);
      }
      const responseData = await response.json();
      if (response.status === 200) {
        console.log(responseData, "response");
      }
    } catch (error) {
      console.error(error);
    }

  }

  useEffect(() => {
    let intervalId;
    let queryParams = new URLSearchParams(window.location.search);
    let transaction_id = queryParams.get("transaction_id");

    if (messageList?.formgenerate || (transaction_id && transaction_id !== "")) {
      console.log("inside if useEffect", messageList);
      intervalId = setInterval(() => {
        console.log(transaction_id, "messageListmessageListmessageList");
        if (messageList?.transactionData?.length > 0) {
          dispatch(messageActions.formgenerate(false));
          clearInterval(intervalId); // Clear the interval when condition is met
          if (messageList?.transactionData[0].transaction_status === "CHALLENGE_REQUIRED") {

          }
        }

        if (messageList?.formgenerate || (transaction_id && transaction_id !== "")) {
          getPaymentSuccess();
        }
      }, 6000);
    }
    return () => {
      clearInterval(intervalId);
    };

  }, [messageList?.formgenerate]);



  useEffect(() => {
    let data = document.getElementById('welcome-message')
    let string = ` var viewportMetaTag = document.querySelector('meta[name=viewport]');
      if (!viewportMetaTag) {
      viewportMetaTag = document.createElement('meta');
      viewportMetaTag.name = 'viewport';
      document.head.appendChild(viewportMetaTag);
      viewportMetaTag.content = 'width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no';
      }else{
      let viewportContent = viewportMetaTag.getAttribute('content');
      if(!viewportContent.includes('maximum-scale=1.0')){
      viewportContent = viewportContent+', maximum-scale=1.0'
      }
      if(!viewportContent.includes('user-scalable=no')){
      viewportContent = viewportContent+', user-scalable=no'
      }
      viewportMetaTag.content = viewportContent;
      }`

    let timer = 3;
    setTimeout(() => {
      if (divRef.current) {
        // let width = divRef.current.clientWidth;
        // let height = divRef.current.clientHeight;

        // console.log('Width:', typeof (height), 'px000000');
        // console.log('Height:', typeof (height), 'px');
        // if (height < 40) height = 60
        // width = width + 60

        // width = 80 + parseInt(width);
        // height = 10 + parseInt(height);
        window.top.postMessage(JSON.stringify({ style: string, type: "metatag" }), "*");
        // window.top.postMessage(JSON.stringify({ style: `border-style: none; bottom: 20px; ${messageList.personalisation.position === flexstart ? 'left: 30px;' : 'right: 30px;'}  position: fixed; z-index: 999999; width:${width}px; height:${height}px`, type: "style" }), "*");
      } else {
        if (chatIconRef.current) {
          // let width = chatIconRef.current.clientWidth;
          // let height = chatIconRef.current.clientHeight;
          // console.log('Width:', width, 'px');
          // console.log('Height:', height, 'px');
          // if (height < 40) height = 60
          // width = width + 60
          // width = 80 + parseInt(width);
          // height = 10 + parseInt(height);
          window.top.postMessage(JSON.stringify({ style: string, type: "metatag" }), "*");
          // window.top.postMessage(JSON.stringify({ style: `border-style: none; bottom: 20px; ${messageList.personalisation.position === flexstart ? 'left: 30px;' : 'right: 30px;'}  position: fixed; z-index: 999999; width:${width}px; height:${height}px`, type: "style" }), "*");
        }
      }
    }, timer * 1000)

  }, [messageList?.personalisationAll]);


  document.documentElement.style.setProperty("--widgetIconpostion", messageList.personalisation.position);
  document.documentElement.style.setProperty("--widgetBorderRadius", `${messageList.personalisation.roundedCorners}px`);
  document.documentElement.style.setProperty("--widgetIconShape", `${messageList.personalisation.chatIconShape}px`);
  document.documentElement.style.setProperty("--widgetHeaderBG", messageList.personalisation.chatHeaderColor);
  document.documentElement.style.setProperty("--widgetChatMessageColor", messageList.personalisation.agentMessageColor);
  document.documentElement.style.setProperty("--widgetChatMessageColorhover", `${messageList.personalisation.dynamicAgentMessageTextColor}d1`);
  document.documentElement.style.setProperty("--widgetChatCustomerMessageColor", messageList.personalisation.customerMessageColor);
  document.documentElement.style.setProperty("--widgetActionMessageColor", messageList.personalisation.actionMessageColor);
  document.documentElement.style.setProperty("--widgetActionMessageColorbg", `${messageList.personalisation.actionMessageColor}0f`);
  const font = messageList.personalisation.font;
  const fallbackFont = {
    Lato: '"Lato", sans-serif',
    Noto: '"Noto Sans", sans-serif',
    Manrope: '"Manrope", sans-serif',
    Poppins: '"Poppins", sans-serif',
    Opensans: '"Open Sans", sans-serif',
    Roboto: '"Roboto", sans-serif',
  };
  const selectedFont = fallbackFont[font] || '"Inter", sans-serif';
  document.documentElement.style.setProperty("--widgetPreviewFont", selectedFont);
  document.documentElement.style.setProperty("--widgetIconBg", messageList.personalisation.widgetIconBg);
  document.documentElement.style.setProperty("--dynamicCustomerMessageTextColor", messageList.personalisation.dynamicCustomerMessageTextColor);
  document.documentElement.style.setProperty("--dynamicAgentMessageTextColor", messageList.personalisation.dynamicAgentMessageTextColor);
  document.documentElement.style.setProperty("--dynamictextHeader", messageList.personalisation.dynamictextHeader);
  document.documentElement.style.setProperty("--dynamicfontFamily", messageList.personalisation.font);
  document.documentElement.style.setProperty("--dynamicprimaryButtonBg", messageList.formDetails.primaryButtonBg);
  document.documentElement.style.setProperty("--backgroundImage", `url(${(messageList?.formDetails?.backgroundImage)})`);

  const handleChangeForm = () => {
    sessionStorage.setItem("status", "engage");
    dispatch(messageActions.formVisible(false));
    // setIconHide(true);
    dispatch(liveTrackerActions.setIconHideStatus(true))
    if (messageList.personalisationAll.preChartFormFlag !== 1) {
      dispatch(messageActions.chatVisible(false));
      const data = {
        domainId: messageList.personalisationAll?.domainId ?? 0
      }
      !messageList?.customerFormData?.coid ? saveContact(data) : []
    }
    let LiveAgentStatus = sessionStorage.getItem('status');
    let liveTrackData = {
      sesionId: messageList.uuid,
      domainId: messageList.personalisationAll?.domainId,
      status: LiveAgentStatus
    }
    siteTracking(liveTrackData)
    setClose(false);
    if (!isFirstClick) setFirstclick(true);

    // let visibleClassName = 'worktualChatvisible'
    // if (isMobile) visibleClassName = 'worktualmobilevisible'
    // if (!isTestBot) {
    //   testbotTimer = setTimeout(() => {
    //     document.getElementById('bot-layout')?.classList?.add(visibleClassName);
    //     setTestbot(true);
    //   }, 500)
    // }
    if (isMobile) {
      // setMobileIcon(true);
      dispatch(liveTrackerActions.setMobileIconStatus(true))
    }
    document.body.classList.add('no-scroll');
  };
  handleChangeFormRef.current = handleChangeForm

  const handleServiceCompany = (data) => {
    sessionStorage.setItem("status", "engage");
    dispatch(messageActions.formVisible(false));
    dispatch(liveTrackerActions.setIconHideStatus(true))
    if (data?.preChartFormFlag !== 1) {
      dispatch(messageActions.chatVisible(false));
      const data = {
        domainId: data?.domainId ?? 0
      }
      !messageList?.customerFormData?.coid ? saveContact(data) : []
    }
    let LiveAgentStatus = sessionStorage.getItem('status');
    let liveTrackData = {
      sesionId: messageList.uuid,
      domainId: data?.domainId,
      status: LiveAgentStatus
    }
    siteTracking(liveTrackData)
    setClose(false);
    if (!isFirstClick) setFirstclick(true);
    if (isMobile) {
      dispatch(liveTrackerActions.setMobileIconStatus(true))
    }
    document.body.classList.add('no-scroll');
  }

  useEffect(() => {
    if (liveTracker?.openScreenTriggerStatus) {
      handleChangeForm();
    }
  }, [liveTracker?.openScreenTriggerStatus])

  const handleClose = () => {

    // if (chatIconRef.current) {
    //   let width = chatIconRef.current.clientWidth;
    //   let height = chatIconRef.current.clientHeight;
    //   console.log('close Width:', width, 'px');
    //   console.log('close Height:', height, 'px');
    //   width = parseInt(width);
    //   height = parseInt(height);
    //   window.top.postMessage(JSON.stringify({ style: `border-style: none; bottom: 20px; ${messageList.personalisation.position === flexstart ? 'left: 30px;' : 'right: 30px;'}  position: fixed; z-index: 999999; width:${width}px; height:${height}px`, type: "close" }), "*");
    // }
    setClose(false);
  };

  const handleChangeFormClose = () => {

    // if (chatcloseIconRef.current) {
    //   let width = chatcloseIconRef.current.clientWidth;
    //   let height = chatcloseIconRef.current.clientHeight;
    //   console.log('close Width:', width, 'px');
    //   console.log('close Height:', height, 'px');
    //   width = parseInt(width);
    //   height = parseInt(height);
    //   window.top.postMessage(JSON.stringify({ style: `border-style: none; bottom: 20px; ${messageList.personalisation.position === flexstart ? 'left: 30px;' : 'right: 30px;'}  position: fixed; z-index: 999999; width:${width}px; height:${height}px`, type: "close" }), "*");
    // }
    setClose(false);
    dispatch(messageActions.formVisible(true));
  };



  const handleChangeEndClose = () => {
    try {
      window.speechSynthesis.cancel();
      dispatch(messageActions.setWorktualBooking({ ...messageList.worktualAppointment, isShow: false }))
      dispatch(messageActions.simOrderSts(false));
      dispatch(messageActions.templateKeywordList([]));
      clearTimeout(liveTracker?.timeOutIntervalData);
      const chatDetails = {
        ChatHistoryId: parseInt(sessionStorage?.ChatHistoryId),
        ChatDurationHistoryId: parseInt(sessionStorage?.ChatDurationHistoryId),
        to: sessionStorage?.agentToAddress,
        isBot: messageList?.connectionType?.isBotConnection,
        domainId: messageList?.personalisationAll?.domainId,
        sessionId: messageList?.uuid
      }
      if (parseInt(sessionStorage?.ChatDurationHistoryId)) {
        updateEndSession(chatDetails);
      }
      // if (messageList?.agentConnectingStatus) {
      let closeQueueUser = {
        domainId: messageList.personalisationAll?.domainId,
        sessionId: messageList?.uuid.trim(),
        historyId: parseInt(sessionStorage?.ChatHistoryId),
        qid: liveTracker?.transferQueueId
      }
      socket.emit('dequeueUser', closeQueueUser)
      // }
      if (messageList.agentConnectingStatus && messageList.connectionType.isAgentConnection && Object.keys(liveTracker?.liveChatCutData).length > 0) {
        let liveConnectionPaylaod = {
          domainId: messageList.personalisationAll?.domainId,
          sessionId: messageList?.uuid.trim(),
          historyId: parseInt(sessionStorage?.ChatHistoryId),
          ext: liveTracker?.liveChatCutData?.ext
        }
        socket.emit('closeRingingChat', liveConnectionPaylaod)
      }

      let satisficationData = {
        domainId: messageList.personalisationAll?.domainId,
        channelTypeId: 2003002,
        historyId: parseInt(sessionStorage?.ChatHistoryId),
      };
      const requestData = {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify(satisficationData),
      };
      fetch(`${config().CORESERVICE_URL}/get_customer_satisfication_channel`, requestData)
        .then((response) => {
          if (!response.ok) {
            throw new Error('Network response was not ok');
          }
          return response.json();
        })
        .then((res) => {
          if (res?.insertResult?.length > 0 && res?.insertResult[0]?.isEnable === 1) {
            dispatch(chatBuilderActions.setFeedbackFormData(res?.insertResult[0]));
            dispatch(chatBuilderActions.setFeedbackFormStatus(true));
            // setsettingShow(false);
            dispatch(liveTrackerActions.setSettingStatus(false));
          } else {
            handleEndWithoutCsat();
          }
        })
        .catch((error) => {
          console.log(error);
          handleEndWithoutCsat();
        });
      document.body.classList.remove('no-scroll');
      let liveTrackData = {
        sesionId: liveTracker?.uuid,
        domainId: messageList?.personalisationAll?.domainId,
        status: "closed"
      }
      siteTracking(liveTrackData)

      let visitorData = {
        domainId: sessionStorage.getItem('domainId'),
        visitorId: sessionStorage.getItem('visitorId'),
        sessionEndTime: new Date().getTime()
      }
      socket.emit('visitorCloseTabSite', visitorData);
      sendRNCloseChat();
    } catch (err) {
      console.log(err);
      handleEndWithoutCsat();
    }
  };

  useEffect(() => {
    if (builderList?.closedReferenceStatus) {
      handleEndWithoutCsat()
    }
  }, [builderList?.closedReferenceStatus])


  const handleEndWithoutCsat = () => {
    try {
      nlpConvoSummary()
      const chatDetails = {
        ChatHistoryId: parseInt(sessionStorage?.ChatHistoryId),
        ChatDurationHistoryId: parseInt(sessionStorage?.ChatDurationHistoryId),
        to: sessionStorage?.agentToAddress,
        isBot: messageList?.connectionType?.isBotConnection,
        domainId: messageList?.personalisationAll?.domainId,
        sessionId: messageList?.uuid
      }
      if (chatDetails?.ChatDurationHistoryId) {
        updateEndSession(chatDetails);
      }
      if (messageList?.templateData?.length === 0) {
        chatEndMsg("closed", 2);
      }
  
      let visitorData = {
        domainId: sessionStorage.getItem('domainId'),
        visitorId: sessionStorage.getItem('visitorId'),
        sessionEndTime: new Date().getTime()
      }
      socket.emit('visitorCloseTabSite', visitorData)
      clearSessionStorage();
      sendRNCloseChat();
    } catch (error) {
      console.log(error);
    }
    
  };


  const handleChangeMinimize = () => {
    dispatch(messageActions.formVisible(true));
    setClose(true);
    //setIconHide(false);
    dispatch(liveTrackerActions.setIconHideStatus(false))
    if (isMobile) {
      // setMobileIcon(false)
      dispatch(liveTrackerActions.setMobileIconStatus(false))
    }
    // setTimeout(() => {
    //   if (chatIconRef.current) {
    //     let width = chatIconRef.current.clientWidth;
    //     let height = chatIconRef.current.clientHeight;

    //     console.log('close Width:', width, 'px');
    //     console.log('close Height:', height, 'px');
    //     width = parseInt(width);
    //     height = parseInt(height);
    //     window.top.postMessage(JSON.stringify({ style: `border-style: none; bottom: 20px; ${messageList.personalisation.position === flexstart ? 'left: 30px;' : 'right: 30px;'}  position: fixed; z-index: 999999; width:${width}px; height:${height}px`, type: "close" }), "*");
    //   }
    // }, 200);
    document.body.classList.remove('no-scroll');
  };
  const handleChangeSetting = () => {
    // setsettingShow(true);
    dispatch(liveTrackerActions.setSettingStatus(true));
  };

  const handleNotificationSound = () => {
    setNotifySound(!notifySound)
    dispatch(messageActions.soundNotification(!messageList.soundNotification));
  }

  const downloadPDF = async (type) => {
    let bodyText = "";
    const test = [...messageList.messageList];
    // Start constructing bodyText for the HTML content
    // bodyText += '<div style="width: 100%;"><span style="font-size: 10px;white-space: nowrap;">' + `Overall Summary` + '</span>';
    bodyText += '<div>'
    bodyText += '<div style="color:black; width: 100%; text-align:center; margin: 0 0 24px; font-size: 24px;">Chat Transcript</div>'
    test.forEach((element) => {
      if (element?.agentText) {
        const agentMsg = isJsonString(element.agentText?.message?.message)
          ? JSON.parse(element.agentText?.message?.message)?.data
          : element.agentText?.message?.message;

        const name = element.agentText?.message?.sender?.Name !== undefined
          ? element.agentText?.message?.sender?.Name
          : messageList?.personalisationAll?.title;

        const _timestamp = moment(new Date(element.agentText?.message?.sendAt)).format("hh:mm:ss a");

        bodyText += '<div style="width: 98%;line-height: 17px; float:left;"><div style="width: 100%;line-height: 17px;color: #4B5563;font-size: 13px;float:left; background-color: #f4f4f4;padding: 8px;">' +
          '<p style="margin:0;width: 100%;float:left; font-weight:bold, padding-bottom: 12px; color: #000; font-size: 16px">' + name + '</p>' +
          '<p style="margin:0; float: left; width: 100%;font-size: 12px;line-height: 17px;color: #000000; padding: 6px 0 0; font-size: 13px;">' + agentMsg + ' </p></div><p style="float:left; width: 100%; text-align: right; font-size: 10px;padding-right: 10px;">' + _timestamp + '</p></div>';
      }

      if (element?.userText) {
        const _timestamp = moment(new Date(element.userText?.message?.sendAt)).format("hh:mm:ss a");
        const useMsg = isJsonString(element.userText?.message?.message)
          ? JSON.parse(element.userText?.message?.message)?.data
          : element.userText?.message?.message;

        bodyText += '<div style="width: 98%;line-height: 17px; float:left;"><div style="width: 100%;line-height: 17px;color: #4B5563;font-size: 13px;float:left;background-color: #fff; padding: 8px; ">' +
          '<p style="margin:0;width: 100%;float:left; font-weight:bold, padding-bottom: 12px; color: #000; font-size: 16px;">Visitor</p>' +
          '<p style="margin:0; width: 100%; float: left;font-size: 12px;line-height: 17px;color: #000000; padding: 6px 0 0; font-size: 13px;">' + useMsg + '</p></div><p style="float:left; width: 100%; text-align: right; padding-right: 10px;font-size: 10px;">' + _timestamp + '</p></div>';
      }
      bodyText += '</div></br>';
    });

    // Create a container element to hold the HTML content
    const container = document.createElement('div');
    container.innerHTML = bodyText;

    // Use the html2pdf library to generate and download the PDF
    if (type !== "share") {
      html2pdf().from(container).set({
        margin: [20, 20],
        filename: 'chattranscript.pdf',
        html2canvas: { scale: 2 },
        jsPDF: { unit: 'pt', format: 'a4', orientation: 'portrait' },
      }).save();
    }

    // For "share" type, send the generated PDF URL via email
    if (type === "share") {
      const pdfDataUri = await html2pdf().from(container).outputPdf('datauristring');
      const pdfBase64 = pdfDataUri.split(',')[1];
      const body = {
        id: uuidv4(),
        subject: 'Chat Transcript',
        from: 'support@worktual.com',
        to: mailEvt?.trim(),
        text: 'Kindly have a look at the generated chat transcript report',
        sessionId: uuidv4(),
        attachments: [
          {
            filename: 'chattranscript.pdf',
            content: pdfBase64,
            encoding: 'base64',
          },
        ],
      };
      try {
        const response = await fetch('https://ccaasapi.worktual.co.uk/emailService/v1/send_mail_lead', {
          method: 'POST',
          headers: {
            'Content-Type': 'application/json',
          },
          body: JSON.stringify(body),
        });
        if (!response.ok) {
          throw new Error(`HTTP error! Status: ${response.status}`);
        }
        console.log(response.status);
      } catch (error) {
        console.error('Error:', error);
      }
      setmailEvt('');
      // const data = {
      //   url: pdfUrl,
      //   mail: mailEvt
      // };
      //sendEmailTranscript(data);
    }
  }


  const customerHistory = async (domainId) => {
    const chatDurationCurrentId = sessionStorage.getItem('chatUniqIdentifier');
    const chatHistorySliceId = sessionStorage.getItem('ChatHistoryId')
    let resData = chatDurationCurrentId ?? 0
    dispatch(messageActions.setChatDurationId(parseInt(resData)));
    dispatch(messageActions?.updateChatHistoryId(parseInt(chatHistorySliceId)))
    try {
      const response = await fetch(`${config().CALLSERVICE_URL}/get_customer_history_by_uid/${resData}`);
      if (!response.ok) {
        throw new Error('Network response was not ok');
      }
      const data = await response.json();
      const rechatStatus = data?.customerValidateData[0];
      if (messageList.personalisationAll.preChartFormFlag !== 1) {
        data.messageList?.forEach((list) => {
          dispatch(messageActions.setMessageChat(list));
        });
      }
      if (rechatStatus.length > 0) {
        dispatch(messageActions.chatVisible(false));
      }
      if (rechatStatus.length > 0 && rechatStatus[0]?.chatSession === 0) {
        let agentStatus = {
          domainId: rechatStatus.domainId,
          ext: parseInt(rechatStatus.agentDetails),
        };
        dispatch(messageActions.agentDomainExt(agentStatus));
      }
    } catch (error) {
      console.error(error);
    }
  };

  const handletest = () => {
    document.getElementById("formButton").click();

  }
  const handleSecond = () => {
    document.getElementById("formButtonn").click();

  }
  let methodda = messageList?.paymentData?.method_data?.encoded_method_data

  useEffect(() => {
    let queryParams = new URLSearchParams(window.location.search);
    let status = queryParams.get("status");
    let reference = queryParams.get("reference");
    let payment = sessionStorage.getItem("payment");
    console.log(status, "statusstatusstatusstatus")
    if ((status === "Success" || status === "Failure") && payment) {
      handleChangeForm();

      setTimeoutMsg(setTimeout(() => {
        let msg = status === "Success" ? "Payment successful. Payment reference ID : " + reference : "Your payment failed, please try again later"
        newChatSendMsg("Text", msg, "OUT", 0);
        sessionStorage.setItem("payment", false);
        //sessionStorage.clear();
      }, 3000))

    }
  }, []);





  const iconStatus = sessionStorage.getItem('iconStatus');

  useEffect(() => {
    if (iconStatus) {
      let device = isMobileCheck();
      console.log("auto trigger device >>>>>>>", device);
      if (!device) {
        setTimeout(() => {
          dispatch(messageActions.formVisible(false));
          dispatch(messageActions.chatVisible(false));
        }, 2000)
      }
    }
  }, []);


  const getAddonMultiLanguage = (addonList) => {
    try {
      const languageData = JSON.parse(addonList?.autoResponse);
      const personalData = JSON.parse(addonList?.personalization);
      const addonResult = [
        {
          addon_feature_name: 'English (Default)',
          code: 'en',
          symbol: 'EN',
          lang_code: 'en-UK'
        }
      ];
      if (languageData?.addonMultiLangFullList?.length > 0 && personalData?.isCallUsIcon) {
        let resultArr = [...addonResult, ...languageData?.addonMultiLangFullList];
        dispatch(liveTrackerActions.setMultiLanguageList(resultArr))
      } else {
        dispatch(liveTrackerActions.setMultiLanguageList(addonResult))
      }
    } catch (err) {
      console.log(err)
    }
  }


  // const getAddonMultiLanguage = (companyId) => {
  //   try {
  //     let addOnPayload = {
  //       company_id: companyId
  //     }
  //     fetch(`${config().MYACCOUNT}/get_all_addon_list`, {
  //       method: 'POST',
  //       headers: {
  //         'Content-Type': 'application/json',
  //       },
  //       body: JSON.stringify(addOnPayload),
  //     })
  //       .then((res) => res.json())
  //       .then((res) => {
  //         const addonResult = [
  //           {
  //             addon_feature_name: 'English (Default)',
  //             code: 'en',
  //             symbol: 'EN',
  //             lang_code: 'en-UK'
  //           }
  //         ];
  //         res?.users?.[0]?.map((list) => {
  //           if (list?.addon_feature_name.includes('Multilingual') && list?.isPurchased === 1 && list?.addon_feature_name !== "Multilingual(English)") {
  //             addonResult?.push(list)
  //           }
  //         });
  //         dispatch(liveTrackerActions.setMultiLanguageList(addonResult))
  //       })
  //       .catch((error) => {
  //         console.log(error);
  //       });
  //   } catch (err) {
  //     console.log(err)
  //   }
  // }

  const handleMailSend = () => {
    const emailRegex = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
    if (emailRegex.test(mailEvt?.trim())) {
      downloadPDF('share');
      setmailerror(false);
      setismailShow(false);
    } else {
      setmailerror(true);
    }
  }
  const handleMailChange = (event) => {
    setmailEvt(event?.target.value)
  }
  const onChangeEmailtrans = () => {
    setismailShow(true);
    // setsettingShow(false);
    dispatch(liveTrackerActions.setSettingStatus(false));
  }
  const handleCloseEmail = () => {
    setismailShow(false);
  }

  const handlePurchaseFlow = () => {
    dispatch(messageActions.setPurchasePlanStatus(false));
    dispatch(messageActions.setPurchaseBackIconStatus(false))
    dispatch(messageActions.setPurchasePlanUrl(''));
  }

  const handleStartFlow = () => {
    dispatch(messageActions.multipleTemp(true))
  }

  const selectLangauage = (lan) => {
    dispatch(chatBuilderActions.setSelectedLanguage(lan?.lang_code));
    dispatch(liveTrackerActions.setSelectedTextLanguage(lan))
    dispatch(chatBuilderActions.setMultiLanguage(false))
  }

  const handleCloseSetting = () => {
    dispatch(chatBuilderActions.setMultiLanguage(false));
    dispatch(chatBuilderActions.setSelectedLanguage(''));
  }

  const handleCloseChat = () => {
    // setsettingShow(false);
    dispatch(liveTrackerActions.setSettingStatus(false));
  }

  const selectMultiTextLang = (list) => {
    setMultiLanguageForText(false)
    dispatch(liveTrackerActions.setSelectedTextLanguage(list))
  }


  useEffect(() => {
    const bodyClass = "bot-active";
    const checkAndSetBodyClass = () => {
      const botLayout = document.getElementById("bot-layout");
      if (botLayout) {
        document.body.classList.add(bodyClass);
      } else {
        document.body.classList.remove(bodyClass);
      }
    };

    checkAndSetBodyClass(); // Initial check
    const observer = new MutationObserver(checkAndSetBodyClass);

    // Observe changes in the document
    observer.observe(document.body, {
      childList: true,
      subtree: true,
    });

    return () => observer.disconnect(); // Cleanup on component unmount
  }, []);

  return (
    <>
      <form id="form3DSecures" method="post" action={messageList?.paymentData?.method_url} >
        <iframe id="hidden_iframe" name="hidden_iframe" style={{ display: 'none' }}></iframe>
        <input type="hidden" name="threeDSMethodData" value={methodda} />
        <input type="submit" id="formButton" style={{ display: "none" }} onClick={handletest} />
      </form>
      <form id="form3DSecuress" method="post" action={messageList?.secondTransactionData[0]?.request_url} >
        <iframe id="hidden_iframe" name="hidden_iframe" style={{ display: "none" }} ></iframe>
        <input type="hidden" name="creq" value={messageList?.secondTransactionData[0]?.encoded_data} />
        <input type="submit" id="formButtonn" style={{ display: "none" }} onClick={handleSecond} />

      </form>

      <>
        {
          !messageList.formVisible && liveTracker?.iconHideStatus && (<div className={`${messageList.personalisation.position === flexend ? chatbotlayoutcss : chatbotLayoutRightcss}`} id="bot-layout" ref={chatLayoutRef}>
            <div className="chatFade">
              <div className="chatHeader">
                {!messageList?.worktualAppointment?.isShow && !messageList?.purchasePlanData?.purchasePlanStatus && !messageList?.multipleTemp && messageList?.templateData?.length > 1 ?
                  <svg width="14" height="8" viewBox="0 0 14 8" fill="none" xmlns="http://www.w3.org/2000/svg" onClick={handleStartFlow} style={{ transform: "rotate(90deg)", width: "20px", height: "20px" }}>
                    <path d="M7.00342 7.47949C6.88493 7.47949 6.77555 7.45898 6.67529 7.41797C6.57503 7.3724 6.47933 7.30404 6.38818 7.21289L1.1792 1.88769C1.02425 1.73275 0.946778 1.5459 0.946778 1.32715C0.946778 1.17676 0.983236 1.04232 1.05615 0.923828C1.12451 0.800781 1.22022 0.702799 1.34326 0.629883C1.46175 0.556966 1.59391 0.520508 1.73975 0.520508C1.9585 0.520508 2.15218 0.604818 2.3208 0.773438L7.00342 5.57227L11.686 0.773438C11.8547 0.604818 12.0461 0.520508 12.2603 0.520508C12.4106 0.520508 12.5451 0.556967 12.6636 0.629883C12.7821 0.7028 12.8755 0.800782 12.9438 0.923829C13.0168 1.04232 13.0532 1.17676 13.0532 1.32715C13.0532 1.5459 12.978 1.73275 12.8276 1.8877L7.61865 7.21289C7.52751 7.30404 7.4318 7.3724 7.33154 7.41797C7.23128 7.45898 7.12191 7.47949 7.00342 7.47949Z" fill="#ffffff" />
                  </svg> : messageList?.purchaseBackIconStatus ?
                    <svg width="14" height="8" viewBox="0 0 14 8" fill="none" xmlns="http://www.w3.org/2000/svg" onClick={handlePurchaseFlow} style={{ transform: "rotate(90deg)", width: "20px", height: "20px" }}>
                      <path d="M7.00342 7.47949C6.88493 7.47949 6.77555 7.45898 6.67529 7.41797C6.57503 7.3724 6.47933 7.30404 6.38818 7.21289L1.1792 1.88769C1.02425 1.73275 0.946778 1.5459 0.946778 1.32715C0.946778 1.17676 0.983236 1.04232 1.05615 0.923828C1.12451 0.800781 1.22022 0.702799 1.34326 0.629883C1.46175 0.556966 1.59391 0.520508 1.73975 0.520508C1.9585 0.520508 2.15218 0.604818 2.3208 0.773438L7.00342 5.57227L11.686 0.773438C11.8547 0.604818 12.0461 0.520508 12.2603 0.520508C12.4106 0.520508 12.5451 0.556967 12.6636 0.629883C12.7821 0.7028 12.8755 0.800782 12.9438 0.923829C13.0168 1.04232 13.0532 1.17676 13.0532 1.32715C13.0532 1.5459 12.978 1.73275 12.8276 1.8877L7.61865 7.21289C7.52751 7.30404 7.4318 7.3724 7.33154 7.41797C7.23128 7.45898 7.12191 7.47949 7.00342 7.47949Z" fill="#ffffff" />
                    </svg> :
                    <></>
                }
                <div className="headerimg">
                  <Tooltip placement="top" >
                    <img src={messageList.iconLogo} alt="" />
                  </Tooltip>
                </div>
                <div className="chatHeaderul">
                  <div className="chatHeaderli">
                    <div className="chatHeaderh2">{messageList.personalisationAll.title}</div>
                  </div>
                  <div className="chatHeaderli">
                    <div className="chatHeaderp">{messageList.personalisationAll.description}</div>
                  </div>
                </div>
                {/* <div className="chatHeaderpMul" onClick={() => { setMultiLanguageForText(true) }}>{liveTracker?.selectedTextLanguage?.symbol}</div> */}
                <div className="rightcls">
                  {!isRNCheckHeader &&<svg xmlns="http://www.w3.org/2000/svg" width="18" height="2" viewBox="0 0 18 2" onClick={handleChangeMinimize}>
                    <path d="M16.2647 1.75H1.73438C1.32028 1.75 0.984375 1.4141 0.984375 1C0.984375 0.5859 1.32028 0.25 1.73438 0.25H16.2647C16.6788 0.25 17.0147 0.5859 17.0147 1C17.0147 1.4141 16.6788 1.75 16.2647 1.75Z" />
                  </svg>}
                  {!messageList.chatVisible && !messageList.isConsentForm &&
                    <svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16" onClick={handleChangeSetting}>
                      <path d="M9.20171 8.00004L15.0567 2.14454C15.3887 1.81344 15.3887 1.27444 15.0567 0.943336C14.7247 0.611336 14.1856 0.611336 13.8555 0.943336L8.00051 6.79874L2.14551 0.943336C1.81541 0.611336 1.27441 0.611336 0.944312 0.943336C0.612312 1.27444 0.612312 1.81344 0.944312 2.14454L6.79931 8.00004L0.944312 13.8555C0.612312 14.1866 0.612312 14.7256 0.944312 15.0567C1.11031 15.2227 1.32711 15.3057 1.54491 15.3057C1.76271 15.3057 1.98041 15.2227 2.14551 15.0567L8.00051 9.20134L13.8555 15.0567C14.0205 15.2227 14.2383 15.3057 14.4561 15.3057C14.6739 15.3057 14.8907 15.2227 15.0567 15.0567C15.3887 14.7256 15.3887 14.1866 15.0567 13.8555L9.20171 8.00004Z" />
                    </svg>}
                </div>
              </div>
              <div className={messageList.personalisation.position === "flex-start" ? "chatviewLeft" : "chatviewRight"}> </div>
              {messageList.isConsentForm ? <ConsentForm /> : messageList.chatVisible ? <ChatBot /> : <ChatScreen />}
            </div>


            {(liveTracker?.settingStatus && !builderList?.feedBackFormStatus) && <div className="endchats">
              <div className="endchatsinner">
                <h3>Setting <svg xmlns="http://www.w3.org/2000/svg" width="14" height="14" viewBox="0 0 14 14" fill="none" onClick={handleCloseChat}>
                  <path d="M8.00078 7.00215L12.8799 2.12256C13.1566 1.84665 13.1566 1.39748 12.8799 1.12156C12.6033 0.844896 12.154 0.844896 11.8789 1.12156L6.99978 6.00106L2.12061 1.12156C1.84553 0.844896 1.39469 0.844896 1.11961 1.12156C0.842943 1.39748 0.842943 1.84665 1.11961 2.12256L5.99878 7.00215L1.11961 11.8817C0.842943 12.1576 0.842943 12.6068 1.11961 12.8827C1.25794 13.0211 1.43861 13.0902 1.62011 13.0902C1.80161 13.0902 1.98303 13.0211 2.12061 12.8827L6.99978 8.00323L11.8789 12.8827C12.0164 13.0211 12.1979 13.0902 12.3794 13.0902C12.5609 13.0902 12.7416 13.0211 12.8799 12.8827C13.1566 12.6068 13.1566 12.1576 12.8799 11.8817L8.00078 7.00215Z" fill="#505152" />
                </svg></h3>
                <div className="endchatslist">

                  {messageList.personalisation?.notificationSound == undefined || messageList.personalisation?.notificationSound == "None" ? "" :
                    !notifySound ?
                      <>
                        <p onClick={handleNotificationSound}><svg xmlns="http://www.w3.org/2000/svg" width="20" height="16" viewBox="0 0 20 16" fill="none">
                          <path d="M10.0166 0.476563C9.67264 0.476563 9.32094 0.596162 9.02374 0.863063L6.35874 3.25536C5.95394 3.61886 5.42914 3.81986 4.88514 3.81986H3.26294C1.60604 3.81986 0.262939 5.16316 0.262939 6.81986V9.17996C0.262939 10.837 1.60604 12.18 3.26294 12.18H4.88514C5.42924 12.18 5.95394 12.3809 6.35874 12.7445L9.02374 15.1368C9.32114 15.4039 9.67254 15.5235 10.0166 15.5235C10.7898 15.5235 11.5258 14.9185 11.5258 14.0206V1.97946C11.5258 1.08126 10.79 0.476263 10.0166 0.476563ZM10.0256 14.0205L7.36084 11.6284C6.67984 11.0168 5.80054 10.6799 4.88504 10.6799H3.26284C2.43574 10.6799 1.76284 10.007 1.76284 9.17986V6.81976C1.76284 5.99266 2.43574 5.31976 3.26284 5.31976H4.88504C5.80054 5.31976 6.67994 4.98286 7.36074 4.37156L10.0257 1.98016L10.0256 14.0205Z" fill="#757676" />
                          <path d="M14.7876 4.30566C14.4922 4.01756 14.0176 4.02046 13.7271 4.31636C13.4371 4.61226 13.4419 5.08786 13.7378 5.37786C14.4463 6.07126 14.8384 6.99696 14.8428 7.98336C14.8477 8.97746 14.4575 9.91696 13.7451 10.627C13.4521 10.92 13.4512 11.3946 13.7441 11.6875C13.8906 11.835 14.0825 11.9082 14.2749 11.9082C14.4663 11.9082 14.6582 11.835 14.8047 11.6894C15.8027 10.6933 16.3491 9.37486 16.3428 7.97646C16.3369 6.58586 15.7842 5.28316 14.7876 4.30566Z" fill="#757676" />
                          <path d="M17.2944 2.20016C16.9985 1.91106 16.5239 1.91496 16.2339 2.21186C15.9439 2.50776 15.9487 2.98236 16.2446 3.27236C17.5215 4.52236 18.229 6.19126 18.2373 7.96966C18.2451 9.76166 17.543 11.4531 16.2602 12.7324C15.9672 13.0254 15.9663 13.5 16.2587 13.7929C16.4057 13.9404 16.5976 14.0136 16.7899 14.0136C16.9813 14.0136 17.1732 13.9404 17.3197 13.7948C18.8881 12.2303 19.7469 10.1591 19.7367 7.96276C19.7269 5.78016 18.8598 3.73436 17.2944 2.20016Z" fill="#757676" />
                        </svg>Turn on sound</p></> :
                      <p onClick={handleNotificationSound}><svg xmlns="http://www.w3.org/2000/svg" width="20" height="16" viewBox="0 0 20 16" fill="none">
                        <path d="M10.052 0.476563C9.70804 0.476563 9.35634 0.596162 9.05914 0.863063L6.39414 3.25536C5.98934 3.61886 5.46454 3.81986 4.92054 3.81986H3.29834C1.64144 3.81986 0.29834 5.16316 0.29834 6.81986V9.17996C0.29834 10.837 1.64144 12.18 3.29834 12.18H4.92054C5.46464 12.18 5.98934 12.3809 6.39414 12.7445L9.05914 15.1368C9.35654 15.4039 9.70794 15.5235 10.052 15.5235C10.8252 15.5235 11.5612 14.9185 11.5612 14.0206V1.97946C11.5612 1.08126 10.8254 0.476263 10.052 0.476563ZM10.061 14.0205L7.39624 11.6284C6.71524 11.0168 5.83594 10.6799 4.92044 10.6799H3.29824C2.47114 10.6799 1.79824 10.007 1.79824 9.17986V6.81976C1.79824 5.99266 2.47114 5.31976 3.29824 5.31976H4.92044C5.83594 5.31976 6.71534 4.98286 7.39614 4.37156L10.0611 1.98016L10.061 14.0205Z" fill="#757676" />
                        <path d="M17.6589 8.00026L19.4817 6.17746C19.7747 5.88446 19.7747 5.40986 19.4817 5.11696C19.1887 4.82406 18.7141 4.82396 18.4212 5.11696L16.5984 6.93976L14.7756 5.11696C14.4826 4.82396 14.008 4.82396 13.7151 5.11696C13.4222 5.40996 13.4221 5.88456 13.7151 6.17746L15.5379 8.00026L13.7151 9.82306C13.4221 10.1161 13.4221 10.5907 13.7151 10.8836C13.8616 11.0301 14.0535 11.1033 14.2454 11.1033C14.4373 11.1033 14.6292 11.0301 14.7757 10.8836L16.5985 9.06076L18.4213 10.8836C18.5678 11.0301 18.7597 11.1033 18.9516 11.1033C19.1435 11.1033 19.3354 11.0301 19.4819 10.8836C19.7749 10.5906 19.7749 10.116 19.4819 9.82306L17.6589 8.00026Z" fill="#757676" />
                      </svg>Turn off sound</p>
                  }

                  {messageList.personalisation.emailLink && messageList.personalisation.emailLink !== undefined && (
                    <p onClick={onChangeEmailtrans}><svg xmlns="http://www.w3.org/2000/svg" width="20" height="16" viewBox="0 0 20 16" fill="none">
                      <path d="M19.7259 1.76C19.5159 1.29 19.1859 0.89 18.7759 0.59C18.2759 0.22 17.6659 0 16.9959 0H3.00586C2.31586 0 1.67586 0.24 1.17586 0.63C0.775859 0.93 0.455859 1.34 0.245859 1.81C0.0958594 2.18 0.00585938 2.58 0.00585938 3V13C0.00585938 14.66 1.34586 16 3.00586 16H16.9959C18.6559 16 19.9959 14.66 19.9959 13V3C19.9959 2.56 19.8959 2.14 19.7259 1.76ZM3.00586 1.5H16.9959C17.0892 1.5 17.1766 1.5267 17.2663 1.5437L10.4659 5.84C10.1859 6.0201 9.82586 6.0201 9.54586 5.85L2.66246 1.5572C2.77536 1.5301 2.88816 1.5 3.00586 1.5ZM18.4959 13C18.4959 13.83 17.8259 14.5 16.9959 14.5H3.00586C2.17586 14.5 1.50586 13.83 1.50586 13V3C1.50586 2.8818 1.52676 2.756 1.55776 2.6296L8.74576 7.12C9.12576 7.36 9.55576 7.47 9.99576 7.47C10.4358 7.47 10.8758 7.35 11.2658 7.11L18.4257 2.5822C18.4668 2.7215 18.4958 2.8608 18.4958 3L18.4959 13Z" fill="#757676" />
                    </svg>Email transcript</p>
                  )}
                  {messageList.personalisation.downloadStatus && messageList.personalisation.downloadStatus !== undefined && (<p onClick={() => downloadPDF('download')}><svg xmlns="http://www.w3.org/2000/svg" width="14" height="18" viewBox="0 0 14 18" fill="none">
                    <path d="M5.39361 13.8428C5.82081 14.2793 6.39161 14.5196 7.00051 14.5196C7.60991 14.5196 8.18071 14.2794 8.60791 13.8428L12.8647 9.49708C13.1547 9.20117 13.1499 8.72657 12.854 8.43657C12.5581 8.14747 12.0835 8.15137 11.7935 8.44827L7.75681 12.5691L7.75051 1.23438C7.75051 0.820275 7.41461 0.484375 7.00051 0.484375C6.58641 0.484375 6.25051 0.820275 6.25051 1.23438L6.25681 12.5816L2.20751 8.44827C1.91751 8.15237 1.44291 8.14847 1.14701 8.43657C0.851114 8.72657 0.846214 9.20117 1.13631 9.49708L5.39361 13.8428Z" fill="#757676" />
                    <path d="M12.3292 16.0157L1.67151 16.0216C1.25741 16.0216 0.922014 16.3575 0.922014 16.7716C0.922014 17.1857 1.25791 17.5216 1.67201 17.5216L12.3297 17.5157C12.7438 17.5157 13.0792 17.1798 13.0792 16.7657C13.0792 16.3516 12.7433 16.0157 12.3292 16.0157Z" fill="#757676" />
                  </svg>Download transcript</p>)}
                </div>
                <div className="endchatsbtn" onClick={handleChangeEndClose}>End Chat</div>
              </div>
            </div>}
            {ismailShow && <div className="endchats">
              <div className="endchatsinner">
                <h3><div>
                  <svg xmlns="http://www.w3.org/2000/svg" width="6" height="12" viewBox="0 0 6 12" fill="none" onClick={handleCloseEmail}>
                    <path d="M5.15326 11.667C4.96186 11.667 4.76996 11.5938 4.62346 11.4482L0.758756 7.59469C0.332956 7.16989 0.098157 6.60449 0.097657 6.00189C0.097157 5.39929 0.332056 4.83389 0.758756 4.40719L4.62396 0.554688C4.91696 0.262688 5.39206 0.262688 5.68446 0.556688C5.97696 0.849688 5.97596 1.32429 5.68246 1.61719L1.81776 5.46969C1.67566 5.61129 1.59756 5.80079 1.59756 6.00189C1.59756 6.20209 1.67566 6.39059 1.81776 6.53219L5.68296 10.3857C5.97596 10.6787 5.97686 11.1533 5.68446 11.4462C5.53796 11.5937 5.34556 11.667 5.15326 11.667Z" fill="#757676" />
                  </svg>&nbsp;&nbsp;Email Transcript</div><svg xmlns="http://www.w3.org/2000/svg" width="14" height="14" viewBox="0 0 14 14" fill="none" onClick={handleCloseEmail}>
                    <path d="M8.00078 7.00215L12.8799 2.12256C13.1566 1.84665 13.1566 1.39748 12.8799 1.12156C12.6033 0.844896 12.154 0.844896 11.8789 1.12156L6.99978 6.00106L2.12061 1.12156C1.84553 0.844896 1.39469 0.844896 1.11961 1.12156C0.842943 1.39748 0.842943 1.84665 1.11961 2.12256L5.99878 7.00215L1.11961 11.8817C0.842943 12.1576 0.842943 12.6068 1.11961 12.8827C1.25794 13.0211 1.43861 13.0902 1.62011 13.0902C1.80161 13.0902 1.98303 13.0211 2.12061 12.8827L6.99978 8.00323L11.8789 12.8827C12.0164 13.0211 12.1979 13.0902 12.3794 13.0902C12.5609 13.0902 12.7416 13.0211 12.8799 12.8827C13.1566 12.6068 13.1566 12.1576 12.8799 11.8817L8.00078 7.00215Z" fill="#505152" />
                  </svg></h3>
                <div className="endchatslist">
                  <p className="emailinputs">

                    <Input placeholder="Enter your mail" className="formInputHeight" onChange={handleMailChange} />
                    {mailerror && <span>{mailEvt?.trim() ? "Enter a valid mail" : "Enter your mail"}</span>}
                  </p>

                  {/* <div class="ant-form-item-explain-error">hello</div> */}
                </div>
                <div className="endchatsbtn endchatsgreen" onClick={handleMailSend}>Send</div>
              </div>
            </div>}


            {/* Language Design For mic*/}
            {builderList?.multiLanguage && <div className="endchats">
              <div className="endchatsinner endlanguages">
                <h3>Choose Mic Language <svg xmlns="http://www.w3.org/2000/svg" width="14" height="14" viewBox="0 0 14 14" fill="none" onClick={handleCloseSetting}>
                  <path d="M8.00078 7.00215L12.8799 2.12256C13.1566 1.84665 13.1566 1.39748 12.8799 1.12156C12.6033 0.844896 12.154 0.844896 11.8789 1.12156L6.99978 6.00106L2.12061 1.12156C1.84553 0.844896 1.39469 0.844896 1.11961 1.12156C0.842943 1.39748 0.842943 1.84665 1.11961 2.12256L5.99878 7.00215L1.11961 11.8817C0.842943 12.1576 0.842943 12.6068 1.11961 12.8827C1.25794 13.0211 1.43861 13.0902 1.62011 13.0902C1.80161 13.0902 1.98303 13.0211 2.12061 12.8827L6.99978 8.00323L11.8789 12.8827C12.0164 13.0211 12.1979 13.0902 12.3794 13.0902C12.5609 13.0902 12.7416 13.0211 12.8799 12.8827C13.1566 12.6068 13.1566 12.1576 12.8799 11.8817L8.00078 7.00215Z" fill="#505152" />
                </svg></h3>
                <div className="endchatli">
                  <ul>
                    {/* <li onClick={() => selectLangauage('en-UK')}>English (Default)</li>
                    <li onClick={() => selectLangauage('fr-FR')}>French</li>
                    <li onClick={() => selectLangauage('es-ES')}>Spanish</li>
                    <li onClick={() => selectLangauage('ar-SA')}>Arabic</li>
                    <li onClick={() => selectLangauage('ta-IN')}>Tamil</li>
                    <li onClick={() => selectLangauage('ml-IN')}>Malayalam</li>
                    <li onClick={() => selectLangauage('te-IN')}>Telugu</li>
                    <li onClick={() => selectLangauage('hi-IN')}>Hindi</li> */}
                    {liveTracker?.multiLanguageList?.length > 0 && liveTracker?.multiLanguageList?.map((list) => {
                      return (
                        <li onClick={() => selectLangauage(list)}>
                          {(() => {
                            const match = list?.addon_feature_name.match(/Multilingual\((.*?)\)/);
                            return match ? match[1] : list?.addon_feature_name;
                          })()}
                        </li>
                      );
                    })}
                  </ul>
                </div>
              </div>
            </div>}
            {/* Language Design For Text*/}
            {multiLanguageForText && <div className="endchats">
              <div className="endchatsinner endlanguages">
                <h3>Choose Text Language <svg xmlns="http://www.w3.org/2000/svg" width="14" height="14" viewBox="0 0 14 14" fill="none" onClick={() => setMultiLanguageForText(false)}>
                  <path d="M8.00078 7.00215L12.8799 2.12256C13.1566 1.84665 13.1566 1.39748 12.8799 1.12156C12.6033 0.844896 12.154 0.844896 11.8789 1.12156L6.99978 6.00106L2.12061 1.12156C1.84553 0.844896 1.39469 0.844896 1.11961 1.12156C0.842943 1.39748 0.842943 1.84665 1.11961 2.12256L5.99878 7.00215L1.11961 11.8817C0.842943 12.1576 0.842943 12.6068 1.11961 12.8827C1.25794 13.0211 1.43861 13.0902 1.62011 13.0902C1.80161 13.0902 1.98303 13.0211 2.12061 12.8827L6.99978 8.00323L11.8789 12.8827C12.0164 13.0211 12.1979 13.0902 12.3794 13.0902C12.5609 13.0902 12.7416 13.0211 12.8799 12.8827C13.1566 12.6068 13.1566 12.1576 12.8799 11.8817L8.00078 7.00215Z" fill="#505152" />
                </svg></h3>
                <div className="endchatli">
                  <ul>
                    {liveTracker?.multiLanguageList?.length > 0 && liveTracker?.multiLanguageList?.map((list) => {
                      return (<li onClick={() => selectMultiTextLang(list)}>{list?.addon_feature_name}</li>)
                    })
                    }
                  </ul>
                </div>
              </div>
            </div>}


          </div>)
        }


        <div className={`${messageList.personalisation.position === flexstart ? chatpreviewleftview : chatPreviewRigtCss}`} id="wortual_chat_icon" style={{ display: !triggerStyle ? "none" : 'flex' }}>
          {/* messageList.formVisible && close && */}
          {!liveTracker?.iconHideStatus && close && Object.keys(messageList?.personalisationAll).length > 0 && msgShow !== undefined && msgShow !== '' && (
            <div className="welcomemessage" id="welcome-message" ref={divRef}>
              <img src={closemsgs} onClick={handleClose} alt="close"/>
              <p onClick={handleChangeForm} >{msgShow}</p>
            </div>
          )}
          {!liveTracker?.iconHideStatus &&
            <div className={`txtchatbot ${messageList.personalisation.position === flexend ? chatIconCss : chatIConLeftRigtCss} ${liveTracker?.mobileIconStatus ? chatPreviewMobileHide : chatPreviewMobileVisible}`}
              onClick={handleChangeForm} ref={chatIconRef} id="chat_icon">
              {
                messageList?.widgetIconLogo
                  ? <img src={messageList.widgetIconLogo} alt="logo" />
                  : <svg xmlns="http://www.w3.org/2000/svg" width="18" height="18" viewBox="0 0 18 18" fill="none">
                    <path d="M12.6134 3.11006C11.2369 1.63446 9.32997 0.789062 7.29887 0.789062C3.28227 0.789062 0.0136719 4.05766 0.0136719 8.07426V13.7091C0.0136719 14.6163 0.750972 15.3536 1.65817 15.3536H6.04757C7.31927 16.5031 8.99827 17.21 10.8437 17.21H16.3798C17.2773 17.21 18.0077 16.4795 18.0077 15.5821V10.0479C18.0077 6.70796 15.7091 3.90256 12.6134 3.11006ZM1.51377 13.709V8.07416C1.51377 4.88376 4.10847 2.28896 7.29897 2.28896C8.23747 2.28896 9.14127 2.52096 9.95087 2.94006C6.41967 3.38016 3.67697 6.39456 3.67697 10.0439C3.67697 11.4445 4.08687 12.7484 4.78437 13.8535H1.65837C1.57827 13.8535 1.51377 13.789 1.51377 13.709ZM8.43027 10.8873C7.96497 10.8873 7.58777 10.51 7.58777 10.0446C7.58777 9.57916 7.96497 9.20186 8.43027 9.20186C8.89587 9.20186 9.27307 9.57916 9.27307 10.0446C9.27307 10.51 8.89587 10.8873 8.43027 10.8873ZM10.8427 10.8873C10.3774 10.8873 10.0002 10.51 10.0002 10.0446C10.0002 9.57916 10.3774 9.20186 10.8427 9.20186C11.3083 9.20186 11.6855 9.57916 11.6855 10.0446C11.6855 10.51 11.3083 10.8873 10.8427 10.8873ZM13.2551 10.8873C12.7898 10.8873 12.4123 10.51 12.4123 10.0446C12.4123 9.57916 12.7897 9.20186 13.2551 9.20186C13.7205 9.20186 14.0979 9.57916 14.0979 10.0446C14.0979 10.51 13.7205 10.8873 13.2551 10.8873Z" fill="white" />
                  </svg>
              }
              {messageList?.personalisation?.launcherText ? <p>{messageList?.personalisation?.launcherText}</p> : <></>}
            </div>}

        </div>
      </>

    </>
  );
}
export default App;
