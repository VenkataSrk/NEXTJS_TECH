import {
  mysqlPoolConnection,
  mysqlPoolConnectionRole,
  notificationMessages,
} from '../plugins/db';

/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

import moment from 'moment';
import { v4 as uuidv4 } from 'uuid';
import { ROLES } from '../helpers/constants';
import { logger } from '../plugins/winston';

/**
 * @param data
 * @param qdata
 * @returns
 */
// table not in use needs migration
export const updateUser = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      const selectQuery = `SELECT * FROM user WHERE emailId ='${data.emailId}' AND domainId = ${data.domainId}`;
      mysqlPoolConnection.query(selectQuery, (err, result) => {
        if (err) {
          reject(err);
          logger.error('updateUser - Unexpected Error:', err);
        } else {
          let updateU: any = JSON.stringify(result);
          updateU = JSON.parse(updateU ?? 'null');
          if (updateU.length === 1) {
            const update = [
              data.emailId,
              data.ext,
              data.companyId,
              data.domainId,
              data.directNumber,
              data.address,
              data.routing_profile,
              data.report_to,
              data.phone_type,
              data.phone_number,
              data.desk_phone_number,
              data.after_call_work_timeout,
              data.initial_setup_menu,
              data.initial_setup_flag,
              data.add_skill,
              data.country_code,
              data.initial_selection_flag,
              data.state,
              data.stateTimer,
              data.name,
              data.localization,
              data.routing_profile_name,
              data.active_status,
              data.outBound_call,
              data.report_to_name,
              data.isVoice,
              data.isVoiceCallTransfer,
              data.isChat,
              data.isChatTransfer,
              data.isEmail,
              data.isEmailTransfer,
            ];
            const currentDate = moment(new Date()).format(
              'YYYY-MM-DD HH:mm:ss'
            );
            let updateq = 'UPDATE user a  SET ';
            updateq += data.ext ? `a.ext = ifNull('${data.ext}',a.ext),` : '';
            updateq += data.companyId
              ? `a.companyId = ifNull('${data.companyId}',a.companyId),`
              : '';
            updateq += data.directNumber
              ? `a.directNumber = ifNull('${data.directNumber}',a.directNumber),`
              : 'a.directNumber = ifNull(null,a.directNumber),';
            updateq += data.address
              ? `a.address = ifNull('${data.address}',a.address),`
              : 'a.address = ifNull(null,a.address),';
            updateq += data.routing_profile
              ? `a.routing_profile = ifNull('${data.routing_profile}',a.routing_profile),`
              : 'a.routing_profile = ifNull(0,a.routing_profile),';
            updateq += data.report_to
              ? `a.report_to = ifNull('${data.report_to}',a.report_to),`
              : 'a.report_to = ifNull(null,a.report_to),';
            updateq += data.phone_type
              ? `a.phone_type = ifNull('${data.phone_type}',a.phone_type),`
              : 'a.phone_type = ifNull(null,a.phone_type),';
            updateq += data.phone_number
              ? `a.phone_number = ifNull('${data.phone_number}',a.phone_number),`
              : 'a.phone_number = ifNull(null,a.phone_number),';
            updateq += data.desk_phone_number
              ? `a.desk_phone_number = ifNull(${data.desk_phone_number},a.desk_phone_number),`
              : 'a.desk_phone_number = ifNull(null,a.desk_phone_number),';
            updateq += data.after_call_work_timeout
              ? `a.after_call_work_timeout = ifNull('${data.after_call_work_timeout}',a.after_call_work_timeout),`
              : 'a.after_call_work_timeout = ifNull(null,a.after_call_work_timeout),';
            updateq += data.initial_setup_menu
              ? `a.initial_setup_menu = ifNull('${data.initial_setup_menu}',a.initial_setup_menu),`
              : 'a.initial_setup_menu = ifNull(null,a.initial_setup_menu),';
            updateq += data.initial_setup_flag
              ? `a.initial_setup_flag = ifNull(${data.initial_setup_flag},a.initial_setup_flag),`
              : 'a.initial_setup_flag = ifNull(null,a.initial_setup_flag),';
            updateq += data.add_skill
              ? `a.add_skill = ifNull('${JSON.stringify(
                data.add_skill
              )}',a.add_skill),`
              : 'a.add_skill = ifNull(null,a.add_skill),';
            updateq += data.country_code
              ? `a.country_code = ifNull('${data.country_code}',a.country_code),`
              : 'a.country_code = ifNull(null,a.country_code),';
            updateq += data.initial_selection_flag
              ? `a.initial_selection_flag = ifNull('${data.initial_selection_flag}',a.initial_selection_flag),`
              : 'a.initial_selection_flag = ifNull(null,a.initial_selection_flag),';
            updateq += data.state
              ? `a.state = ifNull('${data.state}',a.state),`
              : 'a.state = ifNull(null,a.state),';
            updateq += data.stateTimer
              ? `a.stateTimer = ifNull('${data.stateTimer}',a.stateTimer),`
              : 'a.stateTimer = ifNull(null,a.stateTimer),';
            updateq += data.callType
              ? `a.callType = ifNull('${data.callType}',a.callType),`
              : 'a.callType = ifNull(null,a.callType),';
            updateq += data.customer
              ? `a.customer = ifNull('${data.customer}',a.customer),`
              : 'a.customer = ifNull(null,a.customer),';
            updateq += data.localization
              ? `a.localization = ifNull('${data.localization}',a.localization),`
              : 'a.localization = ifNull(null,a.localization),';
            updateq += data.name
              ? `a.name = ifNull('${data.name}',a.name),`
              : 'a.name = ifNull(null,a.name),';
            updateq += data.routing_profile_name
              ? `a.routing_profile_name = ifNull('${data.routing_profile_name}',a.routing_profile_name),`
              : 'a.routing_profile_name = ifNull(null,a.routing_profile_name),';
            // updateq += data.active_status ? `a.active_status = ifNull('${data.active_status}',a.active_status)` : 'a.active_status = ifNull(0,a.active_status),';
            updateq += data.outBound_call
              ? `a.outBound_call = ifNull('${data.outBound_call}',a.outBound_call),`
              : 'a.outBound_call = ifNull(0,a.outBound_call),';
            updateq += data.report_to_name
              ? `a.report_to_name = ifNull('${data.report_to_name}',a.report_to_name),`
              : 'a.report_to_name = ifNull(null,a.report_to_name),';
            updateq += `a.updatedAt = '${currentDate}' WHERE a.emailId = '${data.emailId}'`;
            mysqlPoolConnection.query(
              updateq,
              update,
              (error, updateResult) => {
                if (error) {
                  reject(error);
                  logger.error('updateUser - Unexpected Error:', error);
                } else {
                  updateResult.flag = 1;
                  resolve(updateResult);
                  if (data?.add_skill) {
                    data?.add_skill?.map((skill: any) => {
                      const mysqlUser = `INSERT INTO skillMapping (skid,usid, domainId,createdAt) VALUES (${skill.skid},${updateU[0].uid},${updateU[0].domainId},'${currentDate}')`;
                      mysqlPoolConnection.query(mysqlUser, (errr, resultt) => {
                        if (errr) {
                          reject(err);
                          logger.error('updateUser - Unexpected Error:', err);
                        } else {
                          resolve(resultt);
                        }
                      });
                    });
                  } else {
                    return err;
                  }
                }
              }
            );
          } else {
            const currentDate = moment(new Date()).format(
              'YYYY-MM-DD HH:mm:ss'
            );
            const mysqlq = `INSERT IGNORE INTO user (emailId,domainId,directNumber,address,createdAt,routing_profile,report_to,phone_type,phone_number,desk_phone_number,after_call_work_timeout,initial_setup_menu,initial_setup_flag,add_skill,country_code,initial_selection_flag)
        VALUES ('${data.emailId}',${data.domainId},'${data.directNumber}','${data.address}','${currentDate}','${data.routing_profile}','${data.report_to}','${data.phone_type}','${data.phone_number}','${data.desk_phone_number}','${data.after_call_work_timeout}','${data.initial_setup_menu}',${data.initial_setup_flag},'${data.add_skill}','${data.country_code}',${data.initial_selection_flag})`;
            mysqlPoolConnection.query(mysqlq, (errr, resultt) => {
              if (errr) {
                reject(errr);
              } else {
                result.flag = 2;
                resolve(resultt);
              }
            });
          }
        }
      });
    } catch (error) {
      reject(error);
      logger.error('updateUser - Unexpected Error:', error);
    }
  });
};
/**
 *
 * @param data
 * @returns
 */
