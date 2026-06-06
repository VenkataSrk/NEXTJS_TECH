import moment from 'moment';
import { mysqlPoolConnection } from '../plugins/db';
import { logger } from '../plugins/winston';
import VoiceEngine from '../service/testToSpeech';

/**
 *
 * @param data
 * @returns
 */
export const chatFilePath = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      const currentDate = moment(new Date()).format('YYYY-MM-DD HH:mm:ss');
      const chat = `INSERT INTO chatMessages (fieldName,encoding,mimetype,fileName,path,size,uuid,url,name,domainId,message,createdAt) VALUES ('${data?.fieldname}','${data?.encoding}','${data?.mimetype}','${data?.filename}','${data?.path}',${data?.size},'${data?.uuid}','${data?.url}','${data?.name}',${data?.domainId},'${data.message}','${currentDate}')`;
      mysqlPoolConnection.query(chat, (err, result) => {
        if (err) {
          reject(err);
        } else {
          resolve(result);
        }
      });
    } catch (error) {
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @returns Convert the Text to Speech
 */
export const textToConvert = async (data: any) => {
  try {
    const voiceEngine = new VoiceEngine();
    const fileInfo: any = await voiceEngine.convertInToSpeech(data);
    const fileData: any = {
      message: data?.text ?? null,
      path: fileInfo?.nfsPath ?? null,
      url: fileInfo?.filePath ?? null,
      fileName: fileInfo?.fileName ?? null,
      filename: fileInfo?.fileName ?? null,
      domainId: data?.domainId ?? 0,
      name: data?.name ?? null,
      type: data?.type ?? null,
      promptStatus: data?.promptStatus ?? null,
      description: data?.description ?? null,
      promptType: data?.promptType ?? 1,
      fieldname: data?.fieldname ?? null,
      encoding: data?.encoding ?? null,
      mimetype: data?.mimetype ?? null,
      size: data.size ?? 0,
      companyId: data.companyId ?? 0,
    };
    const cid: any = await insertPromptLibrary(fileData);
    fileData.insertId = cid.insertId;
    return { fileData };
  } catch (error) {
    logger.error('textToConvert', { data, Err: error });
  }
};

/**
 *
 * @param data
 * @returns Convert the Text to Speech
 */
export const textToConvertPreview = async (data: any) => {
  try {
    const voiceEngine = new VoiceEngine();
    const fileInfo: any = await voiceEngine.convertInToSpeech(data);
    const fileData: any = {
      message: data?.text ?? null,
      path: fileInfo?.filePath ?? null,
    };
    return { fileData };
  } catch (error) {
    logger.error('textToConvertPreview', { data, Err: error });
  }
};

/**
 *
 * @param data
 * @returns
 */
export const editTextToSpeech = async (data: any) => {
  try {
    const uuid = data.uuid;
    const voiceEngine = new VoiceEngine();
    const fileInfo: any = await voiceEngine.convertInToSpeech(data);
    const fileData = {
      message: data.text,
      name: data.name,
      description: data.description,
      promptType: data.promptType,
      domainId: data.domainId,
      uuid: data.uuid,
      url: fileInfo?.filePath ?? null,
      path: fileInfo?.nfsPath ?? null,
      fileName: fileInfo?.fileName ?? null,
      type: data.type,
    };
    const body = {
      url: fileData.url,
      name: fileData.name,
      description: fileData.description,
      message: fileData?.message,
      domainId: fileData?.domainId
    };
    await updateUserPromptLibrary(body, fileData.uuid);
    return { fileData };
  } catch (error) {
    logger.error('editTextToSpeech', { data, Err: error });
  }
};

/**
 *
 * @param data
 * @returns
 */
export const updateUserPromptLibrary = (data: any, uuid: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_update_user_Prompt_library(?,?,?,?,?,?,?) ',
        [
          uuid ?? null,
          data?.domainId ?? null,
          data?.url ?? null,
          data?.name ?? null,
          data?.description ?? null,
          data?.type ?? null,
          data?.message ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            logger.error('ccaas_update_user_Prompt_library', { data, Err: err, id: uuid });
            reject(err);
          }
          resolve(result?.length > 0 ? result : []);
        }
      );
    } catch (error) {
      logger.error('updateUserPromptLibrary', { data, Err: error, id: uuid });
      reject(error);
    }
  });
};

/**
 *
 * @param data
 * @param uuid
 * @returns
 */
export const deletePromptAudio = (data: any, uuid: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_delete_Prompt_library(?,?) ',
        [
          uuid ?? null,
          data?.domainId ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            logger.error('ccaas_delete_Prompt_library', { data, Err: err, id: uuid });
            reject(err);
          }
          resolve(result?.length > 0 ? result?.[0]?.[0] : null);
        }
      );
    } catch (error) {
      logger.error('deletePromptAudio', { data, Err: error, id: uuid });
      reject(error);
    }
  });
};

export const getAudioPromptLibrary = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_audio_prompt_library(?) ',
        [
          data?.domainId ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            logger.error('ccaas_get_audio_prompt_library', { data, Err: err });
            reject(err);
          }
          resolve(result?.length > 0 ? result?.[0] : []);
        }
      );
    } catch (error) {
      logger.error('getAudioPromptLibrary', { data, Err: error });
      reject(error);
    }
  });
};

export const insertPromptLibrary = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_insert_prompt_text(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?) ',
        [
          data?.fieldname ?? null,
          data?.encoding ?? null,
          data?.mimetype ?? null,
          data?.filename ?? null,
          data?.path ?? null,
          data?.size ?? null,
          data?.type ?? null,
          data?.promptType ?? null,
          data?.url ?? null,
          data?.name ?? null,
          data?.description ?? null,
          data?.domainId ?? null,
          data?.companyId ?? null,
          data?.message ?? null,
          data?.promptStatus ?? null,
        ],
        (err: any, result: any) => {
          if (err) {
            logger.error('ccaas_insert_prompt_text', { data, Err: err });
            reject(err);
          }
          resolve(result?.length > 0 ? result?.[0]?.[0]?.Id : 0);
        }
      );
    } catch (error) {
      logger.error('insertPromptLibrary', { data, Err: error });
      reject(error);
    }
  });
};

export const isValidPromptName = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_is_valid_prompt_name(?,?) ',
        [
          data?.name ?? null,
          data?.domainId ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            logger.error('ccaas_get_is_valid_prompt_name', { data, Err: err });
            reject(err);
          }
          resolve(result?.length > 0 ? result?.[0]?.[0] : 0);
        }
      );
    } catch (error) {
      logger.error('isValidPromptName', { data, Err: error });
      reject(error);
    }
  });
};
