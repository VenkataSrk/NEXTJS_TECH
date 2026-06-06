import React, { useEffect, useLayoutEffect, useRef, useState } from 'react'
import styles from "./scss/chatcomponent.module.scss"
import { assets } from '../../styles/assets/assets'
import ProgressBar from '../custom_components/progressbar/progressbar'
import SocketService from '../../base/service/socketService';
import { v4 as uuidv4 } from "uuid";
import { GetItemFromStorage } from '../../base/customhooks/useStorage';
import { LocalStorage } from '../../base/customhooks/localStorageKeys';
import { jwtDecode } from 'jwt-decode';
import { useSelector } from 'react-redux';
import { knowledgeDataSliceActions } from '../../store/slice/knowledgeBaseChat';
import { useDispatcher } from '../../store/redux-store/dispatch';
import WorktualModal from '../custom_components/worktualModal';
import { whatsappService } from '../../base/service/apiservice';


const MAX_HEIGHT = 200;
const MIN_HEIGHT = 40;
const sessionId = uuidv4();

function Chatcomponent({ headerTitle = "Default Title", rightIcon, isOpen }: any) {
  const textareaRef = useRef<HTMLTextAreaElement>(null);
  const fileInputRef = useRef<HTMLInputElement | null>(null);
  const bottomRef = useRef<HTMLDivElement | null>(null);

  const handleInput: React.FormEventHandler<HTMLTextAreaElement> = (e) => {
    const el = e.currentTarget;
    el.style.height = "auto";
    if (el.value.trim() === "") {
      el.style.height = `${MIN_HEIGHT}px`;
    } else {
      el.style.height = `${Math.min(el.scrollHeight, MAX_HEIGHT)}px`;
    }
    setShowSend(el.value.trim() !== "");
  };
  const knowledgeMessageData = useSelector((state: any) => state.knowledgeBaseData);
  const dispatch = useDispatcher()
  const [showSend, setShowSend] = useState(false);
  const userDetails = GetItemFromStorage(LocalStorage?.ACCESS_TOKEN);
  const data: any = jwtDecode(userDetails);
  const [knowledgeMessage, setKnowledgeMessage] = useState('');

  useLayoutEffect(() => {
    if (!bottomRef.current) return;
    bottomRef.current?.scrollIntoView({ behavior: 'smooth', block: "end" });

  }, [knowledgeMessageData?.knowledgeMessage?.length]);
  
  useEffect(() => {
    const newMsg = {
      sessionId,
      messageId: uuidv4(),
      message: '',
      sender: "userText",
      date: new Date().toISOString(),
      domainId: data?.domainId,
      ext: +(data?.ext),
    };
    SocketService.knowledgeMessage(newMsg);
  }, [])

  const handleSend = () => {
    try {
      if (!knowledgeMessage.trim()) return;
      const newMsg = {
        sessionId,
        messageId: uuidv4(),
        message: knowledgeMessage,
        sender: "userText",
        date: new Date().toISOString(),
        domainId: data?.domainId,
        ext: +(data?.ext),
      };
      dispatch(knowledgeDataSliceActions.setIsTyping(true));
      const temp_data: any = [...knowledgeMessageData?.knowledgeMessage]
      temp_data.push(newMsg)
      dispatch(knowledgeDataSliceActions.setknowledgeMessage(temp_data));
      SocketService.knowledgeMessage(newMsg);
      setKnowledgeMessage("");
      if (textareaRef.current) textareaRef.current.style.height = `${MIN_HEIGHT}px`;
    } catch (error) {
      console.log(error)
    }
  };

  const uploadWhatsappTemplate = (event: React.ChangeEvent<HTMLInputElement>) => {
    try {
      const file = event.target.files?.[0];
      if (!file) return;
      const maxFileSizeInBytes = 5 * 1024 * 1024;
      if (file.size > maxFileSizeInBytes) {
        alert("File size exceeds 5MB. Please choose a smaller file.");
        return;
      }
      const formData = new FormData();
      formData.append("doc", file);
      whatsappService
        .post(formData, `/knowledge_base_upload`)
        .then((res: any) => {
          if (res?.data?.statusCode === 200) {
            const uploadUrl = res.data.fileList?.[0];
            const newMsg = {
              sessionId,
              messageId: uuidv4(),
              message: uploadUrl?.url,
              sender: "userText",
              date: new Date().toISOString(),
              domainId: data?.domainId,
              ext: +(data?.ext),
              file: "file",
              filename: uploadUrl?.filename,
              mimetype: uploadUrl?.mimetype,
              size: uploadUrl?.size
            };
            dispatch(knowledgeDataSliceActions.setIsTyping(true));
            const temp_data: any = [...knowledgeMessageData?.knowledgeMessage]
            temp_data.push(newMsg)
            dispatch(knowledgeDataSliceActions.setknowledgeMessage(temp_data));
            SocketService.knowledgeMessage(newMsg);
            setKnowledgeMessage("");
          } else {
            alert("File upload failed. Please try again.");
          }
        })
        .catch((error: any) => {
          console.error("Upload error:", error);
          alert("An error occurred while uploading the file.");
        });
      
    } catch (error) {
        console.log("API Error:",error)
    }
  };

  const formatFileSize = (bytes?: number) => {
    if (!bytes) return "0 KB";
    const sizes = ["Bytes", "KB", "MB", "GB"];
    const i = Math.floor(Math.log(bytes) / Math.log(1024));
    const size = (bytes / Math.pow(1024, i)).toFixed(1);
    return `${size} ${sizes[i]}`;
  };

   const setIsOpenMode = () => {
      dispatch(knowledgeDataSliceActions.isModalOpen(false))
      dispatch(knowledgeDataSliceActions.reset());
    }

  return (
    <>
      <div className='closenonemdal chatcomponentmodal'>
        <WorktualModal
          show={isOpen}
          // show={false}
          mdsize='modalxxl'
        >
          <div className={styles.elevatebrandbody}>
            <div style={{ padding:"16px 32px" , display:"flex" , alignItems:"center", justifyContent:"space-between" , paddingTop:"0px" }} className={styles.header}>
              <div className={styles.lefts}>
                <h5>{headerTitle}</h5>
              </div>
              <button style={{}}  className='cancelbtn' onClick={setIsOpenMode}>
                   Close
              </button>
            </div>
            <div className={styles.innerbody}>
              <div className={styles.msgbody}>
                <div className={styles.scroller}>
                  <div className={styles.maincontainer}>
                    {Array.isArray(knowledgeMessageData?.knowledgeMessage) && knowledgeMessageData?.knowledgeMessage?.map((msg: any) => {
                      if (msg.sender === "userText") {
                        return (
                          <>
                            {msg?.file ? (
                              <a
                                href={msg?.message}
                                target="_blank"
                                className={`${styles.fileupload} ${styles.msgbubble} ${styles.right}`}
                              >
                                <div className={styles.lefts}>
                                  <svg xmlns="http://www.w3.org/2000/svg" width="30" height="38" viewBox="0 0 30 38" fill="none">
                                    <path d="M30 12.6836V36.0007C30 36.5311 29.7893 37.0397 29.4143 37.4147C29.0393 37.7898 28.5306 38.0005 28.0003 38.0005H1.99974C1.46946 38.0002 0.960971 37.7894 0.586006 37.4144C0.211041 37.0395 0.000266005 36.531 0 36.0007V1.99925C0 1.46889 0.210708 0.960234 0.58573 0.585211C0.960753 0.210189 1.46937 -0.000488192 1.99974 -0.000488192H17.317C18.3948 -0.000714333 19.4287 0.427186 20.1911 1.18911L28.8104 9.80843C29.5725 10.5712 30.0004 11.6054 30 12.6836Z" fill="#439CF3" />
                                    <path d="M9.04008 27.1032H11.9333L14.6026 17.9146H14.712L17.3924 27.1032H20.2876L24.3091 13.0488H21.0636L18.7376 22.8347H18.6131L16.0492 13.0488H13.2745L10.7075 22.8146H10.5911L8.26508 13.0488H5.01953L9.04008 27.1032Z" fill="white" />
                                  </svg>
                                </div>
                                <div className={styles.rights}>
                                  <h5>{msg?.filename || "Unknown File"}</h5>
                                  <p>
                                    {msg?.mimetype?.includes("pdf") ? "PDF" : msg?.mimetype?.includes("msword") ? "WORD" : "FILE"}
                                    {" • "}
                                    {formatFileSize(msg?.size)}
                                  </p>
                                </div>
                              </a>
                            ) : (
                              <div key={msg?.messageId} className={`${styles.msgbubble} ${styles.right}`}>
                                <div  className="chatContent" dangerouslySetInnerHTML={{ __html: msg?.message }} />
                              </div>
                            )}

                          </>
                        );
                      }
                      if (msg.sender === "botText") {
                        let textToShow = msg?.bot_response;
                        return (
                          <>
                            {msg?.progress && (msg?.progress) < "100" ?
                              <>
                                <div className={`${styles.msgbubble} ${styles.left} ${styles.containprogress}`}>
                                  <p>Scraping in Progress...</p>
                                  <ProgressBar progress={msg?.progress} /></div> </> :
                              <div key={msg.messageId} className={`${styles.msgbubble} ${styles.left}`}>
                                <div  className="chatContent" dangerouslySetInnerHTML={{ __html: textToShow }} />
                              </div>
                            }
                          </>
                        );
                      }
                      return null;
                    })}
                    {knowledgeMessageData?.isTyping && (
                      <p className="generatings">
                        {/* AI is Thinking */}
                        Agent is Analysing
                        <div className='typingdot'><span></span><span></span><span></span></div>
                      </p>
                    )}
                    <div ref={bottomRef} />
                  </div>
                </div>
                <div style={{ marginTop: "16px" }} className={`${styles.searchblock}`}>
                  <div className={styles.searchmessage}>
                    <div className={styles.searchmessageText}>
                      <input
                        type="file"
                        accept="application/pdf"
                        ref={fileInputRef}
                        style={{ display: "none" }}
                        onChange={uploadWhatsappTemplate}
                      />
                      <svg
                        xmlns="http://www.w3.org/2000/svg"
                        width="16"
                        height="16"
                        viewBox="0 0 16 16"
                        fill="none"
                        onClick={() => fileInputRef.current?.click()}
                        style={{ cursor: "pointer" }}
                      >
                        <path
                          d="M15.2515 7.24538L8.74468 7.24987V0.748017C8.74468 0.333985 8.40902 -0.00167847 7.99499 -0.00167847C7.58096 -0.00167847 7.2453 0.333985 7.2453 0.748017V7.25097L0.747509 7.25547C0.333482 7.25577 -0.00197814 7.59163 -0.00167827 8.00567C-0.00137839 8.4197 0.334482 8.75516 0.748509 8.75486L7.2453 8.75036V15.2522C7.2453 15.6663 7.58096 16.0019 7.99499 16.0019C8.40902 16.0019 8.74468 15.6663 8.74468 15.2522V8.74936L15.2525 8.74487C15.6665 8.74457 16.002 8.4087 16.0017 7.99467C16.0014 7.58064 15.6655 7.24508 15.2515 7.24538Z"
                          fill="#404040"
                        />
                      </svg>
                      <textarea
                        ref={textareaRef}
                        placeholder="Write your Goal & Objective"
                        className="textareamainsnd"
                        onInput={handleInput}
                        value={knowledgeMessage}
                        onChange={(e) => setKnowledgeMessage(e.target.value)}
                        onKeyDown={(e) => {
                          if (!knowledgeMessageData?.isTyping && !knowledgeMessageData?.progress && e.key === "Enter" && !e.shiftKey) {
                            e.preventDefault();
                            handleSend();
                          }
                        }}
                        rows={1}
                        style={{ minHeight: "40px", maxHeight: "120px", height: "40px" }}
                      />
                      {knowledgeMessageData?.isTyping || knowledgeMessageData?.progress ? (
                        <img src={assets.passicon} alt="typing" />
                      ) : showSend ? (
                        <img
                          src={assets.sndarrowchat}
                          alt="send"
                          onClick={handleSend}
                          style={{ cursor: "pointer" }}
                        />
                      ) : null}
                    </div>
                  </div>
                </div>
              </div>

            </div>
          </div>
        </WorktualModal>
      </div>
    </>
  )
}

export default Chatcomponent;