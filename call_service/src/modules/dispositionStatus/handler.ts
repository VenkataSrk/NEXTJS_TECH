/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */
import { extend, result } from 'lodash';
import { roleInfoDao } from '../../dao/callflow.dao';
import {
  deleteDisposition,
  deleteVoiceRecording,
  getDispositinStatus,
  getVideoChannelHistoryId,
  getVideoRecordDao,
  getVideoSessionIdDao,
  getVoiceRecordDao,
  getVoiceSessionIdDao,
  insertCompanyPurchasedAddonDao,
  insertUpdateDispositionStatus,
  isValidDispositionName,
  updateCalledPersonDetailsCall,
  updateDispositionFacebook,
  updateDispositionForCall,
  updateDispositionForChat,
  updateDispositionForWhatsapp,
  updateDispositionInstagram,
  updateDispositionStatusVideoDao,
  updateSentimentScore,
  updateSentimentScoreChat,
  updateSentimentScoreVideo,
} from '../../dao/dispositionStatus';
import { HUBSPOT_DOMAINID, RESPONSE, ROLES, } from '../../helpers/constants';
import {
  aiRegenerate,
  aiSummarySubject,
  hubSpotApi,
  sentimentalScore,
  // ticketingAutoassign
} from '../../helpers/utils';
import { ioredisChat, redisClient } from '../../plugins/db';
import { logger } from '../../plugins/winston';

/**
 *
 * @param req
 * @param res
 * @param done
 * agent status api
 */
