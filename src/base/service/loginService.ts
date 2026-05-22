// import { Cookie } from "../../constants/base/utils";
import { Cookie } from "../utils";
import { GetItemFromStorage } from "../custom_hooks/useStorage";
import { LocalStorage } from "../custom_hooks/localStorageKeys";
import axios from "axios";
import { config } from "../constant";
import { loginConfig } from "../../pages/login/LoginConstant";
import CryptoJS from "crypto-js";

const process = import.meta.env.VITE_APP_ENV
export class LoginService {
  public static async login(data: any) {
    const url: string = config().LOGIN_API;
    const _body = { ...data, isForceLogin: true }
    let body: any = {}
  if (process === "qa" || process === "umn" || process === "stage") {
      body = await this.encryptCryptoJS(_body);
    } else {
      body = _body;
    }
      
    return axios
      .post(url, body, {
        headers: {
          appId: config().APPID,
          appSecret: config().AppSECRET,
        },
      })
      .then((response: any) => {
         
        return response;
      });
  }

  public static async encryptCryptoJS(body: any) {
     
    const userName = await this.usernameCryptoJS(body);
    
    const pwd = await this.passwordCryptoJS(body);
    
    return { ...body, username: userName, password: pwd };
  }

  public static async passwordCryptoJS(body: any) {
    const secret_key: any = import.meta.env.VITE_REACT_APP_LOGIN_SECRET_KEY;

    return CryptoJS.AES.encrypt(
      body.password,
      secret_key
    ).toString();
  }

  public static async usernameCryptoJS(body: any) {
    const secret_key: any = import.meta.env.VITE_REACT_APP_LOGIN_SECRET_KEY;
    console.log("k----->",secret_key)
    return CryptoJS.AES.encrypt(
      body.username,
      secret_key
    ).toString();
  }

  public static async logOut() {
    const accessToken = GetItemFromStorage(LocalStorage.ACCESS_TOKEN);
    // const token: any = Cookie.get();
    const url: string = config().LOG_OUT_API;
    return axios
      .get(url, {
        headers: {
          appId: config().APPID,
          appSecret: config().AppSECRET,
          Authorization: accessToken,
        },
      })
      .then((response: any) => {
        return response;
      });
  }

  public static async forgotPassword(data: any) {
    const resetPass: any = loginConfig().RESET_PASS_API;
    return axios.post(resetPass, data).then((response: any) => {
      return response;
    });
  }

  //Access Token
  public static getToken() {
    const token: any = Cookie.get();
    let _token = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VybmFtZSI6ImtyaXNobmFrdW1hci5zQHZlY3RvbmUuY29tIiwicGFzc3dvcmQiOiJrcmlzaG5hQDg2NiIsImRldmljZUlkIjoiMTU1LWhkbmRuLXdldXdpZWUiLCJzaXBMb2dpbklkIjoiNTM2NCIsInJvbGVJZCI6NCwiZG9tYWluSWQiOjMzMjgsImV4dCI6MjA5LCJjb21wYW55SWQiOjMzNjIsImVuZXRlcHJpc2VpZCI6ODAzMiwiaG9zdEFkZHJlc3MiOiI4MDMyLnVyY2hhdC51bmlmaWVkcmluZy5jby51ayIsIm9yZGVySWQiOjExMDE4LCJkaXJVc2VySWQiOjMyNjI0LCJpYXQiOjE2MjM3NTQ3ODd9.cR2-rh9bxex3riXVPlHW69gNRqZzDR8Tt6ewhuC9eXg";
    if (token && typeof token.ssoToken != "undefined" && token.ssoToken) {
      _token = token.ssoToken;
    }
    return _token;
  };

  public static async socialMediaLogin(data: any) {
    const accessToken = this.getToken();
    // const token: any = Cookie.get();
    const url: string = `${config().MYACCOUNTSERVICE_URL}/login/social_account_login`;
    return axios
      .post(url, data, {
        headers: {
          appId: config().APPID,
          appSecret: config().AppSECRET,
          Authorization: accessToken,
        },
      })
      .then((response: any) => {
        return response;
      });
  }


}
