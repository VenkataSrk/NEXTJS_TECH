import { LOGIN_USER } from "../../constants/reducer_names";
import { createSlice } from "@reduxjs/toolkit";

export const initialState = {
  isLoadingBack: false,
  token: "",
  loginUserDetails: {},
  loginError: {
    isLoginError: false,
    errorMsg: ''
  },
  rolesandpermissions: {},
  rolesandpermissions_menus : {},
};

const loginSlice = createSlice({
  name: LOGIN_USER,
  initialState,
  reducers: {
    login: (state, { payload }) => {
      state.token = payload.token;
      state.loginUserDetails = payload.loginUserDetails;
    },
    updateLoadingState: (state, { payload }) => {
      state.isLoadingBack = payload.isLoadingBack;
    },
    setLoginError: (state, { payload }) => {
      state.loginError.isLoginError = true;
      state.loginError.errorMsg = payload;
    },
    clearLoginError: (state) => {
      state.loginError.isLoginError = false;
      state.loginError.errorMsg = '';
    },
    setRolesAndPermissions: (state, { payload }) => {
      state.rolesandpermissions = payload
    },
    setRolesAndPermissions_menu: (state, { payload }) => {
      state.rolesandpermissions_menus = payload
    },
    reset: () => { },
  },
});
export const LoginActions = loginSlice.actions;
export default loginSlice.reducer;
