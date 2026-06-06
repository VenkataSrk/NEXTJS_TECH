import axios from 'axios';
import _ from 'lodash';
import nodeHtmlToImage from 'node-html-to-image';
import { mysqlPoolConnection } from '../../src/plugins/db';
import { configs } from '../config/app';
import { getConnectorTokenInfo, getMarketingContactId, insertUpdateMarketing } from '../dao/customerContact';
import { logger } from '../plugins/winston';
import { CRM_DOMAINS } from './constants';
const fs = require('fs');

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
    logger.error('sentimentalScore - data', data);
    return response ?? {};
  } catch (error) {
    logger.error('sentimentalScore', data, error);
    return {};
  }
}

/**
 *
 * @param cssData
 * @param dataHtml
 * @param name
 * @returns
 */
export async function generateImage(cssData: any, dataHtml: any, name: any) {
  return new Promise(async (resolve) => {
    try {
      const components = `<!DOCTYPE html>
  <html>
  <head>
    <title>Email Template</title>
    <meta name="googlebot" content="noindex" />
    <meta content="width=device-width, initial-scale=1.0" name="viewport">
    <META NAME="ROBOTS" CONTENT="NOINDEX, NOFOLLOW" />
    <style>
      ${cssData}
    </style>
  </head>
      ${dataHtml}
  </html>`;
      const screenshotPath = `${configs.paths.upload_destination}${name}.png`;
      await nodeHtmlToImage({
        output: screenshotPath,
        html: components,
      });
      const url = `${configs.paths.fileGetUrl}${name}.png`;
      resolve(url);
    } catch (error) {
      logger.error('generateImage', error);
      resolve(null);
    }
  });
}

export function isJSON(str: any) {
  try {
    return JSON.parse(str) && !!str;
  } catch (e) {
    return false;
  }
}

/**
 *
 * @param data
 * @returns
 */
export const isSelect = () => {
  return new Promise((resolve, reject) => {
    try {
      const mysqlq = 'SELECT 1';
      mysqlPoolConnection.query(mysqlq, (err: any, result: any) => {
        if (err) {
          reject(err);
        } else {
          resolve(result);
        }
      });
    } catch (error) {
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const crmInsertContact = (data: any) => {
  try {
    logger.info(`crmInsertContact incomming: ${data.domainId} -- ${JSON.stringify(data)}`);
    if (`${data.domainId}` === CRM_DOMAINS.LORIO && `${data.phone}`?.trim()) {
      const body = {
        name: data?.name?.trim() || '',
        phone: `${data?.phone}`?.trim() || '',
        email: `${data?.email}`?.trim() || '',
      };
      LorioInsertContact(body);
    }
  } catch (error) {
    logger.info(`crmInsertContact Error: ${JSON.stringify(data)}`, error);
  }
};

export const LorioInsertContact = (body: any) => {
  return new Promise((resolve, reject) => {
    try {
      if (!configs.lorio_tele_crm_api) {
        logger.info('LorioInsertContact Api not available');
        console.log('LorioInsertContact Api not available');
        resolve('LorioInsertContact Api not available');
        return;
      }
      const payload = { fields: body };
      logger.info(`LorioInsertContact payload:${configs.lorio_tele_crm_api} - ${JSON.stringify(payload)}`);
      console.log(`LorioInsertContact payload:${configs.lorio_tele_crm_api} - ${JSON.stringify(payload)}`);
      axios.post(configs.lorio_tele_crm_api, payload, {
        headers: {
          Authorization: configs.lorio_auth_token
        },
        timeout: 3000,
      })
        .then((res) => {
          console.log(`LorioInsertContact Response: ${JSON.stringify(payload)}`);
          logger.info(`LorioInsertContact Response: ${JSON.stringify(payload)}`);
          resolve(res.data);
        })
        .catch((err) => {
          console.log(`LorioInsertContact Error: ${JSON.stringify(payload)}`, err);
          logger.info(`LorioInsertContact Error: ${JSON.stringify(payload)}`, err);
          resolve(err);
        });
    } catch (error) {
      console.log(`LorioInsertContact Error: ${JSON.stringify({ fields: body })}`, error);
      logger.info(`LorioInsertContact Error: ${JSON.stringify({ fields: body })}`, error);
      resolve(error);
    }
  });
};

// call Google Sheet SyncAPI
export const insertGoogleConnectorHandler = (domainId: any, coid: any) => {
  try {
    console.log('Google Connector incomming :: ', domainId, coid);
    if (!domainId || !coid) return;

    getConnectorTokenInfo(domainId, (getConnect) => {
      if (getConnect?.length > 0) {

        logger.info('Google Connector token found:', getConnect[0]);
        console.log('Google Connector token found:', getConnect[0]);

        const data = {
          domainId,
          contactSyncDetails: getConnect[0]?.contactSyncDetails,
          newContacts: coid,
          token: getConnect[0]?.token
        };

        logger.info('Google Connector insert payload:', getConnect[0]);
        console.log('Google Connector insert payload:', getConnect[0]);

        logger.info('Google Connector API:', process.env.googlesheet);
        console.log('Google Connector API:', process.env.googlesheet);

        axios
          .post(process.env.googlesheet, data)
          .catch((err) => {
            logger.error('GoogleSheet sync call failed:', err);
            console.log('GoogleSheet sync call failed:', err);
          });
      }

    });
  } catch (error) {
    logger.error('GoogleSheet insertGoogleConnectorHandler failed:', error);
    console.log('GoogleSheet insertGoogleConnectorHandler failed:', error);
  }
};

// other doamins insert common funtion
export const commonContactInsert = async (data: any, auth: any, insertContactRes: any) => {
  const domainID = auth?.domainId ? auth.domainId : data?.domainId;
  // crm insert
  try {
    console.log('insertUpdateCrmContact - Insert');
    const body = {
      domainId: domainID,
      phone: data?.primaryPhoneNumber ?? '',
      email: data?.primaryEmail ?? '',
      name: data?.firstName ?? '',
    };
    crmInsertContact(body);
  } catch (error) {
    logger.error('insertUpdateCrmContact ::: error:', error);
    console.log('insertUpdateCrmContact ::: error:', error);
  }

  // google sheet insert
  try {
    if (data.source === 'Pre Chat Form') {
      const coid = insertContactRes?.coid || data?.coid;
      logger.info('insertGoogleConnectorHandler ::: Chatbot', coid);
      console.log('insertGoogleConnectorHandler ::: Chatbot', coid);
      insertGoogleConnectorHandler(domainID, coid);
    }
  } catch (error) {
    logger.error('insertUpdateCrmContact ::: error:', error);
  }

  // marketing insert
  // try {
  //   logger.info('insertUpdateMarketing :: ', data.coid);
  //   await getMarketingContactId(data, auth)
  //     .then((getMarketing :any)=> {
  //       logger.info('insertUpdateMarketing', getMarketing);
  //       const newObj = {
  //         ...data,
  //         user_contact_id: getMarketing?.finalresponse[0] ? getMarketing?.finalresponse[0]?.user_contact_id : 0
  //       };
  //       insertUpdateMarketing(newObj, auth);
  //     })
  //     .catch((error)=> {
  //       console.log('insertUpdateMarketing ::: error:', error)
  //     });

  // } catch (error) {
  //   logger.error('insertUpdateMarketing ::: error:', error);
  //   console.log('insertUpdateMarketing ::: error:', error);
  // }
};
