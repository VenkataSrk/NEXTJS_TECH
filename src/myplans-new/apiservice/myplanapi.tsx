// import { API_ROUTE_CONTROLLER } from "../../../base/routes";
// import { ApiServices } from "../../../../services/api_services";
import { MyplanProfileActions } from "../../store/slice/myplan/profile";
import { AICoreServices, CoreServices } from "../../base/service/apiservice";

export const getUserExtensionInfo = (payload:any) => {
  return AICoreServices.post(payload, "/get_my_plans_profile_info");
};
export const getUserManagementInfo = () => {
  return AICoreServices.get( "/get_user_details");
};



export const getOrderHistoryInfo = (payload:any) => {
  return AICoreServices.post(payload, "/get_my_plans_order_history");
};

export const getSubscriptionManagement = (payload:any) => {
  return AICoreServices.post(payload, "/get_my_plans_subscription_management");
};

export const getAddonList = (payload: {
  company_id: Number,
}) => async (dispatch: any) => {
  try {
    console.log("payloadpayload", payload)
    AICoreServices.post(payload, `/get_my_plans_addon_list`)
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
    const response = await AICoreServices.get('/get_user_department');
    if (response && response?.data?.statusCode === 200) {
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
    const response = await AICoreServices.get('/get_user_password_info');

    if (response && response?.data?.statusCode === 200) {
      const responseData = response?.data?.result;
      return responseData[responseData?.length-1];
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
    const response = await AICoreServices.post(body, '/get_syncup_myaccount_details');

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
    const response = await AICoreServices.post(body, '/create_myaccount_syncup_details');

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

// export const getRolesListInfo = (payload:any) => {
//   return AICoreServices.get( "/get_roles_info");
// };

export const getRolesListInfo = async (body: any) => {
  try {
    const response = await AICoreServices.post(body, '/get_roles_info');
    console.log("res???",response)
    if (response && response?.data?.statusCode === 200) {
      const responseData = response?.data?.result;
      return responseData;
    } else {
      return [];
    }
  } catch (err) {
    console.log("getRolesListInfo err:", err);
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