import { cpUpload } from '../../config/upload';
import { EMAIL, EMAIL_CONSTANT } from '../../helpers/constants';
import {
  assignCallBackHandlers,
  createCrmEmailConfigHandler,
  createEmailConfiguration,
  createEmailOnboarding,
  createMailboxHandler,
  deleteDefaultEmailHandler,
  deleteDomainDetailsHandler,
  deleteEmailAddress,
  deleteTicketingEmailConfig,
  fileUploadController,
  getAgentAssistHandler,
  getDefaultEmailHandler,
  getDkimGenerateKeyHandler,
  getDomainAlreayExistHandler,
  getDomainConfigurationDetailsHandler,
  getDomainConfigurationListHandler,
  getEmailConfiguration,
  getEmailFlowHistoryListHandler,
  getEmailHistoryHandler,
  getSupervisiorListHandler,
  getVerificationDomainListHandler,
  graphWebhookHandler,
  insertDomainConfigurationHandler,
  insertTicketingEmailConfig,
  insertUnsubscribeDetailsHandler,
  onbordGetEmailConfiguration,
  onbordUpdateEmailBotStatus,
  postDefaultEmailHandler,
  sendEmailAudracareHandler,
  sendEmailBotHandler,
  sendEmailHandler,
  sendEmailOutboundCompanyHandler,
  sendEmailOutboundHandler,
  sendEmailPlainTextHandler,
  sendEmailSessionExpiryHandler,
  sendEmailTemplateHandler,
  sendMailForLeadGeneration,
  sendMailRouteMobileHandler,
  setPrimaryEmailAddress,
  updateDispositionEmailHandler,
  updateDomainVerificationHandler,
  updateEmailBotStatus,
  updateReplyViaDispositionEmailHandler,
} from './handler';
import { sendEmailSchema } from './schema';

