import multer from 'fastify-multer';
import fs from 'fs';
import { configs } from './app';
// import path from 'path';

const storage = multer.diskStorage({
  destination(req: any, file: any, cb: any) {
    const dir = `${configs.uploadDestination}`;
    if (!fs.existsSync(dir)) {
      fs.mkdirSync(dir, { recursive: true });
    }
    cb(null, dir);
  },
  filename(req: any, file: any, cb: any) {
    cb(null, file.originalname);
  }
});

const upload = multer({ storage, limits: { fileSize: 2000000 } });
export const Upload = upload.fields([{ name: 'doc', maxCount: 10 }]);

// bulk upload
const bulkUplaod = multer.diskStorage({
  destination(req: any, file: any, cb: any) {
    const dir = `${configs.uploadDestination}`;
    if (!fs.existsSync(dir)) {
      fs.mkdirSync(dir, { recursive: true });
    }
    cb(null, dir);
  },
  filename(req: any, file: any, cb: any) {
    cb(null, 'bulkFiles.csv');
  }
});

const bulkpload = multer({ storage: bulkUplaod, limits: { fileSize: 2000000 } });
export const bulkUpload = bulkpload.fields([{ name: 'csv', maxCount: 10 }]);

// artcileUploaded
const artcileUpload = multer.diskStorage({
  destination(req: any, file: any, cb: any) {
    const dir = `${configs.paths.upload_destination_article}`;
    // const dir = path.resolve(__dirname, 'uploads', 'articles');
    if (!fs.existsSync(dir)) {
      fs.mkdirSync(dir, { recursive: true });
    }
    cb(null, dir);
  },
  filename(req: any, file: any, cb: any) {
    cb(null, file.originalname);
  }
});
const uploadArticle = multer({ storage: artcileUpload });
export const artcileUploaded = uploadArticle.fields([{ name: 'doc', maxCount: 10 }]);

// virtual background upload
const virtualBackgroundUpload = multer.diskStorage({
  destination(req: any, file: any, cb: any) {
    const dir = `${configs.paths.upload_virtual_background}`;
    if (!fs.existsSync(dir)) {
      fs.mkdirSync(dir, { recursive: true });
    }
    cb(null, dir);
  },
  filename(req: any, file: any, cb: any) {
    const rr = file.originalname;
    const name = rr.split('.');
    const originalname = name.slice(0, name.length - 1).join('.');
    const last = name.pop();
    cb(null, `${originalname}_${Date.now()}.${last}`);
  }
});
const uploadVirtualBackground = multer({ storage: virtualBackgroundUpload, limits: { fileSize: 2000000 } });
export const uploadedVirtualBackground = uploadVirtualBackground.fields([{ name: 'doc', maxCount: 10 }]);
'';
