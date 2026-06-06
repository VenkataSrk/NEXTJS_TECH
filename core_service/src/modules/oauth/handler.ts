import {
  // getApiCreateEndPointsInfoHandlerDao,
  createInsertDomainDetailsApiHandlerDao,
  deleteApiEndPointDetailsHandlerDao,
  deleteDomainApiInfoHandlerDao,
  deleteOauthClientInfoHandlerDao,
  getApiEndPointsInfoHandlerDao,
  getDomainDetailsApiInfoHandlerDao,
  getOauthClientInfoHandlerDao,
  insertEndPointsDetailsHandlerDao,
  insertUpdateOathClientInfoHandlerDao,
  updateDomainDetailsApiHandlerDao,
  updateEndPointsDetailsApiHandlerDao,
} from '../../dao/oauthDao';
import { RESPONSE } from '../../helpers/constants';
import { logger } from '../../plugins/log';

export async function getOauthClientInfoHandler(req: any, res: any) {
  try {
    const data: any = {
      domainId: req?.headers?.domainId,
      oid: req?.params?.oid,
    };
    const getOauthClientInfoResponse: any = await getOauthClientInfoHandlerDao(data);
    logger.info('getOauthClientInfoHandlerDao', getOauthClientInfoResponse);
    if (Array.isArray(getOauthClientInfoResponse) && getOauthClientInfoResponse.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getClientInfo: getOauthClientInfoResponse,
      });
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        getClientInfo: [],
      });
    }
  } catch (error) {
    req.log.error(error);
    logger.error('getKBQuestionAndAnswerBYcfid', req.headers, req.params, error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function deleteOauthClientInfoHandler(req: any, res: any) {
  try {
    const data: any = {
      domainId: req?.headers?.domainId,
      oid: req?.params?.oid,
    };

    const deleteOauthClientInfoHandlerResponse: any = await deleteOauthClientInfoHandlerDao(data);
    logger.info('deleteOauthClientInfoHandlerDao', deleteOauthClientInfoHandlerResponse);
    if (deleteOauthClientInfoHandlerResponse) {
      res
        .status(200)
        .send({ statusCode: 200, message: deleteOauthClientInfoHandlerResponse[0][0]?.errmsg });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (error) {
    req.log.error(error);
    logger.error('deleteOauthClientInfoHandler', req.headers, req.params, error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function deleteApiEndPointDetailsHandler(req: any, res: any) {
  try {
    const data: any = {
      apid: req?.params?.apid,
      domainId: req?.headers?.domainId,
    };

    const deleteApiEndPointDetailsHandlerResponse: any = await deleteApiEndPointDetailsHandlerDao(data);
    logger.info('deleteApiEndPointDetailsHandlerDao', deleteApiEndPointDetailsHandlerResponse);
    if (deleteApiEndPointDetailsHandlerResponse) {
      res.status(200).send({ statusCode: 200, message: deleteApiEndPointDetailsHandlerResponse[0][0]?.errmsg });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (error) {
    logger.error('deleteApiEndPointDetailsHandler', req.headers, req.params, error);
    req.log.error(error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function deleteDomainApiInfoHandler(req: any, res: any) {
  try {
    const data = {
      apid : req?.params?.apid,
      domainId : req?.headers?.domainId,
    };

    const deleteDomainApiInfoHandlerResponse : any = await deleteDomainApiInfoHandlerDao(data);
    logger.info('deleteDomainApiInfoHandlerDao', deleteDomainApiInfoHandlerResponse);
    if (deleteDomainApiInfoHandlerResponse) {
      res
        .status(200)
        .send({ statusCode: 200, message: deleteDomainApiInfoHandlerResponse[0][0]?.errmsg });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }

  } catch (error) {
    logger.error('deleteDomainApiInfoHandler', req.headers, req.params, error);
    req.log.error(error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function insertUpdateOathClientInfoHandler(req: any, res: any) {
  try {
    const data: any = {
      domainId: req?.headers?.domainId,
      oauthId: req?.body?.oauthId,
      oauthName: req?.body?.oauthName,
      oauthGrandType: req?.body?.oauthGrandType,
      oauthClientId: req?.body?.oauthClientId,
      oauthClientSecret: req?.body?.oauthClientSecret,
      oauthAuthorizeURL: req?.body?.oauthAuthorizeURL,
      oauthRedirectURL: req?.body?.oauthRedirectURL,
      oauthScopes: req?.body?.oauthScopes,
      processtype: req?.body?.processtype,
    };

    const insertUpdateOathClientInfoHandlerRes : any = await insertUpdateOathClientInfoHandlerDao(data);
    logger.info('insertUpdateOathClientInfoHandlerDao', insertUpdateOathClientInfoHandlerRes);
    if (Array.isArray(insertUpdateOathClientInfoHandlerRes) && insertUpdateOathClientInfoHandlerRes.length > 0) {
      res.status(200).send({ statusCode: 200, message: insertUpdateOathClientInfoHandlerRes[0][0]?.errmsg, oauthId: insertUpdateOathClientInfoHandlerRes[0][0]?.oauthId });
    } else {
      res.status(200).send({ statusCode: 404, message: [] });
    }
  } catch (error) {
    logger.error('insertUpdateOathClientInfoHandler', req.headers, req.body, error);
    req.log.error(error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function insertEndPointsDetailsHandler(req: any, res: any) {
  try {
    const data: any = req.body;
    data.domainId = req?.headers?.domainId;
    const insertEndPointsDetailsHandlerRes: any = await insertEndPointsDetailsHandlerDao(data);
    logger.info('insertEndPointsDetailsHandlerDao', insertEndPointsDetailsHandlerRes);
    if (Array.isArray(insertEndPointsDetailsHandlerRes) && insertEndPointsDetailsHandlerRes.length > 0) {
      res.status(200).send({ statusCode: 200, message: insertEndPointsDetailsHandlerRes[0][0]?.errmsg, oauthId: insertEndPointsDetailsHandlerRes[0][0]?.apiendpointId });
    } else {
      res.status(200).send({ statusCode: 404, message: [] });
    }

  } catch (error) {
    logger.error('insertEndPointsDetailsHandler', req.headers, req.body, error);
    req.log.error(error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function getDomainDetailsApiInfoHandler(req:any, res: any) {
  try {
    const data: any = {
      domainId: req?.headers?.domainId,
      apid: req?.params?.apid,
    };

    const getDomainDetailsApiInfoResponse: any = await getDomainDetailsApiInfoHandlerDao(data);
    logger.info('getDomainDetailsApiInfoHandlerDao', getDomainDetailsApiInfoResponse);
    if (Array.isArray(getDomainDetailsApiInfoResponse) && getDomainDetailsApiInfoResponse.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getClientInfo: getDomainDetailsApiInfoResponse,
      });
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        getClientInfo: [],
      });
    }

  } catch (error) {
    logger.error('getDomainDetailsApiInfoHandler', req.headers, req.params, error);
    req.log.error(error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function getApiEndPointsInfoHandler(req: any, res: any) {
  try {
    const data: any = {
      domainId: req?.headers?.domainId,
      apid: req?.params?.apid,
    };
    const getApiEndPointsInfoHandlerResponse: any = await getApiEndPointsInfoHandlerDao(data);
    logger.info('getApiEndPointsInfoHandlerDao', getApiEndPointsInfoHandlerResponse);
    if (Array.isArray(getApiEndPointsInfoHandlerResponse) && getApiEndPointsInfoHandlerResponse.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: getApiEndPointsInfoHandlerResponse[1][0]?.errmsg,
        getClientInfo: getApiEndPointsInfoHandlerResponse[0],
      });
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        getClientInfo: [],
      });
    }

  } catch (error) {
    logger.error('getApiEndPointsInfoHandler', req.headers, req.params, error);
    req.log.error(error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function createInsertDomainDetailsApiHandler(req: any, res: any) {
  try {
    const data: any = {
      domainId: req?.headers?.domainId,
      domainName: req?.body?.domainName,
      domaindesc: req?.body?.domaindesc,
      authtype: req?.body?.authtype,
      header: req?.body?.header,
      headerValue: req?.body?.headerValue,
      userName: req?.body?.userName,
      password: req?.body?.password,
      oauthId: req?.body?.oauthId,
      domainURL: req?.body?.domainURL,
    };

    const createInsertDomainDetailsApiHandlerRes : any = await createInsertDomainDetailsApiHandlerDao(data);
    logger.info('createInsertDomainDetailsApiHandlerDao', createInsertDomainDetailsApiHandlerRes);
    if (Array.isArray(createInsertDomainDetailsApiHandlerRes) && createInsertDomainDetailsApiHandlerRes.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: createInsertDomainDetailsApiHandlerRes[0][0]?.errmsg,
        apidomainId: createInsertDomainDetailsApiHandlerRes[0][0]?.apidomainId
      });
    } else {
      res.status(200).send({ statusCode: 404, message: [] });
    }

  } catch (error) {
    logger.error('createInsertDomainDetailsApiHandler', req.headers, req.body, error);
    req.log.error(error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function updateDomainDetailsApiHandler(req: any, res: any) {
  try {
    const data: any = {
      apidomainId: req?.body?.apidomainId,
      domainId: req?.headers?.domainId,
      header: req?.body?.header,
      headerValue: req?.body?.headerValue,
      userName: req?.body?.userName,
      password: req?.body?.password,
      oauthId: req?.body?.oauthId,
      domainURL: req?.body?.domainURL,
    };

    if (data.apidomainId !== '' && data.apidomainId !== null) {
      const result: any = await updateDomainDetailsApiHandlerDao(data);
      res.status(200).send({ statusCode: 200, message: result[0][0]?.errmsg });
    } else {
      res.status(404).send({ statusCode: 200, message: [], result: 'Please give the apidomainId' });
    }

  } catch (error) {
    logger.error('updateDomainDetailsApiHandler', req.headers, req.body, error);
    req.log.error(error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function updateEndPointsDetailsApiHandler(req: any, res: any) {
  try {
    const data: any = req.body;
    data.domainId = req?.headers?.domainId;
    if (data.apiendpointId !== '' && data.apiendpointId !== null) {
      const result: any = await updateEndPointsDetailsApiHandlerDao(data);
      res.status(200).send({ statusCode: 200, message: result[0][0]?.errmsg });
    } else {
      res.status(404).send({ statusCode: 200, message: [], result: 'Please give the apiendpointId' });
    }

  } catch (error) {
    logger.error('updateEndPointsDetailsApiHandler', req.headers, req.body, error);
    req.log.error(error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}
