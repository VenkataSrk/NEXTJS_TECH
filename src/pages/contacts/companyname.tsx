"use-client"
import { useEffect, useRef, useState } from 'react';
import styles from './scss/contactname.module.scss'
import Callicon from '../../assets/images/contacts/contactcall.svg'
import Emaillicon from '../../assets/images/contacts/contactmail.svg'
import Notesicon from '../../assets/images/contacts/notes.svg'
import Calendaricon from '../../assets/images/contacts/calendar.svg'
import Email from '../../assets/images/contacts/activityemail.svg'
import Calliconlist from '../../assets/images/contacts/callflowicon.svg'
import Activityicon from '../../assets/images/contacts/activityicon.svg'
import Notesactivity from '../../assets/images/contacts/notesactivity.svg'
import Meetinglist from '../../assets/images/contacts/meetingeditor.svg'
import Worktualdropdown from '../component/worktualdropdown';
import Deals from './deals';
import Whatsupcoming from './whatupcoming';
import Contactproject from './contactproject';
import { createInfiniteScroll, formatDate } from '../../base/utils';
import { getCompanyContactDetails, getCompanyDetails, getCompanySessionDetails } from '../../base/service/core_serviceApi';
import { GetItemFromStorage } from '../../base/custom_hooks/useStorage';
import { LocalStorage } from '../../base/custom_hooks/localStorageKeys';
import moment from 'moment';
import { useNavigate, useParams, useSearchParams } from 'react-router-dom';
import { useDispatcher } from '../../store/redux-store/dispatch';
import { commonDataSliceActions } from '../../store/slice/commonSilce/commonSlice';
import { ACTIONS_ACCESS, levelOfAccess, MENUS } from '../../base/constant';
import { useSelector } from 'react-redux';
type CompanynameProps = {
    comapnyData?: any;
};

