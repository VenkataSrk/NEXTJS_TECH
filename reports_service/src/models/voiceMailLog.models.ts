// import { Schema } from 'mongoose';
// import { ccaas } from '../plugins/db';

// export interface IVoiceMailLogSchema extends Document {
//   uuid: string;
//   filePath: string;
//   readFlag: string;
//   createdDAte: string;
//   agent: string;
//   companyId: string;
//   domainId: string;
//   fileUrl: string;
//   queue: string;
//   customer: string;
//   status: string;
//   transcription: string;
// }

// const voiceMailLog: Schema = new Schema({
//   uuid: { type: String },
//   filePath: { type: String },
//   readFlag: { type: String },
//   createdDAte: { type: String },
//   agent: { type: String },
//   companyId: { type: Number },
//   domainId: { type: Number },
//   fileUrl: { type: String },
//   queue: { type: String },
//   customer: { type: String },
//   status: { type: String },
//   transcription: { type: String },

// });

// // Export the model and return your userDetials interface
// export const VoiceMailLogSchema = ccaas.model('voiceMailLog', voiceMailLog, 'voiceMailLog');