// this table not in use need to do migration
export const getuserdetails = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      const validate = `SELECT * FROM user where emailId ='${data.emailId}'`;
      mysqlPoolConnection.query(validate, async (err, rows) => {
        if (err) {
          reject(err);
          logger.error('getuserdetails - Unexpected Error:', err);
        } else if (rows.length > 0) {
          let ds: any = JSON.stringify(rows[0]);
          ds = JSON.parse(ds ?? 'null');
          resolve(ds);
        } else {
          resolve({ statusCode: 404 });
        }
      });
    } catch (error) {
      reject(error);
      logger.error('getuserdetails - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getAgentActivityDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_GetActiveAgentForSupervisor(?,?)',
        [data?.emailId, data?.domainId],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('up_GetActiveAgentForSupervisor - Unexpected Error:', err);

          }
          resolve(result[0]);
        }
      );
    } catch (error) {
      reject(error);
      logger.error('up_GetActiveAgentForSupervisor - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getAgentSupervisior = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_GetUserId(?,?)',
        [
          data?.ext,
          data?.domainId
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('up_GetUserId - Unexpected Error:', err);
          }
          resolve(result[0]);
        }
      );
    } catch (error) {
      reject(error);
      logger.error('up_GetUserId - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updatePassword = (data: any, changedUser: any) => {
  return new Promise((resolve, reject) => {
    try {
      const currentDate = moment(new Date()).format('YYYY-MM-DD HH:mm:ss');
      const updateq = `UPDATE user SET password = '${data}' ,updatedAt = '${currentDate}' WHERE emailId = '${changedUser.emailId}'`;
      mysqlPoolConnection.query(updateq, async (err, rows) => {
        if (err) {
          reject(err);
          logger.error('updatePassword - Unexpected Error:', err);
        } else {
          resolve(rows);
        }
      });
    } catch (error) {
      reject(error);
      logger.error('updatePassword - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param  * @returns
 */
// this table is empty in the old and also in new table need to do migration
export const smtpConfig = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      const mysqlq = `SELECT * FROM smtp_service_settings where domainId = ${data.domainId}`;
      mysqlPoolConnection.query(mysqlq, async (err, result) => {
        if (err) {
          reject(err);
          logger.error('smtpConfig - Unexpected Error:', err);

        }
        resolve(result);
      });
    } catch (error) {
      reject(error);
      logger.error('smtpConfig - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @param dataq
 * @returns
 */
export const getUserListByRole = (data: any, dataq: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_getUserListByRoleHandler(?,?)',
        [dataq?.domainId, data],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccaas_getUserListByRoleHandler - Unexpected Error:', err);

          } else {
            resolve(result[0]);
          }
        }
      );
    } catch (error) {
      reject(error);
      logger.error('ccaas_getUserListByRoleHandler - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param report_to
 * @param domainId
 * @returns
 */
export const getUserDeatils = (report_to: any, domainId: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_getUserDeatils_info(?,?)',
        [report_to, domainId],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccaas_getUserDeatils_info - Unexpected Error:', err);

          } else {
            resolve(result);
          }
        }
      );
    } catch (error) {
      reject(error);
      logger.error('ccaas_getUserDeatils_info - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getAllUserList = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_getAllUserList_info(?,?)',
        [data?.domainId, data?.ext ?? 0],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccaas_getAllUserList_info - Unexpected Error:', err);

          } else {
            resolve(result);
          }
        }
      );
    } catch (error) {
      reject(error);
      logger.error('ccaas_getAllUserList_info - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getAllProfileInfo = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_getProfileInformation(?,?)',
        [
          data?.ext ?? 0,
          data?.domainId

        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccaas_getProfileInformation - Unexpected Error:', err);

          } else {
            resolve(result[0]);
          }
        }
      );
    } catch (error) {
      reject(error);
      logger.error('ccaas_getProfileInformation - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @param uid
 * @returns
 */
export const getUserDetailsList = (data: any, uid: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_getUserDetailsList_info(?,?)',
        [data?.domainId, uid],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccaas_getUserDetailsList_info - Unexpected Error:', err);

          }
          if (Array.isArray(result) && result[0].length > 0) {
            resolve(result[0]);
          }
        }
      );
    } catch (error) {
      reject(error);
      logger.error('ccaas_getUserDetailsList_info - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const userUpdate = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_userUpdate_info(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          data?.emailId,
          data?.domainId,
          data?.routing_profile,
          data?.report_to,
          JSON.stringify(data?.add_skill),
          data?.outBound_call,
          data?.voice,
          data?.chat,
          data?.email,
          data?.sms,
          data?.socialMedia,
          data?.chatLimit,
          data?.pri_language,
          data?.sec_language,
          data?.inboundCall,
          data?.outboundCampaignCall,
          data?.is_outbound_campaign_specific_agent,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccaas_userUpdate_info - Unexpected Error:', err);

          }
          if (Array.isArray(result) && result[0].length > 0) {
            resolve(result[0]);
          }
        }
      );
    } catch (error) {
      reject(error);
      logger.error('ccaas_userUpdate_info - Unexpected Error:', error);
    }
  });
};
/**
 *
 * @param data
 * @returns
 */
