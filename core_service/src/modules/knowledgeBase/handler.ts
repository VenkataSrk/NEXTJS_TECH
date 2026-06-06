import axios from 'axios';
import moment from 'moment';
import cron from 'node-cron';
import cronSchedule from 'node-schedule';
import { v4 as uuidv4 } from 'uuid';
import { configs } from '../../config/app';
import {
  checkChatSessionAvailabilityDao,
  createCategoryDao,
  deleteArticleByIdDao,
  deleteArticleFileToNLP,
  deleteCateoryByIdDao,
  deleteKnowledgeBaseDao,
  deletePaymentIntegrationDao,
  getArticleByCatIdDao,
  getArticleCategoryByWebIdDao,
  getArticleCategoryListByWebIdDao,
  getArticleDao,
  getCategoryDao,
  getCompanyTypeWebsiteI,
  getLeadEmailGenerationByDomainIdDao,
  getLeadGenerationByDomainIdDao,
  getLeadGenerationDao,
  getLeadUserDetailsByDomainIdDao,
  getLeadUserDetailsId,
  getLeadUserReportsDao,
  getPaymentIntergationDao,
  getRemainingSessionEmailSent,
  getReportsInCSVDao,
  getUnansweredReportsInCSVDao,
  getUnanwerQuestionReportsDao,
  getWebsiteUrlDao,
  insertPaymentIntergationDao,
  insertUpdateArticleDao,
  insertUpdateEmailLeadGenerationDao,
  insertUpdateLeadGenerationDao,
  insertUpdateLeadUserDetailsDao,
  updateArticleFileToNLP,
  updateChatSessionCountDao,
  updateMultiplteArticleDao,
  updateWebsiteScrapDao,
} from '../../dao/knowledgeBasedao';
import { getWebcrawlQuestion } from '../../dao/questionAndAnswer.dao';
import { RESPONSE } from '../../helpers/constants';
import { logger } from '../../plugins/log';
/**
 *
 * @param req
 * @param res
 * @param done
 * Create category
 */
