"use-client"
import { useEffect, useMemo, useRef, useState } from 'react';
import styles from './scss/contact.module.scss'
import Contactname from './contactname';
import { get_contact_projects, getCompanyDetails, getContactDetails } from '../../base/service/core_serviceApi';
import { formatDate, getContactModuleByPath, stripHtmlTags } from '../../base/utils';
import Companyname from './companyname';
import { useDispatcher } from '../../store/redux-store/dispatch';
import { Outlet, useLocation, useNavigate, useSearchParams } from 'react-router-dom';
import { commonDataSliceActions } from '../../store/slice/commonSilce/commonSlice';
import { v4 as uuidv4 } from 'uuid';
import { inboxDataSliceActions } from '../../store/slice/inbox/inboxSlice';
import { contactDataSliceActions } from '../../store/slice/contactSlice';
import { useSelector } from 'react-redux';
import { GetItemFromStorage } from '../../base/custom_hooks/useStorage';
import { LocalStorage } from '../../base/custom_hooks/localStorageKeys';
import jwtDecode from 'jwt-decode';
import aiChatAssistSocketservice from '../../services/aiChatService';
import SetUpCommonEditor from '../shared/commonEditor';


const getInitials = (firstName?: string, lastName?: string) => {
    if (!firstName && !lastName) return "N";
    return `${firstName?.[0] || ""}${lastName?.[0] || ""}`.toUpperCase();
};

const getDisplayName = (firstName?: string, lastName?: string) => {
    const name = `${firstName || ""} ${lastName || ""}`.trim();
    return name || "New Customer";
};

