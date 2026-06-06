/**
 * @createdBy <krishnakumar>
 * @createdOn
 */

export const FACEBOOK = {
  INSERT_FACEBOOK_IN: '/insert_facebook_in',
  FACEBOOK_OUT: '/facebook_out',
  UPDATE_DISPOSITION_FACEBOOK: '/update_disposition_facebook',
  UPDATE_DISPOSITION_FACEBOOK_CALLBACK: '/update_disposition_facebook_callbacks',
  ASSIGN_CALLBACK_FACEBOOK: '/assign_callback_facebook',
  INSERT_UPDATE_FACEBOOK_CONFIGURATION: '/insert_update_facebook_configuration',
  GET_FACEBOOK_CONFIGURATION: '/get_facebook_configuration',
  GET_FACEBOOK_CONFIGURATION_CALLBACK: '/get_facebook_configuration_callback/:domainId',
  GET_FACEBOOK_CONFIGURATION_DETAILS: '/get_facebook_configuration_details/:fbId',
  DELETE_FACEBOOK_CONFIGURATION_DETAILS: '/delete_facebook_configuration/:fbConfigId',
  UPDATE_STATUS_FACEBOOK: '/update_status_facebook',
  UPDATE_PAGE_ID: '/update_page_id',
  INSERT_FACEBOOK_CALLBACK_STATUS: '/insert_facebook_callback_Status',
  SEND_AGENT_STATUS_QUEUE_FACEBOOK: '/send_agent_status_queue_facebook',
  SEND_APPOINMENT_PAYMENT: '/send_appoinment_payment_fb'
};

export const INSTAGRAM = {
  INSERT_INSTAGRAM_IN: '/insert_instagram_in',
  INSTAGRAM_OUT: '/instagram_out',
  INSERT_UPDATE_INSTAGRAM_CONFIGURATION: '/insert_update_instagram_configuration',
  GET_INSTAGRAM_CONFIGURATION: '/get_instagram_configuration',
  GET_INSTAGRAM_CONFIGURATION_CALLBACK: '/get_instagram_configuration_callback/:domainId',
  GET_INSTAGRAM_CONFIGURATION_DETAILS: '/get_instagram_configuration_details/:instaId',
  DELETE_INSTAGRAM_CONFIGURATION_DETAILS: '/delete_instagram_configuration_details/:instaId',
  UPDATE_STATUS_INSTAGRAM: '/update_status_instagram',
  UPDATE_DISPOSITION_INSTAGRAM: '/update_disposition_instagram',
  UPDATE_DISPOSITION_INSTAGRAM_CALLBACK: '/update_disposition_instagram_callback',
  ASSIGN_CALLBACK_INSTAGRAM: '/assign_callback_instagram',
  UPDATE_INSTAGRAM_PAGE_ID: '/update_instagram_page_id',
  INSERT_INSTAGRAM_CALLBACK_STATUS: '/insert_instagram_callback_status',
  SEND_AGENT_STATUS_QUEUE_INSTAGRAM: '/send_agent_status_queue_instagram',
  SEND_APPOINMENT_PAYMENT: '/send_appoinment_payment_insta'
};

export const RESPONSE = {
  not_found: 'No data found',
  bad_request: 'Bad Request',
  internal_error: 'Internal server error',
  success_message: 'success',
  faild: 'faild',
  disposition_statuss: 'update disposition status successfully',
  update_asssign: 'update assign callback successfully',
  bot_status: 'bot status update successfully',
  Facebook_send: 'facebook send successfully',
  Instagram_send: 'instagram send successfully',
  insertFacebook: 'facebook Configuration inserted successfully',
  updateFacebook: 'facebook Configuration updated successfully',
  facebookConfigId: 'facebook Config details deleted successfully',
  facebookNot: 'facebook Config details not delete',
  status: 'Status enabled successfully',
  statusDisabled: 'Status disabled successfully',
  insertInstagram: 'instagram Configuration inserted successfully',
  updateInstagram: 'instagram Configuration updated successfully',
  instagramConfigId: 'instagram Config details deleted successfully',
  instagramNot: 'instagram Config details not delete',
  facebookPage: 'facebook page updated successfully',
  facebookNotUpdate: 'facebook page not updated',
  instagramPage: 'instagram page updated successfully',
  instagramNotUpdate: 'instagram page not updated',
  emailSent:'Emails sent successfully'
};

export const messageValue = {
  serverMessage: 'socialmedia server started...',
  mysqlError: 'Error creating MySQL pool:',
  mysqlDatabase: 'Database error:',
  mysqlConnect: 'Database connection was closed or reset:',
  mysqlEstab: 'MySQL Connection Established:',
  errorProtocol: 'PROTOCOL_CONNECTION_LOST',
  e: 'ECONNRESET',
  processing: 'Error processing request',
  appendMessage: 'received append message',
  customerInputError: 'Please enter a correct value.',
  customerInputUrl: 'Please enter a valid url.',
  liveAgent: 'Connecting to live agent...',
  agentBusy: 'Our agents are currently not available. Please feel free to leave a message with a brief summary of your inquiry along with your contact information, would like have a callback.',
  disconnectFlow: 'Are you sure you want to discontinue this process and require assistance with something else?',
  continueFlow: 'Would you like to continue the flow?',
  authentication: 'Missing authentication token',
  invalid: 'Invalid credentials in authentication token',
  anotherAgent:'This conversation has been transferred to another agent.',
  endChat: "I'm ending this chat session now. Thanks for contacting us. Please feel free to get back to us for any further queries and assistance! Have a great day! Bye!",
  checkHere:'appoinment payment check here'
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

export const emiiter = {
  sendInvite: 'sendInvite',
  queueWait: 'callQueueWait',
  fallBackQueue: 'changeFallBackQueue',
  receivedInstagram: 'receivedInstagram',
  agentAssistOmniChannel: 'agentAssistOmniChannel',
  supervisiorActivityBot: 'supervisiorActivityBot',
  interActionClosed: 'interActionClosed',
  SentimentScoreAnalytics: 'SentimentScoreAnalytics',
  supervisiorActivity: 'supervisiorActivity',
  receivedFacebook:'receivedFacebook'
};

export const channelType = {
  Instagram: 'Instagram',
  instagram: 'instagram',
  facebook: 'facebook',
  Facebook: 'Facebook'
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
  SelectAnswer: 'Select answer',
  selectDifferent: 'Sorry, no time slots are available for this date. Please select a different date.'
};

export const swagger = {
  jwtsecret: 'sscret',
  uploadDestination: './files',
  fileGetUrl: '/public/',
  routePrefix: 'socialmedia/docs',
  title: 'socialmedia Service',
  description: 'socialmedia Service api documentation',
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

export const CRM_DOMAINS = {
  LORIO: '19969'
};
