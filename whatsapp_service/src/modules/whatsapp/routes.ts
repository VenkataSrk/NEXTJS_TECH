import { cpUpload } from '../../config/upload';
import { WHATSAPP } from '../../helpers/constants';
import {
  assignCallBackWhatsappHandlers,
  chatQueueByAgentStatusHandler,
  deleteCustomInputTemplateHandler,
  deleteMetaTemplateHandler,
  deleteWhatsappConfigurationHandler,
  fileUploadController,
  getDidPurchaseWhatsappBot,
  getDisableConfigDetailsHandler,
  getDispositionStatusHandler,
  getEcomPurchaseFlowHandler,
  getMetaTemplateDetailsHandler,
  getPlanInNlpHandler,
  getTemplateDetailsHandler,
  getWhatsapChannelHandler,
  getWhatsappCallbackConfigHandler,
  getWhatsappConfigDetailsHandler,
  getWhatsappConfigHandler,
  getWhatsappProviderHandler,
  insertMetaTemplateHandler,
  insertOnboardWhatsappConfigHandler,
  insertUpdateMetaWhatsappHandler,
  insertUpdateWhatsappConfigHandler,
  insertWhatsappCallbackHandler,
  insertWhatsappInHandler,
  insertWhatsappTemplateHandler,
  sendAppoinmentPaymentHandler,
  sendCaasWhatsappHandler,
  updateDidNumberPurchaseWhatsappBot,
  updateDispositionWhatsappCallbackHandler,
  updateDispositionWhatsappHandler,
  updateDispositionWhatsappPurchaseFlowHandler,
  updateEcomIsPurchaseHandler,
  updateEcomPurchaseFlowHandler,
  updateWhatsappBotStatus,
  // updateWhatsappResponse,
} from './handler';

