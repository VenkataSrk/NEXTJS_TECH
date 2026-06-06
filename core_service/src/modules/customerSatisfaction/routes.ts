import { CUSTOMER_SATISFACTION } from '../../helpers/constants';
import { getAgentInteractionNotesHandler, getCustomerSatisficationCHannelHandler, getCustomerSatisficationChannelStatusHandler, getCustomerSatisficationRatingHandler, getInteractionOverAllNotesHandler, insertCustomerSatisficationHandler, insertupdateCustomerFeedbackHandler, insertUpdateInteractionNotesHandler } from './handler';

export default function customerSatisfactionHandler(server: any, options: any, next: any) {

  // insert customer satisfication
  server.post(
    CUSTOMER_SATISFACTION.INSERT_CUSTOMER_SATISFICATION,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'insert customer satisfication ',
        description: 'insert customer satisfication api',
        tags: ['customer_satisfication'],
        // body: insertCustomerTagSchema.body,
        // response: insertCustomerTagSchema.response,
      },
    },
    insertCustomerSatisficationHandler
  );

  // get customer satisfication rating
  server.get(
    CUSTOMER_SATISFACTION.GET_CUSTOMER_SATISFICATION_RATING_DESIGN,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get customer satisfication rating design',
        description: 'get customer satisfication rating design api',
        tags: ['customer_satisfication'],
      },
    },
    getCustomerSatisficationRatingHandler
  );

  // get customer satisfication rating
  server.get(
    CUSTOMER_SATISFACTION.GET_CUSTOMER_SATISFICATION_CHANNEL_STATUS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get customer satisfication channel status',
        description: 'get customer satisfication channel status api',
        tags: ['customer_satisfication'],
      },
    },
    getCustomerSatisficationChannelStatusHandler
  );

  // insert update customer feedback
  server.post(
    CUSTOMER_SATISFACTION.INSERT_UPDATE_CUSTOMER_FEEDBACK,
    {
      // preValidation: [server.validateSession],
      schema: {
        summary: 'insert update customer feedback',
        description: 'insert update customer feedback api',
        tags: ['customer_satisfication'],
        // body: insertCustomerTagSchema.body,
        // response: insertCustomerTagSchema.response,
      },
    },
    insertupdateCustomerFeedbackHandler
  );

  // get customer satisfication channel
  server.post(
    CUSTOMER_SATISFACTION.GET_CUSTOMER_SATISFICATION_CHANNEL,
    {
      // preValidation: [server.validateSession],
      schema: {
        summary: 'get customer satisfication channel',
        description: 'get customer satisfication channelapi',
        tags: ['customer_satisfication'],
        // body: insertCustomerTagSchema.body,
        // response: insertCustomerTagSchema.response,
      },
    },
    getCustomerSatisficationCHannelHandler
  );

  // get customer satisfication channel
  server.post(
    CUSTOMER_SATISFACTION.INSERT_UPDATE_INTERACTION_NOTES,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'nsert update ineraction notes',
        description: 'nsert update interaction notes',
        tags: ['customer_satisfication'],
        // body: insertCustomerTagSchema.body,
        // response: insertCustomerTagSchema.response,
      },
    },
    insertUpdateInteractionNotesHandler
  );

    // get customer satisfication channel
  server.post(
    CUSTOMER_SATISFACTION.GET_INTERACTIONOVERALL_NOTES,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get Interaction Overall Notes',
        description: 'get Interaction Overall Notes',
        tags: ['customer_satisfication'],
          // body: insertCustomerTagSchema.body,
          // response: insertCustomerTagSchema.response,
      },
    },
    getInteractionOverAllNotesHandler
    );
        // get customer satisfication channel
  server.post(
    CUSTOMER_SATISFACTION.GET_AGENT_INTERACTION_NOTES_INFO,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get Interaction Overall Notes',
        description: 'get Interaction Overall Notes',
        tags: ['customer_satisfication'],
              // body: insertCustomerTagSchema.body,
              // response: insertCustomerTagSchema.response,
      },
    },
    getAgentInteractionNotesHandler
        );
  next();
}
