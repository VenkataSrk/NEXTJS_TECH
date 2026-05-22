import styles from "./scss/dashchat.module.scss";
import { useRef, useState, useEffect } from "react";
import { useSearchParams } from "react-router-dom";
import { GetItemFromStorage } from "../../base/custom_hooks/useStorage";
import { LocalStorage } from "../../base/custom_hooks/localStorageKeys";
import { useSelector } from "react-redux";
import aiChatAssistSocketservice from "../../services/aiChatService";
import { useDispatcher } from "../../store/redux-store/dispatch";
import { excelFormats, getFileSize, imageFormat, pdfFormats, stripHtmlTags, wordFormats } from "../../base/utils";
import { dealsDataSliceActions } from "../../store/slice/dealsSlice";
import SetUpCommonEditor from "../shared/commonEditor";
import { homeDataSliceActions } from "../../store/slice/homeSlice";
import { MarketingAgentChatAction } from "../../store/slice/marketingSlice/marketingSlice";
import { commonDataSliceActions } from "../../store/slice/commonSilce/commonSlice";
import { CoreServices } from "../../base/service/apiservices";
import Excelicon from "../../assets/images/xlsicon.svg";
import Wordicon from "../../assets/images/documentformate.svg";
import Pdficon from "../../assets/images/pdfFile.svg";

