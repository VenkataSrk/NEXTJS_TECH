import { CONTACT } from '../../helpers/constants';
import {
  createCustomerEditHandler,
  crmInsertContactHandler,
  deleteCustomerContactHandler,
  deleteMultipleCustomerContactHandler,
  getAiConservationdHandler,
  getBlockCustomerContactHandler,
  getBlockCustomerRequestContactHandler,
  getContactLogsHandler,
  getCustomerAppoinentBookingLogHandler,
  getCustomerConatctByDateHandler,
  getCustomerConatctByEmailHandler,
  getCustomerConatctByPhoneNumberHandler,
  getCustomerConatctDetailsHandler,
  getCustomerConatctHandler,
  getCustomerConatctPaginationHandler,
  getCustomerConatctPhoneNumberHandler,
  getCustomerContactByCoidHandler,
  getCustomerContactChannelsHandler,
  getCustomerContactFieldHandler,
  getCustomerContactFieldTypeHandler,
  getCustomerEditHandler,
  getCustomerTicketLogHandler,
  getDoNotDisturbCustomerContactHandler,
  getDoNotDisturbCustomerContactRequestHandler,
  insertBulkCustometConatctHandler,
  insertCustomerContactHandler,
  insertCustomerContactMyaccHandler,
  insertMergeCustomerContactHandler,
  insertUpdateCustomerContactCallbackHandler,
  insertUpdateCustomerContactHandler,
  insertUpdateCustomerContactPrimaryChatHandler,
  isValidContactHandler,
  updateBlockCustomerHandler,
  updateBlockCustomerReqHandler,
  updateCustomerContactHandler,
  updateCustomerContactTagHandler,
  updateDoNotDisturbHandler,
  updateDoNotDisturbReqHandler,
} from './handler';
import {
  deleteContactSchema,
  getBlockCustomerContactchema,
  getBlockCustomerContactReqchema,
  getCustomerContactchema,
  getCustomerContactDetailschema,
  getCustomerContactPhoneNumberSchema,
  getDoNotDisturbCustomerContactchema,
  getDoNotDisturbCustomerContactReqchema,
  insertCustomerContactSchema,
  isValidSchema,
  updateContactSchema,
  updateCustomerContactSchema,
} from './schema';

