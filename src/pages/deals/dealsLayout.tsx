import { useEffect, useRef, useState } from "react";
import { Outlet, useNavigate, useSearchParams } from "react-router-dom";
import styles from "./scss/dealslayout.module.scss";
import SetUpCommonEditor from "../shared/commonEditor";
import { v4 as uuidv4 } from "uuid";
import { stripHtmlTags } from "../../base/utils";
import { GetItemFromStorage } from "../../base/custom_hooks/useStorage";
import { LocalStorage } from "../../base/custom_hooks/localStorageKeys";
import jwtDecode from "jwt-decode";
import aiChatAssistSocketservice from "../../services/aiChatService";
import { useDispatcher } from "../../store/redux-store/dispatch";
import { dealsDataSliceActions } from "../../store/slice/dealsSlice";
import { commonDataSliceActions } from "../../store/slice/commonSilce/commonSlice";
import { useSelector } from "react-redux";

const DealsLayout = () => {
  const token = GetItemFromStorage(LocalStorage.ACCESS_TOKEN);
  const LoginCcaasUserDetails: any = token ? jwtDecode(token) : {};
  const dispatch = useDispatcher();
  const navigate = useNavigate();
  const [params] = useSearchParams();
  const customerId = params.get("coid");
  const { aiAgentChatMessage } = useSelector((store: any) => store.dealsStore);
  const [pendingSessionId, setPendingSessionId] = useState<string | null>(null);
  const [searchResponse, setSearchResponse] = useState("");
  const [searchStatus, setSearchStatus] = useState("");
  const [isWaitingResponse, setIsWaitingResponse] = useState(false);
  const handledNavigateSessionRef = useRef<string | null>(null);

  const isSearchResponse = (searchValue: any) => {
    if (typeof searchValue === "boolean") return searchValue;
    if (typeof searchValue === "number") return searchValue === 1;
    if (typeof searchValue === "string") return searchValue.toLowerCase() === "true";
    return false;
  };

  const handleConvoStart = (value: any, files: any) => {
    const sessionId: any = uuidv4() + "deals";
    const body: any = {
      query: stripHtmlTags(value?.trim()),
      sessionId,
      roleId: `${LoginCcaasUserDetails?.roleId || ""}`,
      org_role_id_crm: LoginCcaasUserDetails?.org_role_id_crm?.toString() ?? "",
      userId: `${LoginCcaasUserDetails?.userId || ""}`,
      domainId: `${LoginCcaasUserDetails?.domainId || ""}`,
      module: "",
      ext: `${LoginCcaasUserDetails?.ext || ""}`,
      agent: "deals",
      isBot: 0,
      customerId: `${customerId || ""}`,
      files: files ?? [],
    };

    aiChatAssistSocketservice.io.emit("crm_chat", body);

    dispatch(dealsDataSliceActions.setdealsAiAgentChat({ ...body }));
    dispatch(dealsDataSliceActions.setdealsAiAgentChatTyping({ sessionId, typing: true }));
    setPendingSessionId(sessionId);
    setIsWaitingResponse(true);
    handledNavigateSessionRef.current = null;
  };

  useEffect(() => {
    if (!pendingSessionId) return;

    const currentSessionMessages = aiAgentChatMessage?.filter(
      (item: any) => item?.sessionId === pendingSessionId && item?.isBot === 1
    );
    if (!currentSessionMessages?.length) return;

    const latestBotMessage = currentSessionMessages[currentSessionMessages.length - 1];
    const searchFlag = isSearchResponse(latestBotMessage?.isSearch);
    const is_followup = isSearchResponse(latestBotMessage?.is_followup);

    if(is_followup){
      setSearchResponse(latestBotMessage?.response || latestBotMessage?.msg || latestBotMessage?.message || "");
      setSearchStatus(latestBotMessage?.status || "");
      setIsWaitingResponse(false);
      setPendingSessionId(null);
      return;
    } else if (searchFlag) {
      setSearchResponse(latestBotMessage?.response || latestBotMessage?.msg || latestBotMessage?.message || "");
      setSearchStatus(latestBotMessage?.status || "");
      setIsWaitingResponse(false);
      dispatch(
        dealsDataSliceActions.setDealsListFilters({
          fromDate: latestBotMessage?.fromDate ?? latestBotMessage?.from_date ?? null,
          toDate: latestBotMessage?.toDate ?? latestBotMessage?.to_date ?? null,
          contact: latestBotMessage?.contact ?? null,
          dealName: latestBotMessage?.dealName ?? latestBotMessage?.deal_name ?? null,
          companyName: latestBotMessage?.companyName ?? latestBotMessage?.company_name ?? null,
          pipelineStage: latestBotMessage?.pipelineStage ?? latestBotMessage?.pipeline_stage ?? null,
          dealEstimatedClosed:
            latestBotMessage?.dealEstimatedClosed ??
            latestBotMessage?.deal_estimated_closed ??
            latestBotMessage?.estimated_closed_date ??
            null,
          location: latestBotMessage?.location ?? null,
          totalAmount: latestBotMessage?.totalAmount ?? latestBotMessage?.total_amount ?? null,
          closedAmount: latestBotMessage?.closedAmount ?? latestBotMessage?.closed_amount ?? null,
          ownerName: latestBotMessage?.ownerName ?? latestBotMessage?.owner_name ?? null,
        })
      );
      setPendingSessionId(null);
      return;
    }

    if (handledNavigateSessionRef.current === pendingSessionId) return;
    handledNavigateSessionRef.current = pendingSessionId;

    dispatch(
      commonDataSliceActions.setTabMenuPush({
        path: `/deals/chat-conversation?sessionId=${pendingSessionId}`,
        menuName: "New Conversation",
        close: 1,
        type: "deals",
      })
    );
    setIsWaitingResponse(false);
    navigate(`/deals/chat-conversation?sessionId=${pendingSessionId}`);
    setPendingSessionId(null);
    setSearchResponse("");
    setSearchStatus("");
  }, [aiAgentChatMessage, dispatch, navigate, pendingSessionId]);

  return (
    <>
      <div className={styles.tabContent}>
        <Outlet />
      </div>
      <div className={styles.editorbottom}>
        <SetUpCommonEditor
          onChange={handleConvoStart}
          searchResponse={searchResponse}
          searchStatus={searchStatus}
          isWaitingResponse={isWaitingResponse}
          onCloseSearchResponse={() => {
            setSearchResponse("");
            setSearchStatus("");
          }}
        />
      </div>
    </>
  );
};

export default DealsLayout;
