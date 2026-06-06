import axios from 'axios';
import * as fastcsv from 'fast-csv';
import fs from 'fs';
import { mysqlPoolConnection } from '../plugins/db';
import { logger } from '../plugins/winston';

/**
 *  createCategoryDao
 */
export const createCategoryDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call up_InsertUpdateChatCategory(?,?,?,?,?)', [
        data?.ChatCategoryId ?? 0,
        data?.domainId ?? 0,
        data?.web_id ?? 0,
        data?.CategoryName ?? null,
        data?.CategoryDescription ?? null,

      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('up_InsertUpdateChatCategory - Unexpected Error:', err);
        }
        resolve(result);
      });
    } catch (err) {
      reject(err);
      logger.error('up_InsertUpdateChatCategory - Unexpected Error:', err);
    }
  });
};

/**
 *  getCategoryDao
 */
export const getCategoryDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call up_GetChatCategory(?)', [
        data?.domainId ?? 0,
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('up_GetChatCategory - Unexpected Error:', err);
        }
        resolve(result);
      });
    } catch (err) {
      reject(err);
      logger.error('up_GetChatCategory - Unexpected Error:', err);
    }
  });
};

/**
 *  insertUpdateResult
 */
export const insertUpdateArticleDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      const languageType = Array.isArray(data?.ArticleLanguage) ? data?.ArticleLanguage.join(',') : data?.ArticleLanguage;
      mysqlPoolConnection.query('call up_InsertUpdateChatArticle(?,?,?,?,?,?,?,?)', [
        data?.ChatArticleId ?? 0,
        data?.domainId ?? 0,
        data?.web_id ?? 0,
        data?.ChatCategoryId ?? null,
        JSON.stringify(data?.FilePath) ?? null,
        languageType,
        data?.LoggedUserId ?? 0,
        data?.knowledgeBaseType ?? 0,
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('up_InsertUpdateChatArticle - Unexpected Error:', err);
        }
        resolve(result);
      });
    } catch (err) {
      reject(err);
      logger.error('up_InsertUpdateChatArticle - Unexpected Error:', err);
    }
  });
};

/**
 *  getwebsiteUrl
 */
export const getWebsiteUrlDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call ccaas_get_website_url_dao(?)', [
        data?.web_id
      ],
                                (err, result) => {
                                  if (err) {
                                    reject(err);
                                    logger.error('ccaas_get_website_url_dao - Unexpected Error:', err);
                                  } else {
                                    resolve(result?.[0]);
                                  }
                                });
    } catch (err) {
      reject(err);
      logger.error('ccaas_get_website_url_dao - Unexpected Error:', err);
    }
  });
};

/**
 *  updateArticleFileToNLP
 */
export const updateArticleFileToNLP = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      const type: any = data?.CompanyType !== undefined ? data?.CompanyType : '';
      await axios({
        method: 'post',
        url: process.env.NLP_PATH_ARTICLE,
        headers: {
          'Content-Type': 'application/json'
        },
        data: JSON.stringify({
          pdf_links: data?.FilePath,
          domainId: data?.domainId,
          // websiteId: data?.websiteId,
          ext: data?.ext.toString(),
          companyType: type ?? '',
          companyName: data?.companyName ?? '',
          language: data?.ArticleLanguage ?? [],
          by: data?.by
        })
      })
        .then((response) => {
          resolve(response);
        })
        .catch((error) => {
          reject(error);
        });
    } catch (err) {
      reject(err);
      logger.error('updateArticleFileToNLP - Unexpected Error:', err);
    }
  });
};

/**
 *  updateArticleFileToNLP
 */

export const deleteArticleFileToNLP = (data: any, insertUpdateArticleResult?: any) => {
  const articleLanguage = insertUpdateArticleResult && insertUpdateArticleResult[0]?.[0]?.ArticleLanguage || '';
  return new Promise(async (resolve, reject) => {
    try {
      await axios({
        method: 'post',
        url: process.env.NLP_PATH_DELETE,
        headers: {
          'Content-Type': 'application/json'
        },
        data: {
          pdf_links: data?.FilePath,
          domainId: data?.domainId,
          // websiteId: data?.websiteId,
          language: data?.language ? data?.language : articleLanguage,
          by: data?.by ?? 'customer'
        }
      })
        .then((response) => {
          resolve(response);
        })
        .catch((error) => {
          reject(error);
        });
    } catch (err) {
      reject(err);
      logger.error('deleteArticleFileToNLP - Unexpected Error:', err);

    }
  });
};

/**
 *  getArticleDao
 */
