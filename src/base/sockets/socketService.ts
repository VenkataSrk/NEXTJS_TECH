import jwt_decode from "jwt-decode";
import { ClearStorage, GetItemFromStorage, SetItemInStorage } from "../custom_hooks/useStorage";
import { LocalStorage } from "../custom_hooks/localStorageKeys";
// import { CallService } from "../service/call_service";
import { config } from "../constant";
import socketIOClient from "socket.io-client";
import { Cookies } from "../cookies";
import { Cookie, getCurrentUserCallerID, isJSON } from "../utils";
import store from "../../store/redux-store/root_store";
import { CallPopUpAgentActions } from "../../store/slice/callSlice/call_popup_agent";
import { getAgentStatusDetails, getCallSessionDetails } from "../service/core_serviceApi";
import { AgentStateAction } from "../../store/slice/agentStatusSlice/agent_state";
import presenceService from "./presence";
import { agentstatusListAction } from "../../store/slice/agentStatusSlice/agent_status";
import { agentTrackerActions } from "../../store/slice/trackerStatus_slice/agentTrackerslice";
import { queueTrackerActions } from "../../store/slice/trackerStatus_slice/queueTrackerslice";
import { ChatActions } from "../../store/slice/chat_slice/chat_slice";

export default class socketService {
  public static io: any;
  static _instance: any;
  private static hist: any;
  constructor(token: any, navigate: any, dispatch: any) {
    let userData: any = jwt_decode(token);
    const loginUserCcaasDetail = GetItemFromStorage(LocalStorage.USER_DETAILS);
    // const { callBackTimerFunc, SupervisorEndCall } = CallService(dispatch, navigate);
    // const { createCallConnection, CallOutboundCampaignFunction, dialPadChanges } = CallService(dispatch, navigate);

    if (socketService.io) {
      socketService.io.disconnect();
    }

    socketService.io = socketIOClient(config().BASE_URL, {
    // socketService.io = socketIOClient("http://localhost:5008", {
      path: "/crm-service",
      auth: { token: token, id: `${userData?.domainId}_${userData?.ext}` },
      transports: ["websocket", "polling"],
      forceNew: true,
      reconnectionAttempts: 5,
      timeout: 60000,
    });

    socketService.io.on("connect", () => {
      console.log("socket", `${userData?.domainId}_${userData?.ext}`);
      console.log("socket connected");
      const data = { domainId: userData?.domainId, ext: userData?.ext };
      socketService.io.emit("agent_ext_socket_id_mapping", data);
    });

    socketService.io.on("disconnect", (reason: any) => {
      Cookies.clearKey('allowMultipleLogin');
      console.warn("socket disconnect", userData?.domainId, userData?.ext, '>>>>', reason);
    });

    socketService.io.on("connect_error", (msg: any) => {
      if (msg && msg?.message && isJSON(msg?.message)) {
        const parsedMessage: any = JSON.parse(msg?.message);
        if (parsedMessage?.code === 403) {
          console.warn("CRM socket unauthorized (403). Preserving login session and retrying socket later.");
          socketService.destroy();
        }
      }
    });

    socketService.io.on("clientInfo", (message: any) => { });

    socketService.io.on("AGENT_REGISTER", (data: any) => {
      SetItemInStorage("register", "1");
      dispatch(CallPopUpAgentActions.setagentRegisterDetails(data));
      // createCallConnection();
    });

    // socketService.io.on("getAgentTracker", (data: any) => {
    //   if (loginUserCcaasDetail?.roleid == ROLES?.ADMIN_ROLE_ID || loginUserCcaasDetail?.roleid == ROLES?.SUPERVISOR_ROLE_ID) {
    //     getLiveSessions(dispatch);
    //     dispatch(ChatActions.setAllViewAutoRefresh());
    //   }
    // });

    socketService.io.on("updateAgentRoutingProfile", (data: any) => {
      const userDetail = GetItemFromStorage('USER_DETAILS');
      if (+(data?.ext) === +(userDetail?.ext) && +(data?.domainId) === +(userDetail?.domainId)) {
        console.log('if');
        const loginData = { ...userDetail, routing_profile_id: data?.id };
        SetItemInStorage('USER_DETAILS', loginData);
      }
    });

    socketService.io.on("presenceStatusrestriction", async (data: any) => {
      dispatch(getAgentStatusDetails());
    });

    // socketService.io.on("waitingQueueUpdate", (data: any) => {
    //   const parseData = data ? JSON.parse(data) : null;
    //   const campaign = store.getState().outboundcampaign;
    //   console.log("waitingQueueUpdate", parseData, "!campaign?.userCallInfo", !campaign?.userCallInfo)
    //   if (!campaign?.userCallInfo) {
    //     try {
    //       CallServices.get("/get_all_queue_wait_time").then((resp: any) => {
    //         dispatch(
    //           queueTrackerActions.queueTrackerData({
    //             queueTrackerData: resp.data.getAllQueueList,
    //           })
    //         );
    //       });
    //     } catch (error) {
    //       console.log("get_all_queue_wait_time error ", error)
    //     }
    //     dispatch(
    //       queueTrackerActions.updateQueueData({
    //         qid: parseData?.qid,
    //         queueList: parseData?.queueList,
    //         queueName: parseData?.queueName,

    //       })
    //     );
    //     getTime(dispatch)
    //   }
    // });

    socketService.io.on("outboundcallSession", (message: any) => {
      let { queueName, queueDetails }: any = store.getState().callPopUpDetails;
      dispatch(CallPopUpAgentActions.setSessionId(message?.session_id?.trim?.()));
      dispatch(CallPopUpAgentActions.setNumber(message.ddi));
      dispatch(CallPopUpAgentActions.setInitialTime());
      dispatch(CallPopUpAgentActions.cleartimer());
      const currentCallerDetails :any= getCurrentUserCallerID();
      if ((message?.device_type === "CCAAS:BROWSER") && message?.pd === 0) {
        socketService.io.emit("init_transfer", {
          session_id: message?.session_id,
          qid: `${queueDetails?.qid || currentCallerDetails?.key || 0}`,
          req_type: 'EXTERNAL_QUEUE',
          OutboundCallerId: queueDetails?.OutboundCallerId || currentCallerDetails?.OutboundCallerId || '',
          outBoundNumber: queueDetails?.outBoundNumber || currentCallerDetails?.outBoundNumber || ''
        })

        // getCallSessionDetails(message).then((res: any) => {
        //   if (res?.statusCode === 200 && Array.isArray(res?.result) && res?.result?.length > 0) {
        //     let chatSessionData: any = res?.result?.[0];
        //     chatSessionData.Number =
        //       chatSessionData?.callType === "outbound"
        //         ? chatSessionData?.calledNumber
        //         : chatSessionData?.customerDetails;
        //     chatSessionData.showState = true;
        //     chatSessionData.queueName = queueName ?? "Default Queue";
        //     chatSessionData.outboundFromNumber = queueDetails?.outBoundNumber ?? "0";
        //     chatSessionData.queueId = queueDetails?.qid;
        //     dispatch(ChatActions.currentChatSession(chatSessionData));
        //   }
        // });
      }
    });
    // socketService.io.on("callMergeedSuccess", async (message: any) => {
    //   let userData: any = jwt_decode(token);
    //   let setuserAgent = message.Agent;
    //   let AgentWhole: any = [];
    //   let customerContactDetails: any;
    //   const { incomingcallDetails } = store.getState().chatReducer;

    //   let { TransferFrom, transferTo }: any = store.getState().callPopUpDetails;
    //   const userDetails = GetItemFromStorage(LocalStorage.USER_DETAILS);
    //   const isValidAgent = message.agentInfo?.some((agent: any) => {
    //     console.log("callMergeedSuccess", "isValidAgent", userDetails?.ext.toString() === agent?.ext.toString(), agent?.ext.toString(), userDetails?.ext.toString())
    //     return (

    //       userDetails?.ext.toString() === agent?.ext.toString() && agent?.device_type === "CCAAS:BROWSER");
    //   });
    //   console.log(isValidAgent, "isValidAgent")
    //   if (isValidAgent) {

    //     if (+(message?.Agent[message?.Agent.length - 1]) === loginUserCcaasDetail?.ext) {
    //       let details: any = await showDetials({ sessionId: message?.session_id.trim(), channelType: "Call" })
    //       if (details?.data?.statusCode === 200) {
    //         const message_1 = JSON.parse(details?.data?.getEmailListRes[0]?.Message ?? '[]')
    //         dispatch(ChatActions.agentViewWorkSpaceHistory_WB(Array.isArray(message_1) ? message_1 : []));
    //       }
    //     }
    //     await dispatch(
    //       getNecessaryDetailswithsession({
    //         contact: message.customer,
    //         sessionId: message.session_id,
    //       })
    //     )

    //       .unwrap()
    //       .then((Res: any) => {
    //         if (Res?.contact?.data?.statusCode === 200) {
    //           customerContactDetails = Res?.contact.data?.getContactNumberRes[0];

    //           if (Res.sessionDetail.data.statusCode === 200) {
    //             let chatSessionData: any = Res.sessionDetail.data.result[0];
    //             chatSessionData.Number = chatSessionData.customerDetails;
    //             chatSessionData.showState = true;
    //             chatSessionData.queueId = incomingcallDetails?.qid;
    //             // getContactDetailsWhilegettingCall(chatSessionData.Number).then((res: any) => {
    //             //   if (res?.data?.statusCode === 200) {
    //             chatSessionData.calledPersonDetails = customerContactDetails;
    //             chatSessionData.calledPersonDetails.tags =
    //               typeof customerContactDetails?.tags === "string"
    //                 ? JSON.parse(
    //                   customerContactDetails?.tags?.trim().length > 0
    //                     ? customerContactDetails?.tags
    //                     : "null"
    //                 ) ?? []
    //                 : customerContactDetails?.tags;

    //             if (message?.Agent?.[0] !== userDetails?.ext?.toString()) {
    //               try {
    //                 const coid = Res.contact?.data?.getContactNumberRes?.[0]?.coid
    //                 getPriviousChannelSummary(dispatch, { coid, currentSessionId: message.session_id?.trim() })
    //               } catch (error) {
    //                 console.log("getPriviousChannelSummary", error)
    //               }
    //             }
    //             dispatch(ChatActions.chatSessionList(chatSessionData));
    //             dispatch(ChatActions.currentChatSession(chatSessionData));
    //             dispatch(ChatActions.setCallSessionId(chatSessionData?.sessionId ?? ""));
    //             dispatch(CallPopUpAgentActions.setTransferTyper());
    //             console.log(" TransferFrom customerMsg?.length > 0 merge", TransferFrom?.transferDataDetails, "TransferFrom?.transferDataDetails)", TransferFrom)
    //             const merge = { ...TransferFrom?.transferDataDetails, message: `Merge call from ${TransferFrom?.fromExt}` }
    //             dispatch(ChatActions.agentWorkSpaceTansferHistory(merge));
    //             dispatch(CallPopUpAgentActions.setTransferFrom([]))
    //             //navigate("/aiagentworkspace");
    //             // });
    //           }


    //           let customerData = {
    //             from: message.customer,
    //             contactDetails: customerContactDetails,
    //             to: message.Agent[0],
    //             muteState: false,
    //             holdState: false,
    //             medium: false,
    //             bound: "IN",
    //             agentType: "CUSTOMER",
    //             isCustomer: true,
    //             isLogin: false,
    //             isPrimary: message.Agent[0],
    //           };
    //           AgentWhole[0] = customerData;

    //           let primaryAgentDetail: any = {
    //             from: message.customer,
    //             to: message.Agent[0],
    //             contactDetails: getUser(message.Agent[0]) ?? {},
    //             muteState: false,
    //             holdState: false,
    //             medium: false,
    //             bound: "IN",
    //             agentType: "PRIMARY",
    //             isPrimary: message.Agent[0],
    //             isCustomer: false,
    //             isLogin:
    //               userData.ext.toString() === message.Agent[0] ? true : false,
    //           };

    //           AgentWhole.push(primaryAgentDetail);
    //           let SecondaryAgentDetails: any = [];
    //           let seccondary: any;

    //           seccondary = message.Agent.filter((agent: any) => {
    //             return agent !== primaryAgentDetail.to;
    //           });

    //           seccondary.map((item: any) => {
    //             let contacts = {
    //               from: primaryAgentDetail.to,
    //               to: item,
    //               contactDetails: getUser(item) ?? {},
    //               muteState: false,
    //               holdState: false,
    //               medium: false,
    //               bound: "OUT",
    //               agentType: "SECONDARY",
    //               isCustomer: false,
    //               isPrimary: message.Agent[0],
    //               isLogin: userData.ext.toString() === item ? true : false,
    //             };
    //             SecondaryAgentDetails.push(contacts);
    //             AgentWhole.push(contacts);
    //           });

    //           let mergeData: any = {
    //             customer: customerData,
    //             primaryAgent: primaryAgentDetail,
    //             SecondaryAgent: SecondaryAgentDetails,
    //             AgentDetails: AgentWhole,
    //             AllAgent: setuserAgent,
    //             userData: userData,
    //           };

    //           dispatch(CallPopUpAgentActions.mergeCallDataSetUp(mergeData));
    //         }
    //         navigate("/aiagentworkspace");
    //       });

    //     // getContactDetailsWhilegettingCall(message.customer).then((res: any) => {
    //     //   if (res?.data?.statusCode === 200) {
    //     //     customerContactDetails = res?.data?.getContactNumberRes[0];

    //     //     let customerData = {
    //     //       from: message.customer,
    //     //       contactDetails: customerContactDetails,
    //     //       to: message.Agent[0],
    //     //       muteState: false,
    //     //       holdState: false,
    //     //       medium: false,
    //     //       bound: "IN",
    //     //       agentType: "CUSTOMER",
    //     //       isCustomer: true,
    //     //       isLogin: false,
    //     //       isPrimary: false,
    //     //     };
    //     //     AgentWhole[0] = customerData;

    //     //     let primaryAgentDetail: any = {
    //     //       from: message.customer,
    //     //       to: message.Agent[0],
    //     //       contactDetails: getUser(message.Agent[0]) ?? {},
    //     //       muteState: false,
    //     //       holdState: false,
    //     //       medium: false,
    //     //       bound: "IN",
    //     //       agentType: "PRIMARY",
    //     //       isPrimary: true,
    //     //       isCustomer: false,
    //     //       isLogin: userData.ext.toString() === message.Agent[0] ? true : false,
    //     //     };

    //     //     AgentWhole.push(primaryAgentDetail);
    //     //     let SecondaryAgentDetails: any = [];
    //     //     let seccondary: any;

    //     //     seccondary = message.Agent.filter((agent: any) => {
    //     //       return agent !== primaryAgentDetail.to;
    //     //     });

    //     //     seccondary.map((item: any) => {
    //     //       let contacts = {
    //     //         from: primaryAgentDetail.to,
    //     //         to: item,
    //     //         contactDetails: getUser(item) ?? {},
    //     //         muteState: false,
    //     //         holdState: false,
    //     //         medium: false,
    //     //         bound: "OUT",
    //     //         agentType: "SECONDARY",
    //     //         isCustomer: false,
    //     //         isPrimary: false,
    //     //         isLogin: userData.ext.toString() === item ? true : false,
    //     //       };
    //     //       SecondaryAgentDetails.push(contacts);
    //     //       AgentWhole.push(contacts);
    //     //     });

    //     //     let mergeData: any = {
    //     //       customer: customerData,
    //     //       primaryAgent: primaryAgentDetail,
    //     //       SecondaryAgent: SecondaryAgentDetails,
    //     //       AgentDetails: AgentWhole,
    //     //       AllAgent: setuserAgent,
    //     //       userData: userData
    //     //     };

    //     //     dispatch(CallPopUpAgentActions.mergeCallDataSetUp(mergeData));
    //     //   }
    //     //   console.log(window.location.pathname !== "/aiagentworkspace","window.location.pathname ");
    //     //   ApiServices.get(CALLURL, `/get_chatSessionId/${message.session_id}`).then((response: any) => {
    //     //     if (response.data.statusCode === 200) {
    //     //       let chatSessionData: any = response.data.result[0];
    //     //       chatSessionData.Number = chatSessionData.customerDetails;
    //     //       chatSessionData.showState = true;
    //     //       getContactDetailsWhilegettingCall(chatSessionData.Number).then((res: any) => {
    //     //         if (res?.data?.statusCode === 200) {
    //     //           chatSessionData.calledPersonDetails = res?.data?.getContactNumberRes[0];
    //     //           chatSessionData.calledPersonDetails.tags =
    //     //             typeof res?.data?.getContactNumberRes[0]?.tags === "string"
    //     //               ? JSON.parse(res?.data?.getContactNumberRes[0]?.tags?.trim().length > 0 ? res?.data?.getContactNumberRes[0]?.tags : "null") ?? []
    //     //               : res?.data?.getContactNumberRes[0]?.tags;
    //     //           // chatSessionData.calledPersonDetails.tags = JSON.parse(res?.data?.getContactNumberRes[0]?.tags) ?? [];
    //     //         } else {
    //     //           chatSessionData.calledPersonDetails = {};
    //     //         }
    //     //         dispatch(ChatActions.chatSessionList(chatSessionData));
    //     //         dispatch(ChatActions.currentChatSession(chatSessionData));
    //     //         dispatch(CallPopUpAgentActions.setTransferTyper());
    //     //         //navigate("/aiagentworkspace");
    //     //       });
    //     //     }
    //     //     navigate("/aiagentworkspace");
    //     //   });

    //     // if (window.location.pathname !== "/aiagentworkspace") {
    //     //   navigate("/aiagentworkspace");
    //     // }
    //     // });
    //   }
    // });

    // socketService.io.on("callTransferedSuccess", async (message: any) => {
    //   const { answeredDuration, incomingcallDetails } = store.getState().chatReducer;
    //   const userDetails1 = GetItemFromStorage(LocalStorage.LOGIN_USER_CCAAS_DETAILS);
    //   const userDetails = GetItemFromStorage(LocalStorage.USER_DETAILS);
    //   let { TransferFrom, transferTo }: any = store.getState().callPopUpDetails;
    //   if (message?.deviceType === "CCAAS:BROWSER" && message?.transferType === 1) {
    //     await ApiServices.get(
    //       CALLURL,
    //       `/get_callSessionId/${message.session_id}`
    //     ).then(async (response: any) => {
    //       if (response.data.statusCode === 200) {
    //         let chatSessionData: any = response.data.result[0];
    //         console.log("chatSessionData", chatSessionData);

    //         let details: any = await showDetials({ sessionId: message?.session_id.trim(), channelType: "Call" })
    //         if (details?.data?.statusCode === 200) {
    //           const message_1 = JSON.parse(details?.data?.getEmailListRes[0]?.Message ?? '[]')
    //           console.log("message_1>>>>>>", message_1)
    //           dispatch(ChatActions.agentViewWorkSpaceHistory_WB(Array.isArray(message_1) ? message_1 : []));
    //         }
    //         chatSessionData.Number = chatSessionData.customerNumber;
    //         chatSessionData.showState = true;

    //         getContactDetails(chatSessionData.Number, "Call").then(
    //           (res: any) => {
    //             if (res?.data?.statusCode === 200) {
    //               chatSessionData.calledPersonDetails =
    //                 res?.data?.getContactNumberRes[0];
    //               chatSessionData.calledPersonDetails.tags =
    //                 typeof res?.data?.getContactNumberRes[0]?.tags === "string"
    //                   ? JSON.parse(
    //                     res?.data?.getContactNumberRes[0]?.tags?.trim()
    //                       .length > 0
    //                       ? res?.data?.getContactNumberRes[0]?.tags
    //                       : "null"
    //                   ) ?? []
    //                   : res?.data?.getContactNumberRes[0]?.tags;
    //               chatSessionData.queueId = incomingcallDetails?.qid;
    //               chatSessionData.queueName = incomingcallDetails?.queueName ?? null;
    //               try {
    //                 const coid = res?.data?.getContactNumberRes?.[0]?.coid
    //                 getPriviousChannelSummary(dispatch, { coid, currentSessionId: message?.session_id.trim() })
    //               } catch (error) {
    //                 console.log("getPriviousChannelSummary error", error)
    //               }
    //               // chatSessionData.calledPersonDetails.tags = JSON.parse(res?.data?.getContactNumberRes[0]?.tags) ?? [];
    //             } else {
    //               chatSessionData.calledPersonDetails = {};
    //             }
    //             dispatch(CallPopUpAgentActions.setCallingval(false));
    //             dispatch(ChatActions.chatSessionList(chatSessionData));
    //             dispatch(ChatActions.currentChatSession(chatSessionData));
    //             dispatch(CallPopUpAgentActions.setTransferTyper());
    //             console.log(" TransferFrom customerMsg?.length > 0", TransferFrom?.transferDataDetails, "TransferFrom?.transferDataDetails)", TransferFrom)
    //             const merge = { ...TransferFrom?.transferDataDetails, message: `Transfer call from ${TransferFrom?.fromExt}` }

    //             dispatch(ChatActions.agentWorkSpaceTansferHistory(merge));
    //             dispatch(CallPopUpAgentActions.setTransferFrom([]))
    //             const body: any = {
    //               CallHistoryId: chatSessionData?.CallHistoryId,
    //               domainId: userDetails?.domainId,
    //               AgentId: userDetails1?.UserID,
    //               answeredDuration: answeredDuration ?? 0,
    //             }
    //             socketService.callNotifyDialDuration(body);
    //             navigate("/aiagentworkspace");

    //           }
    //         );
    //       }
    //       navigate("/aiagentworkspace");
    //     });
    //   }
    // });

    socketService.io.on("callTransferEvent", (message: any) => {
      dispatch(CallPopUpAgentActions.setTransferTyper());
      navigate("/aiagentworkspace");
    });

    socketService.io.on("leave_call", (message: any) => {
      let userData: any = jwt_decode(token);
      if (message.endBy !== userData.ext) {
        console.log("MESSAGE DISCONNECTED BY", message);
        dispatch(CallPopUpAgentActions.leaveCallDataSetUp({ ...message, userExt: userData.ext }));
      }
    });

    socketService.io.on("leave_call_by_secondary", (message: any) => {
      let userData: any = jwt_decode(token);

      if (message.endBy !== userData.ext) {
        message["userData"] = userData;
        console.log("MESSAGE DISCONNECTED BY", message);
        dispatch(CallPopUpAgentActions.leaveCallSecondaryAgent(message));
      }
    });

    socketService.io.on('incomingcall_queue_name', (data: any) => {
      console.log("incomingcall_queue_name", data)
      dispatch(ChatActions.incomingcallDetails(data))
    })


    socketService.io.on("updatedstatus", (data: any) => {
      const userDetails = GetItemFromStorage(LocalStorage.USER_DETAILS);
      if (
        userDetails?.ext?.toString() === data?.ext?.toString() &&
        userDetails?.domainId?.toString() === data?.domainId?.toString()
      ) {
        dispatch(
          AgentStateAction.setCurrentStatus({ status: data?.statusName })
        );
      }
    });

    socketService?.io?.on("userStatusBackendTransfer", (data: any) => {
      if (data) {
        presenceService?.io?.emit("updatestate_backend", data);
        socketService.io.emit("agent_ready_for_queue_users", data);
      }
    });

    socketService.io.on("updatedstatuslist", (data: any) => {
      dispatch(agentstatusListAction.pushagentlist(data));
    });

    // socketService.io.on("callQueueWait", (data: any) => {
    //   let userDetails = GetItemFromStorage(LocalStorage.USER_DETAILS);
    //   if (parseInt(data?.domainId) === parseInt(userDetails?.domainId)) {
    //     CallServices.get("/get_all_queue_wait_time").then((resp: any) => {
    //       dispatch(
    //         queueTrackerActions.queueTrackerData({
    //           queueTrackerData: resp.data.getAllQueueList,
    //         })
    //       );
    //     });
    //   }
    // });

    socketService.io.on("userStatusUpdated", (data: any) => {
      let userDetails = GetItemFromStorage(LocalStorage.USER_DETAILS);
      if (data?.email && data?.status) {
        let setData = {
          email: data?.email ?? "",
          status: data?.status ?? "Offline",
        };
        dispatch(agentTrackerActions.updateUserInAgentTracker(setData));
        // dispatch(GetAlluserActions.updateStatus(setData));
      }

      // if (parseInt(userDetails?.domainId) === parseInt(data?.domainId) && !DOMAINID.includes(data?.domainId)) {
      //   CallServices.get("/get_all_queue_wait_time").then((resp: any) => {
      //     dispatch(
      //       queueTrackerActions.queueTrackerData({
      //         queueTrackerData: resp.data.getAllQueueList,
      //       })
      //     );
      //   });

    });

    socketService.io.on("supervisiorActivity", () => {
      // dispatch(allActivityActions.setSuperVisorVal());
      // dispatch(ChatActions.setActivityAutoRefresh());
      socketService.emailDashboard(loginUserCcaasDetail?.domainId);
      socketService.smsDashboard(loginUserCcaasDetail?.domainId);
      socketService.socialmediaDashboard(loginUserCcaasDetail?.domainId);
      // dispatch(setCallNotification(true));

    });
    socketService.io.on("adminActivity", () => {
      // dispatch(allActivityActions.setSuperVisorVal());
      // dispatch(ChatActions.setActivityAutoRefresh());
      socketService.emailDashboard(loginUserCcaasDetail?.domainId)
      socketService.smsDashboard(loginUserCcaasDetail?.domainId)
      socketService.socialmediaDashboard(loginUserCcaasDetail?.domainId)
    });
    socketService.io.on("interActionClosed", () => {
      // dispatch(ChatActions.setActivityAutoRefresh());
      // dispatch(ChatActions.setAllViewAutoRefresh());
    });
    socketService.io.on("interActionScreenClosed", (data: any) => {
      console.log('<<<<interActionScreenClosed>>>>', data)
      if (data?.sessionId) {
        // dispatch(ChatActions.sessionRemoveAfterEnd(data.sessionId));
      }
    });
    socketService.io.on("assignActivityListener", (data: any) => {
      if (data?.data?.req_type) {
        // if (loginUserCcaasDetail?.roleid != "6") {
        //   dispatch(allActivityActions.setSuperVisorVal());
        //   dispatch(ChatActions.setActivityAutoRefresh());
        //   dispatch(ChatActions.setActivityAutoRefresh());
        //   dispatch(setCallNotification(true));
        //   dispatch(ChatActions.setDashCallBackRefresh());
        // }
      } else {
        // dispatch(allActivityActions.setSuperVisorVal());
        // dispatch(ChatActions.setActivityAutoRefresh());
      }
    });
    socketService.io.on("reAssignActivityListener", (data: any) => {
      // dispatch(allActivityActions.setReAssignCount());
      // dispatch(ChatActions.setActivityAutoRefresh());
      // dispatch(setCallNotification(true));
    });

    socketService.io.on("supervisiorActivityBot", (data: any) => {
      // dispatch(ChatActions.setAllViewAutoRefresh());
    });
    socketService.io.on("adminActivityBot", (data: any) => {
      // dispatch(ChatActions.setAllViewAutoRefresh());
    });




    socketService.io.on("deleteUserLogout", (data: any) => {
      const currentUser = GetItemFromStorage(LocalStorage.USER_DETAILS);
      const currentDomainId = `${currentUser?.domainId ?? currentUser?.domain_id ?? userData?.domainId ?? ""}`;
      const currentExt = `${currentUser?.ext ?? currentUser?.sipLoginId ?? userData?.ext ?? ""}`;
      const currentUserId = `${currentUser?.UserID ?? currentUser?.userId ?? ""}`;
      const currentEmail = `${currentUser?.emailId ?? currentUser?.Email ?? ""}`.toLowerCase();

      const eventDomainId = `${data?.domainId ?? data?.domain_id ?? ""}`;
      const eventExt = `${data?.ext ?? data?.sipLoginId ?? data?.sip_login_id ?? ""}`;
      const eventUserId = `${data?.userId ?? data?.UserID ?? ""}`;
      const eventEmail = `${data?.email ?? data?.emailId ?? ""}`.toLowerCase();

      const isTargetedForCurrentUser =
        (eventDomainId !== "" && eventDomainId === currentDomainId && eventExt !== "" && eventExt === currentExt) ||
        (eventUserId !== "" && eventUserId === currentUserId) ||
        (eventEmail !== "" && eventEmail === currentEmail);

      if (!isTargetedForCurrentUser) return;

      localStorage.clear();
      sessionStorage.clear();
      Cookie.clear();
      navigate("/");
      ClearStorage();
    })

    socketService.io.on("allviewsRefresh", (data: any) => {
      // dispatch(ChatActions.setAllViewAutoRefresh());
      // if (!DOMAINID.includes(`${loginUserCcaasDetail?.domainId}`) && (loginUserCcaasDetail?.roleid == ROLES?.ADMIN_ROLE_ID || loginUserCcaasDetail?.roleid == ROLES?.SUPERVISOR_ROLE_ID)) {
      //   getLiveSessions(dispatch)
      // }
    });
    socketService.io.on("callTransferDetails", (data: any) => {
      dispatch(CallPopUpAgentActions.setTransferFrom(data));
    });

    socketService.io.on("pstnleavecall", (message: any) => {
      // dispatch(CallPopUpAgentActions.removePstnCallerId(message));
      let userData: any = jwt_decode(token);
      if (message.participant_left !== userData.ext) {
        message["userData"] = userData;
        message['endBy'] = message.participant_left
        console.log("MESSAGE DISCONNECTED BY", message);
        dispatch(CallPopUpAgentActions.leaveCallSecondaryAgent(message));
      }
    })

    // socketService.io.on("CAMPAIGN_DIALOUT", (message: any) => {
    //   console.log("CAMPAIGN_DIALOUT :", message)
    //   if (message?.customer && message?.status === "initiate") {
    //     CallOutboundCampaignFunction(message, "");
    //     dispatch(outboundCampaignSliceAction.setincomingCallInfo(message))
    //   } else if (message?.customer && message?.status === "terminate") {
    //     dialPadChanges(false, false, false, false, false, false);
    //     dispatch(CallPopUpAgentActions.callEnded());
    //     const presence = sessionStorage.getItem('presence')
    //     presenceChangeToReady(presence)
    //   } else if (message?.customer && message?.status === "offline") {

    //     dialPadChanges(false, false, false, false, false, false);
    //     presenceChangeToReady("Offline")
    //     sessionStorage.setItem('presence', "Offline")


    //   } else if (message?.customer && message?.status === "cancelled") {
    //     const popUpDetails: any = store.getState().callPopUpDetails;
    //     let { incomingCallDetail } = popUpDetails;
    //     const campaign: any = store.getState().outboundcampaign;
    //     console.log("CAMPAIGN_DIALOUT :cancelled", campaign?.incomingCallInfo?.session_id === message?.camp_session_id, "campaign?.incomingCallInfo?.session_id", campaign?.incomingCallInfo?.session_id, " message?.camp_session_id", message?.camp_session_id)

    //     if (campaign?.incomingCallInfo?.session_id === message?.camp_session_id) {
    //       if ((incomingCallDetail ? Object.keys(incomingCallDetail).length : 0) > 0) {
    //         SupervisorEndCall(incomingCallDetail?.useragent)
    //       }
    //       dispatch(CallPopUpAgentActions.callEnded());
    //       dialPadChanges(false, false, false, false, false, false);
    //       const presence = sessionStorage.getItem('presence')
    //       presenceChangeToReady(presence)
    //     }
    //   } else if (message?.customer && message?.status === "timeout") {
    //     const popUpDetails: any = store.getState().callPopUpDetails;
    //     let { incomingCallDetail } = popUpDetails;
    //     const campaign: any = store.getState().outboundcampaign;
    //     console.log("CAMPAIGN_DIALOUT :timeout", incomingCallDetail, campaign?.incomingCallInfo?.session_id === message?.camp_session_id, "campaign?.incomingCallInfo?.session_id", campaign?.incomingCallInfo?.session_id, " message?.camp_session_id", message?.camp_session_id)

    //     if (campaign?.incomingCallInfo?.session_id === message?.camp_session_id) {
    //       if ((incomingCallDetail ? Object.keys(incomingCallDetail).length : 0) > 0) {
    //         SupervisorEndCall(incomingCallDetail?.useragent)
    //       }
    //       dispatch(CallPopUpAgentActions.callEnded());
    //       dialPadChanges(false, false, false, false, false, false);
    //       presenceChangeToReady("Offline")
    //       sessionStorage.setItem('presence', "Offline")

    //     }
    //   } else if (message?.customer && message?.status === "disconnect") {
    //     const popUpDetails: any = store.getState().callPopUpDetails;
    //     let { incomingCallDetail } = popUpDetails;
    //     const campaign: any = store.getState().outboundcampaign;
    //     console.log("CAMPAIGN_DIALOUT :disconnect", campaign?.incomingCallInfo?.session_id === message?.camp_session_id, "campaign?.incomingCallInfo?.session_id", campaign?.incomingCallInfo?.session_id, " message?.camp_session_id", message?.camp_session_id)

    //     if (campaign?.incomingCallInfo?.session_id === message?.camp_session_id) {
    //       dialPadChanges(false, false, false, false, false, false);
    //       // const presence = sessionStorage.getItem('presence')
    //       // presenceChangeToReady(presence)
    //       if ((incomingCallDetail ? Object.keys(incomingCallDetail).length : 0) > 0) {
    //         SupervisorEndCall(incomingCallDetail?.useragent)
    //       }
    //       dispatch(CallPopUpAgentActions.callEnded());
    //       hassAccess('Disposition/regenerate') && summarySubjectHandlerForCall(message?.session_id, dispatch, 0)
    //       dispatch(ChatActions.setCallSessionList({ state: true, sessionId: message?.session_id ?? "" }));
    //     }
    //   }

    // });
    // socketService.io.on("CAMPAIGN_DIALOUT_SESSION", (message: any) => {
    //   dispatch(ChatActions.setCallSessionId(message?.session_id ?? ""));
    //   console.log("chatSessionData?.CAMPAIGN_DIALOUT_SESSION", message?.session_id)
    //   dispatch(outboundCampaignSliceAction.outboundCampaignclose());
    //   console.log("getNecessaryDetailswithsession CAMPAIGN_DIALOUT_SESSION", Date.now())
    //   dispatch(
    //     getNecessaryDetailswithsession({
    //       contact: message.ddi,
    //       sessionId: message.session_id,
    //     })
    //   )
    //     .unwrap()
    //     .then((res: any) => {
    //       if (res?.sessionDetail?.data.statusCode === 200) {
    //         console.log("getNecessaryDetailswithsession CAMPAIGN_DIALOUT_SESSION", Date.now(), res?.sessionDetail?.data)
    //         let chatSessionData: any = res?.sessionDetail?.data?.result[0];
    //         chatSessionData.Number =
    //           chatSessionData?.callType === "outbound"
    //             ? chatSessionData?.calledNumber
    //             : chatSessionData?.customerDetails;
    //         // chatSessionData.calledPersonDetails = callSessionDetailswhilePopup.contactDetails;
    //         // chatSessionData.calledPersonDetails.tags = JSON.parse(callSessionDetailswhilePopup.contactDetails?.tags?.trim().length > 0 ? callSessionDetailswhilePopup.contactDetails?.tags : "null") ?? [];
    //         chatSessionData.showState = true;
    //         // chatSessionData.queueName = queueName ?? "Default Queue";
    //         // chatSessionData.queueId = queueDetails?.key;
    //         chatSessionData.queueName = "";
    //         chatSessionData.queueId = ""
    //         if (res?.contact?.data?.statusCode === 200) {
    //           chatSessionData.calledPersonDetails =
    //             res.contact?.data?.getContactNumberRes[0];
    //           chatSessionData.calledPersonDetails.tags =
    //             typeof res?.contact?.data?.getContactNumberRes[0]?.tags ===
    //               "string"
    //               ? JSON.parse(
    //                 res?.contact?.data?.getContactNumberRes[0]?.tags?.trim()
    //                   .length > 0
    //                   ? res?.contact?.data?.getContactNumberRes[0]?.tags
    //                   : "null"
    //               ) ?? []
    //               : res?.contact?.data?.getContactNumberRes[0]?.tags;
    //           // JSON.parse(res?.contact?.data?.getContactNumberRes[0]?.tags?.trim().length > 0 ? res?.contact?.data?.getContactNumberRes[0]?.tags : "null") ?? [];
    //         } else {
    //           chatSessionData.calledPersonDetails = {};
    //         }
    //         dispatch(
    //           updateContactinSession({
    //             sessionId: chatSessionData.sessionId,
    //             calledPersonDetails: chatSessionData.calledPersonDetails,
    //           })
    //         );

    //         sessionStorage.setItem('currentSec', JSON.stringify(chatSessionData))
    //         sessionStorage.setItem('currentSecId', JSON.stringify(chatSessionData?.sessionId))
    //         dispatch(ChatActions.chatSessionList(chatSessionData));
    //         dispatch(ChatActions.currentChatSession(chatSessionData));
    //         dispatch(CallPopUpAgentActions.setOBCState());
    //         navigate("/aiagentworkspace");
    //       }
    //     });
    // });

  }

