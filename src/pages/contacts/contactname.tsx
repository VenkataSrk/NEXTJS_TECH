import { MouseEvent as ReactMouseEvent, useEffect, useMemo, useRef, useState } from 'react';
import styles from './scss/contactname.module.scss'
import Callicon from '../../assets/images/contacts/contactcall.svg'
import Emaillicon from '../../assets/images/contacts/contactmail.svg'
import Notesicon from '../../assets/images/contacts/notes.svg'
import Calendaricon from '../../assets/images/contacts/calendar.svg'
import Voicemailicon from '../../assets/images/callwidget/voicemailactivityicon.svg'
import Email from '../../assets/images/contacts/activityemail.svg'
import Calliconlist from '../../assets/images/contacts/callflowicon.svg'
import SmsIcon from '../../assets/images/contacts/SmsIcon.svg'
import Profileicon from '../../assets/images/contacts/profileavatar.svg'
import Activityicon from '../../assets/images/contacts/activityicon.svg'
import Notesactivity from '../../assets/images/contacts/notesactivity.svg'
import Meetinglist from '../../assets/images/contacts/meetingeditor.svg'
import Worktualdropdown from '../component/worktualdropdown';
import Deals from './deals';
import Whatsupcoming from './whatupcoming';
import Contactproject from './contactproject';
import { createInfiniteScroll, formatDate, getScheduleTime, removeSpaceNormalizetext, validateEmail } from '../../base/utils';
import { useNavigate, useParams, useSearchParams } from 'react-router-dom';
import { useDispatcher } from '../../store/redux-store/dispatch';
import { CallService } from '../../base/service/call_service';
import { CallPopUpAgentActions } from '../../store/slice/callSlice/call_popup_agent';
import { useSelector } from 'react-redux';
import { getContactDetails, getSessionHistoryDetails, handleFileUpload, insertUpdateNotes } from '../../base/service/core_serviceApi';
import { GetItemFromStorage } from '../../base/custom_hooks/useStorage';
import { LocalStorage } from '../../base/custom_hooks/localStorageKeys';
import moment from 'moment';
import WorktualVisualization from '../component/audioplayer/audioplayer';
import preview from "../../assets/images/Attachment.png"
import TextEditor from '../inbox/texteditor';
import { generateAIMessage, updateNLPJoinMeet } from '../../base/service/nlp_service';
import { commonDataSliceActions } from '../../store/slice/commonSilce/commonSlice';
import { inboxDataSliceActions } from '../../store/slice/inbox/inboxSlice';
import { SentimentScore } from '../../base/shared/sentimental';
import AIIcon from '../../assets/images/aiicon.svg'
import Emailicon from '../../assets/images/projectinsights/emailactivityicon.svg';
import Nodata from '../component/nodata';
import SkeletonBox from '../marketing/skeletonbox';
import { v4 as uuidv4 } from "uuid";

