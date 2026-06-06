import axios from 'axios';
import fs from 'fs-extra';
import path from 'path';
import { uuid } from 'uuidv4';
import { configs } from '../config/app';
import { logger } from '../plugins/winston';

export default class VoiceEngine {

  public async convertInToSpeech(data: any) {
    try {
      const fid = uuid();
      const dataToSend: any = {
        domainId: data.domainId,
        uuid: fid,
      };
      const waveFile = `${data.name}.wav`;
      const tempDir = `${configs.paths.upload_destination}${dataToSend.domainId}/${dataToSend.uuid}`;
      if (!fs.existsSync(tempDir)) {
        try {
          fs.mkdirSync(tempDir, { recursive: true });
          console.log('Directory created:', tempDir);
          logger.error('Directory created:', tempDir);

        } catch (err) {
          console.error('Failed to create directory:', err);
          logger.error('Failed to create directory:', err);

        }
      }
      const destination = `${tempDir}/${waveFile}`;

      // TTS from NPL server
      const url = `http://${process.env.TTS_IP}/tts`;
      const body = {
        text: data.text
      };
      const res: any = await axios.post(url, body);
      const audioBuffer = Buffer.from(res.data, 'base64');
      fs.writeFileSync(destination, audioBuffer, 'binary', (err) => {
        if (err) {
          logger.error('error', err);
          return;
        }
        console.log('WAV file has been created successfully.');
        logger.error('WAV file has been created successfully.');

      });
      const fileUrl = `${configs.paths.fileGetUrl}${dataToSend.domainId}/${dataToSend.uuid}/${waveFile}`;
      const nfsPath = `${configs.paths.upload_destination}${dataToSend.domainId}/${dataToSend.uuid}/${waveFile}`;
      return { nfsPath, fileName: waveFile, filePath: fileUrl };
    } catch (err) {
      logger.error('convertInToSpeech', err);
    }
  }

  public mkdirp(dir: string) {
    try {
      if (fs.existsSync(dir)) {
        return true;
      }
      const dirname = path.dirname(dir);
      this.mkdirp(dirname);
      fs.mkdirSync(dir);
    } catch (error) {
      logger.error('mkdirp', error);
    }
  }
}
