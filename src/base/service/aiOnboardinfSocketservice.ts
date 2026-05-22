import jwt_decode from "jwt-decode";
import socketIOClient from "socket.io-client";
import { config } from "../../base/constant";
import { AiSettingsAction } from "../../store/slice/my_plans/settings";

export default class aiOnboardinfSocketservice {
  public static io: any = null;
  private static _instance: aiOnboardinfSocketservice | null = null;

  constructor(token: any, navigate: any, dispatch: any) {

    if (aiOnboardinfSocketservice.io && aiOnboardinfSocketservice.io.connected) {
      console.log("Socket already connected. Reusing existing connection.");
      return aiOnboardinfSocketservice._instance!;
    }
    const userData: any = jwt_decode(token);

    aiOnboardinfSocketservice.io = socketIOClient(`${config().NLP_CHAT_SOCKET_URL}`, {
      path: "/aiccaas_service",
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
    aiOnboardinfSocketservice.io.on("connect", () => {
      console.log("Socket connected");
    });

    aiOnboardinfSocketservice.io.on("reconnect", () => {
      console.log("Socket reconnected");
    });

    aiOnboardinfSocketservice.io.on("disconnect", () => {
      console.log("Socket disconnected");
    });

    aiOnboardinfSocketservice.io.on("connect_error", (err: any) => {
      console.error("Socket connect error:", err?.message);
    });

    aiOnboardinfSocketservice.io.on("setUpChat", (data: any) => {

      // if (data?.response?.trim()) {
        if (data?.queryFrom === 'Home') {
          dispatch(
            AiSettingsAction.setAiAnalyticsMessages({
              isBot: 1,
              message: data.response,
              sentAt: Date.now(),
              image_path: Array.isArray(data.image_path) ? data.image_path : [],
              chatdata: data?.chatdata ?? null,
              sessionId: data?.sessionId,
              tags: data?.tags,
              channel: data?.channel ?? "",
            })
          );
          dispatch(AiSettingsAction.setaiAnalyticsTyping(false))

        } else {
          if (data?.key) {
            dispatch(AiSettingsAction.setOnboardingActiveKey(data.key));
          }
          dispatch(
            AiSettingsAction.updateOnBoardingMsg({
              isBot: 1,
              message: data?.response,
              sentAt: Date.now(),
              channel: data?.channel ?? "",
            })
          );
        }
      // }
    });

    aiOnboardinfSocketservice.io.on("settingsCompletion", (data: any) => {
      if (Array.isArray(data) && data.length > 0) {
        dispatch(AiSettingsAction.updateOnBoardingStatus(data));
      }
    });

    aiOnboardinfSocketservice._instance = this;
  }

  static async destroy() {
    if (aiOnboardinfSocketservice.io) {
      console.log("Destroying socket connection...");
      aiOnboardinfSocketservice.io.disconnect();
      aiOnboardinfSocketservice.io = null;
      aiOnboardinfSocketservice._instance = null;
    }
  }

  static async isConnected() {
    return !!(aiOnboardinfSocketservice.io && aiOnboardinfSocketservice.io.connected);
  }

  static getInstance(token: any, navigate: any, dispatch: any) {
    if (!aiOnboardinfSocketservice._instance) {
      aiOnboardinfSocketservice._instance = new aiOnboardinfSocketservice(token, navigate, dispatch);
    }
    return aiOnboardinfSocketservice._instance;
  }
}
