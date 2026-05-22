import React, { useEffect, useRef, useState } from "react";
import SetUpCommonEditor from "../setup/commoneditor";
import { useDispatcher } from "../../store/redux-store/dispatch";
import aiChatAssistSocketservice from "../../services/aiChatService";
import { useSearchParams } from "react-router-dom";
import { v4 as uuidv4 } from "uuid";
import { GetItemFromStorage } from "../../base/custom_hooks/useStorage";
import { LocalStorage } from "../../base/custom_hooks/localStorageKeys";
import { AIAgentChatAction } from "../../store/slice/aichatSlice/aichatSlice";
import { useSelector } from "react-redux";
import { stripHtmlTags } from "../../base/utils";
import { commonDataSliceActions } from "../../store/slice/commonSilce/commonSlice";

function Chatconversation() {

  const chatEndRef = useRef<HTMLDivElement | null>(null);
  const boxRef = useRef(null);
  const [height, setHeight] = useState(0);
  const dispatch = useDispatcher();
  const [searchParams, setSearchParams] = useSearchParams()
  const LoginCcaasUserDetails = GetItemFromStorage(LocalStorage.USER_DETAILS);

  const { aiAgentChatMessage, aiAgentChatTyping } = useSelector((state :any) => state.aiChatStore);
  const chatsessionId = searchParams.get("sessionId")

  const isInitMsg = useSelector((state :any)=>state?.commonData?.aiChatInitMsg);

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
    if (chatEndRef.current) {
        chatEndRef.current.scrollIntoView({ behavior: 'smooth' });
    }
}, [aiAgentChatMessage.length]);


  const createNewChat = (sessionId = uuidv4()) => {
    searchParams.set("sessionId", sessionId);
    setSearchParams(searchParams, { replace: true });
}

  const onChange = (value :any) => {
    if(!value?.trim()) return;

    let newSessonId = chatsessionId;
    if(!newSessonId){
      newSessonId = uuidv4();
      createNewChat(newSessonId)
    }

    const body = {
      query: stripHtmlTags(value?.trim()),
      sessionId: newSessonId,
      roleId: LoginCcaasUserDetails?.roleId?.toString() ?? "",
      org_role_id_crm: LoginCcaasUserDetails?.org_role_id_crm?.toString() ?? "",
      userId: LoginCcaasUserDetails?.UserID?.toString() ?? "",
      domainId: LoginCcaasUserDetails?.domainId?.toString() ?? "",
      module: "",
      ext: LoginCcaasUserDetails?.ext?.toString() ?? "",
    }

    aiChatAssistSocketservice.io.emit('crm_chat', body);
    dispatch(AIAgentChatAction.setAiAgentChat({ ...body, isBot: 0, query: body?.query }))
    dispatch(AIAgentChatAction.setAiAgentChatTyping(true));
  }

  useEffect(()=>{
    if(isInitMsg?.length>0){
        onChange(isInitMsg);
        dispatch(commonDataSliceActions.setAiChatInitMsg(null));
    }
  },[isInitMsg])

  return (<>
    <div className="msgbody">

      <div className="scroller" style={{ height: `calc(100dvh - ${height}px - 80px)` }}>
        <div className="maincontainer">
          {aiAgentChatMessage.map((item :any, index :any) => {
            if(item?.isBot === 0){
              return (
                <div className="msgbubble right" key={index}>
                  <div className="chatContent">
                    <div dangerouslySetInnerHTML={{__html: item?.query || ''}}/>
                  </div>
                </div>
              )
            }
            return (
              <div className="msgbubble left" key={index}>
                <div className="chatContent">
                  <div dangerouslySetInnerHTML={{__html: item?.response || ''}}/>
                </div>
              </div>
            )
          })}

          {aiAgentChatTyping && (
            <div className="msgbubble left">
              <div className="chatContent">
                   Typing ...
              </div>
            </div>
          )}

          <div ref={chatEndRef} />

        </div>
      </div>
      <div ref={boxRef} style={{ maxHeight: '300px', overflow: 'auto', maxWidth: '700px', margin: '0 auto' }}>
        <SetUpCommonEditor onChange={onChange} />
      </div>

    </div>
  </>)
} export default Chatconversation
