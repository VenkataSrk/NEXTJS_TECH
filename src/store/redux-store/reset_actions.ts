import * as Helper from "../../constants/reducer_names";
import commonSlice from "../slice/commonSlice/commonSlice";
import contactSlice from "../slice/commonSlice/contactSlice";
import userExtensionSlice from "../slice/myplan/profile";
import emailSlice from "../slice/commonSlice/emailSlice";
import knowledge from "../slice/knowledgeBaseChat"
import contacts from '../slice/contacts';
import campaign from "../slice/campaign";
import analytics from "../slice/analytics";
import report from "../slice/report";



export const commonSliceReducerWrapper = (state: any, action: any) => {
  if (action.type === `${Helper.COMMON_DATA}/reset`) {
    state = undefined;
  }
  return commonSlice(state, action);
};

export const contactSliceReducerWrapper = (state: any, action: any) => {
  if (action.type === `${Helper.CONTACT_DATA}/reset`) {
    state = undefined;
  }
  return contactSlice(state, action);
};

export const emailSliceReducerWrapper = (state: any, action: any) => {
  if (action.type === `${Helper.CONTACT_DATA}/reset`) {
    state = undefined;
  }
  return emailSlice(state, action);
};

export const knowledgeReducerWrapper = (state: any, action: any) => {
  if (action.type === `${Helper.KNOWLEDGE_DATA}/reset`) {
    state = undefined;
  }
  return knowledge(state, action);
};
export const contactsReducerWrapper = (state: any, action: any) => {
  if (action.type === `${Helper.CONTACTS_DATA}/reset`) {
    state = undefined;
  }
  return contacts(state, action);
};
export const campaignReducerWrapper = (state: any, action: any) => {
  if (action.type === `${Helper.CAMPAIGN_DATA}/reset`) {
    state = undefined;
  }
  return campaign(state, action);
};

export const analyticsReducerWrapper = (state: any, action: any) => {
  if (action.type === `${Helper.ANALYTICS_DATA}/reset`) {
    state = undefined;
  }
  return analytics(state, action);
};

export const reportReducerWrapper = (state: any, action: any) => {
  if (action.type === `${Helper.REPORT_DATA}/reset`) {
    state = undefined;
  }
  return report(state, action);
};


export const userExtensionReducerWrapper = (state: any, action: any) => {
  if (action.type === `${Helper.MY_PLANS}/reset`) {
    state = undefined;
  }

  return userExtensionSlice(state, action);
};