import styles from "./scss/marketchat.module.scss";
import SetUpCommonEditor from "./commoneditor";
import { useRef, useState, useEffect } from "react";
import ChatLikes from "../component/likedislike";
import { useSearchParams } from "react-router-dom";
import { stripHtmlTags } from "../../base/utils";
import jwtDecode from "jwt-decode";
import { GetItemFromStorage } from "../../base/custom_hooks/useStorage";
import { LocalStorage } from "../../base/custom_hooks/localStorageKeys";
import aiChatAssistSocketservice from "../../services/aiChatService";
import { useDispatch, useSelector } from "react-redux";
import { MarketingAgentChatAction } from "../../store/slice/marketingSlice/marketingSlice";

export default function MarketingTemplateCampaignConversation() {
    const boxRef = useRef(null);
    const [height, setHeight] = useState(0);
    const [conversation,setConversation] = useState<any>(null);
    const [activeChannelName,setActiveChannelName]=useState<any>(null);
    const [searchParam,useSearchParam] = useSearchParams();
    const campaignId = searchParam.get('campaignId');
    const pageSource = searchParam.get('page');
    const marketingChatConversation = useSelector((state :any)=>state?.marketingChatStore?.marketingAgentChatMessage);
    const {aiAgentChatTyping} = useSelector((state :any)=>state?.marketingChatStore)
  
    const chatEndRef = useRef<HTMLDivElement | null>(null);
    const LoginCrmUserDetails: any = GetItemFromStorage(LocalStorage.USER_DETAILS);
    const handleChannelConversation=(item :any)=>{
        setActiveChannelName(item?.name);

    }
    const dispatch=useDispatch();

    const onSend = (val: any) => {
        try {
            console.log("alll", val)
            if (!val?.trim()) return;

            const oldCampId = campaignId;
            const body = {
                query: stripHtmlTags(val?.trim()),
                sessionId: oldCampId,
                roleId: `${LoginCrmUserDetails?.roleId || ''}`,
                org_role_id_crm: LoginCrmUserDetails?.org_role_id_crm?.toString() ?? "",
                userId: `${LoginCrmUserDetails?.userId || ''}`,
                domainId: `${LoginCrmUserDetails?.domainId || ''}`,
                module: "",
                ext: `${LoginCrmUserDetails?.ext || ''}`,
                agent: 'marketing',
                isBot: 0,
                campChatId:oldCampId,
                pageSource:pageSource
            }
            aiChatAssistSocketservice.io.emit('crm_chat', body);
            dispatch(MarketingAgentChatAction.setMarketingAgentChat({ ...body }))
            dispatch(MarketingAgentChatAction.setMarketingAgentChatTyping({sessionId:oldCampId,typing:true}));
        } catch (error) {
            console.log("err :", error)
        }
    }

    useEffect(() => {
        if (!boxRef.current) return;

        const observer = new ResizeObserver((entries) => {
            for (let entry of entries) {
                setHeight(Math.round(entry.contentRect.height) + 48);
            }
        });

        observer.observe(boxRef.current);

        return () => observer.disconnect();
    }, []);


    const initialIntent=async()=>{
        
        if ((pageSource == "pastCampaign" || pageSource=="scheduleCampaign" || pageSource=="activeCampaign") && !marketingChatConversation?.[`${campaignId}`]) {
           
            const oldCampId = campaignId;
            const body = {
                query: pageSource == "pastCampaign"?"past campaign summary":pageSource=="scheduleCampaign"?"show me the summary and template of this campaign":pageSource=="activeCampaign"?"Active Campaign Summary":"",
                sessionId: oldCampId ?? "",
                roleId: `${LoginCrmUserDetails?.roleId || ''}`,
                org_role_id_crm: LoginCrmUserDetails?.org_role_id_crm?.toString() ?? "",
                userId: `${LoginCrmUserDetails?.userId || ''}`,
                domainId: `${LoginCrmUserDetails?.domainId || ''}`,
                module: "",
                ext: `${LoginCrmUserDetails?.ext || ''}`,
                agent: 'marketing',
                isBot: 0,
                campChatId: oldCampId ?? "",
                pageSource: pageSource,
                hiddenText: 1
            }
            aiChatAssistSocketservice.io.emit('crm_chat', body);
            dispatch(MarketingAgentChatAction.setMarketingAgentChat({ ...body }))
            dispatch(MarketingAgentChatAction.setMarketingAgentChatTyping({ sessionId: oldCampId, typing: true }));
        }
    }

    useEffect(()=>{
        initialIntent();
    },[])

    useEffect(() => {
        if (chatEndRef.current) {
            chatEndRef.current.scrollIntoView({ behavior: 'smooth' });
        }
    }, [marketingChatConversation?.[`${campaignId}`]?.length, aiAgentChatTyping[`${campaignId}`]]);


    return (
     <>

                { conversation?.campGoal && <div className={styles.campaigndetail}>
                    <div className={styles.campaigndetailleft}>
                        <div className={styles.title}>{conversation?.campaignName}{conversation?.campGoal && <span>Primary Goal: {conversation?.campGoal}</span>}</div>
                        <div className={styles.tab}>
                            {conversation?.channels?.map((item :any)=>{
                                return(
                                <div className={styles.tabinner} onClick={()=>handleChannelConversation(item)}>{item?.name}</div>)
                            })
                            }
                        </div>
                    </div>
                </div>}
                
                <div className={`${styles.chatarea} haschatarea`} style={{ height: `calc(100dvh - ${height}px - 300px)` }}>
                     <div className={styles.bubbleblock}>
                    {
                    // conversation?.chatConversation?.map((msg :any)=>{
                        marketingChatConversation?.[`${campaignId}`]?.map((msg :any)=>{

                          return( 
                            <>
                            
                                {(msg?.isBot==0 && msg?.hiddenText!=1) &&

                                <div className={styles.bubbleblockagent}>
                                    <div className={styles.bubbleblockagenttextblock}>
                                        <div className={styles.bubbleblockagenttext}>{msg?.query}</div>
                                    </div>
                                </div>
                                }
                               {msg?.isBot==1 &&

                                <div className={styles.bubbleblockcustomer}>
                                    <div className={styles.bubbleblockcustomertextblock}>
                                        <div className={styles.bubbleblockcustomertext}>
                                            
                                         <div dangerouslySetInnerHTML={{ __html: msg?.response }} />
                                        </div>
                                    </div>

                                    <ChatLikes />
                                </div>
                                }

                                {/* <div className={styles.bubbleblockcustomer}>

                                    <div className={styles.bubbleblockcustomertextblock}>
                                        <div className={styles.bubbleblockcustomertextlong}>
                                            Applying your edits — this won’t take long
                                            <div className={styles.line}>
                                                <div className={styles.innerline} style={{ width: "70%" }}></div>
                                            </div>
                                        </div>
                                    </div>
                                </div> */}
                         
                            </>)
                    })
                 
                    }
                    {!!aiAgentChatTyping[`${campaignId}`] && (
                        <div className="msgbubble left">
                            <div style={{paddingBottom:"12px"}} className="chatContent">
                                { aiAgentChatTyping[`${campaignId}`] || "Typing ..."}
                                 {/* <div  className="pulse" /> */}
                            </div>
                        </div>
                    )}
                    <div ref={chatEndRef} />
                    </div>
                    
                </div>
                <div className={`${styles.editorbottom} maincontainer`} ref={boxRef}>
                    <SetUpCommonEditor pageSource={"MarketingAnalyticsConversation"} onSend={(val :any)=>onSend(val)} />
                </div>

</>
          
    );
}
