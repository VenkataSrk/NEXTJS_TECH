
import { mysqlPoolConnection } from '../plugins/db';
import { logger } from '../plugins/winston';

/**
 *
 * @param data
 * @returns
 */
export const insertUpdateCustomerType = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_insert_update_customer_type(?,?,?,?,?,?)',
        [
          data.id,
          data.domainId,
          data.customerTypeName,
          data.description,
          data.isStatus,
          data.type
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccaas_insert_update_customer_type - Unexpected Error:', err);
          }
          if (result && result[0]?.length > 0) {
            resolve(result[0]);
          } else {
            resolve([]);
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('ccaas_insert_update_customer_type - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getCustomerTypeList = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_customer_type_info(?)',
        [
          data.domainId
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccaas_get_customer_type_info - Unexpected Error:', err);
          }
          if (result && result[0]?.length > 0) {
            resolve(result[0]);
          } else {
            resolve([]);
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('ccaas_get_customer_type_info - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const deleteCustomerType = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_delete_customer_type(?,?)',
        [
          data.id,
          data.domainId
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccaas_delete_customer_type - Unexpected Error:', err);
          }
          if (result && result[0]?.length > 0) {
            resolve(result[0]);
          } else {
            resolve([]);
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('ccaas_delete_customer_type - Unexpected Error:', err);
    }
  });
};