import Smsicon from '../../assets/images/projectinsights/smsicon.svg'
import CustomSingleSelectDropdown from '../component/CustomSingleSelect';
import DateRangeCalendar from '../../base/shared/DateRangeCalendar';
import Dealscard from './dealscard';
import { contactDataSliceActions } from '../../store/slice/contactSlice';
import Emailininbox from './emailininbox';
import jwtDecode from 'jwt-decode';
import aiChatAssistSocketservice from '../../services/aiChatService';
import { ACTIONS_ACCESS, levelOfAccess, MENUS } from '../../base/constant';
import Draggable from "react-draggable";
import { toastnotification } from '../../base/shared/toastmsg';
import Catalog from '../product/catalog';
import Productname from '../product/productname';
const aiOptions = [
    {
        label: 'Formal', value: <div className={styles.formalist}><svg xmlns="http://www.w3.org/2000/svg" width="9" height="19" viewBox="0 0 9 19" fill="none">
            <path d="M5.21414 4.1518H3.03084C2.93684 4.1518 2.85584 4.2067 2.83674 4.2835L0.0174414 15.5523C-0.0467586 15.8089 0.0692413 16.0736 0.316541 16.2352L3.73594 18.4696C3.84944 18.5437 3.98574 18.5807 4.12194 18.5807C4.25864 18.5807 4.39534 18.5435 4.50894 18.4691L8.01434 16.1717C8.20134 16.049 8.28924 15.8483 8.24064 15.6539L5.40834 4.2837C5.38934 4.207 5.30814 4.1518 5.21414 4.1518ZM4.12134 16.9295L1.62674 15.2995L4.04054 5.6518H4.20314L6.60674 15.3006L4.12134 16.9295Z" fill="#404040" />
            <path d="M2.31844 3.434C2.36114 3.5728 2.51084 3.6698 2.68244 3.6698H5.57114C5.74274 3.6698 5.89264 3.5728 5.93544 3.434L6.87074 0.3918C6.93174 0.1933 6.75234 0 6.50674 0H1.74724C1.50164 0 1.32194 0.1933 1.38294 0.3918L2.31844 3.434ZM5.23154 1.2999L4.90244 2.3698H3.35114L3.02224 1.2999H5.23154Z" fill="#404040" />
        </svg> Formal</div>
    },
    {
        label:'Friendly' , value: <div className={styles.formalist}><svg xmlns="http://www.w3.org/2000/svg" width="19" height="13" viewBox="0 0 19 13" fill="none">
            <path d="M0 3.50093V9.53973C0 9.93553 0.3208 10.2564 0.7168 10.2564H1.8008C2.1966 10.2564 2.5176 9.93553 2.5176 9.53973V2.78423H0.7168C0.3208 2.78423 0 3.10513 0 3.50093Z" fill="#404040" />
            <path d="M18.2575 2.78823H16.4567V9.54383C16.4567 9.93963 16.7775 10.2605 17.1735 10.2605H18.2575C18.6535 10.2605 18.9741 9.93963 18.9741 9.54383V3.50493C18.9741 3.10913 18.6535 2.78823 18.2575 2.78823Z" fill="#404040" />
            <path d="M15.7998 2.42653C15.7947 2.23373 15.6628 1.95733 15.5154 1.83153C12.7661 -0.528973 10.6048 0.0671275 10.6048 0.0671275C10.5914 0.0702275 10.5047 0.0970274 10.4925 0.101127C10.2974 0.164027 10.1397 0.253727 9.9942 0.344528L9.473 0.705227C7.849 -0.336573 5.9588 -0.0445728 3.8485 1.59743L3.7103 1.70463C3.2894 2.01403 3.2264 2.39343 3.2264 2.58533L3.1595 8.59103C3.1553 8.96333 3.4503 9.27153 3.8226 9.28393L4.4124 9.30453L6.8089 11.7814C7.2193 12.2269 7.7225 12.4713 8.2288 12.4713H8.2371C8.6929 12.4692 9.1084 12.2743 9.4385 11.9073C9.4663 11.8764 9.492 11.8454 9.5169 11.8134C9.8387 12.0609 10.221 12.1929 10.6233 12.1929H10.6336C11.1585 12.1898 11.6482 11.9619 12.0052 11.5566C12.034 11.5246 12.0599 11.4927 12.0845 11.4607C12.699 11.7535 13.387 11.6484 13.8479 11.4411C14.749 11.8175 15.176 11.1967 15.3171 10.9048C15.4431 10.647 15.3957 10.3377 15.1965 10.1293L9.904 4.58943L10.2592 4.34123L14.6252 9.01983C14.7583 9.16213 14.9409 9.23843 15.1276 9.23843C15.2121 9.23843 15.2975 9.22193 15.38 9.18993C15.642 9.08573 15.8141 8.83213 15.8141 8.55063L15.8009 2.44593C15.8009 2.43973 15.7998 2.43273 15.7998 2.42653ZM13.3323 10.1647C13.2664 10.1997 12.8064 10.4225 12.5166 10.0997L12.2185 9.76873C12.0298 9.56043 11.7358 9.48823 11.4709 9.58313C11.208 9.68003 11.0295 9.92753 11.0222 10.208L11.0193 10.3297C11.0161 10.5235 11.011 10.6092 10.9812 10.6432C10.8801 10.7567 10.7541 10.8195 10.6252 10.8206H10.6232C10.4943 10.8206 10.3715 10.7608 10.2704 10.6484L9.7951 10.1193C9.6105 9.91413 9.322 9.83783 9.0588 9.92963C8.7981 10.0213 8.6164 10.2606 8.6001 10.5359C8.5886 10.7122 8.5256 10.869 8.4175 10.9896C8.3196 11.0989 8.2547 11.0989 8.2298 11.0989C8.1422 11.0989 7.9866 11.0339 7.8069 10.839L5.2063 8.15063C5.0837 8.02273 4.9146 7.94853 4.7371 7.94233L4.5403 7.93503L4.5979 2.75223L4.6907 2.68103C5.6746 1.91563 6.9234 1.15123 8.1907 1.59243L6.6253 2.67583C6.1107 3.00993 5.8025 3.56063 5.8025 4.14943C5.8025 4.56403 5.9487 4.95073 6.218 5.26213C6.8264 5.98703 7.7678 6.05713 8.6868 5.43953L8.7654 5.38463L13.3323 10.1647ZM10.8562 2.97003C10.7212 2.82573 10.5388 2.75143 10.3542 2.75143C10.217 2.75143 10.0798 2.79273 9.9611 2.87523L7.9101 4.30753C7.4636 4.60963 7.3337 4.45603 7.2624 4.37143C7.2038 4.30443 7.176 4.23123 7.176 4.14973C7.176 4.02803 7.2492 3.90843 7.3894 3.81663L10.7449 1.49333C10.8181 1.44803 10.8697 1.42213 10.9646 1.39223C10.98 1.38813 12.4546 1.11073 14.4285 2.71113L14.4378 6.80703L10.8562 2.97003Z" fill="#404040" />
        </svg> Friendly</div>
    },
    {
        label: 'Concise', value: <div className={styles.formalist}><svg xmlns="http://www.w3.org/2000/svg" width="17" height="17" viewBox="0 0 17 17" fill="none">
            <path d="M12.868 9.32437C12.277 9.32437 11.7282 9.48257 11.2382 9.73847L3.442 0.274573C3.1783 -0.0467271 2.7052 -0.0916273 2.3858 0.172073C2.0664 0.435773 2.0206 0.908373 2.2837 1.22777L7.2345 7.23757L5.1741 9.73857C4.6842 9.48267 4.1355 9.32447 3.5446 9.32447C1.587 9.32447 0 10.9114 0 12.8689C0 14.8264 1.587 16.4133 3.5446 16.4133C5.5022 16.4133 7.0892 14.8264 7.0892 12.8689C7.0892 12.0466 6.7975 11.2991 6.3275 10.6975L8.2063 8.41717L10.0851 10.698C9.6151 11.2991 9.3236 12.0466 9.3236 12.8689C9.3236 14.8264 10.9105 16.4133 12.8681 16.4133C14.8257 16.4133 16.4127 14.8264 16.4127 12.8689C16.4127 10.9114 14.8257 9.32437 12.868 9.32437ZM3.5446 14.9133C2.4172 14.9133 1.5 13.9963 1.5 12.8689C1.5 11.7415 2.4172 10.8245 3.5446 10.8245C4.672 10.8245 5.5892 11.7415 5.5892 12.8689C5.5892 13.9963 4.672 14.9133 3.5446 14.9133ZM12.868 14.9133C11.7407 14.9133 10.8235 13.9963 10.8235 12.8689C10.8235 11.7415 11.7407 10.8245 12.868 10.8245C13.9953 10.8245 14.9126 11.7415 14.9126 12.8689C14.9126 13.9963 13.9954 14.9133 12.868 14.9133Z" fill="#404040" />
            <path d="M14.129 1.22767C14.3922 0.908373 14.3463 0.435673 14.0269 0.171973C13.7071 -0.0926272 13.2344 -0.0468272 12.9707 0.274473L8.9646 5.13727L9.9363 6.31697L14.129 1.22767Z" fill="#404040" />
        </svg> Concise</div>
    },
    {
        label:'Elaborate', value: <div className={styles.formalist}><svg xmlns="http://www.w3.org/2000/svg" width="17" height="17" viewBox="0 0 17 17" fill="none">
            <path d="M2.5606 1.5H5.1895C5.6036 1.5 5.9395 1.1641 5.9395 0.75C5.9395 0.3359 5.6036 0 5.1895 0H2.0069C0.9005 0 0.000100136 0.9004 0.000100136 2.0068V5.1904C0.000100136 5.6045 0.336 5.9404 0.7501 5.9404C1.1642 5.9404 1.5001 5.6045 1.5001 5.1904V2.5605L5.5812 6.6416C5.7277 6.7881 5.9191 6.8613 6.1115 6.8613C6.3039 6.8613 6.4953 6.7881 6.6418 6.6416C6.9348 6.3486 6.9348 5.874 6.6418 5.5811L2.5606 1.5Z" fill="#404040" />
            <path d="M5.5811 9.6749L1.5 13.756V11.1271C1.5 10.713 1.1641 10.3771 0.75 10.3771C0.3359 10.3771 0 10.713 0 11.1271V14.3097C0 15.4161 0.9004 16.3165 2.0068 16.3165H5.1894C5.6035 16.3165 5.9394 15.9806 5.9394 15.5665C5.9394 15.1524 5.6035 14.8165 5.1894 14.8165H2.5605L6.6416 10.7354C6.9346 10.4424 6.9346 9.9678 6.6416 9.6749C6.3486 9.382 5.874 9.3819 5.5811 9.6749Z" fill="#404040" />
            <path d="M14.2296 0H11.046C10.6319 0 10.296 0.3359 10.296 0.75C10.296 1.1641 10.6319 1.5 11.046 1.5H13.6759L9.5948 5.5811C9.3018 5.8741 9.3018 6.3487 9.5948 6.6416C9.7413 6.7881 9.9327 6.8613 10.1251 6.8613C10.3175 6.8613 10.5089 6.7881 10.6554 6.6416L14.7365 2.5605V5.1904C14.7365 5.6045 15.0724 5.9404 15.4865 5.9404C15.9006 5.9404 16.2365 5.6045 16.2365 5.1904V2.0068C16.2365 0.9004 15.336 0 14.2296 0Z" fill="#404040" />
            <path d="M15.4864 10.377C15.0723 10.377 14.7364 10.7129 14.7364 11.127V13.7559L10.6553 9.6748C10.3623 9.3818 9.8877 9.3818 9.5948 9.6748C9.3019 9.9678 9.3018 10.4424 9.5948 10.7353L13.6759 14.8164H11.046C10.6319 14.8164 10.296 15.1523 10.296 15.5664C10.296 15.9805 10.6319 16.3164 11.046 16.3164H14.2296C15.336 16.3164 16.2364 15.416 16.2364 14.3096V11.127C16.2364 10.7129 15.9005 10.377 15.4864 10.377Z" fill="#404040" />
        </svg> Elaborate</div>
    },
];

