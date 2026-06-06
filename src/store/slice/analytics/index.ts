import { createSlice } from "@reduxjs/toolkit";
import { ANALYTICS_DATA } from "../../../constants/reducer_names";


export const initialState = {
    analyticsMessage: [] as any[],
    isTyping: false,
    progress: false,
    clearMessage:false,
    product:{type:"campaign",data:null,query:null},
    templateConversation:[],
};

const analytics: any = createSlice({
    name: ANALYTICS_DATA,
    initialState,
    reducers: {
        setAnalyticsMessage: (state, { payload }) => {
            state.analyticsMessage = payload
        },
        setIsTyping: (state, { payload }) => {
            state.isTyping = payload
        },
        progress: (state, { payload }) => {
            state.progress = payload
        }, 
        setProduct:(state,{payload})=>{
            state.product = payload
        },
        setClearMessage:(state,{payload})=>{
            state.clearMessage = payload
        },
        setTemplateSpace:(state,{payload})=>{
            state.templateConversation = payload
        },
        reset: () => initialState,
    },
});

export const analyticsSliceActions = analytics.actions;
export default analytics.reducer;