import styles from "../../assets/styles/agentroleview.module.scss";
import Memberslist from '../../assets/images/myplan/memberslist.svg'
import { useEffect, useState } from "react";
import { getAllMembersRole, getRolePermission, getUserDetails, insertUpdateRolePermission } from "../../apiservice/myplanapi";
import { MyplanProfileActions } from "../../../../store/slice/my_plans/profile";
import moment from "moment";
import { GetItemFromStorage } from "../../../../base/custom_hooks/useStorage";
import { LocalStorage } from "../../../../base/custom_hooks/localStorageKeys";
import { useDispatch, useSelector } from "react-redux";
import { useNavigate, useSearchParams } from "react-router-dom";
import jwt_decode from 'jwt-decode'

const Roleteam = () => {
    const navigate = useNavigate();
    const [searchParams] = useSearchParams();
    const roleId: any = searchParams.get("roleId");
    const [roleInfo, setRoleInfo] = useState<any>({});
    const [viewAll, setViewAll] = useState(false);
    const myPlanUpdation: any = useSelector(
        (state: any) =>
            state?.myPlanStore?.myPlanUpdation ??
            state?.userExtension?.myPlanUpdation ??
            false
    );
    const dispatch = useDispatch()
    const [userInfo, setUserInfo] = useState({ queueNames: "", reportToNames: "" });
    const [rolePermission, setRolePermission] = useState({
        is_agent_monitor: 0,
        is_queue_management: 0,
        is_reporting_dashboard: 0,
        is_knowledge_base: 0,
    });
    const accessToken = GetItemFromStorage(LocalStorage?.ACCESS_TOKEN);
    const LoginUserDetails: any = accessToken && jwt_decode(accessToken);

    const getParticularRole = (roleId: any) => {
        if (!roleId) {
            return;
        }
        try {
            const body = { roleId: Number(roleId) };
            getAllMembersRole(body)
                .then((res: any) => {
                    if (res) {
                        const firstCreated = res
                            .map((item: any) => new Date(item.created_date))
                            .sort((a: any, b: any) => a - b)[0];

                        const lastModified = res
                            .map((item: any) => item.modified_date ? new Date(item.modified_date) : item.last_viewed ? new Date(item.last_viewed) : null)
                            .filter(Boolean)
                            .sort((a: any, b: any) => b - a)[0];
                        const roleData = {
                            members: res,
                            firstCreated: moment(firstCreated).format("MMM DD, YYYY"),
                            lastModified: moment(lastModified).format("MMM DD, YYYY"),
                            lastViewed: moment(lastModified).format("MMM DD, YYYY")
                        };
                        setRoleInfo(roleData);
                    } else {
                        setRoleInfo({})
                    }
                    dispatch(MyplanProfileActions.setMyPlanUpdation(false));
                })
                .catch((err: any) => {
                    setRoleInfo({})
                    console.log(err)
                }
                );

        } catch (error) {
            console.log("getParticularRole Error", error);
        }
    };

    const getUserDetail = (roleId: any) => {
        try {
            const body = { roleId: Number(roleId) };
            getUserDetails(body)
                .then((res: any) => {
                    if (Array.isArray(res)) {
                        setUserInfo({
                            queueNames: Array.from(new Set(res?.map(data => data?.queueName))).join(", "),
                            reportToNames: Array.from(new Set(res?.map(data => data?.report_to_UserName))).join(", ")
                        });
                    }
                    dispatch(MyplanProfileActions.setMyPlanUpdation(false));
                })
                .catch((err: any) => {
                    console.log(err)
                }
                );
        } catch (error) {
            console.log("getUserDetail Error", error);
        }
    }

    const handleToggleChange = (key: string, value: boolean) => {
        const updatedValue = value ? 1 : 0;

        const updatedState = {
            ...rolePermission,
            [key]: updatedValue,
        };
        setRolePermission(updatedState);
        insertUpdatePermission(updatedState);
    };

    const insertUpdatePermission = async (data: any) => {
        try {
            const body = {
                roleId: roleId,
                is_agent_monitor: data?.is_agent_monitor,
                is_queue_management: data?.is_queue_management,
                is_reporting_dashboard: data?.is_reporting_dashboard,
                is_knowledge_base: data?.is_knowledge_base,
            };

            await insertUpdateRolePermission(body);

            setRolePermission({
                is_agent_monitor: body?.is_agent_monitor,
                is_queue_management: body?.is_queue_management,
                is_reporting_dashboard: body?.is_reporting_dashboard,
                is_knowledge_base: body?.is_knowledge_base,
            });

        } catch (error) {
            console.log("insertUpdatePermission Error:", error);
        }
    }

    const getRolePermissionData = async () => {
        try {
            const response = await getRolePermission();

            if (response) {
                setRolePermission({
                    is_agent_monitor: response?.is_agent_monitor,
                    is_queue_management: response?.is_queue_management,
                    is_reporting_dashboard: response?.is_reporting_dashboard,
                    is_knowledge_base: response?.is_knowledge_base,
                });
            }
        } catch (err) {
            console.log("getRolePermissionData GET Error:", err);
        }
    };

    useEffect(() => {
        if (roleId) {
            getParticularRole(roleId);
            getUserDetail(roleId)
        }
    }, [roleId, myPlanUpdation]);

    // useEffect(() => {
    //     getRolePermissionData();
    // }, [])

    const roleMap: any = {
        1: "Owner",
        2: "Sub-Owner",
        3: "Admin",
        4: "Member",
        5: "Supervisor",
        6: "Agent",
        7: "Consultant",
        8: "Executive",
    };

    const roleName = roleMap[roleId] || "Unknown";
    console.log('sadasdaroleName>', roleName)


    const visibleMembers = viewAll
        ? roleInfo?.members
        : roleInfo?.members?.slice(0, 2);

    const domainProducts: Record<string, string> = {
        "4": "AI Contact Center",
        "5": "Campaign Management",
        "6": "CRM",
    };
    const productName = "CRM";

    return (
        <>
            <div className={styles.yourinfo}>
                <div
                    className={`${styles.breadcrumscontainer} ${styles.maincontainer}`}
                >
                    <div className={styles.breadtitle} onClick={() => navigate("/myplans")}>Roles</div>
                    <div className={`${styles.breadtitle} ${styles.active}`}>{roleName}</div>
                </div>
                <div className={styles.userdetailsmaincontainer}>
                    <div className={`${styles.titlearea} ${styles.titlefl}`}>
                        <div className={styles.profileleft}>{roleName?.charAt(0).toUpperCase()}</div>
                        <div className={styles.useraccright}>
                            <div className={styles.title}>{roleName}</div>
                            <div className={styles.desc}>
                                Manage the personal details used across all your applications
                            </div>
                        </div>
                    </div>
                    <div className={styles.createdmodify}>
                        <div className={styles.createdaccount}>
                            <div className={styles.createtitle}>Created</div>
                            <div className={styles.createsubtitle}>{roleInfo?.firstCreated || '-'}</div>
                        </div>
                        <div className={styles.createdaccount}>
                            <div className={styles.createtitle}>Modified</div>
                            <div className={styles.createsubtitle}>{roleInfo?.lastModified || '-'}</div>
                        </div>
                        <div className={styles.createdaccount}>
                            <div className={styles.createtitle}>Product</div>
                            <div className={styles.createsubtitle}>{productName}</div>
                        </div>
                    </div>
                </div>

                {/* Role Info Section */}
                <div className={styles.yourinfotop}>
                    <div className={styles.fieldareamain}>
                        <div className={styles.labelheader}>Role Information</div>
                    </div>

                    <div className={styles.fieldareamain}>
                        <div className={styles.label}>Role Category</div>
                        <div className={styles.value}>{roleName}</div>
                    </div>

                    <div className={styles.fieldareamain}>
                        <div className={styles.label}>Queue names</div>
                        <div className={styles.value}>{userInfo?.queueNames || '-'}</div>
                    </div>

                    {String(roleId) !== "3" && <div className={styles.fieldareamain}>
                        <div className={styles.label}>Reporting To</div>
                        <div className={styles.value}>{userInfo?.reportToNames || '-'}</div>
                    </div>}

                    <div className={styles.fieldareamain}>
                        <div className={styles.label}>Last viewed</div>
                        <div className={styles.value}>{roleInfo?.lastViewed || '-'}</div>
                    </div>

                    <div className={styles.fieldareamain}>
                        <div className={styles.label}>Team</div>
                        <div className={styles.value}>-</div>
                    </div>
                </div>

                {/* Members Section */}
                <div className={styles.accesspermisson}>
                    <div className={styles.accesspert}>
                        <img src={Memberslist} alt="Memberslist" />
                        <div className={styles.featuresa}>
                            <h3>Members with this Role</h3>
                            <p>This role contributes to ongoing operations.</p>
                        </div>
                    </div>
                    {Array.isArray(roleInfo?.members) && roleInfo.members.length > 2 && (
                        <div className={styles.featurenet}>
                            <p onClick={() => setViewAll(!viewAll)}>
                                {viewAll ? "Show Less" : "View All Members"}
                                <svg xmlns="http://www.w3.org/2000/svg" width="6" height="12" viewBox="0 0 6 12" fill="none">
                                    <path d="M0.749951 11.3309C0.557551 11.3309 0.365152 11.2577 0.218752 11.1102C-0.0737485 10.8172 -0.0727486 10.3426 0.220751 10.0497L4.08505 6.19718C4.22715 6.05558 4.30525 5.86608 4.30525 5.66498C4.30525 5.46478 4.22715 5.27628 4.08505 5.13468L0.220352 1.28118C-0.0726482 0.988176 -0.0735484 0.513576 0.218852 0.220676C0.510852 -0.0732242 0.985952 -0.0732245 1.27985 0.218675L5.14415 4.07218C5.56995 4.49698 5.80475 5.06238 5.80525 5.66498C5.80575 6.26758 5.57085 6.83298 5.14415 7.25968L1.27945 11.1122C1.13345 11.2577 0.941351 11.3309 0.749951 11.3309Z" fill="#6B46C1" />
                                </svg>
                            </p>
                        </div>
                    )}
                </div>

                {Array.isArray(visibleMembers) && visibleMembers.length > 0
                    ? visibleMembers.map((item: any, id: number) => (
                        <div key={id} className={styles.accesspermisson}>
                            <div className={styles.accesspert}>
                                <span className={styles.profileic}>
                                    {item?.Name ? item.Name.charAt(0).toUpperCase() : "?"}
                                </span>
                                <div className={styles.featuresa}>
                                    <h3>{item?.Name || "-"}</h3>
                                    <p>{item?.RoleName || roleName}</p>
                                </div>
                            </div>
                            <div className={styles.featurenet}>
                                <div className={styles.assignedactive}>
                                    <p>Assigned</p>
                                    <p>{item?.created_date ? moment(item.created_date).format("MMM DD, YYYY") : "-"}</p>
                                </div>
                                <span>{item?.Status || "-"}</span>
                            </div>
                        </div>
                    ))
                    : <p>No members available</p>
                }

            </div>
        </>
    );
}

export default Roleteam
