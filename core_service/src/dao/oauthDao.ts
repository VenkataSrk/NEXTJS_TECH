import { mysqlPoolConnection } from '../plugins/db';
import { logger } from '../plugins/winston';

export const getOauthClientInfoHandlerDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_APIGetOAuth_Client_info(?,?)',
        [data?.domainId, data?.oid],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccaas_APIGetOAuth_Client_info - Unexpected Error:', err);
          }
          resolve(result);
        }
      );
    } catch (error) {
      reject(error);
      logger.error('ccaas_APIGetOAuth_Client_info - Unexpected Error:', error);

    }
  });
};

export const deleteOauthClientInfoHandlerDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_APIDeleteOAuth_Client_info(?,?)',
        [data?.oid, data?.domainId],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccaas_APIDeleteOAuth_Client_info - Unexpected Error:', err);

          }
          resolve(result);
        }
      );
    } catch (error) {
      reject(error);
      logger.error('ccaas_APIDeleteOAuth_Client_info - Unexpected Error:', error);

    }
  });
};

export const deleteDomainApiInfoHandlerDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_APIDeleteDomain_details(?,?)',
        [data?.apid, data?.domainId],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccaas_APIDeleteDomain_details - Unexpected Error:', err);

          }
          resolve(result);
        }
      );
    } catch (error) {
      reject(error);
      logger.error('ccaas_APIDeleteDomain_details - Unexpected Error:', error);

    }
  });
};

export const deleteApiEndPointDetailsHandlerDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_APIDeleteEndPointDetails(?,?)',
        [data?.apid, data?.domainId],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccaas_APIDeleteEndPointDetails - Unexpected Error:', err);

          }
          resolve(result);
        }
      );
    } catch (error) {
      reject(error);
      logger.error('ccaas_APIDeleteEndPointDetails - Unexpected Error:', error);

    }
  });
};

export const insertUpdateOathClientInfoHandlerDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_APICreateOAuth_Client_info(?,?,?,?,?,?,?,?,?,?)',
        [
          data?.oauthId,
          data?.domainId,
          data?.oauthName,
          data?.oauthGrandType,
          data?.oauthClientId,
          data?.oauthClientSecret,
          data?.oauthAuthorizeURL,
          data?.oauthRedirectURL,
          data?.oauthScopes,
          data?.processtype,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccaas_APICreateOAuth_Client_info - Unexpected Error:', err);

          }
          resolve(result);
        }
      );
    } catch (error) {
      reject(error);
      logger.error('ccaas_APICreateOAuth_Client_info - Unexpected Error:', error);

    }
  });
};

export const insertEndPointsDetailsHandlerDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_APIInsertEndPointDetails(?,?,?,?,?,?,?,?,?)',
        [
          data?.domainId,
          data?.endPointName,
          data?.endPointDesc,
          data?.requestMethod,
          data?.endPointURL,
          data?.keyword,
          data?.parameters,
          data?.apidomainId,
          data?.fileFormat
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccaas_APIInsertEndPointDetails - Unexpected Error:', err);

          }
          resolve(result);
        }
      );
    } catch (error) {
      reject(error);
      logger.error('ccaas_APIInsertEndPointDetails - Unexpected Error:', error);

    }
  });
};

export const getDomainDetailsApiInfoHandlerDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_APIGetDomain_details(?,?)',
        [data?.domainId, data?.apid],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccaas_APIGetDomain_details - Unexpected Error:', err);

          }
          resolve(result);
        }
      );
    } catch (error) {
      reject(error);
      logger.error('ccaas_APIGetDomain_details - Unexpected Error:', error);

    }
  });
};

export const getApiEndPointsInfoHandlerDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'CALL ccaas_APIGetEndPointDetails(?,?)',
        [data?.domainId, data?.apid],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccaas_APIGetEndPointDetails - Unexpected Error:', err);

          }
          resolve(result);
        }
      );
    } catch (error) {
      reject(error);
      logger.error('ccaas_APIGetEndPointDetails - Unexpected Error:', error);

    }
  });
};

export const createInsertDomainDetailsApiHandlerDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_APIInsertDomain_details(?,?,?,?,?,?,?,?,?,?)',
        [
          data?.domainId,
          data?.domainName,
          data?.domaindesc,
          data?.authtype,
          data?.header,
          data?.headerValue,
          data?.userName,
          data?.password,
          data?.oauthId,
          data?.domainURL,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccaas_APIInsertDomain_details - Unexpected Error:', err);

          }
          resolve(result);
        }
      );
    } catch (error) {
      reject(error);
      logger.error('ccaas_APIInsertDomain_details - Unexpected Error:', error);

    }
  });
};

export const updateDomainDetailsApiHandlerDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_APIUpdateDomain_details(?,?,?,?,?,?,?,?)',
        [
          data?.apidomainId,
          data?.domainId,
          data?.header,
          data?.headerValue,
          data?.userName,
          data?.password,
          data?.oauthId,
          data?.domainURL,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccaas_APIUpdateDomain_details - Unexpected Error:', err);

          }
          resolve(result);
        }
      );
    } catch (error) {
      reject(error);
      logger.error('ccaas_APIUpdateDomain_details - Unexpected Error:', error);

    }
  });
};

export const updateEndPointsDetailsApiHandlerDao = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_APIUpdateEndPointDetails(?,?,?,?,?,?,?,?,?,?)',
        [
          data?.apiendpointId,
          data?.domainId,
          data?.endPointName,
          data?.endPointDesc,
          data?.requestMethod,
          data?.endPointURL,
          data?.keyword,
          data?.parameters,
          data?.apidomainId,
          data?.fileFormat
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccaas_APIUpdateEndPointDetails - Unexpected Error:', err);

          }
          resolve(result);
        }
      );
    } catch (error) {
      reject(error);
      logger.error('ccaas_APIUpdateEndPointDetails - Unexpected Error:', error);

    }
  });
};
