import styles from "../../assets/styles/agentroleview.module.scss";
import Activityloglist from '../../assets/images/myplan/activitylog.svg'
import Security from '../../assets/images/myplan/securitylist.svg'
import Devicelist from '../../assets/images/myplan/activedevices.svg'
import { useEffect, useState } from "react";
import { getUserExtensionInfo, getUserLogActivity, getuserPermission, UpdateUserPermission } from "../../apiservice/myplanapi";
import { roleAccess } from "../../../../base/utils";
import { useLocation, useNavigate } from "react-router-dom";
import { GetItemFromStorage } from "../../../../base/custom_hooks/useStorage";
import { LocalStorage } from "../../../../base/custom_hooks/localStorageKeys";
import { useDispatch, useSelector } from "react-redux";
import { MyplanProfileActions } from "../../../../store/slice/my_plans/profile";
import { getDevices, getMyplanDescription } from "../../apiservice/myplan";
import Skeleton from "../../skeleton";
import moment from "moment";

const Agentroleview = () => {
    const navigate = useNavigate();
    const userDetails = GetItemFromStorage(LocalStorage.USER_DETAILS);
    const loginUserDetails: any = GetItemFromStorage(LocalStorage.USER_DETAILS);
    const [profileData, setProfileData] = useState<any>(null);
    const location = useLocation();
    const stateData = location.state;
    const myPlanUpdation: any = useSelector(
        (state: any) =>
            state?.myPlanStore?.myPlanUpdation ??
            state?.userExtension?.myPlanUpdation ??
            false
    );
    const dispatch = useDispatch()
    const [loader, setLoader] = useState(false);
    const [myplanDesc, setMyPlanDesc] = useState('');
    const [LogData, setLogData] = useState<any[]>([]);
    const [showAll, setShowAll] = useState(false);
    const visibleLogs = showAll ? LogData : LogData.slice(0, 2);
    const [userPermission, serUserPermission] = useState({
        is_voice: 0,
        is_chat: 0,
        is_recording: 0,
        is_email: 0,
        is_knowledgeBase: 0,
    });
    useEffect(() => {
        if (stateData?.Email) {
            getDevices1(stateData?.Email);
        }
    }, [stateData?.Email])

    const getParticularUserList = (data: any) => {
        if (!data || !data.Extension_Number) {
            return;
        }

        try {
            const body = {
                company_id: userDetails.company_id,
                orderid: userDetails.order_id,
                extension_number: Number(data?.Extension_Number)
            };

            getUserExtensionInfo(body)
                .then((res: any) => {
                    setProfileData(res.data.finalresponse[0]);
                    dispatch(MyplanProfileActions.setMyPlanUpdation(false));
                })
                .catch((err: any) => console.log(err));

        } catch (error) {
            console.log("getParticularUserList Error", error);
        }
    };

    useEffect(() => {
        if (stateData) {
            getParticularUserList(stateData);
        }
    }, [stateData, myPlanUpdation]);


    const first = profileData?.Firstname?.[0]?.toUpperCase() || "";
    const last = profileData?.Surname?.[0]?.toUpperCase() || "";
    const initials = (first + last) || "NA";
    const roleMap: any = {
        3: "Admin",
        5: "Supervisor",
        6: "Agent"
    };

    const roleName = roleMap[profileData?.org_role_id] || "Unknown";

    const handleToggleChange = (key: string, value: boolean) => {
        const updatedValue = value ? 1 : 0;

        const updatedState = {
            ...userPermission,
            [key]: updatedValue,
        };
        serUserPermission(updatedState);
        insertUpdatePermission(updatedState);
    };

    const insertUpdatePermission = async (data: any) => {
        try {
            const body = {
                userId: stateData?.Idx,
                is_voice: data?.is_voice,
                is_chat: data?.is_chat,
                is_recording: data?.is_recording,
                is_email: data?.is_email,
                is_knowledgeBase: data?.is_knowledgeBase,
            };

            await UpdateUserPermission(body);

            serUserPermission({
                is_voice: body?.is_voice,
                is_chat: body?.is_chat,
                is_recording: body?.is_recording,
                is_email: body?.is_email,
                is_knowledgeBase: body?.is_knowledgeBase,
            });

        } catch (error) {
            console.log("insertUpdatePermission Error:", error);
        }
    }

    const getuserPermissionData = async () => {
        try {
            const response = await getuserPermission(stateData?.Idx);

            if (response) {
                serUserPermission({
                    is_voice: response?.voice,
                    is_chat: response?.chat,
                    is_recording: response?.recording,
                    is_email: response?.email,
                    is_knowledgeBase: response?.knowledgeBase,
                });
            }
        } catch (err) {
            console.log("getuserPermissionData GET Error:", err);
        }
    };

    // useEffect(() => {
    //     getuserPermissionData();
    // }, [])

    useEffect(() => {
        const fetchPlanDesc = async () => {
            setLoader(true);
            try {
                const data = {
                    "company_name": loginUserDetails?.companyName,
                    "role_id": `${stateData?.org_role_id}`,
                    "access_information": roleAccess[stateData?.org_role_id]
                }
                const res = await getMyplanDescription(data);
                setMyPlanDesc(res || '');
            } catch (error) {
                console.error(error);
            } finally {
                setLoader(false);
            }
        };

        fetchPlanDesc();
    }, []);
    
        const [devicesData, setDevicesData] = useState<any>([])
        console.log("devicesData---devicesData-->>",devicesData)
        const getDevices1 = async (emailId: string) => {
            try {
                setLoader(true)
                const res: any = await getDevices(emailId)
                if (res?.data?.statusCode === 200 && Array.isArray(res?.data?.result)) {
                    const mappedDevices = res.data.result.map((device: any) => ({
                        ...device,
                        OS_Type: device?.OS_Type || device?.os_type || "-",
                        IP_Address: device?.IP_Address || device?.ip_address || "-",
                        updated_On: device?.updated_On || device?.updated_on || null,
                    }));
                    setDevicesData(mappedDevices)
                } else {
                    setDevicesData([]);
                }
            } catch (error) {
                console.log("getDevices1 error:", error);
                setDevicesData([]);
            } finally {
                setLoader(false)
            }
        }
    useEffect(() => {
        if (!stateData?.Idx) return;
        const body: any = {
            userId: stateData?.Idx,
            roleId : loginUserDetails?.org_role_id_crm,
            limit:100,
            offset : 0
          };

        getUserLogActivity(body)
            .then((res: any) => {
                setLogData(Array.isArray(res) ? res : []);
            })
            .catch((err: any) => { console.log(err); setLogData([]); });
    }, [stateData?.Idx, loginUserDetails?.org_role_id_crm])

    const formatDateTime = (dateString: string) => {
        if (!dateString) return "--";
      
        const date = new Date(dateString);
      
        const formattedDate = date.toLocaleDateString("en-GB", {
          day: "2-digit",
          month: "short",
          year: "numeric",
        });
      
        const formattedTime = date.toLocaleTimeString("en-GB", {
          hour: "2-digit",
          minute: "2-digit",
          second: "2-digit",
          hour12: true,
        });
      
        return `${formattedDate}, ${formattedTime}`;
      };
    const fullName = `${profileData?.Firstname || ""} ${profileData?.Surname || ""}`.trim() || stateData?.username || "Not Applicable";
    const roleText = roleName || stateData?.RoleName || "Unknown";
    const queueNames = stateData?.queueName || "-";
    const reportingTo = stateData?.report_to_UserName || "-";
    const lastViewed = profileData?.lastlogin ? formatDateTime(profileData?.lastlogin) : "-";

    return (
        <>
            <div className={styles.yourinfo}>
                <div
                    className={`${styles.breadcrumscontainer} ${styles.maincontainer}`}
                >
                    <div className={styles.breadtitle} onClick={() => navigate('/myplans')}>Users</div>
                    <div className={`${styles.breadtitle} ${styles.active}`}>{fullName}</div>
                </div>
                <div className={styles.userdetailsmaincontainer}>
                    <div className={styles.profilenamelist}>
                        <div className={styles.profiletitle}>Welcome, {fullName}</div>
                        <div className={styles.profilesubtitle}>Manage the personal details used across all your Worktual applications</div>
                    </div>
                    <div className={styles.useractivitylist}>
                    <div className={`${styles.titlearea} ${styles.titlefl} ${styles.titleflrow}`}>
                        <div className={styles.profileleft}>{initials}</div>
                        <div className={styles.useraccright}>
                            <div className={styles.desc}>{loader ? <Skeleton count={1}/> : (myplanDesc || "No description available")}</div>
                        </div>
                    </div>
                    <div className={styles.workdetails}>
                          <div className={styles.workdetailstitle}><span>Full Name </span>{fullName}</div>
                    </div>
                    </div>
                </div>

                {/* Role Info Section */}
                <div className={styles.yourinfotop}>
                    <div className={styles.fieldareamain}>
                        <div className={styles.labelheader}>Work Details</div>
                    </div>

                    <div className={styles.fieldareamain}>
                        <div className={styles.label}>Modified</div>
                        <div className={styles.value}>{profileData?.modified_date ? formatDateTime(profileData?.modified_date) : "-"}</div>
                    </div>

                    <div className={styles.fieldareamain}>
                        <div className={styles.label}>Product</div>
                        <div className={styles.value}>{stateData?.product_name || "CRM"}</div>
                    </div>

                    <div className={styles.fieldareamain}>
                        <div className={styles.label}>Queue Names</div>
                        <div className={styles.value}>{queueNames}</div>
                    </div>

                    <div className={styles.fieldareamain}>
                        <div className={styles.label}>Reporting To</div>
                        <div className={styles.value}>{reportingTo}</div>
                    </div>

                    <div className={styles.fieldareamain}>
                        <div className={styles.label}>Last Viewed</div>
                        <div className={styles.value}>{lastViewed}</div>
                    </div>
                </div>

                   <div className={styles.yourinfotop}>
                    <div className={styles.fieldareamain}>
                        <div className={styles.labelheader}>Contact Information</div>
                    </div>

                    <div className={styles.fieldareamain}>
                        <div className={styles.label}>Contact Email</div>
                        <div className={styles.value}>{profileData?.Email || stateData?.Email || "-"}</div>
                    </div>

                    <div className={styles.fieldareamain}>
                        <div className={styles.label}>Contact Number</div>
                        <div className={styles.value}>{profileData?.Mobileno || "-"}</div>
                    </div>
                </div>
                <div className={styles.iconaccess}>
                    <div className={styles.accesspermisson}>
                        <div className={styles.accesspert}>
                            <img src={Activityloglist} alt="Activityloglist" />

                            <div className={styles.featuresa}>
                                <h3>Recent Activity</h3>
                                <p>Latest actions and events</p>
                            </div>
                        </div>

                        <div className={styles.featurenet} onClick={() => setShowAll((prev) => !prev)}>
                            {LogData?.length > 2 && <p>
                                {showAll ? "Hide Activity" : "View All Activity"}
                                <svg
                                    xmlns="http://www.w3.org/2000/svg"
                                    width="6"
                                    height="12"
                                    viewBox="0 0 6 12"
                                    fill="none"
                                >
                                    <path
                                        d="M0.749951 11.3309C0.557551 11.3309 0.365152 11.2577 0.218752 11.1102C-0.0737485 10.8172 -0.0727486 10.3426 0.220751 10.0497L4.08505 6.19718C4.22715 6.05558 4.30525 5.86608 4.30525 5.66498C4.30525 5.46478 4.22715 5.27628 4.08505 5.13468L0.220352 1.28118C-0.0726482 0.988176 -0.0735484 0.513576 0.218852 0.220676C0.510852 -0.0732242 0.985952 -0.0732245 1.27985 0.218675L5.14415 4.07218C5.56995 4.49698 5.80475 5.06238 5.80525 5.66498C5.80575 6.26758 5.57085 6.83298 5.14415 7.25968L1.27945 11.1122C1.13345 11.2577 0.941351 11.3309 0.749951 11.3309Z"
                                        fill="#6B46C1"
                                    />
                                </svg>
                            </p>}
                        </div>
                    </div>

                    <div className={styles.activityterms}>
                      {visibleLogs?.map((item: any, index: number) => (
                        <div className={styles.accesslogges} key={index}>
                            <div className={styles.chromeinput}>
                                <div className={`${styles.windowste} ${index === 0 ? styles.active : ""}`}>
                                    <h3>{item?.UserName || "-"}</h3>
                                    <p>{item?.message_log || "-"}</p>
                                </div>
                            </div>
                            <p className={styles.alarchr}>{formatDateTime(item?.createdAt)}</p>
                        </div>
                      ))}
                    </div>
                    <div className={styles.iconaccess}>
                <div className={styles.accesspermisson}>
                    <div className={styles.accesspert}>
                        <img src={Security} alt="Security" />
                        <div className={styles.featuresa}>
                            <h3>Security & Account</h3>
                            <p>Manage credentials and security settings</p>
                        </div>
                    </div>

                </div>
                <div className={styles.mfastatus}>
                    <div className={styles.enableteam}>
                            <h3>MFA Status</h3>
                            <p>{userPermission?.is_voice === 1 || userPermission?.is_chat === 1 ? "Enabled" : "Not set"}</p>
                        </div>
                        <div className={styles.enableteam}>
                            <h3>Last Password Reset</h3>
                            <p>{profileData?.last_password_changed ? formatDateTime(profileData?.last_password_changed) : "-"}</p>
                        </div>
                    </div>
                 <div className={styles.mfastatus}>
                    <div className={styles.enableteam}>
                            <h3>Authentication</h3>
                            <p>{loginUserDetails?.socialLoginType || "Password"}</p>
                        </div>
                        <div className={styles.enableteam}>
                            <h3>Account Security</h3>
                            <p>{profileData?.Status === 1 ? "Secure" : "Limited"}</p>
                        </div>
                    </div>
                    </div>
                     {devicesData?.length > 0 && <div className={styles.iconaccess}>
                            <div className={styles.accesspermisson}>
                                <div className={styles.accesspert}>
                                    <img src={Devicelist} alt="Devicelist" />
                                    <div className={styles.featuresa}>
                                        <h3>Active Devices</h3>
                                        <p>Logged in sessions during last 7 days</p>
                                    </div>
                                </div>

                            </div>

                            { devicesData.map((product: any, index: number) => (
                                    <div className={styles.ccaasnm} key={index}>
                                        <div className={styles.features}>
                                            <h3>{product?.OS_Type || '-'}</h3>
                                            <p>{product?.IP_Address || '-'} • Last active: {product?.updated_On ? moment(product?.updated_On).format('DD/MM/YYYY') : '-'}</p>
                                        </div>
                                        <button>Remove</button>
                                    </div>
                                ))}

                        </div>}
                </div>


            </div>
        </>
    );
}

export default Agentroleview
