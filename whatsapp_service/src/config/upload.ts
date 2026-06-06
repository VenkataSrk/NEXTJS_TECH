import multer from 'fastify-multer';
import fs from 'fs';
import { configs } from './app';

const storage = multer.diskStorage({
  destination(req: any, file: any, cb: any) {
    const dir = configs.email_path.Email_upload_destination;
    if (!fs.existsSync(dir)) {
      fs.mkdirSync(dir, { recursive: true });
    }
    cb(null, dir);
  },
  filename(req: any, file: any, cb: any) {
    cb(null, file.originalname);
  }
});

const upload = multer({ storage, limits: { fileSize: 20000010 } });
export const cpUpload = upload.fields([{ name: 'doc', maxCount: 10 }]);
