export interface IRegisterDetails {
  ext: number;
  domainId: number;
  domainName: string;
  callerId: string;
  deviceToken: string;
  deviceType: string;
  authToken: string;
}

export interface INcall {
  status: number;
  agentId:number;
  domainId:number;
}
