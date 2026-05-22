import store from "../../store/redux-store/root_store";
import { contactDataSliceActions } from "../../store/slice/contactSlice";
import { homeDataSliceActions } from "../../store/slice/homeSlice";
import { inboxDataSliceActions } from "../../store/slice/inbox/inboxSlice";
import { LoginActions } from "../../store/slice/loginSlice/loginSlice";
import { MENUS } from "../constant";
import { Cookies } from "../cookies";
import { LocalStorage } from "../custom_hooks/localStorageKeys";
import { GetItemFromStorage, SetItemInStorage } from "../custom_hooks/useStorage";
import { getClientData } from "../device";
import { groupbyday } from "../utils";
import { CoreServices } from "./apiservices";
import jwt_decode from "jwt-decode";

export const getContactDetails = async (customerId: any, contactValues: any) => {
    try {
        const normalizedCustomerId = customerId === null || customerId === undefined || customerId === "null" ? "null" : customerId;
        const hasContactValues = !(contactValues === null || contactValues === undefined || contactValues === "null");
        const endPoint = hasContactValues ? `/get_customer_contact/${normalizedCustomerId}?contactValues=${contactValues}` : `/get_customer_contact/${normalizedCustomerId}`;
        const response = await CoreServices.get(endPoint);
        if (response?.data?.statusCode === 200) {
            const res = response?.data?.customerContactRes;
            return res;
        }
        return [];
    } catch (error) {
        console.log(error);
        return null;
    }
};

export const getContactDetailsDomainList = async (data: any) => {
    try {
        const body = {
            offset: data?.offset ?? 0,
            limit: data?.limit ?? 100,
            ...data
        };
        const response = await CoreServices.post(body, '/get_customer_contact_list');
        if (response?.data?.statusCode === 200) {
            const res = response?.data?.response;
            return res;
        }
        return [];
    } catch (error) {
        console.log(error);
        return null;
    }
};

export const getCompanyDetails = async (params: any) => {
    try {
        const body = {
            company_name: params?.company_name ?? null,
            owner_name: params?.owner_name ?? null,
            source_name: params?.source_name ?? null,
            from_date: params?.from_date ?? null,
            to_date: params?.to_date ?? null,
            industry: params?.industry ?? null,
            company_address: params?.company_address ?? null,
            company_size: params?.company_size ?? null,
            offset: params?.offset ?? 0,
            limit: params?.limit ?? 100,

        };
        const response = await CoreServices.post(body, '/get_company_details');
        if (response?.data?.statusCode === 200) {
            const res = response?.data?.companyDetailsRes ?? response?.data?.response ?? [];
            return res;
        }
        return [];
    } catch (error) {
        console.log(error);
        return null;
    }
};

export const getCompanyContactDetails = async (params: any) => {
    try {
        const companyParam = encodeURIComponent(String(params));
        const endPoint = `/get_company_customer/${companyParam}`;
        const response = await CoreServices.get(endPoint);
        if (response?.data?.statusCode === 200) {
            const res = response?.data?.companyCustomerRes;
            return res;
        }
        return [];
    } catch (error) {
        console.log(error);
        return null;
    }
};

export const getAgentStatusDetails = async () => {
    try {
        const endPoint = `/get_agent_status`;
        const response = await CoreServices.get(endPoint);
        if (response?.data?.statusCode === 200) {
            const res = response?.data?.getAgentListRes;
            return res;
        }
        return [];
    } catch (error) {
        console.log(error);
        return null;
    }
};

export const fetchAndStoreCrmUserDetails = async () => {
    try {
        const response: any = await CoreServices.get(`/get_users_details`);
        if (
            response?.data?.statusCode === 200 &&
            Array.isArray(response?.data?.getUserDetailsRes) &&
            response?.data?.getUserDetailsRes.length > 0
        ) {
            SetItemInStorage(LocalStorage.LOGIN_USER_CRM_DETAILS, response.data.getUserDetailsRes);
        }
    } catch (error) {
        console.log("get_user_details_email error>>>>", error);
    }
};

