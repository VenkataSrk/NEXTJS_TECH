"use-client"
import { useEffect, useMemo, useState } from 'react';
import styles from './scss/project.module.scss'
import { stripHtmlTags } from '../../base/utils';
import { useDispatcher } from '../../store/redux-store/dispatch';
import { Outlet, useNavigate, useSearchParams } from 'react-router-dom';
import { commonDataSliceActions } from '../../store/slice/commonSilce/commonSlice';
import { v4 as uuidv4 } from 'uuid';
import { inboxDataSliceActions } from '../../store/slice/inbox/inboxSlice';
import { contactDataSliceActions } from '../../store/slice/contactSlice';
import { GetItemFromStorage } from '../../base/custom_hooks/useStorage';
import { LocalStorage } from '../../base/custom_hooks/localStorageKeys';
import jwtDecode from 'jwt-decode';
import aiChatAssistSocketservice from '../../services/aiChatService';
import SetUpCommonEditor from '../shared/commonEditor';
import { projectDataSliceActions } from '../../store/slice/projectSlice';


const getInitials = (firstName?: string, lastName?: string) => {
    if (!firstName && !lastName) return "N";
    return `${firstName?.[0] || ""}${lastName?.[0] || ""}`.toUpperCase();
};

const getDisplayName = (firstName?: string, lastName?: string) => {
    const name = `${firstName || ""} ${lastName || ""}`.trim();
    return name || "New Customer";
};

const ProjectsLayout = () => {
    const token = GetItemFromStorage(LocalStorage.ACCESS_TOKEN)
    const LoginCcaasUserDetails: any = token ? jwtDecode(token) : {};

    const dispatch = useDispatcher()
    const navigate = useNavigate()

    const [paramns] = useSearchParams();
    const customerId = paramns.get("coid")
    const companyId = paramns.get("company_id")
    const companyName = paramns.get("companyId")

    const handleConvoStart = (value :any, files :any) => {
            const sessionId: any = uuidv4() + 'projects';
            let body: any = {
              query: stripHtmlTags(value?.trim()),
              sessionId: sessionId,
              roleId: LoginCcaasUserDetails?.roleId?.toString() ?? "",
              org_role_id_crm: LoginCcaasUserDetails?.org_role_id_crm?.toString() ?? "",
              userId: `${LoginCcaasUserDetails?.userId || ''}`,
              domainId: `${LoginCcaasUserDetails?.domainId || ''}`,
              module: "",
              ext: `${LoginCcaasUserDetails?.ext || ''}`,
              agent: 'projects',
              isBot: 0,
              customerId: `${customerId || ''}`,
              companyId:`${companyId || ''}`,
              companyName:`${companyName || ''}`,
              files: files ?? []
            }

            aiChatAssistSocketservice.io.emit('crm_chat', body);

            dispatch(projectDataSliceActions.setprojectAiAgentChat({ ...body }))
            dispatch(projectDataSliceActions.setprojectAiAgentChatTyping({sessionId:sessionId,typing:true}));

            const path = `/projects/chat-conversation?sessionId=${sessionId}`
            dispatch(commonDataSliceActions.setTabMenuPush({ path: path, menuName: "New Conversation", close: 1 ,type: "projects"}));
            navigate(path);
          }

    return (
        <>
            <div className={styles.tabContent}>
                <Outlet />
            </div>
            
            <div className={styles.editorbottom}>
                <SetUpCommonEditor onChange={handleConvoStart}/>
            </div>
        </>
    )
}
export default ProjectsLayout
