import { FastifyReply, FastifyRequest } from 'fastify';
import { getDataAndPrivacySetDao, getLiveInteractionFromDb, getMyplanRolePermissionDao, getMyplanTeamLogActivityDao, getMyplanTeamPermissionDao, getMyplanUserogActivityDao, getMyplanUserPermissionDao, getTeamDetailsDao, getUserDetailsByRoleDao, getUserOnboardSettingFromDb, iinsertUpdateDataAndPrivacySetDao, insertUpdateMyplanRolePermissionDao, insertUpdateMyplanTeamsPermissionDao, iupdateMyplanUserPermissionDao } from '../../dao/login';
import {
  billingGetInvoiceSummaryModel,
  companyUserOrderExtensionModel,
  createMyAccountFromDb,
  getAddonListModel,
  getAllAddonListModel,
  getAllMembersRoleDao,
  getBillServicePlanModel,
  getCompanyBilling,
  getEmailOtpFromDb,
  getManagementFromDb,
  getRolesInfoFromDb,
  getsecuritytwostepverificationFromDb,
  getStorageData,
  getUserDataPrivacySettingFromDb,
  getUserDepartmentFromDb,
  getUserDevicesDao,
  getUserDevicesFromDb,
  getUserLoginLogFromDb,
  getUserogActivityLogDao,
  getUserPasswordInfoFromDb,
  getUserRolesFromDB,
  insertDevicesDtlDao,
  insertupdateDataPrivacySettingFromDb,
  insertUpdateTwoStepVerificationDao,
  insertUserActivityLogFromDb,
  passwordUpdateFromDb,
  resendOtpToDb,
  sendVerifyOtp,
  syncMyAccountDetailsFromDb,
  userGetDetailsFromDb,
  usersGetUserExtensionInfoModel
} from '../../dao/myPlan';
import { RESPONSE } from '../../helpers/constants';
// const parser = require('xml-js');

import * as fs from 'fs';
import * as path from 'path';
import { logger } from '../../plugins/winston';

let nodemailer: any;
try {
  nodemailer = require('nodemailer');
} catch (err) {
  console.warn('⚠️ nodemailer module not found; email sending disabled', err);
    // logger.warn("nodemailer module not found; email sending disabled", err);
}

function resolveEmailTemplatePath(templateName: string) {
  const distTemplatePath = path.join(
    __dirname,
    '../../plugins/emailtemplates',
    templateName,
  );
  if (fs.existsSync(distTemplatePath)) {
    return distTemplatePath;
  }
  return path.join(process.cwd(), 'src', 'plugins', 'emailtemplates', templateName);
}

/**
 *
 * @param req
 * @param res
 */

export async function usersGetUserExtensionInfoController(req: FastifyRequest, res: FastifyReply) {
  try {
    const body: any = req.body;
    const results: any = await usersGetUserExtensionInfoModel(body);
    console.log(results);
    if (results.message !== undefined) {
      res.send({ status_code: 404, err_code: -1, message: results.message });
    }
    // CreateLog(req.url, req.body, results);
    res.send({ status_code: 200, err_code: 1, affected_rows: results.length, message: 'success', finalresponse: results });
  } catch (err) {
    res.log.error(err);
    res.send({ status_code: 500, err_code: -1, affected_rows: 0, message: 'internal server error' });
  }
}

export async function companyUserOrderExtensionController(req: FastifyRequest, res: FastifyReply) {
  try {
    const body: any = req.body;
    const results: any = await companyUserOrderExtensionModel(body);
    console.log(results);
    if (results.message !== undefined) {
      res.send({ status_code: 404, err_code: -1, message: results.message });
    }
    // CreateLog(req.url, req.body, results);
    res.send({ status_code: 200, err_code: 1, affected_rows: results.length, message: 'success', finalresponse: results });
  } catch (err) {
    res.log.error(err);
    res.send({ status_code: 500, err_code: -1, affected_rows: 0, message: 'internal server error' });
  }
}

export async function billingGetInvoiceSummaryController(req: FastifyRequest, res: FastifyReply) {
  try {
    const body: any = req.body;
    const results: any = await billingGetInvoiceSummaryModel(body);

    if (Array.isArray(results) && results.length > 0) {
      res.status(200).send({ statusCode: 200, message: 'Success', finalresponse: results });
    } else {
      res.status(200).send({ statusCode: 200, message: 'No data found', finalresponse: [] });
    }
  } catch (err) {
    res.log.error(err);
    res.send({ status_code: 500, err_code: -1, affected_rows: 0, message: 'internal server error' });
  }
}

export async function getBillServicePlan(req: FastifyRequest, res: FastifyReply) {
  try {
    const body: any = req.body;
    const response: any = await getBillServicePlanModel(body);
    if (Array.isArray(response) && response.length > 0) {
      res.status(200).send({ statusCode: 0, message: 'Success', result: response });
    } else {
      res.status(200).send({ statusCode: -1, message: 'No data found', result: [] });
    }

  } catch (err) {
    res.log.error(err);
    res.send({ statusCode: 500, message: 'Internal server error' });
  }
}

