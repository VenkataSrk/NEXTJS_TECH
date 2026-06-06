
import styles from "./scss/newonboard.module.scss";
// import logo from "../../assets/images/fulllogo.svg";
import logo from "./Archive/fulllogo.svg";

import step1 from "../step1.svg";
// import step1 from "../../assets/images/onboard/step1.svg";
// import step2 from "../../assets/images/onboard/step2.svg";
import step2 from "../step2.svg";
// import step3 from "../../assets/images/onboard/step3.svg";
// import step4 from "../../assets/images/onboard/step4.svg";
// import step5 from "../../assets/images/onboard/step5.svg";
// import step6 from "../../assets/images/onboard/step6.svg";
// import step7 from "../../assets/images/onboard/step7.svg";
// import step8 from "../../assets/images/onboard/step8.svg";
// import step9 from "../../assets/images/onboard/step9.svg";
import OnBoardEditor from "./onboardeditor";
import { useRef, useState, useEffect, useLayoutEffect } from "react";

import { v4 as uuidv4 } from "uuid";
import { useSelector } from "react-redux";
import { useDispatcher } from "../../../store/redux-store/dispatch";
import { GetItemFromStorage } from "../../../base/customhooks/useStorage";
import { LocalStorage } from "../../../base/customhooks/localStorageKeys";
import { jwtDecode } from "jwt-decode";
import SocketService from "../../../base/service/socketService";
import { knowledgeDataSliceActions } from "../../../store/slice/knowledgeBaseChat";
import { whatsappService } from "../../../base/service/apiservice";
import ProgressBar from "../../custom_components/progressbar/progressbar";
import { useLocation, useNavigate } from "react-router-dom";
import { config } from "../../../base/constant";
import { commonDataSliceActions } from "../../../store/slice/commonSlice/commonSlice";

