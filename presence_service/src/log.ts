import { configure, getLogger } from 'log4js';
import moment from 'moment';

const dir = process.platform === 'win32' ? '/tmp' : '/var/log';

// configure({
//     appenders: {
//         everything: {
//             type: 'file',
//             filename: `${dir}/${moment().format('YYYY-MM-DD')}-Presence-service.log`,
//             maxLogSize: 10485760, // 10 MB
//             backups: 5,
//         },
//         // console: { type: 'console' }
//     },
//     categories: {
//         default: {
//             appenders: [
//                 'everything',
//                 // 'console'
//             ], level: 'trace'
//         }
//     }
// });

export const logger = getLogger();