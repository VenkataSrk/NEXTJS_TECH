import { deleteKnowledgeBaseCategoryDao, getKnowledgeBaseArticleDtlDao, getKnowledgeBaseDtlDao, insertKnowledgeBaseDao, updateKnowledgeBaseCategoryDao, updateMoveFilesToFolderDao } from '../../dao/knowledgeBase';
import { deleteArticleFileToNLP, updateArticleFileToNLP } from '../../dao/knowledgeBasedao';
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
export async function insertKnowledgeBaseHandlerWrapper(req: any, res: any, done: any) {
  try {
    const data = {
      domainId: req.headers.domainId,
      websiteId: req?.body?.websiteId,
      knowledgeBaseType: req?.body?.knowledgeBaseType,
      articleType: req?.body?.articleType,
      categoryId: req?.body?.categoryId,
      articlePath: req?.body?.articlePath,
      websiteUrl: req?.body?.websiteUrl,
      articleLanguage: req?.body?.articleLanguage,
      articleStatus: req?.body?.articleStatus,
      metaDesc: req?.body?.metaDesc,
      relatedPages: req?.body?.relatedPages,
      categoryName: req?.body?.categoryName,
      categoryDescription: req?.body?.categoryDescription,
      createdBy: req?.body?.createdBy
    };
    let insertKnowledgeBaseRes: any;
    if (Array.isArray(data?.articlePath) && data?.articlePath.length > 0) {

      const articlePath = data.articlePath.map((elem: any) => elem?.articleUrl);
      const articleLang = data.articlePath.map((elem: any) => elem?.language);
      const payload = {
        FilePath: articlePath,
        domainId: data.domainId,
        // websiteId: data?.websiteId,
        ext: req.headers.ext?.toString(),
        CompanyType: '',
        companyName: req.body?.companyName ?? '',
        ArticleLanguage: articleLang,
        by: data?.knowledgeBaseType === 2007001 ? 'agent' : 'customer'
      };
      const articleToNLPResult: any = await updateArticleFileToNLP(payload);
      logger.info('updateArticleFileToNLP', articleToNLPResult);
      if (articleToNLPResult?.status === 200) {
        const nlpResult = articleToNLPResult?.data?.message ?? {};
        for (const key of Object.keys(nlpResult)) {
          if (nlpResult[key] === 'success') {
            const insertPayload = {
              ...data,
              articlePath: data.articlePath.filter((elem: any) => elem?.articleUrl === key)
            };
            const result: any = await insertKnowledgeBaseDao(insertPayload);
            insertKnowledgeBaseRes = [...result[0], articleToNLPResult?.data?.message];
          }
        }
      }
    } else if (data?.websiteUrl) {
      const payload = {
        domain: data.websiteUrl,
        domainId: data.domainId,
        // websiteId: data.websiteId,
        ext: req.headers.ext?.toString(),
        industry_type: ''
      };
      const result: any = await getWebcrawlQuestion(payload);
      const insertRes = await insertKnowledgeBaseDao(data);

      insertKnowledgeBaseRes = [...insertRes[0], result?.data];
    } else {
      insertKnowledgeBaseRes = await insertKnowledgeBaseDao(data);
    }
    if (insertKnowledgeBaseRes && insertKnowledgeBaseRes.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        result: insertKnowledgeBaseRes,
      });
    } else {
      res
        .status(200)
        .send({
          statusCode: 404,
          message: RESPONSE.not_found,
          result: [],
        });
    }
  } catch (err) {
    logger.error('insertKnowledgeBaseHandlerWrapper', req.headers, req.body, err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * Create category
 */
export async function getknowledgeBaseDtlHandlerWrapper(req: any, res: any, done: any) {
  try {
    const data = {
      domainId: req.headers.domainId,
      knowledgeBaseType: req?.params?.knowledgeBaseType ?? null,
    };
    const getknowledgeBaseDtlRes: any = await getKnowledgeBaseDtlDao(data);
    logger.info('getKnowledgeBaseDtlDao', getknowledgeBaseDtlRes);
    if (getknowledgeBaseDtlRes && getknowledgeBaseDtlRes.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        result: getknowledgeBaseDtlRes,
      });
    } else {
      res
        .status(200)
        .send({
          statusCode: 404,
          message: RESPONSE.not_found,
          result: [],
        });
    }
  } catch (err) {
    logger.error('getknowledgeBaseDtlHandlerWrapper', req.headers, req.params, err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * Create category
 */
export async function getknowledgeBaseArticleDtlHandlerWrapper(req: any, res: any, done: any) {
  try {
    const data = {
      knowledgeBaseType: req?.params?.knowledgeBaseType,
      categoryId: req?.params?.categoryId,
    };
    const getknowledgeBaseDtlRes: any = await getKnowledgeBaseArticleDtlDao(data);
    logger.info('getKnowledgeBaseArticleDtlDao', getknowledgeBaseDtlRes);
    if (getknowledgeBaseDtlRes && getknowledgeBaseDtlRes.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        result: getknowledgeBaseDtlRes,
      });
    } else {
      res
        .status(200)
        .send({
          statusCode: 404,
          message: RESPONSE.not_found,
          result: [],
        });
    }
  } catch (err) {
    logger.error('getknowledgeBaseArticleDtlHandlerWrapper', req.params, err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * update category details
 */
export async function updateKnowledgeBaseCategoryDtlWrapper(req: any, res: any, done: any) {
  try {
    const data = {
      kid: req?.body?.kid,
      categoryId: req?.body?.categoryId,
      categoryName: req?.body?.categoryName,
      categoryDescription: req?.body?.categoryDescription,
    };
    const updateKnowledgeBaseCategoryRes: any = await updateKnowledgeBaseCategoryDao(data);
    logger.info('updateKnowledgeBaseCategoryDao', updateKnowledgeBaseCategoryRes);
    if (Array.isArray(updateKnowledgeBaseCategoryRes) && updateKnowledgeBaseCategoryRes.length > 0 && updateKnowledgeBaseCategoryRes[0][0].errcode === 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        result: updateKnowledgeBaseCategoryRes,
      });
    } else {
      res
        .status(200)
        .send({
          statusCode: 404,
          message: RESPONSE.not_found,
          result: [],
        });
    }
  } catch (err) {
    logger.error('updateKnowledgeBaseCategoryDtlWrapper', req.body, err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * delete category details
 */
export async function deleteKnowledgeBaseCategoryDtlWrapper(req: any, res: any, done: any) {
  try {
    const data = {
      kid: req?.body?.kid,
      categoryId: req?.body?.categoryId,
      articleId: req?.body?.articleId,
      FilePath: req?.body?.articleUrl,
      domainId: req.headers?.domainId,
      language: req?.body?.language ?? 'en',
      by: req?.body?.knowledgeBaseType
    };
    let updateKnowledgeBaseCategoryRes: any;
    if (data?.FilePath) {
      const deleteFromNlpRes: any = await deleteArticleFileToNLP(data);
      logger.info('deleteArticleFileToNLP', deleteFromNlpRes);
      if (deleteFromNlpRes?.status === 200 && deleteFromNlpRes?.data === 'success') {
        updateKnowledgeBaseCategoryRes = await deleteKnowledgeBaseCategoryDao(data);
      }
    } else {
      updateKnowledgeBaseCategoryRes = await deleteKnowledgeBaseCategoryDao(data);
    }
    if (updateKnowledgeBaseCategoryRes && updateKnowledgeBaseCategoryRes.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        result: updateKnowledgeBaseCategoryRes,
      });
    } else {
      res
        .status(200)
        .send({
          statusCode: 404,
          message: RESPONSE.Delete_Failed,
          result: [],
        });
    }
  } catch (err) {
    logger.error('deleteKnowledgeBaseCategoryDtlWrapper', req.headers, req.body, err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * update category details
 */
export async function updateMoveFilesToFolderWrapper(req: any, res: any, done: any) {
  try {
    const data = {
      articleId: req?.body?.articleId,
      categoryId: req?.body?.categoryId,
    };
    const updateKnowledgeBaseCategoryRes: any = await updateMoveFilesToFolderDao(data);
    logger.info('updateMoveFilesToFolderDao', updateKnowledgeBaseCategoryRes);
    if (Array.isArray(updateKnowledgeBaseCategoryRes) && updateKnowledgeBaseCategoryRes.length > 0 && updateKnowledgeBaseCategoryRes[0][0].errcode === 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        result: updateKnowledgeBaseCategoryRes,
      });
    } else {
      res
        .status(200)
        .send({
          statusCode: 404,
          message: RESPONSE.not_found,
          result: [],
        });
    }
  } catch (err) {
    logger.error('updateMoveFilesToFolderWrapper', req.body, err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}
