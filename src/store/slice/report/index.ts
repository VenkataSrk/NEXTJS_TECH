import { createSlice } from "@reduxjs/toolkit";
import { REPORT_DATA } from "../../../constants/reducer_names";


export const initialState = {
    reportMessage: [] as any[],
    isTyping: false,
};

const report: any = createSlice({
    name: REPORT_DATA,
    initialState,
    reducers: {
        setReportMessage: (state, { payload }) => {
            state.reportMessage = payload
        },
        setIsTyping: (state, { payload }) => {
            state.isTyping = payload
        },
        reset: () => initialState,
    },
});

export const reportSliceActions = report.actions;
export default report.reducer;