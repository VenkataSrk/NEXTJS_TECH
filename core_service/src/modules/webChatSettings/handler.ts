import {
  addChatFlowlanguage,
  assignCallBackChat,
  botDetailsDao,
  checkWebchatTitleDao,
  cloneWebChatConfigurationdao,
  createChatCustomer,
  createGroupCoChat,
  createWebChatWidgetField,
  deleteWebChatWidgetConfigurationdao,
  encoderDao,
  getAgentTracker,
  getAllChatSessionMessageDao,
  getCallbackListNotification,
  getChatCustomerList,
  getChatFlowlanguage,
  GetChatFlowVariablesDao,
  getChatLablesDao,
  getChatwidgetById,
  getchatwidgetByIdwithOutDomainID,
  getcoChatDetailsByIDDao,
  getcoChatDetailsDao,
  getCompanyDomainDao,
  getConcurrentChat,
  getCustomerContactDao,
  getCustomerLatestMsgDao,
  getGroupCoChat,
  getGroupCoChatId,
  getTriggerDao,
  getWebChatConfig,
  getwebChatQuickActionsDao,
  InsertChatFlowVariablesDao,
  insertUpdateConcurrentChat,
  insertUpdateEmailFlowHistory,
  insertUpdateIrrelevantQaDao,
  insertUpdateTriggerDao,
  insertwebChatQuickActionsDao,
  sendMessageToCoChat,
  setChatFlowlanguage,
  storeCochatDetailsByIDDao,
  UpdateIsAgentConcurrentChat,
  updateWebChatWidget,
  updateWebChatWidgetStatus,
  updateWeChatQuickActionsDao
} from '../../dao/chatSetting';

/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */
import { request } from 'http';
import shortid from 'shortid';
import { v4 as uuidv4 } from 'uuid';
import { RESPONSE } from '../../helpers/constants';
import { generateImage } from '../../helpers/utils';
import { ioredisWhatsapp } from '../../plugins/db';
import { logger } from '../../plugins/log';

/**
 *
 * @param req
 * @param res
 * @param done
 * create Chat Customer api
 */
