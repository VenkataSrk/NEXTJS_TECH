import { CUSTOMER } from '../../helpers/constants';
import { customContactCreateHandler, customContactCreateHandlerNoAuth, customContactGetHandler, customContactGetHandlerNoAuth, customContactUpdateHandler, customerTagDisableHandler, deleteCustomContactGetHandler, deleteCustomerTagHandler, getCustomerTagDetailsHandler, getCustomerTagDisableHandler, getCustomerTagHandler, getMobileNumberStatus, getPdfPageNumber, insertCustomerTagHandler, isValidCustomerTagHandler, updateCustomerTagHandler } from './handler';
import { customerTagSchema, deleteCustomerTagSchema, getCustomerTagDetailschema, getCustomerTagListSchema, getCustomerTagSchema, insertCustomerTagSchema, isValidSchema, updateCustomerTagSchema } from './schema';

export default function customerTagHandler(server: any, options: any, next: any) {
  // insert agent status
  server.post(
    CUSTOMER.INSERT_CUSTOMER_TAG,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'insert customer tag',
        description: 'insert customer tag api',
        tags: ['customer'],
        body: insertCustomerTagSchema.body,
        response: insertCustomerTagSchema.response,
      },
    },
    insertCustomerTagHandler
  );

  // get customer tag
  server.get(
    CUSTOMER.GET_CUSTOMER_TAG,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get customer tag',
        description: 'get customer tag api',
        tags: ['customer'],
        response: getCustomerTagSchema.response,
      },
    },
    getCustomerTagHandler
  );

  // get customer tag details
  server.get(
    `${CUSTOMER.GET_CUSTOMER_TAG_DETAILS}/:cid`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get customer tag details',
        description: 'get customer tag details api',
        tags: ['customer'],
        response: getCustomerTagDetailschema.response,
      },
    },
    getCustomerTagDetailsHandler
  );

  // update customer tag
  server.put(
    `${CUSTOMER.UPDATE_CUSTOMER_TAG}/:cid`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update agent status',
        description: 'update agent status api',
        tags: ['customer'],
        params: updateCustomerTagSchema.params,
        body: updateCustomerTagSchema.body,
        response: updateCustomerTagSchema.response,
      },
    },
    updateCustomerTagHandler
  );

  // delete customer tag
  server.delete(
    `${CUSTOMER.DELETE_CUSTOMER_TAG}/:cid`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'delete customer tag',
        description: 'delete customer tag api',
        tags: ['customer'],
        params: deleteCustomerTagSchema.params,
        response: deleteCustomerTagSchema.response,
      },
    },
    deleteCustomerTagHandler
  );

  // is valid customer tag
  server.post(
    CUSTOMER.IS_VALID_CUSTOMER_TAG,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'valid agent',
        description: ' valid agent api',
        tags: ['customer'],
        body: isValidSchema.body,
        response: isValidSchema.response,
      },
    },
    isValidCustomerTagHandler
  );

  // customer tag disabled
  server.post(
    CUSTOMER.CUSTOMER_TAG_DISABLE,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'customer tag disable',
        description: 'customer tag disable api',
        tags: ['customer'],
        body: customerTagSchema.body,
        response: customerTagSchema.response,
      },
    },
    customerTagDisableHandler
  );

  // get customer tag disabled
  server.get(
    CUSTOMER.GET_CUSTOMER_TAG_DISABLE,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get customer tag disable',
        description: 'get customer tag disable api',
        tags: ['customer'],
        response: getCustomerTagListSchema.response,
      },
    },
    getCustomerTagDisableHandler
  );
// get balance
  server.get(
    `${CUSTOMER.GET_MOBILE_NUMBER_STATUS}/:mobileNumber`,
    {
      schema: {
        summary: 'get mobile number status',
        description: 'get mobile number status api',
        tags: ['customer'],
      },
    },
    getMobileNumberStatus
  );

  server.post(
    `${CUSTOMER.CREATE_CUSTOM_CONTACT}`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'create custom contact',
        description: 'create custom contact api',
        tags: ['customer'],
      },
    },
    customContactCreateHandler
  );

  server.put(
    `${CUSTOMER.UPDATE_CUSTOM_CONTACT}`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update custom contact',
        description: 'update custom contact api',
        tags: ['customer'],
      },
    },
    customContactUpdateHandler
  );

  server.get(
    `${CUSTOMER.GET_CUSTOM_CONTACT}`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get custom contact',
        description: 'get custom contact api',
        tags: ['customer'],
      },
    },
    customContactGetHandler
  );

  server.post(
    `${CUSTOMER.DELETE_CUSTOM_CONTACT}`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get custom contact',
        description: 'get custom contact api',
        tags: ['customer'],
      },
    },
    deleteCustomContactGetHandler
  );

  server.get(
    `${CUSTOMER.GET_CUSTOM_CONTACT_NO_AUTH}/:domainId`,
    {
      schema: {
        summary: 'get custom contact',
        description: 'get custom contact api',
        tags: ['customer'],
      },
    },
    customContactGetHandlerNoAuth
  );

  server.post(
    `${CUSTOMER.CREATE_CUSTOM_CONTACT_NO_AUTH}`,
    {
      schema: {
        summary: 'create custom contact no auth',
        description: 'create custom contact no auth api',
        tags: ['customer'],
      },
    },
    customContactCreateHandlerNoAuth
  );

  server.post(
    `${CUSTOMER.GET_PDF_PAGE_FINDER}`,
    {
      schema: {
        summary: 'get page pdf finder',
        description: 'get page pdf finder api',
        tags: ['GET PDF'],
      },
    },
    getPdfPageNumber
  );

  next();
}
