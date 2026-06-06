export interface participant {
  video: boolean;
  mute: boolean;
  pined: boolean;
  name: string;
  isHost: boolean;
  isAlternateHost: boolean;
  isRecordingAllowed: boolean;
  moveToWaitingRoom: boolean;
  sipId: string;
  status: string; //"joined" / "inwaitingroom" / "removed";
  hand: boolean;
  ext: any;
  profileImg?: any;
  removeFromMeeting: boolean;
  isGuest: boolean;
  roleType: Number; //on enabling this, the participant is removed
  email: string;
  isHostAllowed: boolean;
  waiting_retrycount: Number;
  recordingStatus: string;
  closedCaption: boolean;
  isScreenshare: boolean;
  isWhiteboard: boolean;
  isMirror: boolean;
  isVoiceengineStart: boolean;
  joinedTime?: any;
  isReconnectstatus?: boolean;
  recordFilePath: any;
  reactions:string;
  non_verbal:string;
  cc_lang:string;
  speaker_lang:string;
  //From the meeting and he can’t rejoin
}
export interface generalsetting {
  isChat: boolean;
  isScreenShare: boolean;
  renameThemselves: boolean;
  lockMeeting: boolean;
  is_participant_unmute_themselves: boolean;
  isWaitingroom: boolean;
  is_participant_mute_entry: boolean
  shareOptions: {
    howManyPart: string,
    whoShare: string,
    startShare: string,
  }
};
export interface participants {
  [propName: string]: participant;
}

export interface joinedparticipants {
  [propName: string]: participant;
}


export interface deviceinfo {
  [propName: string]: participant;
}


export interface setting {

  lockMeeting: boolean; //on enabling this No user will be allowed to join the  ongoing meeting
  waitingRoom: boolean;
  isHostjoined: boolean;
  participants: participants;
  joinedParticipants: Array<any>;
  conversations: Array<any>;
  meetingId: String;
  meetingPwd: String;
  hostDisconnect: boolean;
  chatSetting: string;
  generalSetting: generalsetting;
  activeConn: Array<any>;
  waitingroomConn: Array<any>;
  joinedConn: Array<any>
  alternateHostDetails: any
  hostDetails: any;
  devicesInfo: deviceinfo;
  isHostAudioMute: boolean;
  meetingName: String;
  isWhiteBoard: boolean;
  isVoiceEngineStarted: boolean;
  isScreenshareStarted: boolean;
  isWhiteboardStarted: boolean;
  isParticipantVideo: boolean;
  isHostMyVideo: boolean;
  globalKeywords: Array<any>;
  joinedParticipantlist: Array<any>;
  isParticipantjoinbefhost: boolean;
  startTime: any;
  endTime: any;
  duration: any;
  inviteUrl: any;
  restrictScreenShare: boolean;
  authenticationJoin: string;
  requireAuthentication: boolean;
  sipIdList: Array<any>;
  is_participant_invite_guest: boolean
  is_host_myaudio: boolean
  status: any
  plan_type:number
  user_plan_type:string
  meetStartTime:any;
  agentDetails:any;
  domain_id:any;
  queueId:string;
  guestDetails:any;
}

