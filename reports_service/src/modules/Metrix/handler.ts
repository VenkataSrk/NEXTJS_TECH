import { logger } from '../../../src/plugins/log';
import {
  dashBoardReport,
  // getAgentStateCountList,
  // agentIdOut,
  // agentConnectTime,
  // getAbandoned,
  // getAgentCount,
  // getAgentListRes,
  // getCallAverage,
  // getCallAverageCount,
  // getIncomingCall,
  // getSupervisorCount,
  // getSupervisorTotalcallCount,
  // getSupervisorTotalWebchatCount,
  // getTotalcallCount,
  // getTotalWebchatCount,
} from '../../dao/matrix.dao';

import { RESPONSE } from '../../helpers/constants';

/**
 *
 * @param req
 * @param res
 * @param done
 */

// db used not in live and qa
// export async function getAgentStateCountListHandler(
//   req: any,
//   res: any,
//   done: any
// ) {
//   try {
//     const auth: any = req.headers;
//     const data: any = {
//       domainId: auth.domainId,
//     };
    // const getAgentStateList: any = await getAgentStateCountList(data);
//     if (getAgentStateList) {
//       res.status(200).send({
//         statusCode: 200,
//         message: RESPONSE.success_message,
//         getAgentUserStateCountList: getAgentStateList,
//       });
//     } else {
//       res.status(200).send({
//         statusCode: 404,
//         message: RESPONSE.not_found,
//         getAgentUserStateCountList: [],
//       });
//     }
//   } catch (err) {
//     req.log.error(err);

//     res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
//   }
// }

export async function getCcaasreportDropDown(req: any, res: any) {
  try {
    const auth = req?.headers?.domainId;
    const agentDropDown: any = await dashBoardReport(auth);

    if (agentDropDown.length > 0) {
      res.status(200).send({
        statusCode : 200,
        message : RESPONSE.success_message,
        qName : agentDropDown[0],
        dispositionStatus : agentDropDown[1],
        aName : agentDropDown[2],
        transferred : agentDropDown[3],

        // to-do in future
        // primaryQueue : [],
        // primaryAgent : [],
        // voicemail : [],
        // dispositionStatus : [],
        // direction : [],
        // country : []
      });
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        qidQname: [],
        disposition : [],
        userDetails : [],
        transferred : [],
        // primaryQueue : [],
        // primaryAgent : [],
        // transferredQueue : [],
        // transferredAgent : [],
        // transferredType : [],
        // voicemail : [],
        // dispositionStatus : [],
        // direction : [],
      });
    }
  } catch (error) {
    console.log('getCcaasreportDropDown catch error: ', error);
    req.log.error(error);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
    logger.error('getCcaasreportDropDown catch error: ', error);

  }
}

// used only for testing so commenting
// naren
// export async function getAgentId(req: any, res: any) {
//   try {
//     const data : any = {
//       domainId:  req?.headers?.domainId,
//       ext: req?.headers?.ext
//     }

    // const output: any = await agentIdOut(data);
//     console.log("output: ", output[0].UserID);
//     if (output) {
//       res.status(200).send({ statusCode: 200, message: output });
//     }
//   } catch (error) {
//     console.log('getAgentId catch error: ', error);
//     req.log.error(error);
//     res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
//   }
// };

/**
 *
 * @param req
 * @param res
 * @param done
 */
/* export async function getCallPercentageHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {
      domainId: auth.domainId,
      fromDate: req.body.fromDate,
      toDate: req.body.toDate,
    };
    let totalDialHours = 0;
    let totalDialMinutes = 0;
    let totalDialSeconds = 0;
    let abandonedCalls = 0;
    const getCallAverageList: any = await getCallAverage(data);
    const agentConnect: any = await agentConnectTime(data);
    getCallAverageList.forEach((listData: any) => {
      if (listData?.disposition === 'abandoned') {
        abandonedCalls = abandonedCalls + 1;
      }
      const dialDuration = listData?.dialDuration?.split(':');
      if (dialDuration) {
        totalDialHours = totalDialHours + parseInt(dialDuration[0], 10);
        totalDialMinutes = totalDialMinutes + parseInt(dialDuration[1], 10);
        totalDialSeconds = totalDialSeconds + parseInt(dialDuration[2], 10);
      }
    });
    const totaldialDurationInSeconds =
      totalDialHours * 3600 + totalDialMinutes * 60 + totalDialSeconds;

    const totalCallDurationInSeconds = agentConnect;

    const totalAbandonedCalls = abandonedCalls;

    const calculateServiceLevel =
      (totaldialDurationInSeconds * 100) /
      (totalCallDurationInSeconds + totalAbandonedCalls);

    res.status(200).send({
      statusCode: 200,
      message: RESPONSE.success_message,
      getAgentCallList: getCallAverageList,
    });
  } catch (err) {
    req.log.error(err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
} */

