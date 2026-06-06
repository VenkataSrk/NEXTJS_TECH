// /**
//  * @createdBy <team@vectone.com>
//  * @createdOn
//  */

// import { Document, Schema } from 'mongoose';
// import { ccaas } from '../plugins/db';

// export interface IReportTimeSchema extends Document {
//   domainId: number;
//   lastUpdateTime: number;
// }

// const IreportTimeStamp = new Schema({
//   domainId: { type: Number },
//   lastUpdateTime: { type: Number },
// });

// // Export the model and return your chatHistory interface
// export const reportTimestampRes = ccaas.model<IReportTimeSchema>(
//   'reportTimestamp',
//   IreportTimeStamp,
//   'reportTimestamp'
// );