export async function insertKnowledgeCategory(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {};
    (data.ChatCategoryId = req.body?.ChatCategoryId),
      (data.web_id = req.body?.web_id),
      (data.CategoryName = req.body?.CategoryName),
      (data.CategoryDescription = req.body?.CategoryDescription),
      (data.domainId = auth.domainId);

    const createCategoryResult: any = await createCategoryDao(data);
    logger.info('createCategoryDao', createCategoryResult);
    if (createCategoryResult && createCategoryResult.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        createCategoryResult: createCategoryResult[0],
      });
    } else {
      res
        .status(200)
        .send({
          statusCode: 404,
          message: RESPONSE.not_found,
          createCategoryResult: [],
        });
    }
  } catch (err) {
    logger.error('insertKnowledgeCategory', req.headers, req.body, err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * Get category
 */
export async function getKnowledgeCategory(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {};
    data.domainId = auth.domainId;
    const getCategoryResult: any = await getCategoryDao(data);
    logger.info('getCategoryDao', getCategoryResult);
    if (getCategoryResult && getCategoryResult.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getCategoryResult: getCategoryResult[0],
      });
    } else {
      res
        .status(200)
        .send({
          statusCode: 404,
          message: RESPONSE.not_found,
          getCategoryResult: [],
        });
    }
  } catch (err) {
    logger.error('getKnowledgeCategory', req.headers, err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * upload article
 */
export async function uploadArticleHandler(req: any, res: any, done: any) {
  try {
    const uuid = uuidv4();
    const auth: any = req.headers;
    const filesarry: any = req.files?.doc;
    if (Array.isArray(filesarry)) {
      const articleResult = filesarry.map((elem: any) => {
        return {
          ...elem,
          articleUrl: `${configs.paths.articleUrl}${elem.originalname}`
        };
      });

      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        insertUpdateArticleResult: articleResult,
      });
    } else {
      res
        .status(200)
        .send({ statusCode: 404, message: RESPONSE.not_found, filesarry: [] });
    }
  } catch (err) {
    logger.error('uploadArticleHandler', req.headers, req.files, err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * Get category
 */
export async function insertUpdateArticleHandler(
  req: any,
  res: any,
  done: any
) {
  try {
    const auth: any = req.headers;
    const data: any = {};
    data.ChatArticleId = req.body?.ChatArticleId;
    data.domainId = auth?.domainId;
    data.ext = auth?.ext;
    data.web_id = req.body?.web_id;
    data.ChatCategoryId = req.body?.ChatCategoryId;
    data.FilePath = req.body?.FilePath;
    // data.ArticleLanguage = req.body?.ArticleLanguage;
    data.ArticleLanguage = req.body?.ArticleLanguage ? (Array.isArray(req.body.ArticleLanguage) ? req.body.ArticleLanguage : [req.body.ArticleLanguage]) : [];
    data.LoggedUse = req.body?.LoggedUse;
    data.CompanyType = req.body?.CompanyType;
    data.companyName = req.body?.companyName;
    let NLPResponseData: any;
    if (data?.web_id !== 0 || data.web_id !== null) {
      const getWebsiteUrlDaoResult: any = await getWebsiteUrlDao(data);
      data.websiteId = getWebsiteUrlDaoResult[0]?.websiteId;
      NLPResponseData = await updateArticleFileToNLP(data);
      if (NLPResponseData?.status === 200) {
        const nlpResult = NLPResponseData?.data?.message ?? {};
        const successFileUrl: any = [];
        for (const key of Object.keys(nlpResult)) {
          const index = Object.keys(nlpResult).indexOf(key);
          if (nlpResult[key] === 'success') {
            data.ArticleLanguage = data.ArticleLanguage[index];
            successFileUrl.push(key);
          }
        }
        const insertUpdateArticleResult: any = await successFileUrl?.map(async (list: any) => {
          data.FilePath = list;
          const insertUpdateArticleArr: any = await insertUpdateArticleDao(data);
          return insertUpdateArticleArr[0];
        });

        if (insertUpdateArticleResult && insertUpdateArticleResult.length > 0) {
          res.status(200).send({
            statusCode: 200,
            message: RESPONSE.success_message,
            insertUpdateArticleResult: insertUpdateArticleResult[0],
            NLPResponseData: NLPResponseData?.data?.message
          });
        } else {
          res.status(200).send({ statusCode: 404, message: 'Something went wrong while training your data' });
        }
      } else {
        res.status(200).send({ statusCode: 404, message: NLPResponseData?.data?.message });
      }
    } else {
      res.status(200).send({ statusCode: 404, message: 'Please provide the web ID' });
    }
  } catch (err) {
    logger.error('insertUpdateArticleHandler', req.headers, req.body, err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * Get category
 */
export async function getKnowledgeArticle(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const getArticleResult: any = await getArticleDao(auth);
    logger.info('getArticleDao', getArticleResult);
    if (getArticleResult && getArticleResult.length > 0) {
      res
        .status(200)
        .send({
          statusCode: 200,
          message: RESPONSE.success_message,
          result: getArticleResult,
        });
    } else {
      res
        .status(200)
        .send({
          statusCode: 404,
          message: RESPONSE.not_found,
          getArticleResult: [],
        });
    }
  } catch (err) {
    logger.error('getKnowledgeArticle', req.headers, err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * Get category
 */
export async function updateWebsiteScrap(req: any, res: any, done: any) {
  try {
    const data: any = {};
    const auth = req.headers;
    data.baseName = req.body?.baseName;
    data.discription = req.body?.discription;
    data.domain = req.body?.domain;
    data.web_id = req.body?.web_id;
    data.domainId = auth.domainId;
    data.ext = auth?.ext;
    data.companyType = req.body?.companyType;
    data.industry_type = req.body?.industry_type;
    data.websiteId = req.body?.websiteId ? req.body?.websiteId : uuidv4();
    // data.language = req?.body?.language;
    data.language = Array.isArray(req.body?.language) ? req.body.language.join(',') : req.body?.language;
    let getWebcrawlResult: any;
    if (req.body?.domain !== '' && req.body?.domain !== null) {
      try {
        getWebcrawlResult = await getWebcrawlQuestion(data);

      } catch (err) {
        res.status(200).send({
          statusCode: 404,
          message: getWebcrawlResult?.data?.message ?? "Please re-enter your provider's URL",
        });
      }
    }
    if (
      getWebcrawlResult?.data?.message === 'success' ||
      req.body?.domain === ''
    ) {
      await updateWebsiteScrapDao(data);
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
      });
    } else {
      res.status(200).send({
        statusCode: 404,
        message: getWebcrawlResult?.data?.message ?? 'Failure',
      });
    }
  } catch (err) {
    logger.error('updateWebsiteScrap', req.headers, req.body, err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * Get article and category by web_id
 */
export async function getArticleCategoryBywebId(req: any, res: any, done: any) {
  try {
    const data: any = {};
    data.webId = req.params?.webId;
    data.domainId = req.headers.domainId;
    if (data.webId !== 0 || data.webId !== null) {
      const getArticleCategoryResult: any = await getArticleCategoryListByWebIdDao(data);
      logger.info('getArticleCategoryListByWebIdDao', getArticleCategoryResult);
      if (getArticleCategoryResult && getArticleCategoryResult.length > 0) {
        res
          .status(200)
          .send({
            statusCode: 200,
            message: RESPONSE.success_message,
            result: getArticleCategoryResult,
          });
      } else {
        res
          .status(200)
          .send({
            statusCode: 404,
            message: RESPONSE.not_found,
            getArticleCategoryResult: [],
          });
      }
    } else {
      res
        .status(200)
        .send({ statusCode: 404, message: RESPONSE.please_provide_webId });
    }
  } catch (err) {
    logger.error('getArticleCategoryBywebId', req.headers, req.params, err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * Delete category By id
 */
export async function deleteCateoryById(req: any, res: any, done: any) {
  try {
    const data: any = {};
    data.categoryId = req.params?.categoryId;
    data.domainId = req.headers.domainId;
    await deleteCateoryByIdDao(data);
    res
      .status(200)
      .send({ statusCode: 200, message: RESPONSE.success_message });
  } catch (err) {
    logger.error('deleteCateoryById', req.headers, err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * Update multiple article into category
 */

export async function updateMultiplteArticle(req: any, res: any, done: any) {
  try {
    const data: any = {};
    data.articleId = req.body?.articleId;
    data.ChatCategoryId = req.body?.ChatCategoryId;
    data.articleId?.map(async (list: any) => {
      await updateMultiplteArticleDao(data, list);
    });
    if (data?.articleId.length > 0) {
      res
        .status(200)
        .send({ statusCode: 200, message: RESPONSE.success_message });
    } else {
      res
        .status(200)
        .send({ statusCode: 404, message: RESPONSE.please_select_articleId });
    }
  } catch (err) {
    logger.error('updateMultiplteArticle', req.body, err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * Delete category, article and webScrap by webId
 */

export async function deleteKnowledgeBaseByWebId(
  req: any,
  res: any,
  done: any
) {
  try {
    const data: any = req.params?.webId;
    if (data !== 0 && data !== null) {
      const deleteResponse: any = await deleteKnowledgeBaseDao(data);
      logger.info('deleteKnowledgeBaseDao', deleteResponse);
      if (deleteResponse[0][0]?.errCode === 0) {
        res.status(200).send({ statusCode: 200, message: deleteResponse[0][0]?.errMsg });
      } else {
        res.status(200).send({ statusCode: 404, message: deleteResponse[0][0]?.errMsg });
      }
    } else {
      res
        .status(200)
        .send({ statusCode: 404, message: RESPONSE.please_provide_webId });
    }
  } catch (err) {
    logger.error('deleteKnowledgeBaseByWebId', req.params, err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * Delete category, article and webScrap by webId
 */

export async function getArticleListByCatID(req: any, res: any, done: any) {
  try {
    const data: any = {};
    data.chatCatId = req.body?.catId;
    data.webId = req.body?.webId;
    if (data?.chatCatId !== 0 || data?.webId !== 0) {
      const getArticleListResult: any = await getArticleByCatIdDao(data);
      logger.info('getArticleByCatIdDao', getArticleListResult);
      if (getArticleListResult && getArticleListResult.length > 0) {
        res
          .status(200)
          .send({
            statusCode: 200,
            message: RESPONSE.success_message,
            result: getArticleListResult,
          });
      } else {
        res
          .status(200)
          .send({
            statusCode: 404,
            message: RESPONSE.not_found,
            getArticleListResult: [],
          });
      }
    } else {
      res
        .status(200)
        .send({ statusCode: 404, message: RESPONSE.please_provide_webId });
    }
  } catch (err) {
    logger.error('getArticleListByCatID', req.body, err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * Delete article By id
 */
export async function deleteArticleById(req: any, res: any, done: any) {
  try {
    const data: any = {};
    data.articleId = req.params?.articleId;
    data.websiteId = req.body?.websiteId;
    data.FilePath = req.body?.FilePath;
    data.domainId = req.headers.domainId;
    if (data.articleId !== 0 && data.articleId !== null) {
      const insertUpdateArticleResult: any = await deleteArticleByIdDao(data);
      logger.info('deleteArticleByIdDao', insertUpdateArticleResult);
      const deleteResult: any = await deleteArticleFileToNLP(data, insertUpdateArticleResult);
      logger.info('deleteArticleFileToNLP', deleteResult);
      if (deleteResult?.data === 'success') {
        res.status(200).send({ statusCode: 200, message: RESPONSE.success_message });
      } else {
        res
          .status(200)
          .send({ statusCode: 404, message: RESPONSE.internal_error });
      }

    } else {
      res
        .status(200)
        .send({ statusCode: 404, message: 'Please provide article id' });
    }
  } catch (err) {
    logger.error('deleteArticleById', req.headers, req.params, req.body, err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}
/**
 *
 * @param req
 * @param res
 * @param done
 * Delete article By id
 */
export async function deletePaymentIntegration(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {};
    data.deleteId = req.params?.deleteId;
    data.domainId = auth.domainId;
    if (data.deleteId !== 0 && data.deleteId !== null) {
      await deletePaymentIntegrationDao(data);
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message });
    } else {
      res.status(200).send({ statusCode: 404, message: 'Please provide delete id' });
    }
  } catch (err) {
    logger.error('deletePaymentIntegration', req.headers, req.params, err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * Create and update lead generation
 */
export async function inserUpdateLeadGeneration(req: any, res: any, done: any) {
  try {
    const data: any = {};
    data.LeadId = req.body?.leadId ?? null;
    data.Method = req.body?.method ?? null;
    data.connector = req.body?.connector ?? null;
    data.Email = req.body?.email ?? null;
    data.ScheduleType = req.body?.scheduleType ?? null;
    data.domainId = req.headers?.domainId;
    data.Time = req.body?.time ?? null;
    data.dayStatus = req.body?.dayStatus;
    data.weekStatusdata = req.body?.weekStatusdata;
    data.lastUpdatedTime = req.body?.lastUpdatedTime;
    data.timeZone = req.body?.timeZone;
    data.HourlyBasedTime = req.body?.HourlyBasedTime;
    data.MintuesBasedTime = req.body?.MintuesBasedTime;
    data.cronJobChanges = req.body?.cronJobChanges;
    const schedule = parseInt(data?.ScheduleType, 10);
    const d = new Date(data.Time);
    const minutes = d.getMinutes();
    const hourTime = d.getHours();
    const timeMinutes = minutes;
    const timeHours = hourTime;
    const HourlyTime = parseInt(data?.HourlyBasedTime, 10);
    const minutesTime = parseInt(data?.MintuesBasedTime, 10);
    const methodStatus: any = data.Method?.length ? JSON.parse(data.Method).includes('1') : false;
    data.cronId = req.body?.cronId !== '' ? req.body?.cronId : uuidv4();

    if (data?.LeadId !== '' && data?.LeadId !== null && data?.LeadId !== undefined && data?.LeadId !== 0) {
      try {
        const current_job = cronSchedule.scheduledJobs[data?.cronId];
        current_job.cancel();
        data.cronId = uuidv4();
      } catch (err) {
        logger.error('inserUpdateLeadGeneration', req.headers, req.body, err);
      }
    }

    if (methodStatus) {
      if (schedule === 1) {
        cronSchedule.scheduleJob(data?.cronId, `${timeMinutes} ${timeHours} * * *`, () =>
          processScheduleTask(data)
        );
      } else if (schedule === 2) {
        cronSchedule.scheduleJob(data?.cronId, `${timeMinutes} ${timeHours} * * ${data?.dayStatus}`, () =>
          processScheduleTask(data)
        );
      } else if (schedule === 3) {
        cronSchedule.scheduleJob(data?.cronId, `${timeMinutes} ${timeHours} * * *`, () =>
          processCustomTask(data)
        );
      } else if (schedule === 4) {
        if (HourlyTime !== 0 && minutesTime !== 0) {
          cronSchedule.scheduleJob(data?.cronId, `*/${minutesTime} */${HourlyTime} * * *`, () =>
            processScheduleTask(data)
          );
        } else if (HourlyTime === 0 && minutesTime !== 0) {
          cronSchedule.scheduleJob(data?.cronId, `*/${minutesTime} * * * *`, () =>
            processScheduleTask(data)
          );
        } else if (HourlyTime !== 0 && minutesTime === 0) {
          cronSchedule.scheduleJob(data?.cronId, `0 */${HourlyTime} * * *`, () =>
            processScheduleTask(data)
          );
        }
      }
    }
    const result: any = await insertUpdateLeadGenerationDao(data);
    logger.info('insertUpdateLeadGenerationDao', result);
    res.status(200).send({ statusCode: 200, message: result[0][0]?.errmsg });
  } catch (err) {
    logger.error('inserUpdateLeadGeneration', req.headers, req.body, err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * Lead Generation by Id
 */
export async function getLeadGenerationById(req: any, res: any, done: any) {
  try {
    const data: any = {};
    data.LeadId = req.params?.lead_id;
    if (
      data.LeadId !== '' &&
      data.LeadId !== null &&
      data.LeadId !== undefined
    ) {
      const results: any = await getLeadGenerationDao(data);
      logger.info('getLeadGenerationDao', results);
      if (results[0]?.length > 0) {
        res
          .status(200)
          .send({ statusCode: 200, message: 'Success', result: results[0] });
      } else {
        res
          .status(200)
          .send({ statusCode: 404, message: 'No Data Found', result: [] });
      }
    } else {
      res
        .status(200)
        .send({ statusCode: 404, message: 'Please provide the Lead Id' });
    }
  } catch (err) {
    logger.error('getLeadGenerationById', req.params, err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * Lead Generation by DomainID
 */
export async function getLeadGenerationByDomainId(
  req: any,
  res: any,
  done: any
) {
  try {
    const data: any = {};
    data.domainId = req.headers?.domainId;

    const results: any = await getLeadGenerationByDomainIdDao(data);
    logger.info('getLeadGenerationByDomainIdDao', results);
    if (results.length > 0) {
      res
        .status(200)
        .send({ statusCode: 200, message: 'Success', result: results });
    } else {
      res
        .status(200)
        .send({ statusCode: 404, message: 'No Data Found', result: [] });
    }
  } catch (err) {
    logger.error('getLeadGenerationByDomainId', req.headers, err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * Lead Generation by UserDetails Created
 */
export async function insertUpdateLeadUserDetails(
  req: any,
  res: any,
  done: any
) {
  try {
    const data: any = {};
    data.leadUserId = req.body?.leadUserId;
    data.domainId = req.headers?.domainId;
    data.name = req.body?.name;
    data.email = req.body?.email;
    data.phoneNumber = req.body?.phoneNumber;
    data.ipAddress = req.body?.ipAddress;
    data.address = req.body?.address;
    const results: any = await insertUpdateLeadUserDetailsDao(data);
    logger.info('insertUpdateLeadUserDetailsDao', results);
    res
      .status(200)
      .send({ statusCode: 200, message: 'Success', result: results });
  } catch (err) {
    logger.error('insertUpdateLeadUserDetails', req.headers, req.body, err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}
/**
 *
 * @param req
 * @param res
 * @param done
 * Payment Intergration by UserDetails Created
 */
export async function insertPaymentIntergation(req: any, res: any, done: any) {
  try {
    const data: any = {};
    data.tokenId = req.body?.tokenId;
    data.domainId = req.headers?.domainId;
    data.paymentType = req.body?.paymentType;
    data.secretJson = req.body?.secretJson;
    data.companyId = req.body?.companyId;

    const results: any = await insertPaymentIntergationDao(data);
    logger.info('insertPaymentIntergationDao', results);
    res
      .status(200)
      .send({ statusCode: 200, message: 'Success', result: results });
  } catch (err) {
    logger.error('insertPaymentIntergation', req.headers, req.body, err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}
/**
 *
 * @param req
 * @param res
 * @param done
 * get payment intergration by UserDetails Created
 */
export async function getPaymentIntergation(req: any, res: any, done: any) {
  try {
    const data: any = {};
    data.domainId = req.headers?.domainId;
    data.companyId = req.params?.companyId;
    const results: any = await getPaymentIntergationDao(data);
    logger.info('getPaymentIntergationDao', results);
    if (results.length > 0) {
      res
        .status(200)
        .send({ statusCode: 200, message: 'Success', result: results });
    } else {
      res
        .status(200)
        .send({ statusCode: 404, message: 'No Data Found', result: [] });
    }
  } catch (err) {
    logger.error('getPaymentIntergation', req.headers, req.params, err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * get payment intergration by UserDetails Created no auth
 */
export async function getPaymentIntergationNoAuth(req: any, res: any, done: any) {
  try {
    const data: any = {};
    data.domainId = req.body?.domainId;
    data.companyId = req.body?.companyId;
    const results: any = await getPaymentIntergationDao(data);
    logger.info('getPaymentIntergationDao', results);
    if (results.length > 0) {
      res
        .status(200)
        .send({ statusCode: 200, message: 'Success', result: results });
    } else {
      res
        .status(200)
        .send({ statusCode: 404, message: 'No Data Found', result: [] });
    }
  } catch (err) {
    logger.error('getPaymentIntergationNoAuth', req.body, err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * Lead Generation by DomainID
 */
export async function getLeadUserDetailsByDomainId(
  req: any,
  res: any,
  done: any
) {
  try {
    const data: any = {};
    data.domainId = req.headers?.domainId;
    const results: any = await getLeadUserDetailsByDomainIdDao(data);
    logger.info('getLeadUserDetailsByDomainIdDao', results);
    if (results.length > 0) {
      res
        .status(200)
        .send({ statusCode: 200, message: 'Success', result: results });
    } else {
      res
        .status(200)
        .send({ statusCode: 404, message: 'No Data Found', result: [] });
    }
  } catch (err) {
    logger.error('getLeadUserDetailsByDomainId', req.headers, err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * Lead Generation by DomainID
 */
export async function getLeadUserDetailsById(req: any, res: any, done: any) {
  try {
    const data: any = {};
    data.LeadId = req.params?.lead_id;
    const results: any = await getLeadUserDetailsId(data);
    logger.info('getLeadUserDetailsId', results);
    if (results.length > 0) {
      res
        .status(200)
        .send({ statusCode: 200, message: 'Success', result: results });
    } else {
      res
        .status(200)
        .send({ statusCode: 404, message: 'No Data Found', result: [] });
    }
  } catch (err) {
    logger.error('getLeadUserDetailsById', req.params, err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export const processCustomTask = async (data: any) => {
  try {
    const today = new Date();
    const currentDay = today.getDate();
    const currentWeekday = today.getDay();
    const targetWeekday = parseInt(data?.dayStatus, 10);

    if (currentWeekday === targetWeekday) {
      const currentWeek = Math.ceil(currentDay / 7);
      if (currentWeek === parseInt(data?.weekStatusdata, 10)) {
        const currentTime = moment().valueOf();
        const results: any = await getLeadGenerationByDomainIdDao(data);
        data.lastUpdated = results[0].lastUpdatedDate;
        const leadUserReports: any = await getLeadUserReportsDao(data);
        const updateLeadDetails: any = {
          LeadId: results[0]?.LeadId,
          domainId: data?.domainId,
          lastUpdatedTime: currentTime,
        };
        await insertUpdateLeadGenerationDao(updateLeadDetails);
        if (leadUserReports[0].length > 0) {
          const getReportsInCSV: any = await getReportsInCSVDao(leadUserReports[0]);
          const body = {
            id: uuidv4(),
            subject: 'Lead Generation',
            from: process.env.SUPPORT_EMAIL,
            to: results[0]?.Email,
            text: 'Kindly have a look on the generated lead report',
            sessionId: uuidv4(),
            fileName: '',
            filePath: '',
            attachments: [
              {
                filename: 'reportFile.csv',
                content: getReportsInCSV,
              },
            ],
          };
          await axios
            .post(
              'https://ccaasapi.worktual.co.uk/emailService/v1/send_mail_lead',
              body
            )
            .then((res: any) => {
              logger.info('processCustomTask', res.status);
            });
        }
      }
    }
  } catch (error) {
    logger.error('processCustomTask', error);
  }
};

export const processScheduleTask = async (data: any) => {
  try {
    const currentTime = moment().valueOf();
    const results: any = await getLeadGenerationByDomainIdDao(data);
    data.lastUpdated = results[0].lastUpdatedDate;
    const leadUserReports: any = await getLeadUserReportsDao(data);
    const updateLeadDetails: any = {
      LeadId: results[0]?.LeadId,
      domainId: data?.domainId,
      lastUpdatedTime: currentTime,
    };
    await insertUpdateLeadGenerationDao(updateLeadDetails);
    if (leadUserReports[0].length > 0) {
      const getReportsInCSV: any = await getReportsInCSVDao(leadUserReports[0]);
      const email = results[0]?.Email && JSON.parse(results[0]?.Email);

      const body = {
        id: uuidv4(),
        subject: 'Lead Generation',
        from: process.env.SUPPORT_EMAIL,
        to: email?.join(),
        text: 'Kindly have a look on the generated lead report',
        sessionId: uuidv4(),
        fileName: '',
        filePath: '',
        attachments: [
          {
            filename: 'reportFile.csv',
            content: getReportsInCSV,
          },
        ],
      };
      await axios
        .post(
          'https://ccaasapi.worktual.co.uk/emailService/v1/send_mail_lead',
          body
        )
        .then((res: any) => {
          logger.info('processScheduleTask', res.status);
        });
    }
  } catch (error) {
    logger.error('processScheduleTask', error);
  }

};

/**
 *
 * @param req
 * @param res
 * @param done
 * Create and update lead generation
 */
export async function inserUpdateEmailLeadGeneration(req: any, res: any, done: any) {
  try {
    const data: any = {};
    data.LeadId = req.body?.leadId ?? null;
    data.Method = req.body?.method ?? null;
    data.connector = req.body?.connector ?? null;
    data.Email = req.body?.email ?? null;
    data.ScheduleType = req.body?.scheduleType ?? null;
    data.domainId = req.headers?.domainId;
    data.Time = req.body?.time ?? null;
    data.dayStatus = req.body?.dayStatus,
      data.weekStatusdata = req.body?.weekStatusdata;
    data.lastUpdatedTime = req.body?.lastUpdatedTime;
    data.timeZone = req.body?.timeZone;
    data.HourlyBasedTime = req.body?.HourlyBasedTime;
    data.MintuesBasedTime = req.body?.MintuesBasedTime;
    data.cronId = req.body?.cronId !== '' ? req.body?.cronId : uuidv4();
    data.chatId = req.body?.chatId ?? 0;
    const schedule = parseInt(data?.ScheduleType, 10);
    const d = new Date(data.Time);
    const minutes = d.getMinutes();
    const hourTime = d.getHours();
    const timeMinutes = minutes;
    const timeHours = hourTime;
    const HourlyTime = parseInt(data?.HourlyBasedTime, 10);
    const minutesTime = parseInt(data?.MintuesBasedTime, 10);
    const methodStatus: any = data.Method?.length ? JSON.parse(data.Method).includes('1') : false;

    if (data?.LeadId !== '' && data?.LeadId !== null && data?.LeadId !== undefined && data?.LeadId !== 0) {
      try {
        const current_job = cronSchedule.scheduledJobs[data?.cronId];
        current_job.cancel();
        data.cronId = uuidv4();
      } catch (err) {
        logger.error('inserUpdateEmailLeadGeneration', req.headers, req.body, err);
      }
    }

    if (methodStatus) {
      if (schedule === 1) {
        cronSchedule.scheduleJob(data.cronId, `${timeMinutes} ${timeHours} * * *`, () =>
          processScheduleEmailTask(data)
        );
      } else if (schedule === 2) {
        cronSchedule.scheduleJob(data.cronId, `${timeMinutes} ${timeHours} * * ${data?.dayStatus}`, () => processScheduleEmailTask(data)
        );
      } else if (schedule === 3) {
        cronSchedule.scheduleJob(data.cronId, `${timeMinutes} ${timeHours} * * *`, () =>
          processCustomEmailTask(data)
        );
      } else if (schedule === 4) {
        if (HourlyTime !== 0 && minutesTime !== 0) {
          cronSchedule.scheduleJob(data.cronId, `*/${minutesTime} */${HourlyTime} * * *`, () =>
            processScheduleEmailTask(data)
          );
        } else if (HourlyTime === 0 && minutesTime !== 0) {
          cronSchedule.scheduleJob(data.cronId, `*/${minutesTime} * * * *`, () =>
            processScheduleEmailTask(data)
          );
        } else if (HourlyTime !== 0 && minutesTime === 0) {
          cronSchedule.scheduleJob(data.cronId, `0 */${HourlyTime} * * *`, () =>
            processScheduleEmailTask(data)
          );
        }
      }
    }
    const result: any = await insertUpdateEmailLeadGenerationDao(data);
    logger.info('insertUpdateEmailLeadGenerationDao', result);
    res.status(200).send({ statusCode: 200, message: result[0][0]?.errmsg });
  } catch (err) {
    logger.error('inserUpdateEmailLeadGeneration', req.headers, req.body, err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export const processScheduleEmailTask = async (data: any) => {
  try {
    const currentTime = moment().valueOf();
    const results: any = await getLeadEmailGenerationByDomainIdDao(data);
    logger.info('getLeadEmailGenerationByDomainIdDao', results);
    data.lastUpdated = results[0].lastUpdatedDate;
    const leadUserReports: any = await getUnanwerQuestionReportsDao(data);
    logger.info('getUnanwerQuestionReportsDao', leadUserReports);
    const updateLeadDetails: any = {
      LeadId: results[0]?.LeadId,
      domainId: data?.domainId,
      lastUpdatedTime: currentTime,
    };

    await insertUpdateEmailLeadGenerationDao(updateLeadDetails);
    logger.info('insertUpdateEmailLeadGenerationDao', updateLeadDetails);
    if (JSON.parse(leadUserReports[0]?.questionAnswer).length > 0) {
      const getReportsInCSV: any = await getUnansweredReportsInCSVDao(JSON.parse(leadUserReports[0]?.questionAnswer));
      const email = results[0]?.Email && JSON.parse(results[0]?.Email);
      const body = {
        id: uuidv4(),
        subject: 'Unresponded Queries',
        from: process.env.SUPPORT_EMAIL,
        to: email?.join(),
        text: 'Kindly have a look on the generated unresponded queries report',
        sessionId: uuidv4(),
        fileName: '',
        filePath: '',
        attachments: [
          {
            filename: 'unrespondedFile.csv',
            content: getReportsInCSV,
          },
        ],
      };
      await axios
        .post(
          'https://ccaasapi.worktual.co.uk/emailService/v1/send_mail_lead',
          body
        )
        .then((res: any) => {
          logger.info('processScheduleEmailTask', res.status);
        });
    }
  } catch (error) {
    logger.error('processScheduleEmailTask', error);
  }

};

/**
 *
 * @param data
 */

export const processCustomEmailTask = async (data: any) => {
  try {
    const today = new Date();
    const currentDay = today.getDate();
    const currentWeekday = today.getDay();
    const targetWeekday = parseInt(data?.dayStatus, 10);

    if (currentWeekday === targetWeekday) {
      const currentWeek = Math.ceil(currentDay / 7);
      if (currentWeek === parseInt(data?.weekStatusdata, 10)) {
        const currentTime = moment().valueOf();
        const results: any = await getLeadEmailGenerationByDomainIdDao(data);
        data.lastUpdated = results[0].lastUpdatedDate;
        const leadUserReports: any = await getUnanwerQuestionReportsDao(data);
        const updateLeadDetails: any = {
          LeadId: results[0]?.LeadId,
          domainId: data?.domainId,
          lastUpdatedTime: currentTime,
        };

        await insertUpdateEmailLeadGenerationDao(updateLeadDetails);
        if (JSON.parse(leadUserReports[0]?.questionAnswer).length > 0) {
          const getReportsInCSV: any = await getUnansweredReportsInCSVDao(JSON.parse(leadUserReports[0]?.questionAnswer));
          const email = results[0]?.Email && JSON.parse(results[0]?.Email);
          const body = {
            id: uuidv4(),
            subject: 'Unresponded Queries',
            from: process.env.SUPPORT_EMAIL,
            to: email?.join(),
            text: 'Kindly have a look on the generated unresponded queries report',
            sessionId: uuidv4(),
            fileName: '',
            filePath: '',
            attachments: [
              {
                filename: 'unrespondedFile.csv',
                content: getReportsInCSV,
              },
            ],
          };
          await axios
          .post(
            'https://ccaasapi.worktual.co.uk/emailService/v1/send_mail_lead',
            body
          )
          .then((res: any) => {
            logger.info('processCustomEmailTask', res.status);
          });
        }
      }
    }
  } catch (error) {
    logger.error('processCustomEmailTask', error);
  }
};

/**
 *
 * @param req
 * @param res
 * @param done
 * Lead Generation by DomainID
 */
export async function getIrrelevantDetailsByDomainId(
  req: any,
  res: any,
  done: any
) {
  try {
    const data: any = {};
    data.domainId = req.headers?.domainId;
    data.chatId = req.params?.chatId;
    const results: any = await getLeadEmailGenerationByDomainIdDao(data);
    logger.info('getLeadEmailGenerationByDomainIdDao', results);
    if (results.length > 0) {
      res
        .status(200)
        .send({ statusCode: 200, message: 'Success', result: results });
    } else {
      res
        .status(200)
        .send({ statusCode: 404, message: 'No Data Found', result: [] });
    }
  } catch (err) {
    logger.error('getIrrelevantDetailsByDomainId', req.headers, req.params, err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getCompanyTypeWebsiteIdHandler(req: any, res: any, done: any) {
  try {
    const websiteId = req.body.websiteId;
    const data: any = {
      domainId: req.body?.domainId,
    };
    if (websiteId !== '') {
      const getwebsiteId: any = await getCompanyTypeWebsiteI(data, websiteId);
      logger.info('getCompanyTypeWebsiteI', getwebsiteId);
      if (Array.isArray(getwebsiteId) && getwebsiteId.length > 0) {
        res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getwebsiteIdCompanyTypeRes: getwebsiteId });
      } else {
        res.status(200).send({ statusCode: 200, message: RESPONSE.not_found, getwebsiteIdCompanyTypeRes: [] });
      }
    } else {
      res.status(200).send({ statusCode: 405, message: RESPONSE.websiteId });
    }
  } catch (err) {

    logger.error('getCompanyTypeWebsiteIdHandler', req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function checkChatSessionAvailability(req: any, res: any, done: any) {
  try {
    const data: any = {
      domainId: req.body?.domainId,
      planId: req.body?.planId,
    };
    if (data?.domainId !== '') {
      const getwebsiteId: any = await checkChatSessionAvailabilityDao(data);
      setEmailAlertForUser(data?.domainId);
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getwebsiteIdCompanyTypeRes: getwebsiteId });
    } else {
      res.status(200).send({ statusCode: 405, message: RESPONSE.websiteId });
    }
  } catch (err) {

    logger.error('checkChatSessionAvailability', req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

const setEmailAlertForUser = async (domainId: any) => {
  try {
    const isEmail: any = await getRemainingSessionEmailSent(domainId);
    logger.info('getRemainingSessionEmailSent', isEmail);
    if (isEmail?.[0]?.isEmailSent) {
      callSessionExpiryAPI(isEmail[0]);
    }
  } catch (error) {
    logger.error('setEmailAlertForUser', domainId, error);
    console.log(error);
  }
};

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function updateChatSessionCount(req: any, res: any, done: any) {
  try {
    const data: any = {
      domainId: req.body?.domainId,
      planId: req.body?.planId,
      isBot: req.body?.isBot,
      isAgent: req.body?.isAgent,
      IPAdress: req.body?.IPAdress

    };
    if (data?.domainId !== '' && data?.planId !== '') {
      const getwebsiteId: any = await updateChatSessionCountDao(data);
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getwebsiteIdCompanyTypeRes: getwebsiteId });
    } else {
      res.status(200).send({ statusCode: 405, message: RESPONSE.websiteId });
    }
  } catch (err) {

    logger.error('updateChatSessionCount', req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

// Session ExpiryAPI

export const callSessionExpiryAPI = async (data: any) => {
  const payload = {
    isEmailSent: data?.isEmailSent,
    adminEmail: data?.adminEmail
  };
  try {
    const response = await axios.post(process.env.sessionExpiry, payload, {
      headers: {
        'Content-Type': 'application/json',
      },
    }
    );
    return response.data;
  } catch (error: any) {
    logger.error('callSessionExpiryAPI', error);
    // throw error;
  }
};
