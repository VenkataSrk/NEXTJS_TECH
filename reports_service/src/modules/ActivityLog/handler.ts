// /**
//  * @createdBy <team@vectone.com>
//  * @createdOn
//  */

// // import {
// //   activityUpdateDao,
// //   getActivityDao,
// //   getAgentActivityListDao,
// //   getAllUserDetailsDao,
// //   getCallBackextListDao,
// //   getCallBackListDao,
// //   getUserDetailsDao,
// //   getVoiceCallActivityDao,
// //   updateUserDetailsDao,
// //   userUpdateDao,
// // } from '../../dao/activity.dao';

// import { RESPONSE } from '../../helpers/constants';

// /**
//  * User Status update
//  * @param req
//  * @param res
//  * @param done
//  */
// export async function userHandler(req: any, res: any, done: any) {
//   try {
//     // const auth: any = req.headers;
//     const data: any = req.body;
//     const userData: any = {
//       name: data.profileName,
//       emailId: data.username,
//       companyId: data.companyId,
//       domainId: data.domainId,
//       ext: data.ext,
//       lastModifiedDate: Date.now(),
//       roleId: data.roleId,
//       currentStatus: data.currentStatus,
//       createDate: Date.now(),
//       state: data.state,
//       skillSet: data.skillSet,
//       channelSupport: data.channelSupport,
//       stateTimer: data.stateTimer,
//       callType: data.callType,
//       queue: data.queue,
//       routingProfile: data.routingProfile,
//     };

//     const userRes: any = await userUpdateDao(userData);

//     if (userRes) {
//       res
//         .status(200)
//         .send({ statusCode: 200, message: 'Updated  successfully' });
//     } else {
//       res.status(424).send({ statusCode: 424, message: 'failed' });
//     }
//   } catch (err) {
//     req.log.error(err);
//     res.status(500).send({ statusCode: 500, message: 'internal server error' });
//   }
// }
// /**
//  * get user details
//  * @param req
//  * @param res
//  * @param done
//  */
// export async function getUserDetails(req: any, res: any, done: any) {
//   try {
//     const data: any = req.body;
//     const detials: any = {
//       emailId: data.emailId,
//       ext: data.ext,
//     };
//     const DaoRes: any = await getUserDetailsDao(detials);
//     if (DaoRes) {
//       res
//         .status(200)
//         .send({ statusCode: 200, message: 'Success', result: DaoRes });
//     } else {
//       res.status(424).send({ statusCode: 424, message: 'failed' });
//     }
//   } catch (err) {
//     req.log.error(err);
//     res.status(500).send({ statusCode: 500, message: 'internal server error' });
//   }
// }
// /**
//  * get user details
//  * @param req
//  * @param res
//  * @param done
//  */
// export async function getAllUserDetails(req: any, res: any, done: any) {
//   try {
//     const auth: any = req.headers;
//     const detials: any = {
//       companyId: auth.companyId,
//     };
//     const DaoRes: any = await getAllUserDetailsDao(detials);
//     if (DaoRes) {
//       res
//         .status(200)
//         .send({ statusCode: 200, message: 'Success', result: DaoRes });
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
// export async function updateUserDetails(req: any, res: any, done: any) {
//   try {
//     const auth: any = req.headers;
//     const data: any = req.body;

//     data.ext = auth.ext.toString();
//     data.state = data.state;
//     const DaoRes: any = await updateUserDetailsDao(data);
//     if (DaoRes) {

//       res
//         .status(200)
//         .send({ statusCode: 200, message: 'Updated  successfully' });
//     } else {
//       res.status(424).send({ statusCode: 424, message: 'Update failed' });
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
// export async function updateActivity(req: any, res: any, done: any) {
//   try {
//     const auth: any = req.headers;
//     const data: any = req.body;

//     (data.companyId = auth.companyId), (data.domainId = auth.domainId);

//     const DaoRes: any = await activityUpdateDao(data);

