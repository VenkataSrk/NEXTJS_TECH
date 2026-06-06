import {
  getFacebookAgentACWReportHandlerDao,
  getFacebookAgentReportHandlerDao,
  getFacebookReportHandlerDao,
  getInstagramacwReportHandlerDao,
  getInstagramAgentacwReportHandlerDao,
  getInstagramReportHandlerDao,
  getSocialMediaAgentReportHandlerDao,
  getSocialMediaProfileReportHandlerDao,
  getWhatsappACWReportHandlerDao,
} from '../../../dao/socialMediadao';
import { RESPONSE, Type } from '../../../helpers/constants';
import { logger } from '../../../plugins/log';

export async function getSocialMediaProfileReportHandler(req: any, res: any) {
  try {
    const data: any = {
      startTmpstmp: req?.body?.startTmpstmp,
      endTmpstmp: req?.body?.endTmpstmp,
      //   timeZone: JSON.stringify(req?.body.timeZone), // furutre implemetation
      timeZone: null, // currently added null
      domainId: req?.headers?.domainId,
      agentName: req?.body?.agentName ? req?.body?.agentName : null,
      queueName: req?.body?.queueName ? req?.body?.queueName : null,
      disposition: req?.body?.disposition ? req?.body?.disposition : null,
      limit: req?.body?.limit,
      offset: req?.body?.offset,
    };

    const socialMediaResponse: any = await getSocialMediaProfileReportHandlerDao(data);

    if (socialMediaResponse[2][0]?.totalCount > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        type: Type.socialMediaProfile,
        agentList: socialMediaResponse[0],
        dispositionList: socialMediaResponse[1],
        totalCount: socialMediaResponse[2][0]?.totalCount,
        reportList: socialMediaResponse[3],
      });
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        totalCount: 0,
        reportList: [],
      });
    }
  } catch (error) {
    console.log('getSocialMediaProfileReportHandler catch error: ', error);
    req.log.error(error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getSocialMediaProfileReportHandler - Unexpected Error:', error);
  }
}

export async function getSocialMediaAgentReportHandler(req: any, res: any) {
  try {
    const data: any = {
      startTmpstmp: req?.body?.startTmpstmp,
      endTmpstmp: req?.body?.endTmpstmp,
      //   timeZone: JSON.stringify(req?.body.timeZone), // furutre implemetation
      timeZone: null, // currently added null
      domainId: req?.headers?.domainId,
      agentName: req?.body?.agentName ? req?.body?.agentName : null,
      queueName: req?.body?.queueName ? req?.body?.queueName : null,
      limit: req?.body?.limit,
      offset: req?.body?.offset,
    };

    const socialMediaAgentResponse: any = await getSocialMediaAgentReportHandlerDao(data);

    if (socialMediaAgentResponse[2][0]?.totalCount > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        type: Type.socialMediaAgent,
        agentList: socialMediaAgentResponse[0],
        dispositionList: socialMediaAgentResponse[1],
        totalCount: socialMediaAgentResponse[2][0]?.totalCount,
        reportList: socialMediaAgentResponse[3],
      });
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        totalCount: 0,
        reportList: [],
      });
    }
  } catch (error) {
    console.log('getSocialMediaAgentReportHandler catch error: ', error);
    req.log.error(error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getSocialMediaAgentReportHandler - Unexpected Error:', error);
  }
}

export async function getWhatsappACWReportHandler(req: any, res: any) {
  try {
    const data: any = {
      startTmpstmp: req?.body?.startTmpstmp,
      endTmpstmp: req?.body?.endTmpstmp,
      //   timeZone: JSON.stringify(req?.body.timeZone), // furutre implemetation
      timeZone: null, // currently added null
      domainId: req?.headers?.domainId,
      agentName: req?.body?.agentName ? req?.body?.agentName : null,
      queueName: req?.body?.queueName ? req?.body?.queueName : null,
      limit: req?.body?.limit,
      offset: req?.body?.offset,
    };

    const whatsappACWResponse: any = await getWhatsappACWReportHandlerDao(data);

    if (whatsappACWResponse[2][0]?.totalCount > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        type: Type.socialMediaACW,
        agentList: whatsappACWResponse[0],
        dispositionList: whatsappACWResponse[1],
        totalCount: whatsappACWResponse[2][0]?.totalCount,
        reportList: whatsappACWResponse[3],
      });
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        totalCount: 0,
        reportList: [],
      });
    }
  } catch (error) {
    console.log('getWhatsappACWReportHandler catch error: ', error);
    req.log.error(error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getWhatsappACWReportHandler - Unexpected Error:', error);
  }
}