export async function getAddonListController(req: FastifyRequest, res: FastifyReply) {
  try {
    const body: any = req.body;
    const results: any = await getAddonListModel(body);
    console.log(results);
    if (results.message !== undefined) {
      res.send({ status_code: 404, err_code: -1, message: results.message });
    }
    // CreateLog(req.url, req.body, results);
    res.send({ status_code: 200, err_code: 1, affected_rows: results.length, message: 'success', finalresponse: results });
  } catch (err) {
    res.log.error(err);
    res.send({ status_code: 500, err_code: -1, affected_rows: 0, message: 'internal server error' });
  }
}

export async function getMyAccountsyncDetails(req: FastifyRequest, res: FastifyReply) {
  try {
    const auth: any = req.headers;
    const payload: any = req.body;
    const body: any = { ...payload, company_id: auth.companyId };
    const response: any = await syncMyAccountDetailsFromDb(body);
    console.log('getMyAccountsyncDetails>>', response);
    if (Array.isArray(response) && response.length > 0) {
      res.status(200).send({ statusCode: 200, message: 'Success', result: response });
    } else {
      res.status(200).send({ statusCode: 200, message: 'No data found', result: [] });
    }

  } catch (err) {
    res.log.error(err);
    res.send({ statusCode: 500, message: 'Internal server error' });
  }
}

export async function getUserPasswordInfo(req: FastifyRequest, res: FastifyReply) {
  try {
    const authorization = req.headers;
    const body: any = {
      company_id: authorization.companyId,
      email: authorization.username
    };
    const response: any = await getUserPasswordInfoFromDb(body);
    console.log('getUserPasswordInfo>>', response);
    if (response && response.length > 0) {
      res.status(200).send({ statusCode: 200, message: 'Success', result: response });
    } else {
      res.status(200).send({ statusCode: 200, message: 'No data found', result: [] });
    }

  } catch (err) {
    res.log.error(err);
    res.send({ statusCode: 500, message: 'Internal server error' });
  }
}

export async function createmyaccountsyncupdetails(req: FastifyRequest, res: FastifyReply) {
  try {
    const auth: any = req.headers;
    const payload: any = req.body;

    const body: any = {
      company_id: auth.companyid || auth.companyId,
      myaccount_email: payload.myaccount_email,
      social_account_email: payload.social_account_email,
      processtype: payload.processtype,
      social_account_type: payload.social_account_type,
      login_source: payload.login_source,
      login_device_id: payload.login_device_id,
      login_ipaddress: payload.login_ipaddress,
      extension: payload.extension,
      address: payload.address,
    };

    const response: any = await createMyAccountFromDb(body);
    console.log('createmyaccountsyncupdetails', response);
    const result = response?.[0] || response;

    if (result.errcode === 0) {
      return res.code(200).send({
        statusCode: 200,
        message: 'Inserted successfully',
        result: response,
      });
    } if (result.errcode === -1) {
      return res.code(200).send({
        statusCode: 200,
        message: 'Updated successfully',
        result: response,
      });
    }
    console.warn('⚠️ Operation failed:', result.errcode);
    return res.code(400).send({
      statusCode: 400,
      message: 'Operation failed',
      result: [],
    });

  } catch (err: any) {
    console.error('🔥 Error in createmyaccountsyncupdetails:', err);
    return res.code(500).send({
      statusCode: 500,
      message: 'Internal Server Error',
      error: err.message,
    });
  }
}

export async function getUserDataPrivacySetting(req: FastifyRequest, res: FastifyReply) {
  try {
    const auth: any = req.headers;
    const body: any = { company_id: auth.companyId };
    const response: any = await getUserDataPrivacySettingFromDb(body);
    console.log('getUserDataPrivacySetting>>', response);

    if (Array.isArray(response) && response.length > 0) {
      res.status(200).send({ statusCode: 200, message: 'Success', result: response });
    } else {
      res.status(200).send({ statusCode: 200, message: 'No data found', result: [] });
    }

  } catch (err) {
    res.log.error(err);
    res.send({ statusCode: 500, message: 'Internal server error' });
  }
}
export async function insertUpdateDataPrivacySetting(req: FastifyRequest, res: FastifyReply) {
  try {
    const auth: any = req.headers;
    const payload: any = req.body;

    const body: any = {
      ...payload,
      company_id: auth.companyid || auth.companyId,
      UserEmail: auth.username || auth.useremail,
    };

    const response: any = await insertupdateDataPrivacySettingFromDb(body);
    console.log('insertUpdateDataPrivacySetting>>', response);

    if (Array.isArray(response) && response.length > 0) {
      const result = response[0];

      if (result.errCode === 0) {
        return res.code(200).send({
          statusCode: 200,
          message: 'Data privacy settings inserted successfully',
          result: response,
        });
      } if (result.errCode === 1) {
        return res.code(200).send({
          statusCode: 200,
          message: 'Data privacy settings updated successfully',
          result: response,
        });
      }
      return res.code(400).send({
        statusCode: 400,
        message: RESPONSE.failed || 'Operation failed',
        result: [],
      });

    }
    return res.code(404).send({
      statusCode: 404,
      message: 'No data returned from database',
      result: [],
    });

  } catch (err: any) {
    req.log.error('Error in insertUpdateDataPrivacySetting:', err);
    return res.code(500).send({
      statusCode: 500,
      message: 'Internal Server Error',
      error: err.message,
    });
  }
}

