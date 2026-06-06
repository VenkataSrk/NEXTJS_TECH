import axios from 'axios';
import moment from 'moment';
import { v4 as uuidv4 } from 'uuid';
import { deleteIrrelevantQaDao, getArticleCategoryByWebIdDao, getCsvCallbackData, insertUpdateIrrelevantQaDao } from '../../dao/knowledgeBasedao';
import { bulkInsertQestionAnswer, createQestionAnswer, createWebCrawlerQestionAnswer, deleteOldQaDao, deleteQuestionAnswer, getAllQestionAnswer, getAllQestionAnswerNoAuth, getKBQaDataDao, getQaDataDao, getQuestionAnswerById, getQuestionAnswerByWebsiteId, getQuestionByCfidNoAuth, getSuggestionQuestionFromNlpDao, getUpdateIrrelevantQaDao, getUpdateIrrelevantQaKBDao, insertCrawlWebsiteData, searchQestionAnswer, updateQestionAnswer, updateQuestionFeedback } from '../../dao/questionAndAnswer.dao';
import { RESPONSE } from '../../helpers/constants';
import { logger } from '../../plugins/log';
import { IQuestionAndAnswer, IWebCrawler } from './questionAndAnswer.model';

export async function createQuestionAnswerHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const request = req.body;
    const data: any = {
      domainId: auth.domainId,
      question: request?.question ?? null,
      answer: request?.answer ?? null,
      mapFlow: request?.mapFlow ?? null,
      uuid: uuidv4(),
      chatFlowId: request?.chatFlowId ?? null,
      language: request?.language ? JSON.stringify(request?.language) : null,
      intent: request?.intent ? JSON.stringify(request?.intent) : null,
      keywords: request?.keywords ? JSON.stringify(request?.keywords) : null,
      createdAt: moment(new Date()).format('YYYY-MM-DD HH:mm:ss'),
      discription: request?.discription ?? null,
      baseName: request?.baseName ?? null,
      websiteUrl: request?.websiteUrl ?? null,
      websiteId: request?.websiteId ?? null
    };
    let result: any = [];
    if (request.btnIntent === 1) {
      await deleteOldQaDao(data?.chatFlowId);
      const questionData: any = JSON.parse(request?.question);
      let i = 0;
      const response = request?.intent?.map((list: any) => {
        const combinedObject = {
          mapFlow: data?.mapFlow[i],
          intent: request?.intent[i],
          question: questionData[i],
          language: request?.language?.[i] ?? null
        };
        i = i + 1;
        return combinedObject;
      });
      const btnData: any = {
        chatFlowId: data?.chatFlowId,
        domainId: data?.domainId,
        createdAt: data?.createdAt,
        uuid: data?.uuid,
      };
      response?.map(async (list: any) => {
        list?.question?.map(async (data1: any) => {
          btnData.mapFlow = list?.mapFlow;
          btnData.question = data1;
          btnData.intent = JSON.stringify([list?.intent]);
          btnData.language = list?.language;
          return await createQestionAnswer(btnData);
        });
      });
      result = response;
    } else {
      result = await createQestionAnswer(data);
    }
    if (result.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        data: result
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {

    logger.error('createQuestionAnswerHandler', req.headers, req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function updateQuestionAnswerHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const request: any = req.body;
    const questionId: any = Number(req.query.questionId);
    const obj: any = {};
    Object.keys(request).forEach((key) => {
      obj[key] = request[key];
      if (key === 'intent') {
        obj[key] = JSON.stringify(request[key]);
      }
      if (key === 'language') {
        obj[key] = JSON.stringify(request[key]);
      }
      if (key === 'keywords') {
        obj[key] = JSON.stringify(request[key]);
      }
    });
    obj.updatedAt = moment(new Date()).format('YYYY-MM-DD HH:mm:ss');
    const result: any = await updateQestionAnswer(obj, questionId);
    logger.info('updateQestionAnswer', result);
    if (result.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        data: result
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {

    logger.error('updateQuestionAnswerHandler', req.headers, req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function deleteQuestionAnswerHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const request: IQuestionAndAnswer = req.query;
    const data: any = {
      domainId: auth.domainId,
      questionId: request.questionId
    };
    const result: any = await deleteQuestionAnswer(data);
    logger.info('deleteQuestionAnswer', result);
    if (result) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.delete_question_message,
        data: result
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {

    logger.error('deleteQuestionAnswerHandler', req.headers, req.query, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function getQuestionAnswerByIdHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const request: IQuestionAndAnswer = req.query;
    const result: any = await getQuestionAnswerById(request);
    logger.info('getQuestionAnswerById', result);
    if (result.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        data: result
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {

    logger.error('getQuestionAnswerByIdHandler', req.headers, req.query, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function getQuestionAnswerListHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const request: IQuestionAndAnswer = req.query;
    // temporay check for ccaas chat bot development
    if (req.headers.hasOwnProperty('domainId') || Object.prototype.hasOwnProperty.call(request, 'domainId')) {
      const data: any = {
        domainId: auth.hasOwnProperty('domainId') ? auth.domainId : request.domainId,
        limit: request.limit,
        offset: request.offset
      };
      const result: any = await getAllQestionAnswer(data);
      logger.info('getAllQestionAnswer', result);
      if (result?.[1]?.[0].count > 0) {
        res.status(200).send({
          statusCode: 200,
          message: RESPONSE.success_message,
          count: result?.[1]?.[0].count,
          data: result?.[0]
        });
      } else {
        res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
      }
    } else {
      res.status(400).send({ statusCode: 400, message: RESPONSE.bad_request });
    }
  } catch (err) {

    logger.error('getQuestionAnswerListHandler', req.headers, req.query, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function getQuestionAnswerListHandlerNoAuth(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const request = req.query;
    // temporay check for ccaas chat bot development
    if (req.headers.hasOwnProperty('domainId') || Object.prototype.hasOwnProperty.call(request, 'domainId')) {
      const data: any = {
        domainId: auth.hasOwnProperty('domainId') ? auth.domainId : request.domainId,
        limit: request.limit,
        offset: request.offset,
        chatFlowId: request?.chatFlowId
      };
      const result: any = await getAllQestionAnswerNoAuth(data);
      logger.info('getAllQestionAnswerNoAuth', result);
      if (result?.[1]?.[0].count > 0) {
        res.status(200).send({
          statusCode: 200,
          message: RESPONSE.success_message,
          count: result?.[1]?.[0].count,
          data: result?.[0]
        });
      } else {
        res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
      }
    } else {
      res.status(400).send({ statusCode: 400, message: RESPONSE.bad_request });
    }
  } catch (err) {

    logger.error('getQuestionAnswerListHandlerNoAuth', req.headers, req.query, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function searchQuestionAnswerHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const request: IQuestionAndAnswer = req.body;
    request.domainId = auth.domainId;
    const result: any = await searchQestionAnswer(request);
    logger.info('searchQestionAnswer', result);
    if (result?.[1]?.[0].count > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        count: result?.[1]?.[0].count,
        data: result?.[0]
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {
    logger.error('searchQuestionAnswerHandler', req.headers, req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function bulkUploadQuestionAnswerListHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const request: any = req.body ?? [];
    request.domainId = auth.domainId;
    // request.domainId = 11961;
    const result: any = await bulkInsertQestionAnswer(request);
    logger.info('bulkInsertQestionAnswer', result);
    if (result.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        data: result
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {

    logger.error('bulkUploadQuestionAnswerListHandler', req.headers, req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function createWebCrawlerQuestionAnswerHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      domainId: auth.domainId,
      websiteId: uuidv4(),
      domain: req.body?.domain,
      baseName: req.body?.baseName,
      discription: req.body?.discription,
      companyType: req.body?.companyType,
      ext: auth?.ext,
      language: req?.body?.language,
      web_id: req?.body?.web_id,
      knowledgeBaseType: req?.body?.knowledgeBaseType
    };
    let resultArr: any;
    let resultData: any;
    if (req.body?.domain !== '' && req.body?.domain !== null) {
      resultData = await createWebCrawlerQestionAnswer(data);
      resultArr = resultData?.message === 'success' ? resultData?.result2 : [];
    } else {
      resultArr = await insertCrawlWebsiteData(data);
    }
    if (resultData?.message === 'success' || data.domain === '') {
      const result: any = {
        webId: resultArr[0]?.web_id,
        websiteId: data.websiteId
      };
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        data: result
      });
    } else {
      res.status(200).send({
        statusCode: 404,
        message: resultData?.message ?? 'Failure',
      });
    }
  } catch (err) {

    logger.error('createWebCrawlerQuestionAnswerHandler', req.headers, req.body, err);
    res.status(500).send({ statusCode: 500, message: err });
  }
}

export async function updateQuestionFeedbackHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const request: IWebCrawler = req.body;
    const data = {
      domainId: auth.domainId,
      sent: request.sent ? request.sent : 0,
      helpfull: request.helpfull ? request.helpfull : 0,
      notHelpfull: request.notHelpfull ? request.notHelpfull : 0,
      updatedAt: moment(new Date()).format('YYYY-MM-DD HH:mm:ss')
    };
    const result: any = await updateQuestionFeedback(data);
    logger.info('updateQuestionFeedback', result);
    if (result.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        data: result
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {

    logger.error('updateQuestionFeedbackHandler', req.headers, req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function getQuestionAndAnswerBYcfid(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      domainId: auth.domainId,
      chatFlowId: req.params?.chatId ?? null
    };
    if (data.chatFlowId !== null) {
      const getQaResult: any = await getQaDataDao(data);
      logger.info('getQaDataDao', getQaResult);
      if (getQaResult.length > 0) {
        res.status(200).send({
          statusCode: 200,
          message: RESPONSE.success_message,
          data: getQaResult
        });
      } else {
        res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
      }
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.invalid_chatId });
    }
  } catch (err) {

    logger.error('getQuestionAndAnswerBYcfid', req.headers, req.params, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function getKBQuestionAndAnswerBYcfid(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      domainId: auth.domainId,
      websiteId: req.params?.chatId ?? null
    };
    if (data.websiteId !== null) {
      const getQaResult: any = await getKBQaDataDao(data);
      logger.info('getKBQaDataDao', getQaResult);
      if (getQaResult.length > 0) {
        res.status(200).send({
          statusCode: 200,
          message: RESPONSE.success_message,
          data: getQaResult
        });
      } else {
        res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
      }
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.invalid_chatId });
    }
  } catch (err) {

    logger.error('getKBQuestionAndAnswerBYcfid', req.headers, req.params, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function getQuestionAnswerByUUIdHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      domainId: auth.domainId,
      websiteId: req.params.websiteId ?? null
    };
    const result: any = await getQuestionAnswerByWebsiteId(data);
    logger.info('getQuestionAnswerByWebsiteId', result);
    if (result.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        data: result
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {

    logger.error('getQuestionAnswerByUUIdHandler', req.headers, req.params, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function getQuestionAnswerByDomainIdHandler(req: any, res: any, done: any) {
  try {
    const data = {
      domainId: req.headers.domainId,
      knowledgeBaseType: req?.params?.type ?? null
    };
    const result: any = await getArticleCategoryByWebIdDao(data);
    logger.info('getArticleCategoryByWebIdDao', result);
    if (result.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        data: result
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {

    logger.error('getQuestionAnswerByDomainIdHandler', req.headers, req.params, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 * req
 *
 */

export async function insertUpdateIrrelevantQa(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {};
    data.chatFlowId = req.body?.chatFlowId;
    data.websiteId = req.body?.websiteId;
    data.questionAnswer = req.body?.questionAnswer;
    data.domainId = auth.domainId;
    if (data.chatFlowId !== '' || data.chatFlowId !== null) {
      try {
        await insertUpdateIrrelevantQaDao(data);
      } catch {
        res.status(200).send({ statusCode: 404, message: 'Duplicate entry' });
      }
      res.status(200).send({ statusCode: 200, message: 'Success' });
    } else {
      res.status(200).send({ statusCode: 404, message: 'Please enter chatFlowId' });
    }
  } catch (err) {

    logger.error('insertUpdateIrrelevantQa', req.headers, req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}
export async function deleteIrrelevantQa(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {};
    data.chatFlowId = req.body?.chatFlowId;
    data.questionAnswer = req.body?.questionAnswer;
    data.domainId = auth.domainId;
    if (data.chatFlowId !== '' || data.chatFlowId !== null) {
      try {
        await deleteIrrelevantQaDao(data);
      } catch {
        res.status(200).send({ statusCode: 404, message: 'Duplicate entry' });
      }
      res.status(200).send({ statusCode: 200, message: 'Success' });
    } else {
      res.status(200).send({ statusCode: 404, message: 'Please enter chatFlowId' });
    }
  } catch (err) {

    logger.error('deleteIrrelevantQa', req.headers, req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function getUpdateIrrelevantQa(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      p_domainId: auth.domainId,
      p_chatFlowId: req.params?.chatId ?? null,
      websiteId: null

    };
    if (data.chatFlowId !== null) {
      const getQaResult: any = await getUpdateIrrelevantQaDao(data);
      logger.info('getUpdateIrrelevantQaDao', getQaResult);
      if (getQaResult.length > 0) {
        res.status(200).send({
          statusCode: 200,
          message: RESPONSE.success_message,
          data: getQaResult
        });
      } else {
        res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
      }
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.invalid_chatId });
    }
  } catch (err) {

    logger.error('getUpdateIrrelevantQa', req.headers, req.params, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function getUpdateIrrelevantQakb(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      p_domainId: auth.domainId,
      websiteId: req.params?.webId ?? null

    };
    if (data.websiteId !== null) {
      const getQaResult: any = await getUpdateIrrelevantQaKBDao(data);
      logger.info('getUpdateIrrelevantQaKBDao', getQaResult);
      if (getQaResult.length > 0) {
        res.status(200).send({
          statusCode: 200,
          message: RESPONSE.success_message,
          data: getQaResult
        });
      } else {
        res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
      }
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.invalid_chatId });
    }
  } catch (err) {

    logger.error('getUpdateIrrelevantQakb', req.headers, req.params, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function sendCallbackEmailHandler(req: any, res: any, done: any) {
  try {
    const data: any = {};
    data.to = req.body?.to ?? '';
    data.callbackDetails = req.body?.callbackDetails;

    const callbackDetailsDao = await getCsvCallbackData(data.callbackDetails);
    logger.info('getCsvCallbackData', callbackDetailsDao);
    if (data.callbackDetails?.length > 0) {
      const body = {
        id: uuidv4(),
        subject: 'CallBack remainder',
        from: process.env.SUPPORT_EMAIL,
        to: data.to,
        text: 'Kindly have a look on the CallBack notification',
        sessionId: uuidv4(),
        fileName: '',
        filePath: '',
        attachments: [
          {
            filename: 'callback.csv',
            content: callbackDetailsDao,
          },
        ],
      };
      await axios
        .post(
          'https://ccaasapi.worktual.co.uk/emailService/v1/send_mail_lead',
          body
        )
        .then((res1: any) => {
          logger.info(res1.status);
        });
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message });
    }

  } catch (err) {
    logger.error('sendCallbackEmailHandler', req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function getQuestionAnswerByCfidNoAuth(req: any, res: any, done: any) {
  try {
    const data: any = req.params.cfid;
    const result: any = await getQuestionByCfidNoAuth(data);
    logger.info('getQuestionByCfidNoAuth', result);
    if (result.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        data: result
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {

    logger.error('getQuestionAnswerByCfidNoAuth', req.params, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function getSuggestionQuestionFromNlp(req: any, res: any, done: any) {
  try {
    const data: any = req.body?.intent;
    const result: any = await getSuggestionQuestionFromNlpDao(data);
    logger.info('getSuggestionQuestionFromNlpDao', result);
    if (result?.data.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        data: result?.data
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {

    logger.error('getSuggestionQuestionFromNlp', req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}