export async function createChatCustomerHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      domainName: req.body.domainName ? req.body.domainName : null,
      description: req.body.description ? req.body.description : null,
      codeSnippet: req.body.codeSnippet ? req.body.codeSnippet : null,
      domainId: auth.domainId,
    };
    const createChatRes: any = await createChatCustomer(data);
    logger.info('createChatCustomer', createChatRes);
    if (createChatRes) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.create_chat_customer, insertChatCustomer: createChatRes ?? {} });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {

    logger.error('createChatCustomerHandler - Unexpected Error:', req.headers, req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function createWebChatWidget(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    let sourceId: any = '';
    if (req.body?.webchatid === '' || req.body?.webchatid === null || req.body?.webchatid === undefined) {
      sourceId = shortid.generate();
    } else {
      sourceId = req.body?.webchatid;
    }

    const data: any = {
      domainId: auth.domainId,
      title: req?.body?.title ?? null,
      aliasTitle: req.body?.aliasTitle ?? null,
      callFlowName: req?.body?.callFlowName ?? null,
      description: req?.body?.description ?? null,
      chatFlowId: req?.body?.chatFlowId ?? 0,
      chatIconLogo: req?.body?.chatIconLogo ?? null,
      domain: req?.body?.domain ?? null,
      personalization: req?.body?.personalization ?? null,
      formDetails: req.body?.formDetails ?? null,
      primaryForm: req?.body?.primaryForm ?? null,
      preChartFormFlag: req?.body?.prechatForm ?? null,
      webchatid: sourceId,
      status: 1,
      greeting: req.body?.greeting ?? null,
      templateData: req.body?.templateData ?? null,
      companyName: req.body?.companyName,
      planId: req.body?.planId ?? 0,
      productId: req.body?.productId ?? 0,
      websiteId: req.body?.websiteId ?? null,
      autoResponse: req.body?.autoResponse ?? null,
      isCallBack: req.body?.isCallBack ?? 0,
      threshold: req.body?.threshold ?? null,
      domainUrlId: req.body?.domainUrlId ?? 'https://botclient.worktual.co.uk',
    };
    const codeSnippet = `<div id="chat_bot" data-myparam=${sourceId}></div>
                         <script src='${data?.domainUrlId}'></script> `;
    data.codeSnippet = codeSnippet;
    const createWebChatWidgetFieldResp: any = await createWebChatWidgetField(data);
    logger.info('createWebChatWidgetField', createWebChatWidgetFieldResp);
    if (createWebChatWidgetFieldResp) {
      res.status(200).send({ codeSnippet, statusCode: 200, message: RESPONSE.create_webChat_widget, insertWebChatList: createWebChatWidgetFieldResp ?? {} });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {

    logger.error('createWebChatWidget - Unexpected Error:', req.headers, req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getChatCustomerListHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      domainId: auth.domainId,
    };
    const getChat: any = await getChatCustomerList(data);
    logger.info('getChatCustomerList', getChat);
    if (Array.isArray(getChat) && getChat.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getChatCustomersList: getChat,
      });
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        getChatCustomersList: [],
      });
    }
  } catch (err) {

    logger.error('getChatCustomerListHandler - Unexpected Error:', req.headers, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}
export async function getWebChatConfigurationList(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      domainId: auth.domainId,
    };
    const getPrimaryFieldDetails: any = await getWebChatConfig(data);
    logger.info('getWebChatConfig', getPrimaryFieldDetails);
    if (getPrimaryFieldDetails.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getPrimaryField: getPrimaryFieldDetails,
      });
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        getPrimaryField: [],
      });
    }
  } catch (err) {

    logger.error('getWebChatConfigurationList - Unexpected Error:', req.headers, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function getChatWidgetHandlerById(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const webchatid = req.params.webchatid;
    const data: any = {};
    data.domainId = auth.domainId;
    const getChatWidgetById: any = await getChatwidgetById(webchatid, data);
    logger.info('getChatwidgetById', getChatWidgetById);
    if (Array.isArray(getChatWidgetById) && getChatWidgetById.length > 0) {
      const getChatWidgetDetails: any = getChatWidgetById;
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getWebchatWidgetById: getChatWidgetDetails,
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getQueue: [] });
    }
  } catch (err) {

    logger.error('getChatWidgetHandlerById - Unexpected Error:', req.headers, req.params , err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function updateWebChatWidgetHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const webchatid: any = req.params.webchatid;
    const data: any = {
      domainId: auth.domainId,
      title: req?.body?.title ?? null,
      aid: req?.body?.aid ?? 0,
      callFlowName: req?.body?.callFlowName ?? null,
      description: req?.body?.description ?? null,
      chatFlowId: req?.body?.chatFlowId ?? 0,
      chatIconLogo: req?.body?.chatIconLogo ?? null,
      domain: req?.body?.domain ?? null,
      personalization: req?.body?.personalization ?? null,
      primaryForm: req?.body?.primaryForm ?? null,
      formDetails: req.body?.formDetails ?? null,
      preChartFormFlag: req?.body?.prechatForm ?? null,
      greeting: req.body?.greeting ?? null,
      templateData: req.body?.templateData ?? null,
      companyName: req.body?.companyName,
      planId: req.body?.planId ?? 0,
      productId: req.body?.productId ?? 0,
      websiteId: req.body?.websiteId ?? null,
      autoResponse: req.body?.autoResponse,
      isCallBack: req.body?.isCallBack ?? 0,
      threshold: req.body?.threshold ?? null,
      domainUrlId: req.body?.domainUrlId ?? null,
      aliasTitle: req.body?.aliasTitle ?? null
    };
    const codeSnippet = `<div id="chat_bot" data-myparam=${webchatid}></div>
                         <script src='${data?.domainUrlId}'></script>`;
    data.codeSnippet = codeSnippet;
    if (webchatid.webchatid !== '') {
      const updatequeue: any = await updateWebChatWidget(data, webchatid);
      logger.info('updateWebChatWidget', updatequeue);
      if (updatequeue) {
        res.status(200).send({
          statusCode: 200,
          message: RESPONSE.update_webChat_widget,
          updateWebChatList: updatequeue ?? {},
        });
      } else {
        res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
      }
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.please_provider });
    }
  } catch (err) {

    logger.error('updateWebChatWidgetHandler - Unexpected Error:', req.headers, req.params, req.body , err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function updateWebChatWidgetStatusHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const webchatid: any = req.params.webchatid;
    const data: any = {
      domainId: auth.domainId,
      status: req?.body?.status ?? null,
    };
    if (webchatid.webchatid !== '') {
      const updatequeue: any = await updateWebChatWidgetStatus(data, webchatid);
      logger.info('updateWebChatWidgetStatus', updatequeue);
      if (updatequeue) {
        res.status(200).send({
          statusCode: 200,
          message: RESPONSE.update_webChat_widget,
          updateWebChatList: updatequeue ?? {}
        });
      } else {
        res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
      }
    } else {
      res
        .status(200)
        .send({ statusCode: 404, message: RESPONSE.please_provider });
    }
  } catch (err) {

    logger.error('updateWebChatWidgetStatusHandler - Unexpected Error:', req.headers, req.params , err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * create group cochat
 */
export async function createGroupCoChatHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      uid: uuidv4(),
      channelId: uuidv4(),
      channelName: req?.body?.channelName ?? '',
      subscribedUsers: req?.body?.subscribedUsers ?? [],
      channelIcon: req?.body?.channelIcon ?? '',
      domainId: auth.domainId,
      lastMessageInfo: req?.body?.lastMessageInfo ?? {},
      unseenCount: req?.body?.unseenCount ?? 0,
      allSeenStatus: req?.body?.allSeenStatus ?? 0,
      messages: req?.body?.messages ?? [],
      isChannelMuted: req?.body?.isChannelMuted ?? 0,
      customerInfo: req?.body?.customerInfo ?? {},
    };
    const createGroupRes: any = await createGroupCoChat(data);
    logger.info('createGroupCoChat', createGroupRes);
    if (createGroupRes) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.create_group_co_chat, createGroupCoChat: data ?? {} });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {

    logger.error('createGroupCoChatHandler - Unexpected Error:', req.headers, req.body , err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getGroupCoChatHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data = auth.domainId;
    const getGroupChat: any = await getGroupCoChat(data);
    logger.info('getGroupCoChat', getGroupChat);
    if (Array.isArray(getGroupChat) && getGroupChat.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getGroupChatRes: getGroupChat,
      });
    } else {
      res
        .status(200)
        .send({ statusCode: 404, message: RESPONSE.not_found, getGroupChatRes: [] });
    }
  } catch (err) {

    logger.error('getGroupCoChatHandler - Unexpected Error:', req.headers, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getGroupCoChatIdHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data = auth.domainId;
    const uid = req.params.uid;
    const getGroupChat: any = await getGroupCoChatId(data, uid);
    logger.info('getGroupCoChatId', getGroupChat);
    if (getGroupChat) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getGroupChatIdRes: getGroupChat,
      });
    } else {
      res
        .status(200)
        .send({ statusCode: 404, message: RESPONSE.not_found, getGroupChatIdRes: [] });
    }
  } catch (err) {

    logger.error('getGroupCoChatIdHandler - Unexpected Error:', req.headers, req.params , err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function sendMessageToCoChatHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const dataq: any = {
      uid: req.params.uid,
      channelId: req.params.channelId,
      domainId: auth.domainId,
    };
    const data: any = req?.body ?? {};
    if (dataq !== '') {
      const sendMessage: any = await sendMessageToCoChat(data, dataq);
      logger.info('sendMessageToCoChat', sendMessage);
      if (sendMessage) {
        res.status(200).send({
          statusCode: 200,
          message: RESPONSE.send_message,
        });
      } else {
        res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
      }
    } else {
      res
        .status(200)
        .send({ statusCode: 404, message: RESPONSE.please_provider });
    }
  } catch (err) {

    logger.error('sendMessageToCoChatHandler - Unexpected Error:', req.headers, req.params, req.body , err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

// For Customer side chat

export async function getClientChatWidgetHandlerById(req: any, res: any, done: any) {
  try {
    const webchatid = req.params.webchatid;
    const data: any = {};
    const getChatWidgetById: any = await getchatwidgetByIdwithOutDomainID(webchatid, data);
    logger.info('getchatwidgetByIdwithOutDomainID', getChatWidgetById);
    if (Array.isArray(getChatWidgetById) && getChatWidgetById.length > 0) {
      const getChatWidgetDetails: any = getChatWidgetById;
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getWebchatWidgetById: getChatWidgetDetails,
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getQueue: [] });
    }
  } catch (err) {

    logger.error('getClientChatWidgetHandlerById - Unexpected Error:', req.params , err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

// Get Bot Names

export async function getBotDetails(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data = {
      domainId: auth.domainId
    };
    const getBotNames: any = await botDetailsDao(data);
    logger.info('botDetailsDao', getBotNames);
    if (getBotNames.length > 0) {
      res.status(200).send({ statusCode: 200, message: 'Data Found', result: getBotNames });
    } else {
      res.status(200).send({ statusCode: 400, message: 'Data Not Found', result: [] });
    }
  } catch (err) {

    logger.error('getBotDetails - Unexpected Error:', req.headers , err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

// get  all coChat details

export async function getCochatDetails(req: any, res: any) {
  try {
    const auth: any = req.headers;
    const data = {
      domainId: auth.domainId
    };
    const getcoChatData: any = await getcoChatDetailsDao(data);
    logger.info('getcoChatDetailsDao', getcoChatData);
    if (getcoChatData.length > 0) {
      res.status(200).send({ statusCode: 200, message: 'Data Found', result: getcoChatData });
    } else {
      res.status(200).send({ statusCode: 400, message: 'Data Not Found', result: [] });
    }
  } catch (err) {

    logger.error('getCochatDetails - Unexpected Error:', req.headers , err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

// get coChat details bu uid

export async function getCochatDetailsByID(req: any, res: any) {
  try {
    const auth: any = req.headers;
    const data = {
      domainId: auth.domainId,
      channelId: req.params.channelId
    };
    const getcoChatDataByID: any = await getcoChatDetailsByIDDao(data);
    logger.info('getcoChatDetailsByIDDao', getcoChatDataByID);
    if (getcoChatDataByID.length > 0) {
      res.status(200).send({ statusCode: 200, message: 'Data Found', result: getcoChatDataByID });
    } else {
      res.status(200).send({ statusCode: 400, message: 'Data Not Found', result: [] });
    }
  } catch (err) {

    logger.error('getCochatDetailsByID - Unexpected Error:', req.headers, req.params , err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

// store coChat message

export async function storeCochatMesssage(req: any, res: any) {
  try {
    const auth: any = req.headers;
    const data = {
      domainId: auth.domainId,
      channelId: req.body.channelId,
      message: req?.body?.message
    };
    await storeCochatDetailsByIDDao(data);
    res.status(200).send({ statusCode: 200, message: 'Successfully updated' });
  } catch (err) {

    logger.error('storeCochatMesssage - Unexpected Error:', req.headers, req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

// check the name already exists or not

export async function chatWidgetNameCheck(req: any, res: any) {
  try {
    const auth: any = req.headers;
    const data = {
      title: req?.body?.title,
      domainId: auth?.domainId
    };
    const findWebChatTitleDao: any = await checkWebchatTitleDao(data);
    logger.info('checkWebchatTitleDao', findWebChatTitleDao);
    if (findWebChatTitleDao.length > 0) {
      res.status(200).send({ statusCode: 200, message: 'Name is already exists' });
    } else {
      res.status(200).send({ statusCode: 404, message: 'Data Not Found' });
    }
  } catch (err) {

    logger.error('chatWidgetNameCheck - Unexpected Error:', req.headers, req.body , err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

// webChat Quick actions

export async function insertwebChatQuickActions(req: any, res: any) {
  try {
    const auth: any = req.headers;
    const data = {
      domainId: auth?.domainId,
      chatFlowId: req.body?.chatFlowId,
      labelName: req.body?.labelName,
      goToDialogue: req.body?.goToDialogue,
      uuid: req.body?.uuid ? req.body?.uuid : uuidv4()
    };
    if (data?.chatFlowId !== '' || data?.chatFlowId !== null) {
      await insertwebChatQuickActionsDao(data);
      res.status(200).send({ statusCode: 200, message: 'Data Inserted Successfully' });
    } else {
      res.status(200).send({ statusCode: 404, message: 'Please Enter the chatFlowId' });
    }
  } catch (err) {

    logger.error('insertwebChatQuickActions - Unexpected Error:', req.headers, req.body , err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function getwebChatQuickActions(req: any, res: any) {
  try {
    const auth: any = req.headers;
    const data = {
      domainId: auth?.domainId,
      chatFlowId: req.params?.chatFlowId
    };
    if (data?.chatFlowId !== '' || data?.chatFlowId !== null) {
      const getWebChatQuickActionsResult: any = await getwebChatQuickActionsDao(data);
      logger.info('getwebChatQuickActionsDao', getWebChatQuickActionsResult);
      if (getWebChatQuickActionsResult.length > 0) {
        res.status(200).send({ statusCode: 200, message: 'Success', result: getWebChatQuickActionsResult });
      } else {
        res.status(200).send({ statusCode: 404, message: 'No data found' });
      }
    } else {
      res.status(200).send({ statusCode: 404, message: 'Please Enter the chatFlowId' });
    }
  } catch (err) {

    logger.error('getwebChatQuickActions - Unexpected Error:', req.headers, req.params, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function updateWebchatQuickActions(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data = {
      chatFlowId: req.body?.chatFlowId,
      domainId: req.headers?.domainId,
      labelName: req.body?.labelName,
      goToDialogue: req.body?.goToDialogue
    };
    if (data?.chatFlowId !== '' || data?.chatFlowId !== null) {
      await updateWeChatQuickActionsDao(data);
      res.status(200).send({ statusCode: 200, message: 'Data Inserted Successfully' });
    } else {
      res.status(200).send({ statusCode: 404, message: 'Please Enter the chatFlowId' });
    }
  } catch (err) {

    logger.error('updateWebchatQuickActions - Unexpected Error:', req.headers, req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function getChatLabelsById(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data = {
      chatFlowId: req.params?.chatId,
      domainId: auth.domainId
    };
    if (data?.chatFlowId !== '' || data?.chatFlowId !== null) {
      const getChatLablesResult: any = await getChatLablesDao(data);
      logger.info('getChatLablesDao', getChatLablesResult);
      const resultData: any = [];
      if (getChatLablesResult.length > 0) {
        const nodesData: any = JSON.parse(getChatLablesResult[0].nodes);
        nodesData.map((list: any) => {
          if (list.type === 'customInput') {
            const nodeObj = {
              id: list.id,
              label: list.data.information.chatMessage
            };
            resultData.push(nodeObj);
          }
        });
        res.status(200).send({ statusCode: 200, message: 'Success', result: resultData });

      } else {
        res.status(200).send({ statusCode: 404, message: 'No data found' });
      }
    } else {
      res.status(200).send({ statusCode: 404, message: 'Please Enter the chatFlowId' });
    }
  } catch (err) {

    logger.error('getChatLabelsById - Unexpected Error:', req.headers, req.params, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

// webChat Insert Update Trigger message

export async function insertUpdateTriggerMessage(req: any, res: any) {
  try {
    const auth: any = req.headers;
    const data = {
      tmid: req.body?.tmid,
      Field: req.body?.fieldName,
      message: req.body?.message,
      callbackMessage: req.body?.callbackMessage,
      domainId: auth.domainId,
    };
    await insertUpdateTriggerDao(data);
    res.status(200).send({ statusCode: 200, message: 'Data saved successfully' });
  } catch (err) {

    logger.error('insertUpdateTriggerMessage - Unexpected Error:', req.headers, req.body , err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

// webChat Get Trigger message

export async function getTriggerMessage(req: any, res: any) {
  try {
    const auth: any = req.headers;
    const triggerMessageResult: any = await getTriggerDao(auth.domainId);
    logger.info('getTriggerDao', triggerMessageResult);
    if (triggerMessageResult.length > 0) {
      res.status(200).send({ statusCode: 200, message: 'Success', result: triggerMessageResult });
    } else {
      res.status(200).send({ statusCode: 404, message: 'No Data Found' });
    }
  } catch (err) {

    logger.error('getTriggerMessage - Unexpected Error:', req.headers , err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

// topUp generator

export async function topUpGenerator(req: any, res: any) {
  try {
    const data: any = {};
    data.mobileNo = req.body?.mobileNo ?? 0;
    data.amount = req.body?.amount ?? 0;
    const encoderDetails: any = await encoderDao(data);
    const generateUrl = `https://www.vectonemobile.co.uk/vectonepayment/step1/${encoderDetails}`;
    const datares = {
      url: `Here's the payment link for your £${req.body?.amount} top-up: ${generateUrl}`
    };
    logger.info('generateUrl----', generateUrl);
    res.status(200).send({ statusCode: 200, message: 'success', result: datares });

  } catch (err) {

    logger.error('topUpGenerator - Unexpected Error:', req.body , err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function getCustomerContactFields(req: any, res: any) {
  try {

    const getCustomerDaoResult: any = await getCustomerContactDao();
    logger.info('getCustomerContactDao', getCustomerDaoResult);
    if (getCustomerDaoResult.length > 0) {
      res.status(200).send({ statusCode: 200, message: 'Success', result: getCustomerDaoResult[0] });
    } else {
      res.status(200).send({ statusCode: 404, message: 'No Data Found' });
    }
  } catch (err) {

    logger.error('getCustomerContactFields - Unexpected Error:', err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function deleteWebChatWidgetConfiguration(req: any, res: any) {
  try {
    const aid: any = req.params.aid;
    const getCustomerDaoResult: any = await deleteWebChatWidgetConfigurationdao(aid);
    logger.info('deleteWebChatWidgetConfigurationdao', getCustomerDaoResult);
    if (getCustomerDaoResult.length > 0) {
      res.status(200).send({ statusCode: 200, message: 'Success', result: getCustomerDaoResult[0] });
    } else {
      res.status(200).send({ statusCode: 404, message: 'No Data Found' });
    }
  } catch (err) {

    logger.error('deleteWebChatWidgetConfiguration - Unexpected Error:', req.params, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}
export async function cloneWebChatConfigurationController(req: any, res: any) {
  try {
    const sourceId = shortid.generate();
    const data: any = {};
    data.aid = req.body?.aid ?? 0;
    data.title = req.body?.title ?? '';
    data.webchatid = sourceId;
    const codeSnippet = `<div id="chatbot_id" style="border-style: none; bottom: 20px; right: 30px;  position: fixed; z-index: 999999;">
    <iframe id="wortual_chatbot" src="https://botclient.worktual.co.uk/chatbot?webchatid=${sourceId}" title='${data?.companyName}'  scrolling="no"
      allowtransparency="true" style="border-style: none;  width: 100%; height: 100%;"></iframe>
   </div>
   <script>
    window.addEventListener('message', function (event) { if (JSON.parse(event.data).type === 'metatag') (new Function(JSON.parse(event.data).style))(); document.getElementById('chatbot_id').style = JSON.parse(event.data).style; });
    window.addEventListener('DOMContentLoaded', function () { var iframe = document.getElementById('wortual_chatbot'); iframe.onload = function() { iframe.src = iframe.src +'&url=' + window.location.href + '&title='+document.title; iframe.onload =null;
    };
   });
   </script>`;
    data.codeSnippet = codeSnippet;
    const getCustomerDaoResult: any = await cloneWebChatConfigurationdao(data);
    logger.info('cloneWebChatConfigurationdao', getCustomerDaoResult);
    if (getCustomerDaoResult.length > 0) {
      res.status(200).send({ statusCode: 200, message: 'Success', result: getCustomerDaoResult[0] });
    } else {
      res.status(200).send({ statusCode: 404, message: 'No Data Found' });
    }
  } catch (err) {

    logger.error('cloneWebChatConfigurationController - Unexpected Error:', req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function insertUpdateConcurrentChatHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      domainId: auth.domainId,
      isAllAgent: req?.body?.isAllAgent ?? null,
      allAgentLimit: req?.body?.allAgentLimit ?? null,
      isAgent: req?.body?.isAgent ?? null,
      isRoutingProfile: req?.body?.isRoutingProfile ?? null,
      agentOccupyDetails: req?.body?.agentOccupyDetails ?? null
    };
    const insertConcurrent: any = await insertUpdateConcurrentChat(data);
    logger.info('insertUpdateConcurrentChat', insertConcurrent);
    if (insertConcurrent[0]?.errCode === 0) {
      ioredisWhatsapp.to(`${auth.domainId}`).emit('getAgentTracker');
      res.status(200).send({ statusCode: 200, message: RESPONSE.insertConcurrent });
    } else {
      res.status(200).send({ statusCode: 200, message: RESPONSE.updateConcurrent });
    }
  } catch (err) {

    logger.error('insertUpdateConcurrentChatHandler - Unexpected Error:', req.headers, req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function AgentTrackerSession(req: any, res: any, done: any) {
  try {
    const domainId = req.headers.domainId;
    const response = await getAgentTracker(domainId);
    logger.info('getAgentTracker', response);
    if (response) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, agentTrackerRes: response });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, agentTrackerRes: [] });
    }
  } catch (err) {

    logger.error('AgentTrackerSession - Unexpected Error:', req.headers, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 */
export const getConcurrentChatHandler = async (req: any, res: any) => {
  try {
    const data: any = {
      domainId: req?.headers?.domainId
    };
    const gettConcurrentChatData: any = await getConcurrentChat(data);
    logger.info('getConcurrentChat', gettConcurrentChatData);
    if (gettConcurrentChatData && gettConcurrentChatData.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getConcurrentChatDataRes: gettConcurrentChatData });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getConcurrentChatDataRes: [] });
    }
  } catch (error) {
    req.log.error(error);
    logger.error('getConcurrentChatHandler - Unexpected Error:', req.headers, error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
};

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function UpdateIsAgentConcurrentChatHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      domainId: auth.domainId,
      isAgent: req?.body?.isAgent ?? 0,
      isAllAgent: req?.body?.isAllAgent ?? 0,
      isRoutingProfile: req?.body?.isRoutingProfile ?? 0
    };
    const updateConcurrent: any = await UpdateIsAgentConcurrentChat(data);
    logger.info('UpdateIsAgentConcurrentChat', updateConcurrent);
    if (updateConcurrent[0]?.errcode === 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.updateConcurrent });
    } else {
      res.status(200).send({ statusCode: 200, message: RESPONSE.failed });
    }
  } catch (err) {

    logger.error('UpdateIsAgentConcurrentChatHandler - Unexpected Error:', req.headers, req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function insertUpdateEmailFlowHistoryHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      emailFlowHistory: req?.body?.emailFlowHistory ?? null,
      flowName: req?.body?.flowName ?? null,
      html: req?.body?.html ?? null,
      css: req?.body?.css ?? null,
      domainId: auth.domainId,
      isActive: req?.body?.isActive ?? null,
      url: await generateImage(req?.body?.css, req?.body?.html, req?.body?.flowName)
      // url: req?.body?.url ?? null
    };
    const EmailFlow: any = await insertUpdateEmailFlowHistory(data);
    logger.info('insertUpdateEmailFlowHistory', EmailFlow);
    if (EmailFlow[0]?.errCode === 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.insertEmail });
    } else {
      res.status(200).send({ statusCode: 200, message: RESPONSE.updateEmail });
    }
  } catch (err) {

    logger.error('insertUpdateEmailFlowHistoryHandler - Unexpected Error:', req.headers, req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * ccs assign call back chat api
 */
export async function assignCallBackChatHandlers(
  req: any,
  res: any,
  done: any
) {
  try {
    const data: any = {
      assignedBy: req?.body?.assignedBy ?? null,
      assignedTo: req?.body?.assignedTo ?? null,
      assignedOn: Date.now(),
      ChatHistoryId: req.body.ChatHistoryId ?? null,
    };
    const assignRes: any = await assignCallBackChat(data);
    logger.info('assignCallBackChat', assignRes);
    if (assignRes && Array.isArray(assignRes) && assignRes.length > 0) {
      res
        .status(200)
        .send({ statusCode: 200, message: RESPONSE.update_asssign });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {

    logger.error('assignCallBackChatHandlers - Unexpected Error:', req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 */
export const getCallbackListNotificationHandlers = async (req: any, res: any) => {
  try {
    const data: any = {
      domainId: req?.headers?.domainId,
      channelType: req?.body?.channelType ?? null,
      offset: req?.body?.offset ?? null,
      limit: req?.body?.limit ?? null
    };
    const getChatNotification: any = await getCallbackListNotification(data);
    logger.info('getCallbackListNotification', getChatNotification);
    if (getChatNotification && getChatNotification.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getChatNotificationRes: getChatNotification });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getChatNotificationRes: [] });
    }
  } catch (error) {
    req.log.error(error);
    logger.error('getCallbackListNotificationHandlers - Unexpected Error:', req.headers, req.body, error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
};

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function InsertChatFlowVariablesHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      domainId: auth.domainId,
      chatFlowId: req.body?.chatFlowId,
      chatFlowVariable: req.body?.chatFlowVariable,
      chatFlowValue: req.body?.chatFlowValue
    };
    const InsertChatFlowVariablesResult = await InsertChatFlowVariablesDao(data);
    logger.info('InsertChatFlowVariablesDao', InsertChatFlowVariablesResult);
    if (InsertChatFlowVariablesResult[0][0]?.errCode === 0) {
      res.status(200).send({ statusCode: 200, message: InsertChatFlowVariablesResult[0][0]?.errMsg });
    } else {
      res.status(200).send({ statusCode: 401, message: InsertChatFlowVariablesResult[0][0]?.errMsg });
    }
  } catch (err) {
    logger.error('InsertChatFlowVariablesHandler - Unexpected Error:', req.headers, req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function GetChatFlowVariablesHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      domainId: auth.domainId,
      chatFlowId: req.params.chatFlowId
    };
    const GetChatFlowVariablesResult = await GetChatFlowVariablesDao(data);
    logger.info('GetChatFlowVariablesDao', GetChatFlowVariablesResult);
    if (GetChatFlowVariablesResult[0]?.length > 0) {
      res.status(200).send({ statusCode: 200, message: 'Success', data: GetChatFlowVariablesResult[0] });
    } else {
      res.status(200).send({ statusCode: 401, message: 'No data found', data: [] });
    }
  } catch (err) {
    logger.error('assignCallBackChatHandlers - Unexpected Error:', req.headers, req.params, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function GetAllChatSesssionMessageForNlp(req: any, res: any, done: any) {
  try {
    const data: any = {
      limit: req.query?.limit,
      offset: req.query?.offset
    };
    const getAllChatSessionMessageResult: any = await getAllChatSessionMessageDao(data);
    logger.info('getAllChatSessionMessageDao', getAllChatSessionMessageResult);
    if (getAllChatSessionMessageResult?.length > 0) {
      res.status(200).send({ statusCode: 200, message: 'Success', data: getAllChatSessionMessageResult });
    } else {
      res.status(200).send({ statusCode: 401, message: 'No data found', data: [] });
    }
  } catch (err) {
    logger.error('GetAllChatSesssionMessageForNlp - Unexpected Error:', req.query, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function getCompanyDomainHandler(req: any, res: any, done: any) {
  try {
    const data: any = req.params.domainId;
    const getCompanyDomainResult: any = await getCompanyDomainDao(data);
    logger.info('getCompanyDomainDao', getCompanyDomainResult);
    if (getCompanyDomainResult?.length > 0) {
      res.status(200).send({ statusCode: 200, message: 'Success', data: getCompanyDomainResult });
    } else {
      res.status(200).send({ statusCode: 401, message: 'No data found', data: [] });
    }
  } catch (err) {
    logger.error('getCompanyDomainHandler - Unexpected Error:', req.params, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */

export async function insertBulkIrrelevantQuestion(req: any, res: any, done: any) {
  try {

    const data = req.body?.questionData;
    for (const list of data) {
      const result = {
        chatFlowId: list?.chatFlowId,
        domainId: list?.domainId,
        questionAnswer: list?.questionAnswer,
        websiteId: list?.websiteId
      };
      const d = await insertUpdateIrrelevantQaDao(result);
    }
    res.status(200).send({ statusCode: 200, message: 'Success' });
  } catch (err) {
    logger.error('insertBulkIrrelevantQuestion - Unexpected Error:', req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */

export async function getCustomerLatestMsgHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      domainId: auth.domainId,
      customerId: parseInt(req.params.customerId, 10) ?? null
    };

    const getCustomerLatestMsgResult: any = await getCustomerLatestMsgDao(data);
    logger.info('getCustomerLatestMsgResult', getCustomerLatestMsgDao);
    if (getCustomerLatestMsgResult?.length > 0) {
      res.status(200).send({ statusCode: 200, message: 'Success', data: getCustomerLatestMsgResult });
    } else {
      res.status(200).send({ statusCode: 401, message: 'No data found', data: [] });
    }
  } catch (err) {
    logger.error('getCustomerLatestMsgHandler - Unexpected Error:', req.headers, req.params, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}
/**
 *
 * @param req
 * @param res
 * @param done
 */

export async function addChatFlowlanguageHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;

    const promises = req.body?.addonId?.map(async (elem: any, i: any) => {
      const data: any = {
        p_domainId: auth.domainId,
        p_chatFlowId: req.body?.chatFlowId,
        p_addonId: req.body?.addonId[i],
        p_isEnabled: req.body?.isEnabled[i],
        p_translatedText: JSON.stringify(req.body?.translatedText[i]),
        p_addonName: req.body?.addonName[i]
      };
      return addChatFlowlanguage(data);
    });
    const addChatFlowRes = await Promise.all(promises);
    if (Array.isArray(addChatFlowRes) && addChatFlowRes.every((row: any) => row[0]?.errmsg === 'Inserted')) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.insert_flow_language, result: addChatFlowRes });
    } else if (Array.isArray(addChatFlowRes) && addChatFlowRes.every((row: any) => row[0]?.errmsg === 'Updated')) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.updated_flow_language, result: addChatFlowRes });
    } else {
      res.status(200).send({ statusCode: 401, message: RESPONSE.failed_flow_language, result: addChatFlowRes });
    }
  } catch (err) {
    logger.error('addChatFlowlanguageHandler - Unexpected Error:', req.headers, req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */

export async function getChatFlowlanguageHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      p_domainId: auth.domainId,
      p_chatFlowId: req.params?.flowId
    };
    const getChatFlowRes: any = await getChatFlowlanguage(data);
    logger.info('getChatFlowlanguage', getChatFlowRes);
    if (getChatFlowRes?.length > 0) {
      res.status(200).send({ statusCode: 200, message: 'Success', result: getChatFlowRes });
    } else {
      res.status(200).send({ statusCode: 401, message: 'No data found', result: [] });
    }
  } catch (err) {
    logger.error('getChatFlowlanguageHandler - Unexpected Error:', req.headers, req.params, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */

export async function setChatFlowlanguageHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      p_domainId: auth.domainId,
      p_chatFlowId: req.body?.chatFlowId,
      p_isEnabled: req.body?.isEnabled
    };
    const getCustomerLatestMsgResult: any = await setChatFlowlanguage(data);
    logger.info('setChatFlowlanguage', getCustomerLatestMsgResult);
    if (getCustomerLatestMsgResult?.length > 0) {
      res.status(200).send({ statusCode: 200, message: 'Success', data: getCustomerLatestMsgResult });
    } else {
      res.status(200).send({ statusCode: 401, message: 'No data found', result: [] });
    }
  } catch (err) {
    logger.error('setChatFlowlanguageHandler - Unexpected Error:', req.headers, req.body, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}