  static async destroy() {
    socketService?.io?.disconnect();
  }

  static async popUpFunction(navigate: any) {
    // let roomData: any = store.getState().chatReducer;
    const userDetailsData = GetItemFromStorage(LocalStorage);
    const userDetails = userDetailsData;
    let roomDetials: any = {
      // room: roomData?.roomInfo,
      // sessionId: roomData?.sessionId,
      agentDetails: userDetails,
    };
    let ringer: any = document.getElementById("ringer");
    ringer.pause();
    socketService.io.emit("joinagent", roomDetials);
    // await CallServices.get(`/get_callSessionId/${roomData?.sessionId}`).then(
    //   (response: any) => {
    //     if (response.data.statusCode === 200) {
    //       let chatSessionData: any = response.data.result;
    //       chatSessionData.roomInfo = roomData.roomInfo;
    //       chatSessionData.customerData = response?.data.result.customerData
    //         ? response.data.result.customerData
    //         : "";
    //       let existingListing: any = roomData.sessionList || [];
    //       existingListing.push(chatSessionData);
    //       store.dispatch({
    //         type: Types.CHAT_SESSION_LIST,
    //         payload: {
    //           sessionList: existingListing,
    //         },
    //       });
    //       store.dispatch({
    //         type: Types.CURRENT_CHAT_SESSION,
    //         payload: {
    //           currentSession: chatSessionData,
    //         },
    //       });
    //       store.dispatch({
    //         type: Types.POPUP_MESSAGE,
    //         payload: {
    //           acceptRoom: false,
    //           roomInfo: "",
    //           sessionId: "",
    //           sessionDetailswhilePopup: "",
    //         },
    //       });
    //     }
    //   }
    // );
  }
  // static async getMessageList(data: any) {
  //   let roomData: any = store.getState().chatReducer;

