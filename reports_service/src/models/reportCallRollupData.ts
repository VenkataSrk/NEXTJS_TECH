// /**
//  * @createdBy <team@vectone.com>
//  * @createdOn
//  */

// import { Document, Schema } from 'mongoose';
// import { ccaas } from '../plugins/db';

// export interface IReportRollSchema extends Document {
//   disposition: any;
//   calls: any;
//   recivedCalls: any;
//   answeredCall: any;
//   abandonedCalls: any;
//   totalNumbarOfHold: any;
//   domainId: number;
//   channelType: string;
//   lastUpdateDate: number;
// }

// const reportRollUpDatas = new Schema({
//   disposition: { type: Array },
//   calls: { type: Array },
//   recivedCalls: { type: Object },
//   answeredCall: { type: Object },
//   abandonedCalls: { type: Object },
//   totalNumbarOfHold: { type: Array },
//   domainId: { type: Number },
//   channelType: { type: String },
//   lastUpdateDate: { type: Number, default: Date.now }
// });

// // Export the model and return your chatHistory interface
// export const reportRollRes = ccaas.model<IReportRollSchema>(
//     'reportRollUpData',
//     reportRollUpDatas,
//     'reportRollUpData'
// );
