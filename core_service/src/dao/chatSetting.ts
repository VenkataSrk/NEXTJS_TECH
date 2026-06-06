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
export const createChatCustomer = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      const currentDate = moment(new Date()).format('YYYY-MM-DD HH:mm:ss');
      const chatInsert = `INSERT INTO chat_setting (domainId,domainName, description, codeSnippet,createdAt) VALUES (${data.domainId},'${data.domainName}','${data.description}','${data.codeSnippet}','${currentDate}')`;
      mysqlPoolConnection.query(chatInsert, (err, result) => {
        if (err) {
          reject(err);
        } else {
          resolve({
            botId: result.insertId,
            domainId: data.domainId,
            domainName: data.domainName,
            description: data.description,
            codeSnippet: data.codeSnippet,
            createdAt: currentDate,
            updateddAt: '',
          });
        }
      });
    } catch (error) {
      reject(error);
    }
  });
};
export const createWebChatWidgetField = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      const domain = JSON.stringify(data.domain);
      const personalization = JSON.stringify(data.personalization);
      const primaryForm = JSON.stringify(data.primaryForm);
      const formDetails = JSON.stringify(data.formDetails);
      const templateList = JSON.stringify(data?.templateData);
      mysqlPoolConnection.query('CALL up_InsertUpdate_chat_configuration_primary_form(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)', [
        data?.aid ?? 0,
        data?.domainId,
        data?.title,
        data?.description,
        data?.chatFlowId,
        data?.chatIconLogo,
        domain,
        personalization,
        primaryForm,
        data?.callFlowName,
        data?.preChartFormFlag,
        data?.webchatid,
        data?.codeSnippet,
        data?.status,
        formDetails,
        data?.greeting,
        templateList,
        data?.planId,
        data?.productId,
        JSON.stringify(data?.websiteId),
        data?.autoResponse,
        data?.isCallBack ?? 0,
        data?.threshold ?? null,
        data?.aliasTitle ?? null
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
        }
        resolve(result);
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
export const getChatCustomerList = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      const mysqlq = `SELECT * FROM chat_setting WHERE domainId = ${data.domainId}`;
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
export const getWebChatConfig = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query('CALL ccaas_get_web_chat_config(?)', [
        data?.domainId,
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);

        }
        resolve(result[0]);
      });
    } catch (error) {
      reject(error);
    }
  });
};

/**
 *
 * @param aid
 * @param data
 * @returns
 */
export const getChatwidgetById = (webchatid: any, data: any) => {
  return new Promise((resolve, reject) => {

    try {
      mysqlPoolConnection.query('CALL ccaas_get_chat_widget_by_id(?,?)', [
        data?.domainId,
        webchatid,
      ],                        (err: any, result: any) => {

        if (err) {
          reject(err);
        }
        resolve(result[0]);
      });
    } catch (error) {
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @param aid
 * @returns
 */
export const updateWebChatWidget = (data: any, webchatid: any) => {
  return new Promise((resolve, reject) => {
    try {
      const domain = JSON.stringify(data.domain);
      const personalization = JSON.stringify(data.personalization);
      const primaryForm = JSON.stringify(data.primaryForm);
      const formDetails = JSON.stringify(data.formDetails);
      const templateList = JSON.stringify(data?.templateData);
      mysqlPoolConnection.query('CALL up_InsertUpdate_chat_configuration_primary_form(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)', [
        data?.aid ?? 0,
        data?.domainId,
        data?.title,
        data?.description,
        data?.chatFlowId,
        data?.chatIconLogo,
        domain,
        personalization,
        primaryForm,
        data?.callFlowName,
        data?.preChartFormFlag,
        webchatid,
        data?.codeSnippet,
        data?.status,
        formDetails,
        data?.greeting,
        templateList,
        data?.planId,
        data?.productId,
        JSON.stringify(data?.websiteId),
        data?.autoResponse,
        data?.isCallBack ?? 0,
        data?.threshold ?? null,
        data?.aliasTitle ?? null
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
        }
        resolve(result);
      });
    } catch (error) {
      reject(error);
    }
  });
};

export const updateWebChatWidgetStatus = (data: any, webchatid: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query('CALL ccaas_update_web_chat_widget_status(?,?)', [
        data?.status,
        webchatid
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
        } else {
          resolve({ status: data.status });
        }
      });
    } catch (error) {
      reject(error);
    }
  });
};