  //   let requiredData = {
  //     sessionId: data?.sessionId,
  //     room: data.roomInfo,
  //   };

  //   await CallServices.get(`/get_callSessionId/${data?.sessionId}`).then(
  //     (response: any) => {
  //       if (response.data.statusCode === 200) {
  //         let chatSessionData: any = response.data.result;
  //         chatSessionData.roomInfo = roomData.roomInfo;
  //         chatSessionData.customerData = response?.data.result.customerData
  //           ? response.data.result.customerData
  //           : "";
  //         let existingListing: any = roomData.sessionList || [];
  //         let exist = false;
  //         existingListing.forEach((data: any) => {
  //           if (data.sessionId == chatSessionData?.sessionId) {
  //             exist = true;
  //           }
  //         });
  //         if (!exist) existingListing.push(chatSessionData);
  //         store.dispatch({
  //           type: Types.CHAT_SESSION_LIST,
  //           payload: {
  //             sessionList: existingListing,
  //           },
  //         });
  //         store.dispatch({
  //           type: Types.CURRENT_CHAT_SESSION,
  //           payload: {
  //             currentSession: chatSessionData,
  //           },
  //         });
  //       }
  //     }
  //   );

  //   socketService.io.emit("getMessageList", requiredData);
  // }
  static reconnect(data: any) {
    socketService.io.emit("reconnect", data);
  }

