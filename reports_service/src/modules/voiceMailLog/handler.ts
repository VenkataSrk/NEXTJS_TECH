// // import { getVoiceMailAgentDao, getVoiceMailHandlerDao, voiceMailLogDao } from '../../dao/voiceMailLog.dao';

// /**
//  * @createdBy <team@vectone.com>
//  * @createdOn
//  */
// import { v4 as uuidv4 } from 'uuid';

// export async function voiceMailHandler(req: any, res: any, done: any) {
//   try {
//     const data: any = req.body;
//     data.uuid = uuidv4();
//     const userData: any = {
//       uuid: data.uuid,
//       filePath: data.filePath,
//       companyId: data.companyId,
//       domainId: data.domainId,
//       createdDAte: Date.now(),
//       agent: data.agent,
//       fileUrl: data.fileUrl,
//       queue: data.queue,
//       customer: data.customer,
//       status: data.status,
//       transcription: data.transcription
//     };
//     const userRes: any = await voiceMailLogDao(userData);

//     if (userRes) {
//       res.status(200).send({ statusCode: 200, message: 'Inserted  successfully' });
//     } else {
//       res.status(424).send({ statusCode: 424, message: 'failed' });
//     }
//   } catch (err) {
//     req.log.error(err);

//     res.status(500).send({ statusCode: 500, message: 'internal server error' });
//   }
// }
// /**
//  * Update Agent Activity
//  * @param req
//  * @param res
//  * @param done
//  */
// export async function getVoiceMailHandler(req: any, res: any, done: any) {
//   try {
//     const auth: any = req.headers;
//     const DaoRes: any = await getVoiceMailHandlerDao(auth.companyId);
//     if (DaoRes) {
//       res.status(200).send({ statusCode: 200, message: 'Success', result: DaoRes });
//     } else {
//       res.status(424).send({ statusCode: 424, message: 'failed' });
//     }
//   } catch (err) {
//     req.log.error(err);

//     res.status(500).send({ statusCode: 500, message: 'internal server error' });
//   }
// }
// /**
//  * Update Agent Activity
//  * @param req
//  * @param res
//  * @param done
//  */
// export async function getVoiceMailAgentHandler(req: any, res: any, done: any) {
//   try {
//     const body: any = req.body;
//     const DaoRes: any = await getVoiceMailAgentDao(body.agent);
//     if (DaoRes) {
//       res.status(200).send({ statusCode: 200, message: 'Success', result: DaoRes });
//     } else {
//       res.status(424).send({ statusCode: 424, message: 'failed' });
//     }
//   } catch (err) {
//     req.log.error(err);

//     res.status(500).send({ statusCode: 500, message: 'internal server error' });
//   }
// }
