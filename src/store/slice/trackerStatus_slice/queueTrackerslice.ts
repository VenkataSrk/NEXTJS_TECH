import { createSlice } from "@reduxjs/toolkit";
import { QUEUE_TRACKER_DATA } from "../../constants/reducer_names";

export const initialState = {
  queueTrackerData: [] as any,
  timer:0,
};

const queueTrackerSlice = createSlice({
  name: QUEUE_TRACKER_DATA,
  initialState,
  reducers: {
    queueTrackerData: (state, { payload }) => {
      state.queueTrackerData = payload.queueTrackerData;
    },

    updateQueueData: (state, { payload }) => {
      const index = state.queueTrackerData?.findIndex(
        (data: any) => data?.qid?.toString() === payload?.qid?.toString()
      );
      if (index !== -1) {
        state.queueTrackerData[index] = {
          ...state.queueTrackerData[index],
          queueList: JSON.stringify(payload?.queueList),
        };
        state.queueTrackerData[index].queueName = payload.queueName;
      }
    },
    updatetTimer:(state,{payload})=>{
      state.timer = payload;

    },

    reset: () => {},
  },
});
export const queueTrackerActions = queueTrackerSlice.actions;
export default queueTrackerSlice.reducer;
