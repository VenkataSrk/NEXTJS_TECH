import { createSlice } from "@reduxjs/toolkit";
import { CAMPAIGN_DATA } from "../../../constants/reducer_names";


export const initialState = {
    campaignMessage: [] as any[],
    isTyping: false,
    isOpenCampaignModal: false,
    progress: false,
    isRegenerate: false,
    isEdit: false,
    templeteView:{},
    chatType:"new",
    pastCampaignDetails:{},
    notification:null,
    whatsAppCardDetails:{},
    clearMessage:false,
    product:{type:"campaign",data:null,query:null},
    templateConversation:[],
    
   
};

const campaign: any = createSlice({
    name: CAMPAIGN_DATA,
    initialState,
    reducers: {
        setCampaignMessage: (state, { payload }) => {
            state.campaignMessage = payload
        },
        setIsTyping: (state, { payload }) => {
            state.isTyping = payload
        },
        progress: (state, { payload }) => {
            state.progress = payload
        },
        setIsRegenerate: (state, { payload }) => {
            state.isRegenerate = payload
        },
        setIsEdit: (state, { payload }) => {
            state.isEdit = payload
        },
        setTempleteView: (state, { payload }) => {
            state.templeteView = payload
        },
        setCampaignChatType: (state,{payload})=>{
            state.chatType = payload
        },
        setPastCampaignDetails:(state,{payload})=>{
            state.pastCampaignDetails = payload
        },
        setNotificationDetails:(state,{payload})=>{
            state.notification = payload
        },
        setWhatsappCardDetails:(state,{payload})=>{
            state.whatsAppCardDetails = payload
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
        reset: () => {
        },
    },
});

export const campaignSliceActions = campaign.actions;
export default campaign.reducer;