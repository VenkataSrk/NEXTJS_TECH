import { createSlice } from "@reduxjs/toolkit";
import { getCallTimeFromTimestamp } from "../../../base/utils";
import moment from "moment";
import { AGENT_TRACKER_DATA } from "../../constants/reducer_names";

export const initialState = {
  agentTrackerData: [] as any,
  liveSessions: [] as any,
  agentList: [] as any,
  interval: "",
};

const agentTrackerSlice = createSlice({
  name: AGENT_TRACKER_DATA,
  initialState,
  reducers: {
    updateAgentTrackerData: (state, { payload }) => {
      state.agentTrackerData = payload.agentTrackerData;
    },
    liveSessions: (state, { payload }) => {
      state.liveSessions = payload;
    },
    updateAgentList: (state, { payload }) => {
      state.agentList = payload.agentList;
    },
    updateUserInAgentTracker: (state, { payload }) => {
      const eventEmail = payload?.email?.toString()?.toLowerCase();
      if (!eventEmail) return;
      const nextStatus = payload?.status ?? "Offline";
      const nowTs = Date.now() / 1000;

      const updateTrackerRow = (row: any) => {
        const rowEmail = (row?.emailId ?? row?.email ?? "").toString().toLowerCase();
        if (rowEmail !== eventEmail) return row;
        return { ...row, statusName: nextStatus, stateTimer: nowTs };
      };

      state.agentTrackerData = state.agentTrackerData.map(updateTrackerRow);
      state.agentList = state.agentList.map(updateTrackerRow);
    },

    agentTimer: (state) => {
      state.agentTrackerData.map((data: any) => {
        return (data.Timer = getCallTimeFromTimestamp(data?.stateTimer));
      });
    },

    reset: () => {},
  },
});
export const agentTrackerActions = agentTrackerSlice.actions;
export default agentTrackerSlice.reducer;