export async function insertDispositionStatusHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      dispositionName: req?.body?.dispositionName ?? null,
      type: 'Custom',
      description: req?.body?.description ?? null,
      domainId: auth.domainId,
      status: req?.body?.status ?? 0,
      colorCode: req.body?.colorCode ?? null,
      accessType: 1,
    };
    const insertStatus: any = await insertUpdateDispositionStatus(data);

    if (insertStatus) {
      ioredisChat.to(`${data?.domainId}`).emit('dispositionCreated');
      res.status(200).send({ statusCode: 200, message: RESPONSE.disposition_status, insertStatusList: insertStatus?.[0] ?? {} });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {
    logger.error('insertDispositionStatusHandler catch', { data: req.body, Err: err });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getDispositinStatusHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {};
    data.domainId = auth.domainId;
    const getDispositin: any = await getDispositinStatus(data);
    if (Array.isArray(getDispositin) && getDispositin[0]?.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getDispositinListRes: getDispositin[0] });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getDispositinListRes: [] });
    }
  } catch (err) {
    logger.error('getDispositinStatusHandler catch', { data: req.headers?.domainId, Err: err });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * agent status api
 */
export async function updateDispositionStatusHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      did: req.params.did,
      dispositionName: req?.body?.dispositionName ?? null,
      type: req?.body?.type ?? null,
      description: req?.body?.description ?? null,
      domainId: auth.domainId,
      status: req?.body?.status ?? 0,
      colorCode: req.body?.colorCode ?? null,
      accessType: 1,
    };
    if (data?.did !== '') {
      const updateStatus: any = await insertUpdateDispositionStatus(data);
      if (updateStatus) {
        ioredisChat.to(`${data?.domainId}`).emit('dispositionCreated');
        res.status(200).send({ statusCode: 200, message: RESPONSE.disposition_statuss });
      } else {
        res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
      }
    } else {
      res.status(200).send({ statusCode: 405, message: RESPONSE.did });
    }
  } catch (err) {
    logger.error('updateDispositionStatusHandler catch', { data: req.body, Err: err });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function deleteDispositionStatusHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const did: any = req.params.did;
    const data: any = {
      did: req.params.did,
      domainId: auth.domainId,
      dispositionName: req.body.dispositionName,
    };
    if (data?.did !== '') {
      const deletedisposition: any = await deleteDisposition(data);
      if (deletedisposition) {
        res.status(200).send({ statusCode: 200, message: RESPONSE.dispositionn_statuss });
      } else {
        res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
      }
    } else {
      res.status(200).send({ statusCode: 405, message: RESPONSE.did });
    }
  } catch (err) {
    logger.error('deleteDispositionStatusHandler catch', { data: req.body, params: req.params, Err: err });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function isValidDispositionNameHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {};
    data.domainId = auth.domainId;
    data.dispositionName = req?.body?.name ?? null;
    const dispositionRes: any = await isValidDispositionName(data);
    if (dispositionRes && dispositionRes.length > 0 && dispositionRes[0]?.dispositionName.length > 0) {
      res.status(200).send({ statusCode: 403, message: RESPONSE.name_already_exist, flag: 0 });
    } else {
      res.status(200).send({ statusCode: 200, message: RESPONSE.available, flag: 1 });
    }
  } catch (err) {
    logger.error('isValidDispositionNameHandler catch', { data: req.body, Err: err });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function updateDispositionHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const params = req.params;
    const data: any = {
      domainId: auth.domainId,
      ext: auth.ext,
      sessionId: params.sessionId,
      summary: req?.body?.summary ?? null,
      disposition: req?.body?.disposition ?? {},
      dispositionName: req?.body?.dispositionName ?? null,
      afterCallWorkTime: req?.body?.afterCallWorkTime ?? 0,
      calledPersonDetails: req?.body?.calledPersonDetails ?? {},
      subject: req?.body?.subject ?? null,
      updateAt: Date.now(),
      channelType: req.body?.channelType,
      ChatDurationHistoryId: req.body?.ChatDurationHistoryId,
      dispositionId: req?.body?.disposition?.did ?? req?.body?.dispositionId,
      agentOccupyDetails: req?.body?.agentOccupyDetails ?? null,
      isTicketing: req?.body?.isTicketing ?? false,
      tags: req?.body?.tags ?? null,
    };
    logger.info('updateDispositionHandler - Request Data:', data);
    let updateDispStatus: any = [];
    if (data?.channelType === 'Chat') {
      updateDispStatus = await updateDispositionForChat(data);
      const nlpData: any = {
        HistoryId: updateDispStatus[0]?.ChatHistoryId,
        ChatDurationHistoryId: data?.ChatDurationHistoryId,
        channelType: 'Chat',
        domainId: data?.domainId,
        ext: data?.ext ?? 0
      };
      const updateScore: any = await sentimentalScore(nlpData);
      if (updateScore?.status === 200 && updateScore?.data) {
        ioredisChat.to(`${auth.domainId}_${auth.ext}`).emit('SentimentScoreAnalytics', {
          data,
          speedometerScore: updateScore?.data?.sentimentalScore || 0,
          sentimentalScoreUser: updateScore?.data?.sentimentalScoreUser || 0,
          sentimentalScoreAgent: updateScore?.data?.sentimentalScoreAgent || 0
        });
        updateSentimentScoreChat(nlpData, updateScore.data);
      } else {
        ioredisChat.to(`${auth.domainId}_${auth.ext}`).emit('SentimentScoreAnalytics', {
          data,
          speedometerScore: 0,
          sentimentalScoreUser: 0,
          sentimentalScoreAgent: 0
        });
      }
    } else {
      updateDispStatus = await updateDispositionForCall(data);
      console.log(`${data?.sessionId}_updateDispStatus`, updateDispStatus);
      logger.info(`${data?.sessionId}_updateDispStatus`, updateDispStatus);
      const nlpData: any = {
        HistoryId: updateDispStatus[0]?.CallHistoryId,
        ChatDurationHistoryId: data?.ChatDurationHistoryId ?? 0,
        channelType: 'Call',
        domainId: data?.domainId,
        ext: data?.ext ?? 0
      };

      const updateScore: any = await sentimentalScore(nlpData);
      if (updateScore?.status === 200 && updateScore?.data) {
        ioredisChat.to(`${auth.domainId}_${auth.ext}`).emit('SentimentScoreAnalytics', {
          data,
          speedometerScore: updateScore?.data?.sentimentalScore || 0,
          sentimentalScoreUser: updateScore?.data?.sentimentalScoreUser || 0,
          sentimentalScoreAgent: updateScore?.data?.sentimentalScoreAgent || 0
        });
        updateSentimentScore(nlpData, updateScore.data);
      } else {
        ioredisChat.to(`${auth.domainId}_${auth.ext}`).emit('SentimentScoreAnalytics', {
          data,
          speedometerScore: 0,
          sentimentalScoreUser: 0,
          sentimentalScoreAgent: 0
        });
      }
    }
    if (updateDispStatus) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.disposition_statuss });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
    if (HUBSPOT_DOMAINID.includes(auth.domainId) && data?.channelType !== 'Chat') {
      const payload = {
        domainId: auth?.domainId,
        sessionId: params?.sessionId,
        ext: auth?.ext ?? 0,
        fromNumber: req?.body?.fromNumber ?? 0
      };
      console.log('auth.domainId1', payload);
      hubSpotApi(payload);

    }
  } catch (err) {
    logger.error('updateDispositionHandler catch', { body: req?.body, params: req?.params, error: err });
    console.log('updateDispositionHandler catch', req?.body, req?.params, 'error', err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
/* export async function autoAssignHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const params = req.params;
    const body = req.body;
    const response: any = await ticketingAutoassign(body);
    res.status(200).send({ response, statusCode: 200, message: 'Success' });
  } catch (err) {
    logger.error('autoAssignHandler catch', req?.body, 'error', err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
} */

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function updateCalledPersonDetailsCallHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const params = req.params;
    const data: any = {
      domainId: auth.domainId,
      sessionId: params.sessionId,
      calledPersonDetails: req?.body?.calledPersonDetails ?? {},
    };
    const updateCalledPerson: any = await updateCalledPersonDetailsCall(data);
    if (updateCalledPerson) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {
    logger.error('updateCalledPersonDetailsCallHandler catch', { body: req?.body, params: req?.params?.sessionId, error: err });
    console.log('updateCalledPersonDetailsCallHandler catch', req?.body, 'params.sessionId', req.params.sessionId, 'error', err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getFilterMobileApp(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {};
    data.domainId = auth.domainId;
    const getDisposition: any = await getDispositinStatus(data);
    const dispositionName: any = [];
    getDisposition.map((list: any) => {
      dispositionName.push(list.dispositionName);
    });
    const direction: any = ['inbound', 'outbound'];
    const ChannelType: any = ['Call', 'Chat', 'Email', 'SMS'];
    const dateRange: any = ['Yesterday', 'Last 7 days', 'Custom date'];
    const filterData: any = [{ dispositionList: dispositionName, directionList: direction, channelList: ChannelType, dateRangeList: dateRange }];
    res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, result: filterData });

  } catch (err) {
    logger.error('getFilterMobileApp catch', 'error', { body: req?.headers, error: err });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function insertCompanyPurchasedAddonHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      companyId: req?.body?.companyId,
      domainId: req?.body?.domainId,
      planId: req?.body?.planId,
      productId: req?.body?.productId,
      addonId: req?.body?.addonId,
      addonName: req?.body?.addonName,
      addonprice: req?.body?.addonprice,
      addonqty: req?.body?.addonqty,
    };
    const addon: any = await insertCompanyPurchasedAddonDao(data);
    if (addon[0].errcode === 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.failed });
    }
  } catch (err) {
    console.log('insertCompanyPurchasedAddonHandler catch', req?.body, 'error', err);
    logger.error('insertCompanyPurchasedAddonHandler catch', { data: req?.body, Err: err });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function updateDispositionStatusVideoHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      ext: auth?.ext,
      domainId: auth?.domainId,
      meetingId: req?.body?.meetingId ?? null,
      summary: req?.body?.summary ?? null,
      disposition: req?.body?.disposition ?? null,
      afterCallWorkTime: req?.body?.afterCallWorkTime ?? null,
      subject: req?.body?.subject ?? null,
      agentExtn: req?.body?.agentExtn ?? null,
    };
    logger.info('updateDispositionStatusVideoHandler - Request Data:', data);
    const updateDispositionVedio: any = await updateDispositionStatusVideoDao(data);

    // Extract VideoHistoryId from the RowDataPacket array
    const videoHistoryId = updateDispositionVedio[0]?.VideoHistoryId;

    // Prepare nlpData for sentiment analysis
    const nlpData: any = {
      HistoryId: videoHistoryId,
      ChatDurationHistoryId: data?.ChatDurationHistoryId ?? 0,
      channelType: 'Video',
      domainId: data?.domainId,
      ext: data?.ext ?? 0,
    };
    try {
      // Get the sentimental score
      const updateScore: any = await sentimentalScore(nlpData);
      console.log('updateScore>>>>>', updateScore);
      logger.info('updateScore>>>>> - Request Data:', updateScore);

      if (updateScore?.status === 200) {
        await updateSentimentScoreVideo(nlpData, updateScore?.data);

        const sentimentData = {
          data,
          speedometerScore: updateScore?.data?.sentimentalScore ?? 0,
          sentimentalScoreUser: updateScore?.data?.sentimentalScoreUser ?? 0,
          sentimentalScoreAgent: updateScore?.data?.sentimentalScoreAgent ?? 0,
        };
        // Emit sentimental score update via Redis
        ioredisChat.to(`${auth?.domainId}_${auth?.ext}`).emit('SentimentScoreAnalytics', sentimentData);
        console.log('Sentiment score emitted:', sentimentData);
      } else {
        console.error('Failed to get sentimental score:', updateScore);
        logger.error('Failed to get sentimental score: catch', nlpData, 'error', updateScore);
      }
    } catch (scoreError) {
      console.error('Error in sentiment score process:', scoreError);
      logger.error('Failed to get sentimental score: catch', nlpData, 'error', scoreError);

    }
    res.status(200).send({ statusCode: 200, message: 'Success', result: updateDispositionVedio.result });
  } catch (err) {
    console.error('Error in updateDispositionStatusVideoHandler:', err);
    logger.error('updateDispositionStatusVideoHandler catch', { data: req?.body, Err: err });
    res.status(500).send({ statusCode: 500, message: 'Internal Server Error' });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getVideoMeetRecordInfoHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {
      domainId: auth?.domainId,
      ext: auth?.ext,
      search: req?.body?.search ?? null,
      direction: req?.body?.direction ?? null,
      startDate: req?.body?.startDate ?? null,
      endDate: req?.body?.endDate ?? null,
      agentId: req?.body?.agentId,
      offset: req?.body?.offset,
      limit: req?.body?.limit,

    };

    // Fetch role information
    const roleInfo: any = await roleInfoDao(data);
    data.ccas_role_info = roleInfo?.roleid?.toString();

    console.log('data.ccas_role_info >>>>>>>>>>>', data.ccas_role_info);
    logger.info('data.ccas_role_info >>>>>>>>>>>', data.ccas_role_info);

    // agent
    if (data.ccas_role_info === ROLES?.AGENT_ROLE_ID) {
      const getVideoRecord: any = await getVideoRecordDao(data);
      if (Array.isArray(getVideoRecord) && getVideoRecord?.length > 0) {
        const firstRecord = getVideoRecord[1];
        const count = getVideoRecord[0];

        // const recordData = Array.isArray(firstRecord) ? firstRecord[0] : firstRecord;
        if (firstRecord?.errcode === -1) {
          res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: firstRecord, recordCount: count });
        } else {
          res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, result: firstRecord, recordCount: count });
        }
      }
      return res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    } {
      data.agentId = null;
      const getVideoRecord: any = await getVideoRecordDao(data);
      if (Array.isArray(getVideoRecord) && getVideoRecord?.length > 0) {
        const firstRecord = getVideoRecord[1];
        const count = getVideoRecord[0];

        // const recordData = Array.isArray(firstRecord) ? firstRecord[0] : firstRecord;
        if (firstRecord?.errcode === -1) {
          res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: firstRecord, recordCount: count });
        } else {
          res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, result: firstRecord, recordCount: count });
        }
      }
      return res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [], recordCount: [] });
    }
  } catch (err) {
    logger.error('getVideoMeetRecordInfoHandler catch', { data: req?.body, Err: err });
    return res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getVoiceRecordInfoHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {
      domainId: auth?.domainId,
      ext: auth?.ext,
      search: req?.body?.search ?? null,
      direction: req?.body?.direction ?? null,
      startDate: req?.body?.startDate ?? null,
      endDate: req?.body?.endDate ?? null,
      agentId: req?.body?.agentId,
      limit: req?.body?.limit,
      offset: req?.body?.offset,

    };

    // Fetch role information
    const roleInfo: any = await roleInfoDao(data);
    data.ccas_role_info = roleInfo?.roleid?.toString();

    console.log('data.ccas_role_info >>>>>>>>>>>', data.ccas_role_info);
    logger.info('data.ccas_role_info >>>>>>>>>>>', data.ccas_role_info);

    if (data.ccas_role_info === ROLES?.AGENT_ROLE_ID) {
      const getVoiceRecord: any = await getVoiceRecordDao(data);
      logger.info('getVoiceRecord>>>>>>>>>>>>>>>>>', getVoiceRecord);
      console.log(getVoiceRecord, 'getVoiceRecord>>>>>>>>>>>>>>>>>');
      if (Array.isArray(getVoiceRecord) && getVoiceRecord.length > 0) {
        const firstRecord = getVoiceRecord[1];
        const count = getVoiceRecord[0];

        // const recordData = Array.isArray(firstRecord) ? firstRecord[0] : firstRecord;
        if (firstRecord.errcode === -1) {
          res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: firstRecord, recordCount: count });
        } else {
          res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, result: firstRecord, recordCount: count });
        }
      }
      return res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    } {
      data.agentId = null;
      const getVoiceRecord: any = await getVoiceRecordDao(data);
      if (Array.isArray(getVoiceRecord) && getVoiceRecord.length > 0) {
        const firstRecord = getVoiceRecord[1];
        const count = getVoiceRecord[0];

        // const recordData = Array.isArray(firstRecord) ? firstRecord[0] : firstRecord;
        if (firstRecord.errcode === -1) {
          res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: firstRecord, recordCount: count });
        } else {
          res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, result: firstRecord, recordCount: count });
        }
      }
      return res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [], recordCount: [] });
    }
  } catch (err) {
    logger.error('getVoiceRecordInfoHandler catch', { data: req?.body, Err: err });
    return res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getVideoSessionDetails(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {};
    data.domainId = auth.domainId;
    data.sessionId = req.params.sessionid;
    if (data?.sessionId) {
      const getVoiceRecord: any = await getVideoSessionIdDao(data);
      if (Array.isArray(getVoiceRecord) && getVoiceRecord?.length > 0) {
        res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, result: getVoiceRecord });
      } else {
        res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
      }
    } else {
      res.status(200).send({ statusCode: 422, message: 'SessionId should not be empty' });
    }

  } catch (err) {
    logger.error('getVideoSessionDetails catch', { data: req.params.sessionid, Err: err });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getVoiceSessionDetails(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {};
    data.domainId = auth.domainId;
    data.sessionId = req.params.sessionid;
    if (data?.sessionId) {
      const getVoiceRecord: any = await getVoiceSessionIdDao(data);
      if (Array.isArray(getVoiceRecord) && getVoiceRecord?.length > 0) {
        res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, result: getVoiceRecord });
      } else {
        res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
      }
    } else {
      res.status(200).send({ statusCode: 422, message: 'SessionId should not be empty' });
    }

  } catch (err) {
    logger.error('getVoiceSessionDetails catch', { data: req.params.sessionid, Err: err });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function updateDispositionBeforeUnloadHandler(req: any, res: any, done: any) {
  try {
    console.log('updateDispositionBeforeUnloadHandler>>>', req?.body);
    logger.info('updateDispositionBeforeUnloadHandler>>> catch', { data: req?.body });

    const parsedJson = JSON.parse(req?.body);
    const payload = {
      ChatDurationHistoryId: parsedJson?.ChatDurationHistoryId,
      domainId: parsedJson?.domainId,
      summary: parsedJson?.summary,
      sessionId: parsedJson?.sessionId,
      dispositionId: parsedJson?.dispositionId,
      afterCallWorkTime: parsedJson?.afterCallWorkTime,
      agentOccupyDetails: parsedJson?.agentOccupyDetails,
      disposition: parsedJson?.disposition ?? {},
      calledPersonDetails: parsedJson?.calledPersonDetails,
      subject: parsedJson?.subject,
      ext: parsedJson?.ext,
      isTicketing: parsedJson?.isTicketing,
      isSessionClosed: 1,
      afterWorkTime: 0,
      dispositionUpdatedBy: parsedJson?.dispositionUpdatedBy ?? '',
      duration: parsedJson?.duration,
      dispositionSubject: null,
      WhatsappHistoryId: parsedJson?.WhatsappHistoryId,
      FacebookHistoryId: parsedJson?.FacebookHistoryId,
      InstagramHistoryId: parsedJson?.InstagramHistoryId,
    };
    console.log('updateDispositionBeforeUnloadHandler>>>', payload);
    logger.info('updateDispositionBeforeUnloadHandler>>> catch', payload);

    if (parsedJson?.channelType === 'Chat') {
      await updateDispositionForChat(payload);
    } else if (parsedJson?.channelType === 'Call') {
      await updateDispositionForCall(payload);
    } else if (parsedJson?.channelType === 'Whatsapp') {
      await updateDispositionForWhatsapp(payload);
    } else if (parsedJson?.channelType === 'Facebook') {
      await updateDispositionFacebook(payload);
    } else if (parsedJson?.channelType === 'Instagram') {
      await updateDispositionInstagram(payload);
    }
  } catch (err) {
    logger.error('updateDispositionBeforeUnloadHandler catch', { data: req?.body, Err: err });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function aisummarySubjectHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    // const params = req.params;
    let body = req.body;
    if (body?.channelType === 'video') {
      const payload = {
        domainId: auth.domainId,
        meetingId: body?.meetingId,
        agentId: body?.agentId
      };
      const getVideoChannelHistoryIdRes = await getVideoChannelHistoryId(payload);
      body = {
        ...body,
        ...payload,
        HistoryId: getVideoChannelHistoryIdRes?.[0]?.VideoHistoryId,
        ChatDurationHistoryId: getVideoChannelHistoryIdRes?.[0]?.VideoDurationHistoryId,
        channelType: 'video'
      };
      delete body?.meetingId;
      delete body?.agentId;
    }
    const response: any = await aiSummarySubject(body);
    res.status(200).send({ response, statusCode: 200, message: 'Success' });
  } catch (err) {
    logger.error('aisummarySubjectHandler catch', { data: req?.body, Err: err });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function regeneratedispositionHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const params = req.params;
    const body = req.body;
    const response: any = await aiRegenerate(body);
    res.status(200).send({ response, statusCode: 200, message: 'Success' });
  } catch (err) {
    logger.error('regeneratedispositionHandler catch', { data: req?.body, Err: err });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */

export async function deleteVoiceRecordingsHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      domainId: auth.domainId,
      rid: req?.body?.rid,
    };
    const deleteVoicerecording: any = await deleteVoiceRecording(data);
    console.log('deleteVoicerecordingdeleteVoicerecording', deleteVoicerecording);
    if (deleteVoicerecording[0]?.errcode === -1) {
      res.status(200).send({ statusCode: 422, message: RESPONSE.failed, result: deleteVoicerecording[0] });
    } else {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, result: deleteVoicerecording[0] });
    }
    return res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });

  } catch (err) {
    console.log('deleteVoiceRecording catch', req?.body, 'error', err);
    logger.error('deleteVoiceRecording catch', { data: req?.body, Err: err });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}