export const insertQueue = (data: any) => {
  const hoursOfOperation = JSON.stringify(data.hours_of_operation) ?? '';
  const settingAudio = JSON.stringify(data.settingAudio) ?? '';
  const announcement = JSON.stringify(data.announcement) ?? '';
  const maximumCallers = JSON.stringify(data.maximumCallers) ?? '';
  const maximumWaitTime = JSON.stringify(data.maximumWaitTime);
  const outOfBusinessHours = JSON.stringify(data.outOfBusinessHours) ?? '';
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_myacc_insert_queue(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          data?.name,
          data?.description,
          data?.domainId,
          settingAudio,
          announcement,
          maximumCallers,
          maximumWaitTime,
          data?.OutboundCallerId,
          data?.outBoundNumber,
          data?.maxCallHandlingTimeMM,
          data?.callWrapUpTime,
          data?.outBoundCallStatus,
          data?.hoursOfOperatioId,
          hoursOfOperation,
          outOfBusinessHours,
          data?.status,
          data?.type
        ],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('ccaas_myacc_insert_queue - Unexpected Error:', err);

          } else {
            resolve(result?.[0]);
          }
        });
    } catch (error) {
      reject(error);
      logger.error('ccaas_myacc_insert_queue - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const insertRouting = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_myacc_insert_routing(?,?,?,?,?,?,?,?,?)',
        [
          data?.name,
          data?.description,
          data?.default_outbound_queue,
          data?.No_of_linked_queues,
          data?.status,
          data?.domainId,
          data?.routingMethod,
          data?.concurrentChat,
          data?.type
        ],
        (err, res) => {
          if (err) {
            reject(err);
            logger.error('ccaas_myacc_insert_routing - Unexpected Error:', err);

          } else {
            // resolve(res);
            data?.routingqueue?.map((queue: any) => {
              mysqlPoolConnection.query(
                'call ccaas_myacc_insert_routing_queue(?,?,?,?,?,?,?,?,?,?,?)',
                [
                  res[0][0].rid,
                  queue.qid,
                  queue.calls ?? null,
                  queue.chat,
                  queue.emails ?? null,
                  queue.sms ?? null,
                  queue.social_apps ?? null,
                  queue.priority,
                  queue.delay_in_seconds,
                  data.domainId,
                  queue.voice,
                ],
                (error, result) => {
                  if (error) {
                    reject(error);
                    logger.error('ccaas_myacc_insert_routing - Unexpected Error:', error);
                  } else {
                    resolve(result?.[0]?.[0]);
                  }
                }
              );
            });
          }
        });
    } catch (error) {
      reject(error);
      logger.error('ccaas_myacc_insert_routing - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const inserSkill = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_myacc_insert_skill(?,?,?,?,?,?)',
        [
          data?.sid ?? null,
          data?.skill_name,
          data?.skill_description,
          data?.domainId,
          data.status,
          data?.type ?? null
        ],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('ccaas_myacc_insert_skill - Unexpected Error:', err);

          } else {
            resolve(result?.[0]?.[0]);
          }
        });
    } catch (error) {
      reject(error);
      logger.error('ccaas_myacc_insert_skill - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const basicInsertPrompt = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_myacc_insert_prompt(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          null,
          data?.fieldname,
          data?.encoding,
          data?.mimetype,
          data?.filename,
          data?.path,
          data?.size,
          data?.type,
          data?.promptType,
          data?.url,
          data?.name,
          data?.description,
          data?.domainId,
          data?.companyId,
          data?.message,
          data?.promptStatus,
        ],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('ccaas_myacc_insert_prompt - Unexpected Error:', err);

          } else {
            resolve(result?.[0]?.[0]);
          }
        });
    } catch (error) {
      reject(error);
      logger.error('ccaas_myacc_insert_prompt - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const insertOnboardUser = (data: any) => {
  return new Promise(async (resolve, reject) => {
    const addSkill = JSON.stringify(data?.addSkill);
    try {
      mysqlPoolConnection.query(
        'call insertOnboardUser(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          data?.UserName ?? null,
          data?.emailId ?? null,
          data?.directNumber ?? 0,
          data?.address ?? null,
          data?.routing_profile_id ?? 0,
          data?.report_to_UserID ?? 0,
          data?.sipLoginId ?? 0,
          data?.companyId ?? 0,
          data?.domainId ?? 0,
          data?.ext ?? 0,
          data?.isActive ?? 0,
          data?.callTypeId ?? 0,
          data?.roleid ?? 0,
          data?.localization ?? 0,
          data?.currentSession ?? 0,
          data?.isVoice ?? 0,
          data?.isVoiceCallTransfer ?? 0,
          data?.isChat ?? 0,
          data?.isChatTransfer ?? 0,
          data?.isEmail ?? 0,
          data?.isEmailTransfer ?? 0,
          data?.StatusID ?? 0,
          data?.isSms ?? 0,
          data?.isSocialMedia ?? 0,
          addSkill ?? [],
          data?.voice ?? 0,
          data?.chat ?? 0,
          data?.email ?? 0,
          data?.sms ?? 0,
          data?.socialMedia ?? 0,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('insertOnboardUser - Unexpected Error:', err);

          } else {
            resolve(result[0]);
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('insertOnboardUser - Unexpected Error:', err);

    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getRoutingProfile = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_routing_default_details(?)',
        [
          data ?? null,
        ],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('ccaas_get_routing_default_details - Unexpected Error:', err);

          } else {
            resolve(result?.[0]);
          }
        });
    } catch (error) {
      reject(error);
      logger.error('ccaas_get_routing_default_details - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @param nodeData
 * @returns
 */
export const insertBasicCallFlow = (data: any, nodeData: any) => {
  const node = nodeData?.nodes;
  const edge = nodeData?.edges;
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_call_Flow_details(?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          data?.cfid,
          data?.templateName,
          node,
          edge,
          data?.version,
          data?.type,
          data?.createDate,
          data?.lastModifiedDate,
          data?.status,
          data?.companyId,
          data?.domainId,
          data?.description,
          data?.publishedDataArr,
          data?.callFlowId ?? 0
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccaas_call_Flow_details - Unexpected Error:', err);

          }
          resolve(result);
        }
      );
    } catch (err) {
      reject(err);
      logger.error('ccaas_call_Flow_details - Unexpected Error:', err);

    }
  });
};

