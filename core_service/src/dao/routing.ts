import moment from 'moment';
import { mysqlPoolConnection } from '../plugins/db';
import { logger } from '../plugins/winston';

/**
 *
 * @param data
 * @returns
 */
export const routingqueue = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      const currentDate = moment(new Date()).format('YYYY-MM-DD HH:mm:ss');
      const mysqlq = `INSERT INTO routing_profile (name, description, default_outbound_queue, No_of_linked_queues,status,domainId, createdAt,routingMethod,concurrentChat) VALUES ('${data.name}','${data.description}','${data.default_outbound_queue}',${data.No_of_linked_queues},'${data.status}',${data.domainId},'${currentDate}','${data.routingMethod}',${data.concurrentChat})`;
      mysqlPoolConnection.query(mysqlq, (err, res) => {
        if (err) {
          logger.error('routing_profile - Unexpected Error:', err);
          reject(err);
        } else {
          const resultData = {
            rid: res.insertId,
            name: data.name,
            description: data.description,
            default_outbound_queue: data.default_outbound_queue,
            No_of_linked_queues: data.No_of_linked_queues,
            status: data.status,
            domainId: data.domainId,
            createdAt: currentDate,
            updatedAt: '',
            routingMethod: data.routingMethod,
            concurrentChat: data.concurrentChat,
          };
          const routingProfiles = [];
          data?.routingqueue?.forEach((queue: any, index: any) => {
            const mysqlrouting = 'INSERT INTO routingqueueprofilemapping (rqmid,qid, calls, chat,priority,delay_in_seconds, createdAt,voice,domainId) VALUES ? ';
            const values = [[res.insertId, queue.qid, queue.calls, queue.chat, queue.priority, queue.delay_in_seconds, currentDate, queue.voice, data.domainId]];
            mysqlPoolConnection.query(mysqlrouting, [values], (error, result) => {
              if (err) {
                reject(error);
                logger.error('routingqueueprofilemapping - Unexpected Error:', error);
              } else {
                routingProfiles.push({
                  rqid: result.insertId,
                  rqmid: res.insertId,
                  qid: queue.qid,
                  calls: queue.calls,
                  chat: queue.chat,
                  priority: queue.priority,
                  delay_in_seconds: queue.delay_in_seconds,
                  createdAt: currentDate,
                  voice: queue.voice,
                  domainId: data.domainId,
                });
                if (index === data?.routingqueue?.length - 1) {
                  resolve({ ...resultData, routingqueue: routingProfiles });
                }
              }
            });
          });
        }
      });
    } catch (error) {
      reject(error);
      logger.error('routingqueue - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getRouting = (dataq: any, data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_routing(?,?)',
        [
          dataq,
          data,
        ],
        (err, result) => {
          if (err) {
            logger.error('ccaas_get_routing - Unexpected Error:', err);
            reject(err);
          } else {
            const json = {
              routing: result[0],
              routingqueuedetails: result[1],
            };
            resolve(json);
          }
        });
    } catch (error) {
      reject(error);
      logger.error('ccaas_get_routing - Unexpected Error:', error);

    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateRouting = (data: any, dataq: any) => {
  return new Promise((resolve, reject) => {
    try {
      const name = [
        data.name,
        data.description,
        data.default_outbound_queue,
        data.No_of_linked_queues,
        data.status,
        data.domainId,
        data.routingMethod,
        data.concurrentChat,
        data.type,
        dataq,
      ];

      if (data.routingqueue.length > 0) {
        const deleteQuery = `DELETE FROM routingqueueprofilemapping where rqmid = ${data.routingqueue[0]?.rqmid}`;
        mysqlPoolConnection.query(deleteQuery, (error, result) => {
          if (error) {
            reject(error);
            logger.error('delete routingqueueprofilemapping - Unexpected Error:', error);

          } else {
            resolve(result);
            const currentDatec = moment(new Date()).format('YYYY-MM-DD HH:mm:ss');
            for (const routing of data.routingqueue) {
              const mysqlrouting =
                'INSERT INTO routingqueueprofilemapping (rqmid,qid, calls, chat, emails, sms, social_apps,priority,delay_in_seconds, createdAt,voice,domainId) VALUES ? ';
              const values = [
                [
                  routing.rqmid,
                  routing.qid,
                  routing.calls,
                  routing.chat,
                  routing.emails,
                  routing.sms,
                  routing.social_apps,
                  routing.priority,
                  routing.delay_in_seconds,
                  currentDatec,
                  routing.voice,
                  data.domainId,
                ],
              ];
              mysqlPoolConnection.query(mysqlrouting, [values], (err, resultt) => {
                if (err) {
                  logger.error('INSERT INTO routingqueueprofilemapping - Unexpected Error:', err);

                  reject(err);
                } else {
                  resolve(resultt);
                }
              });
            }
          }
        });
      }
      const currentDate = moment(new Date()).format('YYYY-MM-DD HH:mm:ss');
      const routingp = `update routing_profile SET name =?, description =?, default_outbound_queue =?, No_of_linked_queues =?, status =?,updatedAt = '${currentDate}',domainId =?,routingMethod=?,concurrentChat=?,type = ? WHERE rid = ${dataq}`;
      mysqlPoolConnection.query(routingp, name, (err, result) => {
        if (err) {
          logger.error('update routing_profile - Unexpected Error:', err);
          reject(err);
        }
      });
    } catch (error) {
      reject(error);
      logger.error('updateRouting - Unexpected Error:', error);

    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const deleteRouting = (dataq: any) => {
  return new Promise((resolve, reject) => {
    try {
      const rid = [dataq];
      const rqmid = [dataq];
      const deleteq = `DELETE FROM routing_profile WHERE rid = ${rid}`;
      const deletep = `DELETE FROM routingqueueprofilemapping WHERE rqmid = ${rqmid}`;
      mysqlPoolConnection.query(deleteq, (err, result) => {
        if (err) {
          logger.error('deleteRouting deleteq- Unexpected Error:', err);

          reject(err);
        } else {
          resolve(result);
        }
      });
      mysqlPoolConnection.query(deletep, (err, result) => {
        if (err) {
          logger.error('deleteRouting deletep - Unexpected Error:', err);

          reject(err);
        } else {
          resolve(result);
        }
      });
    } catch (error) {
      reject(error);
      logger.error('deleteRouting - Unexpected Error:', error);

    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getSearchList = (data: any) => {
  // ccaas_getCountOfRoutingProfile_info
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_getSearchList_info(?)',
        [
          data?.domainId
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccaas_getSearchList_info - Unexpected Error:', err);
          }
          if (Array.isArray(result) && result[0]?.length > 0) {
            resolve(result[0]);
          }
        }
      );
    } catch (error) {
      reject(error);
      logger.error('ccaas_getSearchList_info - Unexpected Error:', error);

    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const routingProfileStatus = (rid: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_routing_profile_status(?)',
        [
          rid
        ],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('ccaas_routing_profile_status - Unexpected Error:', err);
          } else {
            if (Array.isArray(result) && result.length > 0) {
              resolve(result[0][0]);
            }
            resolve({});
          }
        });
    } catch (error) {
      reject(error);
      logger.error('ccaas_routing_profile_status - Unexpected Error:', error);

    }
  });
};

/**
 *
 * @param rid
 * @param domainId
 * @returns
 */
export const routingProfileUser = (rid: any, domainId: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_routing_profile_user(?,?)',
        [
          rid,
          domainId
        ],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('ccaas_routing_profile_user - Unexpected Error:', err);
          } else {
            if (Array.isArray(result) && result.length > 0) {
              resolve(result?.[0]?.[0]);
            }
            resolve({});
          }
        });
    } catch (error) {
      reject(error);
      logger.error('ccaas_routing_profile_user - Unexpected Error:', error);

    }
  });
};

