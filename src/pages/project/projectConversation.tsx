import styles from "../contacts/scss/contactchat.module.scss";
import { useRef, useState, useEffect } from "react";
import { useSearchParams } from "react-router-dom";
import { GetItemFromStorage } from "../../base/custom_hooks/useStorage";
import { LocalStorage } from "../../base/custom_hooks/localStorageKeys";
import { useSelector } from "react-redux";
import aiChatAssistSocketservice from "../../services/aiChatService";
import { useDispatcher } from "../../store/redux-store/dispatch";
import { stripHtmlTags } from "../../base/utils";
import { dealsDataSliceActions } from "../../store/slice/dealsSlice";
import SetUpCommonEditor from "../shared/commonEditor";
import { projectDataSliceActions } from "../../store/slice/projectSlice";

export default function ProjectConversation() {
  const boxRef = useRef(null);
  const [height, setHeight] = useState(0);
  const [searchParams] = useSearchParams();
  const LoginCcaasUserDetails = GetItemFromStorage(LocalStorage.USER_DETAILS);
  const chatEndRef = useRef<HTMLDivElement | null>(null);

  const dispatch = useDispatcher();
  const { aiAgentChatMessage, aiAgentChatTyping } = useSelector((state: any) => state.projectStore);
  const chatsessionId = searchParams.get("sessionId");
  const currentaiAgentChatMessage = aiAgentChatMessage.filter((item: any) => item?.sessionId === chatsessionId);

  useEffect(() => {
    if (!boxRef.current) return;

    const observer = new ResizeObserver((entries) => {
      for (const entry of entries) {
        setHeight(Math.round(entry.contentRect.height));
      }
    });

    observer.observe(boxRef.current);
    return () => observer.disconnect();
  }, []);

  useEffect(() => {
    if (chatEndRef.current) {
      chatEndRef.current.scrollIntoView({ behavior: "smooth" });
    }
  }, [currentaiAgentChatMessage?.length, aiAgentChatTyping?.[`${chatsessionId}`]]);

  const onChange = (value: any, files: any) => {
    if (!value?.trim()) return;

    const body = {
      query: stripHtmlTags(value?.trim()),
      sessionId: chatsessionId,
      roleId: `${LoginCcaasUserDetails?.roleId ?? ""}`,
      org_role_id_crm: LoginCcaasUserDetails?.org_role_id_crm?.toString() ?? "",
      userId: `${LoginCcaasUserDetails?.userId ?? ""}`,
      domainId: `${LoginCcaasUserDetails?.domainId ?? ""}`,
      module: "",
      ext: `${LoginCcaasUserDetails?.ext ?? ""}`,
      agent: "projects",
      isBot: 0,
      files: files ?? [],
    };

    aiChatAssistSocketservice.io.emit("crm_chat", body);
    dispatch(projectDataSliceActions.setprojectAiAgentChat({ ...body, isBot: 0, query: body?.query }));
    dispatch(projectDataSliceActions.setprojectAiAgentChatTyping({ sessionId: chatsessionId, typing: true }));
  };

  return (
    <div className={styles.chatmain}>
      <div className={styles.chatmaininner}>
        <div className={styles.chatarea} style={{ height: `calc(100dvh - ${height}px - 180px)` }}>
          <div className={styles.bubbleblock}>
            {currentaiAgentChatMessage?.map((item: any, index: number) => {
              if (item?.isBot === 0) {
                return (
                  <div className={styles.bubbleblockagent} key={`u-${index}`}>
                    <div className={styles.bubbleblockagenttextblock}>
                      <div className={styles.bubbleblockagenttext}>
                        <div dangerouslySetInnerHTML={{ __html: item?.query }} />
                      </div>
                    </div>
                  </div>
                );
              }

              return (
                <div className={styles.bubbleblockcustomer} key={`b-${index}`}>
                  <div className={styles.bubbleblockcustomertextblock}>
                    <div className={styles.bubbleblockcustomertext}>
                      <div dangerouslySetInnerHTML={{ __html: item?.response }} />
                    </div>
                  </div>
                </div>
              );
            })}

            {!!aiAgentChatTyping?.[`${chatsessionId}`] && (
              <div className="msgbubble left">
                {aiAgentChatTyping?.[`${chatsessionId}`] || "Typing ..."}
              </div>
            )}
            <div ref={chatEndRef} />
          </div>
        </div>

        <div className={styles.editorbottom} ref={boxRef}>
          <SetUpCommonEditor onChange={onChange} />
        </div>
      </div>
    </div>
  );
}
