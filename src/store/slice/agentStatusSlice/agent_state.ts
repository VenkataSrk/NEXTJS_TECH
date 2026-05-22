import { createSlice } from "@reduxjs/toolkit";
import { FailedUserFetchResponse } from "../../../base/utils";
import { getAgentStatusDetails } from "../../../base/service/core_serviceApi";
import { AGENT_STATE } from "../../constants/reducer_names";

const initialState = {
  loading: false,
  error: {
    hasError: false,
    errorMessage: "",
  },
  status: [] as any,
  currentStatus: {} as any,
  agentStatus: [] as any
};

const agentState = createSlice({
  name: AGENT_STATE,
  initialState,
  reducers: {
    addStatusList: (state, { payload }) => {
      state.status = payload;
    },
    setCurrentStatus: (state, { payload }) => {
      const targetStatus = String(payload?.status ?? "").toLowerCase();
      const index = state.status?.findIndex(
        (element: any) => String(element?.statusName ?? "").toLowerCase() === targetStatus
      );
      if (index !== -1) {
        state.currentStatus = state.status[index];
      } else if (targetStatus) {
        state.currentStatus = {
          statusName: payload?.status,
          label: payload?.status,
          value: targetStatus,
        };
      }
    },
    setAgentStatus: (state, { payload }) => {
      state.agentStatus = payload
    },
    reset: () => { },
  },
  // extraReducers: (builder) => {
  //   builder.addCase(getAgentStatusDetails.pending, (state) => {
  //     state.loading = true;
  //   });
  //   builder.addCase(getAgentStatusDetails.fulfilled, (state, { payload }) => {
  //     if (payload?.data?.statusCode === 200) {
  //       state.status = payload?.data?.getAgentListRes;
  //       state.agentStatus = payload?.data?.getAgentListRes;
  //       state.error.hasError = false;
  //       state.error.errorMessage = "";
  //     } else {
  //       state.error.hasError = true;
  //       state.error.errorMessage = payload?.message ?? FailedUserFetchResponse;
  //     }
  //     state.loading = false;
  //   });
  //   builder.addCase(getAgentStatusDetails.rejected, (state, { payload }: any) => {
  //     state.error.hasError = true;
  //     state.error.errorMessage = payload?.message ?? FailedUserFetchResponse;
  //     state.loading = false;
  //   });
  // },
});

export const AgentStateAction = agentState.actions;
export default agentState.reducer;