/**
 *  chatFlow
 */

export const insertBasicChatFlow = (data: any, nodeData: any) => {
  const node = nodeData?.nodes;
  const edge = nodeData?.edges;
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_InsertUpdatechatFlowHistory(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          0,
          data?.templateName ?? null,
          node ?? null,
          edge ?? null,
          data?.version ?? null,
          data?.type ?? 'Default',
          data?.status ?? 0,
          data?.companyId ?? 0,
          data?.domainId ?? 0,
          data?.description ?? null,
          data?.publishedDataArr ?? null,
          data?.action,
          data?.category,
          'Chat',
          'English'
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('up_InsertUpdatechatFlowHistory - Unexpected Error:', err);

          }
          resolve(result);
        }
      );
    } catch (err) {
      reject(err);
      logger.error('up_InsertUpdatechatFlowHistory - Unexpected Error:', err);

    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const insertOnboardDidNumber = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_InsertUpdate_did_purchase(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          0,
          data?.user_id ?? 0,
          data?.city ?? null,
          data?.country ?? null,
          data?.your_number ?? null,
          data?.domainId ?? null,
          data?.callFlowId ?? null,
          data?.recording ?? 1,
          data?.status ?? 1,
          data?.queueName ?? null,
          data?.isCall ?? 0,
          data?.isSms ?? 0,
          data?.aliasName ?? null,
          data?.outboundSms ?? 0,
          data?.smsThreshold ?? 0,
          data?.callFlowName ?? null,
          data?.defaultUpdated ?? 0,
          data?.supervisorName ?? null,
          data?.isCallIn ?? 0,
          data?.isCallOut ?? 0,
          data?.isSmsOut ?? 0,
          data?.isSmsIn ?? 0,
          data?.type ?? null,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('up_InsertUpdate_did_purchase - Unexpected Error:', err);

          } else {
            resolve(result[0]);
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('up_InsertUpdate_did_purchase - Unexpected Error:', err);

    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const loginSessionHistory = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_InsertloginSessionHistory_info(?,?,?,?,?,?)',
        [
          data?.logInSessionId,
          data?.logInTimestamp,
          data?.sipLoginId,
          data?.domainId,
          data?.companyId,
          data?.ext,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccaas_InsertloginSessionHistory_info - Unexpected Error:', err);

          }
          resolve(result);
        }
      );
    } catch (error) {
      reject(error);
      logger.error('ccaas_InsertloginSessionHistory_info - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateSessionHistory = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_UpdateloginSessionHistory_info(?,?,?,?,?,?)',
        [
          data?.logInSessionId,
          data?.logOutTimestamp,
          data?.sipLoginId,
          data?.domainId,
          data?.companyId,
          data?.ext,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccaas_UpdateloginSessionHistory_info - Unexpected Error:', err);

          }
          resolve(result[0]);
        }
      );
    } catch (error) {
      reject(error);
      logger.error('ccaas_UpdateloginSessionHistory_info - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getSessionDetails = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_get_session_queue_agent_info(?,?)',
        [data.domainId, data.sessionId],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccaas_get_session_queue_agent_info - Unexpected Error:', err);

          }
          resolve(result);
        }
      );
    } catch (error) {
      reject(error);
      logger.error('ccaas_get_session_queue_agent_info - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const insertBusinessHours = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      const hours = JSON.stringify(data.hours);
      mysqlPoolConnection.query(
        'CALL ccaas_myacc_insert_business_hours(?,?,?,?,?,?,?,?)',
        [
          data?.name,
          data?.description,
          hours,
          data?.domainId,
          data?.timeZone,
          data?.default_hours,
          data?.timeZoneValue,
          data?.type
        ],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('ccaas_myacc_insert_business_hours - Unexpected Error:', err);

          } else {
            resolve(result?.[0]);
          }
        });
    } catch (error) {
      reject(error);
      logger.error('ccaas_myacc_insert_business_hours - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const insertAgentStatus = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_insert_update_agent_status(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          null,
          data?.statusName,
          data?.type,
          data?.description,
          data?.channels,
          data?.status,
          data?.domainId,
          null,
          data?.isVoice,
          data?.isVoiceCallTransfer,
          data?.isChat,
          data?.isChatTransfer,
          data?.isEmail,
          data?.isEmailTransfer,
          data?.colorCode
        ],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('ccaas_insert_update_agent_status - Unexpected Error:', err);

          } else {
            resolve(result?.[0]?.[0]);
          }
        });
    } catch (error) {
      reject(error);
      logger.error('ccaas_insert_update_agent_status - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const insertDispositionStatus = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_insert_update_disposition_status(?,?,?,?,?,?,?)',
        [
          null,
          data?.dispositionName,
          data?.type,
          data?.description,
          data?.status,
          data?.domainId,
          data?.colorCode
        ],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('ccaas_insert_update_disposition_status - Unexpected Error:', err);

          } else {
            resolve(result?.[0]?.[0]);
          }
        });
    } catch (error) {
      reject(error);
      logger.error('ccaas_insert_update_disposition_status - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const insertCustomerTag = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_myacc_insert_customer_tag(?,?,?,?,?,?)',
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
            logger.error('ccaas_myacc_insert_customer_tag - Unexpected Error:', err);

          } else {
            resolve(result?.[0]?.[0]);
          }
        });
    } catch (error) {
      reject(error);
      logger.error('ccaas_myacc_insert_customer_tag - Unexpected Error:', error);
    }
  });
};

