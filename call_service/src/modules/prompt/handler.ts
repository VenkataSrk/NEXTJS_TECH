import { v4 as uuidv4 } from 'uuid';
import { configs } from '../../config/app';
import {
  chatFilePath,
  deletePromptAudio,
  editTextToSpeech,
  getAudioPromptLibrary,
  insertPromptLibrary,
  isValidPromptName,
  textToConvert,
  textToConvertPreview,
  updateUserPromptLibrary,
} from '../../dao/promptIvr.dao';
import { RESPONSE } from '../../helpers/constants';
import { logger } from '../../plugins/winston';

/**
 *
 * @param req
 * @param res
 * @param done
 * @description File Upload
 */
export const chatFileUpload = async (req: any, res: any, done: any) => {
  try {
    const filesarry: any = req.files.doc;
    const authorization = req.headers;
    const domainId = authorization.domainId;
    const uuid = uuidv4();
    for (const chat of filesarry) {
      const url = `${configs.chat_path.fileGetUrl}${domainId}/${chat.originalname}`;
      chat.uuid = uuid;
      chat.url = url;
      chat.name = `${chat.originalname}`;
      chat.domainId = domainId;
      chat.message = chat?.message ?? null;
      delete chat.destination;
      delete chat.originalname;
      // await chatFilePath(chat);
    }
    res.send({ statusCode: 200, errCode: -1, message: 'success', fileList: filesarry });
  } catch (err) {
    logger.error('chatFileUpload', { data: req?.files?.doc, domainid: req?.headers?.domainId, Err: err });
    res.send({ statusCode: 500, errCode: 1, message: 'internal server error' });
  }
};

