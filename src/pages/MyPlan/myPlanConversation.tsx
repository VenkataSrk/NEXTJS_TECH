import styles from "./assets/styles/myplanchat.module.scss";
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
import { MyplanProfileActions } from "../../store/slice/my_plans/profile";

export default function ContactConversation() {
    const boxRef = useRef(null);
    const [height, setHeight] = useState(0);

    const [searchParams, setSearchParams] = useSearchParams()
    const LoginCcaasUserDetails = GetItemFromStorage(LocalStorage.USER_DETAILS);
    const chatEndRef = useRef<HTMLDivElement | null>(null);

    const dispatch = useDispatcher()
    const { aiAgentChatMessage, aiAgentChatTyping } = useSelector((state: any) => state.myPlanStore);
    const chatsessionId = searchParams.get("sessionId");
    const companyId = searchParams.get("company_id");
    const companyName = searchParams.get("companyId");
    const currentaiAgentChatMessage = aiAgentChatMessage.filter((item: any) => item?.sessionId === chatsessionId);


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
            agent: "myplans",
            isBot: 0,
            uploadUrl: uploadfiles ?? []
        }

        aiChatAssistSocketservice.io.emit('crm_chat', body);
        dispatch(MyplanProfileActions.setSetupAiAgentChat({ ...body, isBot: 0, query: body?.query }))
        dispatch(MyplanProfileActions.setSetupAiAgentChatTyping({ sessionId: chatsessionId, typing: true }));
    }

    return (
        <div className={styles.chatmain}>
            <div className={styles.chatmaininner}>

                <div className={`${styles.chatarea} haschatarea`} style={{ height: `calc(100dvh - ${height}px - 116px)` }}>
                    <div className={styles.bubbleblock}>

                        {currentaiAgentChatMessage?.map((item: any, index: number) => {
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
                                                <div dangerouslySetInnerHTML={{ __html: item?.response }}/>
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
