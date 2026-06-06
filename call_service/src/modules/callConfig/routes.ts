import { CALL_CONFIG } from '../../helpers/constants';
import { deletevoiceConfig, getCallConfigByNum, getCallConfigData, getSettingVoiceVideoRecording, insertUpdateVoiceVideoRecording, saveCallConfigData, updateCallConfigStatus } from './handler';

export default function callConfighandler(server: any, options: any, next: any) {
  server.post(
    CALL_CONFIG.INSERT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Insert api',
        description: 'Insert and update api',
        tags: ['Voice'],
      },
    },
    saveCallConfigData
  );
  server.get(
    CALL_CONFIG.GET_CALL_CONFIG,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get call config',
        description: 'Get call config api',
        tags: ['Voice'],
      },
    },
    getCallConfigData
  );
  server.get(
    CALL_CONFIG.GET_BY_NUMBER,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get call config by number',
        description: 'Get call config by number api',
        tags: ['Voice'],
      },
    },
    getCallConfigByNum
  );
  server.delete(
    CALL_CONFIG.DELETE_BY_NUMBER,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Delete call config by number',
        description: 'Delete call config by number api',
        tags: ['Voice'],
      },
    },
    deletevoiceConfig
  );
  server.post(
    CALL_CONFIG.UPDATE_STATUS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Update status',
        description: 'Update status api',
        tags: ['Voice'],
      },
    },
    updateCallConfigStatus
  );
  server.post(
    CALL_CONFIG.INSERT_UPDATE_VOICE_VIDEO_RECORDING,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Update status',
        description: 'Update status api',
        tags: ['Voice_video'],
      },
    },
    insertUpdateVoiceVideoRecording
  );
  server.get(
    CALL_CONFIG.GET_SETTING_VOICE_VIDEO_RECORDING,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get setting voice video recording',
        description: 'Get setting voice video recording api',
        tags: ['Voice_video'],
      },
    },
    getSettingVoiceVideoRecording
  );
  next();
}
