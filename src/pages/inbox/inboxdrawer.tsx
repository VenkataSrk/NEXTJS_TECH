import React, { useEffect, useLayoutEffect, useRef, useState } from "react";
import styles from "../project/scss/projectdrawer.module.scss"
import bluetick from "../../assets/images/project/blutick.svg"
import greytick from "../../assets/images/project/greytick.svg"
import avator1 from "../../assets/images/project/Avatar.png"
import list from "../../assets/images/product/list.svg"
import grid from "../../assets/images/product/grid.svg"
import threedot from "../../assets/images/inbox/threedot.svg";
import pdf from "../../assets/images/project/pdf.svg";
import editpencil from "../../assets/images/project/editpencil.svg"
import bellicon from "../../assets/images/project/notificationbell.svg"
import like from "../../assets/images/project/like.svg"
import eyeicon from "../../assets/images/project/eyeicon.svg"
import reply from "../../assets/images/project/reply.svg"
import Commoneditorinbox from "./commonEditorinbox";
import ChatLikes from "../component/likedislike";
import { stripHtmlTags } from "../../base/utils";
import { v4 as uuidv4 } from "uuid";
import { GetItemFromStorage } from "../../base/custom_hooks/useStorage";
import { LocalStorage } from "../../base/custom_hooks/localStorageKeys";
import inboxAiChatAssistSocketservice from "../../services/inboxAiChatService";
import { useDispatcher } from "../../store/redux-store/dispatch";
import { inboxDataSliceActions } from "../../store/slice/inbox/inboxSlice";
import { useSelector } from "react-redux";
import { AttachmentFormatPoster } from "../shared/inboxattachment";
import { useNavigate, useSearchParams } from "react-router-dom";
import { getAIChathistroyData, insertUpdateAIChathistroyData } from "../../base/service/core_serviceApi";


