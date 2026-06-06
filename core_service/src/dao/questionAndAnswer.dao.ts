import axios from 'axios';
import moment from 'moment';
import { escape } from 'mysql';
import { v4 as uuidv4 } from 'uuid';
import { IQuestionAndAnswer, IWebCrawler } from '../modules/questionAndAnswer/questionAndAnswer.model';
import { mysqlPoolConnection } from '../plugins/db';
import { logger } from '../plugins/winston';

export const createQestionAnswer = async (data: IQuestionAndAnswer) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_insertUpdateQuestionAnswer(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?) ', [
          data?.questionId ?? null,
          data?.domainId ?? null,
          data?.uuid ?? null,
          data?.question ?? null,
          data?.answer ?? null,
          data?.mapFlow ?? null,
          data?.language ?? null,
          data?.intent ?? null,
          data?.keywords ?? null,
          data?.status ?? null,
          data?.version ?? null,
          data?.sent ?? null,
          data?.helpfull ?? null,
          data?.notHelpfull ?? null,
          parseInt(data?.chatFlowId, 10) ?? null,
          data?.embedding ?? null,
          data?.websiteId ?? null,
          data?.discription ?? null,
          data?.websiteUrl ?? null,
          data?.baseName ?? null
        ],
        (err1, results1) => {
          if (err1) {
            reject(err1);
          } else {
            const questionId = results1?.[0][0].questionId;
            mysqlPoolConnection.query(
              'call ccaas_get_question_answer_by_id(?)',
              [
                questionId
              ],
              (err2, results2) => {
                if (err2) {
                  logger.error('ccaas_get_question_answer_by_id - Unexpected Error:', err2);
                  reject(err2);
                } else {
                  resolve(results2?.[0]);
                }
              });
          }
        });
    } catch (err) {
      logger.error('ccaas_insertUpdateQuestionAnswer - Unexpected Error:', err);

      reject(err);
    }
  });
};

export const updateQestionAnswer = async (data: any, questionId: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      const query = `UPDATE question_answer SET ${Object.keys(data).map((key) => {
        return `${key} = ?`;
      }).join(', ')} WHERE questionId = ?`;
      const param = [...Object.values(data), questionId];
      mysqlPoolConnection.query(query, param, async (err1, results1) => {
        if (err1) {
          reject(err1);
        } else {
          mysqlPoolConnection.query(
            'call ccaas_get_question_answer_by_id(?)',
            [
              questionId
            ],
            (err2, results2) => {
              if (err2) {
                reject(err2);
                logger.error('ccaas_get_question_answer_by_id - Unexpected Error:', err2);

              } else {
                resolve(results2?.[0]);
              }
            });
        }
      });
    } catch (error) {
      reject(error);
      logger.error('updateQestionAnswer - Unexpected Error:', error);
    }
  });
};

export const deleteQuestionAnswer = async (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call  ccaas_delete_question_answer(?)',
        [
          data?.questionId
        ],
        (err, results) => {
          if (err) {
            reject(err);
            logger.error('ccaas_delete_question_answer - Unexpected Error:', err);
          } else {
            resolve(results?.[0]?.[0]);
          }
        });
    } catch (error) {
      reject(error);
      logger.error('ccaas_delete_question_answer - Unexpected Error:', error);
    }
  });
};

export const deleteOldQaDao = async (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_delete_old_question_answer_by_flow_id(?)',
        [
          data
        ],
        (err, results) => {
          if (err) {
            reject(err);
            logger.error('ccaas_delete_old_question_answer_by_flow_id - Unexpected Error:', err);
          } else {
            resolve(results?.[0]?.[0]);
          }
        });
    } catch (error) {
      reject(error);
      logger.error('ccaas_delete_old_question_answer_by_flow_id - Unexpected Error:', error);
    }
  });
};

export const insertCrawlWebsiteData = async (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call up_InsertUpdate_websiteScraper(?,?,?,?,?,?,?,?,?,?,?)', [
        data?.web_id ?? null,
        data?.websiteId ?? null,
        data?.domain ?? null,
        data?.baseName ?? null,
        data?.discription ?? null,
        data?.domainId ?? null,
        data?.companyType ?? null,
        data?.language ?? null,
        data?.customerWebSiteUrl ?? null,
        data?.agentWebSiteUrl ?? null,
        data?.knowledgeBaseType ?? null
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('up_InsertUpdate_websiteScraper - Unexpected Error:', err);
        }
        mysqlPoolConnection.query(
          'call ccaas_get_crawl_website_data_by_web_id(?)',
          [
            data?.websiteId
          ],
          async (err2, results2) => {
            if (err2) {
              reject(err2);
              logger.error('ccaas_get_crawl_website_data_by_web_id - Unexpected Error:', err2);

            } else {
              resolve(results2?.[0]);
            }
          });
      });

    } catch (err) {
      reject(err);
      logger.error('up_InsertUpdate_websiteScraper - Unexpected Error:', err);
    }
  });
};

