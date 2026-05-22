import React, { useState, useRef } from "react";
import styles from "../../assets/styles/commoneditor.module.scss";
import addicon from "../../assets/images/editor/addicon.svg";
import voiceicon from "../../assets/images/editor/voiceicon.svg";
import sendwhite from "../../assets/images/editor/sendwhite.svg";
import { useDispatcher } from "../../store/redux-store/dispatch";
import { commonDataSliceActions } from "../../store/slice/commonSilce/commonSlice";
import { AIAgentChatAction } from "../../store/slice/aichatSlice/aichatSlice";
import { MarketingAgentChatAction } from "../../store/slice/marketingSlice/marketingSlice";
import { SpeachToText } from "../../services/speachToText";
import { useLocation, useNavigate } from "react-router-dom";
import { v4 as uuidv4 } from "uuid";
import aiChatAssistSocketservice from "../../services/aiChatService";
import { stripHtmlTags } from "../../base/utils";
import jwtDecode from "jwt-decode";
import { GetItemFromStorage } from "../../base/custom_hooks/useStorage";
import { LocalStorage } from "../../base/custom_hooks/localStorageKeys";
import { handleFileUpload } from "../../base/onboardConfig";
import { fileFormatPoster } from "../setup/commoneditor";

const MAX_ROWS = 6;
const LINE_HEIGHT = 24; // must match CSS line-height

