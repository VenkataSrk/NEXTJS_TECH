"use-client"
import { MouseEvent as ReactMouseEvent, useEffect, useRef, useState } from 'react';
import styles from './scss/dealscontent.module.scss'
import Callicon from '../../assets/images/contacts/contactcall.svg'
import Emaillicon from '../../assets/images/contacts/contactmail.svg'
import Notesicon from '../../assets/images/contacts/notes.svg'
import Calendaricon from '../../assets/images/contacts/calendar.svg'
import Email from '../../assets/images/contacts/activityemail.svg'
import Calliconlist from '../../assets/images/contacts/callflowicon.svg'
import Profileicon from '../../assets/images/contacts/profileavatar.svg'
import Activityicon from '../../assets/images/contacts/activityicon.svg'
import Notesactivity from '../../assets/images/contacts/notesactivity.svg'
import Meetinglist from '../../assets/images/contacts/meetingeditor.svg'
import proposalIcon from '../../assets/images/proposalIcon.svg'
import quotationIcon from '../../assets/images/quotation.svg'
import paymentIcon from '../../assets/images/payment.svg'
import Worktualdropdown from '../component/worktualdropdown';
import SmsIcon from '../../assets/images/contacts/SmsIcon.svg'
import Deals from './deals';
import { createInfiniteScroll, formatDate, getCurrencyByDomain, getScheduleTime, isJSON, validateEmail } from '../../base/utils';
import { useNavigate, useParams, useSearchParams } from 'react-router-dom';
import { useDispatcher } from '../../store/redux-store/dispatch';
import { CallService } from '../../base/service/call_service';
import { CallPopUpAgentActions } from '../../store/slice/callSlice/call_popup_agent';
import { useSelector } from 'react-redux';
import { getContactDetails, getCustomerDealsApi, getDealActivityHistoryDetails, handleFileUpload, insertUpdateNotes } from '../../base/service/core_serviceApi';
import { GetItemFromStorage } from '../../base/custom_hooks/useStorage';
import { LocalStorage } from '../../base/custom_hooks/localStorageKeys';
import moment from 'moment';
import WorktualVisualization from '../component/audioplayer/audioplayer';
import preview from "../../assets/images/Attachment.png"
import TextEditor from '../inbox/texteditor';
import { generateAIMessage, updateNLPJoinMeet } from '../../base/service/nlp_service';
import { commonDataSliceActions } from '../../store/slice/commonSilce/commonSlice';
import { inboxDataSliceActions } from '../../store/slice/inbox/inboxSlice';
import Whatsupcoming from '../contacts/whatupcoming';
import Contactproject from '../contacts/contactproject';
import Activitytimeline from './activitytimeline';
import CustomSingleSelectDropdown from '../component/CustomSingleSelect';
import AIIcon from '../../assets/images/aiicon.svg'
import Googlemeet from '../../assets/images/deals/calendarlist.svg'
import Calendarlist from '../../assets/images/deals/calendarlist.svg'
import Locationlist from '../../assets/images/deals/locationlist.svg'
import Meetinglisticon from '../../assets/images/deals/meetinglist.svg'
import Clockicon from '../../assets/images/deals/clockicon.svg'
import Nodata from '../component/nodata';
import SkeletonBox from '../marketing/skeletonbox';
import { contactDataSliceActions } from '../../store/slice/contactSlice';
import jwtDecode from 'jwt-decode';
import { v4 as uuidv4 } from "uuid";
import aiChatAssistSocketservice from '../../services/aiChatService';
import { dealsDataSliceActions } from '../../store/slice/dealsSlice';
import Draggable from "react-draggable";
import { toastnotification } from '../../base/shared/toastmsg';
// import Videomeet from '../../assets/images/contacts/videomeet.svg'
// import Copied from '../../assets/images/contacts/copiedicon.svg'
// import Cardicon from '../../assets/images/contacts/cardicon.svg'
// import Calendar from '../../assets/images/contacts/calendarview.svg'
// import Location from '../../assets/images/projectinsights/location.svg'
// import Clockupcoming from '../../assets/images/projectinsights/clockcoming.svg'
// import Startmeet from '../../assets/images/deals/startmeeting.svg'

type ContactnameProps = {
    contactData?: any;
};

const aiOptions = [
    {
        label: 'Formal', value: <div className={styles.formalist}><svg xmlns="http://www.w3.org/2000/svg" width="9" height="19" viewBox="0 0 9 19" fill="none">
            <path d="M5.21414 4.1518H3.03084C2.93684 4.1518 2.85584 4.2067 2.83674 4.2835L0.0174414 15.5523C-0.0467586 15.8089 0.0692413 16.0736 0.316541 16.2352L3.73594 18.4696C3.84944 18.5437 3.98574 18.5807 4.12194 18.5807C4.25864 18.5807 4.39534 18.5435 4.50894 18.4691L8.01434 16.1717C8.20134 16.049 8.28924 15.8483 8.24064 15.6539L5.40834 4.2837C5.38934 4.207 5.30814 4.1518 5.21414 4.1518ZM4.12134 16.9295L1.62674 15.2995L4.04054 5.6518H4.20314L6.60674 15.3006L4.12134 16.9295Z" fill="#404040" />
            <path d="M2.31844 3.434C2.36114 3.5728 2.51084 3.6698 2.68244 3.6698H5.57114C5.74274 3.6698 5.89264 3.5728 5.93544 3.434L6.87074 0.3918C6.93174 0.1933 6.75234 0 6.50674 0H1.74724C1.50164 0 1.32194 0.1933 1.38294 0.3918L2.31844 3.434ZM5.23154 1.2999L4.90244 2.3698H3.35114L3.02224 1.2999H5.23154Z" fill="#404040" />
        </svg> Formal</div>
    },
    {
        label: 'Friendly', value: <div className={styles.formalist}><svg xmlns="http://www.w3.org/2000/svg" width="19" height="13" viewBox="0 0 19 13" fill="none">
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
        label: 'Elaborate', value: <div className={styles.formalist}><svg xmlns="http://www.w3.org/2000/svg" width="17" height="17" viewBox="0 0 17 17" fill="none">
            <path d="M2.5606 1.5H5.1895C5.6036 1.5 5.9395 1.1641 5.9395 0.75C5.9395 0.3359 5.6036 0 5.1895 0H2.0069C0.9005 0 0.000100136 0.9004 0.000100136 2.0068V5.1904C0.000100136 5.6045 0.336 5.9404 0.7501 5.9404C1.1642 5.9404 1.5001 5.6045 1.5001 5.1904V2.5605L5.5812 6.6416C5.7277 6.7881 5.9191 6.8613 6.1115 6.8613C6.3039 6.8613 6.4953 6.7881 6.6418 6.6416C6.9348 6.3486 6.9348 5.874 6.6418 5.5811L2.5606 1.5Z" fill="#404040" />
            <path d="M5.5811 9.6749L1.5 13.756V11.1271C1.5 10.713 1.1641 10.3771 0.75 10.3771C0.3359 10.3771 0 10.713 0 11.1271V14.3097C0 15.4161 0.9004 16.3165 2.0068 16.3165H5.1894C5.6035 16.3165 5.9394 15.9806 5.9394 15.5665C5.9394 15.1524 5.6035 14.8165 5.1894 14.8165H2.5605L6.6416 10.7354C6.9346 10.4424 6.9346 9.9678 6.6416 9.6749C6.3486 9.382 5.874 9.3819 5.5811 9.6749Z" fill="#404040" />
            <path d="M14.2296 0H11.046C10.6319 0 10.296 0.3359 10.296 0.75C10.296 1.1641 10.6319 1.5 11.046 1.5H13.6759L9.5948 5.5811C9.3018 5.8741 9.3018 6.3487 9.5948 6.6416C9.7413 6.7881 9.9327 6.8613 10.1251 6.8613C10.3175 6.8613 10.5089 6.7881 10.6554 6.6416L14.7365 2.5605V5.1904C14.7365 5.6045 15.0724 5.9404 15.4865 5.9404C15.9006 5.9404 16.2365 5.6045 16.2365 5.1904V2.0068C16.2365 0.9004 15.336 0 14.2296 0Z" fill="#404040" />
            <path d="M15.4864 10.377C15.0723 10.377 14.7364 10.7129 14.7364 11.127V13.7559L10.6553 9.6748C10.3623 9.3818 9.8877 9.3818 9.5948 9.6748C9.3019 9.9678 9.3018 10.4424 9.5948 10.7353L13.6759 14.8164H11.046C10.6319 14.8164 10.296 15.1523 10.296 15.5664C10.296 15.9805 10.6319 16.3164 11.046 16.3164H14.2296C15.336 16.3164 16.2364 15.416 16.2364 14.3096V11.127C16.2364 10.7129 15.9005 10.377 15.4864 10.377Z" fill="#404040" />
        </svg> Elaborate</div>
    },
];

