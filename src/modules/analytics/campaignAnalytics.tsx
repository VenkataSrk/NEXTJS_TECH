import React, { useCallback, useEffect, useLayoutEffect, useRef, useState } from 'react'
import styles from "../home/campaign/scss/elevatebrand.module.scss"
import { assets, leftnavassets } from '../../styles/assets/assets';
import ProgressBar from '../../components/custom_components/progressbar/progressbar'
// import ProgressBar from '../../../components/custom_components/progressbar/progressbar'
import WorktualModal from '../../components/custom_components/worktualModal'
import { v4 as uuidv4 } from "uuid";
import { GetItemFromStorage } from '../../base/customhooks/useStorage'
import { LocalStorage } from '../../base/customhooks/localStorageKeys'
import { jwtDecode } from 'jwt-decode'
import { useDispatcher } from '../../store/redux-store/dispatch'
import { campaignSliceActions } from '../../store/slice/campaign'
import { useSelector } from 'react-redux'
import SocketService from '../../base/service/socketService'
import { format } from "date-fns";
import WorktualDropdown from '../../components/custom_components/WroktualDropdown'
import ClickTooltip from '../../components/custom_components/clickabletooltip/clickabletooltip'
import { whatsappService } from '../../base/service/apiservice'
import { useLocation, useNavigate, useSearchParams } from 'react-router-dom'
import { decryptAESCVM, downsampleBuffer, floatTo16BitPCM } from '../../base/utils'
import Loaders from '../../components/loader/loader'
import { config } from '../../base/constant'
import VariantDropdown from '../home/campaign/variantdropdown'
// import VariantDropdown from './variantdropdown'
import { commonDataSliceActions } from '../../store/slice/commonSlice/commonSlice'
import NlpWebSocketService from '../../base/service/sttNlpSocket';
import { analyticsSliceActions } from '../../store/slice/analytics';


const MAX_HEIGHT = 200;
const MIN_HEIGHT = 40;
const sessionId = uuidv4();

// function CampaignAnalytics({ isCVM, campaignId, isNewChat }: any) {
//   const location = useLocation();
//   let navigate = useNavigate();
//   const { state } = location;
//   const campaignMessageData = useSelector((state: any) => state.campaignData);
//   console.log("campmsgdata",campaignMessageData)
//   const dispatch = useDispatcher()
//   const textareaRef = useRef<HTMLTextAreaElement>(null);
//   const bottomRef = useRef<HTMLDivElement | null>(null);
//   const nlpSocketUrl: any = config().STT_NLP;
//   const audioContextRef = useRef<AudioContext | null>(null);
//   const sourceRef = useRef<MediaStreamAudioSourceNode | null>(null);
//   const processorRef = useRef<ScriptProcessorNode | null>(null);
//   const streamRef = useRef<MediaStream | null>(null);
//   const textareaId = useRef(uuidv4()).current;
//   const textDataRef = useRef('');
//   const nlpService = NlpWebSocketService.getInstance();
//   const userDetails = GetItemFromStorage(LocalStorage?.ACCESS_TOKEN);
//   const data: any = jwtDecode(userDetails);
//   const [campaignMessage, setCampaignMessage] = useState('');
//   const [isRecording, setIsRecording] = useState(false);
//   const [loader, setLoader] = useState<any>(false);
//   const [uploadedFiles, setUploadedFiles] = useState<any[]>([]);
//   const [emailVariant,setEmailVariant] = useState<any>(1);
//   const [whatsappCard,setWhatsAppCard] = useState<any>(null);
//   console.log("whatsappcard",whatsappCard)

//   const initialWebOption: any = {
//     "label": "Chrome",
//     "value": 1
//   }
//   const initialPhoneOption: any = {
//     "label": "Android",
//     "value": 1
//   }
//   let campaignChatId = localStorage.getItem("campaignChatId");

//   let campaignTemplateData :any= useSelector((state: any) => state?.campaignData);
// // console.log("camptemp",Object.keys(campaignTemplateData?.templeteView?.allTemplates));
// // console.log("camptemp11",Object.keys(campaignTemplateData?.templeteView?.allTemplates));

//   const handleInput: React.FormEventHandler<HTMLTextAreaElement> = (e) => {
//     try {
//       const el = e.currentTarget;
//       el.style.height = "auto";
//       if (el.value.trim() === "") {
//         el.style.height = `${MIN_HEIGHT}px`;
//       } else {
//         el.style.height = `${Math.min(el.scrollHeight, MAX_HEIGHT)}px`;
//       }
//       setShowSend(el.value.trim() !== "");
      
//     } catch (error) {
//        console.log("API error:",error)
//     }
//   };

//   const removeFile = (index: number) => {
//     try {
      
//       setUploadedFiles((prev) => prev.filter((_, i) => i !== index));
//     } catch (error) {
      
//     }
//   };

//   useLayoutEffect(() => {
//     try {
      
//       if (!bottomRef.current) return;
//       bottomRef.current.scrollTo({
//         top: bottomRef.current.scrollHeight,
//         behavior: "smooth",
//       });
//     } catch (error) {
//       console.log("API error:",error)
//     }
//   }, [campaignTemplateData?.templateConversation?.length]);

//   const handleSend = () => {
//     try {
//       if (!campaignMessage.trim()) return;
//       const newMsg = {
//         sessionId,
//         campaignChatId,
//         messageId: uuidv4(),
//         message: campaignMessage ?? '',
//         uploadUrl: uploadedFiles ?? '',
//         sender: "userText",
//         date: new Date().toISOString(),
//         domainId: data?.domainId,
//         ext: +(data?.ext),
//         product: campaignMessageData?.product?.type,
//         proActive: campaignMessageData?.product?.data?.length > 0 ? 1 : 0,
//         template_id:campaignTemplateData?.templeteView?.template?.[0]?.template_id,
//         template_channel: campaignTemplateData?.templeteView?.template?.[0]?.channel,
//         template_chat:1
//       };
//       dispatch(campaignSliceActions.setIsTyping(true));
//       const temp_data: any = [...campaignTemplateData?.templateConversation]
//       temp_data.push(newMsg)
//       dispatch(campaignSliceActions.setTemplateSpace(temp_data));
//       console.log("chatType", campaignMessageData?.chatType)
//       if (campaignMessageData?.chatType == 'new') {
//         SocketService.campaignMessage(newMsg);
//       }
//       else {
//         SocketService.pastCampaignMessage({ ...newMsg, chatType: "analytics" });
//       }
//       setCampaignMessage("");
//       setUploadedFiles([]);
//       if (textareaRef.current) textareaRef.current.style.height = `${MIN_HEIGHT}px`;
//     } catch (error) {
//       console.log(error)
//     }
//   };

//   const uploadWhatsappTemplate = async (event: React.ChangeEvent<HTMLInputElement>) => {
//     try {
      
//       const files = event.target.files;
//       if (!files || files.length === 0) return;
//       const maxFileSizeInBytes = 5 * 1024 * 1024; // 5 MB
//       const fileArray = Array.from(files);
//       for (const file of fileArray) {
//         if (file.size > maxFileSizeInBytes) {
//           alert(`❌ ${file.name} exceeds 5 MB. Skipping.`);
//           continue;
//         }
//         const formData = new FormData();
//         formData.append("doc", file);
//         try {
//           const res = await whatsappService.post(formData, `/knowledge_base_upload`);
//           if (res?.data?.statusCode === 200) {
//             const uploadFile = res.data.fileList?.[0];
//             const fileData: any = {
//               filename: uploadFile.filename,
//               mimetype: uploadFile.mimetype,
//               size: uploadFile.size,
//               uploadUrl: uploadFile.url,
//             };
//             setUploadedFiles((prev: any) => [...prev, fileData]);
//           } else {
//             alert(`⚠️ Upload failed for ${file.name}`);
//           }
//         } catch (error) {
//           console.error("Upload error:", error);
//           alert(`❌ Error uploading ${file.name}`);
//         }
//       }
//       event.target.value = "";
//     } catch (error) {
      
//     }
//   };

//   const phoneOptions = [
//     { label: 'Android', value: 1 },
//     { label: 'Ios', value: 2 }
//   ]

//   const webPushOptions = [
//     { label: 'Chrome', value: 1 },
//     { label: 'Safari', value: 2 }
//   ]
//   const formatFileSize = (bytes?: number) => {
//     try {
      
//       if (!bytes) return "0 KB";
//       const sizes = ["Bytes", "KB", "MB", "GB"];
//       const i = Math.floor(Math.log(bytes) / Math.log(1024));
//       const size = (bytes / Math.pow(1024, i)).toFixed(1);
//       return `${size} ${sizes[i]}`;
//     } catch (error) {
      
//     }
//   };

//   const whatsappTemplateData = useSelector((state: any) => state.campaignData.whatsAppCardDetails);
//   console.log("whatsappTemplateData", whatsappTemplateData);






  

//   useEffect(() => {
//     try {
      
//       if (!nlpService.isConnected()) {
//         nlpService.connect(nlpSocketUrl);
//       }
//     } catch (error) {
      
//     }
//   }, [nlpService.isConnected()])

//   useEffect(() => {
//     try {
      
//       const unsubscribe = nlpService.subscribe((msg) => {
//         if (msg.text && msg.session_id === textareaId) {
//           console.log("insertAtCursor beforeee", msg.text)
//           let text = insertAtCursor(msg.text)
//           handelTextArea(text)
//         }
//       });
//       return () => unsubscribe();
//     } catch (error) {
      
//     }
//   }, []);

//   const handelTextArea = (text: any) => {
//     try {
      
//       textDataRef.current = text
//       setCampaignMessage(text)
//     } catch (error) {
      
//     }
//   }

//   const normalTextBoxFocus = () => {
//     try {
      
//       setTimeout(() => {
//         if (textareaRef.current) {
//           textareaRef.current?.focus()
//           textareaRef.current.selectionStart = textareaRef.current.selectionEnd = (textareaRef.current.value?.length || 0);
//         }
//       }, 100);
//     } catch (error) {
      
//     }
//   }

//   const insertAtCursor = (newText: any) => {
//     try {
      
//       const textarea = textareaRef.current;
//       if (!textarea) return;
//       const start = textarea.selectionStart;
//       const end = textarea.selectionEnd;
//       const updated = (textDataRef.current
//         ? textDataRef.current.slice(0, start) + " " + newText + " " + textDataRef.current.slice(end)
//         : newText
//       )?.trim();
  
//       setTimeout(() => {
//         textarea.selectionStart = textarea.selectionEnd = start + newText.length + 1;
//       }, 0);
  
//       return updated
//     } catch (error) {
      
//     }
//   };

//   const micToggle = () => {
//     try {
      
//       isRecording ? stopRawRecording() : startVoiceToText()
//     } catch (error) {
      
//     }
//   }

//   const startVoiceToText = async () => {
//     try {
//       normalTextBoxFocus()
//       nlpService.sendMessage(JSON.stringify({ session_id: textareaId }));
//       const stream = await navigator.mediaDevices.getUserMedia({ audio: true });
//       streamRef.current = stream;
//       const audioCtx = new AudioContext();
//       audioContextRef.current = audioCtx;
//       const source = audioCtx.createMediaStreamSource(stream);
//       sourceRef.current = source;
//       const processor = audioCtx.createScriptProcessor(4096, 1, 1);
//       processorRef.current = processor;
//       source.connect(processor);
//       processor.connect(audioCtx.destination);
//       let pcmBuffer: number[] = [];
//       processor.onaudioprocess = (e) => {
//         const input = e.inputBuffer.getChannelData(0);
//         const downsampled = downsampleBuffer(input, audioCtx.sampleRate, 16000);
//         const pcm16: any = floatTo16BitPCM(downsampled);
//         pcmBuffer.push(...pcm16);
//         while (pcmBuffer.length >= 320) {
//           const chunk = pcmBuffer.slice(0, 320);
//           pcmBuffer = pcmBuffer.slice(320);
//           nlpService.sendMessage(Int16Array.from(chunk).buffer);
//         }
//       };
//       setIsRecording(true);
//     } catch (err) {
//       alert(err);

//     }
//   };

//   const stopRawRecording = async () => {
//     try {
//       processorRef.current?.disconnect();
//       if (processorRef.current) processorRef.current.onaudioprocess = null;
//       sourceRef.current?.disconnect();
//       streamRef.current?.getTracks().forEach((t) => t.stop());
//       await audioContextRef.current?.close();
//     } finally {
//       processorRef.current = null;
//       sourceRef.current = null;
//       streamRef.current = null;
//       audioContextRef.current = null;
//       setIsRecording(false);
//     }
//   };



//   const handleRoute=()=>{
//     dispatch(campaignSliceActions.setIsTyping(false));
//     // setMainChat();
//     dispatch(commonDataSliceActions.setShowMainChat(true));
//   }

//   useEffect(()=>{
//     const init= async()=>{
       
//       if(campaignTemplateData?.templeteView?.template?.[emailVariant-1]?.channel=="email"){

       
//         try {
//         let body ={
//           templateId:campaignTemplateData?.templeteView?.template?.[emailVariant-1]?.template_id??0
//         }
//           const res = await whatsappService.post(body, `/templateConversation`);
//           console.log("res????",res)
//           if (res?.data?.statusCode === 200) {
//             let conversation = [];
  
