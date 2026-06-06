/**
 * @createdBy <krishna>
 * @createdOn
 */

export const WHATSAPP = {
  INSERT_WHATSAPP_IN: '/insert_whatsapp_in',
  UPDATE_DISPOSITION_WHATSAPP: '/update_disposition_whatsapp',
  UPDATE_DISPOSITION_WHATSAPP_CALLBACK: '/update_disposition_whatsapp_callback',
  ASSIGN_CALLBACK_WHATSAPP: '/assign_callback_whatsapp',
  WHATSAPP_SMS_OUT: '/whatsappp_sms_out',
  UPDATE_WHATSAPP_BOT_STATUS: '/update_whatsapp_bot_status',
  GET_DID_PURCHASE_WHATSAPP: '/get_did_purchase_whatsapp',
  UPDATE_DIDNUMBER_PURCHASE_WHATSAPP: '/udpate_didnumber_purchase_whatsapp',
  INSERT_UPDATE_WHATSAPP_CONFIGURATION: '/insert_update_whatsapp_configuration',
  GET_WHATSAPP_CONFIGURATION: '/get_whatsapp_configuration',
  GET_WHATSAPP_CONFIGURATION_CALLBACK: '/get_whatsapp_configuration_callback/:domainId',
  GET_WHATSAPP_CONFIGURATION_DETAILS: '/get_whisapp_configuration_details/:whatsappId',
  UPDATE_DISABLED_CONFIGURATION: '/update_disabled_configuration/:whatsappIds',
  GET_WHATSAPP_CHANNEL_DETAILS: '/whatsapp_channel_details',
  INSERT_WHATSAPP_TEMPLATE: '/insert_whatsapp_template',
  INSERT_ONBOARD_WHATSAPP_CONFIGURATION: '/insert_onboard_whatsapp_configuration',
  GET_ECOM_PURCHASEFLOW: '/get_ecom_purchaseFlow/:domainId/:tokenId',
  UPDATE_ECOM_PURCHASEFLOW: '/update_ecom_purchaseFlow',
  UPDATE_ECOM_ISPURCHASE: '/update_ecom_isPurchase',
  UPDATE_DISPOSITION_WHATSAPP_PURCHASEFLOW: '/update_disposition_whatsapp_purchaseflow',
  INSERT_UPDATE_META_WHATSAPP: '/insert_update_meta_whatsapp',
  GET_WHATSAPP_PROVIDER: '/get_whatsapp_provider/:whatsappNumber',
  GET_DISPOSITION_STATUS_INFO: '/get_disposition_status_info/:domainId',
  INSERT_WHATSAPP_CALLBACK_STATUS: '/insert_whatsapp_callback_status',
  DELETE_CUSTOM_INPUT_TEMPLATE: '/delete_custom_input_template/:flowId',
  GET_TEMPLATE_DETAILS: '/get_template_details/:flowId',
  SEND_AGENT_STATUS_QUEUE: '/send_agent_status_queue',
  GET_PLANDID_USE_NLP: '/get_planId_use_nlp',
  SEND_APPOINMENT_PAYMENT: '/send_appoinment_payment',
  FILE_UPLOAD_MEDIA: '/file_upload_media',
  INSERT_META_TEMPLATE: '/insert_meta_template',
  GET_META_TEMPLATE: '/get_meta_template/:flowId',
  DELETE_META_TEMPLATE: '/delete_meta_template/:customInputId',
  DELETE_WHATSAPP_CONFIGURATION: '/delete_whatsapp_configuration/:whatsappId'
};

