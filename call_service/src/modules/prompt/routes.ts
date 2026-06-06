/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

import { cpUpload, cpUploaded } from '../../config/upload';
import { PROMPT } from '../../helpers/constants';
import {
  chatFileUpload,
  chatUploadFile,
  deletePromptAudioHandler,
  getAudioPromptHandler,
  insertAudioPromptHandler,
  isValidPromptNameHandler,
  previewTextToSpeechHandler,
  updatePromptAudioHandler
} from './handler';
import { isValidSchema } from './schema';

export default function prompthandler(server: any, options: any, next: any) {
  server.post(
    PROMPT.FILEUPLOAD,
    {
      preValidation: [server.validateSession],
      preHandler: cpUpload,
      schema: {
        description: 'file upload',
        tags: ['PROMPT'],
        // response: fileUploadRes,
      },
    },
    chatFileUpload,
  );
  server.post(
    PROMPT.UPLOADFILE,
    {
      preValidation: [server.validateSession],
      preHandler: cpUploaded,
      schema: {
        description: 'file upload',
        tags: ['PROMPT'],
        // response: fileUploadRes,
      },
    },
    chatUploadFile,
  );

  // insert Prompt Audio
  server.post(
    PROMPT.INSERT_AUDIO_PROMPT,
    {
      preValidation: [server.validateSession],
      preHandler: cpUploaded,
      schema: {
        description: 'text to speech',
        tags: ['PROMPT'],
      },
    },
    insertAudioPromptHandler
  );

  // get Prompt Audio Library
  server.get(
    PROMPT.GET_PROMPT_LIBRAR,
    {
      preValidation: [server.validateSession],
      schema: {
        description: 'get prompt audio library',
        tags: ['PROMPT'],
      },
    },
    getAudioPromptHandler
  );

  // update Prompt Audio Library
  server.put(
    `${PROMPT.UPDATE_PROMPT_AUDIO}/:uuid`,
    {
      preValidation: [server.validateSession],
      preHandler: cpUpload,
      schema: {
        description: 'update prompt audio library',
        tags: ['PROMPT'],
      },
    },
    updatePromptAudioHandler
  );
  // delete prompt audio library
  server.delete(
    `${PROMPT.DELETE_PROMPT_AUDIO}/:uuid`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'delete prompt',
        description: 'delete prompt api',
        tags: ['PROMPT']
      }
    },
    deletePromptAudioHandler
  );

  // is valid prompt name
  server.post(
    PROMPT.IS_VALID_PROMPT_NAME,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'valid prompt ',
        description: ' valid prompt api',
        tags: ['PROMPT'],
        body: isValidSchema.body,
        response: isValidSchema.response,
      },
    },
    isValidPromptNameHandler
  );

  server.post(
    PROMPT.GET_PREVIEW_AUDIO,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'preview audio',
        description: 'get preview audio',
        tags: ['PROMPT'],
      },
    },
    previewTextToSpeechHandler
  );

  next();
}