export async function getUserDevice(req: FastifyRequest, res: FastifyReply) {
  try {
    const auth: any = req.headers;
    const payload: any = req.body;
    const body: any = { ...payload, company_id: auth.companyId };
    const response: any = await getUserDevicesFromDb(body);
    console.log('getUserDevice>>', response);

    if (Array.isArray(response) && response.length > 0) {
      res.status(200).send({ statusCode: 200, message: 'Success', result: response });
    } else {
      res.status(200).send({ statusCode: 200, message: 'No data found', result: [] });
    }

  } catch (err) {
    res.log.error(err);
    res.send({ statusCode: 500, message: 'Internal server error' });
  }
}

export async function getManagement(req: FastifyRequest, res: FastifyReply) {
  try {
    const auth: any = req.headers;
    const payload: any = req.body;
    const body: any = { ...payload, company_id: auth.companyId, UserEmail: auth.username };
    const response: any = await getManagementFromDb(body);
    console.log('getManagement>>', response);

    if (response[0]?.errCode === 0) {
      res.status(200).send({ statusCode: 200, message: 'Inserted successfully', result: response });
    } else if (response[0]?.errCode === 1) {
      res.status(200).send({ statusCode: 200, message: 'update successfully', result: response });
    } else {
      res.status(200).send({ statusCode: 200, message: RESPONSE.failed, result: [] });
    }

  } catch (err) {
    res.log.error(err);
    res.send({ statusCode: 500, message: 'Internal server error' });
  }
}
export async function getuserroles(req: FastifyRequest, res: FastifyReply) {
  try {
    const auth: any = req.headers;
    const payload: any = req.body;

    const body: any = {
      ...payload,
      company_id: auth.companyId
    };

    const roles: any = await getUserRolesFromDB(body);
    console.log('getuserroles>>', roles);

    if (roles && roles.length > 0) {
      res.status(200).send({ statusCode: 200, message: 'Success', result: roles });

    } else {
      res.status(200).send({ statusCode: 200, message: 'No data found', result: [] });
    }

  } catch (err) {
    res.log.error(err);
    res.status(500).send({
      statusCode: 500,
      message: 'Internal server error'
    });
  }
}

export async function userGetDetails(req: FastifyRequest, res: FastifyReply) {
  try {
    const auth: any = req.headers;
    const body: any = { company_id: auth.companyId };
    const response: any = await userGetDetailsFromDb(body);
    console.log('userGetDetails>>', response);

    if (response && response.length > 0) {
      res.status(200).send({ statusCode: 200, message: 'Success', result: response });
    } else {
      res.status(200).send({ statusCode: 200, message: 'No data found', result: [] });
    }

  } catch (err) {
    res.log.error(err);
    res.send({ statusCode: 500, message: 'Internal server error' });
  }
}

export async function getUserDepartment(req: FastifyRequest, res: FastifyReply) {
  try {
    const auth: any = req.headers;
    const body: any = { company_id: auth.companyId };
    const response: any = await getUserDepartmentFromDb(body);
    console.log('getUserDepartment>>', response);

    if (response && response.length > 0) {
      res.status(200).send({ statusCode: 200, message: 'Success', result: response });
    } else {
      res.status(200).send({ statusCode: 200, message: 'No data found', result: [] });
    }

  } catch (err) {
    res.log.error(err);
    res.send({ statusCode: 500, message: 'Internal server error' });
  }
}
export async function getUserLoginLog(req: FastifyRequest, res: FastifyReply) {
  try {
    const auth: any = req.headers;
    const body: any = { company_id: auth.companyId, email: auth.username };
    const response: any = await getUserLoginLogFromDb(body);
    console.log('getUserLoginLog', response);
    if (response && response.length > 0) {
      res.status(200).send({ statusCode: 200, message: 'Success', result: response });
    } else {
      res.status(200).send({ statusCode: 200, message: 'No data found', result: [] });
    }

  } catch (err) {
    res.log.error(err);
    res.send({ statusCode: 500, message: 'Internal server error' });
  }
}

