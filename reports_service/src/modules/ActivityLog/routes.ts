// /**
//  * @createdBy <team@vectone.com>
//  * @createdOn
//  */

// import { ACTIVITY } from '../../helpers/constants';
// import {
//   getAgentActivityList,
//   getAgentDetailsListHandler,
//   getAllActivity, getAllUserDetails, getCallBackextList, getCallBackList, getUserDetails, getVoiceCallActivity, updateActivity, updateUserDetails, userHandler
// } from './handler';
// import { activityUpdateSchema, getAgentActivityListSchema, getAllActivitySchema, getCallActivitySchema, getCallBackExtListSchema, getCallBackListSchema, getUserDetailsSchema, userinsertSchema } from './schema';

// export default function activityHandler(server: any, options: any, next: any) {

//   // USER UPDATE
//   server.post(
//     ACTIVITY.USER_UPDATE,
//     {
//       preValidation: [server.validateSession],
//       schema: {
//         summary: 'User Creation or Updation',
//         description: 'Create / Update user',
//         tags: ['activity'],
//         body: userinsertSchema.body,
//         response: userinsertSchema.response,
//       },
//     },
//     userHandler
//   );
//   // GET USER LIST BY EXT OR EMAIL
//   server.post(
//     ACTIVITY.GET_USER_DETAILS,
//     {
//       preValidation: [server.validateSession],
//       schema: {
//         summary: 'Get user details',
//         description: 'Get user details',
//         tags: ['activity'],
//         response: getUserDetailsSchema.response,
//       },
//     },
//     getUserDetails
//   );
//   // GET ALL USER DETAILS
//   server.get(
//     ACTIVITY.GET_ALL_USER_DETAILS,
//     {
//       preValidation: [server.validateSession],
//       schema: {
//         summary: 'Get user details',
//         description: 'Get user details',
//         tags: ['activity'],
//         response: getUserDetailsSchema.response,
//       },
//     },
//     getAllUserDetails
//   );
//   // GET ALL USER DETAILS
//   server.post(
//     ACTIVITY.UPDATE_USER_DETAILS,
//     {
//       preValidation: [server.validateSession],
//       schema: {
//         summary: 'update user details',
//         description: 'update user details',
//         tags: ['activity'],
//         response: getUserDetailsSchema.response,
//       },
//     },
//     updateUserDetails
//   );

//   // USER UPDATE
//   server.post(
//     ACTIVITY.UPDATE_ACTIVITY,
//     {
//       preValidation: [server.validateSession],
//       schema: {
//         summary: 'Update agent activity',
//         description: 'update agent user',
//         tags: ['activity'],
//         body: activityUpdateSchema.body,
//         response: activityUpdateSchema.response,
//       },
//     },
//     updateActivity
//   );

//   // GET ALL ACTIVITY
//   server.get(
//     ACTIVITY.GET_ALL_ACTIVITY,
//     {
//       preValidation: [server.validateSession],
//       schema: {
//         summary: 'Get all channel activity',
//         description: 'Get all channel activity',
//         tags: ['activity'],
//         response: getAllActivitySchema.response,
//       },
//     },
//     getAllActivity
//   );

//   // GET ALL CHANNEL ACTIVITY
//   server.post(
//     ACTIVITY.GET_CALL_ACTIVITY,
//     {
//       preValidation: [server.validateSession],
//       schema: {
//         summary: 'Get all channel activity',
//         description: 'Get all channel activity',
//         tags: ['activity'],
//         response: getCallActivitySchema.response,
//       },
//     },
//     getVoiceCallActivity
//   );
//   server.post(
//     ACTIVITY.GET_CALLBACK_LIST,
//     {
//       preValidation: [server.validateSession],
//       schema: {
//         summary: 'Get callBack list',
//         description: 'Get callBack list api',
//         tags: ['activity'],
//         response: getCallBackListSchema.response,
//       },
//     },
//     getCallBackList
//   );
//   server.post(
//     ACTIVITY.GET_CALLBACK_EXT_LIST,
//     {
//       preValidation: [server.validateSession],
//       schema: {
//         summary: 'Get callBack ext list',
//         description: 'Get callBack list api',
//         tags: ['activity'],
//         response: getCallBackExtListSchema.response,
//       },
//     },
//     getCallBackextList
//   );
//   /**
//    *
//    */
//   server.post(
//     ACTIVITY.GET_AGENT_ACTIVITY_LIST,
//     {
//       preValidation: [server.validateSession],
//       schema: {
//         summary: 'Get agent activity list',
//         description: 'Get agent activity list api',
//         tags: ['activity'],
//         response: getAgentActivityListSchema.response,
//       },
//     },
//     getAgentActivityList
//   );

//   /**
//    * get agent details page
//    */
//   server.get(
//     `${ACTIVITY.GET_AGENT_DETAILS_LIST}/:ext/:offset/:limit`,
//     {
//       preValidation: [server.validateSession],
//       schema: {
//         summary: 'Get agent activity list',
//         description: 'Get agent activity list api',
//         tags: ['activity'],
//       },
//     },
//     getAgentDetailsListHandler
//   );
//   next();
// }
