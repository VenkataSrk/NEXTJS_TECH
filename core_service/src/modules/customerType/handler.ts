/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */
import {
  deleteCustomerType,
  getCustomerTypeList,
  insertUpdateCustomerType
} from '../../dao/customerType';
import { logger } from '../../plugins/log';

import { RESPONSE } from '../../helpers/constants';

/**
 *
 * @param req
 * @param res
 * @param done
 * insert customer type
 */
export async function insertUpdateCustomerTypeHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      id: req?.body?.id ?? null,
      domainId: auth.domainId,
      customerTypeName: req?.body?.customerTypeName ?? null,
      description: req?.body?.description ?? null,
      isStatus: req?.body?.isStatus ?? null,
      type: req?.body?.type ?? null
    };
    const insertType: any = await insertUpdateCustomerType(data);
    logger.info('insertUpdateCustomerType', insertType);
    if (insertType[0]?.errcode === 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.create_customer_type, errcode: 0 });
    } else if (insertType[0]?.errcode === 1) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.update_customer_type, errcode: 1 });
    } else if (insertType[0]?.errcode === -2) {
      res.status(200).send({ statusCode: 404, message: RESPONSE.exists_customer_type, errcode: -2 });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.failed });
    }
  } catch (err) {

    logger.error('insertUpdateCustomerTypeHandler', req.headers, req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getCustomerTypeListHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {
      domainId: auth.domainId
    };
    const getCustomerType: any = await getCustomerTypeList(data);
    logger.info('getCustomerTypeList', getCustomerType);
    if (Array.isArray(getCustomerType) && getCustomerType.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getCustomerTypeListRes: getCustomerType });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getCustomerTypeListRes: [] });
    }
  } catch (err) {
    logger.error('getCustomerTypeListHandler', req.headers, err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function deleteCustomerTypeHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {
      id: req.params.id,
      domainId: auth.domainId
    };
    const deleteType: any = await deleteCustomerType(data);
    logger.info('deleteCustomerType', deleteType);
    if (deleteType[0]?.errcode === 1) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.delete_customer_type });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.delete_customer_field });
    }
  } catch (err) {

    logger.error('deleteCustomerTypeHandler', req.headers, req.params, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}
