import { AppHeaders, TimeoutDuration } from "../api_constants";
import { GetItemFromStorage } from "../custom_hooks/useStorage";
import { LocalStorage } from "../custom_hooks/localStorageKeys";
import axios from "axios";

export class ApiServices {
  //INFO: GET METHOD
  public static async get(
    baseUrl: any,
    endPoint: any,
    timeoutDuration = TimeoutDuration,
    extraHeaders: any = undefined
  ) {
    const accessToken = GetItemFromStorage(LocalStorage.ACCESS_TOKEN);
    // const token: any = Cookies.get();
    const url: any = baseUrl + endPoint;
    let headers = { ...AppHeaders, Authorization: accessToken };
    if (extraHeaders) headers = { ...headers, ...extraHeaders };
    return axios
      .get(`${url}`, { timeout: timeoutDuration, headers })
      .then((response: any) => response)
      .catch((error: any) => error);
  }

  //INFO: POST METHOD
  public static post(
    baseUrl: any,
    endPoint: any,
    data: any,
    timeoutDuration = TimeoutDuration,
    extraHeaders: any = undefined
  ) {
     const accessToken = GetItemFromStorage(LocalStorage.ACCESS_TOKEN);
    const url: any = baseUrl + endPoint;
    let headers = { ...AppHeaders, Authorization: accessToken };
    if (extraHeaders) headers = { ...headers, ...extraHeaders };
    return axios
      .post(`${url}`, data, { timeout: timeoutDuration, headers })
      .then((response: any) => response)
      .catch((error: any) => error);
  }

  //INFO: PUT METHOD
  public static put(
    baseUrl: any,
    endPoint: any,
    data: any,
    timeoutDuration = TimeoutDuration,
    extraHeaders: any = undefined
  ) {
      const accessToken = GetItemFromStorage(LocalStorage.ACCESS_TOKEN);
    const url: any = baseUrl + endPoint;
    let headers = { ...AppHeaders, Authorization: accessToken };
    if (extraHeaders) headers = { ...headers, ...extraHeaders };
    return axios
      .put(`${url}`, data, { timeout: timeoutDuration, headers })
      .then((response: any) => response)
      .catch((error: any) => error);
  }

  //INFO: PATCH METHOD
  public static patch(
    baseUrl: any,
    endPoint: any,
    data: any,
    timeoutDuration = TimeoutDuration,
    extraHeaders: any = undefined
  ) {
      const accessToken = GetItemFromStorage(LocalStorage.ACCESS_TOKEN);
    const url: any = baseUrl + endPoint;
    let headers = { ...AppHeaders, Authorization: accessToken };
    if (extraHeaders) headers = { ...headers, ...extraHeaders };
    return axios
      .put(`${url}`, data, { timeout: timeoutDuration, headers })
      .then((response: any) => response)
      .catch((error: any) => error);
  }

  //INFO: DELETE METHOD
  public static delete(
    baseUrl: any,
    endPoint: any,
    timeoutDuration = TimeoutDuration,
    extraHeaders: any = undefined
  ) {
      const accessToken = GetItemFromStorage(LocalStorage.ACCESS_TOKEN);
    const url: any = baseUrl + endPoint;
    let headers = { ...AppHeaders, Authorization: accessToken };
    if (extraHeaders) headers = { ...headers, ...extraHeaders };
    return axios
      .delete(`${url}`, { timeout: timeoutDuration, headers })
      .then((response: any) => response)
      .catch((error: any) => error);
  }

  //INFO: LOGIN POST METHOD
  public static login(baseUrl: any, endPoint: any, data: any) {
    const url: any = baseUrl + endPoint;
    return axios
      .post(`${url}`, data, { timeout: TimeoutDuration, headers: AppHeaders })
      .then((response: any) => response)
      .catch((error: any) => error);
  }
}