/**
 * get CallBack List
 * @param data
 * @returns
 */
export const getCcaasRoleInfo = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnectionRole.query(
        'CALL ur_ma_login_validation(?,?,?,?,?)',
        [
          data.login_user_name,
          data.login_password,
          data.login_source,
          data.login_device_id,
          data.login_ipaddress,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ur_ma_login_validation - Unexpected Error:', err);

          }
          resolve(result?.length ? result[0] : {});
        }
      );
    } catch (err) {
      reject(err);
      logger.error('ur_ma_login_validation - Unexpected Error:', err);

    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const defaultGlobalShortcutKeys = (data: any, response: any) => {
  return new Promise(async (resolve, reject) => {
    const uuid = uuidv4();
    const keyValues = JSON.stringify(data);
    const type = 'Default';
    try {
      mysqlPoolConnection.query(
        'CALL ccass_create_short_cut_key_details(?,?,?,?,?,?)',
        [uuid, keyValues, keyValues, response.domainId, response.ext, type],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccass_create_short_cut_key_details - Unexpected Error:', err);

          }
          resolve(result);
        }
      );
    } catch (err) {
      reject(err);
      logger.error('ccass_create_short_cut_key_details - Unexpected Error:', err);

    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const insertDefaultShortcutKeysDao = (data: any, response: any) => {
  return new Promise(async (resolve, reject) => {
    const keyValues = JSON.stringify(data);
    const defaultKeys = JSON.stringify(data);
    const type = 'Custom';
    const uuid = uuidv4();
    try {
      mysqlPoolConnection.query(
        'CALL ccass_create_short_cut_key_details(?,?,?,?,?,?)',
        [uuid, keyValues, defaultKeys, response.domainId, response.ext, type],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccass_create_short_cut_key_details - Unexpected Error:', err);

          }
          resolve(result);
        }
      );
    } catch (err) {
      reject(err);
      logger.error('ccass_create_short_cut_key_details - Unexpected Error:', err);

    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getUserDetailsEmail = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_getUserDetailsEmail_info(?,?)',
        [data?.domainId, data?.emailId],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccaas_getUserDetailsEmail_info - Unexpected Error:', err);

          }
          resolve(result);
        }
      );
    } catch (error) {
      reject(error);

      logger.error('ccaas_getUserDetailsEmail_info - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateOnboardUser = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_ccaas_user_update_v2(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          data?.UserID,
          data.UserName,
          data?.emailId,
          data?.directNumber,
          data?.address ?? null,
          data?.routing_profile,
          data?.report_to,
          data?.sipLoginId,
          data?.companyId,
          data?.domainId,
          data?.ext,
          data?.isActive,
          data?.callType,
          data?.roleid,
          data?.localization,
          data?.currentSession,
          data?.isVoice,
          data?.isVoiceCallTransfer,
          data?.isChat,
          data?.isChatTransfer,
          data?.isEmail,
          data?.isEmailTransfer,
          data?.status,
          data?.isSms,
          data?.isSocialMedia,
          data?.add_skill,
          data?.voice,
          data?.chat,
          data?.email,
          data?.sms,
          data?.socialMedia,
          data?.profileUrl ?? null,
          data?.companyName ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('up_ccaas_user_update_v2 - Unexpected Error:', err);

          }
          if (Array.isArray(result) && result[0].length > 0) {
            resolve(result[0]);
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('up_ccaas_user_update_v2 - Unexpected Error:', err);

    }
  });
};

