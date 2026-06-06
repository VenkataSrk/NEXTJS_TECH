/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

import { ACCOUNT } from '../../helpers/constants';
import {
  forgotUserPassword,
  forgotUserPasswordReset,
} from './handler';

export default function accountChannelsHandler(
  server: any,
  options: any,
  next: any
) {

  // forget password
  server.post(
    ACCOUNT.FORGET_PASSWORD,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Forget password ',
        description: 'Forget password  api',
        tags: ['channels'],
      },
    },
    forgotUserPassword
  );

  // forget password reset
  server.post(
    ACCOUNT.FORGET_PASSWORD_RESET,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Forget password reset',
        description: 'Forget password reset api',
        tags: ['channels'],
      },
    },
    forgotUserPasswordReset
  );
  next();
}