export default function OnBoardLayout() {
      const location = useLocation();
      const { state } = location;
    const companyonboarded =useSelector((state :any)=>state.commonData.companyOnboarded);
    const configDone =useSelector((state :any)=>state.commonData.configDone);
    const integrationDone =useSelector((state :any)=>state.commonData.integrationDone);
    console.log("company",companyonboarded)
    // const whatsappConfig =useSelector((state :any)=>state.commonData.whatsappConfig);
    const whatsappConfig = useSelector((state :any)=>state.commonData.whatsappConfig);

    // const [whatsappConfig,setWhatsAppConfig] = useState<any>(false);
    console.log("whatsappconfig",whatsappConfig);
    const boxRef = useRef(null);
    const [height, setHeight] = useState(0);

    const MAX_HEIGHT = 200;
    // const MAX_HEIGHT = 6;
    const MIN_HEIGHT = 40;
    // const MIN_HEIGHT = 24;

    // const sessionId = uuidv4();
    const sessionRefId = useRef(uuidv4());
    const navigate = useNavigate();

    const [onboardStep,setOnboardStep] = useState<any>(1);
    const textareaRef = useRef<HTMLTextAreaElement>(null);
    console.log("tctref",textareaRef)
    const fileInputRef = useRef<HTMLInputElement | null>(null);
    const bottomRef = useRef<HTMLDivElement | null>(null);


    const knowledgeMessageData = useSelector((state: any) => state.knowledgeBaseData);
    const dispatch = useDispatcher()
    const [showSend, setShowSend] = useState(false);
    const userDetails = GetItemFromStorage(LocalStorage?.ACCESS_TOKEN);
    const data: any = jwtDecode(userDetails);
    const [knowledgeMessage, setKnowledgeMessage] = useState('');
console.log("knlmsg",knowledgeMessage)
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

    useLayoutEffect(() => {
        if (!bottomRef.current) return;
        bottomRef.current?.scrollIntoView({ behavior: 'smooth', block: "end" });

    }, [knowledgeMessageData?.knowledgeMessage?.length]);

    useEffect(() => {
        try {

            dispatch(knowledgeDataSliceActions.reset());
            if(!companyonboarded && !state?.data && onboardStep!=3){

                const init = setTimeout(() => {
                const newMsg = {
                    sessionId:sessionRefId.current,
                    messageId: uuidv4(),
                    message: 'knowledgebase configuration',
                    sender: "userText",
                    date: new Date().toISOString(),
                    domainId: data?.domainId,
                    ext: +(data?.ext),
                    token:userDetails
                };
                SocketService.knowledgeMessage(newMsg);
    
    
                }, 100);
                return () => clearTimeout(init);
            }
            else if(state?.data ||onboardStep==3){
                const init = setTimeout(() => {
                const newMsg = {
                    sessionId: sessionRefId.current,
                    messageId: uuidv4(),
                    message: 'Integration',
                    sender: "userText",
                    date: new Date().toISOString(),
                    domainId: data?.domainId,
                    ext: +(data?.ext),
                    token:userDetails
                };
                SocketService.knowledgeMessage(newMsg);
            }, 100);
                 return () => clearTimeout(init);
            }
            else{
                const init = setTimeout(() => {
                 const newMsg = {
                        sessionId:sessionRefId.current,
                        messageId: uuidv4(),
                        message: 'channel configuration',
                        sender: "userText",
                        date: new Date().toISOString(),
                        domainId: data?.domainId,
                        ext: +(data?.ext),
                        token:userDetails
                    };
                SocketService.knowledgeMessage(newMsg);
                }, 100);
                return () => clearTimeout(init);
            }

        } catch (error) {
            console.log("ERR", error)
        }
    }, [companyonboarded])

    const handleSend = () => {
        try {
            if (!knowledgeMessage.trim()) return;
            const newMsg = {
                sessionId:sessionRefId.current,
                messageId: uuidv4(),
                message: knowledgeMessage,
                sender: "userText",
                date: new Date().toISOString(),
                domainId: data?.domainId,
                ext: +(data?.ext),
                onboardactive:onboardStep==1?0:onboardStep==2?1:4,
                token:userDetails
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
                            sessionId:sessionRefId.current,
                            messageId: uuidv4(),
                            message: uploadUrl?.url,
                            sender: "userText",
                            date: new Date().toISOString(),
                            domainId: data?.domainId,
                            ext: +(data?.ext),
                            file: "file",
                            filename: uploadUrl?.filename,
                            mimetype: uploadUrl?.mimetype,
                            size: uploadUrl?.size,
                            token:userDetails
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
            console.log("API Error:", error)
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




    useEffect(() => {
        if (!boxRef.current) return;

        const observer = new ResizeObserver((entries) => {
            for (let entry of entries) {
                setHeight(Math.round(entry.contentRect.height));
            }
        });

        observer.observe(boxRef.current);

        return () => observer.disconnect();
    }, []);


        useEffect(() => {
        const timer = setTimeout(() => {
          const loaders = document.getElementById('contact_center_mainloader');
          if (loaders) {
            loaders.style.display = 'none';
          }
        }, 1000); // 1 second
      
        // cleanup → remove timer
        return () => clearTimeout(timer);
      }, []);


      useEffect(()=>{
        try {
            dispatch(knowledgeDataSliceActions.setknowledgeMessage([]));
            console.log("company onboard",companyonboarded)
            if(companyonboarded){
              
                setOnboardStep(2);
                
               
            }
        } catch (error) {
            console.log("err:",error)
        }
      },[companyonboarded])

      useEffect(()=>{
        if(configDone){
            if(userDetails?.domain_id=="20667"){
                navigate('/home')
            }else{

                setTimeout(()=>setOnboardStep(3),10);
                dispatch(commonDataSliceActions.setConfigDone(false));
            }
            
        }
      },[configDone])

      useEffect(()=>{
        if(integrationDone){

            navigate('/home')
        }
      },[integrationDone])

       useEffect(()=>{
        console.log("state",state)
        if(state?.data){
             navigate(location.pathname, { replace: true, state: {} });
             setTimeout(()=>setOnboardStep(3),10);
        }
      },[])

    

    //   whatsapp config

const [loader,setLoader]= useState<any>(false);
  
  const wabaIdRef = useRef<string | null>(null);
  const businessIdRef = useRef<string | null>(null);
  const phoneNumberIdRef = useRef<string | null>(null);

  const loginUserDetails :any = jwtDecode(GetItemFromStorage(LocalStorage.ACCESS_TOKEN));
   const launchWhatsAppSignup = () => {
     // console.log("here comes");
     try {
       setLoader(true);
     
        (window as any).FB.login(
       (response: any) => {
         // setLoader(true);
         if (response.authResponse) {
           const code = response.authResponse.code;
           window.location.href = `${config().BUSINESS_WHATSAPP_CONF_URL}?code=${code}&domainId=${loginUserDetails?.domainId}&domainCountry=${loginUserDetails?.company_country??"United Kingdom"}` +
             `${wabaIdRef.current ? `&wabaId=${wabaIdRef.current}` : ''}` +
             `${businessIdRef.current ? `&businessId=${businessIdRef.current}` : ''}` +
             `${phoneNumberIdRef.current ? `&phoneNumberId=${phoneNumberIdRef.current}` : ''}`;
         }
       },
       {
         config_id: "1109736967317755",
         response_type: "code",
         override_default_response_type: true,
         extras: {
           setup: {},
           featureType: "whatsapp_business_app_onboarding",
           sessionInfoVersion: "3",
         },
       }
     );
     } catch (error :any) {
       setLoader(false);
       console.log("err",error)
     }
     finally{
       setLoader(false);
     }
    
   };

   const handleStopResponse=()=>{
        const newMsg = {
                sessionId:sessionRefId.current,
                messageId: uuidv4(),
                message: "",
                sender: "userText",
                date: new Date().toISOString(),
                domainId: data?.domainId,
                ext: +(data?.ext),
                onboardactive:onboardStep==1?0:1,
                token:userDetails
            };
            
            SocketService.knowledgeMessage(newMsg);
   }
 
     useEffect(() => {
        console.log("sdk call")
     // Load the Facebook SDK
     try {
        
         if(whatsappConfig && onboardStep==2){
    
             const loadFbSdk = () => {
               (window as any).fbAsyncInit = function () {
                 (window as any).FB.init({
                   appId: "2792648964217350",
                   autoLogAppEvents: true,
                   xfbml: true,
                   version: "v22.0",
                 });
               };
               
               const script = document.createElement("script");
               script.src = "https://connect.facebook.net/en_US/sdk.js";
               script.async = true;
               script.defer = true;
               script.crossOrigin = "anonymous";
               document.body.appendChild(script);
             };
         
             loadFbSdk();
            
         const timer =setTimeout(()=>{
        
             if ((window as any).FB) {
                        launchWhatsAppSignup();
                        clearTimeout(timer);
                        return;
                    }
         },2000)
        
             const handleReceiveEmbeddedSignupData = ({ waba_id, business_id, phone_number_id }: { waba_id: string, business_id: string, phone_number_id: string }) => {
               wabaIdRef.current = waba_id;
               businessIdRef.current = business_id;
               phoneNumberIdRef.current = phone_number_id;
             };
         
             // Handle messages from Facebook
             const handleMessage = (event: MessageEvent) => {
               if (
                 event.origin !== "https://www.facebook.com" &&
                 event.origin !== "https://web.facebook.com"
               ) {
                 return;
               }
         
               try {
                 const data = JSON.parse(event.data);
                 if (data.type === "WA_EMBEDDED_SIGNUP") {
                   if (data.event === "FINISH_WHATSAPP_BUSINESS_APP_ONBOARDING") {
                     console.log(
                       "Phone number ID:",
                       data.data.phone_number_id,
                       "WhatsApp business account ID:",
                       data.data.waba_id,
                       "Business ID:",
                       data.data.business_id
                     );
                     const waba_id = data.data.waba_id;
                     const business_id = data.data.business_id;
                     const phone_number_id = data.data.phone_number_id;
                     handleReceiveEmbeddedSignupData({ waba_id, business_id, phone_number_id });
                   } else if (data.event === "FINISH") {
                     console.log(
                       "Phone number ID:",
                       data.data.phone_number_id,
                       "WhatsApp business account ID:",
                       data.data.waba_id,
                       "Business ID:",
                       data.data.business_id
                     );
                     const waba_id = data.data.waba_id;
                     const business_id = data.data.business_id;
                     const phone_number_id = data.data.phone_number_id;
                     handleReceiveEmbeddedSignupData({ waba_id, business_id, phone_number_id });
                   } else if (data.event === "CANCEL") {
                     console.warn("Cancel at:", data.data.current_step);
                   } else if (data.event === "ERROR") {
                     console.error("Error:", data.data.error_message);
                   }
                 }
               } catch {
                 console.log("Non JSON Responses", event.data);
               }
               
             };
         
             window.addEventListener("message", handleMessage);
           
               
    
             return () => {
               window.removeEventListener("message", handleMessage);
            //    setWhatsAppConfig(false); 
              dispatch(commonDataSliceActions.setWhatsAppConfig(false));
             };
            
         }

     } catch (error :any) {
        console.log("err:",error)
     }
   
   }, [whatsappConfig]);




    return (

        <>
{/* <p> {`${loader}`}</p> */}

            <div className={styles.mainlayout}>
                <div className={styles.mainlogo}>
                    <img src={logo} alt="" />
                </div>

                <div className={styles.mainblock}>
                    <div className={styles.mainblockleft}>
                        <div className={styles.mainblocklefttop}>
                            <div className={styles.titlearea}>
                                <div className={styles.titlearealeft}>Setup Journey<span>Follow the steps to complete your setup.</span></div>
                                {
                                // onboardStep==2 && 
                                <div className={styles.titlearearight} onClick={()=>navigate('/home')}>Skip</div>
                                
                                }
                            </div>
                        </div>
                        <div className={styles.mainblockleftmid} style={{height: `calc(100dvh - ${height}px - 196px)`}}>
                            <div className={styles.bubbleblock}>
                                {Array.isArray(knowledgeMessageData?.knowledgeMessage) && knowledgeMessageData?.knowledgeMessage?.map((msg: any) => {
                                if (msg.sender === "userText") {
                                    return (
                                        <>
                                            {msg?.file ? (
                                                 <div className={styles.bubbleblockagent}>
                                                    <div className={styles.bubbleblockagenttextblock}>
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
                                                </div></div>
                                            ) : (

                                                <div className={styles.bubbleblockagent}>
                                                    <div className={styles.bubbleblockagenttextblock}>
                                                        <div className={styles.bubbleblockagenttext} dangerouslySetInnerHTML={{ __html: msg?.message }} />

                                                    </div>
                                                </div>)}</>)
                                }
                                if(msg.sender ==="botText"){
                                    return (<>
                                        {msg?.progress && (msg?.progress) < "100" ?
                               
                                            <>
                                                <div className={`${styles.msgbubble} ${styles.left} ${styles.containprogress}`}>
                                                    <p>{msg?.progress_message}</p>
                                                    <p>Scraping in Progress...</p>
                                                    {/* <ProgressBar progress={"hmgfdd"} /></div>  */}
                                                    <ProgressBar progress={msg?.progress} /></div> 
                                            </> 
                                                    :
                                                <>
                                                    <div className={styles.bubbleblockcustomer}>
                                                        <div className={styles.bubbleblockcustomertextblock}>
                                                            <div className={styles.bubbleblockcustomertext} dangerouslySetInnerHTML={{ __html: msg?.bot_response }} />
                                                            {/* <div className={styles.bubbleblockcustomertext} dangerouslySetInnerHTML={{ __html: "kjgvgj" }} /> */}
                                                        </div>
                                                    </div>
                                                </>
                                               
                                                 }


                                   </> )
                                    
                                } 
                                })}
                            </div>
                                  {/* <>
                                            <>
                                                <div className={`${styles.msgbubble} ${styles.left} ${styles.containprogress}`}>
                                                    
                                                    <p>Scraping in Progress...</p>
                                                    <ProgressBar progress={"hmgfdd"} /><span onClick={()=>handleStopResponse()}>x</span></div> 
                                                   
                                            </> 
                                                   
                                                <>
                                                    <div className={styles.bubbleblockcustomer}>
                                                        <div className={styles.bubbleblockcustomertextblock}>
                                                            
                                                            <div className={styles.bubbleblockcustomertext} dangerouslySetInnerHTML={{ __html: "kjgvgj" }} />
                                                        </div>
                                                    </div>
                                                </>
                                                </> */}
                            {knowledgeMessageData?.isTyping && (
                                <div className={styles.genmaincontainer}>
                                <p className="generatings">
                                    {/* AI is Thinking */}
                                    Agent is Analysing
                                    <div className='typingdot'><span></span><span></span><span></span></div>
                                </p>
                                </div>
                                  )}
                            <div ref={bottomRef} />
                        </div>
                         
                        <div className={styles.mainblockleftbottom} ref={boxRef}>
                            <OnBoardEditor textareaRef={textareaRef} handleSend={handleSend} handleInput={handleInput} knowledge knowledgeMessage={knowledgeMessage} setKnowledgeMessage={(e :any)=>setKnowledgeMessage(e)} knowledgeMessageData={knowledgeMessageData} showSend={showSend} uploadWhatsappTemplate={uploadWhatsappTemplate}/>
                        </div>
                    </div>
                    
                    <div className={styles.mainblockright} >
                        {data?.domain_id==20667?
                        <>
                        {onboardStep==1?
                        <div className={styles.mainblockrightblock}>
                            <div className={styles.title}>Your Setup Path</div>
                            <div className={styles.subtitle}><span>Step 1 of 2</span>Knowledge base Setup</div>
                            <p style={{margin:"12px 0 20px 0"}}>A brief description explaining the purpose of the Knowledge base setup and what users need to do to complete it.</p>
                            <img src={step1} alt="" />
                        </div>
:
                        <div className={styles.mainblockrightblock}>
                            <div className={styles.title}>Your Setup Path</div>
                            <div className={styles.subtitle}><span>Step 2 of 2</span>Channel Configuration</div>
                            <p style={{margin:"12px 0 20px 0"}} className={styles.description}>“Short description on Channel Configuration about what should users going to accomplish and what they need to do for such things”</p>
                            <img src={step2} alt="" />
                        </div>}
                        </>:
                        <>


                            {onboardStep == 1 ?
                                <div className={styles.mainblockrightblock}>
                                    <div className={styles.title}>Your Setup Path</div>
                                    <div className={styles.subtitle}><span>Step 1 of 3</span>Knowledge base Setup</div>
                                    <p style={{ margin: "12px 0 20px 0" }}>A brief description explaining the purpose of the Knowledge base setup and what users need to do to complete it.</p>
                                    <img src={step1} alt="" />
                                </div>
                                : onboardStep == 2 ?
                                    <div className={styles.mainblockrightblock}>
                                        <div className={styles.title}>Your Setup Path</div>
                                        <div className={styles.subtitle}><span>Step 2 of 3</span>Channel Configuration</div>
                                        <p style={{ margin: "12px 0 20px 0" }} className={styles.description}>“Short description on Channel Configuration about what should users going to accomplish and what they need to do for such things”</p>
                                        <img src={step2} alt="" />
                                    </div>
                                    :
                                    <div className={styles.mainblockrightblock}>
                                        <div className={styles.title}>Your Setup Path</div>
                                        <div className={styles.subtitle}><span>Step 3 of 3</span>Integration</div>
                                        {/* <p style={{margin:"12px 0 20px 0"}} className={styles.description}>“Short description on Integration about what should users going to accomplish and what they need to do for such things”</p> */}
                                        <p style={{ margin: "12px 0 20px 0" }} className={styles.description}>“A brief description explaining the purpose of Integrations and what users need to do to complete it.”</p>
                                        <img src={step2} alt="" />
                                    </div>
                            }
                        </>}
                    </div>
                </div>
            </div>
        </>
    );
}
