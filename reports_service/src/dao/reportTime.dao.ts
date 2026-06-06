// import { reportRollRes } from '../models/reportCallRollupData';
// import { reportTimestampRes } from '../models/reportTimestamp';
// import { sessionHistoryModel } from '../models/sessionHistoey';

/**
 *
 * @param data
 * @returns
 */
// export const insertReportVoiceCount = async () => {
//   try {
//     return new Promise(async (resolve, reject) => {
//       const result = await reportTimestampRes.find({ domainId: 35789 }).sort({ _id: -1 });
//       const toDate = Math.floor(Date.now());
//       await sessionHistoryModel.aggregate([
//         {
//           $match:
//           /**
//            * query: The query in MQL.
//            */
//           {
//             domainId: 35789,
//             sessionStartTime: {
//               $gte: result[0].lastUpdateTime,
//               $lte: toDate,
//             },
//             channelType: 'Call',
//           },
//         },
//         {
//           $facet: {
//             disposition: [
//               {
//                 $group: {
//                   _id: {
//                     disposition: '$disposition',
//                     queueName: '$queueName',
//                     qid: '$qid',
//                   },
//                   dispositionCount: {
//                     $sum: 1,
//                   },
//                 },
//               },
//             ],
//             calls: [
//               {
//                 $group: {
//                   _id: {
//                     callType: '$callType',
//                     qid: '$qid',
//                   },
//                   callCount: {
//                     $sum: 1,
//                   },
//                 },
//               },
//             ],
//             recivedCalls: [
//               {
//                 $group: {
//                   _id: '$qid',
//                   sessionStartTime: {
//                     $sum: '$sessionStartTime',
//                   },
//                   RecivedCallsCount: {
//                     $sum: 1,
//                   },
//                   totalWaitTimeAnsCall: {
//                     $sum: '$waitDuration',
//                   },
//                   totalTalkTime: {
//                     $sum: '$agentDuration',
//                   },
//                   totalHoldTime: {
//                     $sum: '$agentHoldDuration',
//                   },
//                   totalWrapTime: {
//                     $sum: '$wrapTime',
//                   },
//                   queueName: {
//                     $first: '$queueName',
//                   },
//                   answeredCallcount: {
//                     $sum: {
//                       $cond: [
//                         {
//                           $gt: ['$agentConnectTime', 0],
//                         },
//                         1,
//                         0,
//                       ],
//                     },
//                   },
//                   abandonedCallCount: {
//                     $sum: {
//                       $cond: {
//                         if: {
//                           $in: [
//                             '$disposition',
//                             ['missed', 'abandoned'],
//                           ],
//                         },
//                         then: 1,
//                         else: 0,
//                       },
//                     },
//                   },
//                 },
//               },
//             ],
//             totalNumbarOfHold: [
//               {
//                 $group: {
//                   _id: '$qid',
//                   count: {
//                     $sum: {
//                       $cond: [
//                         {
//                           $gt: [
//                             '$agentHoldDuration',
//                             0,
//                           ],
//                         },
//                         1,
//                         0,
//                       ],
//                     },
//                   },
//                 },
//               },
//             ],
//           },
//         },
//         {
//           $addFields: {
//             domainId: 35789,
//             channelType: 'Call',
//           },
//         },
//       ]).then((list: any) => {
//         reportRollRes.create(list).then((res) => {
//           resolve(res);
//         });
//         reportTimestampRes.create({ domainId: 35789, lastUpdateTime: toDate }).then((res) => {
//           resolve(res);
//         })
//           .catch((err) => {
//             reject(err);
//           });
//       });
//     });
//   } catch (err) {
//     console.log(err);
//   }
// };

// /**
//  *
//  * @param data
//  * @returns
//  */
// export const getReportVoiceCount = async (data: any) => {
//   return new Promise(async (resolve, reject) => {
//     try {
//       await reportRollRes.aggregate([
//         {
//           $match: {
//             domainId: 35789,
//             lastUpdateDate: {
//               $gte: data.fromDate,
//               $lte: data.toDate,
//             },
//             channelType: 'Call',
//           },
//         },
//         {
//           $unwind: {
//             path: '$recivedCalls',
//           },
//         },
//         {
//           $facet: {
//             getVoiceMetrixReports: [
//               {
//                 $group: {
//                   _id: {
//                     queueName:
//                       '$recivedCalls.queueName',
//                   },
//                   recivedCallsCount: {
//                     $sum: '$recivedCalls.RecivedCallsCount',
//                   },
//                   answeredCallsCount: {
//                     $sum: '$recivedCalls.answeredCallcount',
//                   },
//                   abandonedCallCount: {
//                     $sum: '$recivedCalls.abandonedCallCount',
//                   },
//                 },
//               },
//             ],
//           },
//         },
//       ]).then((res) => {
//         resolve(res[0]);
//       })
//         .catch((err) => {
//           reject(err);
//         });
//     } catch (err) {
//       reject(err);
//     }
//   });
// };

