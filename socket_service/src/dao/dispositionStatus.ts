import moment from 'moment';
import { executeMySQLWithRetry } from '../helpers/dbRetryUtil';
import { mysqlPoolConnection } from '../plugins/db';

/**
 *
 * @param data
 * @returns
 */
export const insertDispositionStatus = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      const currentDate = moment(new Date()).format('YYYY-MM-DD HH:mm:ss');
      const disposition = `INSERT INTO disposition_status (dispositionName,type,description,status,domainId,createdAt, colorCode) VALUES ('${data.dispositionName}','${data.type}','${data.description}',${data.status},${data.domainId},'${currentDate}', '${data.colorCode}')`;
      mysqlPoolConnection.query(disposition, (err, result) => {
        if (err) {
          reject(err);
        } else {
          resolve({
            did: result.insertId,
            dispositionName: data.dispositionName,
            type: data.type,
            description: data.description,
            status: data.status,
            domainId: data.domainId,
            colorCode: data.colorCode,
            createdAt: currentDate,
            updatedAt: '',
          });
        }
      });
    } catch (error) {
      reject(error);
    }
  });
};

export const getAgentListByDomianId = async (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_getDispositinStatus_info(?)',
        [
          data?.domainId
        ],
        (err, result) => {
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
export const getDispositinStatus = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_getDispositinStatus_info(?)',
        [
          data?.domainId
        ],
        (err: any, result: any) => {
          if (err) {
            console.log('ccaas_getDispositinStatus_info err: ', err);
            reject(err);
          }
          resolve(result);
        }
      );
    } catch (error) {
      console.log('ccaas_getDispositinStatus_info catch error: ', error);
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @param qdata
 * @returns
 */
export const updateDispositionStatus = (data: any, did: any) => {
  return new Promise((resolve, reject) => {
    try {
      const currentDate = moment(new Date()).format('YYYY-MM-DD HH:mm:ss');
      const updateq = `UPDATE disposition_status SET dispositionName ='${data.dispositionName}',type = '${data.type}',description = '${data.description}',status = ${data.status},updatedAt = '${currentDate}', colorCode = '${data.colorCode}'  WHERE did =${did} AND domainId = ${data.domainId}`;
      mysqlPoolConnection.query(updateq, (err, result) => {
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
export const updateDisposition = (data: any) => {
  return new Promise((resolve, reject) => {
    const query = `UPDATE sessionHistory SET dispositionName = '${data.dispositionName}' WHERE domainId = ${data.domainId}`;
    mysqlPoolConnection.query(query, (error, results) => {
      if (error) {
        reject(error);
      } else {
        resolve(results);
      }
    });
  });
};

/**
 *
 * @param data
 * @returns
 */
export const isValidDispositionName = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      const mysqlq = `SELECT * FROM disposition_status WHERE dispositionName= '${data.dispositionName}' AND domainId = ${data.domainId}`;
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
export const updateDispositionForCall = async (data: any) => {
  try {
    if ((data?.channelType === 'Email' || data?.channelType === 'Sms') && (data?.dispositionName === 'Closed' || data?.dispositionName === 'Resolved')) {
      data.markAsRead = 1;
    } else {
      data.markAsRead = 0;
    }

    console.log('ccaas_UpdateSessionDisposition_details', [
      data.domainId,
      data.sessionId,
      data.summary,
      data.disposition.did,
      data.afterCallWorkTime,
      data.calledPersonDetails.coid,
      data.subject,
      data?.markAsRead
    ]);

    const result = await executeMySQLWithRetry(
      mysqlPoolConnection,
      'call ccaas_UpdateSessionDisposition_details(?,?,?,?,?,?,?,?)',
      [
        data.domainId,
        data.sessionId,
        data.summary,
        data.disposition.did,
        data.afterCallWorkTime,
        data.calledPersonDetails.coid,
        data.subject,
        data?.markAsRead
      ],
      {
        operationName: 'UpdateCallDisposition_Socket',
        logContext: { sessionId: data?.sessionId, domainId: data?.domainId }
      }
    );

    return result?.length > 0 ? result[0] : null;
  } catch (err: any) {
    console.log('updateDispositionForCall err:', err);
    throw err;
  }
};

/**
 *
 * @param data
 * @returns
 */
export const updateCalledPersonDetailsCall = (data: any) => {
  return new Promise((resolve, reject) => {
    mysqlPoolConnection.query(
      'CALL ccaas_UpdateCalledPersonDetails(?,?,?)',
      [data?.calledPersonDetails?.coid, data?.domainId, data?.sessionId],
      (err: any, result: any) => {
        if (err) {
          reject(err);
          console.log(err);
        }
        resolve(result);
      }
    );
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateDispositionForSms = async (data: any) => {
  try {
    const result = await executeMySQLWithRetry(
      mysqlPoolConnection,
      'call up_UpdateSMSHistoryDisposition(?,?,?,?,?,?)',
      [
        data?.SMSHistoryId,
        data?.domainId,
        data?.dispositionId ?? null,
        data?.summary ?? null,
        data?.isSessionClosed ?? 0,
        data?.AfterSMSWorkTime ?? null
      ],
      {
        operationName: 'UpdateSMSDisposition',
        logContext: { SMSHistoryId: data?.SMSHistoryId, domainId: data?.domainId }
      }
    );

    return result?.[0];
  } catch (err) {
    console.log('updateDispositionForSms err:', err);
    throw err;
  }
};