//     if (DaoRes) {
//       res
//         .status(200)
//         .send({ statusCode: 200, message: 'Updated  successfully' });
//     } else {
//       res.status(424).send({ statusCode: 424, message: 'Update failed' });
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
// export async function getAllActivity(req: any, res: any, done: any) {
//   try {
//     const auth: any = req.headers;
//     const data: any = req.body;

//     const DaoRes: any = await getActivityDao(auth.companyId);
//     if (DaoRes) {
//       res
//         .status(200)
//         .send({ statusCode: 200, message: 'Success', result: DaoRes });
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
// export async function getVoiceCallActivity(req: any, res: any, done: any) {
//   try {
//     const auth: any = req.headers;
//     const data: any = req.body;

//     const detials: any = {
//       companyId: auth.companyId,
//       channelId: data.channelId,
//     };
//     const DaoRes: any = await getVoiceCallActivityDao(detials);

//     if (DaoRes) {
//       res
//         .status(200)
//         .send({ statusCode: 200, message: 'Success', result: DaoRes });
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
// export async function getCallBackList(req: any, res: any, done: any) {
//   try {
//     const auth: any = req.headers;
//     const data: any = req.body;

//     const detials: any = {
//       companyId: auth.companyId,
//       channelId: data.channelId,
//     };
//     const DaoRes: any = await getCallBackListDao(detials);
//     if (DaoRes) {
//       res
//         .status(200)
//         .send({ statusCode: 200, message: 'Success', result: DaoRes });
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
// export async function getCallBackextList(req: any, res: any, done: any) {
//   try {
//     const auth: any = req.headers;
//     const data: any = req.body;
//     const DaoRes: any = await getCallBackextListDao(data.agent);
//     if (DaoRes) {
//       res
//         .status(200)
//         .send({ statusCode: 200, message: 'Success', result: DaoRes });
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
// export async function getAgentActivityList(req: any, res: any, done: any) {
//   try {
//     const data: any = req.body;
//     const dataE: any = {
//       agent: data.agent,
//       channelId: data.channelId,
//     };
//     const DaoRes: any = await getAgentActivityListDao(dataE);
//     if (DaoRes) {
//       res
//         .status(200)
//         .send({ statusCode: 200, message: 'Success', result: DaoRes });
//     } else {
//       res.status(424).send({ statusCode: 424, message: 'failed' });
//     }
//   } catch (err) {
//     req.log.error(err);
//     res.status(500).send({ statusCode: 500, message: 'internal server error' });
//   }
// }

// /**
//  * get Agent details
//  * @param req
//  * @param res
//  * @param done
//  */
// export async function getAgentDetailsListHandler(
//   req: any,
//   res: any,
//   done: any
// ) {
//   try {
//     const auth: any = req.headers;
//     const dataq: any = {
//       ext: req.params.ext,
//       offset: req.params.offset,
//       limit: req.params.limit,
//     };
//     const data: any = {
//       domainId: auth.domainId,
//     };
//     if (dataq.ext !== '') {
//       const detailsRes: any = await getAgentDetailsList(data, dataq);
//       const detailsCount: any = await getAgentDetailsList(data, dataq);
//       if (Array.isArray(detailsRes) && detailsRes.length > 0) {
//         res.status(200).send({
//           statusCode: 200,
//           message: RESPONSE.success_message,
//           getAgentDetailsCountList: detailsCount,
//           getAgentDetails: detailsRes,
//         });
//       } else {
//         res.status(200).send({
//           statusCode: 404,
//           message: RESPONSE.not_found,
//           getInitiateRes: [],
//         });
//       }
//     } else {
//       res
//         .status(200)
//         .send({ statusCode: 424, message: 'ext no should not be empty' });
//     }
//   } catch (err) {
//     req.log.error(err);
//     res.status(500).send({ statusCode: 500, message: 'internal server error' });
//   }
// }
// function getAgentDetailsList(data: any, dataq: any): any {
//   throw new Error('Function not implemented.');
// }