export async function getRolesInfo(req: FastifyRequest, res: FastifyReply) {
  try {
    const auth: any = req.headers;
    const payload: any = req.body;
    const body: any = {
      company_id: auth.companyId,
      ...payload
    };
    const response: any = await getRolesInfoFromDb(body);
    console.log('getRolesInfo', response);
    if (Array.isArray(response) && response.length > 0) {
      res.status(200).send({ statusCode: 200, message: 'Success', result: response });
    } else {
      res.status(200).send({ statusCode: 200, message: 'No data found', result: [] });
    }

  } catch (err) {
    res.log.error(err);
    res.send({ statusCode: 500, message: 'Internal server error' });
  }
}

export async function insertUpdateTwoStepVerificationHandler(req: FastifyRequest, res: FastifyReply) {
  try {
    const auth: any = req.headers;
    const payload: any = req.body;
    const body: any = { ...payload, company_id: auth.companyId };
    const response: any = await insertUpdateTwoStepVerificationDao(body);
    console.log('insertUpdateTwoStepVerificationHandler', response);
    if (Array.isArray(response) && response.length > 0) {
      res.status(200).send({ statusCode: 200, message: 'Success', result: response });
    } else {
      res.status(200).send({ statusCode: 200, message: 'No data found', result: [] });
    }
  } catch (err) {
    res.log.error(err);
    res.send({ statusCode: 500, message: 'Internal server error' });
  }
}

export async function getSecurityTwoStepVerification(req: FastifyRequest, res: FastifyReply) {
  try {

    const auth: any = req.headers;
    const body: any = { email: auth.username };
    const response: any = await getsecuritytwostepverificationFromDb(body);
    console.log('getRolesInfo', response);
    if (response && response.length > 0) {
      res.status(200).send({ statusCode: 200, message: 'Success', result: response });
    } else {
      res.status(200).send({ statusCode: 200, message: 'No data found', result: [] });
    }

  } catch (err) {
    res.log.error(err);
    res.send({ statusCode: 500, message: 'Internal server error' });
  }
}

export async function getAllAddonListHandler(req: FastifyRequest, res: FastifyReply) {
  try {
    const auth: any = req.headers;
    const body: any = { company_id: auth.companyId };
    // const body: any = { company_id:7577 };
    const response: any = await getAllAddonListModel(body);
    if (Array.isArray(response) && response.length > 0) {
      res.status(200).send({ statusCode: 200, message: 'Success', result: response });
    } else {
      res.status(200).send({ statusCode: 200, message: 'No data found', result: [] });
    }

  } catch (err) {
    res.log.error(err);
    res.send({ statusCode: 500, message: 'Internal server error' });
  }
}

export async function getUserOnboardSetting(req: FastifyRequest, res: FastifyReply) {
  try {

    const auth: any = req.headers;
    const body: any = { email_id: auth.username, domainId: auth.domainId };
    const response: any = await getUserOnboardSettingFromDb(body);
    if (response && response.length > 0) {
      res.status(200).send({ statusCode: 200, message: 'Success', result: response });
    } else {
      res.status(200).send({ statusCode: 422, message: 'No data found', result: [] });
    }

  } catch (err) {
    res.log.error(err);
    res.send({ statusCode: 500, message: 'Internal server error' });
  }
}
export async function getLiveInteraction(req: FastifyRequest, res: FastifyReply) {
  try {

    const auth: any = req.headers;
    const body: any = { domainId: auth.domainId };

    const response: any = await getLiveInteractionFromDb(body);

    if (response && response.length > 0) {
      res.status(200).send({ statusCode: 200, message: 'Success', result: response });
    } else {
      res.status(200).send({ statusCode: 422, message: 'No data found', result: [] });
    }

  } catch (err) {
    res.log.error(err);
    res.send({ statusCode: 500, message: 'Internal server error' });
  }
}

export async function insertUpdateDataAndPrivacySet(req: FastifyRequest, res: FastifyReply) {
  try {
    const auth: any = req.headers;
    const payload: any = req.body;
    const body: any = {
      ...payload,
      companyId: auth.companyId,
      domainId: auth.domainId
    };
    const response: any = await iinsertUpdateDataAndPrivacySetDao(body);
    if (response && (response[0]?.errCode === 0 || response[0]?.errCode === 1)) {
      res.status(200).send({ statusCode: 200, message: 'Success', result: response });
    } else {
      res.status(200).send({ statusCode: 404, message: response[0]?.errMsg || 'Insert/Update failed', result: [] });
    }
  } catch (err) {
    res.log.error(err);
    res.send({ statusCode: 500, message: 'Internal server error' });
  }
}

export async function getDataAndPrivacySet(req: FastifyRequest, res: FastifyReply) {
  try {

    const auth: any = req.headers;
    const payload: any = req.body;
    const body: any = {
      ...payload,
      companyId: auth.companyId,
      domainId: auth.domainId
    };
    const response: any = await getDataAndPrivacySetDao(body);
    if (response && response.length > 0) {
      res.status(200).send({ statusCode: 200, message: 'Success', result: response });
    } else {
      res.status(200).send({ statusCode: 404, message: 'No data found', result: [] });
    }

  } catch (err) {
    res.log.error(err);
    res.send({ statusCode: 500, message: 'Internal server error' });
  }
}

