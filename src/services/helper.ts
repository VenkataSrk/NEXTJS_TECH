import { getContactModuleByPath } from "../base/utils";
import store from "../store/redux-store/root_store";
import { AIAgentChatAction } from "../store/slice/aichatSlice/aichatSlice";
import { commonDataSliceActions } from "../store/slice/commonSilce/commonSlice";
import { contactDataSliceActions } from "../store/slice/contactSlice";
import { dealsDataSliceActions } from "../store/slice/dealsSlice";
import { homeDataSliceActions } from "../store/slice/homeSlice";
import { inboxDataSliceActions } from "../store/slice/inbox/inboxSlice";
import { MarketingAgentChatAction } from "../store/slice/marketingSlice/marketingSlice";
import { MyplanProfileActions } from "../store/slice/my_plans/profile";
import { AiOnboardingAction } from "../store/slice/onboardingSlice/onboardingSlice";
import { productDataSliceActions } from "../store/slice/productSlice";
import { SetupSliceActions } from "../store/slice/setupSlice/setupSlice";



export const crm_main_chat_response = (dispatch: any, data: any, navigate: any) => {
    try {
        if (data?.agent == "marketing") {

            if (data?.campaignName) {
                dispatch(commonDataSliceActions.setModifyTabMenuMarketing({ ...data }));
            }
            if (data?.template_intent == "progress") {
                dispatch(MarketingAgentChatAction.setMarketingChatProgress({ ...data }))
                return;
            } 
            dispatch(MarketingAgentChatAction.setMarketingAgentChatTyping({ sessionId: data?.sessionId, typing: false }));
            if (!data?.templateId || data?.template_intent == "card") {
                dispatch(MarketingAgentChatAction.setMarketingAgentChat({ ...data }))
                return;
            } else {
                dispatch(MarketingAgentChatAction.setTemplateChatConversation({ ...data }))
                return;
            }
        }
        else if (data?.agent == "contact") {
            if (data?.title?.trim() && data?.sessionId) {
                dispatch(
                    commonDataSliceActions.setTabTitleByAgentSessionId({
                        sessionId: data?.sessionId,
                        title: data?.title,
                        agent: data?.agent
                    })
                );
            }
            dispatch(contactDataSliceActions.setcontactAiAgentChat({ ...data, isBot: 1 }))
            dispatch(contactDataSliceActions.setcontactAiAgentChatTyping({ sessionId: data?.sessionId, typing: false }));

            if(data?.contactUpdateModel == 1) {
                dispatch(contactDataSliceActions.setContactUpdateModel({ sessionId: data?.sessionId , open: true}))
            }

            handleValidateContactResponse(data, dispatch, navigate);

            if(data?.status ==  "completed") {
                dispatch(contactDataSliceActions.setContactRefresh());
            }

        }
        else if (data?.agent == "deals") {
            if (data?.title?.trim() && data?.sessionId) {
                dispatch(
                    commonDataSliceActions.setTabTitleByAgentSessionId({
                        sessionId: data?.sessionId,
                        title: data?.title,
                        agent: data?.agent
                    })
                );
            }
            dispatch(dealsDataSliceActions.setdealsAiAgentChat({ ...data, isBot: 1 }));
            dispatch(dealsDataSliceActions.setdealsAiAgentChatTyping({ sessionId: data?.sessionId, typing: false }));
        }
        else if (data?.agent === 'onboarding') {
            dispatch(
                AiOnboardingAction.setOnboardMessages({
                    ...data,
                    isBot: 1,
                    response: data?.response ?? data?.message ?? "",
                })
            );
            dispatch(AiOnboardingAction.setOnboardTyping({ sessionId: data?.sessionId, typing: false }));
            if(data?.isCompleted === 1){
                setTimeout(() => {
                    navigate('/home');
                }, 2000);
            };
            return;
        }
        else if (data?.agent == "home") {
            
            if (( !data?.templateId || data?.template_intent == "card")) {
                dispatch(
                    commonDataSliceActions.setTabTitleByAgentSessionId({
                        sessionId: data?.sessionId,
                        title: data?.title,
                        agent: data?.agent
                    })
                );
            }else if(data?.templateId){
            
            dispatch(MarketingAgentChatAction.setTemplateChatConversation({ ...data }))
            dispatch(MarketingAgentChatAction.setMarketingAgentChatTyping({ sessionId: data?.sessionId, typing: false }));
            dispatch(homeDataSliceActions.setHomeAiAgentChatTyping({ sessionId: data?.sessionId, typing: false }));
            return;
        }
            dispatch(homeDataSliceActions.setHomeAiAgentChat({ ...data, isBot: 1 }))
            dispatch(homeDataSliceActions.setHomeAiAgentChatTyping({ sessionId: data?.sessionId, typing: false }));
        }
        else if (data?.agent == "products") {
            if (data?.title?.trim() && data?.sessionId) {
                dispatch(
                    commonDataSliceActions.setTabTitleByAgentSessionId({
                        sessionId: data?.sessionId,
                        title: data?.title,
                        agent: data?.agent
                    })
                );
            }
            dispatch(productDataSliceActions.setproductAiAgentChat({ ...data, isBot: 1 }))
            dispatch(productDataSliceActions.setproductAiAgentChatTyping({ sessionId: data?.sessionId, typing: false }));
        }
        else if (data?.agent == "setup") {
            if (data?.title?.trim() && data?.sessionId) {
                dispatch(
                    commonDataSliceActions.setTabTitleByAgentSessionId({
                        sessionId: data?.sessionId,
                        title: data?.title,
                        agent: data?.agent
                    })
                );
            }
            
            dispatch(SetupSliceActions.setSetupAiAgentChat({ ...data, isBot: 1 }))
            dispatch(SetupSliceActions.setSetupAiAgentChatTyping({ sessionId: data?.sessionId, typing: false }));
        }
        else if (data?.agent == "myplans") {
            if (data?.title?.trim() && data?.sessionId) {
                dispatch(
                    commonDataSliceActions.setTabTitleByAgentSessionId({
                        sessionId: data?.sessionId,
                        title: data?.title,
                        agent: data?.agent
                    })
                );
            }

            dispatch(MyplanProfileActions.setSetupAiAgentChat({ ...data, isBot: 1 }))
            dispatch(MyplanProfileActions.setSetupAiAgentChatTyping({ sessionId: data?.sessionId, typing: false }));
        }
        else if (data?.intent == "inbox" && (data?.status == "completed" || data?.status == "refresh")) {
            dispatch(inboxDataSliceActions.setInboxRefresh())
        }
        else if (data?.response?.trim()) {
            dispatch(AIAgentChatAction.setAiAgentChat({
                "response": data?.response,
                "sessionId": data?.sessionId,
                "isBot": 1,
            }))
            dispatch(AIAgentChatAction.setAiAgentChatTyping(false));
        }
    } catch (error) {
        console.log(error)
    }
}

