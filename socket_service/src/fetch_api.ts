import axios from 'axios';
import _ from 'lodash';
import { getBotDetail } from './dao/webChat.dao';

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
    console.log('error-----------', error);
    return error;
  }
};

export const fetchApi: any = async ({ urlEndPoint, urlMethod, urlBody, urlHeaders, inputParams }: any) => {
  try {
    const method = urlMethod ?? 'get';
    const url = urlEndPoint;
    const headers = urlHeaders ?? [];
    const body = urlBody ?? [];
    const formattedHeader = {} as any;
    const formattedBody = {} as any;
    const finalObj = {};
    for (let i = 0; i < (inputParams ? inputParams.length : 0); i += 1) {
      Object.assign(finalObj, inputParams[i]);
    }
    console.log(inputParams, 'inputParamsinputParamsinputParams');
    headers.forEach((header: any) => {
      if (Object.keys(header).length > 0) {
        formattedHeader[header.key] = header.value;
      }
    });
    formattedHeader['Content-Type'] = 'application/json';
    body.forEach((data: any) => {
      if (Object.keys(data).length > 0) {
        const regex_symbols = /\${context\.(\w+)}/;
        const text = data?.value !== undefined ? data?.value : '';
        const test = text.trim().match(regex_symbols);
        const result = test ? test[1] : null;
        const final = finalObj[result] !== undefined ? finalObj[result] : '';
        formattedBody[data.key] = test ? final : text;
        console.log(finalObj, result, finalObj[result], 'resultresultresultresult88');
      }
    });

    const results = await hitEndpoint(method, url, formattedHeader, formattedBody);
    return { results };
  } catch (error) {
    return { error };
  }
};

export const getResponseData = (response: any) => {
  let responseData = response;
  while (responseData?.data || responseData?.results || responseData?.result || responseData?.topupAmountResRes) {
    if (responseData?.data) responseData = responseData?.data;
    if (responseData?.result) responseData = responseData?.result;
    if (responseData?.results) responseData = responseData?.results;
    if (responseData?.topupAmountResRes) responseData = responseData?.topupAmountResRes;
  }
  if (responseData) {
    return responseData;
  }
};

export const replaceDynamicEndPoint = (url: any, inputParams: any) => {
  const contextApiId = url.match(/\${(.*?)}/)[1].replace('context.', '');
  const contextApiIdOrg = url.match(/\${(.*?)}/)[1];
  const index = inputParams.findIndex((item: any) => Object.keys(item).includes(contextApiId));
  if (index !== -1) {
    const updatedUrl = url.replace(`\${${contextApiIdOrg}}`, inputParams[index][contextApiId]);
    return updatedUrl;
  }
  return url;

};

// send message
export async function sendMessage(req: any) {
  try {
    const getId = await getBotDetail(req);
    const body = {
      id: getId[0]?.purchasedat,
      from: req?.agentNumber,
      to: `+${req?.customerNumber}`,
      content: 'Our agents are currently busy. Please feel free to leave a message with a brief summary of your inquiry along with your contact information, or you can get in touch with us at a later time.'
    };
    const response = await axios.post(process.env.whatsappOut, body);
    return body;
  } catch (error) {
    console.error('Error sending message:', error);
  }
}

// send message facebook
export async function sendMessageFacebook(req: any) {
  try {
    const body = {
      domainId: req?.domainId,
      customerId: req?.customerNumber,
      pageId: req?.agentNumber,
      message: 'Our agents are currently not available. Please feel free to leave a message with a brief summary of your inquiry along with your contact information, or you can get in touch with us at a later time.'
    };
    const response = await axios.post(process.env.FacebookOut, body);
    return body;
  } catch (error) {
    console.error('Error sending message:', error);
    // You can handle the error here, e.g., log it or return a specific response
  }
}

// send message connect live agent
export async function sendConnectLiveAgent(req: any) {
  try {
    const getId = await getBotDetail(req);
    const body = {
      id: getId[0]?.purchasedat,
      from: req?.agentNumber,
      to: `+${req?.customerNumber}`,
      content: `Your conversation has been transferred to ${req?.agentName}`
    };
    const response = await axios.post(process.env.whatsappOut, body);
    return body;
  } catch (error) {
    console.error('Error sending message:', error);
  }
}

