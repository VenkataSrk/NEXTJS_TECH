// import { API_ROUTE_CONTROLLER } from "../../../base/routes";

import axios from "axios";
import { CoreServices } from "../../../base/service/apiservices";
import { MyplanProfileActions } from "../../../store/slice/my_plans/profile";
import { config } from "../../../base/constant";
import { GetItemFromStorage } from "../../../base/custom_hooks/useStorage";
import { LocalStorage } from "../../../base/custom_hooks/localStorageKeys";


export const getUserExtensionInfo = (payload:any) => {
  return CoreServices.post(payload, "/get_my_plans_profile_info");
};
export const getUserManagementInfo = () => {
  return CoreServices.get( "/get_user_details");
};

export const getRolesListInfo = (payload:any) => {
  return CoreServices.post(payload,"/get_roles_info");
};

export const getOrderHistoryInfo = (payload:any) => {
  return CoreServices.post(payload, "/get_my_plans_order_history");
};

export const getSubscriptionManagement = (payload:any) => {
  return CoreServices.post(payload, "/get_my_plans_subscription_management");
};

export const getAddonList = (payload: {
  company_id: Number,
}) => async (dispatch: any) => {
  try {
    console.log("payloadpayload", payload)
    CoreServices.post(payload, `/get_my_plans_addon_list`)
      .then((response: any) => {
        // console.log("inside response", response)
        if (response && response.data.status_code === 200) {
          // console.log("getUserManagementInfo", response)
          dispatch(MyplanProfileActions.setAddonList(response.data.finalresponse));
        }
      })

  } catch (err) {
    console.log("getUserExtensionInfo err:", err);
  }

};

// export const getPurchaseAddonList = (payload: {
//   "category_id": Number,
//   "tier_id": Number,
//   "product_id": Number  
// }) => async (dispatch: any) => {
//   try {
//     const _url = API_ROUTE_CONTROLLER.worktualGetProdWiseAddonList;
//     console.log("payloadpayload", payload)
//    await ApiServices.post( _url,'/addon_purchase_list',payload,)
//       .then((response: any) => {
//         console.log("inside response", response)
//         if (response && response.data.status_code === 200) {
//           console.log("getUserManagementInfo", response)
//           dispatch(MyplanProfileActions.setPurchaseAddonList(response.data.finalresponse));
//         }
//       })

//   } catch (err) {
//     console.log("getUserExtensionInfo err:", err);
//   }

// };

export const getTeamList = async () => {
  try {
    const response = await CoreServices.get('/get_user_department');
    if (response?.data?.statusCode === 200 || response?.data?.result?.length > 0) {
      const responseData = response?.data?.result;
      return responseData;
    } else {
      return [];
    }
  } catch (err) {
    console.log("get_user_department err:", err);
    return [];
  }
};

export const getSecurityInfo = async () => {
  try {
    const response = await CoreServices.get('/get_user_password_info');

    if (response?.data?.statusCode === 200 || response?.data?.result?.length > 0) {
      const responseData = response?.data?.result;
      return responseData[0];
    } else {
      return [];
    }
  } catch (err) {
    console.log("get_user_password_info err:", err);
    return [];
  }
};

export const getSyncUpMyaccountDetails = async (body: any) => {
  try {
    const response = await CoreServices.post(body, '/get_syncup_myaccount_details');

    if (response && response?.data?.statusCode === 0) {
      const responseData = response?.data?.result;
      return responseData;
    } else {
      return [];
    }
  } catch (err) {
    console.log("get_syncup_myaccount_details err:", err);
    return [];
  }
};

export const insertSocialLogin = async (body: any) => {
  try {
    const response = await CoreServices.post(body, '/create_myaccount_syncup_details');

    if (response && response?.data?.statusCode === 0) {
      const responseData = response?.data?.result;
      return responseData;
    } else {
      return [];
    }
  } catch (err) {
    console.log("create_myaccount_syncup_details err:", err);
    return [];
  }
};

