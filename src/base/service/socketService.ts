import { io, Socket } from "socket.io-client";
import { jwtDecode } from "jwt-decode";
import { knowledgeDataSliceActions } from "../../store/slice/knowledgeBaseChat";
import store from "../../store/redux-store/root_store";
import { config } from "../constant";
import { contactsSliceActions } from "../../store/slice/contacts";
import { campaignSliceActions } from "../../store/slice/campaign";
import { analyticsSliceActions } from "../../store/slice/analytics";
import { reportSliceActions } from "../../store/slice/report";
import { commonDataSliceActions } from "../../store/slice/commonSlice/commonSlice";
import { GetItemFromStorage, SetItemInStorage } from "../customhooks/useStorage";
import { LocalStorage } from "../customhooks/localStorageKeys";


export default class SocketService {
    private static socket: Socket | null = null;

    constructor(token: string, dispatch: any) {
        const userData: any = jwtDecode(token);
        const loginSessionId :any = GetItemFromStorage(LocalStorage.LOGIN_SESSIONID);
        if (SocketService.socket) {
            SocketService.socket.disconnect();
        }
        
        // Connect new socket
        // SocketService.socket = io("http://localhost:5000", {
        SocketService.socket = io(config().SOCKET_URL, {
            path: "/campaign-socket-service",
            auth: { token, id: `${userData?.domainId}_${userData?.ext}` },
            transports: ["websocket"],
        });

        SocketService.socket.emit('joinRoom', `${userData.domainId}_${userData.ext}`);


        SocketService.socket.on("connect", () => {
            console.log("✅ socket connected", `${userData?.domainId}_${userData?.ext}`);
        });

        SocketService.socket.on("loginSession",(data :any)=>{
            console.log("Login_Session",data);
            // try {
            //     if(data?.sessionId!=loginSessionId){
            //         SetItemInStorage(LocalStorage.ACCESS_TOKEN, "");
            //         dispatch(commonDataSliceActions.setForceLogout(true));
            //     }
            // } catch (error) {
            //     console.log("err",error)
            // }
        })

        SocketService.socket.on("disconnect", (reason: any) => {
            console.log("⚠️ socket disconnected:", reason);
        });

        SocketService.socket.on("knowledgeMessageBot", (data: any) => {
            try {
                if (data?.progress) {
                    dispatch(knowledgeDataSliceActions.progress(true));
                } else {
                    dispatch(knowledgeDataSliceActions.progress(false));
                }
                dispatch(knowledgeDataSliceActions.setIsTyping(false));
                
                if(data?.Navigate){
                    dispatch(knowledgeDataSliceActions.setNavigate({path:data?.Navigate,count:1}))
                }

                if(data?.search){
                    dispatch(commonDataSliceActions.setContactFilter(data?.search));
                }
                
                if(data?.intent ==="docs_added" || data?.intent ==="knowledgebaserefresh"){
                    dispatch(commonDataSliceActions.setKnowledgeBaseDocAdded(true));
                }

                if(data?.intent ==="contactrefresh"){
                    dispatch(commonDataSliceActions.setContactRefresh(true));
                }

                if(data?.intent ==="emailconfigrefresh"){
                    dispatch(commonDataSliceActions.setEmailConfigRefresh(true));
                }

                if(data?.intent ==="smsconfigrefresh"){
                    dispatch(commonDataSliceActions.setSmsConfigRefresh(true));
                }

                if(data?.intent ==="apppushconfigrefresh" ){
                    dispatch(commonDataSliceActions.setAppPushRefresh(true));
                }

                if(data?.intent ==="archiveconfigrefresh" ){
                    dispatch(commonDataSliceActions.setArchiveRefresh(true));
                }
                
                if(data?.intent ==="connectorsrefresh" ){
                    dispatch(commonDataSliceActions.setConnectorsRefresh(true));
                }

                if(data?.intent ==="webpushrefresh" ){
                    dispatch(commonDataSliceActions.setWebPushRefresh(true))
                }
                if(data?.intent === "onboardrefresh"){
                  dispatch(commonDataSliceActions.setOnboardRefresh(true));  
                }
                 
                if(data?.intent ==="company_onboarded"){
                    dispatch(commonDataSliceActions.setCompanyOnboarded(true));
                }
                if(data?.intent ==="config_done"){
                     dispatch(commonDataSliceActions.setConfigDone(true));
                }
                if(data?.intent ==="integration_done"){
                     dispatch(commonDataSliceActions.setIntegrationDone(true));
                }
                if(data?.intent ==="whatsapp_config"){
                     dispatch(commonDataSliceActions.setWhatsAppConfig(true));
                }
                if (data?.intent === 'process_complete') {
                    dispatch(knowledgeDataSliceActions.reset());
                    dispatch(knowledgeDataSliceActions.isModalOpen(false));
                }
                let messages: any[] = store.getState().knowledgeBaseData?.knowledgeMessage || [];
                const lastMessage: any = messages[messages.length - 1]
                if ( lastMessage?.progress !== undefined && data?.progress && data?.sender === "botText") {
                    const updated = [...messages];
                    updated[updated.length - 1] = { ...lastMessage, ...data };
                    dispatch(knowledgeDataSliceActions.setknowledgeMessage(updated));
                } else {
                    dispatch(knowledgeDataSliceActions.setknowledgeMessage([...messages, data]));
                }
            } catch (error) {
                console.log(error)
            }
        });

        SocketService.socket.on("contactchatresponse", (data: any) => {
            try {
                if (data?.progress) {
                    dispatch(campaignSliceActions.progress(true));
                } else {
                    dispatch(campaignSliceActions.progress(false));
                }
                dispatch(contactsSliceActions.setIsTyping(false));
                let messages: any[] = store.getState().contactsData?.contactsMessage || [];
                if (data?.intent === "process_complete") {
                    dispatch(contactsSliceActions.setIsOpenContactModal(false));
                    dispatch(contactsSliceActions.reset());
                }
                dispatch(contactsSliceActions.setContactsMessage([...messages, data]));
            } catch (error) {
                console.log(error)
            }
        });

        SocketService.socket.on("campaignMessageBot", (data: any) => {
            try {
                console.log("data incoming:>>>",data);
                let messages: any[] = store.getState().campaignData.campaignMessage || [];
                let templateMessages :any [] = store.getState().campaignData.templateConversation||[];
                let currentChatId :any = store.getState().commonData.currentCampaignChatId;
                console.warn("currentchatid>>>>??",currentChatId)
                const lastMessage: any = messages[messages.length - 1]
                const lastTemplateMessage :any =templateMessages[templateMessages.length -1];
                if (data?.progress) {
                    dispatch(campaignSliceActions.progress(true));
                } else {
                    dispatch(campaignSliceActions.progress(false));
                }
                
                dispatch(campaignSliceActions.setIsTyping(false));

                if(data?.template_chat==1){
                    if (lastTemplateMessage?.messageId === data?.messageId && lastTemplateMessage?.progress !== undefined && data?.progress && data?.sender === "botText") {
                    const updated = [...templateMessages];
                    updated[updated.length - 1] = { ...lastTemplateMessage, ...data };
                    dispatch(campaignSliceActions.setTemplateSpace(updated));
                } else {
                   
                     dispatch(campaignSliceActions.setTemplateSpace([...templateMessages,data]));
                }
                   
                }else{
                if (data?.chatType === 'regenerate' || data?.chatType === 'editPrompt') {
                    let updated: any = [...messages];
                    const channelKey = data?.regenerate?.channels;
                    const idx = data?.regenerate?.index;

                    if (updated?.[idx]?.campaign_data?.channels?.[channelKey]) {
                        const newChannel = { ...updated[idx].campaign_data.channels[channelKey], template: data?.regenerate?.template, };

                        updated[idx] = {
                            ...updated[idx], campaign_data: {
                                ...updated[idx].campaign_data,
                                channels: { ...updated[idx].campaign_data.channels, [channelKey]: newChannel },
                            },
                        };
                        dispatch(campaignSliceActions.setTempleteView({ channels: channelKey, ...newChannel, index: idx }));
                    }
                    dispatch(campaignSliceActions.setCampaignMessage(updated));
                    dispatch(campaignSliceActions.setIsRegenerate(false));
                    dispatch(campaignSliceActions.setIsEdit(false));
                    return;
                }
        
                if(data?.intent ==='whatsapp_status'){
                    dispatch(campaignSliceActions.setWhatsappCardDetails(data));
                    if(data?.whatsapp_status ==="APPROVED" || data?.whatsapp_status === "REJECTED"){
                        let notifyData = [{
                            id: 1,
                            type: "success",
                            title: "WhatsApp template Approved",
                            message:`${data?.whatsapp_notification}`, 
                            isRead: 0,
                            channelType:"whatsapp",
                            status:"APPROVED"                          
                        },
                        {
                            id: 2,
                            type: "error",
                            title: "WhatsApp template rejected",
                            message:`${data?.whatsapp_notification}`,
                            isRead: 0,
                            channelType:"whatsapp",
                            status:"REJECTED"                            
                        }];

                         if(data?.whatsapp_status ==="APPROVED"){                            
                           dispatch(campaignSliceActions.setNotificationDetails(notifyData[0]));
                         }
                            
                         if(data?.whatsapp_status ==="REJECTED"){
                            dispatch(campaignSliceActions.setNotificationDetails(notifyData[1]));
                         }
                          
                         
                    }
                }


                if(data?.intent ==='navigateOperationHub'){
                    dispatch(commonDataSliceActions.setNavigateOperationHub(true));
                }

                if(currentChatId != data?.campaignChatId ){

                    return ;
                }

                if (lastMessage?.messageId === data?.messageId && lastMessage?.progress !== undefined && data?.progress && data?.sender === "botText") {
                    const updated = [...messages];
                    updated[updated.length - 1] = { ...lastMessage, ...data };
                    dispatch(campaignSliceActions.setCampaignMessage(updated));
                } else {
                    dispatch(campaignSliceActions.setCampaignMessage([...messages, data]));
                }

                if(data?.intent ==='campaign_launched'){
                    console.log("old here"); 
                    window.parent.postMessage(
                        {
                            type: "CVM_IFRAME",
                            message: {
                                data: "campaign_launched"
                            },
                        },
                        "*"
                    )
                    // dispatch(campaignSliceActions.reset());
                    // dispatch(campaignSliceActions.setCampaignChatType('old'));
                    // localStorage.setItem("campaignChatId", data?.campaignChatId);
                    dispatch(campaignSliceActions.setProduct({type:"campaign",data:null,query:null}));
                    dispatch(commonDataSliceActions.setCallOldCampaign(true));
                }
                if(data?.intent ==="refresh_draft"){
                    dispatch(commonDataSliceActions.setUpdateHomeAPI(true));
                }
            }

               
            } catch (error) {
                console.log(error)
            }
        });

        SocketService.socket.on("pastCampaignMessageBot", (data: any) => {
            // try {
            //     let messages: any[] = store.getState().campaignData.campaignMessage || [];
            //     const lastMessage: any = messages[messages.length - 1]
            //     if (data?.progress) {
            //         dispatch(campaignSliceActions.progress(true));
            //     } else {
            //         dispatch(campaignSliceActions.progress(false));
            //     }

            //     dispatch(campaignSliceActions.setIsTyping(false));

            //     if (lastMessage?.messageId === data?.messageId && lastMessage?.progress !== undefined && data?.progress && data?.sender === "botText") {
            //         const updated = [...messages];
            //         updated[updated.length - 1] = { ...lastMessage, ...data };
            //         dispatch(campaignSliceActions.setCampaignMessage(updated));
            //     } else {
            //         dispatch(campaignSliceActions.setCampaignMessage([...messages, data]));
            //     }
            // } catch (error) {
            //     console.log(error)
            // }

            try {
                let messages: any[] = store.getState().analyticsData.analyticsMessage || [];
                dispatch(analyticsSliceActions.setAnalyticsMessage([...messages, data]));
                dispatch(analyticsSliceActions.setIsTyping(false));
            } catch (error) {
                console.log(error)
            }
        });

        SocketService.socket.on("analyticsMessageBot", (data: any) => {
            try {
                let messages: any[] = store.getState().analyticsData.analyticsMessage || [];
                dispatch(analyticsSliceActions.setAnalyticsMessage([...messages, data]));
                dispatch(analyticsSliceActions.setIsTyping(false));
            } catch (error) {
                console.log(error)
            }
        });

        SocketService.socket.on("reportMessageBot", (data: any) => {
            try {
                let messages: any[] = store.getState().reportData.reportMessage || [];
                dispatch(reportSliceActions.setReportMessage([...messages, data]));
                dispatch(reportSliceActions.setIsTyping(false));
            } catch (error) {
                console.log(error)
            }
        });

    }

