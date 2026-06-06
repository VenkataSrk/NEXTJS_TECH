// /**
//  * @createdBy <team@vectone.com>
//  * @createdOn
//  */

// import { Document, Schema } from 'mongoose';
// import { ccaas } from '../plugins/db';

// export interface IUserschema extends Document {
//   name: string;
//   roleId:string;
//   emailId:string;
//   ext : number;
//   companyId : number;
//   domainId : number;
//   skillSet : string;
//   createDate: number;
//   lastModifiedDate : number;
//   state : string;
//   channelSupport:string;
//   stateTimer:string;
//   callType:string;
//   queue:string;
// }

// const userDetials : Schema = new Schema({

//   name: { type: String , required: true },
//   emailId:{ type: String , required: true },
//   ext : { type: String , required: true },
//   roleId : { type: String , required: true },
//   companyId : { type: Number },
//   createDate : { type: String,  default: Date.now },
//   lastModifiedDate : { type: String },
//   domainId : { type: Number, required: true },
//   state : { type: String, required: true },
//   skillSet : { type: String },
//   channelSupport:{ type: String },
//   stateTimer:{ type: String },
//   callType:{ type: String },
//   queue:{ type:String },
// });

//    // Export the model and return your userDetials interface
// export const userDetialsModel = ccaas.model('userDetials', userDetials, 'userDetials');
