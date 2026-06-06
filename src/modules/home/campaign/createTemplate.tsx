import React, { useEffect, useLayoutEffect, useRef, useState } from 'react'
import styles from "./scss/elevatebrand.module.scss"
import { assets, leftnavassets } from '../../../styles/assets/assets'
import ProgressBar from '../../../components/custom_components/progressbar/progressbar'
import WorktualModal from '../../../components/custom_components/worktualModal'
import { v4 as uuidv4 } from "uuid";
import { GetItemFromStorage } from '../../../base/customhooks/useStorage'
import { LocalStorage } from '../../../base/customhooks/localStorageKeys'
import { jwtDecode } from 'jwt-decode'
import { useDispatcher } from '../../../store/redux-store/dispatch'
import { campaignSliceActions } from '../../../store/slice/campaign'
import { useSelector } from 'react-redux'
import SocketService from '../../../base/service/socketService'
import { format } from "date-fns";
import WorktualDropdown from '../../../components/custom_components/WroktualDropdown'
import ClickTooltip from '../../../components/custom_components/clickabletooltip/clickabletooltip'
import { whatsappService } from '../../../base/service/apiservice'
import { useLocation, useNavigate, useSearchParams } from 'react-router-dom'
import { decryptAESCVM, downsampleBuffer, floatTo16BitPCM } from '../../../base/utils'
import Loaders from '../../../components/loader/loader'
import { config } from '../../../base/constant'
import NlpWebSocketService from '../../../base/service/sttNlpSocket'
import ChatLikes from '../../../LikeDislike/chatLikes'


const MAX_HEIGHT = 200;
const MIN_HEIGHT = 40;
const sessionId = uuidv4();

