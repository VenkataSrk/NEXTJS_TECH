import { Upload, uploadedVirtualBackground } from '../../config/upload';
import { FILE } from '../../helpers/constants';
import { uploadFiles, virtualBackgroundUploadedFiles } from './handler';

export default function fileHandler(server: any, options: any, next: any) {
  // file upload
  server.post(
    FILE.UPLOAD,
    {
      // preValidation: [server.validateSession],
      preHandler: Upload,
      schema: {
        summary: 'File upload',
        description: 'File upload Api',
        tags: ['Files'],
      },
    },
    uploadFiles,
  );

  server.post(
    FILE.VIRTUAL_BACKGROUND_UPLOAD,
    {
      // preValidation: [server.validateSession],
      preHandler: uploadedVirtualBackground,
      schema: {
        summary: 'File upload',
        description: 'File upload Api',
        tags: ['Files'],
      },
    },
    virtualBackgroundUploadedFiles
  );
  next();
}
