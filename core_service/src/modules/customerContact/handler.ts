/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

import {
  createColumnEditName,
  deleteCustomerContact,
  deleteMultipleCustomerContact,
  getAiConservationd,
  getBlockCustomerContact,
  getBlockCustomerRequestContact,
  getColumnEditName,
  getContactMobileLogsDao,
  getCustomerAppoinentBookingLogDao,
  getCustomerConatct,
  getCustomerConatctByDate,
  getCustomerConatctByEmail,
  getCustomerConatctByPhoneNumber,
  getCustomerConatctDetails,
  getCustomerConatctPhoneNumber,
  getCustomerContactChannels,
  getCustomerContactDao,
  getCustomerContactField,
  getCustomerContactFieldType,
  getCustomerTicketLogDao,
  getDoNotDisturbCustomerContact,
  getDoNotDisturbCustomerContactRequest,
  getMarketingContactId,
  insertBulkCustometConatct,
  insertBulkUpdateMarketing,
  insertCustomerContact,
  insertMergeCustomerContact,
  insertUpdateCustomerContact,
  insertUpdateCustomerContactPrimaryChat,
  insertUpdateMarketing,
  isValidContact,
  updateBlockCustomer,
  updateBlockCustomerReq,
  updateCustomerContact,
  updateCustomerContactTag,
  updateDoNotDisturb,
  updateDoNotDisturbReq,
} from '../../dao/customerContact';
import { RESPONSE } from '../../helpers/constants';
import { commonContactInsert, crmInsertContact } from '../../helpers/utils';
import { ioredisWhatsapp } from '../../plugins/db';
import { logger } from '../../plugins/winston';

/**
 *
 * @param req
 * @param res
 * @param done
 * agent status api
 */
