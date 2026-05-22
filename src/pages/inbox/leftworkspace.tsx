import React, { memo, useEffect, useMemo, useRef, useState } from "react";
import { v4 as uuidv4 } from "uuid";
import styles from "./scss/inbox.module.scss"
import chevrondown from "../../assets/images/inbox/chevrondown.svg";
import mailicon from "../../assets/images/inbox/mailicon.svg";
import purpleicn from "../../assets/images/inbox/callpurple.svg"
import callicn from "../../assets/images/inbox/callicon.svg"
import contactwindow from "../../assets/images/inbox/contactwindow.svg";
import bluechevron from "../../assets/images/inbox/bluechevron.svg";
import reply from "../../assets/images/inbox/reply.svg";
import forward from "../../assets/images/inbox/forward.svg";
import deleteem from "../../assets/images/inbox/delete.svg";
import ComposeMail from "./composemail";
import threedot from "../../assets/images/inbox/threedot.svg";
import openemail from "../../assets/images/inbox/mailopen.svg";
import newemail from "../../assets/images/inbox/newemail.svg";
import search from "../../assets/images/inbox/searchicon.svg"
import filter from "../../assets/images/inbox/filter.svg";
import searchicn from "../../assets/images/inbox/searchicon.svg"
import notes from "../../assets/images/inbox/notes.svg"
import calender from "../../assets/images/inbox/calendericon.svg"
import cancelicn from "../../assets/images/inbox/cancelicon.svg"
import rightchevron from "../../assets/images/inbox/rightchevron.svg"
import { deleteTrashEmailApi, getContactDetails, getDraftForEmailApi, getInboxList, getInboxUnreadMsgCount, insertDraftForEmailApi, moveEmailToTrashApi, updateDraftForEmailTrashApi, updateEmailPinStatusApi, updateEmilopenStatus } from "../../base/service/core_serviceApi";
import { GetItemFromStorage } from "../../base/custom_hooks/useStorage";
import { LocalStorage } from "../../base/custom_hooks/localStorageKeys";
import { attachScrollPagination } from "../../base/custom_hooks/scrollpagination";
import { useSelector } from "react-redux";
import { useDispatcher } from "../../store/redux-store/dispatch";
import moment from "moment";
import { inboxDataSliceActions } from "../../store/slice/inbox/inboxSlice";
import { useNavigate, useSearchParams } from "react-router-dom";
import Nodata from "../component/nodata";
import SkeletonBox from "../marketing/skeletonbox";
import avator1 from "../../assets/images/project/Avatar.png"
import DateRangeCalendar from "../../base/shared/DateRangeCalendar";
import { ACTIONS_ACCESS, levelOfAccess, MENUS } from "../../base/constant";
import { groupbyday, sanitizePhoneNumber } from "../../base/utils";
import { contactDataSliceActions } from "../../store/slice/contactSlice";
import { CallPopUpAgentActions } from "../../store/slice/callSlice/call_popup_agent";
import { CallService } from "../../base/service/call_service";
import Modal from "../component/modal/modal";

const channelIcons = {
    call: purpleicn,
    email: mailicon,
}



