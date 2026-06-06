import { createSlice } from "@reduxjs/toolkit";
import { CONTACT_DATA } from "../../../constants/reducer_names";

export const initialState = {
contact_Data:{},
loginDetails:{}
};

const contactDataSlice: any = createSlice({
  name: CONTACT_DATA,
  initialState,
  reducers: {
    setContactData: (state, { payload }) => {
      state.contact_Data = payload
    },
    setLoginDetails:(state,{payload})=>{
      state.loginDetails= payload
    },
    reset: () => initialState,
  },
});

export const contactDataSliceActions = contactDataSlice.actions;
export default contactDataSlice.reducer;