export default function emailHandler(server: any, options: any, next: any) {
  // create email congif
  server.post(
    EMAIL.CREATE_EMAIL_CONGIF,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'create email config',
        description: 'create email config api',
        tags: ['email'],
      },
    },
    createEmailConfiguration
  );

  // create email congif
  server.post(
    EMAIL.ONBORD_CREATE_EMAIL,
    {
      // preValidation: [],
      schema: {
        summary: 'onbord create email config',
        description: 'onbord create email config api',
        tags: ['email'],
      },
    },
    createEmailOnboarding
  );

  // GET_EMAIL_CONFIG,
  server.get(
    EMAIL.GET_EMAIL_CONFIG,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get  email config',
        description: 'get email config api',
        tags: ['email'],
      },
    },
    getEmailConfiguration
  );

  // GET_EMAIL_CONFIG,
  server.post(
    EMAIL.ONBORD_GET_EMAIL_CONFIG,
    {
      // preValidation: [],
      schema: {
        summary: 'onbord get  email config',
        description: 'onbord get email config api',
        tags: ['email'],
      },
    },
    onbordGetEmailConfiguration
  );

  // get email channel
  server.get(
    EMAIL_CONSTANT.GET_EMAIL_CHANNEL,
    {

      preValidation: [server.validateSession],
      schema: {
        description: 'get Email Channel',
        tags: ['EMAIL']
      }
    },
    getDefaultEmailHandler
  );

  // post email channel config
  server.post(
    EMAIL_CONSTANT.POST_EMAIL_CHANNEL,
    {
      preValidation: [server.validateSession],
      schema: {
        description: 'post email channel',
        tags: ['EMAIL']
      }
    },
    postDefaultEmailHandler
  );

  // delete email list
  server.delete(
    `${EMAIL_CONSTANT.DELETE_EMAIL_LIST}/:uid`,
    {
      preValidation: [server.validateSession],
      schema: {
        description: 'delete Email',
        tags: ['EMAIL']
      }
    },
    deleteDefaultEmailHandler
  );

  // delete email address
  server.delete(
    `${EMAIL.DELETE_EMAIL_ADDRESS}/:EmailConfigId`,
    {
      preValidation: [server.validateSession],
      schema: {
        description: 'delete email address',
        tags: ['EMAIL']
      }
    },
    deleteEmailAddress
  );

  // update primary email
  server.put(
    EMAIL.UPDATE_PRIMARY_EMAIL,
    {
      preValidation: [server.validateSession],
      schema: {
        description: 'set primary email address',
        tags: ['EMAIL']
      }
    },
    setPrimaryEmailAddress
  );

  // send email
  server.post(
    EMAIL.SEND_EMAIL,
    {
      // preValidation: [server.validateSession],
      schema: {
        summary: 'send email',
        description: 'send a new email',
        tags: ['email'],
        // body: sendEmailSchema.body,
        // response: sendEmailSchema.response,
      },
    },
    sendEmailHandler
  );

  // send email outbound
  server.post(
    EMAIL.SEND_EMAIL_OUTBOUND,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'send email outbound',
        description: 'send email outbound',
        tags: ['email'],
      },
    },
    sendEmailOutboundHandler
  );

  // send email outbound company
  server.post(
    EMAIL.SEND_EMAIL_OUTBOUND_COMPANY,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'send email outbound company',
        description: 'send email outbound company',
        tags: ['email'],
      },
    },
    sendEmailOutboundCompanyHandler
  );

  // send email bot
  server.post(
    EMAIL.SEND_EMAIL_BOT,
    {
      // preValidation: [server.validateSession],
      schema: {
        summary: 'send email bot',
        description: 'send a new email bot',
        tags: ['email'],
      },
    },
    sendEmailBotHandler
  );

  // send email bot for audracare
  server.post(
    EMAIL.SEND_EMAIL_AUDRACARE,
    {
      // preValidation: [server.validateSession],
      schema: {
        summary: 'send email audracare',
        description: 'send a new email audracare',
        tags: ['email'],
      },
    },
    sendEmailAudracareHandler
  );

  // send email template
  server.post(
    EMAIL.SEND_EMAIL_TEMPLATE,
    {
      // preValidation: [server.validateSession],
      schema: {
        summary: 'send email template',
        description: 'send a new email template',
        tags: ['email'],
      },
    },
    sendEmailTemplateHandler
  );

  // get supervisior list
  server.get(
    EMAIL.GET_SUPERVISIOR_LIST,
    {
      preValidation: [server.validateSession],
      schema: {
        description: 'get supervisior list by domainId',
        tags: ['EMAIL']
      }
    },
    getSupervisiorListHandler
  );
  // get email Email History
  server.post(
    EMAIL.GET_EMAIL_HISTORY_DETAILS,
    {
      preValidation: [server.validateSession],
      schema: {
        description: 'get_email_history_details',
        tags: ['EMAIL']
      }
    },
    getEmailHistoryHandler
  );

  // EMAIL FILE UPLOADING
  server.post(
    EMAIL.FILE_UPLOAD,
    {
      preValidation: [server.validateSession],
      preHandler: cpUpload,
      schema: {
        summary: 'email file upload',
        description: 'email file upload api',
        tags: ['EMAIL'],
      },
    },
    fileUploadController
  );

  // update dispositin status
  server.put(
    `${EMAIL.UPDATE_DISPOSITION}/:EmailHistoryId`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update disposition email',
        description: 'update disposition email api',
        tags: ['EMAIL'],
      },
    },
    updateDispositionEmailHandler
  );

  // update bot status
  server.post(
    EMAIL.UPDATE_EMAIL_BOT,
    {
      preValidation: [],
      schema: {
        description: 'update email bot status',
        tags: ['EMAIL']
      }
    },
    updateEmailBotStatus
  );

  // onbord update bot status
  server.post(
    EMAIL.ONBORD_UPDATE_EMAIL_BOT,
    {
      preValidation: [],
      schema: {
        description: 'onbord update email bot status',
        tags: ['EMAIL']
      }
    },
    onbordUpdateEmailBotStatus
  );

  // get agent assist information
  server.post(
    EMAIL.GET_AGENT_ASSIST_INFO,
    {
      preValidation: [server.validateSession],
      schema: {
        description: 'get agent assist information',
        tags: ['EMAIL']
      }
    },
    getAgentAssistHandler
  );

  // assign callback
  server.post(
    EMAIL.ASSIGN_CALLBACK_EMAIL,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update assign callback',
        description: 'update assign callback api',
        tags: ['IVR'],
      },
    },
    assignCallBackHandlers
  );
  // send mail for lead generation
  server.post(
    EMAIL.SEND_MAIL_LEAD,
    {
      // preValidation: [server.validateSession],
      schema: {
        summary: 'send mail for lead generation',
        description: 'send mail for lead generation api',
        tags: ['Email'],
      },
    },
    sendMailForLeadGeneration
  );

  // get email callFlow history
  server.get(
    EMAIL.GET_EMAIL_FLOW_HISTORY,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get email callFlow history',
        description: 'get email callFlow history api',
        tags: ['Email'],
      },
    },
    getEmailFlowHistoryListHandler
  );

  // send route mobile email
  server.post(
    EMAIL.SEND_MAIL__ROUTE_MOBILE_WHATSAPP,
    {
      // preValidation: [server.validateSession],
      schema: {
        summary: 'send mail for route mobile',
        description: 'send mail for route mobile api',
        tags: ['Email'],
      },
    },
    sendMailRouteMobileHandler
  );

  // create crm email
  server.post(
    EMAIL.CREATE_CRM_EMAIL_CONFIG,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'create crm email config',
        description: 'create crm email config api',
        tags: ['Email'],
      },
    },
    createCrmEmailConfigHandler
  );

  // insert domain configuration
  server.post(
    EMAIL.INSERT_DOMAIN_CONFIGURATION,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'insert domain configuration',
        description: 'insert domain configuration api',
        tags: ['Email'],
      },
    },
    insertDomainConfigurationHandler
  );

  // get domain configuration list
  server.get(
    EMAIL.GET_DOMAIN_CONFIGURATION_LIST,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get domain configuration list',
        description: 'get domain configuration list api',
        tags: ['Email'],
      },
    },
    getDomainConfigurationListHandler
  );

  // get dkim generate key
  server.get(
    EMAIL.GET_DKIM_GENERATE_KEY,
    {
      preValidation: [],
      schema: {
        summary: 'get dkim generate key',
        description: 'get dkim generate key api',
        tags: ['Email'],
      },
    },
    getDkimGenerateKeyHandler
  );

  // delete domain details
  server.delete(
    EMAIL.DELETE_DOMAIN_DETAILS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'delete domain details',
        description: 'delete domain details api',
        tags: ['Email']
      },
    },
    deleteDomainDetailsHandler
  );

  // get domain configuration details
  server.get(
    EMAIL.GET_DOMAIN_CONFIGURATION_DETAILS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get domain configuration details',
        description: 'get domain configuration details api',
        tags: ['Email'],
      },
    },
    getDomainConfigurationDetailsHandler
  );

  // get domain name already exists
  server.get(
    EMAIL.GET_DOMAIN_ALREADY_EXIST,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get domain name already exists',
        description: 'get domain name already exists api',
        tags: ['Email'],
      },
    },
    getDomainAlreayExistHandler
  );

  // update domain verfication
  server.post(
    EMAIL.UPDATE_DOMAIN_VERIFICATION,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update domain verfication',
        description: 'update domain verfication api',
        tags: ['Email'],
      },
    },
    updateDomainVerificationHandler
  );

  // get verification domain list
  server.get(
    EMAIL.GET_VERIFICATION_DOMAIN_LIST,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get verification domain list',
        description: 'get verification domain list api',
        tags: ['Email'],
      },
    },
    getVerificationDomainListHandler
  );

  // create mailbox
  server.post(
    EMAIL.CREATE_MAILBOX,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'create mailbox',
        description: 'create mailbox api',
        tags: ['Email'],
      },
    },
    createMailboxHandler
  );
  server.put(
    EMAIL.UPDATE_REPLYVIA_DISPOSITION_EMAIL,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update disposition email',
        description: 'update disposition email api',
        tags: ['EMAIL'],
      },
    },
    updateReplyViaDispositionEmailHandler
  );

  // insert Unsubscribe details
  server.post(
    EMAIL.INSERT_UNSUBSCRIBE_DETAILS,
    {
      // preValidation: [server.validateSession],
      schema: {
        summary: 'insert unsubscribe details',
        description: 'insert unsubscribe details api',
        tags: ['Email'],
      },
    },
    insertUnsubscribeDetailsHandler
  );

  // insert_ticket_email_config
  server.post(
    EMAIL.INSERT_TICKETING_EMAIL_CONFIG,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'insert ticket email config',
        description: 'insert ticket email config api',
        tags: ['Email'],
      },
    },
    insertTicketingEmailConfig
  );

  // delete ticket email config
  server.post(
    EMAIL.DELETE_TICKET_EMAIL_CONFIG,
    {
      // preValidation: [server.validateSession],
      schema: {
        summary: 'delete ticket email config',
        description: 'delete ticket email config api',
        tags: ['Email'],
      },
    },
    deleteTicketingEmailConfig
  );

    // send email for session expiry
  server.post(
    EMAIL.SEND_EMAIL_SESSION_EXPIRY,
    {
      // preValidation: [server.validateSession],
      schema: {
        summary: 'send email session expiry',
        description: 'send email session expiry api',
        tags: ['email'],
      },
    },
    sendEmailSessionExpiryHandler
  );

  // send email plain text
  server.post(
    EMAIL.SEND_EMAIL_PLAINTEXT,
    {
      // preValidation: [server.validateSession],
      schema: {
        summary: 'send email plain text',
        description: 'send a new email plain text',
        tags: ['email'],
      },
    },
    sendEmailPlainTextHandler
  );

  // fetching inbox email - POST for notifications
  server.post(
    EMAIL.FETCHING_INBOX_EMAIL_CCAAS,
    {
      // preValidation: [server.validateSession],
      schema: {
        summary: 'fetching inbox email ccaas',
        description: 'fetching inbox email ccaas',
        tags: ['email'],
      },
    },
    graphWebhookHandler
  );

  // Microsoft Graph sends validation requests as GET with validationToken query param
  server.get(
    EMAIL.FETCHING_INBOX_EMAIL_CCAAS,
    {
      schema: {
        summary: 'MS Graph webhook validation',
        description: 'Handles Microsoft Graph subscription validation requests',
        tags: ['email'],
      },
    },
    graphWebhookHandler
  );

  next();
}
