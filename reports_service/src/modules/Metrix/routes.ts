/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

import { METRIX } from '../../helpers/constants';
import {
  getCcaasreportDropDown,
} from './handler';

export default function matrixHandler(server: any, options: any, next: any) {

  server.get(
    METRIX.GET_REPORT_DROPDOWN,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get dropdown details of agent',
        description: 'get dropdown values of agent',
        tags: ['METRIX'],
      },
    },
    getCcaasreportDropDown
  );
  next();
}
