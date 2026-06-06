import { DASHBOARD } from '../../../helpers/constants';
import { getCompareDashboardHandler } from './handler';

export default function compareDashboardHandler(server: any, options: any, next: any) {

  server.post(
    DASHBOARD.GET_COMPARE_DASHBOARD,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get compare dashboard',
        description: 'get compare dashboard api',
        tags: ['compareDashboard'],
        // body: chatReportScheme.body,
        // response: chatReportScheme.response,
      },
    },
    getCompareDashboardHandler
  );

  next();
}