// /**
//  *
//  * @param data
//  * @returns
//  */
// export const getQueueDispositionStataus = async (data: any) => {
//   return new Promise(async (resolve, reject) => {
//     try {
//       await reportRollRes.aggregate([
//         {
//           $match: {
//             domainId: 35789,
//             lastUpdateDate: {
//               $gte: data.fromDate,
//               $lte: data.toDate,
//             },
//             channelType: 'Call',
//           },
//         },
//         {
//           $unwind: '$disposition',
//         },
//         {
//           $group: {
//             _id: {
//               queueName: '$disposition._id.queueName',
//               qid: '$disposition._id.qid',
//               dispositionName:
//                 '$disposition._id.disposition',
//             },
//             dispositionCount: {
//               $sum: '$disposition.dispositionCount',
//             },
//           },
//         },
//         {
//           $group: {
//             _id: '$_id.queueName',
//             dispositions: {
//               $push: {
//                 dispositionName: '$_id.dispositionName',
//                 dispositionCount: '$dispositionCount',
//               },
//             },
//           },
//         },
//         {
//           $project: {
//             _id: 0,
//             queueName: '$_id',
//             dispositions: {
//               $map: {
//                 input: [
//                   'Resolved',
//                   'Unresolved',
//                   'Transferred',
//                   'Follow-up',
//                   'Closed',
//                 ],
//                 as: 'dispName',
//                 in: {
//                   disposition: '$$dispName',
//                   count: {
//                     $ifNull: [
//                       {
//                         $sum: {
//                           $map: {
//                             input: '$dispositions',
//                             as: 'disp',
//                             in: {
//                               $cond: [
//                                 {
//                                   $eq: [
//                                     '$$disp.dispositionName',
//                                     '$$dispName',
//                                   ],
//                                 },
//                                 '$$disp.dispositionCount',
//                                 0,
//                               ],
//                             },
//                           },
//                         },
//                       },
//                       0,
//                     ],
//                   },
//                 },
//               },
//             },
//           },
//         },
//       ]).then((res) => {
//         resolve(res);
//       })
//         .catch((err) => {
//           reject(err);
//         });
//     } catch (err) {
//       reject(err);
//     }
//   });
// };

// /**
//  *
//  * @param data
//  * @returns
//  */
// export const getIntervalReport = async (data) => {
//   return new Promise(async (resolve, reject) => {
//     try {
//       await reportRollRes.aggregate([
//         {
//           $match:
//           /**
//            * query: The query in MQL.
//            */
//           {
//             domainId: 35789,
//             lastUpdateDate: {
//               $gte: data.fromDate,
//               $lte: data.toDate,
//             },
//             channelType: 'Call',
//           },
//         },
//         {
//           $unwind:
//           {
//             path: '$recivedCalls',
//           },
//         },
//         {
//           $group:
//           {
//             _id: {
//               hour: {
//                 $hour: {
//                   $toDate: '$lastUpdateDate',
//                 },
//               },
//             },
//             recivedCallsCount: {
//               $sum: '$recivedCalls.RecivedCallsCount',
//             },
//             answeredCallcount: {
//               $sum: '$recivedCalls.answeredCallcount',
//             },
//             abandonedCallCount: {
//               $sum: '$recivedCalls.abandonedCallCount',
//             },
//           },
//         },
//       ]).then((res) => {
//         resolve(res);
//       })
//         .catch((err) => {
//           reject(err);
//         });
//     } catch (err) {
//       reject(err);
//     }
//   });
// };

// /**
//  *
//  * @param data
//  * @returns
//  */
// export const getAbandonedCallProfile = async (data: any) => {
//   return new Promise(async (resolve, reject) => {
//     try {
//       await reportRollRes.aggregate([
//         {
//           $match: {
//             domainId: 35789,
//             lastUpdateDate: {
//               $gte: data.fromDate,
//               $lte: data.toDate,
//             },
//             channelType: 'Call',
//           },
//         },
//         {
//           $unwind:

//           {
//             path: '$recivedCalls',
//           },
//         },
//         {
//           $group:
//           {
//             _id: {
//               queueName: '$recivedCalls.queueName',
//               qid: '$recivedCalls._id',
//             },
//             abandonedCallCount: {
//               $sum: '$recivedCalls.abandonedCallCount',
//             },
//           },
//         },
//       ]).then((res) => {
//         resolve(res);
//       })
//         .catch((err) => {
//           reject(err);
//         });
//     } catch (err) {
//       reject(err);
//     }
//   });
// };
