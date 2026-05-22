import jwt_decode from "jwt-decode";
import socketIOClient from "socket.io-client";
import { config } from "../base/constant";
import { AiOnboardingAction } from "../store/slice/onboardingSlice/onboardingSlice";
import { inboxDataSliceActions } from "../store/slice/inbox/inboxSlice";
import { getRegionCode } from "../base/utils";
import { getInboxRecords } from "../base/service/nlp_service";
import { insertUpdateAIChathistroyData } from "../base/service/core_serviceApi";
export default class inboxAiChatAssistSocketservice {
  public static io: any = null;
  private static _instance: inboxAiChatAssistSocketservice | null = null;

  constructor(token: any, navigate: any, dispatch: any) {

    if (inboxAiChatAssistSocketservice?._instance) {
      return inboxAiChatAssistSocketservice?._instance!;
    }
    const userData: any = jwt_decode(token);

    inboxAiChatAssistSocketservice.io = socketIOClient(`${config().NLP_INBOX_CHAT_SOCKET_URL}`, {
      path: "/crm_inbox_chat",
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
    inboxAiChatAssistSocketservice.io.on("connect", () => {
      console.log("Socket connected");
    });

    inboxAiChatAssistSocketservice.io.on("reconnect", () => {
      console.log("Socket reconnected");
    });

    inboxAiChatAssistSocketservice.io.on("disconnect", () => {
      console.log("Socket disconnected");
    });

    inboxAiChatAssistSocketservice.io.on("connect_error", (err: any) => {
      console.error("Socket connect error:", err?.message);
    });

    inboxAiChatAssistSocketservice.io.on("crm_typing", (data: any) => {
      const sessionId = data?.sessionId || "";
      const typingText = typeof data?.typing === "string" ? data.typing.trim() : "";

      if (data?.intent !== "typing" && !typingText) return;

      const payload = {
        sessionId,
        typing: typingText || true,
      };

      const agent = (data?.agent || "").toLowerCase();
      if (agent === "inbox" || sessionId.endsWith("inbox")) {
        dispatch(inboxDataSliceActions.setInboxAiChatTyping(payload));
      }
    });


    inboxAiChatAssistSocketservice.io.on("crm_inbox_chat_response", (data: any) => {

      if (data?.response?.trim() && data?.sessionId) {
        dispatch(inboxDataSliceActions.setInboxAichatMessage({ ...data, isBot: 1, message: data?.response }))
        dispatch(inboxDataSliceActions.setInboxAiChatTyping({ sessionId: data?.sessionId, typing: false }));
        if (data?.isSearch == 1) {
          // getInboxRecords({
          //   query: data?.sql,
          //   limit: 200,
          //   offset: 0
          // });
          const tab = data?.tab || 'Inbox';
          dispatch(inboxDataSliceActions.setInboxFilter({
            fromDate: data?.fromDate ?? null,
            toDate: data?.toDate ?? null,
            search: data?.search ?? null,
            tab: tab,
            channelType: data?.channelType || 'Email,Call',
            inboxType: data?.inboxType === 'all' ? null : data?.inboxType === 'sent' ? 0 : 1
          }));
          window.location.hash = tab;
        
        } else {
          const message = {
            sessionId: data?.sessionId,
            message: data.response ?? '',
            isBot: 1,
            messageId: Date.now(),
            draft_email: data?.draft_email?.body ? data?.draft_email : null
          }
          const payload = {
            sessionId: data?.sessionId ?? null,
            title: data?.title ?? null,
            agentId: userData?.userId ?? null,
            historyMessage: message ?? null,
          }
          insertUpdateAIChathistroyData(payload);

        }
      }
    });

    try {
      const socket = inboxAiChatAssistSocketservice.io;
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

    inboxAiChatAssistSocketservice._instance = this;
  }

  static async destroy() {
    if (inboxAiChatAssistSocketservice.io) {
      console.log("Destroying socket connection...");
      inboxAiChatAssistSocketservice.io.disconnect();
      inboxAiChatAssistSocketservice.io = null;
      inboxAiChatAssistSocketservice._instance = null;
    }
  }

  static async isConnected() {
    return !!(inboxAiChatAssistSocketservice.io && inboxAiChatAssistSocketservice.io.connected);
  }

  static getInstance(token: any, navigate: any, dispatch: any) {
    if (!inboxAiChatAssistSocketservice._instance) {
      inboxAiChatAssistSocketservice._instance = new inboxAiChatAssistSocketservice(token, navigate, dispatch);
    }
    return inboxAiChatAssistSocketservice._instance;
  }
}