export async function getAllMembersRole(req: FastifyRequest, res: FastifyReply) {
  try {

    const auth: any = req.headers;
    const payload: any = req.body;
    const body: any = {
      ...payload,
      companyId: auth.companyId,
    };
    const response: any = await getAllMembersRoleDao(body);
    if (response && response.length > 0) {
      res.status(200).send({ statusCode: 200, message: 'Success', result: response });
    } else {
      res.status(200).send({ statusCode: 404, message: 'No data found', result: [] });
    }

  } catch (err) {
    res.log.error(err);
    res.send({ statusCode: 500, message: 'Internal server error' });
  }
}

export async function getUserDetailsByRole(req: FastifyRequest, res: FastifyReply) {
  try {

    const auth: any = req.headers;
    const payload: any = req.body;
    const body: any = {
      ...payload,
      domainId: auth.domainId,
    };
    const response: any = await getUserDetailsByRoleDao(body);
    if (response && response.length > 0) {
      res.status(200).send({ statusCode: 200, message: 'Success', result: response });
    } else {
      res.status(200).send({ statusCode: 404, message: 'No data found', result: [] });
    }

  } catch (err) {
    res.log.error(err);
    res.send({ statusCode: 500, message: 'Internal server error' });
  }
}

export async function insertUpdateMyplanRolePermission(req: FastifyRequest, res: FastifyReply) {
  try {
    const auth: any = req.headers;
    const payload: any = req.body;
    const body: any = {
      ...payload,
      domainId: auth.domainId
    };
    const response: any = await insertUpdateMyplanRolePermissionDao(body);
    if (response && (response[0]?.errcode === 0 || response[0]?.errcode === 1)) {
      res.status(200).send({ statusCode: 200, message: 'Success', result: response });
    } else {
      res.status(200).send({ statusCode: 404, message: response[0]?.errmsg || 'Insert/Update failed', result: [] });
    }
  } catch (err) {
    res.log.error(err);
    res.send({ statusCode: 500, message: 'Internal server error' });
  }
}

export async function getMyplanRolePermission(req: FastifyRequest, res: FastifyReply) {
  try {

    const auth: any = req.headers;
    const payload: any = req.body;
    const body: any = {
      domainId: auth.domainId,
    };
    const response: any = await getMyplanRolePermissionDao(body);
    if (response && response.length > 0) {
      res.status(200).send({ statusCode: 200, message: 'Success', result: response });
    } else {
      res.status(200).send({ statusCode: 404, message: 'No data found', result: [] });
    }

  } catch (err) {
    res.log.error(err);
    res.send({ statusCode: 500, message: 'Internal server error' });
  }
}

export async function getMyplanTeamsDetails(req: FastifyRequest, res: FastifyReply) {
  try {

    const auth: any = req.headers;
    const payload: any = req.body;
    const body: any = {
      ...payload,
      domainId: auth.domainId,
    };
    const response: any = await getTeamDetailsDao(body);
    if (response && response.length > 0) {
      res.status(200).send({ statusCode: 200, message: 'Success', result: response });
    } else {
      res.status(200).send({ statusCode: 404, message: 'No data found', result: [] });
    }

  } catch (err) {
    res.log.error(err);
    res.send({ statusCode: 500, message: 'Internal server error' });
  }
}

export async function insertUpdateMyplanTeamsPermission(req: FastifyRequest, res: FastifyReply) {
  try {
    const auth: any = req.headers;
    const payload: any = req.body;
    const body: any = {
      ...payload,
      domainId: auth.domainId
    };
    const response: any = await insertUpdateMyplanTeamsPermissionDao(body);
    console.log('insertUpdateMyplanTeamsPermissionDao', response);
    const daoResult = response?.[0]?.[0];

    if (daoResult && (daoResult.errcode === 0 || daoResult.errcode === 1)) {
      res.status(200).send({
        statusCode: 200,
        message: daoResult?.errmsg || 'Success',
        result: daoResult
      });
    } else {
      res.status(400).send({
        statusCode: 400,
        message: daoResult?.errmsg || 'Insert/Update failed',
        result: []
      });
    }

  } catch (err) {
    res.log.error(err);
    res.send({ statusCode: 500, message: 'Internal server error' });
  }
}

export async function getMyplanTeamPermission(req: any, res: any) {
  try {

    const auth: any = req.headers;
    const payload: any = req.body;
    const body: any = {
      qId:req?.body?.qId,
      domainId: auth.domainId,
    };
    const response: any = await getMyplanTeamPermissionDao(body);
    console.log('getMyplanTeamPermissionDao', response);
    if (response && response.length > 0) {
      res.status(200).send({ statusCode: 200, message: 'Success', result: response });
    } else {
      res.status(200).send({ statusCode: 404, message: 'No data found', result: [] });
    }

  } catch (err) {
    res.log.error(err);
    res.send({ statusCode: 500, message: 'Internal server error' });
  }
}

