/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */
import {
  customerCreateContactDao,
  customerGetContactDao,
  customerTagDisable,
  customerUpdateContactDao,
  deleteCustomContactDao,
  deleteCustomerTag,
  getCustomerTag,
  getCustomerTagDetails,
  getCustomerTagDisable,
  getMobileNumberDao,
  getPdgPageDao,
  insertCustomerTag,
  isValidCustomerTag,
  updateCustomerTag,
} from '../../dao/customerTag';
import { logger } from '../../plugins/log';

import { RESPONSE } from '../../helpers/constants';

/**
 *
 * @param req
 * @param res
 * @param done
 * customer tag api
 */
export async function insertCustomerTagHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      customerTagName: req?.body?.customerTagName ?? null,
      type: 'Custom',
      description: req?.body?.description ?? null,
      status: req?.body?.status ?? 0,
      domainId: auth.domainId,
    };
    const insertTag: any = await insertCustomerTag(data);
    logger.info('insertCustomerTag', insertTag);
    if (insertTag) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.customer_tag, insertCustomerTagList: insertTag ?? {} });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {

    logger.error('insertCustomerTagHandler', req.headers, req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getCustomerTagHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {};
    data.domainId = auth.domainId;
    const getCustomer: any = await getCustomerTag(data);
    const getCustomerTagDisableList: any = await getCustomerTagDisable(data);
    logger.info('getCustomerTagDisable', getCustomerTagDisableList);
    if (Array.isArray(getCustomer) && getCustomer.length > 0) {
      res
        .status(200)
        .send({ statusCode: 200, message: RESPONSE.success_message, getCustomerTagStatus: getCustomerTagDisableList[0]?.customerTag, getCustomerTagListRes: getCustomer });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getCustomerTagListRes: [] });
    }
  } catch (err) {
    logger.error('getCustomerTagHandler', req.headers, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getCustomerTagDetailsHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const cid = req.params.cid;
    const data: any = {
      domainId: auth.domainId,
    };
    if (cid !== '') {
      const getCustomer: any = await getCustomerTagDetails(data, cid);
      logger.info('getCustomerTagDetails', getCustomer);
      if (Array.isArray(getCustomer) && getCustomer.length > 0) {
        res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getCustomerTagDetailsRes: getCustomer });
      } else {
        res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getCustomerTagDetailsRes: [] });
      }
    } else {
      res.status(200).send({ statusCode: 405, message: RESPONSE.cid });
    }
  } catch (err) {

    logger.error('getCustomerTagDetailsHandler', req.headers, req.params, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * update customer tag api
 */
export async function updateCustomerTagHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const cid = req.params.cid;
    const data: any = {
      customerTagName: req?.body?.customerTagName ?? null,
      type: req?.body?.type ?? null,
      description: req?.body?.description ?? null,
      status: req?.body?.status ?? 0,
      domainId: auth.domainId,
    };
    data.cid = cid;
    if (cid !== '') {
      const updateTag: any = await updateCustomerTag(data, cid);
      logger.info('updateCustomerTag', updateTag);
      if (updateTag) {
        res.status(200).send({ statusCode: 200, message: RESPONSE.customerr_tag, updateCustomerTagRes: data });
      } else {
        res.status(200).send({ statusCode: 404, message: RESPONSE.not_update });
      }
    } else {
      res.status(200).send({ statusCode: 405, message: RESPONSE.aid });
    }
  } catch (err) {

    logger.error('updateCustomerTagHandler', req.headers, req.params, req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function deleteCustomerTagHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const cid: any = req.params.cid;
    const domainId = auth.domainId;
    if (cid !== '') {
      const deleteAgent: any = await deleteCustomerTag(cid, domainId);
      logger.info('deleteCustomerTag', deleteAgent);
      if (deleteAgent) {
        res.status(200).send({ statusCode: 200, message: RESPONSE.customerr_tagg });
      } else {
        res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
      }
    } else {
      res.status(200).send({ statusCode: 405, message: RESPONSE.aid });
    }
  } catch (err) {

    logger.error('deleteCustomerTagHandler', req.headers, req.params, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function isValidCustomerTagHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {};
    data.domainId = auth.domainId;
    data.customerTagName = req?.body?.name ?? null;
    const customarRes: any = await isValidCustomerTag(data);
    logger.info('isValidCustomerTag', customarRes);
    if (customarRes && customarRes.length > 0 && customarRes[0]?.customerTagName.length > 0) {
      res.status(200).send({ statusCode: 403, message: RESPONSE.name_already_exist, flag: 0 });
    } else {
      res.status(200).send({ statusCode: 200, message: RESPONSE.available, flag: 1 });
    }
  } catch (err) {

    logger.error('isValidCustomerTagHandler', req.headers, req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 * getSmsNotification
 * @param req
 * @param res
 * @param done
 */
export async function customerTagDisableHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      domainId: auth.domainId,
      customerTag: req?.body?.customerTag ?? 0,
    };
    const customer: any = await customerTagDisable(data);
    logger.info('customerTagDisable', customer);
    if (customer.changedRows !== 1) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.customerD });
    } else {
      res.status(200).send({ statusCode: 200, message: RESPONSE.customerA });
    }
  } catch (err) {

    logger.error('customerTagDisableHandler', req.headers, req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getCustomerTagDisableHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {};
    data.domainId = auth.domainId;
    const getCustomer: any = await getCustomerTagDisable(data);
    logger.info('getCustomerTagDisable', getCustomer);
    if (Array.isArray(getCustomer) && getCustomer.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getCustomerTagRes: getCustomer });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getCustomerTagRes: [] });
    }
  } catch (err) {

    logger.error('getCustomerTagDisableHandler', req.headers, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

// Dummy api for chatBot mobile balance check

export async function getMobileNumberStatus(req: any, res: any, done: any) {
  try {
    const data = req.params.mobileNumber;
    const getStatus: any = await getMobileNumberDao(data);
    logger.info('getMobileNumberDao', getStatus);
    if (getStatus.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, result: getStatus });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getStatus: [] });
    }
  } catch (err) {

    logger.error('getMobileNumberStatus', req.params, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

// Custom create contact

export async function customContactCreateHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {};
    data.domainid = auth.domainId;
    data.field_type = req.body?.field_type ?? null;
    data.field_name = req.body?.field_name ?? null;
    data.field_label = req.body?.field_label ?? null;
    data.field_value = req.body?.field_value ?? null;
    data.data_type = req.body?.data_type ?? null;
    data.field_category = req.body?.field_category ?? null;
    data.isMandatory = req.body?.isMandatory ?? null;
    data.placeHolder = req.body?.placeHolder ?? null;
    data.errMsg = req.body?.errMsg ?? null;
    data.isDisplay = req.body?.isDisplay ?? null;
    const customCreateContactResult: any = await customerCreateContactDao(data);
    logger.info('customerCreateContactDao', customCreateContactResult);
    res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, result: customCreateContactResult });
  } catch (err) {

    logger.error('customContactCreateHandler', req.headers, req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

// Update Custom contact

export async function customContactUpdateHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {};
    data.id = req.body?.id,
      data.domainId = auth?.domainId,
      data.field_type = req.body?.field_type,
      data.field_label = req.body?.field_label,
      data.field_value = req.body?.field_value,
      data.isMandatory = req.body?.isMandatory,
      data.placeHolder = req.body?.placeHolder,
      data.errMsg = req.body?.errMsg,
      data.isDisplay = req.body?.isDisplay;

    const customUpdateContactResult: any = await customerUpdateContactDao(data);
    logger.info('customerUpdateContactDao', customUpdateContactResult);
    res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, result: customUpdateContactResult });
  } catch (err) {

    logger.error('customContactUpdateHandler', req.headers, req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

// Custom get contact

export async function customContactGetHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {};
    data.domainid = auth.domainId;
    const customGetContactResult: any = await customerGetContactDao(data);
    logger.info('customerGetContactDao', customGetContactResult);
    res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, result: customGetContactResult });
  } catch (err) {

    logger.error('customContactGetHandler', req.headers, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

// Delete Custom Contact

export async function deleteCustomContactGetHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {};
    data.cid = req.body?.id;
    const deleteCustomContac: any = await deleteCustomContactDao(data);
    logger.info('deleteCustomContactDao', deleteCustomContac);
    res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, result: deleteCustomContac[0] });
  } catch (err) {

    logger.error('deleteCustomContactGetHandler', req.headers, req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

// Custom get contact no auth

export async function customContactGetHandlerNoAuth(req: any, res: any, done: any) {
  try {
    const data: any = {};
    data.domainid = req.params.domainId;
    const customGetContactResult: any = await customerGetContactDao(data);
    logger.info('customerGetContactDao', customGetContactResult);
    res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, result: customGetContactResult });

  } catch (err) {

    logger.error('customContactGetHandlerNoAuth', req.params, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

// Custom create contact

export async function customContactCreateHandlerNoAuth(req: any, res: any, done: any) {
  try {
    const data: any = {};
    data.domainid = req.body?.domainId;
    data.field_type = req.body?.field_type ?? null;
    data.field_name = req.body?.field_name ?? null;
    data.field_label = req.body?.field_label ?? null;
    data.field_value = req.body?.field_value ?? null;
    data.data_type = req.body?.data_type ?? null;
    data.field_category = req.body?.field_category ?? null;
    data.isMandatory = req.body?.isMandatory ?? null;
    data.placeHolder = req.body?.placeHolder ?? null;
    data.errMsg = req.body?.errMsg ?? null;
    data.isDisplay = req.body?.isDisplay ?? null;
    if (data.domainid !== '' && data.domainid !== null) {
      const customCreateContactResult: any = await customerCreateContactDao(data);
      logger.info('customerCreateContactDao', customCreateContactResult);
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, result: customCreateContactResult });
    } else {
      res.status(404).send({ statusCode: 200, message: 'Domain ID is required' });
    }
  } catch (err) {

    logger.error('customContactCreateHandlerNoAuth', req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

// Api For get the pdf page number

export async function getPdfPageNumber(req: any, res: any, done: any) {
  try {
    const data: any = {};
    data.query = req.body?.query ?? null;
    data.pdf_path = req.body?.pdf_path ?? null;
    data.unique_id = req.body?.unique_id ?? null;
    data.top_k = req.body?.top_k ?? null;

    if (data.query !== '' && data.query !== null) {
      const getPdgPageDaoResult: any = await getPdgPageDao(data);
      logger.info('getPdgPageDao', getPdgPageDaoResult);
      if (getPdgPageDaoResult?.status === 200) {
        res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, result: getPdgPageDaoResult?.data });
      } else {
        res.status(404).send({ statusCode: 200, message: 'Failed' });
      }
    } else {
      res.status(404).send({ statusCode: 200, message: 'Domain ID is required' });
    }
  } catch (err) {

    logger.error('getPdfPageNumber', req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}
