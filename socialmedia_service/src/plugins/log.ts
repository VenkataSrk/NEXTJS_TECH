/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

import { configure, getLogger } from 'log4js';

const dir = process.platform === 'win32' ? '/tmp' : '/var/log';

configure({
  appenders: {
    daily: {
      type: 'dateFile',
      filename: `${dir}/socialmedia_service.log`,
      pattern: 'yyyy-MM-dd',
      alwaysIncludePattern: true,
      keepFileExt: true,
      compress: false,
      daysToKeep: 7
    }
  },
  categories: {
    default: { appenders: ['daily'], level: 'info' }
  }
});

export const logger = getLogger();
