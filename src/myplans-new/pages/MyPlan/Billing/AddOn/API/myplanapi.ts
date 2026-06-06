import axios from "axios"
import { config } from "../../../../../../base/constant"
import { LocalStorage } from "../../../../../../base/customhooks/localStorageKeys"
import { GetItemFromStorage } from "../../../../../../base/customhooks/useStorage"
import { CRMCoreServices } from "../../../../../../base/service/apiservice"

export const getBillingAndSubcriptionApi = async () => {
  try {
      const res = await CRMCoreServices.get('/get_company_billing_subscription')
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
      const response = await CRMCoreServices.get(`/get_storage_data`);
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