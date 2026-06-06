/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

import { FastifyReply, FastifyRequest } from 'fastify';
import * as jwt from 'jsonwebtoken';
import { configs } from '../config/app';
import {
  invalidCredentials,
  missingAuthenticationToken,
} from '../helpers/response-builders';

/**
 * Allow authorized user to access resource, without narrowing scope to role/account
 *
 * @export
 * @param {FastifyRequest<{}>} req
 * @param {FastifyReply<{}>} res
 * @param {(err?: Error) => void} done
 * @returns
 */
export function validateSession(
  req: FastifyRequest<{}>,
  res: FastifyReply,
  done: (err?: Error) => void
) {
  try {
    // let decoded: any = {};
    if (!req.headers || !req.headers.authorization) {
      res
        .status(401)
        .send(missingAuthenticationToken('Missing authentication token'));
    } else {
      const decoded: any = jwt.verify(req.headers.authorization, configs.jwtsecret);
      if (!decoded.username && !decoded.password && !decoded.deviceId) {
        res
          .status(403)
          .send(
            invalidCredentials('Invalid credentials in authentication token')
          );
      } else {
        req.headers = {
          username: decoded.username,
          password: decoded.password,
          deviceId: decoded.deviceId,
          sipLoginId: decoded.sipLoginId,
          roleId: decoded.roleId,
          domainId: decoded.domainId,
          ext: decoded.ext,
          companyId: decoded.companyId,
          enetepriseid: decoded.enetepriseid,
          hostAddress: decoded.hostAddress,
          orderId: decoded.orderId,
          dirUserId: decoded.dirUserId,
          ccas_role_info: decoded.ccas_role_info,
          callerId: decoded.caller_id,
          domainName: decoded.domain_name,
        };
        return done();
      }
    }
  } catch (err) {
    res
      .status(403)
      .send(invalidCredentials('Invalid credentials in authentication token'));
  }
}
