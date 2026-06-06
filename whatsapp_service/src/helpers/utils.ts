import axios from 'axios';
import _ from 'lodash';
import moment from 'moment';
import { getBotDetail, getconnectData } from '../dao/whatsapp';
import { logger } from '../plugins/log';
import {  RESPONSE } from './constants';

// send Email
export async function sendEmail(toEmail: any, templateData: any) {
  try {
    const body = {
      to: toEmail?.primaryEmail,
      subject: templateData?.subject ?? null,
      css: templateData?.data?.template_body ?? null,
      html: templateData?.data?.template_logo_url_path ?? null,
    };
    await axios.post(process?.env?.sendEmail, body);
    return body;
  } catch (error) {
    logger.error('sendEmail err:', error);
  }
}

// send Email PlainText
/* tslint:disable */
export async function sendEmailPlainText(toEmail: any, templateData: any, req: any, adminEmail: any) {
  try {
    const connect: any = await getconnectData(req);
    let checkArray: any[] = [];
    if (connect?.connectorData) {
      const parsedData = JSON.parse(connect.connectorData);
      checkArray = Array.isArray(parsedData) ? parsedData : [parsedData];
    }
    const replaceContext = (msg: string) => {
      msg = msg.replace(/<p[^>]*>/g, '').replace(/<\/p>/g, '\n\n').replace(/\s+/g, ' ').trim();
      if (msg.includes('<ul>') && msg.includes('</ul>')) {
        msg = msg.replace(/<ul>/g, '\n').replace(/<\/ul>/g, '');
        msg = msg.replace(/<li>\s*/g, '• ').replace(/<\/li>/g, '\n');
      }
      let counter = 1;
      msg = msg.replace(/<ol>/g, '\n').replace(/<\/ol>/g, '');
      msg = msg.replace(/<li>\s*/g, () => `${counter++}. `).replace(/<\/li>/g, '\n');
      msg = msg.replace(/<b>|<\/b>|<strong>|<\/strong>/g, '');
      msg = msg.replace(/<em>/g, '_').replace(/<\/em>/g, '_');
      msg = msg.replace(/<s>/g, '~').replace(/<\/s>/g, '~');
      msg = msg.replace(/<a[^>]*>(.*?)<\/a>/g, '$1');
      msg = msg.replace(/\n\s*\n/g, '\n').trim();
      const context = contextMatch(msg);
      if (context?.length > 0) {
        for (const context_data of context) {
          let replaced = false;
          for (const temp_data of checkArray) {
            if (`context.${temp_data.key}` === context_data) {
              msg = msg.replace(`\${${context_data}}`, temp_data.value);
              replaced = true;
            }
          }
          if (!replaced) msg = msg.replace(`\${${context_data}}`, '');
        }
      }
      return msg;
    };

    // ✅ Send to Admin
    if (templateData.sendTo.includes('1')) {
      const adminMsg = replaceContext(templateData.message);
      await axios.post(process?.env?.sendEmailPlanText, {
        to: [adminEmail?.emailId],
        subject: templateData?.subject ?? '',
        text: adminMsg,
      });
    }

    // ✅ Send to Customer
    if (templateData.sendTo.includes('2') && toEmail?.primaryEmail) {
      const customerMsg = replaceContext(templateData.customerMessage);
      await axios.post(process?.env?.sendEmailPlanText, {
        to: [toEmail?.primaryEmail],
        subject: templateData?.subject ?? '',
        text: customerMsg,
      });
    }
    return { success: true, message: RESPONSE?.sendEmail };
  } catch (error) {
    logger.error('sendEmailPlainText err:', error);
    throw error;
  }
}

// get Product Feed List
export async function getProductFeedList(req: any) {
  try {
    const getId = await getBotDetail(req);
    const url = `${process.env.productFeed}/${getId[0]?.purchasedat}/${req?.calledNumber}`;
    const response = await axios.get(url);
    const responseData = response.data;
    return responseData;
  } catch (error) {
    logger.error('getProductFeedList err:', error);
    throw error;
  }
}

// get Products
export async function getProductsData(req: any, feed: any, count: any) {
  try {
    const getId = await getBotDetail(req);
    const url = `${process.env.products}/${getId[0]?.purchasedat}/${req?.calledNumber}/${feed}/${count}`;
    const response = await axios.get(url);
    const responseData = response.data;
    return responseData;
  } catch (error) {
    logger.error('getProductsData err:', error);
    throw error;
  }
}