export const getUserIdUser = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_getUserId (?,?)',
        [data?.domain_id, data?.ext],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('ccaas_getUserId - Unexpected Error:', err);

          } else {
            resolve(result[0]);
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('ccaas_getUserId - Unexpected Error:', err);

    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getLoginToken = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnectionRole.query(
        'CALL UR_Myacc_Web_token_login (?,?,?,?,?)',
        [
          data.mobileno,
          data.extension,
          data.device_type,
          data.browser,
          data.login_type,
        ],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('UR_Myacc_Web_token_login - Unexpected Error:', err);

          } else {
            resolve(result[0]);
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('UR_Myacc_Web_token_login - Unexpected Error:', err);

    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getsupervosorData = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      const mysqlq = `SELECT * FROM user WHERE domainId = ${data.domainId} AND roleid=${ROLES.SUPERVISOR_ROLE_ID}`;
      mysqlPoolConnection.query(mysqlq, (err, result) => {
        if (err) {
          reject(err);
          logger.error('getsupervosorData - Unexpected Error:', err);

        } else {
          resolve(result);
        }
      });
    } catch (error) {
      reject(error);

      logger.error('getsupervosorData - Unexpected Error:', error);
    }
  });
};

export const updateTicketingId = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_updateTicketingId(?,?,?,?,?)',
        [data?.ticketId, data?.historyId, data?.domainId, data?.channelType, data?.ext],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccaas_updateTicketingId - Unexpected Error:', err);

          } else {
            resolve(result[0]);
          }
        }
      );
    } catch (error) {
      reject(error);
      logger.error('ccaas_updateTicketingId - Unexpected Error:', error);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getInteraction = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_get_interaction_details (?,?,?,?,?,?,?,?)',
        [
          data?.domainId,
          data?.emailId ?? null,
          data?.phoneNumber ?? null,
          data?.customerId ?? null,
          data?.customerFacebookId ?? null,
          data?.customerInstagramId ?? null,
          data?.startTmpstmp ?? null,
          data?.endTmpstmp ?? null
        ],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('ccaas_get_interaction_details - Unexpected Error:', err);

          } else {
            if (result[0]?.length > 0) {
              resolve(result);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('ccaas_get_interaction_details - Unexpected Error:', err);

    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getInteractionView = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_get_interaction_details_view (?,?,?,?,?,?,?,?)',
        [
          data?.domainId,
          data?.emailId ?? null,
          data?.phoneNumber ?? null,
          data?.customerId ?? null,
          data?.customerFacebookId ?? null,
          data?.customerInstagramId ?? null,
          data?.interactionId ?? null,
          data?.TicketId ?? null
        ],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('ccaas_get_interaction_details_view - Unexpected Error:', err);

          } else {
            if (result[0]?.length > 0) {
              resolve(result);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('ccaas_get_interaction_details_view - Unexpected Error:', err);

    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const deleteOnboardUser = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_deleteUser (?,?)',
        [
          data?.domainId ?? 0,
          data?.ext ?? '',
        ],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('ccaas_deleteUser - Unexpected Error:', err);

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
      reject(err);
      logger.error('ccaas_deleteUser - Unexpected Error:', err);

    }
  });
};

export const deleteUserActiveStatus = (data: any, ext: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL update_userIsActive (?,?,?)',
        [
          data?.domainId ?? 0,
          ext ?? '',
          data?.status ?? 1
        ],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('update_userIsActive - Unexpected Error:', err);

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
      reject(err);
      logger.error('update_userIsActive - Unexpected Error:', err);

    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateOnboardActiveStatus = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_update_myAcc_User_Active_state (?,?,?)',
        [
          data?.isActive ?? null,
          data?.domainId ?? 0,
          data?.emailId
        ],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('ccaas_update_myAcc_User_Active_state - Unexpected Error:', err);

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
      reject(err);
      logger.error('ccaas_update_myAcc_User_Active_state - Unexpected Error:', err);

    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const UpdateChatFlagDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_update_user_chat_flag(?,?)',
        [
          data?.domainId,
          data?.ext,
        ],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('ccaas_update_user_chat_flag - Unexpected Error:', err);

          } else {
            resolve(result?.[0]?.[0]);
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('ccaas_update_user_chat_flag - Unexpected Error:', err);

    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getCallBackOption = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL up_GetCallBackOptions (?)',
        [
          data?.domainId,
        ],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('up_GetCallBackOptions - Unexpected Error:', err);

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
      reject(err);
      logger.error('up_GetCallBackOptions - Unexpected Error:', err);

    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateProfilePicDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call  update_profilePicture_CCAASMobile (?,?,?)',
        [
          data?.domainId,
          data?.profileImage ?? null,
          data?.ext ?? null,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('update_profilePicture_CCAASMobile - Unexpected Error:', err);

          } else {
            if (result?.length > 0) {
              resolve(result);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('update_profilePicture_CCAASMobile - Unexpected Error:', err);

    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const updateProfileNameDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call  update_profileName_CCAASMobile(?,?,?,?)',
        [
          data?.domainId,
          data?.firstName ?? null,
          data?.lastName ?? null,
          data?.ext ?? null,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('update_profileName_CCAASMobile - Unexpected Error:', err);

          } else {
            if (result?.length > 0) {
              resolve(result);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('update_profileName_CCAASMobile - Unexpected Error:', err);

    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getProfileDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call  get_profileImage_CCAASMobile (?,?)',
        [data?.domainId, data?.ext],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('get_profileImage_CCAASMobile - Unexpected Error:', err);

          } else {
            if (result?.length > 0) {
              resolve(result);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('get_profileImage_CCAASMobile - Unexpected Error:', err);

    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const otpGeneratorDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('CALL ccaas_create_flow_otp_dtl(?,?,?)', [
        data?.domainId ?? null,
        data?.toId ?? null,
        data?.expiryTime ?? null
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('ccaas_create_flow_otp_dtl - Unexpected Error:', err);

        } else {
          if (Array.isArray(result)) {
            resolve(result[0]);
          } else {
            resolve(result);
          }
        }
      });
    } catch (err) {
      reject(err);
      logger.error('ccaas_create_flow_otp_dtl - Unexpected Error:', err);

    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const otpValidatorDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('CALL ccaas_flow_otp_validation_dtl(?,?,?)', [
        data?.domainId ?? null,
        data?.toId ?? null,
        data?.verificationCode ?? null
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('ccaas_flow_otp_validation_dtl - Unexpected Error:', err);

        } else {
          if (Array.isArray(result)) {
            resolve(result[0]);
          } else {
            resolve(result);
          }
        }
      });
    } catch (err) {
      reject(err);
      logger.error('ccaas_flow_otp_validation_dtl - Unexpected Error:', err);

    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getAddonLanguagesDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_addon_language_dtl(?)',
        [data?.domainId],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccaas_get_addon_language_dtl - Unexpected Error:', err);

          } else {
            if (result?.length > 0) {
              resolve(result);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('ccaas_get_addon_language_dtl - Unexpected Error:', err);

    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getexpiredplandetailsDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_GetPlanRemainingChatSessionCount(?)',
        [data?.domainId],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('up_GetPlanRemainingChatSessionCount - Unexpected Error:', err);

          } else {
            if (result?.length > 0) {
              resolve(result[0]);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('up_GetPlanRemainingChatSessionCount - Unexpected Error:', err);

    }
  });
};

export const getUserOnboardSettingFromDb = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_get_user_onboard_settings_completion(?,?)',
        [data.email_id, data.domainId],
        (err: any, result: any) => {
          if (err) {
            console.log('err:', err);
            resolve(err);
          } else {
            const rows = Array.isArray(result) && Array.isArray(result[0]) ? result[0] : result || [];
            resolve(rows);
          }
        });
    } catch (err) {
      console.log(err);
      reject(err);
    }
  });
};