/**
 *
 * @param data
 * @param qdata
 * @returns
 */
export const updateStatus = (data: any, rid: any) => {
  return new Promise((resolve, reject) => {
    try {
      const updates = `UPDATE routing_profile SET status = ${data.status} WHERE rid = ${rid}`;
      mysqlPoolConnection.query(updates, (err, result) => {
        if (err) {
          reject(err);
          logger.error('UPDATE routing_profile - Unexpected Error:', err);
        } else {
          resolve(result);
        }
      });
    } catch (error) {
      reject(error);
      logger.error('UPDATE routing_profile - Unexpected Error:', error);

    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const isValidRoutingName = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_isValid_routing_name(?,?)',
        [
          data.domainId,
          data.name
        ],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('ccaas_isValid_routing_name - Unexpected Error:', err);
          } else {
            resolve(result?.[0]);
          }
        });
    } catch (error) {
      reject(error);
      logger.error('ccaas_isValid_routing_name - Unexpected Error:', error);

    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const isRoutingEnableList = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_isRouting_enable_list(?)',
        [
          data.domainId,
        ],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('ccaas_isRouting_enable_list - Unexpected Error:', err);
          } else {
            resolve(result?.[0]);
          }
        });
    } catch (error) {
      reject(error);
      logger.error('ccaas_isRouting_enable_list - Unexpected Error:', error);

    }
  });
};
