// import { ActivityLogModel } from '../models/activity.models';
// import { sessionHistoryModel } from '../models/sessionHistoey';
// import { userDetialsModel } from '../models/userDetial.models';

// /**
//  *
//  * @param data
//  * @returns
//  */
// export const userUpdateDao = async (data: any) => {
//   try {
//     return new Promise(async (resolve, reject) => {
//       const agentRes = await userDetialsModel.findOne({ emailId: data.emailId });

//       if (agentRes) {
//         userDetialsModel.updateOne({ emailId: data.emailId }, { $set: data }).then(async (result) => {

//           resolve(result);
//         }).catch((err: any) => {
//           reject(err);
//         });
//       }
//       else userDetialsModel.create(data);
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
// export const getUserDetailsDao = async (data: any) => {
//   try {

//     return new Promise(async (resolve, reject) => {

//       userDetialsModel.find({
//         $or: [

//           {
//             emailId: data.emailId
//           },
//           {
//             ext: data.ext
//           }

//         ]
//       }).then((res: any) => {

//         resolve(res);
//       }).catch((err: any) => {
//         reject(err);

//       });
//     });
//   } catch (err: any) {
//     console.log(err);
//   }
// };

// /**
//  *
//  * @param data
//  * @returns
//  */
// export const getAllUserDetailsDao = async (data: any) => {

//   try {
//     return new Promise(async (resolve, reject) => {

//       userDetialsModel.find({ companyId: data.companyId }).then((res: any) => {

//         resolve(res);
//       }).catch((err: any) => {
//         reject(err);
//       });
//     });
//   } catch (err: any) {
//     console.log(err);
//   }
// };

// /**
//  *
//  * @param data
//  * @returns
//  */
// export const updateUserDetailsDao = async (data: any) => {
//   try {
//     return new Promise(async (resolve, reject) => {

//       userDetialsModel.updateOne({ ext: data.ext }, { $set: { state: data.state } }).then((res: any) => {
//         resolve(res);
//       }).catch((err: any) => {
//         reject(err);
//       });
//     });
//   } catch (err: any) {
//     console.log(err);
//   }
// };

// /**
//  *
//  * @param data
//  * @returns
//  */
// export const activityUpdateDao = async (data: any) => {

//   try {
//     return new Promise(async (resolve, reject) => {

//       ActivityLogModel.create(data).then((res: any) => {
//         resolve(res);
//       }).catch((err: any) => {
//         reject(err);
//       });
//     });
//   } catch (err: any) {
//     console.log(err);
//   }
// };

// /**
//  *
//  * @param data
//  * @returns
//  */
// export const getActivityDao = async (data: any) => {
//   try {
//     return new Promise(async (resolve, reject) => {

//       ActivityLogModel.find({ companyId: data }).then((res: any) => {

//         resolve(res);
//       }).catch((err: any) => {
//         reject(err);
//       });
//     });
//   } catch (err: any) {
//     console.log(err);
//   }
// };

// /**
//  *
//  * @param data
//  * @returns
//  */
// export const getVoiceCallActivityDao = async (data: any) => {
//   try {
//     return new Promise(async (resolve, reject) => {

//       ActivityLogModel.find({ companyId: data.companyId, channelId: data.channelId }).then((res: any) => {

//         resolve(res);
//       }).catch((err: any) => {
//         reject(err);
//       });
//     });
//   } catch (err: any) {
//     console.log(err);
//   }
// };

// /**
//  *
//  * @param data
//  * @returns
//  */
// export const getCallBackListDao = async (data: any) => {
//   try {
//     return new Promise(async (resolve, reject) => {
//       ActivityLogModel.find({
//         companyId: data.companyId,
//         channelId: data.channelId,
//         $or: [
//           {
//             callType: '3'
//           },
//           {
//             callType: '4'
//           },
//           {
//             callType: '5'
//           }
//         ]
//       }).then((res: any) => {

//         resolve(res);
//       }).catch((err: any) => {
//         reject(err);
//       });
//     });
//   } catch (err: any) {
//     console.log(err);
//   }
// };

// /**
//  *
//  * @param data
//  * @returns
//  */
// export const getCallBackextListDao = async (data: any) => {
//   try {
//     return new Promise(async (resolve, reject) => {
//       ActivityLogModel.find({
//         agent: data, $or: [
//           {
//             callType: '3'
//           },
//           {
//             callType: '4'
//           },
//           {
//             callType: '5'
//           }
//         ]
//       }).then((res: any) => {

//         resolve(res);
//       }).catch((err: any) => {
//         reject(err);
//       });
//     });
//   } catch (err: any) {
//     console.log(err);
//   }
// };

// /**
//  *
//  * @param data
//  * @returns
//  */
// export const getAgentActivityListDao = async (data: any) => {
//   try {

//     return new Promise(async (resolve, reject) => {

//       ActivityLogModel.find({
//         $and: [

//           {
//             channelId: data.channelId
//           },
//           {
//             agent: data.agent
//           }

//         ]
//       }).then((res: any) => {

//         resolve(res);
//       }).catch((err: any) => {
//         reject(err);

//       });
//     });
//   } catch (err: any) {
//     console.log(err);
//   }
// };