export const getLiveInteractionFromDb = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_get_live_interaction_ai(?)',
        [data.domainId],
        (err: any, result: any) => {
          if (err) {
            console.log('err:', err);
            resolve(err);
          } else {
            resolve(result);
          }
        });
    } catch (err) {
      console.log(err);
      reject(err);
    }
  });
};

export const iinsertUpdateDataAndPrivacySetDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('CALL ccaasAi_insert_update_data_and_privacy_set(?,?,?,?,?,?)', [
        data.domainId,
        data.companyId,
        data.userId,
        data.isWorktualDestopApp ?? 0,
        data.isContentAnalysis ?? 0,
        data.isConnectedApps ?? 0,
      ],                        (err, result) => {
        if (err) {
          console.log('err:', err);
          resolve(err);
        } else {
          const rows =
            Array.isArray(result) && Array.isArray(result[0])
              ? result[0]
              : result || [];

          resolve(rows);
        }
      });
    } catch (err) {
      console.log(err);
      reject(err);
    }
  });
};

export const getDataAndPrivacySetDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('CALL ccaasAi_get_data_and_privacy_set(?,?,?)', [
        data.domainId,
        data.companyId,
        data.userId,
      ],                        (err, result) => {
        if (err) {
          console.log('err:', err);
          resolve(err);
        } else {
          const rows =
            Array.isArray(result) && Array.isArray(result[0])
              ? result[0]
              : result || [];

          resolve(rows);
        }
      });
    } catch (err) {
      console.log(err);
      reject(err);
    }
  });
};

export const getUserDetailsByRoleDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('CALL ccaas_get_user_details_by_role(?,?)', [
        data.roleId,
        data.domainId,
      ],                        (err, result) => {
        if (err) {
          console.log('err:', err);
          resolve(err);
        } else {
          const rows =
            Array.isArray(result) && Array.isArray(result[0])
              ? result[0]
              : result || [];

          resolve(rows);
        }
      });
    } catch (err) {
      console.log(err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getInteractionLogDetailsDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_ai_get_customer_overall_interaction (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          data?.domainId,
          data?.offset,
          data?.limit,
          data?.isQualifiedUser,
          data?.AgentId,
          data?.fromDate,
          data?.toDate,
          data?.agentName,
          data?.dispositionName,
          data?.customerName,
          data?.isBot,
          data?.isInbound,
          data?.isMissed,
          data?.isAbonded,
          data?.channelName,

        ],
        (err, result) => {
          if (err) {
            console.log('getInteractionLogDetailsHandler err ::', err);

            reject(err);
            logger.error('ccaas_get_interaction_details_view - Unexpected Error:', err);

          } else {
            if (result[0]?.length > 0) {
              resolve(result);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('ccaas_get_interaction_details_view - Unexpected Error:', err);

    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getParticularInteractionLogDetailsDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_ai_get_customer_particular_interaction (?,?,?,?,?)',
        [
          data?.domainId,
          data?.coid,
          data?.offset,
          data?.limit,
          data?.userId
        ],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('ccaas_get_interaction_details_view - Unexpected Error:', err);

          } else {
            if (result[0]?.length > 0) {
              resolve(result);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('ccaas_get_interaction_details_view - Unexpected Error:', err);

    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getOverallCallbackInteractionDetailsDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_ai_get_customer_overall_callback_interaction (?,?,?)',
        [
          data?.domainId,
          data?.offset,
          data?.limit,

        ],
        (err, result) => {
          if (err) {
            reject(err);
            logger.error('ccaas_get_interaction_details_view - Unexpected Error:', err);

          } else {
            if (result[0]?.length > 0) {
              resolve(result);
            } else {
              resolve([]);
            }
          }
        }
      );
    } catch (err) {
      reject(err);
      logger.error('ccaas_get_interaction_details_view - Unexpected Error:', err);
    }
  });
};

export const insertUpdateMyplanRolePermissionDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('CALL ccaas_insert_update_my_plan_role_permission(?,?,?,?,?,?)', [
        data.domainId ?? null,
        data.roleId ?? null,
        data.is_agent_monitor ?? 0,
        data.is_queue_management ?? 0,
        data.is_reporting_dashboard ?? 0,
        data.is_knowledge_base ?? 0,
      ],                        (err, result) => {
        if (err) {
          console.log('err:', err);
          resolve(err);
        } else {
          const rows =
            Array.isArray(result) && Array.isArray(result[0])
              ? result[0]
              : result || [];

          resolve(rows);
        }
      });
    } catch (err) {
      console.log(err);
      reject(err);
    }
  });
};

