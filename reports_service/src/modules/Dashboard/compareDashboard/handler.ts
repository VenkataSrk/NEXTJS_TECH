
import { getCompareDashboard } from '../../../dao/compareDasboard.dao';
import { RESPONSE } from '../../../helpers/constants';

export async function getCompareDashboardHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      startTmpstmp: req.body.startTmpstmp,
      endTmpstmp: req.body.endTmpstmp,
      timeZone: req.body.timezone,
      domainId: auth.domainId,
    };
    const response: any = await getCompareDashboard(data);
    const getCompareDashboardDetails = response[0];
    const compareDashboardChartPoints = response[1];
    if (getCompareDashboardDetails.length > 0) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.success_message,
        dataset: compareDashboardChartPoints,
        getCompareDashboardRes: response,
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getCallLiveDashboardRes: [] });
    }
  } catch (err) {
    console.log(err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}