const Contactname = () => {
    const [activeTab, setActiveTab] = useState("customer");
    const { filterQueueList } = useSelector((store: any) => store.commonData)
    const loginDeatils: any = GetItemFromStorage(LocalStorage.LOGIN_USER_CRM_DETAILS);
    const navigate = useNavigate();
    const dispatch = useDispatcher();
    const { makeCallFunctionForAll } = CallService(dispatch, navigate);
    const { selectedContact: selectedContactMap, selectedContactLoading: selectedContactLoadingMap } = useSelector((store: any) => store.contactStore);
    const [params] = useSearchParams()
    const { appTabs } = useSelector((state: any) => state.commonData);
    const [selectedOverallFilter, setSelectedOverallFilter] = useState('All activity');
    const [selectedSubFilter, setSelectedSubFilter] = useState<string | null>(null);
    const [showCustomDatePicker, setShowCustomDatePicker] = useState(false);
    const [customDateRange, setCustomDateRange] = useState<{ start: Date | null; end: Date | null }>({ start: null, end: null });
    const [customDateDraft, setCustomDateDraft] = useState<{ start: Date | null; end: Date | null }>({ start: null, end: null });
    const customDateRef = useRef<HTMLDivElement | null>(null);
    const token = GetItemFromStorage(LocalStorage.ACCESS_TOKEN);
    const LoginCcaasUserDetails: any = token ? jwtDecode(token) : {};
    const customerId = params.get('coid')
    const customer_deal_id = params.get('deal_id')
    const { configurationChannel } = useSelector((store :any)=>store?.settingsStore)

    const contactData = (customerId && selectedContactMap?.[customerId]) || {};
    const loading = !!(customerId && selectedContactLoadingMap?.[customerId]);

    useEffect(() => {
        getContantacts();
        dispatch(contactDataSliceActions.setCustomerEmail({}));
    }, [customerId])

    const getContantacts = async () => {
        if (!customerId) return;
        dispatch(contactDataSliceActions.setSelectedContact({ coid: customerId, selectedContactLoading: true }));
        try {
            const res = await getContactDetails(customerId, null);
            const record = Array.isArray(res) ? (res[0] ?? {}) : (res ?? {});
            dispatch(contactDataSliceActions.setSelectedContact({
                coid: customerId,
                selectedContact: record,
                selectedContactLoading: false,
            }));
            const fullName = `${record?.first_name || ""} ${record?.last_name || ""}`.trim() || "New Customer";

            dispatch(commonDataSliceActions.setTabMenuPush({
                path: '/contact/contactdetais?coid=' + customerId,
                menuName: fullName,
                close: 1,
                type: "contact"
            }));
        } catch (err) {
            console.error("getContactDetails error:", err);
            dispatch(contactDataSliceActions.setSelectedContact({ coid: customerId, selectedContactLoading: false }));
        }
    }

    const fullName = `${contactData?.first_name || ""} ${contactData?.last_name || ""}`.trim() || "New Customer";
    const initials = `${contactData?.first_name?.charAt(0) || ""}${contactData?.last_name?.charAt(0) || ""}`.toUpperCase() || "N";
    const emailValue = getPrimaryData(contactData?.primary_email,contactData?.secondary_email)
    const phoneValue = getPrimaryData(contactData?.primary_phone_number,contactData?.secondary_phone_number)

    function getPrimaryData(primary: string, secondary: string) {
        try {
          const unique = new Map();
          if (primary) {
            unique.set(primary.trim().toLowerCase(), `${primary} (Primary)`);
          }
          if (secondary) {
            const key = secondary.trim().toLowerCase();
            if (!unique.has(key)) {
              unique.set(key, secondary);
            }
          }
          return unique.size ? Array.from(unique.values()).join("\n") : "-";
        } catch (err) {
          console.error("Email format error:", err);
          return "-";
        }
      }
   
   
    const selectedPhoneNumber = contactData?.primary_phone_number || contactData?.secondary_phone_number || "";
    const behaviorScore = Number(contactData?.behaviors ?? 0);
    const demoScore = Number(contactData?.demo_graphics ?? 0);
    const firmoScore = Number(contactData?.firmo_graphics ?? 0);
    const LeadScore = Math.round(((behaviorScore + demoScore + firmoScore) / 300) * 100);
    const leadScoreTag = LeadScore < 40 ? "Unqualified" : LeadScore < 70 ? "MQL" : "Qualified";
    const selectedProducts = contactData?.additional_info?.product_interest || "-";
    // const selectedProducts = (Array.isArray(contactData?.additional_info?.interested_product) && contactData?.additional_info?.interested_product?.length > 0)
    //     ? contactData.additional_info.interested_product.join(", ")
    //     : contactData?.additional_info?.product_interest || "-";

        const persons = [
        { label: "Contact Name", name: fullName },
        { label: "Owner Name", name: contactData?.sales_name ?? "-" },
        { label: "Email", name: emailValue },
        { label: "Phone", name: phoneValue },
        { label: "Address", name: contactData?.address ?? "-" },
        { label: "Designation", name: contactData?.additional_info?.designation ?? "-" },
        { label: "Industry", name: contactData?.industry_type ?? "-" },
        { label: "Source", name: contactData?.source_type ?? "-" },
        { label: "Company Size", name: contactData?.company_size ?? "-" },
        { label: "Created Date", name: contactData?.created_at ? formatDate(contactData.created_at) : "-" },
        { label: "Company Name", name: contactData?.company_name ?? "-" },
        { label: "City", name: contactData?.additional_info?.city ?? "-" },
        { label: "Turn Over", name: contactData?.additional_info?.turnover ?? "-" },
        { label: "Selected Products", name: selectedProducts},
    ];
    const value = contactData?.additional_info?.lead_score_summary;
    const leadSummary = typeof value === 'string' ? value.trim() : '-';    
    const userDetails = GetItemFromStorage(LocalStorage.USER_DETAILS);
    const LoginUserDetails: any = userDetails;
    const lastScrollTop = useRef(0);
    const [activityData, setActivityData] = useState<any>([])
    const [loader, setLoader] = useState(false);
    const [offset, setOffset] = useState(0);
    const [hasMore, setHasMore] = useState(true);
    const hasMoreRef = useRef(hasMore);
    const loaderRef = useRef(loader);
    const [channelType, setChannelType] = useState<any>(null);
    const [showNotes, setShowNote] = useState<boolean>(false);
    const [sethowTextEditer, setShowTextEditer] = useState<boolean>(false);
    const [noteText, setNoteText] = useState<string>("");
    const [noteEditorKey, setNoteEditorKey] = useState<number>(0);
    const [noteImages, setNoteImages] = useState<any>("");
    const noteImageInputRef = useRef<HTMLInputElement | null>(null);
    const { activityFlag } = useSelector((state: any) => state.commonData);
    const { permissions } = useSelector((store: any) => store?.loginUser?.rolesandpermissions)

    useEffect(() => {
        if (!activityFlag) return;

        setActiveTab('activity');
        dispatch(commonDataSliceActions.setActivityFlag(false));
    }, [activityFlag])

    useEffect(() => {
        hasMoreRef.current = hasMore;
    }, [hasMore]);

    useEffect(() => {
        loaderRef.current = loader;
    }, [loader]);

    useEffect(() => {
        setActivityData([]);
        setHasMore(true);
        setOffset(0);
    }, [channelType, customerId]);

    useEffect(() => {
        if (!contactData?.primary_phone_number && !contactData?.primary_email) {
            return;
        }
    
        getSessionHistory(channelType, offset, offset !== 0);
    
    }, [offset, channelType, contactData, selectedOverallFilter, selectedSubFilter, customDateRange.start, customDateRange.end]);

    useEffect(() => {
        const closeCustomDatePicker = (event: MouseEvent) => {
            if (!customDateRef.current?.contains(event.target as Node)) {
                setShowCustomDatePicker(false);
            }
        };
        document.addEventListener('mousedown', closeCustomDatePicker);
        return () => document.removeEventListener('mousedown', closeCustomDatePicker);
    }, []);

    const getMeetingTagForDb = (subFilter: string | null) => {
        const sub = String(subFilter ?? '').trim().toLowerCase();
        if (sub === 'scheduled' || sub === 'schedule meeting') return 'scheduled';
        if (sub === 'rescheduled') return 'rescheduled';
        if (sub === 'completed') return 'Completed';
        if (sub === 'cancelled' || sub === 'canceled') return 'Cancelled';
        return null;
    };

    const getDateRangeFilters = (overallFilter: string, subFilter: string | null) => {
        if (overallFilter !== 'Notes') return { from_date: null, to_date: null };

        const sub = String(subFilter ?? '').trim().toLowerCase();
        const today = moment().format('YYYY-MM-DD');
        if (sub === 'custom date') {
            const start = customDateRange.start ? moment(customDateRange.start).format('YYYY-MM-DD') : null;
            const endDate = customDateRange.end ?? customDateRange.start;
            const end = endDate ? moment(endDate).format('YYYY-MM-DD') : null;
            return { from_date: start, to_date: end };
        }
        if (sub === 'yesterday') {
            const day = moment().subtract(1, 'day').format('YYYY-MM-DD');
            return { from_date: day, to_date: day };
        }
        if (sub === 'last week') return { from_date: moment().subtract(7, 'days').format('YYYY-MM-DD'), to_date: today };
        if (sub === 'last month') return { from_date: moment().subtract(1, 'month').format('YYYY-MM-DD'), to_date: today };
        if (sub === 'last year') return { from_date: moment().subtract(1, 'year').format('YYYY-MM-DD'), to_date: today };
        return { from_date: null, to_date: null };
    };

    const getSessionHistoryDbFilters = (overallFilter: string, subFilter: string | null) => {
        const sub = String(subFilter ?? '').trim().toLowerCase();
        const isAllSubFilter = sub === '' || sub.startsWith('all ');
        const { from_date, to_date } = getDateRangeFilters(overallFilter, subFilter);
        const inboundForMeeting = overallFilter === 'Meeting' ? 'url' : 0;
        const inboundForOthers = isAllSubFilter ? null : sub === 'inbound' ? 1 : sub === 'outbound' ? 0 : null;

        return {
            inbound: overallFilter === 'Meeting' ? inboundForMeeting : inboundForOthers,
            unread: sub === 'unread' ? 1 : null,
            missed: sub === 'missed' ? 1 : null,
            voicemail: sub === 'voicemail' ? 1 : null,
            tags: overallFilter === 'Meeting' ? getMeetingTagForDb(subFilter) : null,
            from_date,
            to_date,
        };
    };
    
    console.log("LoginUserDetails", LoginUserDetails)
    const getSessionHistory = async (channelTypeParam: any = null, pageOffset: number = 0, append = false) => {
        try {
            if(!contactData) return;
            
            setLoader(true);
            const data = {
                domainId: LoginUserDetails?.domainId,
                channelType: channelTypeParam,
                agentId: LoginUserDetails?.org_role_id_crm == 3 ? null : LoginUserDetails?.userId,
                customerNumber: contactData?.primary_phone_number ?? null,
                customerEmail: contactData?.primary_email ?? null,
                offset: pageOffset,
                limit: 50,
                customer_id: customerId,
                ...getSessionHistoryDbFilters(selectedOverallFilter, selectedSubFilter),
            };
            const response = await getSessionHistoryDetails(data);
            if (response?.statusCode === 200) {
                const list = response?.customerContactRes || [];
                if (list?.length === 0) setHasMore(false);
                const groups: any = {};

                const iconMap: any = {
                    Call: Calliconlist,
                    Email: Email,
                    Notes: Notesactivity,
                    Meeting: Meetinglist,
                    Video: Meetinglist,
                };

                list?.forEach((row: any, i: number) => {
                    const time = row?.createdAt;
                    const label = moment(time).calendar(null, {
                        sameDay: '[Today]',
                        lastDay: '[Yesterday]',
                        lastWeek: 'DD-MM-YYYY',
                        sameElse: 'DD-MM-YYYY',
                        nextDay: 'DD-MM-YYYY',
                        nextWeek: 'DD-MM-YYYY',
                    });
                    if (!groups[label]) groups[label] = [];
                    groups[label].push({
                        ...row,
                        id: row?.sessionId + i + pageOffset,
                        icon: iconMap[row?.channelType] || Activityicon,
                        type: row?.channelType !== 'Call' ? row?.channelType : (row?.channelType === 'Call' && row?.IsInBound == 0) ? 'Outbound Call' : 'Inbound Call' ,
                        date: row?.channelType === 'Video' ? getScheduleTime(row) : '',
                        title: row?.channelType === 'Notes' ? row?.Notes : row?.channelType === 'Email' ?  row?.EmailSubject : row?.summary ? row?.summary : '-' ,
                        from: row?.agent_name,

                    });
                });
                const formatted = Object.keys(groups).map((label) => ({
                    label,
                    items: groups[label],
                }));
                if (append) {
                    setActivityData((prev: any[]) => {
                        const updated = [...prev];

                        formatted?.forEach((group: any) => {
                            const existing = updated?.find((g) => g.label === group.label);

                            if (existing) {
                                existing.items = [...existing.items, ...group.items];
                            } else {
                                updated?.push(group);
                            }
                        });

                        return updated;
                    });
                } else {
                    setActivityData(formatted);
                }
            }
        } catch (error) {
            console.log(error);
        } finally {
            setLoader(false);
        }
    };

    const handleDropdownChange = (option: any) => {
        const type = option?.channelType ?? (option?.value === "All activity" ? null : option?.value);
        setOffset(0);
        setHasMore(true);
        setChannelType(type);
    };

    const subFilterOptionsByOverall: Record<string, { key: string; value: string }[]> = {
        Email: [
            { key: 'all_email', value: 'All Email' },
            { key: 'unread', value: 'Unread' },
            { key: 'inbound', value: 'Inbound' },
            { key: 'outbound', value: 'Outbound' },
            { key: 'has_files', value: 'Has files' },
        ],
        Call: [
            { key: 'all_calls', value: 'All Calls' },
            { key: 'inbound', value: 'Inbound' },
            { key: 'outbound', value: 'Outbound' },
            { key: 'missed', value: 'Missed' },
            { key: 'voicemail', value: 'Voicemail' },
        ],
        SMS: [
            { key: 'all_sms', value: 'All SMS' },
            { key: 'unread', value: 'Unread' },
            { key: 'inbound', value: 'Inbound' },
            { key: 'outbound', value: 'Outbound' },
        ],
        Meeting: [
            { key: 'all_meetings', value: 'All meetings' },
            { key: 'completed', value: 'Completed' },
            { key: 'scheduled', value: 'Scheduled' },
            { key: 'cancelled', value: 'Cancelled' },
            { key: 'rescheduled', value: 'Rescheduled' },
        ],
        Notes: [
            { key: 'all_notes', value: 'All' },
            { key: 'yesterday', value: 'Yesterday' },
            { key: 'last_week', value: 'Last week' },
            { key: 'last_month', value: 'Last month' },
            { key: 'last_year', value: 'Last year' },
            { key: 'custom_date', value: 'Custom date' },
        ],
    };

    const getSubFilterOptions = (overall: string) => subFilterOptionsByOverall[overall] ?? [];

    const handleOverallFilterChange = (option: any) => {
        const nextOverall = option?.value ?? 'All activity';
        setSelectedOverallFilter(nextOverall);
        handleDropdownChange(option);
        setShowCustomDatePicker(false);

        const subOptions = getSubFilterOptions(nextOverall);
        setSelectedSubFilter(subOptions?.[0]?.value ?? null);
    };

    const handleSubFilterChange = (option: any) => {
        const nextSubFilter = option?.value ?? null;
        setSelectedSubFilter(nextSubFilter);
        if (String(nextSubFilter ?? '').toLowerCase() === 'custom date') {
            setCustomDateDraft({
                start: customDateRange.start,
                end: customDateRange.end,
            });
            setShowCustomDatePicker(true);
        } else {
            setShowCustomDatePicker(false);
        }
    };

    const applyCustomDateRange = () => {
        const appliedStart = customDateDraft.start;
        const appliedEnd = customDateDraft.end ?? customDateDraft.start;
        setCustomDateRange({
            start: appliedStart,
            end: appliedEnd,
        });
        setShowCustomDatePicker(false);
        setHasMore(true);
        setOffset(0);
    };

    const clearCustomDateRange = () => {
        setCustomDateDraft({ start: null, end: null });
        setCustomDateRange({ start: null, end: null });
        setShowCustomDatePicker(false);
        setHasMore(true);
        setOffset(0);
    };

    const handleScroll = useRef(
        createInfiniteScroll({
            lastScrollTopRef: lastScrollTop,
            hasMoreRef,
            loaderRef,
            onLoadMore: () => {
                setOffset((prev) => prev + 1);
            },
        })
    ).current;


    const dropdownOptions: any = [
        { key: 'all_activity', label: 'All activity', value: 'All activity' },
        { key: 'email', label: 'Email', value: 'Email' },
        { key: 'call', label: 'Call', value: 'Call' },
        { key: 'sms', label: 'SMS', value: 'SMS' },
        { key: 'meeting', label: 'Meeting', value: 'Meeting', channelType: 'Video' },
        { key: 'notes', label: 'Notes', value: 'Notes' },
    ];

    const subFilterOptions = useMemo(
        () => getSubFilterOptions(selectedOverallFilter),
        [selectedOverallFilter]
    );

    const filteredActivityData = useMemo(() => {
        if (!selectedSubFilter || selectedOverallFilter === 'All activity') {
            return activityData;
        }

        const matchesSubFilter = (item: any) => {
            const sub = selectedSubFilter?.toLowerCase();
            const isInbound = Number(item?.IsInBound) === 1;
            const isOutbound = Number(item?.IsInBound) === 0;
            const messageText = String(item?.summary || item?.title || '').toLowerCase();
            const tag = String(item?.tags || '').toLowerCase();
            const createdAt = item?.createdAt ? moment(item.createdAt) : null;
            const hasAttachments = Boolean(item?.imageurl || item?.imageUrl || item?.attachmentUrl || item?.Attachments?.length || item?.Attachment?.length);

            if (selectedOverallFilter === 'Email') {
                if (sub === 'all email') return true;
                if (sub === 'unread') return Number(item?.isOpened) === 0;
                if (sub === 'inbound') return isInbound;
                if (sub === 'outbound') return isOutbound;
                if (sub === 'has files') return hasAttachments;
            }

            if (selectedOverallFilter === 'Call') {
                if (sub === 'all calls') return true;
                if (sub === 'inbound') return isInbound;
                if (sub === 'outbound') return isOutbound;
                if (sub === 'missed') return tag.includes('missed') || messageText.includes('missed');
                if (sub === 'voicemail') return tag.includes('voicemail') || messageText.includes('voicemail');
            }

            if (selectedOverallFilter === 'SMS') {
                if (sub === 'all sms') return true;
                if (sub === 'unread') return Number(item?.isOpened) === 0;
                if (sub === 'inbound') return isInbound;
                if (sub === 'outbound') return isOutbound;
            }

            if (selectedOverallFilter === 'Meeting') {
                if (sub === 'all meetings') return true;
                if (sub === 'completed') return tag === 'completed' || tag === 'complete';
                if (sub === 'scheduled' || sub === 'schedule meeting') return tag === 'scheduled';
                if (sub === 'cancelled' || sub === 'canceled') return tag === 'cancelled' || tag === 'canceled';
                if (sub === 'rescheduled') return tag === 'rescheduled';
            }

            if (selectedOverallFilter === 'Notes') {
                if (!createdAt || !createdAt.isValid()) return sub === 'all';
                if (sub === 'all') return true;
                if (sub === 'yesterday') return createdAt.isSame(moment().subtract(1, 'day'), 'day');
                if (sub === 'last week') return createdAt.isAfter(moment().subtract(7, 'days'));
                if (sub === 'last month') return createdAt.isAfter(moment().subtract(1, 'month'));
                if (sub === 'last year') return createdAt.isAfter(moment().subtract(1, 'year'));
                if (sub === 'custom date') {
                    if (!customDateRange.start) return true;
                    const start = moment(customDateRange.start).startOf('day');
                    const end = moment(customDateRange.end ?? customDateRange.start).endOf('day');
                    return createdAt.isBetween(start, end, undefined, '[]');
                }
            }

            return true;
        };

        return (Array.isArray(activityData) ? activityData : [])
            .map((group: any) => ({
                ...group,
                items: (Array.isArray(group?.items) ? group.items : []).filter(matchesSubFilter),
            }))
            .filter((group: any) => (group?.items?.length ?? 0) > 0);
    }, [activityData, selectedOverallFilter, selectedSubFilter, customDateRange.start, customDateRange.end]);
    const campaignMembers = [
        {
            name: 'Michael Anderson',
            role: 'Administrator',
            status: 'Active',
            joined: 'Joined Nov 15, 2023',
            icon: Profileicon
        },
        {
            name: 'Sarah Johnson',
            role: 'Manager',
            status: 'Inactive',
            joined: 'Joined Jan 04, 2024',
            icon: Profileicon
        }
    ];

    const sanitizePhoneNumber = (value: string) => {
        const trimmed = value.trim();
        if (!trimmed) return "";
        const withPlus = trimmed.startsWith("+");
        const digitsOnly = trimmed.replace(/\D/g, "");
        return withPlus ? `+${digitsOnly}` : digitsOnly;
    };

    const isValidPhoneNumber = (value: string) => /^\+?[1-9]\d{6,14}$/.test(value);
    const makecall = async (phoneNumber: any) => {

        if(!`${phoneNumber}`?.trim()) {
            toastnotification.error("No phone number found");
            return;
        }
        const sanitizedPhoneNumber = sanitizePhoneNumber(String(phoneNumber ?? ""));
        if (!isValidPhoneNumber(sanitizedPhoneNumber)) {
            toastnotification.error("Invalid phone number");
            return;
        }
        if (filterQueueList.length === 0) {
            toastnotification.error("Please configure the queue to make a Call");
            return
        }
        dispatch(contactDataSliceActions.setCustomerDealId(Number(contactData?.deal_id)));
        const currentUser = loginDeatils?.find((user: any) => user?.UserID == userDetails?.userId);
        const queueIds = `${currentUser?.queueId ?? ""}`.split(",")?.map((id: string) => id?.trim())?.filter(Boolean);
        const currentQueue = (Array.isArray(filterQueueList) ? filterQueueList : []).find((item: any) => queueIds?.includes(`${item?.qid}`));

        const channelConfiguredCall = Array.isArray(configurationChannel?.call) && configurationChannel?.call?.length > 0
        console.log("channelConfiguredCall", !currentQueue?.outBoundNumber , !channelConfiguredCall)
        
        if(!currentQueue?.outBoundNumber || !channelConfiguredCall) {
            toastnotification.error("Please configure the outbound number to make a Call");
            return
        }

        dispatch(CallPopUpAgentActions.setQueueName(currentQueue ?? {}))

        dispatch(CallPopUpAgentActions.updateTransferType({ transfer_type: "EXTERNAL" }));
        makeCallFunctionForAll(sanitizedPhoneNumber, 2, contactData ?? {});
    }

    const composeMail = () => {
        const channelConfiguredMail = Array.isArray(configurationChannel?.email) && configurationChannel?.email?.length > 0
        console.log("channelConfiguredMail",  !channelConfiguredMail)
        
        if( !channelConfiguredMail) {
            toastnotification.error("Please configure the primary email to make a Email");
            return
        }
        const tabId = "inbox";
        const path = `/inbox?channelType=Email&emailid=${contactData?.primary_email}&CustomerId=${customerId}`;
        const title = "Inbox";
        dispatch(inboxDataSliceActions.setCleanReplyForword());
        
                
        const existing = appTabs?.find((t: any) => t.id === tabId);
        if (!existing) {
            dispatch(commonDataSliceActions.addAppTab({ id: tabId, title, path: "/inbox" }));
        }

        dispatch(commonDataSliceActions.setActiveAppTab(tabId));
        if (contactData?.primary_email){
            if(validateEmail(contactData?.primary_email)) {
                navigate(path);
            } else {
                toastnotification.error("Invalid email address");
            }
        } else {
            toastnotification.error("No primary email found");
        }
    }

    const toggleNote = (event: any) => {
        event.stopPropagation();
        setShowNote((prev: any) => !prev);
    };
    const showTextEditer = () => {
        setShowTextEditer((prev: any) => !prev);
    };
    const handleNoteChange = (value: any) => {
        setNoteText(value?.target?.innerHTML ?? value?.target?.innerText ?? "");
    };

    const saveContactNote = async () => {
        const latestHtml = noteText ?? "";
        const latestIsEmpty = removeSpaceNormalizetext(latestHtml);
        if (latestIsEmpty && !noteImages) return;
        const data = {
            sessionId: "",
            customerId: contactData?.customer_id ?? contactData?.id ?? 0,
            agentId: userDetails?.userId ?? 0,
            channelType: "Note",
            notes: latestHtml,
            imageUrl: noteImages ?? "",
            dealId: Number(contactData?.deal_id) ?? null,
        };
        const response = await insertUpdateNotes(data);
        cancelBtn()
        if (response?.statusCode === 200) {
            setNoteText("");
            setNoteImages("");
            setNoteEditorKey((prev) => prev + 1);
            setHasMore(true);
            if (offset !== 0) {
                setOffset(0);
            } else {
                getSessionHistory(channelType, 0, false);
            }
        }
    };

    // const aiNotes = async () => {
    //     const latestHtml = noteText ?? "";
    //     const latestIsEmpty = removeSpaceNormalizetext(latestHtml);
    //     if (latestIsEmpty) return;
    //     const response: any = await generateAIMessage({ text: latestHtml });
    //     const aiText = response ?? "";
    //     if (!aiText) return;
    //     setNoteText(aiText);
    //     const emailcontent = document.getElementById('emailcontent')
    //     if (emailcontent) {
    //         emailcontent.innerText = aiText
    //     }
    // };

    const aiNotes = async (tone?: string) => {
        const latestHtml = noteText ?? "";
        const latestIsEmpty = removeSpaceNormalizetext(latestHtml);
        if (latestIsEmpty) return;

        const body = {
            text: latestHtml,
            tone: tone
        }
        const response: any = await generateAIMessage(body);

        const aiText = response ?? "";
        if (!aiText) return;
        setNoteText(aiText);

        const emailcontent = document.getElementById('emailcontent');
        if (emailcontent) {
            emailcontent.innerHTML = aiText;
        }
    };

    const imageUpdate = () => {
        noteImageInputRef.current?.click();
    };

    const onNoteImageSelect = async (event: any) => {
        const file = event.target.files?.[0];
        if (!file) return;
        const imageData = await handleFileUpload(file);
        setNoteImages(imageData?.[0]?.url ?? "");
        event.target.value = "";
    };

    const cancelBtn = () => {
        setNoteText("");
        setNoteImages("");
        setShowNote(false)
    }

    const tagStyles = {
        cancelled : styles.cancelled, 
        missed :styles.missed,
        scheduled : styles.meeting, 
        rescheduled : styles.remeeting,
    }

    const openMeet = (item :any)=> {
        window.open(item.meet_url, '_blank')
        updateNLPJoinMeet({
            meetingId: item?.meetingId ?? '',
            domainId: `${item?.domainId ?? ''}`,
            sessionId: item?.sessionId ?? '',
            cusid: `${item?.CustomerId ?? ''}`,
        }) 
    }


    const handlechange = (value: any) => {
        if (!value) return;
        aiNotes(value?.label);
    };

    const getData = (data: any) => {
        if (data?.channelType === 'Video') {
            // getPerticularInboxSessionDetails({
            //     channelType : 'Video', 
            //     sessionId : data?.sessionId, 
            //     historyId : data?.VideoHistoryId
            // }, dispatch)
            if (['rescheduled', 'scheduled'].includes(data?.tags)) {
                navigate(`/contact/scheduledmeeting?channelType=${data?.channelType}&sessionId=${data?.sessionId}&VideoHistoryId=${data?.VideoHistoryId}&coid=${customerId}`)
            } else {
                navigate(`/contact/videoviewdetails?channelType=${data?.channelType}&sessionId=${data?.sessionId}&VideoHistoryId=${data?.VideoHistoryId}&coid=${customerId}`)
            }
        } else if (data?.channelType === "Call") {
            dispatch(inboxDataSliceActions.setInboundCallViewData({
                Message: Array.isArray(data?.Message) ? data?.Message : [],
                RecordingUrl: data?.RecordingUrl ?? data?.recordingUrl ?? "",
                summary: data?.summary ?? "",
                customerNumber: data?.customerNumber ?? "",
                sessionId: data?.sessionId ?? "",
                sessionStartTime: data?.sessionStartTime ?? "",
                sessionEndTime: data?.sessionEndTime ?? "",
                createdAt: data?.createdAt ?? "",
                customerName: fullName,
            }));
            navigate(`/contact/companyinboundcallview?channelType=${data?.channelType ?? ''}&sessionId=${data?.sessionId ?? ''}&coid=${customerId ?? ''}`)
        } else if (data?.channelType === "Email") {
            dispatch(contactDataSliceActions.setCustomerEmail({...data, contactData}));
            navigate(`/contact/contactemail?coid=${customerId}`);
        }

        //  else if (data?.channelType === "Notes") {
        //     dispatch(contactDataSliceActions.setCustomerNotes(data));
        //     navigate(`/contact/contactnotes?channelType=${data?.channelType ?? ''}&sessionId=${data?.sessionId ?? ''}&coid=${customerId ?? ''}`)
        // }
    }

    const handleNewConversation = (type: string) => {

        const channelConfiguredSMS = Array.isArray(configurationChannel?.sms) && configurationChannel?.sms?.length > 0;
        if( !channelConfiguredSMS && type === 'SMS') {
            toastnotification.error("Please configure the outbound number to send an SMS");
            return
        }
        const sessionId: any = uuidv4() + 'contact';
        let body: any = {
            query: type === 'SMS' ? 'Need SMS' : 'Need Meeting',
            sessionId: sessionId,
            roleId: LoginCcaasUserDetails?.roleId?.toString() ?? "",
            org_role_id_crm: LoginCcaasUserDetails?.org_role_id_crm?.toString() ?? "",
            userId: `${LoginCcaasUserDetails?.userId || ''}`,
            domainId: `${LoginCcaasUserDetails?.domainId || ''}`,
            module: "",
            ext: `${LoginCcaasUserDetails?.ext || ''}`,
            agent: 'contact',
            isBot: 0,
            customerId: `${customerId || ''}`,
            uploadUrl: [],
        }

        aiChatAssistSocketservice.io.emit('crm_chat', body);

        // dispatch(contactDataSliceActions.setcontactAiAgentChat({ ...body }))
        dispatch(contactDataSliceActions.setcontactAiAgentChatTyping({ sessionId: sessionId, typing: true }));

        const path = `/contact/chat-conversation?sessionId=${sessionId}`
        dispatch(commonDataSliceActions.setTabMenuPush({ path: path, menuName: "New Conversation", close: 1, type: "contact" }));
        navigate(path);
    }
const ref = useRef(null);
  const [bounds, setBounds] = useState({});

  useEffect(() => {
    const updateBounds = () => {
      if (!ref.current) return;

      const rect = ref.current.getBoundingClientRect();

      setBounds({
        left: -rect.left,
        top: -rect.top,
        right: window.innerWidth - rect.right,
        bottom: window.innerHeight - rect.bottom,
      });
    };

    updateBounds();
    window.addEventListener("resize", updateBounds);

    return () => window.removeEventListener("resize", updateBounds);
  }, []);

  const [productId,setProductId]=useState<any>(null);

    return (
        <>
        {customer_deal_id
            ? <div><Deals /></div>
            : (
            <div className={` commoncompanyheader ${styles.companynameheader} ${activeTab === "activity" ? styles.companylisthidden : ''}`}>
                <div className={`${styles.filtertoplist} filtertoplist`}>
                    <div className={`${styles.companylisttopbar} `}>
                        <div className={styles.companyleft}>
                            <div className={styles.contactblock}>
                                <div className={styles.contactprofile}>{initials}</div>
                                <div className={styles.customername}>
                                    <div className={styles.customernametitle}>
                                        {fullName}
                                    {!!(contactData?.lead_status) && (
                                        <span>{contactData?.lead_status}</span>
                                    )}
                                    </div>
                                    <div className={styles.customermail}>{contactData?.primary_email}</div>
                                </div>
                            </div>
                        </div>
                        <div className={styles.companyright}>

                            <span onClick={() => makecall(selectedPhoneNumber)}>
                                <img src={Callicon} alt="" />
                            </span>
                            <span onClick={composeMail}><img src={Emaillicon} alt="" /></span>
                            <span onClick={()=> handleNewConversation('SMS')}><img src={SmsIcon} alt="" /></span>
                            <span onClick={toggleNote} style={{ position: "relative" }}>
                                <img src={Notesicon} alt="" />
                             
                                {showNotes &&
                                    <div className="wrapper">
                                        <Draggable handle=".drag-handle">
                                        <div className={styles.notesdropdown} onClick={(event) => event.stopPropagation()}>
                                            <div className={`${styles.header} drag-handle`}>
                                                <div> Notes</div>
                                                <div className={styles.cancelbtn} onClick={cancelBtn}>
                                                    <svg xmlns="http://www.w3.org/2000/svg" width="15" height="15" viewBox="0 0 15 15" fill="none">
                                                        <path d="M8.5064 7.3057L14.3614 1.4502C14.6934 1.1191 14.6934 0.5801 14.3614 0.249C14.0294 -0.083 13.4903 -0.083 13.1602 0.249L7.3052 6.1044L1.4502 0.249C1.1201 -0.083 0.5791 -0.083 0.249 0.249C-0.083 0.5801 -0.083 1.1191 0.249 1.4502L6.104 7.3057L0.249 13.1612C-0.083 13.4923 -0.083 14.0313 0.249 14.3624C0.415 14.5284 0.6318 14.6114 0.8496 14.6114C1.0674 14.6114 1.2851 14.5284 1.4502 14.3624L7.3052 8.507L13.1602 14.3624C13.3252 14.5284 13.543 14.6114 13.7608 14.6114C13.9786 14.6114 14.1954 14.5284 14.3614 14.3624C14.6934 14.0313 14.6934 13.4923 14.3614 13.1612L8.5064 7.3057Z" fill="#404040" />
                                                    </svg>
                                                </div>
                                            </div>
                                            <div className={styles.body}>
                                                <div className={styles.inner}>
                                                    <TextEditor key={noteEditorKey} openTools={sethowTextEditer} onchange={handleNoteChange} onlytext={true} bottom={true} />
                                                    <input
                                                        ref={noteImageInputRef}
                                                        type="file"
                                                        accept=".csv, .xlsx, image/*"
                                                        style={{ display: "none" }}
                                                        onChange={onNoteImageSelect}
                                                    />
                                                    {noteImages && <div className={styles.imgpreview}>
                                                        <img src={noteImages} />
                                                    </div>}
                                                </div>
                                            </div>
                                            <div className={styles.footer}>
                                                <div className={styles.lefts}>
                                                    <span onClick={showTextEditer} style={{ cursor: "pointer" }}><svg xmlns="http://www.w3.org/2000/svg" width="19" height="19" viewBox="0 0 19 19" fill="none">
                                                        <path d="M16.0805 7.3334C16.3224 7.3334 16.5482 7.429 16.7165 7.6026C17.0498 7.9464 17.0412 8.4973 16.6973 8.8306L8.7767 16.5079L7.5367 16.4886L7.556 15.2486L15.4645 7.583C15.6306 7.422 15.8493 7.3334 16.0805 7.3334ZM16.0805 5.8334C15.4825 5.8334 14.8839 6.0568 14.4206 6.5059L6.3497 14.3288C6.168 14.5049 6.0637 14.746 6.0597 14.999L6.0283 17.0108C6.0201 17.5377 6.4406 17.9715 6.9674 17.9797L8.9792 18.0111C8.9842 18.0111 8.9891 18.0112 8.9941 18.0112C9.2418 18.0112 9.48 17.9149 9.6582 17.7422L17.7412 9.9075C18.6804 8.9971 18.7039 7.4977 17.7935 6.5584C17.3257 6.0758 16.7034 5.8334 16.0805 5.8334Z" fill="#23232B" />
                                                        <path d="M10.6778 0H0.9004C0.4033 0 0 0.4033 0 0.9004C0 1.3975 0.4033 1.8008 0.9004 1.8008H4.8887V10.6778C4.8887 11.1749 5.292 11.5782 5.7891 11.5782C6.2862 11.5782 6.6895 11.1749 6.6895 10.6778V1.8008H10.6778C11.1749 1.8008 11.5782 1.3975 11.5782 0.9004C11.5782 0.4033 11.1749 0 10.6778 0Z" fill="#23232B" />
                                                    </svg>
                                                    </span>
                                                    <span onClick={imageUpdate} style={{ cursor: "pointer" }}><svg xmlns="http://www.w3.org/2000/svg" width="20" height="18" viewBox="0 0 20 18" fill="none">
                                                        <path d="M4.7147 9.3814C5.3626 9.3814 5.888 8.856 5.888 8.2081C5.888 7.5602 5.3627 7.0348 4.7147 7.0348C4.0667 7.0348 3.5415 7.5602 3.5415 8.2081C3.5415 8.856 4.0668 9.3814 4.7147 9.3814Z" fill="#23232B" />
                                                        <path d="M4.1195 14.8577H13.9577C14.3727 14.8577 14.6071 14.3814 14.3537 14.0525L10.8821 9.5493C10.682 9.2895 10.2903 9.2895 10.0901 9.5493L7.0187 13.5334L5.9671 12.1694C5.7669 11.9096 5.3752 11.9096 5.1751 12.1694L3.7234 14.0525C3.47 14.3814 3.7045 14.8577 4.1195 14.8577Z" fill="#23232B" />
                                                        <path d="M16.7 15.9117C16.7 16.2977 16.386 16.6117 16 16.6117H2C1.614 16.6117 1.3 16.2977 1.3 15.9117V5.981C1.3 5.595 1.614 5.281 2 5.281H11.7361C11.5966 4.8684 11.5087 4.4351 11.4881 3.981H2C0.8954 3.981 0 4.8763 0 5.981V15.9117C0 17.0162 0.8954 17.9117 2 17.9117H16C17.1046 17.9117 18 17.0162 18 15.9117V8.1851C17.5925 8.3487 17.1537 8.441 16.7 8.4859V15.9117Z" fill="#23232B" />
                                                        <path d="M16.2332 0C14.1529 0 12.4664 1.6863 12.4664 3.7666C12.4664 5.8469 14.1529 7.5334 16.2332 7.5334C18.3135 7.5334 20 5.8469 20 3.7666C20 1.6863 18.3135 0 16.2332 0ZM17.959 4.167H16.6333V5.4922C16.6333 5.7129 16.4541 5.8926 16.2334 5.8926C16.0127 5.8926 15.8335 5.7129 15.8335 5.4922V4.167H14.5073C14.2866 4.167 14.1074 3.9873 14.1074 3.7666C14.1074 3.5459 14.2866 3.3662 14.5073 3.3662H15.8335V2.041C15.8335 1.8203 16.0127 1.6406 16.2334 1.6406C16.4541 1.6406 16.6333 1.8203 16.6333 2.041V3.3662H17.959C18.1797 3.3662 18.3589 3.5459 18.3589 3.7666C18.3589 3.9873 18.1797 4.167 17.959 4.167Z" fill="#23232B" />
                                                    </svg>
                                                    </span>
                                                    <div className='activitydropdown aiactive'>
                                                    <Worktualdropdown  options={aiOptions} onChange={handlechange} triggerIcon={AIIcon}/>
                                                    </div>
                                                    {/* <span onClick={aiNotes} style={{ cursor: "pointer" }}>
                                                        <img src={AIIcon} alt="" />
                                                    </span> */}
                                                </div>
                                                <div className={styles.rights}>
                                                    <button onClick={saveContactNote}>Save</button>
                                                </div>
                                            </div>
                                        </div>
                                        </Draggable>
                                    </div>
                                }
                            </span>
                            <span onClick={()=> handleNewConversation('meeting')}><img src={Calendaricon} alt="" /></span>
                           
                            {/* <span><img src={Calendaricon} alt="" style={{ cursor: "pointer" }} /></span> */}
                        </div>
                    </div>
                    <div className={styles.filters}>
                        <span
                            className={`${styles.filterchip} ${activeTab === "customer" ? styles.active : ""
                                }`}
                            onClick={() => setActiveTab("customer")}
                        >
                            The Customer
                        </span>

                        <span
                            className={`${styles.filterchip} ${activeTab === "activity" ? styles.active : ""
                                }`}
                            onClick={() => setActiveTab("activity")}
                        >
                            Activity
                        </span>

                        <span
                            className={`${styles.filterchip} ${activeTab === "deals" ? styles.active : ""
                                }`}
                            onClick={() => setActiveTab("deals")}
                        >
                            Deals
                        </span>

                        {levelOfAccess(MENUS.PRODUCTS, ACTIONS_ACCESS.VIEW, (permissions ?? [])) && <span
                            className={`${styles.filterchip} ${activeTab === "products" ? styles.active : ""
                                }`}
                            onClick={() => setActiveTab("products")}
                        >
                            Products
                        </span>}

                        {levelOfAccess(MENUS.PROJECTS, ACTIONS_ACCESS.VIEW, (permissions ?? [])) &&  <span
                            className={`${styles.filterchip} ${activeTab === "projects" ? styles.active : ""
                                }`}
                            onClick={() => setActiveTab("projects")}
                        >
                            Projects
                        </span>}

                        <span
                            className={`${styles.filterchip} ${activeTab === "whatscoming" ? styles.active : ""
                                }`}
                            onClick={() => setActiveTab("whatscoming")}
                        >
                            What’s upcoming
                        </span>

                    </div>
                </div>
                <div className={`${styles.tabContent} ${activeTab === "activity" ? styles.tablistactive : ''}`}>
                    {activeTab === "customer" &&
                        <div className={styles.customermaincontainer}>
                            <div className={styles.personlistWrapper}>
                                {persons?.map((person, index) => {
                                    console.log("person?.nameperson?.name", person?.name)
                                    const values = typeof person?.name === 'string' ? (person?.name || "")?.split("\n")?.filter(Boolean) : [];


                                    return (
                                        <div key={index} className={styles.personlist}>
                                            <div className={styles.customerlabel}>
                                                {person?.label}
                                            </div>
                                            <div className={styles.customerlabelname}>
                                                {values?.length > 0 ? (
                                                    values?.map((val: any, i: any) => {
                                                        const isPrimary = val?.includes("(Primary)");
                                                        const cleanValue = val?.replace(" (Primary)", "");
                                                        return (
                                                            <div key={i}>
                                                                {cleanValue} {isPrimary && <span className={styles.badge}>Primary</span>}
                                                            </div>
                                                        );
                                                    })
                                                ) : (
                                                    "-"
                                                )}
                                            </div>
                                        </div>
                                    );
                                })}
                            </div>
                            <div className={styles.leadscorecontainer}>
                                <div className={styles.leadtitle}>Lead Score {!!contactData?.lead_status &&<span>{contactData?.lead_status}</span>}</div>
                                <div className={styles.toptool}>
                                    <SentimentScore value={LeadScore} />
                                    <div className={styles.totalscore}>Total Score </div>
                                </div>
                                <div className={styles.leadtotaltitle}>{leadSummary}</div>
                            </div>
                        </div>
                    }
                    {activeTab === "activity" && (
                        <div className={`${styles.customermaincontainer} ${styles.contactprojectlist}`} onScroll={handleScroll}>
                            <div className={styles.customeractive}>
                                <div className={styles.activitymainlog}>
                                    <div className={styles.allemail}>All activity</div>
                                    <div className={`${styles.emaildropdown} activitydropdown activitylist`}>
                                        {selectedOverallFilter !== 'All activity' && subFilterOptions?.length > 0 && (
                                            <div className={styles.customDateFilterWrap} ref={customDateRef}>
                                                <CustomSingleSelectDropdown
                                                    options={subFilterOptions}
                                                    value={selectedSubFilter}
                                                    onChange={handleSubFilterChange}
                                                />
                                                {selectedOverallFilter === 'Notes' && selectedSubFilter === 'Custom date' && showCustomDatePicker && (
                                                    <div className={styles.customDatePopover}>
                                                        <DateRangeCalendar
                                                            onChange={(start, end) => setCustomDateDraft({ start, end })}
                                                            initialStart={customDateDraft.start}
                                                            initialEnd={customDateDraft.end}
                                                        />
                                                        <div className={styles.customDateActions}>
                                                            <button type="button" onClick={clearCustomDateRange}>Clear</button>
                                                            <button type="button" onClick={applyCustomDateRange}>Apply</button>
                                                        </div>
                                                    </div>
                                                )}
                                            </div>
                                        )}
                                        {/* <Worktualdropdown
                                            options={dropdownOptions}
                                            onChange={handleDropdownChange}
                                        /> */}
                                        <CustomSingleSelectDropdown
                                            options={dropdownOptions}
                                            value={selectedOverallFilter}
                                            onChange={handleOverallFilterChange}
                                        />
                                    </div>
                                </div>
                                {filteredActivityData?.length > 0 ? (
                                    <div className={styles.activityListScroll}>
                                        {filteredActivityData?.map((group: any) => {
                                            return (
                                                <div className={styles.allactivitycontainer} key={group?.label}>
                                                    <div className={styles.allactivitytitle}>{group?.label}</div>

                                                    {group?.items.map((item: any) => (
                                                        <div className={styles.activitymain} key={item?.id} onClick={() => getData(item)}>
                                                            <img src={item?.icon} alt={item?.type} />

                                                            <div className={styles.activitycontainer}>
                                                                <div className={styles.direct}>
                                                                    <div className={styles.directleft}>
                                                                        {item.channelType === 'Video' ? item.meeting_title : item?.type}
                                                                        {!!item.tags && <span className={`${tagStyles[item.tags]}`}>{item.tags}</span>}
                                                                    </div>
                                                                    <div className={styles.directright}>{item?.date}</div>
                                                                </div>

                                                                <div
                                                                    className={styles.plantitile}
                                                                    dangerouslySetInnerHTML={{ __html: item?.title }}
                                                                />
                                                                {!!item?.imageurl && (
                                                                    <div className={styles.imgcontainerr}>

                                                                        <img
                                                                            src={item?.imageurl}
                                                                            alt={'img'}
                                                                            height={"60"}
                                                                        /></div>
                                                                )}

                                                                <div className={styles.direct}>
                                                                    <div className={styles.plantitilesub}>
                                                                        From : <span>{item?.from}</span>
                                                                    </div>

                                                                    {!!item.meet_url && (
                                                                        <div className={styles.joinmeet} onClick={(event) => { event.stopPropagation(); openMeet(item); }}>
                                                                            Join Meet
                                                                        </div>)}
                                                                </div>
                                                                {/* <WorktualVisualization src={''} /> */}
                                                            </div>
                                                        </div>
                                                    ))}
                                                </div>
                                            )
                                        })}
                                        {loader && (
                                            <div className={styles.activitySkeleton}>
                                                {Array.from({ length: 3 }).map((_, index) => (
                                                    <SkeletonBox key={`activity-skeleton-more-${index}`} width="100%" height="60px" />
                                                ))}
                                            </div>
                                        )}
                                    </div>
                                ) : (
                                    loader ? (
                                        <div className={styles.activitySkeleton}>
                                            <SkeletonBox width="30%" height="20px" />
                                            {Array.from({ length: 6 }).map((_, index) => (
                                                <SkeletonBox key={`activity-skeleton-${index}`} width="100%" height="60px" />
                                            ))}
                                        </div>
                                    ) : (
                                        <div className={styles.datacenter}>
                                           <Nodata />
                                        </div>
                                    )
                                )}
                            </div>
                        </div>
                       
                      )
                    }
                    {activeTab === "deals" && <div><Dealscard /></div>}
                    {activeTab === "products" && <div>
                                                {!productId?
                                                    // <Catalog contact={true} showProductName={(pid)=>setProductId(pid)} />
                                                    // :<Productname pID={productId} showProductName={(pid)=>setProductId(pid)}/>
                                                    <div > <Nodata /></div>:<></>
                                                }
                                                </div>}
                    {activeTab === "projects" && <div><Contactproject /> </div>}
                    {activeTab === "whatscoming" && <div><Whatsupcoming /></div>}
                </div>
            </div>
        )}
        </>
    )
}
export default Contactname;