const Contacts = () => {
    const LoginCcaasUserDetails = GetItemFromStorage(LocalStorage.USER_DETAILS);

    const dispatch = useDispatcher()
    const navigate = useNavigate()
    const location = useLocation()

    const [paramns, setSearchParams] = useSearchParams();
    const { aiAgentChatMessage, contactPerticularActivity, contactmoduleSessionId, companyListFilters, contactProjectProId } = useSelector((store: any) => store.contactStore);
    const pendingSessionId = useRef(null);

    const customerId = paramns.get("coid")
    const companyId = paramns.get("company_id")
    const companyName = paramns.get("companyId")

    const {
        company_name = null,
        owner_name = null,
        source_name = null,
        from_date = null,
        to_date = null,
        industry = null,
        company_address = null,
        company_size = null,
        offset = 0,
        limit = 50,
    } = companyListFilters ?? {};

    const isSearchResponse = (searchValue: any) => {
        if (typeof searchValue === "boolean") return searchValue;
        if (typeof searchValue === "number") return searchValue === 1;
        if (typeof searchValue === "string") return searchValue.toLowerCase() === "true";
        return false;
    };

    useEffect(() => {
        return () => {
            dispatch(contactDataSliceActions.setCompanyListFilters({ offset: 0 }));
            dispatch(contactDataSliceActions.setCompanyList({ companyList: [], companyListHasMore: true }));
        };
    }, [dispatch]);

    useEffect(() => {
        let cancelled = false;
        const fetchCompanyList = async () => {
            dispatch(contactDataSliceActions.setCompanyList({ companyListLoading: true }));
            try {
                const res = await getCompanyDetails({
                    company_name,
                    owner_name,
                    source_name,
                    from_date,
                    to_date,
                    industry,
                    company_address,
                    company_size,
                    offset,
                    limit,
                });
                if (cancelled) return;
                const items = Array.isArray(res) ? res : [];
                dispatch(contactDataSliceActions.setCompanyList({
                    companyListLoading: false,
                    companyList: items,
                    append: offset > 0,
                    companyListHasMore: items.length >= limit,
                }));
            } catch (err) {
                if (cancelled) return;
                console.error("Error fetching company list:", err);
                dispatch(contactDataSliceActions.setCompanyList({ companyListLoading: false }));
            }
        };
        fetchCompanyList();
        return () => { cancelled = true; };
    }, [company_name, owner_name, source_name, from_date, to_date, industry, company_address, company_size, offset, limit, dispatch]);

    useEffect(() => {
        if (!customerId) {
            dispatch(contactDataSliceActions.setContactProjectList({
                contactProjectList: [],
                contactProjectProId: null,
                contactProjectLoading: false,
            }));
            return;
        }

        let cancelled = false;
        const fetchContactProjects = async () => {
            dispatch(contactDataSliceActions.setContactProjectList({ contactProjectLoading: true }));
            try {
                const res: any = await get_contact_projects({ customerId });
                if (cancelled) return;

                const first = Array.isArray(res?.response) ? res.response[0] : null;
                const list = Array.isArray(first?.projests) ? first.projests : [];
                const proId = first?.pro_id ?? null;

                dispatch(contactDataSliceActions.setContactProjectList({
                    contactProjectLoading: false,
                    contactProjectList: list,
                    contactProjectProId: proId,
                }));
            } catch (err) {
                if (cancelled) return;
                console.error("getContactProjets Error", err);
                dispatch(contactDataSliceActions.setContactProjectList({ contactProjectLoading: false }));
            }
        };
        fetchContactProjects();
        return () => { cancelled = true; };
    }, [customerId, dispatch, location.pathname]);

    const handleConvoStart = (value :any, files :any) => {

        let uploadfiles: any = files;
        if (uploadfiles && uploadfiles.length > 0) {
            uploadfiles = uploadfiles?.map((item: any) => {
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
            const contactModuleByPath = getContactModuleByPath()
            const sessionId: any = contactmoduleSessionId + contactModuleByPath;
            let body: any = {
              query: stripHtmlTags(value?.trim()),
              sessionId: sessionId,
              roleId: LoginCcaasUserDetails?.roleId?.toString() ?? "",
              org_role_id_crm: LoginCcaasUserDetails?.org_role_id_crm?.toString() ?? "",
              userId: `${LoginCcaasUserDetails?.userId || ''}`,
              domainId: `${LoginCcaasUserDetails?.domainId || ''}`,
              module: contactModuleByPath,
              ext: `${LoginCcaasUserDetails?.ext || ''}`,
              agent: 'contact',
              isBot: 0,
              customerId: `${customerId || ''}`,
              companyId:`${companyId || ''}`,
              companyName:`${companyName || ''}`,
              pro_id: `${contactProjectProId || ''}`,
              uploadUrl: uploadfiles ?? [],
              meetingId: contactPerticularActivity?.meetingId || '',
            }

            aiChatAssistSocketservice.io.emit('crm_chat', body);

            dispatch(contactDataSliceActions.setcontactAiAgentChat({ ...body }))
            dispatch(contactDataSliceActions.setcontactAiAgentChatTyping({sessionId:sessionId,typing:true}));
            pendingSessionId.current  = sessionId;
            // setIsWaitingResponse(true);
            dispatch(contactDataSliceActions.setContactModuleFollowUp({
                module: contactModuleByPath,
                sessionId: sessionId,
                response: null,
                searchStatus: null,
                isWaitingResponse: true
            }))

            if(!contactModuleByPath){
                const companyName = new URLSearchParams(window.location.search).get("companyName");
                const companyId = new URLSearchParams(window.location.search).get("companyId");

                const path = `/contact/chat-conversation?sessionId=${sessionId}&companyId=${companyName || ''}&company_id=${companyId || ''}&pro_id=${contactProjectProId || ''}`;
                dispatch(commonDataSliceActions.setTabMenuPush({ path: path, menuName: "New Conversation", close: 1, type: "contact" }));
                dispatch(contactDataSliceActions.resetContactModuleFollowUp({ module: contactModuleByPath }))
                navigate(path);
                dispatch(contactDataSliceActions.setUpdatecontactmoduleSessionId())
            }

          }

    return (
        <>
            <div className={styles.tabContent}>
                <Outlet />
            </div>
            
            <div className={styles.editorbottom}>
                <SetUpCommonEditor onChange={handleConvoStart} />
            </div>
        </>
    )
}
export default Contacts