export const getMyplanRolePermissionDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('CALL ccaas_get_my_plan_role_permission(?)', [
        data.domainId ?? null,
      ],                        (err, result) => {
        if (err) {
          console.log('err:', err);
          resolve(err);
        } else {
          const rows =
            Array.isArray(result) && Array.isArray(result[0])
              ? result[0]
              : result || [];

          resolve(rows);
        }
      });
    } catch (err) {
      console.log(err);
      reject(err);
    }
  });
};

export const getTeamDetailsDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('CALL ccaas_get_queue_detailed_view_for_myplan(?,?)', [
        data.qId,
        data.domainId,
      ],                        (err, result) => {
        if (err) {
          console.log('err:', err);
          resolve(err);
        } else {
          if (result[0]?.length > 0) {
            resolve(result);
          } else {
            resolve([]);
          }
        }
      });
    } catch (err) {
      console.log(err);
      reject(err);
    }
  });
};

export const insertUpdateMyplanTeamsPermissionDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('CALL ccaas_update_my_plan_queue_permission(?,?,?,?,?)', [
        data.domainId ?? null,
        data.qId ?? null,
        data.is_agent_monitor ?? 0,
        data.is_addOrRemove ?? 0,
        data.is_reporting_dashboard ?? 0,
      ],                        (err, result) => {
        if (err) {
          console.log('err:', err);
          resolve(err);
        } else {
          const rows =
              Array.isArray(result) && Array.isArray(result[0])
                ? result
                : result || [];

          resolve(rows);
        }
      });
    } catch (err) {
      console.log(err);
      reject(err);
    }
  });
};

export const getMyplanTeamPermissionDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('CALL ccaas_get_my_plan_queue_permission(?,?)', [
        data?.qId ?? null,
        data?.domainId ?? null,
      ],                        (err, result) => {
        if (err) {
          console.log('err:', err);
          resolve(err);
        } else {
          const rows =
              Array.isArray(result) && Array.isArray(result[0])
                ? result
                : result || [];

          resolve(rows);
        }
      });
    } catch (err) {
      console.log(err);
      reject(err);
    }
  });
};

export const getMyplanTeamLogActivityDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('CALL ccaas_get_queue_user_login_log(?,?)', [
        data?.domainId ?? null,
        data?.qId ?? null,
      ],                        (err, result) => {
        if (err) {
          console.log('err:', err);
          resolve(err);
        } else {
          const rows =
              Array.isArray(result) && Array.isArray(result[0])
                ? result
                : result || [];

          resolve(rows);
        }
      });
    } catch (err) {
      console.log(err);
      reject(err);
    }
  });
};

export const aiccasGetConfigDetailsDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('CALL ccaas_get_channel_config_details(?)', [
        data.domainId ?? null,
      ],                        (err, result) => {
        if (err) {
          console.log('err:', err);
          resolve(err);
        } else {
          const rows =
            Array.isArray(result) && Array.isArray(result[0])
              ? result[0]
              : result || [];

          resolve(rows);
        }
      });
    } catch (err) {
      console.log(err);
      reject(err);
    }
  });
};

export const iupdateMyplanUserPermissionDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('CALL ccaas_update_my_plan_user_permission(?,?,?,?,?,?,?)', [
        data.domainId ?? null,
        data.userId ?? null,
        data.is_voice ?? 0,
        data.is_chat ?? 0,
        data.is_recording ?? 0,
        data.is_email ?? 0,
        data.is_knowledgeBase ?? 0,
      ],                        (err, result) => {
        if (err) {
          console.log('err:', err);
          resolve(err);
        } else {
          const rows =
            Array.isArray(result) && Array.isArray(result[0])
              ? result[0]
              : result || [];

          resolve(rows);
        }
      });
    } catch (err) {
      console.log(err);
      reject(err);
    }
  });
};

export const getMyplanUserPermissionDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('CALL ccaas_ai_get_particular_user_detail(?,?)', [
        data.domainId ?? null,
        data.userId ?? null,
      ],                        (err, result) => {
        if (err) {
          console.log('err:', err);
          resolve(err);
        } else {
          const rows =
            Array.isArray(result) && Array.isArray(result[0])
              ? result[0]
              : result || [];

          resolve(rows);
        }
      });
    } catch (err) {
      console.log(err);
      reject(err);
    }
  });
};

export const getMyplanUserogActivityDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('CALL ccaas_get_user_login_log(?,?)', [
        data?.domainId ?? null,
        data?.userId ?? null,
      ],                        (err, result) => {
        if (err) {
          console.log('ccaas_get_user_login_log err:', err);
          resolve(err);
        } else {
          const rows = Array.isArray(result) && Array.isArray(result[0]) ? result : result || [];
          resolve(rows);
        }
      });
    } catch (err) {
      console.log('ccaas_get_user_login_log err:', err);
      reject(err);
    }
  });
};