// get Available Agent
export async function getAvailableAgent(presence: any) {
  const ext = Object.keys(presence)
    .filter((key) => {
      const statusName = JSON.parse(presence[key])?.statusName;
      return statusName === 'Ready';
    })
    .map(key => key.split('_')[1]);
  const presenceStatus = ext?.join(',');
  return { presenceStatus };
}

// get Start Node
export const getStartNode = (node: any) => {
  return node?.find((list: any) => list.type === 'entryPoint');
};

// get Start Node
export const getStartNodeConnecter = (node: any) => {
  return node?.find((list: any) => list.type === 'integration');
};

// get Start Node
export const getStartNodeCondition = (node: any) => {
  return node?.find((list: any) => list.type === 'checkCondition');
};

// get Node Data By Id
export const getNodeDataById = (nodeid: any, node: any) => {
  return node?.find((list: any) => list.id === nodeid);
};

/* tslint:disable */
export const contextMatch = (str: any) => {
  try {
    const regex = /\${(.*?)}/g;
    const matches = [];
    let match: any[];

    while ((match = regex.exec(str)) !== null) {
      matches.push(match[1]);
    }
    return matches;
  } catch (error) {
    console.log(error);
  }
};

export function safeParseJson(input: any) {
  if (typeof input !== "string") return input;
  try {
    return JSON.parse(input);
  } catch {
    return input;
  }
}

// contextPattern
export const contextPattern = /\${(.*?)}/g;

// replacePatternWithVariables
export const replacePatternWithVariables = (availableContexts: any, text: any, contexts: any) => {
  let originalText = text;
  for (const ctx of availableContexts) {
    const ctxString = ctx.toString();
    const variableName = ctxString.slice(2, ctxString.length - 1);
    const variable = variableName.split('.')[1];
    const specialPattern = /[${}]/g;
    originalText = originalText.replace(specialPattern, '');
    const patternString = `\\bcontext.${variable}\\b`; // Creating pattern with a dynamic string
    const pattern = new RegExp(patternString, 'gi');
    originalText = originalText?.replace(pattern, contexts[variable]);
  }
  return originalText;
};

/* tslint:disable */
export const returnVariableData = (str: any, integrationApiData: any, isImage: any) => {
  return new Promise((resolve) => {
    try {
      const regex = /\${(.*?)}/g;
      const matches = [];
      if (!isImage) {
        let match;
        while ((match = regex.exec(str)) !== null) {
          matches.push(match[1]);
        }
      } else {
        matches.push(str);
      }
      let i = 0;
      const desc_demo = [];
      for (const desc of matches) {
        const rows = desc.split('.').filter(row => row !== 0 && row !== '0' && row !== '' && row !== 'context');
        const temp = extractValues(integrationApiData?.data, rows);
        let j = 0;
        for (const temp_data of temp) {
          const replacementValue = temp_data[rows[rows.length - 1]];

          const regex = /\${(.*?)}/g;
          const matchess = [];
          let match: any[];

          while ((match = regex.exec(str)) !== null) {
            matchess.push(match[0]);
          }

          if (i === 0) {
            if (!isImage) {
              const result = str.replace(`\${${desc}}`, replacementValue);
              desc_demo.push(result);
            } else {
              const result = str.replace(`${desc}`, replacementValue);
              desc_demo.push(result);
            }
          } else {
            if (desc_demo[j] !== undefined) {
              const result = desc_demo[j].replace(`\${${desc}}`, replacementValue);
              desc_demo[j] = result;
            }
          }
          j++;
        }

        i++;
      }
      resolve(desc_demo);
    } catch (error) {
      console.log(error);
      resolve([]);
    }
  });
};

