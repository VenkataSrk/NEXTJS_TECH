import { Outlet, useNavigate, useSearchParams } from "react-router-dom";
import styles from "./scss/dadshlayout.module.scss";
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
import { homeDataSliceActions } from "../../store/slice/homeSlice";

const HomeLayout = () => {
  const token = GetItemFromStorage(LocalStorage.ACCESS_TOKEN);
  const LoginCcaasUserDetails = GetItemFromStorage(LocalStorage.USER_DETAILS);
  const dispatch = useDispatcher();
  const navigate = useNavigate();
  const [params] = useSearchParams();
  const customerId = params.get("coid");

  const handleConvoStart = (value: any, files: any) => {
    const sessionId: any = uuidv4() + "home";
    const body: any = {
      query: stripHtmlTags(value?.trim()),
      sessionId,
      roleId: `${LoginCcaasUserDetails?.roleId || ""}`,
      org_role_id_crm: LoginCcaasUserDetails?.org_role_id_crm?.toString() ?? "",
      userId: `${LoginCcaasUserDetails?.userId || ""}`,
      domainId: `${LoginCcaasUserDetails?.domainId || ""}`,
      module: "",
      ext: `${LoginCcaasUserDetails?.ext || ""}`,
      agent: "home",
      isBot: 0,
      customerId: `${customerId || ""}`,
      files: files ?? [],
    };

    aiChatAssistSocketservice.io.emit("crm_chat", body);

    dispatch(homeDataSliceActions.setHomeAiAgentChat({ ...body }));
    dispatch(homeDataSliceActions.setHomeAiAgentChatTyping({ sessionId, typing: true }));
    dispatch(
      commonDataSliceActions.setTabMenuPush({
        path: `/home/chat-conversation?sessionId=${sessionId}`,
        menuName: "New Conversation",
        close: 1,
        type: "home",
      })
    );
    navigate(`/home/chat-conversation?sessionId=${sessionId}`);
  };

  return (
    <>
      <div className={styles.tabContent}>
        <Outlet />
      </div>
      <div className={styles.editorbottom}>
        <SetUpCommonEditor onChange={handleConvoStart} />
      </div>
    </>
  );
};

export default HomeLayout;