export const getCallSessionDetails = async (data: any) => {
    try {
        const sessionId = typeof data === "string" ? data : data?.sessionId;
        if (!sessionId) return null;
        const endPoint = `/get_callSessionId/${sessionId}`;
        const response = await CoreServices.get(endPoint);
        if (response?.data?.statusCode === 200) {
            return response.data;
        }
        return null;
    } catch (error) {
        console.log(error);
        return null;
    }
};

export const setQueueList = async () => {
    try {
        const endPoint = `/get_queue_list`;
        const response = await CoreServices.get(endPoint);
        if (response?.data?.statusCode === 200) {
            return response.data;
        }
        return null;
    } catch (error) {
        console.log(error);
        return null;
    }
};

export const getSessionHistoryDetails = async (data: any) => {
    try {
        const endPoint = '/get_session_history_details';
        const response = await CoreServices.post(data, endPoint);
        if (response?.data?.statusCode === 200) {
            return response.data;
        }
        return null;
    } catch (error) {
        console.log(error);
        return null;
    }
};

export const getCompanySessionDetails = async (data: any) => {
    try {
        const endPoint = '/get_company_session_history_details';
        const response = await CoreServices.post(data, endPoint);
        if (response?.data?.statusCode === 200) {
            return response.data;
        }
        return null;
    } catch (error) {
        console.log(error);
        return null;
    }
};

export const getInboxList = async (body: any, dispatch: any) => {
    try {
        const { inboxList_allrecord } = store.getState().inboxChatStore;

        const endPoint = '/get_inbox_session_details';
        const response = await CoreServices.post(body, endPoint);
        if (response?.data?.statusCode === 200) {
            const res = response?.data?.inboxSessionDetailsRes ?? [];

            if (body?.offset === 0) {
                const data = groupbyday(res) ?? [];
                dispatch(inboxDataSliceActions.setInboxList({
                    allrecord: res,
                    groupData: data
                }));
            } else {
                const exsit = [...inboxList_allrecord ?? []]
                const data = groupbyday([...exsit, ...res]) ?? [];
                dispatch(inboxDataSliceActions.setInboxList({
                    allrecord: [...exsit, ...res],
                    groupData: data
                }));
            }

        }
        return [];
    } catch (error) {
        console.log(error);
        return [];
    }
}

export const insertUpdateNotes = async (data: any) => {
    try {
        const endPoint = '/insert_update_notes';
        const response = await CoreServices.post(data, endPoint);
        if (response?.data?.statusCode === 200) {
            return response.data;
        }
        return null;
    } catch (error) {
        console.log(error);
        return null;
    }
};

export const insertDraftForEmailApi = async (data: any) => {
    try {
        const body = {
            draftUniqueId: data?.draftUniqueId ?? data?.draft_unique_id ?? null,
            domainId: data?.domainId ?? data?.domain_id ?? null,
            fromEmail: data?.fromEmail ?? data?.from_email ?? null,
            toEmail: data?.toEmail ?? data?.to_email ?? null,
            cc: data?.cc ?? data?.cc ?? 0,
            bcc: data?.bcc ?? data?.bcc ?? 0,
            emailSubject: data?.emailSubject ?? data?.email_subject ?? null,
            messageJson: data?.messageJson ?? data?.message_json ?? null,
            agentId: data?.agentId ?? data?.agent_id ?? null,
            is_sent: data?.is_sent ?? data?.isSent ?? 0,
        };
        const endPoint = '/insert_draft_for_email';
        const response = await CoreServices.post(body, endPoint);
        if (response?.data?.statusCode === 200) {
            return response.data;
        }
        return null;
    } catch (error) {
        console.log('insertDraftForEmailApi Error', error);
        return null;
    }
};

export const getDraftForEmailApi = async (data: any) => {
    try {
        const body = { ...data };
        const endPoint = '/get_draft_for_email';
        const response = await CoreServices.post(body, endPoint);
        if (response?.data?.statusCode === 200) {
            return response?.data?.response ?? [];
        }
        return [];
    } catch (error) {
        console.log('getDraftForEmailApi Error', error);
        return [];
    }
};


