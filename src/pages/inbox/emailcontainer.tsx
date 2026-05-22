import React, { Fragment, useEffect, useMemo, useRef, useState } from "react";
import styles from "./scss/inbox.module.scss"
import chevrondown from "../../assets/images/inbox/chevrondown.svg";
import mailicon from "../../assets/images/inbox/mailicon.svg";
import purpleicn from "../../assets/images/inbox/callpurple.svg"
import contactwindow from "../../assets/images/inbox/contactwindow.svg";
import bluechevron from "../../assets/images/inbox/bluechevron.svg";
import reply from "../../assets/images/inbox/reply.svg";
import forward from "../../assets/images/inbox/forward.svg";
import deleteem from "../../assets/images/inbox/delete.svg";
import ComposeMail from "./composemail";
import TextEditor from "./texteditor";
import { generateAIMessage, generateEmailReplay, generateNewEmailReplay, sendEmailhandler } from "../../base/service/nlp_service";
import { v4 as uuidv4 } from "uuid";
import threedot from "../../assets/images/inbox/threedot.svg";
import openemail from "../../assets/images/inbox/mailopen.svg";
import newemail from "../../assets/images/inbox/newemail.svg";
import filter from "../../assets/images/inbox/filter.svg";
import { useSelector } from "react-redux";
import { useNavigate, useSearchParams } from "react-router-dom";
import { deleteTrashEmailApi, getContactDetails, moveEmailToTrashApi } from "../../base/service/core_serviceApi";
import { useDispatcher } from "../../store/redux-store/dispatch";
import { inboxDataSliceActions } from "../../store/slice/inbox/inboxSlice";
import moment from "moment";
import { ACTIONS_ACCESS, levelOfAccess, MENUS } from "../../base/constant";
import EmojiPicker from "../component/emojipicker/EmojiPicker";
import { AttachmentFormatPoster } from "../shared/inboxattachment";
import { GetItemFromStorage } from "../../base/custom_hooks/useStorage";
import { LocalStorage } from "../../base/custom_hooks/localStorageKeys";
import Modal from "../component/modal/modal";
import textedit from "./../../assets/images/inbox/texteditor.svg"
import ai from "./../../assets/images/inbox/aiicon.svg"
import link from "./../../assets/images/inbox/link.svg"
import emoji from "../../assets/images/inbox/emoji.svg"
import AIWriteEditor from "../component/aiwriteeditor/aiwriteeditor";
import Aicon from '../../assets/images/inbox/aiicon.svg'
import Formal from '../../assets/images/rephraseicon/formal.svg'
import Friendly from '../../assets/images/rephraseicon/friendly.svg'
import Concise from '../../assets/images/rephraseicon/concise.svg'
import Elaborate from '../../assets/images/rephraseicon/elaborate.svg'
import Tick from '../../assets/images/inbox/tick.svg'
import Undo from '../../assets/images/inbox/undo.svg'
import Retry from '../../assets/images/inbox/retry.svg'
import { isJSON } from "../../base/utils";


