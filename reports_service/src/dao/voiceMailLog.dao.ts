// import { VoiceMailLogSchema } from '../models/voiceMailLog.models';

// export const voiceMailLogDao = async (data: any) => {
//   try {
//     return new Promise(async (resolve, reject) => {
//       VoiceMailLogSchema.create(data).then((res: any) => {
//         resolve(res);
//       }).catch((err: any) => {
//         reject(err);
//       });
//     });
//   } catch (err: any) {
//     console.log(err);
//   }

// };
// export const getVoiceMailHandlerDao = async (data: any) => {
//   try {
//     return new Promise(async (resolve, reject) => {
//       VoiceMailLogSchema.find({ companyId: data })
//         .then((res: any) => {

//           resolve(res);
//         }).catch((err: any) => {
//           reject(err);
//         });
//     });
//   } catch (err: any) {
//     console.log(err);
//   }

// };
// export const getVoiceMailAgentDao = async (data: any) => {
//   try {
//     return new Promise(async (resolve, reject) => {
//       VoiceMailLogSchema.find({ agent: data })
//         .then((res: any) => {
//           resolve(res);
//         }).catch((err: any) => {
//           reject(err);
//         });
//     });
//   } catch (err: any) {
//     console.log(err);
//   }

// };