/* tslint:disable */
function extractValues(obj: any, desc: any) {
  if (!obj || !Array.isArray(desc) || desc.length === 0) {
    return [];
  }
  function extract(obj, desc, index = 0) {
    try {
      const key = desc[index].replace(/\s/g, '');

      if (obj && key in obj) {
        const value = obj[key];

        if (index === desc.length - 1) {
          return Array.isArray(value)
            ? value.map(item => ({ [key]: item }))
            : [{ [key]: value }];
        } {
          const nextDesc = desc.slice(index + 1);
          return Array.isArray(value)
            ? value.map(item => extract(item, nextDesc)).reduce((acc, arr) => acc.concat(arr), [])
            : extract(value, nextDesc);
        }
      }
      return [];
    } catch (error) {
      console.log(error);
    }
  }

  return extract(obj, desc);
}

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
    logger.error('hitEndpoint err', error);
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
    headers.forEach((header: any) => {
      if (Object.keys(header).length > 0) {
        formattedHeader[header.key] = header.value;
      }
    });
    formattedHeader['Content-Type'] = 'application/json';
    body.forEach((data: any) => {
      if (Object.keys(data).length > 0) {
        const regex_symbols = /[-!$%^&*()_+|~=`{}\[\]:\/;<>?,.@#]/;
        const test = regex_symbols.test(data?.value);
        const text = data?.value !== undefined ? data?.value : data.key === 'ip_address' ? '192.168.10.199' : '';
        const result = test ? text?.slice(10, text?.length - 1) : '';
        const final = finalObj[result] !== undefined ? finalObj[result] : data.key === 'ip_address' ? '192.168.10.199' : '';
        formattedBody[data.key] = test ? final : text;
      }
    });

    const results = await hitEndpoint(method, url, formattedHeader, formattedBody);
    return { results };
  } catch (error) {
    logger.error('fetchApi err', error);
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

const contextt = [];
export const addContextVariable = (key, value) => {
  if (contextt) {
    contextt[key] = value;
  }
  return contextt;
};

export const replacechatPatternWithVariables = (availableContexts: any, text: any, context: any) => {
  let originalText = text;
  for (const ctx of availableContexts) {
    const ctxString = ctx.toString();
    const variableName = ctxString.slice(2, ctxString.length - 1);
    const variable = variableName.split('.')[1];
    const specialPattern = /[${}]/g;
    originalText = originalText.replace(specialPattern, '');
    const patternString = `\\bcontext.${variable}\\b`; // Creating pattern with a dynamic string
    const pattern = new RegExp(patternString, 'gi');
    /* tslint:disable */
    const test = _.find(context, function (o) { return o.originalkey === variable; });
    console.log(test, 'pattern');
    // originalText = originalText?.replace(pattern, test[variable]);
    originalText = test?.originalkey === "registerdate" ? originalText?.replace(pattern, moment(new Date(test[variable])).format('YYYY-MM-DD'))
      : originalText?.replace(pattern, test[variable]);
  }
  return originalText;
};

export const replaceContextWithVariables = (text: any, context: any) => {
  const originalText = text;
  const variableName = originalText?.slice(2, originalText?.length - 1);
  const variable = variableName.split('.')[1];
  console.log(originalText, variableName, variable);
  if (context[variable]) {
    return context[variable];
  }
  return variable;
};

export const conditionsMapper = (leftOperand: any, condition: any, rightOperand: any) => {
  let left: any;
  let right: any;
  const leftType = isNaN(Number(leftOperand));
  const rightType = isNaN(Number(rightOperand));
  if (!leftType && !rightType) {
    left = Number(leftOperand);
    right = Number(rightOperand);
  } else {
    left = leftOperand;
    right = rightOperand;
  }
  const mappedCondition = conditionalOperator[condition];
  switch (mappedCondition) {
    case 'Equal to': {
      return left === right;
    }
    case 'Greater than': {
      return left > right;
    }
    case 'Greater than or Equal to': {
      return left >= right;
    }
    case 'Less than': {
      return left < right;
    }
    case 'Less than or Equal to': {
      return left <= right;
    }
    case 'Starts with': {
      left = left.toString();
      right = right.toString();
      return left?.at(0) === right;
    }
    case 'Ends with': {
      left = left.toString();
      right = right.toString();
      return left?.at(-1) === right;
    }
    case 'Contains': {
      left = left.toString();
      right = right.toString();
      return left?.includes(right);
    }
    default: {
      return false;
    }
  }
};

const conditionalOperator = {
  1: 'Equal to',
  2: 'Greater than',
  3: 'Greater than or Equal to',
  4: 'Less than',
  5: 'Less than or Equal to',
  6: 'Starts with',
  7: 'Ends with',
  8: 'Contains',
};

/**
 * 
 * @param plan_details 
 * @returns 
 */
export const getPlandetails = (plan_details: any) => {
  try {
    if (plan_details?.planId === 32) { //Xaia1Plus
      return 'Traditional'
    } else if (plan_details?.planId === 141) { //Xaia2Plus
      return plan_details?.isBot === 0 && plan_details?.whatsappFlowId === 0 ? 'Traditional' : 'CAI'
    } else if (plan_details?.planId === 33) { //Xaia3Plus
      return plan_details?.isBotPurchased === 1 && plan_details?.isBot === 1 ? (plan_details?.whatsappFlowId === 0 ? 'CAI' : 'Flow') : 'Traditional'
    } else if (plan_details?.planId === 42) { //Xaia4Plus
      return plan_details?.isBot === 0 && plan_details?.whatsappFlowId === 0 ? 'Traditional' : 'CAI'
    } else if (plan_details?.planId === 54) { //Xaia1
      return 'Traditional'
    } else if (plan_details?.planId === 103) { //Xaia2
      return plan_details?.isBot === 0 && plan_details?.whatsappFlowId === 0 ? 'Traditional' : 'CAI'
    } else if (plan_details?.planId === 56) { //Xaia3
      return plan_details?.isBotPurchased === 1 && plan_details?.isBot === 1 ? (plan_details?.whatsappFlowId === 0 ? 'CAI' : 'Flow') : 'Traditional'
    } else if (plan_details?.planId === 57) { //Xaia4
      return plan_details?.isBot === 0 && plan_details?.whatsappFlowId === 0 ? 'Traditional' : 'CAI'
    }
  } catch (error) {
    logger.error(error);
  }
}

/**
 * 
 * @param btninputCount 
 * @returns Parsed JSON data if successful, null otherwise
 */
export const correctjsondata = async (btninputCount: any) => {
  try {
    const parsedata = JSON.parse(btninputCount);
    return parsedata;
  } catch (error) {
    logger.error('Invalid JSON format:', error);
    return null;
  }
}

// get all agent list
export async function getAgentAvailibility(data: any) {
  try {
    const body = {
      domainId: data?.domainId
    };
    const agentData: any = await axios.post(process?.env?.agentAvailibility, body);
    return agentData?.data?.getAgentResponse;
  } catch (error) {
    logger.error('getAgentAvailibility err:', error);
  }
}

// Session ExpiryAPI
export const callSessionExpiryAPI = async (data: any) => {
  const payload = {
    isEmailSent: data?.isEmailSent,
    adminEmail: data?.adminEmail
  };

  try {
    const response = await axios.post(process.env.sessionExpiry, payload,
      {
        headers: {
          'Content-Type': 'application/json',
        },
      }
    );
    console.log('✅ API response:', response.data);
    return response.data;
  } catch (error: any) {
    logger.error('❌ Error calling session expiry API', error);
    throw error;
  }
};

// call Google Sheet SyncAPI
export function callGoogleSheetSyncAPI(data: any) {
  axios.post(process.env.googlesheet, data)
    .catch((err) => {
      logger.error('GoogleSheet sync call failed:', err);
    });
}

// crm contact create
export function insertUpdateCrmContact(data: any) {
  try {
    logger.info(`insertUpdateCrmContact whatsapp`, `${process.env.core_service}/crm_insert_contact - ${data}`);
    if (!data) return;
    const body = {
      domainId: data?.domainId ?? "",
      phone: data?.phone ?? "",
      email: data?.email ?? "",
      name: data?.name ?? "",
    }
    axios.post(`${process.env.core_service}/crm_insert_contact`, body)
      .then((res) => {
        logger.info('insertUpdateCrmContact res:', res);
      })
      .catch((err) => {
        logger.error('insertUpdateCrmContact failed:', err);
      });
  } catch (error) {
    logger.error('insertUpdateCrmContact ::: error:', error);
  }
}

export const buildMultiLinkMessage = (bot: any): string => {
  let text = '';
  if (bot?.data?.message) {
    text = bot?.data?.message
      .replace(/<\/p>/gi, '\n\n')
      .replace(/<p[^>]*>/gi, '')
      .replace(/<br\s*\/?>/gi, '\n')
      .replace(/<[^>]+>/g, '')
      .trim();
  }
  text += '\n\n';
  if (Array.isArray(bot?.data?.link)) {
    bot?.data?.link.forEach((item: any, index: number) => {
      text += `${index + 1}. ${item.text.replace(/\n/g, ' ').trim()}\n`;
      text += `${item.href}\n\n`;
    });
  }

  return text.trim();
};

