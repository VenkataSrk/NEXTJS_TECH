import axios from 'axios';
import { loggerError, loggerTrace } from './log';

// send nlp sms CAI bot
export async function getSmsBot(nlpResult: any, companyDetails: any, content: string, activeIsBot:any) {
  try {
    const body = {
      query: content ?? null,
      domine_id: nlpResult?.domainId,
      website_id: '',
      company_name: companyDetails?.companyName ?? '',
      industry_type: companyDetails?.industryType ?? '',
      customerId: nlpResult?.customerNumber ?? null,
      isBot: activeIsBot?.isBot ?? 0
    };
    console.log('SMS Bot Request send:', body);
    const response = await axios.post(process?.env?.smsBot, body);
    console.log('SMS Bot from Response', response?.data);
    return response?.data;
  } catch (error) {
    console.error('Error sending message:', error);
  }
}

function stripHtml(html: string): string {
  return html?.replace(/<[^>]*>/g, '');
}
// send Message Customer
export async function sendSmsMessageCustomer(req: any, message: any) {
  try {
    const body = {
      source: req?.calledNumber,
      destination: req?.customerNumber,
      content: stripHtml(message)
    };
    console.log('SMS send to  Request smsout:', body);
    const response = await axios.post(process?.env?.smsOut, body);
    return response;
  } catch (error) {
    loggerError('Error sending message:', error, {});
    console.error('Error sending message:', error);
  }
}

// senti mental Score
export async function sentimentalScore(data: any) {
  try {
    const body = {
      HistoryId: data?.HistoryId,
      ChatDurationHistoryId: data?.ChatDurationHistoryId ?? 0,
      channelType: data?.channelType,
      domainId: data?.domainId,
      ext: parseInt(data?.ext, 10) || 0
    };
    loggerTrace('sentimentalScore - Request', body, {});
    const response: any = await axios.post(
      process?.env?.Sentimental,
      body,
      {
        headers: { 'Content-Type': 'application/json' },
        timeout: 5000
      }
    );
    loggerTrace('sentimentalScore - Response', body, response?.data);
    return response ?? {};
  } catch (error) {
    return {};
  }
}

// send Message Customer
export async function sendSmsMessageOutbound(data: any) {
  try {
    const body = {
      source: data?.from,
      destination: data?.to,
      content: data?.text
    };
    loggerTrace('sendSmsMessageOutbound - Request', body, {});
    const response = await axios.post(process?.env?.smsOut, body);
    loggerTrace('sendSmsMessageOutbound - Response', body, response?.data);
    return body;
  } catch (error) {
    loggerError('sendSmsMessageOutbound - Error sending message', data, error);
    console.error('Error sending message:', error);
  }
}

// call Google Sheet SyncAPI
export function callGoogleSheetSyncAPI(data: any) {
  axios.post(process.env.googlesheet, data)
    .catch((err) => {
      console.error('GoogleSheet sync call failed:', err);
      loggerError('GoogleSheet sync call failed:', err, {});

    });
}
