import jwt_decode from "jwt-decode";
import socketIOClient from "socket.io-client";
import { config } from "../base/constant";
import { AIAgentChatAction } from "../store/slice/aichatSlice/aichatSlice";
import { MarketingAgentChatAction } from "../store/slice/marketingSlice/marketingSlice";
import { contactDataSliceActions } from "../store/slice/contactSlice";
import { dealsDataSliceActions } from "../store/slice/dealsSlice";
import { AiOnboardingAction } from "../store/slice/onboardingSlice/onboardingSlice";
import { commonDataSliceActions } from "../store/slice/commonSilce/commonSlice";
import * as helper from "./helper";
import { getRegionCode } from "../base/utils";
import store from "../store/redux-store/root_store";
import { productDataSliceActions } from "../store/slice/productSlice";
import { SetupSliceActions } from "../store/slice/setupSlice/setupSlice";
export default class aiChatAssistSocketservice {
  public static io: any = null;
  private static _instance: aiChatAssistSocketservice | null = null;

  constructor(token: any, navigate: any, dispatch: any) {

    if (aiChatAssistSocketservice?._instance) {
      return aiChatAssistSocketservice?._instance!;
    }
    const userData: any = jwt_decode(token);

    aiChatAssistSocketservice.io = socketIOClient(`${config().NLP_CHAT_SOCKET_URL}`, {
      path: "/crm",
      auth: {
        token: token,
        domain_id: userData?.domain_id,
        ext: userData?.ext,
      },
      transports: ["websocket", "polling"],
      // reconnection: true,
      // reconnectionAttempts: Infinity,
      // reconnectionDelay: 1000,
      // reconnectionDelayMax: 5000,
      // timeout: 20000
      forceNew: true,
      reconnectionAttempts: 5,
      timeout: 60000,
    });

    // Event listeners
    aiChatAssistSocketservice.io.on("connect", () => {
      console.log("Socket connected");
    });

    aiChatAssistSocketservice.io.on("reconnect", () => {
      console.log("Socket reconnected");
    });

    aiChatAssistSocketservice.io.on("disconnect", () => {
      console.log("Socket disconnected");
    });

    aiChatAssistSocketservice.io.on("connect_error", (err: any) => {
      console.error("Socket connect error:", err?.message);
    });

    aiChatAssistSocketservice.io.on("crm_main_chat_response", (data: any) => {
      helper.crm_main_chat_response(dispatch, data, navigate);
    });
    
    aiChatAssistSocketservice.io.on("crm_typing", (data: any) => {
      const sessionId = data?.sessionId || "";
      const typingText = typeof data?.typing === "string" ? data.typing.trim() : "";
    
      if (data?.intent !== "typing" && !typingText) return;
    
      const payload = {
        sessionId,
        typing: typingText || true,
      };
    
      const agent = (data?.agent || "").toLowerCase();
    
      if (agent === "marketing" || sessionId.endsWith("marketing")) {
        dispatch(MarketingAgentChatAction.setMarketingAgentChatTyping(payload));
      } else if (agent === "contact" || sessionId.endsWith("contact")) {
        dispatch(contactDataSliceActions.setcontactAiAgentChatTyping(payload));
        const contactModuleFollowUp = store.getState()?.contactStore?.contactModuleFollowUp || {};
        const moduleForSession = Object.keys(contactModuleFollowUp).find(
          (key) => contactModuleFollowUp[key]?.sessionId === sessionId
        );
        if (moduleForSession) {
          dispatch(contactDataSliceActions.setContactModuleFollowUp({
            module: moduleForSession,
            response: payload?.typing,
            isWaitingResponse: false
          }))
        }

      } else if (agent === "onboarding" || sessionId.endsWith("onboarding")) {
        dispatch(AiOnboardingAction.setOnboardTyping(payload));
      } else if (agent === "setup" || sessionId.endsWith("setup")) {
        dispatch(SetupSliceActions.setSetupAiAgentChatTyping(payload));
      } else if (agent === "products" || sessionId.endsWith("products")) {
        dispatch(productDataSliceActions.setproductAiAgentChatTyping(payload));
      }
    });

    aiChatAssistSocketservice.io.on("settingsCompletion", (data: any) => {
      if (Array.isArray(data) && data.length > 0) {
        dispatch(AiOnboardingAction.updateOnBoardingStatus(data));
      }
    });

    try {
      const socket = aiChatAssistSocketservice.io;
      const originalEmit = socket.emit;
      socket.emit = function (event: string, ...args: any[]) {
        try {
          const updatedArgs = args.map((arg) => {
            if (typeof arg === "object" && arg !== null) {
              return {
                ...arg,
                country: getRegionCode() || 'in',
              };
            }
            return arg;
          });

          return originalEmit.call(this, event, ...updatedArgs);
        } catch (err) {
          console.error("Emit override error:", err);
          return originalEmit.call(this, event, ...args);
        }
      };
    } catch (err) {
      console.error("Socket patch error:", err);
    }

    aiChatAssistSocketservice._instance = this;
  }

  static async destroy() {
    if (aiChatAssistSocketservice.io) {
      console.log("Destroying socket connection...");
      aiChatAssistSocketservice.io.disconnect();
      aiChatAssistSocketservice.io = null;
      aiChatAssistSocketservice._instance = null;
    }
  }

  static isConnected() {
    return !!(aiChatAssistSocketservice.io && aiChatAssistSocketservice.io.connected);
  }

  static getInstance(token: any, navigate: any, dispatch: any) {
    if (!aiChatAssistSocketservice._instance) {
      aiChatAssistSocketservice._instance = new aiChatAssistSocketservice(token, navigate, dispatch);
    }
    return aiChatAssistSocketservice._instance;
  }
}
