/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */
import moment from 'moment';
import { mysqlPoolConnection } from '../plugins/db';
import { logger } from '../plugins/winston';

/**
 *
 * @param data
 * @returns
 */

export const insert = (data: any) => {
  const hoursOfOperation = JSON.stringify(data.hours_of_operation);
  const settingAudio = JSON.stringify(data.settingAudio);
  const announcement = JSON.stringify(data.announcement);
  const maximumCallers = JSON.stringify(data.maximumCallers);
  const maximumWaitTime = JSON.stringify(data.maximumWaitTime);
  const outOfBusinessHours = JSON.stringify(data.outOfBusinessHours);

  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_insert_update_queue (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          data?.qid ?? null,
          data.name,
          data.description,
          data.domainId,
          settingAudio,
          announcement,
          maximumCallers,
          maximumWaitTime,
          data.OutboundCallerId,
          data.outBoundNumber,
          data.maxCallHandlingTimeMM,
          data.callWrapUpTime,
          data.outBoundCallStatus,
          data.hoursOfOperatioId,
          hoursOfOperation,
          outOfBusinessHours,
          data.status,
          data.type,
        ],
        (err: any, result: any) => {
          if (err) {
            console.error('MySQL SP Error:', err);
            reject(err);
            logger.error('ccaas_insert_update_queue - Unexpected Error:', err);

          } else {
            console.log('SP Result:', JSON.stringify(result));
            resolve(result[0]);
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('ccaas_insert_update_queue - Unexpected Error:', err);

    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const get = (qid: any, data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_get_queue_setting(?, ?)',
        [qid, data.domainId],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccaas_get_queue_setting - Unexpected Error:', err);

          } else {
            resolve(result[0]);
          }
        }
      );
    } catch (error) {
      reject(error);
      logger.error('ccaas_get_queue_setting - Unexpected Error:', error);

    }
  });
};

/**
 *
 * @param data
 * @param qdata
 * @returns
 */

export const update = (data: any, qid: any) => {
  return new Promise((resolve, reject) => {
    try {
      const hoursOfOperation = JSON.stringify(data.hours_of_operation);
      const settingAudio = JSON.stringify(data.settingAudio);
      const announcement = JSON.stringify(data.announcement);
      const maximumCallers = JSON.stringify(data.maximumCallers);
      const maximumWaitTime = JSON.stringify(data.maximumWaitTime);
      const outOfBusinessHours = JSON.stringify(data.outOfBusinessHours);

      const currentDate = moment(new Date()).format('YYYY-MM-DD HH:mm:ss');

      mysqlPoolConnection.query(
        'CALL ccaas_insert_update_queue (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          qid ?? null,
          data.name,
          data.description,
          data.domainId,
          settingAudio,
          announcement,
          maximumCallers,
          maximumWaitTime,
          data.OutboundCallerId,
          data.outBoundNumber,
          data.maxCallHandlingTimeMM,
          data.callWrapUpTime,
          data.outBoundCallStatus,
          data.hoursOfOperatioId,
          hoursOfOperation,
          outOfBusinessHours,
          data.status,
          data.type,
        ],
        (err: any, result: any) => {
          if (err) {
            console.error('MySQL SP Error:', err);
            reject(err);
            logger.error('ccaas_insert_update_queue - Unexpected Error:', err);

          } else {
            console.log('SP Result:', JSON.stringify(result));
            resolve(result.length > 0 ? result?.[0] : []);
          }
        }
      );
    } catch (error) {
      reject(error);
      logger.error('ccaas_insert_update_queue - Unexpected Error:', error);

    }
  });
};

/**
 *
 * @param qdata
 * @returns
 */
export const queueDelete = (qdata: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_queue_delete_by_qid(?)',
        [qdata],   // passing qid as parameter
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('ccaas_queue_delete_by_qid - Unexpected Error:', err);

          } else {
            resolve(result);
          }
        }
      );
    } catch (error) {
      reject(error);
      logger.error('ccaas_queue_delete_by_qid - Unexpected Error:', error);

    }
  });
};

