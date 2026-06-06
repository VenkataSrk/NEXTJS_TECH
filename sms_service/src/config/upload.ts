import multer from 'fastify-multer';
import fs from 'fs';
import { configs } from './app';

/**
 * prompt_file_upload
 */
const storages = multer.diskStorage({
  destination(req, file, cb) {
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
  },
  filename(req, file, cb) {
    cb(null, file.originalname);
  }
});

const uploaded = multer({ storage: storages, limits: { fileSize: 20000010 } });
export const cpUploaded = uploaded.fields([{ name: 'doc', maxCount: 10 }]);