export const getArticleDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call ccaas_get_article_dao(?)', [
        data.domainId
      ],                        (err, result) => {
        if (err) {
          reject(err);
          logger.error('ccaas_get_article_dao - Unexpected Error:', err);

        } else {
          resolve(result?.[0]);
        }
      });
    } catch (err) {
      reject(err);
      logger.error('ccaas_get_article_dao - Unexpected Error:', err);

    }
  });
};

/**
 *  updateWebsiteScrapDao
 */
export const updateWebsiteScrapDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call up_InsertUpdate_websiteScraper(?,?,?,?,?,?,?,?,?,?,?)', [
        data?.web_id,
        data?.websiteId,
        data?.domain,
        data?.baseName,
        data?.discription,
        data?.domainId,
        data?.companyType,
        data?.language,
        data?.customerWebSiteUrl ?? null,
        data?.agentWebSiteUrl ?? null,
        data?.knowledgeBaseType ?? null
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('up_InsertUpdate_websiteScraper - Unexpected Error:', err);

        }
        resolve(result);
      });
    } catch (err) {
      reject(err);
      logger.error('up_InsertUpdate_websiteScraper - Unexpected Error:', err);

    }
  });
};

/**
 *  get Article and category by webIdDao
 */
export const getArticleCategoryByWebIdDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call up_CategoryArticleCount(?,?)', [
        data.domainId,
        data?.knowledgeBaseType ?? null
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('up_CategoryArticleCount - Unexpected Error:', err);

        }
        resolve(result);
      });
    } catch (err) {
      reject(err);
      logger.error('up_CategoryArticleCount - Unexpected Error:', err);

    }
  });
};

/**
 *  get Article and category by webIdDao
 */
export const getArticleCategoryListByWebIdDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call up_CategoryArticleDetails(?,?)', [
        data?.webId,
        data?.domainId,
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('up_CategoryArticleDetails - Unexpected Error:', err);

        }
        resolve(result);
      });
    } catch (err) {
      reject(err);
      logger.error('up_CategoryArticleDetails - Unexpected Error:', err);

    }
  });
};

/**
 *  deleteCateoryByIdDao
 */
export const deleteCateoryByIdDao = (data: any) => {
  const cat_id = parseInt(data?.categoryId, 10);
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call up_DeleteChatCategory(?,?)', [
        cat_id,
        data?.domainId
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('up_DeleteChatCategory - Unexpected Error:', err);

        }
        resolve(result);
      });
    } catch (err) {
      reject(err);
      logger.error('up_DeleteChatCategory - Unexpected Error:', err);

    }
  });
};

/**
 *
 */
export const updateMultiplteArticleDao = (data: any, articleId: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call up_InsertUpdateChatArticle(?,?,?,?,?,?,?,?)', [
        articleId ?? 0,
        data?.domainId ?? 0,
        data?.web_id ?? 0,
        data?.ChatCategoryId ?? null,
        data?.FilePath ?? null,
        data?.ArticleLanguage ?? null,
        data?.LoggedUserId ?? 0,
        data?.knowledgeBaseType ?? 0,
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('up_InsertUpdateChatArticle - Unexpected Error:', err);

        }
        resolve(result);
      });
    } catch (err) {
      reject(err);
      logger.error('up_InsertUpdateChatArticle - Unexpected Error:', err);

    }
  });
};

export const deleteKnowledgeBaseDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call ccaas_DeleteScraper(?)', [
        data ?? 0,
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('ccaas_DeleteScraper - Unexpected Error:', err);

        }
        resolve(result);
      });
    } catch (err) {
      reject(err);
      logger.error('ccaas_DeleteScraper - Unexpected Error:', err);

    }
  });
};

export const getArticleByCatIdDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call ccaas_get_ChatArticleDetails(?,?)', [
        data?.chatCatId ?? 0,
        data?.webId ?? 0
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('ccaas_get_ChatArticleDetails - Unexpected Error:', err);

        }
        resolve(result);
      });
    } catch (err) {
      reject(err);
      logger.error('ccaas_get_ChatArticleDetails - Unexpected Error:', err);

    }
  });
};

/**
 *  deleteArticleByIdDao
 */
export const deleteArticleByIdDao = (data: any) => {
  const at_id = parseInt(data?.articleId, 10);
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call ccaas_deleteArticle(?)', [
        at_id,
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('ccaas_deleteArticle - Unexpected Error:', err);

        }
        resolve(result);
      });
    } catch (err) {
      reject(err);
      logger.error('ccaas_deleteArticle - Unexpected Error:', err);

    }
  });
};

/**
 *  deleteArticleByIdDao
 */
