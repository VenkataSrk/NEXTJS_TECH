import { checkExistsUserDao, getCityConfigDao, getCountry, getCountryConfigDao, getEntryNodeDataDao, getStateConfigDao } from '../../dao/country';
import { RESPONSE } from '../../helpers/constants';
import { logger } from '../../plugins/winston';
/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getCountryHandler(req: any, res: any, done: any) {
  try {
    const getCountryList: any = await getCountry();
    logger.info('getCountry', getCountryList);
    if (Array.isArray(getCountryList) && getCountryList.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getList: getCountryList });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getList: {} });
    }
  } catch (err) {
    logger.error('getCountryHandler - Unexpected Error:', err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getCountryConfigHandler(req: any, res: any, done: any) {
  try {
    const getCountryConfig: any = await getCountryConfigDao();
    logger.info('getCountryConfigDao', getCountryConfig);
    if (Array.isArray(getCountryConfig) && getCountryConfig.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getList: getCountryConfig[0] });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getList: {} });
    }
  } catch (err) {
    logger.error('getCountryConfigHandler - Unexpected Error:', err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function getMultipleEntryNodes(req: any, res: any, done: any) {
  try {
    const multiChatId: any = req.body?.multiChatId;
    const data: any = {
      ipAddress: req.body?.ipAddress ?? null,
      domainId: req.body?.domainId ?? 0

    };
    if (multiChatId?.length > 0) {
      const entryResult: any[] = [];
      await Promise.all(
        multiChatId?.map(async (list: any) => {
          const resultData: any = await getEntryNodeDataDao(list);
          const result: any = resultData.length > 0 ? JSON?.parse(resultData[0]?.nodes) : [];
          entryResult.push(result[0]);
        })
      );
      const ipExistinguser: any = await checkExistsUserDao(data);
      if (entryResult?.length > 0) {
        res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, data: { entryResult, ipExistinguser }, });
      } else {
        res.status(200).send({ statusCode: 403, message: 'No data Found' });
      }
    }
  } catch (err) {
    req.log.error(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}
/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getStateConfigHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      countryCode: req?.body?.countryCode ?? null,
    };
    const getStateConfig: any = await getStateConfigDao(data);
    if (getStateConfig && getStateConfig.length > 0) {
      res.status(200).send({ statusCode: 200, result: getStateConfig[0] });
    } else {
      res.status(404).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    logger.error('getStateConfigHandler - Unexpected Error:', req?.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getCityConfigHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      countryCode: req?.body?.countryCode ?? null,
      cityCode: req?.body?.cityCode ?? null,
    };
    const getCityConfig: any = await getCityConfigDao(data);
    logger.info('getCityConfigDao', getCityConfig);
    if (getCityConfig && getCityConfig.length > 0) {
      res.status(200).send({ statusCode: 200, result: getCityConfig[0] });
    } else {
      res.status(404).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    logger.error('getCityConfigHandler - Unexpected Error:', req?.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}
