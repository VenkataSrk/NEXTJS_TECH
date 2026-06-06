'use client';

import { useState, useRef, useEffect } from 'react';
import styles from './scss/agenteditor.module.scss';
import { whatsappService } from '../../base/service/apiservice';
import { assets } from '../../styles/assets/assets';
import React from 'react';
import { v4 as uuidv4 } from "uuid";
import { GetItemFromStorage } from '../../base/customhooks/useStorage';
import { jwtDecode } from 'jwt-decode';
import { LocalStorage } from '../../base/customhooks/localStorageKeys';
import { knowledgeDataSliceActions } from '../../store/slice/knowledgeBaseChat';
import { useDispatcher } from '../../store/redux-store/dispatch';
import SocketService from '../../base/service/socketService';
import { useDispatch, useSelector } from 'react-redux';
import Ailoadericon from './ailoadericon';
import { MyplanProfileActions } from '../../store/slice/myplan/profile';
import { SpeachToText } from './speachToText';
import aiMyPlanSocketService from '../../base/service/aiMypPanSocketService';
// import Aicon from '../../aiccaas/assets/images/agentworkspace/aicon.svg';
// import workspacecloseicon from '../assets/images/agentworkspace/workspacecloseicon.svg';

export default function Agenteditor({ disableEditor,layoutName,handleEnablePagination }: any) {
  const dispatch = useDispatch();
  const botResponse: any = useSelector((state: any) => state.userExtension.botResponse);
  const userDetails = GetItemFromStorage(LocalStorage?.ACCESS_TOKEN);
  const data: any = jwtDecode(userDetails);
  const sessionIdRef = useRef<any>(uuidv4());
  let sessionId = sessionIdRef.current;
  const [hideAI, setHideAI] = useState(true);
  const editorRef = useRef<HTMLDivElement>(null);
  const [micOpen, setMicOpen] = useState(false);
  const fileInputRef = useRef<HTMLInputElement | null>(null);
  const knowledgeMessageData = useSelector((state: any) => state.knowledgeBaseData);
  const [isFocused, setIsFocused] = useState(false);
  const [isEmpty, setIsEmpty] = useState(true);
  const [uploadedFiles, setUploadedFiles] = useState<any[]>([]);
  console.log("uploadedfiles",uploadedFiles)
  const [saveFiles, setSaveFiles] = useState<any[]>([]);
  const placeholderText = 'Type your message...';

  const handlePaste = (e: React.ClipboardEvent<HTMLDivElement>) => {
        e.preventDefault(); // Stop the browser from pasting styled content

        const text = e.clipboardData.getData('text/plain'); // Only get plain text

        // Insert plain text at cursor position
        const selection = window.getSelection();
        if (!selection?.rangeCount) return;

        selection.deleteFromDocument();
        selection.getRangeAt(0).insertNode(document.createTextNode(text));
        selection.collapseToEnd();
    };

  const handleInput = () => {
    const editor = editorRef.current;
    if (!editor) return;
    const text = editor.innerText || "";
    setIsEmpty(text.trim() === "");
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
            uniqueId:uuidv4()
          };
           if (layoutName == "operationsHub"){

             setUploadedFiles((prev: any) => [...prev, fileData]);
           }
            if (layoutName == "planLayout"){
              setSaveFiles((prev :any)=>[...prev,fileData]);
            }
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

  const removeFile = (index: number) => {
     console.log("indedremove",index,layoutName)
    if (layoutName=="operationsHub") {
      console.log("indedremove",index)
      setUploadedFiles((prev) => prev.filter((_, i) => i !== index));
    }
    if (layoutName =="planLayout") {
      console.log("indedremove",index)
      setSaveFiles((prev: any) => prev.filter((_: any, i: any) => i !== index));
    }
   
  };

  const formatFileSize = (bytes?: number) => {
    if (!bytes) return "0 KB";
    const sizes = ["Bytes", "KB", "MB", "GB"];
    const i = Math.floor(Math.log(bytes) / Math.log(1024));
    const size = (bytes / Math.pow(1024, i)).toFixed(1);
    return `${size} ${sizes[i]}`;
  };
  const [sendMessage, setSendMessage] = useState<any>('')
  const [editor, setEditor] = useState(false)

  const handleSend = () => {
   
    try {
      console.log("layout???",editorRef.current?.innerText,layoutName)
      if (layoutName == "operationsHub") {

        const knowledgeMessage = editorRef.current?.innerText || '';
        if (knowledgeMessage.trim().length === 0) return;

        if (editorRef.current) editorRef.current.innerText = '';
        setIsEmpty(true);
        if (!knowledgeMessage.trim()) return;
        setEditor(true)
        setSendMessage(knowledgeMessage);
        const newMsg = {
          sessionId,
          messageId: uuidv4(),
          message: knowledgeMessage,
          sender: "userText",
          date: new Date().toISOString(),
          domainId: data?.domainId,
          ext: +(data?.ext),
          uploadUrl: uploadedFiles ?? '',
          onboardactive:3,
          token:userDetails
        };

        dispatch(knowledgeDataSliceActions.setIsTyping(true));
        SocketService.knowledgeMessage(newMsg);
        setUploadedFiles([]);
        if (editorRef?.current?.innerText) {
          editorRef.current.innerText = '';
        }
      } else if (layoutName == "planLayout") {
       
        setEditor(true)
        let html = editorRef.current?.innerText || '';
        if (html?.trim()?.length <= 0) {
          return;
        }
        setSendMessage(html);
          
       
       
        // dispatch(MyplanProfileActions.setMyplanBotResponse(null));
         console.log("layout12333???",editorRef.current?.innerText)
        const editor = editorRef.current;
        if (!editor) return;
 console.log("layout1233???",editorRef.current?.innerText)
        const text = editor.innerText.trim();
        const hasText = !!text;
        const hasFile = saveFiles?.length > 0;
        console.log("layout123???",editorRef.current?.innerText)
        if (!hasText) return;
        aiMyPlanSocketService.io.emit("agent_chat_request", html)
        if (editorRef.current) {
          editorRef.current.innerHTML = '';
          editorRef.current.focus();
        }
      }
    } catch (error) {
      console.log(error)
    }
};

  const setSpeechText = (newText: string) => {
        setIsEmpty(false)
        const el = editorRef.current;
        if (!el) return;
        el.innerText = newText
        el.focus();

        const range = document.createRange();
        const selection = window.getSelection();

        range.selectNodeContents(el);
        range.collapse(false);

        if (selection) {
            selection.removeAllRanges();
            selection.addRange(range);
        }

    };


  const handleKeyDown = (e: React.KeyboardEvent<HTMLDivElement>) => {
      if (e.key === 'Enter' && !e.shiftKey) {
          e.preventDefault();
          handleSend()
      }
  };

  // useEffect(() => {
  //   try {
      
  //     if (!hideAI && layoutName!="planLayout") {
  //       sessionIdRef.current=uuidv4();
  //       sessionId = sessionIdRef.current;
  //       // dispatch(knowledgeDataSliceActions.reset());
  //       const newMsg = {
  //         sessionId,
  //         messageId: uuidv4(),
  //         message: "operationhub",
  //         sender: "userText",
  //         date: new Date().toISOString(),
  //         domainId: data?.domainId,
  //         ext: +(data?.ext),
  //         uploadUrl: uploadedFiles?? '',
  //         onboardactive:3,
  //         token:userDetails
  //       };
  //       SocketService.knowledgeMessage(newMsg);
  //       dispatch(knowledgeDataSliceActions.setIsTyping(true));
  //     }
  //   } catch (error) {
  //     console.log("ERROR:",error)
  //   }
  // }, [hideAI])

  useEffect(() => {
    try {
      setHideAI(true);
      setSendMessage(null);
      setEditor(false);
      handleEnablePagination(true);
      dispatch(MyplanProfileActions.setMyplanBotResponse(null));
    } catch (error) {
      console.log("Error:",error)
    }
      
  }, [disableEditor])




  return (
    <div className={`${styles.overalleditor}`} style={{ maxWidth: '798px', padding: '24px 0' , margin:"auto" }}>
      {hideAI ? (
        <div className={styles.editbox}>
          <p onClick={() => {setHideAI(false); handleEnablePagination(false);}} style={{ cursor: 'pointer' }}>
            <svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 20 20" fill="none">
  <g clip-path="url(#clip0_3081_28011)">
    <path d="M8.47723 16.482C7.64004 16.7394 6.73765 16.8314 5.8108 16.7231C2.61808 16.3505 0.236922 13.726 0.0692516 10.6217C0.064594 10.5332 0.0611009 10.4436 0.0599365 10.3539C0.0785666 9.45852 0.24158 7.22407 1.32212 5.22949C1.1137 5.90134 1.18356 6.66634 1.44438 7.32305C1.92527 8.53633 2.87657 9.28968 3.87677 10.0116C4.64992 10.5705 5.44985 11.1108 6.0856 11.8292C7.25114 13.1484 8.09299 14.7623 8.47839 16.482H8.47723Z" fill="url(#paint0_linear_3081_28011)"/>
    <path d="M16.8666 10.9727C17.1484 11.8017 17.266 12.7018 17.1845 13.6309C16.9027 16.833 14.3481 19.2886 11.2508 19.546C11.1623 19.5541 11.0727 19.5599 10.9842 19.5623C10.0876 19.5693 7.84968 19.4715 5.82483 18.4491C6.5025 18.6378 7.264 18.5469 7.91372 18.2675C9.11187 17.7528 9.83844 16.7794 10.5312 15.7594C11.0669 14.9711 11.585 14.1549 12.2836 13.4994C13.5691 12.2966 15.1585 11.4081 16.8655 10.9727H16.8666Z" fill="url(#paint1_linear_3081_28011)"/>
    <path d="M11.472 3.24445C12.308 2.98363 13.2104 2.88815 14.1373 2.99295C17.3311 3.35274 19.7228 5.96793 19.9044 9.071C19.9102 9.1595 19.9137 9.24915 19.9137 9.33881C19.8986 10.2342 19.7449 12.4698 18.6725 14.4691C18.8774 13.7961 18.8052 13.0322 18.5421 12.3755C18.0566 11.1646 17.1018 10.4147 16.0992 9.69627C15.3249 9.14087 14.5215 8.60409 13.8834 7.88799C12.7121 6.57341 11.8644 4.96307 11.4708 3.24445H11.472Z" fill="url(#paint2_linear_3081_28011)"/>
    <path d="M10.0035 16.5046C10.7603 16.9459 11.594 17.2254 12.5209 17.329C15.7148 17.6888 18.6292 15.6733 19.4967 12.689C19.5211 12.604 19.5456 12.5178 19.5654 12.4305C19.7505 11.5537 20.4235 9.53233 19.499 7.15234C19.5491 7.85446 19.308 8.5822 18.9052 9.16439C18.1623 10.2368 17.0643 10.7549 15.9267 11.2312C15.0476 11.6003 14.1452 11.9449 13.3627 12.5003C11.9282 13.5215 10.7673 14.9164 10.0023 16.5035L10.0035 16.5046Z" fill="url(#paint3_linear_3081_28011)"/>
    <path d="M3.21642 8.56908C2.94629 7.73538 2.84149 6.83415 2.93697 5.90614C3.26533 2.70876 5.8549 0.290343 8.95565 0.0772613C9.04414 0.0714394 9.13379 0.0667819 9.22345 0.0644531C10.12 0.070275 12.3556 0.200685 14.3665 1.25328C13.6912 1.05534 12.9285 1.13568 12.2753 1.40465C11.069 1.90184 10.3296 2.86478 9.62167 3.8743C9.07441 4.65443 8.54578 5.46368 7.83668 6.10874C6.53374 7.29292 4.93272 8.15805 3.21875 8.56791L3.21642 8.56908Z" fill="url(#paint4_linear_3081_28011)"/>
    <path d="M16.4601 9.95615C16.8932 9.19464 17.1948 8.30506 17.2903 7.37705C17.6175 4.17966 15.5717 1.28618 12.5792 0.448994C12.4942 0.424542 12.4069 0.402419 12.3196 0.382624C11.4405 0.206803 9.41328 -0.446413 7.0426 0.50372C7.74356 0.446665 8.47479 0.679541 9.06047 1.07659C10.141 1.80899 10.6696 2.90118 11.1587 4.03412C11.5359 4.90973 11.8899 5.80863 12.4535 6.58411C13.4886 8.00814 14.8649 9.20861 16.4601 9.95731V9.95615Z" fill="url(#paint5_linear_3081_28011)"/>
    <path d="M10.0631 3.26415C9.30742 2.82052 8.41434 2.50265 7.48749 2.39436C4.29476 2.02059 1.37217 4.02565 0.493069 7.00646C0.467452 7.09146 0.444165 7.17763 0.423206 7.26496C0.234577 8.14057 -0.446585 10.1596 0.468617 12.5431C0.420877 11.841 0.665397 11.1132 1.0706 10.5345C1.81813 9.46563 2.9173 8.95098 4.05723 8.4794C4.9375 8.11379 5.84222 7.77262 6.62585 7.22071C8.06502 6.20654 9.29228 4.85003 10.0654 3.26531L10.0631 3.26415Z" fill="url(#paint6_linear_3081_28011)"/>
    <path d="M3.59024 9.80469C3.16757 10.572 2.89394 11.4406 2.81243 12.3698C2.53065 15.5719 4.61839 18.4351 7.62365 19.2292C7.70865 19.2525 7.79597 19.2734 7.8833 19.292C8.76474 19.4551 10.8012 20.078 13.1579 19.0953C12.4581 19.1628 11.7234 18.9404 11.1319 18.5515C10.0409 17.8354 9.49597 16.7502 8.99179 15.6254C8.60173 14.7556 8.23495 13.8614 7.65974 13.0941C6.60365 11.6852 5.19475 10.5313 3.58907 9.80469H3.59024Z" fill="url(#paint7_linear_3081_28011)"/>
  </g>
  <defs>
    <linearGradient id="paint0_linear_3081_28011" x1="0.793792" y1="3.01168" x2="4.26998" y2="16.1204" gradientUnits="userSpaceOnUse">
      <stop stop-color="#6C00FF"/>
      <stop offset="0.52" stop-color="#4702D1"/>
      <stop offset="1" stop-color="#250E83"/>
    </linearGradient>
    <linearGradient id="paint1_linear_3081_28011" x1="10.9423" y1="14.6754" x2="16.5581" y2="16.3055" gradientUnits="userSpaceOnUse">
      <stop stop-color="#6C00FF"/>
      <stop offset="0.52" stop-color="#4400CB"/>
      <stop offset="1" stop-color="#1E0099"/>
    </linearGradient>
    <linearGradient id="paint2_linear_3081_28011" x1="17.8097" y1="10.9748" x2="15.6568" y2="5.80259" gradientUnits="userSpaceOnUse">
      <stop offset="0.24" stop-color="#1E0099"/>
      <stop offset="0.81" stop-color="#6C00FF"/>
    </linearGradient>
    <linearGradient id="paint3_linear_3081_28011" x1="16.8826" y1="19.2254" x2="16.8826" y2="5.38249" gradientUnits="userSpaceOnUse">
      <stop stop-color="#00F9FF"/>
      <stop offset="0.5" stop-color="#6C00FF"/>
    </linearGradient>
    <linearGradient id="paint4_linear_3081_28011" x1="3.68683" y1="4.14444" x2="10.0269" y2="4.55197" gradientUnits="userSpaceOnUse">
      <stop stop-color="#6C00FF"/>
      <stop offset="0.52" stop-color="#4400CB"/>
      <stop offset="1" stop-color="#1E0099"/>
    </linearGradient>
    <linearGradient id="paint5_linear_3081_28011" x1="10.6498" y1="0.490911" x2="17.3256" y2="12.4321" gradientUnits="userSpaceOnUse">
      <stop offset="0.269069" stop-color="#6C00FF"/>
      <stop offset="1" stop-color="#00F9FF"/>
    </linearGradient>
    <linearGradient id="paint6_linear_3081_28011" x1="0.000536398" y1="7.44543" x2="11.4347" y2="7.44543" gradientUnits="userSpaceOnUse">
      <stop offset="0.06" stop-color="#380085"/>
      <stop offset="0.5625" stop-color="#4E44FF"/>
      <stop offset="1" stop-color="#00F9FF"/>
    </linearGradient>
    <linearGradient id="paint7_linear_3081_28011" x1="2.78682" y1="14.7207" x2="13.1579" y2="14.7207" gradientUnits="userSpaceOnUse">
      <stop offset="0.212285" stop-color="#6C00FF"/>
      <stop offset="1" stop-color="#00F9FF"/>
    </linearGradient>
    <clipPath id="clip0_3081_28011">
      <rect width="19.9761" height="19.6372" fill="white"/>
    </clipPath>
  </defs>
</svg>
             Search or Make Changes
          </p>
        </div>
      ) : (
        <> 
          {editor && <div
            className={`${styles.mainblocks} ${styles.mainblocktrans} `}
            style={{
              borderBottom: 0,
              padding: '12px 12px 24px',
              margin: '0 0 -20px',
              borderRadius: '12px 12px 0 0',
            }}
          >
            <div className={styles.inner}>

           
              {layoutName == "operationsHub" && <>
                {!knowledgeMessageData?.isTyping ? <>
                  <>{knowledgeMessageData?.knowledgeMessage?.[knowledgeMessageData?.knowledgeMessage?.length - 1]?.sessionId==sessionId?<>
                  <div className={styles.sentimentdes}>
                    <svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 20 20" fill="none">
                      <g clip-path="url(#clip0_3081_28011)">
                        <path d="M8.47723 16.482C7.64004 16.7394 6.73765 16.8314 5.8108 16.7231C2.61808 16.3505 0.236922 13.726 0.0692516 10.6217C0.064594 10.5332 0.0611009 10.4436 0.0599365 10.3539C0.0785666 9.45852 0.24158 7.22407 1.32212 5.22949C1.1137 5.90134 1.18356 6.66634 1.44438 7.32305C1.92527 8.53633 2.87657 9.28968 3.87677 10.0116C4.64992 10.5705 5.44985 11.1108 6.0856 11.8292C7.25114 13.1484 8.09299 14.7623 8.47839 16.482H8.47723Z" fill="url(#paint0_linear_3081_28011)" />
                        <path d="M16.8666 10.9727C17.1484 11.8017 17.266 12.7018 17.1845 13.6309C16.9027 16.833 14.3481 19.2886 11.2508 19.546C11.1623 19.5541 11.0727 19.5599 10.9842 19.5623C10.0876 19.5693 7.84968 19.4715 5.82483 18.4491C6.5025 18.6378 7.264 18.5469 7.91372 18.2675C9.11187 17.7528 9.83844 16.7794 10.5312 15.7594C11.0669 14.9711 11.585 14.1549 12.2836 13.4994C13.5691 12.2966 15.1585 11.4081 16.8655 10.9727H16.8666Z" fill="url(#paint1_linear_3081_28011)" />
                        <path d="M11.472 3.24445C12.308 2.98363 13.2104 2.88815 14.1373 2.99295C17.3311 3.35274 19.7228 5.96793 19.9044 9.071C19.9102 9.1595 19.9137 9.24915 19.9137 9.33881C19.8986 10.2342 19.7449 12.4698 18.6725 14.4691C18.8774 13.7961 18.8052 13.0322 18.5421 12.3755C18.0566 11.1646 17.1018 10.4147 16.0992 9.69627C15.3249 9.14087 14.5215 8.60409 13.8834 7.88799C12.7121 6.57341 11.8644 4.96307 11.4708 3.24445H11.472Z" fill="url(#paint2_linear_3081_28011)" />
                        <path d="M10.0035 16.5046C10.7603 16.9459 11.594 17.2254 12.5209 17.329C15.7148 17.6888 18.6292 15.6733 19.4967 12.689C19.5211 12.604 19.5456 12.5178 19.5654 12.4305C19.7505 11.5537 20.4235 9.53233 19.499 7.15234C19.5491 7.85446 19.308 8.5822 18.9052 9.16439C18.1623 10.2368 17.0643 10.7549 15.9267 11.2312C15.0476 11.6003 14.1452 11.9449 13.3627 12.5003C11.9282 13.5215 10.7673 14.9164 10.0023 16.5035L10.0035 16.5046Z" fill="url(#paint3_linear_3081_28011)" />
                        <path d="M3.21642 8.56908C2.94629 7.73538 2.84149 6.83415 2.93697 5.90614C3.26533 2.70876 5.8549 0.290343 8.95565 0.0772613C9.04414 0.0714394 9.13379 0.0667819 9.22345 0.0644531C10.12 0.070275 12.3556 0.200685 14.3665 1.25328C13.6912 1.05534 12.9285 1.13568 12.2753 1.40465C11.069 1.90184 10.3296 2.86478 9.62167 3.8743C9.07441 4.65443 8.54578 5.46368 7.83668 6.10874C6.53374 7.29292 4.93272 8.15805 3.21875 8.56791L3.21642 8.56908Z" fill="url(#paint4_linear_3081_28011)" />
                        <path d="M16.4601 9.95615C16.8932 9.19464 17.1948 8.30506 17.2903 7.37705C17.6175 4.17966 15.5717 1.28618 12.5792 0.448994C12.4942 0.424542 12.4069 0.402419 12.3196 0.382624C11.4405 0.206803 9.41328 -0.446413 7.0426 0.50372C7.74356 0.446665 8.47479 0.679541 9.06047 1.07659C10.141 1.80899 10.6696 2.90118 11.1587 4.03412C11.5359 4.90973 11.8899 5.80863 12.4535 6.58411C13.4886 8.00814 14.8649 9.20861 16.4601 9.95731V9.95615Z" fill="url(#paint5_linear_3081_28011)" />
                        <path d="M10.0631 3.26415C9.30742 2.82052 8.41434 2.50265 7.48749 2.39436C4.29476 2.02059 1.37217 4.02565 0.493069 7.00646C0.467452 7.09146 0.444165 7.17763 0.423206 7.26496C0.234577 8.14057 -0.446585 10.1596 0.468617 12.5431C0.420877 11.841 0.665397 11.1132 1.0706 10.5345C1.81813 9.46563 2.9173 8.95098 4.05723 8.4794C4.9375 8.11379 5.84222 7.77262 6.62585 7.22071C8.06502 6.20654 9.29228 4.85003 10.0654 3.26531L10.0631 3.26415Z" fill="url(#paint6_linear_3081_28011)" />
                        <path d="M3.59024 9.80469C3.16757 10.572 2.89394 11.4406 2.81243 12.3698C2.53065 15.5719 4.61839 18.4351 7.62365 19.2292C7.70865 19.2525 7.79597 19.2734 7.8833 19.292C8.76474 19.4551 10.8012 20.078 13.1579 19.0953C12.4581 19.1628 11.7234 18.9404 11.1319 18.5515C10.0409 17.8354 9.49597 16.7502 8.99179 15.6254C8.60173 14.7556 8.23495 13.8614 7.65974 13.0941C6.60365 11.6852 5.19475 10.5313 3.58907 9.80469H3.59024Z" fill="url(#paint7_linear_3081_28011)" />
                      </g>
                      <defs>
                        <linearGradient id="paint0_linear_3081_28011" x1="0.793792" y1="3.01168" x2="4.26998" y2="16.1204" gradientUnits="userSpaceOnUse">
                          <stop stop-color="#6C00FF" />
                          <stop offset="0.52" stop-color="#4702D1" />
                          <stop offset="1" stop-color="#250E83" />
                        </linearGradient>
                        <linearGradient id="paint1_linear_3081_28011" x1="10.9423" y1="14.6754" x2="16.5581" y2="16.3055" gradientUnits="userSpaceOnUse">
                          <stop stop-color="#6C00FF" />
                          <stop offset="0.52" stop-color="#4400CB" />
                          <stop offset="1" stop-color="#1E0099" />
                        </linearGradient>
                        <linearGradient id="paint2_linear_3081_28011" x1="17.8097" y1="10.9748" x2="15.6568" y2="5.80259" gradientUnits="userSpaceOnUse">
                          <stop offset="0.24" stop-color="#1E0099" />
                          <stop offset="0.81" stop-color="#6C00FF" />
                        </linearGradient>
                        <linearGradient id="paint3_linear_3081_28011" x1="16.8826" y1="19.2254" x2="16.8826" y2="5.38249" gradientUnits="userSpaceOnUse">
                          <stop stop-color="#00F9FF" />
                          <stop offset="0.5" stop-color="#6C00FF" />
                        </linearGradient>
                        <linearGradient id="paint4_linear_3081_28011" x1="3.68683" y1="4.14444" x2="10.0269" y2="4.55197" gradientUnits="userSpaceOnUse">
                          <stop stop-color="#6C00FF" />
                          <stop offset="0.52" stop-color="#4400CB" />
                          <stop offset="1" stop-color="#1E0099" />
                        </linearGradient>
                        <linearGradient id="paint5_linear_3081_28011" x1="10.6498" y1="0.490911" x2="17.3256" y2="12.4321" gradientUnits="userSpaceOnUse">
                          <stop offset="0.269069" stop-color="#6C00FF" />
                          <stop offset="1" stop-color="#00F9FF" />
                        </linearGradient>
                        <linearGradient id="paint6_linear_3081_28011" x1="0.000536398" y1="7.44543" x2="11.4347" y2="7.44543" gradientUnits="userSpaceOnUse">
                          <stop offset="0.06" stop-color="#380085" />
                          <stop offset="0.5625" stop-color="#4E44FF" />
                          <stop offset="1" stop-color="#00F9FF" />
                        </linearGradient>
                        <linearGradient id="paint7_linear_3081_28011" x1="2.78682" y1="14.7207" x2="13.1579" y2="14.7207" gradientUnits="userSpaceOnUse">
                          <stop offset="0.212285" stop-color="#6C00FF" />
                          <stop offset="1" stop-color="#00F9FF" />
                        </linearGradient>
                        <clipPath id="clip0_3081_28011">
                          <rect width="19.9761" height="19.6372" fill="white" />
                        </clipPath>
                      </defs>
                    </svg>

                  </div>
                  <div className="chatContent" dangerouslySetInnerHTML={{ __html: knowledgeMessageData?.knowledgeMessage?.[knowledgeMessageData?.knowledgeMessage?.length - 1]?.bot_response }} />
                  </>:""}</>
                </> :
                <>
                 {sendMessage &&
                  <>
                    <div style={{ width: '20px', height: '20px' }} className='loaderinline'><Ailoadericon /></div>
                    <div className="chatContent">{sendMessage}</div>
                  </>
                }
                </>
                }</>
              }

              {layoutName == "planLayout" && <>
                {botResponse ? <>
                  <div className={styles.sentimentdes}>
                    <svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 20 20" fill="none">
                      <g clip-path="url(#clip0_3081_28011)">
                        <path d="M8.47723 16.482C7.64004 16.7394 6.73765 16.8314 5.8108 16.7231C2.61808 16.3505 0.236922 13.726 0.0692516 10.6217C0.064594 10.5332 0.0611009 10.4436 0.0599365 10.3539C0.0785666 9.45852 0.24158 7.22407 1.32212 5.22949C1.1137 5.90134 1.18356 6.66634 1.44438 7.32305C1.92527 8.53633 2.87657 9.28968 3.87677 10.0116C4.64992 10.5705 5.44985 11.1108 6.0856 11.8292C7.25114 13.1484 8.09299 14.7623 8.47839 16.482H8.47723Z" fill="url(#paint0_linear_3081_28011)" />
                        <path d="M16.8666 10.9727C17.1484 11.8017 17.266 12.7018 17.1845 13.6309C16.9027 16.833 14.3481 19.2886 11.2508 19.546C11.1623 19.5541 11.0727 19.5599 10.9842 19.5623C10.0876 19.5693 7.84968 19.4715 5.82483 18.4491C6.5025 18.6378 7.264 18.5469 7.91372 18.2675C9.11187 17.7528 9.83844 16.7794 10.5312 15.7594C11.0669 14.9711 11.585 14.1549 12.2836 13.4994C13.5691 12.2966 15.1585 11.4081 16.8655 10.9727H16.8666Z" fill="url(#paint1_linear_3081_28011)" />
                        <path d="M11.472 3.24445C12.308 2.98363 13.2104 2.88815 14.1373 2.99295C17.3311 3.35274 19.7228 5.96793 19.9044 9.071C19.9102 9.1595 19.9137 9.24915 19.9137 9.33881C19.8986 10.2342 19.7449 12.4698 18.6725 14.4691C18.8774 13.7961 18.8052 13.0322 18.5421 12.3755C18.0566 11.1646 17.1018 10.4147 16.0992 9.69627C15.3249 9.14087 14.5215 8.60409 13.8834 7.88799C12.7121 6.57341 11.8644 4.96307 11.4708 3.24445H11.472Z" fill="url(#paint2_linear_3081_28011)" />
                        <path d="M10.0035 16.5046C10.7603 16.9459 11.594 17.2254 12.5209 17.329C15.7148 17.6888 18.6292 15.6733 19.4967 12.689C19.5211 12.604 19.5456 12.5178 19.5654 12.4305C19.7505 11.5537 20.4235 9.53233 19.499 7.15234C19.5491 7.85446 19.308 8.5822 18.9052 9.16439C18.1623 10.2368 17.0643 10.7549 15.9267 11.2312C15.0476 11.6003 14.1452 11.9449 13.3627 12.5003C11.9282 13.5215 10.7673 14.9164 10.0023 16.5035L10.0035 16.5046Z" fill="url(#paint3_linear_3081_28011)" />
                        <path d="M3.21642 8.56908C2.94629 7.73538 2.84149 6.83415 2.93697 5.90614C3.26533 2.70876 5.8549 0.290343 8.95565 0.0772613C9.04414 0.0714394 9.13379 0.0667819 9.22345 0.0644531C10.12 0.070275 12.3556 0.200685 14.3665 1.25328C13.6912 1.05534 12.9285 1.13568 12.2753 1.40465C11.069 1.90184 10.3296 2.86478 9.62167 3.8743C9.07441 4.65443 8.54578 5.46368 7.83668 6.10874C6.53374 7.29292 4.93272 8.15805 3.21875 8.56791L3.21642 8.56908Z" fill="url(#paint4_linear_3081_28011)" />
                        <path d="M16.4601 9.95615C16.8932 9.19464 17.1948 8.30506 17.2903 7.37705C17.6175 4.17966 15.5717 1.28618 12.5792 0.448994C12.4942 0.424542 12.4069 0.402419 12.3196 0.382624C11.4405 0.206803 9.41328 -0.446413 7.0426 0.50372C7.74356 0.446665 8.47479 0.679541 9.06047 1.07659C10.141 1.80899 10.6696 2.90118 11.1587 4.03412C11.5359 4.90973 11.8899 5.80863 12.4535 6.58411C13.4886 8.00814 14.8649 9.20861 16.4601 9.95731V9.95615Z" fill="url(#paint5_linear_3081_28011)" />
                        <path d="M10.0631 3.26415C9.30742 2.82052 8.41434 2.50265 7.48749 2.39436C4.29476 2.02059 1.37217 4.02565 0.493069 7.00646C0.467452 7.09146 0.444165 7.17763 0.423206 7.26496C0.234577 8.14057 -0.446585 10.1596 0.468617 12.5431C0.420877 11.841 0.665397 11.1132 1.0706 10.5345C1.81813 9.46563 2.9173 8.95098 4.05723 8.4794C4.9375 8.11379 5.84222 7.77262 6.62585 7.22071C8.06502 6.20654 9.29228 4.85003 10.0654 3.26531L10.0631 3.26415Z" fill="url(#paint6_linear_3081_28011)" />
                        <path d="M3.59024 9.80469C3.16757 10.572 2.89394 11.4406 2.81243 12.3698C2.53065 15.5719 4.61839 18.4351 7.62365 19.2292C7.70865 19.2525 7.79597 19.2734 7.8833 19.292C8.76474 19.4551 10.8012 20.078 13.1579 19.0953C12.4581 19.1628 11.7234 18.9404 11.1319 18.5515C10.0409 17.8354 9.49597 16.7502 8.99179 15.6254C8.60173 14.7556 8.23495 13.8614 7.65974 13.0941C6.60365 11.6852 5.19475 10.5313 3.58907 9.80469H3.59024Z" fill="url(#paint7_linear_3081_28011)" />
                      </g>
                      <defs>
                        <linearGradient id="paint0_linear_3081_28011" x1="0.793792" y1="3.01168" x2="4.26998" y2="16.1204" gradientUnits="userSpaceOnUse">
                          <stop stop-color="#6C00FF" />
                          <stop offset="0.52" stop-color="#4702D1" />
                          <stop offset="1" stop-color="#250E83" />
                        </linearGradient>
                        <linearGradient id="paint1_linear_3081_28011" x1="10.9423" y1="14.6754" x2="16.5581" y2="16.3055" gradientUnits="userSpaceOnUse">
                          <stop stop-color="#6C00FF" />
                          <stop offset="0.52" stop-color="#4400CB" />
                          <stop offset="1" stop-color="#1E0099" />
                        </linearGradient>
                        <linearGradient id="paint2_linear_3081_28011" x1="17.8097" y1="10.9748" x2="15.6568" y2="5.80259" gradientUnits="userSpaceOnUse">
                          <stop offset="0.24" stop-color="#1E0099" />
                          <stop offset="0.81" stop-color="#6C00FF" />
                        </linearGradient>
                        <linearGradient id="paint3_linear_3081_28011" x1="16.8826" y1="19.2254" x2="16.8826" y2="5.38249" gradientUnits="userSpaceOnUse">
                          <stop stop-color="#00F9FF" />
                          <stop offset="0.5" stop-color="#6C00FF" />
                        </linearGradient>
                        <linearGradient id="paint4_linear_3081_28011" x1="3.68683" y1="4.14444" x2="10.0269" y2="4.55197" gradientUnits="userSpaceOnUse">
                          <stop stop-color="#6C00FF" />
                          <stop offset="0.52" stop-color="#4400CB" />
                          <stop offset="1" stop-color="#1E0099" />
                        </linearGradient>
                        <linearGradient id="paint5_linear_3081_28011" x1="10.6498" y1="0.490911" x2="17.3256" y2="12.4321" gradientUnits="userSpaceOnUse">
                          <stop offset="0.269069" stop-color="#6C00FF" />
                          <stop offset="1" stop-color="#00F9FF" />
                        </linearGradient>
                        <linearGradient id="paint6_linear_3081_28011" x1="0.000536398" y1="7.44543" x2="11.4347" y2="7.44543" gradientUnits="userSpaceOnUse">
                          <stop offset="0.06" stop-color="#380085" />
                          <stop offset="0.5625" stop-color="#4E44FF" />
                          <stop offset="1" stop-color="#00F9FF" />
                        </linearGradient>
                        <linearGradient id="paint7_linear_3081_28011" x1="2.78682" y1="14.7207" x2="13.1579" y2="14.7207" gradientUnits="userSpaceOnUse">
                          <stop offset="0.212285" stop-color="#6C00FF" />
                          <stop offset="1" stop-color="#00F9FF" />
                        </linearGradient>
                        <clipPath id="clip0_3081_28011">
                          <rect width="19.9761" height="19.6372" fill="white" />
                        </clipPath>
                      </defs>
                    </svg>

                  </div>
                  <div className="chatContent" dangerouslySetInnerHTML={{ __html: botResponse }} />

                </> :
                <>
                  <div style={{ width: '20px', height: '20px' }} className='loaderinline'><Ailoadericon /></div>
                  {sendMessage && <div className="chatContent">{sendMessage}</div>}
                  </>
                }</>
              }
               </div>
          </div> }    
          {/* </div> */}

          <div  style={{display:"flex", flexDirection:"column"}} className={`${styles.editorblock} ${styles.editorblockplan} `}>
                 <>{ layoutName == "operationsHub"?
                 <>
                 {uploadedFiles.length > 0 && <div style={{display:"flex"}}>
                         <div className="attachment-container">
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
                        </div>



                      </div>}</>
                      :
                      <>
                       {saveFiles.length > 0 && <div style={{display:"flex"}}>
                         <div className="attachment-container">
                          {saveFiles.map((file, index) => (
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
                        </div>



                      </div>}
                      </>
                        }
                  </>
            <div className={styles.fileformaterblock}>
               <div className={styles.fileaddicon}>
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
                 <div style={{ position: 'relative', width: ` ${!isEmpty ? `calc(100% - 113.5px)` : `calc(100% - 83.5px)`}`}}>
                {isEmpty && !isFocused && (
                    <>
                     
                      {/* <div className={styles.inputcontainer}>
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
                                                </div> */}
                      <div
                        className={styles.placeholder}
                        style={{
                          position: 'absolute',
                          pointerEvents: 'none',
                          top: 0,
                          left: 0,
                          color: 'gray',
                          userSelect: 'none',
                          width: '100%',
                        }}
                      >
                        {placeholderText}
                      </div>
                    </>
                )}

                

                <div
                  ref={editorRef}
                  contentEditable
                  style={{
                    color: 'black',
                    position: 'relative',
                    outline: 'none',
                    // minHeight: '60px',
                  }}
                  className={styles.editorblockcenter}
                  suppressContentEditableWarning={true}
                  onFocus={() => setIsFocused(true)}
                  onBlur={() => setIsFocused(false)}
                  onInput={handleInput}
                  onPaste={handlePaste}
                  onKeyDown={handleKeyDown}
                />
              </div>
              <div  >
                <div  style={{gap:"8px"}}  className={styles.rightcornercontainer}>
               <div className={styles.editorblockright}>
                  <div className={styles.editoricon} onClick={() => setMicOpen(true)}>
                      <SpeachToText
                          open={micOpen}
                          setIsOpen={setMicOpen}
                          textAreaRef={editorRef}
                          placeholderText={placeholderText}
                          onText={(newText: string) => setSpeechText(newText)}
                      />
                  </div>
                </div>
                {!isEmpty && (
                  <div className={`${styles.editoricon} ${styles.sndicon} ab`} style={{cursor: 'pointer'}} onClick={()=>handleSend()}>
                    <svg xmlns="http://www.w3.org/2000/svg" width="20" height="17" viewBox="0 0 20 17" fill="none">
  <path d="M19.5711 0.434772C19.1688 0.0265723 18.5623 -0.107228 18.0247 0.0890722L0.994009 6.25407C0.447109 6.44257 0.0697089 6.91617 0.00860893 7.48947C-0.0514911 8.05197 0.206809 8.58127 0.666809 8.85957L3.71171 10.8254C4.07791 11.0578 4.55351 11.0647 4.92361 10.8381L15.2629 4.06547L5.58281 11.7609C5.38211 11.9328 5.24691 12.1711 5.20191 12.4289L4.75561 14.9562C4.64381 15.589 4.91971 16.1964 5.45921 16.505C5.99631 16.8116 6.64961 16.7501 7.12471 16.3429L9.34741 14.4367L12.761 16.6681C12.9968 16.8224 13.2737 16.9015 13.5535 16.9015C13.7215 16.9015 13.8909 16.8732 14.053 16.8146C14.4876 16.6574 14.8147 16.3156 14.9422 15.9015L19.8797 1.94147C20.0853 1.41507 19.9671 0.837072 19.5711 0.434772Z" fill="black"/>
</svg>
                  </div>
                )}
                </div>

              <div
                className={styles.editoricon}
                onClick={() => {setHideAI(true); handleEnablePagination(true);}}
                style={{ marginLeft: '8px', cursor: 'pointer' ,  }}
              >
                {/* <img src={workspacecloseicon} alt="Close" width={16} /> */}
              </div>
            </div>
             
            </div>
             

            
          </div>
        </>
      )}
    </div>
  );
}


