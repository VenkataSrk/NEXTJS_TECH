import { configs } from '../../config/app';
import { logger } from '../../plugins/log';

/**
 *
 * @param {Object} req - request object
 * @param {Object} reply - response object
 * @description - File upload function
 */
export async function uploadFiles(req: any, res: any, done: any) {
  try {
    const filesarry: any = req.files.doc;
    for (const fileArray of filesarry) {
      let obj: any = [];
      obj = fileArray.destination.split('/');
      fileArray.geturl = `${process.env.APIFILE_LOCAL_URL}${configs.fileGetUrl}${fileArray.originalname}`;
      delete fileArray.path;
      delete fileArray.destination;
    }
    res.send({ statusCode: 200, message: 'SUCCESS', files: filesarry });
  } catch (err) {

    logger.error('uploadFiles', req.files, err);
    res.send({ statusCode: 500, message: 'internal server error' });
  }
}

export async function virtualBackgroundUploadedFiles(req: any, res: any, done: any) {
  try {
    const filesarry: any = req.files.doc;
    for (const fileArray of filesarry) {
      let obj: any = [];
      obj = fileArray.destination.split('/');
      fileArray.geturl = `${configs.paths.virtual_background_path}${fileArray.filename}`;
      delete fileArray.path;
      delete fileArray.destination;
    }
    res.send({ statusCode: 200, message: 'SUCCESS', files: filesarry });
  } catch (err) {

    logger.error('virtualBackgroundUploadedFiles', req.files, err);
    res.send({ statusCode: 500, message: 'internal server error' });
  }
}
