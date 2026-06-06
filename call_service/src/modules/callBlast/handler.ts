import { v4 as uuidv4 } from 'uuid';
import { logger } from '../../../src/plugins/winston';
import { configs } from '../../config/app';
import { callBlastParticularHistory, deleteCallblast, deleteOutboundCampaignDraftDao, getCallBlastAutoAnswerDao, getCallBlastHistoryList, getCallBlastList, getCallBlastOverallScheduledCampaignsDao, getCallBlastPerticularChildScheduledCampaignsDao, getCustomerContactCallBlast, getCustomerContactCallBlastFilterDao, getOverallCallBlastList, insertUpdatCallBlastAutoAnswerHandlerDaos, updateCallBlastConfig, updateCallBlastStutas } from '../../dao/callBlast.dao';
import { insertNotificationDao } from '../../dao/notification.dao';
import { CALL_BLAST, RESPONSE } from '../../helpers/constants';
import { getRedisChannel } from '../../helpers/cron';
import { call, ioredisChat } from '../../plugins/db';

export async function insertUpdateCallBlastConfig(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const body = req.body;
    const data: any = {};
    data.cid = body.cid;
    data.campaignType = body.campaignType;
    data.domainId = auth.domainId;
    data.blastName = body.blastName;
    data.blastDescription = body.blastDescription;
    data.triggerType = body.triggerType;
    data.triggerDailyFile = body.triggerDailyFile;
    data.triggerCustomFile = body.triggerCustomFile;
    data.timeZone = body.timeZone;
    data.outboundNumber = body.outboundNumber;
    data.callerId = body.callerId;
    data.campaignMessageType = body.campaignMessageType;
    data.campaignMessage = body.campaignMessage;
    data.textSpeechFile = body.textSpeechFile;
    data.promptMessage = body.promptMessage;
    data.createdBy = auth.ext;
    data.contactType = body.contactType;
    data.contact = body.contact;
    data.importContact = body.importContact;
    data.importContactFile = body.importContactFile;
    data.addContact = body.addContact;
    data.companyName = auth.domainName;
    data.connectingAudio = body.connectingAudio;
    data.agentUnavailable = body.agentUnavailable;
    data.agentReject = body.agentReject;
    data.maxDuration = body.maxDuration ?? 0;

    const response: any = await updateCallBlastConfig(data);
    console.log('insertUpdateCallBlastConfig response', response, response?.[0]?.errcode);

    if (response && Array.isArray(response) && response?.[0]?.errcode === 0) {
      let path = '';
      const url = data.campaignMessage ? data.campaignMessage?.[0]?.url : null;

      if (url?.trim()) {
        path = url.replace('https://ccaas-storage.worktual.co.uk', '');
      }
      const Texturl = data.textSpeechFile ? data.textSpeechFile?.url : null;
      let textpath = '';
      if (Texturl?.trim()) {
        textpath = Texturl.replace('https://ccaas-storage.worktual.co.uk', '');
      }
      const promptMessage = data.promptMessage ? data.promptMessage?.path : '';
      const connectingAudio = data.connectingAudio ? data.connectingAudio?.path : '';
      const agentUnavailable = data.agentUnavailable ? data.agentUnavailable?.path : '';
      const agentReject = data.agentReject ? data.agentReject?.path : '';

      if (data.triggerType === 1) {
        const dataSet = {
          blastName: data?.blastName,
          campaign_action: 1,
          trigger_type: 1,
          pd_id: response[0]?.blastId,
          session_id: uuidv4(),
          triggered_id: 0,
          interval_id: '',
          req_type: `${data?.campaignType}` === '1' ? 'CALL_BLAST' : `${data?.campaignType}` === '2' ? 'PREDICTIVE_DIALER' : 'AGENT_SPECIFIC_CAMPAIGN',
          frequency: [],
          domain_name: data.companyName ?? 0,
          domain_id: JSON.stringify(data.domainId),
          caller_id: data.outboundNumber,
          play_file: data.campaignMessageType === 1 || data.campaignMessageType === 2
            ? path
              ? `/volume/nfsshare/ccaas${path}`
              : ''
            : textpath
              ? `/volume/nfsshare/ccaas${textpath}`
              : promptMessage
          ,
          did_list: data?.contactType === 1 ? data?.contact ?? [] : (data?.contactType === 2) ? data?.importContactFile ?? [] : (data?.contactType === 3) ? data?.addContact ?? [] : [],
          connecting_audio: connectingAudio,
          agentunavailable_audio: agentUnavailable,
          agentreject_audio: agentReject

        };

        console.log('publish data to umn imm', dataSet);
        const channelName = await getRedisChannel(data?.domainId);
        console.log('publish data to umn channelName', channelName);

        // call.publish(process.env.CAMPAIGN, JSON.stringify(dataSet));
        call.publish(channelName, JSON.stringify(dataSet));

        insertNotificationDao({
          uuid: uuidv4(),
          domainId: data?.domainId,
          toExt: null,
          notificationMsg: `Outbound campaign <b>${data?.blastName}</b> has been started successfully.`,
          type: 'Outbound campaign',
          channelType: 'Call',
          markAsRead: 0,
          timeStamp: new Date().getTime(),
        });
        ioredisChat.to(`${data?.domainId}`).emit('refreshNotification');

      }
      res.status(200).send({
        statusCode: 200,
        message: response,
      });
    }

  } catch (error) {
    console.log('insertUpdateCallBlastConfig err :::::', req?.body, 'error', error);
    logger.error('insertUpdateCallBlastConfig catch', { data: req?.body, Err: error });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function getCustomerContactHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const body = req.body;
    const data: any = {};
    data.domainId = auth.domainId;
    data.doNotDisturb = body?.doNotDisturb;
    data.customerType = body.customerType;
    data.search = body.search;
    data.customerTag = body.customerTag;
    data.source = body.source;
    data.country = body.country;
    data.offset = body.offset;
    data.limit = body.limit;
    const response: any = await getCustomerContactCallBlast(data);
    if (response && Array.isArray(response)) {
      res.status(200).send({
        statusCode: 200,
        contact: response[0],

      });
    }

  } catch (error) {
    console.log('getCustomerContactHandler err :::::', error);
    logger.error('getCustomerContactHandler catch', { data: req?.body, Err: error });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function getCallBlastListHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const body = req.body;
    const data: any = {};
    data.blastId = body.blastId;
    data.domainId = auth.domainId;
    data.campaignName = body.campaignName;
    data.campaignType = body.campaignType;
    data.createdBy = body.createdBy;
    data.timeZone = body.timeZone;
    data.offset = body.offset;
    data.limit = body.limit;
    data.statusName = body.statusName;

    const response: any = await getCallBlastList(data);
    if (response && Array.isArray(response)) {
      res.status(200).send({
        statusCode: 200,
        result: response[0],
        campaignName: response[1],
        timeZone: response[2],
        createrName: response[3],
        count: response[4],
        triggerStatus: response[5]
      });
    }

  } catch (error) {
    console.log('getCallBlastList err :::::', error);
    logger.error('getCallBlastListHandler catch', { data: req?.body, Err: error });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function getOverallCallBlastListHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const body = req.body;
    const data: any = {};
    data.blastId = body.blastId;
    data.domainId = auth.domainId;
    data.campaignName = body.campaignName;
    data.campaignType = body.campaignType;
    data.createdBy = body.createdBy;
    data.timeZone = body.timeZone;
    data.offset = body.offset;
    data.limit = body.limit;
    data.triggerStatus = body.triggerStatus;

    const response: any = await getOverallCallBlastList(data);
    if (response && Array.isArray(response)) {
      res.status(200).send({
        statusCode: 200,
        result: response[0],
        campaignName: response[1],
        timeZone: response[2],
        createrName: response[3],
        count: response[4],
        triggerStatus: response[5]
      });
    }

  } catch (error) {
    console.log('getCallBlastList err :::::', error);
    logger.error('getOverallCallBlastListHandler catch', { data: req?.body, Err: error });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}
