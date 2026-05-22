
import { GetItemFromStorage } from "../custom_hooks/useStorage";
import { LocalStorage } from "../custom_hooks/localStorageKeys";
import _ from "lodash";
import { CallSDK } from "unifiedring-sdk";
import { v4 as uuidv4 } from "uuid";
import { determineIps } from "../utils";
import socketService from "../sockets/socketService";
import { CallPopUpAgentActions } from "../../store/slice/callSlice/call_popup_agent";
import { getContactDetails } from "./core_serviceApi";
import store from "../../store/redux-store/root_store";
import { publicIpv4 } from "public-ip";
import presenceService from "../sockets/presence";
import { CallUserAgentActions } from "../../store/slice/callSlice/login_user_call_details";
import { config } from "../constant";
import { ChatActions } from "../../store/slice/chat_slice/chat_slice";
import { getCallNecessaryDetails } from "../../store/slice/users/user_management";
import { updateNLPcontactDeals } from "./nlp_service";

let listenInterval: any;
let {
  CallAccept,
  CallHold,
  CallUnHold,
  EndCall,
  MakeCall,
  Mute_AudioCall,
  Registration,
  RejectCall,
  UnMute_AudioCall,
  Unregistration,
  MakeReInviteCall,
  CallConference,
  agentBusy,
  MakeCallOutboundCampiagn
} = CallSDK.Call;
let CallTimer: any;
let accpetCallBeforeTerminated = false;
export function CallService(dispatch: any, navigate: any) {
  let isUAInitializing = false;
  const createCallConnection = async () => {
    if (isUAInitializing) return
    isUAInitializing = true;
    const userDetials = GetItemFromStorage(LocalStorage.USER_DETAILS);
    const wssUrl = GetItemFromStorage(LocalStorage.WSS_URL) ?? config().CALL_SERVER_WSS_URL;
    const LoginUserDetails: any = userDetials;

    await determineIps();
    let ipAddress = GetItemFromStorage("IP");
    if (!ipAddress) {
      ipAddress = await publicIpv4();
    }
    const DomainName = `${LoginUserDetails?.domain_name}:${config().DOMAIN_NAME_PORT}`;
    const username = LoginUserDetails?.ext;
    const displayName = LoginUserDetails?.profileName;
    const password = LoginUserDetails?.dp_password;
    const ip = ipAddress;

    const configuration = {
      wsServers: wssUrl,
      viaHost: ip,
      uri: username + "@" + DomainName,
      userAgentString: config().CALL_SERVER_USER_AGENT,
      traceSip: true,
      register: true,
      authorizationUser: username,
      password: password,
      turnServers: {
        urls: config().CALL_SERVER_STUN_URL,
        urls2: config().CALL_SERVER_STUN_URL2,
        username: config().CALL_SERVER_STUN_USERNAME,
        password: config().CALL_SERVER_STUN_PASSWORD,
      },
      displayName: displayName,
      callId: username,
      host: ip,
    };

    let UA = await Registration(configuration);
    UA.on("accepted", function (reason: any) {
      if (reason.method === "REGISTER") {
        console.log("----------registration success------------");
        const reg = GetItemFromStorage('register')
        if ([reg].includes("1")) {
          const registerDetails: any = store.getState().callPopUpDetails;
          let { agentRegisterDetails } = registerDetails
          socketService.io.emit("agent_register_request", {
            ...agentRegisterDetails, status: 'success'
          });
          dispatch(CallPopUpAgentActions.setagentRegisterDetails({}));
          localStorage.removeItem('register')
        }
        dispatch(CallUserAgentActions.setLoginuserUserAgent(UA));
        inviteBinding(UA);
      }
    });

    UA.transport.on("disconnected", () => {
      console.warn("🔴 WebSocket disconnected");
    });

    UA.transport.on("connected", () => {
      console.warn("🟢 WebSocket reconnected");
      isUAInitializing = true;
      createCallConnection()
    });
  };

  const inviteBinding = (UA: any) => {
    UA.on("registrationFailed", function (reason: any) { });

    UA.on("referInviteSent", function (e: any) { });

    UA.on("invite", async function (rsession: any) {
      try {
        console.log("got invite",rsession);
        accpetCallBeforeTerminated = false;
        // const campaign = store.getState().outboundcampaign;
        const popUpDetails: any = store.getState().callPopUpDetails;

        if (popUpDetails?.callDetails && popUpDetails?.callDetails?.length > 0) {
          agentBusy(rsession);
          console.log('Agent busy - rejecting incoming call, current calls:', popUpDetails?.callDetails.length);
          return;
        }

        const sessionCompleteData = rsession?.incomingRequest?.message?.data ?? "";
        const subStringSession = sessionCompleteData.includes("v=0")
          ? sessionCompleteData.split("v=0")[0]
          : sessionCompleteData;
        const sessionval = subStringSession.split("ExtraRemote-Info:")[0] ?? "";
        const TransSession = (subStringSession.split("ExtraRemote-Info:")[1] ?? "").trim();
        const callSession = (sessionval.split("iuid:")[1] ?? "").trim();

        let sessionId = "";
        let getAgentType = "";


        if (TransSession.includes("CCAAS-SESSION-ID:")) {
          sessionId = (TransSession.split("CCAAS-SESSION-ID:")[1] ?? "").trim();
          console.log('sessionId:', sessionId);
          getAgentType = "AGENT";
        } else {
          sessionId = callSession;
          getAgentType = "EXTERNAL";
        }

        void waitForMicrophonePermission().then((micState) => {
          console.log("micState :", micState);
          if (micState === "granted") {
            console.log("✅ Proceed with the call setup...");
          }
        });

        dispatch(CallPopUpAgentActions.clearIncomingCallDetail());
        dispatch(CallPopUpAgentActions.setCloseCall(false));
        dispatch(CallPopUpAgentActions.isCallAccepted());

        const latestPopupDetails: any = store.getState().callPopUpDetails;
        const isCallAccepted = latestPopupDetails?.isCallAccepted;

        if (window.location.pathname !== "/login" && window.location.pathname !== "/") {
          setUpListeners(rsession, rsession?.remoteIdentity?.uri?.normal?.user ?? "New Customer".toString(), 'audio', 'IN', "", uuidv4(), false);
          console.log("[ accpetCallBeforeTerminated ] - 1")
          if (!isCallAccepted) {
            const resolvedSessionId = sessionId?.trim?.() || uuidv4();
            dispatch(CallPopUpAgentActions.setSessionId(resolvedSessionId));
            let FromCallerName = rsession.remoteIdentity.uri.normal.user;
            let Details: any = {};
            let queueName: any = "";
            const loginCrmUserDetails: any = GetItemFromStorage(LocalStorage.USER_DETAILS) ?? {};
            const agentQueueName = popUpDetails?.queueName ?? loginCrmUserDetails?.queueName ?? loginCrmUserDetails?.queue_name ?? "";
            let useragent: any = rsession;
            const guid = uuidv4();
            let medium: string = "audio";
            let bound: string = "IN";
            let fromcaller: string = useragent?.remoteIdentity?.uri?.normal?.user ?? "New Customer";
            const rawContact = `${rsession?.remoteIdentity?.uri?.normal?.user ?? ""}`.trim();
            const digitsOnlyContact = rawContact?.replace(/[^\d]/g, "");
            const normalizedContact = digitsOnlyContact?.length >= 7
              ? digitsOnlyContact
              : rawContact?.replace(/^\+/, "");
            try {
              const responseAction: any = await dispatch(
                getCallNecessaryDetails({
                  contact: normalizedContact,
                })
              );
              const response: any = responseAction?.payload ?? responseAction;

              if (accpetCallBeforeTerminated) {
                console.log('[ accpetCallBeforeTerminated ] - Controled')
                return;
              }

              const contactRoot: any = response?.contact?.data ?? response?.contact ?? {};
              const contactList: any[] = Array.isArray(contactRoot?.customerContactRes)
                ? contactRoot.customerContactRes
                : Array.isArray(contactRoot)
                  ? contactRoot
                  : [];
              const contactStatus = response?.contact?.code === "ECONNABORTED"
                ? 500
                : (contactRoot?.statusCode ?? 200);

              if (contactStatus !== 200) {
                FromCallerName = rsession.remoteIdentity.uri.normal.user;
                Details = {};
              } else {
                FromCallerName =
                  contactList.length > 0
                    ? (contactList[0].firstName ?? contactList[0].first_name ?? rsession.remoteIdentity.uri.normal.user)
                    : rsession.remoteIdentity.uri.normal.user;
                Details = contactList.length > 0 ? contactList[0] : {};
              }
              queueName = agentQueueName;
              // const sessionRoot: any = response?.sessionDetail?.data ?? response?.sessionDetail ?? {};
              // const sessionList: any[] = Array.isArray(sessionRoot?.result)
              //   ? sessionRoot.result
              //   : Array.isArray(sessionRoot?.getSessionDetailsRes)
              //     ? sessionRoot.getSessionDetailsRes
              //     : Array.isArray(sessionRoot?.getCallSessionIdRes)
              //       ? sessionRoot.getCallSessionIdRes
              //       : Array.isArray(sessionRoot)
              //         ? sessionRoot
              //         : [];
              // const sessionStatus = response?.sessionDetail?.code === "ECONNABORTED"
              //   ? 500
              //   : (sessionRoot?.statusCode ?? 200);

              // if (sessionStatus !== 200) {
              //   queueName = agentQueueName;
              // } else {
              //   console.log("Worktual CALL DID COME HERE");
              //   queueName = sessionList.length > 0 ? (sessionList[0].queueName ?? sessionList[0].queue_name ?? "") : "";
              //   if (!queueName) {
              //     queueName = agentQueueName;
              //   }
              // }
              const CalluserDetails = {
                userAgent: useragent,
                medium: medium,
                bound: bound,
                contact: {},
                fromcaller: fromcaller,
                fromCallerName: FromCallerName,
                guid: guid,
                agentType: TransSession?.includes("CCAAS-SESSION-ID:") ? "SECONDARY" : "PRIMARY",
                typeOf: getAgentType,
                contactDetails: Details,
                sessionId: resolvedSessionId,
                queueName: TransSession?.includes("CCAAS-SESSION-ID:") ? (queueName !== "" ? queueName : "Tranfered call") : queueName ?? "",
                isCustomer: TransSession?.includes("CCAAS-SESSION-ID:") ? false : true,
              };
              callPopUpFunction(true, CalluserDetails, false);
              let ringer: any = document.getElementById("ringer");
              console.log(ringer, "ringer");

              const promisesRing: any = ringer?.play?.();
              if (promisesRing !== undefined) {
                promisesRing
                  .then(() => {
                    ringer?.play?.();
                  })
                  .catch(() => {
                    ringer?.play?.();
                  });
              }
            } catch (err: any) {
              console.log('got invite catch:', err)
            }
          }
        }
      } catch (error) {
        console.log("invite err:", error)
      }
    });
  };

  const waitForMicrophonePermission = async (): Promise<"granted" | "denied" | "prompt"> => {
    try {
      const permissionStatus = await navigator.permissions.query({ name: "microphone" as PermissionName });

      if (permissionStatus.state === "granted") {
        console.log("✅ Microphone already granted.");
        return "granted";
      }

      if (permissionStatus.state === "prompt") {
        try {
          console.log("🔄 Requesting microphone access...");
          await navigator.mediaDevices.getUserMedia({ audio: true });
          console.log("✅ Microphone granted after prompt.");
          return "granted";
        } catch (err) {
          console.warn("❌ User denied microphone access.");
          return "denied";
        }
      }

      if (permissionStatus.state === "denied") {
        console.warn("❌ Microphone permission is blocked.");
        return "denied";
      }

      return permissionStatus.state;
    } catch (err) {
      console.error("Error checking microphone permission:", err);
      return "denied";
    }
  };


  const makeCallFunction = (sipId: any, sessionGuid: string) => {
    console.log("sessionGuid>>>>>>>>>>>>>",sessionGuid)
        console.log("sipId>>>>>>>>>>>>>",sipId)

    const popUpDetails: any = store.getState().callPopUpDetails;
    const callLoginUserAgent: any = store.getState().callLoginUserAgent;
    let { incomingCallDetail, queueName } = popUpDetails;
    let { userAgent } = callLoginUserAgent;
    const LoginCrmUserDetails: any = GetItemFromStorage(LocalStorage.USER_DETAILS);
    const userDetails = GetItemFromStorage(LocalStorage.USER_DETAILS);
    const LoginUserDetails: any = userDetails;
    const uri = `sip:${sipId}@${LoginUserDetails.domain_name}`;

    let name = queueName !== "" ? queueName : LoginUserDetails.profileName;

    let useragent = MakeCall(userAgent, uri, "audio", name);
    const { agentStatus } = store.getState().agentState;
    const { users } = store.getState().userList;
    let status: any = null
    if (agentStatus?.length > 0) {
      [status] = agentStatus.filter((i: any) => { return "Busy" === i?.statusName?.trim() })
    }
    let enableStatus: any = null
    if (users.length > 0) {
      [enableStatus] = users?.filter((i: any) => { return LoginCrmUserDetails?.emailId === i?.emailId })
    }
    console.log('presenceService 12', status, enableStatus);
    presenceService.io.emit("updatestate_backend", {
      domainId: LoginUserDetails?.domainId,
      isVoice: (enableStatus?.voice === 1 && status?.isVoice === 1) ? 1 : 0,
      isVoiceCallTransfer: status?.isVoiceCallTransfer ?? 0,
      isChat: (enableStatus?.chat === 1 && status?.isChat === 1) ? 1 : 0,
      isChatTransfer: status?.isChatTransfer ?? 0,
      isEmail: (enableStatus?.email === 1 && status?.isEmail === 1) ? 1 : 0,
      isEmailTransfer: status?.isEmailTransfer ?? 0,
      stateTime: Math.floor(Date.now() / 1000),
      statusName: "Busy",
      ext: LoginUserDetails?.ext,
      email: LoginUserDetails?.Email,
      userId: LoginCrmUserDetails?.UserID,
      routingProfileId: LoginCrmUserDetails?.routing_profile_id,
      userName: LoginCrmUserDetails?.UserName,
      roleId: LoginCrmUserDetails?.org_role_id_crm,
      agentStatus: 0,
      skill: JSON.parse(enableStatus?.addSkill ?? '[]'),
      routingMethod: enableStatus?.routingMethod ?? 0,
      inboundCall: LoginCrmUserDetails?.inboundCall || enableStatus?.inboundCall,
      outboundCampaignCall: enableStatus?.outboundCampaignCall,
      isCrm: 1
    });
    let data = {
      ext: LoginUserDetails?.ext,
      domainId: LoginUserDetails?.domainId
    };
    socketService.io.emit("agent_ready_for_queue_users", data);
    setUpListeners(useragent, sipId, "audio", "OUT", "", sessionGuid, false);
    return useragent;
  };

  const CallOutboundCampaignFunction = (data: any, contactDetails: any) => {
    try {
      const userDetails = GetItemFromStorage(LocalStorage.USER_DETAILS);
      const LoginCcaasUserDetails: any = GetItemFromStorage(LocalStorage.USER_DETAILS);
      const LoginUserDetails: any = userDetails;
      const callLoginUserAgent: any = store.getState().callLoginUserAgent;
      let { userAgent } = callLoginUserAgent;
      const uri = `sip:${data?.customer}@${LoginUserDetails.domain_name}`;
      let name = LoginUserDetails.profileName;

      let ccaasFeatreReq = [
        `CCAAS-FEATURE-REQ:OUTBOUND_CAMPAIGN`,
        `CCAAS-SESSION-ID:${data?.session_id}`,
        `CCAAS-CALLER-TYPE:AGENT`,

      ];

      let useragent = MakeCallOutboundCampiagn(userAgent, uri, "audio", name, ccaasFeatreReq);

      // const { users } = store.getState().userList;
      let enableStatus: any = null
      // if (users.length > 0) {
      //   [enableStatus] = users?.filter((i: any) => { return LoginCcaasUserDetails?.emailId === i?.emailId })
      // }
      console.log('presenceService outbound campaign call:', enableStatus);
      presenceService.io.emit("updatestate_backend", {
        domainId: LoginUserDetails?.domainId,
        isVoice: 0,
        isVoiceCallTransfer: 0,
        isChat: 0,
        isChatTransfer: 0,
        isEmail: 0,
        isEmailTransfer: 0,
        stateTime: Math.floor(Date.now() / 1000),
        statusName: "Busy",
        ext: LoginUserDetails?.ext,
        email: LoginUserDetails?.Email,
        userId: LoginCcaasUserDetails?.UserID,
        routingProfileId: LoginCcaasUserDetails?.routing_profile_id,
        userName: LoginCcaasUserDetails?.UserName,
        roleId: LoginCcaasUserDetails?.org_role_id_crm,
        agentStatus: 0,
        skill: JSON.parse(enableStatus?.addSkill ?? '[]'),
        routingMethod: enableStatus?.routingMethod ?? 0,
        inboundCall: LoginCcaasUserDetails?.inboundCall || enableStatus?.inboundCall,
        outboundCampaignCall: enableStatus?.outboundCampaignCall,
        isCrm: 1
      });
      let queuedata = {
        ext: LoginUserDetails?.ext,
        domainId: LoginUserDetails?.domainId
      };
      try {
        socketService.io.emit("agent_ready_for_queue_users", queuedata);
      } catch (error) {
        console.log('Api error:', error)
      }
      setUpListeners(useragent, data?.customer, "audio", "OUT", "", uuidv4(), true);
    } catch (error) {
      console.log('CallOutboundCampaignFunction err:', error)
    }
  };

  const CallTransferFunction = (ext: any) => {
    const dialPadDetails: any = store.getState().callPopUpDetails;
    const callLoginUserAgent: any = store.getState().callLoginUserAgent;

    let { userAgent } = callLoginUserAgent;
    const chatReducer: any = store.getState().chatReducer;
    let { incomingCallDetail } = dialPadDetails;
    const userDetails = GetItemFromStorage(LocalStorage.USER_DETAILS);
    const LoginUserDetails: any = userDetails;

    let uri = "sip:" + ext + "@" + LoginUserDetails.domain_name;

    let ccaasFeatreReq = [
      `CCAAS-FEATURE-REQ:TRANSFER`,
      `CCAAS-SESSION-ID:${chatReducer?.currentSession?.sessionId}`,
      `CCAAS-CALLER-TYPE:AGENT`,
      `CCAAS-TRANSFER-TYPE:${dialPadDetails?.transfer_type}`, // QUEUE, AGENT, EXTERNAL
    ];

    let useragent = MakeReInviteCall(userAgent, uri, "audio", "unKnown", ccaasFeatreReq);

    setUpListeners(useragent, LoginUserDetails.ext, "audio", "OUT", "Transfer", incomingCallDetail.guid, false);
  };

  const setUpListeners = (thissession: any, callerSip: string, medium: string, bound: string, monitor?: any, sessionGet?: any, obc?: any) => {
    const ringer: any = document.getElementById("ringer");
    try {
      clearInterval(listenInterval);
      thissession.on("accepted", function (e: any) {
        console.log('[thissession] - accepted', e);
        console.log(thissession, "thissession>>>");
        const popUpDetails: any = store.getState().callPopUpDetails;

        let { incomingCallDetail, callDetails, isTrackAgent } = popUpDetails;

        let useragent: any = thissession;
        let fromcaller: string = thissession.remoteIdentity.uri.normal.user;
        let tocaller_id = callerSip;
        let storeData: any = {
          guid: sessionGet,
          useragent: useragent,
          from: fromcaller,
          to: tocaller_id.toString(),
          muteState: false,
          holdState: false,
          medium: "audio",
          bound: bound,
          agentType: "PRIMARY",
          contact: [],
          isTransfer: monitor,
          isCurrent: true,
          isCustomer: false,
          isNewCall: true,
          isConference: false,
          callStatus: "Ringing",
          callType: "direct",
          typeOf: monitor === "Transfer" ? "AGENT" : "EXTERNAL",
          outboundCampaign: obc ?? false
        };

        if (bound === "OUT") {
          console.log("Entering");
          // dialPadChanges(false, false, false, false, true, true);
          if (e !== undefined && e.toString().includes("call-connected-info")) {
            let callInfo: any = new DOMParser().parseFromString(e, "text/xml");
            let connectionStatus: string = callInfo.getElementsByTagName("call-connected-info")[0].getAttribute("callstatus");
            if (connectionStatus === "connected") {
              if (monitor === "Transfer") {
                dispatch(CallPopUpAgentActions.updateCallState("accepted"));
                dialPadChanges(false, true, false, false, true, true);

                dispatch(CallPopUpAgentActions.timerForTransferAgent());
              } else {
                if (callDetails.length > 1) {
                  dialPadChanges(false, true, false, false, true, true);
                  dispatch(CallPopUpAgentActions.updateCallState("accepted"));
                } else {
                  if (storeData.typeOf === "AGENT") {
                    // }else if(callDetails.length===1&&storeData.typeOf==="EXTERNAL"){
                    if (window.location.pathname === "/aiagentworkspace") {
                      dialPadChanges(false, false, false, false, true, true);
                    } else {
                      dialPadChanges(false, true, false, false, true, true);
                    }
                  }
                  dispatch(CallPopUpAgentActions.updateCallState("accepted"));
                  dispatch(ChatActions.closeIntractions());
                  // navigate("/aiagentworkspace");
                }
                callTimerFunc(false);
              }
              if (isTrackAgent) {
                incomingCallUserData(false, storeData);
              }
            }
          } else {
            let contactDetails: any;
            if (callDetails.length === 1) {
              contactDetails = incomingCallDetail.contactDetails;
            } else {
              // contactDetails = getUser(fromcaller) ?? {};
              if (!contactDetails.hasOwnProperty("ext")) {
                getContactDetails(null, fromcaller).then((res: any) => {
                  if (Array.isArray(res) && res.length > 0) {
                    contactDetails = res[0];
                  } else {
                    contactDetails = {};
                  }
                });
              }
            }

            storeData["contactDetails"] = contactDetails;
            if (monitor === "Transfer") {
              dispatch(CallPopUpAgentActions.pushCallDetails(storeData));
              dialPadChanges(false, true, false, false, false, false);
            } else if (isTrackAgent) {
              dialPadChanges(false, false, false, false, false, false);
              incomingCallUserData(false, storeData);
            } else {
              if (callDetails.length >= 1) {
                //changed
                dialPadChanges(false, true, false, false, false, false);
                dispatch(CallPopUpAgentActions.pushCallDetails(storeData));
              } else {
                dispatch(CallPopUpAgentActions.pushCallDetails(storeData));
                dispatch(CallPopUpAgentActions.setCallingval(true));
              }
            }
          }
        } else if (monitor === "LISTEN" || "BARGE" || "WHISPER") {
          if (e !== undefined && e.toString().includes("call-connected-info")) {
            let callInfo: any = new DOMParser().parseFromString(e, "text/xml");
            let connectionStatus: string = callInfo.getElementsByTagName("call-connected-info")[0].getAttribute("callstatus");

            if (connectionStatus === "connected") {
              dispatch(CallPopUpAgentActions.TrackDataUpdate());
              if (monitor === "LISTEN") {
                listenInterval = setInterval(() => {
                  dispatch(CallPopUpAgentActions.agentTrackTimer());
                }, 1000);
              }
            }
          } else {
            if (monitor === "Transfer") {
            }
            dispatch(CallPopUpAgentActions.MonitorType(monitor));
          }
        }
      });
      thissession.on("message", function (e: any) {
        console.log('[thissession] - message', e);
      });

      thissession.on("referInviteSent", function (e: any) {
        console.log('[thissession] - referInviteSent', e);
      });

      thissession.on("progress", function () {
        console.log('[thissession] - progress');
        const popUpDetails: any = store.getState().callPopUpDetails;
        const { isTrackAgent } = popUpDetails;
        if (!isTrackAgent) {
          dispatch(CallPopUpAgentActions.updateCallState("Ringing"));
        }
      });

      thissession.on("bye", function () {
        console.log("bye calling");
        console.log('[thissession] - bye calling');


        const popUpDetails: any = store.getState().callPopUpDetails;
        let { incomingCallDetail, callDetails, isTrackAgent, trackAgentDetail, ShowCallerDetails } = popUpDetails;
        const chatReducer: any = store.getState().chatReducer;
        let { sessionList } = chatReducer;
        endCallNlpApi(incomingCallDetail);
        console.log("bye calling   isTrackAgent", isTrackAgent, sessionList);
        if (incomingCallDetail.callStatus === "Ringing") {
          ringer?.pause?.();
          if (callDetails.length >= 2) {
            CallUnHold(callDetails[callDetails.length - 2].useragent);
          } else if (incomingCallDetail.bound === "OUT" && callDetails.length === 1) {
            dispatch(CallPopUpAgentActions.setSpliceCallDetails(sessionGet));
          }
        }
        // dispatch(CallPopUpAgentActions.callEnded());
        // callPopUpFunction(false, {}, false);
        presenceChangeToReady();
      });

      const bindRemoteAudio = () => {
        const remoteMedia = document.getElementById("remoteVideo") as HTMLMediaElement | null;
        const pc = thissession?.sessionDescriptionHandler?.peerConnection;
        if (!remoteMedia || !pc?.getReceivers) return;
        const remoteStream = new MediaStream();
        pc.getReceivers().forEach((receiver: any) => {
          const track = receiver?.track;
          if (track && track.kind === "audio") {
            remoteStream.addTrack(track);
          }
        });
        remoteMedia.srcObject = remoteStream;
        remoteMedia.autoplay = true;
        const playPromise = remoteMedia.play?.();
        if (playPromise?.catch) {
          playPromise.catch((err: any) => console.warn("Remote audio autoplay blocked:", err));
        }
      };

      thissession.on("trackAdded", function () {
        console.log('[thissession] - trackAdded');
        const popUpDetails: any = store.getState().callPopUpDetails;
        let { isTrackAgent } = popUpDetails;
        if (isTrackAgent) {
          dialPadChanges(false, false, false, false, false, false);
        }
        bindRemoteAudio();
        const pc = thissession?.sessionDescriptionHandler?.peerConnection;
        if (pc && !pc.ontrack) {
          pc.ontrack = () => {
            console.log("[thissession] - peerConnection ontrack");
            bindRemoteAudio();
          };
        }
      });

      thissession.on("reject", function (response: any, cause: any) {
        console.log('[thissession] - reject', response, cause);
        if (cause === "Not Found") {
        } else if (cause === "SIP Failure Code") {
        } else if (cause === "Temporarily Unavailable") {
        } else if (cause === "Canceled") {
        }
      });
      thissession.on("terminated", async function (data: any, cause: any) {
        await handelTerminate(data, cause)
      });
      thissession.on("failed", async function (data: any, cause: any) {
        console.log('[thissession] - failed', data, cause);
        await handelTerminate(data, cause)
      });
    } catch (error) { }
  };

  const handelTerminate = async (data: any, cause: any) => {
    try {
      accpetCallBeforeTerminated = true
      console.log('[thissession] - handelTerminate', data, cause);
      // const dispositionData: any = store.getState().dispositionData;
      const userDetails = GetItemFromStorage(LocalStorage.USER_DETAILS);
      // let { started, storeInterval } = dispositionData;
      const popUpDetails: any = store.getState().callPopUpDetails;
      let { incomingCallDetail, callDetails, isMergeButton, isTrackAgent, mergedCall, isTransferSuccess, trackAgentDetail } = popUpDetails;
      console.log(
        "[TERMINATE] Data", data,
        "\n cause", cause,
        "\ncallDetails", callDetails,
        "\n incomingCallDetail", incomingCallDetail
      );
      // const campaign: any = store.getState().outboundcampaign
      const roomData: any = store.getState().chatReducer;
      const { storeIntentForClearTimer, callSessionId }: any = roomData;
      let fromcaller: any;
      let ringer: any = document.getElementById("ringer");
      ringer?.pause?.();
      if (data !== undefined && !isTrackAgent) {
        if (data?.statusCode !== 408) {
          fromcaller = typeof data.from.uri.normal.user === "string" ? data.from.uri.normal.user : data.from.uri.normal.user.toString();
        }
        console.log("TERMINATE by", fromcaller);
        if (cause === "Canceled") {
          //agent not attended
          // console.log("TERMINATE  callSessionId", callSessionId, "roomData?.incomingCallInfo?.outboundCampaign", campaign?.incomingCallInfo?.outboundCampaign, "roomData?.autoAnswer", campaign?.autoAnswer);
          dispatch(ChatActions.setCallSessionList({ state: true, sessionId: callSessionId ?? "" }));
          dispatch(CallPopUpAgentActions.NotAttendedCall());
          dispatch(CallPopUpAgentActions.clearIncomingCallDetail());
          // dispatch(outboundCampaignSliceAction.clearIncomingCallInfo());

          dispatch(CallPopUpAgentActions.setCloseCall(true));
          // if (campaign?.incomingCallInfo?.outboundCampaign && !campaign?.autoAnswer) {
          // presenceChangeToReady()
          // }

        } else if (cause === "BYE") {
          ringer?.pause?.();
          if (isTrackAgent) {
            transferChanges(false, {});
            dispatch(CallPopUpAgentActions.MonitorType(""));
            clearInterval(listenInterval)
          } else if (callDetails.length > 1) {
            console.log("out >>>>>>>>>");
            if (incomingCallDetail.callStatus === "Ringing" && incomingCallDetail.bound === "OUT") {
              dispatch(CallPopUpAgentActions.CallTransferingpopup(true));
              dispatch(CallPopUpAgentActions.terminateNormal(fromcaller));
              dispatch(CallPopUpAgentActions.updateTransferType({ transfer_type: "" }));
            } else if (incomingCallDetail.callStatus === "accepted" && callDetails.length === 2) {
              console.log(">>>>>>>>>>>>>>>>>>>", incomingCallDetail, callDetails);
              dispatch(CallPopUpAgentActions.removeCallDetails(incomingCallDetail));

              if (isMergeButton === false) {
                dispatch(CallPopUpAgentActions.forMergeBye(fromcaller));
              }
              else {
                dispatch(CallPopUpAgentActions.terminateNormal(fromcaller));
                dispatch(CallPopUpAgentActions.setScondaryagent(incomingCallDetail));
                CallUnHold(callDetails[callDetails.length - 2].useragent);
                //dispatch(outboundCampaignSliceAction.clearIncomingCallInfo());
              }
            }
          } else {
            if (incomingCallDetail.isConference) {
              // console.log(mergedCall, started, "mergedCall,started");
              const userDetails = GetItemFromStorage(LocalStorage.USER_DETAILS);
              console.log("Geting", fromcaller === userDetails.ext);
              if (fromcaller !== userDetails.ext && `${fromcaller}`.length === 3) {
                // if (!started) {
                //   clearInterval(storeInterval);
                //   let InterValData = setInterval(() => {
                //     dispatch(dispositionSliceActions.startDispositionTimer());
                //   }, 1000);
                //   dispatch(dispositionSliceActions.storeIntervalData(InterValData));
                // }
                // dispatch(dispositionSliceActions.setDispositionOpen({ isOpenDisPositionStatus: true }));
                // hassAccess('Disposition/regenerate') && summarySubjectHandlerForCall(callSessionId, dispatch, 0)
                dispatch(ChatActions.setCallSessionList({ state: true, sessionId: callSessionId ?? "" }));
                dispatch(CallPopUpAgentActions.closemergedCall());
              }
              else {
                if (incomingCallDetail.callType === "conCall" && incomingCallDetail?.contact?.length > 0) {
                  const pstn = incomingCallDetail?.contact.filter((i: any) => i.to === fromcaller)
                  console.log('incomingCallDetail---------', incomingCallDetail)
                  console.log('pstn---------', pstn)
                  if (pstn.length > 0) {
                    // dispatch(dispositionSliceActions.setDispositionOpen({ isOpenDisPositionStatus: true }));
                    // hassAccess('Disposition/regenerate') && summarySubjectHandlerForCall(callSessionId, dispatch, 0)
                    dispatch(ChatActions.setCallSessionList({ state: true, sessionId: callSessionId ?? "" }));
                  }
                }
              }
              // dispatch(dispositionSliceActions.setDispositionOpen({ isOpenDisPositionStatus: true }));
              dispatch(CallPopUpAgentActions.forSipFailiure(fromcaller));
            } else {
              try {
                console.log("in >>>>>>>>>", callDetails, incomingCallDetail);
                const { ShowCallerDetails }: any = store.getState().callPopUpDetails;
                if (callDetails.length !== 0) {
                  // if (!mergedCall) {
                  //   if (!started) {
                  //     clearInterval(storeInterval);
                  //     let InterValData = setInterval(() => {
                  //       dispatch(dispositionSliceActions.startDispositionTimer());
                  //     }, 1000);
                  //     dispatch(dispositionSliceActions.storeIntervalData(InterValData));
                  //   }
                  //   dispatch(dispositionSliceActions.setDispositionOpen({ isOpenDisPositionStatus: true }));
                  // }
                  if (callDetails.length === 1 && callDetails?.[0]?.bound === "OUT" && callDetails?.[0]?.isTransfer == "") {
                    console.log("in out >>>>>>>>>", callDetails);

                    // dispatch(dispositionSliceActions.setDispositionOpen({ isOpenDisPositionStatus: true }));
                    // hassAccess('Disposition/regenerate') && summarySubjectHandlerForCall(callSessionId, dispatch, 0)
                    dispatch(ChatActions.setCallSessionList({ state: true, sessionId: callSessionId ?? "" }));
                    // dispatch(outboundCampaignSliceAction.clearIncomingCallInfo());

                  }
                  else if (fromcaller && callDetails.length === 1 && callDetails?.[0]?.AllAgent?.length > 0) {
                    if (callDetails?.[0]?.AllAgent?.includes(fromcaller)) {
                      // dispatch(dispositionSliceActions.setDispositionOpen({ isOpenDisPositionStatus: true }));
                      // hassAccess('Disposition/regenerate') && summarySubjectHandlerForCall(callSessionId, dispatch, 0)
                      dispatch(ChatActions.setCallSessionList({ state: true, sessionId: callSessionId ?? "" }));
                    }
                  }
                  // dispatch(dispositionSliceActions.setDispositionOpen({ isOpenDisPositionStatus: true }));
                  dispatch(CallPopUpAgentActions.terminateNormal(fromcaller));
                  //dispatch(outboundCampaignSliceAction.clearIncomingCallInfo());

                  // clearInterval(storeIntentForClearTimer);
                } else {
                  if (!ShowCallerDetails) {
                    // dispatch(dispositionSliceActions.setDispositionOpen({ isOpenDisPositionStatus: true }));
                    dialPadChanges(false, false, false, false, false, false);
                    // hassAccess('Disposition/regenerate') && summarySubjectHandlerForCall(callSessionId, dispatch, 0)
                    dispatch(ChatActions.setCallSessionList({ state: true, sessionId: callSessionId ?? "" }));
                  }
                }
              } catch (error) {
                console.log('error------', error)
              }
            }
          }
          presenceChangeToReady();
        } else if (cause === "SIP Failure Code") {
          if (isTrackAgent) {
            transferChanges(false, {});
            dispatch(CallPopUpAgentActions.MonitorType(""));
            clearInterval(listenInterval)
          } else {
            dispatch(CallPopUpAgentActions.forSipFailiure(fromcaller));
            // dispatch(outboundCampaignSliceAction.clearIncomingCallInfo());

          }
        } else if (cause === "Unavailable") {
          if (callDetails.length === 1) {
            dispatch(CallPopUpAgentActions.forUnavailable());
            // dispatch(outboundCampaignSliceAction.clearIncomingCallInfo());

          } else {
            dispatch(CallPopUpAgentActions.forUnavailable());
            // dispatch(outboundCampaignSliceAction.clearIncomingCallInfo());
            let index = _.findIndex(callDetails, (e: any) => e.guid === incomingCallDetail.guid);
            if (index > -1) {
              CallUnHold(callDetails["useragent"]);
            }
          }
        } else if (cause === "Busy") {
          if (callDetails.length === 0) {
            if (isTrackAgent) {
              transferChanges(false, {});
              dispatch(CallPopUpAgentActions.MonitorType(""));
              clearInterval(listenInterval)
            } else {
              dispatch(CallPopUpAgentActions.clearCallForBusy());
              // dispatch(outboundCampaignSliceAction.clearIncomingCallInfo());

            }
          } else {
            if (isTrackAgent) {
              transferChanges(isTrackAgent, {});
              clearInterval(listenInterval)
              dispatch(CallPopUpAgentActions.MonitorType(""));

            } else {
              dispatch(CallPopUpAgentActions.clearCallForBusyforMore());
            }
          }
        } else if (cause === "Invalid status: 5") {
          console.log("TERMINATE  invalid status", callSessionId);
          // dispatch(ChatActions.setCallSessionList({ state: true, sessionId: callSessionId ?? "" }));
          dispatch(CallPopUpAgentActions.NotAttendedCall());
          dispatch(CallPopUpAgentActions.clearIncomingCallDetail());
          // dispatch(outboundCampaignSliceAction.clearIncomingCallInfo());
          dispatch(CallPopUpAgentActions.setCloseCall(true));

        } else {
          dispatch(CallPopUpAgentActions.callEnded());
          presenceChangeToReady();

        }
      } else if (cause === "WebRTC Error") {
        console.log("TERMINATE  WebRTC Error", callSessionId);
        const micState = await waitForMicrophonePermission();
        dispatch(CallPopUpAgentActions.callEnded());
        presenceChangeToReady();
      } else {
        if (callDetails.length >= 1) {
          if (callDetails.length === 1) {
            // if (started) {
            // clearInterval(storeInterval);
            let InterValData = setInterval(() => {
              // dispatch(dispositionSliceActions.startDispositionTimer());
            }, 1000);
            // dispatch(dispositionSliceActions.storeIntervalData(InterValData));
            // }
            // dispatch(dispositionSliceActions.setDispositionOpen({ isOpenDisPositionStatus: true }));
            // hassAccess('Disposition/regenerate') && summarySubjectHandlerForCall(callSessionId, dispatch, 0)
            // dispatch(ChatActions.setCallSessionList({ state: true, sessionId: callSessionId ?? "" }));
            dispatch(CallPopUpAgentActions.terminateNormal(incomingCallDetail?.from));
            // dispatch(outboundCampaignSliceAction.clearIncomingCallInfo());

          }
        } else {
          // if (!started) {
          //   clearInterval(storeInterval);
          // let  InterValData = setInterval(() => {
          //     dispatch(dispositionSliceActions.startDispositionTimer());
          //   }, 1000);
          //   dispatch(dispositionSliceActions.storeIntervalData(InterValData));
          // }
          // console.log("called>>>>>>>>");
          // dispatch(dispositionSliceActions.setDispositionOpen());
          dispatch(CallPopUpAgentActions.callEnded());
        }
      }
      dispatch(CallPopUpAgentActions.callEnded());
    } catch (error) {
      console.log(error)
      dispatch(CallPopUpAgentActions.callEnded());
      callPopUpFunction(false, {}, false);
    }
  }

  const acceptCallFunction = async (userAgent: any, incomecaller: any) => {
    try {
      console.log("useragent:", userAgent, "incomecaller:", incomecaller);
      const LoginCcaasUserDetails1 = GetItemFromStorage(LocalStorage.USER_DETAILS);
      const { answeredDuration, incomingcallDetails } = store.getState().chatReducer;
      const popUpDetails: any = store.getState().callPopUpDetails;
      let { callSessionDetailswhilePopup, incomingCallDetail, closeCall } = popUpDetails;
      const popupSource =
        callSessionDetailswhilePopup && Object.keys(callSessionDetailswhilePopup).length > 0
          ? callSessionDetailswhilePopup
          : (incomingCallDetail ?? {});
      const userDetails = GetItemFromStorage(LocalStorage.USER_DETAILS);
      const LoginUserDetails: any = userDetails;
      let guid = uuidv4();
      let tocaller_id = LoginUserDetails?.ext;
      const resolvedIncomingCaller =
        `${incomecaller ?? popupSource?.fromcaller ?? popupSource?.from ?? ""}`.trim();
      let storeData = {
        guid: guid,
        useragent: userAgent,
        from: resolvedIncomingCaller,
        to: `${tocaller_id ?? ""}`.toString(),
        muteState: false,
        holdState: false,
        medium: "audio",
        bound: "IN",
        contact: [],
        contactDetails: popupSource?.contactDetails ?? {},
        isCurrent: true,
        isNewCall: true,
        isConference: false,
        callStatus: "accepted",
        callType: "direct",
        agentType: popupSource?.agentType ?? "PRIMARY",
        typeOf: popupSource?.typeOf ?? "EXTERNAL",
        sessionId: `${popupSource?.sessionId ?? ""}`.trim(),
        queueName: popupSource?.queueName ?? "",
        callduration: "00:00:00",
        isCustomer: popupSource?.isCustomer ?? true,
      };

      console.log("acceptCallFunction>>>>>>", closeCall)
      if (!closeCall) {
        await dispatch(CallPopUpAgentActions.pushCallDetails(storeData));
        console.log('Session id for accept call', `${popupSource?.sessionId ?? ""}`.trim())
        dispatch(ChatActions.setCallSessionId(callSessionDetailswhilePopup?.sessionId?.trim() ?? ""));
        if (storeData.typeOf === "EXTERNAL") {
          callTimerFunc(true);
          callPopUpFunction(false, {}, true);
          CallAccept(userAgent, "audio", resolvedIncomingCaller);
          setUpListeners(userAgent, resolvedIncomingCaller, "audio", "IN", "", guid, false);
          const body: any = {
            // CallHistoryId: chatSessionData?.CallHistoryId,
            domainId: LoginUserDetails?.domainId,
            AgentId: LoginCcaasUserDetails1?.UserID,
            answeredDuration: answeredDuration ?? 0,
          }
          socketService.callNotifyDialDuration(body);
        } else {
          callTimerFunc(false);
          callPopUpFunction(false, {}, true);
          dialPadChanges(false, true, false, false, false, false);
          CallAccept(userAgent, "audio", resolvedIncomingCaller);
          setUpListeners(userAgent, resolvedIncomingCaller, "audio", "IN", "", guid, false);
        }
      }
      let ringer: any = document.getElementById("ringer");
      ringer?.pause?.();
      dispatch(CallPopUpAgentActions.setCloseCall(false));
    } catch (error) {
      console.log('acceptCallFunction err:', error)
    }
  };

  const declineCallFunction = (userAgent: any) => {
    RejectCall(userAgent);
    callPopUpFunction(false, {}, false);
    let ringer: any = document.getElementById("ringer");
    ringer?.pause?.();
  };

  const CallMuteFunction = (incomingCallDetail?: any) => {
    Mute_AudioCall(incomingCallDetail.useragent);
    dispatch(CallPopUpAgentActions.setCallDetailsMuteState(incomingCallDetail.guid));
  };

  const superVisorMuteFunction = () => {
    const popUpDetails: any = store.getState().callPopUpDetails;
    let { trackAgentDetail } = popUpDetails;
    Mute_AudioCall(trackAgentDetail.useragent);
  };

  const superVisorUnMuteFunction = () => {
    const popUpDetails: any = store.getState().callPopUpDetails;
    let { trackAgentDetail } = popUpDetails;
    UnMute_AudioCall(trackAgentDetail.useragent);
  };

  const transferChanges = (isTrackAgent: any, trackAgentDetail: any) => {
    dispatch(
      CallPopUpAgentActions.TrackChanges({
        isTrackAgent: isTrackAgent,
        trackAgentDetail: trackAgentDetail,
      })
    );
  };
  const CallUnMuteFunction = (incomingCallDetail?: any) => {
    UnMute_AudioCall(incomingCallDetail.useragent);
    dispatch(CallPopUpAgentActions.setCallDetailsUnMuteState(incomingCallDetail.guid));
  };

  const CallHoldFunction = (incomingCallDetail?: any) => {
    CallHold(incomingCallDetail.useragent);
    dispatch(CallPopUpAgentActions.setCallDetailsHoldState(incomingCallDetail.guid));
  };

  const CallUnHoldFunction = (incomingCallDetail?: any) => {
    CallUnHold(incomingCallDetail.useragent);
    dispatch(CallPopUpAgentActions.setCallDetailsUnHoldState(incomingCallDetail.guid));
  };

  const sendDTMF = (incomingCallDetail: any, val: any) => {
    CallConference(incomingCallDetail.useragent, val);
  };

  const makeReInvite = (sessionId: any, type: any, AgentData: any) => {
    const popUpDetails: any = store.getState().callPopUpDetails;
    let { isTrackAgent } = popUpDetails;
    const callLoginUserAgent: any = store.getState().callLoginUserAgent;
    let { userAgent } = callLoginUserAgent;
    let ccaasFeatreReq = [`CCAAS-FEATURE-REQ: ${type}`, `CCAAS-SESSION-ID: ${sessionId}`, `CCAAS-CALLER-TYPE:SUPERVISOR`];
    //BARGE/LISTEN/WHISPER
    let medium: any;
    const userDetails = GetItemFromStorage(LocalStorage.USER_DETAILS);
    const LoginUserDetails: any = userDetails;
    var uri = `sip:${LoginUserDetails.ext}@${LoginUserDetails.domain_name}`;
    let useragent = MakeReInviteCall(userAgent, uri, medium, "unKnown", ccaasFeatreReq);
    setUpListeners(useragent, LoginUserDetails.ext, "audio", "ACTIVITY", type);
    let TrackAgentDetailsset: any = {};
    TrackAgentDetailsset.AgentData = AgentData;
    TrackAgentDetailsset.sessionId = sessionId;
    TrackAgentDetailsset.useragent = useragent;
    TrackAgentDetailsset.muteState = false;
    // if (type === "LISTEN" && !isTrackAgent) {
    //   getCallSessionById(sessionId).then(async (response: any) => {
    //     if (response.data.statusCode === 200) {
    //       TrackAgentDetailsset.sessionDetails = response.data.result[0];
    //       TrackAgentDetailsset.sessionDetails.Number = response.data.result[0]?.customerNumber;
    //       TrackAgentDetailsset.sessionDetails.showState = true;
    //       TrackAgentDetailsset.sessionDetails.queueName = response.data.result[0].queueName;
    //       TrackAgentDetailsset.sessionDetails.notLive = true;
    //     } else {
    //       TrackAgentDetailsset.sessionDetails = {};
    //     }
    //     await getContactDetails(TrackAgentDetailsset.sessionDetails.Number, "Call")
    //       .then((res: any) => {
    //         if (res.data.statusCode === 200) {
    //           console.log(res, "resdatadatadata");
    //           TrackAgentDetailsset.sessionDetails.calledPersonDetails = res?.data?.getContactNumberRes.length > 0 ? res?.data?.getContactNumberRes[0] : {};
    //           TrackAgentDetailsset.sessionDetails.calledPersonDetails.tags =
    //             JSON.parse(res?.data?.getContactNumberRes[0]?.tags?.trim().length > 0 ? res?.data?.getContactNumberRes[0]?.tags : "null") ?? [];
    //         } else {
    //           TrackAgentDetailsset.sessionDetails.calledPersonDetails = {};
    //         }
    //         TrackAgentDetailsset.status = "connecting";
    //         TrackAgentDetailsset.Duration = "00:00:00";
    //         dispatch(CallPopUpAgentActions.TrackChanges({ isTrackAgent: true, trackAgentDetail: TrackAgentDetailsset }));
    //       });
    //   });
    // }
  };
  const unregister = () => {
    const callLoginUserAgent: any = store.getState().callLoginUserAgent;
    let { userAgent } = callLoginUserAgent;
    try {
      Unregistration(userAgent);
      dispatch(CallUserAgentActions.reset());
      dispatch(CallPopUpAgentActions.reset());
      dispatch(CallPopUpAgentActions.reset());
    } catch (e: any) { }
  };

  const SupervisorEndCall = (useragent: any) => {
    EndCall(useragent);
  };

  const EndCallfunction = (endCallUserDetails: any) => {
    console.log("presenceChangeToReady :: called")
    presenceChangeToReady();
    const popUpDetails: any = store.getState().callPopUpDetails;
    let { callDetails } = popUpDetails;
    if (callDetails.length === 0) {
      incomingCallUserData(false, {});
    }
    EndCall(endCallUserDetails.useragent);
    if (callDetails.length > 1 && endCallUserDetails?.isCurrent === true) {
      dispatch(CallPopUpAgentActions.removeCallDetails(endCallUserDetails))
      CallUnHold(callDetails[callDetails.length - 2].useragent);
    }
  };


  const makeCallFunctionForAll = async (Input: any, transfertype: any, contactDetails?: any) => {
    const roomData: any = store.getState().chatReducer;
    const popUpDetails: any = store.getState().callPopUpDetails;
    let { incomingCallDetail, callDetails, isTransfer } = popUpDetails;
    const userDetails = GetItemFromStorage(LocalStorage.USER_DETAILS);
    const LoginCcaasUserDetails: any = GetItemFromStorage(LocalStorage.USER_DETAILS);
    const LoginUserDetails: any = userDetails;
    if (Array.isArray(callDetails) && callDetails.length > 0) {
      dispatch(CallPopUpAgentActions.setIsCurrent());
    }

    let data: any = {
      callStatus: "Dialing",
      to: LoginUserDetails.ext.toString(),
      bound: "OUT",
      from: Input,
      callType: "direct",
      isCurrent: true,
      guid: uuidv4(),
      agnetType: "PRIMARY",
      typeOf: isTransfer ? "AGENT" : "EXTERNAL",
      contactDetails: contactDetails ?? {},
      isConference: false,
      isCustomer: (Array.isArray(callDetails) && contactDetails?.length) === 1 || isTransfer ? false : true,
      sessionId: roomData?.currentSession?.sessionId ?? "",
      queueName: LoginCcaasUserDetails?.queueId ?? "",
      callduration: incomingCallDetail?.callduration
    };
    if (isTransfer) {
      let transferdata: any = {
        session_id: roomData?.currentSession?.sessionId,
        req_type: "AGENT_TRANSFER",
        transfer_type: transfertype, //1 - Agent 2 - PSTN 3-Queue
        ddi: parseInt(Input),
        email: (parseInt(contactDetails?.ext) === parseInt(Input)) ? contactDetails?.emailId : "",
        name: contactDetails?.UserName ? contactDetails?.UserName : contactDetails?.firstName ? contactDetails?.firstName : '',
        userId: `${contactDetails.UserID ?? ''}`
      };

      const transferfromdata = {
        domainId: LoginCcaasUserDetails?.domainId,
        toExt: parseInt(Input),
        fromExt: LoginCcaasUserDetails?.UserName,
        transferDataDetails: {
          id: uuidv4(),
          from: `${LoginCcaasUserDetails?.domainId}_${LoginCcaasUserDetails?.ext}`,
          to: roomData?.currentSession?.sessionId,
          sessionId: roomData?.currentSession?.sessionId,
          message: '<p>Transfer From ' + LoginCcaasUserDetails?.UserName + '</p>',
          sender: {
            uuid: LoginCcaasUserDetails?.UserID,
            Name: LoginCcaasUserDetails?.UserName,
            Email: LoginCcaasUserDetails?.emailId
          },
          isBot: false,
          isCustomer: false,
          isTransfer: true,
          messageType: 'Text',
          sendAt: Math.floor(Date.now() / 1000),
          chatIconLogo: '',
          timestamp: Math.floor(Date.now() / 1000)
        }
      };
      const transferTodata = {
        ...transferfromdata.transferDataDetails,
        message: '<p>Transfer To ' + contactDetails?.UserName + '</p>'
      }

      dispatch(CallPopUpAgentActions.updatecurrentState(incomingCallDetail.guid));
      dispatch(CallPopUpAgentActions.pushCallForTransfer(data));
      socketService.transfer(transferdata);
      socketService.transferFrom(transferfromdata);
      dispatch(CallPopUpAgentActions.setTransferto(transferTodata));
      CallTransferFunction(Input);
      callDetails.map((item: any) => CallHold(item.useragent));
    } else {
      const micState = await waitForMicrophonePermission();
      if (micState !== "granted") {
        console.warn("Microphone permission not granted. Outbound call cancelled.");
        return;
      }
      const useragent = makeCallFunction(Input, data.guid);
      makeCallReduxSet({ ...data, useragent });
    }
  };

  const makeCallReduxSet = (data: any) => {
    dialPadChanges(false, true, false, false, false, false);
    dispatch(CallPopUpAgentActions.pushCallDetails(data));
    incomingCallUserData(true, data);
  };
  const makeCallOutboundReduxSet = (data: any) => {
    dialPadChanges(false, true, false, false, false, false);
    dispatch(CallPopUpAgentActions.pushCallDetails(data));
    incomingCallUserData(true, data);
  };

  const conferenceData = (callDetails: any) => {
    dispatch(CallPopUpAgentActions.setCurrentCallDetails(callDetails));
  };

  const incomingCallUserData = (status: any, data: any) => {
    dispatch(
      CallPopUpAgentActions.setIncomingCallDetails({
        status: status,
        incomingCallDetail: data,
      })
    );
  };
  const makeTransferReduxSet = (callDetails: any, data: any) => {
    incomingCallUserData(true, data);
    conferenceData(callDetails);
  };
  const callTimerFunc = (type?: boolean) => {
    clearInterval(CallTimer);
    CallTimer = setInterval(() => {
      dispatch(CallPopUpAgentActions.updateCallTimer());
      if (type) {
        dispatch(ChatActions.updateTimerForCall(store.getState()));
      }
    }, 1000);
    dispatch(ChatActions.clearsessionTimer(CallTimer));
  };
  const basicHold = (userAgent: any) => {
    CallHold(userAgent);
  };
  const callPopUpFunction = (acceptCallRoom: boolean, callSessionDetailswhilePopup: any, isCallAccepted: boolean) => {
    let data = {
      acceptCallRoom: acceptCallRoom,
      callSessionDetailswhilePopup: callSessionDetailswhilePopup,
      isCallAccepted: isCallAccepted,
    };
    console.log("TESTING POPUP DETAILS: ", data, callSessionDetailswhilePopup);
    dispatch(CallPopUpAgentActions.setWholeData(data));
  };

  const dialPadChanges = (isDialPadOpen: any, isCalling: any, isTransfer: any, isTransferSuccess: any, isMergeButton: any, isTransferButton: any) => {
    dispatch(
      CallPopUpAgentActions.DialPadChanges({
        isDialPadOpen: isDialPadOpen,
        isCalling: isCalling,
        isTransfer: isTransfer,
        isTransferSuccess: isTransferSuccess,
        isMergeButton: isMergeButton,
        isTransferButton: isTransferButton,
      })
    );
  };

  const callBackTimerFunc = (num?: string) => {
    const popUpDetails: any = store.getState().callPopUpDetails;
    let { callBackDetails } = popUpDetails;
    let timer: any;
    timer = setInterval(() => {
      const popUp2Details: any = store.getState().callPopUpDetails;
      let { callBackTimer, reject } = popUp2Details;
      if (callBackTimer > 1 && popUp2Details?.isCallBackWaiting) {
        let data1 = {
          isCallBackWaiting: true,
          callBackDetails: callBackDetails,
          callBackTimer: callBackTimer - 1,
          reject: false
        };
        dispatch(CallPopUpAgentActions.setCallBackInfo(data1));
      } else if (reject) {
        let data2 = {
          isCallBackWaiting: false,
          callBackDetails: {},
          callBackTimer: 30,
          reject: false
        };
        dispatch(CallPopUpAgentActions.setCallBackInfo(data2));
        clearInterval(timer)
        // callChatService.callBackReject({ ...callBackDetails, status: 'rejected' });
      } else if (callBackTimer === 1) {
        let data3 = {
          isCallBackWaiting: false,
          callBackDetails: {},
          callBackTimer: 30,
          reject: false
        };
        dispatch(CallPopUpAgentActions.setCallBackInfo(data3));
        socketService.callBackReject({ ...callBackDetails, status: 'rejected' });
        clearInterval(timer)
      }
    }, 1000);
  };

  const presenceChangeToReady = () => {
    const popUpDetails: any = store.getState().callPopUpDetails;
    const LoginCcaasUserDetails = GetItemFromStorage(LocalStorage.USER_DETAILS);
    let { incomingCallDetail } = popUpDetails;
    const { status: agentStatus } = store.getState().agentState;
    const { users } = store.getState().userList;
    const presence = sessionStorage.getItem('presence') || "Not Ready"
    let status: any = null
    if (agentStatus?.length > 0) {
      [status] = agentStatus.filter((i: any) => { return presence === i?.statusName?.trim() })
    }
    let enableStatus: any = null
    if (users?.length > 0) {
      [enableStatus] = users?.filter((i: any) => { 
        return (LoginCcaasUserDetails?.Email || LoginCcaasUserDetails?.username) === i?.emailId 
      })
    }
    console.log('presenceService call_service', status, enableStatus, presence);
    presenceService?.io?.emit("updatestate_backend", {
      domainId: LoginCcaasUserDetails?.domainId,
      isVoice: (enableStatus?.voice === 1 && status?.isVoice === 1 && (popUpDetails?.incomingCallDetail ? Object.keys(popUpDetails?.incomingCallDetail ?? {}).length : 0) === 0) ? 1 : 0,
      isVoiceCallTransfer: status?.isVoiceCallTransfer ?? 0,
      isChat: (enableStatus?.chat === 1 && status?.isChat === 1) ? 1 : 0,
      isChatTransfer: status?.isChatTransfer ?? 0,
      isEmail: (enableStatus?.email === 1 && status?.isEmail === 1) ? 1 : 0,
      isEmailTransfer: status?.isEmailTransfer ?? 0,
      stateTime: Math.floor(Date.now() / 1000),
      statusName: presence,
      ext: LoginCcaasUserDetails?.ext,
      email: LoginCcaasUserDetails?.Email,
      userId: LoginCcaasUserDetails?.UserID,
      routingProfileId: LoginCcaasUserDetails?.routing_profile_id,
      userName: LoginCcaasUserDetails?.UserName,
      roleId: LoginCcaasUserDetails?.roleid,
      agentStatus: (incomingCallDetail ? Object.keys(incomingCallDetail).length : 0) > 0 ? 0 : 1,
      skill: JSON.parse(enableStatus?.addSkill ?? '[]'),
      routingMethod: enableStatus?.routingMethod ?? 0,
      inboundCall: enableStatus?.inboundCall,
      outboundCampaignCall: enableStatus?.outboundCampaignCall
    });
  }

  const setNotification = (customerName: any, FromCallerName: any) => {
    try {
      if ("Notification" in window) {
        Notification.requestPermission().then((permission) => {
          if (permission === "granted") {
            var notificationBrowser = new Notification("CCAAS-Worktual", {
              body: `Call From ${customerName ?? "New Customer"} -  ${FromCallerName} in worktual contact center`,
              icon: "",
            });
            console.log("got invite requestPermission Notification");

            notificationBrowser.onclick = function (event) {
              event.preventDefault();

              window.parent.focus();

              notificationBrowser.close();
            };
          }
        });
      }
    } catch (error) {
      console.log(error)
    }
  }

  const endCallNlpApi = ( incomingCallDetail :any)=> {      
      if(incomingCallDetail?.contactDetails?.customer_id){
        updateNLPcontactDeals({ 
          coid: incomingCallDetail?.contactDetails?.customer_id || 0, 
          domainId: incomingCallDetail?.contactDetails?.domain_id || 0, 
          emailId: incomingCallDetail?.contactDetails?.primary_email || "", 
          phoneNumber: incomingCallDetail?.contactDetails?.primary_phone_number  || ''
        });
      }
  }

  return {
    createCallConnection,
    makeCallFunction,
    acceptCallFunction,
    declineCallFunction,
    CallMuteFunction,
    CallUnMuteFunction,
    CallHoldFunction,
    sendDTMF,
    CallUnHoldFunction,
    EndCallfunction,
    makeReInvite,
    unregister,
    dialPadChanges,
    transferChanges,
    callPopUpFunction,
    CallTransferFunction,
    makeCallReduxSet,
    makeTransferReduxSet,
    makeCallFunctionForAll,
    incomingCallUserData,
    basicHold,
    superVisorUnMuteFunction,
    superVisorMuteFunction,
    SupervisorEndCall,
    callTimerFunc,
    callBackTimerFunc,
    CallOutboundCampaignFunction
  };
}

/*     const devices = await navigator.mediaDevices.enumerateDevices();
    const outputDevices = devices.filter(device => device.kind === 'audiooutput');
    const inputDevices = devices.filter(device => device.kind === 'audioinput');
    console.log('acceptCallFunction inputDevices', inputDevices)
    console.log('acceptCallFunction outputDevices', outputDevices)
    const defaultInputDevice = inputDevices.find(device => device.deviceId === 'default');
    const defaultOutputDevice = outputDevices.find(device => device.deviceId === 'default');
 
    const isDefaultInputOutput = defaultInputDevice && defaultOutputDevice;
    console.log('isDefaultInputOutput===>', isDefaultInputOutput)
    if (!isDefaultInputOutput) {
      // alert('Microphone access has been denied. Please enable it in your browser settings or exit incognito mode.')
      // return;
      alert("No access to microphone and speaker. Please check your device settings")
    } */