/**
 *  create group coChat model
 */
export const createGroupCoChat = (data: any) => {
  return new Promise(async (resolve, reject) => {
    const subscribedUsers = JSON.stringify(data.subscribedUsers);
    const lastMessageInfo = JSON.stringify(data.lastMessageInfo);
    const messages = JSON.stringify(data.messages);
    const customerInfo = JSON.stringify(data.customerInfo);
    try {
      mysqlPoolConnection.query('CALL ccass_create_group_coChat_details(?,?,?,?,?,?,?,?,?,?,?,?)', [
        data.uid,
        data.channelId,
        data.channelName,
        subscribedUsers,
        data.channelIcon,
        data.domainId,
        lastMessageInfo,
        data.unseenCount,
        data.allSeenStatus,
        messages,
        data.isChannelMuted,
        customerInfo
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
        }
        resolve(result);
      });
    } catch (err) {
      reject(err);
    }
  });
};

/**
 * @description
 * @param data
 * @returns
 */
export const getGroupCoChat = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      const mysqlq = `SELECT * FROM coChat WHERE domainId = ${data} ORDER BY coId DESC`;
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
 * @description
 * @param data
 * @returns
 */
export const getGroupCoChatId = (data: any, uid: any) => {
  return new Promise((resolve, reject) => {
    try {
      const mysqlq = `SELECT * FROM coChat WHERE uid = '${uid}' AND domainId = ${data}`;
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
 * @param dataq
 * @returns
 */

export const sendMessageToCoChat = async (data: any, dataq: any) => {
  const query = `UPDATE coChat SET messages = CONCAT(messages, ?) WHERE uid = '${dataq.uid}' AND channelId = '${dataq.channelId}' AND  domainId = ${dataq.domainId}`;
  const values = [JSON.stringify(data), dataq.id];

  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(query, values, (err, res) => {
        if (err) {
          reject(err);
        } else {
          resolve(res);
        }
      });
    } catch (err) {
      reject(err);
    }
  });
};

export const getchatwidgetByIdwithOutDomainID = (webchatid: any, data: any) => {
  return new Promise((resolve, reject) => {
    try {
      const mysqlq = `SELECT * FROM chat_configuration_primary_form WHERE webchatid = '${webchatid}'`;
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

export const botDetailsDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query('CALL ccaas_bot_details_dao(?)', [
        data.domainId,
      ],                        (err: any, result: any) => {
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

export const getcoChatDetailsDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      const mysqlq = `SELECT coId, uid, channelId, channelName,lastMessageInfo, subscribedUsers, customerInfo, unseenCount, allSeenStatus FROM coChat WHERE domainId = ${data.domainId}`;
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

export const getcoChatDetailsByIDDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      const mysqlq = `SELECT * FROM coChat WHERE domainId = ${data.domainId} AND channelId = '${data.channelId}'`;
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

export const storeCochatDetailsByIDDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      const mysqlMessage = `SELECT messages FROM coChat Where domainId = ${data.domainId} AND channelId = '${data.channelId}'`;
      mysqlPoolConnection.query(mysqlMessage, (err, result) => {
        if (err) {
          reject(err);
        } else {
          let resultMessage: any = [];
          if (result?.length > 0 && result[0]?.messages?.length > 0) {
            resultMessage = JSON.parse(result[0]?.messages ?? 'null') ?? [];
            resultMessage.push(data?.message);
          } else {
            resultMessage.push(data?.message);
          }
          const parseResultMessage = JSON.stringify(resultMessage);
          const mysqlq = `UPDATE coChat SET messages='${parseResultMessage}' WHERE domainId = ${data.domainId} AND channelId = '${data.channelId}'`;
          mysqlPoolConnection.query(mysqlq, (errs, result1) => {
            if (errs) {
              reject(errs);
            } else {
              resolve(result1);
            }
          });
        }
      });
    } catch (error) {
      reject(error);
    }
  });
};

export const checkWebchatTitleDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query('CALL ccaas_check_web_chat_title_dao(?,?)', [
        data.title,
        data?.domainId
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
        } else {
          resolve(result[0]);
        }
      });
    } catch (error) {
      reject(error);
    }
  });
};

export const insertwebChatQuickActionsDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('CALL ccaas_chatFlow(?,?,?,?,?)', [
        data?.chatFlowId,
        data?.domainId ?? null,
        data?.labelName ?? null,
        data?.goToDialogue ?? null,
        data?.uuid
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
        }
        resolve(result);
      });
    } catch (err) {
      reject(err);
    }
  });
};

export const getwebChatQuickActionsDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      const mysqlq = `SELECT * FROM quick_actions WHERE chatFlowId='${data.chatFlowId}' AND domainId=${data.domainId}`;
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

export const updateWeChatQuickActionsDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('CALL ccaas_chatFlow(?,?,?,?)', [
        data?.chatFlowId,
        data?.domainId ?? null,
        data?.labelName ?? null,
        data?.goToDialogue ?? null
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
        }
        resolve(result);
      });
    } catch (err) {
      reject(err);
    }
  });
};

export const getChatLablesDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('CALL ccaas_get_chat_lables_dao(?,?,?,?)', [
        data?.chatFlowId,
        data?.domainId,
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
        }
        resolve(result);
      });
    } catch (err) {
      reject(err);
    }
  });
};

export const insertUpdateTriggerDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('CALL ccaas_insertUpdate_TriggerMessage(?,?,?,?,?)', [
        data?.tmid ?? 0,
        data?.Field ?? null,
        data?.message ?? null,
        data?.callbackMessage ?? null,
        data?.domainId
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
        }
        resolve(result);
      });
    } catch (err) {
      reject(err);
    }
  });
};

export const getTriggerDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('CALL ccaas_get_TriggerMessage(?)', [
        data
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
        }
        resolve(result[0]);
      });
    } catch (err) {
      reject(err);
    }
  });
};

export const getCustomerContactDao = () => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('CALL up_getCustomerContactColumns()', [
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
        }
        resolve(result);
      });
    } catch (err) {
      reject(err);
    }
  });
};

export const encoderDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      const input_text = `${data?.mobileNo}@${data?.amount}@${data?.amount}@${1}`;
      const encoderData = Buffer.from(input_text, 'utf-8').toString('base64');
      resolve(encoderData);
    } catch (err) {
      reject(err);
    }
  });
};

