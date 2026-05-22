import { AiOnboardingAction } from "../store/slice/onboardingSlice/onboardingSlice";
import { CoreServices } from "./service/apiservices";

export const EnterPriceDefaultConfig = [
  {
    key: 'companyProfile',
    label: 'Company & Business Profile',
    desc: '',
    isCompleted: 0,
    step: '1',
  },
  {
    key: 'leadSources',
    label: 'Lead Sources & Entry Points',
    desc: '',
    isCompleted: 0,
    step: '2',
  },
  {
    key: 'productsAndPlans',
    label: 'Products, Plans & Catalog',
    desc: '',
    isCompleted: 0,
    step: '3',
  },
  {
    key: 'communicationChannels',
    label: 'Communication Channel Configuration',
    desc: '',
    isCompleted: 0,
    step: '4',
  },
  {
    key: 'teamManagement',
    label: 'Team Management',
    desc: '',
    isCompleted: 0,
    step: '5',
  },
];

export const getUserOnboardDetails = async (dispatch?: any) => {
  try {
    const { data }: any = await CoreServices.get('/get_user_onboard_settings_completion');

    const item = data?.getuserOnboardData?.[0];

    if (data?.statusCode === 200 && item?.flow_track?.length) {
      dispatch?.(AiOnboardingAction.updateOnBoardingStatus(item.flow_track));

      return {
        isCompleted: item?.isCompleted ? 1 : 0,
        isEmpty: false,
      };
    }

    dispatch?.(AiOnboardingAction.updateOnBoardingStatus(EnterPriceDefaultConfig));

    return { isCompleted: 0, isEmpty: true };

  } catch (e) {
    console.log('getUserOnboardDetails Error', e)
    dispatch?.(AiOnboardingAction.updateOnBoardingStatus(EnterPriceDefaultConfig));
    return { isCompleted: 0, isEmpty: true };
  }
};

export const inserUpdateOnboardDetails = async (data: any) => {
  try {
    const endPoint = '/insert_update_user_onboard_settings_completion';
    const response = await CoreServices.post(data, endPoint);
    if (response?.data?.statusCode === 200) {
      return response.data;
    }
    return null;
  } catch (error) {
    console.log('inserUpdateOnboardDetails Error', error);
    return null;
  }
};

export const handleFileUpload = async (file: File) => {
    try {
        const formData = new FormData();
        formData.append("doc", file);
        const res = await CoreServices.post(formData, `/file_upload`);
        if (res.data.statusCode === 200) {
            return res.data.fileList;
        }else{
            return [];
        }
    } catch (error) {
        console.log("handleFileUpload failed:", error);
        return [];
    }
};

export const getOnboardingCompletion = (items: any[]) => {
  try {
    if (!items || items?.length === 0) return 0;

    const total = items?.length;

    const completed = items?.filter( (item) => Number(item?.isCompleted) === 1).length;

    return Math.round((completed / total) * 100);
  } catch (error) {
    return 0;
  }
};