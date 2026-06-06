import { logger } from '../../../src/plugins/winston';
import { deleteVoice, getByNumber, getCallConfig, getSettingVoiceVideoRecordingdao, insertUpdateVoiceVideoRecordingDao, updateConfig, updateStatus } from '../../dao/callConfig.dao';
import { RESPONSE } from '../../helpers/constants';

export async function saveCallConfigData(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const body = req.body;
    const data: any = {};
    data.domainId = auth.domainId;
    data.name = body.name;
    data.contactCenterNumber = body.contactCenterNumber;
    data.Language = body.Language;
    data.threshold = body.threshold;
    data.isBot = body.isBot;
    data.callFlowId = body.callFlowId;
    data.greetingMessage = body.greetingMessage;
    data.status = body.status;
    data.intents = body.intents;
    data.virtualAgent = body.virtualAgent;
    data.assistPriority = body.assistPriority;
    data.isMultilingualStatus = body?.isMultilingualStatus ?? 0;
    const response: any = await updateConfig(data);
    if (response && response?.length > 0 && response[0]?.errCode === 0) {
      res.status(200).send({
        statusCode: 200,
        message: response[0]?.errMsg,
      });
    } else {
      res.status(200).send({
        statusCode: 424,
        message: RESPONSE.failed,
      });
    }
  } catch (err) {
    console.log('saveCallConfigData err :::::', req?.body, 'error', err);
    logger.error('saveCallConfigData catch', { data: req?.body, Err: err });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function getCallConfigData(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {};
    data.domainId = auth.domainId;
    const response: any = await getCallConfig(data);
    if (response && response?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        callConfigRes: response
      });
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        callConfigRes: []
      });
    }
  } catch (err) {
    console.log('getCallConfigData err :::::', err);
    logger.error('getCallConfigData catch', { Err: err });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function getCallConfigByNum(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {};
    data.domainId = auth.domainId;
    data.contactCenterNumber = req.params.number;
    const response: any = await getByNumber(data);
    if (response && response?.length === 1) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        configRes: response[0]
      });
    } else {
      res.status(200).send({
        statusCode: 424,
        message: RESPONSE.failed,
        configRes: {}
      });
    }
  } catch (err) {
    console.log('getCallConfigByNum err :::::', req.params, 'error', err);
    logger.error('getCallConfigByNum catch', { data: req?.params, Err: err });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function deletevoiceConfig(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {};
    data.domainId = auth.domainId;
    data.contactCenterNumber = req.params.number;
    const response: any = await deleteVoice(data);
    if (response && response?.length > 0 && response[0]?.errCode === 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.notification_delete_status,
      });
    } else {
      res.status(200).send({
        statusCode: 424,
        message: RESPONSE.fail,
      });
    }
  } catch (err) {
    console.log('deletevoiceConfig err :::::', req.params, 'error', err);
    logger.error('deletevoiceConfig catch', { data: req?.params, Err: err });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function updateCallConfigStatus(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const body = req.body;
    const data: any = {};
    data.domainId = auth.domainId;
    data.contactCenterNumber = body.contactCenterNumber;
    data.status = body.status;
    const response: any = await updateStatus(data);
    if (response && response?.length > 0 && response[0]?.errcode === 0) {
      res.status(200).send({
        statusCode: 200,
        message: response[0]?.errmsg,
      });
    } else {
      res.status(200).send({
        statusCode: 424,
        message: RESPONSE.failed,
      });
    }
  } catch (err) {
    console.log('updateCallConfigStatus err :::::', req?.body, 'error', err);
    logger.error('updateCallConfigStatus catch', { data: req?.body, Err: err });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 *
 */
export async function insertUpdateVoiceVideoRecording(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      rid: req?.body?.rid ?? null,
      domainId: auth.domainId,
      isVoice: req?.body?.isVoice ?? 0,
      isInbound: req?.body?.isInbound ?? 0,
      isOutbound: req?.body?.isOutbound ?? 0,
      isVoiceConsentNotice: req?.body?.isVoiceConsentNotice ?? 0,
      isVoiceTextToSpeech: req?.body?.isVoiceTextToSpeech ?? 0,
      voiceText: req?.body?.voiceText ?? null,
      isVoiceSelectPrompt: req?.body?.isVoiceSelectPrompt ?? 0,
      voicePrompt: req?.body?.voicePrompt ?? null,
      isVoiceAgentAccess: req?.body?.isVoiceAgentAccess ?? 0,
      isVideo: req?.body?.isVideo ?? 0,
      isVideoConsentNotice: req?.body?.isVideoConsentNotice ?? 0,
      isVideoTextToSpeech: req?.body?.isVideoTextToSpeech ?? 0,
      videoText: req?.body?.videoText ?? null,
      isVideoSelectPrompt: req?.body?.isVideoSelectPrompt ?? 0,
      videoPrompt: req?.body?.videoPrompt ?? null,
      isVideoAgentAccess: req?.body?.isVideoAgentAccess ?? 0,
      voiceUrl: req?.body?.voiceUrl ?? '',
      videoUrl: req?.body?.videoUrl ?? '',
    };
    const insertResult: any = await insertUpdateVoiceVideoRecordingDao(data);
    if (insertResult && insertResult.length > 0) {
      res.status(200).send({ statusCode: 200, insertResult: insertResult[0] });
    } else {
      res.status(200).send({ statusCode: 424, message: RESPONSE.failed, });
    }
  } catch (err) {
    logger.error('insertUpdateVoiceVideoRecording catch', req?.body, 'error', err);
    console.log('insertUpdateVoiceVideoRecording err :::::', { data: req?.body, Err: err });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 *
 */
export async function getSettingVoiceVideoRecording(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {};
    data.domainId = auth.domainId;
    const response: any = await getSettingVoiceVideoRecordingdao(data);
    if (response && response?.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        callConfigRes: response
      });
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        callConfigRes: []
      });
    }
  } catch (err) {
    console.log('getSettingVoiceVideoRecording err :::::', err);
    logger.error('getSettingVoiceVideoRecording catch', { data: req.headers, Err: err });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}
