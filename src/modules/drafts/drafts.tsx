import React, { useEffect, useLayoutEffect, useRef, useState } from 'react'
import styles from "../home/campaign/scss/elevatebrand.module.scss"
import ContentLayout from '../../layout/contentLayout'
import { whatsappService } from '../../base/service/apiservice'
import ChatLikes from '../../LikeDislike/chatLikes';
import { useLocation, useNavigate } from 'react-router-dom';
import WorktualModal from '../../components/custom_components/worktualModal';
import { format } from "date-fns";
import { assets, leftnavassets } from '../../styles/assets/assets';
import WorktualDropdown from '../../components/custom_components/WroktualDropdown';
import ClickTooltip from '../../components/custom_components/clickabletooltip/clickabletooltip';
import { useSelector } from 'react-redux';

function Drafts() {
    
    const selectedDraftData=useSelector((state :any)=>state.commonData.draftData);
    const [allDraftData, setAllDraftData] = useState<any>([]);
    const [conversationData, setConversationData] = useState<any>([]);
    const [selectedDraft, setSelectedDraft] = useState<any>([]);
    const [activeDraft, setActiveDraft] = useState<any>(0);
    const [templateView, setTemplateView] = useState<any>(null);
    const [editsPromptSms, setEditPromptSms] = useState<any>(false);
    const [editsPromptInAPP, setEditPromptInAPP] = useState<any>(false);
    const [editsPromptWebPush, setEditPromptWebPush] = useState<any>(false);
    const [editsPromptAppPush, setEditPromptAppPush] = useState<any>(false);
    const [editsPromptWhatsapp, setEditPromptWhatsapp] = useState<any>(false);


    const initialWebOption: any = {
        "label": "Chrome",
        "value": 1
    }
    const initialPhoneOption: any = {
        "label": "Android",
        "value": 1
    }

    const phoneOptions = [
        { label: 'Android', value: 1 },
        { label: 'Ios', value: 2 }
    ]

    const webPushOptions = [
        { label: 'Chrome', value: 1 },
        { label: 'Safari', value: 2 }
    ]

    const [webOptions, setWebOptions] = useState<any>(initialWebOption);
    const [appPushOptions, setappPushOptions] = useState<any>(initialPhoneOption);


    const editableRef = useRef<HTMLParagraphElement | null>(null);
    console.log("templateview", templateView)
    const [emailActive, setEmailActive] = useState(0);
    const pages = ["A", "B", "C", "D", "E"];
    const [active, setActive] = useState("A");
    const [onEdit, setOnEdit] = useState<any>(false);
    const navigate = useNavigate();
    const getAllDraft = async () => {
        try {
            whatsappService.get('/get_all_draft_campaign').then((res: any) => {
                console.log("getAllDraftres", res)
                if (res?.data?.statusCode == 200) {

                    getIndividualDraft(res?.data?.getAllDraftRes?.[0], 0)
                    setAllDraftData(res?.data?.getAllDraftRes);
                }
                else {
                    setAllDraftData([]);
                }
            })

        } catch (error: any) {
            console.log("err", error)
        }
    }
    const getIndividualDraft = async (data: any, index: any) => {
        console.log("getIndividualDraftsss", data, index)
        try {
            let body = {
                id: data?.id,
                campaignchatid: data?.campaignchatid,
                conversationName: data?.conversationName
                // conversationName: data?.conversationName??"New Campaign"
            }


            await whatsappService.post(body, '/get_draft_details_campaign').then((res: any) => {
                console.log("getIndividualDraft", res)
                if (res?.data?.statusCode == 200 && res?.data?.getDraftData?.[0]?.conversation?.length > 0) {
                    console.log("sfasfdsf", res?.data?.getDraftData?.[0]?.conversation)
                    setConversationData(JSON.parse(res?.data?.getDraftData?.[0]?.conversation))
                    setSelectedDraft(res?.data?.getDraftData?.[0])
                    setActiveDraft(index);
                    // setAllDraftData(res?.data?.getAllDraftRes);
                }
            })

        } catch (error: any) {
            console.log("err", error)
        }
    }

    const formatFileSize = (bytes?: number) => {
        if (!bytes) return "0 KB";
        const sizes = ["Bytes", "KB", "MB", "GB"];
        const i = Math.floor(Math.log(bytes) / Math.log(1024));
        const size = (bytes / Math.pow(1024, i)).toFixed(1);
        return `${size} ${sizes[i]}`;
    };

    const handleRestore = async () => {
        localStorage.setItem("campaignChatId", conversationData?.[0]?.campaignChatId);
        console.warn("chat id 5 restore:",conversationData?.[0]?.campaignChatId);
        navigate('/home', { state: { data: "draft", selectedDraftData: selectedDraft } })
    }

    const handleDelete = () => {
        try {
            
            console.log("campid", conversationData?.[0]?.campaignChatId)
            let body: any = {
                campaignchatid: conversationData?.[0]?.campaignChatId
            }
            whatsappService.post(body, '/delete_draft_campaign').then((res: any) => {
                if (res?.data?.statusCode == 200) {
                    getAllDraft();
                    console.log("draft called")
                }
            })
        } catch (error) {
            console.log("API ERROR:",error)
        }
    }

    const handleViewEmail = (channelKey: any, channelData: any, index: number) => {
        setOnEdit(true);
        // dispatch(campaignSliceActions.setTempleteView({ channels: channelKey, ...channelData, index }));
        setTemplateView({ channels: channelKey, ...channelData, index });


    };

    const handleViewSms = (channelKey: any, channelData: any, index: number) => {
        setOnEdit(true);
        // dispatch(campaignSliceActions.setTempleteView({ channels: channelKey, ...channelData, index }));
        setTemplateView({ channels: channelKey, ...channelData, index });

    };

    const handleViewAppPush = (channelKey: any, channelData: any, index: number) => {
        setOnEdit(true);
        // dispatch(campaignSliceActions.setTempleteView({ channels: channelKey, ...channelData, index }));
        setTemplateView({ channels: channelKey, ...channelData, index });

    };

    const handleViewWebPush = (channelKey: any, channelData: any, index: number) => {
        setOnEdit(true);
        // dispatch(campaignSliceActions.setTempleteView({ channels: channelKey, ...channelData, index }));
        setTemplateView({ channels: channelKey, ...channelData, index });

    };

    const handleViewWhatsapp = (channelKey: any, channelData: any, index: number) => {
        setOnEdit(true);
        // dispatch(campaignSliceActions.setTempleteView({ channels: channelKey, ...channelData, index }));
        setTemplateView({ channels: channelKey, ...channelData, index });

    };

    const handleViewInapp = (channelKey: any, channelData: any, index: number) => {
        setOnEdit(true);
        // dispatch(campaignSliceActions.setTempleteView({ channels: channelKey, ...channelData, index }));
        setTemplateView({ channels: channelKey, ...channelData, index });
    };

    const handleEmailClick = (page: any) => {
        if (page == '+' && emailActive < templateView?.template?.length - 1) {
            setActive(pages[emailActive + 1]);
            setEmailActive(emailActive + 1)
        }
        else if (page == '-' && emailActive > 0) {
            setActive(pages[emailActive - 1]);
            setEmailActive(emailActive - 1)
        }
    };


    const handleButtonTemplateClick = (ind: any) => {
        setActive(pages[ind]);
        setEmailActive(ind)
    }

    useEffect(() => {
        const init = async () => {
            // await getAllDraft();
            // console.log("draft",state?.selectedDraftData)
            getIndividualDraft(selectedDraftData?.draft,selectedDraftData?.index)
        }
        init();
    }, [selectedDraftData?.index])


      const contentHtml = useRef<any>(new Map());
      const [, forceUpdatecount] = useState(0);
    
    
      const fetchHtml = async (assistant_path: any) => {
        try {
          const res = await fetch(assistant_path);
          const html = await res.text();
          contentHtml.current.set(assistant_path, html);
          forceUpdatecount(prev => prev + 1);
        } catch (err) {
          console.error(err);
        }
      }

    return (
        <>


            <div className={`${styles.settingssec} `}>
                
                <div className={`${styles.rightsettingsmenu} rightsettingmenu `}>
                    <div className={`${styles.elevatebrandbody} createcampaignhome ${styles.createcampaignbody} ${styles.draftchat}`}>
                        <div className={`${styles.innerbody} `}>

                            {<div className='draftheader'>
                                <span>{selectedDraft?.conversationName ?? "New Campaign"}</span>
                                <span style={{ display: "flex", gap: "16px", alignItems: "center" }}>
                                    {/* <span style={{ cursor: "pointer" }} onClick={() => handleDelete()}>
                                        <svg xmlns="http://www.w3.org/2000/svg" width="18" height="21" viewBox="0 0 18 21" fill="none" >
                                            <path d="M7.2999 15.9995C6.8858 15.9995 6.5499 15.6636 6.5499 15.2495V6.6797C6.5499 6.2656 6.8858 5.9297 7.2999 5.9297C7.714 5.9297 8.0499 6.2656 8.0499 6.6797V15.2495C8.0499 15.6636 7.714 15.9995 7.2999 15.9995Z" fill="#404040" />
                                            <path d="M10.5401 15.9995C10.126 15.9995 9.7901 15.6636 9.7901 15.2495V6.6797C9.7901 6.2656 10.126 5.9297 10.5401 5.9297C10.9542 5.9297 11.2901 6.2656 11.2901 6.6797V15.2495C11.2901 15.6636 10.9542 15.9995 10.5401 15.9995Z" fill="#404040" />
                                            <path d="M17.0899 2.5298H13.1397V1.9199C13.1397 0.8613 12.3057 0 11.2803 0H6.5498C5.5244 0 4.6899 0.8613 4.6899 1.9199V2.5298H0.75C0.3359 2.5298 0 2.8657 0 3.2798C0 3.6939 0.3359 4.0298 0.75 4.0298H1.3543L2.1216 16.6734C2.2339 18.6119 3.8452 20.1304 5.7901 20.1304H12.0303C13.9737 20.1304 15.5938 18.6099 15.7188 16.6656L16.4866 4.0299H17.0899C17.504 4.0299 17.8399 3.694 17.8399 3.2799C17.8399 2.8658 17.504 2.5298 17.0899 2.5298ZM6.19 1.9199C6.19 1.6885 6.3516 1.5 6.5499 1.5H11.2804C11.4786 1.5 11.6398 1.6885 11.6398 1.9199V2.5298H6.1901L6.19 1.9199ZM14.2217 16.5718C14.1475 17.7261 13.1846 18.6304 12.0303 18.6304H5.7901C4.6392 18.6304 3.6851 17.7329 3.6187 16.5845L2.8569 4.0298H14.983L14.2217 16.5718Z" fill="#404040" />
                                        </svg>
                                    </span> */}
                                    <span style={{ cursor: "pointer" }}><button className='primarybtn' onClick={() => handleRestore()}>Restore</button></span>
                                   
                                </span>

                            </div>}
                            <div className={`${styles.msgbody} msgbody`}>
                                <div style={{ marginTop: "20px" }} className={`${styles.scroller} ${styles.chatflexscroll} ${styles.scrollerofdrafts}`}>
                                    <div className={styles.maincontainer}>

                                        {/* <div  className={`${styles.msgbubble} ${styles.left}`}>
                             www
                            </div>
                           
                            <div  className={`${styles.msgbubble} ${styles.right}`}>
                             www
                            </div> */}
                                        {Array.isArray(conversationData) && conversationData?.map((msg: any, index: number) => {
                                            if (msg.sender !== "botText" && msg?.template_chat==0) {
                                                return (
                                                    <>
                                                        {msg?.uploadUrl?.length > 0 && msg?.uploadUrl?.map((file: any, index: number) => {
                                                            return (
                                                                file.mimetype.startsWith("image/") ? (
                                                                    <a
                                                                        key={index}
                                                                        href={file.uploadUrl}
                                                                        target="_blank"
                                                                        rel="noopener noreferrer"
                                                                        className={`${styles.msgbubble} ${styles.right} imguploadedres`}
                                                                    >
                                                                        <img
                                                                            src={file.uploadUrl}
                                                                            alt={file.filename}
                                                                            style={{
                                                                                width: "40px",
                                                                                height: "40px",
                                                                                borderRadius: "6px",
                                                                                objectFit: "cover",
                                                                            }}
                                                                        />
                                                                    </a>
                                                                ) : (
                                                                    <a
                                                                        key={index}
                                                                        href={file.uploadUrl}
                                                                        target="_blank"
                                                                        rel="noopener noreferrer"
                                                                        className={`${styles.fileupload} ${styles.msgbubble} ${styles.right}`}
                                                                    >
                                                                        <div className={styles.lefts}>
                                                                            <img src={file.mimetype.includes("pdf")?assets.pdficon:file.mimetype.includes("txt")?assets.uploaddoc:assets.uploadxlsx} alt="file-icon" />
                                                                        </div>
                                                                        <div className={styles.rights}>
                                                                            <h5>{file?.filename || "Unknown File"}</h5>
                                                                            <p>
                                                                                {file?.mimetype.includes("pdf") ? "PDF" : file?.mimetype.includes("msword") ? "WORD" : file?.mimetype.split("/")[1]?.toUpperCase() || "FILE"}
                                                                                {" • "}
                                                                                {formatFileSize(file?.size)}
                                                                            </p>
                                                                        </div>
                                                                    </a>
                                                                ))
                                                        }
                                                        )}
                                                        <div key={msg?.messageId} className={`${styles.msgbubble} ${styles.right}`}>
                                                            {/* <div className="chatContent" dangerouslySetInnerHTML={{ __html: msg?.message }} /> */}
                                                            <div
                                                                className="chatContent"
                                                                dangerouslySetInnerHTML={{
                                                                    __html: msg?.message
                                                                        ?.replace(/&nbsp;|\u00A0/g, ' ')  // ✅ convert non-breaking → normal space
                                                                        .replace(/\n/g, '<br/>')
                                                                }}
                                                            />
                                                        </div>
                                                    </>
                                                );
                                            }
                                            if (msg.sender === "botText" && msg?.template_chat==0) {
                                                let textToShow = msg?.bot_response;

                                                if (msg?.graph_url && !contentHtml.current.has(msg?.graph_url)) {
                                                    fetchHtml(msg?.graph_url)
                                                }
                                                return (
                                                    <>

                                                        {(msg?.graph_url) && ( <iframe srcDoc={contentHtml.current.get(msg?.graph_url)} /> )}

                                                        <div key={msg?.messageId} className={`${styles.msgbubble} ${styles.left}`}>
                                                            <div className="chatContent" dangerouslySetInnerHTML={{ __html: textToShow }} />

                                                            {/* {(msg?.intent !== "refresh_dashboard" && textToShow?.trim()?.length > 0) && <ChatLikes changeindex={index} allConversation={conversationData} likeOrDislike={msg?.likeOrDislike} feedBackDataProp={msg?.feedbackData} msg={msg} campaignChatId={msg?.campaignChatId} message={msg?.bot_response}></ChatLikes>} */}
                                                        </div>


                                                        {msg?.intent === "create_campaign" && (
                                                            <div className={`${styles.msgbubble} ${styles.left} ${styles.cardsresponse}`}>
                                                                <div className={styles.cardscontainerscroller}>
                                                                    {Object.entries(msg?.campaign_data?.channels)?.map(([channelKey, channelData]: any) => (
                                                                        <div key={channelKey} className={styles.card}>
                                                                            <div className={styles.top}>
                                                                                <div className={styles.lefts}>
                                                                                    <img
                                                                                        src={
                                                                                            channelKey === "email"
                                                                                                ? assets.emailicncard
                                                                                                : channelKey === "sms"
                                                                                                    ? assets.smsicncard
                                                                                                    : channelKey === "whatsapp"
                                                                                                        ? assets.whtsappicncard
                                                                                                        : channelKey === "apppush"
                                                                                                            ? assets.apppushicon
                                                                                                            : channelKey === "webpush"
                                                                                                                ? assets.webpushicon
                                                                                                                : channelKey === "inapp"
                                                                                                                    ? assets.inappicon
                                                                                                                    : assets.whtsappicncard
                                                                                        }
                                                                                        alt={channelKey}
                                                                                    />
                                                                                </div>
                                                                                <div className={styles.rights}>
                                                                                    <h4>{channelKey?.toUpperCase()}</h4>
                                                                                    <p>{channelData?.customer_description}</p>
                                                                                </div>
                                                                            </div>
                                                                            <div className={`${styles.top} ${styles.bottom}`}>
                                                                                <div className={styles.lefts}>
                                                                                    <span className={styles.timerspan}>
                                                                                        <svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" fill="none" viewBox="0 0 20 20">
                                                                                            <path d="M10 1.5a8.5 8.5 0 1 0 0 17 8.5 8.5 0 0 0 0-17Zm0 15.3a6.8 6.8 0 1 1 0-13.6 6.8 6.8 0 0 1 0 13.6Z" fill="black" />
                                                                                            <path d="M10.6 5.2H9.2v5.1l4.5 2.7.7-1.1-3.8-2.3V5.2Z" fill="black" />
                                                                                        </svg>
                                                                                        <span>{channelData?.schedule_time}</span>
                                                                                    </span>
                                                                                </div>
                                                                                {channelKey === "email" &&
                                                                                    <button className={`viewbutton`} onClick={() => handleViewEmail(channelKey, channelData, index)}>View</button>
                                                                                }
                                                                                {channelKey === "sms" &&
                                                                                    <button className={`viewbutton`} onClick={() => handleViewSms(channelKey, channelData, index)}>View</button>
                                                                                }
                                                                                {channelKey === "apppush" &&
                                                                                    <button className={`viewbutton`} onClick={() => handleViewAppPush(channelKey, channelData, index)}>View</button>
                                                                                }
                                                                                {channelKey === "webpush" &&
                                                                                    <button  className={`viewbutton`} onClick={() => handleViewWebPush(channelKey, channelData, index)}>View</button>
                                                                                }
                                                                                {channelKey === "whatsapp" &&
                                                                                    <button className={`viewbutton`} onClick={() => handleViewWhatsapp(channelKey, channelData, index)}>View</button>
                                                                                }
                                                                                {channelKey === "inapp" &&
                                                                                    <button className={`viewbutton`} onClick={() => handleViewInapp(channelKey, channelData, index)}>View</button>
                                                                                }
                                                                            </div>
                                                                        </div>
                                                                    ))}



                                                                </div >
                                                            </div>
                                                        )}

                                                        {/* Email Modal */}
                                                        {/* {templateView?.channels === "email" && ( */}
                                                        {false && (
                                                            <div className='closetting lgbgmodal'>
                                                                <WorktualModal
                                                                    show={onEdit}
                                                                    mdsize="modallg"
                                                                >
                                                                    <div className={`${styles.emailsmsent} stickyheader `}>
                                                                        <div className={styles.leftcampaign}>
                                                                            <h3>AI Campaign manager - Email</h3>
                                                                            <p>{`Created on: ${format(new Date(), "dd MMM, yyyy • hh:mm a")}`}</p>
                                                                        </div>
                                                                        <div className={styles.rightcampaign}>
                                                                            <button className={styles.schedulebtn}
                                                                                onClick={() => setOnEdit(false)}
                                                                            >
                                                                                Close
                                                                            </button>
                                                                        </div>
                                                                    </div>
                                                                    <div className={`${styles.campaignmangerbody} ${styles.scrollerinner} `}>
                                                                        <div className={`${styles.scroller} ${styles.scrollerfix} ${styles.emailscroller}`}>
                                                                            {
                                                                                <div className={styles.responseslider}>
                                                                                    {templateView?.length > 1 &&
                                                                                        <button className={styles.arrow} onClick={() => handleEmailClick('-')}><svg xmlns="http://www.w3.org/2000/svg" width="6" height="13" viewBox="0 0 6 13" fill="none" >
                                                                                            <path d="M5.15277 12.0655C4.96137 12.0655 4.76947 11.9923 4.62297 11.8467L0.758268 7.99325C0.332468 7.56845 0.0976688 7.00305 0.0971688 6.40045C0.0966688 5.79785 0.331568 5.23245 0.758268 4.80575L4.62347 0.953248C4.91647 0.661248 5.39157 0.661248 5.68397 0.955248C5.97647 1.24825 5.97547 1.72285 5.68197 2.01575L1.81727 5.86825C1.67517 6.00985 1.59707 6.19935 1.59707 6.40045C1.59707 6.60065 1.67517 6.78915 1.81727 6.93075L5.68247 10.7842C5.97547 11.0772 5.97637 11.5518 5.68397 11.8447C5.53747 11.9922 5.34507 12.0655 5.15277 12.0655Z" fill="black" />
                                                                                        </svg></button>
                                                                                    }
                                                                                    <div className={styles.response}>
                                                                                        <div>{`subject: ${templateView?.template?.[emailActive]?.subject || ""}`}</div>
                                                                                        <div
                                                                                            className="chatContent"
                                                                                            dangerouslySetInnerHTML={{
                                                                                                __html: templateView?.template?.[emailActive]?.template || "",
                                                                                            }}
                                                                                        />
                                                                                    </div>
                                                                                    {templateView?.length > 1 &&
                                                                                        <button className={styles.arrow} onClick={() => handleEmailClick('+')}><svg xmlns="http://www.w3.org/2000/svg" width="6" height="13" viewBox="0 0 6 13" fill="none" >
                                                                                            <path d="M0.847607 12.1654C0.655207 12.1654 0.462808 12.0922 0.316408 11.9447C0.0239078 11.6517 0.0249076 11.1771 0.318408 10.8842L4.18271 7.03171C4.32481 6.89011 4.40291 6.70061 4.40291 6.49951C4.40291 6.29931 4.32481 6.11081 4.18271 5.96921L0.318008 2.11571C0.025008 1.82271 0.0241079 1.34811 0.316508 1.05521C0.608508 0.761309 1.08361 0.761309 1.37751 1.05321L5.24181 4.90671C5.66761 5.33151 5.90241 5.89691 5.90291 6.49951C5.90341 7.10211 5.66851 7.66751 5.24181 8.09421L1.37711 11.9467C1.23111 12.0922 1.03901 12.1654 0.847607 12.1654Z" fill="black" />
                                                                                        </svg></button>
                                                                                    }

                                                                                </div>
                                                                            }
                                                                        </div>
                                                                        {/* actions */}

                                                                        <div className={styles.lgbgblock}>
                                                                            <div className={styles.footerPagination}>
                                                                                {templateView?.template?.length > 1 &&
                                                                                    <>
                                                                                        {pages.map((page: any, ind: any) => (
                                                                                            (ind <= templateView?.template?.length - 1) &&
                                                                                            <button
                                                                                                key={page}
                                                                                                className={`${styles.page} ${active === page ? styles.active : ""}`}
                                                                                                onClick={() => handleButtonTemplateClick(ind)}
                                                                                            >
                                                                                                {page}
                                                                                            </button>

                                                                                        ))}
                                                                                    </>
                                                                                }
                                                                            </div>

                                                                        </div>

                                                                    </div>
                                                                </WorktualModal>
                                                            </div>
                                                        )}

                                                        {/* SMS Modal */}
                                                        {/* {templateView?.channels === "sms" && ( */}
                                                        {false && (
                                                            <div className='closetting lgbgmodal smstmode' >
                                                                <WorktualModal show={onEdit} mdsize="modalxxl" footer={
                                                                    <div className={styles.lgbgblock}>
                                                                        <div className={`${styles.actionsblock} ${styles.actionblocknew} ${styles.smsactionblock}`}>
                                                                        </div>
                                                                    </div>
                                                                } customTitle={
                                                                    <div className={`${styles.emailsmsent} stickyheader `}>
                                                                        <div className={styles.leftcampaign}>
                                                                            <h3>AI Campaign manager - SMS</h3>
                                                                            <p>{`Created on: ${format(new Date(), "dd MMM, yyyy • hh:mm a")}`}</p>
                                                                        </div>
                                                                        <div className={styles.rightcampaign}>
                                                                            <button className={styles.schedulebtn} onClick={() => setOnEdit(false)}>
                                                                                Close
                                                                            </button>
                                                                        </div>
                                                                    </div>
                                                                }>
                                                                    <div className={`${styles.discoversub} ${styles.scrollerinner} ${styles.scrosts}`}>
                                                                        {/* sms preview layout */}
                                                                        <div className={`${styles.mobilelayoutviewsec} ${editsPromptSms ? styles.mobileheightzoom : ''}`}>
                                                                            <div className={styles.bodywrapper}>
                                                                                <div className={styles.mobileheader}>
                                                                                    <img className={styles.topbarimg} src={assets.mobile_topbar} alt="topbarimg" />
                                                                                    <img className={styles.camerafront} src={assets.camera_front} alt="camerafront" />
                                                                                </div>
                                                                                <div className={`${styles.mobilemainsec} ${styles.mobilemargin}`}>
                                                                                    <div className={styles.userinfotop}>
                                                                                        <span className={styles.backicon}>←</span>
                                                                                        <div className={styles.contactinfo}>
                                                                                            <span className={styles.avatorcon}>
                                                                                                <img src={assets.avatarimg} alt="avatar" />
                                                                                            </span>
                                                                                            <p>+447441476247</p>
                                                                                        </div>
                                                                                    </div>
                                                                                    {
                                                                                        <div className={styles.contentsec}>
                                                                                            <div className={styles.msgsec}>
                                                                                                <div dangerouslySetInnerHTML={{ __html: templateView?.template?.[0]?.template }} />
                                                                                            </div>
                                                                                        </div>
                                                                                    }
                                                                                </div>
                                                                                <div className={styles.mobilefooter}>
                                                                                    <img src={assets.message_bar} alt="msgbar" />
                                                                                </div>
                                                                            </div>
                                                                        </div>

                                                                        {/* actions */}
                                                                    </div>
                                                                </WorktualModal>
                                                            </div>
                                                        )}

                                                        {/* IN-APP Modal */}
                                                        {/* {templateView?.channels === "inapp" && ( */}
                                                        {false && (
                                                            <div className='closetting lgbgmodal smstmode' >
                                                                <WorktualModal show={true} mdsize="modalxxl" footer={
                                                                    <div className={styles.lgbgblock}>
                                                                        <div className={`${styles.actionsblock} ${styles.actionblocknew} ${styles.smsactionblock}`}>


                                                                        </div>
                                                                    </div>
                                                                } customTitle={
                                                                    <div className={`${styles.emailsmsent} stickyheader `}>
                                                                        <div className={styles.leftcampaign}>
                                                                            <h3>AI Campaign manager - In App</h3>
                                                                            <p>{`Created on: ${format(new Date(), "dd MMM, yyyy • hh:mm a")}`}</p>
                                                                        </div>
                                                                        <div className={styles.rightcampaign}>
                                                                            <button className={styles.schedulebtn} onClick={() => setOnEdit(false)}>
                                                                                Close
                                                                            </button>
                                                                        </div>
                                                                    </div>
                                                                }>
                                                                    <div className={`${styles.discoversub} ${styles.scrollerinner} ${styles.scrosts}`}>
                                                                        {/* sms preview layout */}
                                                                        <div className={`${styles.mobilelayoutviewsec} ${editsPromptInAPP ? styles.mobileheightzoom : ''}`}>
                                                                            <div className={styles.bodywrapper}>
                                                                                <div className={styles.mobileheader}>
                                                                                    <img className={styles.topbarimg} src={assets.mobile_topbar} alt="topbarimg" />
                                                                                    <img className={styles.camerafront} src={assets.camera_front} alt="camerafront" />
                                                                                </div>
                                                                                <div className={`${styles.mobilemainsec} ${styles.mobilemargin}`}>
                                                                                    <div className={`${styles.userinfotop} ${styles.contactsms}`}>
                                                                                        <p><img src={assets.multi_close} alt="" /></p>
                                                                                    </div>
                                                                                    {
                                                                                        <div className={styles.contentsec}>
                                                                                            <div>
                                                                                                {/* <img src={assets.multi_close} alt="" /> */}
                                                                                                <img src={templateView?.template?.[0]?.imageUrl} alt="" />
                                                                                            </div>
                                                                                            <div className={styles.bigsaleummer}>
                                                                                                <div dangerouslySetInnerHTML={{ __html: templateView?.template?.[0]?.template }} />
                                                                                                {/* <h3>Big Sale summer</h3>
                                                    <p>fhdgfgdjfdfdfdfdgsgdsadsadsa djsadjksaj </p>
                                                    <p>fhdgfgdjfdfdfdfdgsgdsadsadsa djsadjksaj </p> */}
                                                                                            </div>
                                                                                        </div>
                                                                                    }
                                                                                </div>
                                                                                {/* <div className={styles.mobilefooter}>
                                              <img src={assets.message_bar} alt="msgbar" />
                                            </div> */}
                                                                            </div>
                                                                        </div>

                                                                        {/* actions */}
                                                                    </div>
                                                                </WorktualModal>
                                                            </div>
                                                        )}

                                                        {/* App push*/}
                                                        {/* {templateView?.channels === "apppush" && ( */}
                                                        {false && (
                                                            <div className='closetting closettingheig lgbgmodal' >
                                                                <WorktualModal show={onEdit} mdsize="modalxxl" customTitle={<>
                                                                    <div className={`${styles.emailsmsent} stickyheader `}>
                                                                        <div className={styles.leftcampaign}>
                                                                            <h3>AI Campaign manager - App Push</h3>
                                                                            <p>{`Created on: ${format(new Date(), "dd MMM, yyyy • hh:mm a")}`}</p>
                                                                        </div>
                                                                        <div className={styles.rightcampaign}>
                                                                            <WorktualDropdown
                                                                                options={phoneOptions} onChange={(e: any) => setappPushOptions(e)}

                                                                            >
                                                                                <button className="whitebtnnew">
                                                                                    {appPushOptions?.label}
                                                                                    <img src={leftnavassets.chevrondown_icon} alt="downarrow" />
                                                                                </button>
                                                                            </WorktualDropdown>
                                                                            <button className={styles.schedulebtn} onClick={() => setOnEdit(false)}>
                                                                                Close
                                                                            </button>
                                                                        </div>
                                                                    </div>
                                                                </>} footer={<>
                                                                    <div className={styles.lgbgblock}>
                                                                        <div className={`${styles.actionsblock} ${styles.actionblocknew} ${styles.smsactionblock}`}>


                                                                        </div>
                                                                    </div>
                                                                </>}>

                                                                    <div className={`${styles.discoversub} ${styles.scrollerinner} ${styles.heightsetmodalsub}`}>
                                                                        <div className={`${styles.mobilemainsec} ${styles.mobilemargin}`}>
                                                                            {
                                                                                <div className={styles.contentsec}>
                                                                                    <div className={styles.discoversub}>
                                                                                        <div className={`${styles.mobilelayoutviewsec} ${styles.mobilelayoutapppushnotify} ${appPushOptions?.value == 1 ? styles.lockscreenandroid : styles.lockscreeniphone} `}>
                                                                                            <div className={styles.bodywrapper}>
                                                                                                <div className={styles.pushnotifications}>
                                                                                                    <img className={styles.notifylogo} src={assets.notify_logo} alt="notifylogo" />
                                                                                                    <div className={styles.pushnotifycenter}>
                                                                                                        <p className={styles.notifytxt}>
                                                                                                            <div dangerouslySetInnerHTML={{ __html: templateView?.template?.[0]?.template }} />
                                                                                                        </p>
                                                                                                    </div>
                                                                                                    <div className={styles.notifyaccord}>
                                                                                                        <svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 14 14" fill="none">
                                                                                                            <path d="M1.82471 4.57617L6.80101 9.55247L11.7773 4.57617" stroke="#1A1C19" stroke-width="1.45142" />
                                                                                                        </svg>
                                                                                                    </div>
                                                                                                </div>
                                                                                            </div>
                                                                                        </div>
                                                                                    </div>
                                                                                </div>
                                                                            }
                                                                        </div>
                                                                        {/* actions */}
                                                                    </div>

                                                                </WorktualModal>
                                                            </div>
                                                        )}

                                                        {/* web push */}
                                                        {/* {onEdit && templateView?.channels === "webpush" && ( */}
                                                        {false && (
                                                            <div className='closetting lgbgmodal smstmode' >
                                                                <WorktualModal show={onEdit} mdsize="modalxxl" customTitle={
                                                                    <>
                                                                        <div className={`${styles.emailsmsent} stickyheader `}>
                                                                            <div className={styles.leftcampaign}>
                                                                                <h3>AI Campaign manager - Web Push</h3>
                                                                                <p>{`Created on: ${format(new Date(), "dd MMM, yyyy • hh:mm a")}`}</p>
                                                                            </div>
                                                                            <div className={styles.rightcampaign}>
                                                                                <WorktualDropdown
                                                                                    options={webPushOptions} onChange={(opt: any) => setWebOptions(opt)}
                                                                                >
                                                                                    <button className="whitebtnnew">
                                                                                        {webOptions?.label}
                                                                                        <img src={leftnavassets.chevrondown_icon} alt="downarrow" />
                                                                                    </button>
                                                                                </WorktualDropdown>
                                                                                <button className={styles.schedulebtn} onClick={() => setOnEdit(false)}>
                                                                                    Close
                                                                                </button>
                                                                            </div>
                                                                        </div>
                                                                    </>
                                                                } footer={
                                                                    <>
                                                                        <div className={styles.lgbgblock}>
                                                                            <div className={`${styles.actionsblock} ${styles.actionblocknew} ${styles.smsactionblock}`}>

                                                                            </div>
                                                                        </div>
                                                                    </>
                                                                }>

                                                                    <div className={`${styles.discoversub} ${styles.scrollerinner} ${styles.heightsetmodal}`}>

                                                                        {
                                                                            <>

                                                                                {webOptions?.value == 1 ?
                                                                                    <div className={`${styles.discoversub} ${styles.heightsetmodal}`}>
                                                                                        <div className={styles.discoversubcontent}>
                                                                                            <div className={styles.prompteditor}>
                                                                                                <img src={assets.Productivedots} alt="" />
                                                                                            </div>
                                                                                            <div className={styles.icontaset}>
                                                                                                <div className={styles.androidwebpush}>
                                                                                                    <div className={styles.chromecontainer}>
                                                                                                        <div className={styles.chromeleft}>
                                                                                                            <img src={assets.Chromeapp} alt='Chromewebpush' />
                                                                                                            <h3>Google Chrome</h3>
                                                                                                        </div>
                                                                                                        <div className={styles.chromeright}>
                                                                                                            <svg xmlns="http://www.w3.org/2000/svg" width="19" height="6" viewBox="0 0 19 6" fill="none">
                                                                                                                <path fill-rule="evenodd" clip-rule="evenodd" d="M13.9874 3.00034C13.9874 4.23424 14.997 5.24384 16.2309 5.24384C17.4648 5.24384 18.4744 4.23424 18.4744 3.00034C18.4744 1.76644 17.4648 0.756836 16.2309 0.756836C14.997 0.756836 13.9874 1.76644 13.9874 3.00034ZM11.7439 3.00034C11.7439 1.76644 10.7343 0.756836 9.50037 0.756836C8.26647 0.756836 7.25687 1.76644 7.25687 3.00034C7.25687 4.23424 8.26647 5.24384 9.50037 5.24384C10.7343 5.24384 11.7439 4.23424 11.7439 3.00034ZM2.76987 0.756836C4.00377 0.756836 5.01337 1.76644 5.01337 3.00034C5.01337 4.23424 4.00377 5.24384 2.76987 5.24384C1.53597 5.24384 0.526367 4.23424 0.526367 3.00034C0.526367 1.76644 1.53597 0.756836 2.76987 0.756836Z" fill="#757676" />
                                                                                                            </svg>
                                                                                                            <svg xmlns="http://www.w3.org/2000/svg" width="10" height="10" viewBox="0 0 10 10" fill="none">
                                                                                                                <path d="M5.52964 5.28274L9.34984 1.46244C9.49634 1.31594 9.49634 1.07864 9.34984 0.932191C9.20334 0.785741 8.96604 0.785691 8.81959 0.932191L4.99939 4.75249L1.17919 0.932141C1.03269 0.785641 0.795388 0.785641 0.648938 0.932141C0.502488 1.07864 0.502437 1.31594 0.648938 1.46239L4.46914 5.28269L0.648938 9.10299C0.502437 9.24949 0.502437 9.48679 0.648938 9.63324C0.722188 9.70649 0.818137 9.74309 0.914087 9.74309C1.01004 9.74309 1.10599 9.70649 1.17924 9.63324L4.99944 5.81294L8.81964 9.63324C8.89289 9.70649 8.98884 9.74309 9.08479 9.74309C9.18074 9.74309 9.27669 9.70649 9.34994 9.63324C9.49644 9.48674 9.49644 9.24944 9.34994 9.10299L5.52964 5.28274Z" fill="#757676" />
                                                                                                            </svg>
                                                                                                        </div>
                                                                                                    </div>
                                                                                                    <div dangerouslySetInnerHTML={{ __html: templateView?.template?.[0]?.template }} />
                                                                                                </div>
                                                                                            </div>
                                                                                        </div>
                                                                                    </div>
                                                                                    // {/* safari */}
                                                                                    :
                                                                                    <div className={styles.discoversub}>
                                                                                        <div className={styles.discoversubcontent}>
                                                                                            <div className={styles.prompteditor}>
                                                                                                <img src={assets.Productivedots} alt="" />
                                                                                            </div>
                                                                                            <div className={styles.icontaset}>
                                                                                                <div className={styles.safarilogo}>
                                                                                                    <img src={assets.Safaridots} alt='Safari' />
                                                                                                    <div className={styles.productivity}>
                                                                                                        <div dangerouslySetInnerHTML={{ __html: templateView?.template?.[0]?.template }} />
                                                                                                    </div>
                                                                                                </div>
                                                                                            </div>
                                                                                        </div>
                                                                                    </div>
                                                                                }
                                                                            </>
                                                                        }
                                                                        {/* actions */}
                                                                    </div>
                                                                </WorktualModal>
                                                            </div>
                                                        )}

                                                        {/* Whatsapp */}
                                                        {/* {templateView?.channels === "whatsapp" && ( */}
                                                        {false && (
                                                            <div className='closetting lgbgmodal smstmode footerpadnone' >
                                                                <WorktualModal show={onEdit} mdsize="modalxxl" footer={
                                                                    <div style={{ position: "relative" }} className={styles.lgbgblock}>
                                                                        <div className={`${styles.actionsblock} ${styles.actionblocknew} ${styles.smsactionblock}`}>


                                                                        </div>

                                                                        {editsPromptWhatsapp && <ClickTooltip
                                                                            trigger={
                                                                                <div className={styles.banner}>
                                                                                    <span>Against WhatsApp guidelines</span>
                                                                                    <a href="#" className={styles.link}>
                                                                                        Read fallback
                                                                                    </a>
                                                                                </div>
                                                                            }
                                                                        >
                                                                            <div className="whiteScroll" style={{ maxHeight: "200px", overflow: "auto", color: "#fff" }}>
                                                                                <b style={{ marginBottom: "16px", fontFamily: "intermedium", fontSize: "16px", display: "block" }}>Structure</b>
                                                                                <p>Header (optional): Text, image, video, or document (1 element).</p>
                                                                                <p>Body: Main text (≤1024 chars) with variables 1, 2 etc.</p>
                                                                                <p>Footer (optional): Short text (≤60 chars).</p>
                                                                                <p>Buttons (optional): Up to 3 (URL, call, or quick reply).</p>
                                                                            </div>
                                                                        </ClickTooltip>}
                                                                    </div>
                                                                } customTitle={
                                                                    <div className={`${styles.emailsmsent} stickyheader `}>
                                                                        <div className={styles.leftcampaign}>
                                                                            <h3>AI Campaign manager - WHATSAPP</h3>
                                                                            <p>{`Created on: ${format(new Date(), "dd MMM, yyyy • hh:mm a")}`}</p>
                                                                        </div>
                                                                        <div className={styles.rightcampaign}>
                                                                            <button className="cancelbtn"
                                                                                onClick={() => window.open("https://developers.facebook.com/docs/whatsapp/message-templates/guidelines/", "_blank")}>
                                                                                Whatsapp guidelines
                                                                            </button>
                                                                            <button className={styles.schedulebtn} onClick={() => setOnEdit(false)}>
                                                                                Close
                                                                            </button>
                                                                        </div>
                                                                    </div>
                                                                }>
                                                                    <div style={{ marginBottom: "32px" }} className={`${styles.discoversub} ${styles.scrollerinner} ${styles.scrosts}`}>
                                                                        {/* whatsapp preview layout */}
                                                                        <div className={`${styles.mobilelayoutviewsec} ${editsPromptSms ? styles.mobileheightzoom : ''}`}>
                                                                            <div className={styles.bodywrapper}>
                                                                                <div className={styles.mobileheader}>
                                                                                    <img className={styles.topbarimg} src={assets.mobile_topbar} alt="topbarimg" />
                                                                                    <img className={styles.camerafront} src={assets.camera_front} alt="camerafront" />
                                                                                </div>
                                                                                <div className={`${styles.mobilemainsec} ${styles.mobilemargin} ${styles.whtsappmobmargin}`}>
                                                                                    <div className={styles.userinfotop}>
                                                                                        <span className={styles.backicon}>←</span>
                                                                                        <div style={{ display: "flex", alignItems: "center", gap: "5px", width: "calc(100% - 54px)" }} className={styles.contactinfo}>
                                                                                            <span className={styles.avatorcon}>
                                                                                                <img src={assets.avatarimg} alt="avatar" />
                                                                                            </span>
                                                                                            <p style={{ margin: "0px" }}>+447441476247</p>
                                                                                        </div>
                                                                                    </div>
                                                                                    {
                                                                                        <div className={`${styles.contentsec} ${styles.contentsecwhtapp}`}>
                                                                                            <div className={styles.msgsec}>
                                                                                                {/* Header */}
                                                                                                {templateView?.template?.[0]?.headerType === "Image" && (
                                                                                                    <div className={styles.imgcon}>
                                                                                                        <img
                                                                                                            src={templateView?.template?.[0]?.headerValue}
                                                                                                            alt="header image"
                                                                                                        />
                                                                                                    </div>
                                                                                                )}
                                                                                                {templateView?.template?.[0]?.headerType === "Text" && (
                                                                                                    <div className={styles.headertxtwhtsapp}>

                                                                                                        <p>{templateView?.template?.[0]?.headerValue}</p>
                                                                                                    </div>
                                                                                                )}
                                                                                                {/* Body */}
                                                                                                <div
                                                                                                    className={styles.bodymessagewrapper}
                                                                                                    dangerouslySetInnerHTML={{
                                                                                                        __html:
                                                                                                            templateView?.template?.[0]?.bodymessage ?? "",
                                                                                                    }}
                                                                                                />
                                                                                                {/* Footer */}
                                                                                                <div
                                                                                                    className={styles.footerwrapper}
                                                                                                    dangerouslySetInnerHTML={{
                                                                                                        __html: templateView?.template?.[0]?.footer ?? "",
                                                                                                    }}
                                                                                                />
                                                                                                {/* Buttons */}
                                                                                                {templateView?.template?.[0]?.buttonType?.map(
                                                                                                    (button: any, index: number) => {
                                                                                                        let label = "";
                                                                                                        if (button?.action?.value === "calltoAction") {
                                                                                                            label = button?.website?.label ?? button?.buttonText;
                                                                                                        } else if (button?.action?.value === "quickReply") {
                                                                                                            label = button?.buttonText;
                                                                                                        }
                                                                                                        console.log("label>>>>>>>>", label)
                                                                                                        return (
                                                                                                            <>
                                                                                                                <div className={styles.insidebtn} key={index}>
                                                                                                                    {label === "Visit website" && <span className='link'><svg xmlns="http://www.w3.org/2000/svg" width="18" height="18" viewBox="0 0 18 18" fill="none">
                                                                                                                        <path d="M13.227 11.9966H13.2231C12.811 11.9966 12.4751 12.3296 12.4731 12.7427L12.4643 14.3687C12.4643 15.1265 11.8442 15.7437 11.0825 15.7437H3.47897C2.71727 15.7437 2.09717 15.1236 2.09717 14.3619V6.75107C2.09717 5.98937 2.71727 5.36927 3.47897 5.36927H6.38917C6.80327 5.36927 7.13917 5.03337 7.13917 4.61927C7.13917 4.20517 6.80327 3.86927 6.38917 3.86927H3.47897C1.89007 3.86927 0.597168 5.16227 0.597168 6.75107V14.3619C0.597168 15.9508 1.89017 17.2437 3.47897 17.2437H11.0825C12.6714 17.2437 13.9643 15.9537 13.9643 14.3726L13.9731 12.7505C13.9751 12.3364 13.6411 11.9985 13.227 11.9966Z" fill="#0096DE" />
                                                                                                                        <path d="M16.8403 4.33297L13.4829 0.976067C13.1899 0.683067 12.7153 0.683067 12.4224 0.976067C12.1295 1.26907 12.1294 1.74367 12.4224 2.03657L15.4141 5.02767C15.3704 5.01867 15.3308 4.99957 15.2847 4.99847C8.10397 4.83537 7.22607 6.10927 6.81497 11.1977C6.78177 11.6108 7.08937 11.9721 7.50247 12.0063C7.52297 12.0073 7.54347 12.0083 7.56397 12.0083C7.95067 12.0083 8.27877 11.7114 8.31107 11.3188C8.64217 7.20547 8.71047 6.34607 15.2515 6.49847C15.2613 6.49847 15.2691 6.49367 15.2788 6.49347L12.4224 9.34907C12.1294 9.64207 12.1294 10.1167 12.4224 10.4096C12.5689 10.5561 12.7603 10.6293 12.9527 10.6293C13.1441 10.6293 13.3365 10.5561 13.483 10.4096L16.8404 7.05317C17.5904 6.30317 17.5903 5.08297 16.8403 4.33297Z" fill="#0096DE" />
                                                                                                                    </svg>
                                                                                                                    </span>}
                                                                                                                    {label === "Call Phone Number" && <span className='phone'><svg xmlns="http://www.w3.org/2000/svg" width="18" height="18" viewBox="0 0 18 18" fill="none">
                                                                                                                        <path d="M3.94222 1.51336C4.32432 1.51336 4.68842 1.69106 4.93932 1.99846L6.16772 3.51046C6.53772 3.96556 6.59042 4.60056 6.29562 5.09616L5.29662 6.79246C4.67362 7.83006 5.20752 9.11376 7.03942 10.9658C8.83422 12.7409 9.76552 12.9463 10.3099 12.9463C10.6154 12.9463 10.9126 12.8704 11.2031 12.7068L12.8929 11.7007C13.1043 11.5772 13.3463 11.512 13.5927 11.512C13.9174 11.512 14.2356 11.6255 14.4903 11.8328L15.9995 13.0594C16.5041 13.471 16.6326 14.1685 16.2831 14.7056L16.1591 14.9043C15.5306 15.8804 14.455 16.4866 13.3522 16.4866C13.1761 16.4866 12.9989 16.471 12.8296 16.4407C10.6775 16.0498 8.33012 14.6013 5.85772 12.1416C3.39092 9.66206 1.94512 7.31786 1.56072 5.17476C1.33572 3.91746 1.95282 2.57756 3.12472 1.82226L3.30322 1.70096C3.49452 1.57836 3.71522 1.51336 3.94222 1.51336ZM3.94222 0.0134583C3.44232 0.0134583 2.94002 0.151858 2.49332 0.438458L2.28412 0.580058C0.691623 1.60596 -0.259777 3.51556 0.0843235 5.43896C0.529723 7.92236 2.11552 10.5069 4.79452 13.1995C7.49382 15.8853 10.0716 17.4641 12.5617 17.9165C12.8263 17.9639 13.0905 17.9866 13.3523 17.9866C14.9935 17.9866 16.5358 17.0901 17.4203 15.7166L17.5553 15.5005C18.3112 14.3399 18.048 12.7944 16.9479 11.897L15.4364 10.6687C14.9016 10.2332 14.248 10.012 13.5929 10.012C13.0906 10.012 12.5875 10.1421 12.1365 10.4056L10.4359 11.4181C10.4023 11.4369 10.3602 11.4464 10.31 11.4464C9.94612 11.4464 9.15562 10.9491 8.09432 9.89946C6.89982 8.69166 6.42072 7.83456 6.58272 7.56476L7.58822 5.85746C8.20222 4.82496 8.09432 3.50226 7.33172 2.56426L6.10362 1.05276C5.54942 0.373558 4.74872 0.0134583 3.94222 0.0134583Z" fill="#0096DE" />
                                                                                                                    </svg>
                                                                                                                    </span>}
                                                                                                                    {label === "Copy Offer Code" && <span className='copy'><svg xmlns="http://www.w3.org/2000/svg" width="18" height="18" viewBox="0 0 18 18" fill="none">
                                                                                                                        <path d="M16.669 1.47352C15.5308 0.393423 13.959 0.210824 12.4312 0.174724C11.1871 0.142524 10.3062 0.142524 9.06597 0.174724C7.53717 0.210824 5.96537 0.392524 4.82717 1.47352C4.35597 1.92032 4.06827 2.46182 3.88657 3.03062C2.84447 3.22792 2.01987 3.60702 1.39107 4.20392C0.140572 5.39042 0.104972 7.21762 0.0790718 8.55252L0.0761719 11.9646L0.0790718 12.1121C0.104972 13.4461 0.140572 15.2732 1.39107 16.4695C2.57857 17.5965 4.22357 17.7869 5.82267 17.825C6.47647 17.8416 7.03457 17.8494 7.59217 17.8494C8.14977 17.8494 8.70837 17.8416 9.36027 17.825C10.9609 17.7869 12.6059 17.5965 13.7934 16.4695C14.4994 15.8001 14.8041 14.9314 14.9481 14.1516C15.5778 13.9602 16.1668 13.665 16.6689 13.1882C17.8652 12.0544 17.9008 10.3298 17.9243 9.17362V5.47342C17.9009 4.33282 17.8653 2.60832 16.669 1.47352ZM16.4119 5.06292C15.6554 5.05442 15.1092 5.02242 14.6787 4.89832C13.4585 4.56142 12.5376 3.43832 12.439 2.16202C12.4256 2.00042 12.4333 1.83852 12.4357 1.67692C13.8345 1.71232 14.9171 1.87812 15.6367 2.56142C16.2375 3.13172 16.3718 4.05822 16.4119 5.06292ZM12.7613 15.3817C11.9874 16.1161 10.8282 16.2899 9.32377 16.3251C8.04447 16.3583 7.14257 16.3583 5.85937 16.3251C4.35647 16.2899 3.19727 16.1151 2.42577 15.3837C1.62787 14.621 1.60207 13.2724 1.57907 12.0839L1.57617 8.73142L1.57907 8.58202C1.60197 7.39352 1.62787 6.04682 2.42327 5.29202C2.73227 4.99902 3.12217 4.78862 3.60487 4.63842C3.58137 4.98782 3.57427 5.32542 3.56827 5.63472L3.56537 8.87982L3.56827 9.02042C3.59267 10.2948 3.62637 12.0409 4.82657 13.1884C5.96477 14.2685 7.53657 14.4511 9.06437 14.4872C9.68647 14.5028 10.2172 14.5106 10.748 14.5106C11.2783 14.5106 11.8095 14.5028 12.4296 14.4872C12.729 14.4801 13.0296 14.4643 13.3288 14.4403C13.1957 14.8341 13.012 15.1439 12.7613 15.3817ZM15.6368 12.1005C14.9093 12.7909 13.8145 12.954 12.3931 12.9872C11.1763 13.0175 10.3199 13.0175 9.10107 12.9872C7.68117 12.954 6.58647 12.7909 5.86137 12.1024C5.11427 11.3876 5.08987 10.1151 5.06787 8.99202L5.06547 5.80652L5.06787 5.66292C5.08987 4.54082 5.11427 3.26842 5.85987 2.56132C6.58647 1.87092 7.68117 1.70782 9.10257 1.67462C9.71097 1.65902 10.2291 1.65122 10.7481 1.65122C10.8004 1.65122 10.8549 1.65222 10.9073 1.65242C10.9046 1.86382 10.8967 2.07622 10.9141 2.28402C11.063 4.21762 12.4126 5.86022 14.2632 6.37092C14.8599 6.54302 15.5245 6.58232 16.4243 6.59162V9.15802C16.4004 10.3377 16.3219 11.4511 15.6368 12.1005Z" fill="#0096DE" />
                                                                                                                    </svg>
                                                                                                                    </span>}
                                                                                                                    <span className={`${styles.clickabletxt} clickabletxt`}>{label}</span>
                                                                                                                </div>
                                                                                                            </>
                                                                                                        );
                                                                                                    }
                                                                                                )}
                                                                                            </div>
                                                                                        </div>
                                                                                    }
                                                                                </div>
                                                                                <div className={styles.mobilefooter}>
                                                                                    <img src={assets.message_bar} alt="msgbar" />
                                                                                </div>
                                                                            </div>
                                                                        </div>
                                                                    </div>
                                                                </WorktualModal>
                                                            </div>
                                                        )}
                                                    
                                                        {msg?.intent === "whatsapp_template_table" && (
                                                            <div
                                                                dangerouslySetInnerHTML={{ __html: msg?.whatsappTable }}
                                                                onClick={(e) => {
                                                                    const row = e?.target?.closest("tr");

                                                                    if (row && row.dataset.action === "row-click") {
                                                                        const id = row.dataset.id;
                                                                        // setWhatsappTemplateEdit(true);
                                                                        // handleViewWhatsappTemplate(id)
                                                                        console.log("Clicked ID:", id);

                                                                        // fetchFullData(id);
                                                                    }
                                                                }}
                                                            />
                                                        )}

                                                    </>
                                                );
                                            }
                                            return null;
                                        })}

                                    </div>
                                </div>

                            </div>



                        </div >
                    </div >
                </div>
            </div>





        </>
    )
}

export default Drafts