export const deletePaymentIntegrationDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call delete_ChatBotPaymentIntegration(?,?)', [
        data?.deleteId,
        data?.domainId
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('delete_ChatBotPaymentIntegration - Unexpected Error:', err);

        }
        resolve(result);
      });
    } catch (err) {
      reject(err);
      logger.error('delete_ChatBotPaymentIntegration - Unexpected Error:', err);

    }
  });
};

/**
 *  Insert and Update lead generation
 */
export const insertUpdateLeadGenerationDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call ccaas_InsertUpdateLeadGeneration(?,?,?,?,?,?,?,?,?,?,?,?,?,?)', [
        data?.LeadId ?? 0,
        data?.Method ?? null,
        data?.connector ?? null,
        data?.Email ?? null,
        data?.ScheduleType ?? null,
        data?.domainId ?? null,
        data?.dayStatus ?? null,
        data?.weekStatusdata ?? null,
        data?.Time ?? null,
        data?.lastUpdatedTime ?? null,
        data?.timeZone ?? null,
        data?.HourlyBasedTime ?? null,
        data?.MintuesBasedTime ?? null,
        data?.cronId
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('ccaas_InsertUpdateLeadGeneration - Unexpected Error:', err);

        }
        resolve(result);
      });
    } catch (err) {
      reject(err);
      logger.error('ccaas_InsertUpdateLeadGeneration - Unexpected Error:', err);

    }
  });
};

/**
 *  Insert and Update lead generation
 */
export const getLeadGenerationDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call up_getLeadGeneration(?)', [
        data?.LeadId,
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('up_getLeadGeneration - Unexpected Error:', err);

        }
        resolve(result);
      });
    } catch (err) {
      reject(err);
      logger.error('up_getLeadGeneration - Unexpected Error:', err);

    }
  });
};

/**
 *  Insert and Update lead generation by domainId
 */
export const getLeadGenerationByDomainIdDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call ccaas_get_lead_generation_by_domainId_dao(?)', [
        data?.domainId
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('ccaas_get_lead_generation_by_domainId_dao - Unexpected Error:', err);

        }
        resolve(result?.[0]);
      });
    } catch (err) {
      reject(err);
      logger.error('ccaas_get_lead_generation_by_domainId_dao - Unexpected Error:', err);

    }
  });
};

/**
 *  Insert and Update lead generation user details
 */
export const insertUpdateLeadUserDetailsDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call ccaas_InsertUpdateleadUserDetails(?,?,?,?,?,?,?)', [
        data?.leadUserId ?? 0,
        data?.name ?? null,
        data?.email ?? null,
        data?.phoneNumber ?? null,
        data?.address ?? null,
        data?.ipAddress ?? null,
        data?.domainId
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('ccaas_InsertUpdateleadUserDetails - Unexpected Error:', err);

        }
        resolve(result);
      });
    } catch (err) {
      reject(err);
      logger.error('ccaas_InsertUpdateleadUserDetails - Unexpected Error:', err);

    }
  });
};

export const insertPaymentIntergationDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call insertUpdate_ChatBotPaymentIntegration(?,?,?,?,?)', [
        data?.tokenId ?? 0,
        data?.companyId ?? null,
        data?.domainId ?? null,
        data?.paymentType ?? null,
        data?.secretJson ?? null

      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('insertUpdate_ChatBotPaymentIntegration - Unexpected Error:', err);

        }
        resolve(result);
      });
    } catch (err) {
      reject(err);
      logger.error('insertUpdate_ChatBotPaymentIntegration - Unexpected Error:', err);

    }
  });
};

export const getPaymentIntergationDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call get_ChatBotPaymentIntegration(?,?)', [
        data?.companyId,
        data?.domainId
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('get_ChatBotPaymentIntegration - Unexpected Error:', err);

        }
        resolve(result);
      });
    } catch (err) {
      reject(err);
      logger.error('get_ChatBotPaymentIntegration - Unexpected Error:', err);

    }
  });
};

/**
 *  Insert and Update lead generation by domainId
 */
export const getLeadUserDetailsByDomainIdDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call ccaas_get_lead_user_details_by_domainId_dao(?)', [
        data?.domainId
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('ccaas_get_lead_user_details_by_domainId_dao - Unexpected Error:', err);

        }
        resolve(result?.[0]);
      });
    } catch (err) {
      reject(err);
      logger.error('ccaas_get_lead_user_details_by_domainId_dao - Unexpected Error:', err);

    }
  });
};

/**
 *  Insert and Update lead user details Id
 */