export default function DashboardConversation() {
  const boxRef = useRef(null);
  const [height, setHeight] = useState(0);
  const [searchParams] = useSearchParams();
  const LoginCcaasUserDetails = GetItemFromStorage(LocalStorage.USER_DETAILS);
  const chatEndRef = useRef<HTMLDivElement | null>(null);
  const dispatch = useDispatcher();
  const { aiAgentChatMessage, aiAgentChatTyping } = useSelector((state: any) => state.homeStore);
  const chatsessionId = searchParams.get("sessionId");
  const currentaiAgentChatMessage = aiAgentChatMessage.filter((item: any) => item?.sessionId === chatsessionId);
  const currentConversationType: any = useSelector((state: any) => state?.marketingChatStore?.marketingConversationType)
  const marketingTemplateChatConversation = useSelector((state: any) => state?.marketingChatStore?.marketingAgentTemplateChatMessage);
  const campaigndetail :any = marketingTemplateChatConversation?.[`${currentConversationType?.[`${chatsessionId}`]?.selectedTemplateId}`]?.[0]||{}

  useEffect(() => {
    if (!boxRef.current) return;

    const observer = new ResizeObserver((entries) => {
      for (const entry of entries) {
        setHeight(Math.round(entry.contentRect.height));
      }
    });

    observer.observe(boxRef.current);
    return () => observer.disconnect();
  }, []);

  useEffect(() => {
    if (chatEndRef.current) {
      chatEndRef.current.scrollIntoView({ behavior: "smooth" });
    }
  }, [currentaiAgentChatMessage?.length, aiAgentChatTyping?.[`${chatsessionId}`],currentConversationType?.[`${chatsessionId}`]?.templateConvo,marketingTemplateChatConversation?.[`${currentConversationType?.[`${chatsessionId}`]?.selectedTemplateId}`]?.length]);

  useEffect(() => {
    const selectedtemplateId = currentConversationType?.[`${chatsessionId}`]?.selectedTemplateId;
    if (selectedtemplateId) {
      console.log("jhgjg", selectedtemplateId)
      fetchTemplateConversation();
    }
  }, [currentConversationType?.[`${chatsessionId}`]?.selectedTemplateId])

  const getAttachments = (msg: any) => {
    const attachments =
      Array.isArray(msg?.uploadUrl) && msg?.uploadUrl.length > 0
        ? msg.uploadUrl
        : msg?.files ?? msg?.uploadUrl ?? [];
    return Array.isArray(attachments) ? attachments : [];
  };

  const getFileUrl = (file: any) => file?.url || file?.path || file?.uploadUrl || "";

  const getIconSrc = (file: any, fileUrl: string) => {
    const mimetype = file?.mimetype || "";

    if (imageFormat.includes(mimetype) || mimetype.startsWith("image/")) {
      return fileUrl;
    }

    if (excelFormats.includes(mimetype)) {
      return Excelicon;
    }

    if (wordFormats.includes(mimetype)) {
      return Wordicon;
    }

    if (pdfFormats.includes(mimetype)) {
      return Pdficon;
    }

    return Pdficon;
  };

  const onChange = (value: any, files: any) => {
    if (!value?.trim()) return;

    let body :any= {
      query: stripHtmlTags(value?.trim()),
      sessionId: chatsessionId,
      roleId: `${LoginCcaasUserDetails?.roleId ?? ""}`,
      org_role_id_crm: LoginCcaasUserDetails?.org_role_id_crm?.toString() ?? "",
      userId: `${LoginCcaasUserDetails?.userId ?? ""}`,
      domainId: `${LoginCcaasUserDetails?.domainId ?? ""}`,
      module: "",
      ext: `${LoginCcaasUserDetails?.ext ?? ""}`,
      agent: "home",
      isBot: 0,
      files: files ?? [],
    };

    if (currentConversationType?.[`${chatsessionId}`]?.templateConvo) {
      body = { ...body, templateId: currentConversationType?.[`${chatsessionId}`]?.selectedTemplateId, channel: currentConversationType?.[`${chatsessionId}`]?.viewCard }
    }
    aiChatAssistSocketservice.io.emit("crm_chat", body);

    if (!(currentConversationType?.[`${chatsessionId}`]?.templateConvo)) {

      dispatch(homeDataSliceActions.setHomeAiAgentChat({ ...body, isBot: 0, query: body?.query }));
      // dispatch(MarketingAgentChatAction.setMarketingAgentChat({ ...body }))
      // dispatch(MarketingAgentChatAction.setMarketingAgentChat({ ...dummy }))
    } else {
      dispatch(MarketingAgentChatAction.setTemplateChatConversation({ ...body, templateId: currentConversationType?.[`${chatsessionId}`]?.selectedTemplateId, channel: currentConversationType?.[`${chatsessionId}`]?.viewCard }))
    }
    
    dispatch(homeDataSliceActions.setHomeAiAgentChatTyping({ sessionId: chatsessionId, typing: true }));
  };

    const handleBackToMainConvo = () => {
        try {

            dispatch(MarketingAgentChatAction.setSessionConversationType({ sessionId: chatsessionId, type: false }));
        } catch (error) {
            console.log("err", error)
        }
    }

    const templateConversation = (selectedTemplateCards: any, item: any) => {
      console.log("iten???", item?.[0]?.templateid, item, selectedTemplateCards)
      // setSelectedTemplateId(item?.[0]?.templateid);
      dispatch(MarketingAgentChatAction.setSessionConversationType({ sessionId: chatsessionId, templateConvo: true, selectedTemplateCard: selectedTemplateCards, viewCard: item?.[0]?.channel, selectedTemplateId: item?.[0]?.templateid }));
    }

  const fetchTemplateConversation = async () => {

    try {

      const selectedtemplateId = currentConversationType?.[`${chatsessionId}`]?.selectedTemplateId;
      dispatch(MarketingAgentChatAction.setMarketingTemplateChatLoader({ sessionId: chatsessionId, isLoader: true }));
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
      
        dispatch(MarketingAgentChatAction.setTemplateConversation((response?.data?.conversationDataRes?.[0]?.conversation) || []))
        dispatch(MarketingAgentChatAction.setMarketingTemplateChatLoader({ sessionId: chatsessionId, isLoader: false }));
      }
    } catch (error) {
        console.log("get_user_details_email error>>>>", error);
        dispatch(MarketingAgentChatAction.setMarketingTemplateChatLoader({ sessionId: chatsessionId, isLoader: false }));
    } finally {
        dispatch(MarketingAgentChatAction.setMarketingTemplateChatLoader({ sessionId: chatsessionId, isLoader: false }));
    }

  }


  return (
    <>

   
      <div>
          <div className={styles.chatarea}>
            {/* marketing section */}
            {currentConversationType?.[`${chatsessionId}`]?.templateConvo &&
              <div className={`${styles.campaigndetail} chatmaincontainer  ${currentConversationType?.[`${chatsessionId}`]?.templateConvo && styles.stickydetails}`}>

                <div className={styles.campaigndetailleft}>
<div style={{display:"flex",gap:"12px", alignItems:"baseline"}} onClick={() => handleBackToMainConvo()}>
                  <span style={{ display: "grid" }}>
                    <svg style={{ position: "relative", cursor: "pointer" }} xmlns="http://www.w3.org/2000/svg" width="8" height="16" viewBox="0 0 8 16" fill="none"><path d="M7.04593 15.9644C6.81453 15.9644 6.58302 15.8755 6.40723 15.6988L0.889625 10.147C-0.295875 8.95361 -0.296875 7.01131 0.888625 5.81791L6.40323 0.266115C6.75283 -0.0883855 7.32413 -0.0873855 7.67573 0.261215C8.02923 0.611814 8.03123 1.18111 7.68063 1.53371L2.16603 7.08551C1.67483 7.57961 1.67582 8.38342 2.16702 8.87751L7.68463 14.4293C8.03423 14.7818 8.03323 15.3522 7.68073 15.7027C7.50493 15.8765 7.27543 15.9644 7.04593 15.9644Z" fill="#404040"></path></svg>

                  </span>
                  <div className={styles.title} >
                  
                  {campaigndetail?.campaignName?campaigndetail?.campaignName?.charAt(0)?.toUpperCase()+campaigndetail?.campaignName?.slice(1):  "New campaign"}{<span>Primary Goal: {campaigndetail?.goal ?? "goal"}</span>}</div></div>
                  <div className={styles.tab}>
                    {Object.keys(currentConversationType?.[`${chatsessionId}`]?.selectedTemplateCard)?.length > 0 && Object.keys(currentConversationType?.[`${chatsessionId}`]?.selectedTemplateCard)?.map((item: any) => {
                      const allTemplateCards: any = currentConversationType?.[`${chatsessionId}`]?.selectedTemplateCard;
                      return (
                        <div className={`${styles.tabinner} ${(currentConversationType?.[`${chatsessionId}`]?.viewCard == item) ? styles.active : ''} `}
                          onClick={() => templateConversation(allTemplateCards, allTemplateCards[`${item}`])}
                        >

                          {(item?.charAt(0).toUpperCase() + item?.slice(1))}

                        </div>)
                    })
                    }
                  </div>
                </div>
              </div>}
            {/* marketing section */}
            <div className={styles.bubbleblock}>
              {(!(currentConversationType?.[`${chatsessionId}`]?.templateConvo)) && currentaiAgentChatMessage?.map((item: any, index: number) => {
                if (item?.isBot === 0 && (!item?.templateId)) {
                  const attachments = getAttachments(item);

                  return (
                    <div className={styles.bubbleblockagent} key={`u-${index}`}>
                      {attachments?.length > 0 ? (
                        <div className={styles.bubbleblockagenttextblock}>
                          <div className={`${styles.bubbleblockagenttext} ${styles.textwithimg}`}>
                            {attachments?.map((file: any, i: number) => {
                              const fileName = file?.name || file?.filename || "Attachment";
                              const fileSize = getFileSize(Number(file?.size) || 0) || "";
                              const fileUrl = getFileUrl(file);
                              const iconSrc = getIconSrc(file, fileUrl);
                              const isImage = iconSrc === fileUrl;

                              return (
                                <div className={`${isImage ? "" : styles.chateditorfile} `} key={file?.uuid || i}>
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
                                    isImage ? <></> : <div className={styles.right}>
                                      <p className="truncate1">{fileName}</p>
                                      <p className={styles.filesize}>{fileSize}</p>
                                    </div>
                                  }
                                </div>
                              );
                            })}
                            <div>
                              {item?.query}
                            </div>

                          </div>
                        </div>
                      ) : (
                        <div className={styles.bubbleblockagenttextblock}>
                          <div className={styles.bubbleblockagenttext}>{item?.query}</div>
                        </div>
                      )}
                    </div>
                  );
                }

                if (item?.isBot == 1 && item?.template_intent != "card" && (!item?.templateId)) {
                  return (
                    <div className={styles.bubbleblockcustomer}>
                      <div className={styles.bubbleblockcustomertextblock}>
                        <div className={`${styles.bubbleblockcustomertext}`}>
                          <div dangerouslySetInnerHTML={{ __html: item?.response }} />
                        </div>
                      </div>

                      {/* <ChatLikes message={msg?.response}/> */}
                    </div>
                  )
                }

                if (item?.isBot == 1 && item?.template_intent == "card") {
                  return (
                    <div className={styles.cardWrapper}>
                      <div className={styles.cardmaincontainer}>
                        {
                          Object?.keys(item?.channel || {})?.length > 0 &&
                          Object?.keys(item?.channel)?.map((channel: any) => {
                            return (
                              <div className={styles.cardisplay} onClick={() => console.log("data", channel, item?.channel, item?.channel?.[channel])}>
                                <div className={styles.cardcontainer}>
                                  <div className={styles.cardtitle}>{channel?.charAt(0)?.toUpperCase() + channel?.slice(1)}</div>
                                  <div className={styles.cardsubtitle}>Primary Goal: {item?.goal ?? "Goal"}</div>
                                </div>
                                <button onClick={() => {
                                  console.log("card---->", item)
                                  templateConversation(item?.channel, item?.channel?.[channel])
                                }}>View</button>
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
                    </div>
                  )
                }

                // return (
                //   <div className={styles.bubbleblockcustomer} key={`b-${index}`}>
                //     <div className={styles.bubbleblockcustomertextblock}>
                //       <div className={styles.bubbleblockcustomertext}>
                //         <div dangerouslySetInnerHTML={{ __html: item?.response }} />
                //       </div>
                //     </div>
                //   </div>
                // );
              })}

              {((currentConversationType?.[`${chatsessionId}`]?.templateConvo)) && marketingTemplateChatConversation?.[`${currentConversationType?.[`${chatsessionId}`]?.selectedTemplateId}`]?.map((item: any, index: number) => {
                if (item?.isBot === 0 && (item?.templateId)) {
                  const attachments = getAttachments(item);

                  return (
                    <div className={styles.bubbleblockagent} key={`u-${index}`}>
                      {attachments?.length > 0 ? (
                        <div className={styles.bubbleblockagenttextblock}>
                          <div className={`${styles.bubbleblockagenttext} ${styles.textwithimg}`}>
                            {attachments?.map((file: any, i: number) => {
                              const fileName = file?.name || file?.filename || "Attachment";
                              const fileSize = getFileSize(Number(file?.size) || 0) || "";
                              const fileUrl = getFileUrl(file);
                              const iconSrc = getIconSrc(file, fileUrl);
                              const isImage = iconSrc === fileUrl;

                              return (
                                <div className={`${isImage ? "" : styles.chateditorfile} `} key={file?.uuid || i}>
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
                                    isImage ? <></> : <div className={styles.right}>
                                      <p className="truncate1">{fileName}</p>
                                      <p className={styles.filesize}>{fileSize}</p>
                                    </div>
                                  }
                                </div>
                              );
                            })}
                            <div>
                              {item?.query}
                            </div>

                          </div>
                        </div>
                      ) : (
                        <div className={styles.bubbleblockagenttextblock}>
                          <div className={styles.bubbleblockagenttext}>{item?.query}</div>
                        </div>
                      )}
                    </div>
                  );
                }

                if (item?.isBot == 1 && item?.template_intent != "card") {
                  return (
                    <div className={styles.bubbleblockcustomer}>
                      <div className={styles.bubbleblockcustomertextblock}>
                        <div className={`${styles.bubbleblockcustomertext}`}>
                          <div dangerouslySetInnerHTML={{ __html: item?.response }} />
                        </div>
                      </div>

                      {/* <ChatLikes message={msg?.response}/> */}
                    </div>
                  )
                }


                // return (
                //   <div className={styles.bubbleblockcustomer} key={`b-${index}`}>
                //     <div className={styles.bubbleblockcustomertextblock}>
                //       <div className={styles.bubbleblockcustomertext}>
                //         <div dangerouslySetInnerHTML={{ __html: item?.response }} />
                //       </div>
                //     </div>
                //   </div>
                // );
              })}

              {!!aiAgentChatTyping?.[`${chatsessionId}`] && (
                <div className="msgbubble left">
                  <div className="chatContent">Typing ...</div>
                </div>
              )}
              <div ref={chatEndRef} />
            </div>
          </div>
      </div>

      
      <div className={`${styles.editorbottom} `} ref={boxRef}>
        <SetUpCommonEditor onChange={onChange} templateChat={true}/>
      </div>
      
    </>
  );
}
