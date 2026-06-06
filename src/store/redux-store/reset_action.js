import * as Helper from "./reducer_names";

import messageReducer from  '../slice/message_slice'
import liveTrackerReducer from  '../slice/live_tracker'
import chatBuilderReducer from  '../slice/builder_slice'
import shopingReducer from  '../slice/shoping_slice'

export const messageReducerWrapper = (state, action) => {
  if (action.type === `${Helper.MESSAGES}/reset`) {
    state = undefined;
  }
  return messageReducer(state, action);
};


export const liveTrackerReducerWrapper = (state, action) => {
  if (action.type === `${Helper.MESSAGES}/reset`) {
    state = undefined;
  }
  return liveTrackerReducer(state, action);
};

export const chatBuilderReducerWrapper = (state, action) => {
  if (action.type === `${Helper.BUILDER}/reset`) {
    state = undefined;
  }
  return chatBuilderReducer(state, action);
};

export const shopingSliceReducerWrapper = (state, action) => {
  if (action.type === `${Helper.SHOPING_SLICE}/reset`) {
    state = undefined;
  }
  return shopingReducer(state, action);
};