export const RESPONSE = {
  not_found: 'No data found',
  bad_request: 'Bad Request',
  internal_error: 'Internal server error',
  not_update: 'not update',
  success_message: 'success',
  deleteFaild: 'delete failed',
  faild: 'faild',
  Disabled: 'ID cannot be disabled',
  IdDisabled: 'Disabled successfully',
  name_already_exist: 'name already exist',
  email_already_exist: 'email already exist',
  whatsapp_failure: 'message not sent',
  whatsapp_DB_failure: 'message recived not inserted in Data Base',
  bot_status: 'bot status update successfully',
  insertWhatsapp: 'Whatsapp Configuration inserted successfully',
  updateWhatsapp: 'Whatsapp Configuration updated successfully',
  isDisabled: 'Disable successfully',
  disposition_statuss: 'update disposition status successfully',
  update_asssign: 'update assign callback successfully',
  insertWhatsapptemplates: 'whatsapp templates inserted successfully',
  updateWhatsapptemplates: 'whatsapp templates updated successfully',
  numberUpdate: 'whatsapp Update Number updated successfully',
  numberNotUpdate: 'whatsapp Update number not updated.',
  update: 'updated successfully',
  delete_template: 'delete template successfully',
  serverError: 'Error starting server:',
  serverMessage: 'WhatsApp server started at',
  mysqlError: 'Error creating MySQL pool:',
  mysqlDatabase: 'Database error:',
  mysqlConnect: 'Database connection was closed or reset:',
  mysqlEstab: 'MySQL Connection Established:',
  errorProtocol: 'PROTOCOL_CONNECTION_LOST',
  e: 'ECONNRESET',
  appendMessage: 'received Whatsapp append message',
  slots: 'Sorry, no time slots are available for this date. Please select a different date.',
  customerInputError: 'Please enter a correct value.',
  customerInputUrl: 'Please enter a valid url.',
  sendEmail: 'Emails sent successfully'
};

export const ROLES = {
  OWNER_ROLE_ID: '1',
  SUB_OWNER_ROLE_ID: '2',
  ADMIN_ROLE_ID: '3',
  MEMBER_ROLE_ID: '4',
  SUPERVISOR_ROLE_ID: '5',
  AGENT_ROLE_ID: '6',
};

export const channelTypeCode = {
  Call: 2003001,
  Chat: 2003002,
  Email: 2003003,
  SMS: 2003004,
  Whatsapp: 2003005,
  Facebook: 2003006,
  Instagram: 2003007
};

export const channelType = {
  Whatsapp: 'Whatsapp'
};

export const emitter = {
  invite: 'sendInvite',
  queueWait: 'callQueueWait',
  fallBackQueue: 'changeFallBackQueue',
  emitterDispStatus: 'emitterDispStatus',
  interActionClosed: 'interActionClosed',
  SentimentScoreAnalytics: 'SentimentScoreAnalytics',
  receivedWhatsapp: 'receivedWhatsapp',
  agentAssistOmniChannel: 'agentAssistOmniChannel',
  supervisiorActivityBot: 'supervisiorActivityBot',
  supervisiorActivity: 'supervisiorActivity'
};

export const messageValue = {
  liveAgent: 'Connecting to live agent...',
  agentBusy: 'Our agents are currently not available. Please feel free to leave a message with a brief summary of your inquiry along with your contact information, would like have a callback.',
  disconnectFlow: 'Are you sure you want to discontinue this process and require assistance with something else?',
  continueFlow: 'Would you like to continue the flow?',
  authentication: 'Missing authentication token',
  invalid: 'Invalid credentials in authentication token',
  looksLike: "Oops! Looks like the item you're searching for couldn't be found.",
  transferred: 'This conversation has been transferred to another agent.',
  endChat: "I'm ending this chat session now. Thanks for contacting us. Please feel free to get back to us for any further queries and assistance! Have a great day! Bye!",
  interested: 'Choose the service you’re interested in to move ahead',
  processing:'Error processing request'
};

export const appoinment = {
  category: 'Select the category that suits your needs to proceed!',
  SelectCategory: 'Select the Category',
  date: 'Pick a date that works best for you. Click below to choose your preferred appointment date!',
  SelectDate: 'Select date',
  duration: 'Select the duration that works best for your appointment. Click below to choose!',
  booking: 'Pick a slot to confirm your booking.',
  consultant: 'Click below to select a preferable consultant',
  SelectConsultant: 'Select Consultant',
  SelectAnswer: 'Select answer'
};

export const swagger = {
  jwtsecret: 'sscret',
  uploadDestination: './files',
  fileGetUrl: '/public/',
  routePrefix: 'whatsapp/docs',
  title: 'whatsapp Service',
  description: 'whatsapp Service api documentation',
  version: '0.1.0',
  descriptionStage: 'staging',
  descriptionLocal: 'development',
  descriptionProduction: 'production',
  descriptionQa: 'qa',
  apiKeyDes: 'Standard Authorization header using the Key and Value scheme. Example: "key" : "authorization", "value" : "{token}"',
  type: 'apiKey',
  name: 'Authorization',
  in: 'header',
  origin: '*',
  methods: 'GET,HEAD,PUT,PATCH,POST,DELETE',
};
