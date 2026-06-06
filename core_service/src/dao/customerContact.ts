import axios from 'axios';
import moment from 'moment';
import { mysqlPoolConnection } from '../plugins/db';
import { logger } from '../plugins/winston';

/**
 *
 * @param data
 * @returns
 */
export const insertCustomerContact = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      const tags = JSON.stringify(data.tags);
      const phoneNumber = JSON.stringify(data.phoneNumber);
      mysqlPoolConnection.query(
        'CALL ccaas_insert_customer_contact(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          data?.domainId,
          data?.firstName,
          data?.lastName,
          phoneNumber,
          data?.email,
          data?.accountNumber,
          tags,
          data?.location,
          data?.address,
          data?.country,
          data?.state,
          data?.city,
          data?.postalCode,
          data?.facebook,
          data?.instagram,
          data?.whatsapp,
          data?.twitter,
          data?.voiceBiometric,
          data?.customerType,
          data?.timeZone,
        ],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('ccaas_insert_customer_contact - Unexpected Error:', err);
          } else {
            resolve(result[0]);
          }
        }
      );
    } catch (error) {
      reject(error);
      logger.error('ccaas_insert_customer_contact - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const insertMergeCustomerContact = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_MergeCustomerContactUpdate_Info(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          data?.domainId ?? 0,
          data?.coid ?? 0,
          data?.deleteCoid ?? 0,
          data?.firstName ?? null,
          data?.lastName ?? null,
          data?.phoneNumber === null ? '[]' : JSON.stringify(data?.phoneNumber),
          data?.email === null ? '[]' : JSON.stringify(data?.email),
          data?.tags === null ? '[]' : JSON.stringify(data?.tags),
          data?.location ?? null,
          data?.address ?? null,
          data?.country ?? null,
          data?.state ?? null,
          data?.city ?? null,
          data?.postalCode ?? null,
          data?.facebook ?? null,
          data?.instagram ?? null,
          data?.whatsapp ?? null,
          data?.primaryPhoneNumber ?? null,
          data?.primaryEmail ?? null,
          data?.blockCustomer ?? 0,
          data?.blockCustomerReq ?? 0,
          data?.doNotDisturb ?? 0,
          data?.doNotDisturbReq ?? 0,
          data?.customerType ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccaas_MergeCustomerContactUpdate_Info - Unexpected Error:', err);
          }
          if (result) {
            resolve(result[0]);
          } else {
            reject(err);
            logger.error('ccaas_MergeCustomerContactUpdate_Info - Unexpected Error:', err);
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('ccaas_MergeCustomerContactUpdate_Info - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getCustomerConatct = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_getCustomerConatct(?,?,?,?,?,?,?,?,?)',
        [
          data?.domainId,
          data?.newCustomer ?? null,
          data?.limit ?? null,
          data?.offset ?? 0,
          data?.search ?? null,
          data?.c_type ?? null,
          data?.aditionalColumn ?? null,
          data?.fromDate ?? null,
          data?.toDate ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            resolve(err);
          } else {
            if (result && result.length > 0) {
              resolve(result);
            } else {
              reject(err);
              logger.error('ccaas_getCustomerConatct - Unexpected Error:', err);
            }
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('ccaas_getCustomerConatct - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getCustomerConatctByDate = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_getCustomerConatct_by_date(?,?,?)',
        [
          data?.fromDate,
          data?.toDate,
          data?.domainId,
        ],
        (err: any, result: any) => {
          if (err) {
            resolve(err);
          } else {
            if (result && result.length > 0) {
              resolve(result);
            } else {
              reject(err);
              logger.error('ccaas_getCustomerConatct - Unexpected Error:', err);
            }
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('ccaas_getCustomerConatct - Unexpected Error:', err);
    }
  });
};
/**
 *
 * @param data
 * @returns
 */
export const getCustomerConatctDetails = (data: any, coid: any) => {
  return new Promise((resolve, reject) => {
    try {
      if (coid === null && coid === undefined) {
        reject(new Error('coid cannot be null'));
      } else {
        mysqlPoolConnection.query(
          'CALL ccaas_getCustomerConatctDetails(?,?)',
          [
            coid ?? 0,
            data?.domainId
          ],
          (err: any, result: any) => {
            if (err) {
              resolve(err);
            } else {
              if (result && result.length > 0) {
                resolve(result[0]);
              } else {
                reject(err);
                logger.error('ccaas_getCustomerConatctDetails - Unexpected Error:', err);
              }
            }
          }
        );
      }
    } catch (err) {
      reject(err);
      logger.error('ccaas_getCustomerConatctDetails - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @param qdata
 * @returns
 */
export const updateCustomerContact = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      const currentDate = moment(new Date()).format('YYYY-MM-DD HH:mm:ss');
      const tags = JSON.stringify(data.tags);
      const phoneNumber = JSON.stringify(data.phoneNumber);
      mysqlPoolConnection.query(
        'CALL ccaas_update_customer_contact(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          data?.coid,
          data?.domainId,
          data?.firstName,
          data?.lastName,
          phoneNumber,
          data?.email,
          data?.accountNumber,
          tags,
          data?.location,
          data?.address,
          data?.country,
          data?.state,
          data?.city,
          data?.postalCode,
          data?.facebook,
          data?.instagram,
          data?.whatsapp,
          data?.twitter,
          data?.customerType,
          data?.voiceBiometric,
          data?.mergeCustomer,
          data?.blockCustomer,
          data?.doNotDisturb,
          data?.blockCustomerReq,
          data?.doNotDisturbReq,
        ],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('ccaas_update_customer_contact - Unexpected Error:', err);
          } else {
            resolve(result[0]);
          }
        });
    } catch (error) {
      reject(error);
      logger.error('ccaas_update_customer_contact - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const deleteCustomerContact = (data: any, coid: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_deleteCustomerContact(?,?)',
        [
          coid,
          data?.domainId
        ],
        (err: any, result: any) => {
          if (err) {
            logger.error('ccaas_deleteCustomerContact - Unexpected Error:', err);
            resolve(err);
          } else {
            resolve(result);
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('ccaas_deleteCustomerContact - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const isValidContact = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_isvalid_contact(?,?)',
        [
          data.email,
          data.domainId
        ],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('ccaas_isvalid_contact - Unexpected Error:', err);
          } else {
            resolve(result[0]);
          }
        });
    } catch (error) {
      reject(error);
      logger.error('ccaas_isvalid_contact - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getBlockCustomerContact = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_GetBlockedCustomerDetails_info(?)',
        [
          data?.domainId
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccaas_GetBlockedCustomerDetails_info - Unexpected Error:', err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            reject(err);
            logger.error('ccaas_GetBlockedCustomerDetails_info - Unexpected Error:', err);
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('ccaas_GetBlockedCustomerDetails_info - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getDoNotDisturbCustomerContact = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_GetdoNotDisturbCustomerDetails_info(?)',
        [
          data?.domainId
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccaas_GetdoNotDisturbCustomerDetails_info - Unexpected Error:', err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            reject(err);
            logger.error('ccaas_GetdoNotDisturbCustomerDetails_info - Unexpected Error:', err);
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('ccaas_GetdoNotDisturbCustomerDetails_info - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getBlockCustomerRequestContact = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_GetblockCustomerReqCustomerDetails_info(?)',
        [
          data?.domainId
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccaas_GetblockCustomerReqCustomerDetails_info - Unexpected Error:', err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            reject(err);
            logger.error('ccaas_GetblockCustomerReqCustomerDetails_info - Unexpected Error:', err);
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('ccaas_GetblockCustomerReqCustomerDetails_info - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getDoNotDisturbCustomerContactRequest = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_GetdoNotDisturbReqByCustomerDetails_info(?)',
        [
          data?.domainId
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccaas_GetdoNotDisturbReqByCustomerDetails_info - Unexpected Error:', err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            reject(err);
            logger.error('ccaas_GetdoNotDisturbReqByCustomerDetails_info - Unexpected Error:', err);
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('ccaas_GetdoNotDisturbReqByCustomerDetails_info - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateBlockCustomer = (data: any, coid: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_UpdateCustomerContact_blockCustomer(?,?,?,?,?)',
        [
          coid,
          data?.domainId,
          data?.blockCustomer,
          data?.blockedBy,
          data?.Reason
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('up_UpdateCustomerContact_blockCustomer - Unexpected Error:', err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            reject(err);
            logger.error('up_UpdateCustomerContact_blockCustomer - Unexpected Error:', err);
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('up_UpdateCustomerContact_blockCustomer - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateDoNotDisturb = (data: any, coid: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_UpdateCustomerContact_doNotDisturbBy(?,?,?,?,?)',
        [
          coid,
          data?.domainId,
          data?.doNotDisturb,
          data?.doNotDisturbBy,
          data?.Reason
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('up_UpdateCustomerContact_doNotDisturbBy - Unexpected Error:', err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            reject(err);
            logger.error('up_UpdateCustomerContact_doNotDisturbBy - Unexpected Error:', err);
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('up_UpdateCustomerContact_doNotDisturbBy - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @param qdata
 * @returns
 */
export const updateBlockCustomerReq = (data: any, coid: any, auth: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_update_block_customer_req(?,?,?,?,?)',
        [
          coid,
          data.domainId,
          data.blockCustomerReq,
          data.blockedReqBy,
          data.Reason
        ],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('ccaas_update_block_customer_req - Unexpected Error:', err);
          } else {
            resolve(result[0]);
          }
        });
    } catch (error) {
      reject(error);
      logger.error('ccaas_update_block_customer_req - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @param coid
 * @param auth
 * @returns
 */
export const updateDoNotDisturbReq = (data: any, coid: any, auth: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_update_do_not_disturb_req(?,?,?,?,?)',
        [
          coid,
          data.domainId,
          data.doNotDisturbReq,
          data.doNotDisturbReqBy,
          data.Reason
        ],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('ccaas_update_do_not_disturb_req - Unexpected Error:', err);
          } else {
            resolve(result[0]);
          }
        });
    } catch (error) {
      reject(error);
      logger.error('ccaas_update_do_not_disturb_req - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @param qdata
 * @returns
 */
export const insertBulkCustometConatct = (datas: any, auth: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      datas.map((data: any) => {
        mysqlPoolConnection.query(
          'CALL ccaas_insertUpdateCustomerContact(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
          [
            data?.coid ?? 0,
            data?.firstName ?? null,
            data?.lastName ?? null,
            data?.phoneNumber === null ? '[]' : JSON.stringify(data?.phoneNumber),
            data?.email === null ? '[]' : JSON.stringify(data.email),
            data?.accountNumber ?? null,
            data?.tags === null ? '[]' : JSON.stringify(data?.tags),
            data?.location ?? null,
            data?.address ?? null,
            data?.country ?? null,
            data?.state ?? null,
            data?.city ?? null,
            data?.postalCode ?? null,
            data?.facebook ?? null,
            data?.instagram ?? null,
            data?.whatsapp ?? null,
            data?.twitter ?? null,
            data?.voiceBiometric ?? null,
            auth?.domainId,
            data?.mergeCustomer ?? null,
            data?.blockCustomer ?? 0,
            data?.doNotDisturb ?? 0,
            data?.blockCustomerReq ?? 0,
            data?.doNotDisturbReq ?? 0,
            data?.type ?? null,
            data?.currentCustomerNumber ?? null,
            data?.customerType ?? null,
            data?.timeZone ?? null,
            data?.cookiesId ?? null,
            data?.source ?? null,
            data?.customField ?? null,
            data?.customValues ?? null,
            data?.primaryPhoneNumber ?? null,
            data?.primaryEmail ?? null
          ],
          (err: any, result: any) => {
            if (err) {
              reject(err);
              logger.error('ccaas_insertUpdateCustomerContact - Unexpected Error:', err);
            }
            if (result && result.length > 0) {
              resolve(result[0]);
            } else {
              reject(err);
              logger.error('ccaas_insertUpdateCustomerContact - Unexpected Error:', err);
            }
          }
        );
      });
    } catch (error) {
      reject(error);
      logger.error('ccaas_insertUpdateCustomerContact - Unexpected Error:', error);
    }
  });
};

/**
 * @param data
 * @param auth
 * @returns
 */
export const insertBulkUpdateMarketing = (datas: any, auth: any) => {
  return new Promise((resolve, reject) => {
    try {
      const requestData: any = datas.map((data: any) => ({
        First_Name: data?.firstName ?? '',
        Last_Name: data?.lastName ?? '',
        Contact_owner: '1',
        Phone_number: data?.primaryPhoneNumber ?? '',
        Email: data?.primaryEmail ?? '',
        p_UserId: 5,
        p_CompanyId: auth?.companyId,
        user_contact_id: data?.user_contact_id,
        City: data?.city,
        Postal_Code: data?.postalCode,
        P_customerType: data?.customerType,
        P_tags: data?.tags?.length > 0 ? JSON.stringify(data?.tags) : '',
        P_location: data?.location,
        P_state: data?.state,
        P_country: data?.country
      }));
      axios.post('https://stagemarketingapi.worktual.co.uk/marketing_automation/v1/contact/create_bulk_contact', requestData)
        .then((response: any) => {
          if (response?.data?.status_code === 200) {
            resolve(response?.data);
          } else {
            logger.error('insertBulkUpdateMarketing - Unexpected Error:', response?.data?.status_code);
            reject(new Error(`Server returned status code: ${response?.data?.status_code}`));
          }
        })
        .catch((error: any) => {
          logger.error('insertBulkUpdateMarketing - Unexpected Error:', error);
          reject(new Error(`Request failed: ${error.message}`));
        });
    } catch (error) {
      logger.error('insertBulkUpdateMarketing - Unexpected Error:', error);
      reject(new Error(`Synchronous error: ${error.message}`));
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getCustomerConatctPhoneNumber = (data: any, phoneNumber: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_get_customer_conatct_phone_number(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          data?.domainId,
          null,
          null,
          phoneNumber,
          null,
          null,
          null,
          null,
          null,
          null,
          null,
          null,
          null,
          null,
          null,
          null,
          null,
          null,
          null,
          null,
        ],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('ccaas_get_customer_conatct_phone_number - Unexpected Error:', err);
          } else {
            resolve(result[0]);
          }
        });
    } catch (error) {
      reject(error);
      logger.error('ccaas_get_customer_conatct_phone_number - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getColumnEditName = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_get_column_edit_name(?,?)',
        [
          data.domainId,
          data.ext
        ],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('ccaas_get_column_edit_name - Unexpected Error:', err);
          } else {
            resolve(result[0]);
          }
        });
    } catch (error) {
      reject(error);
      logger.error('ccaas_get_column_edit_name - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const createColumnEditName = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      const editColumnNames = JSON.stringify(data.editColumnNames);
      mysqlPoolConnection.query('CALL Edit_CustomerContact_Details(?,?,?)', [
        data.domainId,
        data.ext,
        editColumnNames,
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('Edit_CustomerContact_Details - Unexpected Error:', err);
        }
        resolve(result);
      });
    } catch (err) {
      reject(err);
      logger.error('Edit_CustomerContact_Details - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @param qdata
 * @returns
 */
export const updateCustomerContactTag = (data: any, coid: any) => {
  return new Promise((resolve, reject) => {
    try {
      const tags = JSON.stringify(data.tags);
      mysqlPoolConnection.query(
        'CALL ccaas_update_customer_contact_tag(?,?,?)',
        [
          coid,
          data.domainId,
          tags
        ],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('ccaas_update_customer_contact_tag - Unexpected Error:', err);
          } else {
            resolve(result[0]);
          }
        });
    } catch (error) {
      reject(error);
      logger.error('ccaas_update_customer_contact_tag - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const insertUpdateCustomerContact = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_insertUpdateCustomerContact(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          data?.coid ?? 0,
          data?.firstName ?? null,
          data?.lastName ?? null,
          data?.phoneNumber ? JSON.stringify(data?.phoneNumber) : null,
          data?.email ? JSON.stringify(data.email) : null,
          data?.accountNumber ?? null,
          data?.tags === null ? '[]' : JSON.stringify(data?.tags),
          data?.location ?? null,
          data?.address ?? null,
          data?.country ?? null,
          data?.state ?? null,
          data?.city ?? null,
          data?.postalCode ?? null,
          data?.facebook ?? null,
          data?.instagram ?? null,
          data?.whatsapp ?? null,
          data?.twitter ?? null,
          data?.voiceBiometric ?? null,
          data?.domainId,
          data?.mergeCustomer ?? null,
          data?.blockCustomer ?? 0,
          data?.doNotDisturb ?? 0,
          data?.blockCustomerReq ?? 0,
          data?.doNotDisturbReq ?? 0,
          data?.type ?? null,
          data?.currentCustomerNumber ?? null,
          data?.customerType ?? null,
          data?.timeZone ?? null,
          data?.cookiesId ?? null,
          data?.source ?? null,
          data?.customField ?? null,
          data?.customValues ?? null,
          data?.primaryPhoneNumber ?? null,
          data?.primaryEmail ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccaas_insertUpdateCustomerContact - Unexpected Error:', err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            reject(err);
            logger.error('ccaas_insertUpdateCustomerContact - Unexpected Error:', err);
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('ccaas_insertUpdateCustomerContact - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const insertUpdateCustomerContactPrimaryChat = (data: any) => {
  return new Promise(async (resolve, reject) => {

    try {
      mysqlPoolConnection.query(
        'CALL ccaas_insertUpdateCustomerContact_PrimaryForm(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          data?.coid ?? null,
          data?.firstName ?? null,
          data?.lastName ?? null,
          data?.phoneNumber === null ? '[]' : JSON.stringify(data?.phoneNumber),
          data?.email === null ? '[]' : JSON.stringify(data.email),
          data?.accountNumber ?? null,
          data?.tags === null ? '[]' : JSON.stringify(data?.tags),
          data?.location ?? null,
          data?.address ?? null,
          data?.country ?? null,
          data?.state ?? null,
          data?.city ?? null,
          data?.postalCode ?? null,
          data?.facebook ?? null,
          data?.instagram ?? null,
          data?.whatsapp ?? null,
          data?.twitter ?? null,
          data?.voiceBiometric ?? null,
          data?.domainId,
          data?.mergeCustomer ?? null,
          data?.blockCustomer ?? null,
          data?.doNotDisturb ?? 0,
          data?.blockCustomerReq ?? 0,
          data?.doNotDisturbReq ?? 0,
          data?.type ?? null,
          data?.currentCustomerNumber ?? null,
          data?.customerType ?? null,
          data?.timeZone ?? null,
          data?.cookiesId ?? null,
          data?.source ?? null,
          data?.customField ?? null,
          data?.customValues ?? null,
          data?.primaryPhoneNumber ?? null,
          data?.primaryEmail ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccaas_insertUpdateCustomerContact_PrimaryForm - Unexpected Error:', err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            reject(err);
            logger.error('ccaas_insertUpdateCustomerContact_PrimaryForm - Unexpected Error:', err);
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('ccaas_insertUpdateCustomerContact_PrimaryForm - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getCustomerConatctByEmail = (data: any, email: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('CALL getCustomerContactByEmail(?,?)', [
        data.domainId,
        email ?? null
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('getCustomerContactByEmail - Unexpected Error:', err);
        }
        if (result && result.length > 0) {
          resolve(result[0]);
        } else {
          reject(err);
          logger.error('getCustomerContactByEmail - Unexpected Error:', err);
        }
      });
    } catch (err) {
      reject(err);
      logger.error('getCustomerContactByEmail - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getCustomerConatctByPhoneNumber = (data: any, phoneNumber: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('CALL getCustomerContactByPhoneNumber(?,?)', [
        data.domainId,
        phoneNumber ?? null
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('getCustomerContactByPhoneNumber - Unexpected Error:', err);
        }
        if (result[0]?.length > 0) {
          resolve(result[0]);
        } else {
          resolve([]);
        }
      });
    } catch (err) {
      reject(err);
      logger.error('getCustomerContactByPhoneNumber - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getCustomerContactChannels = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL up_ccaas_InsertGetCustomerContact(?,?,?)',
        [
          data?.domainId,
          data?.chennalType,
          data?.channelValue,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('up_ccaas_InsertGetCustomerContact - Unexpected Error:', err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            reject(err);
            logger.error('up_ccaas_InsertGetCustomerContact - Unexpected Error:', err);
          }
        }
      );

    } catch (err) {
      reject(err);
      logger.error('up_ccaas_InsertGetCustomerContact - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const deleteMultipleCustomerContact = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL up_DeleteCustomerContact(?,?)',
        [
          data?.coid,
          data?.domainId,
        ],
        (err: any, result: any) => {
          if (err) {
            resolve(err);
          } else {
            if (result && result.length > 0) {
              resolve(result[0]);
            } else {
              reject(err);
              logger.error('up_DeleteCustomerContact - Unexpected Error:', err);
            }
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('up_DeleteCustomerContact - Unexpected Error:', err);
    }
  });
};

/**
 * @param data
 * @param auth
 * @returns
 */
export const insertUpdateMarketing = (data: any, auth: any) => {
  return new Promise((resolve, reject) => {
    try {
      const requestData = {
        First_Name: data?.firstName ?? '',
        Last_Name: data?.lastName ?? '',
        Contact_owner: '1',
        Phone_number: data?.primaryPhoneNumber ?? '',
        Email: data?.primaryEmail ?? '',
        p_UserId: 5,
        p_CompanyId: auth?.companyId,
        user_contact_id: data?.user_contact_id,
        City: data?.city,
        Postal_Code: data?.postalCode,
        P_customerType: data?.customerType,
        P_tags: data?.tags?.length > 0 ? JSON.stringify(data?.tags) : '',
        P_location: data?.location,
        P_state: data?.state,
        P_country: data?.country
      };
      axios.post('https://stagemarketingapi.worktual.co.uk/marketing_automation/v1/contact/create_contact', requestData)
        .then((response: any) => {
          if (response?.data?.status_code === 200) {
            resolve(response?.data);
          } else {
            logger.error('insertUpdateMarketing - Unexpected Error:', response?.data?.status_code);
            reject(new Error(`Server returned status code: ${response?.data?.status_code}`));
          }
        })
        .catch((error: any) => {
          logger.error('insertUpdateMarketing - Unexpected Error:', error);
          reject(new Error(`Request failed: ${error.message}`));
        });
    } catch (error) {
      logger.error('insertUpdateMarketing - Unexpected Error:', error);
      reject(new Error(`Synchronous error: ${error.message}`));
    }
  });
};

export const getMarketingContactId = (data: any, auth: any) => {
  return new Promise((resolve, reject) => {
    try {
      const requestData = {
        p_emailId: data?.primaryEmail,
        p_companyId: auth?.companyId
      };
      axios.post('https://stagemarketingapi.worktual.co.uk/marketing_automation/v1/contact/get_contact_by_emailId', requestData)
        .then((response: any) => {
          if (response?.data?.status_code === 200) {
            resolve(response?.data);
          } else {
            logger.error('getMarketingContactId - Unexpected Error:', response?.data?.status_code);
            reject(response?.data);
            // reject(new Error(`Server returned status code: ${response?.data?.status_code}`));
          }
        })
        .catch((error: any) => {
          logger.error('getMarketingContactId - Unexpected Error:', error);
          reject(error);
          // reject(new Error(`Request failed: ${error.message}`));
        });
    } catch (error) {
      logger.error('getMarketingContactId - Unexpected Error:', error);
      reject(error);
      // reject(new Error(`Synchronous error: ${error.message}`));
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getCustomerContactFieldType = () => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_getCustomFieldType()',
        [],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('up_ccaas_getCustomFieldType - Unexpected Error:', err);
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
      logger.error('up_ccaas_getCustomFieldType - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getCustomerContactField = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_getCustomField(?,?,?)',
        [
          data.domainId,
          data.screenId,
          data.isArchive
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('up_ccaas_getCustomField - Unexpected Error:', err);
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
      logger.error('up_ccaas_getCustomField - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getCustomerContactDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_get_customer_contact_dao(?)',
        [data],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccaas_get_customer_contact_dao - Unexpected Error:', err);
          } else {
            resolve(result[0]);
          }
        });
    } catch (error) {
      reject(error);
      logger.error('ccaas_get_customer_contact_dao - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @param callback
 */
export const getConnectorTokenInfo = (domainId: any, callback: (res: any) => void) => {
  try {
    mysqlPoolConnection.query(
      'call ccaas_get_connector_token_info(?)',
      [
        domainId,
      ],
      (error: any, results: any) => {
        if (error) {
          logger.error('getConnectorTokenInfo - Unexpected Error:', error);
          callback([]);
          return;
        }
        const result = Array.isArray(results) && results.length > 0 ? results[0] : [];
        callback(result);
      });
  } catch (error) {
    logger.error('getConnectorTokenInfo - Unexpected Error:', error);
  }
};

/**
 *
 * @param data
 * @returns
 */
export const getAiConservationd = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_get_ai_conservation_overview(?,?)',
        [
          data?.sessionId,
          data?.domainId
        ],
        (err: any, result: any) => {
          if (err) {
            resolve(err);
          } else {
            if (result && result.length > 0) {
              resolve(result[0]);
            } else {
              reject(err);
              logger.error('getAiConservationd - Unexpected Error:', err);
            }
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('getAiConservationd - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getContactMobileLogsDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      const editColumnNames = JSON.stringify(data.editColumnNames);
      mysqlPoolConnection.query('CALL ccaas_get_mobile_contact_log(?,?,?,?)', [
        data.domainId,
        data.offset,
        data.limit,
        data.agentId
      ],                        (err: any, result: any) => {
        if (err) {
          console.log('getContactLogsHandler err', err);

          reject(err);
          logger.error('ccaas_get_mobile_contact_log - Unexpected Error:', err);
        }
        resolve(result);
      });
    } catch (err) {
      reject(err);
      logger.error('ccaas_get_mobile_contact_log - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getCustomerTicketLogDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('CALL ccaas_get_customer_ticket_log(?)', [
        data.coid,
      ],                        (err: any, result: any) => {
        if (err) {
          console.log('getContactLogsHandler err', err);

          reject(err);
          logger.error('ccaas_get_customer_ticket_log - Unexpected Error:', err);
        }
        resolve(result?.[0]);
      });
    } catch (err) {
      reject(err);
      logger.error('ccaas_get_customer_ticket_log - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getCustomerAppoinentBookingLogDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('CALL ccaas_get_customer_appointment_booking_log(?,?)', [
        data.coid,
        data.domainId,
      ],                        (err: any, result: any) => {
        if (err) {
          console.log('getContactLogsHandler err', err);

          reject(err);
          logger.error('ccaas_get_customer_appointment_booking_log - Unexpected Error:', err);
        }
        resolve(result?.[0]);
      });
    } catch (err) {
      reject(err);
      logger.error('ccaas_get_customer_appointment_booking_log - Unexpected Error:', err);
    }
  });
};