export async function getMyplanTeamLogActivity(req: any, res: any) {
  try {

    const auth: any = req.headers;
    const payload: any = req.body;
    const body: any = {
      domainId: auth.domainId,
      qId:req?.body?.qId,
    };
    const response: any = await getMyplanTeamLogActivityDao(body);
    console.log('getMyplanTeamPermissionDao', response);
    if (response && response.length > 0) {
      res.status(200).send({ statusCode: 200, message: 'Success', result: response });
    } else {
      res.status(200).send({ statusCode: 404, message: 'No data found', result: [] });
    }

  } catch (err) {
    res.log.error(err);
    res.send({ statusCode: 500, message: 'Internal server error' });
  }
}

export async function updateMyplanUserPermission(req: FastifyRequest, res: FastifyReply) {
  try {
    const auth: any = req.headers;
    const payload: any = req.body;
    const body: any = {
      ...payload,
      domainId: auth.domainId
    };
    const response: any = await iupdateMyplanUserPermissionDao(body);
    if (response && (response[0]?.errcode === 0 || response[0]?.errcode === 1)) {
      res.status(200).send({ statusCode: 200, message: 'Success', result: response });
    } else {
      res.status(200).send({ statusCode: 404, message: response[0]?.errmsg || 'Insert/Update failed', result: [] });
    }
  } catch (err) {
    res.log.error(err);
    res.send({ statusCode: 500, message: 'Internal server error' });
  }
}

export async function getMyplanUserPermission(req: FastifyRequest, res: FastifyReply) {
  try {
    const auth: any = req.headers;
    const payload: any = req.body;
    const body: any = {
      ...payload,
      domainId: auth.domainId,
    };
    const response: any = await getMyplanUserPermissionDao(body);
    if (response && response.length > 0) {
      res.status(200).send({ statusCode: 200, message: 'Success', result: response });
    } else {
      res.status(200).send({ statusCode: 404, message: 'No data found', result: [] });
    }
  } catch (err) {
    res.log.error(err);
    res.send({ statusCode: 500, message: 'Internal server error' });
  }
}

export async function getMyplanUserActivityLogActivity(req: any, res: any) {
  try {

    const auth: any = req.headers;
    const payload: any = req.body;
    const body: any = {
      domainId: auth.domainId,
      userId:req?.body?.userId,
    };
    const response: any = await getMyplanUserogActivityDao(body);
    console.log('getMyplanTeamPermissionDao', response);
    if (response && response.length > 0) {
      res.status(200).send({ statusCode: 200, message: 'Success', result: response });
    } else {
      res.status(200).send({ statusCode: 404, message: 'No data found', result: [] });
    }

  } catch (err) {
    res.log.error(err);
    res.send({ statusCode: 500, message: 'Internal server error' });
  }
}

export async function insertUserActivityLogHandler(req: any, res: any) {
  try {
    const auth: any = req.headers;
    const payload: any = req.body;
    const body: any = {
      userId:req?.body?.userId,
      roleId:req?.body?.roleId,
      domainId: auth.domainId,
      messagelog :req?.body?.messagelog
    };
    const response: any = await insertUserActivityLogFromDb(body);
    console.log('insertUserActivityLogFromDb>>', response);

    if (response[0]?.errcode === 0) {
      return res.code(200).send({
        statusCode: 200,
        message: 'Inserted successfully',
        result: response,
      });
    } if (response[0]?.errcode === -1) {
      return res.code(200).send({
        statusCode: 200,
        message: 'Updated successfully',
        result: response,
      });
    }

  } catch (err) {
    res.log.error(err);
    res.send({ statusCode: 500, message: 'Internal server error' });
  }
}

export async function getUserActivityLogHandler(req: any, res: any) {
  try {

    const auth: any = req.headers;
    const payload: any = req.body;
    const body: any = {
      userId:req?.body?.userId,
      roleId : req?.body?.roleId,
      domainId: auth.domainId,
      limit: req?.body?.limit,
      offset: req?.body?.offset
    };
    const response: any = await getUserogActivityLogDao(body);
    console.log('getUserogActivityLogDao', response);
    if (response && response[0].length > 0) {
      res.status(200).send({ statusCode: 200, message: 'Success', result: response[0] });
    } else {
      res.status(200).send({ statusCode: 404, message: 'No data found', result: [] });
    }

  } catch (err) {
    res.log.error(err);
    res.send({ statusCode: 500, message: 'Internal server error' });
  }
}
export async function insertUserDevicesHandler(req: any, res: any) {
  try {
    const auth: any = req.headers;
    const payload: any = req.body;
    const body: any = {
      companyId:req?.body?.companyId,
      userEmail:req?.body?.userEmail,
      device:req?.body?.device,
      deviceModal:req?.body?.deviceModal,
      osType:req?.body?.osType,
      browserType:req?.body?.browserType,
      ipAddress:req?.body?.ipAddress,
      location:req?.body?.location,
      isActive:req?.body?.isActive,
      productType:req?.body?.productType,
      deviceId:req?.body?.deviceId,
    };

    const response: any = await insertDevicesDtlDao(body);
    console.log('insertUserDevicesHandler>>', response);

    if (response[0]?.errCode === 0) {
      res.status(200).send({ statusCode: 200, message: 'Inserted successfully', result: response });
    } else if (response[0]?.errCode === 1) {
      res.status(200).send({ statusCode: 200, message: 'update successfully', result: response });
    } else {
      res.status(200).send({ statusCode: 200, message: RESPONSE.failed, result: [] });
    }

  } catch (err) {
    res.log.error(err);
    res.send({ statusCode: 500, message: 'Internal server error' });
  }
}

