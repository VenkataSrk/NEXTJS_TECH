import { LocalStorage } from "./localStorageKeys";

import CryptoJS from 'crypto-js';
// import { CoreServices, ReportServices } from "../base/service/apiServices";
// import presenceService from "../base/service/presence";
// import jwt_decode from "jwt-decode";
// import { Types } from "../base/action/action";
// import { shortcutKeyActions } from "../store/slices/shortcutkeys_slice/shortcutkeys_slice";
// import store from "../store/redux-store/root_store";


const toEncrypt = [
  LocalStorage?.PLAN_ID,
  LocalStorage?.LOGIN_USER_CRM_DETAILS,
  LocalStorage?.ACCESS_TOKEN,
  LocalStorage?.REFRESH_TOKEN,
  LocalStorage?.ROLE_ID,
  LocalStorage?.PRODUCT_ID,
  LocalStorage?.USER_DETAILS,
  LocalStorage?.USER_EMAIL,
];

export const GetItemFromStorage = (key: any) => {
  const initialValue = null;
  if (typeof window === "undefined") {
    return initialValue;
  }
  try {
    const item: any = window?.localStorage.getItem(key);
    if (toEncrypt?.includes(key) && item) {
      const secretKey: any = import.meta.env.VITE_REACT_APP_LOCAL_STORAGE_SECRET_KEY;
      const decrypted: any = CryptoJS.AES.decrypt(item, secretKey).toString(CryptoJS.enc.Utf8);
      if (decrypted) {
        return JSON.parse(decrypted);
      } else {
        localStorage.clear();
        sessionStorage.clear();
        const host = window.location.hostname;
        const domainParts = host.split(".");
        domainParts.shift();
        const domain = domainParts.join(".");
        document.cookie.split(";").forEach((ck) => {
          document.cookie = ck
            .replace(/^ +/, "")
            .replace(
              /=.*/,
              "=;expires=" + new Date().toUTCString() + ";path=/" + domain
            );
        });
        window.location.href = '/login';
        return item && item !== 'undefined' ? JSON.parse(item) : initialValue;
      }
    }
    return item && item !== 'undefined' ? JSON.parse(item) : initialValue;
  } catch (error) {
    console.error(error);
    return initialValue;
  }
};

export const SetItemInStorage = (key: any, value: any) => {
  try {
    if (typeof window !== "undefined") {
      if (toEncrypt?.includes(key)) {
        const secretKey: any = import.meta.env.VITE_REACT_APP_LOCAL_STORAGE_SECRET_KEY;
        const encrypted = CryptoJS.AES.encrypt(JSON.stringify(value), secretKey).toString();
        window.localStorage.setItem(key, encrypted);
      } else {
        window.localStorage.setItem(key, JSON.stringify(value));
      }
    }
  } catch (error) {
    console.error(error);
  }
};

export const ClearStorage = () => {
  try {
    if (typeof window !== "undefined") {
      window.localStorage.clear();
    }
  } catch (error) {
    console.error(error);
  }
};

// const signOut = () => {
//   try {
//     const userEmailId = GetItemFromStorage(LocalStorage.USER_EMAIL);
//     const LoginCcaasUserDetails: any = GetItemFromStorage(LocalStorage.LOGIN_USER_CCAAS_DETAILS);
//     const { users } = store.getState().userList;
//     const { agentStatus } = store.getState().agentState;
//     const accessToken = GetItemFromStorage(LocalStorage.ACCESS_TOKEN);
//     let LoginUserDetails: any = accessToken && jwt_decode(accessToken);
//     const popUpDetails: any = store.getState().callPopUpDetails;
//     const { incomingCallDetail } = popUpDetails;

//     console.log("Signoutttt")
//     let updatedata = {
//       isVoice: 0,
//       isVoiceCallTransfer: 0,
//       isChat: 0,
//       isChatTransfer: 0,
//       isEmail: 0,
//       isEmailTransfer: 0,
//       stateTime: Math.floor(Date.now() / 1000),
//       statusName: "Offline",
//     };
//     CoreServices.put(updatedata, `/update_agent_status_value/${userEmailId}`).then(async (value: any) => {
//       if (value && value.data.statusCode === 200) {
//         let status: any = null
//         if (agentStatus?.length > 0) {
//           [status] = agentStatus.filter((i: any) => { return "Offline" === i?.statusName?.trim() })
//         }
//         let enableStatus: any = null
//         if (users.length > 0) {
//           [enableStatus] = users?.filter((i: any) => { return LoginCcaasUserDetails?.emailId === i?.emailId })
//         }
//         console.log('presenceService 8', status, enableStatus);
//         presenceService?.io?.emit("updatestate_backend", {
//           domainId: LoginUserDetails?.domainId,
//           isVoice: (enableStatus?.voice === 1 && status?.isVoice === 1) ? 1 : 0,
//           isVoiceCallTransfer: status?.isVoiceCallTransfer ?? 0,
//           isChat: (enableStatus?.chat === 1 && status?.isChat === 1) ? 1 : 0,
//           isChatTransfer: status?.isChatTransfer ?? 0,
//           isEmail: (enableStatus?.email === 1 && status?.isEmail === 1) ? 1 : 0,
//           isEmailTransfer: status?.isEmailTransfer ?? 0,
//           stateTime: Math.floor(Date.now() / 1000),
//           statusName: "Offline",
//           ext: LoginUserDetails?.ext,
//           email: LoginUserDetails?.Email,
//           agentStatus: Object.keys(incomingCallDetail)?.length > 0 ? 0 : 1,
//           userId: LoginCcaasUserDetails?.UserID,
//           routingProfileId: LoginCcaasUserDetails?.routing_profile_id,
//           roleId: LoginCcaasUserDetails?.roleid,
//          isCrm: 1
//         });
//         presenceService.destroy();
//       }
//       try {
//         await ReportServices.get("/get_dashboard_agent_status")
//       } catch (error) {
//         console.log(error)
//       }
//       const date = new Date();
//       const uuid = GetItemFromStorage("uuid");
//       // const uuid = GetItemFromStorage("uuid");
//       const body = {
//         logOutTimestamp: date.getTime(),
//         logInSessionId: uuid,
//       };
//       CoreServices.put(body, `/update_login_session_history`).catch((e: any) => console.error(e));
//       store.dispatch({
//         type: Types.USER_LOGOUT,
//       });
//       // Cookie.clear();
//       // ClearStorage();
//       // navigate("/login");
//       // localStorage.clear();
//       // sessionStorage.clear();
//       store.dispatch(
//         shortcutKeyActions.propsValue({
//           logOut: false,
//         }),

//       );
//     });
//     localStorage.clear();
//     sessionStorage.clear();
//     window.location.href = '/login';
//     //unregister();
//     // navigate("/login");

//   } catch (error) {
//     console.log(error)
//   }
// };
