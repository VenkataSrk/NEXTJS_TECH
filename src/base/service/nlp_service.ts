import axios from "axios";
import { config } from "../constant";


export const sendEmailhandler = async(body: any) => {
    try {
        const endPoint = `${config().NLP_DNS_URL}/replay_email_crm`;
        const response = await axios
            .post(endPoint, body)
            .catch((error) => {
                console.log(error);
            });
        return response?.data;
    } catch (error) {
        console.log('NLP sendEmailhandler Error', error);
        return [];
    }
}

export const generateAIMessage = async (body: any) => {
    try {
        const endPoint = `${config().NLP_DNS_URL}/text_reprase`;
        const response = await axios.post(endPoint, body);
        return response?.data ?? null;
    } catch (error) {
        console.log(error);
        return null;
    }
};

export const updateNLPcontactDeals = async (data: any) => {
    try {
        const body = {
            customer_id: data?.coid,
            domain_id: data?.domainId,
            p_email: data?.emailId,
            p_phone_no: data?.phoneNumber
        }
        const endPoint = `${config().NLP_DNS_URL}/process-call-email`;
        const response = await axios.post(endPoint, body);
        return response?.data ?? null;
    } catch (error) {
        console.log(error);
        return null;
    }
};

export const updateNLPJoinMeet = async (data :any)=> {
    try {
        const res = await axios.post(`${config().NLP_DNS_URL}/process_meeting`, data);
        return res?.data
    } catch (error) {
        return null   
    }
}

export const generateEmailReplay = async (body: any) => {
    try {
        const endPoint = `${config().NLP_DNS_URL}/crm_draft_email`;
        const response = await axios.post(endPoint, body);
        return response?.data ?? null;
    } catch (error) {
        console.log(error);
        return null;
    }
};

export const generateNewEmailReplay = async (body: any) => {
    try {
        const endPoint = `${config().NLP_DNS_URL}/crm_email_content`;
        const response = await axios.post(endPoint, body);
        return response?.data ?? null;
    } catch (error) {
        console.log(error);
        return null;
    }
};

export const getInboxRecords = async (body: any) => {
    try {
        const endPoint = `${config().NLP_DNS_URL}/inbox_query_executer`;
        const response = await axios.post(endPoint, body);
        return response?.data ?? null;
    } catch (error) {
        console.log(error);
        return null;
    }
};

export const getMarketingHomePage = async (body: any) => {
    try {
        const url = `${config().NLP_DNS_URL}/marketing_home_page_crm`;
        const response = await axios.post(url, { domainId: body?.domainId });
        return response?.data ?? null;
    } catch (error) {
        console.log(error);
        return null;
    }
};