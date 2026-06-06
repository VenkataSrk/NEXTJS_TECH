import { mysqlPoolConnection } from '../plugins/db';
import { logger } from '../plugins/winston';

export const insertKnowledgeBaseDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query('call ccaas_insert_KnowledgeBase(?,?,?,?,?,?,?,?,?,?,?,?,?,?)', [
        data?.domainId ?? null,
        data?.websiteId ?? null,
        data?.knowledgeBaseType ?? null,
        data?.articleType ?? null,
        data?.categoryId ?? null,
        data?.articlePath ? data?.articlePath?.length > 0 ? JSON.stringify(data?.articlePath[0]) : JSON.stringify(data?.articlePath) : data?.articlePath,
        data?.websiteUrl ?? null,
        data?.articleLanguage ?? null,
        data?.articleStatus ?? null,
        data?.metaDesc ?? null,
        data?.relatedPages ?? null,
        data?.categoryName ?? null,
        data?.categoryDescription ?? null,
        data?.createdBy ?? null,

      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('ccaas_insert_KnowledgeBase - Unexpected Error:', err);
        } else {
          resolve(result);
        }
      });
    } catch (error) {
      reject(error);
      logger.error('ccaas_insert_KnowledgeBase - Unexpected Error:', error);
    }
  });
};

export const getKnowledgeBaseDtlDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query('call ccaas_CategoryArticleDetails(?,?)', [
        data?.domainId,
        data?.knowledgeBaseType,
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('ccaas_CategoryArticleDetails - Unexpected Error:', err);
        } else {
          resolve(result);
        }
      });
    } catch (error) {
      reject(error);
      logger.error('ccaas_CategoryArticleDetails - Unexpected Error:', error);
    }
  });
};

export const getKnowledgeBaseArticleDtlDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query('call ccaas_get_knowledgeBaseArticleDetails(?,?)', [
        data?.categoryId,
        data?.knowledgeBaseType,
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('ccaas_get_knowledgeBaseArticleDetails - Unexpected Error:', err);
        } else {
          resolve(result);
        }
      });
    } catch (error) {
      reject(error);
      logger.error('ccaas_get_knowledgeBaseArticleDetails - Unexpected Error:', error);
    }
  });
};

export const updateKnowledgeBaseCategoryDao = (data: any) => {

  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query('call ccaas_update_KnowledgeBaseCategory_dtl(?,?,?,?)', [
        data?.kid ?? null,
        data?.categoryId ?? null,
        data?.categoryName ?? null,
        data?.categoryDescription ?? null,
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('ccaas_update_KnowledgeBaseCategory_dtl - Unexpected Error:', err);
        } else {
          resolve(result);
        }
      });
    } catch (error) {
      reject(error);
      logger.error('ccaas_update_KnowledgeBaseCategory_dtl - Unexpected Error:', error);
    }
  });
};

export const deleteKnowledgeBaseCategoryDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query('call ccaas_delete_knowledgeBase(?,?,?)', [
        data?.kid ?? null,
        data?.categoryId ?? null,
        data?.articleId ?? null,
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('ccaas_delete_knowledgeBase - Unexpected Error:', err);
        } else {
          resolve(result);
        }
      });
    } catch (error) {
      reject(error);
      logger.error('ccaas_delete_knowledgeBase - Unexpected Error:', error);
    }
  });
};

export const updateMoveFilesToFolderDao = (data: any) => {

  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query('call ccaas_update_article_category_move(?,?)', [
        data?.articleId ?? null,
        data?.categoryId ?? null,
      ],                        (err: any, result: any) => {
        if (err) {
          reject(err);
          logger.error('ccaas_update_article_category_move - Unexpected Error:', err);
        } else {
          resolve(result);
        }
      });
    } catch (error) {
      reject(error);
      logger.error('ccaas_update_article_category_move - Unexpected Error:', error);
    }
  });
};
