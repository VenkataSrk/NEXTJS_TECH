import styles from "../../assets/styles/agentroleview.module.scss";
import Memberslist from '../../assets/images/myplan/memberslist.svg'
import Activitylog from '../../assets/images/myplan/activitylog.svg'
import { getTeamDetails, getTeamLogActivity, getTeamPermission, insertUpdateTeamPermission } from "../../apiservice/myplanapi";
import { useEffect, useState } from "react";
import { LocalStorage } from "../../../../base/custom_hooks/localStorageKeys";
import { GetItemFromStorage } from "../../../../base/custom_hooks/useStorage";
import { useLocation, useNavigate, useSearchParams } from "react-router-dom";
import jwt_decode from 'jwt-decode'
import { getInitials } from "../../../../base/utils";

const Roleagent = () => {
    const location = useLocation();
    const [showAllMembers, setShowAllMembers] = useState(false);
    const [showAll, setShowAll] = useState(false);
    const [LogData, setLogData] = useState<any[]>([]);

    const state = location.state ?? {};
 
    // const qId = state.qId;
    // const teamName = state.teamName;
    // const teamMeta = state[0]?.[0];     // first query result
    // const teamMembers = state[1] ?? [];

    const [teamMeta, setteamMeta] = useState<any>({});
    const [teamMembers, setteamMembers] = useState<any[]>([]);

    const [params] = useSearchParams()
    const qId = params.get('queueId');
    const teamName = params.get('teamName');

    useEffect(()=>{
        if (!qId) {
            setteamMeta({});
            setteamMembers([]);
            return;
        }
        const body = { qId: Number(qId) };
        getTeamDetails(body)
        .then((res: any) => {
            setteamMeta(res?.[0]?.[0] ?? {});
            setteamMembers(res[1] ?? []);
        })
        .catch((err: any) => {
          console.log(err);
        });
    },[qId])


    const teamMembersCount = teamMembers.length;
    const visibleMembers = showAllMembers
        ? teamMembers
        : teamMembers.slice(0, 2);
    const visibleLogs = showAll ? LogData : LogData.slice(0, 2);
  const navigate = useNavigate()
    const accessToken = GetItemFromStorage(LocalStorage?.ACCESS_TOKEN);
    const LoginUserDetails: any = accessToken && jwt_decode(accessToken);

    const [rolePermission, setRolePermission] = useState({
        is_agent_monitor: 0,
        is_addOrRemove_queue: 0,
        is_reporting_dashboard: 0,
    });
    useEffect(() => {
        if (!qId) return;
        getRolePermissionData();
        const body = {
            qId: qId
        }

        getTeamLogActivity(body)
            .then((res: any) => {
                setLogData(Array.isArray(res) ? res : []);
            })
            .catch((err: any) => { console.log(err); setLogData([]); });
    }, [qId])
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
                qId: qId,
                is_agent_monitor: data?.is_agent_monitor,
                is_addOrRemove_queue: data?.is_addOrRemove_queue,
                is_reporting_dashboard: data?.is_reporting_dashboard,
            };

            await insertUpdateTeamPermission(body);

            setRolePermission({
                is_agent_monitor: body?.is_agent_monitor,
                is_addOrRemove_queue: body?.is_addOrRemove_queue,
                is_reporting_dashboard: body?.is_reporting_dashboard,
            });

        } catch (error) {
            console.log("insertUpdatePermission Error:", error);
        }
    }
    const getRolePermissionData = async () => {
        try {
            const body = {
                qId: qId
            }
            const response = await getTeamPermission(body);
            if (response) {
                setRolePermission({
                    is_agent_monitor: response?.[0]?.agentMonitoring,
                    is_addOrRemove_queue: response?.[0]?.addOrRemove,
                    is_reporting_dashboard: response?.[0]?.reportingDashboard,
                });
            }
        } catch (err) {
            console.log("getRolePermissionData GET Error:", err);
        }
    };
    const formatDate = (dateString: string) => {
        if (!dateString) return "";
        const date = new Date(dateString);
        return date.toLocaleString("en-GB", {
            day: "2-digit",
            month: "short",
            year: "numeric",
            hour: "numeric",
            minute: "2-digit",
            hour12: true,
        });
    };

    const formatTime = (dateString: string) => {
        const date = new Date(dateString);

        return date.toLocaleTimeString("en-IN", {
            hour: "2-digit",
            minute: "2-digit",
            hour12: true,
        });
    };
    
    const domainProducts: Record<string, string> = {
        "4": "AI Contact Center",
        "5": "Campaign Management",
        "6": "CRM",
    };
    const productName =  "CRM";
    return (
        <>
            <div className={styles.yourinfo}>
                <div
                    className={`${styles.breadcrumscontainer} ${styles.maincontainer}`}
                >
                    <div className={styles.breadtitle} onClick={() => navigate('/myplans')}>Teams</div>
                    <div className={`${styles.breadtitle} ${styles.active}`}>{teamName || "-"}</div>
                </div>
                <div className={styles.rolecontainerlist}>
                    <div className={`${styles.userdetailsmaincontainer} ${styles.adminlistborder}`}>
                        <div className={`${styles.titlearea} ${styles.titlefl}`}>
                            <div className={styles.profileleft}>{getInitials(teamName || "-")}</div>
                            <div className={styles.useraccright}>
                                <div className={styles.title}>{teamName || "-"}</div>
                                <div className={styles.desc}>
                                    Manage the personal details used across all your applications
                                </div>
                            </div>
                        </div>
                        <div className={styles.createdmodify}>
                            <div className={styles.createdaccount}>
                                <div className={styles.createtitle}>Created</div>
                                <div className={styles.createsubtitle}>{formatDate(teamMeta?.createdAt)}</div>
                            </div>
                            <div className={styles.createdaccount}>
                                <div className={styles.createtitle}>Modified</div>
                                <div className={styles.createsubtitle}>{formatDate(teamMeta?.updatedAt)}</div>
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
                            <div className={styles.labelheader}>Team Information</div>
                        </div>

                        <div className={styles.fieldareamain}>
                            <div className={styles.label}>Modified</div>
                            <div className={styles.value}>{formatDate(teamMeta?.updatedAt)}</div>
                        </div>

                        <div className={styles.fieldareamain}>
                            <div className={styles.label}>Product</div>
                            <div className={styles.value}>{productName}</div>
                        </div>

                        <div className={styles.fieldareamain}>
                            <div className={styles.label}>Queue Names</div>
                            <div className={styles.value}>{teamName || "-"}</div>
                        </div>

                        <div className={styles.fieldareamain}>
                            <div className={styles.label}>Reporting To</div>
                            <div className={styles.value}>{teamMeta?.reporting_user_name ?? '-'}</div>
                        </div>

                        <div className={styles.fieldareamain}>
                            <div className={styles.label}>Last Viewed</div>
                            <div className={styles.value}>{formatDate(teamMeta?.updatedAt)}</div>
                        </div>
                    </div>

                    {/* Members Section */}
                    <div>
                        <div className={styles.accesspermisson}>
                            <div className={styles.accesspert}>
                                <img src={Memberslist} alt="Memberslist" />
                                <div className={styles.featuresa}>
                                    <h3>Members with this Role</h3>
                                    <p>This role contributes to ongoing operations.</p>
                                </div>
                            </div>
                            <div className={styles.featurenet} onClick={() => setShowAllMembers(prev => !prev)}>
                                {teamMembers?.length > 2 && (
                                    <p>
                                        {showAllMembers ? "Show Less" : "View All Members"}
                                        <svg xmlns="http://www.w3.org/2000/svg" width="6" height="12" viewBox="0 0 6 12" fill="none">
                                            <path d="M0.749951 11.3309C0.557551 11.3309 0.365152 11.2577 0.218752 11.1102C-0.0737485 10.8172 -0.0727486 10.3426 0.220751 10.0497L4.08505 6.19718C4.22715 6.05558 4.30525 5.86608 4.30525 5.66498C4.30525 5.46478 4.22715 5.27628 4.08505 5.13468L0.220352 1.28118C-0.0726482 0.988176 -0.0735484 0.513576 0.218852 0.220676C0.510852 -0.0732242 0.985952 -0.0732245 1.27985 0.218675L5.14415 4.07218C5.56995 4.49698 5.80475 5.06238 5.80525 5.66498C5.80575 6.26758 5.57085 6.83298 5.14415 7.25968L1.27945 11.1122C1.13345 11.2577 0.941351 11.3309 0.749951 11.3309Z" fill="#6B46C1" />
                                        </svg>
                                    </p>
                                )}
                            </div>
                        </div>

                        <div>
                            {visibleMembers?.map((member: any, index: number) => (
                                <div className={styles.accesspermisson} key={index}>
                                    <div className={styles.accesspert}>
                                        <span className={styles.profileic}>{getInitials(member?.UserName || "-")}</span>
                                        <div className={styles.featuresa}>
                                            <h3>{member?.UserName || "-"}</h3>
                                            <p>{formatDate(member?.updatedAt)}</p>
                                        </div>
                                    </div>
                                    <div className={styles.featurenet}>
                                        <div className={styles.assignedactive}>
                                            <p>Assigned</p>
                                            <p>{formatDate(member?.createdAt || member?.updatedAt)}</p>
                                        </div>
                                        <span>{member?.user_active_status === 1 ? "Active" : "Inactive"}</span>
                                    </div>
                                </div>
                            ))}
                        </div>
                    </div>
                    <div className={styles.iconaccess}>
                        <div className={styles.accesspermisson}>
                            <div className={styles.accesspert}>
                                <img src={Activitylog} alt="Activitylog" />

                                <div className={styles.featuresa}>
                                    <h3>Activity Log</h3>
                                    <p>Recent team configuration changes</p>
                                </div>
                            </div>

                            <div className={styles.featurenet} onClick={() => setShowAll(prev => !prev)}>
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
                            {visibleLogs.map((item: any, index: any) => {
                                return (
                                    <div className={styles.accesslogges} key={index}>
                                        <div className={styles.chromeinput}>
                                            <div className={styles.windowste}>
                                                <h3>{item?.UserName || "-"}</h3>
                                                <p>{item?.errmsg || item?.message_log || "-"}</p>
                                            </div>
                                        </div>
                                        <p className={styles.alarchr}>{formatTime(item?.login_date || item?.createdAt)}</p>
                                    </div>
                                )
                            })}
                        </div>
                    </div>
                </div>

            </div>
        </>
    );
}

export default Roleagent
