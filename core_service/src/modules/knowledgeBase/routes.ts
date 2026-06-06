
import { artcileUploaded } from '../../config/upload';
import { KNOWLEDGEBASEBOT, LEADGENERATION } from '../../helpers/constants';
import { checkChatSessionAvailability, deleteArticleById, deleteCateoryById, deleteKnowledgeBaseByWebId, deletePaymentIntegration, getArticleCategoryBywebId, getArticleListByCatID, getCompanyTypeWebsiteIdHandler, getIrrelevantDetailsByDomainId, getKnowledgeArticle, getKnowledgeCategory, getLeadGenerationByDomainId, getLeadGenerationById, getLeadUserDetailsByDomainId, getLeadUserDetailsById, getPaymentIntergation, getPaymentIntergationNoAuth, insertKnowledgeCategory, insertPaymentIntergation, insertUpdateArticleHandler, insertUpdateLeadUserDetails, inserUpdateEmailLeadGeneration, inserUpdateLeadGeneration, updateChatSessionCount, updateMultiplteArticle, updateWebsiteScrap, uploadArticleHandler } from './handler';

export default function knowledgeBaseRouter(server: any, options: any, next: any) {
  server.post(
    KNOWLEDGEBASEBOT.CREATE_CATEGORY_FOLDER,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'create category folder',
        description: 'create category folder Api',
        tags: ['KnowledgeBase'],
      },
    },
    insertKnowledgeCategory
  );

  server.get(
    `${KNOWLEDGEBASEBOT.GET_CATEGORY_FOLDER}`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get category folder',
        description: 'get category folder Api',
        tags: ['KnowledgeBase'],
      },
    },
    getKnowledgeCategory
  );

  server.post(
    `${KNOWLEDGEBASEBOT.UPLOAD_ARTICLE}`,
    {
      preValidation: [server.validateSession],
      preHandler: artcileUploaded,
      schema: {
        summary: 'upload article file',
        description: 'upload article file Api',
        tags: ['KnowledgeBase'],
      },
    },
    uploadArticleHandler
  );

  server.post(
    `${KNOWLEDGEBASEBOT.INSERT_UPDATE_ARTICLE}`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'insert and update article file',
        description: 'insert and update article file Api',
        tags: ['KnowledgeBase'],
      },
    },
    insertUpdateArticleHandler
  );
  server.get(
    `${KNOWLEDGEBASEBOT.GET_ARTICLE_LIST}`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get article list',
        description: 'get article list Api',
        tags: ['KnowledgeBase'],
      },
    },
    getKnowledgeArticle
  );
  server.put(
    `${KNOWLEDGEBASEBOT.UPDATE_WEBSITE_SCRAP}`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update website scraping',
        description: 'update website scraping Api',
        tags: ['KnowledgeBase'],
      },
    },
    updateWebsiteScrap
  );
  server.post(
    `${KNOWLEDGEBASEBOT.INSERT_UPDATE_LEAD_EMAIL_GENERATION}`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'insert update lead email generation',
        description: 'insert update lead email generation Api',
        tags: ['Lead Generation'],
      },
    },
    inserUpdateEmailLeadGeneration
  );

  server.get(
    `${KNOWLEDGEBASEBOT.GET_ARTICLE_CATEGORY_WEBID}/:webId`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get article and category by webid',
        description: 'get article and category by webid Api',
        tags: ['KnowledgeBase'],
      },
    },
    getArticleCategoryBywebId
  );
  server.delete(
    `${KNOWLEDGEBASEBOT.DELETE_CATEGORY_ID}/:categoryId`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'delete category by category id',
        description: 'delete category by category id Api',
        tags: ['KnowledgeBase'],
      },
    },
    deleteCateoryById
  );

  server.put(
    `${KNOWLEDGEBASEBOT.UPDATE_MULTIPLE_ARTICLE}`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update multiple article',
        description: 'update multiple article Api',
        tags: ['KnowledgeBase'],
      },
    },
    updateMultiplteArticle
  );
  server.delete(
    `${KNOWLEDGEBASEBOT.DELETE_KNOWLEDGE_WEBID}/:webId`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'delete knowledge base by webId',
        description: 'delete knowledge base by webId Api',
        tags: ['KnowledgeBase'],
      },
    },
    deleteKnowledgeBaseByWebId
  );

  server.post(
    `${KNOWLEDGEBASEBOT.GET_ARTICLE_LIST_CATID}`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get article list by category id',
        description: 'get article list by category id Api',
        tags: ['KnowledgeBase'],
      },
    },
    getArticleListByCatID
  );

  server.post(
    `${KNOWLEDGEBASEBOT.DELETE_ARTICLE_ID}/:articleId`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'delete article by article id',
        description: 'delete article by article id Api',
        tags: ['KnowledgeBase'],
      },
    },
    deleteArticleById
  );

  server.delete(
    `${LEADGENERATION.DELETE_PAYMENT_INTERGATION}/:deleteId`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'delete article by article id',
        description: 'delete article by article id Api',
        tags: ['payment methoad'],
      },
    },
    deletePaymentIntegration
  );

  server.post(
    `${LEADGENERATION.INSERT_UPDATE_LEAD_GENERATION}`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'insert update lead generation',
        description: 'insert update lead generation Api',
        tags: ['Lead Generation'],
      },
    },
    inserUpdateLeadGeneration
  );

  server.get(
    `${LEADGENERATION.GET_LEAD_GENERATION_ID}/:lead_id`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get lead generation by id',
        description: 'get lead generation by id Api',
        tags: ['Lead Generation'],
      },
    },
    getLeadGenerationById
  );

  server.get(
    `${LEADGENERATION.GET_LEAD_GENERATION_DOMAINID}`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get lead generation by domain id',
        description: 'get lead generation by domain id Api',
        tags: ['Lead Generation'],
      },
    },
    getLeadGenerationByDomainId
  );

  server.post(
    `${LEADGENERATION.INSERT_UPDATE_LEAD_USER_DETAILS}`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'insert update lead user details',
        description: 'insert update lead user details Api',
        tags: ['Lead Generation'],
      },
    },
    insertUpdateLeadUserDetails
  );

  server.get(
    `${LEADGENERATION.GET_LEAD_USER_DETAILS_DOMAINID}`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get lead user details by domain id',
        description: 'get lead user details by domain id Api',
        tags: ['Lead Generation'],
      },
    },
    getLeadUserDetailsByDomainId
  );

  server.get(
    `${LEADGENERATION.GET_LEAD_USER_DETAILS_ID}/:lead_id`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get lead user details by lead id',
        description: 'get lead user details by lead id Api',
        tags: ['Lead Generation'],
      },
    },
    getLeadUserDetailsById
  );

  server.post(
    `${LEADGENERATION.INSERT_PAYMENT_INTERGATION}`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'insert update payment intergation',
        description: 'insert update intergation Api',
        tags: ['Payment Intergation'],
      },
    },
    insertPaymentIntergation
  );

  server.get(
    `${LEADGENERATION.GET_PAYMENT_INTERGATION}/:companyId`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get Payment user details by lead id',
        description: 'get Payment user details by lead id Api',
        tags: ['Payment Intergation'],
      },
    },
    getPaymentIntergation
  );

  server.post(
    `${LEADGENERATION.GET_PAYMENT_INTERGATION_NO_AUTH}`,
    {
      schema: {
        summary: 'get Payment user details by lead id',
        description: 'get Payment user details by lead id Api',
        tags: ['Payment Intergation'],
      },
    },
    getPaymentIntergationNoAuth
  );

  server.get(
    `${LEADGENERATION.GET_IRRELEVANT_DETAILS_DOMAINID}/:chatId`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get Payment user details by lead id',
        description: 'get Payment user details by lead id Api',
        tags: ['Payment Intergation'],
      },
    },
    getIrrelevantDetailsByDomainId
  );

  // get company type website
  server.post(
    LEADGENERATION.GET_COMPANY_TYPE_WEBSITEID,
    {
      // preValidation: [server.validateSession],
      schema: {
        summary: 'get company type website',
        description: 'get company type website api',
        tags: ['KnowledgeBase'],
        // response: getCustomerContactPhoneNumberchema.response,
      },
    },
    getCompanyTypeWebsiteIdHandler
  );

  server.post(
    LEADGENERATION.GET_CHAT_SESSION_AVAILABILITY,
    {
      // preValidation: [server.validateSession],
      schema: {
        summary: 'get company type website',
        description: 'get company type website api',
        tags: ['chat session'],
        // response: getCustomerContactPhoneNumberchema.response,
      },
    },
    checkChatSessionAvailability
  );

  server.post(
    LEADGENERATION.UPDATE_CHAT_SESSION_COUNT,
    {
      // preValidation: [server.validateSession],
      schema: {
        summary: 'get company type website',
        description: 'get company type website api',
        tags: ['chat session'],
        // response: getCustomerContactPhoneNumberchema.response,
      },
    },
    updateChatSessionCount
  );
  next();
}