const Companyname = () => {
    const [activeTab, setActiveTab] = useState("company");
    const [customerContactData, setCustomerContactData] = useState<any>([]);
    const lastScrollTop = useRef(0);
    const [companyActivityData, setCompanyActivityData] = useState<any>([])
    const [loader, setLoader] = useState(false);
    const [offset, setOffset] = useState(0);
    const [hasMore, setHasMore] = useState(true);
    const hasMoreRef = useRef(hasMore);
    const loaderRef = useRef(loader);
    const [channelType, setChannelType] = useState<any>(null);
    const userDetails = GetItemFromStorage(LocalStorage.USER_DETAILS);
    const [comapnyData, setSelectedcomapnyData] = useState<any>({});
    const dispatch = useDispatcher();
    const [params] = useSearchParams()
    const companyId = params.get('companyId')
    const company_id = params.get('company_id')
    const navigate = useNavigate()
    const { permissions } = useSelector((store: any) => store?.loginUser?.rolesandpermissions)

    useEffect(() => {
        getCompanyDetailshandler();
    }, [companyId])

    const getCompanyDetailshandler = async () => {
        const res = await getCompanyDetails({ search: companyId });
        const record = Array.isArray(res) ? (res[0] ?? {}) : (res ?? {});
        setSelectedcomapnyData(record);


        const fullName = `${record?.company_name || ""}`.trim() || "New Company";

        dispatch(commonDataSliceActions.setTabMenuPush({
           path: `/contact/companydetais?companyId=${companyId}&company_id=${company_id}` ,
           menuName: fullName, 
           close: 1 , 
           type: "contact"
       }));

    }

    const initials = comapnyData?.company_name?.charAt(0).toUpperCase() ?? '';
    const LoginUserDetails: any = userDetails;

    const persons = [
        { label: "Website", name: comapnyData?.company_url ?? "_" },
        { label: "Owner name", name: comapnyData?.owner_name?.toString() ?? "-" },
        { label: "Source", name: comapnyData?.source_name ?? "-" },
        { label: "Address", name: comapnyData?.company_address ?? "-" },
        { label: "Created Date", name: comapnyData?.created_at ? formatDate(comapnyData.created_at) : "-" },
        { label: "Industry", name: comapnyData?.industry ?? "-" },
        { label: "About us", name: comapnyData?.about ?? "-" },
        { label: "Company Size", name: comapnyData?.company_size ?? "-" },
        { label: "Company Name", name: comapnyData?.company_name ?? "-" },
    ];

    useEffect(() => {
        let cancelled = false;
        const companyName = comapnyData?.company_name;
        if (!companyName) {
            setCustomerContactData([]);
            return;
        }
        const fetchData = async () => {
            try {
                const res = await getCompanyContactDetails(companyName);
                if (!cancelled) {
                    setCustomerContactData(Array.isArray(res) ? res : []);
                }
            } catch (err) {
                if (!cancelled) {
                    console.error("Error fetching company contacts:", err);
                    setCustomerContactData([]);
                }
            }
        };
        fetchData();
        return () => {
            cancelled = true;
        };
    }, [comapnyData?.company_name]);

    const dropdownOptions = [
        { label: 'All activity', value: 'All activity' },
        { label: 'Email ', value: 'Email ' },
        { label: 'Call', value: 'Call' },
        { label: 'Meeting', value: 'Meeting' },
        { label: 'Note', value: 'Note' },

    ];

    useEffect(() => {
        hasMoreRef.current = hasMore;
    }, [hasMore]);

    useEffect(() => {
        loaderRef.current = loader;
    }, [loader]);


    useEffect(() => {
        setCompanyActivityData([]);
        setHasMore(true);
        setOffset(0);
    }, [channelType]);

    useEffect(() => {
        getSessionHistory(channelType, offset, offset !== 0);
    }, [offset, channelType]);

    const getSessionHistory = async (channelTypeParam: any = null, pageOffset: number = 0, append = false) => {
        try {
            setLoader(true);

            const data = {
                domainId: LoginUserDetails?.domainId,
                channelType: channelTypeParam,
                companyName: comapnyData?.company_name ?? null,
                agentId: LoginUserDetails?.userId,
                offset: pageOffset,
                limit: 50
            };

            const response = await getCompanySessionDetails(data);

            if (response?.statusCode === 200) {
                const list = response?.customerContactRes || [];

                if (list?.length === 0) setHasMore(false);

                const groups: any = {};

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
                        id: row?.sessionId + i + pageOffset,
                        icon: row?.channelType === 'Call' ? Calliconlist : Email,
                        type: row?.channelType,
                        date: moment(time).format("dddd MMM D, YYYY - hh:mm a"),
                        title: row?.summary || '-',
                        from: row?.agent_name,
                    });
                });

                const formatted = Object.keys(groups).map((label) => ({
                    label,
                    items: groups[label],
                }));

                if (append) {
                    setCompanyActivityData((prev: any) => [...prev, ...formatted]);
                } else {
                    setCompanyActivityData(formatted);
                }
            }
        } catch (error) {
            console.log(error);
        } finally {
            setLoader(false);
        }
    };

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
                setOffset((prev) => {
                    const next = prev + 1;
                    getSessionHistory(channelType, next, true);
                    return next;
                });
            },
        })
    ).current;

    const navigateToContact = (coid : any)=>{
        navigate('/contact/contactdetais?coid=' + coid)
    }

    return (
        <>
            
            <div className={`${styles.companynameheader} ${activeTab === "activity" ? styles.companylisthidden : ''}`}>
                <div className={styles.filtertoplist}>
                    <div className={`${styles.companylisttopbar}`}>
                        <div className={styles.companyleft}>
                            <div className={styles.contactblock}>
                                <div className={styles.contactprofile}>{initials}</div>
                                <div className={styles.customername}>
                                    <div className={styles.customernametitle}>{comapnyData?.company_name ?? "-"}
                                        <span>Qualified</span>
                                    </div>
                                    <div className={styles.customermail}>{comapnyData?.primary_email ?? "-"}</div>
                                </div>
                            </div>
                        </div>
                        {/* <div className={styles.companyright}>
                            <span><img src={Callicon} alt="" /></span>
                            <span><img src={Emaillicon} alt="" /></span>
                            <span><img src={Notesicon} alt="" /></span>
                            <span><img src={Calendaricon} alt="" /></span>
                        </div> */}
                    </div>
                    <div className={styles.filters}>
                        <span
                            className={`${styles.filterchip} ${activeTab === "company" ? styles.active : ""
                                }`}
                            onClick={() => setActiveTab("company")}
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

                        {levelOfAccess(MENUS.PRODUCTS, ACTIONS_ACCESS.VIEW, (permissions ?? [])) &&<span
                            className={`${styles.filterchip} ${activeTab === "products" ? styles.active : ""
                                }`}
                            onClick={() => setActiveTab("products")}
                        >
                            Products
                        </span>}

                       {levelOfAccess(MENUS.PROJECTS, ACTIONS_ACCESS.VIEW, (permissions ?? [])) && <span
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
                    {activeTab === "company" &&
                        <div className={styles.customermaincontainer}>
                            <div className={styles.personlistWrapper}>
                                {persons.map((person, index) => (
                                    <div key={index} className={styles.personlist}>
                                        <div className={styles.customerlabel}>
                                            {person.label}
                                        </div>
                                        <div className={styles.customerlabelname}>
                                            {person.name}
                                        </div>
                                    </div>
                                ))}
                            </div>
                            {Array.isArray(customerContactData) && customerContactData?.length > 0 ? (
                                customerContactData.map((item: any, index: number) => {
                                    const cname =item?.customer_name?.trim() || 'New Customer'
                                    return (
                                        <div className={styles.campaignblock}>
                                            <div className={styles.campaignblockcontainer} key={index} onClick={() => navigateToContact(item?.customer_id)}>
                                                <div className={styles.administratorlead}>
                                                    <div className={styles.campaignicons}>
                                                        <div>{cname?.charAt(0) ?? ""}</div>
                                                    </div>
                                                    <div className={styles.retailblock}>
                                                        <div className={styles.retailblocktitle}>{cname ?? "-"}</div>
                                                    </div>
                                                </div>
                                                <div className={styles.joinedtitle}>
                                                    <div className={styles.joinedtitleupdate}>{Number(item?.is_active) === 1 ? "Active" : "Inactive"}</div>
                                                    <div className={styles.subtitle}>
                                                        Joined {item?.created_at ? formatDate(item.created_at) : "-"}
                                                    </div>
                                                </div>
                                            </div>
                                        </div>
                                    )
                                })
                            ) : null}
                        </div>}
                    {activeTab === "activity" &&
                        <div className={`${styles.customermaincontainer} ${styles.contactprojectlist}`} onScroll={handleScroll}>
                            <div className={styles.customeractive}>
                                <div className={styles.activitymainlog}>
                                    <div className={styles.allemail}>All activity</div>
                                    <div className={`${styles.emaildropdown} activitydropdown`}>
                                        <Worktualdropdown
                                            options={dropdownOptions}
                                            onChange={handleDropdownChange}
                                        />
                                        {/* <Worktualdropdown options={dropdownOptions} /> */}
                                    </div>
                                </div>
                                {companyActivityData?.map((group: any) => (
                                    <div className={styles.allactivitycontainer} key={group?.label}>
                                        <div className={styles.allactivitytitle}>{group?.label}</div>
                                        {group?.items.map((item: any) => (
                                            <div className={styles.activitymain} key={item.id}>
                                                <img src={item?.icon} alt={item?.type} />

                                                <div className={styles.activitycontainer}>
                                                    <div className={styles.direct}>
                                                        <div className={styles.directleft}>{item?.type}
                                                            {/* <span className={`${styles.meeting} ${styles.cancelled} ${styles.missed}`}>Complete</span> */}
                                                        </div>
                                                        <div className={styles.directright}>{item?.date}</div>
                                                    </div>

                                                    <div className={styles.plantitile}>{item?.title}</div>
                                                    <div className={styles.plantitilesub}>
                                                        From : <span>{item?.from}</span>
                                                    </div>
                                                </div>
                                            </div>
                                        ))}
                                    </div>
                                ))}
                            </div>
                        </div>
                    }
                    {activeTab === "deals" && <div><Deals /></div>}
                    {activeTab === "products" && <div>Contacts content</div>}
                    {activeTab === "projects" && <div><Contactproject /></div>}
                    {activeTab === "whatscoming" && <div><Whatsupcoming /></div>}
                </div>
            </div>
        </>
    )
}
export default Companyname;
