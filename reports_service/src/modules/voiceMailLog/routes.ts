// /**
//  * @createdBy <team@vectone.com>
//  * @createdOn
//  */

// import { VOICEMAILLOG } from '../../helpers/constants';
// import { getVoiceMailAgentHandler, getVoiceMailHandler, voiceMailHandler } from './handler';
// import { getVoiceMailListSchema, getVoiceMailLogSchema, voiceMailInsertSchema } from './schema';

// export default function voiceMailLogHandler(server: any, options: any, next: any) {

//   // voiceMailLog Insert
//   server.post(
//     VOICEMAILLOG.VOICE_MAIL_LOG_INSERT,
//     {
//       preValidation: [server.validateSession],
//       schema: {
//         summary: 'insert voiceMailLog ',
//         description: 'insert voiceMailLog',
//         tags: ['voiceMailLog'],
//         // body: userinsertSchema.body,
//         response: voiceMailInsertSchema.response,
//       },
//     },
//     voiceMailHandler
//   );
//   // GET voiceMailLog
//   server.get(
//     VOICEMAILLOG.GET_VOICE_MAIL_LOG,
//     {
//       preValidation: [server.validateSession],
//       schema: {
//         summary: 'Get all voiceMailLog',
//         description: 'Get all voiceMailLog',
//         tags: ['voiceMailLog'],
//         response: getVoiceMailListSchema.response,
//       },
//     },
//     getVoiceMailHandler
//   );
//   // GET voiceMailLog
//   server.post(
//     VOICEMAILLOG.GET_VOICE_MAIL_LOG_AGENT,
//     {
//       preValidation: [server.validateSession],
//       schema: {
//         summary: 'Get voiceMailLog agent',
//         description: 'Get voiceMailLog agent',
//         tags: ['voiceMailLog'],
//         response: getVoiceMailLogSchema.response,
//       },
//     },
//     getVoiceMailAgentHandler
//   );
//   next();
// }
