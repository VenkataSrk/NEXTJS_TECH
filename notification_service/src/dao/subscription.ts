import { mysqlconnection } from '../plugin/db';

export const getDeviceInfo = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlconnection.query('CALL get_Call_register_details(?,?,?)', [
        parseInt(data.ext, 10),
        parseInt(data.domainId, 10),
        data.deviceType

      ],                    (err: any, result: any) => {
        if (err) {
          reject(err);
        }
        resolve(result[0].length > 0 ? result[0] : []);
      });
    } catch (err) {
      reject(err);
    }
  });
};

export const getUserListByRole = (domainId: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlconnection.query(
        'call ccaas_getUserListByRoleHandler(?,?)',
        [domainId ?? null, '1,2,3,5,6'],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            console.log('getUserListByRole reject err: ', err);
          } else {
            resolve(result[0]);
          }
        }
      );
    } catch (error) {
      reject(error);
    }
  });
};