export  const isSearchResponse = (searchValue: any) => {
    if (typeof searchValue === "boolean") return searchValue;
    if (typeof searchValue === "number") return searchValue === 1;
    if (typeof searchValue === "string") return searchValue.toLowerCase() === "true";
    return false;
};

export const handleValidateContactResponse = (data: any, dispatch: any, navigate: any) => {
    try {
        const { aiAgentChatMessage, contactProjectProId } = store.getState().contactStore;
        const currentSessionMessages = aiAgentChatMessage?.filter(
            (item: any) => item?.sessionId === data.sessionId && item?.isBot === 1
        );
        if (!currentSessionMessages?.length) return;

        // const latestBotMessage = currentSessionMessages[currentSessionMessages.length - 1];

        const latestBotMessage :any= {...data};
        const searchFlag = isSearchResponse(latestBotMessage?.isSearch);
        const is_followup = isSearchResponse(latestBotMessage?.is_followup);
        const contactModuleByPath = getContactModuleByPath()
        const sessionModule = latestBotMessage?.module || contactModuleByPath;

        if(is_followup){
            dispatch(contactDataSliceActions.setContactModuleFollowUp({
                module: sessionModule,
                response: latestBotMessage?.response || latestBotMessage?.msg || latestBotMessage?.message || "",
                searchStatus: latestBotMessage?.status || "",
                isWaitingResponse: false
            }))            
            return;
        } else if (searchFlag) {
            dispatch(contactDataSliceActions.setContactModuleFollowUp({
                module: sessionModule,
                response: latestBotMessage?.response || latestBotMessage?.msg || latestBotMessage?.message || "",
                searchStatus: latestBotMessage?.status || "",
                isWaitingResponse: false
            }))
            const filterPayload = {
                customer_name: latestBotMessage?.customer_name,
                account_owner: latestBotMessage?.account_owner,
                email: latestBotMessage?.email,
                phone_number: latestBotMessage?.phone_number,
                address: latestBotMessage?.address,
                industry_type: latestBotMessage?.industry_type,
                source_type: latestBotMessage?.source_type,
                company_name: latestBotMessage?.company_name,
                company_size: latestBotMessage?.company_size,
                tags: latestBotMessage?.tags,
                lead_status: latestBotMessage?.lead_status,
                from_date: latestBotMessage?.from_date,
                to_date: latestBotMessage?.to_date,
                is_active: latestBotMessage?.is_active,
                owner_name: latestBotMessage?.owner_name,
                source_name: latestBotMessage?.source_name,
                industry: latestBotMessage?.industry,
                company_address: latestBotMessage?.company_address,
            };

            if (latestBotMessage?.module === "companyList") {
                dispatch(contactDataSliceActions.setCompanyListFilters({ ...filterPayload, offset: 0 }));
            } else {
                dispatch(contactDataSliceActions.setContactListFilters(filterPayload));
            }
            return;
        }   
        if(contactModuleByPath) {
            const companyName = new URLSearchParams(window.location.search).get("companyName");
            const companyId = new URLSearchParams(window.location.search).get("companyId");
    
            const path = `/contact/chat-conversation?sessionId=${data.sessionId}&companyId=${companyName || ''}&company_id=${companyId || ''}&pro_id=${contactProjectProId || ''}`;
            dispatch(commonDataSliceActions.setTabMenuPush({ path: path, menuName: "New Conversation", close: 1, type: "contact" }));
            if (sessionModule) {
                dispatch(contactDataSliceActions.resetContactModuleFollowUp({ module: sessionModule }))
            }
            navigate(path);
            dispatch(contactDataSliceActions.setUpdatecontactmoduleSessionId())
        }
    } catch (error) {
        
    }
}