/**
 *
 * @param req
 * @param res
 * @param done
 */
/* export async function getAgentCountHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      agentDetails: req.body.agentDetails,
      callType: req.body.callType,
      fromDate: req.body.fromDate,
      toDate: req.body.toDate,
    };
    if (data.agentDetails.length === 0) {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.empty_data,
      });
    } else {
      const getAgentCountRes: any = await getAgentCount(data);
      const getcallcountres: any = await getTotalcallCount();
      const getTotalWebchatCountRes: any = await getTotalWebchatCount();
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getAgentCallCount: getAgentCountRes,
        getTotalAgentCallCount: getcallcountres,
        getTotalWebchatCount: getTotalWebchatCountRes,
      });
    }
  } catch (err) {
    req.log.error(err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
} */

/**
 *
 * @param req
 * @param res
 * @param done
 */
/* export async function getSupervisorCountHandler(req: any, res: any, done: any) {
  try {
    const data: any = {
      agentDetails: req.body.agentDetails,
      callType: req.body.callType,
      fromDate: req.body.fromDate,
      toDate: req.body.toDate,
    };
    if (data.agentDetails.length === 0) {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.empty_data,
      });
    } else {
      const getSupervisorCountRes: any = await getSupervisorCount(data);
      const getcallcountres: any = await getSupervisorTotalcallCount();
      const getTotalWebchatCountRes: any =
        await getSupervisorTotalWebchatCount();
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getSupervisorCallCount: getSupervisorCountRes,
        getTotalSupervisorCallCount: getcallcountres,
        getSupervisorTotalWebchatCount: getTotalWebchatCountRes,
      });
    }
  } catch (err) {
    req.log.error(err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
} */

/**
 *
 * @param req
 * @param res
 * @param done
 */
/* export async function getAgentListHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      domainId: auth.domainId,
    };
    const getAgentList: any = await getAgentListRes(data);
    if (Array.isArray(getAgentList) && getAgentList.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getAgentUserList: getAgentList,
      });
    } else {
      res.status(200).send({
        statusCode: 404,
        message: RESPONSE.not_found,
        getAgentUserList: [],
      });
    }
  } catch (err) {
    req.log.error(err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
} */

/**
 *
 * @param req
 * @param res
 * @param done
 */
/* export async function getCallAverageHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {
      domainId: auth.domainId,
      fromDate: req.body.fromDate,
      toDate: req.body.toDate,
    };
    let totalCallHours = 0;
    let totalCallMinutes = 0;
    let totalCallSeconds = 0;
    const getCallAverageList: any = await getCallAverage(data);
    const getAbandonedList: any = await getAbandoned(data);
    const getIncomingCallList: any = await getIncomingCall(data);
    const getCallAveragecount: any = await getCallAverageCount(data);
    const agentConnect: any = await agentConnectTime(data);
    getCallAverageList.forEach((listData: any) => {
      const callDuration = listData?.callDuration?.split(':');
      if (callDuration) {
        totalCallMinutes = totalCallMinutes + parseInt(callDuration[1], 10);
      }
    });
    const totalCallDurationMins = totalCallMinutes;
    getCallAverageList.forEach((listData: any) => {
      if (
        listData?.disposition !== 'abandoned' ||
        listData?.disposition !== 'missed'
      ) {
        const queueDuration = listData?.queueDuration?.split(':');
        if (queueDuration && queueDuration.length === 3) {
          totalCallHours = totalCallHours + parseInt(queueDuration[0], 10);
          totalCallMinutes = totalCallMinutes + parseInt(queueDuration[1], 10);
          totalCallSeconds = totalCallSeconds + parseInt(queueDuration[2], 10);
        }
      }
    });
    const totalCallDurationInSeconds =
      totalCallHours * 3600 + totalCallMinutes * 60 + totalCallSeconds;
    const averageResponseTime =
      (getCallAveragecount * 100) / totalCallDurationInSeconds;
    const abandonedCallRate = (getAbandonedList / getIncomingCallList) * 100;
    const averageCallMins = agentConnect / totalCallDurationMins;
    res.status(200).send({
      statusCode: 200,
      message: RESPONSE.success_message,
      getTotalCallCount: getCallAveragecount,
      averageAnswerSpeed: averageResponseTime.toFixed(2),
      getAbandonedCallRate: abandonedCallRate.toFixed(2),
      getAverageCallMins: averageCallMins.toFixed(2),
      getAgentCallList: getCallAverageList,
    });
  } catch (err) {
    req.log.error(err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
} */
