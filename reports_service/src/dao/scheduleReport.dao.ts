import axios from 'axios';
import moment from 'moment';
import { logger } from '../../src/plugins/log';
import { mysqlPoolConnection } from '../plugins/db';

/**
 *
 * @param data
 * @returns
 */
export const insertScheduleReportDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_schedule_report_info(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          data?.reportName,
          data?.recurrence,
          data?.rrule ?? null,
          data?.isRecurr ?? 0,
          data?.timezone ?? null,
          data?.isSetPassword ?? null,
          data?.setPassword ?? null,
          data?.fileType ?? null,
          data?.toAddr ?? null,
          data?.subject ?? null,
          data?.message ?? null,
          data?.domainId,
          data?.nextOccurrence,
          data?.startDataRange,
          data?.endDataRange
        ],
        (err: any, result: any) => {
          if (err) {
            console.log(err);
            reject(err);
            logger.error('insertScheduleReportDao - Unexpected Error:', err);
          } else {
            if (result[0]?.length > 0) {
              resolve(result[0]);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (err) {
      console.log(err);
      reject(err);
      logger.error('insertScheduleReportDao - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getScheduleReportDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_get_schedule_report_info(?)',
        [
          data?.domainId,
        ],
        (err: any, result: any) => {
          if (err) {
            console.log(err);
            reject(err);
            logger.error('getScheduleReportDao - Unexpected Error:', err);
          } else {
            if (result[0]?.length > 0) {
              resolve(result[0]);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (err) {
      console.log(err);
      reject(err);
      logger.error('getScheduleReportDao - Unexpected Error:', err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const deleteScheduleReportDao = (data: any, uid: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_delete_schedule_report_info(?,?)',
        [
          data?.domainId,
          uid,
        ],
        (err: any, result: any) => {
          if (err) {
            resolve(err);
            logger.error('deleteScheduleReportDao - Unexpected Error:', err);
          } else {
            resolve(result);
          }
        }
      );
    } catch (err) {
      console.log(err);
      reject(err);
      logger.error('deleteScheduleReportDao - Unexpected Error:', err);
    }
  });
};
