import { deleteSkill, getSearchList, getSkill, insertSkill, isSkillEnableList, isValidSkillName, skillStatus, updateSkill, updateSkillStatus } from '../../dao/skills';
import { logger } from '../../plugins/log';

import { RESPONSE } from '../../helpers/constants';

/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

/**
 *
 * @param req
 * @param res
 * @param done
 * ccs skill api
 */
export async function insertSkillHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const dataS: any = {
      skill_name: req?.body?.skill_name ?? null,
      skill_description: req?.body?.skill_description ?? null,
      status: req?.body?.status ?? 1,
      domainId: auth.domainId,
      skillUser: req?.body?.skillUser ?? null,
      type: req?.body?.type ?? null,
    };
    const insertSkillRes: any = await insertSkill(dataS);
    logger.info('insertSkill', insertSkillRes);
    if (insertSkillRes) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.skill_insert_successfully, insertSkillList: insertSkillRes ?? {} });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {

    logger.error('insertSkillHandler', req.headers, req.body , err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function getSkillHandler(req: any, res: any, done: any) {
  try {
    const sid = req.params.sid;
    if (sid !== '') {
      const getSkillNumber: any = await getSkill(sid);
      logger.info('getSkill', getSkillNumber);
      if (Array.isArray(getSkillNumber) && getSkillNumber.length > 0) {
        res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getSkillNumberlist: getSkillNumber });
      } else {
        res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getSkillNumber: [] });
      }
    } else {
      res.status(200).send({ statusCode: 424, message: 'sid number should not be empty' });
    }
  } catch (err) {

    logger.error('getSkillHandler', req.params , err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function updateSkillHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const sid = req.params.sid;
    const datak: any = {
      skill_name: req.body.skill_name ? req.body.skill_name : null,
      skill_description: req.body.skill_description ? req.body.skill_description : null,
      domainId: auth.domainId,
      type: req.body.type ? req.body.type : null,
    };
    const updateSkillNumber: any = await updateSkill(datak, sid);
    logger.info('updateSkill', updateSkillNumber);
    if (updateSkillNumber.affectedRows === 1) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.skill_updated_successfully });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_update });
    }
  } catch (err) {

    logger.error('updateSkillHandler', req.headers, req.params, req.body , err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function deleteSkillHandler(req: any, res: any, done: any) {
  try {
    const sid: any = req.params.sid;
    if (sid !== '') {
      const deleteSkillNumber: any = await deleteSkill(sid);
      logger.info('deleteSkill', deleteSkillNumber);
      if (deleteSkillNumber) {
        res.status(200).send({ statusCode: 200, message: RESPONSE.skill_deleted_successfully });
      } else {
        res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
      }
    } else {
      res.status(200).send({ statusCode: 424, message: 'sid number should not be empty' });
    }
  } catch (err) {

    logger.error('deleteSkillHandler', req.params , err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getSkillListHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const status = req.query.status;
    const data: any = {};
    data.domainId = auth.domainId;
    data.limit = req.query.limit ?? null;
    data.offset = req.query.offset ?? null;
    data.searchBy = req.query.searchBy;
    data.searchByValue = req.query.searchByValue ?? null;
    const getSkillListNumber: any = await getSearchList(data, status);
    logger.info('getSearchList', getSkillListNumber);
    if (getSkillListNumber && getSkillListNumber.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getSkillNumberlist: getSkillListNumber,
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getSkillNumberlist: [] });
    }
  } catch (err) {

    logger.error('getSkillListHandler', req.headers, req.query , err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function skillStatusHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {};
    data.domainId = auth.domainId;
    const sid = req.params.sid;
    if (sid !== '') {
      const userRes: any = await skillStatus(sid, data);
      logger.info('skillStatus', userRes);
      if (userRes?.length === 0) {
        res.status(200).send({ statusCode: 200, message: RESPONSE.activate });
      } else {
        res.status(200).send({ statusCode: 405, message: RESPONSE.deactivate });
      }
    } else {
      res.status(200).send({ statusCode: 405, message: RESPONSE.sid });
    }
  } catch (err) {

    logger.error('skillStatusHandler', req.headers, req.params , err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * update skill api
 */
export async function updateSkillStatusHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const sid = req.params.sid;
    const data: any = {
      status: req.body.status,
      domainId: auth.domainId,
    };
    const updateStatusRes: any = await updateSkillStatus(data, sid);
    logger.info('updateSkillStatus', updateStatusRes);
    if (updateStatusRes.affectedRows === 1) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.stataus_updated_successfully });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_update });
    }
  } catch (err) {

    logger.error('updateSkillStatusHandler', req.headers, req.params, req.body , err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function isValidSkillNameHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {};
    data.domainId = auth.domainId;
    data.skill_name = req?.body?.name ?? null;
    const SkillRes: any = await isValidSkillName(data);
    logger.info('isValidSkillName', SkillRes);
    if (SkillRes && SkillRes.length > 0 && SkillRes[0]?.skill_name.length > 0) {
      res.status(200).send({ statusCode: 403, message: RESPONSE.name_already_exist, flag: 0 });
    } else {
      res.status(200).send({ statusCode: 200, message: RESPONSE.available, flag: 1 });
    }
  } catch (err) {

    logger.error('getSkillHisValidSkillNameHandlerandler', req.headers, req.body , err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function isSkillEnableListHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {};
    data.domainId = auth.domainId;
    const getEnable: any = await isSkillEnableList(data);
    logger.info('isSkillEnableList', getEnable);
    if (Array.isArray(getEnable) && getEnable.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getSkillEnableListRes: getEnable });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getSkillEnableListRes: [] });
    }
  } catch (err) {

    logger.error('isSkillEnableListHandler', req.headers , err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}
