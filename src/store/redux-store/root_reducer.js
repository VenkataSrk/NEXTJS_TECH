import * as Helper from "./reset_action";

import { combineReducers } from "redux";

const combinedReducers = combineReducers({
  message:Helper.messageReducerWrapper,
  liveTracker:Helper.liveTrackerReducerWrapper,
  chatBuilder: Helper.chatBuilderReducerWrapper,
  shopingStore: Helper.shopingSliceReducerWrapper,
});

const rootReducer = (state, action) => {
//  if (action.type === `${LOGIN_USER}/logout`) state = undefined;
  return combinedReducers(state, action);
};
export default rootReducer;
