import { VIDEO_MEET } from '../../../src/helpers/constants';
import { createAgentMeeting, createAuthToken, deleteSettings, getAgentMeeting, getSettings, getVirtualBackgroundUrls, insertUpdateSettings, insertUpdateVirtualBackgroundUrls, insertVmRecordTranscript, insertVmVerificationAttachments } from './handler';

export default function videomeetservice(server: any, options: any, next: any) {

  server.post(
    VIDEO_MEET.CREATE_AGENT_VIDEOMEET,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: ' post agent meeting ',
        description: 'post agent meeting ',
        tags: ['Video Meet'],
        // response: getUserSchema.response,
      },
    },
    createAgentMeeting
  );

  server.post(
    VIDEO_MEET.INSERT_VIDEO_RECORD_TRANSCRIPT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: ' post record transcript ',
        description: 'post record transcript ',
        tags: ['Video Meet'],
        // response: getUserSchema.response,
      },
    },
    insertVmRecordTranscript
  );

  server.post(
    VIDEO_MEET.INSERT_VM_VERIFICATION_ATTACHMENTS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: ' post vm verification attachments ',
        description: ' post vm verification attachments ',
        tags: ['Video Meet'],
        // response: getUserSchema.response,
      },
    },
    insertVmVerificationAttachments
  );

  server.get(
    VIDEO_MEET.GET_AGENT_MEETING,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: ' post agent meeting ',
        description: 'post agent meeting ',
        tags: ['Video Meet'],
        // response: getUserSchema.response,
      },
    },
    getAgentMeeting
  );

  server.post(
    VIDEO_MEET.CREATE_GUEST_USER_AUTH,
    {
      schema: {
        summary: 'Create Auth token',
        description: 'Create auth token for guest users',
        tags: ['Video Meet'],
        // body: createGuestuserSchema.body,
        // response: createGuestuserSchema.response,
      },
    },
    createAuthToken
  );

  server.get(
    VIDEO_MEET.VIDEO_MEET_SETTINGS,
    {
      schema: {
        summary: 'Get video meet settings',
        description: 'Get video meet settings api',
        tags: ['Video Meet'],
      }
    },
    getSettings
  );

  server.post(
    VIDEO_MEET.VIDEO_MEET_SETTINGS,
    {
      schema: {
        summary: 'Create / update video meet settings',
        description: 'Create / update video meet settings api',
        tags: ['Video Meet'],
      }
    },
    insertUpdateSettings
  );

  server.delete(
    VIDEO_MEET.VIDEO_MEET_SETTINGS,
    {
      schema: {
        summary: 'Delete video meet settings',
        description: 'Delete video meet settings api',
        tags: ['Video Meet'],
      }
    },
    deleteSettings
  );

  server.get(
    VIDEO_MEET.VIRTUAL_BACKGROUND_URLS,
    {
      schema: {
        summary: 'Get virtual background urls',
        description: 'Get virtual background urls api',
        tags: ['Video Meet'],
      }
    },
    getVirtualBackgroundUrls
  );

  server.post(
    VIDEO_MEET.VIRTUAL_BACKGROUND_URLS,
    {
      schema: {
        summary: 'Insert update virtual background urls',
        description: 'Insert update virtual background urls api',
        tags: ['Video Meet'],
      }
    },
    insertUpdateVirtualBackgroundUrls
  );
  next();
}
