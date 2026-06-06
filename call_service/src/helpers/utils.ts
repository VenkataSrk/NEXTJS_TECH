import axios from 'axios';
import { logger } from '../plugins/winston';

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

    const response: any = await axios.post(
      process?.env?.Sentimental,
      body,
      {
        headers: { 'Content-Type': 'application/json' },
        timeout: 5000
      }
    );
    return response ?? {};
  } catch (error) {
    logger.error('sentimentalScore', data, error);
    return {};
  }
}

// ticketing auto assign
/* export async function ticketingAutoassign(data: any) {
  try {
    const response = await axios.post('http://173.234.75.165:4025/live_ticketing_autoassign', data, {
      headers: {
        'Content-Type': 'application/json'
      }
    });
    return response.data;
  } catch (error) {
    logger.error('ticketingAutoassign', data, error)
    throw error;
  }
} */

// get location details
export const getLocationDetails = async (data: any) => {
  try {
    const response = await axios.get(`https://ipinfo.io/${data?.IPAddress}`);
    if (response.status >= 200) {
      return response.data;
    }
  } catch (error) {
    logger.error('getLocationDetails', data, error);
  }
};

export async function aiSummarySubject(data: any) {
  try {
    const response = await axios.post(process?.env?.AI_SUMMARY, data, {
      headers: {
        'Content-Type': 'application/json'
      },
      timeout: 5000
    });
    return response?.data ?? null;
  } catch (error) {
    console.error('aiSummarySubject error:', error);
    return null;
  }
}

export async function aiRegenerate(data: any) {
  try {
    const response = await axios.post(process?.env?.REGENERATE, data, {
      headers: {
        'Content-Type': 'application/json'
      },
      timeout: 5000
    });
    return response?.data ?? {};
  } catch (error) {
    logger.error('aiRegenerate', data, error);
    return {};
  }
}
export async function hubSpotApi(data: any) {
  try {
    console.log('HUBSPOT_API>>>>>>>>', data);
    const response = await axios.post(process?.env.HUBSPOT_API, data, {
      headers: {
        'Content-Type': 'application/json'
      }
    });
    console.log('HUBSPOT_API>>>>>>>>', response.data);
    return response.data;
  } catch (error) {
    logger.error('HUBSPOT_API', data, error);

  }
}