export const getLeadUserDetailsId = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call up_getleadUserDetails(?)', [
        data?.LeadId,
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('up_getleadUserDetails - Unexpected Error:', err);

        }
        resolve(result);
      });
    } catch (err) {
      reject(err);
      logger.error('up_getleadUserDetails - Unexpected Error:', err);

    }
  });
};

/**
 *  Insert and Update lead generation
 */
export const getLeadUserReportsDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call ccaas_leadGenerationReports(?,?)', [
        data?.domainId,
        data?.lastUpdated
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('ccaas_leadGenerationReports - Unexpected Error:', err);

        }
        resolve(result);
      });
    } catch (err) {
      reject(err);
      logger.error('ccaas_leadGenerationReports - Unexpected Error:', err);

    }
  });
};

/**
 *
 * @param data
 * @returns
 */

export const getReportsInCSVDao = (data: any[]): Promise<string> => {
  return new Promise((resolve, reject) => {
    try {
      const csvRows = [];

      if (data.length > 0) {
        const headers = Object.keys(data[0]);
        csvRows.push(headers);

        data.forEach((obj) => {
          const rowData = headers.map(header => obj[header]);
          csvRows.push(rowData);
        });
      }
      const writableStream = fs.createWriteStream('data.csv', { encoding: 'utf8' });
      fastcsv
        .writeToStream(writableStream, csvRows, { headers: false })
        .on('finish', () => {
          fs.readFile('data.csv', 'utf8', (err, fileContent) => {
            if (err) {
              reject(err);
              logger.error('getReportsInCSVDao - Unexpected Error:', err);

            } else {
              resolve(fileContent);
            }
          });
        })
        .on('error', (err) => {
          reject(err);
          logger.error('getReportsInCSVDao - Unexpected Error:', err);

        });
    } catch (err) {
      reject(err);
      logger.error('getReportsInCSVDao - Unexpected Error:', err);

    }
  });
};

/**
 *
 * @param data
 * @returns
 */

export const getUnansweredReportsInCSVDao = (data: any[]): Promise<string> => {
  return new Promise((resolve, reject) => {
    try {
      const csvRows = [];

      if (data.length > 0) {
        const headers = ['Questions', 'Answers', 'Created at'];
        csvRows.push(headers);

        data.forEach((obj) => {
          const rowData = [
            obj.Question,
            obj.Answer,
            obj.created_At
          ];
          csvRows.push(rowData);
        });
      }

      const writableStream = fs.createWriteStream('data.csv', { encoding: 'utf8' });
      fastcsv
        .writeToStream(writableStream, csvRows, { headers: false })
        .on('finish', () => {
          fs.readFile('data.csv', 'utf8', (err, fileContent) => {
            if (err) {
              reject(err);
              logger.error('getUnansweredReportsInCSVDao - Unexpected Error:', err);
            } else {
              resolve(fileContent);
            }
          });
        })
        .on('error', (err) => {
          reject(err);
          logger.error('getUnansweredReportsInCSVDao - Unexpected Error:', err);
        });
    } catch (err) {
      reject(err);
      logger.error('getUnansweredReportsInCSVDao - Unexpected Error:', err);
    }
  });
};
/**
 *  Insert and Update lead generation
 */

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
          logger.error('up_InsertUpdateIrrelevantChatQuestionAnswer - Unexpected Error:', err);
        }
        resolve(result);
      });
    } catch (err) {
      reject(err);
      logger.error('up_InsertUpdateIrrelevantChatQuestionAnswer - Unexpected Error:', err);
    }
  });
};

export const deleteIrrelevantQaDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call up_SetIrrelevantChatQuestionAnswer(?,?,?)', [
        data?.chatFlowId,
        data?.domainId,
        JSON.stringify(data?.questionAnswer) ?? null

      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('up_SetIrrelevantChatQuestionAnswer - Unexpected Error:', err);
        }
        resolve(result);
      });
    } catch (err) {
      reject(err);
      logger.error('up_SetIrrelevantChatQuestionAnswer - Unexpected Error:', err);
    }
  });
};

/**
 *  Insert and Update lead generation by domainId
 */
export const getLeadEmailGenerationByDomainIdDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call ccaas_get_lead_email_generation_by_domainId_dao(?,?)', [
        data?.domainId,
        data?.chatId
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('ccaas_get_lead_email_generation_by_domainId_dao - Unexpected Error:', err);
        }
        resolve(result?.[0]);
      });
    } catch (err) {
      reject(err);
      logger.error('ccaas_get_lead_email_generation_by_domainId_dao - Unexpected Error:', err);
    }
  });
};

/**
 *  Insert and Update lead generation
 */
export const getUnanwerQuestionReportsDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call ccaas_get_unanswer_question_reports_dao(?,?)', [
        data?.domainId,
        data?.chatId
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('ccaas_get_unanswer_question_reports_dao - Unexpected Error:', err);
        }
        resolve(result?.[0]);
      });
    } catch (err) {
      reject(err);
      logger.error('ccaas_get_unanswer_question_reports_dao - Unexpected Error:', err);
    }
  });
};

/**
 *  Insert and Update lead generation
 */
export const insertUpdateEmailLeadGenerationDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call up_InsertUpdateIrrelevantChatLeadGeneration(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)', [
        data?.LeadId ?? 0,
        data?.Method ?? null,
        data?.connector ?? null,
        data?.Email ?? null,
        data?.ScheduleType ?? null,
        data?.domainId ?? null,
        data?.dayStatus ?? null,
        data?.weekStatusdata ?? null,
        data?.Time ?? null,
        data?.lastUpdatedTime ?? null,
        data?.timeZone ?? null,
        data?.HourlyBasedTime ?? null,
        data?.MintuesBasedTime ?? null,
        data?.cronId ?? null,
        data?.chatId
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('up_InsertUpdateIrrelevantChatLeadGeneration - Unexpected Error:', err);
        }
        resolve(result);
      });
    } catch (err) {
      reject(err);
      logger.error('up_InsertUpdateIrrelevantChatLeadGeneration - Unexpected Error:', err);
    }
  });
};

/**
 *  get Company Type WebsiteI
 */
export const getCompanyTypeWebsiteI = (data: any, websiteId: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call ccaas_get_company_type_websiteI(?,?)', [
        data?.domainId,
        websiteId
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('ccaas_get_company_type_websiteI - Unexpected Error:', err);
        }
        resolve(result?.[0]);
      });
    } catch (err) {
      reject(err);
      logger.error('ccaas_get_company_type_websiteI - Unexpected Error:', err);
    }

  });
};

export const getCsvCallbackData = (data: any[]): Promise<string> => {
  return new Promise((resolve, reject) => {
    try {
      const csvRows = [];

      if (data.length > 0) {
        const headers = Object.keys(data[0]);
        csvRows.push(headers);

        data.forEach((obj) => {
          const rowData = headers.map(header => obj[header]);
          csvRows.push(rowData);
        });
      }
      const writableStream = fs.createWriteStream('data.csv', { encoding: 'utf8' });
      fastcsv
        .writeToStream(writableStream, csvRows, { headers: false })
        .on('finish', () => {
          fs.readFile('data.csv', 'utf8', (err, fileContent) => {
            if (err) {
              reject(err);
              logger.error('getCsvCallbackData - Unexpected Error:', err);
            } else {
              resolve(fileContent);
            }
          });
        })
        .on('error', (err) => {
          reject(err);
          logger.error('getCsvCallbackData - Unexpected Error:', err);
        });
    } catch (err) {
      reject(err);
      logger.error('getCsvCallbackData - Unexpected Error:', err);
    }
  });
};

/**
 *  Insert and Update lead generation
 */
export const checkChatSessionAvailabilityDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call up_GetPlanRemainingChatSessionCount(?)', [
        data?.domainId,
        // data?.planId
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('up_GetPlanRemainingChatSessionCount - Unexpected Error:', err);
        }
        resolve(result);
      });
    } catch (err) {
      reject(err);
      logger.error('up_GetPlanRemainingChatSessionCount - Unexpected Error:', err);
    }
  });
};

/**
 *  Insert and Update lead generation
 */
export const updateChatSessionCountDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('call up_UpdateChatSessionCount(?,?,?,?,?)', [
        data?.domainId,
        data?.planId,
        data?.isBot,
        data?.isAgent,
        data?.IPAdress
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('up_UpdateChatSessionCount - Unexpected Error:', err);
        }
        resolve(result);
      });
    } catch (err) {
      reject(err);
      logger.error('up_UpdateChatSessionCount - Unexpected Error:', err);
    }
  });
};

/**
 * For Email Alert to the user
 */

export const getRemainingSessionEmailSent = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query('call up_GetRemainingSessionForEmailSent(?)', [
        data
      ],                        (errors: any, res: any) => {
        if (errors) {
          logger.error('up_GetRemainingSessionForEmailSent - Unexpected Error:', errors);
          reject([]);
        } else {
          if (res[0]?.length > 0) {
            resolve(res[0]);
          } else {
            resolve([]);
          }
        }
      }
      );
    } catch (error) {
      logger.error('up_GetRemainingSessionForEmailSent - Unexpected Error:', error);
      reject(error);
    }
  });
};
