/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

import { SMS } from '../../helpers/constants';
import {
  getSupervisiorListHandler
} from './handler';

export default function smsHandler(server: any, options: any, next: any) {
  // get supervisior list
  server.get(
    SMS.GET_SUPERVISIOR_LIST,
    {
      preValidation: [server.validateSession],
      schema: {
        description: 'get supervisior list by domainId',
        tags: ['sms']
      }
    },
    getSupervisiorListHandler
  );
  next();
}