    static async knowledgeMessage(data: any) {
        if (SocketService.socket) {
            SocketService.socket.emit("knowledgeMessage", data);
        } else {
            console.warn("⚠️ No active socket connection");
        }
    }

    static async contactMessage(data: any) {
        if (SocketService.socket) {
            SocketService.socket.emit("contactchatreq", data);
        } else {
            console.warn("⚠️ No active socket connection");
        }
    }

    static async campaignMessage(data: any) {
        if (SocketService.socket) {
            SocketService.socket.emit("campaignMessage", data);
        } else {
            console.warn("⚠️ No active socket connection");
        }
    }

    static async pastCampaignMessage(data: any) {
        if (SocketService.socket) {
            SocketService.socket.emit("pastCampaignMessage", data);
        } else {
            console.warn("⚠️ No active socket connection");
        }
    }
    static async sentAnalyticsMessage(data: any) {
        if (SocketService.socket) {
            SocketService.socket.emit("analyticsMessage", data);
        } else {
            console.warn("⚠️ No active socket connection");
        }
    }
      static async sentReportMessage(data: any) {
        if (SocketService.socket) {
            SocketService.socket.emit("reportMessage", data);
        } else {
            console.warn("⚠️ No active socket connection");
        }
    }

    static getSocket(): Socket | null {
        return SocketService.socket;
    }

    static destroy() {
        if (SocketService.socket) {
            SocketService.socket.disconnect();
            SocketService.socket = null;
        }
    }
}
