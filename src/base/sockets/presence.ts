import { config } from "../constant";
import { isJSON } from "../utils";
import socketIOClient from "socket.io-client";
import jwt_decode from "jwt-decode";
import { GetItemFromStorage } from "../custom_hooks/useStorage";
import { LocalStorage } from "../custom_hooks/localStorageKeys";
import store from "../../store/redux-store/root_store";
import { agentTrackerActions } from "../../store/slice/trackerStatus_slice/agentTrackerslice";
import { GetAlluserActions } from "../../store/slice/users/get_all_users";
import { commonDataSliceActions } from "../../store/slice/commonSilce/commonSlice";
import { AgentStateAction } from "../../store/slice/agentStatusSlice/agent_state";
import { getAgentStatusDetails } from "../service/core_serviceApi";

export default class presenceService {
    public static io: any;
    static _instance: any;
    private static hist: any;
    constructor(token: any, navigate: any, dispatch: any, key: any) {
        let userData: any = jwt_decode(token);
        const domainId = userData?.domainId ?? userData?.domain_id;
        const ext = userData?.ext ?? userData?.sipLoginId ?? userData?.sip_login_id;
        const socketAuthId = `${domainId ?? ""}_${ext ?? ""}`;

        if (presenceService.io) {
            presenceService.io.disconnect();
        }

        presenceService.io = socketIOClient(config().BASE_URL, {
        // presenceService.io = socketIOClient("http://localhost:3000", {
            path: "/presence-service",
            auth: { token: token, id: socketAuthId },
            transports: ["websocket", "polling"],
            forceNew: true,
            reconnectionAttempts: 5,
            timeout: 60000,
        });

        let force_pre: any = key;
        let statusListPromise: Promise<any[]> | null = null;

        const ensureStatusList = async () => {
            const inStore = store.getState()?.agentState?.status;
            if (Array.isArray(inStore) && inStore.length > 0) return inStore;

            if (!statusListPromise) {
                statusListPromise = (async () => {
                    const apiStatuses = await getAgentStatusDetails();
                    const statusList = Array.isArray(apiStatuses) ? apiStatuses : [];
                    if (statusList.length > 0) {
                        dispatch(AgentStateAction.addStatusList(statusList));
                    }
                    return statusList;
                })();
            }

            try {
                return await statusListPromise;
            } finally {
                statusListPromise = null;
            }
        };

        const resolveStatusObject = async (statusName: string) => {
            const normalized = String(statusName ?? "").toLowerCase();
            const statusList = await ensureStatusList();
            const matched = statusList.find(
                (item: any) => String(item?.statusName ?? "").toLowerCase() === normalized
            );
            return matched ?? { statusName, label: statusName, value: normalized, colorCode: "#22C55E" };
        };

        presenceService.io.on('connect', () => {
            void ensureStatusList();
            const popUpDetails: any = store.getState().callPopUpDetails ?? {};
            const incomingCallDetail: any = popUpDetails?.incomingCallDetail ?? {};
            const LoginCrmUserDetails: any = GetItemFromStorage(LocalStorage.USER_DETAILS) ?? {};
            const currentPresence = sessionStorage.getItem("presence") ?? "Not Ready";
            const status: any = null;
            const enableStatus: any = null;
            let skill: any[] = [];
            try {
                skill = JSON.parse(enableStatus?.addSkill ?? "[]");
            } catch (error) {
                skill = [];
            }

            presenceService?.io?.emit("updatestate_backend", {
                domainId: domainId,
                isVoice: (enableStatus?.voice === 1 && status?.isVoice === 1 && (popUpDetails?.incomingCallDetail ? Object.keys(popUpDetails?.incomingCallDetail ?? {}).length : 0) === 0) ? 1 : 0,
                isVoiceCallTransfer: status?.isVoiceCallTransfer ?? 0,
                isChat: (enableStatus?.chat === 1 && status?.isChat === 1) ? 1 : 0,
                isChatTransfer: status?.isChatTransfer ?? 0,
                isEmail: (enableStatus?.email === 1 && status?.isEmail === 1) ? 1 : 0,
                isEmailTransfer: status?.isEmailTransfer ?? 0,
                stateTime: Math.floor(Date.now() / 1000),
                statusName: currentPresence,
                ext: ext,
                email: userData?.Email,
                agentStatus: Object.keys(incomingCallDetail)?.length > 0 ? 0 : 1,
                userId: LoginCrmUserDetails?.UserID,
                routingProfileId: LoginCrmUserDetails?.routing_profile_id,
                roleId: LoginCrmUserDetails?. org_role_id_crm,
                skill,
                routingMethod: enableStatus?.routingMethod ?? 0,
                inboundCall: LoginCrmUserDetails?.inboundCall || enableStatus?.inboundCall,
                outboundCampaignCall: enableStatus?.outboundCampaignCall,
                forced_presence: force_pre ?? 0,
                isCrm: 1
            });
            force_pre = 0;
        });

        presenceService.io.on("reconnect", () => {
            console.log("socket reconnect");
        });
        presenceService.io.on('disconnect', () => {
            console.log('socket disconnect')
        });

        presenceService.io.on("connect_error", (msg: any) => {
            if (msg && msg?.message && isJSON(msg?.message)) {
                const parsedMessage: any = JSON.parse(msg?.message);
                if (parsedMessage?.code === 403) {
                    console.warn("Presence socket unauthorized (403). Preserving login session.");
                    presenceService.destroy();
                }
            }
        });

        presenceService.io.on("updatedstatus", async (data: any) => {
            const LoginCrmUserDetails: any = GetItemFromStorage(LocalStorage.USER_DETAILS) ?? {};
            if (LoginCrmUserDetails?.ext?.toString() === data?.ext?.toString() && LoginCrmUserDetails?.domainId?.toString() === data?.domainId?.toString()) {
                dispatch(AgentStateAction.setCurrentStatus({ status: data?.statusName }));
            }
        });

        presenceService.io.on("userStatusUpdated", async (data: any) => {
            try {
                if (data?.email && data?.status) {
                    let setData = { email: data?.email ?? "", status: data?.status };
                    dispatch(agentTrackerActions.updateUserInAgentTracker(setData));
                    dispatch(GetAlluserActions.updateStatus(setData));
                    const nextStatus = data?.status ?? "Offline";
                    const matchedStatus = await resolveStatusObject(nextStatus);
                    dispatch(commonDataSliceActions.setAgentStatus(matchedStatus));
                }

            } catch (error) {
                console.log("userStatusUpdated error", error)
            }

        });
    }

    static async destroy() {
        presenceService?.io?.disconnect();
    }
}
