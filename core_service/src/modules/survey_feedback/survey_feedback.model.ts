// export interface ISingleChoice {
//   surveryFeedbackId: number;
//   flowId: IFlowId;
//   question: string;
//   choices: IChoice[];
//   createdAt: Date;
//   updatedAt: Date;
// }

// export interface ImultipleChoice {
//   surveryFeedbackId: number;
//   flowId: IFlowId;
//   question: string[];
//   choices: string[];
//   mapflow: string;
//   createdAt: Date;
//   updatedAt: Date;
// }

// export interface IRatingScale {
//   surveryFeedbackId: number;
//   flowId: IFlowId;
//   question: string[];
//   rating: IRating;
//   mapflow: string;
//   createdAt: Date;
//   updatedAt: Date;
// }
// export interface ITypeYourAnswer {
//   surveryFeedbackId: number;
//   flowId: IFlowId;
//   question: string[];
//   createdAt: Date;
//   updatedAt: Date;
// }

// export interface IRating {
//   ratingType: string;
//   ratingObj: string[];
// }

// export class IChoice {
//   public choice: string;
//   public mapflow?: string;
// }

// export interface IsurverFeedback {
//   surveyFeedbackId:number;
//   type: surveyFeedback;
// }
// export class surveyFeedback {

//   private singleChoice: ISingleChoice;
//   private multipleChoice:ImultipleChoice;
//   private ratingScale:IRatingScale;
//   private typeYourAnswer:ITypeYourAnswer;
//   constructor(typeId: number) {
//       if (typeId == 1) {
//           this.singleChoice;
//         }
//     }
// }

// export class IFlowId {
//   public singleChoice: number = 1;
//   public multipleChoice: number = 2;
//   public ratingScale: number = 3;
//   public typeYourAnswer: number = 4;
// }
