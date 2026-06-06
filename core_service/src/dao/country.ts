
/**
 *
 * @param data
 * @returns
 */
import { mysqlPoolConnection } from '../plugins/db';
import { logger } from '../plugins/winston';
export const getCountry = () => {
  return new Promise((resolve, reject) => {
    try {
      const mysqlq = 'SELECT * FROM country';
      mysqlPoolConnection.query(mysqlq, (err, result) => {
        if (err) {
          logger.error('getCountry - Unexpected Error:', err);
          reject(err);
        } else {
          resolve(result);
        }
      });
    } catch (error) {
      logger.error('getCountry - Unexpected Error:', error);
      reject(error);
    }
  });
};

/**
 *  check exists user
 */
export const checkExistsUserDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call up_CheckChatIPAddress(?,?)', [
        data?.domainId,
        data?.ipAddress
      ],                        (err: any, result: any) => {
        if (err) {
          logger.error('up_CheckChatIPAddress - Unexpected Error:', err);
          reject(err);
        }
        resolve(result);
      });
    } catch (err) {
      logger.error('up_CheckChatIPAddress - Unexpected Error:', err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getCountryConfigDao = () => {
  return new Promise((resolve, reject) => {
    try {
      const mysqlq = 'CALL ccaas_get_country_config()';
      mysqlPoolConnection.query(mysqlq, (err: any, result: any) => {
        if (err) {
          reject(err);
        } else {
          if (result?.length > 0) {
            logger.error('ccaas_get_country_config - Unexpected Error:', err);
            resolve(result);
          } else {
            resolve([]);
          }
        }
      });
    } catch (error) {
      logger.error('ccaas_get_country_config - Unexpected Error:', error);
      reject(error);
    }
  });
};

export const getEntryNodeDataDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      const mysqlq = `SELECT nodes FROM chatFlowHistory WHERE chatFlowId = ${data}`;
      mysqlPoolConnection.query(mysqlq, (err, result) => {
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
export const getStateConfigDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call ccaas_get_state_config(?)', [
        data?.countryCode,
      ],                        (err: any, result: any) => {
        if (err) {
          logger.error('ccaas_get_state_config - Unexpected Error:', err);
          reject(err);
        } else {
          if (result?.length > 0) {
            resolve(result);
          } else {
            resolve([]);
          }
        }
      });
    } catch (error) {
      logger.error('ccaas_get_state_config - Unexpected Error:', error);
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getCityConfigDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call ccaas_get_city_config(?,?)', [
        data?.countryCode,
        data?.cityCode
      ],                        (err: any, result: any) => {
        if (err) {
          logger.error('ccaas_get_city_config - Unexpected Error:', err);
          reject(err);
        } else {
          if (result?.length > 0) {
            resolve(result);
          } else {
            resolve([]);
          }
        }
      });
    } catch (error) {
      logger.error('ccaas_get_city_config - Unexpected Error:', error);
      reject(error);
    }
  });
};