// get queue all list
export const getSearchList = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_get_search_list_queue(?, ?)',
        [
          data?.name ?? '',
          data?.domainId
        ],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('ccaas_get_search_list_queue - Unexpected Error:', err);

          } else {
            resolve(result[0]);
          }
        });
    } catch (error) {
      reject(error);
      logger.error('ccaas_get_search_list_queue - Unexpected Error:', error);

    }
  });
};

/**
 *
 * @param data
 * @param qid
 * @returns
 */
export const updateQueue = (data: any, qid: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_update_queue_status(?, ?)',
        [qid, data.status],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('ccaas_update_queue_status - Unexpected Error:', err);

          } else {
            resolve(result);
          }
        }
      );
    } catch (error) {
      reject(error);
      logger.error('ccaas_update_queue_status - Unexpected Error:', error);

    }
  });
};

/**
 *
 * @param data
 * @param qdata
 * @returns
 */
export const updateStatus = (data: any, qid: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_update_queue_status(?, ?)',
        [qid, data.status],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('ccaas_update_queue_status - Unexpected Error:', err);

          } else {
            resolve(result);
          }
        }
      );
    } catch (error) {
      reject(error);
      logger.error('ccaas_update_queue_status - Unexpected Error:', error);

    }
  });
};
/**
 *
 * @param data
 * @returns
 */
export const insertCustomHours = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_insert_update_createBusinessHours (?,?,?,?,?,?,?,?,?,?)',
        [
          data?.cid ?? null,
          data?.name,
          data?.description,
          JSON.stringify(data.hours),
          data?.domainId,
          data?.timeZone,
          data.default_hours,
          data.timeZoneValue,
          data.type,
          data.holiday.length > 0 ? JSON.stringify(data.holiday) : null,

        ],

        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccaas_insert_update_createBusinessHours - Unexpected Error:', err);

          } else {
            resolve(result);
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('ccaas_insert_update_createBusinessHours - Unexpected Error:', err);

    }
  });
};
/**
 *
 * @param data
 * @returns
 */
export const getCustomHoursList = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_get_custom_hours_list(?)',
        [data.domainId],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('ccaas_get_custom_hours_list - Unexpected Error:', err);

          } else {
            resolve(result[0]);
          }
        }
      );
    } catch (error) {
      reject(error);
      logger.error('ccaas_get_custom_hours_list - Unexpected Error:', error);

    }
  });
};

export const updateCustomHours = (data: any, cid: any) => {
  return new Promise((resolve, reject) => {
    try {
      const hours = JSON.stringify(data.hours);
      mysqlPoolConnection.query(
        'CALL ccaas_update_custom_hours(?, ?, ?, ?, ?, ?, ?, ?, ?)',
        [
          cid,
          data.name,
          data.description,
          hours,
          data.domainId,
          data.timeZone,
          data.default_hours,
          data.timeZoneValue,
          data.type
        ],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('ccaas_update_custom_hours - Unexpected Error:', err);

          } else {
            resolve(result[0]);
          }
        }
      );
    } catch (error) {
      reject(error);
      logger.error('ccaas_update_custom_hours - Unexpected Error:', error);

    }
  });
};

/**
 *
 * @param qdata
 * @returns
 */
export const deleteCustomHours = (cid: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_delete_custom_hours(?)',
        [cid],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('ccaas_delete_custom_hours - Unexpected Error:', err);

          } else {
            resolve(result);
          }
        }
      );
    } catch (error) {
      reject(error);
      logger.error('ccaas_delete_custom_hours - Unexpected Error:', error);

    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getQueueDetails = (cid: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_get_queue_details(?)',
        [cid],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('ccaas_get_queue_details - Unexpected Error:', err);

          } else {
            resolve(result[0][0]); // SP returns nested array, so taking first row
          }
        }
      );
    } catch (error) {
      reject(error);
      logger.error('ccaas_get_queue_details - Unexpected Error:', error);

    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const checkQueueMapping = (qid: any, data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_check_queue_mapping(?, ?)',
        [qid, data],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('ccaas_check_queue_mapping - Unexpected Error:', err);

          } else {
            resolve(result[0]); // SP returns array of rows
          }
        }
      );
    } catch (error) {
      reject(error);
      logger.error('ccaas_check_queue_mapping - Unexpected Error:', error);

    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const customHoursStatus = (cid: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_custom_hours_status(?)',
        [cid],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('ccaas_custom_hours_status - Unexpected Error:', err);

          } else {
            resolve(result[0][0]); // return single record
          }
        }
      );
    } catch (error) {
      reject(error);
      logger.error('ccaas_custom_hours_status - Unexpected Error:', error);

    }
  });
};

