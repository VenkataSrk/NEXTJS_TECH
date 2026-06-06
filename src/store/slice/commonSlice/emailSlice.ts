import { createSlice } from "@reduxjs/toolkit";
import { EMAIL_DATA } from "../../../constants/reducer_names";

export const initialState = {
basicCampaignDetails:{},
templateDetails:[],
abCampaignDetails:{},
deliveryDetails:{}
};

const emailDataSlice: any = createSlice({
  name: EMAIL_DATA,
  initialState,
  reducers: {
    setBasicCampaignDetails: (state, { payload }) => {
      state.basicCampaignDetails= payload
    },
    setTemplateDetails:(state,{payload})=>{
        state.templateDetails=payload;
    },
    setABCampaignDetails:(state,{payload})=>{
        state.abCampaignDetails = payload;
    },
    setDeliveryDetails:(state,{payload})=>{
        state.deliveryDetails = payload;
    },
    reset: () => initialState,
  },
});

export const emailDataSliceActions = emailDataSlice.actions;
export default emailDataSlice.reducer;