export async function insertCustomerContactHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      domainId: auth?.domainId,
      firstName: req?.body?.firstName ?? null,
      lastName: req?.body?.lastName ?? null,
      phoneNumber: req?.body?.phoneNumber ?? null,
      email: req?.body?.email ?? null,
      accountNumber: req?.body?.accountNumber ?? null,
      tags: req?.body?.tags ?? null,
      location: req?.body?.location ?? null,
      address: req?.body?.address ?? null,
      country: req?.body?.country ?? null,
      state: req?.body?.state ?? null,
      city: req?.body?.city ?? null,
      postalCode: req?.body?.postalCode ?? null,
      facebook: req?.body?.facebook ?? null,
      instagram: req?.body?.instagram ?? null,
      whatsapp: req?.body?.whatsapp ?? null,
      twitter: req?.body?.twitter ?? null,
      voiceBiometric: req?.body?.voiceBiometric ?? null,
      customerType: req?.body?.customerType ?? null,
      timeZone: req?.body?.timeZone ?? null
    };
    const insertContact: any = await insertCustomerContact(data);
    logger.info('insertCustomerContactHandler res', insertContact);
    if (insertContact[0].errcode === 1) {
      res.status(200).send({ statusCode: 404, message: RESPONSE.User_already_exist });
    } else if (insertContact[0].errcode === 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.contact, insertContactList: data });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {
    logger.error('insertCustomerContactHandler - Unexpected Error:', req.headers, req?.body, err);
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
export async function insertCustomerContactMyaccHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      domainId: req?.body?.domainId,
      firstName: req?.body?.firstName ?? null,
      lastName: req?.body?.lastName ?? null,
      phoneNumber: req?.body?.phoneNumber ?? null,
      email: req?.body?.email ?? null,
      accountNumber: req?.body?.accountNumber ?? null,
      tags: req?.body?.tags ?? null,
      location: req?.body?.location ?? null,
      address: req?.body?.address ?? null,
      country: req?.body?.country ?? null,
      state: req?.body?.state ?? null,
      city: req?.body?.city ?? null,
      postalCode: req?.body?.postalCode ?? null,
      facebook: req?.body?.facebook ?? null,
      instagram: req?.body?.instagram ?? null,
      whatsapp: req?.body?.whatsapp ?? null,
      twitter: req?.body?.twitter ?? null,
      voiceBiometric: req?.body?.voiceBiometric ?? null,
      customerType: req?.body?.customerType ?? null,
      timeZone: req?.body?.timeZone ?? null
    };
    const insertContact: any = await insertCustomerContact(data);
    logger.info('insertCustomerContactMyaccHandler res', insertContact);
    if (insertContact[0].errcode === 1) {
      res.status(200).send({ statusCode: 404, message: RESPONSE.User_already_exist });
    } else if (insertContact[0].errcode === 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.contact, insertContactList: data });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {
    logger.error('insertCustomerContactMyaccHandler - Unexpected Error:', req?.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getCustomerConatctHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {};
    data.domainId = auth.domainId;
    data.newCustomer = req?.params?.newCustomer === 'null' ? null : req?.params?.newCustomer;
    const getConatct: any = await getCustomerConatct(data);
    logger.info('getCustomerConatct', getConatct);
    if (Array.isArray(getConatct) && getConatct.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getConatctListRes: getConatct[0] });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getConatctListRes: [] });
    }
  } catch (err) {
    logger.error('getCustomerConatctHandler - Unexpected Error:', req?.params, req.headers, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getCustomerContactByCoidHandler(req: any, res: any, done: any) {
  try {
    const data =  req?.params?.coid ?? null;
    const getContact: any = await getCustomerContactDao(data);
    logger.info('getCustomerContactDao', getContact);
    if (Array.isArray(getContact) && getContact.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getContactData: getContact });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getConatctListRes: [] });
    }
  } catch (err) {
    logger.error('getCustomerContactByCoidHandler - Unexpected Error:', req?.params, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getCustomerConatctDetailsHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const coid = req.params.coid;
    const data: any = {
      domainId: auth.domainId,
    };
    if (coid !== '') {
      const getCustomer: any = await getCustomerConatctDetails(data, coid);
      logger.info('getCustomerConatctDetails', getCustomer);
      if (Array.isArray(getCustomer) && getCustomer.length > 0) {
        res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getContactDetailsRes: getCustomer });
      } else {
        res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getContactDetailsRes: [] });
      }
    } else {
      res.status(200).send({ statusCode: 405, message: RESPONSE.coid });
    }
  } catch (err) {
    logger.error('getCustomerConatctDetailsHandler - Unexpected Error:', req?.params, req.headers, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * update contact api
 */
export async function updateCustomerContactHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const coid = req.params.coid;
    const data: any = {
      coid,
      domainId: auth.domainId,
      firstName: req?.body?.firstName ?? null,
      lastName: req?.body?.lastName ?? null,
      phoneNumber: req?.body?.phoneNumber ?? null,
      email: req?.body?.email ?? null,
      accountNumber: req?.body?.accountNumber ?? null,
      tags: req?.body?.tags ?? null,
      location: req?.body?.location ?? null,
      address: req?.body?.address ?? null,
      country: req?.body?.country ?? null,
      state: req?.body?.state ?? null,
      city: req?.body?.city ?? null,
      postalCode: req?.body?.postalCode ?? null,
      facebook: req?.body?.facebook ?? null,
      instagram: req?.body?.instagram ?? null,
      whatsapp: req?.body?.whatsapp ?? null,
      twitter: req?.body?.twitter ?? null,
      voiceBiometric: req?.body?.voiceBiometric ?? null,
      mergeCustomer: req?.body?.mergeCustomer ?? 0,
      blockCustomer: req?.body?.blockCustomer ?? 0,
      doNotDisturb: req?.body?.doNotDisturb ?? 0,
      blockCustomerReq: req?.body?.blockCustomerReq ?? 0,
      doNotDisturbReq: req?.body?.doNotDisturbReq ?? 0,
      customerType: req?.body?.customerType ?? null,
    };
    if (coid) {
      const updateStatus: any = await updateCustomerContact(data);
      logger.info('updateCustomerContactHandler res', updateStatus);
      if (updateStatus[0].errcode === 0) {
        res.status(200).send({ statusCode: 200, message: RESPONSE.update_contact });
      } else {
        res.status(200).send({ statusCode: 404, message: RESPONSE.not_update });
      }
    } else {
      res.status(200).send({ statusCode: 405, message: RESPONSE.coid });
    }
  } catch (err) {
    logger.error('updateCustomerContactHandler - Unexpected Error:', req?.params, req.headers, req?.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function deleteCustomerContactHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const coid: any = req.params.coid;
    const data = {
      domainId: auth.domainId
    };
    if (coid !== '') {
      const deleteAgent: any = await deleteCustomerContact(data, coid);
      logger.info('deleteCustomerContact res', deleteAgent);
      if (deleteAgent.affectedRows === 1) {
        ioredisWhatsapp.to(`${data?.domainId}`).emit('customerContactEmitter');
        res.status(200).send({ statusCode: 200, message: RESPONSE.delete_contact });
      } else {
        res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
      }
    } else {
      res.status(200).send({ statusCode: 405, message: RESPONSE.aid });
    }
  } catch (err) {
    logger.error('deleteCustomerContactHandler - Unexpected Error:', req?.params, req.headers, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function isValidContactHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {};
    data.domainId = auth.domainId;
    data.email = req?.body?.email ?? null;
    const contactRes: any = await isValidContact(data);
    logger.info('isValidContact', contactRes);
    if (contactRes && contactRes.length > 0 && contactRes[0]?.email.length > 0) {
      res.status(200).send({ statusCode: 403, message: RESPONSE.email_already_exist, flag: 0 });
    } else {
      res.status(200).send({ statusCode: 200, message: RESPONSE.available, flag: 1 });
    }
  } catch (err) {
    logger.error('isValidContactHandler - Unexpected Error:', req?.params, req.headers, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getBlockCustomerContactHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {};
    data.domainId = auth.domainId;
    const getBlock: any = await getBlockCustomerContact(data);
    logger.info('getBlockCustomerContact', getBlock);
    if (Array.isArray(getBlock) && getBlock.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getBlockCustomerListRes: getBlock });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getBlockCustomerListRes: [] });
    }
  } catch (err) {
    logger.error('getBlockCustomerContactHandler - Unexpected Error:', req.headers, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getDoNotDisturbCustomerContactHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {};
    data.domainId = auth.domainId;
    const getDoNotDisturb: any = await getDoNotDisturbCustomerContact(data);
    logger.info('getDoNotDisturbCustomerContact', getDoNotDisturb);
    if (Array.isArray(getDoNotDisturb) && getDoNotDisturb.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getDoNotDisturbCustomerListRes: getDoNotDisturb });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getDoNotDisturbCustomerListRes: [] });
    }
  } catch (err) {
    logger.error('getDoNotDisturbCustomerContactHandler - Unexpected Error:', req.headers, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getBlockCustomerRequestContactHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {};
    data.domainId = auth.domainId;
    const getBlockReq: any = await getBlockCustomerRequestContact(data);
    logger.info('getBlockCustomerRequestContact', getBlockReq);
    if (Array.isArray(getBlockReq) && getBlockReq.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getBlockCustomerRequestListRes: getBlockReq });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getBlockCustomerRequestListRes: [] });
    }
  } catch (err) {
    logger.error('getBlockCustomerRequestContactHandler - Unexpected Error:', req.headers, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getDoNotDisturbCustomerContactRequestHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {};
    data.domainId = auth.domainId;
    const getDoNotDisturbRes: any = await getDoNotDisturbCustomerContactRequest(data);
    logger.info('getDoNotDisturbCustomerContactRequest', getDoNotDisturbRes);
    if (Array.isArray(getDoNotDisturbRes) && getDoNotDisturbRes.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getDoNotDisturbCustomerReqListRes: getDoNotDisturbRes });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getDoNotDisturbCustomerReqListRes: [] });
    }
  } catch (err) {
    logger.error('getDoNotDisturbCustomerContactRequestHandler - Unexpected Error:', req.headers, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * merge customer status api
 */
export async function insertMergeCustomerContactHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      domainId: auth.domainId,
      coid: req?.body?.coid ?? 0,
      deleteCoid: req?.body?.deleteCoid ?? 0,
      firstName: req?.body?.firstName ?? null,
      lastName: req?.body?.lastName ?? null,
      phoneNumber: req?.body?.phoneNumber ?? null,
      email: req?.body?.email ?? null,
      tags: req?.body?.tags ?? null,
      location: req?.body?.location ?? null,
      address: req?.body?.address ?? null,
      country: req?.body?.country ?? null,
      state: req?.body?.state ?? null,
      city: req?.body?.city ?? null,
      postalCode: req?.body?.postalCode ?? null,
      facebook: req?.body?.facebook ?? null,
      instagram: req?.body?.instagram ?? null,
      whatsapp: req?.body?.whatsapp ?? null,
      primaryPhoneNumber: req?.body?.primaryPhoneNumber ?? null,
      primaryEmail: req?.body?.primaryEmail ?? null,
      blockCustomer: req?.body?.blockCustomer ?? 0,
      blockCustomerReq: req?.body?.blockCustomerReq ?? 0,
      doNotDisturb: req?.body?.doNotDisturb ?? 0,
      doNotDisturbReq: req?.body?.doNotDisturbReq ?? 0,
      customerType: req?.body?.customerType ?? null
    };
    const insertMergeContact: any = await insertMergeCustomerContact(data);
    logger.info('insertMergeCustomerContact', insertMergeContact);
    if (insertMergeContact[0]?.errcode === 0) {
      ioredisWhatsapp.to(`${data?.domainId}`).emit('customerContactEmitter');
      res.status(200).send({ statusCode: 200, message: RESPONSE.merge_contact });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_merge });
    }
  } catch (err) {
    logger.error('insertMergeCustomerContactHandler - Unexpected Error:', req.headers, req?.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * update block api
 */
export async function updateBlockCustomerHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const coid = req.params.coid;
    const data: any = {
      domainId: auth.domainId,
      blockCustomer: req?.body?.blockCustomer ?? 0,
      blockedBy: req?.body?.blockedBy ?? 0,
      Reason: req?.body?.reason ?? null,
    };
    if (coid !== '') {
      const updateStatus: any = await updateBlockCustomer(data, coid);
      logger.info('updateBlockCustomer', updateStatus);
      if (updateStatus[0].errCode === 0) {
        ioredisWhatsapp.to(`${data?.domainId}`).emit('customerContactEmitter');
        res.status(200).send({ statusCode: 200, message: RESPONSE.update_block });
      } else {
        res.status(200).send({ statusCode: 404, message: RESPONSE.not_update });
      }
    } else {
      res.status(200).send({ statusCode: 405, message: RESPONSE.coid });
    }
  } catch (err) {
    logger.error('updateBlockCustomerHandler - Unexpected Error:', req.headers, req?.body, req.params, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * update do not customer api
 */
export async function updateDoNotDisturbHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const coid = req.params.coid;
    const data: any = {
      domainId: auth.domainId,
      doNotDisturb: req?.body?.doNotDisturb ?? 0,
      doNotDisturbBy: req?.body?.doNotDisturbBy ?? 0,
      Reason: req?.body?.reason ?? null,
    };
    if (coid !== '') {
      const updateStatus: any = await updateDoNotDisturb(data, coid);
      logger.info('updateDoNotDisturb', updateStatus);
      if (updateStatus[0].errCode === 0) {
        ioredisWhatsapp.to(`${data?.domainId}`).emit('customerContactEmitter');
        res.status(200).send({ statusCode: 200, message: RESPONSE.update_doNot });
      } else {
        res.status(200).send({ statusCode: 404, message: RESPONSE.not_update });
      }
    } else {
      res.status(200).send({ statusCode: 405, message: RESPONSE.coid });
    }
  } catch (err) {
    logger.error('updateDoNotDisturbHandler - Unexpected Error:', req.headers, req?.body, req.params, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * update block customer req api
 */
export async function updateBlockCustomerReqHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const coid = req.params.coid;
    const data: any = {
      domainId: auth.domainId,
      blockCustomerReq: req?.body?.blockCustomerReq ?? 0,
      blockedReqBy: req?.body?.blockedReqBy ?? 0,
      Reason: req?.body?.reason ?? null,
    };
    if (coid !== '') {
      const updateStatus: any = await updateBlockCustomerReq(data, coid, auth);
      logger.info('updateBlockCustomerReq res', updateStatus);
      if (updateStatus[0].errcode === 0) {
        ioredisWhatsapp.to(`${data?.domainId}`).emit('customerContactEmitter');
        res.status(200).send({ statusCode: 200, message: RESPONSE.update_doNot });
      } else {
        res.status(200).send({ statusCode: 404, message: RESPONSE.not_update });
      }
    } else {
      res.status(200).send({ statusCode: 405, message: RESPONSE.coid });
    }
  } catch (err) {
    logger.error('updateBlockCustomerReqHandler - Unexpected Error:', req.headers, req?.body, req.params, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * update block customer req api
 */
export async function updateDoNotDisturbReqHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const coid = req.params.coid;
    const data: any = {
      domainId: auth.domainId,
      doNotDisturbReq: req?.body?.doNotDisturbReq ?? 0,
      doNotDisturbReqBy: req?.body?.doNotDisturbReqBy ?? 0,
      Reason: req?.body?.reason ?? null,
    };
    if (coid !== '') {
      const updateStatus: any = await updateDoNotDisturbReq(data, coid, auth);
      logger.info('updateDoNotDisturbReq', updateStatus);
      if (updateStatus[0].errcode === 0) {
        ioredisWhatsapp.to(`${data?.domainId}`).emit('customerContactEmitter');
        res.status(200).send({ statusCode: 200, message: RESPONSE.update_doNot });
      } else {
        res.status(200).send({ statusCode: 404, message: RESPONSE.not_update });
      }
    } else {
      res.status(200).send({ statusCode: 405, message: RESPONSE.coid });
    }
  } catch (err) {
    logger.error('updateDoNotDisturbReqHandler - Unexpected Error:', req.headers, req?.body, req.params, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 * bulk data
 * @param req
 * @param res
 * @param done
 */
export async function insertBulkCustometConatctHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data = req?.body ?? [];
    const insertConatct: any = await insertBulkCustometConatct(data, auth);
    logger.info('insertBulkCustometConatct', insertConatct);
    if (insertConatct) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.insert_customer });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
    const insertMarketingContact: any = await insertBulkUpdateMarketing(data, auth);
    if (insertMarketingContact) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.insert_customer_marketing });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {
    logger.error('insertBulkCustometConatctHandler - Unexpected Error:', req.headers, req?.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getCustomerConatctPhoneNumberHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const phoneNumber = req.params.phoneNumber;
    const data: any = {
      domainId: auth.domainId,
    };
    if (phoneNumber !== '') {
      const getContactNumber: any = await getCustomerConatctPhoneNumber(data, phoneNumber);
      logger.info('getCustomerConatctPhoneNumber', getContactNumber);
      if (Array.isArray(getContactNumber) && getContactNumber.length > 0) {
        res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getContactNumberRes: getContactNumber });
      } else {
        res.status(200).send({ statusCode: 200, message: 'User is New One. Created account for that user', getContactNumberRes: getContactNumber });
      }
    } else {
      res.status(200).send({ statusCode: 405, message: RESPONSE.coid });
    }
  } catch (err) {
    logger.error('getCustomerConatctPhoneNumberHandler - Unexpected Error:', req.headers, req?.params, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */

export async function createCustomerEditHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {
      domainId: auth.domainId,
      ext: auth.ext,
      editColumnNames: req?.body?.editColumnNames ?? [],
    };
    await createColumnEditName(data);
    res.status(200).send({ statusCode: 200, message: RESPONSE.success_message });
  } catch (err) {
    logger.error('createCustomerEditHandler - Unexpected Error:', req.headers, req?.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}
/**
 *
 * @param req
 * @param res
 * @param done
 */

export async function getCustomerEditHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {
      domainId: auth.domainId,
      ext: auth.ext,
    };
    const getConatctRes = await getColumnEditName(data);
    logger.info('getColumnEditName', getConatctRes);
    if (Array.isArray(getConatctRes) && getConatctRes.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getCustomerConatct: getConatctRes[0] });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {
    logger.error('getCustomerEditHandler - Unexpected Error:', req.headers, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function updateCustomerContactTagHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const dataq = req.params.coid;
    const data: any = {
      domainId: auth.domainId,
      tags: req?.body?.tags ?? null,
    };
    {
      const contactTag: any = await updateCustomerContactTag(data, dataq);
      logger.info('updateCustomerContactTag', contactTag);
      if (contactTag[0].errcode === 0) {
        res.status(200).send({ statusCode: 200, message: RESPONSE.update_block });
      } else {
        res.status(200).send({ statusCode: 404, message: RESPONSE.not_update });
      }
    }
  } catch (err) {
    logger.error('updateCustomerContactTagHandler - Unexpected Error:', req.headers, req.params, req?.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * customer contact information
 */
export async function insertUpdateCustomerContactHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req?.headers;
    const data: any = {
      coid: req.body.coid ?? 0,
      firstName: req?.body?.firstName ?? null,
      lastName: req?.body?.lastName ?? null,
      phoneNumber: req?.body?.phoneNumber ?? null,
      email: req?.body?.email ?? null,
      accountNumber: req?.body?.accountNumber ?? null,
      tags: req?.body?.tags ?? null,
      location: req?.body?.location ?? null,
      address: req?.body?.address ?? null,
      country: req?.body?.country ?? null,
      state: req?.body?.state ?? null,
      city: req?.body?.city ?? null,
      postalCode: req?.body?.postalCode ?? null,
      facebook: req?.body?.facebook ?? null,
      instagram: req?.body?.instagram ?? null,
      whatsapp: req?.body?.whatsapp ?? null,
      twitter: req?.body?.twitter ?? null,
      voiceBiometric: req?.body?.voiceBiometric ?? null,
      domainId: auth?.domainId ? auth.domainId : req.body?.domainId,
      mergeCustomer: req?.body?.mergeCustomer ?? null,
      blockCustomer: req?.body?.blockCustomer ?? 0,
      doNotDisturb: req?.body?.doNotDisturb ?? 0,
      blockCustomerReq: req?.body?.blockCustomerReq ?? 0,
      doNotDisturbReq: req?.body?.doNotDisturbReq ?? 0,
      type: req?.body?.type ?? null,
      currentCustomerNumber: req?.body?.currentCustomerNumber ?? null,
      customerType: req?.body?.customerType ?? null,
      timeZone: req?.body?.timeZone ?? null,
      cookiesId: req?.body?.cookiesId ?? null,
      source: req?.body?.source ?? null,
      customField: req?.body?.customField ?? null,
      customValues: req?.body?.customValues ?? null,
      primaryPhoneNumber: req?.body?.primaryPhoneNumber ?? null,
      primaryEmail: req?.body?.primaryEmail ?? null,
    };
    const insertContact: any = await insertUpdateCustomerContact(data);
    logger.info('insertUpdateCustomerContact', insertContact);
    if (insertContact && Array.isArray(insertContact) && insertContact?.length > 0 && insertContact[0]?.errcode === 0) {
      ioredisWhatsapp.to(`${data?.domainId}`).emit('customerContactEmitter', data);
      res.status(200).send({ statusCode: 200, message: RESPONSE.contact, insertContactList: data, insertContactResult: insertContact[0] });
    } else if (insertContact && Array.isArray(insertContact) && insertContact?.length > 0 && insertContact[0]?.errcode === -1) {
      res.status(200).send({ statusCode: 404, message: RESPONSE.contactExists, insertContactResult: insertContact[0] });
    } else {
      ioredisWhatsapp.to(`${data?.domainId}`).emit('customerContactEmitter', data);
      res.status(200).send({ statusCode: 200, message: RESPONSE.update_contact, insertContactList: data, insertContactResult: insertContact[0] });
    }
    if (insertContact?.[0] && insertContact[0]?.errcode !== -1) {
      commonContactInsert(req?.body, auth, insertContact[0]);
    }
  } catch (err) {
    logger.error('insertUpdateCustomerContactHandler - Unexpected Error:', req.headers, req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * customer contact information
 */
export async function insertUpdateCustomerContactPrimaryChatHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req?.headers;
    const data: any = {
      coid: req?.body?.coid ?? null,
      firstName: req?.body?.firstName ?? null,
      lastName: req?.body?.lastName ?? null,
      phoneNumber: req?.body?.phoneNumber ?? null,
      email: req?.body?.email ?? null,
      accountNumber: req?.body?.accountNumber ?? null,
      tags: req?.body?.tags ?? null,
      location: req?.body?.location ?? null,
      address: req?.body?.address ?? null,
      country: req?.body?.country ?? null,
      state: req?.body?.state ?? null,
      city: req?.body?.city ?? null,
      postalCode: req?.body?.postalCode ?? null,
      facebook: req?.body?.facebook ?? null,
      instagram: req?.body?.instagram ?? null,
      whatsapp: req?.body?.whatsapp ?? null,
      twitter: req?.body?.twitter ?? null,
      voiceBiometric: req?.body?.voiceBiometric ?? null,
      domainId: auth?.domainId ? auth.domainId : req.body?.domainId,
      mergeCustomer: req?.body?.mergeCustomer ?? null,
      blockCustomer: req?.body?.blockCustomer ?? null,
      doNotDisturb: req?.body?.doNotDisturb ?? 0,
      blockCustomerReq: req?.body?.blockCustomerReq ?? 0,
      doNotDisturbReq: req?.body?.doNotDisturbReq ?? 0,
      type: req?.body?.type ?? null,
      currentCustomerNumber: req?.body?.currentCustomerNumber ?? null,
      customerType: req?.body?.customerType ?? null,
      timeZone: req?.body?.timeZone ?? null,
      cookiesId: req?.body?.cookiesId ?? null,
      source: req?.body?.source ?? null,
      customField: req?.body?.customField ?? null,
      customValues: req?.body?.customValues ?? null,
      primaryPhoneNumber: req?.body?.primaryPhoneNumber ?? null,
      primaryEmail: req?.body?.primaryEmail ?? null,
    };
    const insertContact: any = await insertUpdateCustomerContactPrimaryChat(data);
    logger.info('insertUpdateCustomerContactPrimaryChat', insertContact);
    if (insertContact[0]?.errcode === -2) {
      return res.status(200).send({ statusCode: 403, message: insertContact[0]?.errmsg, insertContactResult: insertContact[0] });
    }
    if (insertContact && Array.isArray(insertContact) && insertContact?.length > 0 && insertContact[0]?.errcode === 0) {
      ioredisWhatsapp.to(`${data?.domainId}`).emit('customerContactEmitter');
      res.status(200).send({ statusCode: 200, message: RESPONSE.contact, insertContactList: data, insertContactResult: insertContact[0] });
    } else if (insertContact && Array.isArray(insertContact) && insertContact?.length > 0 && insertContact[0]?.errcode === -1) {
      res.status(200).send({ statusCode: 404, message: RESPONSE.contactExists, insertContactResult: insertContact[0] });
    } else {
      res.status(200).send({ statusCode: 200, message: RESPONSE.update_contact, insertContactList: data, insertContactResult: insertContact[0] });
    }

    if (insertContact?.[0] && insertContact[0]?.errcode !== -1 && insertContact[0]?.errcode !== -2) {
      commonContactInsert(req?.body, auth, insertContact[0]);
    }
  } catch (err) {
    logger.error('insertUpdateCustomerContactPrimaryChatHandler - Unexpected Error:', req.headers, req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * customer contact information
 */
export async function insertUpdateCustomerContactCallbackHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      coid: req.body.coid ?? 0,
      firstName: req?.body?.firstName ?? null,
      lastName: req?.body?.lastName ?? null,
      phoneNumber: req?.body?.phoneNumber ?? null,
      email: req?.body?.email ?? null,
      accountNumber: req?.body?.accountNumber ?? null,
      tags: req?.body?.tags ?? null,
      location: req?.body?.location ?? null,
      address: req?.body?.address ?? null,
      country: req?.body?.country ?? null,
      state: req?.body?.state ?? null,
      city: req?.body?.city ?? null,
      postalCode: req?.body?.postalCode ?? null,
      facebook: req?.body?.facebook ?? null,
      instagram: req?.body?.instagram ?? null,
      whatsapp: req?.body?.whatsapp ?? null,
      twitter: req?.body?.twitter ?? null,
      voiceBiometric: req?.body?.voiceBiometric ?? null,
      domainId: req.body?.domainId,
      mergeCustomer: req?.body?.mergeCustomer ?? null,
      blockCustomer: req?.body?.blockCustomer ?? 0,
      doNotDisturb: req?.body?.doNotDisturb ?? 0,
      blockCustomerReq: req?.body?.blockCustomerReq ?? 0,
      doNotDisturbReq: req?.body?.doNotDisturbReq ?? 0,
      type: req?.body?.type ?? null,
      currentCustomerNumber: req?.body?.currentCustomerNumber ?? null,
      customerType: req?.body?.customerType ?? null,
      timeZone: req?.body?.timeZone ?? null,
      cookiesId: req?.body?.cookiesId ?? null,
      source: req?.body?.source ?? null,
      customField: req?.body?.customField ?? null,
      customValues: req?.body?.customValues ?? null,
      primaryPhoneNumber: req?.body?.primaryPhoneNumber ?? null,
      primaryEmail: req?.body?.primaryEmail ?? null,
    };
    const insertContact: any = await insertUpdateCustomerContact(data);
    logger.info('insertUpdateCustomerContact', insertContact);
    if (insertContact && Array.isArray(insertContact) && insertContact?.length > 0 && insertContact[0]?.errcode === 0) {
      ioredisWhatsapp.to(`${data?.domainId}`).emit('customerContactEmitter', data);
      res.status(200).send({ statusCode: 200, message: RESPONSE.contact, insertContactList: data, insertContactResult: insertContact[0] });
    } else if (insertContact && Array.isArray(insertContact) && insertContact?.length > 0 && insertContact[0]?.errcode === -1) {
      res.status(200).send({ statusCode: 404, message: RESPONSE.contactExists, insertContactResult: insertContact[0] });
    } else {
      ioredisWhatsapp.to(`${data?.domainId}`).emit('customerContactEmitter', data);
      res.status(200).send({ statusCode: 200, message: RESPONSE.update_contact, insertContactList: data, insertContactResult: insertContact[0] });
    }
  } catch (err) {
    logger.error('insertUpdateCustomerContactCallbackHandler - Unexpected Error:', req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getCustomerConatctByEmailHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const email = req.params.email;
    const data: any = {
      domainId: auth.domainId,
    };
    if (email !== '') {
      const getContactEmail: any = await getCustomerConatctByEmail(data, email);
      logger.info('getCustomerConatctByEmail', getContactEmail);
      if (Array.isArray(getContactEmail) && getContactEmail.length > 0) {
        res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getContactEmailRes: getContactEmail });
      } else {
        res.status(200).send({ statusCode: 200, message: RESPONSE.not_found, getContactEmailRes: [] });
      }
    } else {
      res.status(200).send({ statusCode: 405, message: RESPONSE.email });
    }
  } catch (err) {
    logger.error('getCustomerConatctByEmailHandler - Unexpected Error:', req.headers, req.params, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getCustomerConatctByPhoneNumberHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const phoneNumber = req.params.phoneNumber;
    const data: any = {
      domainId: auth.domainId,
    };
    if (phoneNumber !== '') {
      const getContactPhoneNUmber: any = await getCustomerConatctByPhoneNumber(data, phoneNumber);
      logger.info('getCustomerConatctByPhoneNumber', getContactPhoneNUmber);
      if (Array.isArray(getContactPhoneNUmber) && getContactPhoneNUmber.length > 0) {
        res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getContactPhoneNumberRes: getContactPhoneNUmber });
      } else {
        res.status(200).send({ statusCode: 200, message: RESPONSE.not_found, getContactPhoneNumberRes: [] });
      }
    } else {
      res.status(200).send({ statusCode: 405, message: RESPONSE.phoneNumber });
    }
  } catch (err) {
    logger.error('getCustomerConatctByPhoneNumberHandler - Unexpected Error:', req.headers, req.params, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getCustomerContactChannelsHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {
      domainId: auth.domainId,
      chennalType: req?.body?.chennalType,
      channelValue: req?.body?.channelValue
    };
    const getContactNumber: any = await getCustomerContactChannels(data);
    logger.info('getCustomerContactChannels', getContactNumber);
    if (Array.isArray(getContactNumber) && getContactNumber.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getContactNumberRes: getContactNumber });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getContactNumberRes: [] });
    }
  } catch (err) {
    logger.error('getCustomerContactChannelsHandler - Unexpected Error:', req.headers, req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function deleteMultipleCustomerContactHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data = {
      coid: req?.body?.coid ?? '',
      domainId: auth.domainId,
    };
    const deleteAgent: any = await deleteMultipleCustomerContact(data);
    logger.info('deleteMultipleCustomerContact', deleteAgent);
    if (deleteAgent[0]?.errcode === 0) {
      ioredisWhatsapp.to(`${data?.domainId}`).emit('customerContactEmitter');
      res.status(200).send({ statusCode: 200, message: RESPONSE.delete_contact });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.contact_not_deleted });
    }
  } catch (err) {
    logger.error('deleteMultipleCustomerContactHandler - Unexpected Error:', req.headers, req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */

export async function getCustomerContactFieldTypeHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const getFieldType = await getCustomerContactFieldType();
    logger.info('getCustomerContactFieldType', getFieldType);
    if (Array.isArray(getFieldType) && getFieldType.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getCustomerConatctFieldType: getFieldType });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getCustomerConatctFieldType: [] });
    }
  } catch (err) {
    logger.error('getCustomerContactFieldTypeHandler - Unexpected Error:', req.headers, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */

export async function getCustomerContactFieldHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {
      domainId: auth.domainId,
      screenId: req?.params?.screenId ?? null,
      isArchive: req?.params?.isArchive ?? null
    };
    const getField = await getCustomerContactField(data);
    logger.info('getCustomerContactField', getField);
    if (Array.isArray(getField) && getField.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getCustomerConatctField: getField });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getCustomerConatctField: [] });
    }
  } catch (err) {
    logger.error('getCustomerContactFieldHandler - Unexpected Error:', req.headers, req?.params, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getCustomerConatctPaginationHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {
      domainId : auth.domainId,
      limit : req?.body?.limit ?? null,
      offset : req?.body?.offset ?? 0,
      newCustomer : req?.body?.newCustomer ?? null,
      search: req?.body?.search ?? null,
      c_type: req?.body?.c_type ?? null,
      aditionalColumn: req?.body?.aditionalColumn ?? null,
      fromDate: req?.body?.fromDate ?? null,
      toDate: req?.body?.toDate ?? null
    };
    const getConatct: any = await getCustomerConatct(data);
    logger.info('getCustomerConatct', getConatct);
    const totalDataCount: any = getConatct?.[1]?.[0]?.totalCount || 0;
    if (Array.isArray(getConatct) && getConatct.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getConatctListRes: getConatct[0], totalCount: totalDataCount });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getConatctListRes: [], totalCount: 0 });
    }
  } catch (err) {
    logger.error('getCustomerConatctPaginationHandler - Unexpected Error:', req.headers, req?.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getCustomerConatctByDateHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {
      fromDate : req?.body?.fromDate,
      toDate :req?.body?.toDate,
      domainId : auth.domainId,
    };

    const getConatct: any = await getCustomerConatctByDate(data);
    logger.info('getCustomerConatct', getConatct);
    if (Array.isArray(getConatct) && getConatct.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getConatctListRes: getConatct[0] });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getConatctListRes: [], totalCount: 0 });
    }
  } catch (err) {
    logger.error('getCustomerConatctPaginationHandler - Unexpected Error:', req.headers, req?.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}
/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function crmInsertContactHandler(req: any, res: any, done: any) {
  try {
    const body = req?.body;
    crmInsertContact(body);
    res.status(200).send({ message: RESPONSE.success_message });
  } catch (err) {
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getAiConservationdHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {
      sessionId:req?.params?.sessionId,
      domainId: auth.domainId
    };
    const getAi: any = await getAiConservationd(data);
    if (Array.isArray(getAi) && getAi.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getAitRes: getAi });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getAitRes: [] });
    }
  } catch (err) {
    logger.error('getCustomerConatctPaginationHandler - Unexpected Error:', req.headers, req?.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */

export async function getContactLogsHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {
      domainId: auth.domainId,
      offset: req?.body?.offset ?? null,
      limit: req?.body?.limit ?? null,
      agentId: req?.body?.agentId ?? null,
    };
    console.log('getContactMobileLogsDao', data);
    const recentLogs = await getContactMobileLogsDao(data);
    console.log('getContactMobileLogsDao RESPONSE', recentLogs);
    res.status(200).send({ statusCode: 200, message:recentLogs[0] });
  } catch (err) {
    logger.error('createCustomerEditHandler - Unexpected Error:', req.headers, req?.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */

export async function getCustomerTicketLogHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {
      coid: req?.body?.coid ?? null,
    };
    const ticketLog = await getCustomerTicketLogDao(data);
    res.status(200).send({ statusCode: 200, message:RESPONSE.success_message , response : ticketLog });
  } catch (err) {
    logger.error('getCustomerTicketLogHandler - Unexpected Error:', req.headers, req?.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error , response : [] });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */

export async function getCustomerAppoinentBookingLogHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {
      coid: req?.body?.coid ?? null,
      domainId: auth.domainId ?? null,
    };
    const appoinmentRes = await getCustomerAppoinentBookingLogDao(data);
    res.status(200).send({ statusCode: 200, message:RESPONSE.success_message , response : appoinmentRes });
  } catch (err) {
    logger.error('getCustomerTicketLogHandler - Unexpected Error:', req.headers, req?.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error , response : [] });
  }
}