// send message connect live agent
export async function sendConnectLiveAgentNotification(req: any) {
  try {
    const getId = await getBotDetail(req);
    const body = {
      id: getId[0]?.purchasedat,
      from: req?.agentNumber,
      to: `+${req?.customerNumber}`,
      content: 'Connecting to live agent...'
    };
    await axios.post(process.env.whatsappOut, body);
    return body;
  } catch (error) {
    console.error('Error sending message:', error);
    // You can handle the error here, e.g., log it or return a specific response
  }
}

// senti mental Score
export async function sentimentalScoreData(data: any) {
  try {
    const body = {
      HistoryId: data?.historyId,
      ChatDurationHistoryId: data?.ChatDurationHistoryId ?? 0,
      channelType: data?.channelType,
      domainId: data?.domainId,
      ext: parseInt(data?.ext, 10) ?? 0
    };
    return await axios.post(process?.env?.Sentimental, body);
  } catch (error) {
    console.error('Error sending message:', error);
    // You can handle the error here, e.g., log it or return a specific response
  }
}

// chat deflection qa
export async function chatDeflectionData(data: any) {   //
  try {
    const body = {
      HistoryId: data?.historyId,
      ChatDurationHistoryId: data?.ChatDurationHistoryId ?? 0,
      channelType: data?.channelType,
      domainId: data?.domainId,
      ext: data?.ext ?? 0
    };
    return await axios.post(process?.env?.Deflection, body);
  } catch (error) {
    console.error('Error sending message:', error);
    // You can handle the error here, e.g., log it or return a specific response
  }
}

// send message connect live agent
export async function sendConnectLiveAgentFB(data: any) {
  try {
    const body = {
      domainId: data?.domainId,
      customerId: data?.customerNumber,
      pageId: data?.agentNumber,
      type: 'text',
      message: `Your conversation has been transferred to ${data?.agentName}`
    };
    const response = await axios.post(process.env.FacebookOut, body);
    return body;
  } catch (error) {
    console.error('Error sending message:', error);
  }
}

// send message connect live agent
export async function sendConnectLiveAgentFBNotification(data: any) {
  try {
    const body = {
      domainId: data?.domainId,
      customerId: data?.customerNumber,
      pageId: data?.agentNumber,
      type: 'text',
      message: 'Connecting to live agent...'
    };
    const response = await axios.post(process.env.FacebookOut, body);
    return body;
  } catch (error) {
    console.error('Error sending message:', error);
  }
}

// send message connect live agent
export async function sendConnectLiveAgentInsta(data: any) {
  try {
    const body = {
      domainId: data?.domainId,
      customerId: data?.customerNumber,
      pageId: data?.agentNumber,
      type: 'text',
      message: `Your conversation has been transferred to ${data?.agentName}`
    };
    const response = await axios.post(process.env.InstagramOut, body);
    return body;
  } catch (error) {
    console.error('Error sending message:', error);
  }
}

// send message connect live agent
export async function sendConnectLiveAgentInstaNotification(data: any) {
  try {
    const body = {
      domainId: data?.domainId,
      customerId: data?.customerNumber,
      pageId: data?.agentNumber,
      type: 'text',
      message: 'Connecting to live agent...'
    };
    const response = await axios.post(process.env.InstagramOut, body);
    return body;
  } catch (error) {
    console.error('Error sending message:', error);
  }
}

// send Button Link for callback
export async function sendCallbackRequest(req: any, callbackUrl: any) {
  try {
    const getId = await getBotDetail(req);
    const body = {
      id: getId[0]?.purchasedat,
      from: req?.agentNumber,
      to: `+${req?.customerNumber}`,
      templateName: process.env.templateNameCallBack,
      type: 'cta_template_body_buttons',
      buttonUrl: callbackUrl,
    };
    await axios.post(process?.env?.sendCallbackRequest, body);
    return body;
  } catch (error) {
    console.error('Error sending message:', error?.response);
    // You can handle the error here, e.g., log it or return a specific response
  }
}
