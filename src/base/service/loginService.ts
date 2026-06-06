import axios from "axios";
import { config } from "../constant";
// import { config } from "../constants";
import { encryptAES } from "../utils";


export class LoginService {
  public static async login(data: any) {
  
    let userName =await encryptAES(data?.username);  

    let pass =await encryptAES(data?.password);  
    let body={...data} ;
    if(import.meta.env.VITE_APP_ENV=="development"){
      body = {
        ...data, "username": userName, "password": pass
      }
    }
     
    try {
      const url = config().LOGIN_API;
      if (!url) throw new Error('LOGIN_API is not defined');
      const _body = { ...body, isForceLogin: true ,loginCheck:'CAMPAIGN'};
      const response = await axios.post(url, _body, {
        headers: {
          appId: config().APPID,
          appSecret: config().AppSECRET,
        },
      });
      return response;
    } catch (error :any) {
      console.error('Login failed<><><<:', error);
      return error?.response;
      throw error;
      
    }
  }
}
  export class LoginAllServices {
  public static async loginall(data: any) {
    try {
      const url = config().LOGIN_All;

      if (!url) throw new Error("LOGIN_ALL_API is not defined");

      const response = await axios.post(url, data);
      return response;
    } catch (error) {
      console.error("Login failed:", error);
      throw error;
    }
  }
}

  export class LoginOutService {
  // ✅ FIXED LOGOUT FUNCTION (INSIDE CLASS)
  public static async logout(data: any) {
    try {
      const url = config().LOGIN_OUT;

      if (!url) throw new Error("LOGOUT_ALL_API is not defined");

      const response = await axios.post(url, data);
      return response;
    } catch (error) {
      console.error("Logout failed:", error);
      throw error;
    }
  }
}

  /*public static async logOut() {
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
  }*/

  /*public static async forgotPassword(data: any) {
    const resetPass: any = loginConfig().RESET_PASS_API;
    return axios.post(resetPass, data).then((response: any) => {
      return response;
    });
  }*/
// }
