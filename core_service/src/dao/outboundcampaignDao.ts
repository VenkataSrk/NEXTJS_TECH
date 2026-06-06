/**
 * @param data
 * @returns
 */

import { mysqlPoolConnection } from '../../src/plugins/db';

export const insertUpdateOutboundCampaignDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('CALL ccaas_ai_insert_update_outbound_campaign_draft(?,?,?,?,?)', [
        data.sessionId,
        data.domainId,
        data.title,
        data.campaignMessage ? JSON.stringify(data.campaignMessage) : null,
        data.cid,
      ],                        (err, result) => {
        if (err) {
          console.log('err:', err);
          resolve(err);
        } else {
          const rows =
              Array.isArray(result) && Array.isArray(result[0])
                ? result[0]
                : result || [];

          resolve(rows);
        }
      });
    } catch (err) {
      console.log(err);
      reject(err);
    }
  });
};

export const getOutboundCampaignDraftDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('CALL ccaas_get_outbound_campaign_draft_chats(?,?,?)', [
        data.domainId,
        data.offset,
        data.limit,
      ],                        (err, result) => {
        if (err) {
          console.log('err:', err);
          resolve(err);
        } else {
          const rows =
              Array.isArray(result) && Array.isArray(result[0])
                ? result
                : result || [];

          resolve(rows);
        }
      });
    } catch (err) {
      console.log(err);
      reject(err);
    }
  });
};
