
import * as Helper from "../constants/reducer_names";
import commonSlice from "../slice/commonSilce/commonSlice";
import LoginUserReducer from "../slice/loginSlice/loginSlice"
import AichatSliceReducer from "../slice/aichatSlice/aichatSlice"
import callPopUpAgnet from "../slice/callSlice/call_popup_agent"
import callUserAgnetDetails from "../slice/callSlice/login_user_call_details"
import ChatReducer from "../slice/chat_slice/chat_slice"
import AgentStateReducer from "../slice/agentStatusSlice/agent_state"
import UserList from "..//slice/users/get_all_users"
import MarketingchatSliceReducer from "../slice/marketingSlice/marketingSlice"
import InboxchatSliceReducer from "../slice/inbox/inboxSlice"
import ContactSliceReducer from "../slice/contactSlice/index"
import DealsSliceReducer from "../slice/dealsSlice/index"
import onboardingSliceReducer from "../slice/onboardingSlice/onboardingSlice"
import setupSliceReducer from "../slice/setupSlice/setupSlice"
import homeSliceReducer from "../slice/homeSlice/index"
import projectSliceReducer from "../slice/projectSlice/index"
import productSliceReducer from "../slice/productSlice/index"
import userExtensionSlice from "../slice/my_plans/profile"
import userSettingsSlice from "../slice/settings_slice/user_settings_slice"
import AISettings from "../slice/my_plans/settings"



export const commonSliceReducerWrapper = (state: any, action: any) => {
  if (action.type === `${Helper.COMMON_DATA}/reset`) {
    state = undefined;
  }
  return commonSlice(state, action);
};

export const LoginUserReducerWrapper = (state: any, action: any) => {
  if (action.type === `${Helper.LOGIN_USER}/reset`) {
    state = undefined;
  }
  return LoginUserReducer(state, action);
};

export const AiChatReducerWrapper = (state: any, action: any) => {
  if (action.type === `${Helper.AI_AGENT}/reset`) {
    state = undefined;
  }
  return AichatSliceReducer(state, action);
};

export const CallPOpUpAgnetModelWrapper = (state: any, action: any) => {
  if (action.type === `${Helper.POPUP_CALLS}/reset`) {
    state = undefined;
  }
  return callPopUpAgnet(state, action);
};

export const CallUserAgentReducerWrapper = (state: any, action: any) => {
  if (action.type === `${Helper.CALL_USER_AGENT_REDUCER}/reset`) {
    state = undefined;
  }

  return callUserAgnetDetails(state, action);
};

export const ChatReducerWrapper = (state: any, action: any) => {
  if (action.type === `${Helper.CHAT_REDUCER}/reset`) {
    state = undefined;
  }
  return ChatReducer(state, action);
};

export const agentstateReducerWrapper = (state: any, action: any) => {
  if (action.type === `${Helper.AGENT_STATE}/reset`) {
    state = undefined;
  }
  return AgentStateReducer(state, action);
};

export const UserListReducerWrapper = (state: any, action: any) => {
  if (action.type === `${Helper.ALL_USER_LIST}/reset`) {
    state = undefined;
  }
  return UserList(state, action);
};

export const MarketingChatReducerWrapper = (state: any, action: any) => {
  if (action.type === `${Helper.MARKETING_AGENT}/reset`) {
    state = undefined;
  }
  return MarketingchatSliceReducer(state, action);
};

export const InboxChatReducerWrapper = (state: any, action: any) => {
  if (action.type === `${Helper.INBOX}/reset`) {
    state = undefined;
  }
  return InboxchatSliceReducer(state, action);
};

export const ContactReducerWrapper = (state: any, action: any) => {
  if (action.type === `${Helper.INBOX}/reset`) {
    state = undefined;
  }
  return ContactSliceReducer(state, action);
};

export const OnboardingReducerWrapper = (state: any, action: any) => {
  if (action.type === `${Helper.CRM_ONBOARDING}/reset`) {
    state = undefined;
  }
  return onboardingSliceReducer(state, action);
};

export const DealsReducerWrapper = (state: any, action: any) => {
  if (action.type === `${Helper.DEALS}/reset`) {
    state = undefined;
  }
  return DealsSliceReducer(state, action);
};

export const SetupReducerWrapper = (state: any, action: any) => {
  if (action.type === `${Helper.SETUP}/reset`) {
    state = undefined;
  }
  return setupSliceReducer(state, action);
};

export const HomeReducerWrapper = (state: any, action: any) => {
  if (action.type === `${Helper.SETUP}/reset`) {
    state = undefined;
  }
  return homeSliceReducer(state, action);
};

export const ProjectReducerWrapper = (state: any, action: any) => {
  if (action.type === `${Helper.PROJECT}/reset`) {
    state = undefined;
  }
  return projectSliceReducer(state, action);
};


export const ProductReducerWrapper = (state: any, action: any) => {
  if (action.type === `${Helper.PRODUCT}/reset`) {
    state = undefined;
  }
  return productSliceReducer(state, action);
};

export const MyPlanReducerWrapper = (state: any, action: any) => {
  if (action.type === `${Helper.MY_PLANS}/reset`) {
    state = undefined;
  }
  return userExtensionSlice(state, action);
};

export const SettingsReducerWrapper = (state: any, action: any) => {
  if (action.type === `${Helper.USER_SETTINGS}/reset`) {
    state = undefined;
  }
  return userSettingsSlice(state, action);
};

export const aiSettingsReducerWrapper = (state: any, action: any) => {
  if (action.type === `${Helper.AI_CCAAS_SETTINGS}/reset`) {
    state = undefined;
  }
  return AISettings(state, action);
};