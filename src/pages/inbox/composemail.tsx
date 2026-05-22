import styles from "./scss/composemail.module.scss";
import mailicon from "../../assets/images/inbox/mailicon.svg";
import emoji from "../../assets/images/inbox/emoji.svg"
import textedit from "../../assets/images/inbox/texteditor.svg"
import link from "../../assets/images/inbox/link.svg"
import ai from "../../assets/images/inbox/aiicon.svg"
import TextEditor from "./texteditor";
import { useEffect, useRef, useState } from "react";
import chevrondown from "../../assets/images/inbox/chevrondown.svg";
import { useDispatch, useSelector } from "react-redux";
import { inboxDataSliceActions } from "../../store/slice/inbox/inboxSlice";
import { generateAIMessage, generateEmailReplay, generateNewEmailReplay, sendEmailhandler, updateNLPcontactDeals } from "../../base/service/nlp_service";
import { v4 as uuidv4 } from 'uuid';
import { GetItemFromStorage } from "../../base/custom_hooks/useStorage";
import { LocalStorage } from "../../base/custom_hooks/localStorageKeys";
import { excelFormats, imageFormat, isJSON, pdfFormats, removeSpaceNormalizetext, validateEmail, wordFormats } from "../../base/utils";
import { useNavigate, useSearchParams } from "react-router-dom";
import moment from "moment";
import Modal from "../component/modal/modal";
import EmojiPicker from "../component/emojipicker/EmojiPicker";
import FileCard from "../component/attachment/attachement";
import pdficon from "../../assets/images/pdfFile.svg"
import csv from "../../assets/images/inbox/newemail.svg"
import FileAttach from "../../assets/images/inbox/fileattach.svg";
import searchicon from "../../assets/images/inbox/searchicon.svg";
import { handleFileUpload, getEmailSuggestion } from "../../base/service/core_serviceApi";
import { fileFormatPoster } from "../shared/commonEditor";
import { insertDraftForEmailApi } from "../../base/service/core_serviceApi";
import { AttachmentFormatPoster } from "../shared/inboxattachment";
import threedot from '../../assets/images/inbox/threedot.svg'
import AIWriteEditor from "../component/aiwriteeditor/aiwriteeditor";
import AIRephraseTooltip from './airephrasetooltip';


const RenderTOCCBCC = ({
  inputRef,
  tags,
  value,
  removeTag,
  onChange,
  handleKeyDown,
  type, handleDrop,
  onSelectSuggestion,
}: any) => {
  const [showSuggestion, setShowSuggestion] = useState(false);
  const [suggestions, setSuggestions] = useState<any[]>([]);
  const [hasFetched, setHasFetched] = useState(false);
  const [searchedMore, setSearchedMore] = useState(false);
  const wrapperRef = useRef<HTMLDivElement | null>(null);

  useEffect(() => {
    const handleClickOutside = (e: MouseEvent) => {
      if (wrapperRef.current && !wrapperRef.current.contains(e.target as Node)) {
        setShowSuggestion(false);
      }
    };
    document.addEventListener('mousedown', handleClickOutside);
    return () => document.removeEventListener('mousedown', handleClickOutside);
  }, []);

  const fetchSuggestions = async (query: string) => {
    const res = await getEmailSuggestion(query ?? '');
    setHasFetched(true);
    if (Array.isArray(res)) {
      setSuggestions(res);
    } else {
      setSuggestions([]);
    }
  };

  const openSuggestions = () => {
    setShowSuggestion(true);
    setSearchedMore(false);
    fetchSuggestions(value ?? '');
  };

  useEffect(() => {
    if (!showSuggestion) return;
    setSearchedMore(false);
    fetchSuggestions(value ?? '');
  }, [value]);

  const handleShowMore = (e: any) => {
    e.stopPropagation();
    setSearchedMore(true);
    fetchSuggestions(value ?? '');
  };

  const filteredSuggestions = suggestions;

  const getInitials = (name: string) => {
    if (!name) return '';
    const parts = name.trim().split(/\s+/);
    const first = parts[0]?.[0] ?? '';
    const second = parts[1]?.[0] ?? '';
    return (first + second).toUpperCase();
  };

  const handleSelect = (email: string) => {
    if (!email) return;
    if (onSelectSuggestion) onSelectSuggestion(email, type);
    setShowSuggestion(false);
  };

  return (
    <div ref={wrapperRef} className={styles.chips} onClick={() => {
      inputRef?.current?.focus();
      openSuggestions();
    }} onDragOver={(e) => {
      e.preventDefault();
      e.stopPropagation();
    }}

      onDrop={(e) => {
        e.preventDefault();
        e.stopPropagation();

        handleDrop(e, type);
      }}>
      {tags.map((tag: any, i: number) => {
        const validEmail = validateEmail(tag);

        return (
          <span key={i} className={`${styles.chip} ${validEmail ? "" : styles.error}`}    draggable
            onDragStart={(e) => {
              e.dataTransfer.setData("tag", tag);
              e.dataTransfer.setData("from", type);
            }}>
            <svg xmlns="http://www.w3.org/2000/svg" width="14" height="14" viewBox="0 0 14 14" fill="none">
              <path d="M6.66389 0C2.98362 0 0 2.98349 0 6.66389C0 10.3443 2.98356 13.3278 6.66389 13.3278C10.3442 13.3278 13.3278 10.3441 13.3278 6.66389C13.3278 2.98369 10.3443 0 6.66389 0ZM6.65823 6.55807C6.51409 6.55807 6.36628 6.54227 6.21634 6.50929C5.49371 6.34982 4.93135 5.78739 4.77181 5.06489C4.49319 3.80302 5.44486 2.68781 6.65816 2.68781C6.8023 2.68781 6.9501 2.70361 7.10004 2.73659C7.82267 2.89619 8.38504 3.45849 8.54457 4.18112C8.82319 5.4428 7.87152 6.55807 6.65823 6.55807ZM5.39322 7.21713C5.42427 7.20946 5.45573 7.20606 5.48711 7.20606C5.57381 7.20606 5.66011 7.23305 5.73668 7.27697C5.90041 7.37086 6.079 7.44223 6.26965 7.48442C6.41979 7.5176 6.5678 7.53319 6.712 7.53319C7.05679 7.53319 7.38019 7.44337 7.66174 7.2869C7.73684 7.24525 7.82081 7.21953 7.90511 7.21953C7.93723 7.21953 7.96935 7.22326 8.00113 7.23125C8.9664 7.47302 9.74427 7.97461 10.1644 8.62068C10.2505 8.75329 10.25 8.93108 10.1636 9.06336C9.55629 9.99297 8.21664 10.6398 6.65743 10.6398C6.37081 10.6398 6.077 10.6178 5.77873 10.572C4.61928 10.3933 3.67107 9.8503 3.17874 9.12127C3.07345 8.9654 3.07205 8.76302 3.17768 8.60735C3.61709 7.95982 4.41536 7.45549 5.39322 7.21713Z"
              fill={validEmail ?"#23232B" : "#DB4437"} />
            </svg>
            {tag}
            <span className={styles.x} onClick={() => removeTag(i)}>
              <svg xmlns="http://www.w3.org/2000/svg" width="14" height="14" viewBox="0 0 14 14" fill="none">
                <path d="M6.66389 0C2.98356 0 0 2.98356 0 6.66389C0 10.3442 2.98356 13.3278 6.66389 13.3278C10.3442 13.3278 13.3278 10.3442 13.3278 6.66389C13.3278 2.98356 10.3443 0 6.66389 0ZM9.58467 8.8042C9.77993 8.99945 9.77993 9.31572 9.58467 9.5109C9.48705 9.60853 9.35917 9.65731 9.23129 9.65731C9.10341 9.65731 8.97553 9.60853 8.8779 9.5109L6.66396 7.29696L4.45001 9.5109C4.35239 9.60853 4.22451 9.65731 4.09663 9.65731C3.96875 9.65731 3.84087 9.60853 3.74324 9.5109C3.54799 9.31565 3.54799 8.99938 3.74324 8.8042L5.95719 6.59025L3.74324 4.37631C3.54799 4.18106 3.54799 3.86479 3.74324 3.6696C3.93849 3.47442 4.25476 3.47435 4.44995 3.6696L6.66389 5.88355L8.87783 3.6696C9.07309 3.47435 9.38936 3.47435 9.58454 3.6696C9.77973 3.86486 9.77979 4.18112 9.58454 4.37631L7.3706 6.59025L9.58467 8.8042Z"
                fill= {validEmail ?"#23232B" : "#DB4437"}/>
              </svg>
            </span>
          </span>
        )
      })}
      <input
        ref={inputRef}
        type="text"
        placeholder=""
        value={value}
        onChange={(e) => onChange(e)}
        onKeyDown={handleKeyDown}
      />
      {showSuggestion && hasFetched && (
        <div className="callhoveredoverlay" onClick={(e) => e.stopPropagation()}>
          <div className={styles.calllists}>
            {filteredSuggestions?.length > 0 ? (
              filteredSuggestions.map((s: any, i: number) => (
                <div
                  key={`${s?.email}-${i}`}
                  className={styles.list}
                  onMouseDown={(e) => e.preventDefault()}
                  onClick={(e) => { e.stopPropagation(); handleSelect(s?.email); }}
                >
                  <div className={styles.lefts}>
                    <div className={styles.avatorcontainer}>
                      {getInitials(s?.full_name)}
                    </div>
                    <div className={styles.info}>
                      <div className={styles.name}>{s?.full_name}</div>
                      <div className={styles.phn}>{s?.email}</div>
                    </div>
                  </div>
                  <div className={styles.rights}>
                    <img src={""} />
                  </div>
                </div>
              ))
            ) : searchedMore ? (
              <div
                className={styles.list}
                style={{ justifyContent: 'center', cursor: 'default', pointerEvents: 'none' }}
              >
                <span>No results found</span>
              </div>
            ) : (
              <div
                className={styles.list}
                onMouseDown={(e) => e.preventDefault()}
                onClick={handleShowMore}
                style={{ cursor: 'pointer' }}
              >
                <div className={styles.lefts}>
                  <img src={searchicon} alt="search" style={{ width: 18, height: 18 }} />
                  <div className={styles.info}>
                    <div className={styles.name}>Show more results</div>
                  </div>
                </div>
              </div>
            )}
          </div>
        </div>
      )}
    </div>
  )
};


