import { CoreServices } from "./apiservices";
import { UserSettingsActions } from "../../store/slice/settings_slice/user_settings_slice";
import store from "../../store/redux-store/root_store";

// eslint-disable-next-line react-hooks/rules-of-hooks

export const getBusinessprofileDetails = async () => {
    try {
        const endPoint = `/get_onboard_conversation_flow`;
        const response = await CoreServices.get(endPoint);
        if (response?.data?.statusCode !== 200) return null;

        const result = response?.data?.result;
        const flowNode = Array.isArray(result?.[0]) ? result?.[0]?.[0] : result?.[0];
        const profile = flowNode?.conversation_flow?.company_profile ?? null;
        if (profile) return profile;

        return null;
    } catch (error) {
        console.log(error);
        return null;
    }
};
export const getCustomHoursList = async () => {
    try {
        const endPoint = `/get_custom_hours_list`;
        const response = await CoreServices.get(endPoint);
        if (response?.data?.statusCode !== 200) return [];

        const result = response?.data?.getCustomHoursListList ?? response?.data?.result;
        if (Array.isArray(result?.[0])) {
            return result[0];
        }
        if (Array.isArray(result)) {
            return result;
        }
        return [];
    } catch (error) {
        console.log(error);
        return [];
    }
};

export const getUpdateBusinessHolidays = async (
    businessId: number | string,
    holidayId: number | string | null = null
) => {
    try {
        const endPoint = `/get_update_business_holidays?businessId=${businessId}&holidayId=${holidayId ?? "null"}`;
        const response = await CoreServices.get(endPoint);
        const result =
            response?.data?.result ??
            response?.data?.getUpdateBusinessHolidaysList ??
            response?.data?.getBusinessHolidayList ??
            [];

        if (response?.data?.statusCode && response?.data?.statusCode !== 200) {
            return [];
        }

        if (Array.isArray(result?.[0])) {
            return result[0];
        }
        if (Array.isArray(result)) {
            return result;
        }
        return [];
    } catch (error) {
        console.log(error);
        return [];
    }
};
export const getTeamManagementQueue = async () => {
    try {
        const endPoint = `/crm_get_team_management_queue`;
        const response = await CoreServices.get(endPoint);
        if (response?.data?.statusCode && response?.data?.statusCode !== 200) return [];

        const result = response?.data?.result ?? [];
        if (Array.isArray(result?.[0])) {
            return result[0];
        }
        if (Array.isArray(result)) {
            return result;
        }
        return [];
    } catch (error) {
        console.log(error);
        return [];
    }
};

export const getSmsConfigurationList = async () => {
    try {
        const endPoint = `/crm_getsmsConfigurationList`;
        const response = await CoreServices.get(endPoint);
        if (response?.data?.statusCode && response?.data?.statusCode !== 200) return [];

        const result = response?.data?.result ?? [];
        if (Array.isArray(result?.[0])) {
            
             store.dispatch(UserSettingsActions.setOperationHubChannelConfiguration({
                    key: "sms",
                    value: result[0] ?? []
                }))
            return result[0];
        }
        if (Array.isArray(result)) {
            return result;
        }
        return [];
    } catch (error) {
        console.log(error);
        return [];
    }
};

export const getCallConfigurationList = async () => {
    try {
        const endPoint = `/crm_getCallConfigurationList`;
        const response = await CoreServices.get(endPoint);
        if (response?.data?.statusCode && response?.data?.statusCode !== 200) return [];

        const result = response?.data?.result ?? [];
        if (Array.isArray(result?.[0])) {
            
            store.dispatch(UserSettingsActions.setOperationHubChannelConfiguration({
                    key: "call",
                    value: result[0] ?? []
                }))
            return result[0];
        }
        if (Array.isArray(result)) {
            return result;
        }
        return [];
    } catch (error) {
        console.log(error);
        return [];
    }
};

export const getEmailConfigDomain = async () => {
    try {
        const endPoint = '/get_email_config';
        const response = await CoreServices.get(endPoint);
        if (response?.data?.statusCode === 200) {
            try {
                store.dispatch(UserSettingsActions.setOperationHubChannelConfiguration({
                    key: "email",
                    value: response?.data?.result?.[0] ?? []
                }))
            } catch (error) {
                console.log("get_email_config", error)
            }
            return response?.data?.result?.[0] ?? [];
        }
        return [];
    } catch (error) {
        console.log(error);
        return [];
    }
};
