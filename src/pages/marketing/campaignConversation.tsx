import styles from "./scss/marketchat.module.scss";
import SetUpCommonEditor from "./commoneditor";
import { useRef, useState, useEffect } from "react";
import ChatLikes from "../component/likedislike";
import { useSearchParams } from "react-router-dom";
import { v4 as uuidv4 } from "uuid";
import { excelFormats, getFileSize, imageFormat, pdfFormats, stripHtmlTags, wordFormats } from "../../base/utils";
import { GetItemFromStorage } from "../../base/custom_hooks/useStorage";
import { LocalStorage } from "../../base/custom_hooks/localStorageKeys";
import aiChatAssistSocketservice from "../../services/aiChatService";
import { MarketingAgentChatAction } from "../../store/slice/marketingSlice/marketingSlice";
import { useDispatch, useSelector } from "react-redux";
import jwtDecode from "jwt-decode";
import { CoreServices } from "../../base/service/apiservices";
import { commonDataSliceActions } from "../../store/slice/commonSilce/commonSlice";
import Loader from "../component/loader/loader";
import Excelicon from "../../assets/images/xlsicon.svg";
import Pdficon from "../../assets/images/pdfFile.svg";
import Wordicon from "../../assets/images/documentformate.svg";

export default function MarketingCampaignConversation() {
    const dispatch = useDispatch();
    const [searchParam, useSearchParam] = useSearchParams();
     const pageSource = searchParam.get('page');
    const boxRef = useRef(null);
    const [height, setHeight] = useState(0);
    const [conversation, setConversation] = useState<any>(null);
    const sessionId = searchParam.get('sessionId');
    const LoginCcaasUserDetails: any = jwtDecode(GetItemFromStorage(LocalStorage.ACCESS_TOKEN));
    // console.log("login", LoginCcaasUserDetails)
    // console.log("typiing", typingValue)
    const marketingChatConversation = useSelector((state: any) => state?.marketingChatStore?.marketingAgentChatMessage);
    console.log("messageconvo:-->1", marketingChatConversation[`${sessionId}`])
    console.log("messageconvo:-->", marketingChatConversation)
    const marketingTemplateChatConversation = useSelector((state: any) => state?.marketingChatStore?.marketingAgentTemplateChatMessage);
    // console.log("template conversation:", marketingTemplateChatConversation);
    const currentConversationType: any = useSelector((state: any) => state?.marketingChatStore?.marketingConversationType)
    // console.log("tempconvo", currentConversationType?.[`${sessionId}`])
    // console.log("tempconvoall", currentConversationType)
    const progressBar :any = useSelector((state :any)=>state?.marketingChatStore?.marketingProgress);
    const { loader , aiAgentChatTyping} :any = useSelector((state :any)=>state?.marketingChatStore);
    // console.log("loader:",loader)
    const chatEndRef = useRef<HTMLDivElement | null>(null);
    const [selectedtemplateId, setSelectedTemplateId] = useState<any>(null);
    // console.log("tempid", selectedtemplateId)
    const campaigndetail :any = marketingTemplateChatConversation?.[`${currentConversationType?.[`${sessionId}`]?.selectedTemplateId}`]?.[0]||{}

    // console.log("templateconversation:", marketingTemplateChatConversation?.[`${currentConversationType?.[`${sessionId}`]?.selectedTemplateId}`])

    const onSend = (val: any,files :any) => {
        try {
            let uploadfiles :any =files;
            if(uploadfiles && uploadfiles.length>0){
                uploadfiles = uploadfiles?.map((item :any)=>{
                    return (
                        {
                            "filename": item?.filename,
                            "mimetype": item?.mimetype,
                            "size": item?.size,
                            "uploadUrl": item?.url,
                            "uniqueId": item?.uuid
                        }
                )
                })
            }
            console.log("alll", val)
            if (!val?.trim()) return;

            const newSessonId = sessionId;
            let body: any = {
                query: stripHtmlTags(val?.trim()),
                sessionId: newSessonId,
                roleId: `${LoginCcaasUserDetails?.roleId ?? '' }`,
                org_role_id_crm: `${LoginCcaasUserDetails?.org_role_id_crm ?? '' }`,
                domainId: `${LoginCcaasUserDetails?.domainId ?? '' }`,
                module: "",
                ext: `${LoginCcaasUserDetails?.ext ?? '' }`,
                agent: 'marketing',
                isBot: 0,
                uploadUrl:uploadfiles ||[],
                pageSource:pageSource
            }

            if (currentConversationType?.[`${sessionId}`]?.templateConvo) {
                body = { ...body, templateId: currentConversationType?.[`${sessionId}`]?.selectedTemplateId, channel: currentConversationType?.[`${sessionId}`]?.viewCard }
            }
            aiChatAssistSocketservice.io.emit('crm_chat', body);
            if (!(currentConversationType?.[`${sessionId}`]?.templateConvo)) {

                
                dispatch(MarketingAgentChatAction.setMarketingAgentChat({ ...body }))
                // dispatch(MarketingAgentChatAction.setMarketingAgentChat({ ...dummy }))
            } else {
                dispatch(MarketingAgentChatAction.setTemplateChatConversation({ ...body, templateId: currentConversationType?.[`${sessionId}`]?.selectedTemplateId, channel: currentConversationType?.[`${sessionId}`]?.viewCard }))
            }
            dispatch(MarketingAgentChatAction.setMarketingAgentChatTyping({sessionId:sessionId,typing:true}));
        } catch (error) {
            console.log("err :", error)
        }
    }


    const templateConversation = (selectedTemplateCards: any, item: any) => {
        console.log("iten???", item?.[0]?.templateid,item,selectedTemplateCards)
        // setSelectedTemplateId(item?.[0]?.templateid);
        dispatch(MarketingAgentChatAction.setSessionConversationType({ sessionId: sessionId, templateConvo: true, selectedTemplateCard: selectedTemplateCards, viewCard: item?.[0]?.channel, selectedTemplateId: item?.[0]?.templateid }));
    }

    const fetchTemplateConversation = async () => {

        try {

            const selectedtemplateId = currentConversationType?.[`${sessionId}`]?.selectedTemplateId;
            dispatch(MarketingAgentChatAction.setMarketingTemplateChatLoader({sessionId:sessionId,isLoader:true}));
            let body: any = {
                templateId: selectedtemplateId
            }
            const response: any = await CoreServices.post(body, `/get_template_conversation`);
            console.log("res-->", response)
            if (
                response?.data?.statusCode === 200 &&
                // Array.isArray(response?.data?.conversationDataRes) &&
                response?.data?.conversationDataRes.length > 0
            ) {
                // SetItemInStorage(LocalStorage.LOGIN_USER_CRM_DETAILS, response.data.getUserDetailsRes[0]);
                // dispatch(MarketingAgentChatAction.setTemplateConversation(JSON.parse(response?.data?.conversationDataRes?.[0]?.conversation) || []))
                // console.log("res11-->", response?.data?.conversationDataRes?.[0]?.conversation?.[0])
                
                dispatch(commonDataSliceActions.setModifyTabMenuMarketing({...response?.data?.conversationDataRes?.[0]?.conversation?.[0]}));
               
                dispatch(MarketingAgentChatAction.setTemplateConversation((response?.data?.conversationDataRes?.[0]?.conversation) || []))
                 dispatch(MarketingAgentChatAction.setMarketingTemplateChatLoader({sessionId:sessionId,isLoader:false}));
            }
        } catch (error) {
            console.log("get_user_details_email error>>>>", error);
             dispatch(MarketingAgentChatAction.setMarketingTemplateChatLoader({sessionId:sessionId,isLoader:false}));
        }finally{
             dispatch(MarketingAgentChatAction.setMarketingTemplateChatLoader({sessionId:sessionId,isLoader:false}));
        }

    }

    const handleBackToMainConvo = () => {
        try {

            dispatch(MarketingAgentChatAction.setSessionConversationType({ sessionId: sessionId, type: false }));
        } catch (error) {
            console.log("err", error)
        }
    }

    const fetchMainChatConvo=async()=>{
        try {
             dispatch(MarketingAgentChatAction.setMarketingAgentChatTyping({sessionId:sessionId,typing:true}));
             let body: any = {
                sessionId: sessionId
            }
            const response: any = await CoreServices.post(body, `/get_mainChat_conversation`);
           
            if (
                response?.data?.statusCode === 200 &&
                // Array.isArray(response?.data?.conversationDataRes) &&
                response?.data?.conversationDataRes?.length > 0
            ) {
                 
                 dispatch(MarketingAgentChatAction.setFullMarketingAgentChat(response?.data?.conversationDataRes?.[0]?.conversation|| []))
                 dispatch(MarketingAgentChatAction.setMarketingAgentChatTyping({sessionId:sessionId,typing:false}));
            }

           
        } catch (error) {
            console.log("error:",error)
        }
    }

    useEffect(() => {
        try {

            if (!boxRef.current) return;

            const observer = new ResizeObserver((entries) => {
                for (let entry of entries) {
                    setHeight(Math.round(entry.contentRect.height) + 48);
                }
            });

            observer.observe(boxRef.current);

            return () => observer.disconnect();
        } catch (error) {
            console.log("err", error)
        }
    }, [loader]);


    useEffect(() => {
        if (chatEndRef.current) {
            chatEndRef.current.scrollIntoView({ behavior: 'smooth' });
        }
    }, [marketingChatConversation?.[`${sessionId}`]?.length,currentConversationType?.[`${sessionId}`]?.templateConvo,marketingTemplateChatConversation?.[`${currentConversationType?.[`${sessionId}`]?.selectedTemplateId}`]]);

    useEffect(() => {
        const selectedtemplateId = currentConversationType?.[`${sessionId}`]?.selectedTemplateId;
        if (selectedtemplateId) {
            console.log("jhgjg", selectedtemplateId)
            fetchTemplateConversation();
        }
    }, [currentConversationType?.[`${sessionId}`]?.selectedTemplateId])

    useEffect(()=>{
        if(pageSource){
            fetchMainChatConvo();
        }
    },[pageSource])

    return (<>
        {loader?.[`${sessionId}`]?.isLoader ?

            <Loader/>:
            <>
        <div>
            {/* {currentConversationType?.[`${sessionId}`]?.templateConvo &&
                <div className={`${styles.campaigndetail} chatmaincontainer`}>

                    <div className={styles.campaigndetailleft}>


                        <div className={styles.title} onClick={() => handleBackToMainConvo()}>{campaigndetail?.campaignName??"New campaign"}{<span>Primary Goal: {campaigndetail?.goal ?? "goal"}</span>}</div>
                        <div className={styles.tab}>
                            {Object.keys(currentConversationType?.[`${sessionId}`]?.selectedTemplateCard)?.length > 0 && Object.keys(currentConversationType?.[`${sessionId}`]?.selectedTemplateCard)?.map((item: any) => {
                               const allTemplateCards :any=currentConversationType?.[`${sessionId}`]?.selectedTemplateCard;
                               return (
                                    <div className={`${styles.tabinner} ${(currentConversationType?.[`${sessionId}`]?.viewCard == item) ? styles.active : ''} `}
                                        onClick={() => templateConversation(allTemplateCards,allTemplateCards[`${item}`])}
                                    >

                                        {(item?.charAt(0).toUpperCase() + item?.slice(1))}

                                    </div>)
                            })
                            }
                        </div>
                    </div>
                </div>} */}

            <div >
                  {currentConversationType?.[`${sessionId}`]?.templateConvo &&
                <div className={`${styles.campaigndetail} chatmaincontainer  ${currentConversationType?.[`${sessionId}`]?.templateConvo && styles.stickydetails}`}>

                    <div className={styles.campaigndetailleft}>

                        <div style={{ display: "flex", gap: "12px", alignItems: "baseline" }} onClick={() => handleBackToMainConvo()}>
                            <span style={{ display: "grid" }}>
                                <svg style={{ position: "relative", cursor: "pointer" }} xmlns="http://www.w3.org/2000/svg" width="8" height="16" viewBox="0 0 8 16" fill="none"><path d="M7.04593 15.9644C6.81453 15.9644 6.58302 15.8755 6.40723 15.6988L0.889625 10.147C-0.295875 8.95361 -0.296875 7.01131 0.888625 5.81791L6.40323 0.266115C6.75283 -0.0883855 7.32413 -0.0873855 7.67573 0.261215C8.02923 0.611814 8.03123 1.18111 7.68063 1.53371L2.16603 7.08551C1.67483 7.57961 1.67582 8.38342 2.16702 8.87751L7.68463 14.4293C8.03423 14.7818 8.03323 15.3522 7.68073 15.7027C7.50493 15.8765 7.27543 15.9644 7.04593 15.9644Z" fill="#404040"></path></svg>

                            </span>

                            <div className={styles.title}>{campaigndetail?.campaignName ?? "New campaign"}{<span>Primary Goal: {campaigndetail?.goal ?? "goal"}</span>}</div>
                        </div>
                        <div className={styles.tab}>
                            {Object.keys(currentConversationType?.[`${sessionId}`]?.selectedTemplateCard)?.length > 0 && Object.keys(currentConversationType?.[`${sessionId}`]?.selectedTemplateCard)?.map((item: any) => {
                               const allTemplateCards :any=currentConversationType?.[`${sessionId}`]?.selectedTemplateCard;
                               return (
                                    <div className={`${styles.tabinner} ${(currentConversationType?.[`${sessionId}`]?.viewCard == item) ? styles.active : ''} `}
                                        onClick={() => templateConversation(allTemplateCards,allTemplateCards[`${item}`])}
                                    >

                                        {(item?.charAt(0).toUpperCase() + item?.slice(1))}

                                    </div>)
                            })
                            }
                        </div>
                    </div>
                </div>}
                <div className={`${styles.conversationtypeid} ${styles.chatarea} haschatarea`} style={{
  height: currentConversationType?.[`${sessionId}`]?.templateConvo
    ? `calc(100dvh - ${height}px - 246px)`
    : `calc(100dvh - ${height}px - 116px)`
}}>
                    <div className={styles.bubbleblock}>
                    {(!(currentConversationType?.[`${sessionId}`]?.templateConvo)) && marketingChatConversation?.[`${sessionId}`]?.map((msg: any) => {

                        return (
                            <>
                                {(msg?.isBot == 0 && (!msg?.templateId)) &&

                                    <div className={styles.bubbleblockagent}>
                                        {msg?.uploadUrl?.length > 0 && (
                                            <div className={`${styles.bubbleblockagenttextblock}`}>
                                                {/* <div className={styles.bubbleblockagenttext}>
                                                                {msg?.uploadUrl?.map((file: any, i: number) => {
                                                                    const fileName = file?.name || file?.filename || "Attachment";
                                                                    const fileSize = getFileSize(Number(file?.size) || 0) || "";
                                                                    const fileUrl = file?.url || file?.path ||file?.uploadUrl;
                                                                    const mimetype = file?.mimetype;

                                                                    let iconSrc = Pdficon; // default icon

                                                                    if (imageFormat.includes(mimetype) || (mimetype || "").startsWith("image/")) {
                                                                        iconSrc = fileUrl; // show actual image preview
                                                                    } else if (excelFormats.includes(mimetype)) {
                                                                        iconSrc = Excelicon;
                                                                    } else if (wordFormats.includes(mimetype)) {
                                                                        iconSrc = Wordicon;
                                                                    } else if (pdfFormats.includes(mimetype)) {
                                                                        iconSrc = Pdficon;
                                                                    } else {}
                                                                    return (
                                                                        <div className={styles.chateditorfile} key={file?.uuid || i}>
                                                                            <a
                                                                                href={fileUrl}
                                                                                target="_blank"
                                                                                rel="noopener noreferrer"
                                                                            >
                                                                                <img
                                                                                    src={iconSrc}
                                                                                    alt="File attachment"
                                                                                    className={styles.fileIcon}
                                                                                />
                                                                            </a>

                                                                            <div className={styles.right}>
                                                                                <p>{fileName}</p>
                                                                                <p className={styles.filesize}>{fileSize}</p>
                                                                            </div>
                                                                        </div>
                                                                    );
                                                                })}
                                                                
                                                            </div> */}
                                                <div className={`${styles.bubbleblockagenttext} ${styles.textwithimg}`}>
                                                    {msg?.uploadUrl?.map((file: any, i: number) => {
                                                        const fileName = file?.name || file?.filename || "Attachment";
                                                        const fileSize = getFileSize(Number(file?.size) || 0) || "";
                                                        const fileUrl = file?.url || file?.path || file?.uploadUrl;
                                                        const mimetype = file?.mimetype;

                                                        let iconSrc = Pdficon; // default icon

                                                        if (imageFormat.includes(mimetype) || (mimetype || "").startsWith("image/")) {
                                                            iconSrc = fileUrl; // show actual image preview
                                                        } else if (excelFormats.includes(mimetype)) {
                                                            iconSrc = Excelicon;
                                                        } else if (wordFormats.includes(mimetype)) {
                                                            iconSrc = Wordicon;
                                                        } else if (pdfFormats.includes(mimetype)) {
                                                            iconSrc = Pdficon;
                                                        } else { }
                                                        return (
                                                            <div className={` ${iconSrc === fileUrl ? "" : styles.chateditorfile} `} key={file?.uuid || i}>
                                                                <a
                                                                    href={fileUrl}
                                                                    target="_blank"
                                                                    rel="noopener noreferrer"
                                                                >
                                                                    <div className={styles.imgcontainer}>
                                                                        <img
                                                                            src={iconSrc}
                                                                            alt="File attachment"
                                                                            className={styles.fileIcon}
                                                                        />
                                                                    </div>

                                                                </a>

                                                                {
                                                                    iconSrc === fileUrl ? <></> : <div className={styles.right}>
                                                                        <p className="truncate1">{fileName}</p>
                                                                        <p className={styles.filesize}>{fileSize}</p>
                                                                    </div>
                                                                }
                                                            </div>
                                                        );
                                                    })}
                                                    <div>
                                                        {msg?.query}
                                                    </div>

                                                </div>
                                            </div>
                                        )}
                                        {msg?.uploadUrl?.length === 0 && <div className={styles.bubbleblockagenttextblock}>
                                            <div className={styles.bubbleblockagenttext}>{msg?.query}</div>
                                        </div>}
                                    </div>
                                }
                                {(msg?.isBot == 1 && msg?.template_intent != "card" && (!msg?.templateId)) &&

                                    <div className={styles.bubbleblockcustomer}>
                                        <div className={styles.bubbleblockcustomertextblock}>
                                            <div className={`${styles.bubbleblockcustomertext}`}>
                                                <div dangerouslySetInnerHTML={{ __html: msg?.response }} />
                                            </div>
                                        </div>

                                        {/* <ChatLikes message={msg?.response}/> */}
                                    </div>
                                }

                                {(msg?.isBot == 1 && msg?.template_intent == "card") &&
                                <div className={styles.cardWrapper}>
                                    <div className={styles.cardmaincontainer}>
                                        {
                                            Object?.keys(msg?.channel || {})?.length > 0 &&
                                            Object?.keys(msg?.channel)?.map((item: any) => {
                                                return (
                                                    <div className={styles.cardisplay}>
                                                        <div className={styles.cardcontainer}>
                                                            <div className={`${styles.cardtitle} truncate2`}>{item?.charAt(0)?.toUpperCase()+item?.slice(1)}</div>
                                                            <div className={`${styles.cardsubtitle}  truncate2`}>Primary Goal: {msg?.goal??"Goal"}</div>
                                                        </div>
                                                        <button onClick={() => templateConversation(msg?.channel, msg?.channel?.[item])}>View</button>
                                                    </div>
                                                )
                                            })
                                        }
                                        {/* <div className={styles.cardisplay}>
                                                <div className={styles.cardcontainer}>
                                                <div className={styles.cardtitle}>Email</div>
                                                <div className={styles.cardsubtitle}>Primary Goal: Boost Online Sales</div>
                                                </div>
                                            <button>View</button>
                                        </div> */}

                                    </div>
                                    </div>}

                               {progressBar?.[`${sessionId}`]?.progress>0 &&
                                <div className={styles.bubbleblockcustomer}>

                                    <div className={styles.bubbleblockcustomertextblock}>
                                        <div className={styles.bubbleblockcustomertextlong}>
                                            {/* {progressBar?.[`${sessionId}`]?.progressMessage} */}
                                            Generating Templates
                                            <div className={styles.line}>
                                                <div className={styles.innerline} style={{ width: `${progressBar?.progress}%` }}></div>
                                            </div>
                                        </div>
                                    </div>
                                </div>
                                }


                            </>)
                    })
                    }

                    {(currentConversationType?.[`${sessionId}`]?.templateConvo) && marketingTemplateChatConversation?.[`${currentConversationType?.[`${sessionId}`]?.selectedTemplateId}`]?.map((msg: any) => {

                        return (
                            <>
                             
                                    {(msg?.isBot == 0 && (msg?.templateId)) &&

                                        <div className={styles.bubbleblockagent}>
                                               {msg?.uploadUrl?.length > 0 && (
                                                        <div className={styles.bubbleblockagenttextblock}>
                                                            <div className={styles.bubbleblockagenttext}>
                                                                {msg?.uploadUrl?.map((file: any, i: number) => {
                                                                    const fileName = file?.name || file?.filename || "Attachment";
                                                                    const fileSize = getFileSize(Number(file?.size) || 0) || "";
                                                                    const fileUrl = file?.url || file?.path;
                                                                    const mimetype = file?.mimetype;

                                                                    let iconSrc = Pdficon; // default icon

                                                                    if (imageFormat.includes(mimetype)) {
                                                                        iconSrc = fileUrl; // show actual image preview
                                                                    } else if (excelFormats.includes(mimetype)) {
                                                                        iconSrc = Excelicon;
                                                                    } else if (wordFormats.includes(mimetype)) {
                                                                        iconSrc = Wordicon;
                                                                    } else if (pdfFormats.includes(mimetype)) {
                                                                        iconSrc = Pdficon;
                                                                    }
                                                                    return (
                                                                        <div className={styles.chateditorfile} key={file?.uuid || i}>
                                                                            <a
                                                                                href={fileUrl}
                                                                                target="_blank"
                                                                                rel="noopener noreferrer"
                                                                            >
                                                                                <img
                                                                                    src={iconSrc}
                                                                                    alt="File attachment"
                                                                                    className={styles.fileIcon}
                                                                                />
                                                                            </a>

                                                                            <div className={styles.right}>
                                                                                <p>{fileName}</p>
                                                                                <p className={styles.filesize}>{fileSize}</p>
                                                                            </div>
                                                                        </div>
                                                                    );
                                                                })}
                                                                <div dangerouslySetInnerHTML={{ __html: msg?.query }} />
                                                            </div>
                                                        </div>
                                                    )}
                                            <div className={styles.bubbleblockagenttextblock}>
                                                <div className={styles.bubbleblockagenttext}>{msg?.query}</div>
                                            </div>
                                        </div>
                                    }
                                    {(msg?.isBot == 1 && msg?.template_intent != "card") &&

                                        <div className={styles.bubbleblockcustomer}>
                                            <div className={styles.bubbleblockcustomertextblock}>
                                                <div className={styles.bubbleblockcustomertext}>
                                                    <div dangerouslySetInnerHTML={{ __html: msg?.response }} />
                                                </div>
                                            </div>

                                            {/* <ChatLikes message={msg?.response}/> */}
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


                               
                            </>
                        )
                    })
                    }
                    {!!aiAgentChatTyping[`${sessionId}`] && (
                        <div className="msgbubble left">
                            <div style={{paddingBottom:"12px"}} className="chatContent">
                                { aiAgentChatTyping[`${sessionId}`] || "Typing ..."}
                                 {/* <div  className="pulse" /> */}
                            </div>
                        </div>
                    )}
                    </div>
                    <div ref={chatEndRef} />
                </div>
            </div>

          
        </div>
          <div className={`${styles.editorbottom} maincontainer`} ref={boxRef}>
                <SetUpCommonEditor pageSource={"marketingconversation"} onSend={(val: any,files :any) => onSend(val,files)} />
            </div></>
    }
    </>
    );
}