const ComposeMail = () => {

  const [minimizeSession, setMinimizeSession] = useState<any>(false);
  const [openTools, setOpenTools] = useState<any>(false);
  const dispatch = useDispatch();
  const LoginUserDetails = GetItemFromStorage(LocalStorage.USER_DETAILS);
  const {
    domainEmailConfig, forwardedMessage, forwardedMailDetails, replyMessage,
    replyAllMailDetails, replyALLMessage, replyMailDetails, aiSuggestions,
    currentSuggestionIndex, inboxList_allrecord, pendingInsertReply } = useSelector((store: any) => store.inboxChatStore)
  const [forwardedEditorContent, setForwardedEditorContent] = useState({
    fresh: true,
    content: "",
    bottomPush: true
  });
  const [forwardedEditorTick, setForwardedEditorTick] = useState(0);
  const pushExistContent = (content: string, bottomPush = true) => {
    setForwardedEditorContent({
      fresh: false,
      content,
      bottomPush
    });
    setForwardedEditorTick((t) => t + 1);
  };
  const pushNewContent = (content: string) => {
    setForwardedEditorContent({
      fresh: true,
      content,
      bottomPush: false
    });
    setForwardedEditorTick((t) => t + 1);
  };
  const [params] = useSearchParams()
  const emailId = params.get('emailid')
  const draftSessionId = params.get('draftSessionId');
  const EmailHistryId = params.get('EmailHistryId');
  const draftId = params.get('draftId');
  const CustomerId = params.get('CustomerId') || '0';
  const isforwardmail = params.get('forward');
  const isreplaymail = params.get('replay');
  const currentsessionId = params.get('sessionId') || uuidv4();
  const [openAi, setOpenAi] = useState<any>(false);
  const currentSuggestion = aiSuggestions?.[currentSuggestionIndex];
  const [aiPrompt, setAiPrompt] = useState("");
  const [isGenerating, setIsGenerating] = useState(false);
  const [isRegenerating, setIsRegenerating] = useState(false);
  const [isInitialLoading, setIsInitialLoading] = useState(false);
  const [aiwriteLoading, setAiwriteLoading] = useState(false);
  const [aiwriteInitialText, setAiwriteInitialText] = useState("");
  const [openConfirmDiscard, setopenConfirmDiscard] = useState(false);
  const [subjectChangeModal, setSubjectChangeModal] = useState<{ open: boolean; prevSubject: string; newSubject: string }>({ open: false, prevSubject: "", newSubject: "" });
  const pickerRef = useRef<HTMLDivElement | null>(null);
  const emojiRef = useRef<HTMLImageElement | null>(null);
  const editorRef = useRef<any>(null);
  const draftUniqueIdRef = useRef<string>(uuidv4());
  const draftDebounceRef = useRef<any>(null);
  const hasEditedDraftRef = useRef(false);
  const navigate = useNavigate()

  const [tagsInput, setTagsInput] = useState<any>({ to: [], cc: [], bcc: [] });
  const [mailInput, setMailInput] = useState<any>({ to: "", cc: "", bcc: "",});
  
  const toInputRef = useRef<HTMLInputElement>(null);
  const ccInputRef = useRef<HTMLInputElement>(null);
  const bccInputRef = useRef<HTMLInputElement>(null);


  const openToolsHandler = () => {
    setOpenTools(!openTools)
  }

  const discardHandler = () => {
    dispatch(inboxDataSliceActions.setCleanReplyForword());
    dispatch(inboxDataSliceActions.clearAiSuggestions());
    navigate('/inbox');

    if (draftUniqueIdRef.current) {
      insertDraftForEmailApi({
        draftUniqueId: draftUniqueIdRef.current,
        domainId: LoginUserDetails?.domainId ?? LoginUserDetails?.domain_id ?? null,
        fromEmail: domainEmailConfig?.emailAddress ?? null,
        toEmail: composemailData?.to ?? mailInput?.to ?? null,
        cc: composemailData?.cc ?? null,
        bcc: composemailData?.bcc ?? null,
        emailSubject: composemailData?.subject ?? null,
        messageJson: JSON.stringify({
          html: composemailData?.mainbody ?? "",
        }),
        agentId: LoginUserDetails?.userId ?? null,
        is_sent: 1,
      });
    }
  }



  const initialState = {
    sessionId: uuidv4(),
    domainId: LoginUserDetails?.domainId,
    to: emailId || "",
    cc: '',
    bcc: '',
    subject: '',
    mainbody: '',
    attachments: [],
    replay_content:''
  }
  const [composemailData, setComposemailData] = useState<any>({ ...initialState })

  useEffect(() => {
    const hasDraftSelector = Boolean(draftSessionId || draftId);
    if (!hasDraftSelector) return;

    let active = true;

    if (replyMessage || forwardedMessage || replyALLMessage) {
      dispatch(inboxDataSliceActions.setCleanReplyForword());
    }

    const loadDraft = () => {
      try {
        const draftList: any[] = Array.isArray(inboxList_allrecord) ? inboxList_allrecord : [];
        if (!active || draftList.length === 0) return;

        const selectedDraft = draftList.find((draft: any) => {
          const currentSessionId = String(draft?.sessionId ?? draft?.session_id ?? "");
          const currentDraftId = String(draft?.draftId ?? draft?.draft_id ?? "");
          if (draftId) return currentDraftId === String(draftId);
          if (draftSessionId) return currentSessionId === String(draftSessionId);
          return false;
        }) ?? null;

        if (!selectedDraft) return;

        const rawMessage = selectedDraft?.messageJson ?? selectedDraft?.message_json ?? null;
        let parsedMessage: any = null;
        if (typeof rawMessage === "string") {
          try {
            parsedMessage = JSON.parse(rawMessage);
          } catch {
            parsedMessage = { html: rawMessage };
          }
        } else if (rawMessage && typeof rawMessage === "object") {
          parsedMessage = rawMessage;
        }

        const sessionUniqueId = selectedDraft?.sessionId ?? selectedDraft?.session_id ?? null;
        if (sessionUniqueId !== null && sessionUniqueId !== undefined && String(sessionUniqueId).trim() !== "") {
          draftUniqueIdRef.current = String(sessionUniqueId);
        }

        const draftTo = selectedDraft?.toEmail ?? selectedDraft?.to_email ?? "";
        const draftCc = selectedDraft?.cc ?? selectedDraft?.draft_cc ?? "";
        const draftBcc = selectedDraft?.bcc ?? selectedDraft?.draft_bcc ?? "";
        const draftSubject = selectedDraft?.emailSubject ?? selectedDraft?.email_subject ?? "";
        const draftBody = parsedMessage?.html ?? "";
        const draftReplay = parsedMessage?.replay_content ?? "";

        const splitTags = (val: string) =>
          String(val ?? "").split(',').map((m) => m.trim()).filter(Boolean);

        hasEditedDraftRef.current = false;
        pushNewContent(draftBody);
        setTagsInput({
          to: splitTags(draftTo),
          cc: splitTags(draftCc),
          bcc: splitTags(draftBcc),
        });
        setMailInput({ to: "", cc: "", bcc: "" });
        setComposemailData((prev: any) => ({
          ...prev,
          to: draftTo,
          cc: draftCc,
          bcc: draftBcc,
          subject: draftSubject,
          mainbody: draftBody,
          replay_content: draftReplay,
          attachments: [],
        }));

        if (String(draftCc ?? "").trim() || String(draftBcc ?? "").trim()) {
          setMinimizeSession(true);
        }
      } catch (error) {
        console.log(error);
      }
    };

    loadDraft();
    return () => {
      active = false;
    };
  }, [
    draftSessionId,
    draftId,
    replyMessage,
    forwardedMessage,
    replyALLMessage,
    inboxList_allrecord,
  ]);

  useEffect(() => {
    if (!replyMessage && !forwardedMessage && !replyALLMessage) {
      if (draftSessionId || draftId) return;
      hasEditedDraftRef.current = false;
      draftUniqueIdRef.current = uuidv4();
      setForwardedEditorContent({
        fresh: true,
        content: "",
      });
      const splitTags = (val: string) => String(val ?? "").split(',').map((m) => m.trim()).filter(Boolean);
      setTagsInput({ to: splitTags(emailId || ""), cc: splitTags(""), bcc: splitTags("") });
      setMailInput({ to: "", cc: "", bcc: "" });
      setMinimizeSession(false);
      setComposemailData((prev: any) => ({
        ...prev,
        to: emailId || "",
        cc: "",
        bcc: "",
        subject: "",
        mainbody: "",
        replay_content: ""
      }));
      return;
    }

    if (replyMessage && replyMailDetails) {
      handleReplyMessage()
      return;
    }
    if (replyALLMessage && replyAllMailDetails) {
      handleReplyAllMessage()
      return;
    }
    if (forwardedMessage && forwardedMailDetails) {
      handleForwardMessage()
    }
  }, [forwardedMessage, forwardedMailDetails, replyMessage,replyALLMessage,replyAllMailDetails, replyMailDetails, emailId, draftSessionId, draftId]);

  const handleReplyAllMessage =()=> {
    try {
      const replyHtml = (records) => {
        return `
            <br/>
            <div class="gmail_quote" style="font-family:inherit;color:#5F6368;">
              <div class="gmail_attr" style="margin-bottom:6px;">On ${records?.date
            ? moment(records.date).format("ddd, MMM DD, YYYY [at] h:mm A")
            : ""}, ${records?.from || ""} &lt;${records?.fromEmail || ""}&gt; wrote:</div>
              <blockquote style="margin:0 0 0 0.8ex;border-left:2px solid #C9C9CF;padding-left:1ex;color:#5F6368;">
                ${records?.message || ""}
              </blockquote>
            </div>
          `;
      };
      const replayHtmlHandler = replyAllMailDetails?.map((item: any) => {
        return replyHtml(item)
      })
      // setForwardedEditorContent(replayHtmlHandler?.join("") || "");

      const replyAllTo = replyAllMailDetails[replyAllMailDetails?.length - 1] ?? {};
      console.log("replyAllMailDetails?.[0]", replyAllTo)
      if (replyAllTo?.parent_FromEmail) {
        setTagsInput((prev: any) => ({
          ...prev,
          to: replyAllTo?.parent_FromEmail.split(',').map((m: string) => m.trim()).filter(Boolean),
          cc: validateArray(replyAllTo?.cc),
          bcc: validateArray(replyAllTo?.bcc)
        }));
      }
      setComposemailData((prev: any) => ({
        ...prev,
        to: replyAllTo?.parent_FromEmail,
        subject: prev?.subject || `Re: ${replyAllMailDetails?.[0]?.parent_EmailSubject || ""}`,
        mainbody: "",
        replay_content: replayHtmlHandler?.join("") || ""
      }));

    } catch (error) {
      console.log(error)
    }
  }

      const validateArray = (item :any) => {
          try {
              if(!item) return []
              if(typeof item === 'string') return [item]
              if(Array.isArray(item)){
                  const f_item = item.filter((i)=> typeof i === 'string' ? i?.trim() : false)
                  return f_item?.length > 0 ? f_item : [] 
              }
          } catch (error) {
              return []
          }
      }

  const handleReplyMessage =()=> {
    try {
      // if (replyMessage) {
      //   setOpenAi(true)
      // }

      // old code
      // const replyHtml = `
      //   <div>
      //     <h3>-------Replied Message----------</h3>
      //     <p>From : ${replyMailDetails?.from || ""} &lt;${replyMailDetails?.fromEmail || ""}&gt;</p>
      //     <p>Date : ${replyMailDetails?.date ? moment(replyMailDetails.date).format("MMM DD, YYYY [at] h:mm A") : ""}</p>
      //     <p>Subject : ${replyMailDetails?.subject || ""}</p>
      //     <p>To : ${replyMailDetails?.toMail || replyMailDetails?.to || ""}</p>
      //   </div>
      //   <br/>
      //   <p>${replyMailDetails?.message || ""}</p>
      // `;

      let replyHtml = "";
      if(!replyMailDetails?.newCompose){
        replyHtml = `
          <div class="gmail_quote" style="font-family:inherit;color:#5F6368;">
            <div class="gmail_attr" style="margin-bottom:6px;">On ${replyMailDetails?.date
            ? moment(replyMailDetails.date).format("ddd, MMM DD, YYYY [at] h:mm A")
            : ""}, ${replyMailDetails?.from || ""} &lt;${replyMailDetails?.fromEmail || ""}&gt; wrote:</div>
            <blockquote style="margin:0 0 0 0.8ex;border-left:2px solid #C9C9CF;padding-left:1ex;color:#5F6368;">
              ${replyMailDetails?.message || ""}
            </blockquote>
          </div>
        `;
        // setForwardedEditorContent(replyHtml);
      }
      const replyTo = replyMailDetails?.toMail || replyMailDetails?.to || "";
      if (replyTo) {
        setTagsInput((prev: any) => ({
          ...prev,
          to: replyTo.split(',').map((m: string) => m.trim()).filter(Boolean),
        }));
      }

      const originalSubject = replyMailDetails?.subject || "";
      const subject = originalSubject.startsWith("Re:")
        ? originalSubject
        : originalSubject
          ? `Re: ${originalSubject}`
          : "";

      setComposemailData((prev: any) => ({
        ...prev,
        to: replyTo,
        subject: prev?.subject || subject ,
        mainbody: "",
        replay_content: replyHtml
      }));

      gendrateEmail({
        email: replyMailDetails?.fromEmail,
        domainId: `${LoginUserDetails?.domainId}`,
        subject: replyMailDetails?.subject || "",
      })

    } catch (error) {

    }
  }

  const handleForwardMessage =()=> {
    const forwardedHtml = `
      <br/>
      <br/>
        <div>
          <h3>-------Forwarded Message----------</h3>
          <p>From : ${forwardedMailDetails?.from || ""} &lt;${forwardedMailDetails?.fromEmail || ""}&gt;</p>
          <p>Date : ${forwardedMailDetails?.date ? moment(forwardedMailDetails.date).format("MMM DD, YYYY [at] h:mm A") : ""}</p>
          <p>Subject : ${forwardedMailDetails?.subject || ""}</p>
          <p>To : ${forwardedMailDetails?.toMail || forwardedMailDetails?.to || ""}</p>
        </div>
        <br/>
        <p>${forwardedMailDetails?.message || ""}</p>
      `;
    pushNewContent(forwardedHtml);
    setComposemailData((prev: any) => ({
      ...prev,
      subject: prev?.subject || `Fwd: ${forwardedMailDetails?.subject || ""}`,
      mainbody: forwardedHtml,
    }));

    // gendrateEmail({
    //   email: domainEmailConfig?.emailAddress,
    //   domainId: `${LoginUserDetails?.domainId}`,
    //   subject: replyMailDetails?.subject || "",
    // })
  }

  const gendrateEmail = async (data: any) => {
    setIsInitialLoading(true);

    try {
      const res = await generateEmailReplay(data);

      if (res?.status === "success" && res?.response) {
        // dispatch(
        //   inboxDataSliceActions.setAiSuggestions([res.response])
        // );
        pushExistContent(res?.response);
        setComposemailData(prev => ({
          ...prev,
          mainbody: res?.response,
        }));
      }

      // setForwardedEditorContent((prev : any) => (`
      //     <div>
      //       ${res?.data}
      //     </div>
      //     <br/>` + 
      //     (!!prev ? prev : '')
      //   ));

      // setComposemailData((prev : any) => ({
      //   ...prev,
      //   mainbody: `
      //     <div>
      //       ${res?.data}
      //     </div>
      //     <br/>` + 
      //     (`${!!prev?.mainbody?.trim() ? <p>{prev?.mainbody || ""}</p> : ''}`)
      //   ,
      // }));

    } catch (err) {
      console.error("Initial generate error:", err);
    } finally {
      setIsInitialLoading(false);
    }
  };

  const handleNext = () => {
    if (currentSuggestionIndex < aiSuggestions.length - 1) {
      dispatch(
        inboxDataSliceActions.setCurrentSuggestionIndex(
          currentSuggestionIndex + 1
        )
      );
    }
  };

  const handlePrev = () => {
    if (currentSuggestionIndex > 0) {
      dispatch(
        inboxDataSliceActions.setCurrentSuggestionIndex(
          currentSuggestionIndex - 1
        )
      );
    }
  };

  const applyInsertReply = (draft: { subject?: string; body?: string } | null | undefined , bottomPush = true) => {
    if (!draft?.body) return;
    setOpenAi(false);
    const html = `${draft.body}<br/>`;
    const newContent = html + (composemailData.mainbody || "");
    pushExistContent(html, bottomPush);
    setComposemailData(prev => ({
      ...prev,
      mainbody: newContent,
      subject: draft?.subject || prev?.subject
    }));
  };

  const applyInsertReplyBodyOnly = (draft: { body?: string } | null | undefined) => {
    if (!draft?.body) return;
    setOpenAi(false);
    const html = `${draft.body}<br/>`;
    const newContent = html + (composemailData.mainbody || "");
    pushExistContent(html);
    setComposemailData(prev => ({
      ...prev,
      mainbody: newContent,
    }));
  };

  const confirmSubjectChange = () => {
    setComposemailData((prev: any) => ({ ...prev, subject: subjectChangeModal.newSubject }));
    setSubjectChangeModal({ open: false, prevSubject: "", newSubject: "" });
  };

  const dismissSubjectChange = () => {
    setSubjectChangeModal({ open: false, prevSubject: "", newSubject: "" });
  };

  useEffect(() => {
    if (!pendingInsertReply?.body) return;
    applyInsertReply(pendingInsertReply, true);
    dispatch(inboxDataSliceActions.clearPendingInsertReply());
  }, [pendingInsertReply]);

  const handleInsertReply = () => {
    if (!currentSuggestion?.body) return;
    applyInsertReply(currentSuggestion);

    // if(!(isforwardmail || isreplaymail)){
    //   setComposemailData((prev: any) => ({
    //     ...prev,
    //     subject: currentSuggestion?.subject || prev?.subject,
    //   }))
    // }
  };

  const handleGenerate = async () => {
    if (!aiPrompt.trim() || isGenerating) return;

    setIsGenerating(true);

    try {
      const res: any = await generateNewEmailReplay({
        content: aiPrompt,
        domainId: `${LoginUserDetails?.domainId}`,
      });

      if (res?.status === "success" && res?.response) {
        if(isJSON(res?.response)) {
          const data = JSON.parse(res?.response);
          dispatch(inboxDataSliceActions.addAiSuggestion(data));
          dispatch(
            inboxDataSliceActions.setCurrentSuggestionIndex(aiSuggestions.length)
          );
        }
        // setAiPrompt("");
      }
    } catch (err) {
      console.error(err);
    } finally {
      setIsGenerating(false);
    }
  };

  const handleRegenerate = async () => {
    if (!currentSuggestion || isRegenerating) return;

    setIsRegenerating(true);

    const body = {
      text: JSON.stringify(currentSuggestion),
      tone: "Elaborate",
    };

    try {
      const response: any = await generateAIMessage(body);
      console.log("generateAIMessage response", response)

      if (isJSON(response)) {
        dispatch(
          inboxDataSliceActions.replaceAiSuggestion({
            index: currentSuggestionIndex,
            data: JSON.parse(response),
          })
        );
      }
    } catch (err) {
      console.error(err);
    } finally {
      setIsRegenerating(false);
    }
  };

  const stripHtmlForEditor = (html: string): string => {
    if (!html) return "";
    const tmp = document.createElement("div");
    tmp.innerHTML = html;
    return (tmp.textContent || tmp.innerText || "").trim();
  };

  const handleAIWriteSparkle = async (prompt: string) => {
    if (!prompt?.trim() || aiwriteLoading) return;
    setAiwriteLoading(true);
    try {
      const res: any = await generateNewEmailReplay({
        content: prompt,
        domainId: `${LoginUserDetails?.domainId}`,
      });
      if (res?.status === "success" && res?.response && isJSON(res?.response)) {
        const data = JSON.parse(res?.response);
        dispatch(inboxDataSliceActions.setAiSuggestions([data]));

        if (!data?.body) return;

        const existingSubject = (composemailData?.subject || "").trim();
        const newSubject = (data?.subject || "").trim();
        const shouldConfirmSubject = !!existingSubject && !!newSubject && existingSubject !== newSubject;

        if (shouldConfirmSubject) {
          applyInsertReplyBodyOnly(data);
          setSubjectChangeModal({
            open: true,
            prevSubject: existingSubject,
            newSubject,
          });
        } else {
          applyInsertReply(data);
        }
      }
    } catch (err) {
      console.error(err);
    } finally {
      setAiwriteLoading(false);
    }
  };

  useEffect(() => {
    if (currentSuggestion?.body) {
      setAiwriteInitialText(stripHtmlForEditor(currentSuggestion.body));
    }
  }, [currentSuggestion?.body]);


  const handleInput = (e) => {
    const { name, value } = e.target
    hasEditedDraftRef.current = true;
    setComposemailData({ ...composemailData, [name]: value })
  }

  useEffect(() => {
    if (!hasEditedDraftRef.current) return;

    if (draftDebounceRef.current) {
      clearTimeout(draftDebounceRef.current);
    }

    const tomails = parseEmails(composemailData?.to);
    const ccmails = parseEmails(composemailData?.cc);
    const bccmails = parseEmails(composemailData?.bcc);

    if(mailInput?.to) tomails.push(mailInput?.to);
    if(mailInput?.cc) ccmails.push(mailInput?.cc);
    if(mailInput?.bcc) bccmails.push(mailInput?.bcc);

    draftDebounceRef.current = setTimeout(async () => {
      const hasAnyContent = [
        tomails?.join(","),
        ccmails?.join(","),
        bccmails?.join(","),
        composemailData?.subject,
        composemailData?.mainbody,
      ].some((value) => String(value ?? "").trim().length > 0);

      if (!hasAnyContent) return;

      await insertDraftForEmailApi({
        draftUniqueId: draftUniqueIdRef.current,
        domainId: LoginUserDetails?.domainId ?? LoginUserDetails?.domain_id ?? null,
        fromEmail: domainEmailConfig?.emailAddress ?? null,
        toEmail: tomails?.join(",") ?? null,
        cc: ccmails?.join(",") ?? null,
        bcc: bccmails?.join(",") ?? null,
        emailSubject: composemailData?.subject ?? null,
        messageJson: JSON.stringify({
          html: composemailData?.mainbody ?? "",
        }),
        agentId: LoginUserDetails?.userId ?? null,
      });
    }, 900);

    return () => {
      if (draftDebounceRef.current) {
        clearTimeout(draftDebounceRef.current);
      }
    };
  }, [
    composemailData?.to,
    composemailData?.cc,
    composemailData?.bcc,
    composemailData?.subject,
    composemailData?.mainbody,
    mailInput,
    LoginUserDetails?.domainId,
    LoginUserDetails?.domain_id,
    LoginUserDetails?.userId,
    domainEmailConfig?.emailAddress,
  ]);

  const validateData = (data) => {

    if (!(data?.to || mailInput?.to)) {
      alert("Please add To Address")
      return false
    };

    // if (!(data?.mainbody?.trim() || composemailData?.attachments?.length > 0)) {
    //   alert("Please add Email Body")
    //   return false
    // };

    const checkToMail = data?.to?.trim() ? data?.to?.split(",")?.every((mail) => validateEmail(mail))
      : mailInput?.to ? validateEmail(mailInput?.to) : false
    const checkCCMail = data?.cc?.trim() ? data?.cc?.split(",")?.every((mail) => validateEmail(mail))
      : mailInput?.cc ? validateEmail(mailInput?.cc) : true
    const checkBCCMail = data?.bcc?.trim() ? data?.bcc?.split(",")?.every((mail) => validateEmail(mail))
      : mailInput?.bcc ? validateEmail(mailInput?.bcc) : true

    if(!!mailInput?.to?.trim()) {
      setTagsInput((prev: any) => ({ ...prev, to: [...(prev?.to || []), mailInput?.to],
      }))
      setMailInput((prev)=> ({ ...prev, to: "" }))
    }
    if(!!mailInput?.cc?.trim()) {
      setTagsInput((prev: any) => ({ ...prev, cc: [...(prev?.cc || []), mailInput?.cc] }))
      setMailInput((prev)=> ({ ...prev, cc: "" }))
    }
    if(!!mailInput?.bcc?.trim()) {
      setTagsInput((prev: any) => ({ ...prev, bcc: [...(prev?.bcc || []), mailInput?.bcc] }))
      setMailInput((prev)=> ({ ...prev, bcc: "", }))
    }

    if(!checkToMail || !checkCCMail || !checkBCCMail) {
      return false
    }

    return true
  }

  function parseEmails (value: any){
    try {
      if (!value || typeof value !== "string") return [];
      const x = value.split(",").map((mail) => mail?.trim()).filter((mail) => mail?.length > 0)
      return x
    } catch (err) {
      console.error("Email parsing error:", err);
      return [];
    }
  };

  const sendEmail = async () => {

    if (!validateData(composemailData)) {
      return
    } else if (!domainEmailConfig?.emailAddress) {
      return
    }

    // updateing all emails
    const tomails = parseEmails(composemailData?.to);
    const ccmails = parseEmails(composemailData?.cc);
    const bccmails = parseEmails(composemailData?.bcc);

    if(mailInput?.to) tomails.push(mailInput?.to);
    if(mailInput?.cc) ccmails.push(mailInput?.cc);
    if(mailInput?.bcc) bccmails.push(mailInput?.bcc);

    const body = {
      sessionId: uuidv4(),
      domainId: LoginUserDetails?.domainId,
      from_email: domainEmailConfig?.emailAddress ?? '',
      to_email: tomails?.length > 0 ? tomails : [],
      cc: ccmails?.length > 0 ? ccmails : [],
      bcc: bccmails?.length > 0 ? bccmails : [],
      subject: composemailData?.subject ?? '',
      content: composemailData?.mainbody ?? '',
      emailhistoryId: EmailHistryId || "",
      attachments: composemailData?.attachments?.length > 0 ? composemailData?.attachments : [],
      userId: LoginUserDetails?.userId ?? null,
      replay_content: composemailData?.replay_content ?? ''
    }

    const response: any = await sendEmailhandler(body)
    if (response?.response?.status === 'success' || response?.status === 'processing') {
      endEmailNlpApi()
      dispatch(inboxDataSliceActions.setComposeMailToggle(false))
      // dispatch(inboxDataSliceActions.setEmailSendSuccess())
      dispatch(inboxDataSliceActions.setCleanReplyForword());
      dispatch(inboxDataSliceActions.clearAiSuggestions())
    }

    navigate("/inbox");
    if (draftDebounceRef.current) {
      clearTimeout(draftDebounceRef.current);
    }
    insertDraftForEmailApi({
      draftUniqueId: draftUniqueIdRef.current,
      domainId: LoginUserDetails?.domainId ?? LoginUserDetails?.domain_id ?? null,
      fromEmail: domainEmailConfig?.emailAddress ?? null,
      toEmail: composemailData?.to ?? null,
      cc: composemailData?.cc ?? null,
      bcc: composemailData?.bcc ?? null,
      emailSubject: composemailData?.subject ?? null,
      messageJson: JSON.stringify({
        html: composemailData?.mainbody ?? "",
      }),
      agentId: LoginUserDetails?.userId ?? null,
      is_sent: 1,
    });
  }


  const endEmailNlpApi = () => {
    updateNLPcontactDeals({
      coid: Number(CustomerId) || 0,
      domainId: LoginUserDetails?.domainId || 0,
      emailId: composemailData?.to || "",
      phoneNumber: "",
    });
  }

  const [showEmojiPicker, setShowEmojiPicker] = useState(false);

  const toggleEmojiPicker = () => {
    setShowEmojiPicker(prev => !prev);
  };

  const handleEmojiClick = (emojiData: any) => {
    if (editorRef.current) {
      editorRef.current.insertEmoji(emojiData.emoji || emojiData);
    }
  };


  useEffect(() => {
    const handleClickOutside = (e: MouseEvent) => {
      if (
        pickerRef.current &&
        !pickerRef.current.contains(e.target as Node) &&
        emojiRef.current &&
        e.target !== emojiRef.current
      ) {
        setShowEmojiPicker(false);
      }
    };

    document.addEventListener("mousedown", handleClickOutside);
    return () => {
      document.removeEventListener("mousedown", handleClickOutside);
    };
  }, []);

  useEffect(() => {
    hasEditedDraftRef.current = true;
    setComposemailData((prev: any) => ({
      ...prev,
      to: tagsInput?.to?.join(','),
      cc: tagsInput?.cc?.join(','),
      bcc: tagsInput?.bcc?.join(','),
    }));
  }, [tagsInput]);

  const handleKeyDown = (e: any, type: string) => {
    try {
      const val = mailInput[type]?.replace(',', '').trim();

      if (e.key === 'Backspace' && !mailInput[type] && tagsInput[type].length) {
        e.preventDefault();
        setTagsInput((prev: any) => ({
          ...prev,
          [type]: prev[type].slice(0, -1),
        }))
      }
      // if(!validateEmail(val)) {
      //   return;
      // }
      else if ((e.key === 'Enter' || e.key === ',') && val) {
        e.preventDefault();
        if (!tagsInput[type]?.includes(val)) {
          setTagsInput((prev: any) => ({
            ...prev,
            [type]: [...(prev[type] || []), val],
          }))
        }
        setMailInput((prev: any) => ({ ...prev, [type]: '' }));
      }

    } catch (error) {
      console.log(error)
    }
  };

  const removeTag = (index: number, type :any) => {
    setTagsInput((prev: any) => ({
      ...prev,
      [type]: prev[type]?.filter((_: any, i: any) => i !== index)
    }))
  };
  const handleMailInput = (e, type) => {
    setMailInput((prev: any) => ({ ...prev, [type]: e.target.value }))
  }

  const handleSelectSuggestion = (email: string, type: string) => {
    if (!email) return;
    setTagsInput((prev: any) => {
      if (prev?.[type]?.includes(email)) return prev;
      return { ...prev, [type]: [...(prev?.[type] || []), email] };
    });
    setMailInput((prev: any) => ({ ...prev, [type]: '' }));
  }

  const fileRef = useRef(null)
  const handleFileChange = (event: React.ChangeEvent<HTMLInputElement>) => {
    const file: any =  event.target.files;
    if (file) {
      handleFileUpload(file).then((res: any) => {
        setComposemailData((prev: any) => ({
          ...prev,
          attachments: [...(prev?.attachments || []), ...res],
        }))
      })
        .catch((error) => {
          console.log(error);
        });
    }

    event.target.value = '';
  };

  const removeFile = (index: number) => {
    setComposemailData((prev: any) => ({
      ...prev,
      attachments: prev?.attachments?.filter((_: any, i: any) => i !== index),
    }));
  };

  const handleDrop = (e: any, toField: string) => {
    e.preventDefault();

    const tag = e.dataTransfer.getData("tag");
    const fromField = e.dataTransfer.getData("from");

    console.log("DROP:", { tag, fromField, toField });

    if (!tag || !fromField) return;

    setTagsInput((prev: any) => {
      const updated = { ...prev };
      updated[fromField] = updated[fromField].filter(
        (t: string) => t !== tag
      );
      if (!updated[toField].includes(tag)) {
        updated[toField].push(tag);
      }
      return updated;
    });
  };

  return (
    <div className={styles.composeWrapper}>
      <div className={` ${styles.header}`}>

        <span style={{ display: "grid" }}><img src={mailicon} /></span>
        <span className={`content ${styles.subcontent}`}>Email</span>

      </div>
      <div className={styles.innercomposebody}>
        <div className={styles.row} style={{position:"relative"}}>
          <label className="boldtxt">To</label>
          <RenderTOCCBCC
            type="to"
            inputRef={toInputRef}
            tags={tagsInput.to ?? []}
            value= {mailInput.to ?? ''}
            removeTag = {(i :any)=> removeTag(i, 'to')}
            onChange = {(e : any) => handleMailInput(e, 'to')}
            handleKeyDown = {(e) => handleKeyDown(e, 'to')}
            handleDrop={handleDrop}
            onSelectSuggestion={handleSelectSuggestion}
          />

          <div className={styles.ccbcc}>
            <span>Cc</span>
            <span>Bcc</span>
            <span
              style={{
                display: 'grid',
                placeContent: 'center',
                rotate: !minimizeSession ? '0deg' : '180deg',
              }}
              onClick={() => setMinimizeSession(!minimizeSession)}
            >
              <img src={chevrondown} />
            </span>
          </div>
        </div>

        {minimizeSession && (<>
          <div className={styles.row}>
            <label className="boldtxt">Cc</label>
            <RenderTOCCBCC
              type="cc"
              inputRef={ccInputRef}
              tags={tagsInput.cc ?? []}
              value={mailInput.cc ?? ''}
              removeTag={(i: any) => removeTag(i, 'cc')}
              onChange={(e: any) => handleMailInput(e, 'cc')}
              handleKeyDown={(e) => handleKeyDown(e, 'cc')}
              handleDrop={handleDrop}
              onSelectSuggestion={handleSelectSuggestion}
            />
          </div>
          <div className={styles.row}>
            <label className="boldtxt">Bcc</label>
            <RenderTOCCBCC
              type="bcc"
              inputRef={bccInputRef}
              tags={tagsInput.bcc ?? []}
              value={mailInput.bcc ?? ''}
              removeTag={(i: any) => removeTag(i, 'bcc')}
              onChange={(e: any) => handleMailInput(e, 'bcc')}
              handleKeyDown={(e) => handleKeyDown(e, 'bcc')}
              handleDrop={handleDrop}
              onSelectSuggestion={handleSelectSuggestion}
            />
          </div>
        </>)}


        {/* Subject */}
        <div className={styles.row}>
          <label className="boldtxt">Subject</label>
          <input
            type="text"
            className={styles.subject}
            placeholder="Add Subject here"
            name="subject"
            onChange={handleInput}
            maxLength={100}
            value={composemailData?.subject || ''}
            onDragOver={(e) => {
              e.preventDefault();
            }}

            onDrop={(e) => {
              e.preventDefault();
              const tag = e.dataTransfer.getData("tag");
              if (!tag) return;
              setComposemailData((prev: any) => ({
                ...prev,
                subject: (prev.subject ? prev.subject + " " : "") + tag,
              }));
            }}
          />
        </div>

        {/* Body */}
        <div className={styles.body}>
          {/* <FileCard
            key={1}
            name="resume.pdf"
            size="102 KB"
            img={csv}
            onRemove={handleRemove}
          /> */}
          {composemailData?.attachments?.length > 0 && (
            <div style={{ display: "flex", flexWrap: "wrap", gap: "8px" , alignItems:"center" }}>
              {composemailData?.attachments?.map(
                (attachment: any, index: number) => AttachmentFormatPoster(attachment, index, removeFile)
              )}
            </div>
          )}
          <TextEditor
            ref={editorRef}
            openTools={openTools}
            initialContent={forwardedEditorContent}
            contentTick={forwardedEditorTick}
            onchange={(e) => handleInput({ target: { name: "mainbody", value: e.target.innerHTML } })}
          />
          <AIRephraseTooltip
            editorElementId="emailcontent"
            onContentChange={(html) => handleInput({ target: { name: "mainbody", value: html } })}
          />

          {composemailData?.replay_content && (
            <span
              className={styles.replayimg}
              onClick={() => {
                setComposemailData((prev: any) => ({ ...prev, replay_content: "" }));
                pushExistContent(composemailData?.replay_content);
              }}>
              <img src={threedot} />
            </span>)}

        </div>

        {/* Footer */}
        <div className={styles.footer} style={{gap:"12px"}}>
          <div className={styles.leftActions}>
            <div style={{ position: "relative" , display:"flex"}} className={`${styles.emojipickerinbox} ${showEmojiPicker ? styles.active : ""}`}>
              <img
                ref={emojiRef}
                src={emoji}
                alt="emoji"
                onClick={toggleEmojiPicker}
                onMouseDown={(e) => e.preventDefault()}
              />

              {showEmojiPicker && (
                <div ref={pickerRef}>
                  <EmojiPicker onEmojiSelect={handleEmojiClick} />
                </div>
              )}
            </div>

            <div className={`${styles.emojipickerinbox} ${openTools ? styles.active : ""}`}>
              <img src={textedit} alt="edit" onClick={openToolsHandler} />
            </div>
 <div className={`${styles.emojipickerinbox} `}>
            <img src={FileAttach} alt="attach" onClick={() => fileRef.current?.click()}/></div>
            <input
              type="file"
              multiple
              accept={`${[...pdfFormats, ...imageFormat, ...wordFormats, ...excelFormats,].join(",")}`}
              ref={fileRef}
              style={{ display: "none" }}
              onChange={handleFileChange}
            />

           {false &&  <div style={{ position: "relative" }}>

              <button className={`${styles.aiwritebtn} ${openAi && styles.active}`} onClick={() => setOpenAi(!openAi)}>
                <img src={ai} alt="attach"></img> Ai write
              </button>

              {openAi &&
                <div className={styles.suggestionboxai}>
                  <div className={styles.header}>
                    <div className={styles.lefts}>
                      <img src={ai} alt="attach"></img>
                      Ai Reply recommendation
                    </div>
                    <div className={styles.cancelbtn} onClick={() => setOpenAi(false)}>
                      <svg xmlns="http://www.w3.org/2000/svg" width="15" height="15" viewBox="0 0 15 15" fill="none">
                        <path d="M8.5064 7.3057L14.3614 1.4502C14.6934 1.1191 14.6934 0.5801 14.3614 0.249C14.0294 -0.083 13.4903 -0.083 13.1602 0.249L7.3052 6.1044L1.4502 0.249C1.1201 -0.083 0.5791 -0.083 0.249 0.249C-0.083 0.5801 -0.083 1.1191 0.249 1.4502L6.104 7.3057L0.249 13.1612C-0.083 13.4923 -0.083 14.0313 0.249 14.3624C0.415 14.5284 0.6318 14.6114 0.8496 14.6114C1.0674 14.6114 1.2851 14.5284 1.4502 14.3624L7.3052 8.507L13.1602 14.3624C13.3252 14.5284 13.543 14.6114 13.7608 14.6114C13.9786 14.6114 14.1954 14.5284 14.3614 14.3624C14.6934 14.0313 14.6934 13.4923 14.3614 13.1612L8.5064 7.3057Z" fill="#2D2B2F" />
                      </svg>
                    </div>
                  </div>
                  <div className={styles.body}>
                    <div className={styles.geninptblock}>
                      <label> Tell AI what email you want to generate</label>
                      <div className={styles.inputwithbutton}>
                        <input
                          type="text"
                          placeholder="Write"
                          value={aiPrompt}
                          onChange={(e) => setAiPrompt(e.target.value)}
                        />

                        <button className="primarybtn" onClick={handleGenerate}>
                          {isGenerating ? "Generating..." : "Generate"}
                        </button>
                      </div>
                    </div>
                    <div className={styles.sliderblck}>
                      <div>
                        {
                          isInitialLoading
                            ? "Generating suggestion..."
                            : aiSuggestions.length > 0
                              ? `Suggestion ${currentSuggestionIndex + 1} of ${aiSuggestions.length}`
                              : ""
                        }
                      </div>
                      <div>
                        {aiSuggestions.length > 0 && <div className={styles.leftrightchevrons}>
                          <span
                            onClick={handlePrev}
                            className={
                              aiSuggestions.length <= 1 || currentSuggestionIndex === 0
                                ? styles.disabled
                                : ""
                            }
                          >
                            <svg xmlns="http://www.w3.org/2000/svg" width="6" height="12" viewBox="0 0 6 12" fill="none">
                              <path d="M5.0556 11.331C4.8642 11.331 4.6723 11.2579 4.5258 11.1123L0.6611 7.25875C0.2353 6.83395 0.000500797 6.26855 7.96571e-07 5.66595C-0.000499203 5.06335 0.2344 4.49795 0.6611 4.07125L4.5263 0.21875C4.8193 -0.0732495 5.2944 -0.0732492 5.5868 0.220751C5.8793 0.513751 5.8783 0.988351 5.5848 1.28125L1.7201 5.13375C1.578 5.27535 1.4999 5.46485 1.4999 5.66595C1.4999 5.86615 1.578 6.05465 1.7201 6.19625L5.5853 10.0498C5.8783 10.3428 5.8792 10.8174 5.5868 11.1103C5.4403 11.2578 5.2479 11.331 5.0556 11.331Z" fill="black" />
                            </svg>
                          </span>
                          {`${currentSuggestionIndex + 1} / ${aiSuggestions.length}`}
                          <span
                            onClick={handleNext}
                            className={
                              aiSuggestions.length <= 1 ||
                                currentSuggestionIndex === aiSuggestions.length - 1
                                ? styles.disabled
                                : ""
                            }
                          >
                            <svg xmlns="http://www.w3.org/2000/svg" width="6" height="12" viewBox="0 0 6 12" fill="none">
                              <path d="M0.749951 11.3309C0.557551 11.3309 0.365152 11.2577 0.218752 11.1102C-0.0737485 10.8172 -0.0727486 10.3426 0.220751 10.0497L4.08505 6.19718C4.22715 6.05558 4.30525 5.86608 4.30525 5.66498C4.30525 5.46478 4.22715 5.27628 4.08505 5.13468L0.220352 1.28118C-0.0726482 0.988176 -0.0735484 0.513576 0.218852 0.220676C0.510852 -0.0732242 0.985952 -0.0732245 1.27985 0.218675L5.14415 4.07218C5.56995 4.49698 5.80475 5.06238 5.80525 5.66498C5.80575 6.26758 5.57085 6.83298 5.14415 7.25968L1.27945 11.1122C1.13345 11.2577 0.941351 11.3309 0.749951 11.3309Z" fill="black" />
                            </svg>
                          </span>
                        </div>}
                      </div>
                    </div>

                    {/* { !(isforwardmail || isreplaymail) &&
                      <div className={styles.dynamicsubjectblock} dangerouslySetInnerHTML={{ __html: currentSuggestion?.subject || '' }} />} */}

                    {aiSuggestions.length > 0 && <div
                      className={styles.dynamiccontentblock}
                      dangerouslySetInnerHTML={{
                        __html:  `${currentSuggestion?.subject ? `<div> <span style={{ fontWeight: "bold" }}>Subject:</span> ${currentSuggestion?.subject}</div>` : ''}<div>${currentSuggestion?.body || ''}</div>`}} 
                    />}

                  </div>
                  <div className={styles.footer}>
                    <div className={styles.btncontainer}>

                      <button onClick={handleRegenerate} disabled={!aiSuggestions.length}> <span style={{ display: "grid" }} >
                        <svg xmlns="http://www.w3.org/2000/svg" width="12" height="13" viewBox="0 0 12 13" fill="none">
                          <path d="M11.5381 5.68847C11.2875 5.68847 11.0844 5.89167 11.0844 6.1422C11.0844 8.94793 8.80187 11.2307 5.99587 11.2307C3.18987 11.2307 0.9076 8.94787 0.9076 6.1422C0.9076 3.33653 3.19013 1.05367 5.99587 1.05367C6.92533 1.05367 7.8196 1.31453 8.60847 1.78907L7.95113 1.80133C7.62033 1.8078 7.35713 2.0812 7.36327 2.412C7.3694 2.73927 7.6364 3.0004 7.9622 3.0004H7.97393L9.74127 2.967C10.0721 2.96053 10.3353 2.68713 10.3291 2.35633L10.2957 0.588734C10.2896 0.257335 10.0121 -0.0107987 9.68507 0.000334615C9.35427 0.00680128 9.09107 0.280201 9.0972 0.611001L9.10507 1.03053C8.16773 0.459068 7.1024 0.146135 5.9958 0.146135C2.6898 0.146135 0 2.83593 0 6.14227C0 9.4486 2.6898 12.1383 5.9958 12.1383C9.3018 12.1383 11.9919 9.44853 11.9919 6.14227C11.9919 5.89167 11.7887 5.68847 11.5381 5.68847Z" fill="#404040" />
                        </svg></span>
                        {isRegenerating ? "Rephrasing..." : "Rephrase"}
                      </button>

                      <button className="primarybtn" disabled={!aiSuggestions.length} onClick={handleInsertReply}>Insert reply</button>

                    </div>
                  </div>
                </div>}
            </div>}

          <div style={{position:"relative" , width:"100%"}}>
              <AIWriteEditor
                initialText={aiwriteInitialText}
                loading={aiwriteLoading || isRegenerating}
                onAIWrite={(text: string) => handleAIWriteSparkle(text)}
                onRegenerate={() => handleRegenerate()}
                // onAccept={() => handleInsertReply()}
              />
          </div>

          </div>

          <div className={styles.rightActions}>
            <button className={"secondarybtn"} onClick={()=> setopenConfirmDiscard(true)}>Discard</button>
            <button onClick={sendEmail}>Send</button>
          </div>
        </div>
      </div>
      {
        false &&
        <Modal
          header={<div className={"modalheader"}> <div> Move to Trash?</div></div>}
          children={<div className={"modalcontent"}>Are you sure you want to move this conversations to Trash?</div>}
          btntxtsecondary={"Cancel"}
          btntxtprimary={"Trash"}
        />
      }
      {
        openConfirmDiscard &&
        <Modal
          header={<div className={"modalheader"}> <div> Discard mail</div></div>}
          children={<div className={"modalcontent"}>Are you sure you want to Discard this mail?</div>}
          btntxtsecondary={"Cancel"}
          btntxtprimary={"Discard"}
            onclosePrimary={()=>  discardHandler()}
            oncloseSecondary={()=> setopenConfirmDiscard(false)}
        />
      }

      {subjectChangeModal.open && (
        <Modal
          header={<div className={"modalheader"}><div>Do you want changes</div></div>}
          onClose={dismissSubjectChange}
          children={<div className={`modalcontent ${styles.planupgrademodal}`}>
            Are you sure you want to Change subject of this mail?
            <ul>Previous: {subjectChangeModal.prevSubject}</ul>
            <ul>New: {subjectChangeModal.newSubject}</ul>
          </div>}
          btntxtsecondary={"Cancel"}
          btntxtsecondary2={"Keep subject"}
          btntxtprimary={"Change"}
          onclosePrimary={confirmSubjectChange}
          oncloseSecondary={dismissSubjectChange}
        />
      )}

    </div>

  );
};

export default ComposeMail;