export const getQuestionAnswerById = async (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_question_answer_by_id(?)',
        [
          data?.questionId
        ],
        (err, results) => {
          if (err) {
            reject(err);
            logger.error('ccaas_get_question_answer_by_id - Unexpected Error:', err);
          } else {
            resolve(results?.[0]);
          }
        });
    } catch (error) {
      reject(error);
      logger.error('ccaas_get_question_answer_by_id - Unexpected Error:', error);
    }
  });
};

export const getAllQestionAnswer = async (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_all_question_answer(?,?,?)',
        [
          data?.domainId,
          data?.limit,
          data?.offset
        ],
        (err, results) => {
          if (err) {
            reject(err);
            logger.error('ccaas_get_all_question_answer - Unexpected Error:', err);
          } else {
            resolve(results);
          }
        });
    } catch (error) {
      reject(error);
      logger.error('ccaas_get_all_question_answer - Unexpected Error:', error);
    }
  });
};

export const getAllQestionAnswerNoAuth = async (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_all_question_answer_no_auth(?,?)',
        [
          data?.domainId,
          data?.chatFlowId
        ],
        (err, results) => {
          if (err) {
            reject(err);
            logger.error('ccaas_get_all_question_answer_no_auth - Unexpected Error:', err);
          } else {
            resolve(results);
          }
        });
    } catch (error) {
      reject(error);
      logger.error('ccaas_get_all_question_answer_no_auth - Unexpected Error:', error);
    }
  });
};

export const searchQestionAnswer = async (data: IQuestionAndAnswer) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_search_question_answer(?,?,?,?,?)',
        [
          data?.domainId,
          data?.searchByValue,
          data.chatFlowId,
          data?.offset,
          data?.limit
        ],
        (err, results) => {
          if (err) {
            reject(err);
            logger.error('ccaas_get_search_question_answer - Unexpected Error:', err);
          } else {
            resolve(results);
          }
        });
    } catch (error) {
      reject(error);
      logger.error('ccaas_get_search_question_answer - Unexpected Error:', error);
    }
  });
};

export const bulkInsertQestionAnswer = async (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      const createdQestion = [];
      const websiteUrld: any = uuidv4();
      for (const obj of data?.dataArr) {
        // for (let index = 0; index < data.length; index++) {
        const param = {
          domainId: data.domainId,
          uuid: uuidv4(),
          chatFlowId: data.chatFlowId ?? 0,
          question: obj.question,
          answer: obj.answer,
          mapFlow: obj.mapFlow ?? null,
          language: obj.language ?? null,
          intent: obj.intent ?? null,
          keywords: obj.keywords ?? null,
          embedding: JSON.stringify(obj.embedding) ?? null,
          websiteId: websiteUrld ?? null,
          baseName: data?.baseName ?? null,
          discription: data?.discription ?? null,
          websiteUrl: data?.domain ?? null,
          createdAt: moment(new Date()).format('YYYY-MM-DD HH:mm:ss'),
        };
        await createQestionAnswer(param)
          .then((result2: any) => {
            createdQestion.push(result2[0]);
          })
          .catch((err) => {
            reject(err);
            logger.error('bulkInsertQestionAnswer - Unexpected Error:', err);
          });
      }
      resolve(createdQestion);
    } catch (error) {
      reject(error);
      logger.error('bulkInsertQestionAnswer - Unexpected Error:', error);
    }
  });
};

export const createWebCrawlerQestionAnswer = async (reqParam: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      await getWebcrawlQuestion(reqParam)
        .then(async (result1: any) => {
          if (result1?.data?.message === 'success') {
            await insertCrawlWebsiteData(reqParam)
              .then((result2: any) => {
                resolve({ result2, message: result1?.data?.message });
              })
              .catch((err) => {
                reject(err);
                logger.error('createWebCrawlerQestionAnswer - Unexpected Error:', err);
              });
          } else {
            resolve({ result1, message: result1?.data?.message });
          }
        })
        .catch((err) => {
          reject(err);
          logger.error('createWebCrawlerQestionAnswer - Unexpected Error:', err);
        });
    } catch (err) {
      reject(err);
      logger.error('createWebCrawlerQestionAnswer - Unexpected Error:', err);
    }
  });
};

