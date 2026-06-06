import { COMMON_DATA } from "../../constants/reducer_names";
import analytics from "../slice/analytics";
import * as Helper from "./reset_actions";
import { combineReducers } from "redux";

const rootReducer = (state: any, action: any) => {
    if (action.type === `${COMMON_DATA}/logout`) state = undefined;
    return combinedReducers(state, action);
};

const combinedReducers = combineReducers({
    commonData: Helper.commonSliceReducerWrapper,
    contactData: Helper.contactSliceReducerWrapper,
    emailData: Helper.emailSliceReducerWrapper,
    knowledgeBaseData: Helper.knowledgeReducerWrapper,
    contactsData: Helper.contactsReducerWrapper,
    campaignData: Helper.campaignReducerWrapper,
    analyticsData: Helper.analyticsReducerWrapper,
    reportData:Helper.reportReducerWrapper,
    userExtension: Helper.userExtensionReducerWrapper,
});

export default rootReducer;