function SentimentScore({ value = 80 }: { value?: number }) {

    const safeValue = Number.isFinite(value) ? Number(value) : 0;
    const normalizedValue = Math.max(0, Math.min(100, safeValue));
    const scoreColor =
        normalizedValue >= 70 ? "#00b87c" : normalizedValue >= 40 ? "#f5b400" : "#e5484d";

    const size = 60;
    const center = size / 2;
    const radius = 24;
    const strokeWidth = 6;

    const circumference = 2 * Math.PI * radius;
    const dashOffset = circumference * (1 - normalizedValue / 100);

    return (
        <svg width={size} height={size} viewBox={`0 0 ${size} ${size}`}>
            {/* Background */}
            <circle
                cx={center}
                cy={center}
                r={radius}
                stroke="#d9d9d9"
                strokeWidth={strokeWidth}
                fill="none"
            />

            {/* Progress */}
            <circle
                cx={center}
                cy={center}
                r={radius}
                stroke={scoreColor}
                strokeWidth={strokeWidth}
                fill="none"
                strokeDasharray={circumference}
                strokeDashoffset={dashOffset}
                strokeLinecap="round"
                transform={`rotate(-90 ${center} ${center})`}
            />

            {/* Value */}
            <text
                x="50%"
                y="50%"
                textAnchor="middle"
                dy=".35em"
                fontSize="18"
                fontWeight="600"
                fill="#222"
            >
                {value}
            </text>
        </svg>
    );
}
const Dealscontent = () => {
    const [activeTab, setActiveTab] = useState("overview");
    const { filterQueueList, appTabs} = useSelector((store: any) => store.commonData)
    const navigate = useNavigate();
    const dispatch = useDispatcher();
    const { makeCallFunctionForAll } = CallService(dispatch, navigate);
    const [contactData, setSelectedContact] = useState<any>({});
    const [selectedDeal, setSelectedDeal] = useState<any>({});
    const [params, setParams] = useSearchParams()
    const customerId = params.get('coid')
    const dealId = params.get('dealsId')
    const activityTab = params.get("tab")
    const token = GetItemFromStorage(LocalStorage.ACCESS_TOKEN);
    const LoginCcaasUserDetails: any = token ? jwtDecode(token) : {};
    const currency = getCurrencyByDomain()
    const loginDeatils: any = GetItemFromStorage(LocalStorage.LOGIN_USER_CRM_DETAILS);

    useEffect(() => {
        getContantacts();
        getDealDetails();
    }, [customerId, dealId])

    const getContantacts = async () => {
        const res = await getContactDetails(customerId, null);
        const record = Array.isArray(res) ? (res[0] ?? {}) : (res ?? {});
        setSelectedContact(record);
        const fullName = `${record?.first_name || ""} ${record?.last_name || ""}`.trim() || "New Customer";

        dispatch(commonDataSliceActions.setTabMenuPush({
            path: '/contact/contactdetais?coid=' + customerId,
            menuName: fullName,
            close: 1,
            type: "contact"
        }));

    }

    const getDealDetails = async () => {
        if (!customerId) {
            setSelectedDeal({});
            return;
        }
        try {
            const body = {
                customerId: customerId ?? null,
            };
            const customerDeals = await getCustomerDealsApi(body);
            const deals = customerDeals?.customerDealsData || [];
            const matchedDeal = dealId
                ? deals?.find((deal: any) => String(deal?.deal_id) === String(dealId))
                : deals?.[0];
            const dealRecord = matchedDeal || {};
            setSelectedDeal(dealRecord);
            // const dealPath = dealId
            //     ? `/deals/dealscontent?coid=${customerId}&dealsId=${dealId}`
            //     : `/deals/dealscontent?coid=${customerId}`;

            // dispatch(commonDataSliceActions.setTabMenuPush({
            //     path: dealPath,
            //     menuName: dealRecord?.deal_name || "Deal Name",
            //     close: 1,
            //     type: "deals"
            // }));
        } catch (error) {
            console.log("getDealDetails Error", error);
            setSelectedDeal({});
        }
    };

    const switchDealTab = (tab: any) => {
        setActiveTab(tab);
    };

    const fullName = `${contactData?.first_name || ""} ${contactData?.last_name || ""}`.trim() || "New Customer";
    const initials = `${contactData?.first_name?.charAt(0) || ""}${contactData?.last_name?.charAt(0) || ""}`.toUpperCase() || "N";
    const emailValue = [contactData?.primary_email ? `${contactData.primary_email} (Primary)` : null, contactData?.secondary_email]
        ?.filter(Boolean)
        ?.join("\n") || "-";
    const phoneValue = [contactData?.primary_phone_number ? `${contactData.primary_phone_number} (Primary)` : null, contactData?.secondary_phone_number]
        ?.filter(Boolean)
        ?.join("\n") || "-";
    const selectedPhoneNumber = contactData?.primary_phone_number || contactData?.secondary_phone_number || "";
    const behaviorScore = Number(contactData?.behaviors ?? 0);
    const demoScore = Number(contactData?.demo_graphics ?? 0);
    const firmoScore = Number(contactData?.firmo_graphics ?? 0);
    const LeadScore = Math.round((((behaviorScore + demoScore + firmoScore) / 300) * 100));
    const persons = [
        { label: "Contact Name", name: fullName },
        { label: "Owner Name", name: contactData?.user_id?.toString() ?? "-" },
        { label: "Email", name: emailValue },
        { label: "Phone", name: phoneValue },
        { label: "Address", name: contactData?.address ?? "-" },
        { label: "Industry", name: contactData?.industry_type ?? "-" },
        { label: "Source", name: contactData?.source_type ?? "-" },
        { label: "Company Size", name: contactData?.company_size ?? "-" },
        { label: "Created Date", name: contactData?.created_date ? formatDate(contactData.created_date) : "-" },
        { label: "Company Name", name: contactData?.company_name ?? "-" },
    ];
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
    const [selected, setSelected] = useState('All activity');
    const { configurationChannel } = useSelector((store :any)=>store?.settingsStore)

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
    }, [channelType]);

    useEffect(() => {
        if (contactData?.primary_email || contactData?.primary_phone_number) {
            getSessionHistory(channelType, offset, offset !== 0);
        }
    }, [offset, channelType, dealId, contactData]);

    const getChannelType = (row: any) => {
        if(row?.channelType === 'Invoice') {
            return row?.invoice?.invoice_number ? `Payment ID ${row?.invoice?.invoice_number}` : 'Payment';
        } else if(row?.channelType !== 'Call') {
            return row?.channelType;
        } else if(row?.channelType === 'Call' && row?.IsInBound == 0) {
            return 'Outbound Call';
        } else {
            return 'Inbound Call';
        }
    }

    const getSessionHistory = async (channelTypeParam: any = null, pageOffset: number = 0, append = false) => {
        try {
            setLoader(true);
            const data = {
                domainId: LoginUserDetails?.domainId,
                channelType: channelTypeParam,
                agentId: LoginUserDetails?.userId,
                customerNumber: contactData?.primary_phone_number ?? null,
                customerEmail: contactData?.primary_email ?? null,
                offset: pageOffset,
                limit: 50,
                customer_id: customerId,
                dealId: Number(dealId) ?? null,
            };
            const response = await getDealActivityHistoryDetails(data);
            if (response?.statusCode === 200) {
                const list = response?.customerDealActivityData || [];
                if (list?.length === 0) setHasMore(false);
                const groups: any = {};

                const iconMap: any = {
                    Call: Calliconlist,
                    Email: Email,
                    Notes: Notesactivity,
                    Meeting: Meetinglist,
                    Video: Meetinglist,
                    Proposal: proposalIcon,
                    quotation: quotationIcon,
                    Invoice: paymentIcon,
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
                        type: getChannelType(row),
                        date: row?.channelType === 'Meeting' ? getScheduleTime(row) : moment(time).format("dddd MMM D, YYYY - hh:mm a"),
                        title: row?.channelType === 'Notes' ? row?.Notes : row?.channelType === 'Email' ? row?.EmailSubject : row?.summary ? row?.summary : '',
                        from: row?.agent_name,
                        imageurl: row?.imageurl,
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

    const handleConvoStart = (value: any) => {
        console.log('value>>>>>>>>', value);
        const sessionId: string = uuidv4() + "deals";
        const body_ = {
            query: 'Show me the proposal in a clear, well-structured format so it’s easy to read and review.',
            sessionId,
            roleId: String(LoginCcaasUserDetails?.roleId || ""),
            org_role_id_crm: String(LoginCcaasUserDetails?.org_role_id_crm ?? ""),
            userId: String(LoginCcaasUserDetails?.userId || ""),
            domainId: String(LoginCcaasUserDetails?.domainId || ""),
            module: "",
            ext: String(LoginCcaasUserDetails?.ext || ""),
            agent: "deals",
            isBot: 0,
            deal_id: String(value?.deal_id || ""),
            customerId: String(customerId || value?.cus_id || ""),
        }

        const proposalBody = {
            query: 'Show me the proposal in a clear, well-structured format so it’s easy to read and review.',
            proposal_id: String(value?.proposal_id?.proposal_id || ""),
        };

        const quotationBody = {
            query: 'Show me the quotation in a clear, well-structured format so it’s easy to read and review.',
            quotation_id: String(value?.quotation_id?.quotation_id || ""),
        };

        const invoice_id = isJSON(value?.invoice) ? JSON.parse(value?.invoice)?.invoice_id : (value?.invoice?.invoice_id || "")
        const invoiceBody = {
            query: 'Show me the invoice in a clear, well-structured format so it’s easy to read and review.',
            invoice_id: String(invoice_id || ""),
        }

        if(value?.channelType === 'quotation') {
            Object.assign(body_, quotationBody)
        } else if(value?.channelType === 'Invoice') {
            Object.assign(body_, invoiceBody)
        } else if(value?.channelType === 'Proposal') {
            Object.assign(body_, proposalBody)
        }

        aiChatAssistSocketservice.io.emit("crm_chat", body_);
        dispatch(dealsDataSliceActions.setdealsAiAgentChatTyping({ sessionId, typing: true }));

        dispatch(
            commonDataSliceActions.setTabMenuPush({
                path: `/deals/chat-conversation?sessionId=${sessionId}`,
                menuName: "New Conversation",
                close: 1,
                type: "deals",
            })
        );

        navigate(`/deals/chat-conversation?sessionId=${sessionId}`);
    };

    const getData = (data: any) => {
        if (data?.channelType === 'Video') {
            if (data?.tags === 'scheduled') {
                navigate(`/deals/dealscheduledmeeting?channelType=${data?.channelType}&sessionId=${data?.sessionId}&VideoHistoryId=${data?.VideoHistoryId}&coid=${customerId}`)
            } else {
                navigate(`/deals/dealvideoviewdetails?channelType=${data?.channelType}&sessionId=${data?.sessionId}&VideoHistoryId=${data?.VideoHistoryId}&coid=${customerId}`)
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
            navigate(`/deals/companyinboundcallview?channelType=${data?.channelType ?? ''}&sessionId=${data?.sessionId ?? ''}&coid=${customerId ?? ''}`)

        } else if (data?.channelType === "Email") {
            dispatch(contactDataSliceActions.setCustomerEmail(data));
            navigate(`/deals/emailInbox?coid=${customerId}`);

        } else if (data?.channelType === 'Proposal' || data?.channelType === 'quotation') {
            if (data?.invoice?.pdf_link) {
                window.open(data?.invoice?.pdf_link, '_blank');
            } else {
                handleConvoStart(data);
            }
        } else if (data?.channelType === 'Invoice' && !data?.invoice?.payment?.invoice_link) {
            handleConvoStart(data);
        }
    }

    const handleDropdownChange = (option: any) => {
        const type =
            option?.channelType ??
            (option?.value === "All activity" ? null : option?.value);
        setOffset(0);
        setHasMore(true);
        setChannelType(type);
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
        { label: 'All activity', value: 'All activity' },
        { label: 'Email', value: 'Email' },
        { label: 'Call', value: 'Call' },
        { label: 'Meeting', value: 'Meeting' },
        { label: 'Notes', value: 'Notes' },

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
            return;
        }
        if (filterQueueList.length === 0) {
            return
        }
        dispatch(contactDataSliceActions.setCustomerDealId(Number(contactData?.deal_id)));
        const currentUser = loginDeatils?.find((user: any) => user?.UserID == userDetails?.userId);
        const queueIds = `${currentUser?.queueId ?? ""}`.split(",")?.map((id: string) => id?.trim())?.filter(Boolean);
        const currentQueue = (Array.isArray(filterQueueList) ? filterQueueList : []).find((item: any) => queueIds?.includes(`${item?.qid}`));

        const channelConfiguredCall = Array.isArray(configurationChannel?.call) && configurationChannel?.call?.length > 0        
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
        if (contactData?.primary_email) {
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
        const latestIsEmpty = latestHtml?.replace(/<br\s*\/?>/gi, "")?.replace(/&nbsp;/gi, " ")?.replace(/<[^>]*>/g, "")?.replace(/\u00a0/g, " ")?.trim()?.length === 0;
        if (latestIsEmpty) return;
        const data = {
            sessionId: "",
            customerId: contactData?.customer_id ?? contactData?.id ?? 0,
            agentId: userDetails?.userId ?? 0,
            channelType: "Note",
            notes: latestHtml,
            imageUrl: noteImages ?? "",
            dealId: Number(dealId) ?? null,
        };
        const response = await insertUpdateNotes(data);
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
    //     const latestIsEmpty = latestHtml?.replace(/<br\s*\/?>/gi, "")?.replace(/&nbsp;/gi, " ")?.replace(/<[^>]*>/g, "")?.replace(/\u00a0/g, " ")?.trim()?.length === 0;
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

        const latestIsEmpty =
            latestHtml
                ?.replace(/<br\s*\/?>/gi, "")
                ?.replace(/&nbsp;/gi, " ")
                ?.replace(/<[^>]*>/g, "")
                ?.replace(/\u00a0/g, " ")
                ?.trim()
                ?.length === 0;

        if (latestIsEmpty) return;

        const body = {
            text: latestHtml,
            tone: tone
        };

        const response: any = await generateAIMessage(body);

        const aiText = response ?? "";
        if (!aiText) return;

        setNoteText(aiText);

        const emailcontent = document.getElementById("emailcontent");
        if (emailcontent) {
            emailcontent.innerText = aiText;
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

    const normalizeToArray = (value: any) => {
        if (Array.isArray(value)) return value;
        if (!value) return [];
        if (typeof value === "string" && isJSON(value)) {
            try {
                const parsed = JSON.parse(value);
                if (Array.isArray(parsed)) return parsed;
                if (parsed && typeof parsed === "object") return [parsed];
                return [];
            } catch {
                return [];
            }
        }
        if (typeof value === "object") return [value];
        return [];
    };

    const dealDetailsList = normalizeToArray(selectedDeal?.deal_details);
    const primaryDealDetailsEntry = dealDetailsList[0] || {};
    const dealDetails = primaryDealDetailsEntry?.deal_details || primaryDealDetailsEntry || {};
    const aiSuggestionList = normalizeToArray(selectedDeal?.ai_suggest_summary_step);
    const selectedProductName = primaryDealDetailsEntry?.product || dealDetails?.product_name || "";
    const aiSuggestions = aiSuggestionList.find((item: any) => item?.product === selectedProductName) || aiSuggestionList[0] || {};
    const validateDealActivty = normalizeToArray(selectedDeal?.deal_activity);
    const formatListValue = (value: any) => {
        if (Array.isArray(value)) return value?.length ? value?.join(", ") : "-";
        if (value === null || value === undefined || value === "") return "-";
        return String(value);
    };
    const getDealInfoRows = (details: any) => ([
        { label: "Company", value: details?.company || "-" },
        {
            label: "Solutions",
            value: Array.isArray(details?.solutions) && details?.solutions?.length
                ? details?.solutions?.join(", ") : details?.solutions ? details?.solutions
                    : Array.isArray(details?.interested_product) && details?.interested_product?.length
                        ? details?.interested_product?.join(", ")
                        : "-"
        },
        { label: "Plan", value: details?.plan || "-" },
        { label: "Owner", value: details?.["Deal owner"] || details?.assigned_agent || "-" },
        { label: "Expected Close", value: details?.["Expected close"] || "-" },
    ]);
    const dealOverviewSections = (dealDetailsList?.length ? dealDetailsList : [dealDetails]).map((detailItem: any, index: number) => {
        const details = detailItem?.deal_details || detailItem || {};
        const productName = detailItem?.product || details?.product_name || "";
        const sectionAiSuggestion = aiSuggestionList.find((item: any) => item?.product === productName) || aiSuggestionList[index] || aiSuggestions || {};
        return {
            productName,
            dealInfoRows: getDealInfoRows(details),
            aiSuggestion: sectionAiSuggestion,
        };
    });
    const amountData = [
        { title: 'Total Amount', value: formatListValue(selectedDeal?.total_amount) },
        { title: 'Weighted Amount', value: formatListValue(selectedDeal?.weighted_amount) },
        { title: 'Open Amount', value: formatListValue(selectedDeal?.open_amount) },
        { title: 'Closed Amount', value: formatListValue(selectedDeal?.closed_amount) },
        { title: 'New amount', value: formatListValue(selectedDeal?.new_amount) },
        { title: 'Pending Amount', value: formatListValue(selectedDeal?.pending_amount) },
    ];
    const formatActivityText = (value: any) => {
        if (!value) return "";
        return String(value)
            .replace(/_/g, " ")
            .replace(/\b\w/g, (char) => char.toUpperCase());
    };
    const getBadgeClassName = (status: any) => {
        const normalizedStatus = String(status || '')
            .toLowerCase()
            .replace(/[\s_]+/g, '');

        if (normalizedStatus === 'inprogress') return styles.inprogress;
        if (normalizedStatus === 'completed') return '';
        return styles.upcoming;
    };
    const getActivityLineLabel = (item: any) => {
        if (item?.status) return item.status;
        if (!item?.date_time) return "Timeline";
        const parsedDate = moment(item?.date_time, ["YYYY-MM-DD HH:mm:ss", moment.ISO_8601], true);
        return parsedDate?.isValid() ? parsedDate.format("DD MMM YYYY, hh:mm A") : item?.date_time;
    };
    const dealActivity = validateDealActivty?.map((item: any, index: number) => ({
        step: index + 1,
        title: item?.title || formatActivityText(item?.action) || "Deal Activity",
        subtitle: item?.description || "-",
        lineLabel: getActivityLineLabel(item),
        badgeText: item?.status || formatActivityText(item?.action) || "Completed",
        active: true
    }));
    // const activityTimelineData = (selectedDeal?.activity_timeline || []).map((item: any) => ({
    //     title: item?.title || '-',
    //     desc: item?.description || '-',
    //     time: item?.datetime || '-',
    // }));

    const navigateToActivity = (data: any) => {
        navigate('/deals/activitydeals')
        dispatch(inboxDataSliceActions.setActivityTimeLine(data));
    }

    const navigateToQuote = () => {
        navigate('/deals/invoice')
    }

    const handlechange = (value: any) => {
        if (!value) return;
        aiNotes(value?.label);
    };

    const openMeet = (item: any) => {
        window.open(item.meet_url, '_blank')
        updateNLPJoinMeet({
            meetingId: item?.meetingId ?? '',
            domainId: `${item?.domainId ?? ''}`,
            sessionId: item?.sessionId ?? '',
            cusid: `${item?.CustomerId ?? ''}`,
        })
    }

    const handleNewConversation = (type: string) => {
       const channelConfiguredSMS = Array.isArray(configurationChannel?.sms) && configurationChannel?.sms?.length > 0;
       if( !channelConfiguredSMS && type === 'SMS') {
           toastnotification.error("Please configure the outbound number to send an SMS");
           return
       }
       const sessionId: any = uuidv4() + 'deals';
       let body: any = {
           query: type === 'SMS' ? 'Need SMS' : 'Need Meeting',
           sessionId: sessionId,
           roleId: LoginCcaasUserDetails?.roleId?.toString() ?? "",
           org_role_id_crm: LoginCcaasUserDetails?.org_role_id_crm?.toString() ?? "",
           userId: `${LoginCcaasUserDetails?.userId || ''}`,
           domainId: `${LoginCcaasUserDetails?.domainId || ''}`,
           module: "",
           ext: `${LoginCcaasUserDetails?.ext || ''}`,
           agent: 'deals',
           isBot: 0,
           customerId: `${customerId || ''}`,
           uploadUrl: [],
       }

       aiChatAssistSocketservice.io.emit('crm_chat', body);
       dispatch(contactDataSliceActions.setcontactAiAgentChatTyping({ sessionId: sessionId, typing: true }));

       const path = `/deals/chat-conversation?sessionId=${sessionId}`
       dispatch(commonDataSliceActions.setTabMenuPush({ path: path, menuName: "New Conversation", close: 1, type: "deals" }));
       navigate(path);
   }


    const updateActivityHandler =()=> {
        const sessionId: any = uuidv4() + 'deals';
        dispatch(commonDataSliceActions.setTabMenuPush({ path: `/deals/chat-conversation?sessionId=${sessionId}`, menuName: "New Conversation", close: 1, type: "deals" }));
        navigate(`/deals/chat-conversation?sessionId=${sessionId}`);
    }

    return (
        <>
            <div className={`${styles.companynameheader}`}>
                <div className={styles.tabactivemain}>
                    <div className={styles.filtertoplist}>
                        <div className={`${styles.companylisttopbar} `}>
                            <div className={styles.companyleft}>
                                <div className={styles.contactblock}>
                                    <div className={styles.contactprofile}>{initials}</div>
                                    <div className={styles.customername}>
                                        <div className={styles.customernametitle}>
                                            <div className={styles.customerdatafile}>{fullName}</div>
                                            {!!(selectedDeal?.pipeline_stage) && (
                                                <span>{selectedDeal?.pipeline_stage}</span>
                                            )}
                                        </div>
                                        <div className={styles.customermail}>{contactData?.primary_email}</div>
                                    </div>

                                </div>
                            </div>
                            <div className={styles.companyright}>
                                {/* <div className='dropdownquote'>
                                    <CustomSingleSelectDropdown options={[]} />
                                </div> */}
                                <span onClick={() => makecall(selectedPhoneNumber)}><img src={Callicon} alt="" /></span>
                                <span onClick={composeMail}><img src={Emaillicon} alt="" /></span>
                                <span onClick={()=> handleNewConversation('SMS')}><img src={SmsIcon} alt="" /></span>
                                <span onClick={toggleNote} style={{ position: "relative" }}><img src={Notesicon} alt="" />
                                    {showNotes &&
                                        <Draggable>
                                            <div className={styles.notesdropdown} onClick={(event) => event.stopPropagation()}>
                                                <div className={styles.header}>
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
                                                            accept="image/*"
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
                                                        {/* <span onClick={aiNotes} style={{ cursor: "pointer" }}>
                                                    <svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 20 20" fill="none">
                                                    <path d="M8.48652 16.5039C7.64821 16.7616 6.74461 16.8537 5.81653 16.7453C2.61954 16.3722 0.235205 13.7441 0.0673109 10.6358C0.0626471 10.5472 0.0591493 10.4574 0.0579834 10.3676C0.0766383 9.471 0.239869 7.23357 1.32186 5.23633C1.11315 5.90907 1.18311 6.67509 1.44428 7.33268C1.92581 8.54758 2.87838 9.30194 3.87991 10.0248C4.65409 10.5845 5.45509 11.1255 6.09169 11.8448C7.25879 13.1658 8.10176 14.7818 8.48768 16.5039H8.48652Z" fill="url(#paint0_linear_3560_53450)" />
                                                    <path d="M16.8871 10.9863C17.1693 11.8165 17.287 12.7177 17.2054 13.6482C16.9233 16.8545 14.3652 19.3134 11.2638 19.5711C11.1752 19.5793 11.0854 19.5851 10.9968 19.5874C10.099 19.5944 7.85812 19.4965 5.83057 18.4728C6.50914 18.6617 7.27166 18.5707 7.92225 18.2909C9.122 17.7756 9.84954 16.8008 10.5433 15.7795C11.0796 14.9901 11.5984 14.1728 12.298 13.5164C13.5852 12.312 15.1767 11.4224 16.8859 10.9863H16.8871Z" fill="url(#paint1_linear_3560_53450)" />
                                                    <path d="M11.4852 3.24875C12.3224 2.98758 13.226 2.89197 14.1541 2.99691C17.3522 3.35718 19.747 5.97587 19.9289 9.08308C19.9348 9.17169 19.9383 9.26147 19.9383 9.35125C19.9231 10.2478 19.7692 12.4864 18.6954 14.4883C18.9006 13.8144 18.8283 13.0496 18.5648 12.392C18.0786 11.1794 17.1225 10.4286 16.1187 9.70919C15.3433 9.15304 14.5388 8.61554 13.8999 7.89849C12.727 6.58215 11.8782 4.96967 11.4841 3.24875H11.4852Z" fill="url(#paint2_linear_3560_53450)" />
                                                    <path d="M10.0148 16.5269C10.7726 16.9688 11.6074 17.2486 12.5355 17.3524C15.7337 17.7126 18.652 15.6944 19.5206 12.7061C19.5451 12.621 19.5696 12.5347 19.5894 12.4473C19.7748 11.5693 20.4487 9.54528 19.523 7.16211C19.5731 7.86517 19.3318 8.59388 18.9283 9.17684C18.1845 10.2507 17.085 10.7695 15.9459 11.2464C15.0656 11.616 14.162 11.9611 13.3785 12.5172C11.9421 13.5398 10.7796 14.9366 10.0136 16.5257L10.0148 16.5269Z" fill="url(#paint3_linear_3560_53450)" />
                                                    <path d="M3.21861 8.58043C2.94812 7.74562 2.84318 6.84319 2.93879 5.91394C3.26758 2.71229 5.86062 0.290644 8.9655 0.0772784C9.05411 0.0714487 9.14389 0.066785 9.23366 0.0644531C10.1314 0.0702828 12.37 0.200867 14.3836 1.25487C13.7073 1.05666 12.9437 1.13711 12.2896 1.40644C11.0817 1.9043 10.3413 2.86852 9.63241 3.87939C9.08442 4.66056 8.55509 5.47089 7.84504 6.11681C6.54036 7.30257 4.9372 8.16886 3.22094 8.57927L3.21861 8.58043Z" fill="url(#paint4_linear_3560_53450)" />
                                                    <path d="M16.48 9.96944C16.9137 9.20692 17.2157 8.31615 17.3113 7.3869C17.6389 4.18524 15.5904 1.2879 12.5939 0.449593C12.5088 0.425109 12.4214 0.402956 12.3339 0.383135C11.4537 0.207079 9.42377 -0.447009 7.04993 0.504392C7.75182 0.447262 8.48403 0.680448 9.07049 1.07803C10.1525 1.8114 10.6818 2.90505 11.1715 4.0395C11.5493 4.91628 11.9037 5.81638 12.468 6.5929C13.5045 8.01883 14.8827 9.22091 16.48 9.9706V9.96944Z" fill="url(#paint5_linear_3560_53450)" />
                                                    <path d="M10.0744 3.26928C9.3177 2.82506 8.42343 2.50676 7.49535 2.39833C4.29836 2.02406 1.37187 4.0318 0.491588 7.01659C0.465937 7.1017 0.442619 7.18798 0.421632 7.27542C0.232751 8.15221 -0.449321 10.1739 0.467103 12.5606C0.4193 11.8575 0.664146 11.1288 1.06989 10.5494C1.81842 9.47904 2.91906 8.9637 4.06051 8.49149C4.94196 8.12539 5.84789 7.78377 6.63256 7.23112C8.07365 6.21559 9.30255 4.85728 10.0767 3.27044L10.0744 3.26928Z" fill="url(#paint6_linear_3560_53450)" />
                                                    <path d="M3.5929 9.81836C3.16966 10.5867 2.89567 11.4565 2.81405 12.3869C2.5319 15.5932 4.62242 18.4603 7.63169 19.2554C7.71681 19.2787 7.80425 19.2997 7.8917 19.3184C8.77431 19.4816 10.8135 20.1054 13.1734 19.1213C12.4726 19.189 11.7369 18.9663 11.1447 18.5769C10.0522 17.8598 9.50651 16.7732 9.00166 15.6469C8.61108 14.7759 8.24381 13.8805 7.66784 13.1121C6.61033 11.7013 5.19955 10.5459 3.59173 9.81836H3.5929Z" fill="url(#paint7_linear_3560_53450)" />
                                                    <defs>
                                                        <linearGradient id="paint0_linear_3560_53450" x1="0.792818" y1="3.01555" x2="4.27365" y2="16.1417" gradientUnits="userSpaceOnUse">
                                                            <stop stop-color="#6C00FF" />
                                                            <stop offset="0.52" stop-color="#4702D1" />
                                                            <stop offset="1" stop-color="#250E83" />
                                                        </linearGradient>
                                                        <linearGradient id="paint1_linear_3560_53450" x1="10.9548" y1="14.694" x2="16.5781" y2="16.3263" gradientUnits="userSpaceOnUse">
                                                            <stop stop-color="#6C00FF" />
                                                            <stop offset="0.52" stop-color="#4400CB" />
                                                            <stop offset="1" stop-color="#1E0099" />
                                                        </linearGradient>
                                                        <linearGradient id="paint2_linear_3560_53450" x1="17.8314" y1="10.9894" x2="15.6756" y2="5.81031" gradientUnits="userSpaceOnUse">
                                                            <stop offset="0.24" stop-color="#1E0099" />
                                                            <stop offset="0.81" stop-color="#6C00FF" />
                                                        </linearGradient>
                                                        <linearGradient id="paint3_linear_3560_53450" x1="16.9031" y1="19.2513" x2="16.9031" y2="5.38989" gradientUnits="userSpaceOnUse">
                                                            <stop stop-color="#00F9FF" />
                                                            <stop offset="0.5" stop-color="#6C00FF" />
                                                        </linearGradient>
                                                        <linearGradient id="paint4_linear_3560_53450" x1="3.68965" y1="4.14988" x2="10.0382" y2="4.55796" gradientUnits="userSpaceOnUse">
                                                            <stop stop-color="#6C00FF" />
                                                            <stop offset="0.52" stop-color="#4400CB" />
                                                            <stop offset="1" stop-color="#1E0099" />
                                                        </linearGradient>
                                                        <linearGradient id="paint5_linear_3560_53450" x1="10.662" y1="0.491567" x2="17.3467" y2="12.4487" gradientUnits="userSpaceOnUse">
                                                            <stop offset="0.269069" stop-color="#6C00FF" />
                                                            <stop offset="1" stop-color="#00F9FF" />
                                                        </linearGradient>
                                                        <linearGradient id="paint6_linear_3560_53450" x1="-0.00160205" y1="7.45614" x2="11.4479" y2="7.45614" gradientUnits="userSpaceOnUse">
                                                            <stop offset="0.06" stop-color="#380085" />
                                                            <stop offset="0.5625" stop-color="#4E44FF" />
                                                            <stop offset="1" stop-color="#00F9FF" />
                                                        </linearGradient>
                                                        <linearGradient id="paint7_linear_3560_53450" x1="2.7884" y1="14.7409" x2="13.1734" y2="14.7409" gradientUnits="userSpaceOnUse">
                                                            <stop offset="0.212285" stop-color="#6C00FF" />
                                                            <stop offset="1" stop-color="#00F9FF" />
                                                        </linearGradient>
                                                    </defs>
                                                </svg>
                                                </span> */}
                                                        <div className='activitydropdown aiactive'>
                                                            <Worktualdropdown options={aiOptions} onChange={handlechange} triggerIcon={AIIcon} />
                                                        </div>
                                                    </div>
                                                    <div className={styles.rights}>
                                                        <button onClick={saveContactNote}>Save</button>
                                                    </div>
                                                </div>
                                            </div>
                                        </Draggable>
                                    }
                                </span>

                                <span onClick={() => handleNewConversation('meeting')}><img src={Calendaricon} alt="" style={{ cursor: "pointer" }} /></span>

                                {/* <button onClick={() => navigateToQuote()}>View quote</button> */}
                            </div>
                        </div>
                        <div className={styles.filters}>
                            <span
                                className={`${styles.filterchip} ${activeTab === "overview" ? styles.active : ""}`}
                                onClick={() => switchDealTab("overview")}

                            >
                                Deal overview
                            </span>
                            <span
                                className={`${styles.filterchip} ${activeTab === "activity" ? styles.active : ""}`}
                                onClick={() => switchDealTab("activity")}

                            >
                                Activity
                            </span>



                        </div>
                    </div>
                    {activeTab === 'overview' &&
                        <>
                            <div className={`${styles.tabContent} ${activeTab === "overview" ? styles.tablistactive : ''}`}>
                                <div className={styles.summarycardblock}>
                                    <div className={styles.summarycardtitle}>Summary</div>
                                    <div className={styles.summarysubtitle}>{selectedDeal?.deal_summary || '-'}</div>
                                </div>
                                <div className={styles.totalblock}>
                                    {amountData?.map((item, index) => (
                                        <div className={styles.amountsetblock} key={index}>
                                            <div className={styles.amountitle}>{item?.title}</div>
                                            <div className={styles.amountsubtitle}>
                                                {item?.value === "-" ? "-" : `${currency}${item?.value}`}
                                            </div>
                                        </div>
                                    ))}
                                </div>
                                <div className={styles.dealactivityleft}>
                                    <div className={styles.dealblock}>
                                        <div className={styles.dealblocktitle}>Deal Activity</div>
                                        <button onClick={updateActivityHandler}>Update Status</button>
                                    </div>

                                    <div className={styles.dealcreatedblock}>
                                        {(dealActivity?.length ? dealActivity : [{
                                            step: 1,
                                            title: 'Deal Created',
                                            subtitle: '-',
                                            lineLabel: 'Timeline',
                                            badgeText: 'Completed',
                                            active: true
                                        }]).map((item, index) => (
                                            <div className={styles.loopcontainer} key={index}>
                                                <div
                                                    className={`${styles.loopcount} ${item?.active ? styles.active : ''
                                                        }`}
                                                >
                                                    {item?.step}
                                                </div>

                                                <div className={styles.loopblock}>
                                                    <div className={styles.loopblocktitle}>{item?.title}<span className={getBadgeClassName(item?.badgeText)}>{item?.badgeText}</span></div>
                                                    <div className={styles.loopblocksubtitle}>{item?.subtitle}</div>
                                                    <div className={styles.loopblocktitleline}>{item?.lineLabel}</div>
                                                </div>
                                            </div>
                                        ))}
                                    </div>
                                </div>
                                {dealOverviewSections.map((section: any, sectionIndex: number) => (
                                    <div className={styles.newcardblockdes}>
                                        {/* <div className={styles.dealblocktitle}>CCAAS subscription</div> */}
                                        <div key={`${section?.productName || "deal"}-${sectionIndex}`}>
                                            {section?.productName ? (
                                                <div className={styles.dealblocktitle}>{section?.productName}</div>
                                            ) : null}
                                            <div className={styles.customermaincontainer}>
                                                <div className={styles.personlistWrapper}>
                                                    {section?.dealInfoRows?.map((item: any, index: number) => (
                                                        <div className={styles.personlist} key={index}>
                                                            <div className={styles.customerlabel}>
                                                                {item?.label}
                                                            </div>
                                                            <div className={styles.customerlabelname}>
                                                                {item?.value}
                                                            </div>
                                                        </div>
                                                    ))}
                                                </div>
                                            </div>
                                            <div className={styles.blockcontainersuggestion}>
                                                <div className={styles.requiredtitle}>🧩 AI Suggested Requirement:</div>
                                                <div className={styles.requiredtitleaction}>{section?.aiSuggestion?.['AI suggested Requirement'] || '-'}</div>
                                            </div>
                                            <div className={styles.summarycontainerset}>
                                                <div className={styles.summarytitlesection}>Summary</div>
                                                <div className={styles.subsummary}>{section?.aiSuggestion?.Summary || section?.aiSuggestion?.lead_summary || '-'}</div>
                                            </div>
                                            <div className={styles.summarycontainerset}>
                                                <div className={styles.summarytitlesection}>Next Steps</div>
                                                <div className={styles.subsummary}>{section?.aiSuggestion?.['Next Steps'] || section?.aiSuggestion?.recommended_next_action || '-'}</div>
                                            </div>
                                        </div>
                                    </div>
                                ))}
                            </div>

                        </>


                    }
                    {activeTab === 'activity' && (
                        <>
                        <div className={styles.feedWrap}>
                            <div className={styles.feedHeader}>
                                <div className={styles.feedTitle}>All activity</div>
                                <div className={`${styles.emaildropdown} activitydropdown activitylist`}>
                                    <CustomSingleSelectDropdown
                                        options={dropdownOptions?.filter((data: any) => data?.value !== selected)}
                                        value={selected}
                                        onChange={(option: any) => {
                                            setSelected(option?.value);
                                            handleDropdownChange(option);
                                        }}
                                    />
                                </div>
                            </div>

                            {activityData?.length > 0 ? (
                                activityData.map((group: any) => (
                                    <div key={group.label}>
                                        <div className={styles.groupLabel}>{group.label}</div>

                                        {group?.items?.map((item: any, idx: any) => (
                                            <div className={styles.item} key={idx} onClick={() => getData(item)}>

                                                <div className={styles.iconWrap}>
                                                    <img src={item.icon} alt={item.type} />

                                                </div>

                                                <div className={styles.itemBody}>
                                                    <div>
                                                        <div className={styles.itemTop}>
                                                            <div className={styles.itemName}>
                                                                {item.channelType === 'Video' ? item.meeting_title : item?.type}

                                                                {(!!item?.invoice?.status && item?.channelType === 'Invoice') && 
                                                                    <span className={`${styles.badge} ${styles.badgeGreen}`}>{item?.invoice?.status}</span>}

                                                            </div>
                                                            <div className={styles.itemDate}>{item.date}</div>
                                                        </div>

                                                        {item.title && (
                                                            <div
                                                                className={styles.itemDesc}
                                                                dangerouslySetInnerHTML={{ __html: item.title }}
                                                            />
                                                        )}

                                                        {item.imageurl && (
                                                            <div className={styles.imgContainer}>
                                                                <img src={item.imageurl} alt="item" height={60} />
                                                            </div>
                                                        )}
                                                    </div>

                                                    <div className={styles.bottom}>
                                                        {(item?.channelType === 'Invoice' && item?.invoice?.total_amount) ? (
                                                                <div className={styles.itemMeta}>
                                                                    Total amount: {item?.invoice?.total_amount} {item?.invoice?.currency === 'INR' ? "₹" : "£"}
                                                                </div>
                                                                
                                                            ) : item.from ? (
                                                                    <div className={styles.itemMeta}>
                                                                        {item.type === 'Notes' ? 'Created By:' :
                                                                            item.type === 'Call' ? 'Handled by:' :
                                                                                item.type === 'Quote' ? 'Quote owner:' :
                                                                                    item.type === 'Proposal' ? 'Proposal owner:' : 'From'
                                                                        }
                                                                        <span>{item.from}</span>
                                                                    </div>

                                                                ) : item.amount ? (
                                                                        <div className={styles.itemMeta}>
                                                                            Total amount: <span>{item.amount}</span>
                                                                        </div>

                                                                    ) : item.meet_url && (
                                                                            <button
                                                                                className={styles.meetingBtn}
                                                                                onClick={(e) => {
                                                                                    e.stopPropagation();
                                                                                    openMeet(item);
                                                                                }}
                                                                            >
                                                                                Join Meet
                                                                            </button>
                                                                        )}
                                                        {item?.invoice?.payment?.invoice_link && (
                                                            <div className={styles.itemMeta} onClick={() => window.open(item?.invoice?.payment?.invoice_link, '_blank')}>
                                                                <svg width="24" height="24" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg">
                                                                    <path d="M10.3926 16.8398C10.8198 17.2763 11.3906 17.5166 11.9995 17.5166C12.6089 17.5166 13.1797 17.2764 13.6069 16.8398L17.8637 12.4941C18.1537 12.1982 18.1489 11.7236 17.853 11.4336C17.5571 11.1445 17.0825 11.1484 16.7925 11.4453L12.7558 15.5661L12.7495 4.23145C12.7495 3.81735 12.4136 3.48145 11.9995 3.48145C11.5854 3.48145 11.2495 3.81735 11.2495 4.23145L11.2558 15.5786L7.20654 11.4453C6.91654 11.1494 6.44194 11.1455 6.14604 11.4336C5.85014 11.7236 5.84524 12.1982 6.13534 12.4941L10.3926 16.8398Z" fill="#23232B" />
                                                                    <path d="M17.3282 19.0127L6.67054 19.0186C6.25644 19.0186 5.92104 19.3545 5.92104 19.7686C5.92104 20.1827 6.25694 20.5186 6.67104 20.5186L17.3287 20.5127C17.7428 20.5127 18.0782 20.1768 18.0782 19.7627C18.0782 19.3486 17.7423 19.0127 17.3282 19.0127Z" fill="#23232B" />
                                                                </svg>
                                                                Download
                                                            </div>
                                                        )}
                                                        {/* <div className={styles.itemMeta}>Participants:
                                                            <div className="iconStack">
                                                                <img src={Email} />
                                                                <img src={Email} />
                                                                <img src={Email} />

                                                            </div>
                                                        </div> */}

                                                
                                                    </div>
                                                </div>
                                            </div>
                                        ))}
                                        {loader && (
                                            <div className={styles.activitySkeleton}>
                                                {Array.from({ length: 3 }).map((_, index) => (
                                                    <SkeletonBox key={`activity-skeleton-more-${index}`} width="100%" height="60px" />
                                                ))}
                                            </div>
                                        )}
                                    </div>
                                ))
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
                         
                        </>
                    )}

                </div>
                {/* <div className={styles.dealactivityleft}>
                    <div className={styles.calldiscover}>
                        <div className={styles.discoverblocklive}>
                            <img src={Googlemeet} alt="Googlemeet" />
                            <div className={styles.calltitlelist}>
                                <div className={styles.callsubtitle}>Google meet - Discovery Call <span>Schedule meeting</span></div>
                                <div>Initial discovery call to understand their requirements</div>
                            </div>
                        </div>
                        <div className={styles.callslot}>Starts in 2 min</div>
                    </div>
                    <div className={styles.cardblocksetcall}>
                        <div className={styles.callcardtitle}>
                            <div className={styles.discovery}>
                                <div className={styles.callcardblock}>
                                    <div className={styles.datecontainerleft}>
                                        <img src={Calendarlist} alt="Calendarlist" />
                                    </div>
                                    <div className={styles.datecontainerright}>
                                        <div className={styles.discovercall}>Date</div>
                                        <div className={styles.discovercalldate}>Tuesday, January 6, 2026</div>
                                    </div>
                                </div>
                                <div className={styles.callcardblock}>
                                    <div className={styles.datecontainerleft}>
                                        <img src={Clockicon} alt="Clockicon" />
                                    </div>
                                    <div className={styles.datecontainerright}>
                                        <div className={styles.discovercall}>Time</div>
                                        <div className={styles.discovercalldate}>14:00 - 15:30</div>
                                    </div>
                                </div>
                                <div className={styles.callcardblock}>
                                    <div className={styles.datecontainerleft}>
                                        <img src={Locationlist} alt="Locationlist" />
                                    </div>
                                    <div className={styles.datecontainerright}>
                                        <div className={styles.discovercall}>Location</div>
                                        <div className={styles.discovercalldate}>Google Meet</div>
                                    </div>
                                </div>
                                  <div className={styles.callcardblock}>
                                    <div className={styles.datecontainerleft}>
                                        <img src={Meetinglisticon} alt="Meetinglisticon" />
                                    </div>
                                    <div className={styles.datecontainerright}>
                                        <div className={styles.discovercall}>Meeting with</div>
                                        <div className={styles.discovercalldate}>Steve Harrington</div>
                                    </div>
                                </div>
                            </div>
                        </div>
                    </div>
                    <div className={styles.meetinglinkcontainer}>
                        <div className={styles.meetingblock}>
                            <div className={styles.meetingleftblock}>
                            </div>
                            <div className={styles.copyrightblock}>
                                <div className={styles.meetingrightblock}>
                                    <svg xmlns="http://www.w3.org/2000/svg" width="18" height="18" viewBox="0 0 18 18" fill="none">
                                        <path d="M16.5928 1.32295C15.4546 0.24285 13.8828 0.06025 12.355 0.02415C11.1109 -0.00805 10.23 -0.00805 8.9898 0.02415C7.461 0.06025 5.8892 0.24195 4.751 1.32295C4.2798 1.76975 3.9921 2.31125 3.8104 2.88005C2.7683 3.07735 1.9437 3.45645 1.3149 4.05335C0.0644 5.23985 0.0287999 7.06705 0.00289989 8.40195L0 11.814L0.00289989 11.9615C0.0287999 13.2955 0.0644 15.1227 1.3149 16.319C2.5024 17.446 4.1474 17.6364 5.7465 17.6744C6.4003 17.691 6.9584 17.6989 7.516 17.6989C8.0736 17.6989 8.6322 17.691 9.2841 17.6744C10.8847 17.6364 12.5297 17.446 13.7172 16.319C14.4232 15.6496 14.7279 14.7809 14.8719 14.001C15.5016 13.8097 16.0906 13.5145 16.5927 13.0377C17.789 11.9039 17.8246 10.1792 17.8481 9.02305V5.32285C17.8247 4.18225 17.7891 2.45775 16.5928 1.32295ZM16.3357 4.91235C15.5792 4.90385 15.033 4.87185 14.6025 4.74775C13.3823 4.41085 12.4614 3.28775 12.3628 2.01145C12.3494 1.84985 12.3571 1.68795 12.3595 1.52635C13.7583 1.56175 14.8409 1.72755 15.5605 2.41085C16.1613 2.98115 16.2956 3.90765 16.3357 4.91235ZM12.6851 15.2311C11.9112 15.9655 10.752 16.1394 9.2476 16.1746C7.9683 16.2078 7.0664 16.2078 5.7832 16.1746C4.2803 16.1394 3.1211 15.9646 2.3496 15.2331C1.5517 14.4704 1.5259 13.1219 1.5029 11.9333L1.5 8.58085L1.5029 8.43145C1.5258 7.24295 1.5517 5.89625 2.3471 5.14145C2.6561 4.84845 3.046 4.63805 3.5287 4.48785C3.5052 4.83725 3.4981 5.17485 3.4921 5.48415L3.4892 8.72925L3.4921 8.86985C3.5165 10.1442 3.5502 11.8904 4.7504 13.0379C5.8886 14.118 7.4604 14.3005 8.9882 14.3366C9.6103 14.3523 10.141 14.36 10.6718 14.36C11.2021 14.36 11.7333 14.3523 12.3534 14.3366C12.6528 14.3295 12.9534 14.3138 13.2526 14.2898C13.1195 14.6835 12.9358 14.9933 12.6851 15.2311ZM15.5606 11.9499C14.8331 12.6403 13.7383 12.8034 12.3169 12.8366C11.1001 12.867 10.2437 12.867 9.0249 12.8366C7.605 12.8034 6.5103 12.6404 5.7852 11.9518C5.0381 11.2371 5.0137 9.96455 4.9917 8.84145L4.9893 5.65595L4.9917 5.51235C5.0137 4.39025 5.0381 3.11785 5.7837 2.41075C6.5103 1.72035 7.605 1.55725 9.0264 1.52405C9.6348 1.50845 10.1529 1.50065 10.6719 1.50065C10.7242 1.50065 10.7787 1.50165 10.8311 1.50185C10.8284 1.71325 10.8205 1.92565 10.8379 2.13345C10.9868 4.06705 12.3364 5.70965 14.187 6.22035C14.7837 6.39245 15.4483 6.43175 16.3481 6.44105V9.00745C16.3242 10.1871 16.2457 11.3005 15.5606 11.9499Z" fill="#23232B" />
                                    </svg>
                                    <span>Copy Link</span>
                                </div>
                                <div className={styles.startmeet}>
                                    <svg xmlns="http://www.w3.org/2000/svg" width="19" height="19" viewBox="0 0 19 19" fill="none">
                                        <path d="M17.4748 8.2154C17.0606 8.2142 16.7238 8.5489 16.7227 8.9632C16.7173 10.7967 16.7119 12.6297 16.7065 14.463C16.7033 15.703 15.6964 16.7069 14.4566 16.7067H3.75C2.5076 16.7043 1.5022 15.6991 1.4999 14.4567V3.7501C1.5022 2.5077 2.5076 1.5023 3.75 1.4999H9.1033C9.5174 1.4999 9.8531 1.1642 9.8531 0.7501C9.8531 0.3358 9.5174 0.000299931 9.1033 0.000299931L3.75 9.98974e-05C1.6786 0.000599897 0.0005 1.6788 0 3.7501V14.4566C0.0004 16.5279 1.6786 18.2061 3.75 18.2064H14.4565C16.5235 18.2062 18.1999 16.5348 18.2063 14.4674C18.2117 12.6341 18.217 10.8007 18.2224 8.9676C18.2236 8.5533 17.8889 8.2166 17.4748 8.2154Z" fill="white" />
                                        <path d="M11.4772 7.8062L16.7226 2.5606V5.293C16.7226 5.7071 17.0584 6.043 17.4726 6.043C17.8868 6.043 18.2226 5.7071 18.2226 5.293V1.75C18.2221 0.783 17.4394 0.0005 16.4726 0H12.9296C12.5154 0 12.1796 0.3357 12.1796 0.75C12.1796 1.1641 12.5154 1.5 12.9296 1.5L15.662 1.4998L10.4165 6.7454C10.1237 7.0384 10.1237 7.5132 10.4165 7.8062C10.7093 8.0989 11.1843 8.0989 11.4772 7.8062Z" fill="white" />
                                    </svg>
                                    <span>Start Meeting Now</span>
                                </div>
                            </div>
                        </div>
                    </div>
                </div> */}

                {/* {activityTimelineData?.length > 0 ? <div className={styles.dealactivityleft}>
                    <div className={styles.dealblock}>
                        <div className={styles.dealblocktitle}>Activity Timeline</div>
                        <button>Update activity</button>
                    </div>
                    <Activitytimeline activities={activityTimelineData} />
                    <button className={styles.viewallactivity} onClick={() => navigateToActivity(activityTimelineData)}>View all Activity<svg xmlns="http://www.w3.org/2000/svg" width="18" height="13" viewBox="0 0 18 13" fill="none">
                        <path d="M16.3604 4.48975L12.0899 0.21975C11.7969 -0.07325 11.3223 -0.07325 11.0294 0.21975C10.7365 0.51275 10.7364 0.98735 11.0294 1.28025L15.0797 5.33005H0.75C0.3359 5.33005 0 5.66595 0 6.08005C0 6.49415 0.3359 6.83005 0.75 6.83005H15.0791L11.0293 10.8799C10.7363 11.1729 10.7363 11.6475 11.0293 11.9404C11.1758 12.0869 11.3672 12.1601 11.5596 12.1601C11.752 12.1601 11.9434 12.0869 12.0899 11.9404L16.3555 7.67475C16.7832 7.25485 17.0196 6.69335 17.0225 6.09365C17.0254 5.48865 16.7901 4.91945 16.3604 4.48975Z" fill="white" />
                    </svg></button>
                </div> : ''} */}
            </div>
        </>
    )
}
export default Dealscontent;
