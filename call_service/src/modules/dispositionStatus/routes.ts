import { DISPOSITION } from '../../helpers/constants';
import {
  aisummarySubjectHandler,
  // autoAssignHandler,
  deleteDispositionStatusHandler,
  deleteVoiceRecordingsHandler,
  getDispositinStatusHandler,
  getFilterMobileApp,
  getVideoMeetRecordInfoHandler,
  getVideoSessionDetails,
  getVoiceRecordInfoHandler,
  getVoiceSessionDetails,
  insertCompanyPurchasedAddonHandler,
  insertDispositionStatusHandler,
  isValidDispositionNameHandler,
  regeneratedispositionHandler,
  updateCalledPersonDetailsCallHandler,
  updateDispositionBeforeUnloadHandler,
  updateDispositionHandler,
  updateDispositionStatusHandler,
  updateDispositionStatusVideoHandler
} from './handler';
import {
  getDispositinStatusSchema,
  insertDispositinStatusSchema,
  isValidSchema,
  updateCalledPersonDetailsCallSchema,
  updateDispositionSchema
} from './schema';

export default function dispositionStatusHandler(server: any, options: any, next: any) {
  // insert agent status
  server.post(
    DISPOSITION.INSERT_DISPOSITION_STATUS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'insert disposition status',
        description: 'insert disposition status api',
        tags: ['disposition'],
        body: insertDispositinStatusSchema.body,
        response: insertDispositinStatusSchema.response,
      },
    },
    insertDispositionStatusHandler
  );

  // get dispositin status
  server.get(
    DISPOSITION.GET_DISPOSITION_STATUS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get disposition status',
        description: 'get disposition status api',
        tags: ['disposition'],
        response: getDispositinStatusSchema.response,
      },
    },
    getDispositinStatusHandler
  );

  // update dispositin status
  server.put(
    `${DISPOSITION.UPDATE_DISPOSITION_STATUS}/:did`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update disposition status',
        description: 'update disposition status api',
        tags: ['disposition'],
        params: updateDispositionSchema.params,
        body: updateDispositionSchema.body,
        response: updateDispositionSchema.response,
      },
    },
    updateDispositionStatusHandler
  );

  // delete disposition status
  server.delete(
    `${DISPOSITION.DELETE_DISPOSITION_STATUS}/:did`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'delete disposition status',
        description: 'delete disposition status api',
        tags: ['disposition'],
        // params: deleteDispositionSchema.params,
        // response: deleteDispositionSchema.response,
      },
    },
    deleteDispositionStatusHandler
  );

  // is valid disposition name
  server.post(
    DISPOSITION.IS_VALID_DISPOSITION_NAME,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'valid disposition',
        description: ' valid disposition api',
        tags: ['disposition'],
        body: isValidSchema.body,
        response: isValidSchema.response,
      },
    },
    isValidDispositionNameHandler
  );
  // update dispositin status
  server.put(
    `${DISPOSITION.UPDATE_DISPOSITION}/:sessionId`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update disposition ',
        description: 'update disposition  api',
        tags: ['disposition'],
      },
    },
    updateDispositionHandler
  );

  // Auto assign
/*   server.post(
    DISPOSITION.AUTO_ASSIGN,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Auto assign',
        description: 'Auto assign  api',
        tags: ['disposition'],
      },
    },
    autoAssignHandler
  ); */

  // update calledPersonDetails
  server.put(
    `${DISPOSITION.UPDATE_CALLED_PERSON_DETAILS}/:sessionId`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update calledPersonDetails ',
        description: 'update calledPersonDetails  api',
        tags: ['disposition'],
        params: updateCalledPersonDetailsCallSchema.params,
        body: updateCalledPersonDetailsCallSchema.body,
        response: updateCalledPersonDetailsCallSchema.response,
      },
    },
    updateCalledPersonDetailsCallHandler
  );

  server.get(
    DISPOSITION.GET_DISPOSITION_FILTER_LIST,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get advanced filter data',
        description: 'Get advanced filter data api',
        tags: ['disposition'],
      },
    },
    getFilterMobileApp
  );

  // insert company purchase addon
  server.post(
    DISPOSITION.ONBOARD_COMPANY_PURCHASE_ADDON,
    {
      preValidation: [],
      schema: {
        summary: 'onboard purchase addon',
        description: 'onboard purchase addon api',
        tags: ['user'],
      },
    },
    insertCompanyPurchasedAddonHandler
  );

  // update dispositin status video
  server.post(
    DISPOSITION.UPDATE_DISPOSITION_VIDEO,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update disposition status video',
        description: 'update disposition status video api',
        tags: ['disposition_video'],
        // body: updateDispositionSchema.body,
        // response: updateDispositionSchema.response,
      },
    },
    updateDispositionStatusVideoHandler
  );

  // Get video Meet Record Info
  server.post(
    DISPOSITION.GET_VIDEOMEET_RECORD_INFO,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get disposition status video',
        description: 'get disposition status video api',
        tags: ['Recording'],
      },
    },
    getVideoMeetRecordInfoHandler
  );

  // Get Voice Meet Record Info
  server.post(
    DISPOSITION.GET_VOICE_RECORD_INFO,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get disposition status video',
        description: 'get disposition status video api',
        tags: ['Recording'],
      },
    },
    getVoiceRecordInfoHandler
  );

  // Get video session details
  server.get(
    DISPOSITION.GET_VIDEO_SESSION_DETAILS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get video session details api',
        description: 'Get video session details api',
        tags: ['Recording'],
      },
    },
    getVideoSessionDetails
  );

  // Get voice session details
  server.get(
    DISPOSITION.GET_VOICE_SESSION_DETAILS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get voice session details api',
        description: 'Get voice session details api',
        tags: ['Recording'],
      },
    },
    getVoiceSessionDetails
  );

  // update disposition before reloading
  server.post(
    DISPOSITION.UPDATE_DISPOSITION_BEFOREUNLOAD,
    {
      // preValidation: [server.validateSession],
      schema: {
        summary: 'update disposition before reloading',
        description: 'update disposition before reloading api',
        tags: ['Disposition'],
      },
    },
    updateDispositionBeforeUnloadHandler
  );

  server.post(
    DISPOSITION.AI_SUMMARY_SUBJECT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Ai summary subject ',
        description: 'Ai summary subject api',
        tags: ['disposition'],
      },
    },
    aisummarySubjectHandler
  );

  server.post(
    DISPOSITION.REGENERATE_DISPOSITION,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Regenerate ',
        description: 'Regenerate api',
        tags: ['disposition'],
      },
    },
    regeneratedispositionHandler
  );

    // delete disposition status
  server.post(
    DISPOSITION.DELETE_VOICE_RECORDINGS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'delete voice recordings',
        description: 'delete voice recordings api',
        tags: ['voice recordings'],
          // params: deleteDispositionSchema.params,
          // response: deleteDispositionSchema.response,
      },
    },
    deleteVoiceRecordingsHandler
    );

  next();
}
