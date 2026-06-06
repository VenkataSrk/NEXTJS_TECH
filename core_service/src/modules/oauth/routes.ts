import { OAUTHCLIENT } from '../../helpers/constants';
import {
  createInsertDomainDetailsApiHandler,
  deleteApiEndPointDetailsHandler,
  deleteDomainApiInfoHandler,
  deleteOauthClientInfoHandler,
  getApiEndPointsInfoHandler,
  getDomainDetailsApiInfoHandler,
  getOauthClientInfoHandler,
  insertEndPointsDetailsHandler,
  insertUpdateOathClientInfoHandler,
  updateDomainDetailsApiHandler,
  updateEndPointsDetailsApiHandler,
} from './handler';

export default function oauthClientHandler(
  server: any,
  options: any,
  next: any
) {
  // oauthclient get api connectors
  server.get(
    `${OAUTHCLIENT.GET_OAUTH_CLIENT_INFO}/:oid`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get Oauth Client Info',
        description: 'Get Oauth Client api',
        tags: ['oauthClient'],
      },
    },
    getOauthClientInfoHandler
  );

  // oauthclient get api domainId
  server.get(
    `${OAUTHCLIENT.GET_DOMAIN_DETAILS_API}/:apid`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get Oauth Client Info',
        description: 'Get Oauth Client api',
        tags: ['oauthClient'],
      },
    },
    getDomainDetailsApiInfoHandler
  );

  // oauthclient get api domainId
  server.get(
    `${OAUTHCLIENT.GET_API_END_POINTS_DETAILS}/:apid`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get Oauth Client Info',
        description: 'Get Oauth Client api',
        tags: ['oauthClient'],
      },
    },
    getApiEndPointsInfoHandler
  );

  // oauthclient delete api connectors
  server.delete(
    `${OAUTHCLIENT.DELETE_OAUTH_CLIENT_INFO}/:oid`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'delete Oauth Client Info',
        description: 'delete Oauth Client Info Api',
        tags: ['oauthClient'],
      },
    },
    deleteOauthClientInfoHandler
  );

  server.delete(
    `${OAUTHCLIENT.DELETE_DOMAIN_API}/:apid`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'delete Domain Details Info',
        description: 'delete Oauth Client Info Api',
        tags: ['oauthClient'],
      },
    },
    deleteDomainApiInfoHandler
  );

  server.delete(
    `${OAUTHCLIENT.DELETE_API_END_POINT_DETAILS}/:apid`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'delete API END POINT Details Info',
        description: 'delete API END POINT',
        tags: ['oauthClient'],
      },
    },
    deleteApiEndPointDetailsHandler
  );

  // insert and update ccaas_APICreateOAuth_Client_info
  server.post(
    OAUTHCLIENT.INSERT_UPDATE_OATH_CLIENT_INFO,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Insert Oauth Client Info and udpate Oauth Client Info',
        description: 'Insert Oauth Client Info and udpate Oauth Client Info api',
        tags: ['oauthClient'],
      },
    },
    insertUpdateOathClientInfoHandler
  );

  // insert ccaas_APIInsertDomain_details
  server.post(
    OAUTHCLIENT.INSERT_END_POINTS_DETAILS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Insert Oauth Client Info and udpate Oauth Client Info',
        description: 'Insert Oauth Client Info and udpate Oauth Client Info api',
        tags: ['oauthClient'],
      },
    },
    insertEndPointsDetailsHandler
  );

  // insert ccaas_APIInsertDomain_details
  server.post(
    OAUTHCLIENT.CREATE_INSERT_DOMAIN_DETAILS_API,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Insert Oauth Client Info and udpate Oauth Client Info',
        description: 'Insert Oauth Client Info and udpate Oauth Client Info api',
        tags: ['oauthClient'],
      },
    },
    createInsertDomainDetailsApiHandler
  );

  // update ccaas_APIUpdateDomain_details
  server.patch(
    OAUTHCLIENT.UPDATE_DOMAIN_DETAILS_API,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update Client Info and udpate Oauth Client Info',
        description: 'update Client Info api',
        tags: ['oauthClient'],
      },
    },
    updateDomainDetailsApiHandler
  );

  // update ccaas_APIUpdateEndPointDetails
  server.patch(
    OAUTHCLIENT.UPDATE_END_POINTS_DETAILS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update Client Info and udpate Oauth Client Info',
        description: 'update Client Info api',
        tags: ['oauthClient'],
      },
    },
    updateEndPointsDetailsApiHandler
  );
  next();
}