/**
 *
 * @param datar
 * @returns
 */
export const getCustomHoursDetail = (cid: any, data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_get_custom_hours_detail(?, ?)',
        [cid, data.domainId],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('ccaas_get_custom_hours_detail - Unexpected Error:', err);

          } else {
            resolve(result[0]); // returns all matching rows
          }
        }
      );
    } catch (error) {
      reject(error);
      logger.error('ccaas_get_custom_hours_detail - Unexpected Error:', error);

    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getCustomHoursDetailById = (cid: any, domainId: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_getHoursOfOperation (?)',
        [
          cid ?? null,
        ],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('ccaas_getHoursOfOperation - Unexpected Error:', err);

          } else {
            resolve(result?.length > 0 ? result[0] : []);
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('ccaas_getHoursOfOperation - Unexpected Error:', err);

    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateCustomHoursById = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_updateHoursOfOperation (?,?,?)',
        [
          data.cid,
          JSON.stringify(data.hoursOfOperation),
          data.domainId
        ],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('ccaas_updateHoursOfOperation - Unexpected Error:', err);

          } else {
            resolve(result?.length > 0 ? result[0] : []);
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('ccaas_updateHoursOfOperation - Unexpected Error:', err);

    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const isValidCustomHoursName = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_get_is_valid_custom_hours_name(?, ?)',
        [data.name, data.domainId],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('ccaas_get_is_valid_custom_hours_name - Unexpected Error:', err);

          } else {
            resolve(result[0]); // SP result set
          }
        }
      );
    } catch (error) {
      reject(error);
      logger.error('ccaas_get_is_valid_custom_hours_name - Unexpected Error:', error);

    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const isValidQueueName = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_get_is_valid_queue_name(?, ?)',
        [data.name, data.domainId],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('ccaas_get_is_valid_queue_name - Unexpected Error:', err);

          } else {
            resolve(result[0]); // first result set from SP
          }
        }
      );
    } catch (error) {
      reject(error);
      logger.error('ccaas_get_is_valid_queue_name - Unexpected Error:', error);

    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const insertUpdateBusinessHolidaysDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_insert_update_businessHolidays (?,?,?,?,?,?,?)',
        [
          data?.holidayId ?? null,
          data?.domainId,
          data?.businessId,
          data?.holidayName,
          data?.startDate,
          data?.endDate,
          data?.holidayDesc
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccaas_insert_update_businessHolidays - Unexpected Error:', err);

          } else {
            resolve(result);
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('ccaas_insert_update_businessHolidays - Unexpected Error:', err);

    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getBusinessHolidaysDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_get_businessHolidays (?,?,?)',
        [
          data?.businessId,
          data?.holidayId ?? null,
          data?.domainId,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccaas_get_businessHolidays - Unexpected Error:', err);

          } else {
            resolve(result);
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('ccaas_get_businessHolidays - Unexpected Error:', err);

    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const deleteBusinessHolidaysDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_delete_businessHolidays (?,?,?)',
        [
          data?.businessId,
          data?.holidayId ?? null,
          data?.domainId,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccaas_delete_businessHolidays - Unexpected Error:', err);

          } else {
            resolve(result);
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('ccaas_delete_businessHolidays - Unexpected Error:', err);

    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateBusinessHolidaysStatusDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_update_businessHoliday_status (?,?,?)',
        [
          data?.businessId,
          data?.holidayId ?? null,
          data?.status,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccaas_update_businessHoliday_status - Unexpected Error:', err);

          } else {
            resolve(result);
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('ccaas_update_businessHoliday_status - Unexpected Error:', err);

    }
  });
};
