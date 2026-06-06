/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

import axios from 'axios';
import request from 'request';
import { configs } from '../config/app';
// const request = require('request');
class Hubspot {
  public host: string;
  public apikey: string;
  constructor() {
    this.host = configs.hubspot.host;
    this.apikey = configs.hubspot.apikey;
  }

  /**
   * Description: get contact id
   * @param data any
   */
  public async contactIdSearch(email: any) {
    const search = configs.hubspot.host;
    const key = configs.hubspot.apikey;
    const options = {
      method: 'POST',
      url: search,
      headers: {
        accept: 'application/json',
        'content-type': 'application/json',
        Authorization: `Bearer ${key}`,
      },
      body: {
        filterGroups: [
          {
            filters: [{ value: email, propertyName: 'email', operator: 'EQ' }],
          },
        ],
      },
      json: true,
    };
    return new Promise(async (resolve, reject) => {
      try {
        return request(options, (error: any, response: any, body: any) => {
          if (error) throw new Error(error);
          resolve(body.results);
        });
      } catch (err) {
        reject(err);
      }
    });
  }

  /**
   * Description: hubSpot SendMail
   * @param conatctId
   * @param data
   */
  public hubSpotSendMail(conatctId: any, data: any) {
    const key = configs.hubspot.apikey;
    return new Promise(async (resolve, reject) => {
      try {
        return await axios
          .patch(`https://api.hubapi.com/crm/v3/objects/contacts/${conatctId}`, data, {
            headers: { accept: 'application/json', 'content-type': 'application/json', Authorization: `Bearer ${key}` },
          })
          .then((response: any) => {
            resolve(response.status);
          })
          .catch((error) => {
            reject(error);
          });
      } catch (err) {
        reject(err);
      }
    });
  }
}
export default Hubspot;
