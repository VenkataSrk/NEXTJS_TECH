export interface ICreateinternalChat {
  groupName: string;
  historyId: number;
  channelType: number;
  agentExt: any[];
  domainId: number;
  agentUserId: string;
  queueId: number;
  customerId: number;
  hostUserId: number;
  internalChatGroupId: number;
}

export interface IGetinternalChat {
  hostUserId: number;
  domainId: number;
}

export interface IGetparticularInternalChat {
  hostUserId: number;
  domainId: number;
  internalChatGroupId: number;
}

export interface IEdit {
  agentId: number;
  domainId: number;
  internalChatId: number;
  msg: object;
  agentExt: any[];
}

export interface IDelete {
  agentId: number;
  domainId: number;
  internalChatId: number;
  agentExt: any[];
}

export interface IReadInternalChat {
  agentId: number;
  domainId: number;
  internalChatGroupId: string;
  agentExt: any[];
  msgStatus: number;
}

export interface IInternalChatByHistoryId {
  agentId: number;
  domainId: number;
  historyId: number;
  channelType: number;
}
