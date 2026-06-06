/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */
import moment from 'moment';
import { mysqlPoolConnection } from '../plugins/db';

/**
 *
 * @param data
 * @returns
 */
export const callflowMap = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      const currentDate = moment(new Date()).format('YYYY-MM-DD HH:mm:ss');
      const callInsert = `INSERT INTO callMap (sourceId, callFlowId,domainId,description,codeSnippet,createdAt) VALUES ('${data.sourceId}', '${data.callFlowId}',${data.domainId},'${data.description}','${data.codeSnippet}','${currentDate}')`;
      mysqlPoolConnection.query(callInsert, (err, result) => {
        if (err) {
          reject(err);
        } else {
          resolve({
            cid: result.insertId,
            sourceId: data.sourceId,
            callFlowId: data.callFlowId,
            domainId: data.domainId,
            description: data.description,
            codeSnippet: data.codeSnippet,
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
/**
 *
 * @param data
 * @returns
 */
export const getCallflowMap = (cid: number) => {
  return new Promise((resolve, reject) => {
    try {
      const mysqlq = `SELECT * FROM callMap WHERE cid = '${cid}'`;
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
 * @param qdata
 * @returns
 */
export const updateCallFlowMap = (data: any, cid: any) => {
  return new Promise((resolve, reject) => {
    try {
      const name = [data.callFlowId, cid];
      const currentDate = moment(new Date()).format('YYYY-MM-DD HH:mm:ss');
      const updatep = `UPDATE callMap SET callFlowId = ?,updatedAt = '${currentDate}' WHERE cid = ${cid}`;
      mysqlPoolConnection.query(updatep, name, (err, result) => {
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
 * @param qdata
 * @returns
 */
export const deletecallFlow = (cid: any) => {
  return new Promise((resolve, reject) => {
    try {
      const datad = [cid];
      const updateq = `DELETE FROM callMap WHERE cid ='${datad}'`;
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
export const getcallflowList = () => {
  return new Promise((resolve, reject) => {
    try {
      const mysqlq = 'SELECT * FROM callMap';
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
export const callflowSetting = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      const currentDate = moment(new Date()).format('YYYY-MM-DD HH:mm:ss');
      const callInsert = `INSERT INTO callMap (callFlow,domainId,status,description,sourceId,createdAt) VALUES ('${data.callFlow}',${data.domainId},'${data.status}','${data.description}','${data.sourceId}','${currentDate}')`;
      mysqlPoolConnection.query(callInsert, (err, result) => {
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
export const getcallflowSetting = (domainId: any) => {
  return new Promise((resolve, reject) => {
    try {
      const mysqlq = `SELECT * FROM callMap WHERE domainId = ${domainId} order by cid desc`;
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
 * @param datau
 * @returns
 */
export const updateCallFlowMap1 = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_call_update_callMap_sourceid_details(?,?,?)',
        [
          data?.your_number ?? null,
          data?.domainId,
          data?.callFlowId ?? null,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
          }
          resolve(result[0]);
        }
      );
    } catch (err) {
      reject(err);
    }
  });
};
