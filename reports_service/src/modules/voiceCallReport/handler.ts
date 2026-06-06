/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

// import { getAbandonedCallProfile, getIntervalReport, getQueueDispositionStataus, getReportVoiceCount, insertReportVoiceCount } from '../../dao/reportTime.dao';

import console from 'console';
import { RESPONSE } from '../../helpers/constants';

/**
 *
 * @param req
 * @param res
 * @param done
 */
/* export async function getQueueInboundCallHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {
      domainId: auth.domainId,
      fromDate: req.body.fromDate,
      toDate: req.body.toDate
    };
    await insertReportVoiceCount();
    const result: any = await getReportVoiceCount(data);
    const labelArr: any = [];
    const recivedCallsCountArr: any = [];
    const abandonedCallsCountArr: any = [];
    const answeredCallsCountArr: any = [];
    result.getVoiceMetrixReports.map((report: any) => {
      labelArr.push(report._id.queueName);
      recivedCallsCountArr.push(report.recivedCallsCount);
      abandonedCallsCountArr.push(report.abandonedCallsCount);
      answeredCallsCountArr.push(report.answeredCallsCount);
    });
    const datasets = {
      labels: labelArr,
      datasets: [
        {
          label: 'Received',
          backgroundColor: 'rgb(47, 150, 214)',
          borderColor: 'white ',
          borderWidth: 1.5,
          hoverBackgroundColor: 'rgba(255,99,132,0.4)',
          hoverBorderColor: 'rgba(255,99,132,1)',
          data: recivedCallsCountArr,
          borderSkipped: false
        },
        {
          label: 'Abonded Calls',
          backgroundColor: 'rgba(214, 47, 147)',
          borderColor: 'white',
          borderWidth: 1.5,
          hoverBackgroundColor: 'rgba(255,205,86,0.4)',
          hoverBorderColor: 'rgba(255,205,86,1)',
          data: abandonedCallsCountArr,
          boderSkipped: false
        },
        {
          label: 'Answered',
          backgroundColor: 'rgb(15, 92, 40)',
          borderColor: 'white',
          borderWidth: 1.5,
          hoverBackgroundColor: 'rgba(255,205,86,0.4)',
          hoverBorderColor: 'rgba(255,205,86,1)',
          data: answeredCallsCountArr,
          borderSkipped: false,
          borderRadius: {
            topLeft: 10,
            topRight: 10,
          }
        }
      ]

    };
    if (result) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getQueueInboundCallRes: datasets
      });
    } else {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.not_found,
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
/* export async function getQueueDispositionStatausHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {
      domainId: auth.domainId,
      fromDate: req.body.fromDate,
      toDate: req.body.toDate
    };
    await insertReportVoiceCount();
    const getQueueDisposition: any = await getQueueDispositionStataus(data);
    const queueName: any = [];
    const resolvedResultData: any = [];
    const unresolvedResultData: any = [];
    const transferredResultData: any = [];
    const followUpresultData: any = [];
    const closedResultData: any = [];
    const datas: any = [];
    const resultReport: any = [];
    await getQueueDisposition.map((report: any) => {
      queueName.push(report.queueName),
        datas.push(report.dispositions);
    });
    datas.map((innerArray: any) => {
      innerArray.forEach((item: any) => {
        resultReport.push(item);
      });
    });
    await resultReport.map((report: any) => {
      switch (report.disposition) {
        case 'Resolved':
          resolvedResultData.push(report.count);
          break;
        case 'Unresolved':
          unresolvedResultData.push(report.count);
          break;
        case 'Transferred':
          transferredResultData.push(report.count);
          break;
        case 'Follow-up':
          followUpresultData.push(report.count);
          break;
        case 'Closed':
          closedResultData.push(report.count);
          break;
        default:
          break;
      }
    });
    const dataResult = {
      labels: queueName,
      datasets: [
        {
          label: 'Resolved',
          data: resolvedResultData,
          pointRadius: 0,
          borderColor: 'red',
          borderWidth: 2,
        },
        {
          label: 'Unresolved',
          data: unresolvedResultData,
          pointRadius: 0,
          borderColor: 'blue',
          borderWidth: 2,
        },
        {
          label: 'Follow-up',
          data: transferredResultData,
          pointRadius: 0,
          borderColor: 'black',
          borderWidth: 2,
        }, {
          label: 'Transferred',
          data: followUpresultData,
          pointRadius: 0,
          borderColor: 'green',
          borderWidth: 2,
        }, {
          label: 'Closed',
          data: closedResultData,
          pointRadius: 0,
          borderColor: 'orange',
          borderWidth: 2,
        }
      ]
    };
    if (getQueueDisposition) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getDispositionStatusRes: dataResult
      });
    } else {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.not_found,
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
/* export async function getIntervalReportHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {
      domainId: auth.domainId,
      fromDate: req.body.fromDate,
      toDate: req.body.toDate
    };
    await insertReportVoiceCount();
    const getIntervalRes: any = await getIntervalReport(data);
    const labelArr: any = [];
    const recivedCallsCountArr: any = [];
    const abandonedCallsCountArr: any = [];
    const answeredCallsCountArr: any = [];
    getIntervalRes.map((report: any) => {
      labelArr.push(report._id.hour);
      recivedCallsCountArr.push(report.recivedCallsCount);
      abandonedCallsCountArr.push(report.abandonedCallCount);
      answeredCallsCountArr.push(report.answeredCallcount);
    });
    const datasets = {
      labels: labelArr,
      datasets: [
        {
          label: 'Recieved',
          data: recivedCallsCountArr,
          borderColor: 'blue',
          pointRadius: 0,
          borderWidth: 2,
          backgroundColor: 'rgba(255, 99, 132, 0.5)',
        },
        {
          label: 'Abandoned Calls',
          data: abandonedCallsCountArr,
          borderColor: 'red',
          borderWidth: 2,
          pointRadius: 0,
          backgroundColor: 'rgba(53, 162, 235, 0.5)',
        },
        {
          label: 'Answered',
          data: answeredCallsCountArr,
          borderColor: 'green',
          borderWidth: 2,
          pointRadius: 0,
          backgroundColor: 'rgba(53, 162, 235, 0.5)',
        }
      ],
    };
    if (getIntervalRes) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getIntervalReportsRes: datasets
      });
    } else {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.not_found,
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
/* export async function getAbandonedCallProfileReportHandler(req: any, res: any, done: any) {
  try {
    const auth = req.headers;
    const data: any = {
      domainId: auth.domainId,
      fromDate: req.body.fromDate,
      toDate: req.body.toDate
    };
    await insertReportVoiceCount();
    const getAbandonedCallRes: any = await getAbandonedCallProfile(data);
    const labelArr: any = [];
    const abandonedCallsCountArr: any = [];
    getAbandonedCallRes.map((report: any) => {
      labelArr.push(report._id.queueName);
      abandonedCallsCountArr.push(report.abandonedCallCount);
    });
    const datasets = {
      labels: labelArr,
      datasets: [
        {
          label: '# of Votes',
          data: abandonedCallsCountArr,
          backgroundColor: [
            'rgba(255, 99, 132, 0.2)',
            'rgba(54, 162, 235, 0.2)',
            'rgba(255, 206, 86, 0.2)',
            'rgba(75, 192, 192, 0.2)',
            'rgba(153, 102, 255, 0.2)',
            'rgba(255, 159, 64, 0.2)',
          ],
          borderColor: [
            'rgba(255, 99, 132, 1)',
            'rgba(54, 162, 235, 1)',
            'rgba(255, 206, 86, 1)',
            'rgba(75, 192, 192, 1)',
            'rgba(153, 102, 255, 1)',
            'rgba(255, 159, 64, 1)',
          ],
          borderWidth: 1,
        },
      ],
    };
    if (getAbandonedCallRes) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        getAbandonedCallProfileRes: datasets
      });
    } else {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.not_found,
      });
    }
  } catch (err) {
    req.log.error(err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
} */