export const handleFileUpload = async (files: any) => {
    try {
        const formData = new FormData();
        const fileArray = files instanceof FileList ? Array.from(files) : [files];

        for (const file of fileArray) {
            formData.append("doc", file);
        }

        const res = await CoreServices.post(formData, `/file_upload`);
        if (res.data.statusCode === 200) {
            return res.data.fileList;
        } else {
            return [];
        }
    } catch (error) {
        console.error("File upload failed:", error);
        return [];
    }
};

export const getDomainEmailConfig = async (dispatch: any) => {
    try {
        const endPoint = '/get_emailConfig';
        const response = await CoreServices.get(endPoint);
        if (response?.data?.statusCode === 200) {
            const res = response?.data?.response?.filter((i: any) => i?.primaryEmail === 1);
            dispatch(inboxDataSliceActions.setDomainEmailconfig(res[0] ?? {}));
            return response.data;
        }
        return null;
    } catch (error) {
        console.log(error);
        return null;
    }
};

export const getPerticularInboxSessionDetails = async (data: any, dispatch: any, isDispatch = true) => {
    try {
        const body = {
            channelType: data?.channelType ?? null,
            historyId: data?.historyId ?? null,
            agentId: null
        };

        const endPoint = '/get_perticular_inbox_session_details';
        const response = await CoreServices.post(body, endPoint);
        if (response?.data?.statusCode === 200) {
            const res = response?.data?.response;
            if (isDispatch) {
                dispatch(inboxDataSliceActions.setPerticularInboxData(res));
            }
            return res;
        }
        return null;
    } catch (error) {
        console.log(error);
        return null;
    }
};

export const getCustomerDealsApi = async (data: any) => {
    try {
        const body = {
            customerId: data?.customerId ?? data?.customer_id ?? null,
            domainId: data?.domainId ?? data?.domain_id ?? null,
            agentId: data?.agentId ?? data?.agent_id ?? null,
            dealName: data?.dealName ?? data?.deal_name ?? data?.search ?? null,
            contact: data?.contact ?? null,
            companyName: data?.companyName ?? data?.company_name ?? null,
            pipelineStage: data?.pipelineStage ?? data?.pipeline_stage ?? null,
            fromDate: data?.fromDate ?? data?.from_date ?? null,
            toDate: data?.toDate ?? data?.to_date ?? null,
            dealEstimatedClosed: data?.dealEstimatedClosed ?? data?.deal_estimated_closed ?? data?.estimated_closed_date ?? null,
            location: data?.location ?? null,
            totalAmount: data?.totalAmount ?? data?.total_amount ?? null,
            closedAmount: data?.closedAmount ?? data?.closed_amount ?? null,
            ownerName: data?.ownerName ?? data?.owner_name ?? null,
        };
        const endPoint = '/get_customer_deal_details_filter';
        const response = await CoreServices.post(body, endPoint);
        if (response?.data?.statusCode === 200) {
            return response.data;
        }
        return null;
    } catch (error) {
        console.log('getCustomerDealsApi Error', error);
        return null;
    }
};

export const moveEmailToTrashApi = async (data: any) => {
    try {
        const endPoint = '/update_email_trash';
        const response = await CoreServices.post(data, endPoint);
        if (response?.data?.statusCode === 200) {
            return response.data;
        }
        return null;
    } catch (error) {
        console.log('moveEmailToTrashApi Error', error);
        return null;
    }
}

export const updateDraftForEmailTrashApi = async (data: any) => {
    try {
        const body = {
            domainId: data?.domainId ?? data?.domain_id ?? null,
            draftId: data?.draftId ?? data?.draft_id ?? null,
            isTrash: data?.isTrash ?? data?.istrash ?? 0,
        };
        const endPoint = '/update_draft_for_email_trash';
        const response = await CoreServices.post(body, endPoint);
        if (response?.data?.statusCode === 200) {
            return response.data;
        }
        return null;
    } catch (error) {
        console.log('updateDraftForEmailTrashApi Error', error);
        return null;
    }
}

