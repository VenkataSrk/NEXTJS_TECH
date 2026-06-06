import axios from "axios";
// import { config } from "../constants";
import { config } from "../constant";
import { LocalStorage } from "../customhooks/localStorageKeys";
import { GetItemFromStorage } from "../customhooks/useStorage";
import { Cookie } from "../utils";
export class whatsappService {
    public static async get(endPoint: string) {
        const accessToken = GetItemFromStorage(LocalStorage.ACCESS_TOKEN);
        const url: any = config().BASE_URL;
        const response = await axios.get(`${url}${endPoint}`, {
            headers: {
                appId: config().APPID,
                appSecret: config().AppSECRET,
                Authorization: accessToken,
            },
        });
        return response;
    }
    public static async post(data: any, endPoint: string) {
        const url: any = config().BASE_URL;
        const accessToken = GetItemFromStorage(LocalStorage.ACCESS_TOKEN);
        const response = await axios.post(`${url}${endPoint}`, data, {
            headers: {
                appId: config().APPID,
                appSecret: config().AppSECRET,
                Authorization: accessToken,
            },
        });
        return response;
    }
    public static async put(data: any, endPoint: string) {
        const accessToken = GetItemFromStorage(LocalStorage.ACCESS_TOKEN);
        const url: any = config().BASE_URL;
        const response = await axios.put(`${url}${endPoint}`, data, {
            headers: {
                appId: config().APPID,
                appSecret: config().AppSECRET,
                Authorization: accessToken,
            },
        });
        if (response.data.statusCode === 200) {
            return response;
        }
    }
    public static async patch(data: object, endPoint: string) {
        const accessToken = GetItemFromStorage(LocalStorage.ACCESS_TOKEN);
        const url: any = config().BASE_URL;
        const response = await axios
            .put(`${url}${endPoint}`, data, {
                headers: {
                    appId: config().APPID,
                    appSecret: config().AppSECRET,
                    Authorization: accessToken,
                },
            });
        return response;
    }
    public static async delete(endPoint: string) {
        const accessToken = GetItemFromStorage(LocalStorage.ACCESS_TOKEN);
        const url: any = config().BASE_URL;
        try {
            const response = await axios
                .delete(`${url}${endPoint}`, {
                    headers: {
                        appId: config().APPID,
                        appSecret: config().AppSECRET,
                        Authorization: accessToken,
                    },
                });
            return response;
        } catch (error) {
            return error;
        }
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

export class CoreServices {
    public static async get(endPoint: any) {
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
    public static async post(data: any, endPoint: any) {
        const accessToken = GetItemFromStorage(LocalStorage.ACCESS_TOKEN);
        const url: any = config().CORESERVICE_URL;
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
}

export class emailService {
    public static async get(endPoint: string) {
        const accessToken = GetItemFromStorage(LocalStorage.ACCESS_TOKEN);
        const url: any = config().EC_BASE_URL;
        const response = await axios.get(`${url}${endPoint}`, {
            headers: {
                appId: config().APPID,
                appSecret: config().AppSECRET,
                Authorization: accessToken,
            },
        });
        return response;
    }
    public static async post(data: any, endPoint: string) {
        const url: any = config().EC_BASE_URL;
        const accessToken = GetItemFromStorage(LocalStorage.ACCESS_TOKEN);
        const response = await axios.post(`${url}${endPoint}`, data, {
            headers: {
                appId: config().APPID,
                appSecret: config().AppSECRET,
                Authorization: accessToken,
            },
        });
        return response;
    }
    public static async put(data: any, endPoint: string) {
        const accessToken = GetItemFromStorage(LocalStorage.ACCESS_TOKEN);
        const url: any = config().EC_BASE_URL;
        const response = await axios.put(`${url}${endPoint}`, data, {
            headers: {
                appId: config().APPID,
                appSecret: config().AppSECRET,
                Authorization: accessToken,
            },
        });
        if (response.data.statusCode === 200) {
            return response;
        }
    }
    public static async patch(data: object, endPoint: string) {
        const accessToken = GetItemFromStorage(LocalStorage.ACCESS_TOKEN);
        const url: any = config().EC_BASE_URL;
        const response = await axios
            .put(`${url}${endPoint}`, data, {
                headers: {
                    appId: config().APPID,
                    appSecret: config().AppSECRET,
                    Authorization: accessToken,
                },
            });
        return response;
    }
    public static async delete(endPoint: string) {
        const accessToken = GetItemFromStorage(LocalStorage.ACCESS_TOKEN);
        const url: any = config().EC_BASE_URL;
        try {
            const response = await axios
                .delete(`${url}${endPoint}`, {
                    headers: {
                        appId: config().APPID,
                        appSecret: config().AppSECRET,
                        Authorization: accessToken,
                    },
                });
            return response;
        } catch (error) {
            return error;
        }
    }
}

export class inAppService {
    public static async get(endPoint: string) {
        const accessToken = GetItemFromStorage(LocalStorage.ACCESS_TOKEN);
        const url: any = config().INAPP_BASE_URL;
        const response = await axios.get(`${url}${endPoint}`, {
            headers: {
                appId: config().APPID,
                appSecret: config().AppSECRET,
                Authorization: accessToken,
            },
        });
        return response;
    }
    public static async post(data: any, endPoint: string) {
        const url: any = config().INAPP_BASE_URL;
        const accessToken = GetItemFromStorage(LocalStorage.ACCESS_TOKEN);
        const response = await axios.post(`${url}${endPoint}`, data, {
            headers: {
                appId: config().APPID,
                appSecret: config().AppSECRET,
                Authorization: accessToken,
            },
        });
        return response;
    }
    public static async put(data: any, endPoint: string) {
        const accessToken = GetItemFromStorage(LocalStorage.ACCESS_TOKEN);
        const url: any = config().INAPP_BASE_URL;
        const response = await axios.put(`${url}${endPoint}`, data, {
            headers: {
                appId: config().APPID,
                appSecret: config().AppSECRET,
                Authorization: accessToken,
            },
        });
        if (response.data.statusCode === 200) {
            return response;
        }
    }
    public static async patch(data: object, endPoint: string) {
        const accessToken = GetItemFromStorage(LocalStorage.ACCESS_TOKEN);
        const url: any = config().INAPP_BASE_URL;
        const response = await axios
            .put(`${url}${endPoint}`, data, {
                headers: {
                    appId: config().APPID,
                    appSecret: config().AppSECRET,
                    Authorization: accessToken,
                },
            });
        return response;
    }
    public static async delete(endPoint: string) {
        const accessToken = GetItemFromStorage(LocalStorage.ACCESS_TOKEN);
        const url: any = config().INAPP_BASE_URL;
        try {
            const response = await axios
                .delete(`${url}${endPoint}`, {
                    headers: {
                        appId: config().APPID,
                        appSecret: config().AppSECRET,
                        Authorization: accessToken,
                    },
                });
            return response;
        } catch (error) {
            return error;
        }
    }
}


export class CCAASEmailServices {
    public static async get(endPoint: any) {
        const accessToken = GetItemFromStorage(LocalStorage.ACCESS_TOKEN);
        const url: any = config().CCAAS_EMAILSERVICE_URL;
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
    public static async post(data: any, endPoint: any) {
        const accessToken = GetItemFromStorage(LocalStorage.ACCESS_TOKEN);
        const url: any = config().CCAAS_EMAILSERVICE_URL;
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
}

export class AICoreServices {
    public static async get(endPoint: any) {
        const accessToken = GetItemFromStorage(LocalStorage.ACCESS_TOKEN);
        const url: any = config().AI_CORESERVICE_URL;
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
    public static async post(data: any, endPoint: any) {
        const accessToken = GetItemFromStorage(LocalStorage.ACCESS_TOKEN);
        const url: any = config().AI_CORESERVICE_URL;
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
}
export const emailotpverification = async (data: any) => {
    try {
        const url: any = config().BASE_URL;
        const endPoint = '/email_otp_verification';
        const response = await axios.post(`${url}${endPoint}`, data, {
            headers: {
                appId: config().APPID,
                appSecret: config().AppSECRET,
            },
        });
        if (response?.data?.statusCode === 200) {
            return response.data;
        }
        return null;
    } catch (error) {
        console.log(error);
        return null;
    }
};

export const emailOtpVerification = async (data: any) => {
    try {
        const url: any = config().BASE_URL;
        const endPoint = '/check_otp_validation';
        const response = await axios.post(`${url}${endPoint}`, data, {
            headers: {
                appId: config().APPID,
                appSecret: config().AppSECRET,
            },
        });
        if (response?.data?.statusCode === 200) {
            return response.data;
        }
        return null;
    } catch (error) {
        console.log(error);
        return null;
    }
};

export const updatePassword = async (data: any) => {
    try {
        const url: any = config().BASE_URL;
        const endPoint = '/update_password';
        const response = await axios.post(`${url}${endPoint}`, data, {
            headers: {
                appId: config().APPID,
                appSecret: config().AppSECRET,
            },
        });
        if (response?.data?.statusCode === 200) {
            return response.data;
        }
        return null;
    } catch (error) {
        console.log(error);
        return null;
    }
};

export const resendOtp = async (data: any) => {
    try {
        const url: any = config().BASE_URL;
        const endPoint = '/resend_otp';
        const response = await axios.post(`${url}${endPoint}`, data, {
            headers: {
                appId: config().APPID,
                appSecret: config().AppSECRET,
            },
        });
        if (response?.data?.statusCode === 200) {
            return response.data;
        }
        return null;
    } catch (error) {
        console.log(error);
        return null;
    }
};

export class CRMCoreServices {
  public static get(endPoint: any) {
    const accessToken = GetItemFromStorage(LocalStorage.ACCESS_TOKEN);
    const url: any = config().CRMCORESERVICE_URL;
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
    const url: any = config().CRMCORESERVICE_URL;
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
    const url: any = config().CRMCORESERVICE_URL;
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
    const url: any = config().CRMCORESERVICE_URL;
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
    const url: any = config().CRMCORESERVICE_URL;
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
    const url: any = config().CRMCORESERVICE_URL;
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