export async function updateCallBlastStutasHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const body = req.body;
    const data: any = {};
    data.blastId = body.blastId;
    data.domainId = auth?.domainId ?? body?.domainId;
    data.blastStatus = body.blastStatus;
    data.pause = body.pause;
    data.forceStop = body.forceStop;

    const blastData = body?.blastData;

    const response: any = await updateCallBlastStutas(data);
    if (response && Array.isArray(response)) {
      // console.log('getCallBlastList response', response);
      if (data?.forceStop) {
        const Stopdata = {
          campaign_action: 5,
          trigger_type: blastData?.triggerType,
          pd_id: data.blastId,
          session_id: uuidv4(),
          req_type: blastData?.campaignType === 1 ? 'CALL_BLAST' : blastData?.campaignType === 2 ? 'PREDICTIVE_DIALER' : 'AGENT_SPECIFIC_CAMPAIGN',
          domain_id: JSON.stringify(blastData?.domainId),
        };
        console.log('campaign_action>>>> Stopdata', Stopdata);
        logger.info('campaign_action>>>> Stopdata', Stopdata);
        const channelName = await getRedisChannel(blastData?.domainId);
        console.log('publish data to umn channelName', channelName);
        logger.info('publish data to umn channelName', channelName);

        // call.publish(process.env.CAMPAIGN, JSON.stringify(Stopdata));
        call.publish(channelName, JSON.stringify(Stopdata));

      } else if (data?.pause === 1) {
        const pauseData = {
          campaign_action: 2,
          trigger_type: blastData?.triggerType,
          pd_id: data.blastId,
          session_id: uuidv4(),
          req_type: blastData?.campaignType === 1 ? 'CALL_BLAST' : blastData?.campaignType === 2 ? 'PREDICTIVE_DIALER' : 'AGENT_SPECIFIC_CAMPAIGN',
          domain_id: JSON.stringify(blastData?.domainId),
        };
        console.log('campaign_action>>>>pauseData', pauseData);
        logger.info('campaign_action>>>> pauseData', pauseData);
        const channelName = await getRedisChannel(blastData?.domainId);
        console.log('publish data to umn channelName', channelName);
        logger.info('publish data to umn channelName', channelName);

        call.publish(channelName, JSON.stringify(pauseData));

        // call.publish(process.env.CAMPAIGN, JSON.stringify(pauseData));
      } else if (data?.pause === 0) {
        const resumeData = {
          campaign_action: 3,
          trigger_type: blastData?.triggerType,
          pd_id: data.blastId,
          session_id: uuidv4(),
          req_type: blastData?.campaignType === 1 ? 'CALL_BLAST' : blastData?.campaignType === 2 ? 'PREDICTIVE_DIALER' : 'AGENT_SPECIFIC_CAMPAIGN',
          domain_id: JSON.stringify(blastData?.domainId),
        };
        logger.info('campaign_action>>>> resumeData', resumeData);
        const channelName = await getRedisChannel(blastData?.domainId);
        console.log('publish data to umn channelName', channelName);
        logger.info('publish data to umn channelName', channelName);
        call.publish(channelName, JSON.stringify(resumeData));

        // call.publish(process.env.CAMPAIGN, JSON.stringify(resumeData));
      }
      // EN_CCAAS_CAMPAIGN_ACTION_NULL = 0,
      // EN_CCAAS_CAMPAIGN_ACTION_ENABLE = 1,
      // EN_CCAAS_CAMPAIGN_ACTION_PAUSE = 2,
      // EN_CCAAS_CAMPAIGN_ACTION_RESUME = 3,
      // EN_CCAAS_CAMPAIGN_ACTION_COMPLETED = 4,
      // EN_CCAAS_CAMPAIGN_ACTION_FORCE_STOP = 5,
    }
    res.status(200).send({
      statusCode: 200,
      result: response[0],
    });
  } catch (error) {
    console.log('getCallBlastList err :::::', req?.body, 'error', error);
    logger.error('updateCallBlastStutasHandler catch', { data: req?.body, Err: error });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}
