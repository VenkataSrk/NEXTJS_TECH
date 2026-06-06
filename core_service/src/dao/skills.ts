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
export const insertSkill = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      const currentDate = moment(new Date()).format('YYYY-MM-DD HH:mm:ss');
      const skills = `INSERT INTO skills (skill_name,skill_description,domainId,createdAt,status,type) VALUES ('${data.skill_name}','${data.skill_description}', ${data.domainId}, '${currentDate}',${data.status},'${data.type}')`;
      mysqlPoolConnection.query(skills, (err, result) => {
        if (err) {
          reject(err);
          logger.error('insertSkill - Unexpected Error:', err);
        } else {
          resolve({
            sid: result.insertId,
            skill_name: data.skill_name,
            skill_description: data.skill_description,
            domainId: data.domainId,
            createdAt: currentDate,
            updatedAt: '',
            status: data.status,
            no_of_agnt_sk: 0,
            type: data.type,
          });
        }
      });
    } catch (error) {
      reject(error);
      logger.error('insertSkill - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getSkill = (sid: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_skill(?)',
        [
          sid
        ],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('ccaas_get_skill - Unexpected Error:', err);
          } else {
            resolve(result[0]);
          }
        });
    } catch (error) {
      reject(error);
      logger.error('ccaas_get_skill - Unexpected Error:', error);
    }
  });
};
/**
 *
 * @param data
 * @param qdata
 * @returns
 */
export const updateSkill = (data: any, sid: any) => {
  return new Promise((resolve, reject) => {
    try {
      const name = [data.skill_name, data.skill_description, data.domainId, data.type, sid];
      const currentDate = moment(new Date()).format('YYYY-MM-DD HH:mm:ss');
      const updates = `UPDATE skills SET skill_name = ?,skill_description = ?, domainId = ?,type = ?, updatedAt = '${currentDate}'  WHERE sid = '${sid}'`;
      mysqlPoolConnection.query(updates, name, (err, result) => {
        if (err) {
          reject(err);
          logger.error('updateSkill - Unexpected Error:', err);
        } else {
          resolve(result);
        }
      });
    } catch (error) {
      reject(error);
      logger.error('updateSkill - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param qdata
 * @returns
 */
export const deleteSkill = (qdata: any) => {
  return new Promise((resolve, reject) => {
    try {
      const sid = [qdata];
      const updates = `DELETE FROM skills WHERE sid ='${sid}'`;
      mysqlPoolConnection.query(updates, (err, result) => {
        if (err) {
          reject(err);
          logger.error('deleteSkill - Unexpected Error:', err);
        } else {
          resolve(result);
        }
      });
    } catch (error) {
      reject(error);
      logger.error('deleteSkill - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getSearchList = (data: any, status: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_skill_details(?,?,?,?)',
        [
          data?.domainId,
          data?.searchByValue,
          data.limit,
          data.offset,
        ],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('ccaas_get_skill_details - Unexpected Error:', err);
          } else {
            resolve(result[0]);
          }
        });
    } catch (error) {
      reject(error);
      logger.error('ccaas_get_skill_details - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const skillStatus = (sid: any, data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_skill_status(?,?)',
        [
          sid,
          data.domainId,
        ],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('ccaas_get_skill_status - Unexpected Error:', err);
          } else {
            resolve(result[0]);
          }
        });
    } catch (error) {
      reject(error);
      logger.error('ccaas_get_skill_status - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @param qdata
 * @returns
 */
export const updateSkillStatus = (data: any, sid: any) => {
  return new Promise((resolve, reject) => {
    try {
      const updates = `UPDATE skills SET status = ${data.status} WHERE sid = ${sid}`;
      mysqlPoolConnection.query(updates, (err, result) => {
        if (err) {
          reject(err);
          logger.error('updateSkillStatus - Unexpected Error:', err);
        } else {
          resolve(result);
        }
      });
    } catch (error) {
      reject(error);
      logger.error('updateSkillStatus - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const isValidSkillName = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_isValid_skill_name(?,?)',
        [
          data.skill_name,
          data.domainId,
        ],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('ccaas_get_isValid_skill_name - Unexpected Error:', err);
          } else {
            resolve(result[0]);
          }
        });
    } catch (error) {
      reject(error);
      logger.error('ccaas_get_isValid_skill_name - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const isSkillEnableList = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_isSkill_enable_list(?)',
        [
          data.domainId,
        ],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('ccaas_get_isSkill_enable_list - Unexpected Error:', err);
          } else {
            resolve(result[0]);
          }
        });
    } catch (error) {
      reject(error);
      logger.error('ccaas_get_isSkill_enable_list - Unexpected Error:', error);
    }
  });
};
