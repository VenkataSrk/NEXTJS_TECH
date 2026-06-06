
import { getCompareDashboard } from '../../../dao/compareDasboard.dao';
import { getAgentActivityDetailsModel, getAgentActivityListDetailsModel, getAgentReportDetailsModel, getAgentReportRecentChannelDetailsModel, getDispositionForChannelstHandlerDao, getLeaderboardDetails, getLeaderboardListDetails } from '../../../dao/leaderboard.dao';
import { RESPONSE } from '../../../helpers/constants';
import { logger } from '../../../plugins/log';

export async function getLeaderBoardDashboardHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      agentId: req.body.agentId,
      roleid: req.body.roleid,
      search: req.body.search,
      domain_id: auth.domainId,
      startTmpstmp: req.body.startTmpstmp ?? null,
      endTmpstmp: req.body.endTmpstmp ?? null,
    };
    const response: any = await getLeaderboardDetails(data);
    res.status(200).send({ statusCode: 200, message: RESPONSE.not_found, getLeaderboard: response[1] });
  } catch (err) {
    console.log(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function getAgentActivityListDetailsHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      uid: req.body.uid,
      ext: req.body.ext,
      domain_id: auth.domainId,
    };
    const response: any = await getAgentActivityListDetailsModel(data);
    const getLeaderboard = response[0];
    if (getLeaderboard.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getLeaderboard: response,
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getCallLiveDashboardRes: [] });
    }
  } catch (err) {
    console.log(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function getAgentActivityDetailsHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      uid: req.body.uid,
      ext: req.body.ext,
      domain_id: auth.domainId,
    };
    const response: any = await getAgentActivityDetailsModel(data);
    const getLeaderboard = response[0];
    if (getLeaderboard.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getLeaderboard: response,
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getCallLiveDashboardRes: [] });
    }
  } catch (err) {
    console.log(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function getLeaderBoardDashboardListHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      domainid: auth.domainId,
    };
    const response: any = await getLeaderboardListDetails(data);
    const getLeaderboard = response[0];
    if (getLeaderboard.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getLeaderboard: response,
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getCallLiveDashboardRes: [] });
    }
  } catch (err) {
    console.log(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function getAgentReportDashboardListHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      domainId: auth.domainId,
      agentId:  req?.body?.agentId
    };
    const getAgentReport: any = await getAgentReportDetailsModel(data);
    if (getAgentReport.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        result: getAgentReport,
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getAgentReport: [] });
    }
  } catch (err) {
    console.log(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function getAgentReportRecentListHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      domainId: auth.domainId,
      agentId:  req?.body?.agentId
    };

    const getAgentReportRecentChannel: any = await getAgentReportRecentChannelDetailsModel(data);
    if (getAgentReportRecentChannel.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        result: getAgentReportRecentChannel,
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getAgentReport: [] });
    }
  } catch (err) {
    console.log(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getAgentReportRecentChannel - Unexpected Error:', err);
  }
}

export async function getDispositionForChannelstHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      domainId: auth.domainId,
      channelType: req?.body?.channelType	 ?? 'All',
      startTmpstmp: req.body.startTmpstmp ?? null,
      endTmpstmp: req.body.endTmpstmp ?? null,
      userId: req.body.userId ?? null,
    };

    const getResponse: any = await getDispositionForChannelstHandlerDao(data);
    if (getResponse.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        result: getResponse,
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, result: [] });
    }
  } catch (err) {
    console.log(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getAgentReportRecentChannel - Unexpected Error:', err);
  }
}