export const getWebcrawlQuestion = async (data) => {
  return new Promise(async (resolve, reject) => {
    try {
      await axios({
        method: 'post',
        url: process.env.NLP_PATH,
        headers: {
          'Content-Type': 'application/json'
        },
        data: {
          url_text: data.domain,
          domain_id: data.domainId,
          ext: data.ext,
          industry_type: data?.industry_type,
          language: data?.language ?? 'English',
          by: 'customer'
        }
      })
        .then((response) => {
          resolve(response);
        })
        .catch((error) => {
          reject(error);
          logger.error('getWebcrawlQuestion - Unexpected Error:', error);
        });
    } catch (err) {
      reject(err);
      logger.error('getWebcrawlQuestion - Unexpected Error:', err);
    }
  });
};

export const updateQuestionFeedback = async (data: IQuestionAndAnswer) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_update_question_feedback_by_domain(?,?,?,?)',
        [
          data?.domainId,
          data?.sent,
          data?.helpfull,
          data?.notHelpfull
        ],
        (err, results) => {
          if (err) {
            reject(err);
            logger.error('ccaas_update_question_feedback_by_domain - Unexpected Error:', err);
          } else {
            resolve(results?.[0]?.[0]);
          }
        });
    } catch (err) {
      reject(err);
      logger.error('ccaas_update_question_feedback_by_domain - Unexpected Error:', err);
    }
  });
};

export const getQaDataDao = async (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_question_answer_by_chat_flow_id(?,?)',
        [
          data?.domainId,
          data?.chatFlowId,
        ],
        (err, results) => {
          if (err) {
            reject(err);
            logger.error('ccaas_get_question_answer_by_chat_flow_id - Unexpected Error:', err);
          } else {
            resolve(results?.[0]);
          }
        });
    } catch (err) {
      reject(err);
      logger.error('ccaas_get_question_answer_by_chat_flow_id - Unexpected Error:', err);
    }
  });
};

export const getKBQaDataDao = async (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_KB_question_answer_data(?,?)',
        [
          data?.domainId,
          data?.websiteId,
        ],
        (err, results) => {
          if (err) {
            reject(err);
            logger.error('ccaas_get_KB_question_answer_data - Unexpected Error:', err);
          } else {
            resolve(results?.[0]);
          }
        });
    } catch (err) {
      reject(err);
      logger.error('ccaas_get_KB_question_answer_data - Unexpected Error:', err);
    }
  });
};

export const getQuestionAnswerByWebsiteId = async (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_question_answer_by_website_id(?,?)',
        [
          data?.domainId,
          data?.websiteId
        ],
        (err, results) => {
          if (err) {
            reject(err);
            logger.error('ccaas_get_question_answer_by_website_id - Unexpected Error:', err);
          } else {
            resolve(results?.[0]);
          }
        });
    } catch (error) {
      reject(error);
      logger.error('ccaas_get_question_answer_by_website_id - Unexpected Error:', error);
    }
  });
};

/**
 *  Insert and Update lead generation
 */
export const getUpdateIrrelevantQaDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_GetIrrelevantChatQuestionAnswer(?,?,?)', [
          parseInt(data?.p_chatFlowId, 10),
          data?.p_domainId,
          null

        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('up_GetIrrelevantChatQuestionAnswer - Unexpected Error:', err);
          }
          resolve(result);
        });
    } catch (err) {
      reject(err);
      logger.error('up_GetIrrelevantChatQuestionAnswer - Unexpected Error:', err);
    }
  });
};

export const getUpdateIrrelevantQaKBDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_GetIrrelevantChatQuestionAnswer(?,?,?)', [
          null,
          data?.p_domainId,
          data?.websiteId

        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('up_GetIrrelevantChatQuestionAnswer - Unexpected Error:', err);
          }
          resolve(result);
        });
    } catch (err) {
      reject(err);
      logger.error('up_GetIrrelevantChatQuestionAnswer - Unexpected Error:', err);
    }
  });
};

export const getQuestionByCfidNoAuth = async (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_question_by_Cf_id_no_auth(?)', [
          data
        ],
        (err, results) => {
          if (err) {
            reject(err);
            logger.error('ccaas_get_question_by_Cf_id_no_auth - Unexpected Error:', err);
          } else {
            resolve(results?.[0]);
          }
        });
    } catch (error) {
      reject(error);
      logger.error('ccaas_get_question_by_Cf_id_no_auth - Unexpected Error:', error);
    }
  });
};

export const getSuggestionQuestionFromNlpDao = async (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      await axios({
        method: 'post',
        url: process.env.NLP_QA_SUGGESTION_API,
        headers: {
          'Content-Type': 'application/json'
        },
        data: {
          intent: data,
        }
      })
        .then((response) => {
          resolve(response);
        })
        .catch((error) => {
          reject(error);
          logger.error('getSuggestionQuestionFromNlpDao - Unexpected Error:', error);
        });
    } catch (err) {
      reject(err);
      logger.error('getSuggestionQuestionFromNlpDao - Unexpected Error:', err);
    }
  });
};