export default function whatsappHandler(server: any, options: any, next: any) {

  // whatsapp client insert
  server.post(
    WHATSAPP.INSERT_WHATSAPP_IN,
    {
      // preValidation: [server.validateSession],
      schema: {
        summary: 'insert whatsapp onbound',
        description: 'insert whatsapp onbound',
        tags: ['whatsapp'],
      },
    },
    insertWhatsappInHandler
  );

  // update dispositin status
  server.put(
    `${WHATSAPP.UPDATE_DISPOSITION_WHATSAPP}/:WhatsappHistoryId`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update disposition whatsapp',
        description: 'update disposition whatsapp api',
        tags: ['whatsapp'],
      },
    },
    updateDispositionWhatsappHandler
  );

  // update dispositin status callback
  server.put(
    `${WHATSAPP.UPDATE_DISPOSITION_WHATSAPP_CALLBACK}/:WhatsappHistoryId`,
    {
      preValidation: [],
      schema: {
        summary: 'update disposition whatsapp',
        description: 'update disposition whatsapp api',
        tags: ['whatsapp'],
      },
    },
    updateDispositionWhatsappCallbackHandler
  );
  // delete for whatsapp configuration
  server.delete(
    WHATSAPP.DELETE_WHATSAPP_CONFIGURATION,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'delete whatsapp configuration',
        description: 'delete whatsapp configuration api',
        tags: ['whatsapp'],
      },
    },
    deleteWhatsappConfigurationHandler
  );

  server.post(
    WHATSAPP.ASSIGN_CALLBACK_WHATSAPP,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update assign callback whatsapp',
        description: 'update assign callback whatsapp api',
        tags: ['whatsapp'],
      },
    },
    assignCallBackWhatsappHandlers
  );

  // send sms request
  server.post(
    WHATSAPP.WHATSAPP_SMS_OUT,
    {
      preValidation: [],
      schema: {
        summary: 'send whatsapp',
        description: 'send a new whatsapp',
        tags: ['whatsapp'],
        // body: sendSmsSchema.body,
        // response: sendSmsSchema.response,
      },
    },
    sendCaasWhatsappHandler
  );

  server.post(
    WHATSAPP.UPDATE_WHATSAPP_BOT_STATUS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update whatsapp bot',
        description: 'update whether supervisor has enabled whatsapp bot or not',
        tags: ['whatsapp']
      },
    },
    updateWhatsappBotStatus,
  );

  server.get(
    WHATSAPP.GET_DID_PURCHASE_WHATSAPP,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update whatsapp bot',
        description: 'update whether supervisor has enabled whatsapp bot or not',
        tags: ['whatsapp']
      },
    },
    getDidPurchaseWhatsappBot,
  );

  server.post(
    `${WHATSAPP.UPDATE_DIDNUMBER_PURCHASE_WHATSAPP}/:pid`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update whatsapp bot',
        description: 'update whether supervisor has enabled whatsapp bot or not',
        tags: ['whatsapp']
      },
    },
    updateDidNumberPurchaseWhatsappBot,
  );

  // insert and update whatsapp configuration
  server.post(
    WHATSAPP.INSERT_UPDATE_WHATSAPP_CONFIGURATION,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'insert whatsapp configuration',
        description: 'insert whatsapp configuration api',
        tags: ['whatsapp']
      }
    },
    insertUpdateWhatsappConfigHandler
  );

  // insert onboard whatsapp configuration
  server.post(
    WHATSAPP.INSERT_ONBOARD_WHATSAPP_CONFIGURATION,
    {
      preValidation: [],
      schema: {
        summary: 'insert onboard whatsapp configuration',
        description: 'insert onboard whatsapp configuration api',
        tags: ['whatsapp']
      }
    },
    insertOnboardWhatsappConfigHandler
  );

  // get whatsapp configuration
  server.get(
    WHATSAPP.GET_WHATSAPP_CONFIGURATION,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get whatsapp configuration',
        description: 'get whatsapp configuration api',
        tags: ['whatsapp']
      },
    },
    getWhatsappConfigHandler,
  );

  // get whatsapp configuration
  server.get(
    WHATSAPP.GET_WHATSAPP_CONFIGURATION_CALLBACK,
    {
      preValidation: [],
      schema: {
        summary: 'get whatsapp configuration',
        description: 'get whatsapp configuration api',
        tags: ['whatsapp']
      },
    },
    getWhatsappCallbackConfigHandler,
  );

  // get whatsapp configuration details
  server.get(
    WHATSAPP.GET_WHATSAPP_CONFIGURATION_DETAILS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get whatsapp configuration',
        description: 'get whatsapp configuration api',
        tags: ['whatsapp']
      },
    },
    getWhatsappConfigDetailsHandler,
  );

  // update disable configuration
  server.put(
    WHATSAPP.UPDATE_DISABLED_CONFIGURATION,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update disable configuration',
        description: 'update disable configuration api',
        tags: ['whatsapp']
      },
    },
    getDisableConfigDetailsHandler,
  );

  // get whatsapp configuration channels
  server.post(
    WHATSAPP.GET_WHATSAPP_CHANNEL_DETAILS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get whatsapp configuration',
        description: 'get whatsapp configuration api',
        tags: ['whatsapp']
      },
    },
    getWhatsapChannelHandler,
  );

  // create whatsapp template
  server.post(
    WHATSAPP.INSERT_WHATSAPP_TEMPLATE,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'insert whatsapp template',
        description: 'insert whatsapp template api',
        tags: ['whatsapp']
      },
    },
    insertWhatsappTemplateHandler,
  );

  // get ecom purchaseflow
  server.get(
    WHATSAPP.GET_ECOM_PURCHASEFLOW,
    {
      preValidation: [],
      schema: {
        summary: 'get ecom purchaseflow',
        description: 'get ecom purchaseflow api',
        tags: ['whatsapp']
      },
    },
    getEcomPurchaseFlowHandler,
  );

  // update ecom purchaseflow
  server.put(
    WHATSAPP.UPDATE_ECOM_PURCHASEFLOW,
    {
      preValidation: [],
      schema: {
        summary: 'update Ecom purchaseflow',
        description: 'update Ecom purchaseflow api',
        tags: ['whatsapp']
      },
    },
    updateEcomPurchaseFlowHandler,
  );

  // update ecom isPurchase
  server.put(
    WHATSAPP.UPDATE_ECOM_ISPURCHASE,
    {
      preValidation: [],
      schema: {
        summary: 'update Ecom isPurchase',
        description: 'update Ecom isPurchase api',
        tags: ['whatsapp']
      },
    },
    updateEcomIsPurchaseHandler,
  );

  // update disposion purchaseflow
  server.post(
    WHATSAPP.UPDATE_DISPOSITION_WHATSAPP_PURCHASEFLOW,
    {
      preValidation: [],
      schema: {
        summary: 'update disposition whatsapp purchaseFlow',
        description: 'update disposition whatsapp purchaseFlow api',
        tags: ['whatsapp'],
      },
    },
    updateDispositionWhatsappPurchaseFlowHandler
  );

  // insert update meta whatsapp
  server.post(
    WHATSAPP.INSERT_UPDATE_META_WHATSAPP,
    {
      preValidation: [],
      schema: {
        summary: 'insert update meta whatsapp',
        description: 'insert update meta whatsapp api',
        tags: ['whatsapp'],
      },
    },
    insertUpdateMetaWhatsappHandler
  );

  // get whatsapp provider
  server.get(
    WHATSAPP.GET_WHATSAPP_PROVIDER,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get whatsapp provider',
        description: 'get whatsapp provider api',
        tags: ['whatsapp'],
      },
    },
    getWhatsappProviderHandler
  );

  // get disposition status info
  server.get(
    WHATSAPP.GET_DISPOSITION_STATUS_INFO,
    {
      preValidation: [],
      schema: {
        summary: 'get disposition status info',
        description: 'get disposition status info api',
        tags: ['whatsapp'],
      },
    },
    getDispositionStatusHandler
  );

  // insert whatsapp callback status info
  server.post(
    WHATSAPP.INSERT_WHATSAPP_CALLBACK_STATUS,
    {
      preValidation: [],
      schema: {
        summary: 'insert whatsapp callback status info',
        description: 'insert whatsapp callback status infoapi',
        tags: ['whatsapp'],
      },
    },
    insertWhatsappCallbackHandler
  );

  // delete for customer input template
  server.delete(
    WHATSAPP.DELETE_CUSTOM_INPUT_TEMPLATE,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'delete for customer input template',
        description: 'delete for customer input template api',
        tags: ['whatsapp'],
      },
    },
    deleteCustomInputTemplateHandler
  );

  // get template data
  server.get(
    WHATSAPP.GET_TEMPLATE_DETAILS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get template details',
        description: 'get template details api',
        tags: ['whatsapp'],
      },
    },
    getTemplateDetailsHandler
  );

  // send agent status queue for whatsapp
  server.post(
    WHATSAPP.SEND_AGENT_STATUS_QUEUE,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'send agent status queue',
        description: 'send agent status queue api',
        tags: ['whatsapp'],
      },
    },
    chatQueueByAgentStatusHandler
  );

  // get planId useBy nlp
  server.post(
    WHATSAPP.GET_PLANDID_USE_NLP,
    {
      preValidation: [],
      schema: {
        summary: 'get planId use nlp',
        description: 'get planId use nlp api',
        tags: ['whatsapp'],
      },
    },
    getPlanInNlpHandler
  );

  // send appoinment payment
  server.post(
    WHATSAPP.SEND_APPOINMENT_PAYMENT,
    {
      preValidation: [],
      schema: {
        summary: 'send appoinment payment',
        description: 'send appoinment payment api',
        tags: ['whatsapp'],
      },
    },
    sendAppoinmentPaymentHandler
  );

  // upload file for media
  server.post(
    WHATSAPP.FILE_UPLOAD_MEDIA,
    {
      preValidation: [],
      preHandler: cpUpload,
      schema: {
        summary: 'email file upload',
        description: 'email file upload api',
        tags: ['EMAIL'],
      },
    },
    fileUploadController
  );

  // insert meta templete
  server.post(
    WHATSAPP.INSERT_META_TEMPLATE,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'insert mete templete',
        description: 'insert mete templete api',
        tags: ['whatsapp'],
      },
    },
    insertMetaTemplateHandler
  );

  // get template data
  server.get(
    WHATSAPP.GET_META_TEMPLATE,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get meta template',
        description: 'get meta template api',
        tags: ['whatsapp'],
      },
    },
    getMetaTemplateDetailsHandler
  );

  // delete for meta template
  server.delete(
    WHATSAPP.DELETE_META_TEMPLATE,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'delete meta template',
        description: 'delete meta template api',
        tags: ['whatsapp'],
      },
    },
    deleteMetaTemplateHandler
  );
  next();
}