  static callBackReject(data: any) {
    const emailId = GetItemFromStorage(LocalStorage.USER_EMAIL);
    socketService.io.emit("callback_session", {
      ...data,
      email: emailId ?? "", device_type: "CCAAS:BROWSER",
    });
  }
  // static async sessioIdTransfer(id: any, dispatch: any, getAgentType: any) {
  //   let callData: any = {
  //     sessionId: id,
  //   };
  //   setTimeout(() => {
  //     socketService.io.emit("joinCall", callData);
  //   }, 1000);
  //   await CallServices.get(`/get_callSessionId/${id.trim()}`).then(
  //     (response: any) => {
  //       if (response.data.statusCode === 200) {
  //         let chatSessionData: any = response?.data?.result[0];
  //         chatSessionData.showState = false;
  //         chatSessionData.customerData = response?.data.result[0]
  //           .customerDetails
  //           ? response.data.result[0].customerDetails
  //           : "";
  //         dispatch(ChatActions.chatSessionList(chatSessionData));
  //         dispatch(ChatActions.currentChatSession(chatSessionData));
  //       }
  //     }
  //   );
  // }

  static async chatAgentTransfer(data: any) {
    // socketService.io.emit("transferAgenttoAgent",data);
  }
  static async transfer(data: any) {
    socketService.io.emit("init_transfer", data);
  }
  static async agentAcceptRequest(data: any) {
    socketService.io.emit("agentAcceptRequest", data);
  }
  static async assignActivityEmiiter(data: any) {
    socketService.io.emit("assignActivityEmitter", data);
    socketService.callDashboard(data?.domainId)
    socketService.emailDashboard(data?.domainId)
    socketService.smsDashboard(data?.domainId)
    socketService.socialmediaDashboard(data?.domainId)
  }
  static async reAssignActivityEmiiter(data: any) {
    socketService.io.emit("reAssignActivityEmitter", data);
    socketService.callDashboard(data?.domainId)
    socketService.emailDashboard(data?.domainId)
    socketService.smsDashboard(data?.domainId)
    socketService.socialmediaDashboard(data?.domainId)

  }