export async function getUserDevicesHandler(req: any, res: any) {
  try {
    const body: any = {
      userEmail:req?.body?.userEmail,
    };
    const response: any = await getUserDevicesDao(body);
    console.log('getUserDevicesHandler', response);
    if (response && response[0].length > 0) {
      res.status(200).send({ statusCode: 200, message: 'Success', result: response[0] });
    } else {
      res.status(200).send({ statusCode: 404, message: 'No data found', result: [] });
    }

  } catch (err) {
    res.log.error(err);
    res.send({ statusCode: 500, message: 'Internal server error' });
  }
}

export async function getCompanyBillingSubscription(req: FastifyRequest, res: FastifyReply) {
  try {
    const auth: any = req.headers;
    const body: any = { company_id: auth.companyId };
    const response: any = await getCompanyBilling(body);
    if (Array.isArray(response) && response.length > 0) {
      res.status(200).send({ statusCode: 200, message: 'Success', result: response });
    } else {
      res.status(200).send({ statusCode: 200, message: 'No data found', result: [] });
    }

  } catch (err) {
    res.log.error(err);
    res.send({ statusCode: 500, message: 'Internal server error' });
  }
}

export async function getStorageDataHandler(req: FastifyRequest, res: FastifyReply) {
  try {
    const auth: any = req.headers;
    const body: any = { domainId: auth.domainId };
    const response: any = await getStorageData(body);
    if (Array.isArray(response) && response?.length > 0) {
      res.status(200).send({ statusCode: 200, message: 'Success', result: response });
    } else {
      res.status(200).send({ statusCode: 200, message: 'No data found', result: [] });
    }

  } catch (err) {
    res.log.error(err);
    res.send({ statusCode: 500, message: 'Internal server error' });
  }
}

// forgot password

/**
 *  Forgot-Password functionality
 * @param req any
 * @param res  any
 * @param done  any
 */