function Inboxdrawer() {
    const [view, setView] = useState("list");
    const dispatch = useDispatcher();
    const navigate = useNavigate();
    const LoginCcaasUserDetails: any = GetItemFromStorage(LocalStorage.USER_DETAILS);
    const { inboxAiChatMessage, inboxAiChatTyping, inboxChatSessionId, inboxFilter, composeMailToggle } = useSelector((state: any) => state.inboxChatStore);
    const sessionMessages = (inboxAiChatMessage || []).filter((msg: any) => msg?.sessionId === inboxChatSessionId);
    const chatEndRef = useRef<HTMLDivElement | null>(null);
    const [boxNode, setBoxNode] = useState<HTMLDivElement | null>(null);
    const [height, setHeight] = useState(56);
    const [params] = useSearchParams();
    const CustomerId = params.get('CustomerId');

    const handleSend = async (value: any, files: any) => {
        if (!value?.trim()) return;

        let newSessonId = inboxChatSessionId;
        if (!newSessonId) {
            newSessonId = uuidv4();
            dispatch(inboxDataSliceActions.setInboxChatSessionId(newSessonId));
        }

        let body: any = {
            query: stripHtmlTags(value?.trim()),
            sessionId: newSessonId,
            roleId: `${LoginCcaasUserDetails?.role_id ?? ""}`,
            org_role_id_crm: `${LoginCcaasUserDetails?.org_role_id_crm ?? ""}`,
            userId: `${LoginCcaasUserDetails?.userId ?? ""}`,
            domainId: `${LoginCcaasUserDetails?.domainId ?? ""}`,
            module: "",
            ext: `${LoginCcaasUserDetails?.ext ?? ""}`,
            agent: "inbox",
            isBot: 0,
            files: files ?? [],
            tab: inboxFilter.tab || 'Inbox',
            customerId: CustomerId?.trim() || null,
        };
        console.log("handlesend", body);
        inboxAiChatAssistSocketservice.io.emit('crm_inbox_chat', body);
        dispatch(inboxDataSliceActions.setInboxAichatMessage({ ...body, isBot: 0, message: body?.query }))
        dispatch(inboxDataSliceActions.setInboxAiChatTyping({ sessionId: newSessonId, typing: true }));

        const payload = {
            sessionId: newSessonId ?? null,
            title: sessionMessages?.length === 0 ? "New conversation" : null,
            agentId: LoginCcaasUserDetails?.userId ?? null,
            historyMessage: {
                sessionId: newSessonId,
                message: value,
                uploadUrl: files ?? '',
                isBot: 0,
                messageId: Date.now(),
            },
        }
        await insertUpdateAIChathistroyData(payload);
        if (sessionMessages?.length === 0) {
            const body = {
                sessionId: null,
                agentId: LoginCcaasUserDetails?.userId ?? null,
            };
            getAIChathistroyData(body)
                .then((res: any) => {
                    dispatch(inboxDataSliceActions.setAiChatHistoryData(res));
                })
        }
    }

    useEffect(() => {
        if (!boxNode) return;

        setHeight(Math.round(boxNode.getBoundingClientRect().height));

        const observer = new ResizeObserver((entries) => {
            for (let entry of entries) {
                setHeight(Math.round(entry.contentRect.height));
            }
        });

        observer.observe(boxNode);

        return () => observer.disconnect();
    }, [boxNode]);

    useLayoutEffect(() => {
        if (!chatEndRef.current) return;
        requestAnimationFrame(() => {
            chatEndRef.current?.scrollIntoView({ behavior: 'smooth', block: 'end' });
        });
    }, [sessionMessages?.length, inboxAiChatTyping?.[`${inboxChatSessionId}`]]);


    const insertComposemail = (data :any) => {
        try {
            if(!composeMailToggle) {
                dispatch(inboxDataSliceActions.setComposeMailToggle(true));
                navigate('/inbox?channelType=Email')
            } 
            dispatch(inboxDataSliceActions.setPendingInsertReply({
                subject: data?.draft_email?.subject || '',
                body: data?.draft_email?.body || '',
            }))
        } catch (error) {
            console.log(error);
        }
    }

    return (<>
        <div className={`${styles.drawer} ${styles.drawersmall} ${styles.aichatdrawer}`}>
            <div className={`${styles.body} ${styles.bodyeditor}`}>
                <div 
                    style={{  height: sessionMessages?.length > 0 ? `calc(100% - ${height}px)` : "100%" }} 
                    className={`${styles.chatlist} ${styles.chatlistmaincontainer}`}
                >
                    <div className={`${styles.instructcenter} ${sessionMessages?.length === 0 ? styles.instructest : ''}`}>
                        {sessionMessages?.length > 0 ? (
                            <div className={`${styles.chatlist} maincontainer`} style={{ minHeight: "300px" }}>
                                <div className={styles.bubblemaincontainer}>

                                    {sessionMessages.map((data: any, index: number) => {
                                        const isBot = Number(data?.isBot) === 1;
                                        if (isBot) {
                                            return (
                                                <div className={styles.bubbleagentcontainer} key={index}>
                                                    <div className={styles.bubbleagenttext} dangerouslySetInnerHTML={{ __html: data?.message || '' }} />
                                                    <div className="chatlikebtn"><ChatLikes message={data?.message} /></div>

                                                    {data?.draft_email?.body && (
                                                        <div className={styles.draftmails}>
                                                            <div className={styles.draftmailsheader}>
                                                                <div className={styles.label}>Email</div>
                                                                <button
                                                                    className={styles.insertbutton}
                                                                    onClick={() => insertComposemail(data)}
                                                                >Insert</button>
                                                            </div>
                                                            <div className={styles.subject}>
                                                                Subject: {data?.draft_email?.subject || ''}
                                                            </div>
                                                            <hr />
                                                            <div dangerouslySetInnerHTML={{ __html: data?.draft_email?.body || '' }} />
                                                        </div> )}
                                                </div>
                                            );
                                        }
                                        return (
                                            <>
                                                {data?.files?.map((attachment: any, index: number) => (
                                                    <div className={`${styles.bubblecustomercontainer} attachmentindrawer`} key={index}>
                                                        {AttachmentFormatPoster(attachment, index)}
                                                    </div>)
                                                )}
                                                <div className={styles.bubblecustomercontainer} key={index}>
                                                    <div className={styles.bubblecustomertext}>{data?.message}</div>
                                                </div>

                                            </>
                                        );
                                    })}
                                    {!!inboxAiChatTyping?.[`${inboxChatSessionId}`] && (
                                        <div className="msgbubble left">
                                            {inboxAiChatTyping?.[`${inboxChatSessionId}`] || "Typing ..."}
                                        </div>
                                    )}

                                    <div ref={chatEndRef} />
                                </div>
                            </div>
                        ) : (
                            <div className={styles.centeredwitheditor}> <div className={styles.instructiontitle}><span>Instruct or Ask </span>Let Your Assistant Act on Calls, SMS & Email</div>  <Commoneditorinbox onChange={handleSend} /></div>
                        )}
                    </div>
                </div>
                {sessionMessages?.length > 0 &&
                    <div className="editorinbox" ref={setBoxNode}>
                        <Commoneditorinbox onChange={handleSend} />
                    </div>}
            </div>
        </div>
    </>)
} export default Inboxdrawer