export default function contactHandler(server: any, options: any, next: any) {
  // insert contact contact
  server.post(
    CONTACT.INSERT_CUSTOMER_CONTACT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'insert customer contact',
        description: 'insert customer contact api',
        tags: ['contact'],
        body: insertCustomerContactSchema.body,
        response: insertCustomerContactSchema.response,
      },
    },
    insertCustomerContactHandler
  );

  // insert contact contact myacc
  server.post(
    CONTACT.INSERT_CUSTOMER_CONTACT_MYACC,
    {
      // preValidation: [server.validateSession],
      schema: {
        summary: 'insert customer contact',
        description: 'insert customer contact api',
        tags: ['contact'],
        body: insertCustomerContactSchema.body,
        response: insertCustomerContactSchema.response,
      },
    },
    insertCustomerContactMyaccHandler
  );

  // get customer conatct
  server.get(
    CONTACT.GET_CUSTOMER_CONTACT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get customer contact',
        description: 'get customer contact api',
        tags: ['contact'],
        // response: getCustomerContactchema.response,
      },
    },
    getCustomerConatctHandler
  );

  // get customer contact
  server.get(
    CONTACT.GET_CUSTOMER_CONTACT_BY_COID,
    {
      schema: {
        summary: 'get customer contact by coid',
        description: 'get customer contact by coid api',
        tags: ['contact'],
      },
    },
    getCustomerContactByCoidHandler
  );

  // get customer conatct details
  server.get(
    `${CONTACT.GET_CUSTOMER_CONTACT_DETAILS}/:coid`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get customer contact details',
        description: 'get customer contact details api',
        tags: ['contact'],
        // response: getCustomerContactDetailschema.response,
      },
    },
    getCustomerConatctDetailsHandler
  );

  // update customer contact
  server.put(
    `${CONTACT.UPDATE_CUSTOMER_CONTACT}/:coid`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update customer contact',
        description: 'update customer contact api',
        tags: ['contact'],
        params: updateContactSchema.params,
        body: updateContactSchema.body,
        response: updateContactSchema.response,
      },
    },
    updateCustomerContactHandler
  );

  // delete customer contact
  server.delete(
    `${CONTACT.DELETE_CUSTOMER_CONTACT}/:coid`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'delete customer contact',
        description: 'delete customer contact api',
        tags: ['contact'],
        params: deleteContactSchema.params,
        response: deleteContactSchema.response,
      },
    },
    deleteCustomerContactHandler
  );

  // is valid CONTACT
  server.post(
    CONTACT.IS_VALID_CONTACT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'is valid contact',
        description: ' valid contact api',
        tags: ['contact'],
        body: isValidSchema.body,
        response: isValidSchema.response,
      },
    },
    isValidContactHandler
  );

  // get customer block contact
  server.get(
    CONTACT.GET_BLOCK_CUSTOMER_CONTACT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get block customer contact',
        description: 'get block customer contact api',
        tags: ['contact'],
        // response: getBlockCustomerContactchema.response,
      },
    },
    getBlockCustomerContactHandler
  );

  // get customer doNotDisturb contact
  server.get(
    CONTACT.GET_DONOTDISTURB_CUSTOMER_CONTACT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get do not disturb customer contact',
        description: 'get do not disturb customer contact api',
        tags: ['contact'],
        // response: getDoNotDisturbCustomerContactchema.response,
      },
    },
    getDoNotDisturbCustomerContactHandler
  );
  // get customer block contact request
  server.get(
    CONTACT.GET_BLOCK_CUSTOMER_CONTACT_REQUEST,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get block customer contact request',
        description: 'get block customer contact request api',
        tags: ['contact'],
        // response: getBlockCustomerContactReqchema.response,
      },
    },
    getBlockCustomerRequestContactHandler
  );

  // get customer doNotDisturb contact request
  server.get(
    CONTACT.GET_DONOTDISTURB_CUSTOMER_CONTACT_REQUEST,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get do not disturb customer contact request',
        description: 'get do not disturb customer contact request api',
        tags: ['contact'],
        // response: getDoNotDisturbCustomerContactReqchema.response,
      },
    },
    getDoNotDisturbCustomerContactRequestHandler
  );

  // insert merge customer contact
  server.post(
    CONTACT.INSERT_MERGE_CUSTOMER_CONTACT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'insert merge customer contact',
        description: 'insert merge customer contact api',
        tags: ['contact'],
      },
    },
    insertMergeCustomerContactHandler
  );

  // update block customer
  server.put(
    `${CONTACT.UPDATE_BLOCK_CUSTOMER}/:coid`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update block customer',
        description: 'update block customer api',
        tags: ['contact'],
        // params: updateContactSchema.params,
        // body: updateContactSchema.body,
        // response: updateContactSchema.response,
      },
    },
    updateBlockCustomerHandler
  );

  // update do not disturb contact
  server.put(
    `${CONTACT.UPDATE_DONOTDISTURB_CONTACT}/:coid`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update donot disturb contact',
        description: 'update donot disturb contact api',
        tags: ['contact'],
        // params: updateContactSchema.params,
        // body: updateContactSchema.body,
        // response: updateContactSchema.response,
      },
    },
    updateDoNotDisturbHandler
  );

  // update block Customer Req contact
  server.put(
    `${CONTACT.UPDATE_BLOCK_CUSTOMER_REQUEST}/:coid`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update block Customer Req contact',
        description: 'update block Customer Req contact api',
        tags: ['contact'],
        // params: updateContactSchema.params,
        // body: updateContactSchema.body,
        // response: updateContactSchema.response,
      },
    },
    updateBlockCustomerReqHandler
  );

  // update do not disturb req  contact
  server.put(
    `${CONTACT.UPDATE_DONOTDISTURB_CONTACT_REQUEST}/:coid`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update customer contact',
        description: 'update customer contact api',
        tags: ['contact'],
        // params: updateContactSchema.params,
        // body: updateContactSchema.body,
        // response: updateContactSchema.response,
      },
    },
    updateDoNotDisturbReqHandler
  );

  // upload bulk insert data
  server.post(
    CONTACT.INSERT_BULK_CUSTOMER_CONTACT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update bulk insert data',
        description: 'update bulk insert data api',
        tags: ['contact'],
      },
    },
    insertBulkCustometConatctHandler
  );

  // get customer conatct phoneNumber based
  server.get(
    CONTACT.GET_CUSTOMER_CONTACT_PHONENUMBER,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get customer contact for phoneNumber based',
        description: 'get customer contact for phoneNumber based api',
        tags: ['contact'],
        // response: getCustomerContactPhoneNumberSchema.response,
      },
    },
    getCustomerConatctPhoneNumberHandler
  );

  server.post(
    CONTACT.CREATE_EDIT_COLUMN_CONTACT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'create edit column customer contact',
        description: 'create edit column  customer contact',
        tags: ['contact'],
      },
    },
    createCustomerEditHandler
  );

  server.get(
    CONTACT.GET_EDIT_COLUMN_CONTACT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get edit column customer contact',
        description: 'get edit column  customer contact',
        tags: ['contact'],
      },
    },
    getCustomerEditHandler
  );
  // update block customer
  server.put(
    `${CONTACT.UPDATE_CUSTOMER_CONTACT_TAG}/:coid`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update customer contact tag',
        description: 'update customer contact tag api',
        tags: ['contact'],
        params: updateCustomerContactSchema.params,
        body: updateCustomerContactSchema.body,
        // response: updateCustomerContactSchema.response,
      },
    },
    updateCustomerContactTagHandler
  );

  // insert and update contact contact
  server.post(
    CONTACT.INSERT_UPDATE_CUSTOMER_CONTACT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'insert update customer contact',
        description: 'insert update customer contact api',
        tags: ['contact'],
        // body: insertCustomerContactSchema.body,
        // response: insertCustomerContactSchema.response,
      },
    },
    insertUpdateCustomerContactHandler
  );

  // insert and update without Authorization
  server.post(
    CONTACT.INSERT_UPDATE_CUSTOMER_CONTACT_NOAUTH,
    {
      schema: {
        summary: 'insert update customer contact',
        description: 'insert update customer contact api',
        tags: ['contact'],
      },
    },
    insertUpdateCustomerContactPrimaryChatHandler
  );

  // insert and update contact contact
  server.post(
    CONTACT.INSERT_UPDATE_CUSTOMER_CONTACT_CALLBACK,
    {
      preValidation: [],
      schema: {
        summary: 'insert update customer contact',
        description: 'insert update customer contact api',
        tags: ['contact'],
        // body: insertCustomerContactSchema.body,
        // response: insertCustomerContactSchema.response,
      },
    },
    insertUpdateCustomerContactCallbackHandler
  );

  // get customer conatct by email
  server.get(
    `${CONTACT.GET_CUSTOMER_CONTACT_BY_EMAIL}/:email`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get customer contact for by email',
        description: 'get customer contact for by email api',
        tags: ['contact'],
        // response: getCustomerContactEmailSchema.response,
      },
    },
    getCustomerConatctByEmailHandler
  );

  // get customer conatct by phoneNumber
  server.get(
    `${CONTACT.GET_CUSTOMER_CONTACT_BY_PHONENUMBER}/:phoneNumber`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get customer contact for by phoneNumber',
        description: 'get customer contact for by phoneNumber api',
        tags: ['contact'],
        // response: getCustomerContactPhoneNumberchema.response,
      },
    },
    getCustomerConatctByPhoneNumberHandler
  );

  // session insert for contact
  server.post(
    CONTACT.GET_CUSTOMER_CONTACT_CHANNEL,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get customer contact for channels',
        description: 'get customer contact for channels api',
        tags: ['contact'],
        // response: getCustomerContactPhoneNumberchema.response,
      },
    },
    getCustomerContactChannelsHandler
  );

  // delete for multiple customer contact
  server.post(
    CONTACT.DELETE_MULTIPLE_CUSTOMER_CONTACT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'delete multiple customer contact',
        description: 'delete multiple customer contact api',
        tags: ['contact'],
        // params: deleteContactSchema.params,
        // response: deleteContactSchema.response,
      },
    },
    deleteMultipleCustomerContactHandler
  );

  // get customer field type
  server.get(
    CONTACT.GET_CUSTOMER_CONTACT_FIELD_TYPE,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get customer contact field type',
        description: 'get customer contact field type',
        tags: ['contact'],
      },
    },
    getCustomerContactFieldTypeHandler
  );

  // get customer field
  server.get(
    CONTACT.GET_CUSTOMER_CONTACT_FIELD,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get customer contact field',
        description: 'get customer contact field',
        tags: ['contact'],
      },
    },
    getCustomerContactFieldHandler
  );

  // get customer conatct
  server.post(
    CONTACT.GET_CUSTOMER_CONTACT_PAGINATION,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get customer contact with pagination',
        description: 'get customer contact with pagination api',
        tags: ['contact'],
        // response: getCustomerContactchema.response,
      },
    },
    getCustomerConatctPaginationHandler
  );

  // get customer conatct
  server.post(
    CONTACT.GET_CUSTOMER_CONTACT_BY_DATE,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get customer contact by Date',
        description: 'get customer contact by Date Api',
        tags: ['contact'],
        // response: getCustomerContactchema.response,
      },
    },
    getCustomerConatctByDateHandler
  );

  // insert crm customer conatct
  server.post(
    CONTACT.CRM_INSERT_CONTACT,
    {
      schema: {
        summary: 'insert crm customer contact',
        description: 'insert crm customer contact api',
        tags: ['contact'],
      },
    },
    crmInsertContactHandler
  );

  // ccaas ai conservation overview
  server.get(
    CONTACT.GET_AI_CONSERVATION,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get ai conservation',
        description: 'get ai conservation',
        tags: ['contact'],
      },
    },
    getAiConservationdHandler
  );

  server.post(
    CONTACT.GET_MOBILE_CONTACT_LOG,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'create get mobile contact log',
        description: 'create get mobile contact log Api',
        tags: ['contact'],
      },
    },
    getContactLogsHandler
  );

  server.post(
    CONTACT.GET_CUSTOMER_TICKET_LOG,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get Customer Ticket Log',
        description: 'Get Customer Ticket Log Api',
        tags: ['contact'],
      },
    },
    getCustomerTicketLogHandler
  );

  server.post(
    CONTACT.GET_CUSTOMER_APPOINMENT_BOOKING_LOG,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get Customer Appoinment Booking Log',
        description: 'Get Customer Appoinment Booking Log Api',
        tags: ['contact'],
      },
    },
    getCustomerAppoinentBookingLogHandler
  );

  next();
}