const LeftWorkspace = memo(({ activeFilter = 'Inbox', setTabLoading }: any) => {

    const LoginUserDetails = GetItemFromStorage(LocalStorage.USER_DETAILS);
    const offset = useRef<any>(0)
    const isFirstMountRef = useRef(true)
    const containerRef = useRef(null)
    const dispatch = useDispatcher()
    const { inboxList_groupData, emailSendSuccess, inboxFilter, inboxList_allrecord, domainEmailConfig , inboxRefresh,
        unReadCount, totalEmailCount
    } = useSelector((state: any) => state.inboxChatStore)    
    const { permissions } = useSelector((store: any) => store?.loginUser?.rolesandpermissions)
    const hasMailDeleteAccess = levelOfAccess(MENUS.INBOX, ACTIONS_ACCESS.DELETE, permissions)
    const navigate = useNavigate();
    const { makeCallFunctionForAll } = CallService(dispatch, navigate);

    const { filterQueueList } = useSelector((store: any) => store.commonData)
    const loginDeatils: any = GetItemFromStorage(LocalStorage.LOGIN_USER_CRM_DETAILS);
    
    const [params, setParams] = useSearchParams()
    const emailId = params.get('emailid')
    const currentActivesessionId = params.get('sessionId') || '';
    const selectedindex = params.get('sindex') || '';
    const loaderRef = useRef(false);
    const activeFilterRef = useRef(activeFilter);

    const allAccess = levelOfAccess(MENUS.INBOX, ACTIONS_ACCESS.VIEW, permissions, 'all');
    const teamAccess = levelOfAccess(MENUS.INBOX, ACTIONS_ACCESS.VIEW, permissions , 'team');

    useEffect(() => {
        activeFilterRef.current = activeFilter;
    }, [activeFilter]);


    useEffect(() => {
        const cleanup = attachScrollPagination(containerRef, () => {
            if (!loaderRef.current) {
                fetchData(false);
            }
        });

        return () => cleanup && cleanup();
    }, []);

    
    useEffect(() => {
        offset.current = 0;
        loaderRef.current = false;

        const hasExistingRecords = Array.isArray(inboxList_allrecord) && inboxList_allrecord.length > 0;
        const skipLoader = isFirstMountRef.current && hasExistingRecords;
        isFirstMountRef.current = false;

        if (!skipLoader) {
            dispatch(inboxDataSliceActions.setInboxList({
                allrecord: [],
                groupData: []
            }));
        }

        fetchData(true, skipLoader);
        getInboxUnreadMsgCount({
            agentId: (allAccess || teamAccess) ? null : LoginUserDetails?.userId,
        });

    }, [inboxFilter, activeFilter]);

    const isFirstRefreshRef = useRef(true);
    useEffect(() => {
        if (isFirstRefreshRef.current) {
            isFirstRefreshRef.current = false;
            return;
        }
        offset.current = 0;
        loaderRef.current = false;
        fetchData(true, true);
        getInboxUnreadMsgCount({
            agentId: (allAccess || teamAccess) ? null : LoginUserDetails?.userId,
        });
    }, [emailSendSuccess, inboxRefresh]);

    const buildFilterBody = (currentOffset: number) => {

        const body: any = {
            channelType: null,
            agentId: (allAccess || teamAccess) ? null : LoginUserDetails?.userId,
            // agentId: LoginUserDetails?.org_role_id_crm == 3 ? null : LoginUserDetails?.userId,
            isTrash: 0,
            isInbox: 0,
            offset: currentOffset,
            limit: 200,
        };

        if (inboxFilter.search !== null) {
            body.search = inboxFilter.search;
        }

        if (inboxFilter.fromDate !== null) {
            body.fromDate = inboxFilter.fromDate;
        }

        if (inboxFilter.toDate !== null) {
            body.toDate = inboxFilter.toDate;
        }

        const channel = inboxFilter?.channelType?.split(',').map((item: any) => {
            let d = item?.trim() || ''
            if(!d) return d
            return d[0].toUpperCase() + d.slice(1);
        } );

        console.log("channelchannelchannelchannel", activeFilterRef.current)
        switch (activeFilterRef.current) {
            // case 'Email':
            //     body.channelType = 'Email';
            //     body.isInbox = null;
            //     break;
            // case 'Call':
            //     body.channelType = 'Call';
            //     break;
            case 'Inbox':
                body.channelType =  channel?.length > 0 ? channel.join(',') : 'Email,Call';
                body.isInbox = inboxFilter?.inboxType ?? null;
                body.isInbox = 1;
                break;
            // case 'Unassigned':
            //     body.agentId = 0;
            //     break;
            case 'Trash':
                body.channelType = 'Email,Call';
                body.isTrash = 1;
                body.isInbox = null;
                break;
            case 'Sent':
                body.channelType = 'Email';
                body.isInbox = 0;
                body.isTrash = 0;
                break;
            default:
                break;
        }

        return body;
    }

    const [loader, setLoader] = useState(false)

    const fetchData = async (reset = false, skipLoader = false) => {
        try {
            setLoader(!!reset && !skipLoader)
            if (loaderRef.current) return;

            loaderRef.current = true;
            setTabLoading(true);

            if (reset) {
                offset.current = 0;

                if (!skipLoader) {
                    dispatch(inboxDataSliceActions.setInboxList({
                        allrecord: [],
                        groupData: []
                    }));
                }
            }
            const endDate = inboxFilter.fromDate ? moment.utc(inboxFilter.fromDate).add(24, 'hours').format('YYYY-MM-DD HH:mm'): null;
            if (activeFilterRef.current === "Draft") {
                const draftResponse: any = await getDraftForEmailApi({
                    domainId: LoginUserDetails?.domainId ?? LoginUserDetails?.domain_id ?? null,
                    agentId: LoginUserDetails?.userId ?? null,
                    search: inboxFilter.search || null,
                    fromDate: inboxFilter.fromDate || null,
                    toDate: inboxFilter.toDate || endDate || null,
                });

                const rawDrafts = Array.isArray(draftResponse)
                    ? draftResponse
                    : Array.isArray(draftResponse?.response)
                        ? draftResponse.response
                        : [];

                const normalizedDrafts = rawDrafts.map((draft: any) => {
                    const rawMessage = draft?.messageJson ?? draft?.message_json ?? null;
                    let parsedMessage: any = null;
                    if (typeof rawMessage === "string") {
                        try {
                            parsedMessage = JSON.parse(rawMessage);
                        } catch {
                            parsedMessage = { html: rawMessage };
                        }
                    } else if (rawMessage && typeof rawMessage === "object") {
                        parsedMessage = rawMessage;
                    }

                    return {
                        ...draft,
                        isDraft: 1,
                        draftId: draft?.draftId ?? draft?.draft_id ?? null,
                        sessionId: draft?.sessionId ?? draft?.session_id ?? null,
                        channelType: "Email,Call",
                        EmailSubject:
                            draft?.emailSubject
                            ?? draft?.email_subject
                            ?? draft?.toEmail
                            ?? draft?.to_email
                            ?? (parsedMessage?.html ? getPreview(parsedMessage?.html) : "Draft"),
                        ToEmail: draft?.toEmail ?? draft?.to_email ?? "",
                        message_body: parsedMessage?.html ?? "",
                        createdAt: draft?.createdAt ?? draft?.created_at ?? null,
                        updatedAt: draft?.updatedAt ?? draft?.updated_at ?? draft?.createdAt ?? draft?.created_at ?? null,
                        isOpened: 1,
                        is_pin: 0,
                    };
                });

                const sortedDrafts = [...normalizedDrafts].sort((a: any, b: any) => {
                    const aTime = new Date(a?.updatedAt ?? a?.createdAt ?? 0).getTime();
                    const bTime = new Date(b?.updatedAt ?? b?.createdAt ?? 0).getTime();
                    return bTime - aTime;
                });

                dispatch(inboxDataSliceActions.setInboxList({
                    allrecord: sortedDrafts,
                    groupData: groupbyday(sortedDrafts) ?? [],
                }));
            } else {
                const body = buildFilterBody(offset.current);
                await getInboxList(body, dispatch);
            }

            offset.current += 1;

        } catch (error) {
            console.log('get_inbox_session_details Error', error);

        } finally {
            loaderRef.current = false;
            setTabLoading(false);
            setLoader(false)
        }
    };

    const [minimizeSession, setMinimizeSession] = useState<Record<string, boolean>>({});
    const [openActionMenuId, setOpenActionMenuId] = useState<string | null>(null);
    const [hoveredRowId, setHoveredRowId] = useState<string | null>(null);
    const [selectedTrashItemIds, setSelectedTrashItemIds] = useState<string[]>([]);
    const [openConfirmDelete, setOpenConfirmDelete] = useState(false);
    const [openConfirmRestore, setOpenConfirmRestore] = useState(false);
    const pendingDeleteActionRef = useRef<null | (() => Promise<void> | void)>(null);
    const pendingRestoreActionRef = useRef<null | (() => Promise<void> | void)>(null);

    const [openMoveTrashDelete, setOpenMoveTrashDelete] = useState(false);
    const pendingTrashDeleteActionRef = useRef<null | (() => Promise<void> | void)>(null);

    const bulkSelectionFilters = ["Draft", "Trash", "Inbox", "Email"];

    const getEmailHistoryId = (item: any) => item?.EmailHistoryId || null;
    const getSmsHistoryId = (item: any) => item?.SMSHistoryId || null;
    const getCallHistoryId = (item: any) => item?.CallHistoryId || null;
    const getDraftId = (item: any) => item?.draftId || item?.draft_id || null;

    const getSelectableItemId = (item: any) => {
        const draftId = getDraftId(item);
        const emailhistoryId = getEmailHistoryId(item);
        const smsHistoryId = getSmsHistoryId(item);
        const callHistoryId = getCallHistoryId(item);
        if (getDraftId(item)) 
            return `d_${draftId}`;
        else if (emailhistoryId) 
            return `eh_${emailhistoryId}`;
        else if ( smsHistoryId) 
            return `sh_${smsHistoryId}`;
        else if (callHistoryId) 
            return `ch_${callHistoryId}`;
        else
        return "";
    };

    useEffect(() => {
        const handleOutsideClick = (event: MouseEvent) => {
            const target = event.target as HTMLElement | null;
            if (!target) return;
            const clickedTrigger = target.closest('[data-inbox-action-trigger="true"]');
            const clickedMenu = target.closest('[data-inbox-action-menu="true"]');
            if (!clickedTrigger && !clickedMenu) {
                setOpenActionMenuId(null);
            }
        };

        document.addEventListener("mousedown", handleOutsideClick);
        return () => document.removeEventListener("mousedown", handleOutsideClick);
    }, []);

    useEffect(() => {
        setSelectedTrashItemIds([]);
        setHoveredRowId(null);
        setMinimizeSession({});
    }, [activeFilter]);
    const openCompose = async () => {
        dispatch(inboxDataSliceActions.clearAiSuggestions())
        dispatch(inboxDataSliceActions.setCleanReplyForword());
        await insertDraftForEmailApi({
            draftUniqueId: uuidv4(),
            domainId: LoginUserDetails?.domainId ?? LoginUserDetails?.domain_id ?? null,
            fromEmail: domainEmailConfig?.emailAddress ?? null,
            toEmail: null,
            cc: null,
            bcc: null,
            emailSubject: null,
            messageJson: null,
            agentId: LoginUserDetails?.userId ?? null,
        });
        navigate("/inbox?channelType=Email");
        dispatch(inboxDataSliceActions.setComposeMailToggle(true))
    }

    const openForwardCompose = (mailData?: any) => {
        dispatch(inboxDataSliceActions.setCleanReplyForword());

        dispatch(inboxDataSliceActions.setComposeMailToggle(true))
        dispatch(inboxDataSliceActions.setForwardedMessage(true))
        dispatch(inboxDataSliceActions.setForwardMailDetails({
            from: mailData?.customer_name?.trim() ? mailData?.customer_name : "New Customer",
            fromEmail: mailData?.FromEmail || "",
            date: mailData?.createdAt,
            subject: mailData?.EmailSubject || "No Subject",
            toMail: mailData?.ToEmail || "",
            message: mailData?.message_body || "",
        }))
        navigate(`/inbox?channelType=Email&CustomerId=${mailData?.CustomerId || 0}`);
    }

    const openReplyCompose = (mailData?: any) => {
        dispatch(inboxDataSliceActions.setCleanReplyForword());
        dispatch(inboxDataSliceActions.clearAiSuggestions())
        dispatch(inboxDataSliceActions.setComposeMailToggle(true))
        dispatch(inboxDataSliceActions.setReplyMessage(true))
        dispatch(inboxDataSliceActions.setReplyMailDetails({
            from: mailData?.customer_name?.trim() ? mailData?.customer_name : "New Customer",
            fromEmail: mailData?.FromEmail || "",
            date: mailData?.createdAt,
            subject: mailData?.EmailSubject || "No Subject",
            toMail: mailData?.FromEmail || "",
            message: mailData?.message_body || "",
        }))
        navigate(`/inbox?channelType=Email&CustomerId=${mailData?.CustomerId || 0}&replay=true`);
    }

    const moveEmailToTrash = async (mailData?: any) => {
        const draftId = getDraftId(mailData);
        let response: any = null;

        const emailhistoryId = getEmailHistoryId(mailData);
        const smsHistoryId = getSmsHistoryId(mailData);
        const callHistoryId = getCallHistoryId(mailData);

        if (draftId) {
            response = await updateDraftForEmailTrashApi({
                domainId: LoginUserDetails?.domainId ?? LoginUserDetails?.domain_id ?? null,
                draftId,
                isTrash: 1,
            });
        } else {
            const body = {
                email_history_ids: emailhistoryId,
                sms_history_ids: smsHistoryId,
                call_history_ids: callHistoryId,
                isTrash: 1,
            }
            response = await moveEmailToTrashApi(body)
        }

        if (response?.statusCode === 200) {
            if(mailData?.isOpened === 0){
                dispatch(inboxDataSliceActions.setUnReadCount((unReadCount || 0) - 1));
            }
            if(draftId){
                dispatch(inboxDataSliceActions.setMoveToTrash({ draft_ids : draftId }))
            } else {
                dispatch(inboxDataSliceActions.setMoveToTrash({
                    email_history_ids: emailhistoryId,
                    sms_history_ids: smsHistoryId,
                    call_history_ids: callHistoryId,
                }))
            }
            navigate('/inbox')
        }
    }

    const toggleTrashSelection = (item: any) => {
        const id = getSelectableItemId(item);
        if (!id) return;
        setSelectedTrashItemIds((prev) =>
            prev.includes(id) ? prev.filter((x) => x !== id) : [...prev, id]
        );
    };

    const organizeHistryId = (selectedTrashItemIds :any)=> {
        const emailhistoryIds: string[] = [];
        const draftIds: string[] = [];
        const smshistoryIds: string[] = [];
        const callhistoryIds: string[] = [];

        selectedTrashItemIds.forEach((id :any) => {
            if (id.startsWith("eh_")) emailhistoryIds.push(id.replace("eh_", ""));
            if (id.startsWith("d_")) draftIds.push(id.replace("d_", ""));
            if (id.startsWith("sh_")) smshistoryIds.push(id.replace("sh_", ""));
            if (id.startsWith("ch_")) callhistoryIds.push(id.replace("ch_", ""));
        });
        return  {
            emailhistoryIds,
            draftIds,
            smshistoryIds,
            callhistoryIds
        }
    }


    const bulkDeleteFromCurrentTab = async () => {
        if (selectedTrashItemIds.length === 0) return;

        const {emailhistoryIds, draftIds, smshistoryIds, callhistoryIds} = organizeHistryId(selectedTrashItemIds)

        let statusOk = false;

        if (activeFilterRef.current === "Trash") {
            if ([...emailhistoryIds, ...callhistoryIds, ...smshistoryIds].length > 0) {
                const response = await deleteTrashEmailApi({
                    domainId: LoginUserDetails?.domainId ?? LoginUserDetails?.domain_id ?? null,
                    emailhistoryIds: emailhistoryIds.join(","),
                    callhistoryIds: callhistoryIds.join(","),
                    smshistoryIds: smshistoryIds.join(",")
                });
                statusOk = statusOk || response?.statusCode === 200;
            }

            if (draftIds.length > 0) {
                const response = await updateDraftForEmailTrashApi({
                    domainId: LoginUserDetails?.domainId ?? LoginUserDetails?.domain_id ?? null,
                    draftId: draftIds.join(","),
                    isTrash: 1,
                });
                statusOk = statusOk || response?.statusCode === 200;
            }
        } else {
            if ([...emailhistoryIds, ...callhistoryIds, ...smshistoryIds].length > 0) {
                
                const response = await moveEmailToTrashApi({
                    email_history_ids: emailhistoryIds.join(","),
                    call_history_ids: callhistoryIds.join(","),
                    sms_history_ids: smshistoryIds.join(","),
                    isTrash: 1,
                });
                statusOk = statusOk || response?.statusCode === 200;
            }

            if (draftIds.length > 0) {
                const response = await updateDraftForEmailTrashApi({
                    domainId: LoginUserDetails?.domainId ?? LoginUserDetails?.domain_id ?? null,
                    draftId: draftIds.join(","),
                    isTrash: 1,
                });
                statusOk = statusOk || response?.statusCode === 200;
            }
        }

        if (statusOk) {
            setSelectedTrashItemIds([]);
            dispatch(inboxDataSliceActions.setInboxRefresh())
            // if([...emailhistoryIds, ...callhistoryIds, ...smshistoryIds].length > 0){
            //     dispatch(inboxDataSliceActions.setMoveToTrash({
            //         email_history_ids: emailhistoryIds.join(","),
            //         call_history_ids: callhistoryIds.join(","),
            //         sms_history_ids: smshistoryIds.join(","),
            //     }))
            // } else if(draftIds.length > 0){
            //     dispatch(inboxDataSliceActions.setMoveToTrash({ draft_ids : draftIds }))
            // }
        }
    };

    const openDeleteConfirmation = (action: () => Promise<void> | void) => {
        pendingDeleteActionRef.current = action;
        setOpenConfirmDelete(true);
    };

    const confirmDeleteAction = async () => {
        setOpenConfirmDelete(false);
        if (pendingDeleteActionRef.current) {
            await pendingDeleteActionRef.current();
        }
        pendingDeleteActionRef.current = null;
    };

    const openMoveTrashConfirmation = (action: () => Promise<void> | void) => {
        pendingTrashDeleteActionRef.current = action;
        setOpenMoveTrashDelete(true);
    };
    
    const confirmTrashDeleteAction = async () => {
        setOpenMoveTrashDelete(false);
        if (pendingTrashDeleteActionRef.current) {
            await pendingTrashDeleteActionRef.current();
        }
        pendingTrashDeleteActionRef.current = null;
    };



    const openRestoreConfirmation = (action: () => Promise<void> | void) => {
        pendingRestoreActionRef.current = action;
        setOpenConfirmRestore(true);
    };

    const confirmRestoreAction = async () => {
        setOpenConfirmRestore(false);
        if (pendingRestoreActionRef.current) {
            await pendingRestoreActionRef.current();
        }
        pendingRestoreActionRef.current = null;
    };

    const handleBulkDeleteClick = () => {
        if (selectedTrashItemIds.length === 0) return;
        openDeleteConfirmation(() => bulkDeleteFromCurrentTab());
    };

    const bulkRestoreFromTrash = async () => {

        if (selectedTrashItemIds.length === 0) return;

        const {emailhistoryIds, draftIds, smshistoryIds, callhistoryIds} = organizeHistryId(selectedTrashItemIds)

        let statusOk = false;
        if ([...emailhistoryIds, ...callhistoryIds, ...smshistoryIds].length > 0) {
            const response = await moveEmailToTrashApi({
                email_history_ids: emailhistoryIds.join(","),
                call_history_ids: callhistoryIds.join(","),
                sms_history_ids: smshistoryIds.join(","),
                isTrash: 0,
            });
            statusOk = statusOk || response?.statusCode === 200;
        }

        if (draftIds.length > 0) {
            const response = await updateDraftForEmailTrashApi({
                domainId: LoginUserDetails?.domainId ?? LoginUserDetails?.domain_id ?? null,
                draftId: draftIds.join(","),
                isTrash: 0,
            });
            statusOk = statusOk || response?.statusCode === 200;
        }

        if (statusOk) {
            setSelectedTrashItemIds([]);
            dispatch(inboxDataSliceActions.setInboxRefresh())
        }
    };

    const permanentlyDeleteTrashItem = async (mailData?: any) => {
        const emailhistoryId = getEmailHistoryId(mailData);
        const smsHistoryId = getSmsHistoryId(mailData);
        const callHistoryId = getCallHistoryId(mailData);

        if (!(emailhistoryId || smsHistoryId || callHistoryId)) return;

        const response = await deleteTrashEmailApi({
            domainId: LoginUserDetails?.domainId ?? LoginUserDetails?.domain_id ?? null,
            emailhistoryIds: emailhistoryId,
            callhistoryIds: callHistoryId,
            smshistoryIds: smsHistoryId
        });
        console.log("response", response)
        if (response?.statusCode === 200) {
            dispatch(inboxDataSliceActions.setMoveToTrash({
                email_history_ids: emailhistoryId,
                call_history_ids: callHistoryId,
                sms_history_ids: smsHistoryId
            }));
        }
    };

    const restoreTrashItem = async (mailData?: any) => {

            const emailhistoryId = getEmailHistoryId(mailData);
            const smsHistoryId = getSmsHistoryId(mailData);
            const callHistoryId = getCallHistoryId(mailData);

            const body = {
                email_history_ids: emailhistoryId,
                sms_history_ids: smsHistoryId,
                call_history_ids: callHistoryId,
                isTrash: 0,
            }
            const response = await moveEmailToTrashApi(body)

        if (response?.statusCode === 200) {
            dispatch(inboxDataSliceActions.setMoveToTrash(body))
        }
    };

    const getCurrentSelectableIds = () => {
        const source = Array.isArray(inboxList_allrecord) ? inboxList_allrecord : [];
        return source
            .map((item: any) => getSelectableItemId(item))
            .filter((id: string) => !!id);
    };

    const toggleSelectAllCurrent = () => {
        const ids = getCurrentSelectableIds();
        if (ids.length === 0) return;

        const allSelected = ids.every((id) => selectedTrashItemIds.includes(id));
        if (allSelected) {
            setSelectedTrashItemIds([]);
            return;
        }
        setSelectedTrashItemIds(ids);
    };

    const updateEmailOpenStatus = async (mailData: any, isOpened: number) => {
        const historyId = mailData?.EmailHistoryId ?? mailData?.SMSHistoryId ?? null;
        if (!historyId) return;
        dispatch(inboxDataSliceActions.markEmailOpenStatus({ historyId, isOpened }));
        dispatch(inboxDataSliceActions.setUnReadCount((unReadCount || 0) + (isOpened === 0 ? 1 : -1)));
        await updateEmilopenStatus({ historyId, isOpened });
    }

    const updateEmailPinStatus = async (mailData: any, isPin: number) => {
        const historyId = mailData?.EmailHistoryId ?? mailData?.SMSHistoryId ?? null;
        if (!historyId) return;
        dispatch(inboxDataSliceActions.markEmailPinStatus({ historyId, isPin }));
        await updateEmailPinStatusApi({ historyId, isPin });
    }

    useEffect(() => {
        if (emailId) {
            dispatch(inboxDataSliceActions.setCleanReplyForword());
            dispatch(inboxDataSliceActions.setComposeMailToggle(true))
        }
    }, [emailId])

    const openInbox = async (item: any, index: any) => {
        try {
            if (Number(item?.isDraft) === 1) {
                dispatch(inboxDataSliceActions.setCleanReplyForword());
                dispatch(inboxDataSliceActions.setComposeMailToggle(true));
                const query = new URLSearchParams({
                    channelType: "Email",
                    draftSessionId: String(item?.sessionId ?? item?.session_id ?? ""),
                    draftId: String(item?.draftId ?? item?.draft_id ?? ""),
                    sindex: index
                });
                navigate(`/inbox?${query.toString()}`, { state: { selectedDraft: item } });
                return;
            }

            const data: any = {
                channelType: item?.channelType ?? null,
                agentId: LoginUserDetails?.userId,
                historyId: 0
            };
            if (item?.channelType === "Email") {
                data.historyId = item?.EmailHistoryId
            } else if (item?.channelType === "Call") {
                data.historyId = item?.CallHistoryId
            } else if (item?.channelType === "Sms") {
                data.historyId = item?.SMSHistoryId
            }
            dispatch(inboxDataSliceActions.setComposeMailToggle(false))
            dispatch(inboxDataSliceActions.setPerticularInboxContactData(item));
            const query = new URLSearchParams({
                sessionId: item?.sessionId ?? "",
                channelType: item?.channelType ?? "",
                historyId: data?.historyId ?? "",
                CustomerId: item?.CustomerId ?? "",
                sindex: index
            });
            navigate(`/inbox?${query.toString()}`);

            if (item?.channelType === "Email" && Number(item?.isOpened) === 0 && data?.historyId) {
                dispatch(inboxDataSliceActions.markEmailOpened({ historyId: data.historyId }));
                updateEmilopenStatus({ historyId: data.historyId, isOpened: 1 });
                dispatch(inboxDataSliceActions.setUnReadCount((unReadCount || 0) - 1));
            }

        } catch (error) {
            console.log(error)
        }
    }

    const getPreview = (html) => {
        if (!html) return "";
        const text = html
            .replace(/<[^>]+>/g, "")
            .replace(/&nbsp;/gi, " ")
            .replace(/&amp;/gi, "&")
            .replace(/&lt;/gi, "<")
            .replace(/&gt;/gi, ">")
            .trim();
        const lines = text.split(/\n+/).filter(l => l.trim() !== "");
        if (!lines?.length) return "";
        return lines[0]?.length < 20 && lines[1] ? `${lines[0]} ${lines[1]}` : (lines[0] ?? "");
    };

    const getDraftRecipient = (item: any) => {
        const rawTo = item?.ToEmail ?? item?.toEmail ?? item?.to_email ?? "";
        const firstTo = String(rawTo).split(",")[0]?.trim();
        if (!firstTo) return "";
        return firstTo;
    };

    const getDraftTitle = (item: any) => {
        const recipient = getDraftRecipient(item);
        return recipient ? `Draft ${recipient}` : "Draft";
    };

    const getDraftSubject = (item: any) => {
        const subject = item?.EmailSubject ?? item?.emailSubject ?? item?.email_subject ?? "";
        return String(subject).trim() || "(No subject)";
    };

    const getDraftPreview = (item: any) => {
        const preview = getPreview(item?.message_body ?? "");
        return preview || "No preview is available";
    };

    const customerName = (item) => {
      try {
        if (Number(item?.isDraft) === 1) {
            return getDraftTitle(item);
        }
        return item?.customer_name?.trim() ? item?.customer_name 
        :item?.channelType?.toLowerCase() === "call" ?  item?.calledNumber
        : item?.channelType?.toLowerCase() === "email" ? item?.IsInBound === 1 ? item?.FromEmail :  item?.ToEmail
        : 'New Customer';

      } catch (error) {
        return 'New Customer';
      }
    };

    const isValidToMakeCall = (phoneNumber: string) => {
        if(!phoneNumber) return false;
        const sanitizedPhoneNumber = sanitizePhoneNumber(String(phoneNumber ?? ""));
        return /^\+?[1-9]\d{6,14}$/.test(sanitizedPhoneNumber) && filterQueueList.length > 0
    };

    const makecall = async (phoneNumber: any) => {
        // const isValidPhoneNumber = (value: string) => /^\+?[1-9]\d{6,14}$/.test(value);
        const sanitizedPhoneNumber = sanitizePhoneNumber(String(phoneNumber ?? ""));
        // if (!isValidPhoneNumber(sanitizedPhoneNumber)) {
        //     console.warn("Invalid phone number:", phoneNumber);
        //     return;
        // }
        // if (filterQueueList.length === 0) {
        //     return
        // }
        const currentUser = loginDeatils?.find((user: any) => user?.UserID == LoginUserDetails?.userId);
        const queueIds = `${currentUser?.queueId ?? ""}`.split(",")?.map((id: string) => id?.trim())?.filter(Boolean);
        const currentQueue = (Array.isArray(filterQueueList) ? filterQueueList : []).find((item: any) => queueIds?.includes(`${item?.qid}`));

        dispatch(CallPopUpAgentActions.setQueueName(currentQueue ?? {}))
        dispatch(CallPopUpAgentActions.updateTransferType({ transfer_type: "EXTERNAL" }));
       
        const contactData = await getContactDetails(null, phoneNumber);
        makeCallFunctionForAll(sanitizedPhoneNumber, 2, contactData?.[0] ?? {});
    }

    const isMailSelectedAll = useMemo(() => {
        const ids = getCurrentSelectableIds();
        return ids.length > 0 && ids.every((id) => selectedTrashItemIds.includes(id));
    },[selectedTrashItemIds, inboxList_allrecord])
    const isMailSelectedSome = useMemo(() => {
        const ids = getCurrentSelectableIds();
        return ids.length > 0 && ids.some((id) => selectedTrashItemIds.includes(id));
    },[selectedTrashItemIds, inboxList_allrecord])

    return (
        <div className={styles.leftcontainer} >

            <div className={`p12 ${styles.header}`}>

                {/* <label className="content">
                    <input type="checkbox" /> Select all
                </label>
                <div className={styles.rightend}>
                    <span><img src={openemail} /></span>
                    <span><svg xmlns="http://www.w3.org/2000/svg" width="18" height="21" viewBox="0 0 18 21" fill="none">
                        <path d="M7.2999 15.9995C6.8858 15.9995 6.5499 15.6636 6.5499 15.2495V6.6797C6.5499 6.2656 6.8858 5.9297 7.2999 5.9297C7.714 5.9297 8.0499 6.2656 8.0499 6.6797V15.2495C8.0499 15.6636 7.714 15.9995 7.2999 15.9995Z" fill="#757676" />
                        <path d="M10.5401 15.9995C10.126 15.9995 9.7901 15.6636 9.7901 15.2495V6.6797C9.7901 6.2656 10.126 5.9297 10.5401 5.9297C10.9542 5.9297 11.2901 6.2656 11.2901 6.6797V15.2495C11.2901 15.6636 10.9542 15.9995 10.5401 15.9995Z" fill="#757676" />
                        <path d="M17.0899 2.5298H13.1397V1.9199C13.1397 0.8613 12.3057 0 11.2803 0H6.5498C5.5244 0 4.6899 0.8613 4.6899 1.9199V2.5298H0.75C0.3359 2.5298 0 2.8657 0 3.2798C0 3.6939 0.3359 4.0298 0.75 4.0298H1.3543L2.1216 16.6734C2.2339 18.6119 3.8452 20.1304 5.7901 20.1304H12.0303C13.9737 20.1304 15.5938 18.6099 15.7188 16.6656L16.4866 4.0299H17.0899C17.504 4.0299 17.8399 3.694 17.8399 3.2799C17.8399 2.8658 17.504 2.5298 17.0899 2.5298ZM6.19 1.9199C6.19 1.6885 6.3516 1.5 6.5499 1.5H11.2804C11.4786 1.5 11.6398 1.6885 11.6398 1.9199V2.5298H6.1901L6.19 1.9199ZM14.2217 16.5718C14.1475 17.7261 13.1846 18.6304 12.0303 18.6304H5.7901C4.6392 18.6304 3.6851 17.7329 3.6187 16.5845L2.8569 4.0298H14.983L14.2217 16.5718Z" fill="#757676" />
                    </svg></span>
                </div> */}

                <label style={{display:"flex", alignItems:"center", gap:"6px"}} className={`content ${styles.checkboxdiv}`}>
                    {(bulkSelectionFilters.includes(activeFilterRef.current)) && (
                        isMailSelectedAll ? (<input
                            className="custom-checkbox"
                            type="checkbox"
                            checked={isMailSelectedAll}
                            onChange={toggleSelectAllCurrent}
                        />) : ( isMailSelectedSome ? <input
                            className="custom-checkbox deselect"
                            type="checkbox"
                            checked={true}
                            onChange={()=>  setSelectedTrashItemIds([])}
                        /> : <input
                            className="custom-checkbox"
                            type="checkbox"
                            checked={false}
                            onChange={toggleSelectAllCurrent}
                        />)
                    )}
                    {" "}
                    {/* {(bulkSelectionFilters.includes(activeFilterRef.current) && selectedTrashItemIds.length > 0)
                        ? (["Draft", "Trash"].includes(activeFilterRef.current) ? "Deleted Items" : "Conversations")
                        :  */}
                        Conversations
                    {activeFilter === 'Inbox' && !!totalEmailCount && (
                        <span style={{
                            backgroundColor: "#E5E7EB",
                            color: "#000",
                            marginLeft: 6,
                            padding: "2px 8px",
                            borderRadius: 10,
                            fontSize: 12,
                            display: "inline-block",
                            lineHeight: "16px",
                        }}>
                            {totalEmailCount}
                        </span>
                    )}
                </label>
                <div className={styles.rightend}>
                    {/* <span><img src={newemail} onClick={openCompose} /></span> */}

                    {/* <span className={styles.iconclickpoint}><img src={search} /></span> */}
                    {!selectedTrashItemIds?.length && (
                    <span style={{ position: "relative" }} className={styles.iconclickpoint}>
                        <img src={notes} onClick={openCompose} />
                        {false &&
                            <div className={"dropdowncompose"}>
                                <div className="list"> Email</div>
                                <div className="list"> Sms</div>
                                <div style={{ position: "relative" }} className="list call"> Call
                                    <div className="callhoveredoverlay">
                                        <div className="inputsection">
                                            <input type="text" placeholder="Search contact" />
                                        </div>
                                        <div className={styles.calllists}>
                                            <div className={styles.list}>
                                                <div className={styles.lefts}>
                                                    <div className={styles.avatorcontainer}>
                                                        AB
                                                    </div>

                                                    <div className={styles.info}>
                                                        <div className={styles.name}>Ruby</div>
                                                        <div className={styles.phn}>90375474921</div>
                                                    </div>
                                                </div>
                                                <div className={styles.rights}>
                                                    <img src={callicn} />
                                                </div>
                                            </div>
                                            <div className={styles.list}>
                                                <div className={styles.lefts}>
                                                    <div className={styles.avatorcontainer}>
                                                        <img src={avator1} />
                                                    </div>

                                                    <div className={styles.info}>
                                                        <div className={styles.name}>Ruby</div>
                                                        <div className={styles.phn}>90375474921</div>
                                                    </div>
                                                </div>
                                                <div className={styles.rights}>
                                                    <img src={callicn} />
                                                </div>
                                            </div>
                                            <div className={styles.list}>
                                                <div className={styles.lefts}>
                                                    <div className={styles.avatorcontainer}>
                                                        <img src={avator1} />
                                                    </div>

                                                    <div className={styles.info}>
                                                        <div className={styles.name}>Ruby</div>
                                                        <div className={styles.phn}>90375474921</div>
                                                    </div>
                                                </div>
                                                <div className={styles.rights}>
                                                    <img src={callicn} />
                                                </div>
                                            </div>
                                            <div className={styles.list}>
                                                <div className={styles.lefts}>
                                                    <div className={styles.avatorcontainer}>
                                                        <img src={avator1} />
                                                    </div>

                                                    <div className={styles.info}>
                                                        <div className={styles.name}>Ruby</div>
                                                        <div className={styles.phn}>90375474921</div>
                                                    </div>
                                                </div>
                                                <div className={styles.rights}>
                                                    <img src={callicn} />
                                                </div>
                                            </div>
                                        </div>

                                    </div>
                                </div>
                            </div>}
                            {
                                false && <DateRangeCalendar
                                onChange={(start, end) => console.log(start, end)}
                                initialStart={null}
                                initialEnd={null}
                                />
                            }
                    </span>
                    )}
                    {(activeFilterRef.current === "Trash" && selectedTrashItemIds.length > 0) && (
                        <>
                            <span className={styles.iconclickpoint} onClick={() => openRestoreConfirmation(() => bulkRestoreFromTrash())} title="Restore selected">
                                <svg width="24" height="24" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg">
                                    <path d="M20.315 11.4289C19.9391 11.4289 19.6344 11.7337 19.6344 12.1095C19.6344 16.3181 16.2106 19.7423 12.0016 19.7423C7.79261 19.7423 4.36921 16.318 4.36921 12.1095C4.36921 7.90104 7.79301 4.47674 12.0016 4.47674C13.3958 4.47674 14.7372 4.86804 15.9205 5.57984L14.9345 5.59824C14.4383 5.60794 14.0435 6.01804 14.0527 6.51424C14.0619 7.00514 14.4624 7.39684 14.9511 7.39684H14.9687L17.6197 7.34674C18.1159 7.33704 18.5107 6.92694 18.5015 6.43074L18.4514 3.77934C18.4422 3.28224 18.0259 2.88004 17.5354 2.89674C17.0392 2.90644 16.6444 3.31654 16.6536 3.81274L16.6654 4.44204C15.2594 3.58484 13.6614 3.11544 12.0015 3.11544C7.04251 3.11544 3.00781 7.15014 3.00781 12.1096C3.00781 17.0691 7.04251 21.1037 12.0015 21.1037C16.9605 21.1037 20.9956 17.069 20.9956 12.1096C20.9956 11.7337 20.6908 11.4289 20.315 11.4289Z" fill="#23232B"/>
                                </svg>
                            </span>
                        </>
                    )}
                    {selectedTrashItemIds.length > 0 && (
                        <>
                            <span className={styles.iconclickpoint} onClick={handleBulkDeleteClick} title="Delete selected">
                                <img src={deleteem} />
                            </span>
                        </>
                    )}
                    {/* <span  style={{ position: "relative" }}  className={styles.iconclickpoint}><img src={filter} />
                      {false &&
                            <div className={"dropdowncompose"}>
                                <div className="list"> All</div>
                                <div className="list"> Unread</div>
                               <div style={{ position: "relative" }} className="list submenu"> Email <span><img src={rightchevron}/></span>
                                   <div className="callhoveredoverlay subdropdown">
                                       <div className="list"> All Email</div>
                                          <div className="list"> Inbound</div>
                                             <div className="list"> Outbound</div>
                                              <div className="list"> Has files</div>

                                    </div>
                                </div>
                                  <div style={{ position: "relative" }} className="list submenu"> Sms <span><img src={rightchevron}/></span>
                                    <div className="callhoveredoverlay subdropdown">
                                       <div className="list"> All Email</div>
                                          <div className="list"> Inbound</div>
                                             <div className="list"> Outbound</div>
                                              <div className="list"> Has files</div>

                                    </div>
                                </div>
                                <div style={{ position: "relative" }} className="list submenu"> Call <span><img src={rightchevron}/></span>
                                    <div className="callhoveredoverlay subdropdown">
                                       <div className="list"> All Email</div>
                                          <div className="list"> Inbound</div>
                                             <div className="list"> Outbound</div>
                                              <div className="list"> Has files</div>

                                    </div>
                                </div>
                                  <div className="list"> Has files</div>
                            </div>}
                    </span>
                    <span style={{ position: "relative" }} className={styles.iconclickpoint}>
                        <img src={calender} />
                         {false &&
                            <div className={"dropdowncompose"}>
                                <div className="list"> Today</div>
                                <div className="list"> Yesterday</div>
                              
                            </div>}
                        
                        </span> */}
                </div>

            </div>
            {/* <div className={`p12 ${styles.header} ${styles.searchheader}`}>

                 <div className={styles.searchBox}>
          <img src={search} alt="search" />
          <input
            type="text"
            placeholder="Search "
          
          />
        </div>


            </div> */}

            <div className={styles.greycontainerwrapper} ref={containerRef}>
                {activeFilterRef.current === "Trash" && (
                    <div className={styles.trashInfoBox}>
                        Messages that have been in Trash more than 30 days will be automatically deleted.
                    </div>
                )}
                {inboxList_groupData?.map((item: any, index: number) => {
                    const [listDate, listItems]: any = Object.entries(item)[0];

                    return (
                        <div className={styles.greycontainer} key={`p_${index}`}>
                            
                            <div
                                className={`subtitle ${styles.datebox}`}
                                onClick={() => {
                                    setMinimizeSession(prev => {
                                        if (!prev[`p_${index}`]) {
                                            fetchData()
                                        }
                                        return ({
                                            ...prev,
                                            [`p_${index}`]: !prev[`p_${index}`]
                                        })
                                    })

                                }}
                            >
                                <span style={{ display: "grid", rotate: !minimizeSession[`p_${index}`] ? '0deg' : "180deg" }}>
                                    <img src={chevrondown} />
                                </span>
                                {listDate}
                            </div>

                            {!minimizeSession[`p_${index}`] && listItems?.map((item: any, idx: number) => {
                                const canShowMessageActions = ["Email", "Sms", 'Call'].includes(String(item?.channelType ?? ""));
                                const isDraftItem = Number(item?.isDraft) === 1;
                                const rowId = `${index}_${idx}`;
                                const selectableId = getSelectableItemId(item);
                                const isSelected = selectableId ? selectedTrashItemIds.includes(selectableId) : false;
                                const showCheckboxOnIcon = !!selectableId && (hoveredRowId === rowId || isSelected);
                                return (
                                    <div
                                        className={`${styles.item} ${(selectedindex === `${index}_${idx}`) ? styles.active : ''} ${isSelected ? styles.selected : ''} p12`}
                                        key={`c_${index}_${idx}`}
                                        // onMouseEnter={() => setHoveredRowId(rowId)}
                                        // onMouseLeave={() => setHoveredRowId((prev) => prev === rowId ? null : prev)}
                                        onClick={(e) => {
                                            if(selectedindex !== `${index}_${idx}`){
                                                const target = e.target as HTMLElement | null;
                                                if (target?.closest('[data-inbox-action-trigger="true"]') || target?.closest('[data-inbox-action-menu="true"]')) {
                                                    return;
                                                }
                                                openInbox(item, `${index}_${idx}`);
                                            }
                                        }}

                                    >

                                        <div className={styles.top}>
                                            <div className={styles.leftwithcheckbox}>
                                                
                                                   
                                                        <div className={`${styles.checkboxdiv} ${styles.havecheckboxinpt}`} onClick={(e) => { e.stopPropagation(); toggleTrashSelection(item); }}>
                                                            <input className="custom-checkbox small" type="checkbox" checked={isSelected} onChange={() => {}} />
                                                        </div>
                                                        <img src={channelIcons[item?.channelType?.toLowerCase()] || mailicon}  className={`${channelIcons[item?.channelType?.toLowerCase()] || mailicon}`}/>
                                                
                                                <div className={styles.namewithcontent}>
                                                    <p className="innertitle">
                                                        {isDraftItem ? (
                                                            <>
                                                                <span className={styles.draftText}>Draft</span>
                                                                <br/>
                                                                {getDraftRecipient(item) ? ` ${getDraftRecipient(item)}` : ""}
                                                            </>
                                                        ) : (
                                                            // <Tooltip text= {customerName(item)} >
                                                            customerName(item)
                                                            // </Tooltip>
                                                        )}
                                                    </p>
                                                    <span className="content truncate1">
                                                        {Number(item?.isDraft) === 1
                                                            ? getDraftSubject(item)
                                                            : (item?.channelType === 'Call' ? item?.customerNumber : item?.EmailSubject)}
                                                    </span>
                                                </div>
                                            </div>

                                            <div className={`${styles.rightend} ${styles.noht}`}>
                                            {/* {(selectedindex !== `${index}_${idx}`) &&  */}
                                                <div className={styles.namewithdot}>
                                                    <span className={styles.timespan}>
                                                        {item?.updatedAt
                                                            ? moment(item?.updatedAt).format('hh:mm A')
                                                            : moment(item?.createdAt).format('hh:mm A')}
                                                    </span>
                                                    {item?.isOpened === 0 && item?.channelType === 'Email' && <span className={styles.reddot}></span>}
                                                    {item?.is_pin == 1 && <svg width="24" height="24" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg">
                                                        <path d="M19.8598 8.63443L17.2426 6.19403L16.2763 5.29263C16.2763 5.29263 16.2756 5.29213 16.2753 5.29193L14.6255 3.75363C14.3228 3.47143 13.8477 3.48903 13.5654 3.79073C13.2827 4.09343 13.2993 4.56803 13.602 4.85123L14.7042 5.87903L10.844 10.0189C10.1918 9.70643 9.50667 9.50673 8.81887 9.44393C7.41267 9.31603 6.16167 9.75153 5.30427 10.6705C5.16857 10.816 5.09627 11.0094 5.10307 11.2086C5.10987 11.4068 5.19537 11.5953 5.34087 11.7311L8.39607 14.5805L4.10207 19.1852C3.81937 19.4879 3.83597 19.9625 4.13867 20.2457C4.28367 20.3805 4.46677 20.4469 4.65037 20.4469C4.85107 20.4469 5.05127 20.3668 5.19867 20.2086L9.49297 15.6034L12.5483 18.4528C12.6928 18.5876 12.8764 18.654 13.06 18.654C13.2607 18.654 13.4609 18.5739 13.6083 18.4157C14.4662 17.4958 14.8139 16.2204 14.5873 14.8239C14.4768 14.1423 14.2299 13.4728 13.8729 12.8442L17.734 8.70433L18.8363 9.73213C18.9808 9.86693 19.1649 9.93333 19.348 9.93333C19.5487 9.93333 19.7489 9.85323 19.8963 9.69503C20.179 9.39233 20.1625 8.91763 19.8598 8.63443ZM12.9296 16.7565L7.00577 11.2321C7.47597 10.9841 8.05117 10.8766 8.68207 10.9382C9.69037 11.03 10.709 11.5134 11.5508 12.2986C12.3921 13.0828 12.9448 14.0652 13.107 15.0642C13.2086 15.6902 13.1454 16.2702 12.9296 16.7565ZM12.9911 11.5905L12.1561 10.8112L15.8014 6.90233L16.637 7.68143L12.9911 11.5905Z" fill="#757676"/>
                                                    </svg>}
                                                </div>
                                                {/* } */}
                                                <div className={styles.whenselectediconsec}>
                                                    {/* {(item?.IsInBound == 0 && item?.channelType === 'Email' && activeFilterRef.current === 'Inbox') && <span */}
                                                    {(item?.channelType === 'Email' && !isDraftItem && activeFilterRef.current !== "Trash" && item?.is_replied === 1) && <span
                                                        // onClick={(e) => {
                                                        //     e.stopPropagation();
                                                        //     openForwardCompose(item);
                                                        // }}
                                                        style={{ cursor: "pointer" }}
                                                    >
                                                        <svg width="24" height="24" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg">
                                                            <path d="M12.0521 7.23408C12.0174 6.39618 11.9447 5.86198 11.8089 5.43038C11.6307 4.88348 11.3465 4.49678 10.9373 4.24878C10.2264 3.82108 9.38412 3.80838 8.62582 4.21268C7.80892 4.65218 6.87972 5.52718 5.43102 6.91778L4.70792 7.61308C3.71382 8.55937 3.16592 9.07988 3.05802 9.85718C3.03022 10.0535 3.03022 10.2625 3.05752 10.4558C3.16642 11.24 3.71962 11.7712 4.72402 12.7361L5.47552 13.4578C6.91542 14.8338 7.86712 15.743 8.70062 16.1502C9.28702 16.4432 9.96432 16.4803 10.557 16.2508C11.1039 16.0389 11.5209 15.6229 11.7303 15.0809C11.9281 14.5741 12.0081 13.9334 12.0428 13.0663C14.2098 13.1923 15.4774 13.6229 16.0067 14.4071C16.5668 15.2372 16.453 16.6717 15.6586 18.7938C15.5258 19.1483 15.6137 19.5477 15.8827 19.8104C16.0634 19.9881 16.3036 20.0819 16.5492 20.0819C16.6693 20.0819 16.7909 20.0594 16.9076 20.0126C18.8661 19.2226 20.9633 16.5673 20.9633 13.8739C20.9633 8.35628 16.0784 7.37368 12.0521 7.23408ZM17.5423 17.8415C18.0408 15.9714 17.9422 14.5935 17.2498 13.5681C16.3431 12.2243 14.573 11.6169 11.3313 11.5359H11.3127C11.1193 11.5359 10.9328 11.6111 10.7932 11.7449C10.6487 11.8836 10.5661 12.074 10.5627 12.2742C10.5466 13.3191 10.5012 14.1033 10.3317 14.5379C10.2741 14.6873 10.1676 14.7928 10.0158 14.8524C9.81802 14.9266 9.57782 14.912 9.36492 14.8055C8.73452 14.4979 7.80242 13.6063 6.46162 12.325L5.76292 11.6541C5.05102 10.9705 4.58082 10.5193 4.54322 10.2479C4.53492 10.1903 4.53492 10.1258 4.54322 10.0653C4.57932 9.80548 5.04172 9.36608 5.77222 8.67168L6.47042 7.99978C7.79222 6.73028 8.70582 5.87278 9.33472 5.53398C9.62522 5.37778 9.90502 5.37778 10.1614 5.53198C10.1931 5.55148 10.2898 5.61008 10.3802 5.88838C10.512 6.30638 10.5472 7.02218 10.5638 7.59738L10.5697 7.86688C10.538 8.10228 10.6195 8.34348 10.7972 8.51238L10.8065 8.52118C10.975 8.67938 11.2044 8.74968 11.4252 8.71938C17.1327 8.81218 19.4633 10.3102 19.4633 13.8737C19.4633 15.3757 18.5877 16.8981 17.5423 17.8415Z" fill="#757676" />
                                                        </svg>
                                                    </span>}
                                                    {canShowMessageActions && activeFilterRef.current !== "Draft" && (
                                                        <span
                                                            style={{ position: "relative" }}
                                                            onClick={(e) => {
                                                                e.stopPropagation();
                                                                setOpenActionMenuId((prev) => prev === `${index}_${idx}` ? null : `${index}_${idx}`);
                                                            }}
                                                            data-inbox-action-trigger="true"
                                                        >
                                                            <svg width="18" height="5" viewBox="0 0 18 5" fill="none" xmlns="http://www.w3.org/2000/svg">
                                                                <path fill-rule="evenodd" clip-rule="evenodd" d="M13.461 2.2435C13.461 3.4774 14.4706 4.487 15.7045 4.487C16.9384 4.487 17.948 3.4774 17.948 2.2435C17.948 1.0096 16.9384 0 15.7045 0C14.4706 0 13.461 1.0096 13.461 2.2435ZM11.2175 2.2435C11.2175 1.0096 10.2079 0 8.974 0C7.7401 0 6.7305 1.0096 6.7305 2.2435C6.7305 3.4774 7.7401 4.487 8.974 4.487C10.2079 4.487 11.2175 3.4774 11.2175 2.2435ZM2.2435 0C3.4774 0 4.487 1.0096 4.487 2.2435C4.487 3.4774 3.4774 4.487 2.2435 4.487C1.0096 4.487 0 3.4774 0 2.2435C0 1.0096 1.0096 0 2.2435 0Z" fill="#757676" />
                                                            </svg>
                                                            {openActionMenuId === `${index}_${idx}` && (
                                                                <div className={"dropdowncompose right"} data-inbox-action-menu="true">
                                                                    {activeFilterRef.current === "Trash" ? (
                                                                        <>
                                                                            <div
                                                                                className="list"
                                                                                onClick={(e) => {
                                                                                    e.stopPropagation();
                                                                                    openRestoreConfirmation(() => restoreTrashItem(item));
                                                                                    setOpenActionMenuId(null);
                                                                                }}
                                                                            >
                                                                                Restore
                                                                            </div>
                                                                            {!isDraftItem && hasMailDeleteAccess && (
                                                                                <div
                                                                                    className="list"
                                                                                    onClick={(e) => {
                                                                                        e.stopPropagation();
                                                                                        openDeleteConfirmation(() => permanentlyDeleteTrashItem(item));
                                                                                        setOpenActionMenuId(null);
                                                                                    }}
                                                                                >
                                                                                    Permanently Delete
                                                                                </div>
                                                                            )}
                                                                        </>
                                                                    ) : (
                                                                        <>
                                                                            {item?.IsInBound == 1 
                                                                                && item?.channelType !== "Call"
                                                                                && !isDraftItem
                                                                                && (
                                                                                <div className="list" onClick={(e) => { e.stopPropagation(); openReplyCompose(item); setOpenActionMenuId(null); }}>
                                                                                    Reply
                                                                                </div>
                                                                            )}

                                                                            {!isDraftItem && item?.channelType !== "Call" && <div
                                                                                className="list"
                                                                                onClick={(e) => {
                                                                                    e.stopPropagation();
                                                                                    updateEmailOpenStatus(item, Number(item?.isOpened ?? 0) === 0 ? 1 : 0);
                                                                                    setOpenActionMenuId(null);
                                                                                }}
                                                                            >
                                                                                {Number(item?.isOpened ?? 0) === 0 ? "Mark as read" : "Mark as unread"}
                                                                            </div>}
                                                                            {item?.channelType !== "Call" && <div
                                                                                className="list"
                                                                                onClick={(e) => {
                                                                                    e.stopPropagation();
                                                                                    updateEmailPinStatus(item, Number(item?.is_pin ?? 0) === 0 ? 1 : 0);
                                                                                    setOpenActionMenuId(null);
                                                                                }}
                                                                            >
                                                                                { Number(item?.is_pin ?? 0) === 1 ? "Unpin Conversation" : "Pin Conversation"}
                                                                            </div>}
                                                                            {!isDraftItem  && hasMailDeleteAccess && (
                                                                                <div
                                                                                    className="list"
                                                                                    onClick={(e) => {
                                                                                        e.stopPropagation();
                                                                                        openMoveTrashConfirmation(() => moveEmailToTrash(item));
                                                                                        setOpenActionMenuId(null);
                                                                                    }}
                                                                                >
                                                                                    Move to Trash
                                                                                </div>
                                                                            )}

                                                                            {isValidToMakeCall(item?.customerNumber) && (
                                                                                <div 
                                                                                    className="list"
                                                                                    onClick={(e) => { 
                                                                                        e.stopPropagation(); 
                                                                                        makecall(item?.customerNumber); 
                                                                                        setOpenActionMenuId(null); 
                                                                                    }}> 
                                                                                    Make a Call 
                                                                                </div>
                                                                            )}
                                                                        </>
                                                                    )}
                                                                </div>
                                                            )}
                                                        </span>
                                                    )}
                                                </div>
                                                {/* <span>
                                                    <img src={threedot} />
                                                </span> */}
                                            </div>
                                        </div>

                                        {item?.channelType === 'Call' ?
                                            <div className={`content ${styles.contentinner} truncate1 `}>
                                                {item?.IsInBound == 1 ? 'Inbound Call' : 'Outbound Call'}
                                            </div>
                                            :
                                            <div className={`content ${styles.contentinner} truncate1 `}
                                                dangerouslySetInnerHTML={{ __html: Number(item?.isDraft) === 1 ? getDraftPreview(item) : getPreview(item?.message_body) }} />
                                        }
                                    </div>
                                )
                            })}
                        </div>
                    )
                })}
                {
                    (inboxList_groupData?.length === 0 && !loader) &&
                    <div style={{ height: '100%', display: 'flex', flexDirection: "column", alignItems: 'center', justifyContent: 'center' }}>
                        <Nodata />
                    </div>
                }
                {(loader && inboxList_groupData?.length === 0) && (
                    <>
                        <SkeletonBox width="100%" height="20px" />
                        {Array.from({ length: 10 })?.map((_, index) => (
                            <SkeletonBox key={`inbox-skeleton-${index}`} width="100%" height="60px" />
                        ))}
                    </>
                )}
            </div>

            {openMoveTrashDelete && (
                <Modal
                    header={<div className={"modalheader"}><div>Move to Trash?</div></div>}
                    children={<div className={"modalcontent"}>Are you sure you want to move this conversation to Trash?</div>}
                    btntxtsecondary={"Cancel"}
                    btntxtprimary={"Move to Trash"}
                    onclosePrimary={confirmTrashDeleteAction}
                    oncloseSecondary={() => {
                        pendingTrashDeleteActionRef.current = null;
                        setOpenMoveTrashDelete(false);
                    }}
                />
            )}
            {openConfirmDelete && (
                <Modal
                    header={<div className={"modalheader"}><div>Delete</div></div>}
                    children={<div className={"modalcontent"}>Do you want to permanently delete the selected conversations?</div>}
                    btntxtsecondary={"Cancel"}
                    btntxtprimary={"Yes, delete"}
                    onclosePrimary={confirmDeleteAction}
                    oncloseSecondary={() => {
                        pendingDeleteActionRef.current = null;
                        setOpenConfirmDelete(false);
                    }}
                />
            )}
            {openConfirmRestore && (
                <Modal
                    header={<div className={"modalheader"}><div>Restore mail</div></div>}
                    children={<div className={"modalcontent"}>Are you sure you want to Restore this mail?</div>}
                    btntxtsecondary={"Cancel"}
                    btntxtprimary={"Restore"}
                    onclosePrimary={confirmRestoreAction}
                    oncloseSecondary={() => {
                        pendingRestoreActionRef.current = null;
                        setOpenConfirmRestore(false);
                    }}
                />
            )}

        </div>
    )
})

export default LeftWorkspace
