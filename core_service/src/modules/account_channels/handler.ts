
/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */
import { forgotPassword } from '../../dao/channel';
import Hubspot from '../../services/hupspot.service';

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function forgotUserPassword(req: any, res: any, reply: any) {
  try {
    const data: any = {};
    data.email = req.body.email;
    data.new_pwd = req.body.new_pwd;
    data.type = req.body.type;
    data.called_by = req.body.called_by;
    data.log_id = req.body.log_id;
    const hubspotService = new Hubspot();
    const hubspotRes: any = await hubspotService.contactIdSearch(req.body.email);

    const forgotPasswordRes: any = await forgotPassword(data);
    if (forgotPasswordRes[0].errcode === 0) {
      const email_id = Buffer.from(`${data.email} `);
      const called_by = Buffer.from(`${data.called_by} `);
      const log_id = Buffer.from(`${forgotPasswordRes[0].log_id}`);
      const usermailId_Encrypted = email_id.toString('base64');
      const medium = called_by.toString('base64');
      const frame = log_id.toString('base64');
      const URL = `https://myaccount.worktual.co.uk/reset-password/?keys=${usermailId_Encrypted}=&medium=${medium}&frame=${frame}`;
      const params = {
        properties: {
          requested_password_reset: 'Yes',
          password_reset_link: URL,
        },
      };
      if (hubspotRes[0]?.id !== null || hubspotRes[0]?.id !== '') {
        await hubspotService.hubSpotSendMail(hubspotRes[0]?.id, params);
        res.send({ statusCode: 200, message: 'Mail send successfully' });
      } else {
        res.send({ statusCode: 422, message: 'No contactId found for this mail' });
      }
    } else {
      res.send({ statusCode: 422, message: forgotPasswordRes[0].errmsg });
    }
  } catch (err) {
    res.send({ statusCode: 500, message: 'internal server error' });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 */
export async function forgotUserPasswordReset(req: any, res: any, reply: any) {
  try {
    const data: any = {};
    data.email = req.body.email;
    data.new_pwd = req.body.new_pwd;
    data.type = req.body.type;
    data.called_by = req.body.called_by;
    data.log_id = req.body.log_id;

    const forgotPasswordRes: any = await forgotPassword(data);
    if (forgotPasswordRes[0].errcode === 0) {
      res.status(200).send({ statusCode: 200, message: 'Password reset successfully' });
    } else {
      res.status(200).send({ statusCode: 422, message: forgotPasswordRes[0].errmsg });
    }
  } catch (err) {
    res.send({ statusCode: 500, message: 'internal server error' });
  }
}
