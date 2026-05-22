import { LOGIN_USER } from "../constants/reducer_names";
import * as Helper from "./reset_action";
import { combineReducers } from "redux";

const rootReducer = (state: any, action: any) => {
    if (action.type === `${LOGIN_USER}/logout`) state = undefined;
    return combinedReducers(state, action);
};

const combinedReducers = combineReducers({
    loginUser: Helper.LoginUserReducerWrapper,
    commonData: Helper.commonSliceReducerWrapper,
    aiChatStore: Helper.AiChatReducerWrapper,
    callPopUpDetails: Helper.CallPOpUpAgnetModelWrapper,
    callLoginUserAgent: Helper.CallUserAgentReducerWrapper,
    chatReducer: Helper.ChatReducerWrapper,
    agentState: Helper.agentstateReducerWrapper,
    userList: Helper.UserListReducerWrapper,
    marketingChatStore: Helper.MarketingChatReducerWrapper,
    inboxChatStore: Helper.InboxChatReducerWrapper,
    contactStore: Helper.ContactReducerWrapper,
    dealsStore: Helper.DealsReducerWrapper,
    onboarding: Helper.OnboardingReducerWrapper,
    setupStore: Helper.SetupReducerWrapper,
    homeStore: Helper.HomeReducerWrapper,
    projectStore:Helper.ProjectReducerWrapper,
    productStore:Helper.ProductReducerWrapper,
    myPlanStore:Helper.MyPlanReducerWrapper,
    aiSettings: Helper.aiSettingsReducerWrapper,
    settingsStore: Helper.SettingsReducerWrapper,

});

export default rootReducer;
