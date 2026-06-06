import axios from 'axios';
import moment from 'moment';
import { mysqlPoolConnection } from '../plugins/db';
import { logger } from '../plugins/winston';

/**
 *
 * @param data
 * @returns
 */
export const insertCustomerTag = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_insert_update_customer_tag(?,?,?,?,?,?)',
        [
          null,
          data?.customerTagName,
          data?.type,
          data?.description,
          data?.status,
          data?.domainId
        ],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('ccaas_insert_update_customer_tag - Unexpected Error:', err);
          } else {
            resolve(result?.[0]?.[0]);
          }
        });
    } catch (error) {
      reject(error);
      logger.error('ccaas_insert_update_customer_tag - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getCustomerTag = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_customer_tag(?)',
        [
          data?.domainId
        ],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('ccaas_get_customer_tag - Unexpected Error:', err);
          } else {
            resolve(result?.[0]);
          }
        });
    } catch (error) {
      reject(error);
      logger.error('ccaas_get_customer_tag - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @param qdata
 * @returns
 */
export const updateCustomerTag = (data: any, cid: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_insert_update_customer_tag(?,?,?,?,?,?)',
        [
          cid ?? null,
          data?.customerTagName,
          data?.type,
          data?.description,
          data?.status,
          data?.domainId
        ],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('ccaas_insert_update_customer_tag - Unexpected Error:', err);
          } else {
            resolve(result?.[0]?.[0]);
          }
        });
    } catch (error) {
      reject(error);
      logger.error('ccaas_insert_update_customer_tag - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param aid
 * @param domainId
 * @returns
 */
export const deleteCustomerTag = (cid: any, domainId: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_delete_customer_tag(?,?)',
        [
          cid,
          domainId
        ],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('ccaas_delete_customer_tag - Unexpected Error:', err);
          } else {
            resolve(result?.[0]?.[0]);
          }
        });
    } catch (error) {
      reject(error);
      logger.error('ccaas_delete_customer_tag - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getCustomerTagDetails = (data: any, cid: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_customer_tag_by_cid(?,?)',
        [
          data?.domainId,
          cid
        ],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('ccaas_get_customer_tag_by_cid - Unexpected Error:', err);
          } else {
            resolve(result?.[0]);
          }
        });
    } catch (error) {
      reject(error);
      logger.error('ccaas_get_customer_tag_by_cid - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const isValidCustomerTag = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_is_valid_customer_tag(?,?)',
        [
          data?.domainId,
          data?.customerTagName
        ],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('ccaas_get_is_valid_customer_tag - Unexpected Error:', err);
          } else {
            resolve(result?.[0]);
          }
        });
    } catch (error) {
      reject(error);
      logger.error('ccaas_get_is_valid_customer_tag - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const customerTagDisable = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_insert_update_customer_tag_disable(?,?)',
        [
          data?.domainId,
          data?.customerTag
        ],
        (error, result) => {
          if (error) {
            reject(error);
            logger.error('ccaas_insert_update_customer_tag_disable - Unexpected Error:', error);
          } else {
            resolve(result?.[0]?.[0]);
          }
        });
    } catch (error) {
      reject(error);
      logger.error('ccaas_insert_update_customer_tag_disable - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getCustomerTagDisable = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_customer_tag_disable(?)',
        [
          data?.domainId,
        ],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('ccaas_get_customer_tag_disable - Unexpected Error:', err);
          } else {
            resolve(result?.[0]);
          }
        });
    } catch (error) {
      reject(error);
      logger.error('ccaas_get_customer_tag_disable - Unexpected Error:', error);
    }
  });
};

export const getMobileNumberDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      const mysqlq = `SELECT * FROM test_mobile where mobileNumber =${data}`;
      mysqlPoolConnection.query(mysqlq, (err, result) => {
        if (err) {
          reject(err);
          logger.error('getMobileNumberDao - Unexpected Error:', err);
        } else {
          resolve(result);
        }
      });
    } catch (error) {
      reject(error);
      logger.error('getMobileNumberDao - Unexpected Error:', error);
    }
  });
};

export const customerCreateContactDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_create_custom_field_info(?,?,?,?,?,?,?,?,?,?,?)',
        [
          data?.domainid ?? null,
          data?.field_type ?? null,
          data?.field_name ?? null,
          data?.data_type ?? null,
          data?.field_label ?? null,
          data?.field_value ?? null,
          data?.field_category ?? null,
          data?.isMandatory ?? null,
          data?.placeHolder ?? null,
          data?.errMsg ?? null,
          data.isDisplay ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('up_ccaas_create_custom_field_info - Unexpected Error:', err);
          }
          resolve(result);
        }
      );
    } catch (error) {
      reject(error);
      logger.error('up_ccaas_create_custom_field_info - Unexpected Error:', error);
    }
  });
};

export const customerUpdateContactDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_update_custom_field_info(?,?,?,?,?,?,?,?,?)',
        [
          data?.id ?? 0,
          data?.domainId ?? null,
          data?.field_type ?? null,
          data?.field_label ?? null,
          data?.field_value ?? null,
          data?.isMandatory ?? null,
          data?.placeHolder ?? null,
          data?.errMsg ?? null,
          data?.isDisplay ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('up_ccaas_update_custom_field_info - Unexpected Error:', err);
          }
          resolve(result);
        }
      );
    } catch (error) {
      reject(error);
      logger.error('up_ccaas_update_custom_field_info - Unexpected Error:', error);
    }
  });
};

export const customerGetContactDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_get_customer_contact_custom_field(?)',
        [
          data?.domainid ?? null,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('up_ccaas_get_customer_contact_custom_field - Unexpected Error:', err);
          }
          resolve(result);
        }
      );
    } catch (error) {
      reject(error);
      logger.error('up_ccaas_get_customer_contact_custom_field - Unexpected Error:', error);
    }
  });
};

export const deleteCustomContactDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_delete_customer_contact_custom_field(?)',
        [
          data?.cid ?? null,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('up_ccaas_delete_customer_contact_custom_field - Unexpected Error:', err);
          }
          resolve(result);
        }
      );
    } catch (error) {
      reject(error);
      logger.error('up_ccaas_delete_customer_contact_custom_field - Unexpected Error:', error);
    }
  });
};

export const getPdgPageDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      await axios({
        method: 'post',
        url: process.env.NLP_PDF_PAGE_FINDER,
        headers: {
          'Content-Type': 'application/json'
        },
        data: {
          query: data?.query,
          pdf_path: '',
          unique_id: '',
          top_k: ''
        }
      })
        .then((response) => {
          resolve(response);
        })
        .catch((error) => {
          reject(error);
          logger.error('getPdgPageDao - Unexpected Error:', error);
        });

    } catch (error) {
      reject(error);
      logger.error('getPdgPageDao - Unexpected Error:', error);
    }
  });
};
