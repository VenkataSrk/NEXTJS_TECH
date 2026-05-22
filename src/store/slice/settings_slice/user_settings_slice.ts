import { createSlice } from "@reduxjs/toolkit";
import { USER_SETTINGS } from "../../constants/reducer_names";

const initialState = {
  settingsMenuValue: "",
  refreshNotification: false,
  isPlanUpgrade: false,
  updateConfig: {
    open: false,
    source: "",
    type: ""
  },
  channelsConfigutationOptions: {},
  loginAccountFetures: [],
  notify: [] as any,
  configurationChannel: {} as any

};
const userSettingsSlice = createSlice({
  name: USER_SETTINGS,
  initialState,
  reducers: {
    updateMenuValue: (state, { payload }) => {
      state.settingsMenuValue = payload.settingsMenuValue;
    },
    setRefreshNotification: (state) =>{
      state.refreshNotification = !state.refreshNotification
    },
    setUpgradePlan: (state, { payload }) =>{
      state.isPlanUpgrade = payload
    },
    setUpdateConfigModal: (state, { payload }) =>{
      state.updateConfig = payload
    },
    setChannelsConfigutationOptions: (state, { payload }) =>{
      state.channelsConfigutationOptions = payload
    },
    setLoginAccountFetures: (state, { payload }) =>{
      state.loginAccountFetures = payload
    },
    setNotification: (state, { payload }) => {
      let id = Date.now()
      state.notify.push({
        type: payload.type,
        message: payload.message,
        id,
        time: payload.time ? payload.time * 1000 : 3000
      });
    },
    removeNotification: (state, { payload }) => {
      if (payload) {
        state.notify = state.notify.filter((item: any) => item.id !== payload);
      }
    },
    setOperationHubChannelConfiguration:(state,{payload})=>{
      if(payload?.key ) {
        console.log("keys",payload)
        state.configurationChannel[payload?.key] = payload?.value
      }
    },
    reset: () => {},
  },
});
export const UserSettingsActions = userSettingsSlice.actions;
export default userSettingsSlice.reducer;