//             try {
//               conversation = JSON.parse(
//                 res?.data?.getPastCampaignListResponse?.[0]?.conversation || "[]"
//               );
//             } catch (e) {
//               console.error("JSON parse error", e);
//             }
//             console.log("called345")
//             dispatch(campaignSliceActions.setTemplateSpace(conversation));
//             dispatch(campaignSliceActions.setIsTyping(false));
//             console.log("called456")
//             setEmailVariant(emailVariant);
                      
  
//           }
//       } catch (error :any) {
//         console.log("API ERROR:",error)
//       }
//       finally{
//         setLoader(false);
//       }
//       }
//     }
//     init();
    
//   },[emailVariant])

//     useEffect(() => {   
//          const newMsg = {
//           sessionId,
//           campaignChatId: campaignId?.CampaignChatId,
//           messageId: uuidv4(),
//           message: ``,
//           sender: "userText",
//           date: new Date().toISOString(),
//           domainId: data?.domainId,
//           ext: +(data?.ext),
//           chatType: "analytics",
//           template_chat:0
//         };
//         SocketService.pastCampaignMessage({ ...newMsg, campaignDetail: campaignMessageData?.pastCampaignDetails });
      
//       speechSynthesis.cancel();
//     }, []);

//   return (
//     <>
    
     
                                              

//       {loader ? <div className='loadersinner'><Loaders /></div> :
//         <div className={`${styles.elevatebrandbody} createcampaignhome ${styles.createcampaignbody}`}>
//           <div className={`${styles.innerbody} ${isCVM ? "innerbodyiframe" : ""}`}>
           
//             {/* {(campaignMessageData?.campaignMessage?.length > 0 || showMainChat) && */}
//               <div className={`${styles.msgbody} msgbody`}>
//                     <div className={styles.headertopmost}>
//                       <div className={styles.navwithtitle} onClick={()=>handleRoute()}>
//                          <span><svg xmlns="http://www.w3.org/2000/svg" width="6" height="13" viewBox="0 0 6 13" fill="none" >
//                                                     <path d="M5.15277 12.0655C4.96137 12.0655 4.76947 11.9923 4.62297 11.8467L0.758268 7.99325C0.332468 7.56845 0.0976688 7.00305 0.0971688 6.40045C0.0966688 5.79785 0.331568 5.23245 0.758268 4.80575L4.62347 0.953248C4.91647 0.661248 5.39157 0.661248 5.68397 0.955248C5.97647 1.24825 5.97547 1.72285 5.68197 2.01575L1.81727 5.86825C1.67517 6.00985 1.59707 6.19935 1.59707 6.40045C1.59707 6.60065 1.67517 6.78915 1.81727 6.93075L5.68247 10.7842C5.97547 11.0772 5.97637 11.5518 5.68397 11.8447C5.53747 11.9922 5.34507 12.0655 5.15277 12.0655Z" fill="black" />
//                                                   </svg></span>
//                          {campaignTemplateData?.templeteView?.campaign_name}
//                       </div>
//                       <div className={styles.tabcontainer}>
                       
                         
//                       </div>
//                     </div>
//                 <div style={{ marginTop: "20px" }} ref={bottomRef} className={`${styles.scroller} ${styles.chatflexscroll}`}>
//                   <div className={styles.maincontainer}>
            




//                     {Array.isArray((campaignTemplateData?.templateConversation)) && (campaignTemplateData?.templateConversation)?.map((msg: any, index: number) => {
//                       if (msg.sender === "userText") {
//                         return (
//                           <>
//                             {msg?.uploadUrl?.length > 0 && msg?.uploadUrl?.map((file: any, index: number) => {
//                               return (
//                                 file.mimetype.startsWith("image/") ? (
//                                   <a
//                                     key={index}
//                                     href={file.uploadUrl}
//                                     target="_blank"
//                                     rel="noopener noreferrer"
//                                     className={`${styles.msgbubble} ${styles.right} imguploadedres`}
//                                   >
//                                     <img
//                                       src={file.uploadUrl}
//                                       alt={file.filename}
//                                       style={{
//                                         width: "40px",
//                                         height: "40px",
//                                         borderRadius: "6px",
//                                         objectFit: "cover",
//                                       }}
//                                     />
//                                   </a>
//                                 ) : (
//                                   <a
//                                     key={index}
//                                     href={file.uploadUrl}
//                                     target="_blank"
//                                     rel="noopener noreferrer"
//                                     className={`${styles.fileupload} ${styles.msgbubble} ${styles.right}`}
//                                   >
//                                     <div className={styles.lefts}>
//                                       <img src={assets.pdficon} alt="file-icon" />
//                                     </div>
//                                     <div className={styles.rights}>
//                                       <h5>{file?.filename || "Unknown File"}</h5>
//                                       <p>
//                                           {file?.mimetype.includes("pdf") ? "PDF" : file?.mimetype.includes("msword") ? "WORD" : file?.mimetype.split("/")[1]?.toUpperCase() || "FILE"}
//                                           {" • "}
//                                           {formatFileSize(file?.size)}
//                                         </p>
//                                     </div>
//                                   </a>
//                                 ))
//                             }
//                             )}
//                             <div key={msg?.messageId} className={`${styles.msgbubble} ${styles.right}`}>
//                               <div className="chatContent" dangerouslySetInnerHTML={{ __html: msg?.message }} />
//                             </div>
//                           </>
//                         );
//                       }
//                       if (msg.sender === "botText" && campaignTemplateData?.templeteView?.template?.[0]?.channel==msg?.template_channel) {
//                         let textToShow = msg?.bot_response;
                        

//                         return (
//                           <>

//                             {msg?.progress && (msg?.progress) < "100" ?
//                               <>
//                                 <div className={`${styles.msgbubble} ${styles.left} ${styles.containprogress}`}>
//                                   <p>{msg?.progress_message}</p>
//                                   <ProgressBar progress={msg?.progress} /></div> </> :
//                               <div key={msg?.messageId} className={`${styles.msgbubble} ${styles.left}`}>
//                                 <div className="chatContent" dangerouslySetInnerHTML={{ __html: textToShow }} />
                               
//                                 {/* {(msg?.intent !== "refresh_dashboard" && textToShow?.trim()?.length>0)&&<ChatLikes changeindex={index} allConversation ={campaignMessageData?.campaignMessage} likeOrDislike={msg?.likeOrDislike} feedBackDataProp={msg?.feedbackData} msg ={msg} campaignChatId={campaignChatId} message={msg?.bot_response}></ChatLikes>} */}
//                                 </div>
//                             }
                           
                           
//                               <div className={`${styles.msgbubble} ${styles.left} ${styles.cardsresponse}`}>
//                                 <div className={styles.cardscontainerscroller}>
                                

                                

                                
                                

                               

                               

                                
//                                 </div >
//                               </div>
//                             {/* )} */}
                            
                            





                            
//                           </>
//                         );
//                       }
//                     //   return null;

//                     {/* whatsapp status card */}
                    
//                     })}
      

                    

//                     {campaignMessageData?.isTyping && (
//                       <>
//                         <p className="generatings">
//                           Agent is Analysing
//                           <div className='typingdot'><span></span><span></span><span></span></div>
//                         </p>
                        
//                       </>
//                     )}

                    
//                   </div>
//                 </div>
              
//               </div>
//               {/* } */}
//           </div >
//         </div >
//       }
//     </>
//   )
// }

// export default CampaignAnalytics;


// import React, { useEffect, useLayoutEffect, useRef, useState } from 'react'
// import styles from "./scss/elevatebrand.module.scss"
// import { assets, leftnavassets } from '../../../styles/assets/assets'
// import ProgressBar from '../../../components/custom_components/progressbar/progressbar'
// import WorktualModal from '../../../components/custom_components/worktualModal'
// import { v4 as uuidv4 } from "uuid";
// import { GetItemFromStorage } from '../../../base/customhooks/useStorage'
// import { LocalStorage } from '../../../base/customhooks/localStorageKeys'
// import { jwtDecode } from 'jwt-decode'
// import { useDispatcher } from '../../../store/redux-store/dispatch'
// import { campaignSliceActions } from '../../../store/slice/campaign'
// import { useSelector } from 'react-redux'
// import SocketService from '../../../base/service/socketService'
// import { format } from "date-fns";
// import WorktualDropdown from '../../../components/custom_components/WroktualDropdown'
// import ClickTooltip from '../../../components/custom_components/clickabletooltip/clickabletooltip'
// import { whatsappService } from '../../../base/service/apiservice'
// import { useLocation, useNavigate, useSearchParams } from 'react-router-dom'
// import { decryptAESCVM, downsampleBuffer, floatTo16BitPCM } from '../../../base/utils'
// import Loaders from '../../../components/loader/loader'
// import { config } from '../../../base/constant'
// import NlpWebSocketService from '../../../base/service/sttNlpSocket'
// import ChatLikes from '../../../LikeDislike/chatLikes'


// const MAX_HEIGHT = 200;
// const MIN_HEIGHT = 40;
// const sessionId = uuidv4();

