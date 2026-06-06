
import { Uploaded } from '../../config/upload';
import { WEBCHAT } from '../../helpers/constants';
import {
  agentConnectIdGenerate,
  botConnectIdGenerate,
  chatCustomerFileUpload,
  getAgentAvailabilityHandler,
  getAIChatMessage,
  getAvailableAgentDetails,
  getLocationDetailsHandler,
  replaceWebchatMessage,
  storeCustomerCallBackRequest,
  updateChatDisposition,
  webChatGetAvailableAgent,
  webChatValidateCustomer,
  webChatValidateCustomerByUid
} from './handlers';

export default function CustomerWebChat(server: any, options: any, next: any) {
  server.post(
    WEBCHAT.CREATE_WEBCHAT,
    {
      schema: {
        description: 'webchat create session and available agent call',
        tags: ['WEBCHAT'],
      },
    },
    webChatGetAvailableAgent,
  );

  server.get(
    `${WEBCHAT.GET_CUSTOMER_HISTORY}/:chatDurationHistoryId`,
    {
      preValidation: [server.validateSession],
      schema: {
        description: 'validate Customer Already exists',
        tags: ['WEBCHAT'],
      },
    },
    webChatValidateCustomer,
  );

  server.get(
    `${WEBCHAT.GET_CUSTOMER_HISTORY_BY_UID}/:chatUniqIdentifier`,
    {
      schema: {
        description: 'validate Customer Already exists',
        tags: ['WEBCHAT'],
      },
    },
    webChatValidateCustomerByUid,
  );

  server.post(
    `${WEBCHAT.STORE_CUSTOMER_CALL_BACK}`,
    {
      schema: {
        description: 'store the customer callBack request api',
        tags: ['WEBCHAT'],
      },
    },
    storeCustomerCallBackRequest,
  );

  server.post(
    WEBCHAT.CREATE_CUSTOMER_UPLOAD,
    {
      preHandler: Uploaded,
      schema: {
        description: 'create the file path  api',
        tags: ['WEBCHAT'],
      },
    },
    chatCustomerFileUpload,
  );

  server.post(
    WEBCHAT.GET_AGENT_DETAILS,
    {
      schema: {
        description: 'get customer history by email id',
        tags: ['WEBCHAT'],
      },
    },
    getAvailableAgentDetails,
  );

  server.get(
    `${WEBCHAT.GET_AICHAT_HISTORY}/:chid`,
    {
      schema: {
        description: 'get ai chat history by chathistory id',
        tags: ['WEBCHAT'],
      },
    },
    getAIChatMessage,
  );
  server.post(
    `${WEBCHAT.UPDATE_CHAT_DISPOSTION}`,
    {
      schema: {
        description: 'Update chat disposition status',
        tags: ['WEBCHAT'],
      },
    },
    updateChatDisposition,
  );

  server.post(
    `${WEBCHAT.BOT_CONNECT_ID_GENERATE}`,
    {
      schema: {
        description: 'Bot connected updated',
        tags: ['WEBCHAT'],
      },
    },
    botConnectIdGenerate,
  );

  server.post(
    `${WEBCHAT.AGENT_CONNECT_ID_GENERATE}`,
    {
      schema: {
        description: 'agent connected updated',
        tags: ['WEBCHAT'],
      },
    },
    agentConnectIdGenerate,
  );

  // get location information
  server.get(
    `${WEBCHAT.GET_LOCATION_DETAILS}/:IPAddress`,
    {
      schema: {
        description: 'get location details api',
        tags: ['WEBCHAT'],
      },
    },
    getLocationDetailsHandler,
  );

  // get agent availability information
  server.post(
    WEBCHAT.GET_AGENT_AVAILABILITY,
    {
      schema: {
        description: 'get agent availability api',
        tags: ['WEBCHAT'],
      },
    },
    getAgentAvailabilityHandler
  );

  // replace the webchat message
  server.post(
    WEBCHAT.REPLACE_WEBCHAT_MESSAGE,
    {
      schema: {
        description: 'replace the webchat message api',
        tags: ['WEBCHAT'],
      },
    },
    replaceWebchatMessage
  );

  next();
}
