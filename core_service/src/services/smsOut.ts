import axios from 'axios';
import { parsePhoneNumberFromString } from 'libphonenumber-js';
import { logger } from '../plugins/winston';

export async function sendSmsMessageOutbound(data: any) {
  try {
    const rawNumber = data?.toId;
    const defaultCountry = 'IN';
    const phoneNumber = parsePhoneNumberFromString(rawNumber, defaultCountry);
    if (phoneNumber && phoneNumber.isValid()) {
      const body = {
        destination: phoneNumber?.number,
        content: data?.htmlContent
      };
      const response = await axios.post(process?.env?.smsOutInfo, body);
      logger.info('sendSmsMessageOutbound>>', body, response.data);
      return response;
    }
    logger.info('❌ Invalid or unrecognized phone number');
    return 'Invalid or unrecognized phone number';

  } catch (error) {
    logger.error('Error sending message:', error);
  }
}