export async function deleteCallblastHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const body = req.body;
    const data: any = {};
    data.blastId = body.blastId;
    data.domainId = auth.domainId;

    const response: any = await deleteCallblast(data);
    if (response && Array.isArray(response)) {
      res.status(200).send({
        statusCode: 200,
        result: response[0],
      });
    }

  } catch (error) {
    console.log('deleteCallblastHandler err :::::', req?.body, 'error', error);
    logger.error('deleteCallblastHandler catch', { data: req?.body, Err: error });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function getCallBlastHistoryListHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const body = req.body;
    const data: any = {};
    data.blastId = body.blastId;
    data.domainId = auth.domainId;
    data.campaignName = body.campaignName;
    data.campaignType = body.campaignType;
    data.createdBy = body.createdBy;
    data.timeZone = body.timeZone;
    data.startDate = body.startDate;
    data.endDate = body.endDate;
    data.statusName = body.statusName;
    data.offset = body.offset;
    data.limit = body.limit;
    data.outboundNumber = data.outboundNumber;

    const response: any = await getCallBlastHistoryList(data);
    if (response && Array.isArray(response)) {
      res.status(200).send({
        statusCode: 200,
        result: response[0],
        campaignName: response[1],
        timeZone: response[2],
        createrName: response[3],
        count: response[4],
        Campaignstatus: response[5]

      });
    }

  } catch (error) {
    console.log('getCallBlastList err :::::', error);
    logger.error('getCallBlastHistoryListHandler catch', { data: req?.body, Err: error });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function getCallBlastParticularHistory(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const body = req.body;
    const data: any = {
      blastId: body?.blastId ?? null,
      triggeredId: body?.triggeredId ?? null,
      customerName: body?.customerName ?? null,
      callStatus: body?.callStatus ?? null,
      agentName: body?.agentName ?? null,
      phoneNumber: body?.phoneNumber ?? null,
      limit: body?.limit ?? 10,
      offset: body?.offset ?? 0,
      dispositionId: body.dispositionId ?? null
    };

    const response: any = await callBlastParticularHistory(data);
    if (response.length > 0 && Array.isArray(response)) {
      res.status(200).send({
        statusCode: 200,
        result: response,
      });
    }

  } catch (error) {
    console.log('callBlastParticularHistory err :::::', req?.body, 'error', error);
    logger.error('getCallBlastParticularHistory catch', { data: req?.body, Err: error });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function getCallBlastAutoAnswerHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const body = req.body;
    const data: any = {};
    data.domainId = auth.domainId;
    const response: any = await getCallBlastAutoAnswerDao(data);
    if (response.length > 0 && Array.isArray(response)) {
      res.status(200).send({
        statusCode: 200,
        result: response,
      });
    }
  } catch (error) {
    console.log('getCallBlastAutoAnswerHandler err :::::', error);
    logger.error('getCallBlastAutoAnswerHandler catch', { Err: error });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function insertUpdatCallBlastAutoAnswerHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const body = req.body;
    const data: any = {};
    data.isAutoAnswer = body.isAutoAnswer;
    data.domainId = auth.domainId;
    const response: any = await insertUpdatCallBlastAutoAnswerHandlerDaos(data);
    if (response && Array.isArray(response) && (response?.[0]?.errcode === 0 || response?.[0]?.errcode === 1)) {
      ioredisChat.to(`${data?.domainId}`).emit('auto_answer', data);
      res.status(200).send({
        statusCode: 200,
        result: response[0],
      });
    } else {
      res.status(200).send({
        statusCode: 404,
        result: [],
      });
    }
  } catch (error) {
    console.log('insertUpdatCallBlastAutoAnswerHandler err :::::', error);
    logger.error('insertUpdatCallBlastAutoAnswerHandler catch', { data: req?.body, Err: error });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function getCustomerContactFilterValueHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const body = req.body;
    const data: any = {};
    data.domainId = auth.domainId;
    const response: any = await getCustomerContactCallBlastFilterDao(data);
    if (response && Array.isArray(response)) {
      res.status(200).send({
        statusCode: 200,
        customerType: response[0],
        customerTag: response[1],
        Source: response[2],
        country: response[3]

      });
    }

  } catch (error) {
    console.log('getCustomerContactHandler err :::::', error);
    logger.error('getCustomerContactHandler catch', { data: req?.body, Err: error });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function deleteOutboundCampaignDraftHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const body = req.body;
    const data: any = {};
    data.domainId = auth.domainId;
    data.campDraftId = body.campDraftId;

    const response: any = await deleteOutboundCampaignDraftDao(data);
    if (response && Array.isArray(response)) {
      res.status(200).send({
        statusCode: 200,
        result: response[0],
      });
    }

  } catch (error) {
    console.log('deleteOutboundCampaignDraftHandler err :::::', req?.body, 'error', error);
    logger.error('deleteOutboundCampaignDraftHandler catch', { data: req?.body, Err: error });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function getCallBlastOverallScheduledCampaigns(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {
      domainId: auth.domainId
    };

    const response: any = await getCallBlastOverallScheduledCampaignsDao(data);
    if (Array.isArray(response) && response.length > 0) {
      res.status(200).send({
        statusCode: 200,
        result: response,
      });
    } else {
      res.status(200).send({
        statusCode: 404,
        result: [],
      });
    }

  } catch (error) {
    console.log('getCallBlastOverallScheduledCampaignsDao err :::::', req?.body, 'error', error);
    logger.error('getCallBlastOverallScheduledCampaignsDao catch', { data: req?.body, Err: error });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function getCallBlastPerticularChildScheduledCampaigns(req: any, res: any, done: any) {
  try {
    const data: any = {
      blastId: req.body.blastId ?? 0,
      offset: req.body.offset ?? 0,
      limit: req.body.limit ?? 100,
    };
    const response: any = await getCallBlastPerticularChildScheduledCampaignsDao(data);
    if (Array.isArray(response) && response.length > 0) {
      res.status(200).send({
        statusCode: 200,
        result: response,
      });
    } else {
      res.status(200).send({
        statusCode: 404,
        result: [],
      });
    }

  } catch (error) {
    console.log('getCallBlastPerticularChildScheduledCampaigns err :::::', req?.body, 'error', error);
    logger.error('getCallBlastPerticularChildScheduledCampaigns catch', { data: req?.body, Err: error });
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}