export const deleteTrashEmailApi = async (data: any) => {
    try {
        const body = {
            domainId: data?.domainId ?? data?.domain_id ?? null,
            email_history_ids: data?.emailhistoryIds || null,
            call_history_ids: data?.callhistoryIds || null,
            sms_history_ids: data?.smshistoryIds || null,
        };
        const endPoint = '/delete_trash_email';
        const response = await CoreServices.post(body, endPoint);
        if (response?.data?.statusCode === 200) {
            return response.data;
        }
        return null;
    } catch (error) {
        console.log('deleteTrashEmailApi Error', error);
        return null;
    }
}

export const updateEmailPinStatusApi = async (data: any) => {
    try {
        const body = {
            historyId: data?.historyId ?? data?.EmailHistoryId ?? data?.SMSHistoryId ?? null,
            isPin: data?.isPin ?? null,
        };
        if (!body?.historyId) return null;
        const endPoint = '/update_email_pin';
        const response = await CoreServices.post(body, endPoint);
        if (response?.data?.statusCode === 200) {
            return response.data;
        }
        return null;
    } catch (error) {
        console.log('updateEmailPinStatusApi Error', error);
        return null;
    }
}

export const updateEmilopenStatus = async (data: any) => {
    try {
        const historyId = data?.historyId ?? data?.EmailHistoryId ?? null;
        if (!historyId) return null;
        const body = {
            historyId,
            isOpened: data?.isOpened ?? 0,
        }
        const endPoint = '/update_email_open_status';
        const response = await CoreServices.post(body, endPoint);
        if (response?.data?.statusCode === 200) {
            return response.data;
        }
        return null;
    } catch (error) {
        console.log('updateEmilopenStatus Error', error);
        return null;
    }
}

export const getNotificationApi = async (data: any, dispatch: any) => {
    try {
        const LoginUserDetails = GetItemFromStorage(LocalStorage.USER_DETAILS);
        let agentId = data?.agentId;
        if (!agentId) agentId = LoginUserDetails?.userId
        const endPoint = '/get_crm_notification';
        const response = await CoreServices.post({ agentId }, endPoint);
        if (response?.data?.statusCode === 200) {
            dispatch(homeDataSliceActions.setNotification(response?.data?.notificationData ?? []));
            return response.data;
        }
        return null;
    } catch (error) {
        console.log('getNotificationApi Error', error);
        return null;
    }
}

export const get_contact_projects = async (data: any) => {
    try {
        const endPoint = '/crm_get_projects';
        const response = await CoreServices.post(data, endPoint);
        if (response?.data?.statusCode === 200) {
            return response.data;
        }
        return null;
    } catch (error) {
        console.log('crm_get_projects Error', error);
        return null;
    }
}

export const get_Individual_projects = async (data: any) => {
    try {
        const endPoint = '/crm_get_individual_project';
        const response = await CoreServices.post(data, endPoint);
        if (response?.data?.statusCode === 200) {
            return response.data;
        }
        return null;
    } catch (error) {
        console.log('crm_get_individual_project Error', error);
        return null;
    }
}

export const getProductDetailsApi = async (data: any) => {
    try {
        const endPoint = '/get_product_catalog_list';
        const response = await CoreServices.post(data, endPoint);
        if (response?.data?.statusCode === 200) {
            return response.data;
        }
        return null;
    } catch (error) {
        console.log('getProductDetailsApi Error', error);
        return null;
    }
}

export const getIndividualProductDetailsApi = async (data: any) => {
    try {
        const endPoint = '/get_individual_product_catalog_details';
        const response = await CoreServices.post(data, endPoint);
        if (response?.data?.statusCode === 200) {
            return response.data;
        }
        return null;
    } catch (error) {
        console.log('getProductDetailsApi Error', error);
        return null;
    }
}

