import jwt_decode from "jwt-decode";
import socketIOClient from "socket.io-client";
import { config } from "../../base/constant";
import { MyplanProfileActions } from "../../store/slice/my_plans/profile";


export default class aiMyPlanSocketService {
  public static io: any = null;
  private static _instance: aiMyPlanSocketService | null = null;

  constructor(token: any, navigate: any, dispatch: any) {

    if (aiMyPlanSocketService.io && aiMyPlanSocketService.io.connected) {
      console.log("Socket already connected. Reusing existing connection.");
      return aiMyPlanSocketService._instance!;
    }
    const userData: any = jwt_decode(token);
    aiMyPlanSocketService.io = socketIOClient(`${config().NLP_MYPLAN_SOCKET_URL}`, {
    // aiMyPlanSocketService.io = socketIOClient(`https://nlpccaas.worktual.co.uk`, {
        // http://173.234.75.165:8069/
    // aiMyPlanSocketService.io = socketIOClient(`http://192.168.13.252:8061/`,  {
      path: "/myplan_service",
      auth: {
        token: token,
        domain_id: userData?.domain_id,
        ext: userData?.ext,
      },
      transports: ["websocket", "polling"],
      reconnection: true,
      reconnectionAttempts: Infinity,
      reconnectionDelay: 1000,
      reconnectionDelayMax: 5000,
      timeout: 20000
    });

    // Event listeners
    aiMyPlanSocketService.io.on("connect", () => {
      console.log("Socket connected");
      // aiMyPlanSocketService.io.emit("agent_chat_request", "go to yourinfo")
    });

    aiMyPlanSocketService.io.on("reconnect", () => {
      console.log("Socket reconnected");
    });

    aiMyPlanSocketService.io.on("disconnect", () => {
      console.log("Socket disconnected");
    });

    aiMyPlanSocketService.io.on("connect_error", (err: any) => {
      console.error("Socket connect error:", err?.message);
    });

    aiMyPlanSocketService.io.on("setUpChat", (data: any) => {
        console.log("dataresp",data)
            const routes :any= {
                your_info: '/yourinfo',
                security: '/security',
                data_and_privacy: '/privacy',
                devices: '/devices',
                role: '/roles',
                users: '/user',
                teams: '/teams',
                user_activity_logs: '/activitylog',
                order_history: '/Orderhistory',
                add_ons: '/addon',
                purchases: '/renewal',
                home:'/home'
            };

            const path :any= routes[data?.intent];
            if (path) navigate(path);
           

            if (data?.response?.trim()) {
                dispatch(MyplanProfileActions.setMyplanBotResponse({
                  loading: false,
                  response: data?.response?.trim()
                }));
            }

            if(data?.update){
              dispatch(MyplanProfileActions.setMyPlanUpdation(true));
            }
        });

    aiMyPlanSocketService._instance = this;
  }

  static async destroy() {
    if (aiMyPlanSocketService.io) {
      console.log("Destroying socket connection...");
      aiMyPlanSocketService.io.disconnect();
      aiMyPlanSocketService.io = null;
      aiMyPlanSocketService._instance = null;
    }
  }

  static async isConnected() {
    return !!(aiMyPlanSocketService.io && aiMyPlanSocketService.io.connected);
  }

  static getInstance(token: any, navigate: any, dispatch: any) {
    if (!aiMyPlanSocketService._instance) {
      aiMyPlanSocketService._instance = new aiMyPlanSocketService(token, navigate, dispatch);
    }
    return aiMyPlanSocketService._instance;
  }
}
