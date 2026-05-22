import styles from "./scss/contactchat.module.scss";
import { useRef, useState, useEffect } from "react";
import ChatLikes from "../component/likedislike";
import { useSearchParams } from "react-router-dom";
import { GetItemFromStorage } from "../../base/custom_hooks/useStorage";
import { LocalStorage } from "../../base/custom_hooks/localStorageKeys";
import { useSelector } from "react-redux";
import aiChatAssistSocketservice from "../../services/aiChatService";
import { useDispatcher } from "../../store/redux-store/dispatch";
import { excelFormats, getFileSize, imageFormat, pdfFormats, stripHtmlTags, wordFormats } from "../../base/utils";
import { contactDataSliceActions } from "../../store/slice/contactSlice";
import Excelicon from "../../assets/images/xlsicon.svg";
import Wordicon from "../../assets/images/documentformate.svg";
import Pdficon from "../../assets/images/pdfFile.svg";
import SetUpCommonEditor from "../shared/commonEditor";
import Modal from "../component/modal/modal";
import Upgradelist from '../../assets/images/upgradelist.svg'
import Whatsapp from '../../assets/images/whatsappicon.svg'
import voicecall from '../../assets/images/callwidget/calliconwidget.svg'


export default function ContactConversation() {
    const boxRef = useRef(null);
    const [height, setHeight] = useState(0);

    const [searchParams, setSearchParams] = useSearchParams()
    const LoginCcaasUserDetails = GetItemFromStorage(LocalStorage.USER_DETAILS);
    const chatEndRef = useRef<HTMLDivElement | null>(null);

    const dispatch = useDispatcher()
    const { aiAgentChatMessage, aiAgentChatTyping, openContactUpdateModel, contactProjectProId } = useSelector((state: any) => state.contactStore);
    const chatsessionId = searchParams.get("sessionId");
    const companyId = searchParams.get("company_id");
    const companyName = searchParams.get("companyId");
    const pro_id = searchParams.get("pro_id");
    const currentaiAgentChatMessage = aiAgentChatMessage.filter((item: any) => item?.sessionId === chatsessionId);
   
    const contentHtml = useRef<any>(new Map());
    const [, forceUpdatecount] = useState(0);

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

    useEffect(() => {
        if (chatEndRef.current) {
            chatEndRef.current.scrollIntoView({ behavior: 'smooth' });
        }
    }, [currentaiAgentChatMessage?.length, aiAgentChatTyping?.[`${chatsessionId}`]]);

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


    const onChange = (value: any, files :any) => {
        if (!value?.trim()) return;
        let uploadfiles: any = files;
        if (uploadfiles && uploadfiles.length > 0) {
            uploadfiles = uploadfiles?.map((item: any) => {
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

        const body = {
            query: stripHtmlTags(value?.trim()),
            sessionId: chatsessionId,
            roleId: LoginCcaasUserDetails?.roleId?.toString() ?? "",
            org_role_id_crm: LoginCcaasUserDetails?.org_role_id_crm?.toString() ?? "",
            userId: `${LoginCcaasUserDetails?.userId ?? ''}`,
            domainId: `${LoginCcaasUserDetails?.domainId ?? ''}`,
            module: "",
            ext: `${LoginCcaasUserDetails?.ext ?? ''}`,
            agent: 'contact',
            isBot: 0,
            uploadUrl: uploadfiles ?? [],
            chat_page: 'newconvo',
            pro_id: `${pro_id || contactProjectProId || ''}`,
        }

        aiChatAssistSocketservice.io.emit('crm_chat', body);
        dispatch(contactDataSliceActions.setcontactAiAgentChat({ ...body, isBot: 0, query: body?.query }))
        dispatch(contactDataSliceActions.setcontactAiAgentChatTyping({ sessionId: chatsessionId, typing: true }));
    }

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
        <div className={styles.chatmain}>
            <div className={styles.chatmaininner}>

                <div className={`${styles.chatarea} haschatarea`} style={{ height: `calc(100dvh - ${height}px - 116px)` }}>
                    <div className={styles.bubbleblock}>

                        {currentaiAgentChatMessage?.map((item: any, index: number) => {

                            if(item?.assistant_path && !contentHtml.current.has(item?.assistant_path)){
                                fetchHtml(item?.assistant_path)
                            }
                            if (item?.isBot === 0) {
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
                                                            <div className={`${isImage ? "" : styles.chateditorfile}`} key={file?.uuid || i}>
                                                                <a href={fileUrl} target="_blank" rel="noopener noreferrer">
                                                                    <div className={styles.imgcontainer}>
                                                                        <img src={iconSrc} alt="File attachment" className={styles.fileIcon} />
                                                                    </div>
                                                                </a>

                                                                {!isImage ? (
                                                                    <div className={styles.right}>
                                                                        <p className="truncate1">{fileName}</p>
                                                                        <p className={styles.filesize}>{fileSize}</p>
                                                                    </div>
                                                                ) : null}
                                                            </div>
                                                        );
                                                    })}

                                                    <div>{item?.query}</div>
                                                </div>
                                            </div>
                                        ) : (
                                            <div className={styles.bubbleblockagenttextblock}>
                                                <div className={styles.bubbleblockagenttext}>{item?.query}</div>
                                            </div>
                                        )}
                                    </div>
                                )
                            } else {
                                return (
                                    <div className={styles.bubbleblockcustomer} key={`b-${index}`}>
                                        <div className={styles.bubbleblockcustomertextblock}>
                                            <div className={styles.bubbleblockcustomertext}>

                                                {!!contentHtml.current.has(item?.assistant_path) && ( 
                                                    <iframe
                                                        srcDoc={contentHtml.current.get(item?.assistant_path)} 
                                                        // width="600px" 
                                                    />
                                                )}

                                                {!!item?.response && (
                                                    <div dangerouslySetInnerHTML={{ __html: item?.response }}/>
                                                )}

                                            </div>
                                        </div>
                                    </div>
                                )
                            }
                        })}

                        {!!aiAgentChatTyping?.[`${chatsessionId}`] && (
                            <div className="msgbubble left">
                                <div className="chatContent">
                                    {aiAgentChatTyping?.[`${chatsessionId}`] ?? "Typing..."}
                                </div>
                            </div>
                        )}
                     
                    </div>
                      <div ref={chatEndRef}  />
                </div>

                <div className={styles.editorbottom} ref={boxRef}>
                    <SetUpCommonEditor onChange={onChange} />
                </div>
            </div>

            {(openContactUpdateModel?.sessionId === chatsessionId && openContactUpdateModel?.open) 
                && <Modal
                    header={<div className={"modalheader"}><div>Upgrade your plan</div></div>}
                    onClose={() => dispatch(contactDataSliceActions.setContactUpdateModel(false))}
                    children={
                        <div className={styles.upgradelistbg}>
                            <img src={Upgradelist} alt="Upgradeicon" />
                            <div className={styles.contactsaletitle}>
                                You’ve reached your contact limit (1000/1000). Upgrade to continue adding and managing more customers
                            </div>
                            <div className={styles.contentblocker}>
                                <div className={styles.contentblock}>
                                    <img src={Whatsapp} alt="Whatsapp" />
                                    <div className={styles.available}>WhatsApp</div>
                                </div>
                                <div className={styles.contentile}>Or</div>
                                <div className={styles.contentblock}>
                                    <img src={voicecall} alt="voicecall" />
                                    <div className={styles.available}>Call: +91 8428224590</div>
                                </div>
                            </div>
                        </div>
                    }
                />
            }
        </div>
    );
}


{/* <ChatLikes /> */ }

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
