import { SCHEDULEREPORT } from '../../helpers/constants';
import { deleteScheduleReportHandler, getScheduleReportHandler, insertScheduleReportHandler } from './handler';
import { deleteScheduleReportSchema } from './schema';

export default function scheduleReportsHandler(server: any, options: any, next: any) {

  // insert schedule report info api
  server.post(
    SCHEDULEREPORT.INSERT_SCHEDULE_REPORT_INFO,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'insert schedule report info',
        description: 'insert schedule report info api',
        tags: ['schedule_Reports'],
      },
    },
    insertScheduleReportHandler
  );

  // get schedule report info api
  server.get(
    SCHEDULEREPORT.GET_SCHEDULE_REPORT_INFO,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get schedule report info',
        description: 'get schedule report info api',
        tags: ['schedule_Reports'],
      },
    },
    getScheduleReportHandler
  );

  // delete schedule report info api
  server.put(
    `${SCHEDULEREPORT.DELETE_SCHEDULE_REPORT_INFO}/:uid`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'delete schedule report',
        description: 'delete schedule report api',
        tags: ['schedule_Reports'],
        // params: deleteScheduleReportSchema.params,
        // response: deleteScheduleReportSchema.response,
      },
    },
    deleteScheduleReportHandler
  );

  next();
}