function CreateTemplates({ isCVM, campaignId, isNewChat }: any) {
  const location = useLocation();
  let navigate = useNavigate();
  const { state } = location;
  const pages = ["A", "B", "C", "D", "E"];
  const [active, setActive] = useState("A");
  const [emailActive, setEmailActive] = useState(0);
  const campaignMessageData = useSelector((state: any) => state.campaignData);
  const dispatch = useDispatcher()
  const textareaRef = useRef<HTMLTextAreaElement>(null);
  const bottomRef = useRef<HTMLDivElement | null>(null);
  const editableRef = useRef<HTMLParagraphElement | null>(null);
  const fileInputRef = useRef<HTMLInputElement | null>(null);
  const nlpSocketUrl: any = config().STT_NLP;
  const audioContextRef = useRef<AudioContext | null>(null);
  const sourceRef = useRef<MediaStreamAudioSourceNode | null>(null);
  const processorRef = useRef<ScriptProcessorNode | null>(null);
  const streamRef = useRef<MediaStream | null>(null);
  const textareaId = useRef(uuidv4()).current;
  const textDataRef = useRef('');
  const analyticsRef = useRef<boolean>(false)
  const nlpService = NlpWebSocketService.getInstance();
  const [showSend, setShowSend] = useState(false);
  const userDetails = GetItemFromStorage(LocalStorage?.ACCESS_TOKEN);
  const data: any = jwtDecode(userDetails);
  const [campaignMessage, setCampaignMessage] = useState('');
  const [onEdit, setOnEdit] = useState<any>(false);
  const [editPromptEmail, setEditPromptEmail] = useState<any>(false);
  const [editsPromptSms, setEditPromptSms] = useState<any>(false);
  const [editsPromptInAPP, setEditPromptInAPP] = useState<any>(false);
  const [editsPromptWebPush, setEditPromptWebPush] = useState<any>(false);
  const [editsPromptAppPush, setEditPromptAppPush] = useState<any>(false);
  const [editsPromptWhatsapp, setEditPromptWhatsapp] = useState<any>(false);
  const [editValue, setEditValue] = useState('');
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
  const [webOptions, setWebOptions] = useState<any>(initialWebOption);
  const [appPushOptions, setappPushOptions] = useState<any>(initialPhoneOption);
  const [analyticsData, setAnalyticsData] = useState<any>(null)
  let campaignChatId = localStorage.getItem("campaignChatId");

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

  const removeFile = (index: number) => {
    setUploadedFiles((prev) => prev.filter((_, i) => i !== index));
  };

  useLayoutEffect(() => {
    if (!bottomRef.current) return;
    bottomRef.current.scrollTo({
      top: bottomRef.current.scrollHeight,
      behavior: "smooth",
    });
  }, [campaignMessageData?.campaignMessage?.length]);

  const handleSend = () => {
    try {
      if (!campaignMessage.trim()) return;
      const newMsg = {
        sessionId,
        campaignChatId,
        messageId: uuidv4(),
        message: campaignMessage ?? '',
        uploadUrl: uploadedFiles ?? '',
        sender: "userText",
        date: new Date().toISOString(),
        domainId: data?.domainId,
        ext: +(data?.ext),
        product: campaignMessageData?.product?.type,
        proActive: campaignMessageData?.product?.data?.length > 0 ? 1 : 0
      };
      dispatch(campaignSliceActions.setIsTyping(true));
      const temp_data: any = [...campaignMessageData?.campaignMessage]
      temp_data.push(newMsg)
      dispatch(campaignSliceActions.setCampaignMessage(temp_data));
      console.log("chatType", campaignMessageData?.chatType)
      if (campaignMessageData?.chatType == 'new') {
        SocketService.campaignMessage(newMsg);
      }
      else {
        SocketService.pastCampaignMessage({ ...newMsg, chatType: "analytics" });
      }
      setCampaignMessage("");
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
  };

  const phoneOptions = [
    { label: 'Android', value: 1 },
    { label: 'Ios', value: 2 }
  ]

  const webPushOptions = [
    { label: 'Chrome', value: 1 },
    { label: 'Safari', value: 2 }
  ]
  const formatFileSize = (bytes?: number) => {
    if (!bytes) return "0 KB";
    const sizes = ["Bytes", "KB", "MB", "GB"];
    const i = Math.floor(Math.log(bytes) / Math.log(1024));
    const size = (bytes / Math.pow(1024, i)).toFixed(1);
    return `${size} ${sizes[i]}`;
  };

  useEffect(() => {
    if (campaignMessageData.clearMessage) {
      setCampaignMessage('');
      setUploadedFiles([]);
      dispatch(campaignSliceActions.setClearMessage(false));
    }
  }, [campaignMessageData.clearMessage])

  const [statusViewCard, setStatusViewCard] = useState<boolean>(false);
  const whatsappTemplateData = useSelector((state: any) => state.campaignData.whatsAppCardDetails);
  console.log("whatsappTemplateData", whatsappTemplateData);
  const [showMainChat, setShowMainChat] = useState<any>(false);

  const handleViewEmail = (channelKey: any, channelData: any, index: number) => {
    dispatch(campaignSliceActions.setTempleteView({ channels: channelKey, ...channelData, index }));
    setOnEdit(true);
    setEditValue('');
    setEditPromptEmail('')

  };

  const handleViewSms = (channelKey: any, channelData: any, index: number) => {
    dispatch(campaignSliceActions.setTempleteView({ channels: channelKey, ...channelData, index }));
    setOnEdit(true);
    setEditValue('');
    setEditPromptSms('');
  };

  const handleViewAppPush = (channelKey: any, channelData: any, index: number) => {
    dispatch(campaignSliceActions.setTempleteView({ channels: channelKey, ...channelData, index }));
    setOnEdit(true);
    setEditValue('');
    setEditPromptAppPush('');
  };

  const handleViewWebPush = (channelKey: any, channelData: any, index: number) => {
    dispatch(campaignSliceActions.setTempleteView({ channels: channelKey, ...channelData, index }));
    setOnEdit(true);
    setEditValue('');
    setEditPromptWebPush('');
  };

  const handleViewWhatsapp = (channelKey: any, channelData: any, index: number) => {
    dispatch(campaignSliceActions.setTempleteView({ channels: channelKey, ...channelData, index }));
    setOnEdit(true);
    setEditValue('');
    setEditPromptWhatsapp('');
  };

  const handleViewInapp = (channelKey: any, channelData: any, index: number) => {
    dispatch(campaignSliceActions.setTempleteView({ channels: channelKey, ...channelData, index }));
    setOnEdit(true);
    setEditValue('');
    setEditPromptInAPP('');
  };

  const regenerateSms = (datas: any) => {
    if(campaignMessageData?.isRegenerate){
      return ;
    }
    const newMsg = {
      sessionId,
      campaignChatId,
      messageId: uuidv4(),
      message: campaignMessage ?? '',
      sender: "userText",
      date: new Date().toISOString(),
      domainId: data?.domainId,
      ext: +(data?.ext),
      regenerate: datas,
      chatType: "regenerate"
    };
    SocketService.campaignMessage(newMsg);
    dispatch(campaignSliceActions.setIsRegenerate(true));
  };

  const regenerateInAPP = (datas: any) => {
    if(campaignMessageData?.isRegenerate){
      return ;
    }
    const newMsg = {
      sessionId,
      campaignChatId,
      messageId: uuidv4(),
      message: campaignMessage ?? '',
      sender: "userText",
      date: new Date().toISOString(),
      domainId: data?.domainId,
      ext: +(data?.ext),
      regenerate: datas,
      chatType: "regenerate"
    };
    SocketService.campaignMessage(newMsg);
    dispatch(campaignSliceActions.setIsRegenerate(true));
  };

  const regenerateAppPush = (datas: any) => {
    if(campaignMessageData?.isRegenerate){
      return ;
    }
    const newMsg = {
      sessionId,
      campaignChatId,
      messageId: uuidv4(),
      message: campaignMessage ?? '',
      sender: "userText",
      date: new Date().toISOString(),
      domainId: data?.domainId,
      ext: +(data?.ext),
      regenerate: datas,
      chatType: "regenerate"
    };
    SocketService.campaignMessage(newMsg);
    dispatch(campaignSliceActions.setIsRegenerate(true));
  };

  const regenerateWebPush = (datas: any) => {
    if(campaignMessageData?.isRegenerate){
      return ;
    }
    const newMsg = {
      sessionId,
      campaignChatId,
      messageId: uuidv4(),
      message: campaignMessage ?? '',
      sender: "userText",
      date: new Date().toISOString(),
      domainId: data?.domainId,
      ext: +(data?.ext),
      regenerate: datas,
      chatType: "regenerate"
    };
    SocketService.campaignMessage(newMsg);
    dispatch(campaignSliceActions.setIsRegenerate(true));
  };

  const regenerateWhatsapp = (datas: any) => {
    if(campaignMessageData?.isRegenerate){
      return ;
    }
    const newMsg = {
      sessionId,
      campaignChatId,
      messageId: uuidv4(),
      message: campaignMessage ?? '',
      sender: "userText",
      date: new Date().toISOString(),
      domainId: data?.domainId,
      ext: +(data?.ext),
      regenerate: datas,
      chatType: "regenerate"
    };
    SocketService.campaignMessage(newMsg);
    dispatch(campaignSliceActions.setIsRegenerate(true));
  };

  const regenerateEmail = (datas: any) => {
    if(campaignMessageData?.isRegenerate){
      return ;
    }
    console.log("datas????", datas)
    let regenData = {
      ...datas,
      "template_index": emailActive,
    }
    const newMsg = {
      sessionId,
      campaignChatId,
      messageId: uuidv4(),
      message: campaignMessage ?? '',
      sender: "userText",
      date: new Date().toISOString(),
      domainId: data?.domainId,
      ext: +(data?.ext),
      regenerate: regenData,
      chatType: "regenerate"

    };
    SocketService.campaignMessage(newMsg);
    dispatch(campaignSliceActions.setIsRegenerate(true));
  };

  const editEmailPrompt = (datas: any) => {
    const newMsg = {
      sessionId,
      campaignChatId,
      messageId: uuidv4(),
      message: campaignMessage ?? '',
      sender: "userText",
      date: new Date().toISOString(),
      domainId: data?.domainId,
      ext: +(data?.ext),
      regenerate: { ...datas, "template_index": emailActive },
      editPrompt: editValue ?? '',
      chatType: "editPrompt",
    };
    SocketService.campaignMessage(newMsg);
    dispatch(campaignSliceActions.setIsRegenerate(true));
    setEditValue('');
    setEditPromptEmail('')
  };

  const editPromptSms = (datas: any) => {
    const newMsg = {
      sessionId,
      campaignChatId,
      messageId: uuidv4(),
      message: campaignMessage ?? '',
      sender: "userText",
      date: new Date().toISOString(),
      domainId: data?.domainId,
      ext: +(data?.ext),
      regenerate: datas,
      editPrompt: editValue ?? '',
      chatType: "editPrompt",
    };
    SocketService.campaignMessage(newMsg);
    dispatch(campaignSliceActions.setIsEdit(true));
    setEditValue('');
    setEditPromptSms(false);
  };

  const editPromptInAPP = (datas: any) => {
    const newMsg = {
      sessionId,
      campaignChatId,
      messageId: uuidv4(),
      message: campaignMessage ?? '',
      sender: "userText",
      date: new Date().toISOString(),
      domainId: data?.domainId,
      ext: +(data?.ext),
      regenerate: datas,
      editPrompt: editValue ?? '',
      chatType: "editPrompt",
    };
    SocketService.campaignMessage(newMsg);
    dispatch(campaignSliceActions.setIsEdit(true));
    setEditValue('');
    setEditPromptInAPP(false);
  };

  const editPromptAppPush = (datas: any) => {
    const newMsg = {
      sessionId,
      campaignChatId,
      messageId: uuidv4(),
      message: campaignMessage ?? '',
      sender: "userText",
      date: new Date().toISOString(),
      domainId: data?.domainId,
      ext: +(data?.ext),
      regenerate: datas,
      editPrompt: editValue ?? '',
      chatType: "editPrompt",
    };
    SocketService.campaignMessage(newMsg);
    dispatch(campaignSliceActions.setIsEdit(true));
    setEditValue('');
    setEditPromptAppPush(false);
  };

  const editPromptWebPush = (datas: any) => {
    const newMsg = {
      sessionId,
      campaignChatId,
      messageId: uuidv4(),
      message: campaignMessage ?? '',
      sender: "userText",
      date: new Date().toISOString(),
      domainId: data?.domainId,
      ext: +(data?.ext),
      regenerate: datas,
      editPrompt: editValue ?? '',
      chatType: "editPrompt",
    };
    SocketService.campaignMessage(newMsg);
    dispatch(campaignSliceActions.setIsEdit(true));
    setEditValue('');
    setEditPromptWebPush(false);
  };

  const editPromptWhatsapp = (datas: any) => {
    const newMsg = {
      sessionId,
      campaignChatId,
      messageId: uuidv4(),
      message: campaignMessage ?? '',
      sender: "userText",
      date: new Date().toISOString(),
      domainId: data?.domainId,
      ext: +(data?.ext),
      regenerate: datas,
      editPrompt: editValue ?? '',
      chatType: "editPrompt",
    };
    SocketService.campaignMessage(newMsg);
    dispatch(campaignSliceActions.setIsEdit(true));
    setEditValue('');
    setEditPromptWhatsapp(false);
  };

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
    setCampaignMessage(text)
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

  const handleEmailClick = (page: any) => {
    if (page == '+' && emailActive < campaignMessageData?.templeteView?.template?.length - 1) {
      setActive(pages[emailActive + 1]);
      setEmailActive(emailActive + 1)
    }
    else if (page == '-' && emailActive > 0) {
      setActive(pages[emailActive - 1]);
      setEmailActive(emailActive - 1)
    }
  };

  const handleButtonTemplateClick =(ind :any)=>{
    setActive(pages[ind]);
    setEmailActive(ind)
  }

  const handleWhatsappView = (data: any) => {
    setStatusViewCard(true);
  }

  useEffect(() => {
    if (!isNewChat) {
      analyticsRef.current = false;
      // const newMsg = {
      //   sessionId,
      //   campaignChatId: campaignMessageData?.pastCampaignDetails?.CampaignChatId,
      //   messageId: uuidv4(),
      //   message: `give me the brief about the open rate and click rate of this campaign`,
      //   sender: "userText",
      //   date: new Date().toISOString(),
      //   domainId: data?.domainId,
      //   ext: +(data?.ext),
      //   chatType: "analytics",
      // };
      // SocketService.pastCampaignMessage({ ...newMsg, campaignDetail: campaignMessageData?.pastCampaignDetails });
      getAnalyticsData();
    }
    const interval = setInterval(() => {
      if (!isNewChat)
        getAnalyticsData();
    }, 30000);
    if (isNewChat)
      clearInterval(interval);
    speechSynthesis.cancel();
    return () => clearInterval(interval);
  }, [campaignId]);

  const getAnalyticsData = async () => {
    try {
      let body: any = {
        campaignId: campaignId?.CampaignChatId
      }
      await whatsappService.post(body, `/get_individual_analytics`)
        .then((response) => {
          if (response?.data?.statusCode === 200 && response?.data?.response?.length > 0) {
            const initialMsg = {
              sessionId,
              campaignChatId: campaignMessageData?.pastCampaignDetails?.CampaignChatId,
              messageId: uuidv4(),
              message: `open rate and click rate`,
              sender: "botText",
              date: new Date().toISOString(),
              domainId: data?.domainId,
              ext: +(data?.ext),
              chatType: "analytics",
              intent: "refresh_dashboard",
              analytics_data: {
                "delivered": response?.data?.response?.[6]?.[0]?.TotalDeliveryCount ?? 0,
                "openRate": response?.data?.response?.[6]?.[0]?.TotalOpenRate ?? 0,
                // "clickRate": response?.data?.response?.[6]?.[0]?.TotalDeliveryCount ? ((response?.data?.response[6]?.[0]?.TotalClickRate * 100) / response?.data?.response[6]?.[0]?.TotalCustomerCount).toFixed(2) : 0
                "clickRate": response?.data?.response[6]?.[0]?.TotalClickRate??0
              },
              percentDelivered: Math.round(((response?.data?.response?.[6]?.[0]?.TotalDeliveryCount * 100) / (response?.data?.response?.[6]?.[0]?.TotalCustomerCount || 1))) || 0,
              percentOpened: Math.round(((response?.data?.response?.[6]?.[0]?.TotalOpenRate * 100) / (response?.data?.response?.[6]?.[0]?.TotalCustomerCount || 1))) || 0,
              percentClicked: Math.round(((response?.data?.response?.[6]?.[0]?.TotalClickRate * 100) / (response?.data?.response?.[6]?.[0]?.TotalCustomerCount || 1))) || 0,
            };

            setAnalyticsData({
              analytics_data: {
                "delivered": response?.data?.response?.[6]?.[0]?.TotalDeliveryCount ?? 0,
                "openRate": response?.data?.response?.[6]?.[0]?.TotalOpenRate ?? 0,
                "clickRate": response?.data?.response?.[6]?.[0]?.TotalDeliveryCount ? ((response?.data?.response[6]?.[0]?.TotalClickRate * 100) / response?.data?.response[6]?.[0]?.TotalDeliveryCount).toFixed(2) : 0
              },
              percentDelivered: ((response?.data?.response?.[6]?.[0]?.TotalDeliveryCount * 100) / (response?.data?.response?.[6]?.[0]?.TotalDeliveryCount || 1)) || 0,
              percentOpened: ((response?.data?.response?.[6]?.[0]?.TotalOpenRate * 100) / (response?.data?.response?.[6]?.[0]?.TotalDeliveryCount || 1)) || 0,
              percentClicked: ((response?.data?.response?.[6]?.[0]?.TotalClickRate * 100) / (response?.data?.response?.[6]?.[0]?.TotalDeliveryCount || 1)) || 0,
            })
            dispatch(campaignSliceActions.setCampaignMessage([initialMsg, ...campaignMessageData?.campaignMessage.slice(1)]));
          }
        }).catch((error) => {
          console.log(error)
        })
    } catch (error) {
      console.log("error", error);
    }
  }


  useEffect(() => {
    try {
      let timeoutId: any;
      const init = async () => {
        if (campaignMessageData?.product?.type == "CVM" && (campaignMessageData?.product?.query?.trim()?.length > 0 || campaignMessageData?.product?.cvmanalytics?.trim()?.length > 0)) {
          console.log("data", campaignMessageData)
          const newMsg = {
            sessionId,
            campaignChatId,
            messageId: uuidv4(),
            message: campaignMessageData?.product?.query?.trim()?.length > 0 ? campaignMessageData?.product?.query : campaignMessageData?.product?.cvmanalytics?.trim()?.length > 0 ? campaignMessageData?.product?.cvmanalytics : "",
            sender: "userText",
            date: new Date().toISOString(),
            domainId: data?.domainId,
            ext: +(data?.ext),
            product: campaignMessageData?.product?.type,
            cvmCampaigngoal: campaignMessageData?.product?.data,
            proActive: campaignMessageData?.product?.data?.length > 0 ? 1 : 0,
          };
          SocketService.campaignMessage(newMsg);
        }
        if (campaignMessageData?.product?.data || campaignMessageData?.product?.cvmanalytics || state?.data) {
          setShowMainChat(true);
        }
        timeoutId = setTimeout(() => {
          setShowMainChat(false);
        }, 15000);
        if (state?.data && campaignMessageData?.product?.type != "CVM") {
          dispatch(campaignSliceActions.reset());
          console.log("cretecampaign")
          const newMsg = {
            sessionId,
            campaignChatId,
            messageId: uuidv4(),
            message: state?.data ?? '',
            sender: "userText",
            date: new Date().toISOString(),
            domainId: data?.domainId,
            ext: +(data?.ext),
            product: campaignMessageData?.product?.type,
            proActive: campaignMessageData?.product?.data?.length > 0 ? 1 : 0,
            campaignAnalytics: 1
          };
          SocketService.campaignMessage(newMsg);
          navigate(location.pathname, { replace: true, state: {} });
        }
      }
      init();

      if (timeoutId) {
        return () => clearTimeout(timeoutId);
      }

    } catch (error) {
      console.log("err", error);
    }
    finally {

    }
  }, [campaignMessageData?.pastCampaignDetails?.CampaignChatId])

  useEffect(() => {
    if (state?.data) {
      dispatch(campaignSliceActions.setIsTyping(true));
    }
  }, [state?.data])


  return (
    <>
      {loader ? <div className='loadersinner'><Loaders /></div> :
        <div className={`${styles.elevatebrandbody} createcampaignhome ${styles.createcampaignbody}`}>
          <div className={`${styles.innerbody} ${isCVM ? "innerbodyiframe" : ""}`}>
            {campaignMessageData?.campaignMessage?.length === 0 && isNewChat && !showMainChat &&
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
                            value={campaignMessage}
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
                            {campaignMessage && !isRecording ? (
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
            {(campaignMessageData?.campaignMessage?.length > 0 || showMainChat) &&
              <div className={`${styles.msgbody} msgbody`}>
                <div style={{ marginTop: "20px" }} ref={bottomRef} className={`${styles.scroller} ${styles.chatflexscroll}`}>
                  <div className={styles.maincontainer}>
                     {/* <div className={styles.lgbgblock}>
                                          <div className={`${styles.actionsblock} ${styles.actionblocknew} ${styles.smsactionblock}`}>
                                            {editsPromptSms &&
                                              <p
                                                contentEditable
                                                suppressContentEditableWarning={true}
                                                ref={editableRef}
                                                onInput={(e) => {
                                                  const el = e.target as HTMLElement;
                                                  const text = el.innerText.replace(/\u200B/g, "").trim();
                                                  setEditValue(text);
                                                  if (!text) el.innerHTML = "";
                                                }}
                                                data-placeholder={!editValue && editsPromptSms ? "Enter prompt..." : ""}
                                                className={`${styles.editable} inptfocusnone`}
                                              />
                                            }
                                            <div className={styles.btncontainer}>
                                              {!editsPromptSms && <button className="whitebtnnew" onClick={setEditPromptSms}>
                                                <img src={assets.editPrompt} alt="Edit" />
                                                Edit prompt</button>}
                                              {!editsPromptSms && <button className="whitebtnnew" onClick={() => regenerateSms(campaignMessageData?.templeteView)}>
                                                <img src={assets.regenerate} alt="Edit" />
                                                Regenerate</button>}
                                              {editsPromptSms && editValue && (
                                                <button
                                                  className="primarybtn"
                                                  onClick={() => editPromptSms(campaignMessageData?.templeteView)}
                                                >
                                                  Make Changes
                                                </button>
                                              )}
                                              {editsPromptSms && editValue && <button className='cancelbtn' onClick={() => {
                                                setEditValue("");
                                                setEditPromptSms(false);
                                                if (editableRef.current) editableRef.current.innerText = ""
                                              }}>Cancel</button>}
                                            </div>
                                          </div>
                                        </div> */}
                     {/* <div className={`${styles.emailsmsent} stickyheader `}>
                                          <div className={styles.leftcampaign}>
                                            <h3>AI Campaign manager - Email</h3>
                                            <p>{`Created on: ${format(new Date(), "dd MMM, yyyy • hh:mm a")}`}</p>
                                          </div>
                                          <div className={styles.rightcampaign}>
                                            <button className={styles.schedulebtn} onClick={() => setOnEdit(false)}>
                                              Close
                                            </button>
                                          </div>
                                        </div>
                                        <div className={`${styles.campaignmangerbody} ${styles.scrollerinner} `}>
                                          <div className={`${styles.scroller} ${styles.scrollerfix} ${styles.emailscroller}`}>
                                            {campaignMessageData?.isRegenerate ? (
                                              <div className="spinnerbody" style={{ height: "320px" }}>
                                                <div className="spinwithtxt">
                                                  <div className="lds-spinner">
                                                    <div></div><div></div><div></div><div></div><div></div><div></div>
                                                    <div></div><div></div><div></div><div></div><div></div><div></div>
                                                  </div>
                                                  <span>AI is generating the message</span>
                                                </div>
                                              </div>
                                            ) : (
                                              <div className={styles.responseslider}>
                                                {campaignMessageData?.templeteView?.template?.length > 1 &&
                                                  <button className={styles.arrow} onClick={() => handleEmailClick('-')}><svg xmlns="http://www.w3.org/2000/svg" width="6" height="13" viewBox="0 0 6 13" fill="none" >
                                                    <path d="M5.15277 12.0655C4.96137 12.0655 4.76947 11.9923 4.62297 11.8467L0.758268 7.99325C0.332468 7.56845 0.0976688 7.00305 0.0971688 6.40045C0.0966688 5.79785 0.331568 5.23245 0.758268 4.80575L4.62347 0.953248C4.91647 0.661248 5.39157 0.661248 5.68397 0.955248C5.97647 1.24825 5.97547 1.72285 5.68197 2.01575L1.81727 5.86825C1.67517 6.00985 1.59707 6.19935 1.59707 6.40045C1.59707 6.60065 1.67517 6.78915 1.81727 6.93075L5.68247 10.7842C5.97547 11.0772 5.97637 11.5518 5.68397 11.8447C5.53747 11.9922 5.34507 12.0655 5.15277 12.0655Z" fill="black" />
                                                  </svg></button>
                                                }
                                                <div className={styles.response}>
                                                  <div>{`subject: ${campaignMessageData?.templeteView?.template?.[emailActive]?.subject || ""}`}</div>
                                                  <div
                                                    className="chatContent"
                                                    dangerouslySetInnerHTML={{
                                                      __html: campaignMessageData?.templeteView?.template?.[emailActive]?.template || "",
                                                    }}
                                                  />
                                                </div>
                                                {campaignMessageData?.templeteView?.template?.length > 1 &&
                                                  <button className={styles.arrow} onClick={() => handleEmailClick('+')}><svg xmlns="http://www.w3.org/2000/svg" width="6" height="13" viewBox="0 0 6 13" fill="none" >
                                                    <path d="M0.847607 12.1654C0.655207 12.1654 0.462808 12.0922 0.316408 11.9447C0.0239078 11.6517 0.0249076 11.1771 0.318408 10.8842L4.18271 7.03171C4.32481 6.89011 4.40291 6.70061 4.40291 6.49951C4.40291 6.29931 4.32481 6.11081 4.18271 5.96921L0.318008 2.11571C0.025008 1.82271 0.0241079 1.34811 0.316508 1.05521C0.608508 0.761309 1.08361 0.761309 1.37751 1.05321L5.24181 4.90671C5.66761 5.33151 5.90241 5.89691 5.90291 6.49951C5.90341 7.10211 5.66851 7.66751 5.24181 8.09421L1.37711 11.9467C1.23111 12.0922 1.03901 12.1654 0.847607 12.1654Z" fill="black" />
                                                  </svg></button>
                                                }

                                              </div>
                                            )}
                                          </div>

                                         
                                          <div className={styles.lgbgblock}>
                                            <div className={styles.footerPagination}>
                                               {campaignMessageData?.templeteView?.template?.length > 1 &&
                                               <>
                                              {pages.map((page: any, ind: any) => (
                                                (ind <= campaignMessageData?.templeteView?.template?.length - 1) &&
                                                <button
                                                  key={page}
                                                  className={`${styles.page} ${active === page ? styles.active : ""}`}
                                                  onClick={()=>handleButtonTemplateClick(ind)}
                                                >
                                                  {page}
                                                </button>

                                              ))}
                                              </>
                                            }
                                            </div>
                                            <div className={`${styles.actionsblock} ${styles.actionblocknew}`}>
                                              {editPromptEmail &&
                                                <p
                                                  contentEditable
                                                  suppressContentEditableWarning={true}
                                                  ref={editableRef}
                                                  onInput={(e) => {
                                                    const el = e.target as HTMLElement;
                                                    const text = el.innerText.replace(/\u200B/g, "").trim();

                                                    setEditValue(text);

                                                    if (!text) {
                                                      el.innerHTML = "";
                                                    }
                                                  }}
                                                  data-placeholder={
                                                    !editValue && editPromptEmail ? "Enter prompt..." : ""
                                                  }
                                                  className={`${styles.editable} inptfocusnone`}
                                                />
                                              }
                                              <div className={styles.btncontainer}>
                                                {!editPromptEmail && <button className="whitebtnnew" onClick={setEditPromptEmail}>
                                                  <img src={assets.editPrompt} alt="Edit" />
                                                  Edit prompt</button>}
                                                {!editPromptEmail && <button className="whitebtnnew" onClick={() => regenerateEmail(campaignMessageData?.templeteView)}>
                                                  <img src={assets.regenerate} alt="Edit" />
                                                  Regenerate</button>}
                                                {editPromptEmail && editValue && (
                                                  <>
                                                    <button
                                                      className="primarybtn"
                                                      onClick={() => editEmailPrompt(campaignMessageData?.templeteView)}
                                                    >
                                                      Make Changes
                                                    </button>
                                                    <button
                                                      className='cancelbtn'
                                                      onClick={() => {
                                                        setEditValue("");
                                                        setEditPromptEmail(false);
                                                        if (editableRef.current)
                                                          editableRef.current.innerText = ""
                                                      }}>Cancel</button>
                                                  </>
                                                )}
                                              </div>
                                            </div>
                                          </div>

                                        </div> */}
                    {Array.isArray(campaignMessageData?.campaignMessage) && campaignMessageData?.campaignMessage?.map((msg: any, index: number) => {
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
                            <div key={msg?.messageId} className={`${styles.msgbubble} ${styles.right}`}>
                              <div className="chatContent" dangerouslySetInnerHTML={{ __html: msg?.message }} />
                            </div>
                          </>
                        );
                      }
                    //   if (msg.sender === "botText") {
                    //     let textToShow = msg?.bot_response;
                        

                        // return (
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
                                

                                  {/* Email Modal */}
                                  {/* {onEdit && campaignMessageData?.templeteView?.channels === "email" && ( */}
                                    {/* <div className='closetting lgbgmodal'>
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
                                            <button className={styles.schedulebtn} onClick={() => setOnEdit(false)}>
                                              Close
                                            </button>
                                          </div>
                                        </div>
                                        <div className={`${styles.campaignmangerbody} ${styles.scrollerinner} `}>
                                          <div className={`${styles.scroller} ${styles.scrollerfix} ${styles.emailscroller}`}>
                                            {campaignMessageData?.isRegenerate ? (
                                              <div className="spinnerbody" style={{ height: "320px" }}>
                                                <div className="spinwithtxt">
                                                  <div className="lds-spinner">
                                                    <div></div><div></div><div></div><div></div><div></div><div></div>
                                                    <div></div><div></div><div></div><div></div><div></div><div></div>
                                                  </div>
                                                  <span>AI is generating the message</span>
                                                </div>
                                              </div>
                                            ) : (
                                              <div className={styles.responseslider}>
                                                {campaignMessageData?.templeteView?.template?.length > 1 &&
                                                  <button className={styles.arrow} onClick={() => handleEmailClick('-')}><svg xmlns="http://www.w3.org/2000/svg" width="6" height="13" viewBox="0 0 6 13" fill="none" >
                                                    <path d="M5.15277 12.0655C4.96137 12.0655 4.76947 11.9923 4.62297 11.8467L0.758268 7.99325C0.332468 7.56845 0.0976688 7.00305 0.0971688 6.40045C0.0966688 5.79785 0.331568 5.23245 0.758268 4.80575L4.62347 0.953248C4.91647 0.661248 5.39157 0.661248 5.68397 0.955248C5.97647 1.24825 5.97547 1.72285 5.68197 2.01575L1.81727 5.86825C1.67517 6.00985 1.59707 6.19935 1.59707 6.40045C1.59707 6.60065 1.67517 6.78915 1.81727 6.93075L5.68247 10.7842C5.97547 11.0772 5.97637 11.5518 5.68397 11.8447C5.53747 11.9922 5.34507 12.0655 5.15277 12.0655Z" fill="black" />
                                                  </svg></button>
                                                }
                                                <div className={styles.response}>
                                                  <div>{`subject: ${campaignMessageData?.templeteView?.template?.[emailActive]?.subject || ""}`}</div>
                                                  <div
                                                    className="chatContent"
                                                    dangerouslySetInnerHTML={{
                                                      __html: campaignMessageData?.templeteView?.template?.[emailActive]?.template || "",
                                                    }}
                                                  />
                                                </div>
                                                {campaignMessageData?.templeteView?.template?.length > 1 &&
                                                  <button className={styles.arrow} onClick={() => handleEmailClick('+')}><svg xmlns="http://www.w3.org/2000/svg" width="6" height="13" viewBox="0 0 6 13" fill="none" >
                                                    <path d="M0.847607 12.1654C0.655207 12.1654 0.462808 12.0922 0.316408 11.9447C0.0239078 11.6517 0.0249076 11.1771 0.318408 10.8842L4.18271 7.03171C4.32481 6.89011 4.40291 6.70061 4.40291 6.49951C4.40291 6.29931 4.32481 6.11081 4.18271 5.96921L0.318008 2.11571C0.025008 1.82271 0.0241079 1.34811 0.316508 1.05521C0.608508 0.761309 1.08361 0.761309 1.37751 1.05321L5.24181 4.90671C5.66761 5.33151 5.90241 5.89691 5.90291 6.49951C5.90341 7.10211 5.66851 7.66751 5.24181 8.09421L1.37711 11.9467C1.23111 12.0922 1.03901 12.1654 0.847607 12.1654Z" fill="black" />
                                                  </svg></button>
                                                }

                                              </div>
                                            )}
                                          </div>
                                          
                                         
                                          <div className={styles.lgbgblock}>
                                            <div className={styles.footerPagination}>
                                               {campaignMessageData?.templeteView?.template?.length > 1 &&
                                               <>
                                              {pages.map((page: any, ind: any) => (
                                                (ind <= campaignMessageData?.templeteView?.template?.length - 1) &&
                                                <button
                                                  key={page}
                                                  className={`${styles.page} ${active === page ? styles.active : ""}`}
                                                  onClick={()=>handleButtonTemplateClick(ind)}
                                                >
                                                  {page}
                                                </button>

                                              ))}
                                              </>
                                            }
                                            </div>
                                            <div className={`${styles.actionsblock} ${styles.actionblocknew}`}>
                                              {editPromptEmail &&
                                                <p
                                                  contentEditable
                                                  suppressContentEditableWarning={true}
                                                  ref={editableRef}
                                                  onInput={(e) => {
                                                    const el = e.target as HTMLElement;
                                                    const text = el.innerText.replace(/\u200B/g, "").trim();

                                                    setEditValue(text);

                                                    if (!text) {
                                                      el.innerHTML = "";
                                                    }
                                                  }}
                                                  data-placeholder={
                                                    !editValue && editPromptEmail ? "Enter prompt..." : ""
                                                  }
                                                  className={`${styles.editable} inptfocusnone`}
                                                />
                                              }
                                              <div className={styles.btncontainer}>
                                                {!editPromptEmail && <button className="whitebtnnew" onClick={setEditPromptEmail}>
                                                  <img src={assets.editPrompt} alt="Edit" />
                                                  Edit prompt</button>}
                                                {!editPromptEmail && <button className="whitebtnnew" onClick={() => regenerateEmail(campaignMessageData?.templeteView)}>
                                                  <img src={assets.regenerate} alt="Edit" />
                                                  Regenerate</button>}
                                                {editPromptEmail && editValue && (
                                                  <>
                                                    <button
                                                      className="primarybtn"
                                                      onClick={() => editEmailPrompt(campaignMessageData?.templeteView)}
                                                    >
                                                      Make Changes
                                                    </button>
                                                    <button
                                                      className='cancelbtn'
                                                      onClick={() => {
                                                        setEditValue("");
                                                        setEditPromptEmail(false);
                                                        if (editableRef.current)
                                                          editableRef.current.innerText = ""
                                                      }}>Cancel</button>
                                                  </>
                                                )}
                                              </div>
                                            </div>
                                          </div>

                                        </div>
                                      </WorktualModal>
                                    </div> */}
                                  {/* )} */}

                                  {/* SMS Modal */}
                                  {/* {onEdit && campaignMessageData?.templeteView?.channels === "sms" && ( */}
                                    <div className='closetting lgbgmodal smstmode' >
                                      <WorktualModal show={true} mdsize="modalxxl" footer={
                                        <div className={styles.lgbgblock}>
                                          <div className={`${styles.actionsblock} ${styles.actionblocknew} ${styles.smsactionblock}`}>
                                            {/* {editsPromptSms && */}
                                              <p
                                                contentEditable
                                                suppressContentEditableWarning={true}
                                                ref={editableRef}
                                                onInput={(e) => {
                                                  const el = e.target as HTMLElement;
                                                  const text = el.innerText.replace(/\u200B/g, "").trim();
                                                  setEditValue(text);
                                                  if (!text) el.innerHTML = "";
                                                }}
                                                data-placeholder={!editValue && editsPromptSms ? "Enter prompt..." : ""}
                                                className={`${styles.editable} inptfocusnone`}
                                              />
                                            {/* } */}
                                            <div className={styles.btncontainer}>
                                              {!editsPromptSms && <button className="whitebtnnew" onClick={setEditPromptSms}>
                                                <img src={assets.editPrompt} alt="Edit" />
                                                Edit prompt</button>}
                                              {!editsPromptSms && <button className="whitebtnnew" onClick={() => regenerateSms(campaignMessageData?.templeteView)}>
                                                <img src={assets.regenerate} alt="Edit" />
                                                Regenerate</button>}
                                              {editsPromptSms && editValue && (
                                                <button
                                                  className="primarybtn"
                                                  onClick={() => editPromptSms(campaignMessageData?.templeteView)}
                                                >
                                                  Make Changes
                                                </button>
                                              )}
                                              {editsPromptSms && editValue && <button className='cancelbtn' onClick={() => {
                                                setEditValue("");
                                                setEditPromptSms(false);
                                                if (editableRef.current) editableRef.current.innerText = ""
                                              }}>Cancel</button>}
                                            </div>
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
                                                {campaignMessageData?.isRegenerate || campaignMessageData?.isEdit ? (
                                                  <div className="spinnerbody" style={{ height: "320px" }}>
                                                    <div className="spinwithtxt">
                                                      <div className="lds-spinner">
                                                        <div></div><div></div><div></div><div></div><div></div><div></div>
                                                        <div></div><div></div><div></div><div></div><div></div><div></div>
                                                      </div>
                                                      <span>AI is generating the message</span>
                                                    </div>
                                                  </div>
                                                ) : (
                                                  <div className={styles.contentsec}>
                                                    <div className={styles.msgsec}>
                                                      <div dangerouslySetInnerHTML={{ __html: campaignMessageData?.templeteView?.template?.[0]?.template }} />
                                                    </div>
                                                  </div>
                                                )}
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
                                  {/* )} */}

                                  {/* IN-APP Modal */}
                                  {/* {onEdit && campaignMessageData?.templeteView?.channels === "inapp" && ( */}
                                    <div className='closetting lgbgmodal smstmode' >
                                      <WorktualModal show={true} mdsize="modalxxl" footer={
                                        <div className={styles.lgbgblock}>
                                          <div className={`${styles.actionsblock} ${styles.actionblocknew} ${styles.smsactionblock}`}>
                                            {editsPromptInAPP &&
                                              <p
                                                contentEditable
                                                suppressContentEditableWarning={true}
                                                ref={editableRef}
                                                onInput={(e) => {
                                                  const el = e.target as HTMLElement;
                                                  const text = el.innerText.replace(/\u200B/g, "").trim();
                                                  setEditValue(text);
                                                  if (!text) el.innerHTML = "";
                                                }}
                                                data-placeholder={!editValue && editsPromptInAPP ? "Enter prompt..." : ""}
                                                className={`${styles.editable} inptfocusnone`}
                                              />
                                            }
                                            <div className={styles.btncontainer}>
                                              {!editsPromptInAPP && <button className="whitebtnnew" onClick={setEditPromptInAPP}>
                                                <img src={assets.editPrompt} alt="Edit" />
                                                Edit prompt</button>}
                                              {!editsPromptInAPP && <button className="whitebtnnew" onClick={() => regenerateInAPP(campaignMessageData?.templeteView)}>
                                                <img src={assets.regenerate} alt="Edit" />
                                                Regenerate</button>}
                                              {editsPromptInAPP && editValue && (
                                                <button
                                                  className="primarybtn"
                                                  onClick={() => editPromptInAPP(campaignMessageData?.templeteView)}
                                                >
                                                  Make Changes
                                                </button>
                                              )}
                                              {editsPromptInAPP && editValue && <button className='cancelbtn' onClick={() => {
                                                setEditValue("");
                                                setEditPromptInAPP(false);
                                                if (editableRef.current) editableRef.current.innerText = ""
                                              }}>Cancel</button>}
                                            </div>
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
                                                {campaignMessageData?.isRegenerate || campaignMessageData?.isEdit ? (
                                                  <div className="spinnerbody" style={{ height: "320px" }}>
                                                    <div className="spinwithtxt">
                                                      <div className="lds-spinner">
                                                        <div></div><div></div><div></div><div></div><div></div><div></div>
                                                        <div></div><div></div><div></div><div></div><div></div><div></div>
                                                      </div>
                                                      <span>AI is generating the message</span>
                                                    </div>
                                                  </div>
                                                ) : (
                                                  <div className={styles.contentsec}>
                                                    <div>
                                                      {/* <img src={assets.multi_close} alt="" /> */}
                                                      <img src={campaignMessageData?.templeteView?.template?.[0]?.imageUrl} alt="" />
                                                    </div>
                                                    <div className={styles.bigsaleummer}>
                                                      <div dangerouslySetInnerHTML={{ __html: campaignMessageData?.templeteView?.template?.[0]?.template }} />
                                                      {/* <h3>Big Sale summer</h3>
                                                    <p>fhdgfgdjfdfdfdfdgsgdsadsadsa djsadjksaj </p>
                                                    <p>fhdgfgdjfdfdfdfdgsgdsadsadsa djsadjksaj </p> */}
                                                    </div>
                                                  </div>
                                                )}
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
                                  {/* )} */}

                                  {/* App push*/}
                                  {/* {onEdit && campaignMessageData?.templeteView?.channels === "apppush" && ( */}
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
                                            {editsPromptAppPush &&
                                              <p
                                                contentEditable
                                                suppressContentEditableWarning={true}
                                                ref={editableRef}
                                                onInput={(e) => {
                                                  const el = e.target as HTMLElement;
                                                  const text = el.innerText.replace(/\u200B/g, "").trim();
                                                  setEditValue(text);
                                                  if (!text) el.innerHTML = "";
                                                }}
                                                data-placeholder={!editValue && editsPromptAppPush ? "Enter prompt..." : ""}
                                                className={`${styles.editable} inptfocusnone`}
                                              />
                                            }
                                            <div className={styles.btncontainer}>
                                              {!editsPromptAppPush && <button className="whitebtnnew" onClick={setEditPromptAppPush}>
                                                <img src={assets.editPrompt} alt="Edit" />
                                                Edit prompt</button>}
                                              {!editsPromptAppPush && <button className="whitebtnnew" onClick={() => regenerateAppPush(campaignMessageData?.templeteView)}>
                                                <img src={assets.regenerate} alt="Edit" />
                                                Regenerate</button>}
                                              {editsPromptAppPush && editValue && (
                                                <button
                                                  className="primarybtn"
                                                  onClick={() => editPromptAppPush(campaignMessageData?.templeteView)}
                                                >
                                                  Make Changes
                                                </button>
                                              )}
                                              {editsPromptAppPush && editValue && <button className='cancelbtn' onClick={() => {
                                                setEditValue("");
                                                setEditPromptAppPush(false);
                                                if (editableRef.current) editableRef.current.innerText = ""
                                              }}>Cancel</button>}
                                            </div>
                                          </div>
                                        </div>
                                      </>}>

                                        <div className={`${styles.discoversub} ${styles.scrollerinner} ${styles.heightsetmodalsub}`}>
                                          <div className={`${styles.mobilemainsec} ${styles.mobilemargin}`}>
                                            {campaignMessageData?.isRegenerate || campaignMessageData?.isEdit ? (
                                              <div className="spinnerbody" style={{ height: "320px" }}>
                                                <div className="spinwithtxt">
                                                  <div className="lds-spinner">
                                                    <div></div><div></div><div></div><div></div><div></div><div></div>
                                                    <div></div><div></div><div></div><div></div><div></div><div></div>
                                                  </div>
                                                  <span>AI is generating the message</span>
                                                </div>
                                              </div>
                                            ) : (
                                              <div className={styles.contentsec}>
                                                <div className={styles.discoversub}>
                                                  <div className={`${styles.mobilelayoutviewsec} ${styles.mobilelayoutapppushnotify} ${appPushOptions?.value == 1 ? styles.lockscreenandroid : styles.lockscreeniphone} `}>
                                                    <div className={styles.bodywrapper}>
                                                      <div className={styles.pushnotifications}>
                                                        <img className={styles.notifylogo} src={assets.notify_logo} alt="notifylogo" />
                                                        <div className={styles.pushnotifycenter}>
                                                          <p className={styles.notifytxt}>
                                                            <div dangerouslySetInnerHTML={{ __html: campaignMessageData?.templeteView?.template?.[0]?.template }} />
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
                                            )}
                                          </div>
                                          {/* actions */}
                                        </div>

                                      </WorktualModal>
                                    </div>
                                  {/* )} */}

                                  {/* web push */}
                                  {/* {onEdit && campaignMessageData?.templeteView?.channels === "webpush" && ( */}
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
                                              {editsPromptWebPush &&
                                                <p
                                                  contentEditable
                                                  suppressContentEditableWarning={true}
                                                  ref={editableRef}
                                                  onInput={(e) => {
                                                    const el = e.target as HTMLElement;
                                                    const text = el.innerText.replace(/\u200B/g, "").trim();
                                                    setEditValue(text);
                                                    if (!text) el.innerHTML = "";
                                                  }}
                                                  data-placeholder={!editValue && editsPromptWebPush ? "Enter prompt..." : ""}
                                                  className={`${styles.editable} inptfocusnone`}
                                                />
                                              }
                                              <div className={styles.btncontainer}>
                                                {!editsPromptWebPush && <button className="whitebtnnew" onClick={setEditPromptWebPush}>
                                                  <img src={assets.editPrompt} alt="Edit" />
                                                  Edit prompt</button>}
                                                {!editsPromptWebPush && <button className="whitebtnnew" onClick={() => regenerateWebPush(campaignMessageData?.templeteView)}>
                                                  <img src={assets.regenerate} alt="Edit" />
                                                  Regenerate</button>}
                                                {editsPromptWebPush && editValue && (
                                                  <button
                                                    className="primarybtn"
                                                    onClick={() => editPromptWebPush(campaignMessageData?.templeteView)}
                                                  >
                                                    Make Changes
                                                  </button>
                                                )}
                                                {editsPromptWebPush && editValue && <button className='cancelbtn' onClick={() => {
                                                  setEditValue("");
                                                  setEditPromptWebPush(false);
                                                  if (editableRef.current) editableRef.current.innerText = ""
                                                }}>Cancel</button>}
                                              </div>
                                            </div>
                                          </div>
                                        </>
                                      }>

                                        <div className={`${styles.discoversub} ${styles.scrollerinner} ${styles.heightsetmodal}`}>
                                          {/* sms preview layout */}
                                          {/* <div className={styles.mobilelayoutviewsec}> */}
                                          {/* <div className={styles.bodywrapper}> */}
                                          {/* <div className={styles.mobileheader}>
                                              <img className={styles.topbarimg} src={assets.mobile_topbar} alt="topbarimg" />
                                              <img className={styles.camerafront} src={assets.camera_front} alt="camerafront" />
                                            </div> */}
                                          {/* <div className={`${styles.mobilemainsec} ${styles.mobilemargin}`}> */}
                                          {/* <div className={styles.userinfotop}>
                                                <span className={styles.backicon}>←</span>
                                                <div className={styles.contactinfo}>
                                                  <span className={styles.avatorcon}>
                                                    <img src={assets.avatarimg} alt="avatar" />
                                                  </span>                                              
                                                </div>
                                              </div> */}
                                          {campaignMessageData?.isRegenerate || campaignMessageData?.isEdit ? (
                                            <div className="spinnerbody" style={{ height: "320px" }}>
                                              <div className="spinwithtxt">
                                                <div className="lds-spinner">
                                                  <div></div><div></div><div></div><div></div><div></div><div></div>
                                                  <div></div><div></div><div></div><div></div><div></div><div></div>
                                                </div>
                                                <span>AI is generating the message</span>
                                              </div>
                                            </div>
                                          ) : (
                                            <>
                                              {/* <div dangerouslySetInnerHTML={{ __html: campaignMessageData?.templeteView?.template?.[0]?.template }} /> */}
                                              {/* Chrome */}
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
                                                        <div dangerouslySetInnerHTML={{ __html: campaignMessageData?.templeteView?.template?.[0]?.template }} />
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
                                                          <div dangerouslySetInnerHTML={{ __html: campaignMessageData?.templeteView?.template?.[0]?.template }} />
                                                        </div>
                                                      </div>
                                                    </div>
                                                  </div>
                                                </div>
                                              }
                                            </>
                                          )}
                                          {/* actions */}
                                        </div>
                                      </WorktualModal>
                                    </div>
                                  {/* )} */}

                                  {/* Whatsapp */}
                                  {/* {onEdit && campaignMessageData?.templeteView?.channels === "whatsapp" && ( */}
                                    <div className='closetting lgbgmodal smstmode footerpadnone' >
                                      <WorktualModal show={onEdit} mdsize="modalxxl" footer={
                                        <div style={{ position: "relative" }} className={styles.lgbgblock}>
                                          <div className={`${styles.actionsblock} ${styles.actionblocknew} ${styles.smsactionblock}`}>
                                            {editsPromptWhatsapp &&
                                              <p
                                                contentEditable
                                                suppressContentEditableWarning={true}
                                                ref={editableRef}
                                                onInput={(e) => {
                                                  const el = e.target as HTMLElement;
                                                  const text = el.innerText.replace(/\u200B/g, "").trim();
                                                  setEditValue(text);
                                                  if (!text) el.innerHTML = "";
                                                }}
                                                data-placeholder={!editValue && editsPromptWhatsapp ? "Enter prompt..." : ""}
                                                className={`${styles.editable} inptfocusnone`}
                                              />
                                            }
                                            <div className={styles.btncontainer}>
                                              {!editsPromptWhatsapp && <button className="whitebtnnew" onClick={setEditPromptWhatsapp}>
                                                <img src={assets.editPrompt} alt="Edit" />
                                                Edit prompt</button>}
                                              {!editsPromptWhatsapp && <button className="whitebtnnew" onClick={() => regenerateWhatsapp(campaignMessageData?.templeteView)}>
                                                <img src={assets.regenerate} alt="Edit" />
                                                Regenerate</button>}
                                              {editsPromptWhatsapp && editValue && (
                                                <button
                                                  className="primarybtn"
                                                  onClick={() => editPromptWhatsapp(campaignMessageData?.templeteView)}
                                                >
                                                  Make Changes
                                                </button>
                                              )}
                                              {editsPromptWhatsapp && editValue && <button className='cancelbtn' onClick={() => {
                                                setEditValue("");
                                                setEditPromptWhatsapp(false);
                                                if (editableRef.current) editableRef.current.innerText = ""
                                              }}>Cancel</button>}
                                            </div>
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
                                                {campaignMessageData?.isRegenerate || campaignMessageData?.isEdit ? (
                                                  <div className="spinnerbody" style={{ height: "320px" }}>
                                                    <div className="spinwithtxt">
                                                      <div className="lds-spinner">
                                                        <div></div><div></div><div></div><div></div><div></div><div></div>
                                                        <div></div><div></div><div></div><div></div><div></div><div></div>
                                                      </div>
                                                      <span>AI is generating the message</span>
                                                    </div>
                                                  </div>
                                                ) : (
                                                  <div className={`${styles.contentsec} ${styles.contentsecwhtapp}`}>
                                                    <div className={styles.msgsec}>
                                                      {/* Header */}
                                                      {campaignMessageData?.templeteView?.template?.[0]?.headerType === "Image" && (
                                                        <div className={styles.imgcon}>
                                                          <img
                                                            src={campaignMessageData?.templeteView?.template?.[0]?.headerValue}
                                                            alt="header image"
                                                          />
                                                        </div>
                                                      )}
                                                      {campaignMessageData?.templeteView?.template?.[0]?.headerType === "Text" && (
                                                        <div className={styles.headertxtwhtsapp}>

                                                          <p>{campaignMessageData?.templeteView?.template?.[0]?.headerValue}</p>
                                                        </div>
                                                      )}
                                                      {/* Body */}
                                                      <div
                                                        className={styles.bodymessagewrapper}
                                                        dangerouslySetInnerHTML={{
                                                          __html:
                                                            campaignMessageData?.templeteView?.template?.[0]?.bodymessage ?? "",
                                                        }}
                                                      />
                                                      {/* Footer */}
                                                      <div
                                                        className={styles.footerwrapper}
                                                        dangerouslySetInnerHTML={{
                                                          __html: campaignMessageData?.templeteView?.template?.[0]?.footer ?? "",
                                                        }}
                                                      />
                                                      {/* Buttons */}
                                                      {campaignMessageData?.templeteView?.template?.[0]?.buttonType?.map(
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
                                                )}
                                              </div>
                                              <div className={styles.mobilefooter}>
                                                <img src={assets.message_bar} alt="msgbar" />
                                              </div>
                                            </div>
                                          </div>
                                        </div>
                                      </WorktualModal>
                                    </div>
                                  {/* )} */}
                                </div >
                              </div>
                            {/* )} */}
                            
                            





                            
                          </>
                    //     );
                    //   }
                    //   return null;
                    })}
                    {/* {campaignMessageData?.isTyping && (
                      <>
                        <p className="generatings">
                          Agent is Analysing
                          <div className='typingdot'><span></span><span></span><span></span></div>
                        </p>
                        
                      </>
                    )} */}
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
                            value={campaignMessage}
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
                            ) : campaignMessage && !isRecording ? (
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
          </div >
        </div >
      }
    </>
  )
}

export default CreateTemplates;