import { bulkUpload } from '../../config/upload';
import { BOTSETTINGS } from '../../helpers/constants';
import {
  bulkUploadQuestionAnswerListHandler,
  createQuestionAnswerHandler,
  createWebCrawlerQuestionAnswerHandler,
  deleteIrrelevantQa,
  deleteQuestionAnswerHandler,
  getKBQuestionAndAnswerBYcfid,
  getQuestionAndAnswerBYcfid,
  getQuestionAnswerByCfidNoAuth,
  getQuestionAnswerByDomainIdHandler,
  getQuestionAnswerByIdHandler,
  getQuestionAnswerByUUIdHandler,
  getQuestionAnswerListHandler,
  getQuestionAnswerListHandlerNoAuth,
  getSuggestionQuestionFromNlp,
  getUpdateIrrelevantQa,
  getUpdateIrrelevantQakb,
  insertUpdateIrrelevantQa,
  searchQuestionAnswerHandler,
  sendCallbackEmailHandler,
  updateQuestionAnswerHandler,
  updateQuestionFeedbackHandler
} from './handler';

export default function botSettingsHandler(server: any, options: any, next: any) {
  // get api for role
  server.post(
    BOTSETTINGS.CREATE_QUESTION,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'create question & answer',
        description: 'create question & answer',
        tags: ['BOT_Settings'],
      },
    },
    createQuestionAnswerHandler
  );

  server.post(
    BOTSETTINGS.UPDATE_QUESTION,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update question & answer',
        description: 'update question & answer',
        tags: ['BOT_Settings'],
      },
    },
    updateQuestionAnswerHandler
  );

  server.delete(
    BOTSETTINGS.DELETE_QUESTION,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'delete question & answer',
        description: 'delete question & answer',
        tags: ['BOT_Settings'],
      },
    },
    deleteQuestionAnswerHandler
  );

  server.get(
    BOTSETTINGS.GET_QUESTION_BY_ID,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get question & answer',
        description: 'get question & answer',
        tags: ['BOT_Settings'],
      },
    },
    getQuestionAnswerByIdHandler
  );

  server.get(
    BOTSETTINGS.GET_QUESTION_LIST,
    {
      // preValidation: [server.validateSession],
      schema: {
        summary: 'get question & answer',
        description: 'get question & answer',
        tags: ['BOT_Settings'],
      },
    },
    getQuestionAnswerListHandlerNoAuth
  );

  server.get(
    BOTSETTINGS.GET_QUESTION_LIST_AUTH,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get question & answer',
        description: 'get question & answer',
        tags: ['BOT_Settings'],
      },
    },
    getQuestionAnswerListHandler
  );

  server.post(
    BOTSETTINGS.SEARCH_QUESTION_ANSWER,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get question & answer',
        description: 'get question & answer',
        tags: ['BOT_Settings'],
      },
    },
    searchQuestionAnswerHandler
  );

  server.post(
    BOTSETTINGS.BULK_UPLOAD_QUESTION_LIST,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get question & answer',
        description: 'get question & answer',
        tags: ['BOT_Settings'],
      },
    },
    bulkUploadQuestionAnswerListHandler
  );

  server.post(
    BOTSETTINGS.CREATE_WEB_CRAWLER,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'create web crawler question & answer',
        description: 'create web crawler question & answer',
        tags: ['BOT_Settings'],
      },
    },
    createWebCrawlerQuestionAnswerHandler
  );

  server.post(
    BOTSETTINGS.UPDATE_CHAT_FEEDBACK_COUNT,
    {
      // preValidation: [server.validateSession],
      schema: {
        summary: 'update feedback for question',
        description: 'update feedback for question',
        tags: ['BOT_Settings'],
      },
    },
    updateQuestionFeedbackHandler
  );

  server.get(
    `${BOTSETTINGS.GET_QUESTION_ANSWER_CFID}/:chatId`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get question and answer based on the chatflow id',
        description: 'get question and answer based on the chatflow id api',
        tags: ['BOT_Settings'],
      },
    },
    getQuestionAndAnswerBYcfid
  );
  server.get(
    `${BOTSETTINGS.GET_KBQUESTION_ANSWER_CFID}/:chatId`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get question and answer based on the chatflow id',
        description: 'get question and answer based on the chatflow id api',
        tags: ['BOT_Settings'],
      },
    },
    getKBQuestionAndAnswerBYcfid
  );
  server.get(
    `${BOTSETTINGS.GET_QA_WEBSITEID}/:websiteId`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get question and answer based on the website id',
        description: 'get question and answer based on the website id api',
        tags: ['BOT_Settings'],
      },
    },
    getQuestionAnswerByUUIdHandler
  );
  server.get(
    `${BOTSETTINGS.GET_QA_DOMAINID}`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get question and answer based on the website id',
        description: 'get question and answer based on the website id api',
        tags: ['BOT_Settings'],
      },
    },
    getQuestionAnswerByDomainIdHandler
  );

  server.post(
    `${BOTSETTINGS.INSERT_UPDATE_IRRELEVANT_QA}`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'insert and update irrelevant question and answer',
        description: 'insert and update irrelevant question and answer api',
        tags: ['BOT_Settings'],
      },
    },
    insertUpdateIrrelevantQa
  );
  server.post(
    `${BOTSETTINGS.DELETE_IRRELEVANT_QA}`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'insert and update irrelevant question and answer',
        description: 'insert and update irrelevant question and answer api',
        tags: ['BOT_Settings'],
      },
    },
    deleteIrrelevantQa
  );
  server.get(
    `${BOTSETTINGS.GET_QA_IRRELAVANTQA}/:chatId`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get question and answer based on the website id',
        description: 'get question and answer based on the website id api',
        tags: ['QA_Irrelavant'],
      },
    },
    getUpdateIrrelevantQa
  );

  server.get(
    `${BOTSETTINGS.GET_KBQA_IRRELAVANTQA}/:webId`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get question and answer based on the website id',
        description: 'get question and answer based on the website id api',
        tags: ['QA_Irrelavant'],
      },
    },
    getUpdateIrrelevantQakb
  );

  server.post(
    `${BOTSETTINGS.SEND_CALLBACK_EMAIL_USER}`,
    {
      // preValidation: [server.validateSession],
      schema: {
        summary: 'send call back email notification',
        description: 'send call back email notification api',
        tags: ['email notification'],
      },
    },
    sendCallbackEmailHandler
  );

  server.get(
    `${BOTSETTINGS.GET_QUESTION_NO_AUTH}/:cfid`,
    {
      schema: {
        summary: 'send call back email notification',
        description: 'send call back email notification api',
        tags: ['question and answer'],
      },
    },
    getQuestionAnswerByCfidNoAuth
  );

  server.post(
    `${BOTSETTINGS.GET_QA_SUGGESTION_API}`,
    {
      schema: {
        summary: 'get qa suggestion list',
        description: 'get qa suggestion list api',
        tags: ['question and answer'],
      },
    },
    getSuggestionQuestionFromNlp
  );
  next();
}
