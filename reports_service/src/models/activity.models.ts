// /**
//  * @createdBy <team@vectone.com>
//  * @createdOn
//  */

// import { Document, Schema } from 'mongoose';
// import { ccaas } from '../plugins/db';

// export interface IActivityLogSchema extends Document {
//   calledDate: string;
//   customer: number;
//   queueID: string;
//   queueName: string;
//   callType: string;
//   talkTime: string;
//   wrapTime: string;
//   outcome: string;
//   recordingUrl: string;
//   recordingTranscript: string;
//   agent: number;
//   routingprofileName: string;
//   routingProfileId: number;
//   callState: number;
//   channelSupportId: number;
//   channelSupportName: string;
//   stateTimer: number;
//   disposition: string;
//   channelId: number;
//   ChannelName: string;
//   callDuration: number;
//   assigned: string;
//   assignedTo: string;
//   queryOutcome: string;
//   customerFeedBack: string;
//   botFeedback: string;
//   companyId: number;
//   domainId: number;
// }

// const activityLog: Schema = new Schema({
//   calledDate: { type: String },
//   customer: { type: String },
//   queueID: { type: String },
//   queueName: { type: String },
//   callType: { type: String },
//   talkTime: { type: String },
//   wrapTime: { type: String },
//   outcome: { type: String },
//   recordingUrl: { type: String },
//   recordingTranscript: { type: String },
//   agent: { type: String },
//   routingprofileName: { type: String },
//   routingProfileId: { type: String },
//   callState: { type: Number },
//   channelSupportId: { type: String },
//   channelSupportName: { type: String },
//   assigned: { type: String },
//   assignedTo: { type: String },
//   stateTimer: { type: String },
//   disposition: { type: String },
//   channelId: { type: String },
//   ChannelName: { type: String },
//   duration: { type: String },
//   queryOutcome: { type: String },
//   customerFeedBack: { type: String },
//   botFeedback: { type: String },
//   companyId: { type: Number },
//   domainId: { type: Number },
// });

// // Export the model and return your userDetials interface
// export const ActivityLogModel = ccaas.model('activityLog', activityLog, 'activityLog');