interface MyInterface {
  pageSource?: string |null; 
  onSend?: (val,file) => any;
}
const SetUpCommonEditor = ({pageSource=null,onSend} :MyInterface) => {
    const navigate = useNavigate();
  const dispatch = useDispatcher();
  const { pathname } = useLocation();
  const [value, setValue] = useState("");
  // const textareaRef = useRef(null);
  const [micOpen, setMicOpen] = useState(false);
    const placeholderText = "Ask your question";
  const textareaRef = useRef<HTMLTextAreaElement | null>(null);
   const LoginCcaasUserDetails: any = jwtDecode(GetItemFromStorage(LocalStorage.ACCESS_TOKEN));

  const [saveFiles, setSaveFiles] = useState<any>([]);
  console.log("savefiles",saveFiles)

  const handleChange = (e: React.ChangeEvent<HTMLTextAreaElement>) => {
    setValue(e.target.value);

    const textarea = textareaRef.current;
    if (!textarea) return;
    textarea.style.height = "auto";

    const maxHeight = LINE_HEIGHT * MAX_ROWS;
    textarea.style.height = Math.min(textarea.scrollHeight, maxHeight) + "px";
  };
  const fileInputRef = useRef<HTMLInputElement>(null);
  const handleSend = () => {
    if (!value.trim()) return;
    if(!pageSource){
      dispatch(AIAgentChatAction.reset());
      dispatch(commonDataSliceActions.setAiChatInitMsg(value));
      dispatch(commonDataSliceActions.setAiChatNavigate(true));
    }else{
      if(onSend)
        onSend(value,saveFiles);
      
      if(pageSource=="marketing"){
        const pathGroup = pathname?.split('/')?.[1];
        switch (pathGroup) {
          case 'marketing': {
            const sessionId: any = uuidv4() + 'marketing';
            let uploadfiles :any =saveFiles;
            if(uploadfiles && uploadfiles.length>0){
                uploadfiles = uploadfiles?.map((item :any)=>{
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
            let body: any = {
              query: stripHtmlTags(value?.trim()),
              sessionId: sessionId,
              roleId: LoginCcaasUserDetails?.roleId?.toString() ?? "",
              org_role_id_crm: LoginCcaasUserDetails?.org_role_id_crm?.toString() ?? "",
              userId: `${LoginCcaasUserDetails?.userId || ''}`,
              domainId: `${LoginCcaasUserDetails?.domainId || ''}`,
              module: "",
              ext: `${LoginCcaasUserDetails?.ext || ''}`,
              agent: 'marketing',
              isBot: 0,
              uploadUrl:uploadfiles ||[]
            }
            aiChatAssistSocketservice.io.emit('crm_chat', body);
            dispatch(MarketingAgentChatAction.setMarketingAgentChat({ ...body }))
            dispatch(MarketingAgentChatAction.setMarketingAgentChatTyping({sessionId:sessionId,typing:true}));
            dispatch(commonDataSliceActions.setTabMenuPush({ path: `/marketing/campaign-conversation?sessionId=${sessionId}`, menuName: "New Conversation", close: 1, type: "marketing" }));
            navigate(`/marketing/campaign-conversation?sessionId=${sessionId}`);
          }
            break;

          default:
            break;
        }
      }
        setSaveFiles([]);
    }
       

    console.log("Sent:", value);
    setValue("");

    if (textareaRef.current) {
      textareaRef.current.style.height = "auto";
    }
  };

  const handleKeyDown = (e: React.KeyboardEvent<HTMLTextAreaElement>) => {
    if (e.key === "Enter" && !e.shiftKey) {
      e.preventDefault();
      handleSend();
    }
  };

  
  const handleFileChange = (event: React.ChangeEvent<HTMLInputElement>) => {
    const file: any = event.target.files?.[0];

    if (file) {
      handleFileUpload(file)
        .then((res: any) => {
          setSaveFiles((prev: any) => [...prev, ...res]);
        })
        .catch((error) => {
          console.log(error);
        });
    }

    event.target.value = '';
  };

  const removeFile = (index: number) => {
    setSaveFiles((prev: any) => prev.filter((_: any, i: any) => i !== index));
  };

  return (
    <div className={styles.editor}>
             {saveFiles?.length > 0 && (
          <div className={styles.previewContainer}>
            {saveFiles?.map((itm: any, index: any) => fileFormatPoster(itm, index, removeFile))}
          </div>
        )}
      <div className={styles.editorblock}  style={{
  alignItems: textareaRef.current?.offsetHeight >= 48 ? "flex-end" : "center"
}}>
        <div className={styles.editorblockicon}>
          <img src={addicon} alt="Attach File" onClick={() => fileInputRef.current?.click()}/>
              <input
                type="file"
                ref={fileInputRef}
                style={{ display: 'none' }}
                onChange={handleFileChange}
              />
        </div>

        <textarea
          ref={textareaRef}
          rows={1}
          value={value}
          onChange={handleChange}
          onKeyDown={handleKeyDown}
          placeholder="Ask your question"
          className={styles.textarea}
        />
{/* 
        <div className={styles.editorblockicon}>
          <img src={voiceicon} alt="" />
        </div> */}

        <div className={styles.editorblockicon} onClick={() => setMicOpen(true)}>
            <SpeachToText
                    open={micOpen}
                    setIsOpen={setMicOpen}
                    textAreaRef={textareaRef}
                    placeholderText={placeholderText}
                    onText={(newText: string) => setValue(newText)}
                  />
        </div>

        {value.trim() && (
          <div className={styles.editorblockicon} onClick={handleSend}>
            <img src={sendwhite} alt="" />
          </div>
        )}
      </div>
      {
        false && 
        <div className={styles.searchcontent}>
          <div className={styles.btn}>
            <img src={sendwhite}/>
           </div>
           <div className={styles.body}>
  

Widely available across major browsers (Baseline since January 2018)
Learn more
Widely available across major browsers (Baseline since January 2018)
Learn more
Widely available across major browsers (Baseline since January 2018)
Learn more
Widely available across major browsers (Baseline since January 2018)
Learn more
Widely available across major browsers (Baseline since January 2018)
Learn more


            </div>
            <div className={styles.btn}>
              <svg xmlns="http://www.w3.org/2000/svg" width="15" height="15" viewBox="0 0 15 15" fill="none">
  <path d="M8.5064 7.3057L14.3614 1.4502C14.6934 1.1191 14.6934 0.5801 14.3614 0.249C14.0294 -0.083 13.4903 -0.083 13.1602 0.249L7.3052 6.1044L1.4502 0.249C1.1201 -0.083 0.5791 -0.083 0.249 0.249C-0.083 0.5801 -0.083 1.1191 0.249 1.4502L6.104 7.3057L0.249 13.1612C-0.083 13.4923 -0.083 14.0313 0.249 14.3624C0.415 14.5284 0.6318 14.6114 0.8496 14.6114C1.0674 14.6114 1.2851 14.5284 1.4502 14.3624L7.3052 8.507L13.1602 14.3624C13.3252 14.5284 13.543 14.6114 13.7608 14.6114C13.9786 14.6114 14.1954 14.5284 14.3614 14.3624C14.6934 14.0313 14.6934 13.4923 14.3614 13.1612L8.5064 7.3057Z" fill="#FCFBFB"/>
</svg>
            </div>
        </div>
        
      }
    </div>
  );
};

export default SetUpCommonEditor;