export async function emailOtp(req: any, res: any, reply: any) {
  try {
    const data: any = {};
    data.email = String(req?.body?.email || '')
      .trim()
      .toLowerCase();

    if (!data.email) {
      return res.send({ statusCode: 400, message: 'email is required' });
    }
    const forgotPasswordRes: any = await getEmailOtpFromDb(data);

    if (forgotPasswordRes[0]?.errcode === 0) {
      const dbRow = forgotPasswordRes[0] || {};
      const otpValue =
        dbRow.verify_code ??
        dbRow.otp ??
        dbRow.otp_code ??
        dbRow.email_otp ??
        dbRow.verification_code ??
        dbRow.code ??
        req?.body?.otp ??
        '';
      const firstName = dbRow.userName || dbRow.first_name || 'User';

      const templatePath = resolveEmailTemplatePath('otpemail.html');
      let htmlContent = fs.readFileSync(templatePath, 'utf-8');

      // Inject OTP and recipient-specific values in template.
      htmlContent = htmlContent
        .replace('{{firstname}}', String(firstName))
        .replace('{{email}}', data.email)
        .replace('{{otp}}', String(otpValue));

      const mailParams = {
        email: data.email, // send OTP only to this particular email
        subject: 'Your OTP for email verification',
        html: htmlContent,
      };

      const finalMail: any = await SmtpSendMail(mailParams);
      if (finalMail.message === 'success') {
        return res.send({
          statusCode: 200,
          message:
            forgotPasswordRes[0]?.errmsg || 'OTP mail sent successfullysss',
        });
      }

      return res.send({
        statusCode: 422,
        message: 'OTP mail send failed',
        error: finalMail.error,
      });
    }

    return res.send({
      statusCode: 422,
      message: forgotPasswordRes[0]?.errmsg || 'OTP request failed',
    });
  } catch (err) {
    logger.error('emailOtp - Error:', err);
    res.send({ statusCode: 500, message: 'internal server error' });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * get verifyOtp details api
 */
export async function verifyOtp(req: any, res: any) {
  try {
    const data: any = {
      email: req?.body?.email,
      verify_code: req?.body?.verify_code,
    };
    const otpFromDb: any = await sendVerifyOtp(data);
    if (Array.isArray(otpFromDb) && otpFromDb.length > 0) {
      const result = otpFromDb[0];
      const code = Number(result?.errcode);
      const message = result?.errmsg;

      // ❌ Invalid OTP
      if (code === -1) {
        return res.status(401).send({
          message,
          statusCode: 401,
        });
      }

      // ✅ Success
      if (code === 1) {
        return res.status(200).send({
          statusCode: 200,
          message: 'OTP verified successfully',
          otp: message,
        });
      }
      return res.status(400).send({
        statusCode: 400,
        message: 'Unexpected response from server',
        debug: result,
      });
    }

    return res.status(404).send({
      statusCode: 404,
      message: 'No OTP data found',
    });

  } catch (err) {
    logger.error('verifyOtp - Unexpected Error:', err);
    return res.status(500).send({
      statusCode: 500,
      message: 'Internal server error',
    });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * get verifyOtp details api
 */
export async function passwordUpdate(req: any, res: any, done: any) {
  try {
    const data: any = {
      email: req?.body?.email,
      password: req?.body?.new_pwd,
    };
    const updatepasswordFromDb: any = await passwordUpdateFromDb(data);
    if (
      Array.isArray(updatepasswordFromDb) &&
      updatepasswordFromDb?.length > 0
    ) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        otp: updatepasswordFromDb[0]?.errmsg,
      });
    } else {
      res.status(404).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        otp: [],
      });
    }
  } catch (err) {
    logger.error('updatepassword - Unexpected Error:', req?.query, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * get verifyOtp details api
 */
export async function resendOtp(req: any, res: any) {
  try {
    const data: any = {};
    data.email = String(req?.body?.email || '')
      .trim()
      .toLowerCase();

    // ✅ Validate email
    if (!data.email) {
      return res.status(400).send({
        statusCode: 400,
        message: 'Email is required',
      });
    }
    const resendOtpFromDb: any = await resendOtpToDb(data);
    console.log('resend otp', resendOtpFromDb);

    if (
      Array.isArray(resendOtpFromDb) &&
      resendOtpFromDb.length > 0 &&
      resendOtpFromDb[0]?.errcode === 0
    ) {
      const dbRow = resendOtpFromDb[0] || {};

      const otpValue =
        dbRow.verify_code ||
        dbRow.otp ||
        dbRow.email_otp ||
        dbRow.code ||
        '';

      if (!otpValue) {
        return res.status(500).send({
          statusCode: 500,
          message: 'OTP generation failed',
        });
      }

      const firstName = dbRow.userName || dbRow.first_name || 'User';

      const templatePath = resolveEmailTemplatePath('otpemail.html');
      let htmlContent = await fs.promises.readFile(templatePath, 'utf-8');

      htmlContent = htmlContent
        .replace('{{firstname}}', String(firstName))
        .replace('{{email}}', data.email)
        .replace('{{otp}}', String(otpValue));
      const mailParams = {
        email: data.email,
        subject: 'Your OTP for email verification',
        html: htmlContent,
      };

      const finalMail: any = await SmtpSendMail(mailParams);

      if (finalMail.message === 'success') {
        return res.status(200).send({
          statusCode: 200,
          message: 'OTP resent successfully',
        });
      }

      return res.status(500).send({
        statusCode: 500,
        message: 'OTP mail send failed',
        error: finalMail.error,
      });
    }

    return res.status(422).send({
      statusCode: 422,
      message:
        resendOtpFromDb[0]?.errmsg || 'Resend OTP request failed',
    });

  } catch (err) {
    logger.error('resendOtp - Error:', err);
    return res.status(500).send({ statusCode: 500,  message: 'Internal server error', });
  }
}

export async function SmtpSendMail(params: any) {
  try {
    if (!nodemailer) {
      logger.warn('SmtpSendMail called but nodemailer is not installed');
      return { message: 'failed', error: 'nodemailer not installed' };
    }
    const transporter = nodemailer.createTransport({
      host: process.env.SMPT_HOST,
      port: Number(process.env.SMPT_PORT),
      secure: true, // 465 → true
      auth: {
        user: process.env.SMPT_USER,
        pass: process.env.SMPT_PASSWORD,
      },
    });

    const mailOptions = {
      from: `"${process.env.SMPT_FROM_NAME}" <${process.env.SMPT_FROM_EMAIL}>`,
      to: params.email,
      subject: params.subject || 'Reset Your Password',
      html:
        params.html ||
        `
        <div style="font-family: Arial, sans-serif;">
          <h3>Hello ${params.firstname},</h3>
          <p>You requested to reset your password.</p>
          <p>Click below to reset:</p>
          <a href="${params.password_reset_link}"
             style="background:#007bff;color:#fff;padding:10px 15px;text-decoration:none;border-radius:5px;">
             Reset Password
          </a>
          <p>If you did not request this, please ignore this email.</p>
        </div>
      `,
    };

    const info = await transporter.sendMail(mailOptions);

    return {
      message: 'success',
      messageId: info.messageId,
    };
  } catch (error: any) {
    console.error('SMTP Error:', error);

    return {
      message: 'failed',
      error: error.message,
    };
  }
}
