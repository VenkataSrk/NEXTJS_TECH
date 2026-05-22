import { createSlice } from "@reduxjs/toolkit";
import { CALL_USER_AGENT_REDUCER } from "../../constants/reducer_names";

export const initialState = {
  userAgent: {} as any,
  isSuccess: false,
};

const CallUserAgentSlice = createSlice({
  name: CALL_USER_AGENT_REDUCER,
  initialState,
  reducers: {
    setLoginuserUserAgent: (state, { payload }) => {
      state.userAgent = payload;
      state.isSuccess = !state.isSuccess;
    },

    reset: () => {},
  },
});

export const CallUserAgentActions = CallUserAgentSlice.actions;
export default CallUserAgentSlice.reducer;