function CampaignAnalytics({ isCVM, campaignId, isNewChat }: any) {
  const location = useLocation();
  const { state } = location;
  // const campaignMessageData = ["hi"];
  const campaignMessageData = useSelector((state: any) => state.analyticsData);
  console.log("analytics message data---->",campaignMessageData)
  const dispatch = useDispatcher()
  const textareaRef = useRef<HTMLTextAreaElement>(null);
  const bottomRef = useRef<HTMLDivElement | null>(null);
  const fileInputRef = useRef<HTMLInputElement | null>(null);
  const nlpSocketUrl: any = config().STT_NLP;
  const audioContextRef = useRef<AudioContext | null>(null);
  const sourceRef = useRef<MediaStreamAudioSourceNode | null>(null);
  const processorRef = useRef<ScriptProcessorNode | null>(null);
  const streamRef = useRef<MediaStream | null>(null);
  const textareaId = useRef(uuidv4()).current;
  const textDataRef = useRef('');
  const nlpService = NlpWebSocketService.getInstance();
  const [showSend, setShowSend] = useState(false);
  const userDetails = GetItemFromStorage(LocalStorage?.ACCESS_TOKEN);
  const data: any = jwtDecode(userDetails);
  // const [campaignMessage, setCampaignMessage] = useState('');
  const [analyticsMessage, setAnalyticsMessage] = useState('');

  
  const [isRecording, setIsRecording] = useState(false);
  const [loader, setLoader] = useState<any>(false);
  const [uploadedFiles, setUploadedFiles] = useState<any[]>([]);

  const initialWebOption: any = {
    "label": "Chrome",
    "value": 1
  }
  const initialPhoneOption: any = {
    "label": "Android",
    "value": 1
  }

  let campaignChatId = localStorage.getItem("campaignChatId");

  const adjustTextareaHeight = useCallback((value: string) => {
    const textarea = textareaRef.current;
    if (!textarea) return;
    textarea.style.height = "auto";
    const height = value.trim() === "" ? MIN_HEIGHT : Math.min(textarea.scrollHeight, MAX_HEIGHT);
    textarea.style.height = `${height}px`;
  }, []);

  const handleInput: React.FormEventHandler<HTMLTextAreaElement> = (e) => {
    const { value } = e.currentTarget;
    adjustTextareaHeight(value);
    setShowSend(value.trim() !== "");
  };

  const removeFile = (index: number) => {
    setUploadedFiles((prev) => prev.filter((_, i) => i !== index));
  };

  useLayoutEffect(() => {
    if (!bottomRef.current) return;
    bottomRef.current.scrollTo({
      top: bottomRef.current.scrollHeight,
      behavior: "smooth",
    });
  }, [campaignMessageData?.analyticsMessage?.length]);

  const handleSend = () => {
    try {
      if (!analyticsMessage.trim() && uploadedFiles.length==0) return;
      const newMsg = {
        sessionId,
        campaignChatId:campaignId,
        messageId: uuidv4(),
        message: analyticsMessage ?? '',
        uploadUrl: uploadedFiles ?? '',
        sender: "userText",
        date: new Date().toISOString(),
        domainId: data?.domainId,
        ext: +(data?.ext),
        product: campaignMessageData?.product?.type,
        proActive: campaignMessageData?.product?.data?.length > 0 ? 1 : 0
      };
      dispatch(analyticsSliceActions.setIsTyping(true));
      const temp_data: any = [...campaignMessageData?.analyticsMessage]
      temp_data.push(newMsg)
      dispatch(analyticsSliceActions.setAnalyticsMessage(temp_data));
      console.log("chatType", campaignMessageData?.chatType)
  
        SocketService.pastCampaignMessage({ ...newMsg, chatType: "analytics" });
      
      setAnalyticsMessage("");
      setUploadedFiles([]);
      if (textareaRef.current) textareaRef.current.style.height = `${MIN_HEIGHT}px`;
    } catch (error) {
      console.log(error)
    }
  };

  const uploadWhatsappTemplate = async (event: React.ChangeEvent<HTMLInputElement>) => {
    const files = event.target.files;
    if (!files || files.length === 0) return;
    const maxFileSizeInBytes = 5 * 1024 * 1024; // 5 MB
    const fileArray = Array.from(files);
    for (const file of fileArray) {
      if (file.size > maxFileSizeInBytes) {
        alert(`❌ ${file.name} exceeds 5 MB. Skipping.`);
        continue;
      }
      const formData = new FormData();
      formData.append("doc", file);
      try {
        const res = await whatsappService.post(formData, `/knowledge_base_upload`);
        if (res?.data?.statusCode === 200) {
          const uploadFile = res.data.fileList?.[0];
          const fileData: any = {
            filename: uploadFile.filename,
            mimetype: uploadFile.mimetype,
            size: uploadFile.size,
            uploadUrl: uploadFile.url,
          };
          setUploadedFiles((prev: any) => [...prev, fileData]);
        } else {
          alert(`⚠️ Upload failed for ${file.name}`);
        }
      } catch (error) {
        console.error("Upload error:", error);
        alert(`❌ Error uploading ${file.name}`);
      }
    }
    event.target.value = "";
     setTimeout(() => {
      textareaRef.current?.focus();
    }, 10);
  };


  const formatFileSize = (bytes?: number) => {
    if (!bytes) return "0 KB";
    const sizes = ["Bytes", "KB", "MB", "GB"];
    const i = Math.floor(Math.log(bytes) / Math.log(1024));
    const size = (bytes / Math.pow(1024, i)).toFixed(1);
    return `${size} ${sizes[i]}`;
  };

  useEffect(() => {
    if (campaignMessageData.clearMessage) {
      setAnalyticsMessage('');
      setUploadedFiles([]);
      dispatch(analyticsSliceActions.setClearMessage(false));
    }
  }, [campaignMessageData.clearMessage])

  useEffect(() => {
    adjustTextareaHeight(analyticsMessage);
    setShowSend(analyticsMessage.trim() !== "");
  }, [analyticsMessage, adjustTextareaHeight]);

  const [showMainChat, setShowMainChat] = useState<any>(false);


  useEffect(() => {
    if (!nlpService.isConnected()) {
      nlpService.connect(nlpSocketUrl);
    }
  }, [nlpService.isConnected()])

  useEffect(() => {
    const unsubscribe = nlpService.subscribe((msg) => {
      if (msg.text && msg.session_id === textareaId) {
        console.log("insertAtCursor beforeee", msg.text)
        let text = insertAtCursor(msg.text)
        handelTextArea(text)
      }
    });
    return () => unsubscribe();
  }, []);

  const handelTextArea = (text: any) => {
    textDataRef.current = text
    setAnalyticsMessage(text)
  }

  const normalTextBoxFocus = () => {
    setTimeout(() => {
      if (textareaRef.current) {
        textareaRef.current?.focus()
        textareaRef.current.selectionStart = textareaRef.current.selectionEnd = (textareaRef.current.value?.length || 0);
      }
    }, 100);
  }

  const insertAtCursor = (newText: any) => {
    const textarea = textareaRef.current;
    if (!textarea) return;
    const start = textarea.selectionStart;
    const end = textarea.selectionEnd;
    const updated = (textDataRef.current
      ? textDataRef.current.slice(0, start) + " " + newText + " " + textDataRef.current.slice(end)
      : newText
    )?.trim();

    setTimeout(() => {
      textarea.selectionStart = textarea.selectionEnd = start + newText.length + 1;
    }, 0);

    return updated
  };

  const micToggle = () => {
    isRecording ? stopRawRecording() : startVoiceToText()
  }

  const startVoiceToText = async () => {
    try {
      normalTextBoxFocus()
      nlpService.sendMessage(JSON.stringify({ session_id: textareaId }));
      const stream = await navigator.mediaDevices.getUserMedia({ audio: true });
      streamRef.current = stream;
      const audioCtx = new AudioContext();
      audioContextRef.current = audioCtx;
      const source = audioCtx.createMediaStreamSource(stream);
      sourceRef.current = source;
      const processor = audioCtx.createScriptProcessor(4096, 1, 1);
      processorRef.current = processor;
      source.connect(processor);
      processor.connect(audioCtx.destination);
      let pcmBuffer: number[] = [];
      processor.onaudioprocess = (e) => {
        const input = e.inputBuffer.getChannelData(0);
        const downsampled = downsampleBuffer(input, audioCtx.sampleRate, 16000);
        const pcm16: any = floatTo16BitPCM(downsampled);
        pcmBuffer.push(...pcm16);
        while (pcmBuffer.length >= 320) {
          const chunk = pcmBuffer.slice(0, 320);
          pcmBuffer = pcmBuffer.slice(320);
          nlpService.sendMessage(Int16Array.from(chunk).buffer);
        }
      };
      setIsRecording(true);
    } catch (err) {
      alert(err);

    }
  };

  const stopRawRecording = async () => {
    try {
      processorRef.current?.disconnect();
      if (processorRef.current) processorRef.current.onaudioprocess = null;
      sourceRef.current?.disconnect();
      streamRef.current?.getTracks().forEach((t) => t.stop());
      await audioContextRef.current?.close();
    } finally {
      processorRef.current = null;
      sourceRef.current = null;
      streamRef.current = null;
      audioContextRef.current = null;
      setIsRecording(false);
    }
  };





useEffect(() => {   
    const newMsg = {
    sessionId,
    campaignChatId: campaignId,
    messageId: uuidv4(),
    message: ``,
    sender: "userText",
    date: new Date().toISOString(),
    domainId: data?.domainId,
    ext: +(data?.ext),
    chatType: "analytics",
    template_chat:0
  };
  // SocketService.sentAnalyticsMessage({ ...newMsg });
  speechSynthesis.cancel();
  SocketService.pastCampaignMessage({ ...newMsg });
  setTimeout(()=>dispatch(analyticsSliceActions.setIsTyping(true)),200);
}, [campaignId]);




useEffect(() => {
  if (state?.data) {
    dispatch(analyticsSliceActions.setIsTyping(true));
  }
}, [state?.data])


const res={
    "analyticsMessage": [
        {
            "sessionId": "f6d047d0-7809-4142-8ab5-98f456f5c2d6",
            "message": "",
            "domainId": 13490,
            "messageId": "55ca3f7e-2533-487c-b98b-c2bfaef0e15c",
            "ext": 200,
            "campaignChatId": "13a0063e-4ec4-4598-9f0e-7f8c1bc94338",
            "chatType": "analytics",
            "allCampaignId": {},
            "bot_response": "<section style=\"font-family: Arial, sans-serif; background-color: #ffffff; padding: 20px; border-radius: 10px; box-shadow: 0 2px 8px rgba(0,0,0,0.1); line-height: 1.6; max-width: 650px;\">\n\n    <h2 style=\"color: #1a5276; margin-top: 0;\">Campaign Performance Summary</h2>\n\n    <div style=\"font-size: 20px; font-weight: bold; margin-bottom: 15px;\">\n        ChatBoost Launch\n    </div>\n\n    <p>\n        Your multi-channel campaign reached audiences across <b>Email</b>.\n    </p>\n\n    <ul>\n        <li>\n            Email delivered to <b>137</b> recipients, with an open rate of \n            <b>40.1460%</b> and click-through rate of <b>39.4161%</b>.\n        </li>\n    </ul>\n\n    <p>\n    average engagement rate of <b>39.708029197%</b>.\n    </p>\n\n</section>",
            "intent": "faq",
            "error": 0,
            "error_message": null,
            "sender": "botText",
            "campaign_data": {}
        }
    ],
    "isTyping": false,
    "progress": false,
    "clearMessage": false,
    "product": {
        "type": "campaign",
        "data": null,
        "query": null
    },
    "templateConversation": []
}

  return (
    <>
      {loader ? <div className='loadersinner'><Loaders /></div> :
        <div className={`${styles.elevatebrandbody} createcampaignhome ${styles.createcampaignbody}`}>
          <div className={`${styles.innerbody} ${isCVM ? "innerbodyiframe" : ""}`}>
            {campaignMessageData?.analyticsMessage?.length < 0 && isNewChat && !showMainChat &&
              <div className={`${styles.centered} ${isCVM ? "centered" : ""}`}>
                <div className={`${styles.content} `}>
                  <h5>Build campaigns with ease!</h5>
                  <p style={{ marginBottom: "24px" }}>Everything you need to launch smarter, faster, and better.</p>
                  <div style={{ marginTop: "16px" }} className={`${styles.searchblock} ${styles.newinput}`}>
                    <div className={` ${styles.searchmessage}`}>
                      <div className={`${styles.searchmessageText}`}>
                        {uploadedFiles.length > 0 && <div className="attachment-container">
                          {uploadedFiles.map((file, index) => (
                            <React.Fragment key={index}>
                              {file.mimetype.startsWith("image/") ? (
                                <div className="simpleimgcard">
                                  <img
                                    src={file.uploadUrl}
                                    alt={file.filename}
                                    className="file-preview"
                                    style={{
                                      width: "40px",
                                      height: "40px",
                                      borderRadius: "6px",
                                      objectFit: "cover",
                                    }}
                                  />
                                  <svg
                                    onClick={() => removeFile(index)}
                                    className="removeebtn"
                                    width="15"
                                    height="15"
                                    viewBox="0 0 15 15"
                                    fill="none"
                                    xmlns="http://www.w3.org/2000/svg"
                                  >
                                    <path
                                      d="M7.52143 0.857544C3.8411 0.857544 0.857544 3.8411 0.857544 7.52143C0.857544 11.2018 3.8411 14.1853 7.52143 14.1853C11.2018 14.1853 14.1853 11.2018 14.1853 7.52143C14.1853 3.8411 11.2018 0.857544 7.52143 0.857544ZM10.4422 9.66174C10.6375 9.85699 10.6375 10.1733 10.4422 10.3684C10.3446 10.4661 10.2167 10.5149 10.0888 10.5149C9.96095 10.5149 9.83307 10.4661 9.73545 10.3684L7.5215 8.1545L5.30756 10.3684C5.20993 10.4661 5.08205 10.5149 4.95417 10.5149C4.82629 10.5149 4.69841 10.4661 4.60078 10.3684C4.40553 10.1732 4.40553 9.85693 4.60078 9.66174L6.81473 7.4478L4.60078 5.23385C4.40553 5.0386 4.40553 4.72233 4.60078 4.52715C4.79604 4.33196 5.1123 4.3319 5.30749 4.52715L7.52143 6.74109L9.73538 4.52715C9.93063 4.3319 10.2469 4.3319 10.4421 4.52715C10.6373 4.7224 10.6373 5.03867 10.4421 5.23385L8.22814 7.4478L10.4422 9.66174Z"
                                      fill="#484848"
                                    />
                                  </svg>
                                </div>
                              ) : (
                                <div className="attachment-card">
                                  <div className="thumb">
                                    <div className="file-icon">
                                      <img src={file.mimetype.includes("pdf")?assets.pdficon:file.mimetype.includes("txt")?assets.uploaddoc:assets.uploadxlsx} alt="file-icon" />
                                    </div>
                                  </div>
                                  <div className="file-info">
                                    <p className="filename" title={file.filename}>
                                      {file.filename}
                                    </p>
                                    <p className="details">
                                      {file.mimetype.includes("pdf")
                                        ? "PDF"
                                        : file.mimetype.split("/")[1].toUpperCase()}{" "}
                                      • {formatFileSize(file.size)}
                                    </p>
                                  </div>
                                  <svg
                                    onClick={() => removeFile(index)}
                                    className="removeebtn"
                                    width="15"
                                    height="15"
                                    viewBox="0 0 15 15"
                                    fill="none"
                                    xmlns="http://www.w3.org/2000/svg"
                                  >
                                    <path
                                      d="M7.52143 0.857544C3.8411 0.857544 0.857544 3.8411 0.857544 7.52143C0.857544 11.2018 3.8411 14.1853 7.52143 14.1853C11.2018 14.1853 14.1853 11.2018 14.1853 7.52143C14.1853 3.8411 11.2018 0.857544 7.52143 0.857544ZM10.4422 9.66174C10.6375 9.85699 10.6375 10.1733 10.4422 10.3684C10.3446 10.4661 10.2167 10.5149 10.0888 10.5149C9.96095 10.5149 9.83307 10.4661 9.73545 10.3684L7.5215 8.1545L5.30756 10.3684C5.20993 10.4661 5.08205 10.5149 4.95417 10.5149C4.82629 10.5149 4.69841 10.4661 4.60078 10.3684C4.40553 10.1732 4.40553 9.85693 4.60078 9.66174L6.81473 7.4478L4.60078 5.23385C4.40553 5.0386 4.40553 4.72233 4.60078 4.52715C4.79604 4.33196 5.1123 4.3319 5.30749 4.52715L7.52143 6.74109L9.73538 4.52715C9.93063 4.3319 10.2469 4.3319 10.4421 4.52715C10.6373 4.7224 10.6373 5.03867 10.4421 5.23385L8.22814 7.4478L10.4422 9.66174Z"
                                      fill="#484848"
                                    />
                                  </svg>
                                </div>
                              )}
                            </React.Fragment>
                          ))}
                        </div>}
                        <div className={styles.inputcontainer}>
                          <textarea
                            ref={textareaRef}
                            placeholder="Write your Goal & Objective"
                            className="textareamainsnd"
                            rows={1}
                            style={{ minHeight: "40px", maxHeight: "120px", height: "40px" }}
                            onInput={handleInput}
                            value={analyticsMessage}
                            onChange={(e) => handelTextArea(e.target.value)}
                            onKeyDown={(e) => {
                              if (!campaignMessageData?.isTyping && !campaignMessageData?.progress && e.key === "Enter" && !e.shiftKey) {
                                e.preventDefault();
                                handleSend();
                              }
                            }}
                          />
                        </div>
                        <div className={`${styles.actionbottom} ${styles.actionbottomtop}`}>
                          <div>
                            {analyticsMessage && !isRecording ? (
                              <img
                                src={assets.sndarrowchat}
                                alt="send"
                                onClick={handleSend}
                                style={{ cursor: "pointer" , height:"38px" }}
                              />
                            ) : (
                              <>
                                {!isRecording ? <svg width="38" height="38" viewBox="0 0 38 38" fill="none" xmlns="http://www.w3.org/2000/svg" onClick={micToggle} aria-pressed={isRecording}>
                                  <path d="M24.9911 20.2275C24.5907 20.1162 24.1776 20.3427 24.0634 20.7412C23.3202 23.3271 20.9189 25.1338 18.2236 25.1338C15.5507 25.1338 13.1543 23.3457 12.3965 20.7861C12.2803 20.3896 11.8662 20.1601 11.4649 20.2802C11.0674 20.3974 10.8409 20.8144 10.959 21.2118C11.832 24.1635 14.4568 26.268 17.4815 26.5722V28.3261C17.4815 28.7402 17.8174 29.0761 18.2315 29.0761C18.6456 29.0761 18.9815 28.7402 18.9815 28.3261V26.5707C22.0242 26.2595 24.649 24.1343 25.5049 21.1552C25.6192 20.7577 25.3896 20.3417 24.9911 20.2275Z" fill="#12C3B3" />
                                  <g filter="url(#filter0_i_273_2583)">
                                    <path d="M21.9571 17.1995L21.0801 15.1556C21.0332 15.0482 20.9463 14.9603 20.8399 14.9154L18.8399 14.058C18.3741 13.8266 18.1221 13.4291 18.1221 12.973C18.1221 12.5169 18.3741 12.1195 18.795 11.9085L20.8389 11.0315C20.9473 10.9846 21.0332 10.8987 21.0801 10.7893L21.3471 10.1665C20.5066 9.55758 19.4817 9.18848 18.3646 9.18848H18.0982C15.2792 9.18848 12.9941 11.4737 12.9941 14.2926V18.528C12.9941 21.347 15.2793 23.6321 18.0982 23.6321H18.3646C21.1834 23.6321 23.4686 21.3469 23.4686 18.528V17.7708C23.3282 17.8309 23.1805 17.8724 23.0216 17.8724C22.5655 17.8724 22.1681 17.6204 21.9571 17.1995Z" fill="url(#paint0_linear_273_2583)" />
                                  </g>
                                  <path d="M19.175 13.2766C19.0398 13.2095 18.9727 13.1081 18.9727 12.9729C18.9727 12.8384 19.0398 12.7372 19.175 12.6692L21.174 11.8126C21.4829 11.6802 21.7291 11.4341 21.8615 11.1251L22.7181 9.12613C22.7855 8.99163 22.8866 8.92383 23.0218 8.92383C23.1563 8.92383 23.2575 8.99163 23.3255 9.12613L24.1823 11.1252C24.3147 11.4341 24.5608 11.6802 24.8697 11.8126L26.8686 12.6692C27.0031 12.7373 27.0712 12.8384 27.0712 12.9729C27.0712 13.1081 27.0031 13.2095 26.8686 13.2766L24.8697 14.1332C24.5608 14.2656 24.3147 14.5117 24.1823 14.8206L23.3255 16.8197C23.2574 16.9549 23.1563 17.0223 23.0218 17.0223C22.8866 17.0223 22.7854 16.9549 22.7181 16.8197L21.8615 14.8207C21.7291 14.5118 21.483 14.2656 21.174 14.1332L19.175 13.2766Z" fill="#757676" />
                                  <path d="M19.175 13.2766C19.0398 13.2095 18.9727 13.1081 18.9727 12.9729C18.9727 12.8384 19.0398 12.7372 19.175 12.6692L21.174 11.8126C21.4829 11.6802 21.7291 11.4341 21.8615 11.1251L22.7181 9.12613C22.7855 8.99163 22.8866 8.92383 23.0218 8.92383C23.1563 8.92383 23.2575 8.99163 23.3255 9.12613L24.1823 11.1252C24.3147 11.4341 24.5608 11.6802 24.8697 11.8126L26.8686 12.6692C27.0031 12.7373 27.0712 12.8384 27.0712 12.9729C27.0712 13.1081 27.0031 13.2095 26.8686 13.2766L24.8697 14.1332C24.5608 14.2656 24.3147 14.5117 24.1823 14.8206L23.3255 16.8197C23.2574 16.9549 23.1563 17.0223 23.0218 17.0223C22.8866 17.0223 22.7854 16.9549 22.7181 16.8197L21.8615 14.8207C21.7291 14.5118 21.483 14.2656 21.174 14.1332L19.175 13.2766Z" fill="url(#paint1_linear_273_2583)" />
                                  <defs>
                                    <filter id="filter0_i_273_2583" x="12.9941" y="9.18848" width="10.4745" height="16.4436" filterUnits="userSpaceOnUse" color-interpolation-filters="sRGB">
                                      <feFlood flood-opacity="0" result="BackgroundImageFix" />
                                      <feBlend mode="normal" in="SourceGraphic" in2="BackgroundImageFix" result="shape" />
                                      <feColorMatrix in="SourceAlpha" type="matrix" values="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 127 0" result="hardAlpha" />
                                      <feOffset dy="2" />
                                      <feGaussianBlur stdDeviation="2" />
                                      <feComposite in2="hardAlpha" operator="arithmetic" k2="-1" k3="1" />
                                      <feColorMatrix type="matrix" values="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0.1 0" />
                                      <feBlend mode="normal" in2="shape" result="effect1_innerShadow_273_2583" />
                                    </filter>
                                    <linearGradient id="paint0_linear_273_2583" x1="13.9726" y1="9.18848" x2="36.7322" y2="27.1426" gradientUnits="userSpaceOnUse">
                                      <stop offset="0.0336994" stop-color="#00E2A4" />
                                      <stop offset="0.115715" stop-color="#00E2A4" />
                                      <stop offset="0.538859" stop-color="#575CFF" />
                                      <stop offset="0.868747" stop-color="#FF793D" />
                                      <stop offset="1" stop-color="#00B885" />
                                    </linearGradient>
                                    <linearGradient id="paint1_linear_273_2583" x1="18.3027" y1="12.9068" x2="27.2121" y2="9.92262" gradientUnits="userSpaceOnUse">
                                      <stop stop-color="#EA57FF" />
                                      <stop offset="0.945" stop-color="#FF5300" />
                                    </linearGradient>
                                  </defs>
                                </svg> : <svg
                                  onClick={micToggle}
                                  width="38" height="38" viewBox="0 0 38 38" fill="none" xmlns="http://www.w3.org/2000/svg">
                                  <rect width="38" height="38" rx="19" fill="#00E2A4" />
                                  <path d="M24.9911 20.2275C24.5907 20.1162 24.1776 20.3427 24.0634 20.7412C23.3202 23.3271 20.9189 25.1338 18.2236 25.1338C15.5507 25.1338 13.1543 23.3457 12.3965 20.7861C12.2803 20.3896 11.8662 20.1601 11.4649 20.2802C11.0674 20.3974 10.8409 20.8144 10.959 21.2118C11.832 24.1635 14.4568 26.268 17.4815 26.5722V28.3261C17.4815 28.7402 17.8174 29.0761 18.2315 29.0761C18.6456 29.0761 18.9815 28.7402 18.9815 28.3261V26.5707C22.0242 26.2595 24.649 24.1343 25.5049 21.1552C25.6192 20.7577 25.3896 20.3417 24.9911 20.2275Z" fill="#F0FAF7" />
                                  <g filter="url(#filter0_i_3036_4344)">
                                    <path d="M21.9571 17.1995L21.0801 15.1556C21.0332 15.0482 20.9463 14.9603 20.8399 14.9154L18.8399 14.058C18.3741 13.8266 18.1221 13.4291 18.1221 12.973C18.1221 12.5169 18.3741 12.1195 18.795 11.9085L20.8389 11.0315C20.9473 10.9846 21.0332 10.8987 21.0801 10.7893L21.3471 10.1665C20.5066 9.55758 19.4817 9.18848 18.3646 9.18848H18.0982C15.2792 9.18848 12.9941 11.4737 12.9941 14.2926V18.528C12.9941 21.347 15.2793 23.6321 18.0982 23.6321H18.3646C21.1834 23.6321 23.4686 21.3469 23.4686 18.528V17.7708C23.3282 17.8309 23.1805 17.8724 23.0216 17.8724C22.5655 17.8724 22.1681 17.6204 21.9571 17.1995Z" fill="#F0FAF7" />
                                  </g>
                                  <path d="M19.175 13.2766C19.0398 13.2095 18.9727 13.1081 18.9727 12.9729C18.9727 12.8384 19.0398 12.7372 19.175 12.6692L21.174 11.8126C21.4829 11.6802 21.7291 11.4341 21.8615 11.1251L22.7181 9.12613C22.7855 8.99163 22.8866 8.92383 23.0218 8.92383C23.1563 8.92383 23.2575 8.99163 23.3255 9.12613L24.1823 11.1252C24.3147 11.4341 24.5608 11.6802 24.8697 11.8126L26.8686 12.6692C27.0031 12.7373 27.0712 12.8384 27.0712 12.9729C27.0712 13.1081 27.0031 13.2095 26.8686 13.2766L24.8697 14.1332C24.5608 14.2656 24.3147 14.5117 24.1823 14.8206L23.3255 16.8197C23.2574 16.9549 23.1563 17.0223 23.0218 17.0223C22.8866 17.0223 22.7854 16.9549 22.7181 16.8197L21.8615 14.8207C21.7291 14.5118 21.483 14.2656 21.174 14.1332L19.175 13.2766Z" fill="#757676" />
                                  <path d="M19.175 13.2766C19.0398 13.2095 18.9727 13.1081 18.9727 12.9729C18.9727 12.8384 19.0398 12.7372 19.175 12.6692L21.174 11.8126C21.4829 11.6802 21.7291 11.4341 21.8615 11.1251L22.7181 9.12613C22.7855 8.99163 22.8866 8.92383 23.0218 8.92383C23.1563 8.92383 23.2575 8.99163 23.3255 9.12613L24.1823 11.1252C24.3147 11.4341 24.5608 11.6802 24.8697 11.8126L26.8686 12.6692C27.0031 12.7373 27.0712 12.8384 27.0712 12.9729C27.0712 13.1081 27.0031 13.2095 26.8686 13.2766L24.8697 14.1332C24.5608 14.2656 24.3147 14.5117 24.1823 14.8206L23.3255 16.8197C23.2574 16.9549 23.1563 17.0223 23.0218 17.0223C22.8866 17.0223 22.7854 16.9549 22.7181 16.8197L21.8615 14.8207C21.7291 14.5118 21.483 14.2656 21.174 14.1332L19.175 13.2766Z" fill="url(#paint0_linear_3036_4344)" />
                                  <defs>
                                    <filter id="filter0_i_3036_4344" x="12.9941" y="9.18848" width="10.4746" height="16.4436" filterUnits="userSpaceOnUse" color-interpolation-filters="sRGB">
                                      <feFlood flood-opacity="0" result="BackgroundImageFix" />
                                      <feBlend mode="normal" in="SourceGraphic" in2="BackgroundImageFix" result="shape" />
                                      <feColorMatrix in="SourceAlpha" type="matrix" values="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 127 0" result="hardAlpha" />
                                      <feOffset dy="2" />
                                      <feGaussianBlur stdDeviation="2" />
                                      <feComposite in2="hardAlpha" operator="arithmetic" k2="-1" k3="1" />
                                      <feColorMatrix type="matrix" values="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0.1 0" />
                                      <feBlend mode="normal" in2="shape" result="effect1_innerShadow_3036_4344" />
                                    </filter>
                                    <linearGradient id="paint0_linear_3036_4344" x1="18.3027" y1="12.9068" x2="27.2121" y2="9.92262" gradientUnits="userSpaceOnUse">
                                      <stop stop-color="#EA57FF" />
                                      <stop offset="0.945" stop-color="#FF5300" />
                                    </linearGradient>
                                  </defs>
                                </svg>}
                              </>
                            )}
                          </div>
                          <div>
                            <input
                              type="file"
                              accept="application/pdf, image/*"
                              multiple
                              ref={fileInputRef}
                              style={{ display: "none" }}
                              onChange={uploadWhatsappTemplate}
                            />
                            <svg
                              xmlns="http://www.w3.org/2000/svg"
                              width="16"
                              height="16"
                              viewBox="0 0 16 16"
                              fill="currentColor"
                              onClick={() => fileInputRef.current?.click()}
                              style={{ cursor: "pointer" }}
                            >
                              <path d="M15.2515 7.24538L8.74468 7.24987V0.748017C8.74468 0.333985 8.40902 -0.00167847 7.99499 -0.00167847C7.58096 -0.00167847 7.2453 0.333985 7.2453 0.748017V7.25097L0.747509 7.25547C0.333482 7.25577 -0.00197814 7.59163 -0.00167827 8.00567C-0.00137839 8.4197 0.334482 8.75516 0.748509 8.75486L7.2453 8.75036V15.2522C7.2453 15.6663 7.58096 16.0019 7.99499 16.0019C8.40902 16.0019 8.74468 15.6663 8.74468 15.2522V8.74936L15.2525 8.74487C15.6665 8.74457 16.002 8.4087 16.0017 7.99467C16.0014 7.58064 15.6655 7.24508 15.2515 7.24538Z" fill="#404040" />
                            </svg>
                          </div>

                        </div>
                      </div>
                      <></>
                    </div>
                  </div>
                </div>
              </div>}

             {(campaignMessageData?.analyticsMessage?.length === 0 || showMainChat) &&
              <div className={`${styles.msgbody} msgbody`}>
                <div style={{ marginTop: "20px" }} ref={bottomRef} className={`${styles.scroller} ${styles.chatflexscroll}`}>
                  <div className={styles.maincontainer}>
                    
                    
                    {Array.isArray(campaignMessageData?.analyticsMessage) && campaignMessageData?.analyticsMessage?.map((msg: any, index: number) => {
                      if (msg.sender === "userText") {
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
                                      <img src={assets.pdficon} alt="file-icon" />
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
                              <div className="chatContent" dangerouslySetInnerHTML={{ __html: msg?.message }} />
                            </div>
                          </>
                        );
                      }
                      if (msg.sender === "botText" && msg?.campaignChatId==campaignId) {
                        let textToShow = msg?.bot_response;
                        

                        return (
                          <>
                            {msg?.progress && (msg?.progress) < "100" ?
                              <>
                                <div className={`${styles.msgbubble} ${styles.left} ${styles.containprogress}`}>
                                  <p>{msg?.progress_message}</p>
                                  <ProgressBar progress={msg?.progress} /></div> </> :
                              <div key={msg?.messageId} className={`${styles.msgbubble} ${styles.left}`}>
                                <div className="chatContent" dangerouslySetInnerHTML={{ __html: textToShow }} />
                               
                                {/* {(msg?.intent !== "refresh_dashboard" && textToShow?.trim()?.length>0)&&<ChatLikes changeindex={index} allConversation ={campaignMessageData?.campaignMessage} likeOrDislike={msg?.likeOrDislike} feedBackDataProp={msg?.feedbackData} msg ={msg} campaignChatId={campaignChatId} message={msg?.bot_response}></ChatLikes>} */}
                                </div>
                            }
                           
                            {/* {msg?.intent === "create_campaign" && ( */}
                              <div className={`${styles.msgbubble} ${styles.left} ${styles.cardsresponse}`}>
                                <div className={styles.cardscontainerscroller}>
                                

                                  

                                  
                                </div >
                              </div>
                            {/* )} */}
                            
                            





                            
                          </>
                        );
                      }
                    //   return null;
                    })}
                    {campaignMessageData?.isTyping && (
                      <>
                        <p className="generatings">
                          Agent is Analysing
                          <div className='typingdot'><span></span><span></span><span></span></div>
                        </p>
                        
                      </>
                    )}
                  </div>
                </div>
                <div className={styles.searchblockouter}>
                  <div style={{ marginTop: "16px" }} className={`${styles.searchblock} ${styles.newinput}`}>
                    <div className={styles.searchmessage}>
                      <div className={styles.searchmessageText}>
                        {uploadedFiles.length > 0 && <div className="attachment-container">
                          {uploadedFiles.map((file, index) => (
                            <React.Fragment key={index}>
                              {file.mimetype.startsWith("image/") ? (
                                <div className="simpleimgcard">
                                  <img
                                    src={file.uploadUrl}
                                    alt={file.filename}
                                    className="file-preview"
                                    style={{
                                      width: "40px",
                                      height: "40px",
                                      borderRadius: "6px",
                                      objectFit: "cover",
                                    }}
                                  />
                                  <svg
                                    onClick={() => removeFile(index)}
                                    className="removeebtn"
                                    width="15"
                                    height="15"
                                    viewBox="0 0 15 15"
                                    fill="none"
                                    xmlns="http://www.w3.org/2000/svg"
                                  >
                                    <path
                                      d="M7.52143 0.857544C3.8411 0.857544 0.857544 3.8411 0.857544 7.52143C0.857544 11.2018 3.8411 14.1853 7.52143 14.1853C11.2018 14.1853 14.1853 11.2018 14.1853 7.52143C14.1853 3.8411 11.2018 0.857544 7.52143 0.857544ZM10.4422 9.66174C10.6375 9.85699 10.6375 10.1733 10.4422 10.3684C10.3446 10.4661 10.2167 10.5149 10.0888 10.5149C9.96095 10.5149 9.83307 10.4661 9.73545 10.3684L7.5215 8.1545L5.30756 10.3684C5.20993 10.4661 5.08205 10.5149 4.95417 10.5149C4.82629 10.5149 4.69841 10.4661 4.60078 10.3684C4.40553 10.1732 4.40553 9.85693 4.60078 9.66174L6.81473 7.4478L4.60078 5.23385C4.40553 5.0386 4.40553 4.72233 4.60078 4.52715C4.79604 4.33196 5.1123 4.3319 5.30749 4.52715L7.52143 6.74109L9.73538 4.52715C9.93063 4.3319 10.2469 4.3319 10.4421 4.52715C10.6373 4.7224 10.6373 5.03867 10.4421 5.23385L8.22814 7.4478L10.4422 9.66174Z"
                                      fill="#484848"
                                    />
                                  </svg>
                                </div>
                              ) : (
                                <div className="attachment-card">
                                  <div className="thumb">
                                    <div className="file-icon">
                                      <img src={file.mimetype.includes("pdf")?assets.pdficon:file.mimetype.includes("txt")?assets.uploaddoc:assets.uploadxlsx} alt="file-icon" />
                                    </div>
                                  </div>
                                  <div className="file-info">
                                    <p className="filename" title={file.filename}>
                                      {file.filename}
                                    </p>
                                    <p className="details">
                                      {file.mimetype.includes("pdf")
                                        ? "PDF"
                                        : file.mimetype.split("/")[1].toUpperCase()}{" "}
                                      • {formatFileSize(file.size)}
                                    </p>
                                  </div>
                                  <svg
                                    onClick={() => removeFile(index)}
                                    className="removeebtn"
                                    width="15"
                                    height="15"
                                    viewBox="0 0 15 15"
                                    fill="none"
                                    xmlns="http://www.w3.org/2000/svg"
                                  >
                                    <path
                                      d="M7.52143 0.857544C3.8411 0.857544 0.857544 3.8411 0.857544 7.52143C0.857544 11.2018 3.8411 14.1853 7.52143 14.1853C11.2018 14.1853 14.1853 11.2018 14.1853 7.52143C14.1853 3.8411 11.2018 0.857544 7.52143 0.857544ZM10.4422 9.66174C10.6375 9.85699 10.6375 10.1733 10.4422 10.3684C10.3446 10.4661 10.2167 10.5149 10.0888 10.5149C9.96095 10.5149 9.83307 10.4661 9.73545 10.3684L7.5215 8.1545L5.30756 10.3684C5.20993 10.4661 5.08205 10.5149 4.95417 10.5149C4.82629 10.5149 4.69841 10.4661 4.60078 10.3684C4.40553 10.1732 4.40553 9.85693 4.60078 9.66174L6.81473 7.4478L4.60078 5.23385C4.40553 5.0386 4.40553 4.72233 4.60078 4.52715C4.79604 4.33196 5.1123 4.3319 5.30749 4.52715L7.52143 6.74109L9.73538 4.52715C9.93063 4.3319 10.2469 4.3319 10.4421 4.52715C10.6373 4.7224 10.6373 5.03867 10.4421 5.23385L8.22814 7.4478L10.4422 9.66174Z"
                                      fill="#484848"
                                    />
                                  </svg>
                                </div>
                              )}
                            </React.Fragment>
                          ))}
                        </div>}
                        <div className={styles.inputcontainer}>
                          <textarea
                            ref={textareaRef}
                            placeholder="Write your Goal & Objective"
                            className="textareamainsnd textareaneww"
                            onInput={handleInput}
                            value={analyticsMessage}
                            onChange={(e) => handelTextArea(e.target.value)}
                            onKeyDown={(e) => {
                              if (!campaignMessageData?.isTyping && !campaignMessageData?.progress && e.key === "Enter" && !e.shiftKey) {
                                e.preventDefault();
                                handleSend();
                              }
                            }}
                            rows={1}
                            style={{ minHeight: "40px", maxHeight: "120px", height: "40px" }}
                          />
                        </div>
                        <div className={`${styles.actionbottom} ${styles.actionbottomtop}`}>
                          <div>
                            {campaignMessageData?.isTyping || campaignMessageData?.progress ? (
                              <img src={assets.passicon} alt="typing" />
                            ) : analyticsMessage && !isRecording ? (
                              <img
                                src={assets.sndarrowchat}
                                alt="send"
                                onClick={handleSend}
                                style={{ cursor: "pointer" , height:"38px"}}
                              />
                            ) : (
                              <>
                                {!isRecording ? 
                                <svg
                                  width="38"
                                  height="38"
                                  viewBox="0 0 38 38"
                                  fill="none"
                                  xmlns="http://www.w3.org/2000/svg"
                                  onClick={micToggle}
                                  aria-pressed={isRecording}
                                >
                                  <path
                                    d="M24.9911 20.2275C24.5907 20.1162 24.1776 20.3427 24.0634 20.7412C23.3202 23.3271 20.9189 25.1338 18.2236 25.1338C15.5507 25.1338 13.1543 23.3457 12.3965 20.7861C12.2803 20.3896 11.8662 20.1601 11.4649 20.2802C11.0674 20.3974 10.8409 20.8144 10.959 21.2118C11.832 24.1635 14.4568 26.268 17.4815 26.5722V28.3261C17.4815 28.7402 17.8174 29.0761 18.2315 29.0761C18.6456 29.0761 18.9815 28.7402 18.9815 28.3261V26.5707C22.0242 26.2595 24.649 24.1343 25.5049 21.1552C25.6192 20.7577 25.3896 20.3417 24.9911 20.2275Z"
                                    fill="#12C3B3"
                                  />
                                  <g filter="url(#filter0_i_273_2583)">
                                    <path
                                      d="M21.9571 17.1995L21.0801 15.1556C21.0332 15.0482 20.9463 14.9603 20.8399 14.9154L18.8399 14.058C18.3741 13.8266 18.1221 13.4291 18.1221 12.973C18.1221 12.5169 18.3741 12.1195 18.795 11.9085L20.8389 11.0315C20.9473 10.9846 21.0332 10.8987 21.0801 10.7893L21.3471 10.1665C20.5066 9.55758 19.4817 9.18848 18.3646 9.18848H18.0982C15.2792 9.18848 12.9941 11.4737 12.9941 14.2926V18.528C12.9941 21.347 15.2793 23.6321 18.0982 23.6321H18.3646C21.1834 23.6321 23.4686 21.3469 23.4686 18.528V17.7708C23.3282 17.8309 23.1805 17.8724 23.0216 17.8724C22.5655 17.8724 22.1681 17.6204 21.9571 17.1995Z"
                                      fill="url(#paint0_linear_273_2583)"
                                    />
                                  </g>
                                  <path
                                    d="M19.175 13.2766C19.0398 13.2095 18.9727 13.1081 18.9727 12.9729C18.9727 12.8384 19.0398 12.7372 19.175 12.6692L21.174 11.8126C21.4829 11.6802 21.7291 11.4341 21.8615 11.1251L22.7181 9.12613C22.7855 8.99163 22.8866 8.92383 23.0218 8.92383C23.1563 8.92383 23.2575 8.99163 23.3255 9.12613L24.1823 11.1252C24.3147 11.4341 24.5608 11.6802 24.8697 11.8126L26.8686 12.6692C27.0031 12.7373 27.0712 12.8384 27.0712 12.9729C27.0712 13.1081 27.0031 13.2095 26.8686 13.2766L24.8697 14.1332C24.5608 14.2656 24.3147 14.5117 24.1823 14.8206L23.3255 16.8197C23.2574 16.9549 23.1563 17.0223 23.0218 17.0223C22.8866 17.0223 22.7854 16.9549 22.7181 16.8197L21.8615 14.8207C21.7291 14.5118 21.483 14.2656 21.174 14.1332L19.175 13.2766Z"
                                    fill="url(#paint1_linear_273_2583)"
                                  />
                                  <defs>
                                    <filter
                                      id="filter0_i_273_2583"
                                      x="12.9941"
                                      y="9.18848"
                                      width="10.4745"
                                      height="16.4436"
                                      filterUnits="userSpaceOnUse"
                                      colorInterpolationFilters="sRGB"
                                    >
                                      <feFlood floodOpacity="0" result="BackgroundImageFix" />
                                      <feBlend
                                        mode="normal"
                                        in="SourceGraphic"
                                        in2="BackgroundImageFix"
                                        result="shape"
                                      />
                                      <feColorMatrix
                                        in="SourceAlpha"
                                        type="matrix"
                                        values="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 127 0"
                                        result="hardAlpha"
                                      />
                                      <feOffset dy="2" />
                                      <feGaussianBlur stdDeviation="2" />
                                      <feComposite in2="hardAlpha" operator="arithmetic" k2="-1" k3="1" />
                                      <feColorMatrix
                                        type="matrix"
                                        values="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0.1 0"
                                      />
                                      <feBlend
                                        mode="normal"
                                        in2="shape"
                                        result="effect1_innerShadow_273_2583"
                                      />
                                    </filter>
                                    <linearGradient
                                      id="paint0_linear_273_2583"
                                      x1="13.9726"
                                      y1="9.18848"
                                      x2="36.7322"
                                      y2="27.1426"
                                      gradientUnits="userSpaceOnUse"
                                    >
                                      <stop offset="0.0337" stopColor="#00E2A4" />
                                      <stop offset="0.1157" stopColor="#00E2A4" />
                                      <stop offset="0.5389" stopColor="#575CFF" />
                                      <stop offset="0.8687" stopColor="#FF793D" />
                                      <stop offset="1" stopColor="#00B885" />
                                    </linearGradient>
                                    <linearGradient
                                      id="paint1_linear_273_2583"
                                      x1="18.3027"
                                      y1="12.9068"
                                      x2="27.2121"
                                      y2="9.92262"
                                      gradientUnits="userSpaceOnUse"
                                    >
                                      <stop stopColor="#EA57FF" />
                                      <stop offset="0.945" stopColor="#FF5300" />
                                    </linearGradient>
                                  </defs>
                                </svg> 
                                : <svg
                                  onClick={micToggle}
                                  width="38" height="38" viewBox="0 0 38 38" fill="none" xmlns="http://www.w3.org/2000/svg">
                                  <rect width="38" height="38" rx="19" fill="#00E2A4" />
                                  <path d="M24.9911 20.2275C24.5907 20.1162 24.1776 20.3427 24.0634 20.7412C23.3202 23.3271 20.9189 25.1338 18.2236 25.1338C15.5507 25.1338 13.1543 23.3457 12.3965 20.7861C12.2803 20.3896 11.8662 20.1601 11.4649 20.2802C11.0674 20.3974 10.8409 20.8144 10.959 21.2118C11.832 24.1635 14.4568 26.268 17.4815 26.5722V28.3261C17.4815 28.7402 17.8174 29.0761 18.2315 29.0761C18.6456 29.0761 18.9815 28.7402 18.9815 28.3261V26.5707C22.0242 26.2595 24.649 24.1343 25.5049 21.1552C25.6192 20.7577 25.3896 20.3417 24.9911 20.2275Z" fill="#F0FAF7" />
                                  <g filter="url(#filter0_i_3036_4344)">
                                    <path d="M21.9571 17.1995L21.0801 15.1556C21.0332 15.0482 20.9463 14.9603 20.8399 14.9154L18.8399 14.058C18.3741 13.8266 18.1221 13.4291 18.1221 12.973C18.1221 12.5169 18.3741 12.1195 18.795 11.9085L20.8389 11.0315C20.9473 10.9846 21.0332 10.8987 21.0801 10.7893L21.3471 10.1665C20.5066 9.55758 19.4817 9.18848 18.3646 9.18848H18.0982C15.2792 9.18848 12.9941 11.4737 12.9941 14.2926V18.528C12.9941 21.347 15.2793 23.6321 18.0982 23.6321H18.3646C21.1834 23.6321 23.4686 21.3469 23.4686 18.528V17.7708C23.3282 17.8309 23.1805 17.8724 23.0216 17.8724C22.5655 17.8724 22.1681 17.6204 21.9571 17.1995Z" fill="#F0FAF7" />
                                  </g>
                                  <path d="M19.175 13.2766C19.0398 13.2095 18.9727 13.1081 18.9727 12.9729C18.9727 12.8384 19.0398 12.7372 19.175 12.6692L21.174 11.8126C21.4829 11.6802 21.7291 11.4341 21.8615 11.1251L22.7181 9.12613C22.7855 8.99163 22.8866 8.92383 23.0218 8.92383C23.1563 8.92383 23.2575 8.99163 23.3255 9.12613L24.1823 11.1252C24.3147 11.4341 24.5608 11.6802 24.8697 11.8126L26.8686 12.6692C27.0031 12.7373 27.0712 12.8384 27.0712 12.9729C27.0712 13.1081 27.0031 13.2095 26.8686 13.2766L24.8697 14.1332C24.5608 14.2656 24.3147 14.5117 24.1823 14.8206L23.3255 16.8197C23.2574 16.9549 23.1563 17.0223 23.0218 17.0223C22.8866 17.0223 22.7854 16.9549 22.7181 16.8197L21.8615 14.8207C21.7291 14.5118 21.483 14.2656 21.174 14.1332L19.175 13.2766Z" fill="#757676" />
                                  <path d="M19.175 13.2766C19.0398 13.2095 18.9727 13.1081 18.9727 12.9729C18.9727 12.8384 19.0398 12.7372 19.175 12.6692L21.174 11.8126C21.4829 11.6802 21.7291 11.4341 21.8615 11.1251L22.7181 9.12613C22.7855 8.99163 22.8866 8.92383 23.0218 8.92383C23.1563 8.92383 23.2575 8.99163 23.3255 9.12613L24.1823 11.1252C24.3147 11.4341 24.5608 11.6802 24.8697 11.8126L26.8686 12.6692C27.0031 12.7373 27.0712 12.8384 27.0712 12.9729C27.0712 13.1081 27.0031 13.2095 26.8686 13.2766L24.8697 14.1332C24.5608 14.2656 24.3147 14.5117 24.1823 14.8206L23.3255 16.8197C23.2574 16.9549 23.1563 17.0223 23.0218 17.0223C22.8866 17.0223 22.7854 16.9549 22.7181 16.8197L21.8615 14.8207C21.7291 14.5118 21.483 14.2656 21.174 14.1332L19.175 13.2766Z" fill="url(#paint0_linear_3036_4344)" />
                                  <defs>
                                    <filter id="filter0_i_3036_4344" x="12.9941" y="9.18848" width="10.4746" height="16.4436" filterUnits="userSpaceOnUse" color-interpolation-filters="sRGB">
                                      <feFlood flood-opacity="0" result="BackgroundImageFix" />
                                      <feBlend mode="normal" in="SourceGraphic" in2="BackgroundImageFix" result="shape" />
                                      <feColorMatrix in="SourceAlpha" type="matrix" values="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 127 0" result="hardAlpha" />
                                      <feOffset dy="2" />
                                      <feGaussianBlur stdDeviation="2" />
                                      <feComposite in2="hardAlpha" operator="arithmetic" k2="-1" k3="1" />
                                      <feColorMatrix type="matrix" values="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0.1 0" />
                                      <feBlend mode="normal" in2="shape" result="effect1_innerShadow_3036_4344" />
                                    </filter>
                                    <linearGradient id="paint0_linear_3036_4344" x1="18.3027" y1="12.9068" x2="27.2121" y2="9.92262" gradientUnits="userSpaceOnUse">
                                      <stop stop-color="#EA57FF" />
                                      <stop offset="0.945" stop-color="#FF5300" />
                                    </linearGradient>
                                  </defs>
                                </svg>}
                              </>
                            )}</div>
                          <div>
                            <input
                              type="file"
                              accept="application/pdf, image/*"
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
                          </div>
                        </div>
                      </div>
                    </div>
                  </div>
                </div>
              </div>}

            {(campaignMessageData?.analyticsMessage?.length > 0 || showMainChat) &&
              <div className={`${styles.msgbody} msgbody`}>
                <div style={{ marginTop: "20px" }} ref={bottomRef} className={`${styles.scroller} ${styles.chatflexscroll}`}>
                  <div className={styles.maincontainer}>
                    
                    {Array.isArray(campaignMessageData?.analyticsMessage) && campaignMessageData?.analyticsMessage?.map((msg: any, index: number) => {
                      if (msg.sender === "userText") {
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
                            {msg?.message?.trim() &&
                            <div key={msg?.messageId} className={`${styles.msgbubble} ${styles.right}`}>
                              <div className="chatContent" dangerouslySetInnerHTML={{ __html: msg?.message }} />
                            </div>
                            }
                          </>
                        );
                      }
                      if (msg.sender === "botText"  && msg?.campaignChatId==campaignId) {
                        let textToShow = msg?.bot_response;
                        

                        return (
                          <>
                            {msg?.progress && (msg?.progress) < "100" ?
                              <>
                                <div className={`${styles.msgbubble} ${styles.left} ${styles.containprogress}`}>
                                  <p>{msg?.progress_message}</p>
                                  <ProgressBar progress={msg?.progress} /></div> </> :
                              <div key={msg?.messageId} className={`${styles.msgbubble} ${styles.left}`}>
                                <div className="chatContent" dangerouslySetInnerHTML={{ __html: textToShow }} />
                               
                                {/* {(msg?.intent !== "refresh_dashboard" && textToShow?.trim()?.length>0)&&<ChatLikes changeindex={index} allConversation ={campaignMessageData?.campaignMessage} likeOrDislike={msg?.likeOrDislike} feedBackDataProp={msg?.feedbackData} msg ={msg} campaignChatId={campaignChatId} message={msg?.bot_response}></ChatLikes>} */}
                                </div>
                            }
                           
                            {/* {msg?.intent === "create_campaign" && ( */}
                              <div className={`${styles.msgbubble} ${styles.left} ${styles.cardsresponse}`}>
                                <div className={styles.cardscontainerscroller}>
                                

                                  

                                  
                                </div >
                              </div>
                            {/* )} */}
                            
                            





                            
                          </>
                        );
                      }
                    //   return null;
                    })}
                    {campaignMessageData?.isTyping && (
                      <>
                        <p className="generatings">
                          Agent is Analysing
                          <div className='typingdot'><span></span><span></span><span></span></div>
                        </p>
                        
                      </>
                    )}
                  </div>
                </div>
                <div className={styles.searchblockouter}>
                  <div style={{ marginTop: "16px" }} className={`${styles.searchblock} ${styles.newinput}`}>
                    <div className={styles.searchmessage}>
                      <div className={styles.searchmessageText}>
                        {uploadedFiles.length > 0 && <div className="attachment-container">
                          {uploadedFiles.map((file, index) => (
                            <React.Fragment key={index}>
                              {file.mimetype.startsWith("image/") ? (
                                <div className="simpleimgcard">
                                  <img
                                    src={file.uploadUrl}
                                    alt={file.filename}
                                    className="file-preview"
                                    style={{
                                      width: "40px",
                                      height: "40px",
                                      borderRadius: "6px",
                                      objectFit: "cover",
                                    }}
                                  />
                                  <svg
                                    onClick={() => removeFile(index)}
                                    className="removeebtn"
                                    width="15"
                                    height="15"
                                    viewBox="0 0 15 15"
                                    fill="none"
                                    xmlns="http://www.w3.org/2000/svg"
                                  >
                                    <path
                                      d="M7.52143 0.857544C3.8411 0.857544 0.857544 3.8411 0.857544 7.52143C0.857544 11.2018 3.8411 14.1853 7.52143 14.1853C11.2018 14.1853 14.1853 11.2018 14.1853 7.52143C14.1853 3.8411 11.2018 0.857544 7.52143 0.857544ZM10.4422 9.66174C10.6375 9.85699 10.6375 10.1733 10.4422 10.3684C10.3446 10.4661 10.2167 10.5149 10.0888 10.5149C9.96095 10.5149 9.83307 10.4661 9.73545 10.3684L7.5215 8.1545L5.30756 10.3684C5.20993 10.4661 5.08205 10.5149 4.95417 10.5149C4.82629 10.5149 4.69841 10.4661 4.60078 10.3684C4.40553 10.1732 4.40553 9.85693 4.60078 9.66174L6.81473 7.4478L4.60078 5.23385C4.40553 5.0386 4.40553 4.72233 4.60078 4.52715C4.79604 4.33196 5.1123 4.3319 5.30749 4.52715L7.52143 6.74109L9.73538 4.52715C9.93063 4.3319 10.2469 4.3319 10.4421 4.52715C10.6373 4.7224 10.6373 5.03867 10.4421 5.23385L8.22814 7.4478L10.4422 9.66174Z"
                                      fill="#484848"
                                    />
                                  </svg>
                                </div>
                              ) : (
                                <div className="attachment-card">
                                  <div className="thumb">
                                    <div className="file-icon">
                                      <img src={file.mimetype.includes("pdf")?assets.pdficon:file.mimetype.includes("txt")?assets.uploaddoc:assets.uploadxlsx} alt="file-icon" />
                                    </div>
                                  </div>
                                  <div className="file-info">
                                    <p className="filename" title={file.filename}>
                                      {file.filename}
                                    </p>
                                    <p className="details">
                                      {file.mimetype.includes("pdf")
                                        ? "PDF"
                                        : file.mimetype.split("/")[1].toUpperCase()}{" "}
                                      • {formatFileSize(file.size)}
                                    </p>
                                  </div>
                                  <svg
                                    onClick={() => removeFile(index)}
                                    className="removeebtn"
                                    width="15"
                                    height="15"
                                    viewBox="0 0 15 15"
                                    fill="none"
                                    xmlns="http://www.w3.org/2000/svg"
                                  >
                                    <path
                                      d="M7.52143 0.857544C3.8411 0.857544 0.857544 3.8411 0.857544 7.52143C0.857544 11.2018 3.8411 14.1853 7.52143 14.1853C11.2018 14.1853 14.1853 11.2018 14.1853 7.52143C14.1853 3.8411 11.2018 0.857544 7.52143 0.857544ZM10.4422 9.66174C10.6375 9.85699 10.6375 10.1733 10.4422 10.3684C10.3446 10.4661 10.2167 10.5149 10.0888 10.5149C9.96095 10.5149 9.83307 10.4661 9.73545 10.3684L7.5215 8.1545L5.30756 10.3684C5.20993 10.4661 5.08205 10.5149 4.95417 10.5149C4.82629 10.5149 4.69841 10.4661 4.60078 10.3684C4.40553 10.1732 4.40553 9.85693 4.60078 9.66174L6.81473 7.4478L4.60078 5.23385C4.40553 5.0386 4.40553 4.72233 4.60078 4.52715C4.79604 4.33196 5.1123 4.3319 5.30749 4.52715L7.52143 6.74109L9.73538 4.52715C9.93063 4.3319 10.2469 4.3319 10.4421 4.52715C10.6373 4.7224 10.6373 5.03867 10.4421 5.23385L8.22814 7.4478L10.4422 9.66174Z"
                                      fill="#484848"
                                    />
                                  </svg>
                                </div>
                              )}
                            </React.Fragment>
                          ))}
                        </div>}
                        <div className={styles.inputcontainer}>
                          <textarea
                            ref={textareaRef}
                            placeholder="Write your Goal & Objective"
                            className="textareamainsnd textareaneww"
                            onInput={handleInput}
                            value={analyticsMessage}
                            onChange={(e) => handelTextArea(e.target.value)}
                            onKeyDown={(e) => {
                              if (!campaignMessageData?.isTyping && !campaignMessageData?.progress && e.key === "Enter" && !e.shiftKey) {
                                e.preventDefault();
                                handleSend();
                              }
                            }}
                            rows={1}
                            style={{ minHeight: "40px", maxHeight: "120px", height: "40px" }}
                          />
                        </div>
                        {/* old code ref */}
                        {/* <div className={`${styles.actionbottom} ${styles.actionbottomtop}`}>
                          <div className={styles.clickableiconpoint}>
                            {campaignMessageData?.isTyping || campaignMessageData?.progress ? (
                              <img src={assets.passicon} alt="typing" />
                            ) :( (analyticsMessage||uploadedFiles.length>0)  && !isRecording) ? (
                              <img
                                src={assets.sndarrowchat}
                                alt="send"
                                onClick={handleSend}
                                
                              />
                            ) : (
                              <>
                                {!isRecording ? 
                                <svg
                                  width="32"
                                  height="32"
                                  viewBox="0 0 32 32"
                                  fill="none"
                                  xmlns="http://www.w3.org/2000/svg"
                                  onClick={micToggle}
                                  aria-pressed={isRecording}
                                >
                                  <path
                                    d="M24.9911 20.2275C24.5907 20.1162 24.1776 20.3427 24.0634 20.7412C23.3202 23.3271 20.9189 25.1338 18.2236 25.1338C15.5507 25.1338 13.1543 23.3457 12.3965 20.7861C12.2803 20.3896 11.8662 20.1601 11.4649 20.2802C11.0674 20.3974 10.8409 20.8144 10.959 21.2118C11.832 24.1635 14.4568 26.268 17.4815 26.5722V28.3261C17.4815 28.7402 17.8174 29.0761 18.2315 29.0761C18.6456 29.0761 18.9815 28.7402 18.9815 28.3261V26.5707C22.0242 26.2595 24.649 24.1343 25.5049 21.1552C25.6192 20.7577 25.3896 20.3417 24.9911 20.2275Z"
                                    fill="#12C3B3"
                                  />
                                  <g filter="url(#filter0_i_273_2583)">
                                    <path
                                      d="M21.9571 17.1995L21.0801 15.1556C21.0332 15.0482 20.9463 14.9603 20.8399 14.9154L18.8399 14.058C18.3741 13.8266 18.1221 13.4291 18.1221 12.973C18.1221 12.5169 18.3741 12.1195 18.795 11.9085L20.8389 11.0315C20.9473 10.9846 21.0332 10.8987 21.0801 10.7893L21.3471 10.1665C20.5066 9.55758 19.4817 9.18848 18.3646 9.18848H18.0982C15.2792 9.18848 12.9941 11.4737 12.9941 14.2926V18.528C12.9941 21.347 15.2793 23.6321 18.0982 23.6321H18.3646C21.1834 23.6321 23.4686 21.3469 23.4686 18.528V17.7708C23.3282 17.8309 23.1805 17.8724 23.0216 17.8724C22.5655 17.8724 22.1681 17.6204 21.9571 17.1995Z"
                                      fill="url(#paint0_linear_273_2583)"
                                    />
                                  </g>
                                  <path
                                    d="M19.175 13.2766C19.0398 13.2095 18.9727 13.1081 18.9727 12.9729C18.9727 12.8384 19.0398 12.7372 19.175 12.6692L21.174 11.8126C21.4829 11.6802 21.7291 11.4341 21.8615 11.1251L22.7181 9.12613C22.7855 8.99163 22.8866 8.92383 23.0218 8.92383C23.1563 8.92383 23.2575 8.99163 23.3255 9.12613L24.1823 11.1252C24.3147 11.4341 24.5608 11.6802 24.8697 11.8126L26.8686 12.6692C27.0031 12.7373 27.0712 12.8384 27.0712 12.9729C27.0712 13.1081 27.0031 13.2095 26.8686 13.2766L24.8697 14.1332C24.5608 14.2656 24.3147 14.5117 24.1823 14.8206L23.3255 16.8197C23.2574 16.9549 23.1563 17.0223 23.0218 17.0223C22.8866 17.0223 22.7854 16.9549 22.7181 16.8197L21.8615 14.8207C21.7291 14.5118 21.483 14.2656 21.174 14.1332L19.175 13.2766Z"
                                    fill="url(#paint1_linear_273_2583)"
                                  />
                                  <defs>
                                    <filter
                                      id="filter0_i_273_2583"
                                      x="12.9941"
                                      y="9.18848"
                                      width="10.4745"
                                      height="16.4436"
                                      filterUnits="userSpaceOnUse"
                                      colorInterpolationFilters="sRGB"
                                    >
                                      <feFlood floodOpacity="0" result="BackgroundImageFix" />
                                      <feBlend
                                        mode="normal"
                                        in="SourceGraphic"
                                        in2="BackgroundImageFix"
                                        result="shape"
                                      />
                                      <feColorMatrix
                                        in="SourceAlpha"
                                        type="matrix"
                                        values="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 127 0"
                                        result="hardAlpha"
                                      />
                                      <feOffset dy="2" />
                                      <feGaussianBlur stdDeviation="2" />
                                      <feComposite in2="hardAlpha" operator="arithmetic" k2="-1" k3="1" />
                                      <feColorMatrix
                                        type="matrix"
                                        values="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0.1 0"
                                      />
                                      <feBlend
                                        mode="normal"
                                        in2="shape"
                                        result="effect1_innerShadow_273_2583"
                                      />
                                    </filter>
                                    <linearGradient
                                      id="paint0_linear_273_2583"
                                      x1="13.9726"
                                      y1="9.18848"
                                      x2="36.7322"
                                      y2="27.1426"
                                      gradientUnits="userSpaceOnUse"
                                    >
                                      <stop offset="0.0337" stopColor="#00E2A4" />
                                      <stop offset="0.1157" stopColor="#00E2A4" />
                                      <stop offset="0.5389" stopColor="#575CFF" />
                                      <stop offset="0.8687" stopColor="#FF793D" />
                                      <stop offset="1" stopColor="#00B885" />
                                    </linearGradient>
                                    <linearGradient
                                      id="paint1_linear_273_2583"
                                      x1="18.3027"
                                      y1="12.9068"
                                      x2="27.2121"
                                      y2="9.92262"
                                      gradientUnits="userSpaceOnUse"
                                    >
                                      <stop stopColor="#EA57FF" />
                                      <stop offset="0.945" stopColor="#FF5300" />
                                    </linearGradient>
                                  </defs>
                                </svg> 
                                : <svg
                                  onClick={micToggle}
                                  width="32" height="32" viewBox="0 0 32 32" fill="none" xmlns="http://www.w3.org/2000/svg">
                                  <rect width="38" height="38" rx="19" fill="#00E2A4" />
                                  <path d="M24.9911 20.2275C24.5907 20.1162 24.1776 20.3427 24.0634 20.7412C23.3202 23.3271 20.9189 25.1338 18.2236 25.1338C15.5507 25.1338 13.1543 23.3457 12.3965 20.7861C12.2803 20.3896 11.8662 20.1601 11.4649 20.2802C11.0674 20.3974 10.8409 20.8144 10.959 21.2118C11.832 24.1635 14.4568 26.268 17.4815 26.5722V28.3261C17.4815 28.7402 17.8174 29.0761 18.2315 29.0761C18.6456 29.0761 18.9815 28.7402 18.9815 28.3261V26.5707C22.0242 26.2595 24.649 24.1343 25.5049 21.1552C25.6192 20.7577 25.3896 20.3417 24.9911 20.2275Z" fill="#F0FAF7" />
                                  <g filter="url(#filter0_i_3036_4344)">
                                    <path d="M21.9571 17.1995L21.0801 15.1556C21.0332 15.0482 20.9463 14.9603 20.8399 14.9154L18.8399 14.058C18.3741 13.8266 18.1221 13.4291 18.1221 12.973C18.1221 12.5169 18.3741 12.1195 18.795 11.9085L20.8389 11.0315C20.9473 10.9846 21.0332 10.8987 21.0801 10.7893L21.3471 10.1665C20.5066 9.55758 19.4817 9.18848 18.3646 9.18848H18.0982C15.2792 9.18848 12.9941 11.4737 12.9941 14.2926V18.528C12.9941 21.347 15.2793 23.6321 18.0982 23.6321H18.3646C21.1834 23.6321 23.4686 21.3469 23.4686 18.528V17.7708C23.3282 17.8309 23.1805 17.8724 23.0216 17.8724C22.5655 17.8724 22.1681 17.6204 21.9571 17.1995Z" fill="#F0FAF7" />
                                  </g>
                                  <path d="M19.175 13.2766C19.0398 13.2095 18.9727 13.1081 18.9727 12.9729C18.9727 12.8384 19.0398 12.7372 19.175 12.6692L21.174 11.8126C21.4829 11.6802 21.7291 11.4341 21.8615 11.1251L22.7181 9.12613C22.7855 8.99163 22.8866 8.92383 23.0218 8.92383C23.1563 8.92383 23.2575 8.99163 23.3255 9.12613L24.1823 11.1252C24.3147 11.4341 24.5608 11.6802 24.8697 11.8126L26.8686 12.6692C27.0031 12.7373 27.0712 12.8384 27.0712 12.9729C27.0712 13.1081 27.0031 13.2095 26.8686 13.2766L24.8697 14.1332C24.5608 14.2656 24.3147 14.5117 24.1823 14.8206L23.3255 16.8197C23.2574 16.9549 23.1563 17.0223 23.0218 17.0223C22.8866 17.0223 22.7854 16.9549 22.7181 16.8197L21.8615 14.8207C21.7291 14.5118 21.483 14.2656 21.174 14.1332L19.175 13.2766Z" fill="#757676" />
                                  <path d="M19.175 13.2766C19.0398 13.2095 18.9727 13.1081 18.9727 12.9729C18.9727 12.8384 19.0398 12.7372 19.175 12.6692L21.174 11.8126C21.4829 11.6802 21.7291 11.4341 21.8615 11.1251L22.7181 9.12613C22.7855 8.99163 22.8866 8.92383 23.0218 8.92383C23.1563 8.92383 23.2575 8.99163 23.3255 9.12613L24.1823 11.1252C24.3147 11.4341 24.5608 11.6802 24.8697 11.8126L26.8686 12.6692C27.0031 12.7373 27.0712 12.8384 27.0712 12.9729C27.0712 13.1081 27.0031 13.2095 26.8686 13.2766L24.8697 14.1332C24.5608 14.2656 24.3147 14.5117 24.1823 14.8206L23.3255 16.8197C23.2574 16.9549 23.1563 17.0223 23.0218 17.0223C22.8866 17.0223 22.7854 16.9549 22.7181 16.8197L21.8615 14.8207C21.7291 14.5118 21.483 14.2656 21.174 14.1332L19.175 13.2766Z" fill="url(#paint0_linear_3036_4344)" />
                                  <defs>
                                    <filter id="filter0_i_3036_4344" x="12.9941" y="9.18848" width="10.4746" height="16.4436" filterUnits="userSpaceOnUse" color-interpolation-filters="sRGB">
                                      <feFlood flood-opacity="0" result="BackgroundImageFix" />
                                      <feBlend mode="normal" in="SourceGraphic" in2="BackgroundImageFix" result="shape" />
                                      <feColorMatrix in="SourceAlpha" type="matrix" values="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 127 0" result="hardAlpha" />
                                      <feOffset dy="2" />
                                      <feGaussianBlur stdDeviation="2" />
                                      <feComposite in2="hardAlpha" operator="arithmetic" k2="-1" k3="1" />
                                      <feColorMatrix type="matrix" values="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0.1 0" />
                                      <feBlend mode="normal" in2="shape" result="effect1_innerShadow_3036_4344" />
                                    </filter>
                                    <linearGradient id="paint0_linear_3036_4344" x1="18.3027" y1="12.9068" x2="27.2121" y2="9.92262" gradientUnits="userSpaceOnUse">
                                      <stop stop-color="#EA57FF" />
                                      <stop offset="0.945" stop-color="#FF5300" />
                                    </linearGradient>
                                  </defs>
                                </svg>}
                              </>
                            )}</div>
                          <div>
                            <input
                              type="file"
                              accept="application/pdf, image/*"
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
                          </div>
                        </div> */}
{/* new code ref */}
                            <div className={`${styles.actionbottom} ${styles.actionbottomtop}`}>
                                                  <div>
                                                    {campaignMessageData?.isTyping || campaignMessageData?.progress ? (
                                                      <img src={assets.passicon} alt="typing" />
                                                    ) : ((analyticsMessage || uploadedFiles?.length>0)&& !isRecording) ? (
                                                      // <img
                                                      //   src={assets.sndarrowchat}
                                                      //   alt="send"
                                                      //   onClick={handleSend}
                                                      //   style={{ cursor: "pointer" , height:"38px"}}
                                                      // />
                                                      <div style={{ cursor: "pointer" , width:"38px", height:"42px",display:"grid" ,   placeItems: "center",}}>
                        
                                                      <svg  onClick={handleSend}  xmlns="http://www.w3.org/2000/svg" width="20" height="17" viewBox="0 0 20 17" fill="none">
                          <path d="M19.5711 0.434772C19.1688 0.0265723 18.5623 -0.107228 18.0247 0.0890722L0.994009 6.25407C0.447109 6.44257 0.0697089 6.91617 0.00860893 7.48947C-0.0514911 8.05197 0.206809 8.58127 0.666809 8.85957L3.71171 10.8254C4.07791 11.0578 4.55351 11.0647 4.92361 10.8381L15.2629 4.06547L5.58281 11.7609C5.38211 11.9328 5.24691 12.1711 5.20191 12.4289L4.75561 14.9562C4.64381 15.589 4.91971 16.1964 5.45921 16.505C5.99631 16.8116 6.64961 16.7501 7.12471 16.3429L9.34741 14.4367L12.761 16.6681C12.9968 16.8224 13.2737 16.9015 13.5535 16.9015C13.7215 16.9015 13.8909 16.8732 14.053 16.8146C14.4876 16.6574 14.8147 16.3156 14.9422 15.9015L19.8797 1.94147C20.0853 1.41507 19.9671 0.837072 19.5711 0.434772Z" fill="black"/>
                        </svg> </div>
                                                    ) : (
                                                      <>
                                                        {!isRecording ? 
                                                        <svg
                                                          width="38"
                                                          height="38"
                                                          viewBox="0 0 38 38"
                                                          fill="none"
                                                          xmlns="http://www.w3.org/2000/svg"
                                                          onClick={micToggle}
                                                          aria-pressed={isRecording}
                                                        >
                                                          <path
                                                            d="M24.9911 20.2275C24.5907 20.1162 24.1776 20.3427 24.0634 20.7412C23.3202 23.3271 20.9189 25.1338 18.2236 25.1338C15.5507 25.1338 13.1543 23.3457 12.3965 20.7861C12.2803 20.3896 11.8662 20.1601 11.4649 20.2802C11.0674 20.3974 10.8409 20.8144 10.959 21.2118C11.832 24.1635 14.4568 26.268 17.4815 26.5722V28.3261C17.4815 28.7402 17.8174 29.0761 18.2315 29.0761C18.6456 29.0761 18.9815 28.7402 18.9815 28.3261V26.5707C22.0242 26.2595 24.649 24.1343 25.5049 21.1552C25.6192 20.7577 25.3896 20.3417 24.9911 20.2275Z"
                                                            fill="#000"
                                                          />
                                                          <g filter="url(#filter0_i_273_2583)">
                                                            <path
                                                              d="M21.9571 17.1995L21.0801 15.1556C21.0332 15.0482 20.9463 14.9603 20.8399 14.9154L18.8399 14.058C18.3741 13.8266 18.1221 13.4291 18.1221 12.973C18.1221 12.5169 18.3741 12.1195 18.795 11.9085L20.8389 11.0315C20.9473 10.9846 21.0332 10.8987 21.0801 10.7893L21.3471 10.1665C20.5066 9.55758 19.4817 9.18848 18.3646 9.18848H18.0982C15.2792 9.18848 12.9941 11.4737 12.9941 14.2926V18.528C12.9941 21.347 15.2793 23.6321 18.0982 23.6321H18.3646C21.1834 23.6321 23.4686 21.3469 23.4686 18.528V17.7708C23.3282 17.8309 23.1805 17.8724 23.0216 17.8724C22.5655 17.8724 22.1681 17.6204 21.9571 17.1995Z"
                                                              fill="url(#paint0_linear_273_2583)"
                                                            />
                                                          </g>
                                                          <path
                                                            d="M19.175 13.2766C19.0398 13.2095 18.9727 13.1081 18.9727 12.9729C18.9727 12.8384 19.0398 12.7372 19.175 12.6692L21.174 11.8126C21.4829 11.6802 21.7291 11.4341 21.8615 11.1251L22.7181 9.12613C22.7855 8.99163 22.8866 8.92383 23.0218 8.92383C23.1563 8.92383 23.2575 8.99163 23.3255 9.12613L24.1823 11.1252C24.3147 11.4341 24.5608 11.6802 24.8697 11.8126L26.8686 12.6692C27.0031 12.7373 27.0712 12.8384 27.0712 12.9729C27.0712 13.1081 27.0031 13.2095 26.8686 13.2766L24.8697 14.1332C24.5608 14.2656 24.3147 14.5117 24.1823 14.8206L23.3255 16.8197C23.2574 16.9549 23.1563 17.0223 23.0218 17.0223C22.8866 17.0223 22.7854 16.9549 22.7181 16.8197L21.8615 14.8207C21.7291 14.5118 21.483 14.2656 21.174 14.1332L19.175 13.2766Z"
                                                            fill="url(#paint1_linear_273_2583)"
                                                          />
                                                          <defs>
                                                            <filter
                                                              id="filter0_i_273_2583"
                                                              x="12.9941"
                                                              y="9.18848"
                                                              width="10.4745"
                                                              height="16.4436"
                                                              filterUnits="userSpaceOnUse"
                                                              colorInterpolationFilters="sRGB"
                                                            >
                                                              <feFlood floodOpacity="0" result="BackgroundImageFix" />
                                                              <feBlend
                                                                mode="normal"
                                                                in="SourceGraphic"
                                                                in2="BackgroundImageFix"
                                                                result="shape"
                                                              />
                                                              <feColorMatrix
                                                                in="SourceAlpha"
                                                                type="matrix"
                                                                values="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 127 0"
                                                                result="hardAlpha"
                                                              />
                                                              <feOffset dy="2" />
                                                              <feGaussianBlur stdDeviation="2" />
                                                              <feComposite in2="hardAlpha" operator="arithmetic" k2="-1" k3="1" />
                                                              <feColorMatrix
                                                                type="matrix"
                                                                values="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0.1 0"
                                                              />
                                                              <feBlend
                                                                mode="normal"
                                                                in2="shape"
                                                                result="effect1_innerShadow_273_2583"
                                                              />
                                                            </filter>
                                                            <linearGradient
                                                              id="paint0_linear_273_2583"
                                                              x1="13.9726"
                                                              y1="9.18848"
                                                              x2="36.7322"
                                                              y2="27.1426"
                                                              gradientUnits="userSpaceOnUse"
                                                            >
                                                              <stop offset="0.0337" stopColor="#000" />
                                                              <stop offset="0.1157" stopColor="#000" />
                                                              <stop offset="0.5389" stopColor="#000" />
                                                              <stop offset="0.8687" stopColor="#FF793D" />
                                                              <stop offset="1" stopColor="#00B885" />
                                                            </linearGradient>
                                                            <linearGradient
                                                              id="paint1_linear_273_2583"
                                                              x1="18.3027"
                                                              y1="12.9068"
                                                              x2="27.2121"
                                                              y2="9.92262"
                                                              gradientUnits="userSpaceOnUse"
                                                            >
                                                              <stop stopColor="#EA57FF" />
                                                              <stop offset="0.945" stopColor="#FF5300" />
                                                            </linearGradient>
                                                          </defs>
                                                        </svg> 
                                                        : <svg
                                                          onClick={micToggle}
                                                          width="38" height="38" viewBox="0 0 38 38" fill="none" xmlns="http://www.w3.org/2000/svg">
                                                          <rect width="38" height="38" rx="19" fill="#7423D7" />
                                                          <path d="M24.9911 20.2275C24.5907 20.1162 24.1776 20.3427 24.0634 20.7412C23.3202 23.3271 20.9189 25.1338 18.2236 25.1338C15.5507 25.1338 13.1543 23.3457 12.3965 20.7861C12.2803 20.3896 11.8662 20.1601 11.4649 20.2802C11.0674 20.3974 10.8409 20.8144 10.959 21.2118C11.832 24.1635 14.4568 26.268 17.4815 26.5722V28.3261C17.4815 28.7402 17.8174 29.0761 18.2315 29.0761C18.6456 29.0761 18.9815 28.7402 18.9815 28.3261V26.5707C22.0242 26.2595 24.649 24.1343 25.5049 21.1552C25.6192 20.7577 25.3896 20.3417 24.9911 20.2275Z" fill="#F0FAF7" />
                                                          <g filter="url(#filter0_i_3036_4344)">
                                                            <path d="M21.9571 17.1995L21.0801 15.1556C21.0332 15.0482 20.9463 14.9603 20.8399 14.9154L18.8399 14.058C18.3741 13.8266 18.1221 13.4291 18.1221 12.973C18.1221 12.5169 18.3741 12.1195 18.795 11.9085L20.8389 11.0315C20.9473 10.9846 21.0332 10.8987 21.0801 10.7893L21.3471 10.1665C20.5066 9.55758 19.4817 9.18848 18.3646 9.18848H18.0982C15.2792 9.18848 12.9941 11.4737 12.9941 14.2926V18.528C12.9941 21.347 15.2793 23.6321 18.0982 23.6321H18.3646C21.1834 23.6321 23.4686 21.3469 23.4686 18.528V17.7708C23.3282 17.8309 23.1805 17.8724 23.0216 17.8724C22.5655 17.8724 22.1681 17.6204 21.9571 17.1995Z" fill="#F0FAF7" />
                                                          </g>
                                                          <path d="M19.175 13.2766C19.0398 13.2095 18.9727 13.1081 18.9727 12.9729C18.9727 12.8384 19.0398 12.7372 19.175 12.6692L21.174 11.8126C21.4829 11.6802 21.7291 11.4341 21.8615 11.1251L22.7181 9.12613C22.7855 8.99163 22.8866 8.92383 23.0218 8.92383C23.1563 8.92383 23.2575 8.99163 23.3255 9.12613L24.1823 11.1252C24.3147 11.4341 24.5608 11.6802 24.8697 11.8126L26.8686 12.6692C27.0031 12.7373 27.0712 12.8384 27.0712 12.9729C27.0712 13.1081 27.0031 13.2095 26.8686 13.2766L24.8697 14.1332C24.5608 14.2656 24.3147 14.5117 24.1823 14.8206L23.3255 16.8197C23.2574 16.9549 23.1563 17.0223 23.0218 17.0223C22.8866 17.0223 22.7854 16.9549 22.7181 16.8197L21.8615 14.8207C21.7291 14.5118 21.483 14.2656 21.174 14.1332L19.175 13.2766Z" fill="#757676" />
                                                          <path d="M19.175 13.2766C19.0398 13.2095 18.9727 13.1081 18.9727 12.9729C18.9727 12.8384 19.0398 12.7372 19.175 12.6692L21.174 11.8126C21.4829 11.6802 21.7291 11.4341 21.8615 11.1251L22.7181 9.12613C22.7855 8.99163 22.8866 8.92383 23.0218 8.92383C23.1563 8.92383 23.2575 8.99163 23.3255 9.12613L24.1823 11.1252C24.3147 11.4341 24.5608 11.6802 24.8697 11.8126L26.8686 12.6692C27.0031 12.7373 27.0712 12.8384 27.0712 12.9729C27.0712 13.1081 27.0031 13.2095 26.8686 13.2766L24.8697 14.1332C24.5608 14.2656 24.3147 14.5117 24.1823 14.8206L23.3255 16.8197C23.2574 16.9549 23.1563 17.0223 23.0218 17.0223C22.8866 17.0223 22.7854 16.9549 22.7181 16.8197L21.8615 14.8207C21.7291 14.5118 21.483 14.2656 21.174 14.1332L19.175 13.2766Z" fill="url(#paint0_linear_3036_4344)" />
                                                          <defs>
                                                            <filter id="filter0_i_3036_4344" x="12.9941" y="9.18848" width="10.4746" height="16.4436" filterUnits="userSpaceOnUse" color-interpolation-filters="sRGB">
                                                              <feFlood flood-opacity="0" result="BackgroundImageFix" />
                                                              <feBlend mode="normal" in="SourceGraphic" in2="BackgroundImageFix" result="shape" />
                                                              <feColorMatrix in="SourceAlpha" type="matrix" values="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 127 0" result="hardAlpha" />
                                                              <feOffset dy="2" />
                                                              <feGaussianBlur stdDeviation="2" />
                                                              <feComposite in2="hardAlpha" operator="arithmetic" k2="-1" k3="1" />
                                                              <feColorMatrix type="matrix" values="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0.1 0" />
                                                              <feBlend mode="normal" in2="shape" result="effect1_innerShadow_3036_4344" />
                                                            </filter>
                                                            <linearGradient id="paint0_linear_3036_4344" x1="18.3027" y1="12.9068" x2="27.2121" y2="9.92262" gradientUnits="userSpaceOnUse">
                                                              <stop stop-color="#EA57FF" />
                                                              <stop offset="0.945" stop-color="#FF5300" />
                                                            </linearGradient>
                                                          </defs>
                                                        </svg>}
                                                      </>
                                                    )}</div>
                                                  <div>
                                                    <input
                                                      type="file"
                                                      accept="application/pdf, image/*"
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
                                                  </div>
                                                </div>
                      </div>
                    </div>
                  </div>
                </div>
              </div>}
          </div >
        </div >
      }
    </>
  )
}

export default CampaignAnalytics;
