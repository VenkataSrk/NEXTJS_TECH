// /**
//  * @createdBy <team@vectone.com>
//  * @createdOn
//  */

// import { WEBHOOK } from '../../helpers/constants';
// import {
//   avamoHandler
// } from './handler';
// import { avamoSchema } from './schema';

// export default function webhookRoutes(server: any, options: any, next: any) {

//   // AVAMO_WEBHOOK
//   server.post(
//     WEBHOOK.AVAMO_WEBHOOK,
//     {
//       // preValidation: [server.validateSession],
//       schema: {
//         summary: 'avamo conversation history',
//         description: 'avamo conversation history api',
//         tags: ['webhook'],
//         response: avamoSchema.response,
//       },
//     },
//     avamoHandler
//   );
//   next();
// }
