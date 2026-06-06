import { CUSTOMER_TYPE } from '../../helpers/constants';
import { deleteCustomerTypeHandler, getCustomerTypeListHandler, insertUpdateCustomerTypeHandler } from './handler';

export default function customerTypeHandler(server: any, options: any, next: any) {
  // insert update customer type
  server.post(
    CUSTOMER_TYPE.INSERT_UPDATE_CUSTOMER_TYPE,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'insert_update_customer_type',
        description: 'insert_update_customer_type api',
        tags: ['customerType'],
        // body: insertCustomerTypeSchema.body,
        /// response: insertCustomerTypeSchema.response,
      },
    },
    insertUpdateCustomerTypeHandler
  );

  // get customer type list
  server.get(
    CUSTOMER_TYPE.GET_CUSTOMER_TYPE_LIST,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get customer type list',
        description: 'get customer type list api',
        tags: ['customerType'],
        // response: getCustomerTypeSchema.response,
      },
    },
    getCustomerTypeListHandler
  );

  // delete customer type
  server.delete(
    CUSTOMER_TYPE.DELETE_CUSTOMER_TYPE,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'delete customer type',
        description: 'delete customer type api',
        tags: ['customerType'],
        // params: deleteCustomerTypeSchema.params,
        // response: deleteCustomerTypeSchema.response,
      },
    },
    deleteCustomerTypeHandler
  );
  next();
}
