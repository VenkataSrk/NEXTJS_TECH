import { getbotImagesDao, getBotIntentDao, getFallbackMessageDao, insertBotDetails, insertBotImagesDao, insertBotIntentDao, insertFallbackMessageDao, saveChatFlowDao, updateBotTemplateDao } from '../../dao/bot';

/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */
import { v4 as uuidv4 } from 'uuid';
import { RESPONSE } from '../../helpers/constants';
import { logger } from '../../plugins/winston';

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function createBotIntentHandler(req: any, res: any, done: any) {
  try {
    const data: any = {};
    data.id = req.body?.id;
    data.bot_id = req.body?.botId;
    data.intent = req.body?.intent;
    data.status = req.body?.status;
    if (data.bot_id !== null) {
      await insertBotIntentDao(data);
      res.status(200).send({ statusCode: 200, message: 'Bot intent insert successfully' });
    } else {
      res.status(401).send({ statusCode: 200, message: 'Please provide the botId' });
    }
  } catch (err) {
    logger.error('createBotIntentHandler - Unexpected Error:', req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getBotIntentHandler(req: any, res: any, done: any) {
  try {
    const data: any = {};
    data.bot_id = req.params?.bid;
    const getBotIntentResult: any = await getBotIntentDao(data);
    logger.info('getBotIntentDao', getBotIntentResult);
    if (getBotIntentResult.length > 0) {
      res.status(200).send({ statusCode: 200, result: getBotIntentResult[0] });
    } else {
      res.status(404).send({ statusCode: 200, message: 'No Data Found' });
    }
  } catch (err) {
    logger.error('getBotIntentHandler - Unexpected Error:', req.params, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function insertDefaultBotImages(req: any, res: any, done: any) {
  try {
    const data: any = {};
    data.domainId = req.body.domainId;
    data.planId = req.body?.plan_id;
    const imagesUrl: any = {};
    imagesUrl.imageUrl = [
      'https://ccaas-storage.worktual.co.uk/chat/botoneimages.svg',
      'https://ccaas-storage.worktual.co.uk/chat/6704/bot2.svg',
      'https://ccaas-storage.worktual.co.uk/chat/6704/bot3.svg',
      'https://ccaas-storage.worktual.co.uk/chat/6704/bot4.svg',
      'https://ccaas-storage.worktual.co.uk/chat/6704/bot5.svg',
      'https://ccaas-storage.worktual.co.uk/chat/6704/bot6.svg'
    ];
    imagesUrl.backgroundImage = [
      'https://ccaas-storage.worktual.co.uk/chat/10592/bgg1.png',
      'https://ccaas-storage.worktual.co.uk/chat/10592/bgg2.png',
      'https://ccaas-storage.worktual.co.uk/chat/10592/bgg3.png',
      'https://ccaas-storage.worktual.co.uk/chat/10592/bgg4.png',
      'https://ccaas-storage.worktual.co.uk/chat/10592/bgg5.png',
      'https://ccaas-storage.worktual.co.uk/chat/10592/bgg6.png',
      'https://ccaas-storage.worktual.co.uk/chat/10592/bgg7.png'

    ];
    await insertBotImagesDao(data, imagesUrl);
    if (data.planId !== 32) {
      const botDetails: any = [
        {
          botName: 'VectoneBot',
          bot_id: 1,
        },
        {
          botName: 'WorktulBot',
          bot_id: 2,
        }
      ];
      botDetails.map(async (list: any) => {
        await insertBotDetails(data, list);
      });
      const d = new Date();
      const datestring = `${d.getDate()}/${d.getMonth() + 1}/${d.getFullYear()} ${d.getHours()}:${d.getMinutes()}`;
      const version = req?.body?.status === 2 ? `Version ${datestring} : Published` : `Version ${datestring} : Save`;
      data.version = version;
      data.companyId = req.body.companyId;
      await saveChatFlowDao(data);
    }
    res.status(200).send({ statusCode: 200, message: 'Data Inserted Successfully' });
  } catch (err) {
    logger.error('insertDefaultBotImages - Unexpected Error:', req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getBotDefaultImages(req: any, res: any, done: any) {
  try {
    const data: any = {};
    data.domainId = req.headers.domainId;
    const getbotImagesResult: any = await getbotImagesDao(data);
    logger.info('getbotImagesDao', getbotImagesResult);
    if (getbotImagesResult.length > 0) {
      res.status(200).send({ statusCode: 200, message: 'success', result: getbotImagesResult });
    } else {
      res.status(404).send({ statusCode: 200, message: 'No Data Found' });
    }
  } catch (err) {
    logger.error('getBotDefaultImages - Unexpected Error:', req.headers, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export const updateBotTemplateList = async (req: any, res: any, done: any) => {
  try {
    const data: any = {};
    data.domainId = req.headers?.domainId;
    data.uuid = req.body?.uuid;
    data.title = req.body?.title;
    data.discription = req.body?.discription;
    data.botTemplate = req.body?.botTemplate;
    data.status = req.body?.status;
    if (data.uuid !== '' || data.uuid !== undefined) {
      await updateBotTemplateDao(data);
      res.status(200).send({ statusCode: 200, message: 'Update Successfully' });
    } else {
      res.status(404).send({ statusCode: 404, message: 'Please Enter UUID' });
    }

  } catch (err) {
    logger.error('updatebotTemplateStatus - Unexpected Error:', req.headers, req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
};

/**
 * Fallback message insert api
 *
 */

export const insertFallbackMessageHandler = async (req: any, res: any, done: any) => {
  try {
    const data: any = {};
    data.domainId = req.headers?.domainId;
    data.uuid = req.body?.uuid ? req.body?.uuid : uuidv4();
    data.fallBackMessage = req.body?.fallBackMessage;
    data.mapId = req.body?.mapId;
    await insertFallbackMessageDao(data);
    res.status(200).send({ statusCode: 200, message: 'Data inserted  Successfully' });
  } catch (err) {
    logger.error('insertFallbackMessageHandler - Unexpected Error:', err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
};

export const getFallbackMessageHandler = async (req: any, res: any, done: any) => {
  try {
    const data: any = {};
    data.domainId = req?.headers?.domainId ?? req?.params?.domainId;
    const getFallbackMessageResult: any = await getFallbackMessageDao(data);
    logger.info('getFallbackMessageDao', getFallbackMessageResult);
    if (getFallbackMessageResult.length > 0) {
      res.status(200).send({ statusCode: 200, message: 'Data inserted  Successfully', result: getFallbackMessageResult });
    } else {
      res.status(200).send({ statusCode: 404, message: 'Data Not Fount' });
    }
  } catch (err) {
    logger.error('getFallbackMessageHandler - Unexpected Error:', req.headers, req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
};

export const insertDefaultFallbackMessage = async (req: any, res: any, done: any) => {
  try {
    const data: any = {};
    data.uuid = uuidv4();
    data.domainId = req.headers.domainId;
    data.fallbackMessage = "Apologies! I'm a new bot learning as I go, so I couldn't understand. Please rephrase your query to help me better assist you 😃";
    data.mapId = req.body.mapId;
    await insertFallbackMessageDao(data);
    res.status(200).send({ statusCode: 200, message: 'Data Insert SuccessFully' });
  } catch (error) {
    logger.error('insertDefaultFallbackMessage - Unexpected Error:', req.headers, req.body, error);
    res.send(500).send({ statusCode: 500, message: 'Internal Server Error' });
  }
};
