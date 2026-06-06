export interface ICreateBroadCast {
  domainId: number;
  bcGroupName: string;
  bcMembers: any; // or string[] if applicable
  createBy: number;
  bcHistoryId:number;
}

export interface IGetBroadCastGroup {
  domainId: number;
  bcGroupName: any;
  bcMembers:any;
}

export interface IDeleteAgentDtl {
  domainId: number;
  bcGroupName: string;
  bcMembers: any;
}

export interface IGetAllGroup {
  domainId: number;
  userId:number;
}

export interface IInsertBoardCastMsg {
  bcHistoryId: number;
  domainId:number;
  message: any;
  AgentId: string;
  expiryDate: any;
  sendTimestamp:number;
  // agentExts:string;
}

export interface IDeleteBoardCastMsg {
  bcDurationHistoryId: number;
  bcHistoryId: number;
  domainId: number;
}

export interface IGetBoardCastMsg {
  bcHistoryId: number;
  domainId:number;
  AgentId: string;
}