  static async storeCustomerCoid(data: any) {
    socketService.io.emit("store_customer_data", data);
  }
  static async postMergeData(data: any) {
    socketService.io.emit("merge_call", data);
  }

  static async postTransferData(data: any) {
    socketService.io.emit("callTransferedRequest", data)
  }
  static async transferFrom(data: any) {
    socketService.io.emit("callTransferFrom", data)
  }
  static async removeChatQueueUser(data: any) {
    socketService.io.emit("removeChatQueueUser", data)
  }
  static async removeChatQueueUserSocialmedia(data: any) {
    socketService.io.emit("removeChatQueueUserSocialmedia", data)
  }
  static async sendingtoServer(agentstatus: any) {
    socketService.io.emit("updatedstatuslist", agentstatus);
  }
  static async refershLiveTracker() {
    socketService.io.emit("refershLiveTracker");
  }

  static async videoMeetDecline(data: any) {
    console.log(data, "videoMeetDecline=====");

    socketService.io.emit("videoMeetDecline", data);
  }

  static async agentMissedCallDashboard(data: any) {
    // const roomData: any = store.getState().chatReducer;
    // data["SourceChatDurationHistoryId"] = roomData?.internalSessionDetailswhilePopup?.ChatDurationHistoryId;
    socketService.io.emit("agent_missed_call_dashboard", data);
  }