export const chatUploadFile = async (req: any, res: any, done: any) => {
  try {
    const filesarry: any = req.files.doc;
    const authorization = req.headers;
    const domainId = authorization.domainId;
    const uuid = uuidv4();
    for (const chat of filesarry) {
      const url = `${configs.paths.fileGetUrl}${domainId}/${chat.originalname}`;
      chat.uuid = uuid;
      chat.url = url;
      chat.name = `${chat.originalname}`;
      chat.domainId = domainId;
      chat.message = chat?.message ?? null;
      delete chat.destination;
      delete chat.originalname;
      await chatFilePath(chat);
    }
    res.send({ statusCode: 200, errCode: -1, message: 'success', fileList: filesarry });
  } catch (err) {
    logger.error('chatUploadFile', { data: req?.files?.doc, domainid: req?.headers?.domainId, Err: err });
    res.send({ statusCode: 500, errCode: 1, message: 'internal server error' });
  }
};

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function previewTextToSpeechHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {};
    data.domainId = auth.domainId;
    data.message = req?.body?.message ?? null;
    data.name = req?.body?.name ?? null;
    data.description = req?.body?.description ?? null;
    data.text = req?.body?.text ?? null;
    const { fileData } = await textToConvertPreview(data);
    res.send({
      fileData,
      status_code: 200,
      err_code: -1,
      affected_rows: 1,
      insertAt: fileData.insertId,
      message: 'success',
    });
  } catch (err) {
    logger.error('previewTextToSpeechHandler', { data: req?.body, domainid: req?.headers?.domainId, Err: err });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */

// current
export const insertAudioPromptHandler = async (req: any, res: any, done: any) => {
  try {
    const auth = req.headers;
    const data: any = {};
    data.domainId = auth?.domainId;
    data.companyId = auth?.companyId;
    data.type = req?.body?.type ?? null;
    data.file = req?.body?.file ?? null;
    data.text = req?.body?.text ?? null;
    data.name = req?.body?.name ?? null;
    data.promptStatus = req.body.promptStatus ?? null;
    data.description = req?.body?.description ?? null;
    data.promptType = req?.body?.promptType ?? 1;
    data.message = req?.body?.message ?? null;
    data.filename = req.body.fileName ?? null;
    if (data.type === 'text') {
      const { fileData } = await textToConvert(data);
      res.send({
        fileData,
        status_code: 200,
        err_code: -1,
        affected_rows: 1,
        insertAt: fileData.insertId,
        promptStatus: fileData.promptStatus,
        message: 'success',
      });
    } else if (data.type === 'upload') {
      const filesarry: any = req.files.doc;
      const type = req.body.type;
      const authorization = req.headers;
      const promptType = req.body.promptType;
      const name = req.body.name;
      const promptStatus = req.body.promptStatus;
      const description = req.body.description;
      const domainId = authorization.domainId;
      const companyId = authorization.companyId;
      const uuid = uuidv4();
      let cid: any = '';
      for (const ivr of filesarry) {
        const url = `${configs.paths.fileGetUrl}${domainId}/${ivr.originalname}`;
        ivr.uuid = uuid;
        ivr.type = type;
        ivr.promptType = promptType;
        ivr.url = url;
        ivr.name = name;
        ivr.description = description;
        ivr.domainId = domainId;
        ivr.companyId = companyId;
        ivr.promptStatus = promptStatus;
        ivr.filename = ivr.originalname;
        ivr.message = '';
        delete ivr.destination;
        delete ivr.originalname;
        // const dd: any = await insertPromptText(ivr);
        const dd: any = await insertPromptLibrary(ivr);
        cid = dd.insertId;
      }
      res.send({
        statusCode: 200,
        errCode: -1,
        message: 'success',
        fileList: filesarry,
        insertAt: cid,
      });
    } else if (data.type === 'record') {
      const filesarry: any = req.files.doc;
      const type = req.body.type;
      const name = req.body.name;
      const description = req.body.description;
      const authorization = req.headers;
      const promptStatus = req.body.promptStatus;
      const promptType = req.body.promptType;
      const domainId = authorization.domainId;
      const companyId = authorization.companyId;
      const uuid = uuidv4();
      let cid: any = '';
      for (const ivr of filesarry) {
        const url = `${configs.paths.fileGetUrl}${domainId}/${ivr.originalname}.wav`;
        ivr.uuid = uuid;
        ivr.type = type;
        ivr.description = description;
        ivr.filename = name;
        ivr.promptType = promptType;
        ivr.url = url;
        ivr.name = name;
        ivr.domainId = domainId;
        ivr.promptStatus = promptStatus;
        ivr.promptType = promptType;
        ivr.companyId = companyId;
        ivr.message = '';
        delete ivr.destination;
        delete ivr.originalname;
        // const dd: any = await insertPrompt(ivr);
        const dd: any = await insertPromptLibrary(ivr);
        cid = dd.insertId;
      }
      res.send({
        statusCode: 200,
        errCode: -1,
        message: 'success',
        fileList: filesarry,
        insertId: cid,
      });
    } else {
      res.send({
        status_code: 422,
        err_code: 1,
        affected_rows: 0,
        message: 'Text field is empty!',
      });
    }
  } catch (err) {
    logger.error('insertAudioPromptHandler', { data: req?.body, domainid: req?.headers?.domainId, Err: err });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
};

/**
 *
 * @param req
 * @param res
 * @param done
 */
export const getAudioPromptHandler = async (req: any, res: any, done: any) => {
  try {
    const auth: any = req.headers;
    const data: any = {};
    data.domainId = auth.domainId;
    const getFile: any = await getAudioPromptLibrary(data);
    if (getFile && Array.isArray(getFile) && getFile.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getList: getFile,
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getList: [] });
    }
  } catch (err) {
    logger.error('getAudioPromptHandler', { data: req?.headers, domainid: req?.headers?.domainId, Err: err });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
};

/**
 *
 * @param req
 * @param res
 * @param done
 */
export const updatePromptAudioHandler = async (req: any, res: any, done: any) => {
  try {
    const auth: any = req.headers;
    const body: any = req.body;
    const uuid: any = req.params.uuid;
    const data: any = {
      name: body.name,
      description: body.description,
      domainId: auth.domainId
    };
    if (body.type === 'upload' && body.fileEdit) {
      const filesarry: any = req.files.doc;
      if (filesarry === undefined) {
        // await updateUserPromptBasedOnName(data, uuid);
        await updateUserPromptLibrary(data, uuid);
        res.status(200).send({
          statusCode: 200,
          message: 'updated prompt audio successfully',
          dataList: filesarry === undefined ? 'undefined' : 'defined',
          file: filesarry,
          fileName: data.filename,
        });
      } else {
        for (const ivr of filesarry) {
          const url = `${configs.paths.fileGetUrl}${auth.domainId}/${ivr.originalname}`;
          ivr.uuid = uuid;
          ivr.type = body.type;
          ivr.promptType = body.promptType;
          ivr.url = url;
          ivr.name = body.name;
          ivr.domainId = auth.domainId;
          ivr.companyId = auth.companyId;
          ivr.message = '';
          ivr.description = body.description;
          delete ivr.destination;
          delete ivr.originalname;
          // await updateUserPromptLib(ivr, uuid);
          await updateUserPromptLibrary(ivr, uuid);
        }
      }
      res.status(200).send({
        statusCode: 200,
        message: 'updated prompt audio successfully',
        dataList: filesarry === undefined ? 'undefined' : 'defined',
        file: filesarry,
        fileName: filesarry[0].filename,
      });
    } else if (body.type === 'text' && body.fileEdit) {
      if (body.text === '') {
        body.domainId = auth.domainId;
        // updateTextToSpeechNameAndDescription(body, uuid);
        updateUserPromptLibrary(body, uuid);
      } else {
        body.domainId = auth.domainId;
        body.uuid = uuid;
        const value: any = await editTextToSpeech(body);
        res.status(200).send({
          statusCode: 200,
          message: 'updated prompt audio successfully',
          dataList: body.text === '' ? 'undefined' : 'defined',
          url: value.fileData.url,
          textMessage: value?.fileData?.message,
        });
      }
      res.status(200).send({
        statusCode: 200,
        message: 'updated prompt audio successfully',
        dataList: body.text === '' ? 'undefined' : 'defined',
      });
    } else if (body.type === 'record' && body.fileEdit) {
      body.domainId = auth.domainId;
      body.uuid = uuid;
      const filesarry: any = req.files.doc;
      if (filesarry === undefined) {
        // await updateUserPromptBasedOnName(data, uuid);
        await updateUserPromptLibrary(data, uuid);
      } else {
        for (const ivr of filesarry) {
          const url = `${configs.paths.fileGetUrl}${auth.domainId}/${ivr.originalname}`;
          ivr.uuid = uuid;
          ivr.type = body.type;
          ivr.promptType = body.promptType;
          ivr.url = url;
          ivr.name = body.name;
          ivr.domainId = auth.domainId;
          ivr.companyId = auth.companyId;
          ivr.message = '';
          ivr.description = body.description;
          delete ivr.destination;
          delete ivr.originalname;
          // await updateUserPromptLib(ivr, uuid);
          await updateUserPromptLibrary(ivr, uuid);
        }
      }
      res.status(200).send({
        statusCode: 200,
        message: 'updated Prompt audio successfully',
        dataList: filesarry === undefined ? 'undefined' : 'defined',
        file: filesarry,
      });
    }
    res.status(200).send({ statusCode: 200, message: 'updated prompt audio successfully' });
  } catch (err) {
    logger.error('updatePromptAudioHandler', { data: req.params.uuid, domainid: req?.headers?.domainId, Err: err });
    res.status(500).send({ statusCode: 500, message: 'internal server error' });
  }
};

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function deletePromptAudioHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const uuid = req.params.uuid;
    const data: any = {};
    data.domainId = auth?.domainId;
    const deleteAudioPrompt: any = await deletePromptAudio(data, uuid);
    if (deleteAudioPrompt?.errCode === 1) {
      res.status(200).send({ statusCode: 200, message: 'prompt audio deleted successfully' });
    } else {
      res.status(200).send({ statusCode: 404, message: 'data not found' });
    }
  } catch (err) {
    logger.error('deletePromptAudioHandler', { data: req.params.uuid, domainid: req?.headers?.domainId, Err: err });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function isValidPromptNameHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {};
    data.domainId = auth.domainId;
    data.name = req?.body?.name ?? null;
    const promptRes: any = await isValidPromptName(data);
    if (promptRes?.errcode === 1) {
      res.status(200).send({ statusCode: 403, message: RESPONSE.name_already_exist, flag: 0 });
    } else {
      res.status(200).send({ statusCode: 200, message: RESPONSE.available, flag: 1 });
    }
  } catch (err) {
    logger.error('isValidPromptNameHandler', { data: req.body, domainid: req?.headers?.domainId, Err: err });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}