export const insertUpdateTwoStepVerification = async (body:any) => {
  try {
     const response = await CoreServices.post(body, '/insert_update_two_step_verification');

    if (response && response?.data?.statusCode === 200) {
      const responseData = response?.data?.result;
      return responseData[0];
    } else {
      return [];
    }
  } catch (err) {
    console.log("insertUpdateTwoStepVerification err:", err);
    return [];
  }
};

export const getTwoStepVerfication = async (body:any) => {
  try {
     const response = await CoreServices.get('/get_security_two_step_verification');

    if (response && response?.data?.statusCode === 200) {
      const responseData = response?.data?.result;
      return responseData[0];
    } else {
      return [];
    }
  } catch (err) {
    console.log("getTwoStepVerfication err:", err);
    return [];
  }
};

export const insertUpdateDataPrivacySet = async (body:any) => {
  try {
     const response = await CoreServices.post(body, '/insert_update_data_and_privacy_set');

    if (response && response?.data?.statusCode === 200) {
      const responseData = response?.data?.result;
      return responseData[0];
    } else {
      return [];
    }
  } catch (err) {
    console.log("insertUpdateDataPrivacySet err:", err);
    return [];
  }
};

export const getDataPrivacySet = async (body:any) => {
  try {
     const response = await CoreServices.post(body, '/get_data_and_privacy_set');

    if (response && response?.data?.statusCode === 200) {
      const responseData = response?.data?.result;
      return responseData[0];
    } else {
      return [];
    }
  } catch (err) {
    console.log("getDataPrivacySet err:", err);
    return [];
  }
};

export const getAllMembersRole = async (body:any) => {
  try {
     const response = await CoreServices.post(body, '/get_all_members_role');

    if (response && response?.data?.statusCode === 200) {
      const responseData = response?.data?.result;
      return responseData;
    } else {
      return [];
    }
  } catch (err) {
    console.log("getAllMembersRole err:", err);
    return [];
  }
};

export const getUserDetails = async (body:any) => {
  try {
     const response = await CoreServices.post(body, '/get_user_details_by_role');

    if (response && response?.data?.statusCode === 200) {
      const responseData = response?.data?.result;
      return responseData;
    } else {
      return [];
    }
  } catch (err) {
    console.log("getUserDetails err:", err);
    return [];
  }
};

export const insertUpdateRolePermission = async (body:any) => {
  try {
     const response = await CoreServices.post(body, '/insert_update_myplan_role_permission');

    if (response && response?.data?.statusCode === 200) {
      const responseData = response?.data?.result;
      return responseData[0];
    } else {
      return [];
    }
  } catch (err) {
    console.log("insertUpdateRolePermission err:", err);
    return [];
  }
};


export const getRolePermission = async () => {
  try {
     const response = await CoreServices.get('/get_myplan_role_permission');
    if (response && response?.data?.statusCode === 200) {
      const responseData = response?.data?.result;
      return responseData[0];
    } else {
      return [];
    }
  } catch (err) {
    console.log("getRolePermission err:", err);
    return [];
  }
};

export const UpdateUserPermission = async (body:any) => {
  try {
     const response = await CoreServices.post(body, '/update_myplan_user_permission');

    if (response && response?.data?.statusCode === 200) {
      const responseData = response?.data?.result;
      return responseData[0];
    } else {
      return [];
    }
  } catch (err) {
    console.log("UpdateUserPermission err:", err);
    return [];
  }
};

export const getuserPermission = async (data : any) => {
  try {
    const body = {
      userId : data,
    }
     const response = await CoreServices.post(body , '/get_myplan_user_permission');
    if (response && response?.data?.statusCode === 200) {
      const responseData = response?.data?.result;
      return responseData[0];
    } else {
      return [];
    }
  } catch (err) {
    console.log("getuserPermission err:", err);
    return [];
  }
};

