/**
 * @createdBy <kathiravan.r@worktual.com>
 * @createdOn <07/06/2024>
 */

import {
  deleteScheduleReportDao,
  getScheduleReportDao,
  insertScheduleReportDao
} from '../../dao/scheduleReport.dao';
import { RESPONSE } from '../../helpers/constants';
import { CronJob } from '../../helpers/utils';
import { logger } from '../../plugins/log';
import { scheduleReportJob } from '../../redisBull';

/**
 *
 * @param req
 * @param res
 * @param done
 * insert schedule reports api
 */
export async function insertScheduleReportHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      reportName: req?.body?.reportName ?? null,
      recurrence: req?.body?.recurrence ?? null,
      rrule: req?.body?.rrule ?? null,
      isRecurr: req?.body?.isRecurr ?? 0,
      timezone: req?.body?.timezone ?? null,
      isSetPassword: req?.body?.isSetPassword ?? 0,
      setPassword: req?.body?.setPassword ?? null,
      fileType: req?.body?.fileType ?? null,
      toAddr: req?.body?.toAddr ?? null,
      subject: req?.body?.subject ?? null,
      message: req?.body?.message ?? null,
      domainId: auth.domainId,
      nextOccurrence: req?.body?.nextOccurrence ?? null,
      reportId: req?.body?.reportId ?? null,
      startDataRange: req?.body?.startDataRange.toString() ?? null,
      endDataRange: req?.body?.endDataRange ?? null
    };
    // CronJob(data);
    scheduleReportJob(data);
    const insertScheduleReportInfo: any = await insertScheduleReportDao(data);
    if (insertScheduleReportInfo[0]?.errcode === 0) {
      const insertScheduleReport = insertScheduleReportInfo[0] ?? [];
      res.status(200).send({ insertScheduleReport, statusCode: 200, message: RESPONSE.success_message });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getAnalyticsSkillChatReportHandler - Unexpected Error:', err);
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * get schedule reports api
 */
export const getScheduleReportHandler = async (req: any, res: any, done: any) => {
  try {
    const data: any = {};
    data.domainId = req.headers?.domainId ?? req.params?.domainId;
    const getScheduleReportResult: any = await getScheduleReportDao(data);
    if (getScheduleReportResult.length > 0) {
      res.status(200).send({ statusCode: 200, message: 'Data inserted  Successfully', result: getScheduleReportResult });
    } else {
      res.status(200).send({ statusCode: 404, message: 'Data Not Found' });
    }
  } catch (err) {
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getAnalyticsSkillChatReportHandler - Unexpected Error:', err);
  }
};

/**
 *
 * @param req
 * @param res
 * @param done
 * delete schedule reports api
 */
export async function deleteScheduleReportHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const uid: any = req.params.uid;
    const data = {
      domainId: auth.domainId
    };
    if (uid !== '') {
      const deleteScheduleReport: any = await deleteScheduleReportDao(data, uid);
      if (deleteScheduleReport) {
        res.status(200).send({ ScheduleReport: deleteScheduleReport[0], statusCode: 200, });
      } else {
        res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
      }
    } else {
      res.status(200).send({ statusCode: 405, message: RESPONSE.uid });
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getAnalyticsSkillChatReportHandler - Unexpected Error:', err);
  }
}