  static async dispositionStatusDashboard(data: any) {
    socketService.io.emit("disposition_status_dashboard", data);
  }

  static async chatNotifyDialDuration(data: any) {
    socketService.io.emit("chat_notify_dial_duration", data);
  }
  static async callDashboard(data: any) {
    socketService.io.emit("dashboard_call", data);
  }
  static async emailDashboard(data: any) {
    socketService.io.emit("dashboard_email", data);
  }
  static async smsDashboard(data: any) {
    socketService.io.emit("dashboard_sms", data);
  }
  static async agentaAssistInsert(data: any) {
    socketService.io.emit("insert_update_agent_assist", data);
  }
  static async socialmediaDashboard(data: any) {
    socketService.io.emit("dashboard_socialmedia", data);
  }
  static async agentMissedCallWhatsapp(data: any) {
    socketService.io.emit("whatsapp_agent_missed_call", data);
  }
  static async agentMissedCallFacebook(data: any) {
    socketService.io.emit("facebook_agent_missed_call", data);
  }
  static async agentMissedCallInstagram(data: any) {
    socketService.io.emit("instagram_agent_missed_call", data);
  }
  static async callNotifyDialDuration(data: any) {
    socketService.io.emit("call_answered_duration", data);
  }
  static async callVoiceMail(data: any) {
    const message = JSON.stringify(data.message)
    socketService.io.emit("callViaSmsEmail", { ...data, message });
  }
  static async callViaSmsEmail(data: any) {
    socketService.io.emit("callViaSmsEmail", data);
  }

  static async triggerMessage(data: any) {
    const LoginUserDetails = GetItemFromStorage(LocalStorage.USER_DETAILS);
    const payload =
    {
      ...data,
      domain_id: LoginUserDetails?.domainId,
      ext: LoginUserDetails?.ext,
      userId: LoginUserDetails?.UserID,
    }
    socketService.io.emit("insert_Trigger", payload);
  }
  static async languageSwitch(data: any) {
    socketService.io.emit("language_switch", data)
  }
  static async sendAiDashboardQuery(data: any) {
    socketService.io.emit("ai_dashboard_query", data)
  }
  static async sendAiChatDashboardQuery(data: any) {
    socketService.io.emit("ai_chat_query", data)
  }
  static async firstAssistMessage(data: any) {
    socketService.io.emit("first_assist_msg", data)
  }
  static async updateLogoutForCrossDomain() {
    socketService.io.emit("update_logout_cross_domain");
  }
}