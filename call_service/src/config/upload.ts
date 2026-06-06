import multer from 'fastify-multer';
import fs from 'fs';
import { logger } from '../plugins/winston';
import { configs } from './app';

/**
 * chat_upload_file
 */
const storage = multer.diskStorage({
  destination(req: any, file: any, cb: any) {
    try {
      const authorization = req.headers;
      const domainId = authorization.domainId;
      let dir: any = '';
      if (domainId !== undefined) {
        dir = configs.chat_path.upload_destination + domainId;
      } else {
        dir = configs.chat_path.upload_destination + domainId;
      }
      if (!fs.existsSync(dir)) {
        fs.mkdirSync(dir, { recursive: true });
      }
      cb(null, dir);
    } catch (error) {
      logger.error('chat_upload_file', req?.body, req?.headers, error);
    }
  },
  filename(req: any, file: any, cb: any) {
    try {
      cb(null, file?.originalname);
    } catch (error) {
      logger.error('chat_upload_file filename', file, error);
    }
  }
});

const upload = multer({ storage, limits: { fileSize: 20000010 } });
export const cpUpload = upload.fields([{ name: 'doc', maxCount: 10 }]);

/**
 * prompt_file_upload
 */
const storages = multer.diskStorage({
  destination(req, file, cb) {
    try {
      const authorization = req.headers;
      const domainId = authorization.domainId;
      let dir: any = '';
      if (domainId !== undefined) {
        dir = configs.paths.upload_destination + domainId;
      } else {
        dir = configs.paths.upload_destination + domainId;
      }
      if (!fs.existsSync(dir)) {
        fs.mkdirSync(dir, { recursive: true });
      }
      cb(null, dir);
    } catch (error) {
      logger.error('prompt_file_upload', req?.body, req?.headers, error);
    }
  },
  filename(req: any, file, cb) {
    try {
      cb(null, req.body.type === 'record' ? `${file?.originalname}.wav` : file?.originalname);
    } catch (error) {
      logger.error('prompt_file_upload filename', req?.body?.type, file, error);
    }
  }
});

const uploaded = multer({ storage: storages, limits: { fileSize: 20000010 } });
export const cpUploaded = uploaded.fields([{ name: 'doc', maxCount: 10 }]);

/**
 * customer file upload
 */

const storagess = multer.diskStorage({
  destination(req, file, cb) {
    try {
      const dir: any = configs.chat_path.upload_destination;
      if (!fs.existsSync(dir)) {
        fs.mkdirSync(dir, { recursive: true });
      }
      cb(null, dir);
    } catch (error) {
      logger.error('customer_file_upload', req?.body, req?.headers, error);
    }
  },
  filename(req, file, cb) {
    try {
      cb(null, file?.originalname);
    } catch (error) {
      logger.error('customer_file_upload filename', req?.body, file, error);
    }
  }
});

const chatUpload = multer({ storage: storagess, limits: { fileSize: 20000010 } });
export const Uploaded = chatUpload.fields([{ name: 'doc', maxCount: 10 }]);