export async function getInstagramReportHandler(req: any, res: any) {
  try {
    const data: any = {
      startTmpstmp: req?.body?.startTmpstmp,
      endTmpstmp: req?.body?.endTmpstmp,
      //   timeZone: JSON.stringify(req?.body.timeZone), // furutre implemetation
      timeZone: null, // currently added null
      domainId:req?.headers?.domainId,
      agentName: req?.body?.agentName ? req?.body?.agentName : null,
      disposition: req?.body?.disposition ? req?.body?.disposition : null,
      offset: req?.body?.offset,
      limit: req?.body?.limit,

    };

    const instagramResponse: any = await getInstagramReportHandlerDao(data);

    if (instagramResponse[2][0]?.totalCount > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        type: Type.instaMedia,
        agentList: instagramResponse[0],
        dispositionList: instagramResponse[1],
        totalCount: instagramResponse[2][0]?.totalCount,
        reportList: instagramResponse[3],
      });
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        totalCount: 0,
        reportList: [],
      });
    }
  } catch (error) {
    console.log('getinstagramResponseHandler catch error: ', error);
    req.log.error(error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getInstagramReportHandler - Unexpected Error:', error);
  }
}
export async function getInstagramacwReportHandler(req: any, res: any) {
  try {
    const data: any = {
      startTmpstmp: req?.body?.startTmpstmp,
      endTmpstmp: req?.body?.endTmpstmp,
      //   timeZone: JSON.stringify(req?.body.timeZone), // furutre implemetation
      timeZone: null, // currently added null
      domainId:req?.headers?.domainId,
      agentName: req?.body?.agentName ? req?.body?.agentName : null,
      disposition: req?.body?.disposition ? req?.body?.disposition : null,
      offset: req?.body?.offset,
      limit: req?.body?.limit,

    };

    const instagramResponse: any = await getInstagramacwReportHandlerDao(data);

    if (instagramResponse[2][0]?.totalCount > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        type: Type.instaMediaACW,
        agentList: instagramResponse[0],
        dispositionList: instagramResponse[1],
        totalCount: instagramResponse[2][0]?.totalCount,
        reportList: instagramResponse[3],
      });
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        totalCount: 0,
        reportList: [],
      });
    }
  } catch (error) {
    console.log('getinstagramResponseHandler catch error: ', error);
    req.log.error(error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getInstagramacwReportHandler - Unexpected Error:', error);
  }
}
export async function getInstagramAgentacwReportHandler(req: any, res: any) {
  try {
    const data: any = {
      startTmpstmp: req?.body?.startTmpstmp,
      endTmpstmp: req?.body?.endTmpstmp,
      //   timeZone: JSON.stringify(req?.body.timeZone), // furutre implemetation
      timeZone: null, // currently added null
      domainId:req?.headers?.domainId,
      agentName: req?.body?.agentName ? req?.body?.agentName : null,
      disposition: req?.body?.disposition ? req?.body?.disposition : null,
      offset: req?.body?.offset,
      limit: req?.body?.limit,

    };

    const instagramResponse: any = await getInstagramAgentacwReportHandlerDao(data);

    if (instagramResponse[2][0]?.totalCount > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        type: Type.instaMediaAgentACW,
        agentList: instagramResponse[0],
        dispositionList: instagramResponse[1],
        totalCount: instagramResponse[2][0]?.totalCount,
        reportList: instagramResponse[3],
      });
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        totalCount: 0,
        reportList: [],
      });
    }
  } catch (error) {
    console.log('getInstagramAgentacwReportHandler catch error: ', error);
    req.log.error(error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getInstagramAgentacwReportHandler - Unexpected Error:', error);
  }
}
export async function getFacebookReportHandler(req: any, res: any) {
  try {
    const data: any = {
      startTmpstmp: req?.body?.startTmpstmp,
      endTmpstmp: req?.body?.endTmpstmp,
      //   timeZone: JSON.stringify(req?.body.timeZone), // furutre implemetation
      timeZone: null, // currently added null
      domainId:req?.headers?.domainId,
      agentName: req?.body?.agentName ? req?.body?.agentName : null,
      disposition: req?.body?.disposition ? req?.body?.disposition : null,
      offset: req?.body?.offset,
      limit: req?.body?.limit,

    };

    const facebookResponse: any = await getFacebookReportHandlerDao(data);

    if (facebookResponse[2][0]?.totalCount > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        type: Type.facebookMediaACW,
        agentList: facebookResponse[0],
        dispositionList: facebookResponse[1],
        totalCount: facebookResponse[2][0]?.totalCount,
        reportList: facebookResponse[3],
      });
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        totalCount: 0,
        reportList: [],
      });
    }
  } catch (error) {
    console.log('getfacebookReportHandler catch error: ', error);
    req.log.error(error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getFacebookReportHandler - Unexpected Error:', error);
  }
}
export async function getFacebookAgentReportHandler(req: any, res: any) {
  try {
    const data: any = {
      startTmpstmp: req?.body?.startTmpstmp,
      endTmpstmp: req?.body?.endTmpstmp,
      //   timeZone: JSON.stringify(req?.body.timeZone), // furutre implemetation
      timeZone: null, // currently added null
      domainId:req?.headers?.domainId,
      agentName: req?.body?.agentName ? req?.body?.agentName : null,
      disposition: req?.body?.disposition ? req?.body?.disposition : null,
      offset: req?.body?.offset,
      limit: req?.body?.limit,

    };

    const facebookResponse: any = await getFacebookAgentReportHandlerDao(data);

    if (facebookResponse[2][0]?.totalCount > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        type: Type.facebookMediaACW,
        agentList: facebookResponse[0],
        dispositionList: facebookResponse[1],
        totalCount: facebookResponse[2][0]?.totalCount,
        reportList: facebookResponse[3],
      });
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        totalCount: 0,
        reportList: [],
      });
    }
  } catch (error) {
    console.log('getfacebookAgentReportHandler catch error: ', error);
    req.log.error(error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getfacebookAgentReportHandler - Unexpected Error:', error);
  }
}
export async function getFacebookAgentACWReportHandler(req: any, res: any) {
  try {
    const data: any = {
      startTmpstmp: req?.body?.startTmpstmp,
      endTmpstmp: req?.body?.endTmpstmp,
      //   timeZone: JSON.stringify(req?.body.timeZone), // furutre implemetation
      timeZone: null, // currently added null
      domainId: req?.headers?.domainId,
      agentName: req?.body?.agentName ? req?.body?.agentName : null,
      disposition: req?.body?.disposition ? req?.body?.disposition : null,
      offset: req?.body?.offset,
      limit: req?.body?.limit,

    };

    const facebookResponse: any = await getFacebookAgentACWReportHandlerDao(data);

    if (facebookResponse[2][0]?.totalCount > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        type: Type.facebookMediaACW,
        agentList: facebookResponse[0],
        dispositionList: facebookResponse[1],
        totalCount: facebookResponse[2][0]?.totalCount,
        reportList: facebookResponse[3],
      });
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        totalCount: 0,
        reportList: [],
      });
    }
  } catch (error) {
    console.log('getfacebookAgentACWReportHandler catch error: ', error);
    req.log.error(error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getfacebookAgentACWReportHandler - Unexpected Error:', error);
  }
}