export const deleteWebChatWidgetConfigurationdao = (aid) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('CALL up_delete_chat_configuration_primary_form(?)', [
        aid
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
        }
        resolve(result);
      });
    } catch (err) {
      reject(err);
    }
  });
};
export const cloneWebChatConfigurationdao = (data) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('CALL up_Clone_Chat_Configuration_Primary_Form(?,?,?,?)', [
        data.aid,
        data.title,
        data.webchatid,
        data.codeSnippet
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
        }
        resolve(result);
      });
    } catch (err) {
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const insertUpdateConcurrentChat = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_InsertUpdateConcurrentAgentLimit(?,?,?,?,?,?)',
        [
          data?.domainId,
          data?.isAllAgent ?? null,
          data?.allAgentLimit ?? null,
          data?.isAgent ?? null,
          data?.isRoutingProfile ?? null,
          data?.agentOccupyDetails ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            reject(err);
          }
        }
      );
    } catch (err) {
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getConcurrentChat = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_GetConcurrentAgentLimit(?)',
        [
          data?.domainId ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            reject(err);
          }
        }
      );
    } catch (err) {
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getAgentTracker = (domainId: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_GetAgentUnclosedSession(?)',
        [
          domainId ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
          }
          if (result && result?.length > 0) {
            resolve(result[0]);
          } else {
            reject(err);
          }
        }
      );
    } catch (err) {
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const UpdateIsAgentConcurrentChat = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_UpdateIsAgentLimit(?,?,?,?)',
        [
          data?.domainId,
          data?.isAgent ?? 0,
          data?.isAllAgent ?? 0,
          data?.isRoutingProfile ?? 0
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            reject(err);
          }
        }
      );
    } catch (err) {
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const insertUpdateEmailFlowHistory = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_InsertUpdateEmailFlowHistory(?,?,?,?,?,?,?)',
        [
          data?.emailFlowHistory,
          data?.flowName ?? null,
          data?.html ?? null,
          data?.css ?? null,
          data?.domainId,
          data?.isActive ?? null,
          data?.url ?? null,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            reject(err);
          }
        }
      );
    } catch (err) {
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const assignCallBackChat = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_UpdateAssignAgentToChat(?,?,?,?)',
        [
          data.assignedBy,
          data.assignedTo,
          data.assignedOn,
          data.ChatHistoryId,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            reject(err);
          }
        }
      );
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
export const getCallbackListNotification = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_GetCallBackListForNotification(?,?,?,?)',
        [
          data?.domainId ?? null,
          data?.channelType ?? null,
          data?.offset ?? null,
          data?.limit ?? null,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
          }
          if (result && result.length > 0) {
            resolve(result[0]);
          } else {
            reject(err);
          }
        }
      );
    } catch (err) {
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const InsertChatFlowVariablesDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_InsertChatFlowVariables(?,?,?,?)',
        [
          data?.domainId ?? null,
          data?.chatFlowId ?? null,
          data?.chatFlowVariable ?? null,
          data?.chatFlowValue ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
          } else {
            resolve(result);
          }
        }
      );
    } catch (err) {
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const GetChatFlowVariablesDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_GetChatFlowVariables(?,?)',
        [
          data?.domainId ?? null,
          data?.chatFlowId ?? null,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
          } else {
            resolve(result);
          }
        }
      );
    } catch (err) {
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getAllChatSessionMessageDao = (data: any) => {
  const chatLimit = parseInt(data?.limit, 10);
  const chatOffset = parseInt(data?.offset, 10);
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('CALL ccaas_get_all_chat_session_message_dao(?,?)', [
        chatLimit,
        chatOffset
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
        }
        resolve(result);
      });
    } catch (err) {
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getCompanyDomainDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_GetCompanyDomain(?)',
        [
          data ?? null,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
          } else {
            resolve(result);
          }
        }
      );
    } catch (err) {
      reject(err);
    }
  });
};

export const insertUpdateIrrelevantQaDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call up_InsertUpdateIrrelevantChatQuestionAnswer(?,?,?,?)', [
        data?.chatFlowId,
        data?.domainId,
        JSON.stringify(data?.questionAnswer) ?? null,
        data?.websiteId
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
        }
        resolve(result);
      });
    } catch (err) {
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getCustomerLatestMsgDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_getCustomerLatestMsg(?,?)',
        [
          data?.customerId ?? null,
          data?.domainId
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
          } else {
            resolve(result);
          }
        }
      );
    } catch (err) {
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @param datau
 * @returns
 */
export const addChatFlowlanguage = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_chatFlow_add_language_info(?,?,?,?,?,?)',
        [
          data?.p_domainId,
          data?.p_chatFlowId ?? null,
          data?.p_addonId ?? null,
          data?.p_isEnabled ?? null,
          data?.p_translatedText ?? null,
          data?.p_addonName ?? null
        ],

        (err: any, result: any) => {
          if (err) {
            reject(err);
          }
          resolve(result?.[0]);
        }
      );
    } catch (err) {
      reject(err);
    }
  });
};
/**
 *
 * @param data
 * @param datau
 * @returns
 */
export const getChatFlowlanguage = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_chatFlow_get_language_info(?,?)',
        [
          data?.p_domainId,
          data?.p_chatFlowId ?? null,
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

/**
 *
 * @param data
 * @param datau
 * @returns
 */
export const setChatFlowlanguage = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_chatFlow_set_language_settings(?,?,?)',
        [
          data?.p_domainId,
          data?.p_chatFlowId ?? null,
          data?.p_isEnabled
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
