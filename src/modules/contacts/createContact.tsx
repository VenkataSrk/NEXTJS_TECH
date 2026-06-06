import React, { useEffect, useLayoutEffect, useRef, useState } from "react";
import styles from "../../components/chatcomponent/scss/chatcomponent.module.scss";
import { assets } from "../../styles/assets/assets";
import SocketService from "../../base/service/socketService";
import { v4 as uuidv4 } from "uuid";
import { GetItemFromStorage } from "../../base/customhooks/useStorage";
import { LocalStorage } from "../../base/customhooks/localStorageKeys";
import { jwtDecode } from "jwt-decode";
import { useSelector } from "react-redux";
import { useDispatcher } from "../../store/redux-store/dispatch";
import WorktualModal from "../../components/custom_components/worktualModal";
import { contactsSliceActions } from "../../store/slice/contacts";

const MAX_HEIGHT = 200;
const MIN_HEIGHT = 40;
const sessionId = uuidv4();
const CreateContact = ({ headerTitle = "Default Title", isOpen }: any) => {
  const textareaRef = useRef<HTMLTextAreaElement>(null);
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
  const contactMessageData = useSelector((state: any) => state.contactsData);
  const dispatch = useDispatcher();
  const [showSend, setShowSend] = useState(false);
  const userDetails = GetItemFromStorage(LocalStorage?.ACCESS_TOKEN);
  const data: any = jwtDecode(userDetails);
  const [message, setMessage] = useState("");

  const scrollerRef = useRef<HTMLDivElement>(null);

  useLayoutEffect(() => {
      if (!scrollerRef.current) return;
      scrollerRef.current?.scrollIntoView({ behavior: 'smooth', block: "end" });
  
    }, [contactMessageData?.contactsMessage?.length]);

  useEffect(()=>{
    const newMsg = {
        sessionId,
        messageId: uuidv4(),
        message: '',
        sender: "userText",
        date: new Date().toISOString(),
        domainId: data?.domainId,
        ext: +data?.ext,
    }; 
    SocketService.contactMessage(newMsg);
  },[])
  const handleSend = () => {
    try {
      if (!message.trim()) return;
      const newMsg = {
        sessionId,
        messageId: uuidv4(),
        message: message,
        sender: "userText",
        date: new Date().toISOString(),
        domainId: data?.domainId,
        ext: +data?.ext,
      };
      dispatch(contactsSliceActions.setIsTyping(true));
      const temp_data: any = [...contactMessageData?.contactsMessage];
      temp_data.push(newMsg);
      dispatch(contactsSliceActions.setContactsMessage(temp_data));
      SocketService.contactMessage(newMsg);
      setMessage("");
      if (textareaRef.current)
        textareaRef.current.style.height = `${MIN_HEIGHT}px`;
    } catch (error) {
      console.log(error);
    }
  };

  const setIsOpenMode = () => {
    dispatch(contactsSliceActions.setIsOpenContactModal(false))
  }

  return (
    <>
      <div className="closenonemdal chatcomponentmodal contactmodal">
        <WorktualModal show={isOpen} mdsize="modalxxl">
          <div className={styles.elevatebrandbody}>
            <div style={{ display:"flex" ,alignItems:"center", justifyContent:"space-between" , padding:"16px 32px" ,paddingTop:"0px"  }}  className={styles.header}>
              <div className={styles.lefts}>
                <h5>{headerTitle}</h5>
              </div>
              <button className="cancelbtn" onClick={setIsOpenMode}>
                  Close
              </button>
            </div>
            <div className={styles.innerbody}>
              <div className={styles.msgbody}>
                <div className={`${styles.scroller} `}>
                  <div className={styles.maincontainer}>
                  {Array.isArray(contactMessageData?.contactsMessage) &&
                    contactMessageData?.contactsMessage?.map((msg: any) => {
                      if (msg.sender === "userText") {
                        return (
                          <div
                            key={msg?.messageId}
                            className={`${styles.msgbubble} ${styles.right}`}
                          >
                            <div className="chatContent"
                              dangerouslySetInnerHTML={{ __html: msg?.message }}
                            />
                          </div>
                        );
                      }
                      if (msg.sender === "botText") {
                        let textToShow = msg?.bot_response;
                        return (
                          <div
                            key={msg.messageId}
                            className={`${styles.msgbubble} ${styles.left}`}
                          >
                            <div
                             className="chatContent"
                              dangerouslySetInnerHTML={{ __html: textToShow }}
                            />
                          </div>
                        );
                      }
                      return null;
                    })}
                  {contactMessageData?.isTyping && (
                    <p className="generatings">
                      {/* AI is Thinking */}
                      Agent is Analysing
                      <div className="typingdot">
                        <span></span>
                        <span></span>
                        <span></span>
                      </div>
                    </p>
                  )}
                  <div ref={scrollerRef} />
                </div>
                
              </div>
              <div
                  style={{ marginTop: "16px" }}
                  className={`${styles.searchblock}`}
                >
                  <div className={` ${styles.searchmessage}`}>
                    <div className={`${styles.searchmessageText}`}>
                      <svg
                        xmlns="http://www.w3.org/2000/svg"
                        width="16"
                        height="16"
                        viewBox="0 0 16 16"
                        fill="none"
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
                        value={message}
                        onChange={(e) => setMessage(e.target.value)}
                        onKeyDown={(e) => {
                          if (
                            !contactMessageData?.isTyping &&
                            e.key === "Enter" &&
                            !e.shiftKey
                          ) {
                            e.preventDefault();
                            handleSend();
                          }
                        }}
                        rows={1}
                        style={{
                          minHeight: "40px",
                          maxHeight: "120px",
                          height: "40px",
                        }}
                      />
                      {contactMessageData?.isTyping ? (
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
                    <></>
                  </div>
                </div>
            </div>
          </div>
          </div>
        </WorktualModal>
           <WorktualModal show={false}  >
            <>
            <div className={styles.modbody}>
                <h3>Discard Contact Creation?</h3>
                <p>If you close this now, anything you’ve entered will be cleared. Want to continue or stay and finish?</p>
            </div>
            <div className={styles.footer}>
               <div className={styles.lefts}>
                  <button className="cancelbtn" >
                  Close
              </button>
               </div>
            
              <div className={styles.rights}>
               <button className="cancelbtn" >
                  Resume later
              </button>
               <button className="cancelbtn disconnect" >
                  Discard
              </button>
              </div>
           
                    
            </div>
            </>
              
           </WorktualModal>
      </div>
    </>
  );
};

export default CreateContact;