export const getScheduleMeetingApi = async (data: any, dispatch: any) => {
    try {
        const endPoint = '/crm_get_scheduled_meetings';
        const response = await CoreServices.post(data, endPoint);
        if (response?.data?.statusCode === 200) {
            dispatch(contactDataSliceActions.setContactScheduledata(response?.data?.response ?? []));
            return response?.data?.response;
        }
        return null;
    } catch (error) {
        console.log('getProductDetailsApi Error', error);
        return null;
    }
}

export const getProductInsightPerformanceRankingApi = async () => {
    try {
        const endPoint = '/get_product_insight_performance_ranking';
        const response = await CoreServices.get(endPoint);
        if (response?.data?.statusCode === 200) {
            return response.data;
        }
        return null;
    } catch (error) {
        console.log('getProductInsightPerformanceRankingApi Error', error);
        return null;
    }
}

export const getProductInsightProductRevenueApi = async () => {
    try {
        const endPoint = '/get_product_insight_revenue_by_product';
        const response = await CoreServices.get(endPoint);
        if (response?.data?.statusCode === 200) {
            return response.data;
        }
        return null;
    } catch (error) {
        console.log('getProductInsightProductRevenueApi Error', error);
        return null;
    }
}

export const getProductInsightRevenueGrowthApi = async () => {
    try {
        const endPoint = '/get_product_insight_revenue_growth';
        const response = await CoreServices.get(endPoint);
        if (response?.data?.statusCode === 200) {
            return response.data;
        }
        return null;
    } catch (error) {
        console.log('getProductInsightRevenueGrowthApi Error', error);
        return null;
    }
}

export const getCustomerNotesApi = async (data: any, dispatch: any) => {
    try {
        const endPoint = '/get_customer_notes';
        const response = await CoreServices.post(data, endPoint);
        if (response?.data?.statusCode === 200) {
            dispatch(inboxDataSliceActions.setCustomerNotes(response?.data?.response ?? []));
            return response?.data?.response;
        }
        return null;
    } catch (error) {
        console.log('getCustomerNotesApi Error', error);
        return null;
    }
}

export const getUserRolesPermisions = async (dispatch: any) => {
    try {
        const endPoint = '/get_user_roles_permissions';
        const response = await CoreServices.get(endPoint);
        if (response?.data?.statusCode === 200 && response?.data?.response?.length > 0) {
            dispatch(LoginActions.setRolesAndPermissions(response?.data?.response[0] ?? {}));
            dispatch(LoginActions.setRolesAndPermissions_menu(MENUS));
            return response?.data?.response;
        }
        return null;
    } catch (error) {
        console.log('getCustomerNotesApi Error', error);
        return null;
    }
}


export const emailotpverification = async (data: any) => {
    try {
        const endPoint = '/email_otp_verification';
        const response = await CoreServices.post(data, endPoint);
        if (response?.data?.statusCode === 200) {
            return response.data;
        }
        return null;
    } catch (error) {
        console.log(error);
        return null;
    }
};


export const getProjectInsightApi = async (data: any) => {
    try {
        const agentId = data?.userId ?? null;
        const endPoint = `/get_project_insights/${agentId ?? null}`;
        const response = await CoreServices.get(endPoint);
        if (response?.data?.statusCode === 200) {
            return response.data;
        }
        return null;
    } catch (error) {
        console.log(error);
        return null;
    }
};


export const emailOtpVerification= async (data: any) => {
    try {
        const endPoint = '/check_otp_validation';
        const response = await CoreServices.post(data, endPoint);
        if (response?.data?.statusCode === 200) {
            return response.data;
        }
        return null;
    } catch (error) {
        console.log(error);
        return null;
    }
};

export const updatePassword = async (data: any) => {
    try {
        const endPoint = '/update_password';
        const response = await CoreServices.post(data, endPoint);
        if (response?.data?.statusCode === 200) {
            return response.data;
        }
        return null;
    } catch (error) {
        console.log(error);
        return null;
    }
};


