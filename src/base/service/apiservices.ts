
// import { GetItemFromStorage } from "../../custom_hooks/useStorage";
import { GetItemFromStorage } from "../../base/custom_hooks/useStorage";
import { LocalStorage } from "../../base/custom_hooks/localStorageKeys";
import { Cookies } from "../../base/cookies";
import axios from "axios";
import { config } from "../constant";

export class CoreServices {
  public static get(endPoint: any) {
    const accessToken = GetItemFromStorage(LocalStorage.ACCESS_TOKEN);
    const url: any = config().CORESERVICE_URL;
    return axios
      .get(`${url}${endPoint}`, {
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
  public static post(data: any, endPoint: any) {
    const url: any = config().CORESERVICE_URL;
    //const token: any = Cookie.get();
    const accessToken = GetItemFromStorage(LocalStorage.ACCESS_TOKEN);
    return axios
      .post(`${url}${endPoint}`, data, {
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
  public static put(data: any, endPoint: any) {
    //const token: any = Cookie.get();
    const accessToken = GetItemFromStorage(LocalStorage.ACCESS_TOKEN);
    const url: any = config().CORESERVICE_URL;
    return axios
      .put(`${url}${endPoint}`, data, {
        headers: {
          appId: config().APPID,
          appSecret: config().AppSECRET,
          Authorization: accessToken,
        },
      })
      .then((response: any) => {
        if (response.data.statusCode === 200) {
          return response;
        } else {
          // SignOut();
        }
      });
  }

  public static patch(data: any, endPoint: any) {
    const accessToken = GetItemFromStorage(LocalStorage.ACCESS_TOKEN);
    const url: any = config().CORESERVICE_URL;
    return axios
      .put(`${url}${endPoint}`, data, {
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

  public static patchMethod(data: any, endPoint: any) {
    //const token: any = Cookie.get();
    const accessToken = GetItemFromStorage(LocalStorage.ACCESS_TOKEN);
    const url: any = config().CORESERVICE_URL;
    return axios
      .patch(`${url}${endPoint}`, data, {
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

  public static delete(endPoint: any) {
    //const token: any = Cookie.get();
    const accessToken = GetItemFromStorage(LocalStorage.ACCESS_TOKEN);
    const url: any = config().CORESERVICE_URL;
    return axios
      .delete(`${url}${endPoint}`, {
        headers: {
          appId: config().APPID,
          appSecret: config().AppSECRET,
          Authorization: accessToken,
        },
      })
      .then((response: any) => {
        return response;
      })
      .catch((error) => {
        return error;
      });
  }
}
