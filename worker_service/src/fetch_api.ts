import axios from 'axios';
import _ from 'lodash';
import { logger } from './plugins/log';
import { loggerError } from './plugins/logger';

export const hitEndpoint = async (method: any, url: any, headers: any, body: any) => {
  try {
    switch (method) {
      case 'post': {
        return await axios.post(url, body, {
          headers,
        });
      }
      case 'put': {
        return await axios.put(url, body, {
          headers,
        });
      }
      case 'delete': {
        return await axios.delete(url, {
          headers,
        });
      }
      default: {
        return await axios.get(url, {
          headers,
        });
      }
    }
  } catch (error: any) {
    logger.error('hitEndpoint', { payload: { method, url, headers, body }, errorMessage: error });
    return error;
  }
};

export const fetchApi = async ({ urlEndPoint, urlMethod, urlBody, urlHeaders }: any) => {
  try {
    const method = urlMethod ?? 'get';
    const url = urlEndPoint;
    const headers = urlHeaders ?? [];
    const body = urlBody ?? [];
    const formattedHeader = {} as any;
    const formattedBody = {} as any;
    headers.forEach((header: any) => {
      if (Object.keys(header).length > 0) {
        formattedHeader[header.key] = header.value;
      }
    });
    formattedHeader['Content-Type'] = 'application/json';
    body.forEach((data: any) => {
      if (Object.keys(data).length > 0) {
        formattedBody[data.key] = data?.value;
      }
    });
    const result = await hitEndpoint(method, url, formattedHeader, formattedBody);
    return { result };
  } catch (error) {
    logger.error('fetchApi', { payload: { urlEndPoint, urlMethod, urlBody, urlHeaders }, errorMessage: error });
    return { error };
  }
};

export const getResponseData = (response: any) => {
  try {
    let responseData = response;
    while (responseData?.data || responseData?.results || responseData?.result) {
      if (responseData?.data) responseData = responseData?.data;
      if (responseData?.result) responseData = responseData?.result;
      if (responseData?.results) responseData = responseData?.results;
    }
    if (responseData) {
      return responseData;
    }
  } catch (error) {
    loggerError('getResponseData', response, error);
  }
};
export const nlpSummary = async (data: any) => {
  try {
    const body = {
      mobile_number: data?.cli ?? 0,
      session_id: data?.session_id ?? 0,
      domain_id: data?.domain_id ?? 0
    };
    console.log('nlpSummary body', body);
    const response = await axios.post(process?.env?.nlpSummary, body, {
      headers: {
        'Content-Type': 'application/json'
      }
    }
    );
    if (response) {
      console.log(' nlpSummary response', response?.data);
    }
    return;
  } catch (error) {
    console.log('nlpSummary error', data, error);
    loggerError('nlpSummary error', data, error);
  }
};