export const resendOtp = async (data: any) => {
    try {
        const endPoint = '/resend_otp';
        const response = await CoreServices.post(data, endPoint);
        if (response?.data?.statusCode === 200) {
            return response.data;
        }
        return null;
    } catch (error) {
        console.log(error);
        return null;
    }
};

export const getDealActivityHistoryDetails = async (data: any) => {
    try {
        const endPoint = '/get_customer_deal_activity';
        const response = await CoreServices.post(data, endPoint);
        if (response?.data?.statusCode === 200) {
            return response.data;
        }
        return null;
    } catch (error) {
        console.log(error);
        return null;
    }
};

export const saveDeviceInfo = async (datanew: any) => {
    try {
        const data = datanew?.currentDevice
        let tokenData: any = Cookies.get();
        let Authorization: any = tokenData['ssoToken'] ?? "";
        let LoginUserDetails: any = jwt_decode(Authorization);
        const body = {
            companyId: LoginUserDetails?.companyId,
            userEmail: LoginUserDetails?.Email,
            device: data?.deviceType,
            deviceModal: data?.deviceModel,
            osType: data?.osName,
            browserType: data?.browserName,
            ipAddress: data?.ipAddress,
            location: data?.location?.ipBased?.city,
            isActive: 1,
            productType: 'AICCAAS',
            deviceId: data?.deviceSignature
        }

        const res = await CoreServices.post(body, '/insert_user_devices_dtl').catch((e) => console.log(e))
        return res

    } catch (error) {
        return []
    }
}


export const updateDeviceDetails = async () => {
    try {
        const getDetils: any = await getClientData();
        const body = {
            Device: getDetils.deviceName ?? '',
            Device_Model: getDetils.device ?? '',
            OS_Type: getDetils.os ?? '',
            Browser_Type: getDetils.browser ?? '',
            IP_Address: getDetils.ip ?? '',
            Location: `${getDetils?.location?.city || ''} ${getDetils?.country || ''}`.trim() ?? '',
            is_active: 1,
            product_type: "AICCAAS"

        }
        CoreServices.post(body, '/insert_update_user_devices')
            .then((res) => {
                console.log(res)
            })

    } catch (error) {

    }
}

export const insertUserActivityLogInLog = () => {
    try {
        let tokenData: any = Cookies.get();
        let Authorization: any = tokenData['ssoToken'] ?? "";
        let LoginUserDetails: any = jwt_decode(Authorization);
        console.log('inside insertUserActivityLogInLog>>>>>>>>>', LoginUserDetails);
        const body = {
            userId: LoginUserDetails?.userId,
            roleId: LoginUserDetails?.roleId,
            messagelog: `${LoginUserDetails?.first_name} Logged Out Successfully `
        }
        CoreServices.post(body, `/insert_user_activity_log`).then((value: any) => {
            if (value.status === 200) {
                console.log('insertUserActivityLogInLog >>>>>', value);
            }
        });
    } catch (err) {
        console.log('err', err)
    }
}

export const insertLogoutLog = async () => {
    try {
        const tokenData: any = Cookies.get();
        const authorization: any = tokenData?.ssoToken ?? "";
        const loginUserDetails: any = authorization ? jwt_decode(authorization) : {};
        const productId = Number(GetItemFromStorage(LocalStorage.PRODUCT_ID)) || loginUserDetails?.product_id || 1;

        const body = {
            username: loginUserDetails?.Email ?? loginUserDetails?.username ?? GetItemFromStorage(LocalStorage.USER_EMAIL) ?? "",
            password: "",
            deviceId: GetItemFromStorage("deviceid") ?? "",
            source: "CRM",
            ipAddress: "136.144.56.255:443",
            deviceType: "Web",
            browser: "",
            loginType: "",
            productId
        };

        const response = await CoreServices.post(body, "/insert_logout_log");
        if (response?.data?.statusCode === 200 || response?.status === 200) {
            return response?.data ?? response;
        }
        return null;
    } catch (err) {
        console.log("insertLogoutLog error", err);
        return null;
    }
}