function EmailContainer({ offRightSpace, setOffRightSpace, activeFilter }: any) {

    const navigate = useNavigate();
    const dispatch = useDispatcher();
    const [showDetail, setShowDetail] = useState({})
    const showDetailRef = useRef(null);
    const emailDropDownRef = useRef(null);
    const lastMessageRef = useRef<HTMLDivElement | null>(null);
    const [params] = useSearchParams();
    const CustomerId = params.get('CustomerId');
    const { composeMailToggle, perticularInboxData, perticularInboxContactData, domainEmailConfig } = useSelector((state: any) => state.inboxChatStore);
    const [openIndexes, setOpenIndexes] = useState<number[]>([]);
    const [expandedQuotes, setExpandedQuotes] = useState<Record<number, boolean>>({});
    const initials = perticularInboxContactData?.customer_name
        ?.trim()?.split(" ")
        ?.map((w: any) => w[0])
        ?.join("").toUpperCase() || "";

    const LoginUserDetails = GetItemFromStorage(LocalStorage.USER_DETAILS);
    const [openConfirmDelete, setOpenConfirmDelete] = useState(false);
    const pendingDeleteActionRef = useRef<null | (() => Promise<void> | void)>(null);
    
    const [openMoveTrashDelete, setOpenMoveTrashDelete] = useState(false);
    const pendingTrashDeleteActionRef = useRef<null | (() => Promise<void> | void)>(null);

    const stripQuotedContent = (html: string): string => {
        if (!html) return "";
        let cleaned = html;
        cleaned = cleaned.replace(/<div[^>]*class\s*=\s*["'][^"']*gmail_quote[^"']*["'][^>]*>[\s\S]*$/i, "");
        cleaned = cleaned.replace(/<blockquote[\s\S]*?<\/blockquote>/gi, "");
        cleaned = cleaned.replace(/<[a-z]+[^>]*>\s*On\s+.+?wrote:[\s\S]*$/i, "");
        return cleaned.trim();
    };
    const normalizeMessageList = (message: any) => {
        if (Array.isArray(message)) return message;
        if (message && typeof message === "object") return [message];
        if (isJSON(message)) {
            try {
                const parsed = JSON.parse(message);
                if (Array.isArray(parsed)) return parsed;
                if (parsed && typeof parsed === "object") return [parsed];
            } catch {
                return [];
            }
        }
        return [];
    };

    const openCompose = (mailData?: any) => {
        dispatch(inboxDataSliceActions.setCleanReplyForword());
        dispatch(inboxDataSliceActions.setComposeMailToggle(true))
        dispatch(inboxDataSliceActions.setForwardedMessage(true))
        dispatch(inboxDataSliceActions.setForwardMailDetails({
            from: perticularInboxContactData?.customer_name?.trim() ? perticularInboxContactData?.customer_name : "New Customer",
            fromEmail: perticularInboxContactData?.FromEmail || "",
            toMail: mailData?.IsInBound === 1 ? mailData?.FromEmail || "" : mailData?.ToEmail || "",
            date: mailData?.createdAt,
            subject: mailData?.EmailSubject || "No Subject",
            message: (normalizeMessageList(mailData?.Message)?.[0]?.userText?.message?.message || "").replace( /\n/g, "<br />"),
        }))
        navigate(`/inbox?channelType=Email&CustomerId=${CustomerId || 0}&forward=true`);
    }

    const initialReplyState = {
        active: false,
        parent: null as any,
        target: null as any,
        toTags: [] as string[],
        ccTags: [] as string[],
        bccTags: [] as string[],
        subject: "",
        mainbody: "",
        quotedHtml: "",
        attachments: [] as any[],
        isInitialLoading: false,
        aiwriteLoading: false,
        isRegenerating: false,
        aiSuggestions: [] as any[],
        currentSuggestionIndex: 0,
        aiwriteInitialText: "",
        openTools: false,
        showEmojiPicker: false,
        sending: false,
    };
    const [replyData, setReplyData] = useState<any>(initialReplyState);
    const replyEditorRef = useRef<any>(null);
    const [replyEditorSeed, setReplyEditorSeed] = useState<{ fresh: boolean; content: string }>({ fresh: true, content: "" });
    const [replyEditorTick, setReplyEditorTick] = useState(0);

    const pushReplyEditorContent = (content: string, fresh = false) => {
        setReplyEditorSeed({ fresh, content });
        setReplyEditorTick((t) => t + 1);
    };

    const stripHtmlForEditor = (html: string): string => {
        if (!html) return "";
        const tmp = document.createElement("div");
        tmp.innerHTML = html;
        return (tmp.textContent || tmp.innerText || "").trim();
    };

    const buildQuotedHtml = (mailDataParent: any, msg: any) => {
        const senderEmail = mailDataParent?.IsInBound === 1
            ? mailDataParent?.FromEmail || msg?.fromData || ""
            : mailDataParent?.ToEmail || msg?.toData || "";
        return `
            <br/>
            <div class="gmail_quote" style="font-family:inherit;color:#5F6368;">
              <div class="gmail_attr" style="margin-bottom:6px;">On ${msg?.sendAt
                ? moment(Number(msg.sendAt)).format("ddd, MMM DD, YYYY [at] h:mm A")
                : ""}, ${perticularInboxContactData?.customer_name?.trim() || "New Customer"} &lt;${senderEmail}&gt; wrote:</div>
              <blockquote style="margin:0 0 0 0.8ex;border-left:2px solid #C9C9CF;padding-left:1ex;color:#5F6368;">
                ${msg?.message || ""}
              </blockquote>
            </div>
        `;
    };

    const openInlineReply = (mailDataParent: any, msgItem: any) => {
        console.log("mailDataParent>>>>",mailDataParent)
        try {
            const msg = msgItem?.userText?.message;
            const replyTo = mailDataParent?.IsInBound === 1
                ? mailDataParent?.FromEmail || msg?.fromData || ""
                : mailDataParent?.ToEmail || msg?.toData || "";
            const ourMailbox = mailDataParent?.IsInBound === 0
                ? mailDataParent?.FromEmail || ""
                : mailDataParent?.ToEmail || "";

            const originalSubject = msg?.subject || mailDataParent?.EmailSubject || "";
            const subject = originalSubject.startsWith("Re:")
                ? originalSubject
                : originalSubject ? `Re: ${originalSubject}` : "";

            const quotedHtml = buildQuotedHtml(mailDataParent, msg);

            setReplyData({
                ...initialReplyState,
                active: true,
                parent: mailDataParent,
                target: msgItem,
                toTags: replyTo ? String(replyTo).split(',').map((m: string) => m.trim()).filter(Boolean) : [],
                subject,
                quotedHtml,
                isInitialLoading: true,
            });
            pushReplyEditorContent("", true);

            generateEmailReplay({
                email: ourMailbox,
                domainId: `${LoginUserDetails?.domainId}`,
                subject: msg?.subject || "",
            }).then((res: any) => {
                if (res?.status === "success" && res?.response) {
                    setReplyData((prev: any) => ({
                        ...prev,
                        mainbody: res?.response,
                        isInitialLoading: false,
                    }));
                    pushReplyEditorContent(res?.response, false);
                } else {
                    setReplyData((prev: any) => ({ ...prev, isInitialLoading: false }));
                }
            }).catch(() => {
                setReplyData((prev: any) => ({ ...prev, isInitialLoading: false }));
            });
        } catch (e) {
            console.error("openInlineReply error", e);
        }
    };

    const discardReply = () => {
        setReplyData(initialReplyState);
        pushReplyEditorContent("", true);
    };

    const sendReply = async () => {
        if (replyData?.sending) return;
        if (!replyData?.toTags?.length) {
            alert("Please add To Address");
            return;
        }
        if (!domainEmailConfig?.emailAddress) {
            return;
        }

        const body = {
            sessionId: uuidv4(),
            domainId: LoginUserDetails?.domainId,
            from_email: domainEmailConfig?.emailAddress ?? "",
            to_email: replyData.toTags || [],
            cc: replyData.ccTags || [],
            bcc: replyData.bccTags || [],
            subject: replyData.subject ?? "",
            content: replyData.mainbody ?? "",
            emailhistoryId: replyData?.parent?.EmailHistoryId != null ? String(replyData.parent.EmailHistoryId) : "",
            attachments: replyData.attachments?.length > 0 ? replyData.attachments : [],
            userId: LoginUserDetails?.userId ?? null,
            replay_content: replyData.quotedHtml ?? "",
        };

        setReplyData((prev: any) => ({ ...prev, sending: true }));
        try {
            const response: any = await sendEmailhandler(body);
            if (response?.response?.status === "success" || response?.status === "processing") {
                discardReply();
            } else {
                setReplyData((prev: any) => ({ ...prev, sending: false }));
            }
        } catch {
            setReplyData((prev: any) => ({ ...prev, sending: false }));
        }
    };

    const applyReplyInsert = (draft: { subject?: string; body?: string } | null | undefined) => {
        if (!draft?.body) return;
        const html = `${draft.body}<br/>`;
        setReplyData((prev: any) => ({
            ...prev,
            // subject:draft?.subject || prev?.subject,
            mainbody: html + (prev?.mainbody || ""),
        }));
        pushReplyEditorContent(html, false);
    };

    const handleReplyAIWriteSparkle = async (prompt: string) => {
        if (!prompt?.trim() || replyData?.aiwriteLoading) return;
        setReplyData((prev: any) => ({ ...prev, aiwriteLoading: true }));
        try {
            const res: any = await generateNewEmailReplay({
                content: prompt,
                domainId: `${LoginUserDetails?.domainId}`,
            });
            if (res?.status === "success" && res?.response && isJSON(res?.response)) {
                const data = JSON.parse(res?.response);
                setReplyData((prev: any) => ({
                    ...prev,
                    aiSuggestions: [data],
                    currentSuggestionIndex: 0,
                }));
                if (!data?.body) return;
                applyReplyInsert(data);
            }
        } catch (err) {
            console.error(err);
        } finally {
            setReplyData((prev: any) => ({ ...prev, aiwriteLoading: false }));
        }
    };

    const handleReplyRegenerate = async () => {
        const currentSuggestion = replyData?.aiSuggestions?.[replyData?.currentSuggestionIndex];
        if (!currentSuggestion || replyData?.isRegenerating) return;

        setReplyData((prev: any) => ({ ...prev, isRegenerating: true }));
        try {
            const response: any = await generateAIMessage({
                text: JSON.stringify(currentSuggestion),
                tone: "Elaborate",
            });
            if (isJSON(response)) {
                const data = JSON.parse(response);
                setReplyData((prev: any) => {
                    const next = Array.isArray(prev?.aiSuggestions) ? [...prev.aiSuggestions] : [];
                    if (prev?.currentSuggestionIndex >= 0 && prev?.currentSuggestionIndex < next.length) {
                        next[prev.currentSuggestionIndex] = data;
                    }
                    return { ...prev, aiSuggestions: next };
                });
            }
        } catch (err) {
            console.error(err);
        } finally {
            setReplyData((prev: any) => ({ ...prev, isRegenerating: false }));
        }
    };

    useEffect(() => {
        const currentSuggestion = replyData?.aiSuggestions?.[replyData?.currentSuggestionIndex];
        if (currentSuggestion?.body) {
            setReplyData((prev: any) => ({
                ...prev,
                aiwriteInitialText: stripHtmlForEditor(currentSuggestion.body),
            }));
        }
    }, [replyData?.aiSuggestions, replyData?.currentSuggestionIndex]);

    const handleReplyEditorEmoji = (emojiData: any) => {
        if (replyEditorRef.current) {
            replyEditorRef.current.insertEmoji(emojiData.emoji || emojiData);
        }
    };

    const removeReplyTag = (index: number) => {
        setReplyData((prev: any) => ({
            ...prev,
            toTags: prev.toTags?.filter((_: any, i: number) => i !== index),
        }));
    };

    const openReplayAll = (mailData :any) => {
        dispatch(inboxDataSliceActions.clearAiSuggestions())
        dispatch(inboxDataSliceActions.setComposeMailToggle(true))

        dispatch(inboxDataSliceActions.setCleanReplyForword())
        dispatch(inboxDataSliceActions.setReplyAllMessage(true))

        const records = normalizeMessageList(mailData?.Message)?.reduce((acc :any, item :any) => {
            const mail = item?.userText?.message
            if(mail){
                acc.push({
                    from: "",
                    fromEmail: mail?.fromData || "",
                    date: mail?.sendAt,
                    subject: mail?.subject || '',
                    toMail: mail?.toData || "",
                    message: mail?.message || "",
                    cc: mail?.cc || "",
                    bcc: mail?.bcc || "",

                    parent_EmailSubject: mailData?.EmailSubject?.replace("Re: ", ""),
                    parent_FromEmail: mailData?.FromEmail,
                    parent_ToEmail: mailData?.ToEmail
                })
            }
            return acc;
        },[])

        dispatch(inboxDataSliceActions.setReplyAllMailDetails(records))
        navigate(`/inbox?channelType=Email&CustomerId=${CustomerId || 0}&replayAll=true`);
    }

    const [ showDropDown,setShowDropDown] = useState(false)

    const moveEmailToTrash = async (mailData?: any) => {
        const body = {
            email_history_ids: mailData?.EmailHistoryId ?? null,
            isTrash: 1,
        }
        const response = await moveEmailToTrashApi(body)
        if (response?.statusCode === 200 && response?.response?.[0]?.errcode) {
            dispatch(inboxDataSliceActions.setMoveToTrash(body))
        }
    }

    useEffect(() => {
        const handleClick = (event :any) => {
            if(showDetailRef.current && !showDetailRef.current?.contains(event.target as Node)){
                setShowDetail({})
            }
            if(emailDropDownRef.current && !emailDropDownRef.current?.contains(event.target as Node)){
                setShowDropDown(false)
            }
        }
        window.addEventListener('click', handleClick);
        return () => {
            window.removeEventListener('click', handleClick);
        }
    }, [])

    useEffect(() => {
        const data = perticularInboxData?.[0];
        if (!data) return;
    
        const messageList = normalizeMessageList(data?.Message);

        const lastIndex = messageList.length - 1;
        setOpenIndexes(lastIndex >= 0 ? [lastIndex] : []);
        setExpandedQuotes({});
        requestAnimationFrame(() => {
            lastMessageRef.current?.scrollIntoView({ behavior: "smooth", block: "start" });
        });
    }, [perticularInboxData]);
    
    const { permissions } = useSelector((store :any)=> store?.loginUser?.rolesandpermissions)
    const hasMailDeleteAccess = levelOfAccess(MENUS.INBOX, ACTIONS_ACCESS.DELETE, permissions)

    const messageList =   useMemo(() => {
        return  perticularInboxData?.[0]?.Message ? normalizeMessageList(perticularInboxData?.[0]?.Message) : []
    }, [perticularInboxData?.[0]]);

    const composeNewmail =  () => {
        dispatch(inboxDataSliceActions.clearAiSuggestions())
        dispatch(inboxDataSliceActions.setCleanReplyForword())
        dispatch(inboxDataSliceActions.setComposeMailToggle(true))
        dispatch(inboxDataSliceActions.setReplyMessage(true))
        dispatch(inboxDataSliceActions.setReplyMailDetails({
            fromEmail: perticularInboxContactData?.FromEmail || "",
            toMail: perticularInboxContactData?.ToEmail || "",
            newCompose: true
        }))
        navigate(`/inbox?channelType=Email&CustomerId=${CustomerId || 0}`);
    }

    const moveToTrash = () => {
        moveEmailToTrash(perticularInboxData?.[0])
    }

    const openDeleteConfirmation = (action: () => Promise<void> | void) => {
        pendingDeleteActionRef.current = action;
        setOpenConfirmDelete(true);
    };

    const confirmDeleteAction = async () => {
        setOpenConfirmDelete(false);
        if (pendingDeleteActionRef.current) {
            await pendingDeleteActionRef.current();
        }
        pendingDeleteActionRef.current = null;
    };

    const openMoveTrashConfirmation = (action: () => Promise<void> | void) => {
        pendingTrashDeleteActionRef.current = action;
        setOpenMoveTrashDelete(true);
    };

    const confirmTrashDeleteAction = async () => {
        setOpenConfirmDelete(false);
        if (pendingTrashDeleteActionRef.current) {
            await pendingTrashDeleteActionRef.current();
        }
        pendingTrashDeleteActionRef.current = null;
    };

    const permanentlyDeleteTrashItem = async () => {
        const historyId = perticularInboxData?.[0]?.EmailHistoryId ?? perticularInboxData?.[0]?.SMSHistoryId ?? null
        if (!historyId) return;

        const response = await deleteTrashEmailApi({
            domainId: LoginUserDetails?.domainId ?? LoginUserDetails?.domain_id ?? null,
            historyIds: String(historyId),
        });

        if (response?.statusCode === 200) {
            dispatch(inboxDataSliceActions.setMoveToTrash({
                email_history_ids: historyId,
                call_history_ids: historyId,
                sms_history_ids: historyId
            }));
            navigate('/inbox')
        }
    };

    const validateArray = (item :any) => {
        try {
            if(!item) return ''
            if(typeof item === 'string') return item || ''
            if(Array.isArray(item)){
                const f_item = item.filter((i)=> typeof i === 'string')
                return f_item?.join(', ') || ''
            }
        } catch (error) {
            return ''
        }
    }

    return (
        <>
            {composeMailToggle && <ComposeMail />}

            {!composeMailToggle && (
                <div className={`${styles.mail} `} style={{overflow:"auto"}}>

                    <div className={` ${styles.header} ${styles.headerhavelefts}`}>
                        <div className={styles.lefts}>
                            <span style={{ display: "grid" }}><img src={mailicon} /></span>
                            <span className={`content ${styles.subcontent}`}>Email - {perticularInboxContactData?.customer_name?.trim() || "New Customer"}</span>
                        </div>


                        <div className={styles.contactwindow}>
                            <span 
                                className={styles.replayall}
                                onClick={() => openReplayAll(perticularInboxData?.[0])}
                                    >
                                <svg width="24" height="24" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg">
                                    <path d="M9.75335 14.7524C9.75265 14.7542 9.75225 14.7567 9.75155 14.7585C9.69875 14.8952 9.60085 14.992 9.46465 15.0447C9.38745 15.0743 9.30415 15.0893 9.21735 15.0893C9.09825 15.0893 8.97705 15.0603 8.85365 14.9987C8.28005 14.7197 7.42215 13.9002 6.23775 12.7687L5.52615 12.0847C4.89395 11.4772 4.43715 11.0382 4.40205 10.7837C4.39515 10.7362 4.39535 10.6764 4.40345 10.6194C4.43645 10.3805 4.88805 9.95141 5.51905 9.35161L6.18475 8.71251C7.44425 7.50231 8.25105 6.74501 8.83755 6.43091C8.96925 6.35981 9.10085 6.32371 9.22855 6.32371C9.35445 6.32371 9.47835 6.35981 9.59345 6.42901C9.62615 6.44881 9.71315 6.50151 9.79475 6.75351C9.87355 7.00301 9.91065 7.18941 9.93825 7.74151L11.0673 6.80861C11.2845 6.69171 11.2169 6.72121 11.0971 6.34201C10.9349 5.84061 10.6753 5.48741 10.3008 5.26061C9.96565 5.05901 9.59925 4.95791 9.22845 4.95801C8.87825 4.95801 8.52415 5.04821 8.18905 5.22901C7.43515 5.63281 6.57645 6.44221 5.23855 7.72771L4.57325 8.36641C3.65495 9.23911 3.14905 9.71971 3.05065 10.4325C3.02485 10.6113 3.02485 10.8019 3.05025 10.9781C3.14915 11.6954 3.65865 12.1842 4.58365 13.0731L5.29155 13.7533C6.61635 15.0189 7.49125 15.8546 8.25645 16.2268C8.56005 16.3787 8.88995 16.455 9.21735 16.455C9.46845 16.455 9.71815 16.4101 9.95345 16.3199C10.4531 16.1266 10.8338 15.7462 11.0252 15.2512C11.0838 15.1007 11.128 14.9314 11.1663 14.7525L9.75335 14.7524Z" fill="#23232B"/>
                                    <path d="M11.5517 6.96582C11.6204 6.96582 11.6871 6.98582 11.7514 7.02402C11.7516 7.02432 11.7859 7.06222 11.8235 7.17842C11.8941 7.40172 11.9373 7.76462 11.9596 8.32072C11.9911 9.10642 12.624 9.73432 13.4099 9.75972C18.7333 9.93162 19.467 11.8191 19.467 13.8332C19.467 14.6851 19.0899 15.5733 18.5615 16.2848C18.7076 15.2923 18.6411 14.2703 18.0747 13.431C17.115 12.0091 15.1448 11.7223 13.5356 11.6312C13.5073 11.6296 13.4791 11.6288 13.4508 11.6288C13.0789 11.6288 12.7189 11.7672 12.4421 12.0188C12.1442 12.2894 11.9677 12.6684 11.952 13.0705C11.937 13.458 11.9037 14.0069 11.7894 14.301C11.7671 14.3586 11.7317 14.3932 11.6751 14.415C11.6341 14.4307 11.589 14.4387 11.5414 14.4387C11.4723 14.4387 11.4016 14.4216 11.322 14.3819C10.8353 14.1452 10.026 13.372 9.00455 12.3962L8.35615 11.773C7.94285 11.3757 7.42825 10.8813 7.40825 10.7362C7.40545 10.7169 7.40555 10.6868 7.40975 10.658C7.42755 10.529 7.95725 10.0258 8.34985 9.65282L8.95625 9.07052C10.088 7.98322 10.8114 7.30362 11.317 7.03282C11.3993 6.98842 11.4785 6.96582 11.5517 6.96582ZM11.5515 5.46582C11.2325 5.46582 10.91 5.54802 10.6047 5.71262C9.91785 6.08052 9.13575 6.81782 7.91705 7.98882L7.31095 8.57062C6.47445 9.36552 6.01355 9.80332 5.92385 10.4526C5.90045 10.6155 5.90045 10.7891 5.92345 10.9496C6.01355 11.603 6.47765 12.0482 7.32035 12.8579L7.96515 13.4776C9.17195 14.6305 9.96895 15.3917 10.666 15.7307C10.9426 15.869 11.2431 15.9386 11.5414 15.9386C11.7702 15.9386 11.9976 15.8976 12.2119 15.8155C12.667 15.6394 13.0138 15.293 13.1882 14.842C13.3553 14.4124 13.422 13.8685 13.4508 13.1287C15.2982 13.2333 16.3791 13.6003 16.8313 14.2701C17.3078 14.9762 17.2128 16.1924 16.5404 17.988C16.4318 18.2777 16.5038 18.6044 16.724 18.82C16.8717 18.9648 17.0683 19.0422 17.2695 19.0422C17.3682 19.0422 17.4679 19.0233 17.5633 18.9854C19.2071 18.3213 20.9668 16.0929 20.9668 13.8331C20.9668 9.22642 17.0762 8.37722 13.4582 8.26032C13.4294 7.54272 13.3679 7.08842 13.2536 6.72642C13.1059 6.26972 12.8693 5.94792 12.5283 5.74142C12.2229 5.55782 11.8891 5.46582 11.5515 5.46582Z" fill="black"/>
                                </svg>
                                Reply All
                            </span>
                            
                            <div style={{ display: "flex", alignItems: "center", position: "relative" }} ref={emailDropDownRef}>
                                <svg
                                    onClick={() => setShowDropDown(!showDropDown)}
                                    style={{ transform: "rotate(90deg)" }}
                                    width="18" height="5" viewBox="0 0 18 5" fill="none" xmlns="http://www.w3.org/2000/svg">
                                    <path fill-rule="evenodd" clip-rule="evenodd" d="M13.461 2.2435C13.461 3.4774 14.4706 4.487 15.7045 4.487C16.9384 4.487 17.948 3.4774 17.948 2.2435C17.948 1.0096 16.9384 0 15.7045 0C14.4706 0 13.461 1.0096 13.461 2.2435ZM11.2175 2.2435C11.2175 1.0096 10.2079 0 8.974 0C7.7401 0 6.7305 1.0096 6.7305 2.2435C6.7305 3.4774 7.7401 4.487 8.974 4.487C10.2079 4.487 11.2175 3.4774 11.2175 2.2435ZM2.2435 0C3.4774 0 4.487 1.0096 4.487 2.2435C4.487 3.4774 3.4774 4.487 2.2435 4.487C1.0096 4.487 0 3.4774 0 2.2435C0 1.0096 1.0096 0 2.2435 0Z" fill="#757676" />
                                </svg>

                                {showDropDown && <div className={"dropdowncompose  right"} data-inbox-action-menu="true">
                                    <div className="list" onClick={composeNewmail}> Compose new email</div>
                                    {activeFilter === 'Trash'
                                        ?   <div className="list" onClick={()=> {
                                                openDeleteConfirmation(() => permanentlyDeleteTrashItem());
                                        }}> Permanently delete</div>
                                        :   <div className="list" onClick={() => {
                                                openMoveTrashConfirmation(() => moveToTrash());
                                        }}> Move to trash</div>}
                                </div>}
                            </div>
                         

                            {CustomerId && !offRightSpace && <img 
                                src={contactwindow}
                                onClick={() => setOffRightSpace?.((prev: boolean) => !prev)} 
                                style={{ transform: "rotate(180deg)" }} 
                            />}
                        </div>
                    </div>

                    <div className={`${styles.bodyinner} p12`}>
                            {messageList.map((msgItem: any, msgIndex: number) => {
                                        const isOpen = openIndexes.includes(msgIndex);
                                        const msg = msgItem?.userText?.message;
                                        console.log("msgmsgmsgmsgmsgmsgmsg", msg);
                                        const messageSubject = msg?.subject || perticularInboxData?.[0]?.EmailSubject || "No Subject";
                                        const attachment = Array.isArray(msgItem?.userText?.message?.attachments) ? msgItem?.userText?.message?.attachments : []
                                        return (
                                            <div
                                                className={`${styles.fullviewofemail} mb16`}
                                                key={msgIndex}
                                                ref={msgIndex === messageList?.length - 1 ? lastMessageRef : null}
                                            >
                                                <div className={styles.expandsec} onClick={() =>
                                                       setOpenIndexes((prev) =>
                                                        prev.includes(msgIndex) ? [] : [msgIndex]
                                                    )
                                                    }>
                                                    <div className={styles.lefts}>
                                                        <img src={chevrondown} className={styles.chevrondown} style={{ transform: isOpen ? "rotate(180deg)" : "rotate(0deg)" }} />
                                                        {messageSubject}
                                                    </div>
                                                    <div className={styles.rights}>
                                                        {!!msg?.sendAt && moment(Number(msg?.sendAt)).format("MMM DD, YYYY [at] h:mm A")}
                                                    </div>
                                                </div>
                                                {isOpen && <div className={styles.hidendshowbox}>
                                                    <div className={`${styles.avatorwithactions} ptb16`}>
                                                        <div className={styles.lefts}>
                                                            <div className={styles.avatar}>{initials || "N"}</div>
                                                            <div className={styles.namewithact}>
                                                                <h5>{perticularInboxContactData?.customer_name?.trim() || "New Customer"}</h5>
                                                                <div ref={showDetailRef}>
                                                                    <span
                                                                        
                                                                        className={styles.dtls}
                                                                        onClick={() => {
                                                                            if(showDetail[msgIndex]){
                                                                                setShowDetail({})
                                                                            } else {
                                                                                setShowDetail({[msgIndex]: true})
                                                                            }
                                                                        }}
                                                                    >
                                                                        {showDetail[msgIndex] ? "Hide" : "Show"}  Details {" "}
                                                                        <img style={{ transform: showDetail[msgIndex] ? "rotate(180deg)" : "rotate(0deg)" }} src={bluechevron} />
                                                                    </span>

                                                                    {showDetail[msgIndex] && (
                                                                        <div className={"dropdowncompose toemailinfobox"} data-inbox-action-menu="true">
                                                                            
                                                                            <div className="list"> To : {validateArray(msg?.toData)}</div>
                                                                            <div className="list"> From : {validateArray(msg?.fromData)} </div>

                                                                            {!!validateArray(msg?.cc) && <div className="list"> CC : {validateArray(msg?.cc)} </div>}
                                                                            {!!validateArray(msg?.bcc) && <div className="list"> BCC : {validateArray(msg?.bcc)} </div>}
                                                                            
                                                                            <div className="list"> Date : {msg?.sendAt ? moment(Number(msg?.sendAt)).format("MMM DD, YYYY [at] h:mm A") : ""}  </div>
                                                                            <div className="list"> Subject : {msg?.subject} </div>
                                                                           

                                                                        </div>
                                                                    )}
                                                                </div>
                                                                
                                                            </div>
                                                        </div>
                                                        <div className={styles.rights}>
                                                            <div className={styles.actions} onClick={() => openInlineReply(perticularInboxData?.[0], msgItem)}>
                                                                <img src={reply} />Reply
                                                            </div>
                                                            <div className={styles.actions} onClick={() => openCompose({ ...perticularInboxData?.[0], Message: [msgItem] })}>
                                                                <img src={forward} />Forward
                                                            </div>
                                                            {hasMailDeleteAccess && (
                                                                <div className={styles.actions} onClick={() => {
                                                                    if(activeFilter === 'Trash'){
                                                                        openDeleteConfirmation(() => permanentlyDeleteTrashItem());
                                                                    } else {
                                                                        // moveEmailToTrash(perticularInboxData?.[0])
                                                                        openMoveTrashConfirmation(() => moveToTrash());
                                                                    }
                                                                    }}>
                                                                    <img src={deleteem} />Delete
                                                                </div>)}
                                                        </div>
                                                    </div>
                                                    {/* {showDetail[msgIndex] && ( */}
                                                        <div className={styles.emailcontent}>

                                                            {/* <h5>Hi team,</h5> */}

                                                            <div style={{ display: "flex", flexWrap: "wrap", gap: "8px" }}>
                                                                {attachment?.map(
                                                                    (attachment: any, index: number) => AttachmentFormatPoster(attachment, index)
                                                                    )}
                                                                </div>
                                                            <div
                                                                dangerouslySetInnerHTML={{
                                                                    __html: (msg?.message || "").replace(
                                                                        /\n/g,
                                                                        "<br />"
                                                                    ),
                                                                }}
                                                            />

                                                            {msgIndex > 0 && (
                                                                <>
                                                                    <span
                                                                        className={styles.replayimg}
                                                                        onClick={() =>
                                                                            setExpandedQuotes((prev) => ({
                                                                                ...prev,
                                                                                [msgIndex]: !prev[msgIndex],
                                                                            }))
                                                                        }
                                                                    >
                                                                        <img src={threedot} />
                                                                    </span>

                                                                    {expandedQuotes[msgIndex] && (
                                                                        <div className={styles.quotedthread}>
                                                                            {messageList
                                                                                .slice(0, msgIndex)
                                                                                .reverse()
                                                                                .map((prevItem: any, prevIdx: number) => {
                                                                                    const prevMsg = prevItem?.userText?.message;
                                                                                    const senderEmail = prevMsg?.fromData || "Unknown";
                                                                                    const sentAt = prevMsg?.sendAt
                                                                                        ? moment(Number(prevMsg?.sendAt)).format("ddd, MMM DD, YYYY [at] h:mm A")
                                                                                        : "";
                                                                                    return (
                                                                                        <div
                                                                                            className={styles.quotedmsg}
                                                                                            key={prevIdx}
                                                                                            style={{ marginLeft: `${prevIdx * 16}px` }}
                                                                                        >
                                                                                            <div className={styles.quotedheader}>
                                                                                                On {sentAt}{sentAt && ", "}&lt;{senderEmail}&gt; wrote:
                                                                                            </div>
                                                                                            <div
                                                                                                className={styles.quotedbody}
                                                                                                dangerouslySetInnerHTML={{
                                                                                                    __html: stripQuotedContent(prevMsg?.message || "").replace(/\n/g, "<br />"),
                                                                                                }}
                                                                                            />
                                                                                        </div>
                                                                                    );
                                                                                })}
                                                                        </div>
                                                                    )}
                                                                </>
                                                            )}

                                                        </div>
                                                    {/* )} */}
                                                </div>}
                                            </div>
                                        );
                                    })}

                        {replyData?.active && (
                            <div className={`${styles.replyeditor} replyeditor`}>
                                <div className={styles.top}>
                                    <div className={`${styles.avatorwithactions} `}>
                                        <div className={styles.lefts}>
                                            <div className={styles.avatar}>{initials || "N"}</div>
                                            <div className={styles.namewithact}>
                                                <h5>{perticularInboxContactData?.customer_name?.trim() || "New Customer"}</h5>
                                                <span>
                                                    {(replyData.toTags || []).map((tag: string, i: number) => (
                                                        <span key={i} style={{ marginRight: 6 }}>
                                                            {tag}
                                                            {/* <span
                                                                style={{ marginLeft: 4, cursor: "pointer" }}
                                                                onClick={() => removeReplyTag(i)}
                                                            >×</span> */}
                                                            {i < replyData.toTags.length - 1 ? ";" : ""}
                                                        </span>
                                                    ))}
                                                </span>
                                            </div>
                                        </div>
                                    </div>
                                </div>

                                <div className={`${styles.body} editorwrapperinreply`} style={{ minHeight: "200px", margin: "0", padding:"12px" , maxHeight:"200px", overflow:"auto" , position:"relative"}}>
                                    <TextEditor
                                        ref={replyEditorRef}
                                        openTools={replyData.openTools}
                                        initialContent={replyEditorSeed}
                                        contentTick={replyEditorTick}
                                        onchange={(e: any) =>
                                            setReplyData((prev: any) => ({
                                                ...prev,
                                                mainbody: e?.target?.innerHTML ?? "",
                                            }))
                                        }
                                    />
                                </div>
                                <div className={styles.footer} style={{ gap: "12px", background: "transparent", position: "unset", padding:"12px 0" }}>
                                    <div className={styles.leftActions} style={{ width: "100%" }}>
                                        <div style={{ position: "relative", display: "flex" }}>
                                            <img
                                                src={emoji}
                                                alt="emoji"
                                                onMouseDown={(e) => e.preventDefault()}
                                                onClick={() =>
                                                    setReplyData((prev: any) => ({
                                                        ...prev,
                                                        showEmojiPicker: !prev.showEmojiPicker,
                                                    }))
                                                }
                                            />

                                            {replyData.showEmojiPicker && (
                                                <div >
                                                    <EmojiPicker onEmojiSelect={handleReplyEditorEmoji} />
                                                </div>
                                            )}
                                        </div>

                                        <img
                                            src={textedit}
                                            alt="edit"
                                            onClick={() =>
                                                setReplyData((prev: any) => ({
                                                    ...prev,
                                                    openTools: !prev.openTools,
                                                }))
                                            }
                                        />
                                        <img src={link} alt="link" />

                                        <div style={{ position: "relative", width: "100%" }}>
                                            <AIWriteEditor
                                                initialText={replyData.aiwriteInitialText}
                                                loading={replyData.aiwriteLoading || replyData.isRegenerating || replyData.isInitialLoading}
                                                onAIWrite={(text: string) => handleReplyAIWriteSparkle(text)}
                                                onRegenerate={() => handleReplyRegenerate()}
                                                replyeditor={true}
                                            />
                                        </div>
                                    </div>

                                    <div className={styles.rightActions}>
                                        <button className={"secondarybtn"} onClick={discardReply}>Discard</button>
                                        <button onClick={sendReply} disabled={replyData?.sending}>
                                            {replyData?.sending ? "Sending..." : "Send"}
                                        </button>
                                    </div>
                                </div>
                            </div>
                        )}
                    </div>
                </div>
            )}


            {openConfirmDelete && (
                <Modal
                    header={<div className={"modalheader"}><div>Delete</div></div>}
                    children={<div className={"modalcontent"}>Do you want to permanently delete the selected conversations?</div>}
                    btntxtsecondary={"Cancel"}
                    btntxtprimary={"Yes, delete"}
                    onclosePrimary={confirmDeleteAction}
                    oncloseSecondary={() => {
                        pendingDeleteActionRef.current = null;
                        setOpenConfirmDelete(false);
                    }}
                />
            )}

            {openMoveTrashDelete && (
                <Modal
                    header={<div className={"modalheader"}><div>Move to Trash?</div></div>}
                    children={<div className={"modalcontent"}>Are you sure you want to move this conversation to Trash?</div>}
                    btntxtsecondary={"Cancel"}
                    btntxtprimary={"Move to Trash"}
                    onclosePrimary={confirmTrashDeleteAction}
                    oncloseSecondary={() => {
                        pendingTrashDeleteActionRef.current = null;
                        setOpenMoveTrashDelete(false);
                    }}
                />
            )}
        </>
    )
}


export default EmailContainer;