export const getTeamDetails = async (body:any) => {
  try {
     const response = await CoreServices.post(body, '/get_myplan_teams_details');

    if (response && response?.data?.statusCode === 200) {
      const responseData = response?.data?.result;
      return responseData;
    } else {
      return [];
    }
  } catch (err) {
    console.log("UpdateUserPermission err:", err);
    return [];
  }
};
export const insertUpdateTeamPermission = async (body:any) => {
  try {
     const response = await CoreServices.post(body, '/insert_update_myplan_team_permission');

    if (response && response?.data?.statusCode === 200) {
      const responseData = response?.data?.result;
      return responseData[0];
    } else {
      return [];
    }
  } catch (err) {
    console.log("insertUpdateRolePermission err:", err);
    return [];
  }
};

export const getTeamPermission = async (body:any) => {
  try {
     const response = await CoreServices.post(body,'/get_myplan_team_permission');
    if (response && response?.data?.statusCode === 200) {
      const responseData = response?.data?.result;
      return responseData[0];
    } else {
      return [];
    }
  } catch (err) {
    console.log("getRolePermission err:", err);
    return [];
  }
};

export const getTeamLogActivity = async (body:any) => {
  try {
     const response = await CoreServices.post(body,'/get_myplan_team_log_activity');
    if (response && response?.data?.statusCode === 200) {
      const responseData = response?.data?.result;
      return responseData[0];
    } else {
      return [];
    }
  } catch (err) {
    console.log("getRolePermission err:", err);
    return [];
  }
};

export const getUserLogActivity = async (body:any) => {
  try {
     const response = await CoreServices.post(body,'/get_user_activity_log');
    if (response && response?.data?.statusCode === 200) {
      const responseData = response?.data?.result;
      return responseData;
    } else {
      return [];
    }
  } catch (err) {
    console.log("getRolePermission err:", err);
    return [];
  }
};

export const getQueueList = async () => {
  try {
      const response = await CoreServices.get(`/get_queue_list`);
      if (response?.data?.statusCode === 200) {
          const res = response?.data?.getQueueLists
          return res;
      }
      return [];
  } catch (error) {
      console.log(error)
  }
}

export const getBillingAndSubcriptionApi = async () => {
  try {
      const res = await CoreServices.get('/get_company_billing_subscription')
      if (res.data.statusCode === 200 && res.data.message?.length > 0) {
          return res?.data
      }
      return {}
  } catch (error) {
      return {}
  }
}

export const getStorageDatas = async () => {
  try {
      const response = await CoreServices.get(`/get_storage_data`);
      if (response?.data?.statusCode === 200) {
          const res = response?.data?.result
          return res;
      }
      return [];
  } catch (error) {
      console.log(error)
  }
}

export const getBotSessionsDomain_ccaas = async () => {
  try {
    const body = { channelType: 'All' };
    const accessToken = GetItemFromStorage(LocalStorage.ACCESS_TOKEN);
    const response = await axios.post(
      `${config().AI_CCAAS_REPORTSERVICE_URL}/get_used_no_of_session`, body, {
      headers: {
        appId: config().APPID,
        appSecret: config().AppSECRET,
        Authorization: accessToken,
      },
    })
    if (response?.data?.statusCode === 200) {
      return response?.data?.result ?? []
    }
    return []
  } catch (error) {
    console.log('getTrackerData Error', error)
    return []
  }
}

export const getStorageDatas_ccaas = async () => {
  try {
    const accessToken = GetItemFromStorage(LocalStorage.ACCESS_TOKEN);
    const response = await axios.get(
      `${config().AI_CCAAS_CORESERVICE_URL}/get_storage_data`, {
      headers: {
        appId: config().APPID,
        appSecret: config().AppSECRET,
        Authorization: accessToken,
      },
    })
    if (response?.data?.statusCode === 200) {
      const res = response?.data?.result
      return res;
    }
    return [];
  } catch (error) {
    console.log(error)
  }
}