export const insertLoginLog = async () => {
    try {
        const tokenData: any = Cookies.get();
        const authorization: any = tokenData?.ssoToken ?? "";
        const loginUserDetails: any = authorization ? jwt_decode(authorization) : {};
        const productId = Number(GetItemFromStorage(LocalStorage.PRODUCT_ID)) || loginUserDetails?.product_id || 1;

        const body = {
            username: loginUserDetails?.Email ?? loginUserDetails?.username ?? GetItemFromStorage(LocalStorage.USER_EMAIL) ?? "",
            password: "",
            deviceId: GetItemFromStorage("deviceid") ?? "",
            source: "CRM",
            ipAddress: "136.144.56.255:443",
            deviceType: "Web",
            browser: "",
            loginType: "",
            productId
        };

        const response = await CoreServices.post(body, "/insert_login_log");
        if (response?.data?.statusCode === 200 || response?.status === 200) {
            return response?.data ?? response;
        }
        return null;
    } catch (err) {
        console.log("insertLoginLog error", err);
        return null;
    }
}

export const updateUnreadNotification = async (data: any) => {
    try {
        const endPoint = '/update_unread_notification';
        const response = await CoreServices.post(data, endPoint);
        if (response?.data?.statusCode === 200) {
            return response.data;
        }
        return null;
    } catch (error) {
        console.log(error);
        return null;
    }
};

export const getInboxUnreadMsgCount = async (body :any) => {
    try {
        const endPoint = '/get_inbox_unread_msg_count';
        const response = await CoreServices.post(body, endPoint);
        if (response?.data?.statusCode === 200) {
            const res = response?.data?.response ?? [];

            if(res?.length > 0){
                const count = res?.reduce((acc :any, cur :any) => acc + cur.unread_count, 0)
                store.dispatch(inboxDataSliceActions.setUnReadCount(count))

                const emailObj= res.find((i)=> i.channel_type === 'Email')
                if(emailObj) {
                    const emailCount = emailObj?.total_count || 0;
                    store.dispatch(inboxDataSliceActions.setTotalEmailCount(emailCount))
                }
            }
        }
        return [];
    } catch (error) {
        console.log(error);
        return [];
    }
};

export const getAIChathistroyData = async (body :any) => {
    try {
        const endPoint = '/get_aichat_search_history_log';
        const response = await CoreServices.post(body, endPoint);
        if (response?.data?.statusCode === 200) {
            const res = response?.data?.response ?? [];
            return res;
        }
        return [];
    } catch (error) {
        console.log(error);
        return [];
    }
};

export const deleteAIChathistroyData = async (data :any) => {
    try {
        const endPoint = '/delete_aichat_search_history_log';
        const response = await CoreServices.post(data, endPoint);
        if (response?.data?.statusCode === 200) {
            return response?.data;
        }
        return {};
    } catch (error) {
        console.log(error);
        return {};
    }
};

export const insertUpdateAIChathistroyData = async (data :any) => {
    try {
        const endPoint = '/insert_update_aichat_search_history_log';
        const response = await CoreServices.post(data, endPoint);
        if (response?.data?.statusCode === 200) {
            return response?.data;
        }
        return {};
    } catch (error) {
        console.log(error);
        return {};
    }
};

export const getCheckEmailconfig = async (dispatch: any) => {
    try {
        const endPoint = '/crm_get_check_email_config';
        const response = await CoreServices.get(endPoint);
        if (response?.data?.statusCode === 200 && response?.data?.response?.length > 0) {
            return response?.data?.response;
        }
        return null;
    } catch (error) {
        console.log('getCustomerNotesApi Error', error);
        return null;
    }
}

export const getEmailSuggestion = async (data: any) => {
    try {
        const body = {
            searchEmail: data ?? ''
        };
        const response = await CoreServices.post(body, '/get_email_suggestion_details');
        if (response?.data?.statusCode === 200) {
            const res = response?.data?.response;
            return res;
        }
        return [];
    } catch (error) {
        console.log(error);
        return null;
    }
};