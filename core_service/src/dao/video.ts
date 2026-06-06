import { gueststore, mysqlPoolConnection, store } from '../plugins/db';
import { logger } from '../plugins/winston';

export const setGuestDetails = async (sipDet: any, data: any) => {
  try {
    return await gueststore.set(`${sipDet.user_id}@${data?.video_host}`, JSON.stringify(sipDet));
  } catch (error) {
    logger.error('setGuestDetails', error);
  }
};

export const setMeetInfo = async (id: any, data: any) => {
  try {
    return await store.set(id, JSON.stringify(data), 'EX', 86400);
  } catch (error) {
    logger.error('setMeetInfo', error);
  }
};

export const getMeetInfo = async (uuid: any) => {
  try {
    return await store.get(uuid, async (err: any, data: any) => {
      if (err || data === null || data === '{}') {
        logger.error('getMeetInfo', err, data);
      } else {
        if (data != null) {
          const retData = await JSON.parse(data);
          return retData;
        }
      }
    });
  } catch (error) {
    logger.error('getMeetInfo', error);
  }
};

export const getSettingsApi = async (id: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_getVideoCallConfiguration(?)',
        [id ? id : null],
        (err: any, result: any) => {
          if (err) {
            logger.error('up_getVideoCallConfiguration', err);
            reject(err);
          }
          resolve(result?.[0]);
        }
      );
    } catch (err) {
      logger.error('up_getVideoCallConfiguration', err);
      reject(err);
    }
  });
};

export const getUrls = async (id: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_getVirtualBackground(?)',
        [id ? id : null],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('up_getVirtualBackground', err);

          }
          resolve(result?.[0]);
        }
      );
    } catch (err) {
      reject(err);
      logger.error('up_getVirtualBackground', err);

    }
  });
};

export const insertUpdateSettingsApi = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_insertUpdateVideoCallConfiguration(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)',
        [
          data?.domainId ?? null,
          data?.isLinkExpiry ?? null,
          (data?.expiryUnit) ?? null,
          data?.expiryUnitType ?? null,
          data?.isScreenShare ?? null,
          data?.isCoBrowser ?? null,
          data?.isFaceDocVerification ?? null,
          data?.isVirtualBackground ?? null,
          data?.emailAddress ?? null,
          data?.emailSubject ?? null,
          JSON.stringify(data?.emailMessage) ?? null,
          data?.smsNumber ?? null,
          data?.smsSubject ?? null,
          JSON.stringify(data?.smsMessage) ?? null,
          data?.virtualBackgroundId ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('up_insertUpdateVideoCallConfiguration', err);
          }
          resolve(result[0]);
        }
      );
    } catch (err) {
      reject(err);
      logger.error('up_insertUpdateVideoCallConfiguration', err);
    }
  });
};

export const insertRecordTranscriptSP = (domainid:any, sessionId:any, data:any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call Insert_videoRecordingcc_message_details(?,?,?)',
        [
          domainid, sessionId, JSON.stringify(data)
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('Insert_videoRecordingcc_message_details', err);
          }
          resolve(result);
        }
      );
    } catch (err) {
      reject(err);
      logger.error('Insert_videoRecordingcc_message_details', err);
    }
  });
};

export const insertVerificationAttachmentsSP = (domainid:any, coid:any, attachments:any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_update_customer_attachment(?,?,?)',
        [
          domainid, coid, JSON.stringify(attachments)
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('ccaas_update_customer_attachment', err);
          }
          resolve(result);
        }
      );
    } catch (err) {
      reject(err);
      logger.error('ccaas_update_customer_attachment', err);
    }
  });
};

export const deleteSettingsApi = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_deleteVideoCallConfiguration(?)',
        [
          Number(data?.domainId) ?? null,
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('up_deleteVideoCallConfiguration', err);
          }
          resolve(result);
        }
      );
    } catch (err) {
      reject(err);
      logger.error('up_deleteVideoCallConfiguration', err);
    }
  });
};

export const insertUpdateVirtualBackgroundApi = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call up_insertUpdateVirtualBackground(?,?,?,?)',
        [
          data?.virtualBackgroundId ?? null,
          data?.domainId ?? null,
          data?.virtualBackground ?? null,
          data?.isDefault ?? null
        ],
        (err: any, result: any) => {
          if (err) {
            reject(err);
            logger.error('up_insertUpdateVirtualBackground', err);
          }
          resolve(result[0]);
        }
      );
    } catch (err) {
      reject(err);
      logger.error('up_insertUpdateVirtualBackground', err);
    }
  });
};
